#ifndef VIENNAGRID_ELEMENT_DELETION_HPP
#define VIENNAGRID_ELEMENT_DELETION_HPP

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
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include <iterator>


/** @file element_deletion.hpp
    @brief Contains functions for deleting elements from a domin
*/


namespace viennagrid
{
  /** @brief For internal use only */
  template<typename MeshT, typename ToSwtichElementHandleT>
  struct switch_handle_functor
  {
    typedef typename viennagrid::storage::handle::result_of::value_type<ToSwtichElementHandleT>::type to_switch_element_type;

    switch_handle_functor(MeshT & mesh_, ToSwtichElementHandleT from_, ToSwtichElementHandleT to_) : mesh(mesh_), from(from_), to(to_) {}

    template<typename ParentElementTypeOrTagT>
    void operator() ( viennagrid::meta::tag<ParentElementTypeOrTagT> )
    {
      typedef typename viennagrid::result_of::element<MeshT, ParentElementTypeOrTagT>::type ParentElementType;
      typedef typename viennagrid::result_of::element_range<MeshT, ParentElementTypeOrTagT>::type ParentElementRangeType;
      typedef typename viennagrid::result_of::iterator<ParentElementRangeType>::type ParentElementRangeIterator;

      ParentElementRangeType parent_elements = viennagrid::elements(mesh);
      for (ParentElementRangeIterator it = parent_elements.begin(); it != parent_elements.end(); ++it)
      {
        typedef typename viennagrid::result_of::element_range<ParentElementType, to_switch_element_type>::type ToSwitchElementRangeType;
        typedef typename viennagrid::result_of::iterator<ToSwitchElementRangeType>::type ToSwitchElementRangeIterator;

        ToSwitchElementRangeType to_switch_elements = viennagrid::elements(*it);
        for (ToSwitchElementRangeIterator jt = to_switch_elements.begin(); jt != to_switch_elements.end(); ++jt)
        {
          if (jt.handle() == from)
          {
            jt.handle() = to;
          }
        }
      }
    }

    MeshT & mesh;
    ToSwtichElementHandleT from;
    ToSwtichElementHandleT to;
  };



  /** @brief For internal use only */
  template<typename MeshT, typename HandleT>
  void switch_handle( MeshT & mesh, HandleT old_handle, HandleT new_handle)
  {
    typedef typename viennagrid::storage::handle::result_of::value_type<HandleT>::type ToSwitchElementType;
    typedef typename viennagrid::result_of::referencing_element_typelist<MeshT, ToSwitchElementType>::type ParentElementTypelist;

    switch_handle_functor<MeshT, HandleT> functor(mesh, old_handle, new_handle);

    viennagrid::meta::typelist::for_each<ParentElementTypelist>( functor );
  }



  /** @brief For internal use only */
  template<typename MeshT, typename ToEraseViewT, typename HandleT, typename ReferencingElementTypelist =
      typename viennagrid::result_of::referencing_element_typelist<MeshT, typename viennagrid::storage::handle::result_of::value_type<HandleT>::type >::type >
  struct mark_erase_elements_impl;

  template<typename MeshT, typename ToEraseViewT, typename HandleT, typename CoboundaryElementT, typename TailT>
  struct mark_erase_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::meta::typelist_t<CoboundaryElementT, TailT> >
  {
    static void mark(MeshT & mesh, ToEraseViewT & mesh_view, HandleT to_erase)
    {
      typedef typename viennagrid::storage::handle::result_of::value_type<HandleT>::type ElementType;
      typedef typename viennagrid::result_of::element_range<ToEraseViewT, ElementType>::type ElementRangeType;

      ElementRangeType view_elements = viennagrid::elements( mesh_view );
      if ( viennagrid::find_by_handle(mesh_view, to_erase) == view_elements.end() )
      {
        view_elements.insert_unique_handle( to_erase );
      }

      typedef typename viennagrid::result_of::handle<MeshT, CoboundaryElementT>::type CoboundaryElementHandle;
      typedef typename viennagrid::result_of::coboundary_range<MeshT, ElementType, CoboundaryElementT>::type CoboundaryElementRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryElementRangeType>::type CoboundaryElementRangeIterator;

      CoboundaryElementRangeType coboundary_elements = viennagrid::coboundary_elements<ElementType, CoboundaryElementT>(mesh, to_erase);
      for (CoboundaryElementRangeIterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
      {
        mark_erase_elements_impl<MeshT, ToEraseViewT, CoboundaryElementHandle, TailT>::mark(mesh, mesh_view, it.handle());
      }
    }
  };

