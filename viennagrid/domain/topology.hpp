#ifndef VIENNAGRID_TOPOLOGY_HPP
#define VIENNAGRID_TOPOLOGY_HPP

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

#include "viennagrid/meta/algorithm.hpp"

#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/algorithm.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"


namespace viennagrid
{
    
    
    template<typename container_collection_type>
    class view_domain_setter
    {
    public:
        
        view_domain_setter(container_collection_type & domain_) : domain(domain_) {}
        
        template<typename container_type>
        void operator()( container_type & container )
        {
            typedef typename container_type::value_type value_type;
            container.set_base_container( viennagrid::storage::collection::get<value_type>(domain) );
        }
        
        
    private:
        container_collection_type & domain;
    };

    
    template<typename domain_type>
    struct domain_proxy
    {
        domain_proxy( domain_type & domain_ ) : domain(&domain_){}
        
        domain_type * domain;
    };
    
    
    template<typename container_type_>
    struct coboundary_container_wrapper
    {
        typedef container_type_ container_type;
        coboundary_container_wrapper() : change_counter(0) {}
        
        long change_counter;
        container_type container;
    };
    
    
    template<typename container_type_>
    struct neighbour_container_wrapper
    {
        typedef container_type_ container_type;
        neighbour_container_wrapper() : change_counter(0) {}
        
        long change_counter;
        container_type container;
    };
    
    template<typename container_type_>
    struct boundary_information_wrapper
    {
        typedef container_type_ container_type;
        boundary_information_wrapper() : change_counter(0) {}
        
        long change_counter;
        container_type container;
    };
    
    namespace result_of
    {
      template<typename element_typelist, typename container_typemap>
      struct filter_element_container;
      
      template<typename container_typemap>
      struct filter_element_container<viennagrid::meta::null_type, container_typemap>
      {
          typedef viennagrid::meta::null_type type;
      };
      
      template<typename element_type, typename tail, typename container_typemap>
      struct filter_element_container<viennagrid::meta::typelist_t< element_type, tail> , container_typemap>
      {
          typedef typename viennagrid::meta::typemap::result_of::find<container_typemap, element_type>::type result_type;
          
          typedef typename filter_element_container<tail, container_typemap>::type new_tail;
          
          typedef typename viennagrid::meta::IF<
              viennagrid::meta::EQUAL< result_type, viennagrid::meta::not_found >::value,
              new_tail,
              viennagrid::meta::typelist_t<
                  result_type,
                  new_tail
              >
          >::type type;
      };
      

      // domain_element_collection_type
      template <typename WrappedConfigType>
      struct domain_element_collection_type
      {
        typedef typename WrappedConfigType::type   ConfigType;

        typedef typename result_of::element_collection< ConfigType >::type   type;
      };
      
      template <typename WrappedDomainConfigType, typename ElementTypeList, typename ContainerConfig>
      struct domain_element_collection_type< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >
      {
        typedef typename WrappedDomainConfigType::type   DomainConfigType;

        typedef typename domain_element_collection_type<WrappedDomainConfigType>::type   full_domain_element_collection_type;

        typedef typename filter_element_container<ElementTypeList, typename full_domain_element_collection_type::typemap>::type      view_container_collection_typemap;
        typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, ContainerConfig>::type   type;
      };

      // domain_appendix_type
      template <typename WrappedConfigType>
      struct domain_appendix_type
      {
        typedef typename WrappedConfigType::type   ConfigType;
            
        typedef typename viennagrid::storage::result_of::collection< typename viennagrid::result_of::coboundary_container_collection_typemap<ConfigType>::type >::type   coboundary_collection_type;        
        typedef typename viennagrid::storage::result_of::collection< typename viennagrid::result_of::neighbour_container_collection_typemap< ConfigType>::type >::type   neighbour_collection_type;        
        typedef typename viennagrid::storage::result_of::collection< typename viennagrid::result_of::boundary_information_collection_typemap<ConfigType>::type >::type   boundary_information_type;
          
        typedef typename viennagrid::storage::collection_t<
              typename viennagrid::meta::make_typemap<
              
                  coboundary_collection_tag,
                  coboundary_collection_type,
                  
                  neighbour_collection_tag,
                  neighbour_collection_type,
                  
                  boundary_information_collection_tag,
                  boundary_information_type
                  
              >::type
        > type;
      };

      template <typename WrappedDomainConfigType, typename ElementTypeList, typename ContainerConfig>
      struct domain_appendix_type< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >
      {
        typedef typename domain_appendix_type<WrappedDomainConfigType>::type    type;
      };
      
      // domain_inserter_type
      template <typename WrappedConfigType>
      struct domain_inserter_type
      {
        typedef typename WrappedConfigType::type   ConfigType;
          
