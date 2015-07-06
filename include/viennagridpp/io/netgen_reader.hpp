#ifndef VIENNAGRID_IO_NETGEN_READER_HPP
#define VIENNAGRID_IO_NETGEN_READER_HPP

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
#include <assert.h>
#include <iterator>

#include "viennagridpp/core.hpp"
#include "viennagridpp/io/helper.hpp"

/** @file viennagrid/io/netgen_reader.hpp
    @brief Provides a reader for Netgen files
*/

namespace viennagrid
{
  namespace io
  {

    /** @brief Reader for Netgen files obtained from the 'Export mesh...' menu item. Tested with Netgen version 4.9.12. */
    struct netgen_reader
    {
      /** @brief The functor interface triggering the read operation.
       *
       * @param mesh_obj      The mesh where the file content is written to
       * @param filename      Name of the file
       */
      template <typename MeshType>
      void operator()(MeshType const & mesh_obj, std::string const & filename) const
      {
        typedef typename viennagrid::result_of::coord<MeshType>::type     CoordType;
        typedef typename result_of::element<MeshType, vertex_tag>::type   VertexType;

        std::ifstream reader(filename.c_str());
        std::string tmp;

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception("* ViennaGrid: netgen_reader::operator(): File " + filename + ": Cannot open file!");
        }

        long node_num = 0;
        long cell_num = 0;

        if (!reader.good())
          throw bad_file_format_exception("* ViennaGrid: netgen_reader::operator(): File " + filename + " is empty.");

        //
        // Read vertices:
        //
        {
          getline( reader, tmp );
          std::istringstream line(tmp);

          line >> node_num;
          assert(node_num > 0);
        }

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << node_num << " vertices... " << std::endl;
        #endif


        for (int i=0; i<node_num; i++)
        {
          if (!reader.good())
            throw bad_file_format_exception("* ViennaGrid: netgen_reader::operator(): File " + filename + ": EOF encountered while reading vertices.");

          getline( reader, tmp );
          std::istringstream line(tmp);

          viennagrid::point p;
          std::copy( std::istream_iterator<CoordType>(line),
                     std::istream_iterator<CoordType>(),
                     std::back_inserter(p) );

          viennagrid::make_vertex( mesh_obj, p );
        }

        if (!reader.good())
          throw bad_file_format_exception("* ViennaGrid: netgen_reader::operator(): File " + filename + ": EOF encountered when reading number of cells.");


        //
        // Read cells:
        //
        {
          getline( reader, tmp );
          std::istringstream line(tmp);

          line >> cell_num;
          assert(cell_num > 0);
        }

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;
        #endif

        typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRange;
        ConstVertexRange vertices(mesh_obj);

        for (int i=0; i<cell_num; ++i)
        {
          if (!reader.good())
            throw bad_file_format_exception("* ViennaGrid: netgen_reader::operator(): File " + filename + ": EOF encountered while reading cells (segment index expected).");

          getline( reader, tmp );
          std::istringstream line(tmp);

          int segment_index;
          std::vector<viennagrid_int> vertex_indices;
          line >> segment_index;

          while (line.good())
          {
            viennagrid_int index;
            line >> index;
            vertex_indices.push_back(index);
          }


          std::vector<VertexType> cell_vertex_handles( vertex_indices.size() );
          for (std::size_t j=0; j<vertex_indices.size(); ++j)
            cell_vertex_handles[j] = vertices[ vertex_indices[j]-1];

          if (vertex_indices.size() == 2)
            viennagrid::make_element<viennagrid::line_tag>(mesh_obj.get_or_create_region(segment_index),
                                                           cell_vertex_handles.begin(),
                                                           cell_vertex_handles.end());
          else if (vertex_indices.size() == 3)
            viennagrid::make_element<viennagrid::triangle_tag>(mesh_obj.get_or_create_region(segment_index),
                                                               cell_vertex_handles.begin(),
                                                               cell_vertex_handles.end());
          else if (vertex_indices.size() == 4)
            viennagrid::make_element<viennagrid::tetrahedron_tag>(mesh_obj.get_or_create_region(segment_index),
                                                                  cell_vertex_handles.begin(),
                                                                  cell_vertex_handles.end());
        }
      } //operator()

    }; //class netgen_reader

  } //namespace io
} //namespace viennagrid

#endif
