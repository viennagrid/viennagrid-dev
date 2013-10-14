#ifndef VIENNAGRID_STORAGE_CONTAINER_COLLECTION_HPP
#define VIENNAGRID_STORAGE_CONTAINER_COLLECTION_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/meta/algorithm.hpp"

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/collection.hpp"
#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/range.hpp"

/** @file viennagrid/storage/container_collection.hpp
    @brief Routines for the interaction of containers and collections
*/

namespace viennagrid
{
  namespace storage
  {

    namespace detail
    {

      namespace result_of
      {

        //
        // generates a typemap with value type and container type from a typelist and a container config
        //

        template<typename value_type, typename container_config>
        struct container_from_value_using_container_config
        {
          typedef typename viennagrid::meta::result_of::find<container_config, value_type>::type search_result;
          typedef typename viennagrid::meta::result_of::find<container_config, viennagrid::storage::default_tag>::type default_container;

          typedef typename viennagrid::meta::IF<
              !viennagrid::meta::EQUAL<search_result, viennagrid::not_found>::value,
              search_result,
              default_container
          >::type container_tag_pair;

          typedef typename viennagrid::storage::result_of::container<value_type, typename container_tag_pair::second>::type type;
        };


        template<typename element_list, typename container_config>
        struct container_list_from_value_typelist_using_container_config;

        template<typename container_config>
        struct container_list_from_value_typelist_using_container_config<viennagrid::null_type, container_config>
        {
          typedef viennagrid::null_type type;
        };

        template<typename value_type, typename tail, typename container_config>
        struct container_list_from_value_typelist_using_container_config<viennagrid::typelist<value_type, tail>, container_config>
        {
          typedef viennagrid::typelist<
              typename viennagrid::static_pair<
                      value_type,
                      typename container_from_value_using_container_config<value_type, container_config>::type
                  >,
              typename container_list_from_value_typelist_using_container_config<tail, container_config>::type
          > type;
        };


      } // namespace result_of

    } // namespace detail


    namespace result_of
    {

      template<typename typemap_, typename element_type>
      struct container_of
      {
        typedef typename viennagrid::meta::result_of::second<
            typename viennagrid::meta::result_of::find< typemap_, element_type >::type
        >::type type;
      };

      template<typename typemap_, typename element_type>
      struct container_of< viennagrid::storage::collection< typemap_ >, element_type >
      {
        typedef typename container_of<typemap_, element_type>::type type;
      };


      template<typename container_collection_1, typename container_collection_2>
      struct common_values;

      template<typename container_typelist_1, typename container_typelist_2>
      struct common_values< viennagrid::storage::collection<container_typelist_1>,
                            viennagrid::storage::collection<container_typelist_2> >
      {
        typedef viennagrid::storage::collection<container_typelist_1> from_container_collection_type;
        typedef viennagrid::storage::collection<container_typelist_2> to_container_collection_type;

        typedef typename viennagrid::meta::result_of::key_typelist<typename from_container_collection_type::typemap>::type from_container_collection_value_typelist;
        typedef typename viennagrid::meta::result_of::key_typelist<typename to_container_collection_type::typemap>::type to_container_collection_value_typelist;

        typedef typename viennagrid::meta::result_of::intersection<
            from_container_collection_value_typelist,
            to_container_collection_value_typelist
        >::type type;

      };

    } // namespace result_of



    namespace detail
    {
      typedef viennagrid::make_typemap<
                  viennagrid::storage::default_tag,   viennagrid::storage::handled_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::pointer_handle_tag>
              >::type default_container_config;


      template<typename container_collection_type, typename element_type, typename search_result>
      struct insert_or_ignore_helper
      {
        static void insert_or_ignore( container_collection_type & collection, const element_type & element )
        {
          collection.get( viennagrid::meta::tag<element_type>() ).insert(element);
        }

        static void insert_or_ignore( container_collection_type & collection, element_type & element )
        {
          collection.get( viennagrid::meta::tag<element_type>() ).insert(element);
        }
      };

      template<typename container_collection_type, typename element_type>
      struct insert_or_ignore_helper<container_collection_type, element_type, viennagrid::not_found>
      {
        static void insert_or_ignore( container_collection_type &, const element_type & ) {}

        static void insert_or_ignore( container_collection_type &, element_type & ) {}
      };


      template<typename container_collection_type, typename element_type>
      void insert_or_ignore( container_collection_type & collection, const element_type & element)
      {
        typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
        insert_or_ignore_helper<container_collection_type, element_type, container_type>::insert_or_ignore(collection, element);
      }

      template<typename container_collection_type, typename element_type>
      void insert_or_ignore( container_collection_type & collection, element_type & element)
      {
        typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
        insert_or_ignore_helper<container_collection_type, element_type, container_type>::insert_or_ignore(collection, element);
      }





      template<typename container_collection_type, typename handle_type, typename container_type>
      struct handle_or_ignore_helper
      {
        typedef typename viennagrid::storage::detail::result_of::value_type<handle_type>::type value_type;

        static void handle_or_ignore( container_collection_type & collection, const handle_type & handle )
        {
          collection.get( viennagrid::meta::tag<value_type>() ).insert_unique_handle(handle);
        }

        static void handle_or_ignore( container_collection_type & collection, handle_type & handle )
        {
          collection.get( viennagrid::meta::tag<value_type>() ).insert_unique_handle(handle);
        }
      };

      template<typename container_collection_type, typename handle_type>
      struct handle_or_ignore_helper<container_collection_type, handle_type, viennagrid::not_found>
      {
        static void handle_or_ignore( container_collection_type &, const handle_type & ) {}

        static void handle_or_ignore( container_collection_type &, handle_type & ) {}
      };


      template<typename container_collection_type, typename handle_type, typename element_type>
      void handle_or_ignore( container_collection_type & collection, const handle_type & handle, viennagrid::meta::tag<element_type> )
      {
        typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
        handle_or_ignore_helper<container_collection_type, handle_type, container_type>::handle_or_ignore(collection, handle);
      }

      template<typename container_collection_type, typename handle_type, typename element_type>
      void handle_or_ignore( container_collection_type & collection, handle_type & handle, viennagrid::meta::tag<element_type> )
      {
        typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
        handle_or_ignore_helper<container_collection_type, handle_type, container_type>::handle_or_ignore(collection, handle);
      }




      template<typename container_collection_type>
      struct clear_all_functor
      {
        clear_all_functor( container_collection_type & container_collection_ ) : container_collection(container_collection_) {}

        template<typename type>
        void operator() ( viennagrid::meta::tag<type> )
        {
          viennagrid::storage::detail::get<type>( container_collection ).clear();
        }

        container_collection_type & container_collection;
      };


      template<typename container_collection_typemap>
      void clear_all( viennagrid::storage::collection<container_collection_typemap> & container_collection)
      {
        clear_all_functor< viennagrid::storage::collection<container_collection_typemap> > f( container_collection );
        viennagrid::meta::for_each< typename viennagrid::meta::result_of::key_typelist<container_collection_typemap>::type >( f );
      }



    } // namespace detail




    namespace result_of
    {
      template<typename value_typelist, typename container_config>
      struct container_collection
      {
        typedef viennagrid::storage::collection<
            typename viennagrid::storage::detail::result_of::container_list_from_value_typelist_using_container_config<
                value_typelist,
                container_config
            >::type
        > type;
      };
    }

  } // namespace storage

} // namespace viennagrid

#endif