        typedef typename result_of::element_collection<ConfigType>::type                                                       element_collection_type;
        typedef typename viennagrid::result_of::id_generator<ConfigType>::type                                                 id_generator_type;

        typedef typename viennagrid::storage::result_of::physical_inserter<element_collection_type, id_generator_type>::type   type;
      };
      
      template <typename WrappedDomainConfigType, typename ElementTypeList, typename ContainerConfig>
      struct domain_inserter_type< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >
      {
        typedef decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig>  argument_type;
        typedef typename WrappedDomainConfigType::type                                                   DomainConfigType;

        typedef typename domain_element_collection_type<argument_type>::type                             view_container_collection_type;
        typedef typename domain_inserter_type<WrappedDomainConfigType>::type                             full_domain_inserter_type;
          
        typedef typename viennagrid::storage::result_of::recursive_inserter<view_container_collection_type, full_domain_inserter_type>::type      type;
      };

      
    }

    template <typename WrappedConfigType>
    class domain_t
    {
        typedef domain_t<WrappedConfigType> self_type;

    public:
        typedef WrappedConfigType                    wrapped_config_type;
        typedef typename WrappedConfigType::type     config_type;
        
        
        typedef typename result_of::domain_element_collection_type<WrappedConfigType>::type     element_collection_type;
        typedef typename result_of::domain_appendix_type<WrappedConfigType>::type               appendix_type;
        typedef typename result_of::domain_inserter_type<WrappedConfigType>::type               inserter_type;
        
        
        domain_t() : inserter( element_container_collection ), change_counter_(0) {}
        
        template<typename OtherWrappedConfig>
        domain_t( domain_proxy<domain_t<OtherWrappedConfig> > proxy ) : change_counter_(0)
        {
            typedef typename domain_t<OtherWrappedConfig>::element_collection_type   other_element_collection_type;
          
            view_domain_setter< other_element_collection_type > functor(proxy.domain->element_collection());
            viennagrid::storage::collection::for_each(element_container_collection, functor);
        
            inserter = inserter_type( element_container_collection, proxy.domain->get_inserter() );
        }
        
        ~domain_t() {}

        
        
        // buggy !!
        domain_t(const domain_t & other) : element_container_collection(other.element_container_collection), inserter(other.inserter), change_counter_(other.change_counter_)
        {
            inserter.set_container_collection( element_container_collection );
        }
        
        // buggy !!
        domain_t & operator=( domain_t const & other)
        {
            element_container_collection = other.element_container_collection;
            inserter = other.inserter;
            inserter.set_container_collection( element_container_collection );
            return *this;
        }
    public:
        
        // TODO no direct access to collection!
        element_collection_type & element_collection() { return element_container_collection; }
        element_collection_type const & element_collection() const { return element_container_collection; }
        
        appendix_type & appendix() { return appendix_; }
        appendix_type const & appendix() const { return appendix_; }
        
//         coboundary_collection_type & coboundary_collection() { return coboundary_container_collection; }
//         coboundary_collection_type const & coboundary_collection() const { return coboundary_container_collection; }
//         
//         neighbour_collection_type & neighbour_collection() { return neighbour_container_collection; }
//         neighbour_collection_type const & neighbour_collection() const { return neighbour_container_collection; }
// 
//         boundary_information_collection_type & boundary_information() { return boundary_information_collection; }
//         boundary_information_collection_type const & boundary_information() const { return boundary_information_collection; }


        
        inserter_type & get_inserter() { return inserter; }
        inserter_type const & get_inserter() const { return inserter; }
        
        typedef long ChangeCounterType;
        
//         ChangeCounterType change_counter() const { return change_counter_; }
        bool is_obsolete( ChangeCounterType change_counter_to_check ) const
        {
//           std::cout << "CC: " << change_counter_ << " " << change_counter_to_check << std::endl;
          return change_counter_to_check != change_counter_;
        }
        void update_change_counter( ChangeCounterType & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
        void increment_change_counter() { ++change_counter_; }

        
    protected:
        element_collection_type element_container_collection;

        appendix_type appendix_;
//         coboundary_collection_type coboundary_container_collection;
//         neighbour_collection_type neighbour_container_collection;
//         boundary_information_collection_type boundary_information_collection;
        
        inserter_type inserter;
        
        ChangeCounterType change_counter_;
    };
    
    
    
    template<typename WrappedConfigType>
    bool is_obsolete( domain_t<WrappedConfigType> const & domain, typename domain_t<WrappedConfigType>::ChangeCounterType change_counter_to_check )
    { return domain.is_obsolete( change_counter_to_check ); }
    
