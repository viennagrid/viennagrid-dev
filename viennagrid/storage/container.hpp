#ifndef VIENNAGRID_STORAGE_CONTAINER_HPP
#define VIENNAGRID_STORAGE_CONTAINER_HPP

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <algorithm>

#include "viennagrid/meta/utils.hpp"
#include "viennagrid/storage/forwards.hpp"
#include "viennagrid/storage/handle.hpp"


namespace viennagrid
{
    namespace storage
    {
        namespace container
        {
            
            template<typename container_type>
            void insert(container_type & container, const typename container_type::value_type & value)
            {
                container.push_back(value);
            }

            template<typename type, typename compare, typename allocator>
            void insert(std::set<type, compare, allocator> & container, const typename std::set<type, compare, allocator>::value_type & value)
            {
                container.insert(value);
            }
            
            
            
            
            
            template<typename base_iterator, typename base_const_iterator, typename handle_tag>
            class iterator;
            
            template<typename base_iterator, typename base_const_iterator>
            class iterator<base_iterator, base_const_iterator, no_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                iterator() {}
                iterator(base_iterator const & it) : base_iterator(it) {}
                
                iterator & operator++() { base_iterator::operator++(); return *this; }
                iterator operator++(int) { base_iterator::operator++(int()); return *this; }
                
                iterator & operator--() { base_iterator::operator--(); return *this; }
                iterator operator--(int) { base_iterator::operator--(int()); return *this; }
                                
                value_type & value() { return base_iterator::operator*(); }
                value_type const &  value() const { return base_iterator::operator*(); }
                
                value_type & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            template<typename base_iterator, typename base_const_iterator>
            class iterator<base_iterator, base_const_iterator, pointer_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                typedef value_type * handle_type;
                typedef value_type const * const_handle_type;
                
                iterator() {}
                iterator(base_iterator const & it) : base_iterator(it) {}
                
                iterator & operator++() { base_iterator::operator++(); return *this; }
                iterator operator++(int) { base_iterator::operator++(int()); return *this; }
                
                iterator & operator--() { base_iterator::operator--(); return *this; }
                iterator operator--(int) { base_iterator::operator--(int()); return *this; }
                
                value_type & value() { return base_iterator::operator*(); }
                value_type const &  value() const { return base_iterator::operator*(); }
                
                handle_type handle() { return &value(); }
                const_handle_type handle() const { return &value(); }
                
                
                value_type & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            template<typename base_iterator, typename base_const_iterator>
            class iterator<base_iterator, base_const_iterator, iterator_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                typedef base_iterator handle_type;
                typedef base_const_iterator const_handle_type;
                
                iterator() {}
                iterator(base_iterator const & it) : base_iterator(it) {}
                
                iterator & operator++() { base_iterator::operator++(); return *this; }
                iterator operator++(int) { base_iterator::operator++(int()); return *this; }
                
                iterator & operator--() { base_iterator::operator--(); return *this; }
                iterator operator--(int) { base_iterator::operator--(int()); return *this; }
                
                value_type & value() { return base_iterator::operator*(); }
                value_type const &  value() const { return base_iterator::operator*(); }
                
                handle_type handle() { return static_cast<base_iterator>(*this); }
                const_handle_type handle() const { return static_cast<base_iterator>(*this); }
                
                
                value_type & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            template<typename base_iterator, typename base_const_iterator>
            class iterator<base_iterator, base_const_iterator, id_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::value_type::id_type id_type;
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                typedef id_type handle_type;
                typedef typename result_of::const_id<id_type>::type const_handle_type;
                
                iterator() {}
                iterator(base_iterator const & it) : base_iterator(it) {}
                
                iterator & operator++() { base_iterator::operator++(); return *this; }
                iterator operator++(int) { base_iterator::operator++(int()); return *this; }
                
                iterator & operator--() { base_iterator::operator--(); return *this; }
                iterator operator--(int) { base_iterator::operator--(int()); return *this; }
                
                value_type & value() { return base_iterator::operator*(); }
                value_type const &  value() const { return base_iterator::operator*(); }
                
                handle_type handle() { return base_iterator::operator*().id(); }
                const_handle_type handle() const { return base_iterator::operator*().id(); }
                
                
                value_type & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            
            
            
            template<typename base_iterator, typename base_const_iterator, typename handle_tag>
            class const_iterator;
            
            template<typename base_iterator, typename base_const_iterator>
            class const_iterator<base_iterator, base_const_iterator, no_handle_tag> : public base_const_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_const_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_const_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_const_iterator>::iterator_category iterator_category;
                
                const_iterator() {}
                const_iterator(base_const_iterator const & it) : base_const_iterator(it) {}
                
