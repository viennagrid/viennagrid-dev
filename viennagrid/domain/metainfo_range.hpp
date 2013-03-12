#ifndef VIENNAGRID_METAINFO_RANGE_HPP
#define VIENNAGRID_METAINFO_RANGE_HPP

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
#include "viennagrid/domain/geometric_domain.hpp"

/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
{
    
    


    template<typename range_or_view_type, typename metainfo_container_type>
    class metainfo_range_t
    {
        typedef typename viennagrid::result_of::iterator<range_or_view_type>::type range_iterator;
        typedef typename viennagrid::result_of::const_iterator<range_or_view_type>::type range_const_iterator;
        
    public:
        
        typedef typename range_or_view_type::value_type element_type;
        typedef typename viennagrid::metainfo::result_of::value_type<metainfo_container_type>::type metainfo_type;
        
        typedef typename range_or_view_type::size_type size_type;
        typedef metainfo_type value_type;
        typedef value_type & reference;
        typedef const value_type & const_reference;
        typedef value_type * pointer;
        typedef const value_type * const_pointer;
        
        metainfo_range_t( range_or_view_type const & range_, metainfo_container_type & metainfo_container_ ) : range(range_), metainfo_container(&metainfo_container_) {}
        
        struct const_iterator;
        
        struct iterator : public range_iterator
        {
            typedef range_iterator base;
        public:
            iterator(const base & foo, metainfo_container_type * metainfo_container_) : base(foo), metainfo_container(metainfo_container_) {}

            typedef typename std::iterator_traits<base>::difference_type difference_type;
            typedef metainfo_type value_type;
            typedef value_type & reference;
            typedef value_type * pointer;
            typedef typename std::iterator_traits<base>::iterator_category iterator_category;
            
            
            element_type & element() { return base::operator*(); }
            element_type const & element() const { return base::operator*(); }
            
            metainfo_type & metainfo() { return viennagrid::metainfo::look_up(*metainfo_container, base::operator*()); }
            metainfo_type const & metainfo() const { return viennagrid::metainfo::look_up(*metainfo_container, base::operator*()); }
            
            
            
            // increment- and decrementable
            iterator & operator++() { base::operator++(); return *this; }
            iterator operator++(int) { base::operator++(int()); return *this; }
            
            iterator & operator--() { base::operator--(); return *this; }
            iterator operator--(int) { base::operator--(int()); return *this; }
            
            // add and subtractable; operator+ and operator- is below
            difference_type operator-(const iterator & it) const { return base(*this) - base(it); }
            difference_type operator-(const const_iterator & it) const { return base(*this) - base(it); }
            

            // dereference
            metainfo_type & operator* () { return metainfo(); }
            metainfo_type const & operator* () const { return metainfo(); }
            
            metainfo_type * operator->() { return &**this; }
            metainfo_type const * operator->() const { return &**this; }
            
        private:
            metainfo_container_type * metainfo_container;
        };
        
        
        struct const_iterator : public range_const_iterator
        {
            typedef range_const_iterator base;
        public:
            const_iterator(const base & foo, const metainfo_container_type * metainfo_container_) : base(foo), metainfo_container(metainfo_container_) {}

            typedef typename std::iterator_traits<base>::difference_type difference_type;
            typedef const metainfo_type value_type;
            typedef value_type & reference;
            typedef value_type * pointer;
            typedef typename std::iterator_traits<base>::iterator_category iterator_category;
            
            
            element_type const & element() const { return base::operator*(); }
            metainfo_type const & metainfo() const { return viennagrid::metainfo::look_up(*metainfo_container, base::operator*()); }

            
            
            // increment- and decrementable
            iterator & operator++() { base::operator++(); return *this; }
            iterator operator++(int) { base::operator++(int()); return *this; }
            
            iterator & operator--() { base::operator--(); return *this; }
            iterator operator--(int) { base::operator--(int()); return *this; }
            
            // add and subtractable; operator+ and operator- is below
            difference_type operator-(const iterator & it) const { return base::operator-(it); }
            difference_type operator-(const const_iterator & it) const { return base::operator-(it); }
            
            
            // dereference            
            metainfo_type const & operator* () const { return metainfo(); }
            metainfo_type const * operator->() const { return &**this; }
            
        private:
            const metainfo_container_type * metainfo_container;
        };

        
        iterator begin() { return iterator(range.begin(), metainfo_container); }
        iterator end() { return iterator(range.end(), metainfo_container); }
        
        const_iterator cbegin() { return const_iterator(range.begin(), metainfo_container); }
        const_iterator cend() { return const_iterator(range.end(), metainfo_container); }

        const_iterator begin() const { return const_iterator(range.begin(), metainfo_container); }
        const_iterator end() const { return const_iterator(range.end(), metainfo_container); }
        
        
        reference front() { return look_up(range.front()); }
        const_reference front() const { return look_up(range.front()); }
        
        reference back() { return look_up(range.back()); }
        const_reference back() const { return look_up(range.back()); }
        
        reference operator[]( size_type index ) { return look_up(range[index]); }
        const_reference operator[]( size_type index ) const { return look_up(range[index]); }

        size_type size() const { return range.size(); }
        
    private:
        
        reference look_up( element_type const & element )
        { return viennagrid::metainfo::look_up(*metainfo_container, element); }

        const_reference look_up( element_type const & element ) const
        { return viennagrid::metainfo::look_up(*metainfo_container, element); }

        
        range_or_view_type range;
        metainfo_container_type * metainfo_container;
    };
    
    
    template<typename range_or_view_type, typename metainfo_container_type>
    class metainfo_range_t<range_or_view_type, const metainfo_container_type>
    {
        typedef typename viennagrid::result_of::const_iterator<range_or_view_type>::type range_const_iterator;
        
    public:
        
        typedef typename range_or_view_type::value_type element_type;
        typedef typename viennagrid::metainfo::result_of::value_type<metainfo_container_type>::type metainfo_type;
        
        typedef typename range_or_view_type::size_type size_type;
        typedef metainfo_type value_type;
        typedef value_type & reference;
        typedef const value_type & const_reference;
        typedef value_type * pointer;
        typedef const value_type * const_pointer;
        
        metainfo_range_t( range_or_view_type const & range_, const metainfo_container_type & metainfo_container_ ) : range(range_), metainfo_container(&metainfo_container_) {}        
        
        struct const_iterator : public range_const_iterator
        {
            typedef range_const_iterator base;
        public:
            const_iterator(const base & foo, const metainfo_container_type * metainfo_container_) : base(foo), metainfo_container(metainfo_container_) {}
            
            typedef typename std::iterator_traits<base>::difference_type difference_type;
            typedef metainfo_type value_type;
            typedef value_type & reference;
            typedef value_type * pointer;
            typedef typename std::iterator_traits<base>::iterator_category iterator_category;
            
            const element_type & element() const { return base::operator*(); }
            metainfo_type const & metainfo() const { std::cout << "## " << base::operator*() << std::endl; return viennagrid::metainfo::look_up(*metainfo_container, base::operator*()); }
            
            metainfo_type const & operator* () const { return metainfo(); }
            metainfo_type const * operator->() const { return &**this; }
            
        private:
            const metainfo_container_type * metainfo_container;
        };
        
        typedef const_iterator iterator;
        
        const_iterator cbegin() { return const_iterator(range.begin(), metainfo_container); }
        const_iterator cend() { return const_iterator(range.end(), metainfo_container); }

        const_iterator begin() const { return const_iterator(range.begin(), metainfo_container); }
        const_iterator end() const { return const_iterator(range.end(), metainfo_container); }
        
        
        reference front() { return look_up(range.front()); }
        const_reference front() const { return look_up(range.front()); }
        
        reference back() { return look_up(range.back()); }
        const_reference back() const { return look_up(range.back()); }
        
        reference operator[]( size_type index ) { return look_up(range[index]); }
        const_reference operator[]( size_type index ) const { return look_up(range[index]); }
        
        size_type size() const { return range.size(); }

        
    private:
        
        reference look_up( element_type const & element )
        { return viennagrid::metainfo::look_up(*metainfo_container, element); }

        const_reference look_up( element_type const & element ) const
        { return viennagrid::metainfo::look_up(*metainfo_container, element); }

        
        range_or_view_type range;
        const metainfo_container_type * metainfo_container;
    };
    
    
    
  
    
    
    namespace result_of
    {
        template<typename view_type, typename metainfo_container_type>
        struct metainfo_view
        {
//             typedef typename view_type::value_type element_type;
//             typedef typename metainfo_container<geometric_domain_type, element_type, metainfo_type>::type metainfo_container_type;
            
            typedef metainfo_range_t<view_type, metainfo_container_type> type;
        };
        
//         template<typename view_type, typename metainfo_container_type>
//         struct const_metainfo_view
//         {
//             typedef typename view_type::value_type element_type;
//             typedef typename const_metainfo_container<geometric_domain_type, element_type, metainfo_type>::type metainfo_container_type;
//             
//             typedef metainfo_range_t<view_type, metainfo_container_type> type;
//         };
        
        
        
        
        template<typename geometric_domain_type, typename element_type, typename metainfo_type>
        struct metainfo_range
        {
            typedef typename element_range<geometric_domain_type, element_type>::type element_range_type;
            typedef typename metainfo_container<geometric_domain_type, element_type, metainfo_type>::type metainfo_container_type;
            
            typedef metainfo_range_t<element_range_type, metainfo_container_type> type;
        };
        
        template<typename geometric_domain_type, typename element_type, typename metainfo_type>
        struct const_metainfo_range
        {
            typedef typename const_element_range<geometric_domain_type, element_type>::type element_range_type;
            typedef typename const_metainfo_container<geometric_domain_type, element_type, metainfo_type>::type metainfo_container_type;
            
            typedef metainfo_range_t<element_range_type, metainfo_container_type> type;
        };
        
        
        

            
    }
    
    
    
    template<typename geometric_domain_type, typename element_type, typename metainfo_type>
    struct metainfo_range_helper
    {
        typedef typename result_of::metainfo_range<geometric_domain_type, element_type, metainfo_type>::type result_type;
        
        static result_type exec( geometric_domain_type & domain )
        {
            return result_type(
                viennagrid::elements<element_type>(domain),
                viennagrid::metainfo_container<element_type, metainfo_type>(domain)
            );
        }
    };
    
    template<typename geometric_domain_type, typename element_type, typename metainfo_type>
    struct metainfo_range_helper<const geometric_domain_type, element_type, metainfo_type>
    {
        typedef typename result_of::const_metainfo_range<geometric_domain_type, element_type, metainfo_type>::type result_type;
        
        static result_type exec( geometric_domain_type const & domain )
        {
            return result_type(
                viennagrid::elements<element_type>(domain),
                viennagrid::metainfo_container<element_type, metainfo_type>(domain)
            );
        }
    };
    
    template<typename element_type, typename metainfo_type, typename geometric_domain_type>
    typename metainfo_range_helper<geometric_domain_type, element_type, metainfo_type>::result_type metainfo_range( geometric_domain_type & domain )
    { return metainfo_range_helper<geometric_domain_type, element_type, metainfo_type>::exec(domain); }
     
     
     
     
     
    template<typename view_type, typename geometric_domain_type, typename metainfo_type>
    struct metainfo_view_from_domain_helper
    {
        typedef typename view_type::value_type element_type;
        typedef typename result_of::metainfo_view<
                view_type,
                typename result_of::metainfo_container<geometric_domain_type, element_type, metainfo_type>::type
            >::type result_type;
        
        static result_type exec( view_type & view, geometric_domain_type & domain )
        {
            return result_type(
                view,
                viennagrid::metainfo_container<typename view_type::value_type, metainfo_type>(domain)
            );
        }
    };
    
    template<typename view_type, typename geometric_domain_type, typename metainfo_type>
    struct metainfo_view_from_domain_helper<const view_type, const geometric_domain_type, metainfo_type>
    {
        typedef typename view_type::value_type element_type;
        typedef typename result_of::metainfo_view<
                const view_type,
                const typename result_of::metainfo_container<geometric_domain_type, element_type, metainfo_type>::type
            >::type result_type;
        
        static result_type exec( view_type & view, geometric_domain_type & domain )
        {
            return result_type(
                view,
                viennagrid::metainfo_container<typename view_type::value_type, metainfo_type>(domain)
            );
        }
    };

    
    template<typename metainfo_type, typename view_type, typename geometric_domain_type>
    typename metainfo_view_from_domain_helper<view_type, geometric_domain_type, metainfo_type>::result_type metainfo_view( view_type & view, geometric_domain_type & domain )
    { return metainfo_view_from_domain_helper<view_type, geometric_domain_type, metainfo_type>::exec(view, domain); }
    
    
    
    
    template<typename view_type, typename metainfo_container_type>
    struct metainfo_view_helper
    {
        typedef typename result_of::metainfo_view<view_type, metainfo_container_type>::type result_type;
        
        static result_type exec( view_type & view, metainfo_container_type & metainfo_container )
        {
            return result_type(view, metainfo_container);
        }
    };
    
    template<typename view_type, typename metainfo_container_type>
    struct metainfo_view_helper<const view_type, const metainfo_container_type>
    {
        typedef typename result_of::metainfo_view<view_type, metainfo_container_type>::type result_type;
        
        static result_type exec( view_type & view, metainfo_container_type & metainfo_container )
        {
            return result_type(view, metainfo_container);
        }
    };
    
    
    template<typename view_type, typename metainfo_container_type>
    typename metainfo_view_helper<view_type, metainfo_container_type>::result_type metainfo_view( view_type & view, metainfo_container_type & metainfo_container )
    { return metainfo_view_helper<view_type, metainfo_container_type>::exec(view, metainfo_container); }
    
    
    
    
//     template<typename element_tag, typename boundary_cell_typelist, typename id_type, typename metainfo>
//     std::ostream & operator<<(std::ostream & os, std::pair< element_t<element_tag, boundary_cell_typelist, id_type>, metainfo> const & p)
//     {
//         os << p.first << " " << p.second;
//         return os;
//     }
    
}


#endif