    template<typename WrappedConfigType>
    void update_change_counter( domain_t<WrappedConfigType> & domain, typename domain_t<WrappedConfigType>::ChangeCounterType & change_counter_to_update )
    { domain.update_change_counter( change_counter_to_update ); }
    
    template<typename WrappedConfigType>
    void increment_change_counter( domain_t<WrappedConfigType> & domain )
    { domain.increment_change_counter(); }

    

    
    
    template<typename element_tag, typename coboundary_tag, typename domain_type>
    typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    coboundary_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, coboundary_tag>
                >::type & coboundary_collection( domain_type & domain )
    { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, coboundary_tag> >( viennagrid::storage::collection::get<coboundary_collection_tag>( domain.appendix() ) );}
    
    template<typename element_tag, typename coboundary_tag, typename domain_type>
    typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    coboundary_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, coboundary_tag>
                >::type const & coboundary_collection( domain_type const & domain )
    { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, coboundary_tag> >( viennagrid::storage::collection::get<coboundary_collection_tag>( domain.appendix() ) );}
    
    
    
    template<typename element_tag, typename connector_element_tag, typename domain_type>
    typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    neighbour_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, connector_element_tag>
                >::type & neighbour_collection( domain_type & domain )
    { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, connector_element_tag> >( viennagrid::storage::collection::get<neighbour_collection_tag>( domain.appendix() ) ); }
    
    template<typename element_tag, typename connector_element_tag, typename domain_type>
    typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    neighbour_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, connector_element_tag>
                >::type const & neighbour_collection( domain_type const & domain )
    { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, connector_element_tag> >( viennagrid::storage::collection::get<neighbour_collection_tag>( domain.appendix() ) ); }
    
    
    
    template<typename element_tag, typename domain_type>
    typename viennagrid::storage::result_of::value_type<
        typename viennagrid::storage::result_of::value_type<
            typename domain_type::appendix_type,
            boundary_information_collection_tag 
        >::type,
        element_tag
    >::type & boundary_information_collection( domain_type & domain )
    { return viennagrid::storage::collection::get<element_tag>( viennagrid::storage::collection::get<boundary_information_collection_tag>( domain.appendix() ) ); }   
    
    template<typename element_tag, typename domain_type>
    typename viennagrid::storage::result_of::value_type<
        typename viennagrid::storage::result_of::value_type<
            typename domain_type::appendix_type,
            boundary_information_collection_tag 
        >::type,
        element_tag
    >::type const & boundary_information_collection( domain_type const & domain )
    { return viennagrid::storage::collection::get<element_tag>( viennagrid::storage::collection::get<boundary_information_collection_tag>( domain.appendix() ) ); }   
    
}


namespace viennagrid
{
    
    namespace result_of
    {
        template<typename something>
        struct container_collection_typemap;
        
        template<typename typemap>
        struct container_collection_typemap< storage::collection_t<typemap> >
        {
            typedef typemap type;
        };
        
        template<typename typemap>
        struct container_collection_typemap< const storage::collection_t<typemap> >
        {
            typedef typemap type;
        };
        
        template<typename key_type, typename value_type, typename tail>
        struct container_collection_typemap< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<key_type, value_type>, tail > >
        {
            typedef viennagrid::meta::typelist_t< viennagrid::meta::static_pair<key_type, value_type>, tail > type;
        };

        
        
        template<typename typemap>
        struct element_collection< storage::collection_t<typemap> >
        {
            typedef storage::collection_t<typemap> type;
        };
        
        
        template<typename ConfigType>
        struct element_collection< domain_t<ConfigType> >
        {
            typedef typename domain_t<ConfigType>::element_collection_type type;
        };
        
        template<typename ConfigType>
        struct element_collection< const domain_t<ConfigType> >
        {
            typedef typename domain_t<ConfigType>::element_collection_type type;
        };        
        
        template<typename domain_type>
        struct element_typelist
        {
            typedef typename element_collection<domain_type>::type container_collection;
            typedef typename viennagrid::meta::typemap::result_of::key_typelist<typename container_collection::typemap >::type type;
        };
        
        template<typename domain_type>
        struct element_taglist
        {
            typedef typename viennagrid::meta::typelist::TRANSFORM<
                element_tag,
                typename element_typelist<domain_type>::type
            >::type type;
        };
        
        // for config
//         template<typename head, typename tail>
//         struct element_taglist< viennagrid::meta::typelist_t<head, tail> >
//         {
//             typedef typename viennagrid::meta::typemap::key_typelist< viennagrid::meta::typelist_t<head, tail> >::type type;
//         };
        
        template<typename something, typename boundary_element_type>
        struct is_boundary_type;
        
        template<typename boundary_element_type>
        struct is_boundary_type< viennagrid::meta::null_type, boundary_element_type >
        {
            static const bool value = false;
        };
        
