/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_SEGMENT_ITERATORS_HPP
#define VIENNAGRID_SEGMENT_ITERATORS_HPP

#include <vector>
#include <list>
#include <map>
#include <stack>

#include "viennagrid/forwards.h"
#include "viennagrid/segment.hpp"

namespace viennagrid
{
  
  template <typename IteratorType, typename ElementType>
  class on_segment_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
      typedef on_segment_iterator<IteratorType, ElementType>        self_type;
    
    public:
      on_segment_iterator(IteratorType const & it) : iter_(it) {}

      ElementType & operator*() const { return **iter_; }
      ElementType * operator->() const { return *iter_; }

      self_type & operator++() { ++iter_; return *this; }
      self_type operator++(int) { self_type tmp = *this; ++*this; return tmp; }

      bool operator==(const self_type & i) const { return iter_ == i.iter_; }
      bool operator!=(const self_type & i) const { return iter_ != i.iter_; }

    private:
      IteratorType iter_;
  };
  
  template <typename IteratorType, typename ElementType>
  class const_on_segment_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
      typedef const_on_segment_iterator<IteratorType, ElementType>        self_type;
    public:
      const_on_segment_iterator(IteratorType const & it) : iter_(it) {}

      const ElementType & operator*() const { return **iter_; }
      const ElementType * operator->() const { return *iter_; }

      self_type & operator++() { ++iter_; return *this; }
      self_type operator++(int) { self_type tmp = *this; ++*this; return tmp; }

      bool operator==(const self_type& i) const { return iter_ == i.iter_; }
      bool operator!=(const self_type& i) const { return iter_ != i.iter_; }

    private:
      IteratorType iter_;
  };
  
  
  
  //generic segment iterator
  template <typename Config,
            dim_type dim,
            dim_type cell_level = Config::cell_tag::topology_level>
  struct segment_iterators;
  
  
  //special case: cells:
  template <typename Config, dim_type cell_level>
  struct segment_iterators< Config, cell_level, cell_level>
  {
    typedef segment_t<Config>                     segment_type;
    typedef element<Config,
                    typename Config::cell_tag>    cell_type;
                    
    typedef typename result_of::element_container<segment_type,
                                                  Config::cell_tag::topology_level>::type   container_type;
    
    typedef typename container_type::iterator          iterator;
    typedef typename container_type::const_iterator    const_iterator;
  };
  
  
  //non-const iteration:
  // interface function for container creation,
  // non-const:
  template <dim_type dim, typename DomainConfig>
  ncell_proxy< segment_t<DomainConfig> >
  ncells(segment_t<DomainConfig> & d)
  {
    return ncell_proxy< segment_t<DomainConfig> >(d);
  }


  //container for iteration over a STL vector
  template <typename config_type, dim_type dim>
  class ncell_container < segment_t<config_type>, dim, false >
  {
      typedef segment_t<config_type>                        segment_type;
      typedef element< config_type,
                       typename traits::subcell_desc<typename config_type::cell_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<segment_type, dim>::type   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_segment_iterator< typename container_type::iterator, element_type >   iterator;
      
      ncell_container(ncell_proxy< segment_t<config_type> > const & p) : cont_(p.get().template container<dim>()) {}
      
      ncell_container & operator=(ncell_proxy< segment_t<config_type> > p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const { return iterator(cont_->begin()); }
      iterator end()   const { return iterator(cont_->end()); }

      size_t size() const { return cont_->size(); }

    private:
      container_type * cont_;
  };
  
  
  
  
  
  template <dim_type dim, typename DomainConfig>
  const_ncell_proxy< segment_t<DomainConfig> >
  ncells(segment_t<DomainConfig> const & d)
  {
    return const_ncell_proxy< segment_t<DomainConfig> >(d);
  }
  
  template <typename config_type, dim_type dim>
  class const_ncell_container < segment_t<config_type>, dim, false >
  {
      typedef segment_t<config_type>                        segment_type;
      typedef element< config_type,
                       typename traits::subcell_desc<typename config_type::cell_tag, dim>::element_tag
                     >                                                         element_type;

      typedef element< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<segment_type, dim>::type      container_type;
    
    public: 
      //typedef typename container_type::const_iterator   iterator;
      typedef on_segment_iterator< typename container_type::const_iterator, const element_type >   iterator;
      
      const_ncell_container(const_ncell_proxy< segment_type > const & p) : cont_(p.get().template container<dim>()) {}

      const_ncell_container(ncell_proxy< segment_type > const & p) : cont_(p.get().template container<dim>()) {}

      const_ncell_container & operator=(const_ncell_proxy< segment_type > const & p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const { return iterator(cont_->begin()); }
      iterator end()   const { return iterator(cont_->end()); }
      
      size_t size() const { return cont_->size(); }
      
    private:
      const container_type * cont_;
  };
  
  
} //namespace
#endif

