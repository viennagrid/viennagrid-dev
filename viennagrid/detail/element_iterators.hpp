#ifndef VIENNAGRID_ELEMENT_ITERATORS_HPP
#define VIENNAGRID_ELEMENT_ITERATORS_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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

#include "viennadata/api.hpp"

/** @file element_iterators.hpp
    @brief Provides the iterators and ranges for n-cells
*/

namespace viennagrid
{

  template <typename ElementType>
  class const_on_element_iterator;

  //RangeElement-Type prevents abuse, for example:
  //A vertex-on-facet-iterator is not equal to a vertex-on-cell-iterator!
  /** @brief Iterator class for iterating over a range of elements given by a container of pointers */
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
  /** @brief Iterator class for const iteration over a range of elements given by a container of pointers */
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
  
  
  /////////////// Iterator Types retrieval /////////


  //Check availability of iterators:
  /** @brief A helper class that checks whether a certain iterator is available */
  template <typename ElementTag,
            long level,
            typename handling_tag = typename topology::bndcells<ElementTag, level>::handling_tag>
  struct ElementIteratorChecker
  {
    enum{ ReturnValue = topology::bndcells<ElementTag, level>::ERROR_ITERATOR_NOT_PROVIDED_AT_THIS_LEVEL };
  };

  /** @brief Specialization for an available iterator */
  template <typename ElementTag,
            long level>
  struct ElementIteratorChecker< ElementTag, level, full_handling_tag>
  {
    enum{ ReturnValue = ElementTag::dim - level };
  };


  /** @brief Class that checks the validity of the requested iterator */
  template <typename ElementType,
            long level>
  struct IteratorChecker
  {
    enum{ ReturnValue = ElementType::ERROR_ELEMENT_TYPE_INVALID };
  };

  /** @brief Class that checks the validity of the requested iterator */
  template <typename Config,
            typename ElementTag,
            long level>
  struct IteratorChecker< element_t<Config, ElementTag>, level>
  {
    enum{ ReturnValue = ElementIteratorChecker<ElementTag, level>::ReturnValue };
  };


  
  // non-const:
  /** @brief The main non-const range object for iteration or direct access of boundary k-cells of a n-cell
   * 
   * @tparam config_type       Configuration class
   * @tparam tag               Tag identifying the n-cell
   * @tparam dim               Topological dimension of the requested k-cells
   */
  template <typename config_type, typename tag, long dim>
  class ncell_range < element_t<config_type, tag>, dim, false>
  {
      typedef element_t< config_type,
                       typename topology::bndcells<tag, dim>::tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::dim>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_element_iterator< element_type >                              iterator;
      
      ncell_range() : cont_(NULL) {};
      
      ncell_range(ncell_proxy<host_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}
      
      ncell_range(host_type & e) : cont_(e.container(dimension_tag<dim>())) {}
      
      ncell_range & operator=(ncell_proxy<host_type> p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
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
        return iterator(cont_ + topology::bndcells<tag, dim>::num);
      }
      
      /** @brief Provide direct random-access to boundary cells */
      element_type & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      /** @brief Returns the number of k-cells */
      std::size_t size() const { return topology::bndcells<tag, dim>::num; }
      
    private:
      container_type * cont_;
  };
  
  
  /** @brief Main function for range retrieval. Specialization for iteration over k-cells of a n-cell */
  template <long dim, typename Config, typename ElementTag>
  typename result_of::ncell_range< element_t<Config, ElementTag>, dim>::type
  ncells(element_t<Config, ElementTag> & d)
  {
    return typename result_of::ncell_range< element_t<Config, ElementTag>, dim>::type(d);
  }
  
