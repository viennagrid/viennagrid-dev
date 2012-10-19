#ifndef VIENNAGRID_STORAGE_STATIC_ARRAY_HPP
#define VIENNAGRID_STORAGE_STATIC_ARRAY_HPP

#include "viennagrid/storage/container.hpp"

namespace viennagrid
{
    namespace storage
    {
        template<class T, std::size_t N>
        class static_array {
        public:
            T elems[N];    // fixed-size array of elements of type T

        public:
            // type definitions
            typedef T              value_type;
            typedef T*             pointer;
            typedef const T*       const_pointer;
            
            typedef T&             reference;
            typedef const T&       const_reference;
            typedef std::size_t    size_type;
            typedef std::ptrdiff_t difference_type;
            
            class const_iterator;
            
            
            // TODO: implement random access operation: http://www.cplusplus.com/reference/std/iterator/RandomAccessIterator/
            class iterator
            {
                friend class const_iterator;
            public:
                
                typedef std::size_t difference_type;
                typedef static_array::value_type value_type;
                typedef static_array::pointer pointer;
                typedef static_array::reference reference;
                typedef std::random_access_iterator_tag iterator_category;
                
                iterator(pointer ptr__) : ptr_(ptr__) {}
                
                
                reference operator*() const { return *ptr_; }
                pointer operator->() const { return ptr_; }

                iterator & operator++() { ++ptr_; return *this; }
                iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }
                
                iterator & operator--() { --ptr_; return *this; }
                iterator operator--(int) { iterator tmp = *this; --*this; return tmp; }

                bool operator==(const iterator& i) const { return ptr_ == i.ptr_; }
                bool operator!=(const iterator& i) const { return ptr_ != i.ptr_; }
                
                bool operator==(const const_iterator & it) const;
                bool operator!=(const const_iterator & it) const;
                
            private:
                pointer ptr_;
            };
            
            // TODO: implement random access operation: http://www.cplusplus.com/reference/std/iterator/RandomAccessIterator/
            class const_iterator
            {
                friend class iterator;
            public:
                
                typedef std::size_t difference_type;
                typedef static_array::value_type value_type;
                typedef static_array::const_pointer pointer;
                typedef static_array::const_reference reference;
                typedef std::random_access_iterator_tag iterator_category;
                
                
                const_iterator( iterator it ) : ptr_(it.ptr_) {}
                const_iterator(const_pointer ptr__) : ptr_(ptr__) {}
                
                
                reference operator*() const { return *ptr_; }
                pointer operator->() const { return ptr_; }

                const_iterator & operator++() { ++ptr_; return *this; }
                const_iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }
                
                const_iterator & operator--() { --ptr_; return *this; }
                const_iterator operator--(int) { iterator tmp = *this; --*this; return tmp; }

                bool operator==(const const_iterator& i) const { return ptr_ == i.ptr_; }
                bool operator!=(const const_iterator& i) const { return ptr_ != i.ptr_; }
                
                bool operator==(const iterator & i) const { return ptr_ == i.ptr_; }
                bool operator!=(const iterator & i) const { return ptr_ != i.ptr_; }
                
            private:
                pointer ptr_;
            };

            // iterator support
            iterator        begin()       { return iterator(elems); }
            const_iterator  begin() const { return const_iterator(elems); }
            const_iterator cbegin() const { return const_iterator(elems); }
            
            iterator        end()       { return iterator(elems+N); }
            const_iterator  end() const { return const_iterator(elems+N); }
            const_iterator cend() const { return const_iterator(elems+N); }

            // reverse iterator support
            typedef std::reverse_iterator<iterator> reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

            reverse_iterator rbegin() { return reverse_iterator(end()); }
            const_reverse_iterator rbegin() const {
                return const_reverse_iterator(end());
            }
            const_reverse_iterator crbegin() const {
                return const_reverse_iterator(end());
            }

            reverse_iterator rend() { return reverse_iterator(begin()); }
            const_reverse_iterator rend() const {
                return const_reverse_iterator(begin());
            }
            const_reverse_iterator crend() const {
                return const_reverse_iterator(begin());
            }

            // operator[]
            reference operator[](size_type i) 
            { 
                assert( i < N && "out of range" ); 
                return elems[i];
            }
            
            const_reference operator[](size_type i) const 
            {     
                assert( i < N && "out of range" ); 
                return elems[i]; 
            }

            // at() with range check
            reference at(size_type i) { assert( i < N && "out of range" );  return elems[i]; }
            const_reference at(size_type i) const { assert( i < N && "out of range" );  return elems[i]; }
        
            // front() and back()
            reference front() 
            {
                return elems[0]; 
            }
            
            const_reference front() const 
            {
                return elems[0];
            }
            
            reference back() 
            { 
                return elems[N-1]; 
            }
            
            const_reference back() const 
            { 
                return elems[N-1]; 
            }

            // size is constant
            static size_type size() { return N; }
            static void resize(size_type s) { assert( s == N ); }
            
            static bool empty() { return false; }
            static size_type max_size() { return N; }
            enum { static_size = N };

            // swap (note: linear complexity)
            void swap (static_array<T,N>& y) {
                for (size_type i = 0; i < N; ++i)
                    std::swap(elems[i],y.elems[i]);
            }

            // direct access to data (read-only)
            const T* data() const { return elems; }
            T* data() { return elems; }

            // use static_array as C array (direct read/write access to data)
            T* c_array() { return elems; }

            // assignment with type conversion
            template <typename T2>
            static_array<T,N>& operator= (const static_array<T2,N>& rhs) {
                std::copy(rhs.begin(),rhs.end(), begin());
                return *this;
            }

            // assign one value to all elements
            void assign (const T& value) { fill ( value ); }    // A synonym for fill
            void fill   (const T& value)
            {
                std::fill_n(begin(),size(),value);
            }

        };

        // comparisons
        template<class T, std::size_t N>
        bool operator== (const static_array<T,N>& x, const static_array<T,N>& y) {
            return std::equal(x.begin(), x.end(), y.begin());
        }
        template<class T, std::size_t N>
        bool operator< (const static_array<T,N>& x, const static_array<T,N>& y) {
            return std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
        }
        template<class T, std::size_t N>
        bool operator!= (const static_array<T,N>& x, const static_array<T,N>& y) {
            return !(x==y);
        }
        template<class T, std::size_t N>
        bool operator> (const static_array<T,N>& x, const static_array<T,N>& y) {
            return y<x;
        }
        template<class T, std::size_t N>
        bool operator<= (const static_array<T,N>& x, const static_array<T,N>& y) {
            return !(y<x);
        }
        template<class T, std::size_t N>
        bool operator>= (const static_array<T,N>& x, const static_array<T,N>& y) {
            return !(x<y);
        }

        // global swap()
        template<class T, std::size_t N>
        inline void swap (static_array<T,N>& x, static_array<T,N>& y) {
            x.swap(y);
        }
        
        
        
        
        
        template<long size__>
        struct static_array_tag
        {
            enum { size = size__ };
        };
        
        
        namespace result_of
        {
            template<typename element_type, long size>
            struct container_from_tag<element_type, static_array_tag<size> >
            {
                typedef static_array<element_type, size> type;
            };
        }
        
        
    }
    
}

#endif
