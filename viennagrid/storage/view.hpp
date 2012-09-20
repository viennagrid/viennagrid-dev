#ifndef VIENNAGRID_VIEW_HPP
#define VIENNAGRID_VIEW_HPP

#include "container.hpp"


namespace viennagrid
{
    
    namespace storage
    {
        
        template<typename base_container_type, typename view_container_tag = std_deque_tag>
        class view_t;
        
        namespace container
        {
            
            template<typename base_container_type, typename view_container_tag>
            typename viennagrid::storage::view_t<base_container_type, view_container_tag>::iterator insert(
                    viennagrid::storage::view_t<base_container_type, view_container_tag> & container,
                    const typename viennagrid::storage::view_t<base_container_type, view_container_tag>::value_type & element )
            {
                return container.insert( element );
            }
            
        }
        
        
        template<typename base_container_type, typename view_container_tag>
        class view_t
        {
            typedef typename viennagrid::storage::result_of::container_from_tag<typename base_container_type::pointer, view_container_tag>::type view_container_type;
            
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
            
            
            
            view_t(base_container_type & _bc) : base_container(&_bc)
            {
                for (typename base_container_type::iterator it = _bc.begin(); it != _bc.end(); ++it)
                    container.push_back( &(*it) );
            }

            template<class functor>
            view_t(base_container_type & _bc, functor f) : base_container(&_bc)
            {
                for (typename base_container_type::iterator it = _bc.begin(); it != _bc.end(); ++it)
                    if (f(*it)) container.push_back( &(*it) );
            }
            
            
            
            
            iterator begin() { return iterator(container.begin()); }
            iterator end() { return iterator(container.end()); }
            
            reference front() { return *(container.front()); }
            const_reference front() const { return *(container.front()); }
            
            reference back() { return *(container.back()); }
            const_reference back() const { return *(container.back()); }
            
            iterator insert(const value_type & element)
            {
                return viennagrid::storage::container::insert(
                    container,
                    &(*(viennagrid::storage::container::insert( *base_container, element )))
                );
            }
            
            iterator erase(const iterator & it)
            {
                return container.erase( it );                
            }
            
            
        private:
            
            base_container_type * base_container;
            view_container_type container;
        };


        namespace result_of
        {
            template<typename base_container_type, typename view_container_tag>
            struct view
            {
                typedef view_t<base_container_type, view_container_tag> type;
            };
            
        }


    }
    
}

#endif