  /** @brief Main function for range retrieval. Specialization for iteration over k-cells of a n-cell. Returns only a proxy that must be assigned to a range object.
   *
   * Allows to omit the topological dimension if this is clear from the range type, e.g.
   *  VertexRange vertices = ncells(facet);
   */
  template <typename Config, typename ElementTag>
  ncell_proxy< element_t<Config, ElementTag> >
  ncells(element_t<Config, ElementTag> & d)
  {
    return ncell_proxy< element_t<Config, ElementTag> >(d);
  }
  
  
  //
  // const container:
  //
  /** @brief The main const range object for iteration or direct access of boundary k-cells of a n-cell
   * 
   * @tparam config_type       Configuration class
   * @tparam tag               Tag identifying the n-cell
   * @tparam dim               Topological dimension of the requested k-cells
   */
  template <typename config_type, typename tag, long dim>
  class const_ncell_range < element_t<config_type, tag>, dim, false>
  {
      typedef element_t< config_type,
                       typename topology::bndcells<tag, dim>::tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, tag>                                host_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<host_type, dim, config_type::cell_tag::dim>::type      container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef const_on_element_iterator< element_type >                         iterator;
      const_ncell_range() : cont_(NULL) {};
      
      const_ncell_range(const_ncell_proxy<host_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}

      const_ncell_range(ncell_proxy<host_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}

      const_ncell_range(host_type const & e) : cont_(e.container(dimension_tag<dim>())) {}


      const_ncell_range & operator=(const_ncell_proxy<host_type> p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
        return *this;
      }

      const_ncell_range & operator=(ncell_proxy<host_type> p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
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
        return iterator(cont_ + topology::bndcells<tag, dim>::num);
      }
      
      element_type const & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return topology::bndcells<tag, dim>::num; }
      
