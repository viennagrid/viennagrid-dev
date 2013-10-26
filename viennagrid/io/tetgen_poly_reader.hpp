#ifndef VIENNAGRID_IO_POLY_READER_GUARD
#define VIENNAGRID_IO_POLY_READER_GUARD

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include <fstream>
#include <iostream>
#include <assert.h>

#include "viennagrid/forwards.hpp"
#include "viennagrid/io/helper.hpp"

#include "viennagrid/mesh/mesh.hpp"

#include "viennagrid/algorithm/geometry.hpp"

/** @file viennagrid/io/tetgen_poly_reader.hpp
    @brief Provides a reader for Tetgen .poly files. See http://wias-berlin.de/software/tetgen/fformats.poly.html
*/



namespace viennagrid
{
  namespace io
  {

    template<typename stream_type>
    bool get_valid_line( stream_type & stream, std::string & line, char comment_line = '#' )
    {
      std::string tmp;

      while (true)
      {
        if (!std::getline(stream, tmp))
          return false;

        std::size_t pos = tmp.find(comment_line);
        if (pos != std::string::npos)
          tmp = tmp.substr(0, pos);

        for (std::size_t i = 0; i != tmp.size(); ++i)
        {
          if ( tmp[i] != ' ' )
          {
            line = tmp.substr(i, std::string::npos);
            return true;
          }
        }
      }
    }

    /** @brief Reader for Tetgen .poly files.
      *
      * See http://wias-berlin.de/software/tetgen/fformats.poly.html for a description of the file format
      */
    struct tetgen_poly_reader
    {
      /** @brief The functor interface triggering the read operation. Segmentations are not supported in this version.
       *
       * @param mesh_obj      The mesh where the file content is written to
       * @param filename      Name of the file
       */
      template <typename MeshT>
      int operator()(MeshT & mesh_obj, std::string const & filename) const
      {

        typedef typename viennagrid::result_of::point<MeshT>::type           PointType;
        typedef typename viennagrid::result_of::coord< PointType >::type     CoordType;

        static const int point_dim = viennagrid::result_of::static_size<PointType>::value;

        typedef plc_tag CellTag;
        typedef typename result_of::element<MeshT, CellTag>::type            CellType;
        typedef typename result_of::handle<MeshT, CellTag>::type             CellHandleType;

        typedef typename result_of::element<MeshT, vertex_tag>::type         VertexType;
        typedef typename result_of::handle<MeshT, vertex_tag>::type          VertexHandleType;
        typedef typename VertexType::id_type VertexIDType;

        typedef typename result_of::element<MeshT, line_tag>::type           LineType;
        typedef typename result_of::handle<MeshT, line_tag>::type            LineHandleType;

        std::ifstream reader(filename.c_str());

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* poly_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception(filename);
          return EXIT_FAILURE;
        }

        std::string tmp;
        std::istringstream current_line;

        long node_num = 0;
        long dim = 0;
        long attribute_num = 0;
        long boundary_marker_num = 0;


        if (!reader.good())
          throw bad_file_format_exception(filename, "File is empty.");

        //
        // Read vertices:
        //
        if (!get_valid_line(reader, tmp))
          throw bad_file_format_exception(filename, "EOF encountered when reading information");

        current_line.str(tmp); current_line.clear();
        current_line >> node_num >> dim >> attribute_num >> boundary_marker_num;

        if (node_num < 0)
          throw bad_file_format_exception(filename, "POLY file has less than 0 nodes");
        if (dim != point_dim)
          throw bad_file_format_exception(filename, "POLY point dimension missmatch");
        if (attribute_num < 0)
          throw bad_file_format_exception(filename, "POLY file has less than 0 attributes");
        if ((boundary_marker_num < 0) || (boundary_marker_num > 1))
          throw bad_file_format_exception(filename, "POLY file has not 0 or 1 boundary marker");

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* poly_reader::operator(): Reading " << node_num << " vertices... " << std::endl;
        #endif

        typedef typename VertexIDType::base_id_type id_type;

