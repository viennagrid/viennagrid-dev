#ifndef VIENNAGRID_IO_OPENDX_WRITER_HPP
#define VIENNAGRID_IO_OPENDX_WRITER_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <fstream>
#include <iostream>
#include <cmath>
#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/accessor.hpp"

/** @file viennagrid/io/opendx_writer.hpp
    @brief Provides a writer for OpenDX files
*/

namespace viennagrid
{
  namespace io
  {

    /////////////////// OpenDX export ////////////////////////////


    /** @brief Fix for a OpenDX bug: if floating-values occur, no integers (i.e. only zeros after decimal point) are allowed
     *
     * As a remedy, we add a value of 1e-5. Better to have a small error in the visualization instead of a crashed OpenDX...
     *
     * @tparam FloatingPointType  Dummy template argument to disable external linkage. Almost always float or double.
     */
    template <typename FloatingPointType>
    FloatingPointType DXfixer(FloatingPointType value)
    {
      if ( std::fabs(value * 10000 - static_cast<long>(value * 10000)) < FloatingPointType(1.0))
        return value + 0.00001;

      return value;
    }

    /** @brief A helper class returning dimension-dependent attribute strings. */
    struct DXHelper
    {
      DXHelper(int dimension_) : dimension(dimension_) {}

      std::string getAttributes() const
      {
        switch (dimension)
        {
          case 1:
            return "attribute \"element type\" string \"line\" ";
          case 2:
            return "attribute \"element type\" string \"triangles\" ";
          case 3:
            return "attribute \"element type\" string \"tetrahedra\" ";
        }
        return "";
      }

      int dimension;
    };


//     template <int DIM>
//     struct DXHelper {};
//
//     template <>
//     struct DXHelper<1>
//     {
//       static std::string getAttributes()
//       { return "attribute \"element type\" string \"line\" "; }
//     };
//
//     template <>
//     struct DXHelper<2>
//     {
//       static std::string getAttributes()
//       { return "attribute \"element type\" string \"triangles\" "; }
//     };
//
//     template <>
//     struct DXHelper<3>
//     {
//       static std::string getAttributes()
//       { return "attribute \"element type\" string \"tetrahedra\" "; }
//     };

    /** @brief The OpenDX writer object. Does not support segments - always the full mesh is written.
     *
     * @tparam MeshType   The ViennaGrid mesh.
     */
    template <typename MeshType>
    class opendx_writer
    {

        typedef typename viennagrid::result_of::point<MeshType>::type PointType;
        typedef typename viennagrid::result_of::coord<PointType>::type CoordType;

        typedef typename result_of::element<MeshType>::type     ElementType;

        typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type   VertexRange;
        typedef typename viennagrid::result_of::iterator<VertexRange>::type              VertexIterator;

        typedef typename viennagrid::result_of::const_cell_range<MeshType>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                        CellIterator;

        typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type     VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;

        typedef base_dynamic_accessor<double, ElementType> VertexScalarBaseAccessor;
        typedef std::map< std::string, VertexScalarBaseAccessor * > VertexScalarOutputAccessorContainer;

        typedef base_dynamic_accessor<double, ElementType> CellScalarBaseAccessor;
        typedef std::map< std::string, CellScalarBaseAccessor * > CellScalarOutputAccessorContainer;


      public:
        /** @brief Triggers the writing of the mesh to a file
         *
         * @param mesh_obj  A ViennaGrid mesh
         * @param filename  Name of the file
         */
        void operator()(MeshType const & mesh_obj, std::string const & filename)
        {
          const int geometric_dim = viennagrid::geometric_dimension(mesh_obj);

          DXHelper dxhelper(geometric_dim);
//           typedef DXHelper<geometric_dim>  DXHelper;

          std::ofstream writer(filename.c_str());
          if (!writer.is_open())
          {
            throw cannot_open_file_exception("* ViennaGrid: opendx_writer::operator(): File " + filename + ": Cannot open file!");
          }

          std::size_t pointnum = viennagrid::vertices(mesh_obj).size();

          writer << "object \"points\" class array type float rank 1 shape " << geometric_dim << " items ";
          writer << pointnum << " data follows" << std::endl;

          //Nodes:
          VertexRange vertices(mesh_obj);
          for (VertexIterator vit = vertices.begin();
              vit != vertices.end();
              ++vit)
          {
            PointWriter::write(writer, viennagrid::get_point(mesh_obj, *vit) );
//             PointWriter<geometric_dim>::write(writer, viennagrid::get_point( mesh_obj, *vit ) );
            writer << std::endl;
          }
          writer << std::endl;

          //Cells:
          std::size_t cellnum = viennagrid::cells(mesh_obj).size();
          writer << "object \"grid_Line_One\" class array type int rank 1 shape " << (geometric_dim + 1) << " items " << cellnum << " data follows" << std::endl;

          CellRange cells(mesh_obj);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
            VertexOnCellRange vertices_for_cell(*cit);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
              ElementType const & vertex = *vocit;
              writer << vertex.id() << " ";
            }
            writer << std::endl;
          }

