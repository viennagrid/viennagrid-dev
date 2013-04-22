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
#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/algorithm.hpp"

#include "viennagrid/storage/collection.hpp"
#include "viennagrid/storage/range.hpp"

#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/element/element_orientation.hpp"

/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
{
    template<typename element_tag, typename boundary_config_typemap>
    class boundary_cell_layer;
    
    
    template<typename element_tag, typename bnd_cell_container_type_, typename orientation_container_type_, typename tail>
    class boundary_cell_layer<element_tag, viennameta::typelist_t< viennameta::static_pair<bnd_cell_container_type_, orientation_container_type_>, tail > > :
        public boundary_cell_layer<element_tag, tail>
    {
    public:
        typedef boundary_cell_layer<element_tag, tail> base;
        
        typedef bnd_cell_container_type_ bnd_cell_container_type;
        typedef typename bnd_cell_container_type::value_type bnd_cell_type;
        typedef typename bnd_cell_container_type::hook_type bnd_cell_hook_type;
        typedef typename bnd_cell_container_type::const_hook_type bnd_cell_const_hook_type;
        
        typedef typename bnd_cell_type::tag bnd_cell_tag;
        enum { dim = bnd_cell_tag::dim };
        
        typedef orientation_container_type_ orientation_container_type;
        typedef typename orientation_container_type::value_type orientation_type;
        
        boundary_cell_layer( ) 
        {
            //std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
        };

        boundary_cell_layer( const boundary_cell_layer & llh) : base(llh), elements_(llh.elements_), orientations_(llh.orientations_) {}
        
        template<typename container_typelist>
        void set_container( viennagrid::storage::collection_t<container_typelist> & container_collection )
        {
            elements_.set_base_container( viennagrid::storage::collection::get<bnd_cell_type>(container_collection) );
            base::set_container( container_collection );
        }
        
    //protected:
        
        template<typename element_type, typename inserter_type>
        void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            topology::bndcell_generator<element_tag, bnd_cell_tag, bnd_cell_type>::create_bnd_cells(element, inserter);
            base::create_bnd_cells(element, inserter);
        }
        
        
        using base::container;
        using base::set_bnd_cell;
        using base::dereference_hook;
        using base::global_to_local_orientation;
        
        
        bnd_cell_type &
        dereference_hook(bnd_cell_hook_type hook)
        {
            return elements_.dereference_hook(hook);
        }
        
        const bnd_cell_type &
        dereference_hook(bnd_cell_hook_type hook) const
        {
            return elements_.dereference_hook(hook);
        }
        
        const bnd_cell_type &
        dereference_hook(bnd_cell_const_hook_type hook)
        {
            return elements_.dereference_hook(hook);
        }
        
        const bnd_cell_type &
        dereference_hook(bnd_cell_const_hook_type hook) const
        {
            return elements_.dereference_hook(hook);
        }
        
        
        
        bnd_cell_container_type &
        container(bnd_cell_tag)
        { 
            return elements_;
        }

        //const:     
        const bnd_cell_container_type &
        container(bnd_cell_tag) const
        { 
            return elements_;
        }

        bnd_cell_container_type &
        container(dimension_tag<dim>)
        { 
            return elements_;
        }

        //const:     
        const bnd_cell_container_type &
        container(dimension_tag<dim>) const
        { 
            return elements_;
        }

        template<typename hook_type>
        void set_bnd_cell(const bnd_cell_type & to_insert, std::pair<hook_type, bool> inserted, unsigned int pos)
        {
            //elements_[pos] = inserted.first;
            elements_.set_hook(inserted.first, pos);
            orientations_.resize(pos+1);

            if (inserted.second)
                orientations_[pos].setDefaultOrientation();
            else
            {
                //std::cout << "Non Default Orientation" << std::endl;
                
                typedef typename result_of::const_element_range<bnd_cell_type, vertex_tag>::type           VertexOnElementConstRange;
                typedef typename result_of::const_hook_iterator<VertexOnElementConstRange>::type     VertexOnElementConstIterator;

                typedef typename result_of::element_range<bnd_cell_type, vertex_tag>::type      VertexOnElementRange;
                typedef typename result_of::hook_iterator<VertexOnElementRange>::type     VertexOnElementIterator;
                

                long i=0; dim_type j=0;
                                    
                //set orientation:
                VertexOnElementRange vertices_on_element = elements<vertex_tag>( elements_[pos] );
                for (VertexOnElementIterator voeit = vertices_on_element.hook_begin();
                        voeit != vertices_on_element.hook_end();
                        ++voeit, ++i)
                {
                    
                    VertexOnElementConstRange vertices_on_element_2 = elements<vertex_tag>( to_insert );
                    for (VertexOnElementConstIterator voeit2 = vertices_on_element_2.hook_begin();
                            voeit2 != vertices_on_element_2.hook_end();
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
        
        template<typename hook_type>
        void add_bnd_cell(const bnd_cell_type & to_insert, std::pair<hook_type, bool> inserted)
        {
            set_bnd_cell(to_insert, inserted, elements_.size());
        }
        
        
      ////////////////// orientation: ////////////////////
      std::size_t global_to_local_orientation(bnd_cell_hook_type const & el, long index) const
      { 
        for (std::size_t i=0; i<elements_.size(); ++i)
        {
          if (elements_.hook_at(i) == el)
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
    class boundary_cell_layer<element_tag, viennameta::typelist_t< viennameta::static_pair<bnd_cell_container_type_, viennameta::null_type>, tail > > :
        public boundary_cell_layer<element_tag, tail>
    {
    public:
        typedef boundary_cell_layer<element_tag, tail> base;
        
        typedef bnd_cell_container_type_ bnd_cell_container_type;
        typedef typename bnd_cell_container_type::value_type bnd_cell_type;
        typedef typename bnd_cell_container_type::hook_type bnd_cell_hook_type;
        typedef typename bnd_cell_container_type::const_hook_type bnd_cell_const_hook_type;
        
        typedef typename bnd_cell_type::tag bnd_cell_tag;
        enum { dim = bnd_cell_tag::dim };
        
        
        
        boundary_cell_layer( ) 
        {
            //std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
        };

        boundary_cell_layer( const boundary_cell_layer & llh) : base(llh), elements_(llh.elements_) {}
        
        template<typename container_typelist>
        void set_container( viennagrid::storage::collection_t<container_typelist> & container_collection )
        {
            elements_.set_base_container( viennagrid::storage::collection::get<bnd_cell_type>(container_collection) );
            base::set_container( container_collection );
        }
        
    //protected:
        
        template<typename element_type, typename inserter_type>
        void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            topology::bndcell_generator<element_tag, bnd_cell_tag, bnd_cell_type>::create_bnd_cells(element, inserter);
            base::create_bnd_cells(element, inserter);
        }
        
        using base::container;
        using base::set_bnd_cell;
        using base::dereference_hook;
        
        
        bnd_cell_type &
        dereference_hook(bnd_cell_hook_type hook)
        {
            return elements_.dereference_hook(hook);
        }
        
        const bnd_cell_type &
        dereference_hook(bnd_cell_hook_type hook) const
        {
            return elements_.dereference_hook(hook);
        }
        
        const bnd_cell_type &
        dereference_hook(bnd_cell_const_hook_type hook)
        {
            return elements_.dereference_hook(hook);
        }
        
        const bnd_cell_type &
        dereference_hook(bnd_cell_const_hook_type hook) const
        {
            return elements_.dereference_hook(hook);
        }
        
        
        bnd_cell_container_type &
        container(bnd_cell_tag)
        { 
            return elements_;
        }

        //const:     
        const bnd_cell_container_type &
        container(bnd_cell_tag) const
        { 
            return elements_;
        }
        
        
        bnd_cell_container_type &
        container(dimension_tag<dim>)
        { 
            return elements_;
        }

        //const:     
        const bnd_cell_container_type &
        container(dimension_tag<dim>) const
        { 
            return elements_;
        }

        template<typename hook_type>
        void set_bnd_cell(const bnd_cell_type & to_insert, std::pair<hook_type, bool> inserted, unsigned int pos)
        {
            elements_.set_hook(inserted.first, pos);
        }
        
        template<typename hook_type>
        void add_bnd_cell(const bnd_cell_type & to_insert, std::pair<hook_type, bool> inserted)
        {
            set_bnd_cell(to_insert, inserted, elements_.size());
        }
        
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
    class boundary_cell_layer<element_tag, viennameta::null_type >
    {
    public:
    //protected:
    
        template<typename container_typelist>
        void set_container( viennagrid::storage::collection_t<container_typelist> & container_collection ) {}
    
        template<typename element_type, typename inserter_type>
        void create_bnd_cells(element_type & element, inserter_type & inserter) {}            
        
        static void print_class() {}
        void print_orientation() {}
        
        void container();
        void set_bnd_cell();
        void dereference_hook();
        void global_to_local_orientation();
        
    private:
    };
    

    
    
    namespace result_of
    {
        template<typename bnd_cell_typelist>
        struct boundary_cell_typelist;
        
        template<>
        struct boundary_cell_typelist<viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename boundary_cell_container_type, typename orientation_container_type, typename tail>
        struct boundary_cell_typelist< viennameta::typelist_t<viennameta::static_pair<boundary_cell_container_type, orientation_container_type>, tail > >
        {
            typedef viennameta::typelist_t<
                typename boundary_cell_container_type::value_type,
                typename boundary_cell_typelist<tail>::type
            > type;
        };
        
        template<typename element_tag, typename bnd_cell_container_typelist_, typename id_tag>
        struct boundary_cell_typelist< element_t<element_tag, bnd_cell_container_typelist_, id_tag> >
        {
            typedef typename boundary_cell_typelist<bnd_cell_container_typelist_>::type type;
        };
        
        
        
        
        
        template<typename typelist, typename tag>
        struct container_of_tag_for_element;
        
        template<typename tag>
        struct container_of_tag_for_element< viennameta::null_type, tag >
        {
            typedef viennameta::null_type type;
        };
        
        template<typename container_pair, typename tail, typename tag>
        struct container_of_tag_for_element< viennameta::typelist_t<container_pair, tail>, tag >
        {
            typedef typename container_pair::first container_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename viennameta::_if<
                viennameta::_equal<typename value_type::tag, tag>::value,
                container_type,
                typename container_of_tag_for_element<tail, tag>::type
            >::type type;
        };
        
        
        
        
        
        template<typename typelist, long dim>
        struct container_of_dimension_for_element;
        
        template<long dim>
        struct container_of_dimension_for_element< viennameta::null_type, dim >
        {
            typedef viennameta::null_type type;
        };
        
        template<typename container_pair, typename tail, long dim>
        struct container_of_dimension_for_element< viennameta::typelist_t<container_pair, tail>, dim >
        {
            typedef typename container_pair::first container_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename viennameta::_if<
                value_type::tag::dim == dim,
                container_type,
                typename container_of_dimension_for_element<tail, dim>::type
            >::type type;
        };
        
        
        
        template<typename container_collection_typemap, typename element_tag>
        struct container_of_tag_for_collection;
        
        template<typename element_tag>
        struct container_of_tag_for_collection<viennameta::null_type, element_tag>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename element_type, typename container_type, typename tail, typename element_tag>
        struct container_of_tag_for_collection<viennameta::typelist_t< viennameta::static_pair<element_type, container_type>, tail >, element_tag>
        {
            typedef typename viennameta::_if<
                viennameta::_equal<typename element_type::tag, element_tag>::value,
                container_type,
                typename container_of_tag_for_collection<tail, element_tag>::type
            >::type type;
        };
        
        
        
        template<typename container_collection_typemap, long dim>
        struct container_of_dimension_for_collection;
        
        template<long dim>
        struct container_of_dimension_for_collection<viennameta::null_type, dim>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename element_type, typename container_type, typename tail, long dim>
        struct container_of_dimension_for_collection<viennameta::typelist_t< viennameta::static_pair<element_type, container_type>, tail >, dim>
        {
            typedef typename viennameta::_if<
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
        
        template<typename element_tag, typename boundary_cell_typelist, typename id_type, typename tag>
        struct container_of_tag< element_t<element_tag, boundary_cell_typelist, id_type>, tag >
        {
            typedef typename container_of_tag_for_element<boundary_cell_typelist, tag>::type type;
        };
        
        template<typename element_or_collection, long dim>
        struct container_of_dimension;
        
        template<typename container_collection_typemap, long dim>
        struct container_of_dimension< storage::collection_t<container_collection_typemap>, dim >
        {
            typedef typename container_of_dimension_for_collection<container_collection_typemap, dim>::type type;
        };
        
        template<typename element_tag, typename boundary_cell_typelist, typename id_type, long dim>
        struct container_of_dimension< element_t<element_tag, boundary_cell_typelist, id_type>, dim >
        {
            typedef typename container_of_dimension_for_element<boundary_cell_typelist, dim>::type type;
        };
    }
    
    
    
    
    template<typename element_tag, typename bnd_cell_container_typelist_, typename id_tag>
    class element_t :
        public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::smart_id< element_t<element_tag, bnd_cell_container_typelist_, id_tag>, id_tag>::type
                >,
        public boundary_cell_layer<element_tag, bnd_cell_container_typelist_>
    {
        typedef boundary_cell_layer<element_tag, bnd_cell_container_typelist_> base;
        
    public:
        
        typedef bnd_cell_container_typelist_ bnd_cell_container_typelist;
        typedef element_tag tag;
        typedef typename result_of::boundary_cell_typelist<bnd_cell_container_typelist_>::type boundary_cell_typelist;
        
        typedef typename viennagrid::storage::result_of::smart_id< element_t<element_tag, bnd_cell_container_typelist_, id_tag>, id_tag>::type id_type;
        typedef typename viennagrid::storage::result_of::const_id<id_type>::type const_id_type;
                    
        template<typename container_typelist>
        element_t( viennagrid::storage::collection_t<container_typelist> & container_collection )
        {
            base::set_container(container_collection);
        }
        
        ~element_t()
        {
            viennadata::erase<viennadata::all, viennadata::all>()(*this);
        }
        
        template<typename inserter_type>
        void insert_callback( inserter_type & inserter, bool inserted )
        {
            if (inserted)
            {
                base::create_bnd_cells(*this, inserter);
            }
        }
        
        static void print_class()
        {
            std::cout << element_tag::name() << std::endl;
            base::print_class();
        }
        
        void print_orientation()
        {
            std::cout << element_tag::name() << std::endl;
            base::print_orientation();
        }
        
        
        typedef typename result_of::container_of_dimension_for_element< bnd_cell_container_typelist, 0>::type::hook_type vertex_hook_type;
        void set_vertex( const vertex_hook_type & vertex, unsigned int pos )
        {
            this->container(viennagrid::dimension_tag<0>()).set_hook( vertex, pos );
        }
    };
            

    // separate specialization for vertices at the moment
    template<typename bnd_cell_container_typelist_, typename id_tag>
    class element_t<vertex_tag, bnd_cell_container_typelist_, id_tag> :
        public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::smart_id< element_t<vertex_tag, bnd_cell_container_typelist_, id_tag>, id_tag>::type
                >
    {
        typedef element_t<vertex_tag, bnd_cell_container_typelist_, id_tag>            self_type;
        typedef viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::smart_id< element_t<vertex_tag, bnd_cell_container_typelist_, id_tag>, id_tag>::type
                > id_handler;
        
    public:
        element_t() {}
        
        template<typename container_typelist>
        element_t( viennagrid::storage::collection_t<container_typelist> & container_collection ) {}
        
        typedef bnd_cell_container_typelist_ bnd_cell_container_typelist;
        typedef vertex_tag tag;
        typedef typename result_of::boundary_cell_typelist<bnd_cell_container_typelist_>::type boundary_cell_typelist;
        typedef typename viennagrid::storage::result_of::smart_id< element_t<vertex_tag, bnd_cell_container_typelist_, id_tag>, id_tag>::type id_type;
        typedef typename viennagrid::storage::result_of::const_id<id_type>::type const_id_type;
                    
        template<typename inserter_type>
        void insert_callback( inserter_type & inserter, bool inserted ) {}
        
        void print_orientation()
        {
            std::cout << vertex_tag::name() << std::endl;
        }
        
        static void print_class()
        {
            std::cout << vertex_tag::name() << std::endl;
        }
    };
    


    /** @brief Overload for the output streaming operator for the vertex type */
    template <typename ContainerType, typename IDType>
    std::ostream & operator<<(std::ostream & os, element_t<vertex_tag, ContainerType, IDType> const & el)
    {
        os << "-- Vertex, ID: " << el.id();

        return os;
    }

    

    
    
    namespace result_of
    {          
        

        template<typename element_type>
        struct id_type
        {
            typedef typename element_type::id_type type;
        };
        
        
        
        template<typename element_type, typename boundary_cell_type_or_tag>
        struct has_boundary
        {};
        
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename boundary_cell_type_or_tag>
        struct has_boundary< element_t<element_tag_, boundary_cell_container_typelist, id_type>, boundary_cell_type_or_tag >
        {
            typedef typename element_tag<boundary_cell_type_or_tag>::type boundary_cell_tag;
            
            const static bool value = 
            !viennameta::_equal<
                typename container_of_tag< element_t<element_tag_, boundary_cell_container_typelist, id_type>, boundary_cell_tag >::type,
                viennameta::null_type>::value;
        };
        
        
        
        
        // Defines a SUB-ELEMENT from an ELEMENT using SUB-ELEMENT TYPE or TAG
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct element< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::value_type type;
        };
        
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct element< const element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef const typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::value_type type;
        };
        
        // Defines a HOOK TO a SUB-ELEMENT from an ELEMENT using SUB-ELEMENT TYPE or TAG
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct element_hook< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::hook_type type;
        };
        
        // Defines a const HOOK TO a SUB-ELEMENT from an ELEMENT using SUB-ELEMENT TYPE or TAG
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct const_element_hook< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename element_tag<sub_element_type_or_tag>::type sub_element_tag;
            typedef typename container_of_tag_for_element< boundary_cell_container_typelist, sub_element_tag >::type::const_hook_type type;
        };

        
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct element_range< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename element< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type sub_element_type;
            typedef viennagrid::storage::container_range_wrapper< typename container_of_tag_for_element<boundary_cell_container_typelist, typename sub_element_type::tag>::type > type;
        };
        
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct const_element_range< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename element< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type sub_element_type;
            typedef viennagrid::storage::container_range_wrapper< const typename container_of_tag_for_element<boundary_cell_container_typelist, typename sub_element_type::tag>::type > type;
        };
        
        template<typename element_tag_, typename boundary_cell_container_typelist, typename id_type, typename sub_element_type_or_tag>
        struct element_range< const element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >
        {
            typedef typename const_element_range< element_t<element_tag_, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag >::type type;
        };
        
        
        
        
        
        
//         template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
//         struct ncell< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
//         {
//             typedef typename container_of_dimension_for_element< boundary_cell_container_typelist, dim >::type::value_type type;
//         };
//         
//         template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
//         struct ncell_hook< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
//         {
//             typedef typename container_of_dimension_for_element< boundary_cell_container_typelist, dim >::type::hook_type type;
//         };
//         
//         template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
//         struct const_ncell_hook< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
//         {
//             typedef typename container_of_dimension_for_element< boundary_cell_container_typelist, dim >::type::const_hook_type type;
//         };
//         
//         
//         template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
//         struct ncell_range< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
//         {
//             typedef viennagrid::storage::container_range_wrapper< typename container_of_dimension_for_element<boundary_cell_container_typelist, dim>::type > type;
//         };
//         
//         template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
//         struct const_ncell_range< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
//         {
//             typedef viennagrid::storage::container_range_wrapper< const typename container_of_dimension_for_element<boundary_cell_container_typelist, dim>::type > type;
//         };
//         
//         template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
//         struct ncell_range< const element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
//         {
//             typedef typename const_ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type type;
//         };
        

        
        
        
        

        
        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag;
            typedef typename container_of_tag_for_collection<container_collection_typemap, element_tag>::type::value_type type;
        };
        
//         template<typename container_collection_typemap,
//                  typename sub_element_tag, typename sub_boundary_cell_container_typelist, typename sub_id_type>
//         struct element< storage::collection_t<container_collection_typemap>, element_t<sub_element_tag, sub_boundary_cell_container_typelist, sub_id_type> >
//         {
//             typedef typename element< storage::collection_t<container_collection_typemap>, sub_element_tag >::type type;
//         };
        
        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element_hook<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag;
            typedef typename container_of_tag_for_collection<container_collection_typemap, element_tag>::type::hook_type type;
        };
        
//         template<typename container_collection_typemap,
//                  typename sub_element_tag, typename sub_boundary_cell_container_typelist, typename sub_id_type>
//         struct element_hook< storage::collection_t<container_collection_typemap>, element_t<sub_element_tag, sub_boundary_cell_container_typelist, sub_id_type> >
//         {
//             typedef typename element_hook< storage::collection_t<container_collection_typemap>, sub_element_tag >::type type;
//         };
        
        template<typename container_collection_typemap, typename element_type_or_tag>
        struct const_element_hook<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag;
            typedef typename container_of_tag_for_collection<container_collection_typemap, element_tag>::type::const_hook_type type;
        };
        