  /** @brief For internal use only */
  template<typename MeshT, typename ToEraseViewT, typename HandleT>
  struct mark_erase_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::meta::null_type >
  {
    static void mark(MeshT &, ToEraseViewT & mesh_view, HandleT to_erase)
    {
      typedef typename viennagrid::storage::handle::result_of::value_type<HandleT>::type ElementType;
      typedef typename viennagrid::result_of::element_range<ToEraseViewT, ElementType>::type ElementRangeType;

      ElementRangeType view_elements = viennagrid::elements( mesh_view );
      if ( viennagrid::find_by_handle(mesh_view, to_erase) == view_elements.end() )
      {
        view_elements.insert_unique_handle( to_erase );
      }
    }
  };




  /** @brief Marks an element for deletion
    *
    * @tparam MeshT                   The mesh type in which the element to erase lives
    * @tparam MeshViewT               The mesh view type for all elements to erase
    * @tparam HandleT                   The handle type of the element to delete
    * @param  mesh                    The host mesh object
    * @param  elements_to_erase         A mesh view which stores all elements marked for deletion
    * @param  to_erase                  A handle object referencing the element to delete
    */
  template<typename MeshT, typename MeshViewT, typename HandleT>
  void mark_erase_elements( MeshT & mesh, MeshViewT & elements_to_erase, HandleT to_erase )
  {
    mark_erase_elements_impl<MeshT, MeshViewT, HandleT>::mark(mesh, elements_to_erase, to_erase);
  }

  /** @brief For internal use only */
  template<typename MeshT, typename HandleT>
  void simple_erase_element(MeshT & mesh, HandleT & element_to_erase)
  {
    typedef typename viennagrid::storage::handle::result_of::value_type<HandleT>::type ElementType;
    typedef typename viennagrid::result_of::handle<MeshT, ElementType>::type ElementHandle;

    typedef typename viennagrid::result_of::element_range<MeshT, ElementType>::type ElementRangeType;
    typedef typename viennagrid::result_of::iterator<ElementRangeType>::type ElementRangeIterator;

    ElementRangeType elements = viennagrid::elements( mesh );
    ElementRangeIterator to_erase_it = find_by_handle( mesh, element_to_erase );

    ElementHandle from = (--elements.end()).handle();
    ElementHandle to = to_erase_it.handle();

    std::swap( *to_erase_it, *(--elements.end()) );
    elements.erase( --elements.end() );

//     std::cout << "Switching " << viennagrid::dereference_handle(mesh, from) << " with " << viennagrid::dereference_handle(mesh, to) << std::endl;

    switch_handle( mesh, from, to );
  }

  /** @brief For internal use only */
  template<typename MeshT, typename mesh_view_type>
  struct erase_functor
  {
    erase_functor(MeshT & mesh_, mesh_view_type & view_to_erase_) : mesh(mesh_), view_to_erase(view_to_erase_) {}

    template<typename ElementT>
    void operator()( viennagrid::meta::tag<ElementT> )
    {
      typedef typename viennagrid::result_of::element_range<mesh_view_type, ElementT>::type ToEraseElementRangeType;
      typedef typename viennagrid::result_of::iterator<ToEraseElementRangeType>::type ToEraseElementRangeIterator;

      typedef typename viennagrid::result_of::handle<MeshT, ElementT>::type ElementHandle;
      typedef typename viennagrid::result_of::element_range<MeshT, ElementT>::type ElementRangeType;
      typedef typename viennagrid::result_of::iterator<ElementRangeType>::type ElementRangeIterator;

      typedef typename viennagrid::result_of::id<ElementT>::type id_type;

//             std::vector< std::pair<element_range_iterator, element_range_iterator> > swtiching_map;

      std::deque<id_type> ids_to_erase;

      ToEraseElementRangeType elements_to_erase = viennagrid::elements<ElementT>(view_to_erase);
      for (ToEraseElementRangeIterator it = elements_to_erase.begin(); it != elements_to_erase.end(); ++it)
        ids_to_erase.push_back( it->id() );

      ElementRangeType elements = viennagrid::elements(mesh);
      ElementRangeIterator back_it = --elements.end();


      for (typename std::deque<id_type>::iterator it = ids_to_erase.begin(); it != ids_to_erase.end(); ++it)
      {
        ElementRangeIterator to_erase_it = find( mesh, *it );

        if (back_it != to_erase_it)
        {
          ElementHandle old_handle = back_it.handle();
          std::swap( *back_it, *to_erase_it );
          ElementHandle new_handle = to_erase_it.handle();

          switch_handle( mesh, old_handle, new_handle );
        }


        elements.erase( back_it );

        back_it--;
      }
    }

