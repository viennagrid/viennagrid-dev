#ifndef VIENNAGRID_ALGORITHM_DISTANCE_HPP
#define VIENNAGRID_ALGORITHM_DISTANCE_HPP

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
#include <limits>

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/closest_points.hpp"

/** @file distance.hpp
    @brief Computes the (Cartesian) distance between different elements.
*/


namespace viennagrid
{
  namespace detail
  {
    
    //
    // Distance between points
    //
    
    // Distance between two points:
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                  point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(p1 - p2);
    }

    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename CoordType, typename CoordinateSystem, typename BoundaryCellTypelist, typename IDType>
    CoordType
    distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                  point_t<CoordType, CoordinateSystem> const & p1,
                  element_t<viennagrid::vertex_tag, BoundaryCellTypelist, IDType> const & v2)
    {
      return norm_2(p1 - viennagrid::point(domain, v2));
    }

    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename CoordType, typename CoordinateSystem, typename BoundaryCellTypelist, typename IDType>
    CoordType
    distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                  element_t<viennagrid::vertex_tag, BoundaryCellTypelist, IDType> const & v1,
                  point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(viennagrid::point(domain, v1) - p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename BoundaryCellTypelist, typename IDType>
    typename result_of::coord_type<DomainPointType>::type
    distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                  element_t<viennagrid::vertex_tag, BoundaryCellTypelist, IDType> const & v1,
                  element_t<viennagrid::vertex_tag, BoundaryCellTypelist, IDType> const & v2)
    {
      return norm_2(viennagrid::point(domain, v1) - viennagrid::point(domain, v2));
    }

    
    
    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename ElementType1, typename ElementType2>
    typename result_of::coord_type<DomainPointType>::type
    distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                  ElementType1 const & el1,
                  ElementType2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type   PointType;
      typedef DomainPointType PointType;
      
      std::pair<PointType, PointType> points = closest_points(domain, el1, el2);
      
      return norm_2(points.first - points.second);
    }
    
    
    
    ////////////////////////////////// boundary distance //////////////////////////////////////

    // Distance between two points:
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    boundary_distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                           point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(p1 - p2);
    }

    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename CoordType, typename CoordinateSystem, typename BoundaryCellTypelist, typename IDType>
    CoordType
    boundary_distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                           point_t<CoordType, CoordinateSystem> const & p1,
                           element_t<vertex_tag, BoundaryCellTypelist, IDType> const & v2)
    {
      return norm_2(p1 - viennagrid::point(domain, v2));
    }

    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename CoordType, typename CoordinateSystem, typename BoundaryCellTypelist, typename IDType>
    CoordType
    boundary_distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                           element_t<vertex_tag, BoundaryCellTypelist, IDType> const & v1,
                           point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(viennagrid::point(domain, v1) - p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename BoundaryCellTypelist, typename IDType>
    typename result_of::coord_type<DomainPointType>::type
    boundary_distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                           element_t<vertex_tag, BoundaryCellTypelist, IDType> const & v1,
                           element_t<vertex_tag, BoundaryCellTypelist, IDType> const & v2)
    {
      return norm_2(viennagrid::point(domain, v1) - viennagrid::point(domain, v2));
    }

    
    
    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
              typename ElementType1, typename ElementType2>
    typename result_of::coord_type<DomainPointType>::type
    boundary_distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                           ElementType1 const & el1,
                           ElementType2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type      PointType;
      typedef DomainPointType PointType;
      
      std::pair<PointType, PointType> points = closest_points_on_boundary(domain, el1, el2);
      
      return norm_2(points.first - points.second);
    }
    
    template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
    typename result_of::coord_type<DomainPointType>::type
    boundary_distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                           ElementType const & element)
    {
      //typedef typename result_of::point<ElementType1>::type      PointType;
      typedef DomainPointType PointType;
      
      std::pair<PointType, PointType> points = closest_points_on_boundary<ElementTypeOrTag>(domain, element);
      
      return norm_2(points.first - points.second);
    }
    
    template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
    typename result_of::coord_type<DomainPointType>::type
    boundary_distance_impl(ElementType const & element,
                           domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain)
    {
      //typedef typename result_of::point<ElementType1>::type      PointType;
      typedef DomainPointType PointType;
      
      std::pair<PointType, PointType> points = closest_points_on_boundary<ElementTypeOrTag>(domain, element);
      
      return norm_2(points.first - points.second);
    }
    
    
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the distance between n-cells, segments and/or domains */
  template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
            typename ElementType1, typename ElementType2>
  typename result_of::coord_type<DomainPointType>::type
  distance(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
           ElementType1 const & el1,
           ElementType2 const & el2)
  {
    return detail::distance_impl(domain, el1, el2);
  }
  
  
  /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
  template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
            typename ElementType1, typename ElementType2>
  typename result_of::coord_type<DomainPointType>::type
  boundary_distance(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                    ElementType1 const & el1, ElementType2 const & el2)
  {
    return detail::boundary_distance_impl(domain, el1, el2);
  }
  
  /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
  template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
  typename result_of::coord_type<DomainPointType>::type
  boundary_distance(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
                    ElementType const & element)
  {
    return detail::boundary_distance_impl<ElementTypeOrTag>(domain, element);
  }
  
  /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
  template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
  typename result_of::coord_type<DomainPointType>::type
  boundary_distance(ElementType const & element,
                    domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain)
  {
    return detail::boundary_distance_impl<ElementTypeOrTag>(domain, element);
  }
  
} //namespace viennagrid
#endif
