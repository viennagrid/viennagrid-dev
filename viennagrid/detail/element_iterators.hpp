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
#include <assert.h>

#include "viennagrid/forwards.h"
#include "viennagrid/detail/domain_iterators.hpp"
#include "viennagrid/detail/segment_iterators.hpp"
//#include "viennagrid/topology/celltags.hpp"

#include "viennadata/api.hpp"

namespace viennagrid
{


  //ContainerElement-Type prevents abuse, for example:
  //A vertex-on-facet-iterator is not equal to a vertex-on-cell-iterator!
  template <typename ElementType>
  class on_element_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      on_element_iterator(ElementType **pp) : pp_(pp) {}

      ElementType & operator*() const { return **pp_; }
      ElementType * operator->() const { return *pp_; }

      on_element_iterator & operator++() { ++pp_; return *this; }
      on_element_iterator operator++(int) { on_element_iterator tmp = *this; ++*this; return tmp; }

      bool operator==(const on_element_iterator& i) const { return pp_ == i.pp_; }
      bool operator!=(const on_element_iterator& i) const { return pp_ != i.pp_; }

      //support for element-orientation-retrieval:
//       long operator-(const ocit & o2) const { return pp_ - o2.pp_; }

    private:
      ElementType **pp_;
  };

  //const-version of above:
  template <typename ElementType>
  class const_on_element_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
      typedef ElementType *   ElementPtr;
    
    public:
      const_on_element_iterator(ElementPtr const * pp) : pp_(pp) {}

      const ElementType & operator*() const { return **pp_; }
      const ElementType * operator->() const { return *pp_; }

      const_on_element_iterator & operator++() { ++pp_; return *this; }
      const_on_element_iterator   operator++(int) { const_on_element_iterator tmp = *this; ++*this; return tmp; }

      bool operator==(const const_on_element_iterator& i) const { return pp_ == i.pp_; }
      bool operator!=(const const_on_element_iterator& i) const { return pp_ != i.pp_; }

      //support for element-orientation-retrieval:
