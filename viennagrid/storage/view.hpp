#ifndef VIENNAGRID_STORAGE_VIEW_HPP
#define VIENNAGRID_STORAGE_VIEW_HPP

#include <iterator>
#include <algorithm>

#include "viennagrid/meta/typemap.hpp"

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/handle.hpp"
#include "viennagrid/storage/id.hpp"

namespace viennagrid
{

    namespace storage
    {
        template<typename base_container_type_, typename container_tag>
        class view_t;

        template<typename base_container_type_, typename container_tag>
        class view_t
        {
        public:

            template<typename A, typename B>
            friend class view_t;

            typedef base_container_type_ base_container_type;

            typedef typename base_container_type::handle_tag handle_tag;
            typedef typename handle::result_of::handle_type<base_container_type, handle_tag>::type handle_type;
            typedef typename handle::result_of::const_handle_type<base_container_type, handle_tag>::type const_handle_type;


        private:
            typedef typename viennagrid::storage::result_of::container<handle_type, container_tag>::type handle_container_type;


        public:
            typedef typename handle_container_type::size_type size_type;
            typedef typename base_container_type::value_type value_type;
            typedef value_type & reference;
            typedef const value_type & const_reference;
            typedef value_type * pointer;
            typedef const value_type * const_pointer;



            class iterator : public handle_container_type::iterator
            {
                typedef typename handle_container_type::iterator base;
            public:
                iterator(view_t & view_, const base & foo) : base(foo), view(&view_) {}
                iterator(const iterator & it) : base(it), view(it.view) {}

                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::reference reference;
                typedef view_t::pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;

                handle_type & handle() { return base::operator*(); }
                const_handle_type handle() const { return base::operator*(); }

                reference operator* () { return view->dereference_handle( handle() ); }
                const reference operator* () const { return view->dereference_handle( handle() ); }

                pointer operator->() { return &(operator* ()); }
                pointer operator->() const { return &(operator* ()); }

            private:
                view_t * view;
            };


            class const_iterator : public handle_container_type::const_iterator
            {
                typedef typename handle_container_type::const_iterator base;
            public:
                const_iterator(view_t const & view_, const base & foo) : base(foo), view(&view_) {}
                const_iterator(const const_iterator & it) : base(it), view(it.view) {}
                const_iterator(const iterator & it) : base(it), view(it.view) {}

                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::const_reference reference;
                typedef view_t::const_pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;

                const_handle_type handle() { return base::operator*(); }
                const_handle_type handle() const { return base::operator*(); }

                reference operator* () { return view->dereference_handle( handle() ); }
                const reference operator* () const { return view->dereference_handle( handle() ); }

                pointer operator->() const { return &(operator* ()); }

            private:
                view_t const * view;
            };

            class reverse_iterator : public handle_container_type::reverse_iterator
            {
                typedef typename handle_container_type::reverse_iterator base;
            public:
                reverse_iterator(view_t & view_, const base & foo) : base(foo), view(&view_) {}
                reverse_iterator(const reverse_iterator & it) : base(it), view(it.view) {}

                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::reference reference;
                typedef view_t::pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;

                handle_type & handle() { return base::operator*(); }
                const_handle_type handle() const { return base::operator*(); }

                reference operator* () { return view->dereference_handle( handle() ); }
                const reference operator* () const { return view->dereference_handle( handle() ); }

                pointer operator->() { return &(operator* ()); }
                pointer operator->() const { return &(operator* ()); }

            private:
                view_t * view;
            };


            class const_reverse_iterator : public handle_container_type::const_reverse_iterator
            {
                typedef typename handle_container_type::const_reverse_iterator base;
            public:
                const_reverse_iterator(view_t const & view_, const base & foo) : base(foo), view(&view_) {}
                const_reverse_iterator(const const_reverse_iterator & it) : base(it), view(it.view) {}
                const_reverse_iterator(const iterator & it) : base(it), view(it.view) {}

                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::const_reference reference;
                typedef view_t::const_pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;

                const_handle_type handle() { return base::operator*(); }
                const_handle_type handle() const { return base::operator*(); }

                reference operator* () { return view->dereference_handle( handle() ); }
                const reference operator* () const { return view->dereference_handle( handle() ); }

                pointer operator->() const { return &(operator* ()); }

            private:
                view_t const * view;
            };



            typedef typename handle_container_type::iterator handle_iterator;
            typedef typename handle_container_type::const_iterator const_handle_iterator;