//         template<typename container_collection_typemap,
//                  typename sub_element_tag, typename sub_boundary_cell_container_typelist, typename sub_id_type>
//         struct const_element_hook< storage::collection_t<container_collection_typemap>, element_t<sub_element_tag, sub_boundary_cell_container_typelist, sub_id_type> >
//         {
//             typedef typename const_element_hook< storage::collection_t<container_collection_typemap>, sub_element_tag >::type type;
//         };
        

        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag_;
            //typedef typename element< storage::collection_t<container_collection_typemap>, element_type_or_tag>::type element_type;
            typedef viennagrid::storage::container_range_wrapper< typename container_of_tag_for_collection<container_collection_typemap, element_tag_>::type > type;
        };
        
        template<typename container_collection_typemap, typename element_type_or_tag>
        struct const_element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag_;
            //typedef typename element< storage::collection_t<container_collection_typemap>, element_type_or_tag>::type element_type;
            typedef const viennagrid::storage::container_range_wrapper< const typename container_of_tag_for_collection<container_collection_typemap, element_tag_>::type > type;
        };
        
        template<typename container_collection_typemap, typename element_type_or_tag>
        struct element_range< const storage::collection_t<container_collection_typemap>, element_type_or_tag>
        {
            typedef typename const_element_range<storage::collection_t<container_collection_typemap>, element_type_or_tag>::type type;
        };
        
        
        