        template<typename head, typename tail, typename boundary_element_type>
        struct is_boundary_type< viennagrid::meta::typelist_t<head, tail>, boundary_element_type >
        {
            typedef typename head::first boundary_container_type;
            typedef typename boundary_container_type::value_type element_type;
            
            static const bool value = viennagrid::meta::EQUAL< element_type, boundary_element_type >::value ? true : is_boundary_type<tail, boundary_element_type>::value;
        };
        
        template<typename element_tag, typename WrappedConfigType, typename boundary_element_type>
        struct is_boundary_type< element_t<element_tag, WrappedConfigType>, boundary_element_type>
        {
            typedef typename result_of::element_boundary_cell_container_typelist<WrappedConfigType>::type bnd_cell_container_typelist;
            static const bool value = is_boundary_type<bnd_cell_container_typelist, boundary_element_type>::value;
        };
        
        
        
        
        template<typename element_typelist, typename element_type>
        struct referencing_element_typelist_impl;

        template<typename element_type>
        struct referencing_element_typelist_impl<viennagrid::meta::null_type, element_type>
        {
            typedef viennagrid::meta::null_type type;
        };
        
        template<typename head, typename tail, typename element_type>
        struct referencing_element_typelist_impl< viennagrid::meta::typelist_t<head, tail>, element_type >
        {
            typedef typename referencing_element_typelist_impl<tail, element_type>::type tail_typelist;
            
            typedef typename viennagrid::meta::IF<
                is_boundary_type<head, element_type>::value,
                viennagrid::meta::typelist_t< head, tail_typelist>,
                tail_typelist
            >::type type;
        };

        
        template<typename domain_type, typename element_type_or_tag>
        struct referencing_element_typelist
        {
            typedef typename element<domain_type, element_type_or_tag>::type element_type;
            typedef typename element_typelist<domain_type>::type elements;
            typedef typename referencing_element_typelist_impl<elements, element_type>::type type;
        };
        
        
        
        
        template<typename element_typelist>
        struct element_to_tag;

        template<>
        struct element_to_tag<viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };
        
        template<typename element_tag, typename WrappedConfigType, typename tail>
        struct element_to_tag< viennagrid::meta::typelist_t< element_t<element_tag, WrappedConfigType>, tail > >
        {
            typedef viennagrid::meta::typelist_t<
                element_tag,
                typename element_to_tag<tail>::type
            > type;
        };
        
        
        
        
        template<typename something>
        struct cell_dimension;

        template<>
        struct cell_dimension<viennagrid::meta::null_type>
        {
            static const int value = -1;
        };
        
        template<typename key_type, typename value_type, typename tail>
        struct cell_dimension< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<key_type, value_type>, tail > >
        {
            typedef typename value_type::value_type element_type;
            static const int current_topologic_dim = viennagrid::result_of::topologic_dimension<element_type>::value;
            static const int tail_topologic_dim = cell_dimension<tail>::value;
            
            static const int value = (current_topologic_dim < tail_topologic_dim) ? tail_topologic_dim : current_topologic_dim;
        };
        
        template<typename typemap>
        struct cell_dimension< storage::collection_t<typemap> >
        {
            static const int value = cell_dimension<typemap>::value;
        };
        
        template<typename ConfigType>
        struct cell_dimension< domain_t<ConfigType> >
        {
            static const int value = cell_dimension<typename domain_t<ConfigType>::element_collection_type>::value;
        };
        
        
        
        
        
        
    }
        
    template <typename DomainConfigType, typename ElementTypeList, typename ContainerConfig>
    class decorated_domain_view_config
    {
      private:
        typedef typename DomainConfigType::type      config_type;
        
        typedef typename result_of::element_collection< config_type >::type                                                   element_collection_type;
        typedef typename viennagrid::result_of::id_generator<config_type>::type                                               id_generator_type;

        typedef typename result_of::filter_element_container<ElementTypeList, typename element_collection_type::typemap>::type        view_container_collection_typemap;
        typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, ContainerConfig>::type    view_container_collection_type;
        

      public:
        typedef view_container_collection_typemap    type;

        typedef DomainConfigType                     domain_config_type;
    };
    
  
    template <typename DomainConfigType, typename E, typename C, typename ElementTypeList, typename ContainerConfig>
    class decorated_domain_view_config< decorated_domain_view_config<DomainConfigType, E, C>, ElementTypeList, ContainerConfig>
    {
      private:
        typedef typename DomainConfigType::type      config_type;
        
        typedef typename result_of::element_collection< config_type >::type                                                   element_collection_type;
        typedef typename viennagrid::result_of::id_generator<config_type>::type                                               id_generator_type;

