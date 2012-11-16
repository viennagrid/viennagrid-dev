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

#include "viennagrid/storage/range.hpp"
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
            topology::bndcell_generator<element_tag, dim, bnd_cell_type>::create_bnd_cells(element, inserter);
            base::create_bnd_cells(element, inserter);
        }
        
        
        using base::container;
        using base::set_bnd_cell;

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

            if (inserted.second)
                orientations_[pos].setDefaultOrientation();
            else
            {
                typedef typename result_of::const_ncell_range<bnd_cell_type, 0>::type           VertexOnElementConstRange;
                typedef typename result_of::const_iterator<VertexOnElementConstRange>::type     VertexOnElementConstIterator;

                typedef typename result_of::ncell_range<bnd_cell_type, 0>::type      VertexOnElementRange;
                typedef typename result_of::iterator<VertexOnElementRange>::type     VertexOnElementIterator;
                

                long i=0; dim_type j=0;
                                    
                //set orientation:
                VertexOnElementRange vertices_on_element = ncells<0>( elements_[pos] );
                for (VertexOnElementIterator voeit = vertices_on_element.begin();
                        voeit != vertices_on_element.end();
                        ++voeit, ++i)
                {
                    
                    VertexOnElementConstRange vertices_on_element_2 = ncells<0>( to_insert );
                    for (VertexOnElementConstIterator voeit2 = vertices_on_element_2.begin();
                            voeit2 != vertices_on_element_2.end();
                            ++voeit2, ++j)
                    {
                        if (voeit->id() == voeit2->id())
                        {
                            orientations_[pos].setPermutation(i,j);
                            break;
                        }
                    }
                    j=0;
                }
                
            }
        }
        
        static void print_class()
        {
            cout << "  [ + + ]  " << bnd_cell_tag::name() << endl;
            base::print_class();
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
            topology::bndcell_generator<element_tag, dim, bnd_cell_type>::create_bnd_cells(element, inserter);
            base::create_bnd_cells(element, inserter);
        }
        
        using base::container;
        using base::set_bnd_cell;
        
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
        
        static void print_class()
        {
            cout << "  [ + - ]  " << bnd_cell_tag::name() << endl;
            base::print_class();
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

        void container();
        void set_bnd_cell();
        
    private:
    };
    

    
    
    namespace result_of
    {
        template<typename typelist, long dim>
        struct container_of_dimension;
        
        template<long dim>
        struct container_of_dimension< viennameta::null_type, dim >
        {
            typedef viennameta::null_type type;
        };
        
        template<typename container_pair, typename tail, long dim>
        struct container_of_dimension< viennameta::typelist_t<container_pair, tail>, dim >
        {
            typedef typename container_pair::first container_type;
            typedef typename container_type::value_type value_type;
            
            typedef typename viennameta::_if<
                value_type::tag::dim == dim,
                container_type,
                typename container_of_dimension<tail, dim>::type
            >::type type;
        };
    }
    
    
    
    
    template<typename element_tag, typename bnd_cell_container_typelist__, typename id_tag>
    class element_t :
        public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::id< element_t<element_tag, bnd_cell_container_typelist__, id_tag>, id_tag>::type
                >,
        public boundary_cell_layer<element_tag, bnd_cell_container_typelist__>
    {
        typedef boundary_cell_layer<element_tag, bnd_cell_container_typelist__> base;
        
        public:
        
        typedef bnd_cell_container_typelist__ bnd_cell_container_typelist;
        typedef element_tag tag;
                    
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
                base::create_bnd_cells(*this, inserter);
            }
        }
        
        static void print_class()
        {
            cout << element_tag::name() << endl;
            base::print_class();
        }
        
        
        typedef typename result_of::container_of_dimension< bnd_cell_container_typelist__, 0>::type::hook_type vertex_hook_type;
        void set_vertex( const vertex_hook_type & vertex, unsigned int pos )
        {
            this->container(viennagrid::dimension_tag<0>()).set_hook( vertex, pos );
        }
    };
            

    // separate specialization for vertices at the moment
    template<typename bnd_cell_container_typelist__, typename id_tag>
    class element_t<vertex_tag, bnd_cell_container_typelist__, id_tag> :
        public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::id< element_t<vertex_tag, bnd_cell_container_typelist__, id_tag>, id_tag>::type
                >
    {
        typedef element_t<vertex_tag, bnd_cell_container_typelist__, id_tag>            self_type;
        
        public:
        
        typedef bnd_cell_container_typelist__ bnd_cell_container_typelist;
        typedef vertex_tag tag;
                    
        template<typename inserter_type>
        void insert_callback( inserter_type & inserter, bool inserted ) {}
        
        static void print_class()
        {
            cout << vertex_tag::name() << endl;
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
        template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
        struct ncell< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
        {
            typedef typename container_of_dimension< boundary_cell_container_typelist, dim >::type::value_type type;
        };
        
        template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
        struct ncell_hook< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
        {
            typedef typename container_of_dimension< boundary_cell_container_typelist, dim >::type::hook_type type;
        };
        
        template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
        struct ncell_range< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
        {
            //typedef typename container_of_dimension<boundary_cell_container_typelist, dim>::type type;
            typedef viennagrid::storage::container_range_wrapper< typename container_of_dimension<boundary_cell_container_typelist, dim>::type > type;
        };
        
        template<typename element_tag, typename boundary_cell_container_typelist, typename id_type, long dim>
        struct const_ncell_range< element_t<element_tag, boundary_cell_container_typelist, id_type>, dim >
        {
            //typedef const typename container_of_dimension<boundary_cell_container_typelist, dim>::type type;
            typedef const viennagrid::storage::container_range_wrapper< const typename container_of_dimension<boundary_cell_container_typelist, dim>::type > type;
        };
        
    }
    
    
    template<long dim, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    typename result_of::ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type ncells( element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        return typename result_of::ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type( element.container( dimension_tag<dim>() ) );
    }
    
    template<long dim, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    typename result_of::const_ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type ncells(const element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        return typename result_of::const_ncell_range<element_t<element_tag, boundary_cell_container_typelist, id_type>, dim>::type( element.container( dimension_tag<dim>() ) );
    }
    
    
    
    
    /** @brief Overload for the output streaming operator */
    template <typename element_tag, typename bnd_cell_container_typelist__, typename id_type__>
    std::ostream & operator<<(std::ostream & os, element_t<element_tag, bnd_cell_container_typelist__, id_type__> const & el)
    {
        typedef element_t<element_tag, bnd_cell_container_typelist__, id_type__> element_type;
        typedef typename viennagrid::result_of::const_ncell_range< element_type, 0 >::type vertex_range;
        typedef typename viennagrid::result_of::const_iterator< vertex_range >::type const_vertex_iterator;
        
        
        os << "-- " << element_tag::name() << ", ID: " << el.id() << " --";
        const vertex_range & vertices = ncells<0>(el);
        for (const_vertex_iterator vit  = vertices.begin();
                            vit != vertices.end();
                        ++vit)
        os << std::endl << "  " << *vit;
        
        return os;
    }
  
}


#endif