//         template<typename container_collection_typemap, long dim>
//         struct ncell<storage::collection_t<container_collection_typemap>, dim>
//         {
//             typedef typename container_of_dimension_for_collection<container_collection_typemap, dim>::type::value_type type;
//         };
//         
//         template<typename container_collection_typemap, long dim>
//         struct ncell_hook<storage::collection_t<container_collection_typemap>, dim>
//         {
//             typedef typename container_of_dimension_for_collection<container_collection_typemap, dim>::type::hook_type type;
//         };
//         
//         template<typename container_collection_typemap, long dim>
//         struct const_ncell_hook<storage::collection_t<container_collection_typemap>, dim>
//         {
//             typedef typename container_of_dimension_for_collection<container_collection_typemap, dim>::type::const_hook_type type;
//         };
//         
// 
//         template<typename container_collection_typemap, long dim>
//         struct ncell_range<storage::collection_t<container_collection_typemap>, dim>
//         {
//             typedef viennagrid::storage::container_range_wrapper< typename container_of_dimension_for_collection<container_collection_typemap, dim>::type > type;
//         };
//         
//         template<typename container_collection_typemap, long dim>
//         struct const_ncell_range<storage::collection_t<container_collection_typemap>, dim>
//         {
//             typedef const viennagrid::storage::container_range_wrapper< const typename container_of_dimension_for_collection<container_collection_typemap, dim>::type > type;
//         };
//         
//         template<typename container_collection_typemap, long dim>
//         struct ncell_range<const storage::collection_t<container_collection_typemap>, dim>
//         {
//             typedef typename const_ncell_range<storage::collection_t<container_collection_typemap>, dim>::type type;
//         };
        
        
        

        
        
