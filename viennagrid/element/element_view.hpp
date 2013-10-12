#ifndef VIENNAGRID_ELEMENT_VIEW_HPP
#define VIENNAGRID_ELEMENT_VIEW_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include "viennagrid/element/element.hpp"
#include "viennagrid/mesh/mesh.hpp"

/** @file viennagrid/element/element_view.hpp
    @brief Provides routines for extracting the sub-elements of an element (e.g. all edges of a triangle)
*/

namespace viennagrid
{
  namespace result_of
  {

    template <typename something,
             typename element_type_or_tag,
             typename view_container_tag = storage::std_deque_tag>
    struct element_view
    {
      typedef typename element_range<something, element_type_or_tag>::type::base_container_type base_container_type;
      typedef typename storage::result_of::view<base_container_type, view_container_tag>::type type;
    };

    template <typename something,
             typename element_type_or_tag,
             typename view_container_tag = storage::std_deque_tag>
    struct const_element_view
    {
      typedef typename const_element_range<something, element_type_or_tag>::type::base_container_type base_container_type;
      typedef typename storage::result_of::view<const base_container_type, view_container_tag>::type type;
    };

  }


  template<typename element_type_or_tag, typename something>
  typename result_of::element_view<something, element_type_or_tag>::type
  element_view( something & s )
  {
    typedef typename result_of::element_tag<element_type_or_tag>::type element_tag;
    typedef typename result_of::element<something, element_tag>::type element_type;

    typename result_of::element_view<something, element_tag>::type view;
    view.set_base_container( viennagrid::storage::collection::get<element_type>( element_collection(s) ) );

    return view;
  }


  template<typename element_type_or_tag, typename something, typename functor>
  typename result_of::element_view<something, element_type_or_tag>::type
  element_view( something & s, functor f )
  {
    typedef typename result_of::element_tag<element_type_or_tag>::type element_tag;

    typedef typename result_of::element_range<something, element_tag>::type RangeType;
    typedef typename result_of::iterator<RangeType>::type IteratorType;

    RangeType range = viennagrid::elements<element_tag>(s);

    typename result_of::element_view<something, element_tag>::type view;
    view.set_base_container( *range.get_base_container() );

    for ( IteratorType it = range.begin(); it != range.end(); ++it )
    {
      if ( f(*it) )
        view.insert_handle( it.handle() );
    }

    return view;
  }

  template<typename element_type_or_tag, typename something, typename functor>
  typename result_of::const_element_view<something, element_type_or_tag>::type
  element_view( something const & s, functor f )
  {
    typedef typename result_of::element_tag<element_type_or_tag>::type element_tag;

    typedef typename result_of::const_element_range<something, element_tag>::type RangeType;
    typedef typename result_of::iterator<RangeType>::type IteratorType;

    RangeType range = viennagrid::elements<element_tag>(s);

    typename result_of::element_view<something, element_tag>::type view;
    view.set_base_container( *range.get_base_container() );

    for ( IteratorType it = range.begin(); it != range.end(); ++it )
    {
      if ( f(*it) )
        view.insert_handle( it.handle() );
    }

    return view;
  }

}


#endif
