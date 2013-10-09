#ifndef VIENNAGRID_CONFIG_TOPOLOGY_CONFIG_HPP
#define VIENNAGRID_CONFIG_TOPOLOGY_CONFIG_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/config/config.hpp"
#include "viennagrid/config/id_generator_config.hpp"


#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/config/element_config.hpp"

/** @file config/simplex.hpp
    @brief Provides default configuration classes for simplex meshs
*/

namespace viennagrid
{
  namespace config
  {
    namespace result_of
    {
      //
      // Meta Functions for creating a default config
      //

      template<typename ElementTagT, typename boundary_cell_tag>
      struct storage_layout_config
      {
        typedef typename viennagrid::meta::typemap::result_of::insert<
            typename storage_layout_config<ElementTagT, typename boundary_cell_tag::facet_tag>::type,
            viennagrid::meta::static_pair<
                boundary_cell_tag,
                viennagrid::full_handling_tag
            >
        >::type type;
      };

      template<typename ElementTagT>
      struct storage_layout_config<ElementTagT, viennagrid::vertex_tag>
      {
        typedef typename viennagrid::meta::make_typemap<
            viennagrid::vertex_tag,
            viennagrid::no_orientation_handling_tag
        >::type type;
      };

      template<typename ElementTagT>
      struct storage_layout_config<ElementTagT, viennagrid::meta::null_type>
      {
        typedef viennagrid::meta::null_type type;
      };


      template<typename ElementTagT, typename boundary_cell_tag, typename VertexContainerT, typename CellContainerT>
      struct default_container_tag
      {
        typedef viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag > type;
      };

      template<typename ElementTagT, typename VertexContainerT, typename CellContainerT>
      struct default_container_tag<ElementTagT, ElementTagT, VertexContainerT, CellContainerT>
      {
        typedef CellContainerT type;
      };

      template<typename ElementTagT, typename VertexContainerT, typename CellContainerT>
      struct default_container_tag<ElementTagT, viennagrid::vertex_tag, VertexContainerT, CellContainerT>
      {
        typedef VertexContainerT type;
      };

      template<typename VertexContainerT, typename CellContainerT>
      struct default_container_tag<viennagrid::vertex_tag, viennagrid::vertex_tag, VertexContainerT, CellContainerT>
      {
        typedef VertexContainerT type;
      };


      template<typename ElementTagT, typename boundary_cell_tag, typename handle_tag, typename VertexContainerT, typename CellContainerT>
      struct full_element_config
      {
        typedef typename viennagrid::storage::result_of::handled_container<typename default_container_tag<ElementTagT, boundary_cell_tag, VertexContainerT, CellContainerT>::type,
                                                                            handle_tag>::tag                     container_tag;

        typedef typename storage_layout_config<ElementTagT,
                                                typename boundary_cell_tag::facet_tag>::type   boundary_storage_layout;

        typedef typename viennagrid::meta::make_typemap<
            viennagrid::config::element_id_tag,
            viennagrid::storage::smart_id_tag<int>,

            viennagrid::config::element_container_tag,
            container_tag,

            viennagrid::config::element_boundary_storage_layout_tag,
            boundary_storage_layout,

            viennagrid::config::element_appendix_type_tag,
            viennagrid::meta::null_type
        >::type type;
      };

      template<typename ElementTagT, typename handle_tag, typename VertexContainerT, typename CellContainerT>
      struct full_element_config<ElementTagT, viennagrid::vertex_tag, handle_tag, VertexContainerT, CellContainerT>
      {
        typedef typename viennagrid::storage::result_of::handled_container<typename default_container_tag<ElementTagT, viennagrid::vertex_tag, VertexContainerT, CellContainerT>::type,
                                                                            handle_tag>::tag                     container_tag;

        typedef typename viennagrid::meta::make_typemap<
            viennagrid::config::element_id_tag,
            viennagrid::storage::smart_id_tag<int>,

            viennagrid::config::element_container_tag,
            container_tag,

            viennagrid::config::element_boundary_storage_layout_tag,
            viennagrid::meta::null_type,

            viennagrid::config::element_appendix_type_tag,
            viennagrid::meta::null_type
        >::type type;
      };




      template<typename ElementTagT, typename boundary_cell_tag, typename handle_tag, typename VertexContainerT, typename CellContainerT>
      struct full_topology_config_helper
      {
        typedef typename viennagrid::meta::typemap::result_of::insert<
            typename full_topology_config_helper<ElementTagT, typename boundary_cell_tag::facet_tag, handle_tag, VertexContainerT, CellContainerT>::type,
            viennagrid::meta::static_pair<
                boundary_cell_tag,
                typename full_element_config<ElementTagT, boundary_cell_tag, handle_tag, VertexContainerT, CellContainerT>::type
            >
        >::type type;
      };