                const_iterator & operator++() { base_const_iterator::operator++(); return *this; }
                const_iterator operator++(int) { base_const_iterator::operator++(int()); return *this; }
                
                const_iterator & operator--() { base_const_iterator::operator--(); return *this; }
                const_iterator operator--(int) { base_const_iterator::operator--(int()); return *this; }
                
                value_type const & value() { return base_const_iterator::operator*(); }
                value_type const &  value() const { return base_const_iterator::operator*(); }
                
                value_type const & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            template<typename base_iterator, typename base_const_iterator>
            class const_iterator<base_iterator, base_const_iterator, pointer_handle_tag> : public base_const_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_const_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_const_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_const_iterator>::iterator_category iterator_category;
                
                typedef value_type * handle_type;
                typedef value_type const * const_handle_type;
                
                const_iterator() {}
                const_iterator(base_const_iterator const & it) : base_const_iterator(it) {}
                
                const_iterator & operator++() { base_const_iterator::operator++(); return *this; }
                const_iterator operator++(int) { base_const_iterator::operator++(int()); return *this; }
                
                const_iterator & operator--() { base_const_iterator::operator--(); return *this; }
                const_iterator operator--(int) { base_const_iterator::operator--(int()); return *this; }
                
                value_type const & value() { return base_const_iterator::operator*(); }
                value_type const & value() const { return base_const_iterator::operator*(); }
                
                const_handle_type handle() { return &value(); }
                const_handle_type handle() const { return &value(); }
                
                
                value_type const & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            template<typename base_iterator, typename base_const_iterator>
            class const_iterator<base_iterator, base_const_iterator, iterator_handle_tag> : public base_const_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_const_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_const_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_const_iterator>::iterator_category iterator_category;
                
                typedef base_iterator handle_type;
                typedef base_const_iterator const_handle_type;
                
                const_iterator() {}
                const_iterator(base_const_iterator const & it) : base_iterator(it) {}
                
                const_iterator & operator++() { base_const_iterator::operator++(); return *this; }
                const_iterator operator++(int) { base_const_iterator::operator++(int()); return *this; }
                
                const_iterator & operator--() { base_const_iterator::operator--(); return *this; }
                const_iterator operator--(int) { base_const_iterator::operator--(int()); return *this; }
                                
                value_type const & value() { return base_const_iterator::operator*(); }
                value_type const & value() const { return base_const_iterator::operator*(); }
                
                const_handle_type handle() { return static_cast<base_const_iterator>(*this); }
                const_handle_type handle() const { return static_cast<base_const_iterator>(*this); }
                
                
                value_type const & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            template<typename base_iterator, typename base_const_iterator>
            class const_iterator<base_iterator, base_const_iterator, id_handle_tag> : public base_const_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_const_iterator>::value_type::id_type id_type;
                
                typedef typename std::iterator_traits<base_const_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_const_iterator>::value_type value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_const_iterator>::iterator_category iterator_category;
                
                typedef id_type handle_type;
                typedef typename result_of::const_id<id_type>::type const_handle_type;
                
                const_iterator() {}
                const_iterator(base_const_iterator const & it) : base_const_iterator(it) {}
                
                const_iterator & operator++() { base_const_iterator::operator++(); return *this; }
                const_iterator operator++(int) { base_const_iterator::operator++(int()); return *this; }
                
                const_iterator & operator--() { base_const_iterator::operator--(); return *this; }
                const_iterator operator--(int) { base_const_iterator::operator--(int()); return *this; }
                
                value_type const & value() { return base_const_iterator::operator*(); }
                value_type const & value() const { return base_const_iterator::operator*(); }
                
                const_handle_type handle() { return value().id(); }
                const_handle_type handle() const { return value().id(); }
                
                
                value_type const & operator* () { return value(); }
                value_type const & operator* () const { return value(); }
            };
            
            
            
            
            
            
            template<typename base_iterator, typename handle_tag>
            class handle_iterator_impl {};
            
