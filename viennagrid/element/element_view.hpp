#ifndef VIENNAGRID_ELEMENT_VIEW_HPP
#define VIENNAGRID_ELEMENT_VIEW_HPP

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


#include "viennagrid/element/element.hpp"

/** @file element.hpp
    @brief Provides the main n-cell type
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
    
    
    
    template<typename element_type_or_tag, typename something, typename functor>
    typename result_of::element_view<something, element_type_or_tag>::type element_view( something & s, functor f )
    {
        typedef typename result_of::element_tag<element_type_or_tag>::type element_tag;
        
        typedef typename result_of::element<something, element_tag>::type ElementType;
        typedef typename result_of::element_range<something, element_tag>::type RangeType;
        typedef typename result_of::hook_iterator<RangeType>::type IteratorType;
        
        RangeType range = viennagrid::elements<element_tag>(s);
        
        typename result_of::element_view<something, element_tag>::type view;
        view.set_base_container( *range.get_base_container() );
        
        for ( IteratorType it = range.begin(); it != range.end(); ++it )
        {
            ElementType const & element = viennagrid::dereference_hook(s, *it);
            if ( f(element) )
                view.insert_hook( *it );
        }
        
        return view;
    }
    
    
    
    





    template<typename tag, typename base_element_type>
    class element_tag_with_base_key
    {
    public:
        
        element_tag_with_base_key( const base_element_type & base_obj ) : id(base_obj.id()) {}
        
        bool operator<( element_tag_with_base_key<tag, base_element_type> const & rhs ) const
        {
            return id < rhs.id;
        }
        
    private:
        typedef typename result_of::id_type<base_element_type>::type id_type;
        id_type id;
    };
    
    
    
    
    
    template<typename tag_, typename element_to_tag_type>
    void tag( element_to_tag_type const & element_to_tag )
    {
        viennadata::access<tag_, bool>()(element_to_tag) = true;
    }
    
    template<typename tag_, typename element_to_tag_type>
    void untag( element_to_tag_type const & element_to_tag )
    {
        viennadata::access<tag_, bool>()(element_to_tag) = false;
    }
    
    template<typename tag_, typename element_to_tag_type>
    bool is_tagged( element_to_tag_type const & element_to_tag )
    {
        return viennadata::access<tag_, bool>()(element_to_tag);
    }
    
    
    
    template<typename tag_, typename element_to_tag_type, typename base_element_type>
    void tag( element_to_tag_type const & element_to_tag, base_element_type const & base_element )
    {
        viennadata::access<element_tag_with_base_key<tag_, base_element_type>, bool>(element_tag_with_base_key<tag_, base_element_type>(base_element))(element_to_tag) = true;
    }
    
    template<typename tag_, typename element_to_tag_type, typename base_element_type>
    void untag( element_to_tag_type const & element_to_tag, base_element_type const & base_element )
    {
        viennadata::access<element_tag_with_base_key<tag_, base_element_type>, bool>(element_tag_with_base_key<tag_, base_element_type>(base_element))(element_to_tag) = false;
    }
    
    template<typename tag_, typename element_to_tag_type, typename base_element_type>
    bool is_tagged( element_to_tag_type const & element_to_tag, base_element_type const & base_element )
    {
        return viennadata::access<element_tag_with_base_key<tag_, base_element_type>, bool>(element_tag_with_base_key<tag_, base_element_type>(base_element))(element_to_tag);
    }

    
    
    
    template<typename tag, typename base_element_type>
    class element_tag_with_base_predicate
    {
    public:
        
        element_tag_with_base_predicate(const base_element_type & base_element) : key(base_element) {}
        
        template<typename element_type>
        bool operator()( const element_type & element )
        { return viennadata::access< viennagrid::element_tag_with_base_key<tag, base_element_type>, bool >(key)(element); }
        
        
    private:
        viennagrid::element_tag_with_base_key<tag, base_element_type> key;
    };
    
    template<typename tag, typename base_element_type>
    class inverse_element_tag_with_base_predicate
    {
    public:
        
        inverse_element_tag_with_base_predicate(const base_element_type & base_element) : key(base_element) {}
        
        template<typename element_type>
        bool operator()( const element_type & element )
        { return !viennadata::access< viennagrid::element_tag_with_base_key<tag, base_element_type>, bool >(key)(element); }
        
        
    private:
        viennagrid::element_tag_with_base_key<tag, base_element_type> key;
    };
    
    
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename result_of::element_view<base_element_type, element_type_or_tag>::type tagged_elements( base_element_type & base_element )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, viennagrid::element_tag_with_base_predicate<tag_, base_element_type>(base_element) );
    }
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename result_of::const_element_view<base_element_type, element_type_or_tag>::type tagged_elements( base_element_type const & base_element )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, viennagrid::element_tag_with_base_predicate<tag_, base_element_type>(base_element) );
    }
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename result_of::element_view<base_element_type, element_type_or_tag>::type untagged_elements( base_element_type & base_element )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, viennagrid::inverse_element_tag_with_base_predicate<tag_, base_element_type>(base_element) );
    }
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename result_of::const_element_view<base_element_type, element_type_or_tag>::type untagged_elements( base_element_type const & base_element )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, viennagrid::inverse_element_tag_with_base_predicate<tag_, base_element_type>(base_element) );
    }
    
    
    
    
    
    
    struct loose_tag {};
    struct hole_tag {};
    struct bounding_tag {};
    
    


    template<typename element_type_or_tag, typename base_element_type>
    typename result_of::element_view<base_element_type, element_type_or_tag>::type loose_elements( base_element_type & base_element )
    {
        return tagged_elements<element_type_or_tag, loose_tag>(base_element);
    }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename result_of::const_element_view<base_element_type, element_type_or_tag>::type loose_elements( base_element_type const & base_element )
    {
        return tagged_elements<element_type_or_tag, loose_tag>(base_element);
    }
    
    
    template<typename element_type_or_tag, typename base_element_type>
    typename result_of::element_view<base_element_type, element_type_or_tag>::type hole_elements( base_element_type & base_element )
    {
        return tagged_elements<element_type_or_tag, hole_tag>(base_element);
    }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename result_of::const_element_view<base_element_type, element_type_or_tag>::type hole_elements( base_element_type const & base_element )
    {
        return tagged_elements<element_type_or_tag, hole_tag>(base_element);
    }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename result_of::element_view<base_element_type, element_type_or_tag>::type bounding_elements( base_element_type & base_element )
    {
        return tagged_elements<element_type_or_tag, bounding_tag>(base_element);
    }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename result_of::const_element_view<base_element_type, element_type_or_tag>::type bounding_elements( base_element_type const & base_element )
    {
        return tagged_elements<element_type_or_tag, bounding_tag>(base_element);
    }
  
}


#endif
