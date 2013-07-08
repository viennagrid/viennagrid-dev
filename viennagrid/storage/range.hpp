#ifndef VIENNAGRID_STORAGE_RANGE_HPP
#define VIENNAGRID_STORAGE_RANGE_HPP

#include "viennagrid/forwards.hpp"

namespace viennagrid
{
    
    namespace storage
    {
        
        
        template<typename container_type>
        class container_range_wrapper
        {
            friend class container_range_wrapper<const container_type>;
            
        public:
            
            typedef container_type base_container_type;
            
            typedef typename container_type::size_type size_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename container_type::reference reference;
            typedef typename container_type::const_reference const_reference;
            
            typedef typename container_type::pointer pointer;
            typedef typename container_type::const_pointer const_pointer;
            
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            
            typedef typename container_type::reverse_iterator reverse_iterator;
            typedef typename container_type::const_reverse_iterator const_reverse_iterator;            
            

            container_range_wrapper(container_type & container_) : container(&container_) {}
                        
            template<typename element_domain_segment_config_or_something_like_that>
            container_range_wrapper( viennagrid::element_range_proxy<element_domain_segment_config_or_something_like_that> range_proxy )
            { *this = elements< value_type >( range_proxy() ); }
            
            
            
            iterator begin() { return container->begin(); }
            const_iterator begin() const { return container->begin(); }
            iterator end() { return container->end(); }
            const_iterator end() const { return container->end(); }
            
            reverse_iterator rbegin() { return container->rbegin(); }
            const_reverse_iterator rbegin() const { return container->rbegin(); }
            reverse_iterator rend() { return container->rend(); }
            const_reverse_iterator rend() const { return container->rend(); }
            
            
            
            reference front() { return container->front(); }
            const_reference front() const { return container->front(); }
            reference back() { return container->back(); }
            const_reference back() const { return container->back(); }
            
            
            
            reference operator[] (size_type index) { return (*container)[index]; }
            const_reference operator[] (size_type index) const { return (*container)[index]; }
            
            
            
            bool empty() const { return container->empty(); }
            size_type size() const { return container->size(); }
            
            

            
            
            
            
            typedef typename container_type::handle_type handle_type;
            typedef typename container_type::handle_iterator handle_iterator;
            handle_iterator handle_begin() { return container->handle_begin(); }
            handle_iterator handle_end() { return container->handle_end(); }

            typedef typename container_type::const_handle_type const_handle_type;
            typedef typename container_type::const_handle_iterator const_handle_iterator;
            const_handle_iterator handle_begin() const { return container->handle_begin(); }
            const_handle_iterator handle_end() const { return container->handle_end(); }
            
            
            
            iterator erase( iterator pos ) { return container->erase( pos ); }
            handle_iterator erase( handle_iterator pos ) { return handle_iterator(container->erase( pos )); }
            
            
            
            
            handle_type handle_at(std::size_t pos)
            { return *viennagrid::advance(handle_begin(), pos); }
            const_handle_type handle_at(std::size_t pos) const
            { return *viennagrid::advance(handle_begin(), pos); }
            

            void erase_handle(handle_type handle)
            { container->erase_handle(handle); }
            void insert_unique_handle(handle_type handle)
            { container->insert_unique_handle(handle); }
            void insert_handle(handle_type handle)
            { container->insert_handle(handle); }
            void set_handle_at(handle_type handle, std::size_t pos)
            { container->set_handle(handle, pos); }

            container_type * get_base_container() { return container; }
            const container_type * get_base_container() const { return container; }

        private:
            
            container_type * container;
        };
        
        template<typename container_type>
        class container_range_wrapper<const container_type>
        {
        public:
            
            typedef const container_type base_container_type;
            
            typedef typename container_type::size_type size_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename container_type::const_reference reference;
            typedef typename container_type::const_reference const_reference;
            
            typedef typename container_type::const_pointer pointer;
            typedef typename container_type::const_pointer const_pointer;
            
            typedef typename container_type::const_iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            
            typedef typename container_type::const_reverse_iterator reverse_iterator;
            typedef typename container_type::const_reverse_iterator const_reverse_iterator;
            
            
            container_range_wrapper(const container_type & _container) : container(&_container) {}
            container_range_wrapper(const container_range_wrapper<container_type> & rhs) : container(rhs.container) {}
            
            template<typename element_domain_segment_config_or_something_like_that>
            container_range_wrapper( const viennagrid::element_range_proxy<element_domain_segment_config_or_something_like_that> range_proxy )
            { *this = elements< value_type >( range_proxy() ); }
            template<typename element_domain_segment_config_or_something_like_that>
            container_range_wrapper( const viennagrid::element_range_proxy<const element_domain_segment_config_or_something_like_that> range_proxy )
            { *this = elements< value_type >( range_proxy() ); }
            
            
            
            iterator begin() { return container->begin(); }
            const_iterator begin() const { return container->begin(); }
            iterator end() { return container->end(); }
            const_iterator end() const { return container->end(); }
            
            reverse_iterator rbegin() { return container->rbegin(); }
            const_reverse_iterator rbegin() const { return container->rbegin(); }
            reverse_iterator rend() { return container->rend(); }
            const_reverse_iterator rend() const { return container->rend(); }
            
            
            
            reference front() { return container->front(); }
            const_reference front() const { return container->front(); }
            reference back() { return container->back(); }
            const_reference back() const { return container->back(); }
            
            
            
            reference operator[] (size_type index) { return (*container)[index]; }
            const_reference operator[] (size_type index) const { return (*container)[index]; }
            
            
            
            bool empty() const { return container->empty(); }
            size_type size() const { return container->size(); }
            
            
            
            typedef typename container_type::const_handle_type handle_type;
            typedef typename container_type::const_handle_iterator handle_iterator;
            handle_iterator handle_begin() { return container->handle_begin(); }
            handle_iterator handle_end() { return container->handle_end(); }

            typedef typename container_type::const_handle_type const_handle_type;
            typedef typename container_type::const_handle_iterator const_handle_iterator;
            const_handle_iterator handle_begin() const { return container->handle_begin(); }
            const_handle_iterator handle_end() const { return container->handle_end(); }
            
            handle_type handle_at(std::size_t pos)
            { return *viennagrid::advance(handle_begin(), pos); }
            const_handle_type handle_at(std::size_t pos) const
            { return *viennagrid::advance(handle_begin(), pos); }
            
            const container_type * get_base_container() const { return container; }
            
        private:
            
            const container_type * container;
        };
        
        
        
        template<typename iterator_type>
        class forward_iterator_range
        {
        public:
            
            forward_iterator_range(iterator_type _first, iterator_type _last) : first(_first), last(_last) {}
            
            typedef typename iterator_type::T value_type;
            
            typedef typename iterator_type::Reference reference;
            typedef const typename iterator_type::Reference const_reference;
            
            typedef typename iterator_type::Pointer pointer;
            typedef const typename iterator_type::Pointer const_pointer;
            
            typedef iterator_type iterator;
            typedef const iterator_type const_iterator;
            
            
            
            iterator begin() { return first; }
            const_iterator begin() const { return first; }
            iterator end() { return last; }
            const_iterator end() const { return last; }
            
            
            reference front() { return *first; }
            const_reference front() const { return *first; }
            reference back() { iterator_type tmp = last; return *(--tmp); }
            const_reference back() const { iterator_type tmp = last; return *(--tmp); }
            
            
            
            bool empty() const { return first == last; }
            
            
            
        private:
            iterator_type first;
            iterator_type last;
        };

    }
    
}

#endif

