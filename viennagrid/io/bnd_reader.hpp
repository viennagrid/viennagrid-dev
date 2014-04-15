#ifndef VIENNAGRID_BND_READER_GUARD
#define VIENNAGRID_BND_READER_GUARD

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/io/bnd.hpp"
#include "viennagrid/mesh/element_creation.hpp"

/** @file viennagrid/io/bnd_reader.hpp
    @brief Provides a reader for .bnd files.
*/


namespace viennagrid
{
  namespace io
  {

    /** @brief Reader for .bnd files.
      *
      */
    class bnd_reader
    {
    public:

      /** @brief The functor interface triggering the read operation. Segmentations are not supported in this version.
       *
       * @param mesh              The mesh where the file content is written to
       * @param segmentation      The mesh where the file content is written to
       * @param filename          Name of the file
       */
      template <typename MeshT, typename SegmentationT>
      void operator()(MeshT & mesh, SegmentationT & segmentation, std::string const & filename) const
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
        typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;
        typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;

        const int mesh_dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;
        const int num_vertices = viennagrid::boundary_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>::num;

        typedef viennautils::io::bnd_reader BNDReaderType;
        typedef BNDReaderType::index_type BNDIndexType;
        typedef BNDReaderType::point_type BNDPointType;
        typedef BNDReaderType::polygon_container_type BNDPolygonContainerType;
        typedef BNDReaderType::polygon_type BNDPolygonType;


        BNDReaderType bnd;
        if (bnd(filename) != EXIT_SUCCESS)
          throw bad_file_format_exception(filename, "BND reading error");

        if ( mesh_dimension != bnd.dim_geom() )
          throw bad_file_format_exception(filename, "Geometric dimension mismatch.");


        std::map<BNDIndexType, VertexHandleType> vertices;
        std::map<BNDPolygonType, CellHandleType> cells;


        for (std::size_t segment_id = 0; segment_id != bnd.segment_size(); ++segment_id)
        {
          SegmentHandleType segment = segmentation.get_make_segment( segment_id );
          BNDPolygonContainerType & polygons = bnd.segment( segment_id );


          for (std::size_t poly_pos = 0; poly_pos != polygons.size(); ++poly_pos)
          {
            BNDPolygonType & polygon = polygons[poly_pos];
//             std::cout << polygon.size() << std::endl;
            std::sort(polygon.begin(), polygon.end());

            if ((num_vertices > 0) && (num_vertices != polygon.size()))
            {
              std::stringstream ss;
              ss << "ERROR: polygon " << poly_pos << " has " << polygon.size() << " vertices but should have " << num_vertices << std::endl;

              throw bad_file_format_exception(filename, ss.str());
            }



            std::vector<VertexHandleType> vertex_handles;

            typename std::map<BNDPolygonType, CellHandleType>::iterator cit = cells.find( polygon );
            if (cit != cells.end())
            {
              viennagrid::add( segment, cit->second );
            }
            else
            {
              for (std::size_t vtx_pos = 0; vtx_pos != polygon.size(); ++vtx_pos)
              {
                BNDIndexType vtx_index = polygon[vtx_pos];

                typename std::map<BNDIndexType, VertexHandleType>::iterator vit = vertices.find( vtx_index );
                if (vit != vertices.end())
                  vertex_handles.push_back(vit->second);
                else
                {
                  BNDPointType const & bnd_point = bnd.point( vtx_index );
                  PointType point;
                  std::copy( bnd_point.begin(), bnd_point.end(), point.begin() );

                  VertexHandleType vtx_handle = viennagrid::make_vertex( mesh, point );

                  vertices[vtx_index] = vtx_handle;
                  vertex_handles.push_back(vtx_handle);
                }
              }

              cells[polygon] = viennagrid::make_cell( segment, vertex_handles.begin(), vertex_handles.end() );
            }
          }
        }
      }



    private:

    };



  }
}



#endif