            view_t() {}

            void set_base_container( base_container_type & base_container_ )
            { base_container = &base_container_; }

            template<typename other_container_tag>
            void set_base_container( view_t<base_container_type, other_container_tag> & base_view )
            { base_container = base_view.base_container; }

            template<typename other_container_tag>
            void set_base_container( view_t<base_container_type, other_container_tag> const & base_view )
            { base_container = base_view.base_container; }


            iterator begin() { return iterator(*this, handle_container.begin()); }
            iterator end() { return iterator(*this, handle_container.end()); }

            const_iterator cbegin() { return const_iterator(*this, handle_container.begin()); }
            const_iterator cend() { return const_iterator(*this, handle_container.end()); }

            const_iterator begin() const { return const_iterator(*this, handle_container.begin()); }
            const_iterator end() const { return const_iterator(*this, handle_container.end()); }


            handle_iterator handle_begin() { return handle_container.begin(); }
            handle_iterator handle_end() { return handle_container.end(); }

            const_handle_iterator handle_begin() const { return handle_container.begin(); }
            const_handle_iterator handle_end() const { return handle_container.end(); }

            reference dereference_handle( handle_type handle ) { return viennagrid::storage::handle::dereference_handle( *base_container, handle, handle_tag() ); }
            const_reference dereference_handle( const_handle_type handle ) const { return viennagrid::storage::handle::dereference_handle( *base_container, handle, handle_tag() ); }

            handle_type handle( reference element ) { return &element; }
            const_handle_type handle( const_reference element ) const { return &element; }


            reference front() { return dereference_handle(handle_container.front()); }
            const_reference front() const { return dereference_handle(handle_container.front()); }

            reference back() { return dereference_handle(handle_container.back()); }
            const_reference back() const { return dereference_handle(handle_container.back()); }

            reference operator[]( size_type index ) { iterator it = begin(); std::advance(it, index); return *it; }
            const_reference operator[]( size_type index ) const { const_iterator it = begin(); std::advance(it, index); return *it; }


            size_type size() const { return handle_container.size(); }
            void resize(size_type size_) { handle_container.resize(size_); }
            void increment_size() { resize( size()+1 ); }

            bool empty() const { return handle_container.empty(); }
            void clear() { handle_container.clear(); }


            void insert_unique_handle(handle_type handle)
            {
              for (iterator it = begin(); it != end(); ++it)
                if (handle == it.handle()) return;

              insert_handle(handle);
            }

            void insert_handle(handle_type handle) { viennagrid::storage::container::insert(handle_container, handle); }
            void set_handle( handle_type element, size_type pos )
            {
                resize(pos+1);
                handle_container[pos] = element;
            }
            void erase_handle(handle_type handle)
            {
              for (iterator it = begin(); it != end(); ++it)
                if (it.handle() == handle)
                {
                  handle_container.erase( it );
                  return;
                }
            }

            handle_type handle_at(std::size_t pos) { return *viennagrid::advance(handle_begin(), pos); }
            const_handle_type handle_at(std::size_t pos) const { return *viennagrid::advance(handle_begin(), pos); }



        private:

            handle_container_type handle_container;
            base_container_type * base_container;
        };




        namespace result_of
        {
            template<typename base_container_type, typename view_container_tag>
            struct view
            {
                typedef view_t<base_container_type, view_container_tag> type;
            };

            template<typename base_container_type, typename base_view_container_tag, typename view_container_tag>
            struct view< view_t<base_container_type, base_view_container_tag>, view_container_tag>
            {
                typedef view_t<base_container_type, view_container_tag> type;
            };

            template<typename base_container_type, typename base_view_container_tag, typename view_container_tag>
            struct view< const view_t<base_container_type, base_view_container_tag>, view_container_tag>
            {
                typedef view_t<const base_container_type, view_container_tag> type;
            };

            template<typename base_container_type, typename base_view_container_tag, typename view_container_tag>
            struct view< const view_t<const base_container_type, base_view_container_tag>, view_container_tag>
            {
                typedef view_t<const base_container_type, view_container_tag> type;
            };

            template<typename base_container_type, typename base_view_container_tag, typename view_container_tag>
            struct view< view_t<const base_container_type, base_view_container_tag>, view_container_tag>
            {
                typedef view_t<const base_container_type, view_container_tag> type;
            };

