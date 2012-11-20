#ifndef VIENNAGRID_TOPOLOGIC_DOMAIN_HPP
#define VIENNAGRID_TOPOLOGIC_DOMAIN_HPP

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


namespace viennagrid
{
    namespace result_of
    {
        template<typename topologic_domain_config, typename typemap>
        struct continuous_id_generator_config_from_domain_config_helper;
        
        
        template<typename topologic_domain_config, typename element_tag, typename element_config, typename tail>
        struct continuous_id_generator_config_from_domain_config_helper< topologic_domain_config, viennameta::typelist_t<viennameta::static_pair<element_tag, element_config>, tail> >
        {
            typedef typename viennameta::typemap::result_of::insert<
                typename continuous_id_generator_config_from_domain_config_helper<topologic_domain_config, tail>::type,
                viennameta::static_pair<
                    typename viennagrid::result_of::element<topologic_domain_config, element_tag>::type,
                    typename viennagrid::result_of::element_id_tag<topologic_domain_config, element_tag>::type
                >
            >::type type;
        };
        
        template<typename topologic_domain_config>
        struct continuous_id_generator_config_from_domain_config_helper<topologic_domain_config, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        
        template<typename topologic_domain_config>
        struct continuous_id_generator_config_from_domain_config
        {
            typedef typename continuous_id_generator_config_from_domain_config_helper<topologic_domain_config, topologic_domain_config>::type type;
        };
    }
}


namespace viennagrid
{

    template<typename topologic_domain_config_>
    class topologic_domain_t
    {
    public:
        typedef topologic_domain_config_ topologic_domain_config;
        typedef typename viennagrid::result_of::element_container_typemap<topologic_domain_config>::type element_container_typelist;
        typedef typename viennagrid::storage::result_of::collection< element_container_typelist >::type domain_container_collection_type;
        
        typedef typename viennagrid::result_of::continuous_id_generator_config_from_domain_config<topologic_domain_config>::type id_generator_config;
        typedef typename viennagrid::storage::result_of::continuous_id_generator< id_generator_config >::type id_generator_type;
        typedef typename viennagrid::storage::result_of::physical_inserter<domain_container_collection_type, id_generator_type&>::type inserter_type;
        
        
        topologic_domain_t() : id_generator(), container_collection(), inserter(container_collection, id_generator) {}
        
        
        
        
        domain_container_collection_type & get_container_collection() { return container_collection; }
        const domain_container_collection_type & get_container_collection() const { return container_collection; }
        
        inserter_type & get_inserter() { return inserter; }
        
    protected:
        id_generator_type id_generator;
        domain_container_collection_type container_collection;
        inserter_type inserter;
    };
}


namespace viennagrid
{
    
    namespace result_of
    {
        template<typename something>
        struct topologic_domain;
        
        
        // topologic domain type from config
        
        template<typename key_type, typename value_type, typename tail>
        struct topologic_domain< viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail > >
        {
            typedef topologic_domain_t< viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail > > type;
        };
        
        // identity meta functor
        
        template<typename topologic_domain_config>
        struct topologic_domain< topologic_domain_t<topologic_domain_config> >
        {
            typedef topologic_domain_t<topologic_domain_config> type;
        };
        
        
        
        template<typename something>
        struct container_collection;
        
        template<typename typemap>
        struct container_collection< storage::collection_t<typemap> >
        {
            typedef storage::collection_t<typemap> type;
        };
        
        
        template<typename topologic_domain_config>
        struct container_collection< topologic_domain_t<topologic_domain_config> >
        {
            typedef typename topologic_domain_t<topologic_domain_config>::domain_container_collection_type type;
        };
    }
    
    
    
    template<typename topologic_domain_config>
    topologic_domain_t<topologic_domain_config> & topologic_domain( topologic_domain_t<topologic_domain_config> & domain) { return domain; }
    
    template<typename topologic_domain_config>
    const topologic_domain_t<topologic_domain_config> & topologic_domain( const topologic_domain_t<topologic_domain_config> & domain) { return domain; }
    
    template<typename domain_type>
    typename result_of::container_collection< domain_type >::type & container_collection( domain_type & domain)
    { return topologic_domain(domain).get_container_collection(); }

    template<typename domain_type>
    const typename result_of::container_collection< domain_type >::type & container_collection( const domain_type & domain)
    { return topologic_domain(domain).get_container_collection(); }


    template<typename domain_type>
    typename result_of::topologic_domain<domain_type>::type::inserter_type & inserter(domain_type & domain)
    { return topologic_domain(domain).get_inserter(); }
    
    
    template<typename domain_type, typename hook_type>
    typename storage::hook::value_type<hook_type>::type & dereference_hook( domain_type & domain, const hook_type & hook)
    {
        typedef typename storage::hook::value_type<hook_type>::type value_type;
        return storage::collection::get<value_type>(container_collection(domain)).dereference_hook( hook );
    }
    
