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
    template <typename CellTypeOrTag, typename ElementTypeOrTag, typename DomainType>
    typename viennagrid::result_of::coord_type< typename viennagrid::result_of::point_type<DomainType>::type >::type
    surface_domainsegment(DomainType const & domain)
    {
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTag>::type ElementTag;
      
      typedef typename viennagrid::result_of::const_element_range<DomainType, typename ElementTag::facet_tag>::type  FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type         FacetIterator;
      
      typename viennagrid::result_of::coord_type< typename viennagrid::result_of::point_type<DomainType>::type >::type result = 0;
      
      FacetRange facets = viennagrid::elements<typename ElementTag::facet_tag>(domain);
      for (FacetIterator fit = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
          //std::cout << *fit << " boundary=" << is_boundary<CellTypeOrTag>(*fit, d) << std::endl;
        if (is_boundary<CellTypeOrTag>(*fit, domain))
          result += viennagrid::volume(domain, *fit);
      }
      return result;
    }
  } //namespace detail
  
  
  
  //
  // The public interface functions
  //
  /** @brief Returns the surface of a n-cell */
  template <typename DomainType, typename ElementType>
  typename viennagrid::result_of::coord_type< typename viennagrid::result_of::point_type<DomainType>::type >::type
  surface(const DomainType & domain, ElementType const & element)
  {
    //typedef typename ElementType::config_type                   ConfigType;
    typedef typename viennagrid::result_of::const_facet_range<ElementType>::type   ElementBoundaryRange;
    typedef typename viennagrid::result_of::iterator<ElementBoundaryRange>::type       ElementBoundaryIterator;
    
    //typedef typename ElementType::config_type::numeric_type value_type;
    typedef typename viennagrid::result_of::coord_type< typename viennagrid::result_of::point_type<DomainType>::type >::type value_type;
    
    value_type result = 0;
    
    ElementBoundaryRange boundary = viennagrid::facets(element);
    for (ElementBoundaryIterator ebit = boundary.begin();
                                  ebit != boundary.end();
                                ++ebit)
    {
      result += viennagrid::volume(domain, *ebit);
    }
    
    return result;
  }
  
  
  
  //special case: domain
  /** @brief Returns the surface of a domain */
  template <typename CellTypeOrTag, typename ElementTypeOrTag, typename DomainType>
  typename viennagrid::result_of::coord_type< typename viennagrid::result_of::point_type<DomainType>::type >::type
  surface(DomainType const & d)
  {
    return detail::surface_domainsegment<CellTypeOrTag, ElementTypeOrTag>(d);
  }    
  
  template <typename CellTypeOrTag, typename DomainType>
  typename viennagrid::result_of::coord_type< typename viennagrid::result_of::point_type<DomainType>::type >::type
  surface(DomainType const & d)
  {
    return detail::surface_domainsegment<CellTypeOrTag, CellTypeOrTag>(d);
  }  
  
  
  
  
  
    template<typename boundary_element_type_or_tag, typename domain_type, typename element_type>
    typename viennagrid::result_of::handle<domain_type, boundary_element_type_or_tag>::type smallest_boundary_volume( domain_type & domain, element_type & element )
    {
        typedef typename viennagrid::result_of::coord_type<domain_type>::type numeric_type;
        typedef typename viennagrid::result_of::handle<element_type, boundary_element_type_or_tag>::type boundary_element_handle;
        typedef typename viennagrid::result_of::element_range<element_type, boundary_element_type_or_tag>::type boundary_element_range_type;
        typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type boundary_element_range_iterator;
        
        boundary_element_range_type boundary_elements = viennagrid::elements( element );
        boundary_element_range_iterator it = boundary_elements.begin();
        
        boundary_element_handle best_handle = it.handle();
        numeric_type best_volume = viennagrid::volume( domain, *it );
        
        for (; it != boundary_elements.end(); ++it)
        {
            numeric_type volume = viennagrid::volume( domain, *it );
            if (volume < best_volume)
            {
                best_handle = it.handle();
                best_volume = volume;
            }
        }
        
        return best_handle;
    }
    
    template<typename boundary_element_type_or_tag, typename domain_type, typename element_type>
    typename viennagrid::result_of::const_handle<domain_type, boundary_element_type_or_tag>::type smallest_boundary_volume( domain_type const & domain, element_type const & element )
    {
        typedef typename viennagrid::result_of::coord_type<domain_type>::type numeric_type;
        typedef typename viennagrid::result_of::const_handle<element_type, boundary_element_type_or_tag>::type boundary_element_handle;
        typedef typename viennagrid::result_of::const_element_range<element_type, boundary_element_type_or_tag>::type boundary_element_range_type;
        typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type boundary_element_range_iterator;
        
        boundary_element_range_type boundary_elements = viennagrid::elements( element );
        boundary_element_range_iterator it = boundary_elements.begin();
        
        boundary_element_handle best_handle = it.handle();
        numeric_type best_volume = viennagrid::volume( domain, *it );
        
        for (; it != boundary_elements.end(); ++it)
        {
            numeric_type volume = viennagrid::volume( domain, *it );
            if (volume < best_volume)
            {
                best_handle = it.handle();
                best_volume = volume;
            }
        }
        
        return best_handle;
    }
    
    template<typename boundary_element_type_or_tag, typename domain_type, typename element_type>
    typename viennagrid::result_of::handle<domain_type, boundary_element_type_or_tag>::type largest_boundary_volume( domain_type & domain, element_type & element )
    {
        typedef typename viennagrid::result_of::coord_type<domain_type>::type numeric_type;
        typedef typename viennagrid::result_of::handle<element_type, boundary_element_type_or_tag>::type boundary_element_handle;
        typedef typename viennagrid::result_of::element_range<element_type, boundary_element_type_or_tag>::type boundary_element_range_type;
        typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type boundary_element_range_iterator;
        
        boundary_element_range_type boundary_elements = viennagrid::elements( element );
        boundary_element_range_iterator it = boundary_elements.begin();
        
        boundary_element_handle best_handle = it.handle();
        numeric_type best_volume = viennagrid::volume( domain, *it );
        
        for (; it != boundary_elements.end(); ++it)
        {
            numeric_type volume = viennagrid::volume( domain, *it );
            if (volume > best_volume)
            {
                best_handle = it.handle();
                best_volume = volume;
            }
        }
        
        return best_handle;
    }
  
    template<typename boundary_element_type_or_tag, typename domain_type, typename element_type>
    typename viennagrid::result_of::const_handle<domain_type, boundary_element_type_or_tag>::type largest_boundary_volume( domain_type const & domain, element_type const & element )
    {
        typedef typename viennagrid::result_of::coord_type<domain_type>::type numeric_type;
        typedef typename viennagrid::result_of::const_handle<element_type, boundary_element_type_or_tag>::type boundary_element_handle;
        typedef typename viennagrid::result_of::const_element_range<element_type, boundary_element_type_or_tag>::type boundary_element_range_type;
        typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type boundary_element_range_iterator;
        
        boundary_element_range_type boundary_elements = viennagrid::elements( element );
        boundary_element_range_iterator it = boundary_elements.begin();
        
        boundary_element_handle best_handle = it.handle();
        numeric_type best_volume = viennagrid::volume( domain, *it );
        
        for (; it != boundary_elements.end(); ++it)
        {
            numeric_type volume = viennagrid::volume( domain, *it );
            if (volume > best_volume)
            {
                best_handle = it.handle();
                best_volume = volume;
            }
        }
        
        return best_handle;
    }

    
} //namespace viennagrid
#endif
