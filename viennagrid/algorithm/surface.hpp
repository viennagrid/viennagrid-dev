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

#include "viennagrid/forwards.hpp"
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
    template <typename CellTypeOrTag, typename ElementTypeOrTag, typename GeometricContainerType>
    typename viennagrid::traits::value_type< typename viennagrid::result_of::point_type<GeometricContainerType>::type >::type
    surface_domainsegment(GeometricContainerType const & d)
    {
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTag>::type ElementTag;
      
      typedef typename viennagrid::result_of::const_element_range<GeometricContainerType, typename ElementTag::facet_tag>::type  FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type         FacetIterator;
      
      typename viennagrid::traits::value_type< typename viennagrid::result_of::point_type<GeometricContainerType>::type >::type result = 0;
      
      FacetRange facets = viennagrid::elements<typename ElementTag::facet_tag>(d);
      for (FacetIterator fit = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
          //std::cout << *fit << " boundary=" << is_boundary<CellTypeOrTag>(*fit, d) << std::endl;
        if (is_boundary<CellTypeOrTag>(*fit, d))
          result += viennagrid::volume(*fit, d);
      }
      return result;
    }
  } //namespace detail
  
  
  
  //
  // The public interface functions
  //
  /** @brief Returns the surface of a n-cell */
  template <typename ElementType, typename GeometricContainerType>
  typename viennagrid::traits::value_type< typename viennagrid::result_of::point_type<GeometricContainerType>::type >::type
  surface(ElementType const & element, const GeometricContainerType & geometric_container)
  {
    //typedef typename ElementType::config_type                   ConfigType;
    typedef typename viennagrid::result_of::const_ncell_range<ElementType, ElementType::tag::dim-1>::type   ElementBoundaryRange;
    typedef typename viennagrid::result_of::iterator<ElementBoundaryRange>::type       ElementBoundaryIterator;
    
    //typedef typename ElementType::config_type::numeric_type value_type;
    typedef typename viennagrid::traits::value_type< typename viennagrid::result_of::point_type<GeometricContainerType>::type >::type value_type;
    
    value_type result = 0;
    
    ElementBoundaryRange boundary = viennagrid::ncells<ElementType::tag::dim-1>(element);
    for (ElementBoundaryIterator ebit = boundary.begin();
                                  ebit != boundary.end();
                                ++ebit)
    {
      result += viennagrid::volume(*ebit, geometric_container);
    }
    
    return result;
  }
  
  
  
  //special case: domain
  /** @brief Returns the surface of a domain */
  template <typename CellTypeOrTag, typename ElementTypeOrTag, typename GeometricContainerType>
  typename viennagrid::traits::value_type< typename viennagrid::result_of::point_type<GeometricContainerType>::type >::type
  surface(GeometricContainerType const & d)
  {
    return detail::surface_domainsegment<CellTypeOrTag, ElementTypeOrTag>(d);
  }    
  
  template <typename CellTypeOrTag, typename GeometricContainerType>
  typename viennagrid::traits::value_type< typename viennagrid::result_of::point_type<GeometricContainerType>::type >::type
  surface(GeometricContainerType const & d)
  {
    return detail::surface_domainsegment<CellTypeOrTag, CellTypeOrTag>(d);
  }  
  

    
} //namespace viennagrid
#endif
