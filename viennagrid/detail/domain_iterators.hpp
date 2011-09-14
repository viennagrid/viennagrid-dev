#ifndef VIENNAGRID_DOMAIN_ITERATORS_HPP
#define VIENNAGRID_DOMAIN_ITERATORS_HPP

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

/** @file domain_iterators.hpp
    @brief Provides the iterators and ranges for the domain
*/

namespace viennagrid
{


  // iterator over all subcells (not vertex or cell type) on the domain
  // transforms the map-like storage on the domain
  /** @brief Iterator over non-vertices and non-cells on a domain */
  template <typename ElementType, typename MapIterator>
  class domain_subcell_iterator : public std::iterator < std::forward_iterator_tag, ElementType >
  {
    public:
      domain_subcell_iterator(MapIterator const & mit) : mit_(mit) {}

      ElementType & operator*() const { return mit_->second; }
      ElementType* operator->() const { return &(mit_->second); }

      domain_subcell_iterator & operator++() { ++mit_; return *this; }
      domain_subcell_iterator & operator++(int) { domain_subcell_iterator tmp = *this; ++*this; return tmp; }

      bool operator==(const domain_subcell_iterator & i) const { return mit_ == i.mit_; }
      bool operator!=(const domain_subcell_iterator & i) const { return mit_ != i.mit_; }

    private:
      MapIterator mit_;
  };

  

  ///////////////  Iterator construction for iterations over whole domain ////////////////
  //generic case:
  /** @brief Helper for iterator construction for iteration over the whole domain */
  template <typename Config,
            long dim,
            long cell_level = Config::cell_tag::dim>
  struct domain_iterators
  {
    typedef domain_t<Config>                        domain_type;
    typedef element_t<Config,
                    typename Config::cell_tag>    cell_type;
    typedef element_t< Config,
                       typename topology::bndcells<typename Config::cell_tag, dim>::tag
                     >                                                         element_type;
                    
    typedef typename result_of::element_container<domain_type, dim>::type      container_type;
    
    typedef domain_subcell_iterator<element_type,
                                    typename container_type::iterator>         iterator;
    typedef domain_subcell_iterator<const element_type,
                                    typename container_type::const_iterator>   const_iterator;
  };

  //special case: vertices:
  /** @brief Helper for iterator construction for vertex iteration over the whole domain */
  template <typename Config, long cell_level>
  struct domain_iterators< Config, 0, cell_level>
  {
    typedef domain_t<Config>                        domain_type;
    typedef element_t<Config,
                      typename Config::cell_tag>    cell_type;
                    
    typedef typename result_of::element_container<domain_type, 0>::type   container_type;
    
    typedef typename container_type::iterator          iterator;
    typedef typename container_type::const_iterator    const_iterator;
  };

  //special case: cells:
  /** @brief Helper for iterator construction for cell iteration over the whole domain */
  template <typename Config, long cell_level>
  struct domain_iterators< Config, cell_level, cell_level>
  {
    typedef domain_t<Config>                        domain_type;
    typedef element_t<Config,
                    typename Config::cell_tag>    cell_type;
                    
    typedef typename result_of::element_container<domain_type,
                                                  Config::cell_tag::dim>::type   container_type;
    
    typedef typename container_type::iterator          iterator;
    typedef typename container_type::const_iterator    const_iterator;
  };

  
  
  /** @brief A guard that ensures that operator[] access is possible. Throws a verbose compile time error if access is not possible. */
  template <typename ContainerType>
  struct assert_bracket_operator_access
  {
    // The user tried to do something like ncells<1>(domain)[0] to access the edge with ID 0 in the domain.
    // However, edges are configured not to carry IDs at all, therefore throw an error.
    typedef typename ContainerType::ERROR_BRACKET_OPERATOR_ACCESS_IMPOSSIBLE_AT_THIS_DOMAIN_LEVEL___USE_ITERATORS   type;
  };

  /** @brief Specialization: Allow operator[] if the underlying container is a std::vector<> */
  template <typename ElementType>
  struct assert_bracket_operator_access< std::vector<ElementType> >
  {
    typedef void   type;
  };

  /** @brief Specialization: Allow operator[] if the underlying container is a std::deque<> */
  template <typename ElementType>
  struct assert_bracket_operator_access< std::deque<ElementType> >
  {
    typedef void   type;
  };
  
  
  // interface function for container creation,
  // non-const:
  //container for iteration over a STL vector
  /** @brief Main range class. Specialization for use with iteration or access to k-cells on the domain */
  template <typename config_type, long dim>
  class ncell_range < domain_t<config_type>, dim, false >
  {
      typedef domain_t<config_type>                        domain_type;
      typedef element_t< config_type,
                       typename topology::bndcells<typename config_type::cell_tag, dim>::tag
                     >                                                         element_type;
                     
      typedef element_t< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<domain_type, dim>::type    container_type;
    
    public: 
      //typedef typename container_type::iterator   iterator;
      typedef typename domain_iterators<config_type, dim>::iterator   iterator;
      
      ncell_range() : cont_(NULL) {};
      
      ncell_range(ncell_proxy<domain_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}
      
      ncell_range(domain_type & d) : cont_(d.container(dimension_tag<dim>())) {}

      ncell_range & operator=(ncell_proxy<domain_type> p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
        return *this;
      }
      
      iterator begin() const { return cont_->begin(); }
      iterator end()   const { return cont_->end(); }
      
      std::size_t size() const { return cont_->size(); }

      element_type & operator[](std::size_t index)
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return (*cont_)[index]; 
      }

