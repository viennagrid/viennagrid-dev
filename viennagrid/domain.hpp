#ifndef VIENNAGRID_DOMAIN_HPP
#define VIENNAGRID_DOMAIN_HPP

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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


#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include "viennagrid/forwards.h"
//#include "viennagrid/point.hpp"
//#include "viennagrid/element.hpp"
//#include "viennagrid/detail/element_key.hpp"
//#include "viennagrid/segment.hpp"
#include "viennagrid/traits/container.hpp"
#include "viennagrid/detail/domain_iterators.hpp"
#include "viennagrid/detail/domain_layers.hpp"
//#include "viennagrid/config/domain_configs.hpp"

namespace viennagrid
{


/***************************** Domain Type ***********************/

  namespace result_of
  {
    template <typename ConfigType,
              long dim,
              long cell_level /* see forwards.h for default argument */>
    struct element_container< domain_t<ConfigType>, dim, cell_level >
    {
      typedef typename result_of::ncell<ConfigType, dim>::type            element_type;
      
      typedef std::map< element_key<ConfigType, element_type>, element_type >      type;
    };

    //at point level:
    template <typename ConfigType, long cell_level>
    struct element_container< domain_t<ConfigType>, 0, cell_level>
    {
      typedef typename result_of::ncell<ConfigType, 0>::type              element_type;
      
      //typedef std::vector< element_type >      type;
      typedef std::deque< element_type >      type;
    };

    //at cell level:
    template <typename ConfigType, long cell_level>
    struct element_container< domain_t<ConfigType>, cell_level, cell_level>
    {
      typedef typename result_of::ncell<ConfigType, cell_level>::type     element_type;
      
      //typedef std::vector< element_type >      type;
      typedef std::deque< element_type >      type;
    };
    
    
    template <typename ConfigType>
    struct domain
    {
      typedef domain_t<ConfigType>     type; 
    };
    
  }
  
  

  

  template <typename Config>
  class domain_t : public detail::domain_layers<Config,
                                                Config::cell_tag::dim,
                                                true,
                                                full_handling_tag>  //we start with cells
  {
      typedef detail::domain_layers<Config, Config::cell_tag::dim, true>           base_type;
      typedef domain_t<Config>                                                          self_type;
    
    public:
      typedef Config                                    config_type;
      typedef std::size_t                               size_type;
      typedef segment_t<Config>                         segment_type;
      
      typedef detail::domain_segment_container<self_type, segment_type>    segment_container;
      
      //Note: Visual Studio complains about the use of 'this'. However, we just need the address, so this is perfectly safe.
      domain_t() : segments_(this) {}

      template <typename OtherDomainType, typename RefinementTag>
      domain_t(refinement_proxy<OtherDomainType, RefinementTag> const & proxy) : segments_(this)
      {
        detail::refine_impl(proxy.get(), *this, proxy.tag());
      }

      //clean up any data associated with the domain if it is destroyed:
      ~domain_t() { viennadata::erase<viennadata::all, viennadata::all>()(*this); }

      using base_type::push_back;
      
      template <typename OtherDomainType, typename RefinementTag>
      self_type & operator=(refinement_proxy<OtherDomainType, RefinementTag> const & proxy)
      {
        detail::refine_impl(proxy.get(), *this, proxy.tag());
        return *this;
      }
      
      segment_container & segments() { return segments_; }
      segment_container const & segments() const { return segments_; }
    
    private:
      segment_container segments_;
  };

  
  template <typename ConfigType>
  typename domain_t<ConfigType>::segment_container &
  segments(domain_t<ConfigType> & domain)
  {
    return domain.segments(); 
  }
  
  template <typename ConfigType>
  typename domain_t<ConfigType>::segment_container const &
  segments(domain_t<ConfigType> const & domain)
  {
    return domain.segments(); 
  }
  
}
#endif