      template<typename ElementTagT, typename handle_tag, typename VertexContainerT, typename CellContainerT>
      struct full_topology_config_helper<ElementTagT, viennagrid::vertex_tag, handle_tag, VertexContainerT, CellContainerT>
      {
        typedef typename viennagrid::meta::make_typemap<
            viennagrid::vertex_tag,
            typename full_element_config<ElementTagT, viennagrid::vertex_tag, handle_tag, VertexContainerT, CellContainerT>::type
        >::type type;
      };


      template<typename ElementTagT, typename handle_tag, typename VertexContainerT, typename CellContainerT>
      struct full_topology_config
      {
//         typedef typename viennagrid::meta::make_typemap<
//           topology_config_tag,
//           typename full_topology_config_helper<ElementTagT, ElementTagT, handle_tag, VertexContainerT, CellContainerT>::type
//         >::type type;

        typedef typename full_topology_config_helper<ElementTagT, ElementTagT, handle_tag, VertexContainerT, CellContainerT>::type type;
      };



      template<typename cell_tag,
                typename vector_type,
                typename handle_tag = viennagrid::storage::pointer_handle_tag,
                typename VertexContainerT = viennagrid::storage::std_deque_tag,
                typename CellContainerT = viennagrid::storage::std_deque_tag>
      struct full_mesh_config
      {
        typedef typename full_topology_config<cell_tag, handle_tag, VertexContainerT, CellContainerT>::type MeshConfig;
        typedef typename query<MeshConfig, meta::null_type, vertex_tag>::type VertexConfig;

        typedef typename viennagrid::meta::typemap::result_of::insert_or_modify<
            MeshConfig,
            viennagrid::meta::static_pair<
                vertex_tag,
                typename viennagrid::meta::typemap::result_of::insert_or_modify<

                    VertexConfig,
                    viennagrid::meta::static_pair<
                        element_appendix_type_tag,
                        vector_type
                    >

                >::type
            >
        >::type type;
      };

      template<typename cell_tag, typename handle_tag, typename VertexContainerT, typename CellContainerT>
      struct full_mesh_config<cell_tag, void, handle_tag, VertexContainerT, CellContainerT>
      {
        typedef typename viennagrid::config::result_of::full_topology_config<cell_tag, handle_tag, VertexContainerT, CellContainerT>::type type;
      };






      //
      // generates a container for a specified ElementTagT for the mesh container collection
      //
      template<typename WrappedConfigType, typename ElementTagT>
      struct element_container
      {
        typedef element_t<ElementTagT, WrappedConfigType> element_type;
        typedef typename viennagrid::storage::result_of::container<element_type, typename query_element_container_tag<WrappedConfigType, ElementTagT>::type >::type type;
      };

      template<typename container_collection_typemap, typename ElementTagT>
      struct element_container< storage::collection_t<container_collection_typemap>, ElementTagT >
      {
        typedef typename viennagrid::result_of::element<storage::collection_t<container_collection_typemap>, ElementTagT>::type element_type;
        typedef typename viennagrid::meta::typemap::result_of::find< container_collection_typemap, element_type >::type::second type;
      };


      //
      // generates the container typelist for the mesh container collection
      //
      template<typename WrappedConfigT, typename CurrentConfigT = typename WrappedConfigT::type>
      struct element_container_typemap;

      template<typename WrappedConfigT, typename ElementTagT, typename ValueConfigT, typename TailT>
      struct element_container_typemap< WrappedConfigT, viennagrid::meta::typelist_t< viennagrid::meta::static_pair<ElementTagT, ValueConfigT>, TailT > >
      {
        typedef viennagrid::meta::typelist_t<
            viennagrid::meta::static_pair<
                element_t<ElementTagT, WrappedConfigT>,
                typename element_container<WrappedConfigT, ElementTagT>::type
            >,
            typename element_container_typemap<WrappedConfigT, TailT>::type
        > type;
      };

      template<typename WrappedConfigT>
      struct element_container_typemap<WrappedConfigT, viennagrid::meta::null_type>
      {
          typedef viennagrid::meta::null_type type;
      };
    }



    template<typename cell_tag,
              typename vector_type,
              typename handle_tag,
              typename VertexContainerT,
              typename CellContainerT>
    struct wrapped_mesh_config_t
    {
      typedef typename result_of::full_mesh_config<cell_tag, vector_type, handle_tag, VertexContainerT, CellContainerT>::type type;
    };
  }

  namespace result_of
  {
    template<typename WrappedConfigType, typename ElementTagT, typename boundary_ElementTagTlist =
      typename boundary_element_taglist< element_t<ElementTagT, WrappedConfigType> >::type >
    struct coboundary_container_collection_per_element_typemap;