        typedef typename result_of::filter_element_container<ElementTypeList, typename element_collection_type::typemap>::type        view_container_collection_typemap;
        typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, ContainerConfig>::type    view_container_collection_type;
        

      public:
        typedef view_container_collection_typemap    type;

        typedef typename decorated_domain_view_config<DomainConfigType, E, C>::domain_config_type      domain_config_type;
    };
  

    namespace result_of
    {
        
        template<
            typename domain_type,
            typename element_typelist = 
                typename viennagrid::meta::typemap::result_of::key_typelist<
                    typename element_collection<domain_type>::type::typemap
                >::type,
            typename container_config = 
                storage::default_view_container_config
            >
        struct domain_view
        {
            typedef typename result_of::element_collection< domain_type >::type element_collection_type;
            typedef typename domain_type::inserter_type domain_inserter_type;
            
            typedef typename filter_element_container<element_typelist, typename element_collection_type::typemap>::type view_container_collection_typemap;
            typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, container_config>::type view_container_collection_type;
            
            typedef typename viennagrid::storage::result_of::recursive_inserter<view_container_collection_type, domain_inserter_type>::type view_inserter_type;
            

            // TODO: change viennagrid::meta::null_type !!!
            typedef domain_t< decorated_domain_view_config<typename domain_type::wrapped_config_type, element_typelist, container_config> >  type;
            //typedef domain_t<view_container_collection_type, typename domain_type::appendix_type, view_inserter_type> type; 
        };
    }
    
    
//     template<typename container_collection_type, typename inserter_type>
//     void init_domain( topology_t<container_collection_type, inserter_type> & domain )
//     {
//         typedef typename inserter_type::id_generator_type id_generator_type;
//         domain.get_inserter() = inserter_type( domain.get_container_collection(), id_generator_type() );
//     }
//     
//     template<typename topology_type>
//     topology_type create_topology()
//     {
//         topology_type domain;
//         init_domain(domain);
//         return domain;
//     }
    
    

    
    
    

    
    
//     template<typename topology_view_type, typename topology_type> 
//     void init_view( topology_view_type & view, topology_type & domain )
//     {
//         view_domain_setter< typename result_of::container_collection<topology_type>::type > functor(domain.get_container_collection());
//         viennagrid::storage::collection::for_each(view.get_container_collection(), functor);
//         
//         view.get_inserter() = typename topology_view_type::inserter_type( view.get_container_collection(), domain.get_inserter() );
//     }
    

//     template<typename view_type>
//     struct create_view_helper;
// 
//     
//     template<typename A, typename B, typename C, typename D, typename E>
//     struct create_view_helper< domain_t<A, B, C, D, E> >
//     {
//         typedef domain_t<A, B, C, D, E> doman_view_type;
//         
//         template<typename domain_type>
//         static doman_view_type create( domain_type & domain )
//         {
//             doman_view_type view;      
//             init_view(view, domain);
//             return view;
//         }
//     };
    
//     template<typename topology_view_type, typename topology_type>
//     topology_view_type create_view( topology_type & domain )
//     {
//         return create_view_helper<topology_view_type>::create(domain);
//     }
    
    
    template<typename topology_type>
    domain_proxy<topology_type> create_view( topology_type & domain )
    {
        return domain_proxy<topology_type>( domain );
    }
    
    
    
    
    template<typename container_collection_type>
    class handle_domain_functor
    {
    public:
        
        handle_domain_functor(container_collection_type & collection_) : collection(collection_) {}
        
        template<typename container_type>
        void operator()( container_type & container )
        {
            typedef typename container_type::value_type value_type;
            //container.set_base_container(  );
            storage::view::handle( viennagrid::storage::collection::get<value_type>(collection), container);
        }
        
        
    private:
        container_collection_type & collection;
    };
    
    template<typename view_type, typename domain_type>
    void handle_domain( view_type & view, domain_type & domain )
    {
        handle_domain_functor< typename result_of::element_collection<domain_type>::type > functor( element_collection(domain) );
        viennagrid::storage::collection::for_each( element_collection(view), functor);
    }
    
    
    
    
    
    template<typename view_type, typename domain_type, typename handle_type>
    void add_handle( view_type & view, domain_type & domain, handle_type handle )
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type value_type;
        value_type & element = dereference_handle(domain, handle);
        
        typedef typename viennagrid::result_of::element_range< view_type, value_type >::type range_type;
        typedef typename viennagrid::result_of::iterator<range_type>::type iterator_type;
        
        iterator_type it = find_by_id( view, element.id() );
        if ( it == elements<value_type>(view).end() )
            viennagrid::storage::collection::get<value_type>( element_collection(view) ).insert_handle( handle );
    }

    
    
    
