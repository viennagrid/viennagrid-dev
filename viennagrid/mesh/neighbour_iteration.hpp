#ifndef VIENNAGRID_MESH_NEIGHBOUR_ITERATION_HPP
#define VIENNAGRID_MESH_NEIGHBOUR_ITERATION_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/mesh/coboundary_iteration.hpp"

/** @file neighbour_iteration.hpp
    @brief Contains definition and implementation of neighbour iteration
*/

namespace viennagrid
{
  namespace result_of
  {
    /** @brief For internal use only */
    template<typename mesh_type, typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT>
    struct neighbour_view
    {
        typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< ConnectorElementTypeOrTagT >::type connector_element_tag;

        typedef typename viennagrid::storage::result_of::value_type<
            typename viennagrid::storage::result_of::value_type<
                typename mesh_type::appendix_type,
                neighbour_collection_tag
              >::type,
            viennagrid::meta::static_pair<element_tag, connector_element_tag>
          >::type::container_type::value_type type;
    };

    template<typename SegmentationT, typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT>
    struct neighbour_view< segment_handle_t<SegmentationT>, ElementTypeOrTagT, ConnectorElementTypeOrTagT >
    {
        typedef typename neighbour_view< typename segment_handle_t<SegmentationT>::view_type, ElementTypeOrTagT, ConnectorElementTypeOrTagT >::type type;
    };


    /** @brief Metafunction for obtaining a neighbour range of an element type with a connector element type within a mesh/segment
     *
     * @tparam MeshOrSegmentT             The host mesh/segment type
     * @tparam ElementTypeOrTagT            The element type/tag of which the co-boundary range is created
     * @tparam ConnectorElementTypeOrTagT   The connector element type/tag
     */
    template<typename MeshOrSegmentT, typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT>
    struct neighbour_range
    {
        typedef viennagrid::storage::container_range_wrapper< typename neighbour_view<MeshOrSegmentT, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type > type;
    };

    /** @brief Metafunction for obtaining a const neighbour range of an element type with a connector element type within a mesh/segment
     *
     * @tparam MeshOrSegmentT             The host mesh/segment type
     * @tparam ElementTypeOrTagT            The element type/tag of which the co-boundary range is created
     * @tparam ConnectorElementTypeOrTagT   The connector element type/tag
     */
    template<typename MeshOrSegmentT, typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT>
    struct const_neighbour_range
    {
        typedef viennagrid::storage::container_range_wrapper< const typename neighbour_view<MeshOrSegmentT, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type > type;
    };

    template<typename MeshOrSegmentT, typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT>
    struct neighbour_range<const MeshOrSegmentT, ElementTypeOrTagT, ConnectorElementTypeOrTagT>
    {
        typedef typename const_neighbour_range<MeshOrSegmentT, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type type;
    };

  }



  /** @brief For internal use only */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename mesh_type, typename neigbour_accessor_type>
  void create_neighbour_information(mesh_type & mesh, neigbour_accessor_type accessor)
  {
      typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< ConnectorElementTypeOrTagT >::type connector_element_tag;

      typedef typename viennagrid::result_of::element< mesh_type, ElementTypeOrTagT >::type element_type;

      typedef typename viennagrid::result_of::element_range< mesh_type, ElementTypeOrTagT >::type element_range_type;
      typedef typename viennagrid::result_of::iterator< element_range_type >::type element_range_iterator;

      element_range_type elements = viennagrid::elements(mesh);

      for ( element_range_iterator it = elements.begin(); it != elements.end(); ++it )
      {
          accessor( *it ).clear();
          accessor( *it ).set_base_container( viennagrid::storage::collection::get< element_type >( element_collection(mesh) ) );
      }


      typedef typename viennagrid::result_of::element_range< mesh_type, connector_element_tag >::type connector_element_range_type;
      typedef typename viennagrid::result_of::iterator< connector_element_range_type >::type connector_element_range_iterator;

      connector_element_range_type connector_elements = viennagrid::elements(mesh);
      for ( connector_element_range_iterator it = connector_elements.begin(); it != connector_elements.end(); ++it )
      {
          typedef typename viennagrid::result_of::coboundary_range< mesh_type, connector_element_tag, element_tag >::type element_on_connector_element_range_type;
          typedef typename viennagrid::result_of::iterator< element_on_connector_element_range_type >::type element_on_connector_element_range_iterator;

          element_on_connector_element_range_type coboundary_range = viennagrid::coboundary_elements<connector_element_tag, element_tag>( mesh, it.handle() );
          if (coboundary_range.empty())
              continue;

          element_on_connector_element_range_iterator jt1 = coboundary_range.begin(); ++jt1;
          for (; jt1 != coboundary_range.end(); ++jt1)
          {
              for (element_on_connector_element_range_iterator jt0 = coboundary_range.begin(); jt0 != jt1; ++jt0)
              {
                  typedef typename result_of::neighbour_view<mesh_type, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type view_type;
                  view_type & view = accessor( *jt0 );

                  typename view_type::iterator kt = view.begin();
                  for (; kt != view.end(); ++kt)
                      if ( kt->id() == jt1->id() )
                          break;

                  if (kt == view.end())
                  {
                      accessor( *jt0 ).insert_handle( jt1.handle() );
                      accessor( *jt1 ).insert_handle( jt0.handle() );
                  }
              }
          }
      }
  }


