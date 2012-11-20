#ifndef VIENNAGRID_GEOMETRIC_DOMAIN_HPP
#define VIENNAGRID_GEOMETRIC_DOMAIN_HPP

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

#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"

namespace viennagrid
{
    struct topologic_config_tag;
    struct vector_type_tag;
    struct metainfo_typelist_tag;
    struct metainfo_container_config_tag;
    
    namespace result_of
    {
        template<typename domain_config>
        struct topologic_config
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, topologic_config_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct vector_type
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, vector_type_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct metainfo_typelist
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_typelist_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct metainfo_container_config
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_container_config_tag>::type::second type;
        };
        
    }
    
    
    template<typename domain_config_>
    class geometric_domain_t
    {
        typedef geometric_domain_t<domain_config_> self_type;
        
    public:
        
        typedef typename viennagrid::topologic_domain_t<
                typename result_of::topologic_config<domain_config_>::type
            > topological_domain_type;
            
        typedef typename viennagrid::storage::collection_t<
                typename viennagrid::result_of::metainfo_container_typemap<
                    typename viennameta::typelist::result_of::push_back<
                        typename result_of::metainfo_typelist< domain_config_ >::type,
                        viennameta::static_pair<
                            viennagrid::vertex_tag,
                            typename result_of::vector_type<domain_config_>::type
                        >
                    >::type,
                    typename result_of::metainfo_container_config<domain_config_>::type,
                    typename result_of::topologic_config<domain_config_>::type
                >::type
            > metainfo_collection_type;        

        
        typedef typename result_of::element< self_type, viennagrid::vertex_tag >::type vertex_type;
        typedef typename result_of::element_hook< self_type, viennagrid::vertex_tag >::type vertex_hook_type;
        
        typedef typename result_of::vector_type<domain_config_>::type vector_type;
        
        
        topological_domain_type & get_topological_domain() { return topological_domain; }
        const topological_domain_type & get_topological_domain() const { return topological_domain; }
        
        metainfo_collection_type & get_metainfo_collection() { return metainfo_collection; }
        const metainfo_collection_type & get_metainfo_collection() const { return metainfo_collection; }
        
    private:
        
        topological_domain_type topological_domain;
        metainfo_collection_type metainfo_collection;
    };
    
    
    
    namespace result_of
    {
        template<typename geometric_domain_config, typename element_tag>
        struct element< geometric_domain_t<geometric_domain_config>, element_tag >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type topologic_domain_type;
            typedef typename element<topologic_domain_type, element_tag>::type type;
        };
        
        template<typename geometric_domain_config, typename element_tag>
        struct element_hook< geometric_domain_t<geometric_domain_config>, element_tag >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type topologic_domain_type;
            typedef typename element_hook<topologic_domain_type, element_tag>::type type;
        };
        
        template<typename geometric_domain_config, long dim>
        struct ncell< geometric_domain_t<geometric_domain_config>, dim >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type topologic_domain_type;
            typedef typename ncell<topologic_domain_type, dim>::type type;
        };
        
        template<typename geometric_domain_config, long dim>
        struct ncell_hook< geometric_domain_t<geometric_domain_config>, dim >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type topologic_domain_type;
            typedef typename ncell_hook<topologic_domain_type, dim>::type type;
        };
        
        
        template<typename geometric_domain_config, long dim>
        struct ncell_range< geometric_domain_t<geometric_domain_config>, dim >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type topologic_domain_type;
            typedef typename ncell_range< topologic_domain_type, dim>::type type;
        };
        
        template<typename geometric_domain_config, long dim>
        struct const_ncell_range< geometric_domain_t<geometric_domain_config>, dim >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type topologic_domain_type;
            typedef typename const_ncell_range< topologic_domain_type, dim>::type type;
        };
        
        
        
        template<typename geometric_domain_config>
        struct topologic_domain< geometric_domain_t<geometric_domain_config> >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type type;
        };
        
        template<typename geometric_domain_config>
        struct container_collection< geometric_domain_t<geometric_domain_config> >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::topological_domain_type::domain_container_collection_type type;
        };
        
        
        
        template<typename something>
        struct metainfo_collection;
        
        template<typename geometric_domain_config>
        struct metainfo_collection< geometric_domain_t<geometric_domain_config> >
        {
            typedef typename geometric_domain_t<geometric_domain_config>::metainfo_collection_type type;
        };
        
    }
    
    
    template<typename geometric_domain_config>
    typename result_of::topologic_domain< geometric_domain_t< geometric_domain_config > >::type & topologic_domain( geometric_domain_t< geometric_domain_config > & domain )
    { return domain.get_topological_domain(); }
    
    template<typename geometric_domain_config>
    const typename result_of::topologic_domain< geometric_domain_t< geometric_domain_config > >::type & topologic_domain( const geometric_domain_t< geometric_domain_config > & domain )
    { return domain.get_topological_domain(); }
    
    template<typename geometric_domain_config>
    typename result_of::metainfo_collection< geometric_domain_t< geometric_domain_config > >::type & metainfo_collection( geometric_domain_t< geometric_domain_config > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename geometric_domain_config>
    const typename result_of::metainfo_collection< geometric_domain_t< geometric_domain_config > >::type & metainfo_collection( const geometric_domain_t< geometric_domain_config > & domain )
    { return domain.get_metainfo_collection(); }

    
    
    template<typename geometric_domain_config, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< geometric_domain_t<geometric_domain_config> >::type,
                    viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>
                >::type::hook_type,
                bool
            >
        push_element( geometric_domain_t<geometric_domain_config> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        metainfo::increment_size< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> >(metainfo_collection(domain));
        return inserter(domain)(element);
    }
    
    
    
    
    template<long dim, typename domain_config_>
    typename result_of::ncell_range<geometric_domain_t<domain_config_>, dim>::type ncells(geometric_domain_t<domain_config_> & domain)
    {
        return ncells<dim>( domain.get_topological_domain() );
    }
    
    template<long dim, typename domain_config_>
    typename result_of::const_ncell_range<geometric_domain_t<domain_config_>, dim>::type ncells(const geometric_domain_t<domain_config_> & domain)
    {
        return ncells<dim>( domain.get_topological_domain() );
    }
    
    
    template<typename metainfo_type, typename domain_config, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    metainfo_type & look_up( geometric_domain_t<domain_config> & domain, const element_t<element_tag, boundary_cell_container_typelist, id_type> & element )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), element );
    }
    
    template<typename metainfo_type, typename domain_config, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    const metainfo_type & look_up( const geometric_domain_t<domain_config> & domain, const element_t<element_tag, boundary_cell_container_typelist, id_type> & element )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), element );
    }
    
    
    template<typename metainfo_type, typename domain_config, typename element_hook_type>
    metainfo_type & look_up( geometric_domain_t<domain_config> & domain, const element_hook_type & element_hook )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), dereference_hook(domain, element_hook) );
    }
    
    template<typename metainfo_type, typename domain_config, typename element_hook_type>
    const metainfo_type & look_up( const geometric_domain_t<domain_config> & domain, const element_hook_type & element_hook )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), dereference_hook(domain, element_hook) );
    }
    
    
    
    
    template<typename domain_config, typename element_type, typename metainfo_type>
    void set( geometric_domain_t<domain_config> & domain, const element_type & element, const metainfo_type & meta_info )
    {
        metainfo::set( get_info<element_type, metainfo_type>(domain.get_metainfo_collection()), element, meta_info );
    }
    
    
    
    
    template<typename domain_config>
    typename geometric_domain_t<domain_config>::vector_type & point( geometric_domain_t<domain_config> & geometric_domain, const typename geometric_domain_t<domain_config>::vertex_type & vertex )
    {
        return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, vertex );
    }
    
    template<typename domain_config>
    const typename geometric_domain_t<domain_config>::vector_type & point( const geometric_domain_t<domain_config> & geometric_domain, const typename geometric_domain_t<domain_config>::vertex_type & vertex )
    {
        return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, vertex );
    }
    
    template<typename domain_config>
    typename geometric_domain_t<domain_config>::vector_type & point( geometric_domain_t<domain_config> & geometric_domain, const typename geometric_domain_t<domain_config>::vertex_hook_type & vertex_hook )
    {
        return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, dereference_hook(geometric_domain, vertex_hook) );
    }
    
    template<typename domain_config>
    const typename geometric_domain_t<domain_config>::vector_type & point( const geometric_domain_t<domain_config> & geometric_domain, const typename geometric_domain_t<domain_config>::vertex_hook_type & vertex_hook )
    {
        return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, dereference_hook(geometric_domain, vertex_hook) );
    }
    
