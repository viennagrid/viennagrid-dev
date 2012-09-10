#ifndef VIENNAGRID_VIEW_HPP
#define VIENNAGRID_VIEW_HPP

#include "container.hpp"


namespace viennagrid
{
    
    namespace view
    {
        
        template<typename base_container_type, typename pointer_container_type>
        class view_t
        {
        public:
            typedef typename base_container_type::value_type value_type;
            typedef typename base_container_type::reference reference;
            typedef typename base_container_type::const_reference const_reference;
            
            class iterator : public pointer_container_type::iterator
            {
                typedef typename pointer_container_type::iterator base;
            public:
                iterator(const base & foo) : base(foo) {}
                
                value_type & operator* () { return *(base::operator*()); }
                const value_type & operator* () const { return *(base::operator*()); }
            };
            
            
            class const_iterator : public pointer_container_type::const_iterator
            {
                typedef typename pointer_container_type::iterator base;
            public:
                const_iterator(const base & foo) : base(foo) {}
                
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
            
            void push_back(const value_type & element)
            {
                base_container->push_back( element );
                container.push_back( &base_container->back() );
            }
            
            iterator erase(const iterator & it)
            {
                return container.erase( it );                
            }
            
            
        private:
            
            base_container_type * base_container;
            pointer_container_type container;
        };


        template<template<typename,typename> class pointer_container_type, class base_container_type>
        struct result_of
        {
            typedef typename base_container_type::value_type value_type;
            typedef view_t< base_container_type, typename std_container<pointer_container_type,value_type*>::type  > type;            
        };


        
    }
    
}

#endif