            template<typename base_iterator>
            class handle_iterator_impl<base_iterator, pointer_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_iterator>::value_type * value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                handle_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return & base_iterator::operator*(); }
                value_type const operator* () const { return & base_iterator::operator*(); }
            };
            
            template<typename base_iterator>
            class handle_iterator_impl<base_iterator, iterator_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef base_iterator value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                handle_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return static_cast<base_iterator>(*this); }
                value_type const operator* () const { return static_cast<base_iterator>(*this); }
            };
            
            template<typename base_iterator>
            class handle_iterator_impl<base_iterator, id_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::value_type::id_type id_type;
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef id_type value_type;
                typedef id_type * pointer;
                typedef id_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                handle_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return base_iterator::operator*().id(); }
                value_type const operator* () const { return base_iterator::operator*().id(); }
            };
            
            
            
            template<typename base_iterator, typename handle_tag>
            class const_handle_iterator_impl {};
            
            template<typename base_iterator>
            class const_handle_iterator_impl<base_iterator, pointer_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef const typename std::iterator_traits<base_iterator>::value_type * value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                const_handle_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return & base_iterator::operator*(); }
                value_type const operator* () const { return & base_iterator::operator*(); }
            };
            
            template<typename base_iterator>
            class const_handle_iterator_impl<base_iterator, iterator_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef base_iterator value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                const_handle_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return static_cast<base_iterator>(*this); }
                value_type const operator* () const { return static_cast<base_iterator>(*this); }
            };
            
            template<typename base_iterator>
            class const_handle_iterator_impl<base_iterator, id_handle_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::value_type::id_type id_type;
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef id_type value_type;
                typedef id_type * pointer;
                typedef id_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                const_handle_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return base_iterator::operator*().id(); }
                value_type const operator* () const { return base_iterator::operator*().id(); }
            };
        
        }
        
        
        
        template<typename base_container_, typename handle_tag>
        class handled_container_t : public base_container_
        {
        public:
            typedef base_container_ container_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename container_type::pointer pointer;
            typedef typename container_type::const_pointer const_pointer;            
            
            typedef typename container_type::reference reference;
            typedef typename container_type::const_reference const_reference;
            
            typedef typename handle::result_of::handle_type<container_type, handle_tag>::type handle_type;
            typedef typename handle::result_of::const_handle_type<container_type, handle_tag>::type const_handle_type;
            
            
            
            handle_type handle( value_type & element )
            {
                return handle::handle( *this, element, handle_tag() );
            }

            const_handle_type handle( value_type const & element ) const
            {
                return handle::handle( *this, element, handle_tag() );
            }

            value_type & dereference_handle( handle_type handle )
            {
                return handle::dereference_handle( *this, handle, handle_tag() );
            }
            
            value_type const & dereference_handle( const_handle_type handle ) const
            {
                return handle::dereference_handle( *this, handle, handle_tag() );
            }
            
        };
        
        
        
        template<typename base_container_, typename handle_tag>
        class container_base_t : public handled_container_t<base_container_, handle_tag>
        {
        public:

            typedef handled_container_t<base_container_, handle_tag> handled_container_type;
            typedef typename handled_container_type::container_type container_type;
            
            typedef typename handled_container_type::value_type value_type;
            
            typedef typename handled_container_type::pointer pointer;
            typedef typename handled_container_type::const_pointer const_pointer;            
            
            typedef typename handled_container_type::reference reference;
            typedef typename handled_container_type::const_reference const_reference;
            
            typedef typename handled_container_type::iterator iterator;
            typedef typename handled_container_type::const_iterator const_iterator;
            
            typedef typename handled_container_type::handle_type handle_type;
            typedef typename handled_container_type::const_handle_type const_handle_type;
            
            typedef std::pair<handle_type, bool> return_type;
            

            bool is_present( value_type const & element ) const
            {
                return false;
            }
            
            iterator find( value_type const & element ) const
            {
                return container_type::end();
            }
            
            return_type insert( value_type const & element )
            {
                container_type::push_back( element );
                return std::make_pair( this->handle(container_type::back()), true);
            }
        };
        
        
        template<typename key, typename compare, typename allocator, typename handle_tag>
        class container_base_t<std::set<key, compare, allocator>, handle_tag> : public handled_container_t<std::set<key, compare, allocator>, handle_tag>
        {
        public:
            
            typedef handled_container_t<std::set<key, compare, allocator>, handle_tag> handled_container_type;
            typedef typename handled_container_type::container_type container_type;
            
            typedef typename handled_container_type::value_type value_type;
            
            typedef typename handled_container_type::pointer pointer;
            typedef typename handled_container_type::const_pointer const_pointer;            
            
            typedef typename handled_container_type::reference reference;
            typedef typename handled_container_type::const_reference const_reference;
            
            typedef typename handled_container_type::iterator iterator;
            typedef typename handled_container_type::const_iterator const_iterator;
            
            typedef typename handled_container_type::handle_type handle_type;
            typedef typename handled_container_type::const_handle_type const_handle_type;
            
            typedef std::pair<handle_type, bool> return_type;
            
            bool is_present( value_type const & element ) const
            {
                return container_type::find(element) != container_type::end();
            }
            
            typename container_type::iterator find( value_type const & element ) const
            {
                return container_type::find(element);
            }
            
            return_type insert( value_type const & element )
            {
                std::pair<typename container_type::iterator, bool> tmp = container_type::insert( element );
                return std::make_pair( handled_container_type::handle(*tmp.first), tmp.second);
            }
        };
        
        
        
        template<typename base_container_, typename handle_tag_>
        class container_t : public container_base_t<base_container_, handle_tag_>
        {
        public:
            
            typedef base_container_ base_container;
            
            typedef handle_tag_ handle_tag;
            typedef typename handle::result_of::handle_type<base_container, handle_tag>::type handle_type;
            typedef typename handle::result_of::const_handle_type<base_container, handle_tag>::type const_handle_type;
            typedef std::pair<handle_type, bool> insert_return_type;


            typedef typename base_container::value_type value_type;
            
            
            typedef container::iterator<typename base_container::iterator, typename base_container::const_iterator, handle_tag> iterator;
            iterator begin() { return iterator(base_container::begin()); }
            iterator end() { return iterator(base_container::end()); }
            
            typedef container::const_iterator<typename base_container::iterator, typename base_container::const_iterator, handle_tag> const_iterator;
            const_iterator cbegin() const { return const_iterator(base_container::begin()); }
            const_iterator cend() const { return const_iterator(base_container::end()); }
            
            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }

            
            
            typedef container::handle_iterator_impl<typename base_container::iterator, handle_tag> handle_iterator; 
            handle_iterator handle_begin() { return handle_iterator(base_container::begin()); }
            handle_iterator handle_end() { return handle_iterator(base_container::end()); }
            
            typedef container::const_handle_iterator_impl<typename base_container::const_iterator, handle_tag> const_handle_iterator; 
            const_handle_iterator handle_begin() const { return const_handle_iterator(base_container::begin()); }
            const_handle_iterator handle_end() const { return const_handle_iterator(base_container::end()); }
            
            handle_type handle_at(std::size_t pos)
            {
                handle_iterator it = handle_begin();
                std::advance( it, pos );
                return *it;
            }
            const_handle_type handle_at(std::size_t pos) const
            {
                const_handle_iterator it = handle_begin();
                std::advance( it, pos );
                return *it;
            }

            
            
        private:
        };
        
        
        namespace result_of
        {           
            template<typename value_type, typename container_tag>
            struct container;

            template<typename value_type>
            struct container<value_type, std_vector_tag>
            {
                typedef std::vector<value_type> type;
            };

            template<typename value_type>
            struct container<value_type, std_deque_tag>
            {
                typedef std::deque<value_type> type;
            };

            template<typename value_type>
            struct container<value_type, std_list_tag>
            {
                typedef std::list<value_type> type;
            };

            template<typename value_type>
            struct container<value_type, std_set_tag>
            {
                typedef std::set<value_type> type;
            };


            template<typename value_type, typename container_tag, typename handle_tag>
            struct container<value_type, handled_container_tag<container_tag, handle_tag> >
            {
                typedef container_t< typename container<value_type, container_tag>::type, handle_tag > type;
            };

            // vector
            template<typename value_type>
            struct container<value_type, std_vector_with_iterator_handle_tag >
            {
                typedef container_t< typename container<value_type, std_vector_tag>::type, iterator_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_vector_with_pointer_handle_tag >
            {
                typedef container_t< typename container<value_type, std_vector_tag>::type, pointer_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_vector_with_id_handle_tag >
            {
                typedef container_t< typename container<value_type, std_vector_tag>::type, id_handle_tag > type;
            };

            // deque
            template<typename value_type>
            struct container<value_type, std_deque_with_iterator_handle_tag >
            {
                typedef container_t< typename container<value_type, std_deque_tag>::type, iterator_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_deque_with_pointer_handle_tag >
            {
                typedef container_t< typename container<value_type, std_deque_tag>::type, pointer_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_deque_with_id_handle_tag >
            {
                typedef container_t< typename container<value_type, std_deque_tag>::type, id_handle_tag > type;
            };

            // list
            template<typename value_type>
            struct container<value_type, std_list_with_iterator_handle_tag >
            {
                typedef container_t< typename container<value_type, std_list_tag>::type, iterator_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_list_with_pointer_handle_tag >
            {
                typedef container_t< typename container<value_type, std_list_tag>::type, pointer_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_list_with_id_handle_tag >
            {
                typedef container_t< typename container<value_type, std_list_tag>::type, id_handle_tag > type;
            };

            // set
            template<typename value_type>
            struct container<value_type, std_set_with_iterator_handle_tag >
            {
                typedef container_t< typename container<value_type, std_set_tag>::type, iterator_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_set_with_pointer_handle_tag >
            {
                typedef container_t< typename container<value_type, std_set_tag>::type, pointer_handle_tag > type;
            };

            template<typename value_type>
            struct container<value_type, std_set_with_id_handle_tag >
            {
                typedef container_t< typename container<value_type, std_set_tag>::type, id_handle_tag > type;
            };
        }
        
        
    
    }
}
        
#endif