//         template<typename element_tag, typename boundary_cell_typelist, typename id_type>
//         struct facet_type< element_t<element_tag, boundary_cell_typelist, id_type> >
//         {
//             typedef typename element< element_t<element_tag, boundary_cell_typelist, id_type>, typename facet_tag<element_tag>::type >::type type;
//         };
        
    }
    
    

    template<typename sub_element_type_or_tag, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    typename result_of::element_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type elements( element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        typedef typename result_of::element<element_t<element_tag, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type sub_element_type;
        return typename result_of::element_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type( element.container( typename sub_element_type::tag() ) );
    }

    template<typename sub_element_type_or_tag, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    typename result_of::const_element_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type elements( const element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        typedef typename result_of::element<element_t<element_tag, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type sub_element_type;
        //std::cout << typeid(typename sub_element_type::tag).name() << std::endl;
        return typename result_of::const_element_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, sub_element_type_or_tag>::type( element.container( typename sub_element_type::tag() ) );
    }
    
    
    
    template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, typename hook_type>
    typename storage::hook::value_type<hook_type>::type & dereference_hook( element_t<element_tag, boundary_cell_container_typelist, id_type> & element, hook_type hook)
    {
        return element.dereference_hook(hook);
    }
    
    template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, typename hook_type>
    const typename storage::hook::value_type<hook_type>::type & dereference_hook( element_t<element_tag, boundary_cell_container_typelist, id_type> const & element, hook_type hook)
    {
        return element.dereference_hook(hook);
    }

    