//     template<typename A, typename B, typename C, typename d, typename E>
//     domain_t<A, B, C, D, E, inserter_type_> & topology( domain_t<domain_container_collection_type_, inserter_type_> & domain) { return domain; }
    
//     template<typename domain_container_collection_type_, typename inserter_type_>
//     const domain_t<domain_container_collection_type_, inserter_type_> & topology( const domain_t<domain_container_collection_type_, inserter_type_> & domain) { return domain; }
    
    template<typename ConfigType>
    typename domain_t<ConfigType>::element_collection_type & element_collection( domain_t<ConfigType> & domain)
    { return domain.element_collection(); }

    template<typename ConfigType>
    typename domain_t<ConfigType>::element_collection_type const & element_collection( domain_t<ConfigType> const & domain)
    { return domain.element_collection(); }


    template<typename ConfigType>
    typename domain_t<ConfigType>::inserter_type & inserter(domain_t<ConfigType> & domain)
    { return domain.get_inserter(); }
    
    template<typename ConfigType>
    typename domain_t<ConfigType>::inserter_type const & inserter(domain_t<ConfigType> const & domain)
    { return domain.get_inserter(); }
    

    template<typename ConfigType>
    typename domain_t<ConfigType>::inserter_type::id_generator_type & id_generator(domain_t<ConfigType> & domain)
    { return inserter(domain).get_id_generator(); }
    
    template<typename ConfigType>
    typename domain_t<ConfigType>::inserter_type::id_generator_type const & id_generator(domain_t<ConfigType> const & domain)
    { return inserter(domain).get_id_generator(); }
    
    
    


    template<typename ElementTypeOrTag, typename ConfigType>
    typename viennagrid::result_of::id_type<
      typename viennagrid::result_of::element< domain_t<ConfigType>, ElementTypeOrTag>::type
    >::type max_id( domain_t<ConfigType> const & domain )
    { return id_generator(domain).max_id( viennagrid::meta::tag< typename viennagrid::result_of::element< domain_t<ConfigType>, ElementTypeOrTag>::type >() ); }
    
    
    
    
    template<typename something>
    void clear_domain( something & s )
    {
        viennagrid::storage::container_collection::clear_all( viennagrid::element_collection(s) );
    }
    
    
    template<typename WrappedConfigType, typename handle_type>
    typename storage::handle::result_of::value_type<handle_type>::type & dereference_handle( domain_t<WrappedConfigType> & domain, handle_type const & handle)
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type value_type;
        return storage::collection::get<value_type>(element_collection(domain)).dereference_handle( handle );
    }
    
    template<typename WrappedConfigType, typename handle_type>
    typename storage::handle::result_of::value_type<handle_type>::type const & dereference_handle( domain_t<WrappedConfigType> const & domain, handle_type const & handle)
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type value_type;
        return storage::collection::get<value_type>(element_collection(domain)).dereference_handle( handle );
    }
    
    template<typename domain_type, typename ElementTag, typename WrappedConfigType>
    element_t<ElementTag, WrappedConfigType> & dereference_handle( domain_type & domain, element_t<ElementTag, WrappedConfigType> & handle)
    { return handle; }
    
    template<typename domain_type, typename ElementTag, typename WrappedConfigType>
    element_t<ElementTag, WrappedConfigType> const & dereference_handle( domain_type const & domain, element_t<ElementTag, WrappedConfigType> const & handle)
    { return handle; }
    
    
    
    
    template<typename domain_type, typename ElementTag, typename WrappedConfigType>
    typename result_of::handle<domain_type, element_t<ElementTag, WrappedConfigType> >::type handle( domain_type & domain, element_t<ElementTag, WrappedConfigType> & element)
    { return storage::collection::get< element_t<ElementTag, WrappedConfigType> >(element_collection(domain)).handle( element ); }
    
    template<typename domain_type, typename ElementTag, typename WrappedConfigType>
    typename result_of::const_handle<domain_type, element_t<ElementTag, WrappedConfigType> >::type handle( domain_type const & domain, element_t<ElementTag, WrappedConfigType> const & element)
    { return storage::collection::get< element_t<ElementTag, WrappedConfigType> >(element_collection(domain)).handle( element ); }
    
    
    template<typename domain_type, typename handle_type>
    handle_type handle( domain_type & domain, handle_type handle) { return handle; }
    
    template<typename domain_type, typename handle_type>
    const handle_type handle( domain_type const & domain, handle_type handle) { return handle; }
    
    
    
    
  template<typename domain_type,
           typename ElementType, typename WrappedConfigType,
           typename handle_type>
  typename viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type
  local_vertex(const domain_type & domain,
               element_t<ElementType, WrappedConfigType> const & host_ncell,
               handle_type const & bnd_kcell_handle,
               std::size_t index)
  {
    typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type_2;
    const element_type_2 & bnd_kcell = viennagrid::dereference_handle(domain, bnd_kcell_handle);
    return viennagrid::elements< viennagrid::vertex_tag >(bnd_kcell)[host_ncell.global_to_local_orientation(bnd_kcell_handle, index)];
  }
    
    
    
    template<typename container_type>
    class dereference_handle_comperator
    {
    public:
        
        dereference_handle_comperator(const container_type & container_) : container(container_) {}
        
        template<typename handle>
        bool operator() ( handle h1, handle h2 )
        {
            return &viennagrid::dereference_handle( container, h1 ) < &viennagrid::dereference_handle( container, h2 );
        }
        
    private:
        const container_type & container;
    };
    
}



