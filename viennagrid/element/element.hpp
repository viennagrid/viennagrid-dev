#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at

   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include "viennagrid/forwards.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/algorithm.hpp"

#include "viennagrid/storage/collection.hpp"
#include "viennagrid/storage/range.hpp"

#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/element/element_orientation.hpp"

#include "viennagrid/config/element_config.hpp"

/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
{
    template<typename element_tag, typename boundary_config_typemap>
    class boundary_element_layer;


    template<typename element_tag, typename bnd_cell_container_type_, typename orientation_container_type_, typename tail>
    class boundary_element_layer<element_tag, viennagrid::meta::typelist_t< viennagrid::meta::static_pair<bnd_cell_container_type_, orientation_container_type_>, tail > > :
        public boundary_element_layer<element_tag, tail>
    {
    public:
        typedef boundary_element_layer<element_tag, tail> base;

        typedef bnd_cell_container_type_ bnd_cell_container_type;
        typedef typename bnd_cell_container_type::value_type bnd_cell_type;
        typedef typename bnd_cell_container_type::handle_type bnd_cell_handle_type;
        typedef typename bnd_cell_container_type::const_handle_type bnd_cell_const_handle_type;

        typedef typename bnd_cell_type::tag bnd_cell_tag;
        static const int dim = bnd_cell_tag::dim;

        typedef orientation_container_type_ orientation_container_type;
        typedef typename orientation_container_type::value_type orientation_type;

        boundary_element_layer() {}

        boundary_element_layer( const boundary_element_layer & llh) : base(llh), elements_(llh.elements_), orientations_(llh.orientations_) {}

        template<typename container_typelist>
        void set_container( viennagrid::storage::collection_t<container_typelist> & container_collection )
        {
            elements_.set_base_container( viennagrid::storage::collection::get<bnd_cell_type>(container_collection) );
            base::set_container( container_collection );
        }

    //protected:

        template<typename element_type, typename inserter_type>
        void create_boundary_elements(element_type & element, inserter_type & inserter)
        {
            element_topology::boundary_element_generator<element_tag, bnd_cell_tag, bnd_cell_type>::create_boundary_elements(element, inserter);
            base::create_boundary_elements(element, inserter);
        }


        using base::container;
        using base::set_boundary_element;
        using base::add_boundary_element;
        using base::dereference_handle;
        using base::handle;
        using base::global_to_local_orientation;


        bnd_cell_type & dereference_handle(bnd_cell_handle_type handle)
        { return elements_.dereference_handle(handle); }

        const bnd_cell_type & dereference_handle(bnd_cell_const_handle_type handle) const
        { return elements_.dereference_handle(handle); }



        bnd_cell_handle_type handle(bnd_cell_type & boundary_element)
        { return elements_.handle(boundary_element); }

        bnd_cell_const_handle_type handle(bnd_cell_type const & boundary_element) const
        { return elements_.handle(boundary_element); }



        bnd_cell_container_type & container(bnd_cell_tag) { return elements_; }
        const bnd_cell_container_type & container(bnd_cell_tag) const { return elements_; }

        bnd_cell_container_type & container(dimension_tag<dim>) { return elements_; }
        const bnd_cell_container_type & container(dimension_tag<dim>) const { return elements_; }



        template<typename handle_type>
        void set_boundary_element(const bnd_cell_type & to_insert, std::pair<handle_type, bool> inserted, unsigned int pos)
        {
            elements_.set_handle(inserted.first, pos);
            orientations_.resize(pos+1);

            if (inserted.second)
                orientations_[pos].setDefaultOrientation();
            else
            {
                typedef typename result_of::const_element_range<bnd_cell_type, vertex_tag>::type           VertexOnElementConstRange;
                typedef typename result_of::const_handle_iterator<VertexOnElementConstRange>::type     VertexOnElementConstIterator;

                typedef typename result_of::element_range<bnd_cell_type, vertex_tag>::type      VertexOnElementRange;
                typedef typename result_of::handle_iterator<VertexOnElementRange>::type     VertexOnElementIterator;


                long i=0; dim_type j=0;

                //set orientation:
                VertexOnElementRange vertices_on_element = elements<vertex_tag>( elements_[pos] );
                for (VertexOnElementIterator voeit = vertices_on_element.handle_begin();
                        voeit != vertices_on_element.handle_end();
                        ++voeit, ++i)
                {

                    VertexOnElementConstRange vertices_on_element_2 = elements<vertex_tag>( to_insert );
                    for (VertexOnElementConstIterator voeit2 = vertices_on_element_2.handle_begin();
                            voeit2 != vertices_on_element_2.handle_end();
                            ++voeit2, ++j)
                    {
                        if (*voeit == *voeit2)
                        {
                            orientations_[pos].setPermutation(j,i);
                            break;
                        }
                    }
                    j=0;
                }

            }
        }

        template<typename handle_type>
        void add_boundary_element(const bnd_cell_type & to_insert, std::pair<handle_type, bool> inserted)
        { set_boundary_element(to_insert, inserted, elements_.size()); }


      ////////////////// orientation: ////////////////////
      std::size_t global_to_local_orientation(bnd_cell_handle_type const & el, long index) const
      {
        for (std::size_t i=0; i<elements_.size(); ++i)
        {
          if (elements_.handle_at(i) == el)
            return orientations_[i](index);
        }
        assert(false && "Provided k-cell is not a boundary element of the hosting n-cell!");
        return index;
      }


        static void print_class()
        {
            std::cout << "  [ + + ]  " << bnd_cell_tag::name() << std::endl;
            base::print_class();
        }

        void print_orientation()
        {
            std::cout << "  [ + + ]  " << bnd_cell_tag::name() << std::endl;
            for (std::size_t i = 0; i < orientations_.size(); ++i)
                std::cout << "  " << orientations_[i] << std::endl;
            base::print_orientation();
        }

    private:

        bnd_cell_container_type elements_;
        orientation_container_type orientations_;
    };


    template<typename element_tag, typename bnd_cell_container_type_, typename tail>
    class boundary_element_layer<element_tag, viennagrid::meta::typelist_t< viennagrid::meta::static_pair<bnd_cell_container_type_, viennagrid::meta::null_type>, tail > > :
        public boundary_element_layer<element_tag, tail>
    {
    public:
        typedef boundary_element_layer<element_tag, tail> base;

        typedef bnd_cell_container_type_ bnd_cell_container_type;
        typedef typename bnd_cell_container_type::value_type bnd_cell_type;
        typedef typename bnd_cell_container_type::handle_type bnd_cell_handle_type;
        typedef typename bnd_cell_container_type::const_handle_type bnd_cell_const_handle_type;

        typedef typename bnd_cell_type::tag bnd_cell_tag;
        static const int dim = bnd_cell_tag::dim;



        boundary_element_layer() {}

        boundary_element_layer( const boundary_element_layer & llh) : base(llh), elements_(llh.elements_) {}

        template<typename container_typelist>
        void set_container( viennagrid::storage::collection_t<container_typelist> & container_collection )
        {
            elements_.set_base_container( viennagrid::storage::collection::get<bnd_cell_type>(container_collection) );
            base::set_container( container_collection );
        }

    //protected:

        template<typename element_type, typename inserter_type>
        void create_boundary_elements(element_type & element, inserter_type & inserter)
        {
            element_topology::boundary_element_generator<element_tag, bnd_cell_tag, bnd_cell_type>::create_boundary_elements(element, inserter);
            base::create_boundary_elements(element, inserter);
        }

        using base::container;
        using base::set_boundary_element;
        using base::add_boundary_element;
        using base::dereference_handle;
        using base::handle;


        bnd_cell_type & dereference_handle(bnd_cell_handle_type handle)
        { return elements_.dereference_handle(handle); }

        const bnd_cell_type & dereference_handle(bnd_cell_const_handle_type handle) const
        { return elements_.dereference_handle(handle); }



        bnd_cell_handle_type handle(bnd_cell_type & boundary_element)
        { return elements_.handle(boundary_element); }

        bnd_cell_const_handle_type handle(bnd_cell_type const & boundary_element) const
        { return elements_.handle(boundary_element); }



        bnd_cell_container_type & container(bnd_cell_tag) { return elements_; }
        const bnd_cell_container_type & container(bnd_cell_tag) const { return elements_; }

        bnd_cell_container_type & container(dimension_tag<dim>) { return elements_; }
        const bnd_cell_container_type & container(dimension_tag<dim>) const {  return elements_; }



        template<typename handle_type>
        void set_boundary_element(const bnd_cell_type &, std::pair<handle_type, bool> inserted, unsigned int pos)
        { elements_.set_handle(inserted.first, pos); }

        template<typename handle_type>
        void add_boundary_element(const bnd_cell_type & to_insert, std::pair<handle_type, bool> inserted)
        { set_boundary_element(to_insert, inserted, elements_.size()); }

        static void print_class()
        {
            std::cout << "  [ + - ]  " << bnd_cell_tag::name() << std::endl;
            base::print_class();
        }

        void print_orientation()
        {
            std::cout << "  [ - - ]  " << bnd_cell_tag::name() << std::endl;
            base::print_orientation();
        }


    private:

        bnd_cell_container_type elements_;
    };

    template<typename element_tag>
    class boundary_element_layer<element_tag, viennagrid::meta::null_type >
    {
    public:
    //protected:

        template<typename container_typelist>
        void set_container( viennagrid::storage::collection_t<container_typelist> & ) {}

        template<typename element_type, typename inserter_type>
        void create_boundary_elements(element_type &, inserter_type &) {}

        static void print_class() {}
        void print_orientation() {}

        void container();
        void set_boundary_element();
        void add_boundary_element();
        void dereference_handle();
        void handle();
        void global_to_local_orientation();

    private:
    };




//     namespace result_of
//     {
//       template<typename WrappedConfigType>
//       struct element_boundary_cell_container_typelist
//       {
//         typedef typename WrappedConfigType::bnd_cell_container_typelist type;
//       };
//
//       template<typename WrappedConfigType>
//       struct element_id_tag
//       {
//         typedef typename WrappedConfigType::id_tag type;
//       };
//
//       template<typename WrappedConfigType>
//       struct element_appendix_type
//       {
//         typedef typename WrappedConfigType::appendix_type type;
//       };
//     }


    namespace result_of
    {
        template<typename bnd_cell_typelist>
        struct boundary_element_typelist;

        template<>
        struct boundary_element_typelist<viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename boundary_cell_container_type, typename orientation_container_type, typename tail>
        struct boundary_element_typelist< viennagrid::meta::typelist_t<viennagrid::meta::static_pair<boundary_cell_container_type, orientation_container_type>, tail > >
        {
            typedef viennagrid::meta::typelist_t<
                typename boundary_cell_container_type::value_type,
                typename boundary_element_typelist<tail>::type
            > type;
        };

        template<typename element_tag, typename WrappedConfigType>
        struct boundary_element_typelist< element_t<element_tag, WrappedConfigType> >
        {
            typedef typename boundary_element_typelist<typename element_t<element_tag, WrappedConfigType>::bnd_cell_container_typelist>::type type;
        };



        template<typename bnd_cell_typelist>
        struct boundary_element_taglist;

        template<>
        struct boundary_element_taglist<viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename boundary_cell_container_type, typename orientation_container_type, typename tail>
        struct boundary_element_taglist< viennagrid::meta::typelist_t<viennagrid::meta::static_pair<boundary_cell_container_type, orientation_container_type>, tail > >
        {
            typedef viennagrid::meta::typelist_t<
                typename boundary_cell_container_type::value_type::tag,
                typename boundary_element_taglist<tail>::type
            > type;
        };

        template<typename element_tag, typename WrappedConfigType>
        struct boundary_element_taglist< element_t<element_tag, WrappedConfigType> >
        {
            typedef typename boundary_element_taglist< typename element_t<element_tag, WrappedConfigType>::bnd_cell_container_typelist >::type type;
        };






        template<typename typelist, typename tag>
        struct container_of_tag_for_element;

        template<typename tag>
        struct container_of_tag_for_element< viennagrid::meta::null_type, tag >
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename container_pair, typename tail, typename tag>
        struct container_of_tag_for_element< viennagrid::meta::typelist_t<container_pair, tail>, tag >
        {
            typedef typename container_pair::first container_type;
            typedef typename container_type::value_type value_type;

            typedef typename viennagrid::meta::IF<
                viennagrid::meta::EQUAL<typename value_type::tag, tag>::value,
                container_type,
                typename container_of_tag_for_element<tail, tag>::type
            >::type type;
        };





        template<typename typelist, int dim>
        struct container_of_dimension_for_element;

        template<int dim>
        struct container_of_dimension_for_element< viennagrid::meta::null_type, dim >
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename container_pair, typename tail, int dim>
        struct container_of_dimension_for_element< viennagrid::meta::typelist_t<container_pair, tail>, dim >
        {
            typedef typename container_pair::first container_type;
            typedef typename container_type::value_type value_type;

            typedef typename viennagrid::meta::IF<
                value_type::tag::dim == dim,
                container_type,
                typename container_of_dimension_for_element<tail, dim>::type
            >::type type;
        };



        template<typename container_collection_typemap, typename element_tag>
        struct container_of_tag_for_collection;

        template<typename element_tag>
        struct container_of_tag_for_collection<viennagrid::meta::null_type, element_tag>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename element_type, typename container_type, typename tail, typename element_tag>
        struct container_of_tag_for_collection<viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_type, container_type>, tail >, element_tag>
        {
            typedef typename viennagrid::meta::IF<
                viennagrid::meta::EQUAL<typename element_type::tag, element_tag>::value,
                container_type,
                typename container_of_tag_for_collection<tail, element_tag>::type
            >::type type;
        };



        template<typename container_collection_typemap, int dim>
        struct container_of_dimension_for_collection;

        template<int dim>
        struct container_of_dimension_for_collection<viennagrid::meta::null_type, dim>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename element_type, typename container_type, typename tail, int dim>
        struct container_of_dimension_for_collection<viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_type, container_type>, tail >, dim>
        {
            typedef typename viennagrid::meta::IF<
                element_type::tag::dim == dim,
                container_type,
                typename container_of_dimension_for_collection<tail, dim>::type
            >::type type;
        };




        template<typename element_or_collection, typename tag>
        struct container_of_tag;

        template<typename container_collection_typemap, typename tag>
        struct container_of_tag< storage::collection_t<container_collection_typemap>, tag >
        {
            typedef typename container_of_tag_for_collection<container_collection_typemap, tag>::type type;
        };

        template<typename element_tag, typename WrappedConfigType, typename tag>
        struct container_of_tag< element_t<element_tag, WrappedConfigType>, tag >
        {
            typedef typename container_of_tag_for_element<typename element_t<element_tag, WrappedConfigType>::bnd_cell_container_typelist, tag>::type type;
        };

        template<typename element_or_collection, long dim>
        struct container_of_dimension;

        template<typename container_collection_typemap, long dim>
        struct container_of_dimension< storage::collection_t<container_collection_typemap>, dim >
        {
            typedef typename container_of_dimension_for_collection<container_collection_typemap, dim>::type type;
        };

        template<typename element_tag, typename WrappedConfigType, long dim>
        struct container_of_dimension< element_t<element_tag, WrappedConfigType>, dim >
        {
            typedef typename container_of_dimension_for_element<typename element_t<element_tag, WrappedConfigType>::bnd_cell_container_typelist, dim>::type type;
        };
    }



    namespace config
    {
        template<typename bnd_cell_container_typelist_, typename id_tag_, typename appendix_type_>
        struct element_config_wrapper_t
        {
          typedef bnd_cell_container_typelist_ bnd_cell_container_typelist;
          typedef id_tag_ id_tag;
          typedef appendix_type_ appendix_type;
        };
    }


    template<typename ElementTag, typename WrappedConfigType>
    class element_t :
        public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::id<
                      element_t<
                        ElementTag,
                        WrappedConfigType
                      >,
                      typename config::result_of::query_element_id_tag<WrappedConfigType, vertex_tag>::type
                    >::type
                >,
          public boundary_element_layer<ElementTag, typename config::result_of::element_boundary_cell_container_typelist<WrappedConfigType, ElementTag, ElementTag>::type>
    {
    public:

        typedef ElementTag tag;

        typedef typename config::result_of::element_boundary_cell_container_typelist<WrappedConfigType, ElementTag, ElementTag>::type bnd_cell_container_typelist;
        typedef typename config::result_of::query_element_id_tag<WrappedConfigType, ElementTag>::type id_tag;
        typedef typename config::result_of::query_appendix_type<WrappedConfigType, ElementTag>::type appendix_type;

        typedef boundary_element_layer<ElementTag, bnd_cell_container_typelist> base;

        typedef typename result_of::boundary_element_typelist<bnd_cell_container_typelist>::type boundary_cell_typelist;

        typedef typename viennagrid::storage::result_of::id< element_t<ElementTag, WrappedConfigType>, id_tag>::type id_type;
        typedef typename viennagrid::storage::result_of::const_id<id_type>::type const_id_type;

        template<typename container_typelist>
        element_t( viennagrid::storage::collection_t<container_typelist> & container_collection )
        {
            base::set_container(container_collection);
        }

        template<typename inserter_type>
        void insert_callback( inserter_type & inserter, bool inserted )
        {
            if (inserted)
            {
                base::create_boundary_elements(*this, inserter);
            }
        }

        static void print_class()
        {
            std::cout << ElementTag::name() << std::endl;
            base::print_class();
        }

        void print_orientation()
        {
            std::cout << ElementTag::name() << std::endl;
            base::print_orientation();
        }


        typedef typename result_of::container_of_dimension_for_element< bnd_cell_container_typelist, 0>::type::handle_type vertex_handle_type;
        void set_vertex( const vertex_handle_type & vertex, unsigned int pos )
        {
            this->container(viennagrid::dimension_tag<0>()).set_handle( vertex, pos );
        }

//         typedef appendix_type_ appendix_type;

        appendix_type & appendix() { return appendix_; }
        appendix_type const & appendix() const { return appendix_; }

    private:

        appendix_type appendix_;
    };


    // separate specialization for vertices at the moment
    template<typename WrappedConfigType>
    class element_t<vertex_tag, WrappedConfigType> :
        public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::id<
                      element_t<
                        vertex_tag,
                        WrappedConfigType
                      >,
                      typename config::result_of::query_element_id_tag<WrappedConfigType, vertex_tag>::type
                    >::type
                >
    {
    public:

        typedef vertex_tag tag;

        typedef typename config::result_of::element_boundary_cell_container_typelist<WrappedConfigType, vertex_tag, vertex_tag>::type bnd_cell_container_typelist;
        typedef typename config::result_of::query_element_id_tag<WrappedConfigType, vertex_tag>::type id_tag;
        typedef typename config::result_of::query_appendix_type<WrappedConfigType, vertex_tag>::type appendix_type;

        typedef element_t<vertex_tag, WrappedConfigType>            self_type;

        element_t() {}

        template<typename container_typelist>
        element_t( viennagrid::storage::collection_t<container_typelist> & ) {}

        typedef typename result_of::boundary_element_typelist<bnd_cell_container_typelist>::type boundary_cell_typelist;
        typedef typename viennagrid::storage::result_of::id< element_t<vertex_tag, WrappedConfigType>, id_tag>::type id_type;
        typedef typename viennagrid::storage::result_of::const_id<id_type>::type const_id_type;

        template<typename inserter_type>
        void insert_callback( inserter_type &, bool ) {}

        void print_orientation()
        {
            std::cout << vertex_tag::name() << std::endl;
        }

        static void print_class()
        {
            std::cout << vertex_tag::name() << std::endl;
        }

        appendix_type & appendix() { return appendix_; }
        appendix_type const & appendix() const { return appendix_; }

    private:

        appendix_type appendix_;
    };



    namespace result_of
    {


        template<typename element_type>
        struct id_type
        {
            typedef typename element_type::id_type type;
        };





        template<typename element_type_or_tag>
        struct topologic_dimension
        {
            static const int value = element_tag<element_type_or_tag>::type::dim;
        };


        template<typename element_type, typename boundary_cell_type_or_tag>
        struct has_boundary
        {};

        template<typename element_tag_, typename WrappedConfigType, typename boundary_cell_type_or_tag>
        struct has_boundary< element_t<element_tag_, WrappedConfigType>, boundary_cell_type_or_tag >
        {
            typedef typename element_tag<boundary_cell_type_or_tag>::type boundary_cell_tag;

            const static bool value =
            !viennagrid::meta::EQUAL<
                typename container_of_tag< element_t<element_tag_, WrappedConfigType>, boundary_cell_tag >::type,
                viennagrid::meta::null_type>::value;
        };




        // Defines a SUB-ELEMENT from an ELEMENT using SUB-ELEMENT TYPE or TAG
        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct element< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename element_t<element_tag_, WrappedConfigType>::bnd_cell_container_typelist boundary_cell_container_typelist;
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef typename container_of_tag_for_element<boundary_cell_container_typelist , sub_element_tag >::type::value_type type;
        };

        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct element< const element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename element_t<element_tag_, WrappedConfigType>::bnd_cell_container_typelist boundary_cell_container_typelist;
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef const typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::value_type type;
        };

        // Defines a HOOK TO a SUB-ELEMENT from an ELEMENT using SUB-ELEMENT TYPE or TAG
        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct handle< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename element_t<element_tag_, WrappedConfigType>::bnd_cell_container_typelist boundary_cell_container_typelist;
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::handle_type type;
        };

        // Defines a const HOOK TO a SUB-ELEMENT from an ELEMENT using SUB-ELEMENT TYPE or TAG
        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct const_handle< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename element_t<element_tag_, WrappedConfigType>::bnd_cell_container_typelist boundary_cell_container_typelist;
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::const_handle_type type;
        };


        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct element_range< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename element_t<element_tag_, WrappedConfigType>::bnd_cell_container_typelist boundary_cell_container_typelist;
            typedef typename element< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag>::type sub_element_type;
            typedef viennagrid::storage::container_range_wrapper< typename container_of_tag_for_element<boundary_cell_container_typelist, typename sub_element_type::tag>::type > type;
        };

        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct const_element_range< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename element_t<element_tag_, WrappedConfigType>::bnd_cell_container_typelist boundary_cell_container_typelist;
            typedef typename element< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag>::type sub_element_type;
            typedef viennagrid::storage::container_range_wrapper< const typename container_of_tag_for_element<boundary_cell_container_typelist, typename sub_element_type::tag>::type > type;
        };

        template<typename element_tag_, typename WrappedConfigType, typename sub_element_type_or_tag>
        struct element_range< const element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >
        {
            typedef typename const_element_range< element_t<element_tag_, WrappedConfigType>, sub_element_type_or_tag >::type type;
        };



        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag;
            typedef typename container_of_tag_for_collection<container_collection_typemap, element_tag>::type::value_type type;
        };

        template<typename container_collection_typemap, typename element_type_or_tag>
        struct handle<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag;

            // Error in line below possibly means that there is no element_type_or_tag in
            typedef typename container_of_tag_for_collection<container_collection_typemap, element_tag>::type::handle_type type;
        };

        template<typename container_collection_typemap, typename element_type_or_tag>
        struct const_handle<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag;
            typedef typename container_of_tag_for_collection<container_collection_typemap, element_tag>::type::const_handle_type type;
        };


        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag_;
            typedef viennagrid::storage::container_range_wrapper< typename container_of_tag_for_collection<container_collection_typemap, element_tag_>::type > type;
        };

        template<typename container_collection_typemap, typename element_type_or_tag>
        struct const_element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag_;
            typedef viennagrid::storage::container_range_wrapper< const typename container_of_tag_for_collection<container_collection_typemap, element_tag_>::type > type;
        };

        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element_range< const storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename const_element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type type;
        };

    }



    template<typename sub_element_type_or_tag, typename element_tag, typename WrappedConfigType>
    typename result_of::element_range<element_t<element_tag, WrappedConfigType>, sub_element_type_or_tag>::type elements( element_t<element_tag, WrappedConfigType> & element)
    {
        typedef typename result_of::element<element_t<element_tag, WrappedConfigType>, sub_element_type_or_tag>::type sub_element_type;
        return typename result_of::element_range<element_t<element_tag, WrappedConfigType>, sub_element_type_or_tag>::type( element.container( typename sub_element_type::tag() ) );
    }

    template<typename sub_element_type_or_tag, typename element_tag, typename WrappedConfigType>
    typename result_of::const_element_range<element_t<element_tag, WrappedConfigType>, sub_element_type_or_tag>::type elements( const element_t<element_tag, WrappedConfigType> & element)
    {
        typedef typename result_of::element<element_t<element_tag, WrappedConfigType>, sub_element_type_or_tag>::type sub_element_type;
        return typename result_of::const_element_range<element_t<element_tag, WrappedConfigType>, sub_element_type_or_tag>::type( element.container( typename sub_element_type::tag() ) );
    }



    template<typename element_tag, typename WrappedConfigType, typename handle_type>
    typename storage::handle::result_of::value_type<handle_type>::type & dereference_handle( element_t<element_tag, WrappedConfigType> & element, handle_type const & handle)
    {
        return element.dereference_handle(handle);
    }

    template<typename element_tag, typename WrappedConfigType, typename handle_type>
    const typename storage::handle::result_of::value_type<handle_type>::type & dereference_handle( element_t<element_tag, WrappedConfigType> const & element, handle_type const & handle)
    {
        return element.dereference_handle(handle);
    }



    template<typename ElementTag1, typename WrappedConfigType1, typename ElementTag2, typename WrappedConfigType2>
    typename result_of::handle<element_t<ElementTag1, WrappedConfigType1>, element_t<ElementTag2, WrappedConfigType2> >::type handle( element_t<ElementTag1, WrappedConfigType1> & element, element_t<ElementTag2, WrappedConfigType2> & boundary_element )
    {
        return element.handle(boundary_element);
    }

    template<typename ElementTag1, typename WrappedConfigType1, typename ElementTag2, typename WrappedConfigType2>
    typename result_of::const_handle<element_t<ElementTag1, WrappedConfigType1>, element_t<ElementTag2, WrappedConfigType2> >::type handle( element_t<ElementTag1, WrappedConfigType1> const & element, element_t<ElementTag2, WrappedConfigType2> const & boundary_element )
    {
        return element.handle(boundary_element);
    }




    template<typename element_type_or_tag, typename container_collection_typemap>
    typename result_of::element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type elements( storage::collection_t<container_collection_typemap> & collection)
    {
        typedef typename result_of::element<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type element_type;
        return typename result_of::element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type( storage::collection::get<element_type>(collection) );
    }

    template<typename element_type_or_tag, typename container_collection_typemap>
    typename result_of::const_element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type elements( const storage::collection_t<container_collection_typemap> & collection)
    {
        typedef typename result_of::element<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type element_type;
        return typename result_of::const_element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type( storage::collection::get<element_type>(collection) );
    }





    template<typename element_type, typename functor_type>
    struct for_each_boundary_cell_functor
    {
    public:

        for_each_boundary_cell_functor( element_type & element_, functor_type functor_ ) : element(element_), functor(functor_) {}

        template<typename boundary_cell_type>
        void operator()( viennagrid::meta::tag<boundary_cell_type> )
        {
            typedef typename result_of::element_range<element_type, boundary_cell_type>::type boundary_cell_range_type;
            typedef typename result_of::iterator<boundary_cell_range_type>::type boundary_cell_iterator_type;

            boundary_cell_range_type range = viennagrid::elements<boundary_cell_type>( element );
            for (boundary_cell_iterator_type it = range.begin(); it != range.end(); ++it)
                functor(*it);
        }

    private:
        element_type & element;
        functor_type functor;
    };

    template<typename element_type, typename functor_type>
    void for_each_boundary_cell( element_type & element, functor_type functor )
    {
        for_each_boundary_cell_functor<element_type, functor_type> for_each_functor( element, functor );

        viennagrid::meta::typelist::for_each<typename element_type::boundary_cell_typelist>(for_each_functor);
    }




    inline std::ostream & operator<<(std::ostream & os, viennagrid::meta::null_type) { return os; }


    /** @brief Overload for the output streaming operator for the vertex type */
    template <typename WrappedConfigType>
    std::ostream & operator<<(std::ostream & os, element_t<vertex_tag, WrappedConfigType> const & el)
    {
        os << "-- Vertex, ID: " << el.id() << " " << el.appendix();

        return os;
    }


    /** @brief Overload for the output streaming operator */
    template <typename element_tag, typename WrappedConfigType>
    std::ostream & operator<<(std::ostream & os, element_t<element_tag, WrappedConfigType> const & el)
    {
        typedef element_t<element_tag, WrappedConfigType> element_type;
        typedef typename viennagrid::result_of::const_element_range< element_type, vertex_tag >::type vertex_range;
        typedef typename viennagrid::result_of::const_iterator< vertex_range >::type const_vertex_iterator;


        os << "-- " << element_tag::name() << ", ID: " << el.id() << " --";
        const vertex_range & vertices = elements<vertex_tag>(el);
        for (const_vertex_iterator vit  = vertices.begin();
                            vit != vertices.end();
                        ++vit)
        os << std::endl << "  " << *vit;

        return os;
    }

}


#endif