//     template<typename domain_config>
//     typename geometric_domain_t<domain_config>::vector_type & point( geometric_domain_t<domain_config> & geometric_domain, std::size_t pos )
//     {
//         return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, viennagrid::elements<viennagrid::vertex_tag>( geometric_domain )[pos] );
//     }
//     
//     template<typename domain_config>
//     const typename geometric_domain_t<domain_config>::vector_type & point( const geometric_domain_t<domain_config> & geometric_domain, std::size_t pos )
//     {
//         return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, viennagrid::elements<viennagrid::vertex_tag>( geometric_domain )[pos] );
//     }
    
    
    
    namespace result_of
    {
        
        template<typename element_tag, typename vector_type, typename hook_tag = viennagrid::storage::pointer_hook_tag,
            typename metainfo_typelist = viennameta::null_type, typename metainfo_container_config = viennagrid::storage::default_container_config>
        struct geometric_domain_config
        {
            typedef typename viennagrid::result_of::default_topologic_config<viennagrid::tetrahedron_tag, hook_tag>::type toplologic_config;
            typedef typename viennameta::make_typemap<
                viennagrid::topologic_config_tag, toplologic_config,
                viennagrid::vector_type_tag, vector_type,
                viennagrid::metainfo_typelist_tag, metainfo_typelist,
                viennagrid::metainfo_container_config_tag, metainfo_container_config
            >::type type;
        };
        
        
        template<typename geometric_domain_config>
        struct geometric_domain
        {
            typedef geometric_domain_t<geometric_domain_config> type;
        };
        
    }
}

#endif