    template<typename WrappedConfigType, typename ElementTagT>
    struct coboundary_container_collection_per_element_typemap<WrappedConfigType, ElementTagT, viennagrid::meta::null_type>
    {
        typedef viennagrid::meta::null_type type;
    };

    template<typename WrappedConfigType, typename ElementTagT, typename boundary_ElementTagT, typename tail>
    struct coboundary_container_collection_per_element_typemap<WrappedConfigType, ElementTagT, viennagrid::meta::typelist_t<boundary_ElementTagT, tail> >
    {
      typedef typename config::result_of::query<typename WrappedConfigType::type, long, config::mesh_change_counter_tag>::type mesh_change_counter_type;

      typedef typename config::result_of::query<typename WrappedConfigType::type, storage::std_vector_tag, ElementTagT, config::coboundary_container_tag, boundary_ElementTagT>::type coboundary_container_tag;
      typedef typename config::result_of::query<typename WrappedConfigType::type, storage::std_vector_tag, ElementTagT, config::coboundary_view_container_tag, boundary_ElementTagT>::type coboundary_view_container_tag;


      typedef typename config::result_of::element_container< WrappedConfigType, ElementTagT>::type base_element_container;
      typedef typename viennagrid::storage::result_of::view<base_element_container, coboundary_view_container_tag>::type element_view_type;
      typedef typename storage::result_of::container<element_view_type, coboundary_container_tag >::type base_coboundary_container;

      typedef viennagrid::meta::typelist_t<
          viennagrid::meta::static_pair<
              viennagrid::meta::static_pair<
                  boundary_ElementTagT,
                  ElementTagT
              >,
              coboundary_container_wrapper<base_coboundary_container, mesh_change_counter_type>
          >,
          typename coboundary_container_collection_per_element_typemap<WrappedConfigType, ElementTagT, tail>::type
      > type;
    };


    template<typename WrappedConfigType, typename ElementTaglistT = typename viennagrid::meta::typemap::result_of::key_typelist<typename WrappedConfigType::type>::type>
    struct coboundary_container_collection_typemap;

    template<typename WrappedConfigType>
    struct coboundary_container_collection_typemap<WrappedConfigType, viennagrid::meta::null_type>
    {
      typedef viennagrid::meta::null_type type;
    };

    template<typename WrappedConfigType, typename ElementTagT, typename tail>
    struct coboundary_container_collection_typemap<WrappedConfigType, viennagrid::meta::typelist_t<ElementTagT, tail> >
    {
      typedef typename viennagrid::meta::typemap::result_of::merge<
        typename coboundary_container_collection_per_element_typemap<WrappedConfigType, ElementTagT>::type,
        typename coboundary_container_collection_typemap<WrappedConfigType, tail>::type
      >::type type;
    };
  }




  namespace result_of
  {
    template<typename WrappedConfigType, typename ElementTagT, typename BoundaryElementTaglistT =
      typename boundary_element_taglist< element_t<ElementTagT, WrappedConfigType> >::type >
    struct neighbour_container_collection_per_element_typemap;

    template<typename WrappedConfigType, typename ElementTagT>
    struct neighbour_container_collection_per_element_typemap<WrappedConfigType, ElementTagT, viennagrid::meta::null_type>
    {
      typedef viennagrid::meta::null_type type;
    };

    template<typename WrappedConfigType, typename ElementTagT, typename ConnectorElementTagT, typename tail>
    struct neighbour_container_collection_per_element_typemap<WrappedConfigType, ElementTagT, viennagrid::meta::typelist_t<ConnectorElementTagT, tail> >
    {
      typedef typename config::result_of::query<typename WrappedConfigType::type, long, config::mesh_change_counter_tag>::type mesh_change_counter_type;

      typedef typename config::result_of::query<typename WrappedConfigType::type, storage::std_vector_tag, ElementTagT, config::neighbour_container_tag, ConnectorElementTagT>::type neighbour_container_tag;
      typedef typename config::result_of::query<typename WrappedConfigType::type, storage::std_vector_tag, ElementTagT, config::neighbour_view_container_tag, ConnectorElementTagT>::type neighbour_view_container_tag;

      typedef typename config::result_of::element_container< WrappedConfigType, ElementTagT>::type base_element_container;
      typedef typename viennagrid::storage::result_of::view<base_element_container, neighbour_view_container_tag>::type element_view_type;
      typedef typename storage::result_of::container<element_view_type, neighbour_container_tag >::type base_container;

      typedef viennagrid::meta::typelist_t<
          viennagrid::meta::static_pair<
              viennagrid::meta::static_pair<
                  ElementTagT,
                  ConnectorElementTagT
              >,
              neighbour_container_wrapper<base_container, mesh_change_counter_type>
          >,
          typename neighbour_container_collection_per_element_typemap<WrappedConfigType, ElementTagT, tail>::type
      > type;
    };


