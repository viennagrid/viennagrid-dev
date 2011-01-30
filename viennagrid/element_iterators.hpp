/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_ELEMENT_ITERATORS_HPP
#define VIENNAGRID_ELEMENT_ITERATORS_HPP

#include <vector>
#include <list>
#include <map>
#include <stack>

#include "viennagrid/forwards.h"
#include "viennagrid/domain_iterators.hpp"

namespace viennagrid
{


  //ContainerElement-Type prevents abuse, for example:
  //A vertex-on-facet-iterator is not equal to a vertex-on-cell-iterator!
  template <typename ContainerElement, typename ElementType>
  class on_element_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      on_element_iterator(ElementType **pp) : pp_(pp) {}

      ElementType & operator*() const { return **pp_; }
      ElementType * operator->() const { return *pp_; }

      on_element_iterator & operator++() { ++pp_; return *this; }
      on_element_iterator & operator++(int) { on_element_iterator tmp = *this; ++*this; return tmp; }

      bool operator==(const on_element_iterator& i) const { return pp_ == i.pp_; }
      bool operator!=(const on_element_iterator& i) const { return pp_ != i.pp_; }

      //support for element-orientation-retrieval:
//       long operator-(const ocit & o2) const { return pp_ - o2.pp_; }

    private:
      ElementType **pp_;
  };

  //with copy:
  template <typename Type, long edgenum>
  class ocit_with_copy
  {
    public:
      ocit_with_copy(Type *pp, long startindex) : current(startindex)
      {
        //std::cout << "ocit_with_copy constructor" << std::endl;
        for (long i=0; i<edgenum; ++i)
          pp_[i] = pp[i];
      }

      Type & operator*() const { return *(pp_[current].first); }
      Type* operator->() const { return pp_[current].first; }

      ocit_with_copy & operator++() { ++current; return *this; }
      ocit_with_copy & operator++(int) { ocit_with_copy tmp = *this; ++*this; return tmp; }

      bool operator==(const ocit_with_copy& i) const { return current == i.current; }
      bool operator!=(const ocit_with_copy& i) const { return current != i.current; }

    private:
      Type pp_[edgenum];
      long current;
  };


  
  /////////////// Iterator Types retrieval /////////


  //Check availability of iterators:
  template <typename ElementTag,
            dim_type level,
            typename handling_tag = typename subcell_traits<ElementTag, level>::handling_tag>
  struct ElementIteratorChecker
  {
    enum{ ReturnValue = subcell_traits<ElementTag, level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
  };

  template <typename ElementTag,
            dim_type level>
  struct ElementIteratorChecker< ElementTag, level, full_handling_tag>
  {
    enum{ ReturnValue = ElementTag::topology_level - level };
  };


  template <typename ElementType,
            dim_type level>
  struct IteratorChecker
  {
    enum{ ReturnValue = ElementType::ERROR_ELEMENT_TYPE_INVALID };
  };

  template <typename Config,
            typename ElementTag,
            dim_type level>
  struct IteratorChecker< element<Config, ElementTag>, level>
  {
    enum{ ReturnValue = ElementIteratorChecker<ElementTag, level>::ReturnValue };
  };


  
   //interface function for container creation:
  template <dim_type dim, typename Config, typename ElementTag>
  const_ncell_proxy< element<Config, ElementTag> >
  ncells(element<Config, ElementTag> const & d)
  {
    return const_ncell_proxy< element<Config, ElementTag> >(d);
  }
  
  template <dim_type dim, typename Config, typename ElementTag>
  ncell_proxy< element<Config, ElementTag> >
  ncells(element<Config, ElementTag> & d)
  {
    return ncell_proxy< element<Config, ElementTag> >(d);
  }

  template <typename config_type, typename element_tag, dim_type dim>
  class ncell_container < element<config_type, element_tag>, dim>
  {
      typedef element< config_type,
                       typename subcell_traits<element_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element<config_type, element_tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::topology_level>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator< host_type, //the host element type
                                   element_type //the subcell type
                                 >                                          iterator;
      
      ncell_container(ncell_proxy< element<config_type, element_tag> > const & p) : cont_(p.get().template container<dim>()) {}
      
      ncell_container & operator=(ncell_proxy< element<config_type, element_tag> > p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const { return iterator(cont_); }
      iterator end()   const { return iterator(cont_ + subcell_traits<element_tag, dim>::num_elements); }
      
    private:
      container_type cont_;
  };
  
  
  
  
  
  
  namespace result_of
  {
    template <typename Config, typename ElementTag,
              dim_type dim,
              dim_type cell_level /* see forwards.h for default argument */>
    struct element_container< element<Config, ElementTag>, dim, cell_level >
    {
      typedef typename result_of::ncell_type<Config, dim>::type            element_type;
      
      typedef element_type **      type;
    };
    
    //Iterator types for elements
    template <typename config_type, typename element_tag, dim_type dim>
    struct iterator< element<config_type, element_tag>,
                     dim>
    {
      /*typedef on_element_iterator< element<Config, ElementTag>, //the host element type
                                   element<Config, //the subcell type
                                           typename subcell_traits<ElementTag, level>::element_tag> 
                                 > type; */
                                 
      typedef typename viennagrid::ncell_container < element<config_type, element_tag>, dim>::iterator    type;                           
    };


  }
  
} //namespace
#endif

