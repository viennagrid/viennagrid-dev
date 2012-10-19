#ifndef VIENNAGRID_STORAGE_HIDDEN_KEY_MAP_HPP
#define VIENNAGRID_STORAGE_HIDDEN_KEY_MAP_HPP

#include <map>
#include "container.hpp"

namespace viennagrid
{
    namespace storage
    {

        template<typename _key_type, typename _value_type>
        class hidden_key_map
        {
            typedef std::map< _key_type, _value_type > container_type;
            
        public:
            
            typedef _key_type key_type;
            typedef _value_type value_type;
            typedef typename container_type::size_type size_type;
            typedef value_type & reference;
            typedef const value_type & const_reference;
            typedef value_type * pointer;
            typedef const value_type * const_pointer;
            
            
            class iterator : public container_type::iterator
            {
                typedef typename container_type::iterator base;
            public:
                iterator(const base & foo) : base(foo) {}
                
                typedef hidden_key_map::value_type value_type;
                typedef hidden_key_map::reference reference;
                typedef hidden_key_map::pointer pointer;
                
                reference operator* () { return base::operator*().second; }
                const reference operator* () const { return base::operator*().second; }
            };
            
            class const_iterator : public container_type::const_iterator
            {
                typedef typename container_type::const_iterator base;
            public:
                const_iterator(const base & foo) : base(foo) {}
                
                typedef hidden_key_map::value_type value_type;
                typedef hidden_key_map::const_reference reference;
                typedef hidden_key_map::const_pointer pointer;
                
                const reference operator* () const { return base::operator*().second; }
            };
            
            
            iterator begin() { return iterator(container.begin()); }
            iterator end() { return iterator(container.end()); }

            const_iterator begin() const { return const_iterator(container.begin()); }
            const_iterator end() const { return const_iterator(container.end()); }
            
            iterator find( const value_type & element)
            {
                return iterator(container.find( key_type(element) ));
            }
            
            const_iterator find( const value_type & element) const
            {
                return const_iterator(container.find( key_type(element) ));
            }
            
            std::pair<iterator, bool> insert( const value_type & element )
            {
                std::pair<typename container_type::iterator, bool> ret = container.insert( std::make_pair( key_type(element), element ) );
                return std::make_pair( iterator(ret.first), ret.second );
            }

            
            size_type size() const { return container.size(); }
            bool empty() const { return container.empty(); }
            
        private:
            container_type container;
        };
        
        
        
        
        
        template<typename element_type, typename key_type, typename hook_tag>
        class container_base_t<hidden_key_map<element_type, key_type>, hook_tag> : public hooked_container_t<hidden_key_map<element_type, key_type>, hook_tag>
        {
        public:
            
            typedef hooked_container_t<hidden_key_map<element_type, key_type>, hook_tag> hooked_container_type;
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
        
        
        
        template<typename key_type_tag>
        struct hidden_key_map_tag {};
        
        namespace result_of
        {
            template<typename element_tag, typename key_type_tag>
            struct hidden_key_map_key_type_from_tag;
            
            
            template<typename element_type, typename key_type_tag>
            struct container_from_tag<element_type, hidden_key_map_tag<key_type_tag> >
            {
                typedef hidden_key_map< typename hidden_key_map_key_type_from_tag<element_type, key_type_tag>::type, element_type > type;
            };
        }
        
        namespace container
        {
            template<typename key_type, typename value_type>
            std::pair<typename hidden_key_map<key_type, value_type>::iterator, bool>
                insert( hidden_key_map<key_type, value_type> & container, const value_type & element )
            {
                return container.insert( element );
            }

        }
    }
}

#endif
