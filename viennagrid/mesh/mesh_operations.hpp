#ifndef VIENNAGRID_MESH_OPERATIONS_HPP
#define VIENNAGRID_MESH_OPERATIONS_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_creation.hpp"

namespace viennagrid
{

  template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT>
  void copy_cells(SrcMeshT const & src_mesh,  SrcSegmentationT const & src_segmentation,
                  DstMeshT & dst_mesh,        DstSegmentationT & dst_segmentation )
  {
  //     typedef typename src_segment_container_type::value_type src_segment_handle_type;
  //     typedef typename dst_segment_container_type::value_type dst_segment_handle_type;
      typedef typename viennagrid::result_of::segment<SrcSegmentationT>::type SrcSegmentType;
      typedef typename viennagrid::result_of::segment<DstSegmentationT>::type DstSegmentType;
      
      typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type  SrcCellTag;
      typedef typename viennagrid::result_of::cell<SrcMeshT>::type      SrcCellType;
      
      typedef typename viennagrid::result_of::const_vertex_range<SrcMeshT>::type    SrcVertexRangeType;
      typedef typename viennagrid::result_of::iterator<SrcVertexRangeType>::type      SrcVertexRangeIterator;
      
      typedef typename viennagrid::result_of::const_cell_range<SrcSegmentType>::type  SrcCellRangeType;
      typedef typename viennagrid::result_of::iterator<SrcCellRangeType>::type        SrcCellRangeIterator;
      
      typedef typename viennagrid::result_of::const_vertex_handle<SrcMeshT>::type   SrcConstVertexHandle;
      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type         DstVertexHandleType;
      
      if (&src_mesh == &dst_mesh)
          return;
      
      dst_mesh.clear();
      dst_segmentation.clear();
      
      std::map<SrcConstVertexHandle, DstVertexHandleType> vertex_handle_map;
      
      SrcVertexRangeType vertices = viennagrid::elements( src_mesh );
  //     std::cout << "Copy: vertex count = " << vertices.size() << std::endl;
      for (SrcVertexRangeIterator it = vertices.begin(); it != vertices.end(); ++it)
          vertex_handle_map[it.handle()] = viennagrid::make_vertex( dst_mesh, viennagrid::point(src_mesh, *it) );
      

      
      for (typename SrcSegmentationT::const_iterator seg_it = src_segmentation.begin(); seg_it != src_segmentation.end(); ++seg_it)
      {
  //         dst_segments.push_back( viennagrid::make_view<dst_segment_handle_type>(dst_mesh) );
          DstSegmentType & dst_segment = dst_segmentation.get_make_segment( seg_it->id() );
          
          SrcCellRangeType cells = viennagrid::elements( *seg_it );
  //         std::cout << "Copy: cell count = " << cells.size() << std::endl;
          for (SrcCellRangeIterator it = cells.begin(); it != cells.end(); ++it)
          {
              SrcCellType const & cell = *it;
              
              std::deque<DstVertexHandleType> vertex_handles;
              
              typedef typename viennagrid::result_of::const_vertex_range<SrcCellType>::type SrcVertexOnSrcCellRangeType;
              typedef typename viennagrid::result_of::iterator<SrcVertexOnSrcCellRangeType>::type SrcVertexOnSrcCellRangeIterator;
              
              SrcVertexOnSrcCellRangeType cell_vertices = viennagrid::elements(cell);
              for (SrcVertexOnSrcCellRangeIterator jt = cell_vertices.begin(); jt != cell_vertices.end(); ++jt)
                  vertex_handles.push_back( vertex_handle_map[jt.handle()] );
              
              typedef typename viennagrid::result_of::cell<DstMeshT>::type DstCellType;
              viennagrid::make_element<DstCellType>( dst_segment, vertex_handles.begin(), vertex_handles.end() );
          }
      }
  }

}

#endif