      element_type const & operator[](std::size_t index) const 
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return (*cont_)[index]; 
      }
      
      template <typename DomainType, long dim2, bool b2>
      friend class const_ncell_range;

    private:
      container_type * cont_;
  };
  
  /** @brief Main function for range retrieval. Specialization for iteration over k-cells on the domain */
  template <long dim, typename DomainConfig>
  ncell_range<domain_t<DomainConfig>, dim>
  ncells(domain_t<DomainConfig> & d)
  {
    return ncell_range<domain_t<DomainConfig>, dim>(d);
  }
  
  /** @brief Main function for range retrieval. Specialization for iteration over k-cells on the domain. Returns only a proxy that must be assigned to a range object.
   *
   * Allows to omit the topological dimension if this is clear from the range type, e.g.
   *  VertexRange vertices = ncells(domain);
   */
  template <typename DomainConfig>
  ncell_proxy< domain_t<DomainConfig> >
  ncells(domain_t<DomainConfig> & d)
  {
    return ncell_proxy< domain_t<DomainConfig> >(d);
  }


  //
  //const container:  
  //
  /** @brief Main const-range class. Specialization for use with const-iteration or access to k-cells on the domain */
  template <typename config_type, long dim>
  class const_ncell_range < domain_t<config_type>, dim, false >
  {
      typedef domain_t<config_type>                        domain_type;
      typedef element_t< config_type,
                       typename topology::bndcells<typename config_type::cell_tag, dim>::tag
                     >                                                         element_type;

      typedef element_t< config_type,
                       typename config_type::cell_tag
                     >                                                         cell_type;
                     
      //typedef std::vector< element_type >     container_type;
      typedef typename result_of::element_container<domain_type, dim>::type      container_type;
    
    public: 
      //typedef typename container_type::const_iterator   iterator;
      typedef typename domain_iterators<config_type, dim>::const_iterator   iterator;
      
      const_ncell_range() : cont_(NULL) {};
      
      const_ncell_range(const_ncell_proxy<domain_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}

      const_ncell_range(ncell_proxy<domain_type> const & p) : cont_(p.get().container(dimension_tag<dim>())) {}

      const_ncell_range(domain_type const & d) : cont_(d.container(dimension_tag<dim>())) {}

      const_ncell_range(ncell_range<domain_type, dim > const & other) : cont_(other.cont_) {}

      const_ncell_range & operator=(const_ncell_proxy<domain_type> const & p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
        return *this;
      }
      
      const_ncell_range & operator=(ncell_proxy<domain_type> p)
      { 
        cont_ = p.get().container(dimension_tag<dim>());
        return *this;
      }
      
      const_ncell_range & operator=(ncell_range<domain_type, dim > const & other)
      { 
        cont_ = other.cont_;
        return *this;
      }
      
      iterator begin() const { return cont_->begin(); }
      iterator end()   const { return cont_->end(); }
      
      element_type const & operator[](std::size_t index) const 
      {
        typedef typename assert_bracket_operator_access<container_type>::type  asserted_type;
        assert(index < size());
        return (*cont_)[index]; 
      }
      
      size_t size() const { return cont_->size(); }
      
    private:
      const container_type * cont_;
  };
  
  /** @brief Main function for const range retrieval. Specialization for iteration over k-cells on the domain */
  template <long dim, typename DomainConfig>
  const_ncell_range< domain_t<DomainConfig>, dim>
  ncells(domain_t<DomainConfig> const & d)
  {
    return const_ncell_range< domain_t<DomainConfig>, dim>(d);
  }
  
  /** @brief Main function for const range retrieval. Specialization for iteration over k-cells on the domain. Returns only a proxy that must be assigned to a range object.
   *
   * Allows to omit the topological dimension if this is clear from the range type, e.g.
   *  VertexRange vertices = ncells(domain);
   */
  template <typename DomainConfig>
  const_ncell_proxy< domain_t<DomainConfig> >
  ncells(domain_t<DomainConfig> const & d)
  {
    return const_ncell_proxy< domain_t<DomainConfig> >(d);
  }

  
  //metafunction for return type:
  namespace result_of
  {
    /** @brief Specialization of the iterator metafunction for returning the correct iterator from a range */
    template <typename T, long dim, bool is_coboundary>
    struct iterator<viennagrid::ncell_range<T, dim, is_coboundary>, 0>
    {
      typedef typename viennagrid::ncell_range<T, dim, is_coboundary>::iterator     type;
    };
    
    /** @brief Specialization of the iterator metafunction for returning the correct iterator from a const range */
    template <typename T, long dim, bool is_coboundary>
    struct iterator<viennagrid::const_ncell_range<T, dim, is_coboundary>, 0>
    {
      typedef typename viennagrid::const_ncell_range<T, dim, is_coboundary>::iterator     type;
    };
    
    /** @brief Meta function for returning a range of n-cells.
     * 
     * @tparam T       The enclosing complex (domain, segment or a n-cell of higher dimension)
     * @tparam dim     The topological dimension of the n-cells in the range */
    template <typename T, 
              long dim>  //topological level
    struct ncell_range
    {
      typedef viennagrid::ncell_range<T, dim>       type;
    };

    /** @brief Meta function for returning a const-range of n-cells.
     * 
     * @tparam T       The enclosing complex (domain, segment or a n-cell of higher dimension)
     * @tparam dim     The topological dimension of the n-cells in the range */
    template <typename T, 
              long dim>  //topological level
    struct const_ncell_range
    {
      typedef viennagrid::const_ncell_range<T, dim>       type;
    };
    
  }
  
} //namespace
#endif

