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
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"

/** @file viennagrid/mesh/mesh_operations.hpp
    @brief Helper routines on a mesh
*/

namespace viennagrid
{

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

    SrcVertexRangeType vertices = viennagrid::elements( src_mesh_obj );
    for (SrcVertexRangeIterator it = vertices.begin(); it != vertices.end(); ++it)
      vertex_handle_map[it.handle()] = viennagrid::make_vertex( dst_mesh_obj, viennagrid::point(src_mesh_obj, *it) );



    for (typename SrcSegmentationT::const_iterator seg_it = src_segmentation.begin(); seg_it != src_segmentation.end(); ++seg_it)
    {
//         dst_segments.push_back( viennagrid::make_view<dst_segment_handle_type>(dst_mesh) );
      DstSegmentHandleType & dst_segment = dst_segmentation.get_make_segment( seg_it->id() );

      SrcCellRangeType cells = viennagrid::elements( *seg_it );
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
          CoboundaryElementViewRangeType view_elements = viennagrid::elements( mesh_view );
          if ( viennagrid::find_by_handle(mesh_view, it.handle()) == view_elements.end() )
          {
            view_elements.insert_unique_handle( it.handle() );
          }
        }

        mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, TailT>::mark(mesh_obj, mesh_view, host_element);





  //       std::cout << typeid(typename CoboundaryElementT::tag).name() << std::endl;
  //       std::cout << typeid(ReferencingElementTypelist).name() << std::endl;
  //
  //       ElementRangeType view_elements = viennagrid::elements( mesh_view );
  //       if ( viennagrid::find_by_handle(mesh_view, host_element) == view_elements.end() )
  //       {
  //         std::cout << "Adding element " << viennagrid::dereference_handle(mesh_obj, host_element) << std::endl;
  //         view_elements.insert_unique_handle( host_element );
  //       }
  //
  //       typedef typename viennagrid::result_of::handle<MeshT, CoboundaryElementT>::type CoboundaryElementHandle;
  //       typedef typename viennagrid::result_of::coboundary_range<MeshT, ElementType, CoboundaryElementT>::type CoboundaryElementRangeType;
  //       typedef typename viennagrid::result_of::iterator<CoboundaryElementRangeType>::type CoboundaryElementRangeIterator;
  //
  //       CoboundaryElementRangeType coboundary_elements = viennagrid::coboundary_elements<ElementType, CoboundaryElementT>(mesh_obj, host_element);
  //       for (CoboundaryElementRangeIterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
  //       {
  // //         mark_referencing_elements_impl<MeshT, ToEraseViewT, CoboundaryElementHandle, TailT>::mark(mesh_obj, mesh_view, it.handle());
  //         mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, TailT>::mark(mesh_obj, mesh_view, host_element);
  //       }
  //
  //       typedef typename viennagrid::result_of::referencing_element_typelist<MeshT, ElementType>::type CurrentReferencingElementTypelist;
  // //       typedef typename viennagrid::result_of::boundary_element_typelist<ElementType>::type BoundaryEl  ementTypelist;
  // //       mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, CurrentReferencingElementTypelist>::mark(mesh_obj, mesh_view, host_element);
      }
    };

    /** @brief For internal use only */
    template<typename MeshT, typename ToEraseViewT, typename HandleT>
    struct mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::null_type >
    {
      static void mark(MeshT &, ToEraseViewT &, HandleT)
      {
  //       typedef typename viennagrid::storage::detail::result_of::value_type<HandleT>::type ElementType;
  //       typedef typename viennagrid::result_of::element_range<ToEraseViewT, ElementType>::type ElementRangeType;
  //
  // //       std::cout << typeid(typename ElementType::tag).name() << std::endl;
  //
  //       ElementRangeType view_elements = viennagrid::elements( mesh_view );
  //       if ( viennagrid::find_by_handle(mesh_view, host_element) == view_elements.end() )
  //       {
  //         view_elements.insert_unique_handle( host_element );
  //       }
      }
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
