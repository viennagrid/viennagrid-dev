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
  namespace detail
  {

    template<typename typemap>
    class collection_layer;



    template<typename key_, typename value_, typename tail>
    class collection_layer< viennagrid::typelist< viennagrid::static_pair<key_, value_> , tail> > : public collection_layer<tail>
    {
    public:
      typedef collection_layer< tail > base;
      typedef key_ key_type;
      typedef value_ value_type;

      using base::get;
      value_type & get( viennagrid::detail::tag<key_type> ) { return element; }
      const value_type & get( viennagrid::detail::tag<key_type> ) const { return element; }

    private:
      value_type element;
    };

    template<>
    class collection_layer< viennagrid::null_type >
    {
    public:
      void get();
    };

  }

  /** @brief A collection is a compile-time map which returns objects of (possibly distinct) type based on the provided key type.
    *
    * For example, collection may return objects of type std::deque<VertexType>, std::deque<EdgeType>, or std::vector<TriangleType> based on the tag provided.
    */
  template<typename typemap_>
  class collection : public detail::collection_layer< typemap_ >
  {
  public:
    typedef typemap_ typemap;
    typedef detail::collection_layer< typemap > base;

    using base::get;
  private:
  };

//   namespace result_of
//   {
//     template<typename typemap>
//     struct collection
//     {
//         typedef viennagrid::collection<typemap> type;
//     };
//   } // namespace result_of


  namespace detail
  {
    namespace result_of
    {
      template<typename typemap, typename key_type>
      struct lookup< viennagrid::collection<typemap>, key_type >
      {
        typedef typename viennagrid::detail::result_of::lookup<typemap, key_type>::type type;
      };
    }
  }


  template<typename type, typename typemap>
  typename viennagrid::detail::result_of::lookup<typemap, typename viennagrid::detail::remove_const<type>::type >::type & get( collection<typemap> & c )
  {
    return c.get( viennagrid::detail::tag< typename viennagrid::detail::remove_const<type>::type >() );
  }

  template<typename type, typename typemap>
  typename viennagrid::detail::result_of::lookup<typemap, typename viennagrid::detail::remove_const<type>::type >::type const & get( collection<typemap> const & c )
  {
    return c.get( viennagrid::detail::tag< typename viennagrid::detail::remove_const<type>::type >() );
  }


} // namespace viennagrid

#endif
