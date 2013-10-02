#ifndef VIENNAGRID_MESH_COBOUNDARY_ITERATION_HPP
#define VIENNAGRID_MESH_COBOUNDARY_ITERATION_HPP

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
#include "viennagrid/storage/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/accessor.hpp"

/** @file coboundary_iteration.hpp
    @brief Contains definition and implementation of coboundary iteration
*/

namespace viennagrid
{

  namespace result_of
  {
    /** @brief For internal use only */
    template<typename mesh_type, typename element_type_or_tag, typename coboundary_type_or_tag>
    struct coboundary_view
    {
      typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;

      typedef typename viennagrid::storage::result_of::value_type<
          typename viennagrid::storage::result_of::value_type<
              typename mesh_type::appendix_type,
              coboundary_collection_tag
            >::type,
          viennagrid::meta::static_pair<element_tag, coboundary_tag>
        >::type::container_type::value_type type;
    };

    template<typename SegmentationType, typename element_type_or_tag, typename coboundary_type_or_tag>
    struct coboundary_view< segment_handle_t<SegmentationType>, element_type_or_tag, coboundary_type_or_tag >
    {
      typedef typename coboundary_view< typename segment_handle_t<SegmentationType>::view_type, element_type_or_tag, coboundary_type_or_tag >::type type;
    };


    /** @brief Metafunction for obtaining a coboundary range of an element type within a mesh/segment
     *
     * @tparam MeshOrSegmentHandleT       The host mesh/segment type
     * @tparam ElementTypeOrTagT      The element type/tag of which the co-boundary range is created
     * @tparam CoboundaryTypeOrTagT   The co-boundary element type/tag
     */
    template<typename MeshOrSegmentHandleT, typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT>
    struct coboundary_range
    {
      typedef viennagrid::storage::container_range_wrapper< typename coboundary_view<MeshOrSegmentHandleT, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type > type;
    };

    /** @brief Metafunction for obtaining a const coboundary range of an element type within a mesh/segment
     *
     * @tparam MeshOrSegmentHandleT       The host mesh/segment type
     * @tparam ElementTypeOrTagT      The element type/tag of which the co-boundary range is created
     * @tparam CoboundaryTypeOrTagT   The co-boundary element type/tag
     */
    template<typename MeshOrSegmentHandleT, typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT>
    struct const_coboundary_range
    {
      typedef viennagrid::storage::container_range_wrapper< const typename coboundary_view<MeshOrSegmentHandleT, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type > type;
    };

    template<typename MeshOrSegmentHandleT, typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT>
    struct coboundary_range<const MeshOrSegmentHandleT, ElementTypeOrTagT, CoboundaryTypeOrTagT>
    {
      typedef typename const_coboundary_range<MeshOrSegmentHandleT, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type type;
    };
  }



