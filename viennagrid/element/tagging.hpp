#ifndef VIENNAGRID_TAGGING_HPP
#define VIENNAGRID_TAGGING_HPP

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
#include "viennagrid/element/element_view.hpp"

/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
{
    namespace tagging
    {
        template<typename element_type_, typename tag_>
        class element_tag
        {
        public:
            
            typedef tag_ tag;
            typedef element_type_ element_type;
            
            element_tag( const element_type & element ) : id(element.id()) {}
            
            bool operator<( element_tag<element_type, tag> const & rhs ) const
            {
                return id < rhs.id;
            }
            
        private:
            typedef typename result_of::id_type<element_type>::type id_type;
            id_type id;
        };
        
        
        template<typename tag, typename element_type>
        element_tag<element_type, tag> create_element_tag( element_type const & element )
        {
            return element_tag<element_type, tag>(element);
        }
        
        
        
        namespace result_of
        {
            
            template<typename element_type_, typename tag_>
            struct element_tag
            {
                typedef viennagrid::tagging::element_tag<element_type_, tag_> type;
            };
            
        }
        
        
        
        struct access_with_key_tag {};
        struct access_without_key_tag {};
        
        template<typename tag>
        struct viennadata_access_method
        {
            typedef access_without_key_tag type;
        };
        
        template<typename element_type, typename tag>
        struct viennadata_access_method< element_tag<element_type, tag> >
        {
            typedef access_with_key_tag type;
        };
        

        
        template<typename how_to_access>
        struct viennadata_access;
        
        
        template<>
        struct viennadata_access<access_without_key_tag>
        {
            template<typename access_type, typename element_type, typename key_type>
            static access_type & access(element_type const & element, key_type key)
            { return viennadata::access<key_type, access_type>()(element); }
        };
        
        template<>
        struct viennadata_access<access_with_key_tag>
        {
            template<typename access_type, typename element_type, typename key_type>
            static access_type & access(element_type const & element, key_type key)
            { return viennadata::access<key_type, access_type>(key)(element); }
        };
        
        
        
        
        
        
        template<typename tag_, typename element_to_tag_type>
        void tag( element_to_tag_type const & element_to_tag )
        {
    //         viennadata_access<access_without_key_tag>::template access<bool>(element_to_tag, tag_()) = true;
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
        
        
        
        template<typename tag_type, typename element_to_tag_type>
        void tag( element_to_tag_type const & element_to_tag, tag_type tag_ )
        {
            viennadata_access< typename viennadata_access_method<tag_type>::type >::template access<bool>(element_to_tag, tag_) = true;
    //         viennadata::access<element_tag<base_element_type, tag_>, bool>(element_tag<base_element_type, tag_>(base_element))(element_to_tag) = true;
        }
        
        template<typename tag_type, typename element_to_tag_type>
        void untag( element_to_tag_type const & element_to_tag, tag_type tag_ )
        {
            viennadata_access< typename viennadata_access_method<tag_type>::type >::template access<bool>(element_to_tag, tag_) = false;
    //         viennadata::access<element_tag<base_element_type, tag_>, bool>(element_tag<base_element_type, tag_>(base_element))(element_to_tag) = false;
        }
        
        template<typename tag_type, typename element_to_tag_type>
        bool is_tagged( element_to_tag_type const & element_to_tag, tag_type tag_ )
        {
            return viennadata_access< typename viennadata_access_method<tag_type>::type >::template access<bool>(element_to_tag, tag_);
    //         return viennadata::access<element_tag<base_element_type, tag_>, bool>(element_tag<base_element_type, tag_>(base_element))(element_to_tag);
        }

        
        
        
        template<typename tag>
        class tag_predicate
        {
        public:
            
            tag_predicate(const tag & t_) : tag_(t_) {}
            
            template<typename element_type>
            bool operator()( const element_type & element )
            { return is_tagged(element, tag_); }
            
            
        private:
            tag tag_;
        };
        
        template<typename tag>
        class inverse_tag_predicate
        {
        public:
            
            inverse_tag_predicate(const tag & t_) : tag_(t_) {}
            
            template<typename element_type>
            bool operator()( const element_type & element )
            { return !is_tagged(element, tag_); }
            
            
        private:
            tag tag_;
        };
        
    }
        

    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename viennagrid::result_of::element_view<base_element_type, element_type_or_tag>::type tagged_elements( base_element_type & base_element, tag_ t_ )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, tagging::tag_predicate<tag_>(t_) );
    }
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename viennagrid::result_of::const_element_view<base_element_type, element_type_or_tag>::type tagged_elements( base_element_type const & base_element, tag_ t_ )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, tagging::tag_predicate<tag_>(t_) );
    }
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename viennagrid::result_of::element_view<base_element_type, element_type_or_tag>::type untagged_elements( base_element_type & base_element, tag_ t_ )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, tagging::inverse_tag_predicate<tag_>(t_) );
    }
    
    template<typename element_type_or_tag, typename tag_, typename base_element_type>
    typename viennagrid::result_of::const_element_view<base_element_type, element_type_or_tag>::type untagged_elements( base_element_type const & base_element, tag_ t_ )
    {
        return viennagrid::element_view<element_type_or_tag>( base_element, tagging::inverse_tag_predicate<tag_>(t_) );
    }
    
    

    
    
    
    struct loose_tag {};
//     struct hole_tag {};
    struct bounding_tag {};
    



    template<typename element_type_or_tag, typename base_element_type>
    typename viennagrid::result_of::element_view<base_element_type, element_type_or_tag>::type loose_elements( base_element_type & base_element )
    {
        typedef typename tagging::result_of::element_tag<base_element_type, loose_tag>::type tag;
        return tagged_elements<element_type_or_tag, tag>(base_element, tag(base_element) );
    }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename viennagrid::result_of::const_element_view<base_element_type, element_type_or_tag>::type loose_elements( base_element_type const & base_element )
    {
        typedef typename tagging::result_of::element_tag<base_element_type, loose_tag>::type tag;
        return tagged_elements<element_type_or_tag, tag>(base_element, tag(base_element) );
    }
    
    
//     template<typename element_type_or_tag, typename base_element_type>
//     typename viennagrid::result_of::element_view<base_element_type, element_type_or_tag>::type hole_elements( base_element_type & base_element )
//     {
//         typedef typename tagging::result_of::element_tag<base_element_type, hole_tag>::type tag;
//         return tagged_elements<element_type_or_tag, tag>(base_element, tag(base_element) );
//     }
//     
//     template<typename element_type_or_tag, typename base_element_type>
//     typename viennagrid::result_of::const_element_view<base_element_type, element_type_or_tag>::type hole_elements( base_element_type const & base_element )
//     {
//         typedef typename tagging::result_of::element_tag<base_element_type, hole_tag>::type tag;
//         return tagged_elements<element_type_or_tag, tag>(base_element, tag(base_element) );
//     }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename viennagrid::result_of::element_view<base_element_type, element_type_or_tag>::type bounding_elements( base_element_type & base_element )
    {
        typedef typename tagging::result_of::element_tag<base_element_type, bounding_tag>::type tag;
        return tagged_elements<element_type_or_tag, tag>(base_element, tag(base_element) );
    }
    
    template<typename element_type_or_tag, typename base_element_type>
    typename viennagrid::result_of::const_element_view<base_element_type, element_type_or_tag>::type bounding_elements( base_element_type const & base_element )
    {
        typedef typename tagging::result_of::element_tag<base_element_type, bounding_tag>::type tag;
        return tagged_elements<element_type_or_tag, tag>(base_element, tag(base_element) );
    }

  
}


#endif