namespace viennagrid
{
    
    namespace result_of
    {
        
        template<typename domain_type, typename element_type_or_tag>
        struct element
        {
            typedef typename element< typename element_collection<domain_type>::type, element_type_or_tag >::type type;
        };
        
        template<typename domain_type, typename element_type_or_tag>
        struct handle
        {
            typedef typename handle< typename element_collection<domain_type>::type, element_type_or_tag >::type type;
        };
        
        template<typename domain_type, typename element_type_or_tag>
        struct const_handle
        {
            typedef typename const_handle< typename element_collection<domain_type>::type, element_type_or_tag >::type type;
        };
        
        
        template<typename domain_type, typename element_type_or_tag>
        struct element_range
        {
            typedef typename element_range< typename element_collection<domain_type>::type, element_type_or_tag >::type type;
        };
        
        template<typename domain_type, typename element_type_or_tag>
        struct const_element_range
        {
            typedef typename const_element_range< typename element_collection<domain_type>::type, element_type_or_tag >::type type;
        };
        
        
        
        

        
        
        
        
        template<typename typemap>
        struct topologic_cell_dimension_impl;
        
        template<>
        struct topologic_cell_dimension_impl<viennagrid::meta::null_type>
        {
            static const int value = -1;
        };
        
        template<typename element_type, typename element_container_type, typename tail>
        struct topologic_cell_dimension_impl< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_type, element_container_type>, tail > >
        {
            static const int tail_cell_dimension = topologic_cell_dimension_impl<tail>::value;
            static const int current_element_dimension = topologic_dimension<element_type>::value;
            
            static const int value = (tail_cell_dimension > current_element_dimension) ? tail_cell_dimension : current_element_dimension;
        };
        
        template<typename something>
        struct topologic_cell_dimension
        {
            static const int value = topologic_cell_dimension_impl<
                    typename container_collection_typemap<
                        typename element_collection<something>::type
                    >::type
                >::value;
        };
        
        
        

        
        template<typename typemap, int topologic_dimension>
        struct elements_of_topologic_dim_impl;
        
        template<int topologic_dimension>
        struct elements_of_topologic_dim_impl< viennagrid::meta::null_type, topologic_dimension >
        {
            typedef viennagrid::meta::null_type type;
        };
        
        template<typename element_type, typename element_container_type, typename tail, int topologic_dimension>
        struct elements_of_topologic_dim_impl< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_type, element_container_type>, tail >, topologic_dimension >
        {
            typedef typename elements_of_topologic_dim_impl<tail, topologic_dimension>::type tail_typelist;
            
            typedef typename viennagrid::meta::IF<
                viennagrid::result_of::topologic_dimension<element_type>::value == topologic_dimension,
                typename viennagrid::meta::typelist::result_of::push_back<tail_typelist, element_type>::type,
                tail_typelist
            >::type type;
        };
        
        template<typename something, int topologic_dimension>
        struct elements_of_topologic_dim
        {
            typedef typename elements_of_topologic_dim_impl<
                typename container_collection_typemap<
                    typename element_collection<something>::type
                >::type,
                topologic_dimension>::type type;
        };
        
        
        
        template<typename something>
        struct cell_types
        {
            typedef typename elements_of_topologic_dim<
                something,
                topologic_cell_dimension<something>::value
            >::type type;
        };

        
        template<typename something>
        struct cell_type
        {
            typedef typename cell_types<something>::type all_cell_types;
            typedef typename viennagrid::meta::STATIC_ASSERT< viennagrid::meta::typelist::result_of::size<all_cell_types>::value == 1 >::type static_assert_typedef;
            
            typedef typename viennagrid::meta::typelist::result_of::at<all_cell_types,0>::type type;
        };
        
        template<typename something>
        struct cell_tag
        {
            typedef typename element_tag< typename cell_type<something>::type >::type type;
        };
    }
    
    
    