    private:
      const container_type * cont_;
  };
  
  /** @brief Main function for const range retrieval. Specialization for iteration over k-cells of a n-cell */
  template <long dim, typename Config, typename ElementTag>
  typename result_of::const_ncell_range< element_t<Config, ElementTag>, dim>::type
  ncells(element_t<Config, ElementTag> const & d)
  {
    return typename result_of::const_ncell_range< element_t<Config, ElementTag>, dim>::type(d);
  }
  
  /** @brief Main function for const range retrieval. Specialization for iteration over k-cells of a n-cell. Returns only a proxy that must be assigned to a range object.
   *
   * Allows to omit the topological dimension if this is clear from the range type, e.g.
   *  VertexRange vertices = ncells(facet);
   */
  template <typename Config, typename ElementTag>
  const_ncell_proxy< element_t<Config, ElementTag> >
  ncells(element_t<Config, ElementTag> const & d)
  {
    return const_ncell_proxy< element_t<Config, ElementTag> >(d);
  }
  
  
  
  
  
  
  ////////////////   co-boundaries: /////////////////////////
  
  //helper meta function for selecting const/non-const containers:
  /** @brief A helper function that sets up the co-boundary information */
  template <long dim_start,
            long dim_iter,
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
  /** @brief A proxy object for cheaper setup of range objects using the ncells<>() function */
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
  
  /** @brief Returns the co-boundary range of topological dimension k for a n-cell, k>n */
  template <long dim, typename Config, typename ElementTag>
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
   * @param d  The domain in which the neighbors must be located
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


  /** @brief Returns a proxy object for the iteration over the neighbor elements within a segment. Non-const version.
   * 
   * The typical use of this function is as shortcut for the initialization of a Range, e.g.
   * EdgeOnVertexRange edges = ncells(vertex, segment);
   * 
   * @param e  The element over which neighbors to iterate
   * @param seg  The segment in which the neighbors must be located
   * @return   A proxy object
   */
  template <long dim, typename Config, typename ElementTag>
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


  /** @brief The range class for non-const iteration and access of coboundary k-cells of a n-cell, k>n 
   * 
   * @tparam config_type       Configuration class
   * @tparam tag               Tag identifying the n-cell
   * @tparam dim               Topological dimension of the requested k-cells
   */
  template <typename config_type, typename tag,
            long dim>
  class ncell_range < element_t<config_type, tag>, dim, true>
  {
      typedef element_t< config_type,
                       typename topology::bndcells<typename config_type::cell_tag,
                                               dim>::tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, tag>                              host_type;
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
        return iterator(cont_ + num);
      }

      element_type & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return num; }
      
      template <typename element_type, long dim2, bool b2>
      friend class const_ncell_range;
      
    private:
      template <typename EnclosingType>
      void init(host_type const & e,
                EnclosingType const & d)
      //void init(cobnd_proxy< element<config_type, tag>,
      //                       EnclosingType> const & p)
      {
        typedef coboundary_key<EnclosingType, dim>   CoBoundaryKey;
        
        CoBoundaryKey key(d);
        
        //initialize co-boundary if needed
        if (viennadata::find<CoBoundaryKey,
                             viennadata_container_type >(key)(e) == NULL)
        {
           init_coboundary< tag::dim,
                            dim,
                            viennadata_container_type>(key, d);
        }
        
        viennadata_container_type & temp = viennadata::access<CoBoundaryKey,
                                                              viennadata_container_type>(key)(e);
        cont_ = &(temp[0]);
        num = temp.size();
      }
      
      container_type * cont_;
      size_t num;
  };
  
  
  // const:
  /** @brief A proxy object for cheaper setup of range objects using the ncells<>() function */
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
  
  /** @brief The range class for const iteration and access of coboundary k-cells of a n-cell, k>n 
   * 
   * @tparam config_type       Configuration class
   * @tparam tag               Tag identifying the n-cell
   * @tparam dim               Topological dimension of the requested k-cells
   */
  template <typename config_type, typename tag,
            long dim>
  class const_ncell_range < element_t<config_type, tag>, dim, true>
  {
      typedef element_t< config_type,
                       typename topology::bndcells<typename config_type::cell_tag,
                                                     dim>::tag
                     >                                                         element_type;
                     
      typedef element_t<config_type, tag>                                host_type;
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
        num = other.num;
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
        return iterator(cont_ + num);
      }
      
      element_type const & operator[](std::size_t index) const 
      {
        assert(index < size());
        return *(cont_[index]); 
      }
      
      std::size_t size() const { return num; }
      
    private:
      
      template <typename EnclosingType>
      void init(host_type const & e,
                EnclosingType const & d)
      //void init(const_cobnd_proxy< element<config_type, tag>,
      //                                         EnclosingType> const & p)
      {
        typedef coboundary_key<EnclosingType, dim>   CoBoundaryKey;
        
        CoBoundaryKey key(d);
        
        //initialize co-boundary if needed
        if (viennadata::find<CoBoundaryKey,
                             viennadata_container_type >(key)(e) == NULL)
        {
           init_coboundary< tag::dim,
                            dim,
                            viennadata_container_type>(key, d);
        }
        
        viennadata_container_type & temp = viennadata::access<CoBoundaryKey,
                                                              viennadata_container_type>(key)(e);
        cont_ = &(temp[0]);
        num = temp.size();
      }
      
      const container_type * cont_;
      size_t num;
  };
  
  
  /** @brief Returns a proxy object for the iteration over the neighbor elements within a domain. Non-const version.
   * 
   * The typical use of this function is as shortcut for the initialization of a Range, e.g.
   * EdgeOnVertexRange edges = ncells(vertex, segment);
   * 
   * @param e    The element over which neighbors to iterate
   * @param d    The domain in which the neighbors must be located
   * @return     A proxy object
   */
  template <long dim, typename Config, typename ElementTag>
  const_ncell_range < element_t<Config, ElementTag>, dim, true>
  ncells(element_t<Config, ElementTag> const & e, domain_t<Config> const & d)
  {
    return const_ncell_range < element_t<Config, ElementTag>, dim, true>(e, d);
  }

  /** @brief Returns a proxy object for the iteration over the neighbor elements within a domain. Const version.
   * 
   * The typical use of this function is as shortcut for the initialization of a Range, e.g.
   * EdgeOnVertexRange edges = ncells(vertex, segment);
   * 
   * @param e    The element over which neighbors to iterate
   * @param d    The domain in which the neighbors must be located
   * @return     A proxy object
   */
  template <typename Config, typename ElementTag>
  const_cobnd_proxy< element_t<Config, ElementTag>,
                     domain_t<Config> >
  ncells(element_t<Config, ElementTag> const & e, domain_t<Config> const & d)
  {
    return const_cobnd_proxy< element_t<Config, ElementTag>,
                              domain_t<Config> >(e, d);
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
  template <long dim, typename Config, typename ElementTag>
  const_ncell_range < element_t<Config, ElementTag>, dim, true>
  ncells(element_t<Config, ElementTag> const & e, segment_t<Config> const & seg)
  {
    return const_ncell_range < element_t<Config, ElementTag>, dim, true>(e, seg);
  }

  /** @brief Returns a proxy object for the iteration over the neighbor elements within a segment. Const version.
   * 
   * The typical use of this function is as shortcut for the initialization of a Range, e.g.
   * EdgeOnVertexRange edges = ncells(vertex, segment);
   * 
   * @param e    The element over which neighbors to iterate
   * @param seg  The segment in which the neighbors must be located
   * @return     A proxy object
   */
  template <typename Config, typename ElementTag>
  const_cobnd_proxy< element_t<Config, ElementTag>,
                     segment_t<Config> >
  ncells(element_t<Config, ElementTag> const & e, segment_t<Config> const & seg)
  {
    return const_cobnd_proxy< element_t<Config, ElementTag>,
                              segment_t<Config> >(e, seg);
  }

  
  
  //
  // local vertex orientation:
  //
  /** @brief Returns the index-th vertices of a boundary k-cell in the orientation induced by the hosting n-cell 
   * 
   * @param host_ncell     The hosting n-cell
   * @param bnd_kcell      The boundary k-cell
   * @param index          Index for the i-th vertex
   */
  template <typename ConfigType, typename Tag1, typename Tag2>
  typename result_of::ncell<ConfigType, 0>::type
  local_vertex(element_t<ConfigType, Tag1> const & host_ncell,
               element_t<ConfigType, Tag2> const & bnd_kcell,
               std::size_t index)
  {
    //std::cout << host_ncell.global_to_local_orientation(bnd_kcell, index) << std::endl;
    return viennagrid::ncells<0>(bnd_kcell)[host_ncell.global_to_local_orientation(bnd_kcell, index)];
  }
  
  
  
  
  //
  // Generic layer using metafunctions:
  //
  namespace result_of
  {
    /** @brief Helper function for less-than comparison at compile time. */
    template <long a, long b>
    struct is_smaller
    {
      enum { value = (a < b) }; 
    };
    
    /** @brief Range type retrieval for boundary and coboundary iteration */
    template <typename Config, typename ElementTag,
              long dim>  //topological level
    struct ncell_range < element_t<Config, ElementTag>, dim >
    {
      typedef viennagrid::ncell_range<element_t<Config, ElementTag>,
                                          dim,
                                          is_smaller<ElementTag::dim, dim>::value
                                      > 
                                                   type;
    };
    
    /** @brief Const Range type retrieval for boundary and coboundary iteration */
    template <typename Config, typename ElementTag,
              long dim>  //topological level
    struct const_ncell_range < element_t<Config, ElementTag>, dim >
    {
      typedef viennagrid::const_ncell_range<element_t<Config, ElementTag>,
                                             dim,
                                             is_smaller<ElementTag::dim, dim>::value
                                           > 
                                                   type;
    };
    
    /** @brief Returns the internal storage type of ranges for boundary k-cells */
    template <typename Config, typename ElementTag,
              long dim,
              long cell_level /* see forwards.h for default argument */>
    struct element_container< element_t<Config, ElementTag>, dim, cell_level >
    {
      typedef typename result_of::ncell<Config, dim>::type            element_type;
      
      typedef element_type *      type;
    };
    
    //Iterator types for elements
    /** @brief Returns the iterator for iteration over a range */
    template <typename config_type, typename tag, long dim>
    struct iterator< element_t<config_type, tag>,
                     dim>
    {
      /*typedef on_element_iterator< element<Config, ElementTag>, //the host element type
                                   element<Config, //the subcell type
                                           typename subcell_traits<ElementTag, level>::tag> 
                                 > type; */
                                 
      typedef typename viennagrid::ncell_range < element_t<config_type, tag>, dim>::iterator    type;                           
    };


  }
  
} //namespace viennafem

#endif