  /** @brief For internal use only */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename mesh_type>
  void create_neighbour_information(mesh_type & mesh)
  {
      typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< ConnectorElementTypeOrTagT >::type connector_element_tag;
      typedef typename viennagrid::result_of::element< mesh_type, ElementTypeOrTagT >::type element_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  neighbour_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, connector_element_tag>
              >::type neighbour_container_wrapper_type;
      neighbour_container_wrapper_type & neighbour_container_wrapper = neighbour_collection<element_tag, connector_element_tag>( mesh );

      create_neighbour_information<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( mesh, viennagrid::make_accessor<element_type>(neighbour_container_wrapper.container) );

      update_change_counter( mesh, neighbour_container_wrapper.change_counter );
  }



  /** @brief For internal use only */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename neigbour_accessor_type, typename ElementTag, typename WrappedConfigT>
  viennagrid::storage::container_range_wrapper<typename neigbour_accessor_type::value_type>
  neighbour_elements(neigbour_accessor_type accessor, element_t<ElementTag, WrappedConfigT> & element)
  {
      typedef viennagrid::storage::container_range_wrapper<typename neigbour_accessor_type::value_type> range_type;
      return range_type( accessor( element ) );
  }

  /** @brief For internal use only */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename neigbour_accessor_type, typename ElementTag, typename WrappedConfigT>
  viennagrid::storage::container_range_wrapper<const typename neigbour_accessor_type::value_type>
  neighbour_elements(neigbour_accessor_type const accessor, element_t<ElementTag, WrappedConfigT> const & element)
  {
      typedef viennagrid::storage::container_range_wrapper<const typename neigbour_accessor_type::value_type> range_type;
      return range_type( accessor( element ) );
  }


  /** @brief For internal use only */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename neigbour_accessor_type, typename WrappedConfigT, typename ElementOrHandleT>
  viennagrid::storage::container_range_wrapper<typename neigbour_accessor_type::value_type>
  neighbour_elements(mesh_t<WrappedConfigT> & mesh, neigbour_accessor_type accessor, ElementOrHandleT & element_or_handle)
  {
      return neighbour_elements<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( accessor, viennagrid::dereference_handle(mesh, element_or_handle) );
  }

  /** @brief For internal use only */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename neigbour_accessor_type, typename WrappedConfigT, typename ElementOrHandleT>
  viennagrid::storage::container_range_wrapper<const typename neigbour_accessor_type::value_type>
  neighbour_elements(mesh_t<WrappedConfigT> const & mesh, neigbour_accessor_type const accessor, ElementOrHandleT const & element_or_handle)
  {
      return neighbour_elements<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( accessor, viennagrid::dereference_handle(mesh, element_or_handle) );
  }



  /** @brief Obtaines a neighbour range of an element within a mesh. This function caches the neighbour information and re-creates it if the cached information is out of date. The worst case runtime of a re-creation is linear in the number of elements of type ConnectorElementTypeOrTagT time the number of elements of type ElementTypeOrTagT within the mesh.
    *
    * @tparam ElementTypeOrTagT             The base element type/tag from which the neighbour range is obtained
    * @tparam ConnectorElementTypeOrTagT    The connector element type/tag
    * @tparam WrappedConfigT                The wrapped config of the mesh
    * @tparam ElementOrHandleT              The element or handle type for which the neighbour range is obtained
    * @param  mesh                        The host mesh object in which the elements of the neighbour range exist
    * @param  handle_or_element             A handle or the element for which the neighbour range is obained
    * @return                               The neighbour range
    */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename WrappedConfigT, typename ElementOrHandleT>
  typename result_of::neighbour_range<mesh_t<WrappedConfigT>, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type
  neighbour_elements(mesh_t<WrappedConfigT> & mesh, ElementOrHandleT const & element_or_handle)
  {
      typedef mesh_t<WrappedConfigT> mesh_type;
      typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< ConnectorElementTypeOrTagT >::type connector_element_tag;
      typedef typename viennagrid::result_of::element< mesh_type, ElementTypeOrTagT >::type element_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  neighbour_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, connector_element_tag>
              >::type neighbour_container_wrapper_type;
      neighbour_container_wrapper_type & neighbour_container_wrapper = neighbour_collection<element_tag, connector_element_tag>( mesh );

      if ( is_obsolete( mesh, neighbour_container_wrapper.change_counter ) )
          create_neighbour_information<ElementTypeOrTagT, ConnectorElementTypeOrTagT>(mesh);

      return neighbour_elements<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( viennagrid::make_accessor<element_type>(neighbour_container_wrapper.container), viennagrid::dereference_handle(mesh, element_or_handle) );
  }

