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

#ifndef VIENNAGRID_IO_OPENDX_WRITER_GUARD
#define VIENNAGRID_IO_OPENDX_WRITER_GUARD

#include <fstream>
#include <iostream>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"

namespace viennagrid
{
  namespace io
  {
  
    /////////////////// OpenDX export ////////////////////////////
    
    
    // Fix for data-explorer-bug: if floating-values occur, no integers (i.e. only zeros after decimal point) allowed
    double DXfixer(double value)
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
    
    struct opendx_writer
    {
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename, bool isVertexOriented = true)
      {
        typedef typename DomainType::config_type                     DomainConfiguration;
      
        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;
      
        typedef typename result_of::point<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;

        //typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;
      
        typedef typename viennagrid::result_of::ncell_range<DomainType, 0>::type   VertexRange;
        typedef typename viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
            
        typedef typename viennagrid::result_of::ncell_range<DomainType, 1>::type   EdgeRange;
        typedef typename viennagrid::result_of::iterator<EdgeRange>::type          EdgeIterator;

        typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level-1>::type   FacetRange;
        typedef typename viennagrid::result_of::iterator<FacetRange>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                  CellIterator;

        typedef typename viennagrid::result_of::ncell_range<CellType, 0>::type      VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type   VertexOnCellIterator;

        typedef DXHelper<DimensionTag::value>  DXHelper;
      
        std::ofstream writer(filename.c_str());
        if (!writer.is_open())
        {
          throw cannot_open_file_exception(filename);
          return EXIT_FAILURE;
        }
        
      
        long pointnum = domain.template size<CellTag::topology_level>();
      
        writer << "object \"points\" class array type float rank 1 shape " << DimensionTag::value << " items ";
        writer << pointnum << " data follows" << std::endl;
      
        //Nodes:
        VertexRange vertices = viennagrid::ncells<0>(domain);
        for (VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit)
        {
          PointWriter<DimensionTag::value>::write(writer, vit->getPoint());
          writer << std::endl;
        }
        writer << std::endl;

        //Cells:
        long cellnum = domain.template size<CellTag::topology_level>();
        writer << "object \"grid_Line_One\" class array type int rank 1 shape " << (DimensionTag::value + 1) << " items " << cellnum << " data follows" << std::endl;

        CellRange cells = viennagrid::ncells<CellTag::topology_level>(domain);
        for (CellIterator cit = cells.begin();
            cit != cells.end();
            ++cit)
        {
          VertexOnCellRange vertices_for_cell = viennagrid::ncells<0>(*cit);
          for (VertexOnCellIterator vocit = vertices_for_cell.begin();
              vocit != vertices_for_cell.end();
              ++vocit)
          {
            VertexType & vertex = *vocit;
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
        if (isVertexOriented)
        {
          writer << "object \"VisData\" class array items " << pointnum << " data follows" << std::endl;
          //some quantity here
          
          //curSeg.template begin<0>()->setCurrentSegment(curSeg);
          for (VertexIterator vit = vertices.begin();
              vit != vertices.end();
              ++vit)
          {
            //Vertex & vertex = *vit;
            double val = 1.0;//vertex.template retrieveQuantity<double>(keyname);
            writer << DXfixer(val);
            writer << std::endl;
          }

          writer << "attribute \"dep\" string \"positions\"" << std::endl;
        }
        else
        {
          writer << "object \"VisData\" class array items " << cellnum << " data follows" << std::endl;

          //some quantity here
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
            //Cell & cell = *cit;
            //writer << DXfixer(cell.template retrieveQuantity<double>(keyname));
            writer << DXfixer(1.0);
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

    }; // opendx_writer

  } //namespace io
} //namespace viennagrid

#endif


