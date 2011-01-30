/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_DOMAIN_ITERATORS_HPP
#define VIENNAGRID_DOMAIN_ITERATORS_HPP

#include <vector>
#include <list>
#include <map>
#include <stack>

#include "viennagrid/forwards.h"

namespace viennagrid
{


  // iterator over all subcells (not vertex or cell type) on the segment
  // transforms the map-like storage on the domain
  template <typename ElementType, typename MapIterator>
  class domain_subcell_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      domain_subcell_iterator(MapIterator const & mit) : mit_(mit) {}

      ElementType & operator*() const { return mit_->second; }
      ElementType* operator->() const { return &(mit_->second); }

      domain_subcell_iterator & operator++() { ++mit_; return *this; }
      domain_subcell_iterator & operator++(int) { domain_subcell_iterator tmp = *this; ++*this; return tmp; }

      bool operator==(const domain_subcell_iterator & i) const { return mit_ == i.mit_; }
      bool operator!=(const domain_subcell_iterator & i) const { return mit_ != i.mit_; }

    private:
      MapIterator mit_;
  };

  

  ///////////////  Iterator construction for iterations over whole domain ////////////////
  //generic case:
  template <typename Config,
            dim_type dim,
            dim_type cell_level = Config::cell_tag::topology_level>
  struct domain_iterators
  {
    typedef domain<Config>                        domain_type;
    typedef element<Config,
                    typename Config::cell_tag>    cell_type;
      typedef element< Config,
                       typename subcell_traits<typename Config::cell_tag, dim>::element_tag
                     >                                                         element_type;
                    
    typedef typename result_of::element_container<domain_type, dim>::type   container_type;
    
    typedef domain_subcell_iterator<element_type,
                                    typename container_type::iterator>   iterator;
    typedef domain_subcell_iterator<const element_type,
                                    typename container_type::const_iterator>   const_iterator;
  };

  //special case: vertices:
  template <typename Config, dim_type cell_level>
  struct domain_iterators< Config, 0, cell_level>
  {
    typedef domain<Config>                        domain_type;
    typedef element<Config,
                    typename Config::cell_tag>    cell_type;
                    
    typedef typename result_of::element_container<domain_type, 0>::type   container_type;
    
    typedef typename container_type::iterator          iterator;
    typedef typename container_type::const_iterator    const_iterator;
  };

  //special case: cells:
  template <typename Config, dim_type cell_level>
  struct domain_iterators< Config, cell_level, cell_level>
  {
    typedef domain<Config>                        domain_type;
    typedef element<Config,
                    typename Config::cell_tag>    cell_type;
                    
    typedef typename result_of::element_container<domain_type,
                                                  Config::cell_tag::topology_level>::type   container_type;
    
    typedef typename container_type::iterator          iterator;
    typedef typename container_type::const_iterator    const_iterator;
  };

  
  
  //////////////////// brand new containers: /////////////////////
  
  
  
  
  //interface function for container creation:
  template <dim_type dim, typename DomainConfig>
  const_ncell_proxy< domain<DomainConfig> >
  ncells(domain<DomainConfig> const & d)
  {
    return const_ncell_proxy< domain<DomainConfig> >(d);
  }
  
  template <dim_type dim, typename DomainConfig>
  ncell_proxy< domain<DomainConfig> >
  ncells(domain<DomainConfig> & d)
  {
    return ncell_proxy< domain<DomainConfig> >(d);
  }


  //container for iteration over a STL vector
  template <typename config_type, dim_type dim>
  class ncell_container < domain<config_type>, dim >
  {
      typedef domain<config_type>                        domain_type;
      typedef element< config_type,
                       typename subcell_traits<typename config_type::cell_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<domain_type, dim>::type   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef typename domain_iterators<config_type, dim>::iterator   iterator;
      
      ncell_container(ncell_proxy< domain<config_type> > const & p) : cont_(p.get().template container<dim>()) {}
      
      ncell_container & operator=(ncell_proxy< domain<config_type> > p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const { return cont_->begin(); }
      iterator end()   const { return cont_->end(); }
      
    private:
      container_type * cont_;
  };
  
  
  
  
  
  template <typename config_type, dim_type dim>
  class const_ncell_container < domain<config_type>, dim >
  {
      typedef domain<config_type>                        domain_type;
      typedef element< config_type,
                       typename subcell_traits<typename config_type::cell_tag, dim>::element_tag
                     >                                                         element_type;

      typedef element< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<domain_type, dim>::type      container_type;
    
    public: 
      //typedef typename container_type::const_iterator   iterator;
      typedef typename domain_iterators<config_type, dim>::const_iterator   iterator;
      
      const_ncell_container(const_ncell_proxy< domain<config_type> > const & p) : cont_(p.get().template container<dim>()) {}
      
      const_ncell_container & operator=(const_ncell_proxy< domain<config_type> > const & p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const { return cont_->begin(); }
      iterator end()   const { return cont_->end(); }
      
    private:
      const container_type * cont_;
  };
  
  
  template <typename DomainType, 
            dim_type dim>  //topological level
  struct const_container_types
  {
    typedef typename const_ncell_container<DomainType, dim>::iterator   iterator;
    
    typedef const_ncell_container<DomainType, dim>       result_type;
  };

  //metafunction for return type:
  namespace result_of
  {
    
    template <typename T, dim_type dim>
    struct iterator<viennagrid::ncell_container<T, dim>, 0>
    {
      typedef typename viennagrid::ncell_container<T, dim>::iterator     type;
    };
    
    template <typename T, dim_type dim>
    struct iterator<viennagrid::const_ncell_container<T, dim>, 0>
    {
      typedef typename viennagrid::const_ncell_container<T, dim>::iterator     type;
    };
    
    template <typename T, 
              dim_type dim>  //topological level
    struct ncell_container
    {
      typedef viennagrid::ncell_container<T, dim>       type;
    };

    template <typename T, 
              dim_type dim>  //topological level
    struct const_ncell_container
    {
      typedef viennagrid::const_ncell_container<T, dim>       type;
    };
    
  }
  
} //namespace
#endif