    template<typename domain_type, typename functor>
    struct for_each_element_functor
    {
        for_each_element_functor( domain_type & d, functor f ) : f_(f), domain_(d) {}
        
        template<typename element_type>
        void operator() ( viennagrid::meta::tag<element_type> )
        {
            typedef typename viennagrid::result_of::element_range<domain_type, element_type>::type element_range_type;
            typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;
            
            element_range_type range = viennagrid::elements(domain_);
            for (element_range_iterator it = range.begin(); it != range.end(); ++it)
                f_(*it);
        }
        
        template<typename element_type>
        void operator() ( viennagrid::meta::tag<element_type> ) const
        {
            typedef typename viennagrid::result_of::const_element_range<domain_type, element_type>::type element_range_type;
            typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;
            
            element_range_type range = viennagrid::elements(domain_);
            for (element_range_iterator it = range.begin(); it != range.end(); ++it)
                f_(*it);
        }
        
        functor f_;
        domain_type & domain_;
    };
    
    
    
    template<int topologic_dimension, typename domain_type, typename functor>
    void for_each( domain_type & domain, functor f )
    {
        for_each_element_functor<domain_type, functor> for_each_functor(domain, f);
        typedef typename viennagrid::result_of::elements_of_topologic_dim<domain_type, topologic_dimension>::type element_typelist;
        
        viennagrid::meta::typelist::for_each<element_typelist>( for_each_functor );
    }
    
    template<int topologic_dimension, typename domain_type, typename functor>
    void for_each( domain_type const & domain, functor f )
    {
        for_each_element_functor<const domain_type, functor> for_each_functor(domain, f);
        typedef typename viennagrid::result_of::elements_of_topologic_dim<domain_type, topologic_dimension>::type element_typelist;
        
        viennagrid::meta::typelist::for_each<element_typelist>( for_each_functor );
    }
    
    
    
    
    
    template<typename element_type_or_tag, typename domain_type>
    typename result_of::element_range<domain_type, element_type_or_tag>::type elements(domain_type & domain)
    {
        return elements<element_type_or_tag>( element_collection(domain) );
    }
    
    template<typename element_type_or_tag, typename domain_type>
    typename result_of::const_element_range<domain_type, element_type_or_tag>::type elements(domain_type const & domain)
    {
        return elements<element_type_or_tag>( element_collection(domain) );
    }

    
    
//     template<typename domain_type, typename id_type>
//     bool element_present(const domain_type & domain, id_type id)
//     {
//         typedef typename id_type::value_type element_type;
//         typedef typename element_type::tag element_tag;
//         typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
//         typedef typename viennagrid::result_of::const_handle_iterator<RangeType>::type RangeIterator;
//         
//         RangeType range = viennagrid::elements<element_tag>(domain);
//         for (RangeIterator it = range.handle_begin(); it != range.handle_end(); ++it)
//         {
//             if ( viennagrid::dereference_handle(domain, *it).id() == id )
//                 return true;
//         }
//         
//         return false;
//     }
//     




    template<typename element_type_or_tag, typename domain_type>
    typename viennagrid::result_of::id_type< typename viennagrid::result_of::element<domain_type, element_type_or_tag>::type >::type id_upper_bound( domain_type const & domain )
    {
        typedef typename viennagrid::result_of::element<domain_type, element_type_or_tag>::type element_type;
        return id_generator(domain).max_id( viennagrid::meta::tag<element_type>() );
    }
    
    
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find(domain_type & domain, id_type id )
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        RangeType range = viennagrid::elements<element_tag>(domain);
        return std::find_if(
                    range.begin(),
                    range.end(),
                    viennagrid::storage::id_compare<id_type>(id)
            );
    }
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find(const domain_type & domain, id_type id )
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        RangeType range = viennagrid::elements<element_tag>(domain);
        return std::find_if(
                    range.begin(),
                    range.end(),       
                    viennagrid::storage::id_compare<id_type>(id)
            );
    }

    
    
    template<typename ElementTag, typename WrappedConfigType>
    void set_vertex(
            viennagrid::element_t<ElementTag, WrappedConfigType> & element,
            typename viennagrid::result_of::handle< viennagrid::element_t<ElementTag, WrappedConfigType>, vertex_tag >::type vertex_handle,
            unsigned int pos
        )
    {
        element.container( viennagrid::dimension_tag<0>() ).set_handle( vertex_handle, pos );
    }
    
    template<typename domain_type>
    typename result_of::handle<domain_type, viennagrid::vertex_tag>::type get_vertex_handle( domain_type & domain, unsigned int pos )
    {
        typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        return viennagrid::storage::collection::get<vertex_type>( element_collection<>(domain) ).handle_at(pos);
    }
    

}



#endif
