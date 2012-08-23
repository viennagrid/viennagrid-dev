#ifndef VIENNAGRID_STATIC_ARRAY_HPP
#define VIENNAGRID_STATIC_ARRAY_HPP

namespace viennagrid
{
    namespace utils
    {
        template<class T, std::size_t N>
        class static_array {
        public:
            T elems[N];    // fixed-size array of elements of type T

        public:
            // type definitions
            typedef T              value_type;
            typedef T*             iterator;
            typedef const T*       const_iterator;
            typedef T&             reference;
            typedef const T&       const_reference;
            typedef std::size_t    size_type;
            typedef std::ptrdiff_t difference_type;

            // iterator support
            iterator        begin()       { return elems; }
            const_iterator  begin() const { return elems; }
            const_iterator cbegin() const { return elems; }
            
            iterator        end()       { return elems+N; }
            const_iterator  end() const { return elems+N; }
            const_iterator cend() const { return elems+N; }

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
        
        
    }
    
}

#endif
