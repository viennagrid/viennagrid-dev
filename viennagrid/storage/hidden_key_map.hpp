#ifndef VIENNAGRID_STORAGE_HIDDEN_KEY_MAP_HPP
#define VIENNAGRID_STORAGE_HIDDEN_KEY_MAP_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <map>
#include "viennagrid/storage/container.hpp"

namespace viennagrid
{
    namespace storage
    {

        template<typename KeyT, typename ValueT>
        class hidden_key_map
        {
            typedef std::map< KeyT, ValueT > container_type;

        public:

            typedef KeyT                               key_type;
            typedef ValueT                             value_type;
            typedef typename container_type::size_type size_type;
            typedef value_type &                       reference;
            typedef const value_type &                 const_reference;
            typedef value_type *                       pointer;
            typedef const value_type *                 const_pointer;


            class iterator : public container_type::iterator
            {
                typedef typename container_type::iterator base;
            public:
                iterator() {}
                iterator(const base & foo) : base(foo) {}

                typedef typename hidden_key_map::value_type value_type;
                typedef typename hidden_key_map::reference reference;
                typedef typename hidden_key_map::const_reference const_reference;
                typedef typename hidden_key_map::pointer pointer;

                reference operator* () { return base::operator*().second; }
                const_reference operator* () const { return base::operator*().second; }
            };

            class const_iterator : public container_type::const_iterator
            {
                typedef typename container_type::const_iterator base;
            public:
                const_iterator() {}
                const_iterator(const base & foo) : base(foo) {}
                const_iterator(const iterator & it) : base(it) {}

                typedef typename hidden_key_map::value_type value_type;
                typedef typename hidden_key_map::const_reference reference;
                typedef typename hidden_key_map::const_reference const_reference;
                typedef typename hidden_key_map::const_pointer pointer;

                const_reference operator* () const { return base::operator*().second; }
            };

            class reverse_iterator : public container_type::reverse_iterator
            {
                typedef typename container_type::reverse_iterator base;
            public:
                reverse_iterator() {}
                reverse_iterator(const base & foo) : base(foo) {}

                typedef typename hidden_key_map::value_type value_type;
                typedef typename hidden_key_map::reference reference;
                typedef typename hidden_key_map::const_reference const_reference;
                typedef typename hidden_key_map::pointer pointer;

                reference operator* () { return base::operator*().second; }
                const_reference operator* () const { return base::operator*().second; }
            };

            class const_reverse_iterator : public container_type::const_reverse_iterator
            {
                typedef typename container_type::const_reverse_iterator base;
            public:
                const_reverse_iterator() {}
                const_reverse_iterator(const base & foo) : base(foo) {}
                const_reverse_iterator(const reverse_iterator & it) : base(it) {}

                typedef typename hidden_key_map::value_type value_type;
                typedef typename hidden_key_map::const_reference reference;
                typedef typename hidden_key_map::const_reference const_reference;
                typedef typename hidden_key_map::const_pointer pointer;

                const_reference operator* () const { return base::operator*().second; }
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

            iterator erase( iterator to_erase )
            {
                container.erase( to_erase++ );
                return to_erase;
            }

            void clear()
            {
                container.clear();
            }


            size_type size() const { return container.size(); }
            bool empty() const { return container.empty(); }

        private:
            container_type container;
        };




        template<typename KeyT, typename ElementT, typename handle_tag>
        class container_base_t<hidden_key_map<KeyT, ElementT>, handle_tag> : public handled_container_t<hidden_key_map<KeyT, ElementT>, handle_tag>
        {
        public:

            typedef handled_container_t<hidden_key_map<KeyT, ElementT>, handle_tag> handled_container_type;
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
                return std::make_pair( handled_container_type::handle(*tmp.first), tmp.second);
            }
        };



        template<typename key_type_tag>
        struct hidden_key_map_tag {};

        namespace result_of
        {
            template<typename element_tag, typename key_type_tag>
            struct hidden_key_map_key_type_from_tag;


            template<typename element_type, typename key_type_tag>
            struct container<element_type, hidden_key_map_tag<key_type_tag> >
            {
                typedef hidden_key_map< typename hidden_key_map_key_type_from_tag<element_type, key_type_tag>::type, element_type > type;
            };
        }

        namespace container
        {
            template<typename KeyT, typename ValueT>
            std::pair<typename hidden_key_map<KeyT, ValueT>::iterator, bool>
                insert( hidden_key_map<KeyT, ValueT> & container, const ValueT & element )
            {
                return container.insert( element );
            }

        }
    }
}

#endif
