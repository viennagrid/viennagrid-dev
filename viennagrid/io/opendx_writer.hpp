#ifndef VIENNAGRID_IO_OPENDX_WRITER_GUARD
#define VIENNAGRID_IO_OPENDX_WRITER_GUARD

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <fstream>
#include <iostream>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/data_accessor.hpp"

namespace viennagrid
{
  namespace io
  {
  
    /////////////////// OpenDX export ////////////////////////////
    
    
    // Fix for data-explorer-bug: if floating-values occur, no integers (i.e. only zeros after decimal point) allowed
    template <typename FloatingPointType>
    FloatingPointType DXfixer(FloatingPointType value)
    {
      if (value * 10000 == static_cast<long>(value * 10000))
        return value + 0.00001;
    
      return value;
    }

    template <int DIM>
    struct DXHelper {};

    template <>
    struct DXHelper<1>
    {
      static std::string getAttributes()
      { return "attribute \"element type\" string \"line\" "; }
    };
    
    template <>
    struct DXHelper<2>
    {
      static std::string getAttributes()
      { return "attribute \"element type\" string \"triangles\" "; }
    };
    
    template <>
    struct DXHelper<3>
    {
      static std::string getAttributes()
      { return "attribute \"element type\" string \"tetrahedra\" "; }
    };
    
    template <typename DomainType>
    class opendx_writer
    {
        typedef typename DomainType::config_type                         DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type               CoordType;
        typedef typename DomainConfiguration::coordinate_system_tag      CoordinateSystemTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename result_of::point<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;

        typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type   VertexRange;
        typedef typename viennagrid::result_of::iterator<VertexRange>::type              VertexIterator;
        
        typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                        CellIterator;
        
        typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type      VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
        
        
      public:
        int operator()(DomainType const & domain, std::string const & filename)
        {
          typedef DXHelper<CoordinateSystemTag::dim>  DXHelper;
        
          std::ofstream writer(filename.c_str());
          if (!writer.is_open())
          {
            throw cannot_open_file_exception(filename);
            return EXIT_FAILURE;
          }
        
          std::size_t pointnum = viennagrid::ncells<0>(domain).size();
        
          writer << "object \"points\" class array type float rank 1 shape " << CoordinateSystemTag::dim << " items ";
          writer << pointnum << " data follows" << std::endl;
        
          //Nodes:
          VertexRange vertices = viennagrid::ncells<0>(domain);
          for (VertexIterator vit = vertices.begin();
              vit != vertices.end();
              ++vit)
          {
            PointWriter<CoordinateSystemTag::dim>::write(writer, vit->point());
            writer << std::endl;
          }
          writer << std::endl;

          //Cells:
          std::size_t cellnum = viennagrid::ncells<CellTag::dim>(domain).size();
          writer << "object \"grid_Line_One\" class array type int rank 1 shape " << (CoordinateSystemTag::dim + 1) << " items " << cellnum << " data follows" << std::endl;

          CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
            VertexOnCellRange vertices_for_cell = viennagrid::ncells<0>(*cit);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
              VertexType const & vertex = *vocit;
              writer << vertex.id() << " ";
            }
            writer << std::endl;
          }
        
          writer << DXHelper::getAttributes() << std::endl;
          writer << "attribute \"ref\" string \"positions\" " << std::endl;
          writer << std::endl;
        
          //set output-format:
          writer.flags(std::ios::fixed);
          writer.precision(5);
        
          //write quantity:
          if (vertex_data_scalar.size() > 0)
          {
            writer << "object \"VisData\" class array items " << pointnum << " data follows" << std::endl;
            //some quantity here
            
            for (VertexIterator vit = vertices.begin();
                vit != vertices.end();
                ++vit)
            {
              std::string value = vertex_data_scalar[0](*vit);
              writer << DXfixer(atof(value.c_str()));
              writer << std::endl;
            }

            writer << "attribute \"dep\" string \"positions\"" << std::endl;
          }
          else if (cell_data_scalar.size() > 0)
          {
            writer << "object \"VisData\" class array items " << cellnum << " data follows" << std::endl;

            //some quantity here
            for (CellIterator cit = cells.begin();
                cit != cells.end();
                ++cit)
            {
              std::string value = cell_data_scalar[0](*cit);
              writer << DXfixer(atof(value.c_str()));
              writer << std::endl;
            }
            writer << "attribute \"dep\" string \"connections\"" << std::endl;
          }
        
          writer << std::endl;
          writer << "object \"AttPotential\" class field " << std::endl;
          writer << "component \"data\" \"VisData\" " << std::endl;
          writer << "component \"positions\" \"points\"" << std::endl;
          writer << "component \"connections\" \"grid_Line_One\"" << std::endl;
        
          return EXIT_SUCCESS;
          
        } // operator()

        template <typename T>
        void add_scalar_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          if (vertex_data_scalar.size() > 0)
            std::cout << "* ViennaGrid: Warning: OpenDX vertex data " << name 
                      << " will be ignored, because other data is already available!" << std::endl;
          vertex_data_scalar.push_back(wrapper);
        }
        
        template <typename T>
        void add_scalar_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          
          if (cell_data_scalar.size() > 0)
            std::cout << "* ViennaGrid: Warning: OpenDX cell data " << name 
                      << " will be ignored, because other cell data is already available!" << std::endl;
                      
          if (vertex_data_scalar.size() > 0)
            std::cout << "* ViennaGrid: Warning: OpenDX cell data " << name 
                      << " will be ignored, because other vertex data is already available!" << std::endl;
                      
          cell_data_scalar.push_back(wrapper);
        }
        

      private:
        std::vector< data_accessor_wrapper<VertexType> >    vertex_data_scalar;
        std::vector< data_accessor_wrapper<CellType> >      cell_data_scalar;
        
    }; // opendx_writer


    template <typename KeyType, typename DataType, typename DomainType>
    opendx_writer<DomainType> & add_scalar_data_on_vertices(opendx_writer<DomainType> & writer,
                                                            KeyType const & key,
                                                            std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_scalar_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_scalar_data_on_vertices(wrapper, quantity_name);
      return writer;
    }
    
    template <typename KeyType, typename DataType, typename DomainType>
    opendx_writer<DomainType> & add_scalar_data_on_cells(opendx_writer<DomainType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_scalar_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_scalar_data_on_cells(wrapper, quantity_name);
      return writer;
    }
    

  } //namespace io
} //namespace viennagrid

#endif


