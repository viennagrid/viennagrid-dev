#ifndef VIENNAGRID_STORAGE_VIEW_HPP
#define VIENNAGRID_STORAGE_VIEW_HPP

#include "container.hpp"


namespace viennagrid
{
    
    namespace storage
    {
        
        struct pointer_reference_tag;
        struct iterator_reference_tag;
        struct id_tag;
        
        
        
        
        
        template<typename base_container_type, typename view_container_tag, typename view_reference_tag>
        struct view_container
        {};
        
        template<typename base_container_type, typename view_container_tag>
        struct view_container<base_container_type, view_container_tag, pointer_reference_tag>
        {
            typedef typename viennagrid::storage::result_of::container_from_tag<typename base_container_type::pointer, view_container_tag>::type type;
        };
        
        template<typename base_container_type, typename view_container_tag>
        struct view_container<base_container_type, view_container_tag, iterator_reference_tag>
        {
            typedef typename viennagrid::storage::result_of::container_from_tag<typename base_container_type::iterator, view_container_tag>::type type;
        };
        
        
        
        
        template<typename base_container_type, typename view_container_tag = std_deque_tag, typename view_reference_tag = pointer_reference_tag>
        class view_t
        {
            //typedef typename viennagrid::storage::result_of::container_from_tag<typename base_container_type::pointer, view_container_tag>::type view_container_type;
            
            typedef typename view_container<base_container_type, view_container_tag, view_reference_tag>::type view_container_type;
            
        public:
            
            typedef typename base_container_type::size_type size_type;
            typedef typename base_container_type::value_type value_type;
            typedef typename base_container_type::reference reference;
            typedef typename base_container_type::const_reference const_reference;
            typedef typename base_container_type::pointer pointer;
            typedef typename base_container_type::const_pointer const_pointer;
            
            
            
            
            class iterator : public view_container_type::iterator
            {
                typedef typename view_container_type::iterator base;
            public:
                iterator(const base & foo) : base(foo) {}
                
                value_type & operator* () { return *(base::operator*()); }
                const value_type & operator* () const { return *(base::operator*()); }
            };
            
            
            class const_iterator : public view_container_type::const_iterator
            {
                typedef typename view_container_type::const_iterator base;
            public:
                const_iterator(const base & foo) : base(foo) {}
                
                const value_type & operator* () const { return *(base::operator*()); }
            };
            
            
            class reverse_iterator : public view_container_type::reverse_iterator
            {
                typedef typename view_container_type::reverse_iterator base;
            public:
                reverse_iterator(const base & foo) : base(foo) {}
                
                value_type & operator* () { return *(base::operator*()); }
                const value_type & operator* () const { return *(base::operator*()); }
            };
            
            
            class const_reverse_iterator : public view_container_type::const_reverse_iterator
            {
                typedef typename view_container_type::const_reverse_iterator base;
            public:
                const_reverse_iterator(const base & foo) : base(foo) {}
                
                const value_type & operator* () const { return *(base::operator*()); }
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
            
            
            
            size_type size() const { return container.size(); }
            
            
            iterator insert(value_type & element)
            {
                return viennagrid::storage::container::insert(
                    container,
                    &element
                );
            }
            
            
        private:
            
            view_container_type container;
        };
        
        namespace container
        {
            template<typename base_container_type, typename view_container_tag, typename view_reference_tag, typename value_type>
            typename viennagrid::storage::view_t<base_container_type, view_container_tag, view_reference_tag>::iterator insert(
                    viennagrid::storage::view_t<base_container_type, view_container_tag, view_reference_tag> & container,
                    value_type & element)
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

        
        template<typename _view_container_tag, typename _view_reference_tag = pointer_reference_tag>
        struct view_tag
        {
            typedef _view_container_tag view_container_tag;
            typedef _view_reference_tag view_reference_tag;
        };

        namespace result_of
        {
            template<typename base_container_type, typename view_container_tag>
            struct view
            {
                typedef view_t<base_container_type, view_container_tag> type;
            };
            
            template<typename element_type, typename view_container_tag>
            struct container_from_tag<element_type, view_tag<view_container_tag> >
            {
                typedef typename view<element_type, view_container_tag>::type type;
            };
        }


    }
    
}

#endif