  /** @brief Obtaines a const neighbour range of an element within a mesh. This function caches the neighbour information and re-creates it if the cached information is out of date. The worst case runtime of a re-creation is linear in the number of elements of type ConnectorElementTypeOrTagT time the number of elements of type ElementTypeOrTagT within the mesh.
    *
    * @tparam ElementTypeOrTagT             The base element type/tag from which the neighbour range is obtained
    * @tparam ConnectorElementTypeOrTagT    The connector element type/tag
    * @tparam WrappedConfigT                The wrapped config of the mesh
    * @tparam ElementOrHandleT              The element or handle type for which the neighbour range is obtained
    * @param  mesh                        The host mesh object in which the elements of the neighbour range exist
    * @param  handle_or_element             A handle or the element for which the neighbour range is obained
    * @return                               The const neighbour range
    */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename WrappedConfigT, typename ElementOrHandleT>
  typename result_of::const_neighbour_range<mesh_t<WrappedConfigT>, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type
  neighbour_elements(mesh_t<WrappedConfigT> const & mesh, ElementOrHandleT const & element_or_handle)
  {
      typedef mesh_t<WrappedConfigT> mesh_type;
      typedef typename viennagrid::result_of::element_tag< ElementTypeOrTagT >::type element_tag;
      typedef typename viennagrid::result_of::element_tag< ConnectorElementTypeOrTagT >::type connector_element_tag;
      typedef typename viennagrid::result_of::element< mesh_type, ElementTypeOrTagT >::type element_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  neighbour_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, connector_element_tag>
              >::type neighbour_container_wrapper_type;
      neighbour_container_wrapper_type const & neighbour_container_wrapper = neighbour_collection<element_tag, connector_element_tag>( mesh );

      if ( is_obsolete( mesh, neighbour_container_wrapper.change_counter ) )
          create_neighbour_information<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( const_cast<mesh_type&>(mesh) );


      return neighbour_elements<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( viennagrid::make_accessor<element_type>(neighbour_container_wrapper.container), viennagrid::dereference_handle(mesh, element_or_handle) );
  }


  /** @brief Obtaines a neighbour range of an element within a segment. This function caches the neighbour information and re-creates it if the cached information is out of date. The worst case runtime of a re-creation is linear in the number of elements of type ConnectorElementTypeOrTagT time the number of elements of type ElementTypeOrTagT within the segment.
    *
    * @tparam ElementTypeOrTagT             The base element type/tag from which the neighbour range is obtained
    * @tparam ConnectorElementTypeOrTagT    The connector element type/tag
    * @tparam SegmentationT                 The segmentation type of the segment type
    * @tparam ElementOrHandleT              The element or handle type for which the neighbour range is obtained
    * @param  segment                       The host segment object in which the elements of the neighbour range exist
    * @param  handle_or_element             A handle or the element for which the neighbour range is obained
    * @return                               The neighbour range
    */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename SegmentationT, typename ElementOrHandleT>
  typename result_of::neighbour_range<segment_handle_t<SegmentationT>, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type
  neighbour_elements(segment_handle_t<SegmentationT> & segment, ElementOrHandleT const & element_or_handle)
  {
      return neighbour_elements<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( segment.view(), element_or_handle );
  }

  /** @brief Obtaines a const neighbour range of an element within a segment. This function caches the neighbour information and re-creates it if the cached information is out of date. The worst case runtime of a re-creation is linear in the number of elements of type ConnectorElementTypeOrTagT time the number of elements of type ElementTypeOrTagT within the segment.
    *
    * @tparam ElementTypeOrTagT             The base element type/tag from which the neighbour range is obtained
    * @tparam ConnectorElementTypeOrTagT    The connector element type/tag
    * @tparam SegmentationT                 The segmentation type of the segment type
    * @tparam ElementOrHandleT              The element or handle type for which the neighbour range is obtained
    * @param  segment                       The host segment object in which the elements of the neighbour range exist
    * @param  handle_or_element             A handle or the element for which the neighbour range is obained
    * @return                               The const neighbour range
    */
  template<typename ElementTypeOrTagT, typename ConnectorElementTypeOrTagT, typename SegmentationT, typename ElementOrHandleT>
  typename result_of::neighbour_range<segment_handle_t<SegmentationT>, ElementTypeOrTagT, ConnectorElementTypeOrTagT>::type
  neighbour_elements(segment_handle_t<SegmentationT> const & segment, ElementOrHandleT const & element_or_handle)
  {
      return neighbour_elements<ElementTypeOrTagT, ConnectorElementTypeOrTagT>( segment.view(), element_or_handle );
  }

}


#endif