  /** @brief For internal use only */
  template<typename element_type_or_tag, typename coboundary_type_or_tag, typename mesh_type, typename coboundary_accessor_type>
  void create_coboundary_information(mesh_type & mesh, coboundary_accessor_type accessor)
  {
      typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;

      typedef typename viennagrid::result_of::element< mesh_type, coboundary_type_or_tag >::type coboundary_type;

      typedef typename viennagrid::result_of::element_range< mesh_type, element_type_or_tag >::type element_range_type;
      typedef typename viennagrid::result_of::iterator< element_range_type >::type element_range_iterator;

      element_range_type elements = viennagrid::elements(mesh);


      for ( element_range_iterator it = elements.begin(); it != elements.end(); ++it )
      {
          accessor( *it ).clear();
          accessor( *it ).set_base_container( viennagrid::storage::collection::get< coboundary_type >( element_collection(mesh) ) );
      }


      typedef typename viennagrid::result_of::element_range< mesh_type, coboundary_type_or_tag >::type coboundary_element_range_type;
      typedef typename viennagrid::result_of::iterator< coboundary_element_range_type >::type coboundary_element_range_iterator;

      coboundary_element_range_type coboundary_elements = viennagrid::elements(mesh);
      for (coboundary_element_range_iterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
      {
          typedef typename viennagrid::result_of::element_range< coboundary_type, element_tag >::type element_on_coboundary_element_range_type;
          typedef typename viennagrid::result_of::iterator< element_on_coboundary_element_range_type >::type element_on_coboundary_element_range_iterator;

          element_on_coboundary_element_range_type elements_on_coboundary_element = viennagrid::elements( *it );
          for (element_on_coboundary_element_range_iterator jt = elements_on_coboundary_element.begin(); jt != elements_on_coboundary_element.end(); ++jt)
              accessor.at( *jt ).insert_handle( it.handle() );
      }
  }

  /** @brief For internal use only */
  template<typename element_type_or_tag, typename coboundary_type_or_tag, typename mesh_type>
  void create_coboundary_information(mesh_type & mesh)
  {
      typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
      typedef typename viennagrid::result_of::element< mesh_type, element_type_or_tag >::type element_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  coboundary_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, coboundary_tag>
              >::type coboundary_container_wrapper_type;

      coboundary_container_wrapper_type & coboundary_container_wrapper = coboundary_collection<element_tag, coboundary_tag>( mesh );//viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, coboundary_tag> > ( mesh.coboundary_collection() );

      create_coboundary_information<element_type_or_tag, coboundary_type_or_tag>( mesh, viennagrid::make_accessor<element_type>(coboundary_container_wrapper.container) );
      update_change_counter( mesh, coboundary_container_wrapper.change_counter );
  }



  /** @brief For internal use only */
  template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename ElementTag, typename WrappedConfigType>
  viennagrid::storage::container_range_wrapper<typename coboundary_accessor_type::value_type>
  coboundary_elements(coboundary_accessor_type accessor, element_t<ElementTag, WrappedConfigType> & element)
  {
      typedef viennagrid::storage::container_range_wrapper<typename coboundary_accessor_type::value_type> range_type;
      return range_type( accessor( element ) );
  }

  /** @brief For internal use only */
  template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename ElementTag, typename WrappedConfigType>
  viennagrid::storage::container_range_wrapper<const typename coboundary_accessor_type::value_type>
  coboundary_elements(coboundary_accessor_type const accessor, element_t<ElementTag, WrappedConfigType> const & element)
  {
      typedef viennagrid::storage::container_range_wrapper<const typename coboundary_accessor_type::value_type> range_type;
      return range_type( accessor( element ) );
  }


  /** @brief For internal use only */
  template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename WrappedConfigType, typename element_or_handle_type>
  viennagrid::storage::container_range_wrapper<typename coboundary_accessor_type::value_type>
  coboundary_elements(mesh_t<WrappedConfigType> & mesh, coboundary_accessor_type accessor, element_or_handle_type & hendl)
  {
      return coboundary_elements<element_type_or_tag, coboundary_type_or_tag>( accessor, viennagrid::dereference_handle(mesh, hendl) );
  }

  /** @brief For internal use only */
  template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename WrappedConfigType, typename element_or_handle_type>
  viennagrid::storage::container_range_wrapper<const typename coboundary_accessor_type::value_type>
  coboundary_elements(mesh_t<WrappedConfigType> const & mesh, coboundary_accessor_type const accessor, element_or_handle_type const & hendl)
  {
      return coboundary_elements<element_type_or_tag, coboundary_type_or_tag>( accessor, viennagrid::dereference_handle(mesh, hendl) );
  }




  /** @brief Obtaines a coboundary range of an element within a mesh. This function caches the coboundary information and re-creates it if the cached information is out of date. The runtime of a re-creation is linear in the number of elements of type CoboundaryTypeOrTagT within the mesh.
    *
    * @tparam ElementTypeOrTagT       The base element type/tag from which the coboundary range is obtained
    * @tparam CoboundaryTypeOrTagT    The coboundary element type/tag
    * @tparam WrappedConfigT          The wrapped config of the mesh
    * @tparam ElementOrHandleT        The element or handle type for which the coboundary range is obtained
    * @param  mesh                  The host mesh object in which the elements of the coboundary range exist
    * @param  element_or_handle       A handle or the element for which the coboundary range is obained
    * @return                         The coboundary range
    */
  template<typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT, typename WrappedConfigT, typename ElementOrHandleT>
  typename result_of::coboundary_range<mesh_t<WrappedConfigT>, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type
  coboundary_elements(mesh_t<WrappedConfigT> & mesh, ElementOrHandleT const & element_or_handle)
  {
      typedef mesh_t<WrappedConfigT> mesh_type;
      typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< CoboundaryTypeOrTagT >::type coboundary_tag;
      typedef typename viennagrid::result_of::element< mesh_type, ElementTypeOrTagT >::type element_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  coboundary_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, coboundary_tag>
              >::type coboundary_container_wrapper_type;
      coboundary_container_wrapper_type & coboundary_container_wrapper = coboundary_collection<element_tag, coboundary_tag>( mesh );

      if ( is_obsolete(mesh, coboundary_container_wrapper.change_counter) )
          create_coboundary_information<ElementTypeOrTagT, CoboundaryTypeOrTagT>(mesh);

      return coboundary_elements<ElementTypeOrTagT, CoboundaryTypeOrTagT>( viennagrid::make_accessor<element_type>(coboundary_container_wrapper.container), viennagrid::dereference_handle(mesh, element_or_handle) );

  }

  /** @brief Obtaines a const coboundary range of an element within a mesh. This function caches the coboundary information and re-creates it if the cached information is out of date. The runtime of a re-creation is linear in the number of elements of type CoboundaryTypeOrTagT within the mesh.
    *
    * @tparam ElementTypeOrTagT       The base element type/tag from which the coboundary range is obtained
    * @tparam CoboundaryTypeOrTagT    The coboundary element type/tag
    * @tparam WrappedConfigT          The wrapped config of the mesh
    * @tparam ElementOrHandleT        The element or handle type for which the coboundary range is obtained
    * @param  mesh                  The host mesh object in which the elements of the coboundary range exist
    * @param  element_or_handle       A handle or the element for which the coboundary range is obained
    * @return                         The const coboundary range
    */
  template<typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT, typename WrappedConfigT, typename ElementOrHandleT>
  typename result_of::const_coboundary_range<mesh_t<WrappedConfigT>, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type
  coboundary_elements(mesh_t<WrappedConfigT> const & mesh, ElementOrHandleT const & element_or_handle)
  {
      typedef mesh_t<WrappedConfigT> mesh_type;
      typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< CoboundaryTypeOrTagT >::type coboundary_tag;
      typedef typename viennagrid::result_of::element< mesh_type, ElementTypeOrTagT >::type element_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  coboundary_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, coboundary_tag>
              >::type coboundary_container_wrapper_type;
      coboundary_container_wrapper_type const & coboundary_container_wrapper = coboundary_collection<element_tag, coboundary_tag>( mesh );

      if ( is_obsolete(mesh, coboundary_container_wrapper.change_counter) )
          create_coboundary_information<ElementTypeOrTagT, CoboundaryTypeOrTagT>( const_cast<mesh_type&>(mesh) );

      return coboundary_elements<ElementTypeOrTagT, CoboundaryTypeOrTagT>( viennagrid::make_accessor<element_type>(coboundary_container_wrapper.container), viennagrid::dereference_handle(mesh, element_or_handle) );
  }



  /** @brief Obtaines a coboundary range of an element within a segment. This function caches the coboundary information and re-creates it if the cached information is out of date. The runtime of a re-creation is linear in the number of elements of type CoboundaryTypeOrTagT within the segment.
    *
    * @tparam ElementTypeOrTagT       The base element type/tag from which the coboundary range is obtained
    * @tparam CoboundaryTypeOrTagT    The coboundary element type/tag
    * @tparam SegmentationT           The segmentation type of the segment type
    * @tparam ElementOrHandleT        The element or handle type for which the coboundary range is obtained
    * @param  segment                 The host segment object in which the elements of the coboundary range exist
    * @param  element_or_handle       A handle or the element for which the coboundary range is obained
    * @return                         The coboundary range
    */
  template<typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT, typename SegmentationT, typename element_or_handle_type>
  typename result_of::coboundary_range<segment_handle_t<SegmentationT>, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type
  coboundary_elements(segment_handle_t<SegmentationT> & segment, element_or_handle_type const & element_or_handle)
  {
      return coboundary_elements<ElementTypeOrTagT, CoboundaryTypeOrTagT>( segment.view(), element_or_handle );
  }

  /** @brief Obtaines a const coboundary range of an element within a segment. This function caches the coboundary information and re-creates it if the cached information is out of date. The runtime of a re-creation is linear in the number of elements of type CoboundaryTypeOrTagT within the segment.
    *
    * @tparam ElementTypeOrTagT       The base element type/tag from which the coboundary range is obtained
    * @tparam CoboundaryTypeOrTagT    The coboundary element type/tag
    * @tparam SegmentationT           The segmentation type of the segment type
    * @tparam ElementOrHandleT        The element or handle type for which the coboundary range is obtained
    * @param  segment                 The host segment object in which the elements of the coboundary range exist
    * @param  element_or_handle       A handle or the element for which the coboundary range is obained
    * @return                         The const coboundary range
    */
  template<typename ElementTypeOrTagT, typename CoboundaryTypeOrTagT, typename SegmentationT, typename element_or_handle_type>
  typename result_of::const_coboundary_range<segment_handle_t<SegmentationT>, ElementTypeOrTagT, CoboundaryTypeOrTagT>::type
  coboundary_elements(segment_handle_t<SegmentationT> const & segment, element_or_handle_type const & element_or_handle)
  {
      return coboundary_elements<ElementTypeOrTagT, CoboundaryTypeOrTagT>( segment.view(), element_or_handle );
  }

}

#endif