//     template<long dim, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
//     typename result_of::ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type ncells( element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
//     {
//         return typename result_of::ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type( element.container( dimension_tag<dim>() ) );
//     }
//     
//     template<long dim, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
//     typename result_of::const_ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type ncells(const element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
//     {
//         return typename result_of::const_ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type( element.container( dimension_tag<dim>() ) );
//     }
    
    
    
    
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
    
    
//     template<long dim, typename container_collection_typemap>
//     typename result_of::ncell_range<storage::collection_t<container_collection_typemap>, dim>::type ncells( storage::collection_t<container_collection_typemap> & collection)
//     {
//         typedef typename result_of::ncell<storage::collection_t<container_collection_typemap>, dim>::type element_type;
//         return typename result_of::ncell_range<storage::collection_t<container_collection_typemap>, dim>::type( storage::collection::get<element_type>(collection) );
//     }
//     
//     template<long dim, typename container_collection_typemap>
//     typename result_of::const_ncell_range<storage::collection_t<container_collection_typemap>, dim>::type ncells( const storage::collection_t<container_collection_typemap> & collection)
//     {
//         typedef typename result_of::ncell<storage::collection_t<container_collection_typemap>, dim>::type element_type;
//         return typename result_of::const_ncell_range<storage::collection_t<container_collection_typemap>, dim>::type( storage::collection::get<element_type>(collection) );
//     }







    
    
    template<typename element_type, typename functor_type>
    struct for_each_boundary_cell_functor
    {
    public:
        
        for_each_boundary_cell_functor( element_type & element_, functor_type functor_ ) : element(element_), functor(functor_) {}
        
        template<typename boundary_cell_type>
        void operator()( viennameta::tag<boundary_cell_type> )
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
        
        viennameta::typelist::for_each<typename element_type::boundary_cell_typelist>(for_each_functor);
    }
    
    
    
    /** @brief Overload for the output streaming operator */
    template <typename element_tag, typename bnd_cell_container_typelist__, typename id_type__>
    std::ostream & operator<<(std::ostream & os, element_t<element_tag, bnd_cell_container_typelist__, id_type__> const & el)
    {
        typedef element_t<element_tag, bnd_cell_container_typelist__, id_type__> element_type;
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
