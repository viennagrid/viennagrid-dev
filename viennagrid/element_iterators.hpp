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
  class ocit //: public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      ocit(ElementType **pp) : pp_(pp) {}

      ElementType & operator*() const { return **pp_; }
      ElementType * operator->() const { return *pp_; }

      ocit & operator++() { ++pp_; return *this; }
      ocit & operator++(int) { ocit tmp = *this; ++*this; return tmp; }

      bool operator==(const ocit& i) const { return pp_ == i.pp_; }
      bool operator!=(const ocit& i) const { return pp_ != i.pp_; }

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
  template <typename ElementTag, unsigned long level, typename handling_tag = typename subcell_traits<ElementTag, level>::handling_tag>
  struct ElementIteratorChecker
  {
    enum{ ReturnValue = subcell_traits<ElementTag, level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
  };

  template <typename ElementTag, unsigned long level>
  struct ElementIteratorChecker< ElementTag, level, full_handling_tag>
  {
    enum{ ReturnValue = ElementTag::topology_level - level };
  };


  template <typename ElementType, unsigned long level>
  struct IteratorChecker
  {
    enum{ ReturnValue = ElementType::ERROR_ELEMENT_TYPE_INVALID };
  };

  template <typename Config, typename ElementTag, unsigned long level>
  struct IteratorChecker< element<Config, ElementTag>, level>
  {
    enum{ ReturnValue = ElementIteratorChecker<ElementTag, level>::ReturnValue };
  };

  
  
  
  
  //Type factory for all domain iterators
  template <typename ContainerElement,
              unsigned long level,
              bool special = (IteratorChecker<ContainerElement, level>::ReturnValue <= 0 )>
  struct IteratorTypes
  {
    typedef typename ContainerElement::ERROR_ITERATOR_LEVEL_NOT_VALID   result_type;
  };

  //Iterator types for elements
  template <typename T_Configuration, typename ElementTag, unsigned long level>
  struct IteratorTypes< element<T_Configuration, ElementTag>, level, false>
  {
    typedef ocit< element<T_Configuration, ElementTag>,
                    element<T_Configuration,typename subcell_traits<ElementTag, level>::element_tag>
                  >                                                           result_type;
  };

  //cell-on-cell-iterator is just a plain pointer:
  template <typename T_Configuration, typename ElementTag, unsigned long level>
  struct IteratorTypes< element<T_Configuration, ElementTag>, level, true>
  {
    typedef element<T_Configuration, ElementTag>*                         result_type;
  };


  
} //namespace
#endif

