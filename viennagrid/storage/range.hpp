#ifndef VIENNAGRID_STORAGE_RANGE_HPP
#define VIENNAGRID_STORAGE_RANGE_HPP



namespace viennagrid
{
    
    namespace storage
    {
        
        
        template<typename container_type>
        class container_range_wrapper
        {
        public:
            
            container_range_wrapper(container_type & _container) : container(_container) {}
            
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
            
            
            
            iterator begin() { return container.begin(); }
            const_iterator begin() const { return container.begin(); }
            iterator end() { return container.end(); }
            const_iterator end() const { return container.end(); }
            
            reverse_iterator rbegin() { return container.rbegin(); }
            const_reverse_iterator rbegin() const { return container.rbegin(); }
            reverse_iterator rend() { return container.rend(); }
            const_reverse_iterator rend() const { return container.rend(); }
            
            
            
            reference front() { return container.front(); }
            const_reference front() const { return container.front(); }
            reference back() { return container.back(); }
            const_reference back() const { return container.back(); }
            
            
            
            reference operator[] (size_type index) { return container[index]; }
            const_reference operator[] (size_type index) const { return container[index]; }
            
            
            
            bool empty() const { return container.empty(); }
            size_type size() const { return container.size(); }
            
            
        private:
            
            container_type & container;
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

