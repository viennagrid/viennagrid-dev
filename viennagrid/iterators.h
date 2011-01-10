/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_ITERATORS_HPP
#define VIENNAGRID_ITERATORS_HPP

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

  // iterator on segments:
  template <typename ElementType, typename MapIterator>
  class osegit : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      osegit(MapIterator const & mit) : mit_(mit) {}

      ElementType & operator*() const { return mit_->second; }
      ElementType* operator->() const { return &(mit_->second); }

      osegit & operator++() { ++mit_; return *this; }
      osegit & operator++(int) { osegit tmp = *this; ++*this; return tmp; }

      bool operator==(const osegit & i) const { return mit_ == i.mit_; }
      bool operator!=(const osegit & i) const { return mit_ != i.mit_; }

    private:
      MapIterator mit_;
  };

  // multigrid child-iterator
  template <typename ElementType>
  class childit : public std::iterator < std::forward_iterator_tag, ElementType >
  {
      typedef typename std::list<ElementType *>::iterator     ListIterator;

    public:
      childit(ListIterator const & it) : list_iterator(it) {}

      ElementType & operator*() const { return **list_iterator; }
      ElementType * operator->() const { return *list_iterator; }

      childit & operator++() { ++list_iterator; return *this; }
      childit & operator++(int) { childit tmp = *this; ++*this; return tmp; }

      bool operator==(const childit& i) const { return list_iterator == i.list_iterator; }
      bool operator!=(const childit& i) const { return list_iterator != i.list_iterator; }

    private:
      ListIterator list_iterator;
  };

  // multigrid cell-iterator for segments: take refinements into account:
  template <typename ElementType, typename VectorIterator>
  class mgCellIt : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    typedef typename ElementType::ChildIterator                ChildIterator;

    public:
      mgCellIt(VectorIterator const & vit, long levelnum) : vit_(vit), levelnum_(levelnum)
      { }

      ElementType & operator*()
      {
        // get one parent element:
        if (parent_iterators.size() == 0)
          ret_buffer = &(*vit_);
        else
          ret_buffer = &(*(parent_iterators.top()));

        //step down to levelnum:
        long currentlevel = parent_iterators.size();
        while ( currentlevel < levelnum_ && ret_buffer->getChildNum() != 0 )
        {
          parent_iterators.push(ret_buffer->getChildrenBegin());
          parent_stoppers.push(ret_buffer->getChildrenEnd());
          ret_buffer = &(*(ret_buffer->getChildrenBegin()));
          ++currentlevel;
        }

        return *ret_buffer;
      }

      ElementType* operator->() { return &(*(*this)); }

      mgCellIt & operator++()
      {
        //find next parent with unvisited children:
        while ( parent_iterators.size() != 0 &&
                ++(parent_iterators.top()) == parent_stoppers.top() )
        {
          parent_iterators.pop(); parent_stoppers.pop();
        }

        //check whether we have to proceed at top level:
        if (parent_iterators.size() == 0)
          ++vit_;

        return *this;
      }
      mgCellIt & operator++(int) { mgCellIt tmp = *this; ++*this; return tmp; }

      bool operator==(const mgCellIt & i) const { return vit_ == i.vit_; }
      bool operator!=(const mgCellIt & i) const { return vit_ != i.vit_; }

    private:
      VectorIterator vit_;
      long levelnum_;
      ElementType * ret_buffer;
      std::stack< ChildIterator > parent_iterators;
      std::stack< ChildIterator > parent_stoppers;
  };


  //segment iterator with multilevel support:
  template <typename ElementType, typename STLIterator>
  class SegIt : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      SegIt(STLIterator const & stlit, long level) : stlit_(stlit), level_(level) {}

      ElementType & operator*() const { return stlit_->getRefinedSegment(level_); }
      ElementType* operator->() const { return &(stlit_->getRefinedSegment(level_)); }

      SegIt & operator++() { ++stlit_; return *this; }
      SegIt & operator++(int) { SegIt tmp = *this; ++*this; return tmp; }

      bool operator==(const SegIt & i) const { return stlit_ == i.stlit_; }
      bool operator!=(const SegIt & i) const { return stlit_ != i.stlit_; }

    private:
      STLIterator stlit_;
      long level_;
  };

  template <typename ElementType, typename STLIterator>
  class ConstSegIt //: public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      ConstSegIt(STLIterator const & stlit, long level) : stlit_(stlit), level_(level) {}

      ElementType const & operator*() const { return stlit_->getRefinedSegment(level_); }
      ElementType const * operator->() const { return &(stlit_->getRefinedSegment(level_)); }

      ConstSegIt & operator++() { ++stlit_; return *this; }
      ConstSegIt & operator++(int) { ConstSegIt tmp = *this; ++*this; return tmp; }

      bool operator==(const ConstSegIt & i) const { return stlit_ == i.stlit_; }
      bool operator!=(const ConstSegIt & i) const { return stlit_ != i.stlit_; }

    private:
      STLIterator stlit_;
      long level_;
  };
  
  /////////////// Iterator Types retrieval /////////


  //Check availability of iterators:
  template <typename ElementTag, unsigned long level, typename HandlingTag = typename TopologyLevel<ElementTag, level>::HandlingTag>
  struct ElementIteratorChecker
  {
    enum{ ReturnValue = TopologyLevel<ElementTag, level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
  };

  template <typename ElementTag, unsigned long level>
  struct ElementIteratorChecker< ElementTag, level, TopoLevelFullHandling>
  {
    enum{ ReturnValue = ElementTag::TopoLevel - level };
  };

  template <typename Config, unsigned long level, typename HandlingTag = typename SegmentConfig<level>::HandlingTag>
  struct SegmentIteratorChecker
  {
    enum{ ReturnValue = SegmentConfig<level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
  };

  template <typename Config, unsigned long level>
  struct SegmentIteratorChecker<Config, level, TopoLevelFullHandling>
  {
    enum{ ReturnValue = Config::CellTag::TopoLevel - level };
  };

  //for level = 0, there is also special treatment (vertices have vector-container)
  template <typename Config>
  struct SegmentIteratorChecker<Config, 0, TopoLevelFullHandling >
  {
    enum{ ReturnValue = 0 };
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

  template <typename Config, unsigned long level>
  struct IteratorChecker< segment<Config>, level>
  {
    enum{ ReturnValue = SegmentIteratorChecker<Config, level>::ReturnValue };
  };

  //Type factory for all domain iterators
  template <typename ContainerElement,
              unsigned long level,
              bool special = (IteratorChecker<ContainerElement, level>::ReturnValue <= 0 )>
  struct IteratorTypes
  {
    typedef typename ContainerElement::ERROR_ITERATOR_LEVEL_NOT_VALID   ResultType;
  };

  //Iterator types for elements
  template <typename T_Configuration, typename ElementTag, unsigned long level>
  struct IteratorTypes< element<T_Configuration, ElementTag>, level, false>
  {
    typedef ocit< element<T_Configuration, ElementTag>,
                    element<T_Configuration,typename TopologyLevel<ElementTag, level>::ElementTag>
                  >                                                           ResultType;
  };

  //cell-on-cell-iterator is just a plain pointer:
  template <typename T_Configuration, typename ElementTag, unsigned long level>
  struct IteratorTypes< element<T_Configuration, ElementTag>, level, true>
  {
    typedef element<T_Configuration, ElementTag>*                         ResultType;
  };

  //Iterator types for segment
  template <typename T_Configuration, unsigned long level>
  struct IteratorTypes< segment<T_Configuration>, level, false>
  {
    typedef element<T_Configuration,
                     typename TopologyLevel<typename T_Configuration::CellTag, level>::ElementTag
                    >                                                         LevelElementType;

    typedef osegit <LevelElementType,
                     typename std::map< ElementKey<LevelElementType>, LevelElementType >::iterator
                    >                                             ResultType;
  };

  //segment at top level may have a multigrid-iterator:
  template <typename T_Configuration, typename HandlingTag, unsigned long level>
  struct IteratorTypes< segment<T_Configuration, level, HandlingTag, true, FullMultigridTag>, level, true>
  {
    typedef element<T_Configuration,
                     typename T_Configuration::CellTag
                    >                                                         LevelElementType;

    typedef mgCellIt<LevelElementType,
                       typename std::vector<LevelElementType>::iterator
                     >                                                        ResultType;
  };

  template <typename T_Configuration, typename HandlingTag, unsigned long level>
  struct IteratorTypes< segment<T_Configuration, level, HandlingTag, true, NoMultigridTag>, level, true>
  {
    typedef element<T_Configuration,
                     typename T_Configuration::CellTag
                    >                                                         LevelElementType;

    typedef typename std::vector<LevelElementType >::iterator              ResultType;
  };

  //segment at vertex level has a vector:
  template <typename T_Configuration>
  struct IteratorTypes< segment<T_Configuration>, 0, true>
  {
    typedef element<T_Configuration,
                     typename TopologyLevel<typename T_Configuration::CellTag, 0>::ElementTag
                    >                                                         LevelElementType;

    typedef typename std::vector<LevelElementType >::iterator              ResultType;
  };

} //namespace
#endif

