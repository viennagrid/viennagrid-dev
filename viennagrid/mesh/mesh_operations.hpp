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

#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"


/** @file viennagrid/mesh/mesh_operations.hpp
    @brief Helper routines on a mesh
*/

namespace viennagrid
{

  template<typename SrcMeshT, typename DstMeshT>
  class vertex_copy_map
  {
  public:
    vertex_copy_map( DstMeshT & dst_mesh_ ) : dst_mesh(dst_mesh_) {}

    typedef typename viennagrid::result_of::coord<DstMeshT>::type DstNumericType;

    typedef typename viennagrid::result_of::vertex<SrcMeshT>::type SrcVertexType;
    typedef typename viennagrid::result_of::vertex_id<SrcMeshT>::type SrcVertexIDType;

    typedef typename viennagrid::result_of::vertex<DstMeshT>::type DstVertexType;
    typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstVertexHandleType;

    DstVertexHandleType operator()( SrcVertexType const & src_vertex, DstNumericType tolerance = 0.0 )
    {
      typename std::map<SrcVertexIDType, DstVertexHandleType>::iterator vit = vertex_map.find( src_vertex.id() );
      if (vit != vertex_map.end())
        return vit->second;
      else
      {
        DstVertexHandleType vh = viennagrid::make_vertex( dst_mesh, viennagrid::point(src_vertex) );
        vertex_map[src_vertex.id()] = vh;
        return vh;
      }
    }


    template<typename ElementTagT, typename WrappedConfigT>
    typename viennagrid::result_of::handle<DstMeshT, ElementTagT>::type copy_element( element<ElementTagT, WrappedConfigT> const & el, DstNumericType tolerance = 0.0 )
    {
      typedef element<ElementTagT, WrappedConfigT> ElementType;
      typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type ConstVerticesOnElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVerticesOnElementRangeType>::type ConstVerticesOnElementIteratorType;

      std::vector<DstVertexHandleType> vertex_handles;

      ConstVerticesOnElementRangeType vertices(el);
      for (ConstVerticesOnElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        vertex_handles.push_back( (*this)(*vit, tolerance) );

      return viennagrid::make_element<ElementTagT>( dst_mesh, vertex_handles.begin(), vertex_handles.end() );
    }



  private:

    DstMeshT & dst_mesh;
    std::map<SrcVertexIDType, DstVertexHandleType> vertex_map;
  };




  /** @brief Copies a mesh and an associated segmentation over to another mesh and an associated segmentation */
  template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT>
  void copy_cells(SrcMeshT const & src_mesh_obj,  SrcSegmentationT const & src_segmentation,
                  DstMeshT & dst_mesh_obj,        DstSegmentationT & dst_segmentation )
  {
//     typedef typename src_segment_container_type::value_type src_segment_handle_type;
//     typedef typename dst_segment_container_type::value_type dst_segment_handle_type;
    typedef typename viennagrid::result_of::segment_handle<SrcSegmentationT>::type SrcSegmentHandleType;
    typedef typename viennagrid::result_of::segment_handle<DstSegmentationT>::type DstSegmentHandleType;

    //typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type  SrcCellTag;
    typedef typename viennagrid::result_of::cell<SrcMeshT>::type      SrcCellType;

    typedef typename viennagrid::result_of::const_vertex_range<SrcMeshT>::type    SrcVertexRangeType;
    typedef typename viennagrid::result_of::iterator<SrcVertexRangeType>::type    SrcVertexRangeIterator;

    typedef typename viennagrid::result_of::const_cell_range<SrcSegmentHandleType>::type  SrcCellRangeType;
    typedef typename viennagrid::result_of::iterator<SrcCellRangeType>::type              SrcCellRangeIterator;

    typedef typename viennagrid::result_of::const_vertex_handle<SrcMeshT>::type   SrcConstVertexHandle;
    typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type         DstVertexHandleType;

    if (&src_mesh_obj == &dst_mesh_obj)
        return;

    dst_mesh_obj.clear();
    dst_segmentation.clear();

    std::map<SrcConstVertexHandle, DstVertexHandleType> vertex_handle_map;

    SrcVertexRangeType vertices( src_mesh_obj );
    for (SrcVertexRangeIterator it = vertices.begin(); it != vertices.end(); ++it)
      vertex_handle_map[it.handle()] = viennagrid::make_vertex( dst_mesh_obj, viennagrid::point(src_mesh_obj, *it) );



    for (typename SrcSegmentationT::const_iterator seg_it = src_segmentation.begin(); seg_it != src_segmentation.end(); ++seg_it)
    {
//         dst_segments.push_back( viennagrid::make_view<dst_segment_handle_type>(dst_mesh) );
      DstSegmentHandleType & dst_segment = dst_segmentation.get_make_segment( seg_it->id() );

      SrcCellRangeType cells( *seg_it );
      for (SrcCellRangeIterator it = cells.begin(); it != cells.end(); ++it)
      {
        SrcCellType const & cell = *it;

        std::deque<DstVertexHandleType> vertex_handles;

        typedef typename viennagrid::result_of::const_vertex_range<SrcCellType>::type SrcVertexOnSrcCellRangeType;
        typedef typename viennagrid::result_of::iterator<SrcVertexOnSrcCellRangeType>::type SrcVertexOnSrcCellRangeIterator;

        SrcVertexOnSrcCellRangeType cell_vertices(cell);
        for (SrcVertexOnSrcCellRangeIterator jt = cell_vertices.begin(); jt != cell_vertices.end(); ++jt)
          vertex_handles.push_back( vertex_handle_map[jt.handle()] );

        typedef typename viennagrid::result_of::cell<DstMeshT>::type DstCellType;
        viennagrid::make_element<DstCellType>( dst_segment, vertex_handles.begin(), vertex_handles.end() );
      }
    }
  }