          writer << dxhelper.getAttributes() << std::endl;
          writer << "attribute \"ref\" string \"positions\" " << std::endl;
          writer << std::endl;

          //set output-format:
          writer.flags(std::ios::fixed);
          writer.precision(5);

          //write quantity:
          if (vertex_scalar_data.size() > 0)
          {
            writer << "object \"VisData\" class array items " << pointnum << " data follows" << std::endl;
            //some quantity here

            for (VertexIterator vit = vertices.begin();
                vit != vertices.end();
                ++vit)
            {
              writer << DXfixer( vertex_scalar_data.begin()->second->get(*vit) );
              writer << std::endl;
            }

            writer << "attribute \"dep\" string \"positions\"" << std::endl;
          }
          else if (cell_scalar_data.size() > 0)
          {
            writer << "object \"VisData\" class array items " << cellnum << " data follows" << std::endl;

            //some quantity here
            for (CellIterator cit = cells.begin();
                cit != cells.end();
                ++cit)
            {
              writer << DXfixer( cell_scalar_data.begin()->second->get(*cit) );
              writer << std::endl;
            }
            writer << "attribute \"dep\" string \"connections\"" << std::endl;
          }

          writer << std::endl;
          writer << "object \"AttPotential\" class field " << std::endl;
          writer << "component \"data\" \"VisData\" " << std::endl;
          writer << "component \"positions\" \"points\"" << std::endl;
          writer << "component \"connections\" \"grid_Line_One\"" << std::endl;
        } // operator()


    private:


      template<typename MapType, typename AccessorOrFieldType>
      void add_to_container(MapType & map, AccessorOrFieldType const & accessor_or_field, std::string const & name)
      {
          typename MapType::iterator it = map.find(name);
          if (it != map.end())
          {
            delete it->second;
            it->second = new dynamic_accessor_wrapper<base_dynamic_accessor<double, ElementType>,
                                                      AccessorOrFieldType>( accessor_or_field );
          }
          else
            map[name] = new dynamic_accessor_wrapper<base_dynamic_accessor<double, ElementType>,
                                                     AccessorOrFieldType>( accessor_or_field );
      }


    public:


        /** @brief Adds scalar data on vertices for writing to the OpenDX file. Only one quantity at a time is supported! */
        template <typename T>
        void add_scalar_data_on_vertices(T const & accessor, std::string name)
        {
          if (vertex_scalar_data.size() > 0)
            std::cout << "* ViennaGrid: Warning: OpenDX vertex data " << name
                      << " will be ignored, because other data is already available!" << std::endl;

          add_to_container( vertex_scalar_data, accessor, name );
        }

        /** @brief Adds scalar data on cells for writing to the OpenDX file. Note that vertex data has precedence. Only one quantity at a time is supported! */
        template <typename T>
        void add_scalar_data_on_cells(T const & accessor, std::string name)
        {
          if (cell_scalar_data.size() > 0)
            std::cout << "* ViennaGrid: Warning: OpenDX cell data " << name
                      << " will be ignored, because other cell data is already available!" << std::endl;

          if (vertex_scalar_data.size() > 0)
            std::cout << "* ViennaGrid: Warning: OpenDX cell data " << name
                      << " will be ignored, because other vertex data is already available!" << std::endl;

          add_to_container( cell_scalar_data, accessor, name );
        }


      private:

        VertexScalarOutputAccessorContainer          vertex_scalar_data;
        CellScalarOutputAccessorContainer            cell_scalar_data;

    }; // opendx_writer


    /** @brief Registers scalar-valued data on vertices at the OpenDX writer. At most one data set is allowed.
      *
      * @tparam MeshT         The mesh type to be written
      * @tparam AccessorT       An accessor type holding scalar data
      * @param  writer          The OpenDX writer object for which the data should be registered
      * @param  accessor        The accessor object holding scalar data on vertices
      * @param  quantity_name   Ignored. Only used for a homogeneous interface with VTK reader/writer.
      */
    template <typename MeshT, typename AccessorT>
    opendx_writer<MeshT> & add_scalar_data_on_vertices(opendx_writer<MeshT> & writer,
                                                         AccessorT const & accessor,
                                                         std::string const & quantity_name)
    {
      writer.add_scalar_data_on_vertices(accessor, quantity_name);
      return writer;
    }



    /** @brief Registers scalar-valued data on cells at the OpenDX writer. At most one data set is allowed.
      *
      * @tparam MeshT         The mesh type to be written
      * @tparam AccessorT       An accessor type holding scalar data
      * @param  writer          The OpenDX writer object for which the data should be registered
      * @param  accessor        The accessor object holding scalar data on cells
      * @param  quantity_name   Ignored. Only used for a homogeneous interface with VTK reader/writer.
      */
    template <typename MeshT, typename AccessorT>
    opendx_writer<MeshT> & add_scalar_data_on_cells(opendx_writer<MeshT> & writer,
                                                      AccessorT const & accessor,
                                                      std::string const & quantity_name)
    {
      writer.add_scalar_data_on_cells(accessor, quantity_name);
      return writer;
    }



  } //namespace io
} //namespace viennagrid

#endif


