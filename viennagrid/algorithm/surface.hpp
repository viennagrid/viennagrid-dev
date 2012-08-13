#ifndef VIENNAGRID_ALGORITHM_SURFACE_HPP
#define VIENNAGRID_ALGORITHM_SURFACE_HPP

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

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/boundary.hpp"

/** @file surface.hpp
    @brief Computes the surface of different cell types as well as domains and segments
*/


namespace viennagrid
{
  namespace detail
  {
    /** @brief Implementation for the calculation of the surface of a domain or segment */
    template <typename ContainerType>
    typename ContainerType::config_type::numeric_type
    surface_domainsegment(ContainerType const & d)
    {
      typedef ContainerType                                      DomainType;
      typedef typename ContainerType::config_type::cell_tag      CellTag;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim-1>::type  FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type         FacetIterator;
      
      typename ContainerType::config_type::numeric_type result = 0;
      
      FacetRange facets = viennagrid::ncells(d);
      for (FacetIterator fit = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
        if (is_boundary(*fit, d))
          result += viennagrid::volume(*fit);
      }
      return result;
    }
  } //namespace detail
  
  
  
  //
  // The public interface functions
  //
  /** @brief Returns the surface of a n-cell */
  template <typename ElementType>
  typename ElementType::config_type::numeric_type
  surface(ElementType const & element)
  {
    typedef typename ElementType::config_type                   ConfigType;
    typedef typename viennagrid::result_of::const_ncell_range<ElementType, ElementType::tag::dim-1>::type   ElementBoundaryRange;
    typedef typename viennagrid::result_of::iterator<ElementBoundaryRange>::type       ElementBoundaryIterator;
    
    typedef typename ElementType::config_type::numeric_type value_type;
    
    value_type result = 0;
    
    ElementBoundaryRange boundary = viennagrid::ncells(element);
    for (ElementBoundaryIterator ebit = boundary.begin();
                                  ebit != boundary.end();
                                ++ebit)
    {
      result += viennagrid::volume(*ebit);
    }
    
    return result;
  }
  
  //special case: domain
  /** @brief Returns the surface of a domain */
  template <typename ConfigType>
  typename ConfigType::numeric_type
  surface(domain_t<ConfigType> const & d)
  {
    return detail::surface_domainsegment(d);
  }    
  
  //special case: segment
  /** @brief Returns the surface of a segment */
  template <typename ConfigType>
  typename ConfigType::numeric_type
  surface(segment_t<ConfigType> const & d)
  {
    return detail::surface_domainsegment(d);
  }
    
} //namespace viennagrid
#endif
