#ifndef VIENNAGRID_DOMAIN_HPP
#define VIENNAGRID_DOMAIN_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at

   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/config/topology_config.hpp"
#include "viennagrid/domain/topology.hpp"
#include "viennagrid/element/element_view.hpp"

namespace viennagrid
{

    namespace result_of
    {
        template<typename point_container_type>
        struct point {};

        template<typename ConfigType>
        struct point< domain_t<ConfigType> >
        {
            typedef typename viennagrid::result_of::vertex< domain_t<ConfigType> >::type::appendix_type type;
        };

        template<typename ConfigType>
        struct point< const domain_t<ConfigType> >
        {
            typedef typename viennagrid::result_of::vertex< domain_t<ConfigType> >::type::appendix_type type;
        };

        template<typename ElementTag, typename WrappedConfigType>
        struct point< element_t<ElementTag, WrappedConfigType> >
        {
            typedef typename viennagrid::result_of::vertex< element_t<ElementTag, WrappedConfigType> >::type::appendix_type type;
        };

        template<typename ElementTag, typename WrappedConfigType>
        struct point< const element_t<ElementTag, WrappedConfigType> >
        {
            typedef typename viennagrid::result_of::vertex< element_t<ElementTag, WrappedConfigType> >::type::appendix_type type;
        };
    }




    template<typename ConfigType>
    typename result_of::point< domain_t<ConfigType> >::type & point(domain_t<ConfigType> &, typename result_of::vertex< domain_t<ConfigType> >::type & vertex)
    { return vertex.appendix(); }

    template<typename ConfigType>
    typename result_of::point< domain_t<ConfigType> >::type const & point(domain_t<ConfigType> const &, typename result_of::vertex< domain_t<ConfigType> >::type const & vertex)
    { return vertex.appendix(); }


    template<typename ConfigType>
    typename result_of::point< domain_t<ConfigType> >::type & point(domain_t<ConfigType> & domain, typename result_of::vertex_handle< domain_t<ConfigType> >::type vertex_handle)
    { return dereference_handle(domain, vertex_handle).appendix(); }

    template<typename ConfigType>
    typename result_of::point< domain_t<ConfigType> >::type const & point(domain_t<ConfigType> const & domain, typename result_of::const_vertex_handle< domain_t<ConfigType> >::type vertex_handle)
    { return dereference_handle(domain, vertex_handle).appendix(); }



    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_by_id(domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::iterator<RangeType>::type RangeIterator;

        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( viennagrid::dereference_handle(domain, it.handle()).id() == id )
                return it;
        }

        return range.end();
    }

    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::const_element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_by_id(const domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::const_iterator<RangeType>::type RangeIterator;

        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( viennagrid::dereference_handle(domain, it.handle()).id() == id )
                return it;
        }

        return range.end();
    }

    template<typename domain_type, typename handle_type>
    typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<domain_type, typename storage::handle::result_of::value_type<handle_type>::type >::type >::type
            find_by_handle(domain_type & domain, handle_type handle)
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::iterator<RangeType>::type RangeIterator;

        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( it.handle() == handle )
                return it;
        }

        return range.end();
    }

    template<typename domain_type, typename handle_type>
    typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::const_element_range<domain_type, typename storage::handle::result_of::value_type<handle_type>::type >::type  >::type
            find_by_handle(domain_type const & domain, handle_type handle)
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::const_iterator<RangeType>::type RangeIterator;

        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( it.handle() == handle )
                return it;
        }

        return range.end();
    }

}

#endif
