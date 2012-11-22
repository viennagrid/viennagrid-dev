#ifndef VIENNAGRID_STORAGE_CONTAINER_HPP
#define VIENNAGRID_STORAGE_CONTAINER_HPP

#include <vector>
#include <deque>
#include <list>
#include <set>

#include "viennagrid/meta/utils.hpp"
#include "viennagrid/storage/forwards.hpp"
#include "viennagrid/storage/hook.hpp"


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

            
            
            
            template<typename base_iterator, typename hook_tag>
            class hook_iterator_impl {};
            
            template<typename base_iterator>
            class hook_iterator_impl<base_iterator, pointer_hook_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef typename std::iterator_traits<base_iterator>::value_type * value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                hook_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return & base_iterator::operator*(); }
                const value_type operator* () const { return & base_iterator::operator*(); }
            };
            
            template<typename base_iterator>
            class hook_iterator_impl<base_iterator, iterator_hook_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef base_iterator value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                hook_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return static_cast<base_iterator>(*this); }
                const value_type operator* () const { return static_cast<base_iterator>(*this); }
            };
            
            template<typename base_iterator>
            class hook_iterator_impl<base_iterator, id_hook_tag> : public base_iterator
            {
            public:
                
                //typedef smart_id< typename std::iterator_traits<base_iterator>::value_type, base_id_type > id_type;
                //typedef typename viennagrid::storage::result_of::id<typename std::iterator_traits<base_iterator>::value_type, id_tag>::type id_type;
                typedef typename std::iterator_traits<base_iterator>::value_type::id_type id_type;
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef id_type value_type;
                typedef id_type * pointer;
                typedef id_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                hook_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return (*this)->id(); }
                const value_type operator* () const { return (*this)->id(); }
            };
            
            
            
            template<typename base_iterator, typename hook_tag>
            class const_hook_iterator_impl {};
            
            template<typename base_iterator>
            class const_hook_iterator_impl<base_iterator, pointer_hook_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef const typename std::iterator_traits<base_iterator>::value_type * value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                const_hook_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return & base_iterator::operator*(); }
                const value_type operator* () const { return & base_iterator::operator*(); }
            };
            
            template<typename base_iterator>
            class const_hook_iterator_impl<base_iterator, iterator_hook_tag> : public base_iterator
            {
            public:
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef base_iterator value_type;
                typedef value_type * pointer;
                typedef value_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                const_hook_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return static_cast<base_iterator>(*this); }
                const value_type operator* () const { return static_cast<base_iterator>(*this); }
            };
            
            template<typename base_iterator>
            class const_hook_iterator_impl<base_iterator, id_hook_tag> : public base_iterator
            {
            public:
                
                //typedef smart_id< typename std::iterator_traits<base_iterator>::value_type, base_id_type > id_type;
                //typedef typename viennagrid::storage::result_of::id<typename std::iterator_traits<base_iterator>::value_type, id_tag>::type id_type;
                typedef typename std::iterator_traits<base_iterator>::value_type::id_type id_type;
                
                typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
                typedef id_type value_type;
                typedef id_type * pointer;
                typedef id_type & reference;
                typedef typename std::iterator_traits<base_iterator>::iterator_category iterator_category;
                
                
                const_hook_iterator_impl(base_iterator it) : base_iterator(it) {}
                
                
                value_type operator* () { return (*this)->id(); }
                const value_type operator* () const { return (*this)->id(); }
            };
        
        }
        
        
        
        template<typename base_container__, typename hook_tag>
        class hooked_container_t : public base_container__
        {
        public:
            typedef base_container__ container_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename container_type::pointer pointer;
            typedef typename container_type::const_pointer const_pointer;            
            
            typedef typename container_type::reference reference;
            typedef typename container_type::const_reference const_reference;
            
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            
            typedef typename viennagrid::storage::hook::hook_type<container_type, hook_tag>::type hook_type;
            typedef typename viennagrid::storage::hook::const_hook_type<container_type, hook_tag>::type const_hook_type;
            
            value_type & dereference_hook( hook_type hook )
            {
                return *hook;
            }
            
//             const value_type & dereference_hook( const_hook_type hook )
//             {
//                 return *hook;
//             }
        };
        
        template<typename base_container__>
        class hooked_container_t<base_container__, id_hook_tag> : public base_container__
        {
        public:
            typedef base_container__ container_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename container_type::pointer pointer;
            typedef typename container_type::const_pointer const_pointer;            
            
            typedef typename container_type::reference reference;
            typedef typename container_type::const_reference const_reference;
            
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            
            typedef typename viennagrid::storage::hook::hook_type<container_type, id_hook_tag>::type hook_type;
            typedef typename viennagrid::storage::hook::const_hook_type<container_type, id_hook_tag>::type const_hook_type;
            
            value_type & dereference_hook( hook_type hook )
            {
                iterator it = container_type::begin();
                std::advance(it, hook.get());
                return *it;
                //return *hook;
            }
            
//             const value_type & dereference_hook( const_hook_type hook )
//             {
//                 return *hook;
//             }
        };
        
        
        
        
        template<typename base_container__, typename hook_tag>
        class container_base_t : public hooked_container_t<base_container__, hook_tag>
        {
        public:

            typedef hooked_container_t<base_container__, hook_tag> hooked_container_type;
            typedef typename hooked_container_type::container_type container_type;
            
            typedef typename hooked_container_type::value_type value_type;
            
            typedef typename hooked_container_type::pointer pointer;
            typedef typename hooked_container_type::const_pointer const_pointer;            
            
            typedef typename hooked_container_type::reference reference;
            typedef typename hooked_container_type::const_reference const_reference;
            
            typedef typename hooked_container_type::iterator iterator;
            typedef typename hooked_container_type::const_iterator const_iterator;
            
            typedef typename hooked_container_type::hook_type hook_type;
            typedef typename hooked_container_type::const_hook_type const_hook_type;
            
            typedef std::pair<hook_type, bool> return_type;
            

            bool is_present( const value_type & element ) const
            {
                return false;
            }
            
            iterator find( const value_type & element ) const
            {
                return container_type::end();
            }
            
            return_type insert( const value_type & element )
            {
                container_type::push_back( element );
                return std::make_pair( viennagrid::storage::hook::iterator_to_hook<container_type, hook_tag>::convert(--container_type::end()), true);
            }
        };
        
        
        template<typename key, typename compare, typename allocator, typename hook_tag>
        class container_base_t<std::set<key, compare, allocator>, hook_tag> : public hooked_container_t<std::set<key, compare, allocator>, hook_tag>
        {
        public:
            
            typedef hooked_container_t<std::set<key, compare, allocator>, hook_tag> hooked_container_type;
            typedef typename hooked_container_type::container_type container_type;
            
            typedef typename hooked_container_type::value_type value_type;
            
            typedef typename hooked_container_type::pointer pointer;
            typedef typename hooked_container_type::const_pointer const_pointer;            
            
            typedef typename hooked_container_type::reference reference;
            typedef typename hooked_container_type::const_reference const_reference;
            
            typedef typename hooked_container_type::iterator iterator;
            typedef typename hooked_container_type::const_iterator const_iterator;
            
            typedef typename hooked_container_type::hook_type hook_type;
            typedef typename hooked_container_type::const_hook_type const_hook_type;
            
            typedef std::pair<hook_type, bool> return_type;
            
            bool is_present( const value_type & element ) const
            {
                return container_type::find(element) != container_type::end();
            }
            
            typename container_type::iterator find( const value_type & element ) const
            {
                return container_type::find(element);
            }
            
            return_type insert( const value_type & element )
            {
                std::pair<typename container_type::iterator, bool> tmp = container_type::insert( element );
                return std::make_pair( viennagrid::storage::hook::iterator_to_hook<container_type, hook_tag>::convert(tmp.first), tmp.second);
            }
        };
        
        
        
        template<typename base_container__, typename hook_tag__>
        class container_t : public container_base_t<base_container__, hook_tag__>
        {
        public:
            
            typedef base_container__ base_container;
            
            typedef hook_tag__ hook_tag;
            typedef typename hook::hook_type<base_container, hook_tag>::type hook_type;
            typedef typename hook::const_hook_type<base_container, hook_tag>::type const_hook_type;
            typedef std::pair<hook_type, bool> insert_return_type;


            typedef typename base_container::value_type value_type;
            
            typedef container::hook_iterator_impl<typename base_container::iterator, hook_tag> hook_iterator; 
            hook_iterator hook_begin() { return hook_iterator(base_container::begin()); }
            hook_iterator hook_end() { return hook_iterator(base_container::end()); }
            
            typedef container::hook_iterator_impl<typename base_container::const_iterator, hook_tag> const_hook_iterator; 
            const_hook_iterator hook_begin() const { return hook_iterator(base_container::begin()); }
            const_hook_iterator hook_end() const { return hook_iterator(base_container::end()); }
            
            hook_type hook_at(std::size_t pos)
            {
                hook_iterator it = hook_begin();
                std::advance( it, pos );
                return *it;
            }
            const_hook_type hook_at(std::size_t pos) const
            {
                hook_iterator it = hook_begin();
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
            
            
            template<typename value_type, typename container_tag, typename hook_tag>
            struct container<value_type, hooked_container_tag<container_tag, hook_tag> >
            {
                typedef container_t< typename container<value_type, container_tag>::type, hook_tag > type;
            };
            

        }
        
        
    
    }
}
        
#endif