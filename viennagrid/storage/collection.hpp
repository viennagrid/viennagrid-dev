#ifndef VIENNAGRID_STORAGE_COLLECTION_HPP
#define VIENNAGRID_STORAGE_COLLECTION_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/meta/utils.hpp"
#include "viennagrid/meta/typemap.hpp"

/** @file viennagrid/storage/collection.hpp
    @brief Generic implementation of a collection consisting of compile-time dispatched keys and values
*/

namespace viennagrid
{
  namespace storage
  {

    template<typename typemap>
    class collection_layer;

    template<typename typemap_>
    class collection_t : public collection_layer< typemap_ >
    {
    public:
      typedef typemap_ typemap;
      typedef collection_layer< typemap > base;

      using base::get;
    private:
    };

    template<typename key_, typename value_, typename tail>
    class collection_layer< viennagrid::typelist< viennagrid::static_pair<key_, value_> , tail> > : public collection_layer<tail>
    {
    public:
      typedef collection_layer< tail > base;
      typedef key_ key_type;
      typedef value_ value_type;

      using base::get;
      value_type & get( viennagrid::meta::tag<key_type> ) { return element; }
      const value_type & get( viennagrid::meta::tag<key_type> ) const { return element; }

    private:
      value_type element;
    };

    template<>
    class collection_layer< viennagrid::null_type >
    {
    public:
      void get();
    };


    namespace result_of
    {
      template<typename typemap, typename key_type>
      struct value_type
      {
        typedef typename viennagrid::meta::result_of::find<typemap, key_type>::type::second type;
      };

      template<typename typemap, typename key_type>
      struct value_type< collection_t<typemap>, key_type >
      {
        typedef typename viennagrid::meta::result_of::find<typemap, key_type>::type::second type;
      };
    }


    namespace collection
    {
      template<typename type, typename typemap>
      typename result_of::value_type<typemap, typename viennagrid::meta::remove_const<type>::type >::type & get( collection_t<typemap> & c )
      {
        return c.get( viennagrid::meta::tag< typename viennagrid::meta::remove_const<type>::type >() );
      }

      template<typename type, typename typemap>
      typename result_of::value_type<typemap, typename viennagrid::meta::remove_const<type>::type >::type const & get( collection_t<typemap> const & c )
      {
        return c.get( viennagrid::meta::tag< typename viennagrid::meta::remove_const<type>::type >() );
      }
    } // namespace collection


    namespace result_of
    {
      template<typename typemap>
      struct collection
      {
          typedef collection_t<typemap> type;
      };
    } // namespace result_of

  } // namespace storge
} // namespace viennagrid

#endif
