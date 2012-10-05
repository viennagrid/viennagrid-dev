#ifndef VIENNAGRID_STORAGE_VIEW_HPP
#define VIENNAGRID_STORAGE_VIEW_HPP

#include <iterator>

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/reference.hpp"
#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/meta/typemap_macros.hpp"

namespace viennagrid
{
    
    namespace storage
    {
        
        template<typename reference_type, typename view_container_tag>
        class view_t
        {
            typedef typename viennagrid::storage::result_of::container_from_tag<reference_type, view_container_tag>::type view_container_type;
            
        public:
            
            typedef typename view_container_type::size_type size_type;
            typedef typename viennagrid::storage::reference::value_type_from_reference_type<reference_type>::type value_type;
            typedef value_type & reference;
            typedef const value_type & const_reference;
            typedef value_type * pointer;
            typedef const value_type * const_pointer;

            
            
            class iterator : public view_container_type::iterator
            {
                typedef typename view_container_type::iterator base;
            public:
                iterator(const base & foo) : base(foo) {}
                
                typedef view_t::value_type value_type;
                typedef view_t::reference reference;
                typedef view_t::const_reference const_reference;
                typedef view_t::pointer pointer;
                typedef view_t::const_pointer const_pointer;
                
                reference operator* () { return *(base::operator*()); }
                const_reference operator* () const { return *(base::operator*()); }
            };
            
            
            class const_iterator : public view_container_type::const_iterator
            {
                typedef typename view_container_type::const_iterator base;
            public:
                const_iterator(const base & foo) : base(foo) {}
                
                typedef view_t::value_type value_type;
                typedef view_t::const_reference reference;
                typedef view_t::const_reference const_reference;
                typedef view_t::const_pointer pointer;
                typedef view_t::const_pointer const_pointer;
                
                const_reference operator* () const { return *(base::operator*()); }
            };
            
            
            class reverse_iterator : public view_container_type::reverse_iterator
            {
                typedef typename view_container_type::reverse_iterator base;
            public:
                reverse_iterator(const base & foo) : base(foo) {}
                
                typedef view_t::value_type value_type;
                typedef view_t::reference reference;
                typedef view_t::const_reference const_reference;
                typedef view_t::pointer pointer;
                typedef view_t::const_pointer const_pointer;
                
                reference & operator* () { return *(base::operator*()); }
                const_reference operator* () const { return *(base::operator*()); }
            };
            
            
            class const_reverse_iterator : public view_container_type::const_reverse_iterator
            {
                typedef typename view_container_type::const_reverse_iterator base;
            public:
                const_reverse_iterator(const base & foo) : base(foo) {}
                
                typedef view_t::value_type value_type;
                typedef view_t::const_reference reference;
                typedef view_t::const_reference const_reference;
                typedef view_t::const_pointer pointer;
                typedef view_t::const_pointer const_pointer;
                
                const_reference operator* () const { return *(base::operator*()); }
            };
            
            
            view_t() {}
            
            
            iterator begin() { return iterator(container.begin()); }
            iterator end() { return iterator(container.end()); }

            const_iterator begin() const { return const_iterator(container.begin()); }
            const_iterator end() const { return const_iterator(container.end()); }

            
            reference front() { return *(container.front()); }
            const_reference front() const { return *(container.front()); }
            
            reference back() { return *(container.back()); }
            const_reference back() const { return *(container.back()); }
            
            reference operator[]( size_type index ) { return container[index]; }
            const_reference operator[]( size_type index ) const { return container[index]; }
            
            
            
            size_type size() const { return container.size(); }
            
            
            std::pair<iterator, bool> insert(value_type & element)
            {
                return viennagrid::storage::container::insert(container, &element);
            }
            
            
        private:
            
            view_container_type container;
        };
        
        namespace container
        {
            template<typename reference_type, typename view_container_tag>
            std::pair<typename viennagrid::storage::view_t<reference_type, view_container_tag>::iterator, bool> insert(
                    viennagrid::storage::view_t<reference_type, view_container_tag> & container,
                    typename viennagrid::storage::reference::value_type_from_reference_type<reference_type>::type & element)
            {
                return container.insert( element );
            }
            
            
            template<typename from_iterator_type, typename to_container_type>
            void reference( from_iterator_type it_start, from_iterator_type it_end, to_container_type & to )
            {
                for ( ;it_start != it_end; ++it_start)
                    viennagrid::storage::container::insert(to, *it_start);
            }
        }

        
        template<typename _view_container_tag>
        struct view_tag
        {
            typedef _view_container_tag view_container_tag;
        };
        
        
        namespace view
        {
            typedef VIENNAMETA_MAKE_TYPEMAP_1(viennagrid::storage::default_tag, viennagrid::storage::view_tag<viennagrid::storage::std_deque_tag>) default_view_config;
            
            
                template<typename container_typelist, typename reference_config>
                struct reference_typelist_from_container_typelist_using_reference_config {};
                
                template<typename reference_config>
                struct reference_typelist_from_container_typelist_using_reference_config<viennameta::null_type, reference_config>
                {
                    typedef viennameta::null_type type;
                };
                
                template<typename head, typename tail, typename reference_config>
                struct reference_typelist_from_container_typelist_using_reference_config<viennameta::typelist_t<head, tail>, reference_config>
                {
                    typedef viennameta::typelist_t<
                        typename viennagrid::storage::reference::reference_type_from_config<head, reference_config>::type,
                        typename reference_typelist_from_container_typelist_using_reference_config<tail, reference_config>::type
                    > type;
                };
        }
        

        namespace result_of
        {
            template<typename reference_type, typename view_container_tag>
            struct view
            {
                typedef view_t<reference_type, view_container_tag> type;
            };
            
            template<typename reference_type, typename view_container_tag>
            struct container_from_tag<reference_type, view_tag<view_container_tag> >
            {
                typedef typename view<reference_type, view_container_tag>::type type;
            };
        }


    }
    
}

#endif