        for (int i=0; i<node_num; i++)
        {
          if (!get_valid_line(reader, tmp))
              throw bad_file_format_exception(filename, "EOF encountered when reading information");

          typename VertexIDType::base_id_type id;

          current_line.str(tmp); current_line.clear();
          current_line >> id;

          PointType p;

          for (int j=0; j<point_dim; j++)
            current_line >> p[j];

          //VertexHandleType vertex = viennagrid::make_vertex_with_id( mesh_obj, VertexIDType(id), p );

          if (attribute_num > 0)
          {
            std::vector<CoordType> attributes(attribute_num);
            for (int j=0; j<attribute_num; j++)
              current_line >> attributes[j];

              // TODO fix using accesor or appendix!
//                 viennadata::access<poly_attribute_tag, std::vector<CoordType> >()(viennagrid::dereference_handle(mesh_obj, vertex)) = attributes;
          }
        }

        //std::cout << "DONE" << std::endl;
        if (!reader.good())
          throw bad_file_format_exception(filename, "EOF encountered when reading number of cells.");


        //
        // Read facets:
        //
        long facet_num = 0;
        if (!get_valid_line(reader, tmp))
          throw bad_file_format_exception(filename, "EOF encountered when reading information");


        current_line.str(tmp); current_line.clear();
        current_line >> facet_num >> boundary_marker_num;

        if (facet_num < 0)
          throw bad_file_format_exception(filename, "POLY file has less than 0 facets");
        if ((boundary_marker_num < 0) || (boundary_marker_num > 1))
          throw bad_file_format_exception(filename, "POLY file has not 0 or 1 boundary marker");

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;
        #endif

        for (int i=0; i<facet_num; ++i)
        {
          long polygon_num;
          long hole_num;

          if (!get_valid_line(reader, tmp))
            throw bad_file_format_exception(filename, "EOF encountered when reading information");


          current_line.str(tmp); current_line.clear();
          current_line >> polygon_num >> hole_num;

          if (polygon_num < 0)
            throw bad_file_format_exception(filename, "POLY facet has less than 0 polygons");
          if (hole_num < 0)
            throw bad_file_format_exception(filename, "POLY facet has less than 0 holes");

          std::list<LineHandleType> lines;
          std::list<VertexHandleType> vertices;


          typedef typename viennagrid::result_of::element_view<MeshT, VertexType>::type VertexViewType;

          for (int j = 0; j<polygon_num; ++j)
          {
            long vertex_num;

            if (!get_valid_line(reader, tmp))
              throw bad_file_format_exception(filename, "EOF encountered when reading information");


            current_line.str(tmp); current_line.clear();
            current_line >> vertex_num;

            if (vertex_num < 0)
              throw bad_file_format_exception(filename, "POLY polygon has less than 0 vertices");

            std::vector<VertexHandleType> vertex_handles(vertex_num);

            for (int k = 0; k<vertex_num; ++k)
            {
              long id;
              current_line >> id;
              vertex_handles[k] = viennagrid::find( mesh_obj, VertexIDType(id) ).handle();
            }

            if (vertex_num == 1)
            {
              vertices.push_back( vertex_handles.front() );
            }
            else if (vertex_num == 2)
            {
              lines.push_back( viennagrid::make_line(mesh_obj, vertex_handles[0], vertex_handles[1]) );
            }
            else
            {
              typename std::vector<VertexHandleType>::iterator it1 = vertex_handles.begin();
              typename std::vector<VertexHandleType>::iterator it2 = it1; ++it2;
              for (; it2 != vertex_handles.end(); ++it1, ++it2)
                  lines.push_back( viennagrid::make_line(mesh_obj, *it1, *it2) );
              lines.push_back( viennagrid::make_line(mesh_obj, vertex_handles.back(), vertex_handles.front()) );
            }
          }

          std::list<PointType> hole_points;

          for (int j = 0; j<hole_num; ++j)
          {
            if (!get_valid_line(reader, tmp))
              throw bad_file_format_exception(filename, "EOF encountered when reading information");

            long hole_id;

            current_line.str(tmp); current_line.clear();
            current_line >> hole_id;

            PointType p;

            for (int j=0; j<point_dim; j++)
              current_line >> p[j];

            hole_points.push_back(p);
          }



          viennagrid::make_plc(
              mesh_obj,
              lines.begin(), lines.end(),
              vertices.begin(), vertices.end(),
              hole_points.begin(), hole_points.end()
          );

        }

        return EXIT_SUCCESS;
      } //operator()

    }; //class tetgen_poly_reader

  } //namespace io
} //namespace viennagrid

#endif