            template<typename base_container_type, typename view_container_config>
            typename view<base_container_type, view_container_config>::type view_of( base_container_type & base_container )
            {
                typename view<base_container_type, view_container_config>::type tmp;
                tmp.set_base_container(base_container);
                return tmp;
            }
        }






        namespace view
        {
            typedef viennagrid::meta::make_typemap<
                        viennagrid::storage::default_tag,   viennagrid::storage::handled_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::no_handle_tag>
                    >::type default_view_container_config;




            template<typename base_container_type, typename view_base_container_type, typename handle_container_tag, typename predicate>
            void handle_if(base_container_type & src_container, view_t<view_base_container_type, handle_container_tag> & dst_view, predicate pred)
            {
                for (typename base_container_type::handle_iterator it = src_container.handle_begin(); it != src_container.handle_end(); ++it)
                    if (pred(*it))
                        dst_view.insert_handle( *it );
            }

            template<typename base_container_type, typename view_base_container_type, typename view_container_tag>
            void handle(base_container_type & src_container, view_t<view_base_container_type, view_container_tag> & dst_view)
            {
                handle_if(src_container, dst_view, viennagrid::meta::true_predicate());
            }







            template<typename container_type, typename view_container_config>
            struct view_container_tag
            {
                typedef typename container_type::value_type value_type;
                typedef typename viennagrid::meta::typemap::result_of::find<view_container_config, value_type>::type search_result;
                typedef typename viennagrid::meta::typemap::result_of::find<view_container_config, viennagrid::storage::default_tag>::type default_container;

                typedef typename viennagrid::meta::IF<
                    !viennagrid::meta::EQUAL<search_result, viennagrid::meta::not_found>::value,
                    search_result,
                    default_container
                >::type::second type;
            };





            template<typename container_typemap, typename view_container_config>
            struct view_container_typemap;

            template<typename view_container_config>
            struct view_container_typemap<viennagrid::meta::null_type, view_container_config>
            {
                typedef viennagrid::meta::null_type type;
            };

            template<typename value_type, typename container_type, typename tail, typename view_container_config>
            struct view_container_typemap<viennagrid::meta::typelist_t< viennagrid::meta::static_pair<value_type, container_type>, tail>, view_container_config>
            {
                typedef viennagrid::meta::typelist_t<
                    viennagrid::meta::static_pair<
                        value_type,
                        typename viennagrid::storage::result_of::view<
                            container_type,
                            typename view_container_tag<container_type, view_container_config>::type
                        >::type
                    >,
                    typename view_container_typemap<tail, view_container_config>::type
                > type;
            };
        }


        namespace result_of
        {

            template<typename container_typemap, typename view_container_config>
            struct view_collection
            {
                typedef typename viennagrid::storage::result_of::collection<
                    typename viennagrid::storage::view::view_container_typemap<
                        container_typemap,
                        view_container_config
                    >::type
                >::type type;
            };

            template<typename container_typelist, typename view_container_config>
            struct view_collection< collection_t<container_typelist>, view_container_config>
            {
                typedef viennagrid::storage::collection_t<container_typelist> container_collection_type;

                typedef typename viennagrid::storage::result_of::collection<
                    typename viennagrid::storage::view::view_container_typemap<
                        container_typelist,
                        view_container_config
                    >::type
                >::type type;
            };

        }



        template<typename container_collection_typemap>
        struct set_base_container_helper;

        template<>
        struct set_base_container_helper<viennagrid::meta::null_type>
        {
            template<typename base_container_collection_type, typename view_container_collection_type>
            static void exec( base_container_collection_type &, view_container_collection_type & ) {}
        };

        template<typename value_type, typename container_type, typename tail>
        struct set_base_container_helper< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<value_type, container_type>, tail > >
        {
            template<typename base_container_collection_type, typename view_container_collection_type>
            static void exec( base_container_collection_type & base_container_collection, view_container_collection_type & view_container_collection )
            {
                storage::collection::get<value_type>(view_container_collection).set_base_container( storage::collection::get<value_type>(base_container_collection) );

                set_base_container_helper<tail>::exec(base_container_collection, view_container_collection);
            }
        };


        template<typename base_container_collection_type, typename view_container_collection_type>
        void set_base_container( base_container_collection_type & base_container_collection, view_container_collection_type & view_container_collection )
        {
            set_base_container_helper< typename view_container_collection_type::typemap >::exec(base_container_collection, view_container_collection);
        }


    }

}

#endif