//       long operator-(const ocit & o2) const { return pp_ - o2.pp_; }

    private:
      ElementPtr const * pp_;
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
            typename handling_tag = typename topology::subcell_desc<ElementTag, level>::handling_tag>
  struct ElementIteratorChecker
  {
    enum{ ReturnValue = topology::subcell_desc<ElementTag, level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
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
  
  // non-const:
  template <dim_type dim, typename Config, typename ElementTag>
  ncell_proxy< element<Config, ElementTag> >
  ncells(element<Config, ElementTag> & d)
  {
    return ncell_proxy< element<Config, ElementTag> >(d);
  }

  template <typename config_type, typename element_tag, dim_type dim>
  class ncell_range < element<config_type, element_tag>, dim, false>
  {
      typedef element< config_type,
                       typename topology::subcell_desc<element_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element<config_type, element_tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::topology_level>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator< element_type >                              iterator;
      
      ncell_range() : cont_(NULL) {};
      
      ncell_range(ncell_proxy< element<config_type, element_tag> > const & p) : cont_(p.get().template container<dim>()) {}
      
      ncell_range & operator=(ncell_proxy< element<config_type, element_tag> > p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const
      { 
        assert(cont_ != NULL);
        return iterator(cont_);
      }
      
      iterator end()   const
      {
        assert(cont_ != NULL);
        return iterator(cont_ + topology::subcell_desc<element_tag, dim>::num_elements);
      }
      
    private:
      container_type * cont_;
  };
  
  
  
  
  //const container:
  template <dim_type dim, typename Config, typename ElementTag>
  const_ncell_proxy< element<Config, ElementTag> >
  ncells(element<Config, ElementTag> const & d)
  {
    return const_ncell_proxy< element<Config, ElementTag> >(d);
  }
  
  template <typename config_type, typename element_tag, dim_type dim>
  class const_ncell_range < element<config_type, element_tag>, dim, false>
  {
      typedef element< config_type,
                       typename topology::subcell_desc<element_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element<config_type, element_tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::topology_level>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef const_on_element_iterator< element_type >                         iterator;
      const_ncell_range() : cont_(NULL) {};
      
      const_ncell_range(const_ncell_proxy< element<config_type, element_tag> > const & p) : cont_(p.get().template container<dim>()) {}

      const_ncell_range(ncell_proxy< element<config_type, element_tag> > const & p) : cont_(p.get().template container<dim>()) {}

      const_ncell_range & operator=(const_ncell_proxy< element<config_type, element_tag> > p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }

      const_ncell_range & operator=(ncell_proxy< element<config_type, element_tag> > p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }

      iterator begin() const
      {
        assert(cont_ != NULL);
        return iterator(cont_);
      }
      
      iterator end() const
      { 
        assert(cont_ != NULL);
        return iterator(cont_ + topology::subcell_desc<element_tag, dim>::num_elements);
      }
      
    private:
      const container_type * cont_;
  };
  
  
  
  
  
  
  ////////////////   co-boundaries: /////////////////////////
  
  //helper meta function for selecting const/non-const containers:
  template <dim_type dim_start,
            dim_type dim_iter,
            typename ContainerType,
            typename KeyType,
            typename EnclosingType>
  void init_coboundary(KeyType const & key,
                       EnclosingType & domain)
  {
    typedef typename EnclosingType::config_type         Config;
    typedef typename result_of::ncell_type<Config, dim_start>::type    LowerElementType;
    typedef typename result_of::ncell_type<Config, dim_iter>::type     HigherElementType;
    
    typedef typename result_of::ncell_range<EnclosingType, dim_iter>::type      HigherElementContainer;
    typedef typename result_of::iterator<HigherElementContainer>::type                    HigherElementIterator;
    
    typedef typename result_of::ncell_range<HigherElementType, dim_start>::type  LowerOnHigherContainer;
    typedef typename result_of::iterator<LowerOnHigherContainer>::type                     LowerOnHigherIterator;
    
    
    HigherElementContainer higher_container = ncells<dim_iter>(domain);
    for (HigherElementIterator hit = higher_container.begin();
         hit != higher_container.end();
         ++hit)
    {
      LowerOnHigherContainer lower_container = ncells<dim_start>(*hit);
      for (LowerOnHigherIterator low = lower_container.begin();
           low != lower_container.end();
           ++low)
      {
        viennadata::access<KeyType, ContainerType>(key)(*low).push_back(&(*hit));
      }
    }
  }
  
  // non-const:
  template <typename T, typename U>
  class cobnd_proxy
  {
    public:
      cobnd_proxy(T & t_, U & u_) : t(t_), u(u_) {}
      
      T & first() const { return t; }
      U & second() const { return u; }
    
    private:
      T & t;
      U & u;
  };
  
  template <dim_type dim, typename Config, typename ElementTag>
  cobnd_proxy< element<Config, ElementTag>,
               domain<Config> >
  ncells(element<Config, ElementTag> & e, domain<Config> & d)
  {
    return cobnd_proxy< element<Config, ElementTag>,
                        domain<Config> >(e, d);
  }

  template <dim_type dim, typename Config, typename ElementTag>
  cobnd_proxy< element<Config, ElementTag>,
               segment_t<Config> >
  ncells(element<Config, ElementTag> & e, segment_t<Config> & d)
  {
    return cobnd_proxy< element<Config, ElementTag>,
                        segment_t<Config> >(e, d);
  }

  template <typename config_type, typename element_tag,
            dim_type dim>
  class ncell_range < element<config_type, element_tag>, dim, true>
  {
      typedef element< config_type,
                       typename topology::subcell_desc<typename config_type::cell_tag,
                                               dim>::element_tag
                     >                                                         element_type;
                     
      typedef element<config_type, element_tag>                                host_type;
      typedef std::vector<element_type *>                                      viennadata_container_type;
      typedef element_type *                                                   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator<element_type>                                iterator;
      
      ncell_range() : cont_(NULL) {};
      
      template <typename EnclosingType> //either domain or segment
      ncell_range(cobnd_proxy< element<config_type, element_tag>,
                                   EnclosingType> const & p)
      {
        init(p);
      }
      
      template <typename EnclosingType> //either domain or segment
      ncell_range & operator=(cobnd_proxy< element<config_type, element_tag>,
                                               EnclosingType> const & p)
      { 
        init(p);
        return *this;
      }
      
      iterator begin() const
      {
        assert(cont_ != NULL);
        return iterator(cont_);
      }
      
      iterator end()   const
      {
        assert(cont_ != NULL);
        return iterator(cont_ + num_elements);
      }
      
    private:
      template <typename EnclosingType>
      void init(cobnd_proxy< element<config_type, element_tag>,
                             EnclosingType> const & p)
      {
        typedef coboundary_key<EnclosingType, dim>   CoBoundaryKey;
        
        CoBoundaryKey key(p.second());
        
        //initialize co-boundary if needed
        if (viennadata::find<CoBoundaryKey,
                             viennadata_container_type >(key)(p.first()) == NULL)
        {
           init_coboundary< element_tag::topology_level,
                            dim,
                            viennadata_container_type>(key, p.second());
        }
        
        viennadata_container_type & temp = viennadata::access<CoBoundaryKey,
                                                              viennadata_container_type>(key)(p.first());
        cont_ = &(temp[0]);
        num_elements = temp.size();
      }
      
      container_type * cont_;
      size_t num_elements;
  };
  
  
  // non-const:
  template <typename T, typename U>
  class const_cobnd_proxy
  {
    public:
      const_cobnd_proxy(T const & t_, U & u_) : t(t_), u(u_) {}
      
      T const & first() const { return t; }
      U & second() const { return u; }
    
    private:
      T const & t;
      U & u;
  };
  
  template <dim_type dim, typename Config, typename ElementTag>
  const_cobnd_proxy< element<Config, ElementTag>,
                     domain<Config> >
  ncells(element<Config, ElementTag> const & e, domain<Config> & d)
  {
    return const_cobnd_proxy< element<Config, ElementTag>,
                              domain<Config> >(e, d);
  }

  template <dim_type dim, typename Config, typename ElementTag>
  const_cobnd_proxy< element<Config, ElementTag>,
                     segment_t<Config> >
  ncells(element<Config, ElementTag> const & e, segment_t<Config> & seg)
  {
    return const_cobnd_proxy< element<Config, ElementTag>,
                              segment_t<Config> >(e, seg);
  }

  template <typename config_type, typename element_tag,
            dim_type dim>
  class const_ncell_range < element<config_type, element_tag>, dim, true>
  {
      typedef element< config_type,
                       typename topology::subcell_desc<typename config_type::cell_tag,
                                                     dim>::element_tag
                     >                                                         element_type;
                     
      typedef element<config_type, element_tag>                                host_type;
      typedef std::vector<element_type *>                                      viennadata_container_type;
      typedef element_type *                                                   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef const_on_element_iterator<element_type>                                iterator;

      const_ncell_range() {};
      
      template <typename EnclosingType> //either domain or segment
      const_ncell_range(cobnd_proxy< element<config_type, element_tag>,
                                         EnclosingType> const & p) 
      {
        const_cobnd_proxy< element<config_type, element_tag>,
                           EnclosingType> temp(p.first(), p.second());
        init(temp);
      }

      template <typename EnclosingType> //either domain or segment
      const_ncell_range(const_cobnd_proxy< element<config_type, element_tag>,
                                               EnclosingType> const & p)
      {
        init(p);
      }




      template <typename EnclosingType> //either domain or segment
      const_ncell_range & operator=(cobnd_proxy< element<config_type, element_tag>,
                                                     EnclosingType> const & p)
      { 
        const_cobnd_proxy< element<config_type, element_tag>,
                           EnclosingType> temp(p.first(), p.second());
        init(temp);
        return *this;
      }

      template <typename EnclosingType> //either domain or segment
      const_ncell_range & operator=(const_cobnd_proxy< element<config_type, element_tag>,
                                                           EnclosingType> const & p)
      { 
        init(p);
        return *this;
      }

      iterator begin() const
      {
        assert(cont_ != NULL);
        return iterator(cont_);
      }
      
      iterator end()   const
      {
        assert(cont_ != NULL);
        return iterator(cont_ + num_elements);
      }
      
    private:
      
      template <typename EnclosingType>
      void init(const_cobnd_proxy< element<config_type, element_tag>,
                                               EnclosingType> const & p)
      {
        typedef coboundary_key<EnclosingType, dim>   CoBoundaryKey;
        
        CoBoundaryKey key(p.second());
        
        //initialize co-boundary if needed
        if (viennadata::find<CoBoundaryKey,
                             viennadata_container_type >(key)(p.first()) == NULL)
        {
           init_coboundary< element_tag::topology_level,
                            dim,
                            viennadata_container_type>(key, p.second());
        }
        
        viennadata_container_type & temp = viennadata::access<CoBoundaryKey,
                                                              viennadata_container_type>(key)(p.first());
        cont_ = &(temp[0]);
        num_elements = temp.size();
      }
      
      const container_type * cont_;
      size_t num_elements;
  };
  
  
  
  
  
  
  
  
  
  
  namespace result_of
  {
    template <typename Config, typename ElementTag,
              dim_type dim>  //topological level
    struct ncell_range < element<Config, ElementTag>, dim >
    {
      typedef viennagrid::ncell_range<element<Config, ElementTag>,
                                          dim,
                                          (ElementTag::topology_level < dim)> 
                                                   type;
    };
    
    template <typename Config, typename ElementTag,
              dim_type dim>  //topological level
    struct const_ncell_range < element<Config, ElementTag>, dim >
    {
      typedef viennagrid::const_ncell_range<element<Config, ElementTag>,
                                                dim,
                                                (ElementTag::topology_level < dim)> 
                                                   type;
    };
    
    
    template <typename Config, typename ElementTag,
              dim_type dim,
              dim_type cell_level /* see forwards.h for default argument */>
    struct element_container< element<Config, ElementTag>, dim, cell_level >
    {
      typedef typename result_of::ncell_type<Config, dim>::type            element_type;
      
      typedef element_type *      type;
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
                                 
      typedef typename viennagrid::ncell_range < element<config_type, element_tag>, dim>::iterator    type;                           
    };


  }
  
} //namespace viennafem

#endif
