#ifndef VIENNAGRID_SEGMENT_ITERATORS_HPP
#define VIENNAGRID_SEGMENT_ITERATORS_HPP

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

#include "viennagrid/forwards.h"
#include "viennagrid/detail/domain_iterators.hpp"
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/segment.hpp"

/** @file segment_iterators.hpp
    @brief Provides the iterators and ranges for segments
*/

namespace viennagrid
{
  /** @brief Iterator for non-const iteration over n-cells of a segment */
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
  
  /** @brief Iterator for const iteration over n-cells of a segment */
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
            long dim,
            long cell_level = Config::cell_tag::dim>
  struct segment_iterators;
  
  
  //special case: cells:
  /** @brief Segment iterator type retrieval for cells */
  template <typename Config, long cell_level>
  struct segment_iterators< Config, cell_level, cell_level>
  {
    typedef segment_t<Config>                     segment_type;
    typedef element_t<Config,
                    typename Config::cell_tag>    cell_type;
                    
    typedef typename result_of::element_container<segment_type,
                                                  Config::cell_tag::dim>::type   container_type;
    
    typedef typename container_type::iterator          iterator;
    typedef typename container_type::const_iterator    const_iterator;
  };
  
  
  //non-const iteration:
  // interface function for container creation,
  // non-const:

  //container for iteration over a STL vector
  /** @brief Main range class (non-const). Specialization for use with iteration or access to k-cells of a segment */
  template <typename config_type, long dim>
  class ncell_range < segment_t<config_type>, dim, false >
  {
      typedef segment_t<config_type>                        segment_type;
      typedef element_t< config_type,
                       typename topology::bndcells<typename config_type::cell_tag, dim>::tag
                     >                                                         element_type;
                     
      typedef element_t< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<segment_type, dim>::type   container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef on_segment_iterator< typename container_type::iterator, element_type >   iterator;
      
      ncell_range() : cont_(NULL) {};
      
      ncell_range(ncell_proxy<segment_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}
      
      ncell_range(segment_type & d) : cont_(d.container(dimension_tag<dim>())) {}
      
      
      ncell_range & operator=(ncell_proxy<segment_type> p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
        return *this;
      }
      
      iterator begin() const { return iterator(cont_->begin()); }
      iterator end()   const { return iterator(cont_->end()); }

      std::size_t size() const { return cont_->size(); }

      element_type & operator[](std::size_t index)
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return *((*cont_)[index]); 
      }

      element_type const & operator[](std::size_t index) const 
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return *((*cont_)[index]); 
      }

      template <typename DomainType, long dim2, bool b2>
      friend class const_ncell_range;

    private:
      container_type * cont_;
  };
  
  /** @brief Main function for range retrieval. Specialization for iteration over n-cells of a segment */
  template <long dim, typename DomainConfig>
  ncell_range<segment_t<DomainConfig>, dim>
  ncells(segment_t<DomainConfig> & d)
  {
    return ncell_range< segment_t<DomainConfig>, dim>(d);
  }
  
  /** @brief Main function for range retrieval. Specialization for iteration over n-cells of a segment. Returns only a proxy that must be assigned to a range object.
   *
   * Allows to omit the topological dimension if this is clear from the range type, e.g.
   *  VertexRange vertices = ncells(segment);
   */
  template <typename DomainConfig>
  ncell_proxy< segment_t<DomainConfig> >
  ncells(segment_t<DomainConfig> & d)
  {
    return ncell_proxy< segment_t<DomainConfig> >(d);
  }


  //
  // const version
  //
  
  
  /** @brief Main const-range class. Specialization for use with const iteration or const access to k-cells of a segment */
  template <typename config_type, long dim>
  class const_ncell_range < segment_t<config_type>, dim, false >
  {
      typedef segment_t<config_type>                        segment_type;
      typedef element_t< config_type,
                       typename topology::bndcells<typename config_type::cell_tag, dim>::tag
                     >                                                         element_type;

      typedef element_t< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<segment_type, dim>::type      container_type;
    
    public: 
      //typedef typename container_type::const_iterator   iterator;
      typedef on_segment_iterator< typename container_type::const_iterator, const element_type >   iterator;
      
      const_ncell_range() : cont_(NULL) {};
      
      const_ncell_range(const_ncell_proxy< segment_type > const & p) : cont_(p.get().container(dimension_tag<dim>())) {}

      const_ncell_range(ncell_proxy< segment_type > const & p) : cont_(p.get().container(dimension_tag<dim>())) {}

      const_ncell_range(segment_type const & d) : cont_(d.container(dimension_tag<dim>())) {}

      const_ncell_range(ncell_range<segment_type, dim > const & other) : cont_(other.cont_) {}


      const_ncell_range & operator=(const_ncell_proxy< segment_type > const & p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
        return *this;
      }
      
      iterator begin() const { return iterator(cont_->begin()); }
      iterator end()   const { return iterator(cont_->end()); }
      
      std::size_t size() const { return cont_->size(); }
      
      element_type & operator[](std::size_t index)
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return *((*cont_)[index]); 
      }

      element_type const & operator[](std::size_t index) const 
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return *((*cont_)[index]); 
      }
      
    private:
      const container_type * cont_;
  };
  
  /** @brief Main function for range retrieval. Specialization for iteration over n-cells of a segment */
  template <long dim, typename DomainConfig>
  const_ncell_range< segment_t<DomainConfig>, dim>
  ncells(segment_t<DomainConfig> const & d)
  {
    return const_ncell_range< segment_t<DomainConfig>, dim>(d);
  }
  
  /** @brief Main function for const-range retrieval. Specialization for iteration over n-cells of a segment. Returns only a proxy that must be assigned to a range object.
   *
   * Allows to omit the topological dimension if this is clear from the range type, e.g.
   *  VertexRange vertices = ncells(segment);
   */
  template <typename DomainConfig>
  const_ncell_proxy< segment_t<DomainConfig> >
  ncells(segment_t<DomainConfig> const & d)
  {
    return const_ncell_proxy< segment_t<DomainConfig> >(d);
  }
  
  
} //namespace
#endif

