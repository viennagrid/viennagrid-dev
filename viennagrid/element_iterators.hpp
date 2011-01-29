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

  
  template <typename config_type, typename element_tag, dim_type dim>
  class ncell_container < element<config_type, element_tag>, dim>
  {
      typedef element< config_type,
                       typename subcell_traits<element_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element<config_type, element_tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef element_type **   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator< host_type, //the host element type
                                   element_type //the subcell type
                                 >                                          iterator;
      
      //
      // TODO
      //     
                                 
                                 
      /*ncell_container(ncell_proxy<T> const & p) : cont_(p.get().template container<dim>()) {}
      
      ncell_container & operator=(ncell_proxy<T> p)
      { 
        cont_ = p.get().vertex_container();
        return *this;
      }
      
      iterator begin() const { return cont_->begin(); }
      iterator end()   const { return cont_->end(); }*/
      
    private:
      container_type cont_;
  };
  
  
  namespace result_of
  {
    //Type factory for all domain iterators
    /*template <typename ContainerElement,
              unsigned long level,
              bool special = (IteratorChecker<ContainerElement, level>::ReturnValue <= 0 )>
    struct IteratorTypes
    {
      typedef typename ContainerElement::ERROR_ITERATOR_LEVEL_NOT_VALID   result_type;
    };*/

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

    /*
    //cell-on-cell-iterator is just a plain pointer:
    template <typename T_Configuration, typename ElementTag, unsigned long level>
    struct IteratorTypes< element<T_Configuration, ElementTag>, level, true>
    {
      typedef element<T_Configuration, ElementTag>*                         result_type;
    };
    */

  }
  
} //namespace
#endif