    MeshT & mesh;
    mesh_view_type & view_to_erase;
  };

  /** @brief Erases all elements marked for deletion and all elements which references these elements from a mesh
    *
    * @tparam WrappedConfigT          The wrapped config of the mesh type in which the elements to erase live
    * @tparam ToEraseViewT               The mesh view type which stores all elements to erase
    * @param  mesh                    The host mesh object
    * @param  elements_to_erase         A mesh view which stores all elements marked for deletion
    */
  template<typename WrappedConfigT, typename ToEraseViewT>
  void erase_elements(mesh_t<WrappedConfigT> & mesh, ToEraseViewT & elements_to_erase)
  {
    typedef mesh_t<WrappedConfigT> MeshType;

    typedef typename viennagrid::meta::typelist::result_of::reverse<
      typename viennagrid::result_of::element_typelist<ToEraseViewT>::type
    >::type SegmentHandleType;

    erase_functor<MeshType, ToEraseViewT> functor( mesh, elements_to_erase );
    viennagrid::meta::typelist::for_each<SegmentHandleType>(functor);
  }



  template<typename MeshViewT, typename ToEraseViewT>
  struct view_erase_functor
  {
    view_erase_functor(MeshViewT & view_, ToEraseViewT & view_to_erase_) : view(view_), view_to_erase(view_to_erase_) {}

    template<typename ElementT>
    void operator()( viennagrid::meta::tag<ElementT> )
    {
      typedef typename viennagrid::result_of::element_range<ToEraseViewT, ElementT>::type ToEraseElementRangeType;
      typedef typename viennagrid::result_of::iterator<ToEraseElementRangeType>::type ToEraseElementRangeIterator;

      ToEraseElementRangeType elements_to_erase = viennagrid::elements<ElementT>(view_to_erase);
      for (ToEraseElementRangeIterator it = elements_to_erase.begin(); it != elements_to_erase.end(); ++it)
        viennagrid::elements<ElementT>(view).erase_handle( it.handle() );

    }

    MeshViewT & view;
    ToEraseViewT & view_to_erase;
  };


  /** @brief Erases all elements marked for deletion and all elements which references these elements from a mesh
    *
    * @tparam WrappedConfigT          The wrapped config of the mesh type in which the elements to erase live
    * @tparam MeshViewT               The mesh view type which stores all elements to erase
    * @param  mesh                    The host mesh object
    * @param  elements_to_erase         A mesh view which stores all elements marked for deletion
    */
  template<typename WrappedConfigT, typename ToEraseViewT>
  void erase_elements(segment_handle_t<WrappedConfigT> & mesh, ToEraseViewT & elements_to_erase)
  {
    typedef segment_handle_t<WrappedConfigT> SegmentHandleType;

    typedef typename viennagrid::meta::typelist::result_of::reverse<
      typename viennagrid::result_of::element_typelist<ToEraseViewT>::type
    >::type ElementTypelist;

    view_erase_functor<SegmentHandleType, ToEraseViewT> functor( mesh, elements_to_erase );
    viennagrid::meta::typelist::for_each<ElementTypelist>(functor);
  }



  /** @brief Erases a single elements all elements which references this element from a mesh. If more than one element is erase use erase_elements instead for better performance.
    *
    * @tparam WrappedConfigT          The wrapped config of the mesh type in which the elements to erase live
    * @tparam HandleT                   The handle type of the element to delete
    * @param  mesh                    The host mesh object
    * @param  element_to_erase          A handle to the element to be deleted
    */
  template<typename MeshT, typename HandleT>
  void erase_element(MeshT & mesh, HandleT & element_to_erase)
  {
    typedef typename viennagrid::result_of::mesh_view<MeshT>::type ToEraseViewType;
    ToEraseViewType elements_to_erase = viennagrid::make_view(mesh);
    viennagrid::mark_erase_elements( mesh, elements_to_erase, element_to_erase );
    viennagrid::erase_elements(mesh, elements_to_erase);
  }










}


#endif