  namespace detail
  {
    /** @brief For internal use only */
    template<typename MeshT, typename ToEraseViewT, typename HandleT, typename ReferencingElementTypelist =
        typename viennagrid::result_of::referencing_element_typelist<MeshT, typename viennagrid::detail::result_of::value_type<HandleT>::type >::type >
    struct mark_referencing_elements_impl;

    template<typename MeshT, typename ToEraseViewT, typename HandleT, typename CoboundaryElementT, typename TailT>
    struct mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::typelist<CoboundaryElementT, TailT> >
    {
      static void mark(MeshT & mesh_obj, ToEraseViewT & mesh_view, HandleT host_element)
      {
        //typedef viennagrid::typelist<CoboundaryElementT, TailT> ReferencingElementTypelist;
        typedef typename viennagrid::detail::result_of::value_type<HandleT>::type HostElementType;

        //typedef typename viennagrid::result_of::handle<MeshT, CoboundaryElementT>::type CoboundaryElementHandle;
        typedef typename viennagrid::result_of::coboundary_range<MeshT, HostElementType, CoboundaryElementT>::type CoboundaryElementRangeType;
        typedef typename viennagrid::result_of::iterator<CoboundaryElementRangeType>::type CoboundaryElementRangeIterator;

        typedef typename viennagrid::result_of::element_range<ToEraseViewT, CoboundaryElementT>::type CoboundaryElementViewRangeType;

        CoboundaryElementRangeType coboundary_elements = viennagrid::coboundary_elements<HostElementType, CoboundaryElementT>(mesh_obj, host_element);
        for (CoboundaryElementRangeIterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
        {
          CoboundaryElementViewRangeType view_elements( mesh_view );
          if ( viennagrid::find_by_handle(mesh_view, it.handle()) == view_elements.end() )
          {
            view_elements.insert_unique_handle( it.handle() );
          }
        }

        mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, TailT>::mark(mesh_obj, mesh_view, host_element);
      }
    };

    /** @brief For internal use only */
    template<typename MeshT, typename ToEraseViewT, typename HandleT>
    struct mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::null_type >
    {
      static void mark(MeshT &, ToEraseViewT &, HandleT) {}
    };

  } //namespace detail

  /** @brief Marks elements which reference a given host element
    *
    * @tparam MeshT                   The mesh type in which the element to erase lives
    * @tparam MeshViewT               The mesh view type for all elements to erase
    * @tparam HandleT                 The handle type of the element to delete
    * @param  mesh_obj                The host mesh object
    * @param  element_view            A mesh view which stores all elements to be marked
    * @param  host_element            A handle object of the host element
    */
  template<typename MeshT, typename MeshViewT, typename HandleT>
  void mark_referencing_elements( MeshT & mesh_obj, MeshViewT & element_view, HandleT host_element )
  {
    detail::mark_referencing_elements_impl<MeshT, MeshViewT, HandleT>::mark(mesh_obj, element_view, host_element);
  }

}

#endif