    template<typename WrappedConfigType, typename ElementTaglistT = typename viennagrid::meta::typemap::result_of::key_typelist<typename WrappedConfigType::type>::type>
    struct neighbour_container_collection_typemap;

    template<typename WrappedConfigType>
    struct neighbour_container_collection_typemap<WrappedConfigType, viennagrid::meta::null_type>
    {
      typedef viennagrid::meta::null_type type;
    };

    template<typename WrappedConfigType, typename ElementTagT, typename tail>
    struct neighbour_container_collection_typemap<WrappedConfigType, viennagrid::meta::typelist_t<ElementTagT, tail> >
    {
      typedef typename viennagrid::meta::typemap::result_of::merge<
        typename neighbour_container_collection_per_element_typemap<WrappedConfigType, ElementTagT>::type,
        typename neighbour_container_collection_typemap<WrappedConfigType, tail>::type
      >::type type;
    };
  }


  namespace result_of
  {
    template<typename typelist>
    struct topologic_cell_dimension;

    template<>
    struct topologic_cell_dimension<viennagrid::meta::null_type>
    {
      static const int value = -1;
    };

    template<typename ElementTagT, typename tail>
    struct topologic_cell_dimension< viennagrid::meta::typelist_t<ElementTagT, tail> >
    {
      static const int tail_cell_dimension = topologic_cell_dimension<tail>::value;
      static const int current_element_dimension = ElementTagT::dim;

      static const int value = (tail_cell_dimension > current_element_dimension) ? tail_cell_dimension : current_element_dimension;
    };




    template<typename typelist, int topologic_dimension>
    struct elements_of_topologic_dim;

    template<int topologic_dimension>
    struct elements_of_topologic_dim< viennagrid::meta::null_type, topologic_dimension >
    {
      typedef viennagrid::meta::null_type type;
    };

    template<typename ElementTagT, typename tail, int topologic_dimension>
    struct elements_of_topologic_dim< viennagrid::meta::typelist_t<ElementTagT, tail>, topologic_dimension >
    {
      typedef typename elements_of_topologic_dim<tail, topologic_dimension>::type tail_typelist;

      typedef typename viennagrid::meta::IF<
          ElementTagT::dim == topologic_dimension,
          typename viennagrid::meta::typelist::result_of::push_back<tail_typelist, ElementTagT>::type,
          tail_typelist
      >::type type;
    };


    template<typename typelist>
    struct cell_tag_from_typelist
    {
      static const int dim = topologic_cell_dimension<typelist>::value;
      typedef typename elements_of_topologic_dim<typelist, dim>::type cell_types;
      typedef typename viennagrid::meta::typelist::result_of::at<cell_types,0>::type type;
    };






    template<typename WrappedConfigType, typename ElementTagTlist>
    struct boundary_information_collection_typemap_impl;

    template<typename WrappedConfigType>
    struct boundary_information_collection_typemap_impl<WrappedConfigType, viennagrid::meta::null_type>
    {
      typedef viennagrid::meta::null_type type;
    };

    template<typename WrappedConfigType, typename ElementTagT, typename tail>
    struct boundary_information_collection_typemap_impl<WrappedConfigType, viennagrid::meta::typelist_t<ElementTagT, tail> >
    {
      typedef typename config::result_of::query<typename WrappedConfigType::type, long, config::mesh_change_counter_tag>::type mesh_change_counter_type;

      typedef typename config::result_of::query<typename WrappedConfigType::type, storage::std_vector_tag, ElementTagT, config::boundary_information_container_tag>::type boundary_container_tag;
      typedef typename storage::result_of::container<bool, boundary_container_tag >::type base_container;

      typedef viennagrid::meta::typelist_t<
          viennagrid::meta::static_pair<
              ElementTagT,
              boundary_information_wrapper<base_container, mesh_change_counter_type>
          >,
          typename boundary_information_collection_typemap_impl<WrappedConfigType, tail>::type
      > type;
    };


    template<typename WrappedConfigType>
    struct boundary_information_collection_typemap
    {
      typedef typename viennagrid::meta::typemap::result_of::key_typelist<typename WrappedConfigType::type>::type ElementTagTlist;

      typedef typename cell_tag_from_typelist<ElementTagTlist>::type cell_tag;
      typedef typename viennagrid::meta::typelist::result_of::erase< ElementTagTlist, cell_tag>::type element_typelist_without_cell_tag;


      typedef typename boundary_information_collection_typemap_impl<WrappedConfigType, element_typelist_without_cell_tag>::type type;
    };
  }



  namespace config
  {
    namespace result_of
    {
      template<typename WrappedConfigType>
      struct element_collection
      {
        typedef typename config::result_of::element_container_typemap<WrappedConfigType>::type element_container_typelist;
        typedef typename viennagrid::storage::result_of::collection< element_container_typelist >::type type;
      };
    }
  }
}


#endif
