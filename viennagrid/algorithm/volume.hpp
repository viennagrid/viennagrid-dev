#ifndef VIENNAGRID_ALGORITHM_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_VOLUME_HPP

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
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

/** @file volume.hpp
    @brief Computes the volume of different cell types as well as domains and segments
*/


namespace viennagrid
{
  namespace detail
  {
    
    /** @brief Computes the volume of topologically zero-dimensional elements (vertices). Degenerate case, returns 1 */
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::point_tag)
    {
      return typename ElementType::config_type::numeric_type(1);
    }
    
    /** @brief Computes the volume of topologically one-dimensional elements (lines, 1-simplex).*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::simplex_tag<1>)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].point();
      PointType const & p1 = ncells<0>(cell)[1].point();
      
      return norm(p0 - p1);
    }
    
    /** @brief Computes the volume of topologically one-dimensional elements (lines, 1-hypercube).*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::hypercube_tag<1>)
    {
      return volume_impl(cell, viennagrid::simplex_tag<1>());
    }

    //topologically two-dimensional elements
    /** @brief Computes the two-dimensional volume of a triangle.*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].point();
      PointType const & p1 = ncells<0>(cell)[1].point();
      PointType const & p2 = ncells<0>(cell)[2].point();
      
      return spanned_volume(p0, p1, p2);
    }

    /** @brief Computes the two-dimensional volume of a quadrilateral.*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].point();
      PointType const & p1 = ncells<0>(cell)[1].point();
      PointType const & p2 = ncells<0>(cell)[2].point();
      PointType const & p3 = ncells<0>(cell)[3].point();
      
      return spanned_volume(p0, p1, p3) + spanned_volume(p1, p2, p3); //sum up the two triangular parts
    }
    
    
    /** @brief Computes the two-dimensional volume of a polygon.*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::polygon_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename ElementType::config_type::numeric_type NumericType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type       VertexOnCellIterator;
      
      
      VertexOnCellContainer range = viennagrid::ncells<0>( cell );
      if (range.size() < 3) return 0;
      VertexOnCellIterator it1 = range.begin();
      VertexOnCellIterator it2 = it1; ++it2;
      
      PointType origin = it1->point();
      
      NumericType volume = 0;
      
      for (; it2 != range.end(); ++it1, ++it2)
          volume += signed_spanned_volume(origin, it1->point(), it2->point());
      

      it1 = range.end(); --it1;
      volume += signed_spanned_volume( origin, it1->point(), range.begin()->point());
      
      return std::abs(volume);
    }
    

    //topologically three-dimensional elements
    /** @brief Computes the three-dimensional volume of a tetrahedron.*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].point();
      PointType const & p1 = ncells<0>(cell)[1].point();
      PointType const & p2 = ncells<0>(cell)[2].point();
      PointType const & p3 = ncells<0>(cell)[3].point();
      
      return spanned_volume(p0, p1, p2, p3);
    }


    /** @brief Computes the three-dimensional volume of a hexahedron.*/
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::hexahedron_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].point();
      PointType const & p1 = ncells<0>(cell)[1].point();
      PointType const & p2 = ncells<0>(cell)[2].point();
      PointType const & p3 = ncells<0>(cell)[3].point();
      PointType const & p4 = ncells<0>(cell)[4].point();
      PointType const & p5 = ncells<0>(cell)[5].point();
      PointType const & p6 = ncells<0>(cell)[6].point();
      PointType const & p7 = ncells<0>(cell)[7].point();
      
      //decompose hexahedron into six tetrahedra
      return spanned_volume(p0, p1, p3, p4)
             + spanned_volume(p4, p1, p3, p7)
             + spanned_volume(p4, p1, p7, p5)
             + spanned_volume(p1, p2, p3, p7)
             + spanned_volume(p1, p2, p7, p5)
             + spanned_volume(p5, p2, p7, p6);
    }
    

    //
    /** @brief Dispatched function for computing the volume of a domain or segment.*/
    template <typename ContainerType>
    typename ContainerType::config_type::numeric_type
    volume_domainsegment(ContainerType const & d)
    {
      typedef ContainerType                                      DomainType;
      typedef typename ContainerType::config_type::cell_tag      CellTag;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type  CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
      
      typename ContainerType::config_type::numeric_type new_volume = 0;
      CellContainer new_cells = viennagrid::ncells<CellTag::dim>(d);
      for (CellIterator new_cit = new_cells.begin();
                        new_cit != new_cells.end();
                      ++new_cit)
      {
        new_volume += volume(*new_cit);
      }
      return new_volume;
    }
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the n-dimensional volume of a n-cell */
  template <typename ElementType>
  typename ElementType::config_type::numeric_type
  volume(ElementType const & cell)
  {
    return detail::volume_impl(cell, typename ElementType::tag());
  }
  
  //special case: domain
  /** @brief Returns the N-dimensional volume of a domain, where the cell type has topological dimension N. */
  template <typename ConfigType>
  typename ConfigType::numeric_type
  volume(domain_t<ConfigType> const & d)
  {
    return detail::volume_domainsegment(d);
  }    
  
  //special case: segment
  /** @brief Returns the N-dimensional volume of a segment, where the cell type has topological dimension N. */
  template <typename ConfigType>
  typename ConfigType::numeric_type
  volume(segment_t<ConfigType> const & d)
  {
    return detail::volume_domainsegment(d);
  }
  
} //namespace viennagrid
#endif
