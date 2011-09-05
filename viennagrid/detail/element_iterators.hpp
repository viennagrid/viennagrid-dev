#ifndef VIENNAGRID_ELEMENT_ITERATORS_HPP
#define VIENNAGRID_ELEMENT_ITERATORS_HPP

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

  template <typename ElementType>
  class const_on_element_iterator;

  //RangeElement-Type prevents abuse, for example:
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
      bool operator==(const const_on_element_iterator<ElementType> & i) const { return pp_ == i.pp_; }
      
      bool operator!=(const on_element_iterator& i) const { return pp_ != i.pp_; }
      bool operator!=(const const_on_element_iterator<ElementType> & i) const { return pp_ != i.pp_; }

      //support for element-orientation-retrieval:
//       long operator-(const ocit & o2) const { return pp_ - o2.pp_; }

      template <typename ElementType2>
      friend class const_on_element_iterator; //so that a const_on_element iterator can be initialized from an on_element_iterator

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
      const_on_element_iterator(on_element_iterator<ElementType> const & oei) : pp_(oei.pp_) {}

      const ElementType & operator*() const { return **pp_; }
      const ElementType * operator->() const { return *pp_; }

      const_on_element_iterator & operator++() { ++pp_; return *this; }
      const_on_element_iterator   operator++(int) { const_on_element_iterator tmp = *this; ++*this; return tmp; }

      bool operator==(const const_on_element_iterator& i) const { return pp_ == i.pp_; }
      bool operator==(const on_element_iterator<ElementType> & i) const { return pp_ == i.pp_; }
      
      bool operator!=(const const_on_element_iterator& i) const { return pp_ != i.pp_; }
      bool operator!=(const on_element_iterator<ElementType> & i) const { return pp_ != i.pp_; }

      template <typename ElementType2>
      friend class on_element_iterator; //so that a on_element iterator can be compared with a const_on_element_iterator

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
            typename handling_tag = typename topology::subelements<ElementTag, level>::handling_tag>
  struct ElementIteratorChecker
  {
    enum{ ReturnValue = topology::subelements<ElementTag, level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
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
  struct IteratorChecker< element_t<Config, ElementTag>, level>
  {
    enum{ ReturnValue = ElementIteratorChecker<ElementTag, level>::ReturnValue };
  };


  
  //interface function for container creation:
  
  // non-const:
  template <typename config_type, typename element_tag, dim_type dim>
  class ncell_range < element_t<config_type, element_tag>, dim, false>
  {
      typedef element_t< config_type,
                       typename topology::subelements<element_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, element_tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::topology_level>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator< element_type >                              iterator;
      
      ncell_range() : cont_(NULL) {};
      
      ncell_range(ncell_proxy<host_type> const & p) : cont_(p.get().template container<dim>()) {}
      
      ncell_range(host_type & e) : cont_(e.template container<dim>()) {}
      
      ncell_range & operator=(ncell_proxy<host_type> p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }
      
      iterator begin() const
      { 
        assert(cont_ != NULL);
        return iterator(&(cont_[0]));
      }
      
      iterator end()   const
      {
        assert(cont_ != NULL);
        return iterator(cont_ + topology::subelements<element_tag, dim>::num_elements);
      }
      
      element_type & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return topology::subelements<element_tag, dim>::num_elements; }
      
    private:
      container_type * cont_;
  };
  
  template <dim_type dim, typename Config, typename ElementTag>
  typename result_of::ncell_range< element_t<Config, ElementTag>, dim>::type
  ncells(element_t<Config, ElementTag> & d)
  {
    return typename result_of::ncell_range< element_t<Config, ElementTag>, dim>::type(d);
  }
  
  template <typename Config, typename ElementTag>
  ncell_proxy< element_t<Config, ElementTag> >
  ncells(element_t<Config, ElementTag> & d)
  {
    return ncell_proxy< element_t<Config, ElementTag> >(d);
  }
  
  
  //
  // const container:
  //
  template <typename config_type, typename element_tag, dim_type dim>
  class const_ncell_range < element_t<config_type, element_tag>, dim, false>
  {
      typedef element_t< config_type,
                       typename topology::subelements<element_tag, dim>::element_tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, element_tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::topology_level>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef const_on_element_iterator< element_type >                         iterator;
      const_ncell_range() : cont_(NULL) {};
      
      const_ncell_range(const_ncell_proxy<host_type> const & p) : cont_(p.get().template container<dim>()) {}

      const_ncell_range(ncell_proxy<host_type> const & p) : cont_(p.get().template container<dim>()) {}

      const_ncell_range(host_type const & e) : cont_(e.template container<dim>()) {}


      const_ncell_range & operator=(const_ncell_proxy<host_type> p)
      { 
        cont_ = p.get().template container<dim>();
        return *this;
      }

      const_ncell_range & operator=(ncell_proxy<host_type> p)
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
        return iterator(cont_ + topology::subelements<element_tag, dim>::num_elements);
      }
      
      element_type const & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return topology::subelements<element_tag, dim>::num_elements; }
      
    private:
      const container_type * cont_;
  };
  
  template <dim_type dim, typename Config, typename ElementTag>
  typename result_of::const_ncell_range< element_t<Config, ElementTag>, dim>::type
  ncells(element_t<Config, ElementTag> const & d)
  {
    return typename result_of::const_ncell_range< element_t<Config, ElementTag>, dim>::type(d);
  }
  
  template <typename Config, typename ElementTag>
  const_ncell_proxy< element_t<Config, ElementTag> >
  ncells(element_t<Config, ElementTag> const & d)
  {
    return const_ncell_proxy< element_t<Config, ElementTag> >(d);
  }
  
  
  
  
  
  
  ////////////////   co-boundaries: /////////////////////////
  
  //helper meta function for selecting const/non-const containers:
  template <dim_type dim_start,
            dim_type dim_iter,
            typename RangeType,
            typename KeyType,
            typename EnclosingType>
  void init_coboundary(KeyType const & key,
                       EnclosingType const & domain)
  {
    typedef typename EnclosingType::config_type         Config;
    typedef typename result_of::ncell<Config, dim_start>::type    LowerElementType;
    typedef typename result_of::ncell<Config, dim_iter>::type     HigherElementType;
    
    typedef typename result_of::const_ncell_range<EnclosingType, dim_iter>::type      HigherElementRange;
    typedef typename result_of::iterator<HigherElementRange>::type                    HigherElementIterator;
    
    typedef typename result_of::const_ncell_range<HigherElementType, dim_start>::type  LowerOnHigherRange;
    typedef typename result_of::iterator<LowerOnHigherRange>::type                     LowerOnHigherIterator;
    
    
    HigherElementRange higher_container = ncells<dim_iter>(domain);
    for (HigherElementIterator hit = higher_container.begin();
         hit != higher_container.end();
         ++hit)
    {
      LowerOnHigherRange lower_container = ncells<dim_start>(*hit);
      for (LowerOnHigherIterator low = lower_container.begin();
           low != lower_container.end();
           ++low)
      {
        viennadata::access<KeyType, RangeType>(key)(*low).push_back(const_cast<HigherElementType *>(&(*hit)));
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
  ncell_range < element_t<Config, ElementTag>, dim, true>
  ncells(element_t<Config, ElementTag> & e, domain_t<Config> & d)
  {
    return ncell_range < element_t<Config, ElementTag>, dim, true>(e, d);
  }

  /** @brief Returns a proxy object for the iteration over the neighbor elements within a domain. Non-const version.
   * 
   * The typical use of this function is as shortcut for the initialization of a Range, e.g.
   * EdgeOnVertexRange edges = ncells(vertex, domain);
   * 
   * @param e  The element over which neighbors to iterate
   * @param d  The segment in which the neighbors must be located
   * @return   A proxy object
   */
  template <typename Config, typename ElementTag>
  cobnd_proxy< element_t<Config, ElementTag>,
               domain_t<Config> >
  ncells(element_t<Config, ElementTag> & e, domain_t<Config> & d)
  {
    return cobnd_proxy< element_t<Config, ElementTag>,
                        domain_t<Config> >(e, d);
  }



  template <dim_type dim, typename Config, typename ElementTag>
  ncell_range < element_t<Config, ElementTag>, dim, true>
  ncells(element_t<Config, ElementTag> & e, segment_t<Config> & seg)
  {
    return ncell_range < element_t<Config, ElementTag>, dim, true>(e, seg);
  }


  /** @brief Returns a proxy object for the iteration over the neighbor elements within a segment. Non-const version.
   * 
   * The typical use of this function is as shortcut for the initialization of a Range, e.g.
   * EdgeOnVertexRange edges = ncells(vertex, segment);
   * 
   * @param e    The element over which neighbors to iterate
   * @param seg  The segment in which the neighbors must be located
   * @return     A proxy object
   */
  template <typename Config, typename ElementTag>
  cobnd_proxy< element_t<Config, ElementTag>,
               segment_t<Config> >
  ncells(element_t<Config, ElementTag> & e, segment_t<Config> & seg)
  {
    return cobnd_proxy< element_t<Config, ElementTag>,
                        segment_t<Config> >(e, seg);
  }

  template <typename config_type, typename element_tag,
            dim_type dim>
  class ncell_range < element_t<config_type, element_tag>, dim, true>
  {
      typedef element_t< config_type,
                       typename topology::subelements<typename config_type::cell_tag,
                                               dim>::element_tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, element_tag>                              host_type;
      typedef std::vector<element_type *>                                      viennadata_container_type;
      typedef element_type *                                                   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator<element_type>                                iterator;
      
      ncell_range() : cont_(NULL) {};
      
      template <typename EnclosingType> //either domain or segment
      ncell_range(cobnd_proxy<host_type, EnclosingType> const & p)
      {
        init(p.first(), p.second());
      }
      
      template <typename EnclosingType> //either domain or segment
      ncell_range(host_type const & e,
                  EnclosingType const & d) 
      {
        //std::cout << "Using non-const!" << std::endl;
        init(e, d);
      }
      
      template <typename EnclosingType> //either domain or segment
      ncell_range & operator=(cobnd_proxy<host_type, EnclosingType> const & p)
      { 
        init(p.first(), p.second());
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

      element_type & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return num_elements; }
      
      template <typename element_type, dim_type dim2, bool b2>
      friend class const_ncell_range;
      
    private:
      template <typename EnclosingType>
      void init(host_type const & e,
                EnclosingType const & d)
      //void init(cobnd_proxy< element<config_type, element_tag>,
      //                       EnclosingType> const & p)
      {
        typedef coboundary_key<EnclosingType, dim>   CoBoundaryKey;
        
        CoBoundaryKey key(d);
        
        //initialize co-boundary if needed
        if (viennadata::find<CoBoundaryKey,
                             viennadata_container_type >(key)(e) == NULL)
        {
           init_coboundary< element_tag::topology_level,
                            dim,
                            viennadata_container_type>(key, d);
        }
        
        viennadata_container_type & temp = viennadata::access<CoBoundaryKey,
                                                              viennadata_container_type>(key)(e);
        cont_ = &(temp[0]);
        num_elements = temp.size();
      }
      
      container_type * cont_;
      size_t num_elements;
  };
  
  
  // const:
  template <typename T, typename U>
  class const_cobnd_proxy
  {
    public:
      const_cobnd_proxy(T const & t_, U const & u_) : t(t_), u(u_) {}
      
      T const & first() const { return t; }
      U const & second() const { return u; }
    
    private:
      T const & t;
      U const & u;
  };
  
  template <typename config_type, typename element_tag,
            dim_type dim>
  class const_ncell_range < element_t<config_type, element_tag>, dim, true>
  {
      typedef element_t< config_type,
                       typename topology::subelements<typename config_type::cell_tag,
                                                     dim>::element_tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, element_tag>                                host_type;
      typedef std::vector<element_type *>                                      viennadata_container_type;
      typedef element_type *                                                   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef const_on_element_iterator<element_type>                                iterator;

      const_ncell_range() {};
      
      template <typename EnclosingType> //either domain or segment
      const_ncell_range(cobnd_proxy<host_type, EnclosingType> const & p) 
      {
        init(p.first(), p.second());
      }

      template <typename EnclosingType> //either domain or segment
      const_ncell_range(const_cobnd_proxy<host_type, EnclosingType> const & p)
      {
        init(p.first(), p.second());
      }

      template <typename EnclosingType> //either domain or segment
      const_ncell_range(host_type const & e,
                        EnclosingType const & d) 
      {
        //std::cout << "Using const!" << std::endl;
        init(e, d);
      }



      template <typename EnclosingType> //either domain or segment
      const_ncell_range & operator=(cobnd_proxy<host_type, EnclosingType> const & p)
      { 
        init(p.first(), p.second());
        return *this;
      }

      template <typename EnclosingType> //either domain or segment
      const_ncell_range & operator=(const_cobnd_proxy<host_type, EnclosingType> const & p)
      { 
        init(p.first(), p.second());
        return *this;
      }

      const_ncell_range & operator=(ncell_range<host_type, dim, true > const & other)
      { 
        cont_ = other.cont_;
        num_elements = other.num_elements;
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
      
      element_type const & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return num_elements; }
      
    private:
      
      template <typename EnclosingType>
      void init(host_type const & e,
                EnclosingType const & d)
      //void init(const_cobnd_proxy< element<config_type, element_tag>,
      //                                         EnclosingType> const & p)
      {
        typedef coboundary_key<EnclosingType, dim>   CoBoundaryKey;
        
        CoBoundaryKey key(d);
        
        //initialize co-boundary if needed
        if (viennadata::find<CoBoundaryKey,
                             viennadata_container_type >(key)(e) == NULL)
        {
           init_coboundary< element_tag::topology_level,
                            dim,
                            viennadata_container_type>(key, d);
        }
        
        viennadata_container_type & temp = viennadata::access<CoBoundaryKey,
                                                              viennadata_container_type>(key)(e);
        cont_ = &(temp[0]);
        num_elements = temp.size();
      }
      
      const container_type * cont_;
      size_t num_elements;
  };
  
  
  template <dim_type dim, typename Config, typename ElementTag>
  const_ncell_range < element_t<Config, ElementTag>, dim, true>
  ncells(element_t<Config, ElementTag> const & e, domain_t<Config> const & d)
  {
    return const_ncell_range < element_t<Config, ElementTag>, dim, true>(e, d);
  }

  template <typename Config, typename ElementTag>
  const_cobnd_proxy< element_t<Config, ElementTag>,
                     domain_t<Config> >
  ncells(element_t<Config, ElementTag> const & e, domain_t<Config> const & d)
  {
    return const_cobnd_proxy< element_t<Config, ElementTag>,
                              domain_t<Config> >(e, d);
  }



  template <dim_type dim, typename Config, typename ElementTag>
  const_ncell_range < element_t<Config, ElementTag>, dim, true>
  ncells(element_t<Config, ElementTag> const & e, segment_t<Config> const & seg)
  {
    return const_ncell_range < element_t<Config, ElementTag>, dim, true>(e, seg);
  }

  template <typename Config, typename ElementTag>
  const_cobnd_proxy< element_t<Config, ElementTag>,
                     segment_t<Config> >
  ncells(element_t<Config, ElementTag> const & e, segment_t<Config> const & seg)
  {
    return const_cobnd_proxy< element_t<Config, ElementTag>,
                              segment_t<Config> >(e, seg);
  }

  
  
  
  
  
  
  
  
  namespace result_of
  {
    template <typename Config, typename ElementTag,
              dim_type dim>  //topological level
    struct ncell_range < element_t<Config, ElementTag>, dim >
    {
      typedef viennagrid::ncell_range<element_t<Config, ElementTag>,
                                          dim,
                                          (ElementTag::topology_level < dim)> 
                                                   type;
    };
    
    template <typename Config, typename ElementTag,
              dim_type dim>  //topological level
    struct const_ncell_range < element_t<Config, ElementTag>, dim >
    {
      typedef viennagrid::const_ncell_range<element_t<Config, ElementTag>,
                                                dim,
                                                (ElementTag::topology_level < dim)> 
                                                   type;
    };
    
    
    template <typename Config, typename ElementTag,
              dim_type dim,
              dim_type cell_level /* see forwards.h for default argument */>
    struct element_container< element_t<Config, ElementTag>, dim, cell_level >
    {
      typedef typename result_of::ncell<Config, dim>::type            element_type;
      
      typedef element_type *      type;
    };
    
    //Iterator types for elements
    template <typename config_type, typename element_tag, dim_type dim>
    struct iterator< element_t<config_type, element_tag>,
                     dim>
    {
      /*typedef on_element_iterator< element<Config, ElementTag>, //the host element type
                                   element<Config, //the subcell type
                                           typename subcell_traits<ElementTag, level>::element_tag> 
                                 > type; */
                                 
      typedef typename viennagrid::ncell_range < element_t<config_type, element_tag>, dim>::iterator    type;                           
    };


  }
  
} //namespace viennafem

#endif