    template<typename domain_type, typename hook_type>
    const typename storage::hook::value_type<hook_type>::type & dereference_hook( const domain_type & domain, const hook_type & hook)
    {
        typedef typename storage::hook::value_type<hook_type>::type value_type;
        return storage::collection::get<value_type>(container_collection(domain)).dereference_hook( hook );
    }
}



namespace viennagrid
{
    
    namespace result_of
    {
        
        template<typename domain_config, typename element_tag>
        struct element< topologic_domain_t<domain_config>, element_tag >
        {
            typedef typename element<domain_config, element_tag>::type type;
        };
        
        template<typename domain_config, typename element_tag>
        struct element_hook<topologic_domain_t<domain_config>, element_tag>
        {
            typedef typename element_container<domain_config, element_tag>::type::hook_type type;
        };
        
        template<typename domain_config, long dim>
        struct ncell< topologic_domain_t<domain_config>, dim >
        {
            typedef typename ncell<domain_config, dim>::type type;
        };
        
        template<typename domain_config, long dim>
        struct ncell_hook< topologic_domain_t<domain_config>, dim >
        {
            typedef typename ncell<domain_config, dim>::type element_type;
            typedef typename element_container<domain_config, typename element_type::tag>::type::hook_type type;
        };
        
        template<typename domain_config, long dim>
        struct ncell_range< topologic_domain_t<domain_config>, dim >
        {
            typedef viennagrid::storage::container_range_wrapper<
                typename viennagrid::storage::result_of::container_of<
                    typename topologic_domain_t<domain_config>::domain_container_collection_type,
                    typename ncell<topologic_domain_t<domain_config>, dim>::type
                >::type
            > type;
        };
        
        template<typename domain_config, long dim>
        struct const_ncell_range< topologic_domain_t<domain_config>, dim >
        {
            typedef viennagrid::storage::container_range_wrapper<
                const typename viennagrid::storage::result_of::container_of<
                    typename topologic_domain_t<domain_config>::domain_container_collection_type,
                    typename ncell<topologic_domain_t<domain_config>, dim>::type
                >::type
            > type;
        };
    }
    
    template<long dim, typename domain_config>
    typename result_of::ncell_range<topologic_domain_t<domain_config>, dim>::type ncells(topologic_domain_t<domain_config> & domain)
    {
        typedef typename result_of::ncell<topologic_domain_t<domain_config>, dim>::type element_type;
        return typename result_of::ncell_range<topologic_domain_t<domain_config>, dim>::type(
            viennagrid::storage::collection::get<element_type>(domain.get_container_collection()) );
    }
    
    template<long dim, typename domain_config>
    typename result_of::const_ncell_range<topologic_domain_t<domain_config>, dim>::type ncells(const topologic_domain_t<domain_config> & domain)
    {
        typedef const typename result_of::ncell<topologic_domain_t<domain_config>, dim>::type element_type;
        return typename result_of::const_ncell_range<topologic_domain_t<domain_config>, dim>::type(
            viennagrid::storage::collection::get<element_type>(domain.get_container_collection()) );
    }
    
    
    
    

    

    

    
    
    template<typename domain_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection<domain_type>::type,
                    viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>
                >::type::hook_type,
                bool
            >
        push_element( domain_type & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        return inserter(domain)(element);
    }
    
    

    

    
        
    
    template<typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    void set_vertex(
            viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element,
            typename viennagrid::result_of::ncell_hook< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>, 0 >::type vertex_hook,
            unsigned int pos
        )
    {
        element.container( viennagrid::dimension_tag<0>() ).set_hook( vertex_hook, pos );
    }
    
    template<typename domain_type>
    typename result_of::element_hook<domain_type, viennagrid::vertex_tag>::type get_vertex_hook( domain_type & domain, unsigned int pos )
    {
        typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        return viennagrid::storage::collection::get<vertex_type>( container_collection(domain) ).hook_at(pos);
    }
    
    
    
    
       
    template<typename element_type, typename domain_type>
    typename result_of::element_hook<domain_type, viennagrid::vertex_tag>::type create_element( domain_type & domain )
    {
        typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        return push_element(domain, vertex_type() ).first;
    }
    
    
    template<typename element_type, typename domain_type, typename hook_array_type>
    element_type & create_element( domain_type & domain, const hook_array_type & array )
    {
        element_type element = element_type( container_collection(domain) );
        
        size_t element_index = 0;
        for (typename hook_array_type::const_iterator it = array.begin(); it != array.end(); ++it, ++element_index)
            viennagrid::set_vertex( element, *it, element_index );
                
        return dereference_hook(domain, push_element(domain, element ).first);
    }    
}



#endif
