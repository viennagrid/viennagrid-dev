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
        while ( currentlevel < levelnum_ && ret_buffer->child_num() != 0 )
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

  template <typename Config, unsigned long level, typename handling_tag = typename segment_traits<level>::handling_tag>
  struct SegmentIteratorChecker
  {
    enum{ ReturnValue = segment_traits<level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
  };

  template <typename Config, unsigned long level>
  struct SegmentIteratorChecker<Config, level, full_handling_tag>
  {
    enum{ ReturnValue = Config::cell_tag::topology_level - level };
  };

  //for level = 0, there is also special treatment (vertices have vector-container)
  template <typename Config>
  struct SegmentIteratorChecker<Config, 0, full_handling_tag >
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

  template <typename Config, unsigned long level>
  struct IteratorChecker< domain<Config>, level>
  {
    enum{ ReturnValue = 1 };
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

  //Iterator types for segment
  template <typename T_Configuration, unsigned long level>
  struct IteratorTypes< segment<T_Configuration>, level, false>
  {
    typedef element<T_Configuration,
                     typename subcell_traits<typename T_Configuration::cell_tag, level>::element_tag
                    >                                                         LevelElementType;

    typedef osegit <LevelElementType,
                     typename std::map< element_key<LevelElementType>, LevelElementType >::iterator
                    >                                             result_type;
  };

  /*
  //segment at top level may have a multigrid-iterator:
  template <typename T_Configuration, typename handling_tag, unsigned long level>
  struct IteratorTypes< segment<T_Configuration, level, handling_tag, true, full_multigrid_tag>, level, true>
  {
    typedef element<T_Configuration,
                     typename T_Configuration::cell_tag
                    >                                                         LevelElementType;

    typedef mgCellIt<LevelElementType,
                       typename std::vector<LevelElementType>::iterator
                     >                                                        result_type;
  };

  template <typename T_Configuration, typename handling_tag, unsigned long level>
  struct IteratorTypes< segment<T_Configuration, level, handling_tag, true, no_multigrid_tag>, level, true>
  {
    typedef element<T_Configuration,
                     typename T_Configuration::cell_tag
                    >                                                         LevelElementType;

    typedef typename std::vector<LevelElementType >::iterator              result_type;
  };

  //segment at vertex level has a vector:
  template <typename T_Configuration>
  struct IteratorTypes< segment<T_Configuration>, 0, true>
  {
    typedef element<T_Configuration,
                     typename subcell_traits<typename T_Configuration::cell_tag, 0>::element_tag
                    >                                                         LevelElementType;

    typedef typename std::vector<LevelElementType >::iterator              result_type;
  };
  */
  
  
  template <typename ConfigType, bool dummy>
  struct IteratorTypes< domain<ConfigType>, 0, dummy>
  {
    typedef element< ConfigType,
                     typename subcell_traits<typename ConfigType::cell_tag, 0>::element_tag
                    >                                                         element_type;
                    
    typedef typename std::vector<element_type>::iterator             iterator;
    typedef typename std::vector<element_type>::const_iterator       const_iterator;
  };
  
  
  
  
  //////////////////// brand new containers: /////////////////////
  
  
  //proxy class:
  template <typename T>
  class const_ncell_proxy
  {
    public:
      const_ncell_proxy(T const & t_) : t(t_) {}
      
      T const & get() const { return t; }
    
    private:
      T const & t;
  };
  
  template <typename T>
  class ncell_proxy
  {
    public:
      ncell_proxy(T & t_) : t(t_) {}
      
      T & get() const { return t; }
    
    private:
      T & t;
  };
  
  
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
  template <typename T, dim_type dim>
  class vector_container
  {
      typedef element< typename T::config_type,
                       typename subcell_traits<typename T::config_type::cell_tag, dim>::element_tag
                     >                                                         element_type;
                    
      typedef std::vector< element_type >     container_type;
    
    public: 
      typedef typename container_type::iterator   iterator;
      
      vector_container(ncell_proxy<T> const & p) : cont_(p.get().template container<dim>()) {}
      
      vector_container & operator=(ncell_proxy<T> p)
      { 
        cont_ = p.get().vertex_container();
        return *this;
      }
      
      iterator begin() const { return cont_->begin(); }
      iterator end()   const { return cont_->end(); }
      
    private:
      container_type * cont_;
  };
  
  
  
  
  
  template <typename T, dim_type dim>
  class const_vector_container
  {
      typedef element< typename T::config_type,
                       typename subcell_traits<typename T::config_type::cell_tag, dim>::element_tag
                     >                                                         element_type;
                    
      typedef std::vector< element_type >     container_type;
    
    public: 
      typedef typename container_type::const_iterator   iterator;
      
      const_vector_container(const_ncell_proxy<T> const & p) : cont_(p.get().template container<dim>()) {}
      
      const_vector_container & operator=(const_ncell_proxy<T> const & p)
      { 
        cont_ = p.get().vertex_container();
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
    typedef typename const_vector_container<DomainType, dim>::iterator   iterator;
    
    typedef const_vector_container<DomainType, dim>       result_type;
  };

  //metafunction for return type:
  namespace result_of
  {
    
    template <typename T, dim_type dim>
    struct iterator<vector_container<T, dim> >
    {
      typedef typename vector_container<T, dim>::iterator     type;
    };
    
    template <typename T, dim_type dim>
    struct iterator<const_vector_container<T, dim> >
    {
      typedef typename const_vector_container<T, dim>::iterator     type;
    };
    
    template <typename T, 
              dim_type dim>  //topological level
    struct ncell_container
    {
      typedef vector_container<T, dim>       type;
    };

    template <typename T, 
              dim_type dim>  //topological level
    struct const_ncell_container
    {
      typedef const_vector_container<T, dim>       type;
    };
    
  }
  
} //namespace
#endif

