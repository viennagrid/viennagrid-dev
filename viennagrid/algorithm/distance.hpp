#ifndef VIENNAGRID_ALGORITHM_DISTANCE_HPP
#define VIENNAGRID_ALGORITHM_DISTANCE_HPP

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

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "viennagrid/forwards.h"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"

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

    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                  element_t<ConfigType, point_tag> const & v2)
    {
      return norm_2(p1 - v2.point());
    }

    template <typename ConfigType, typename CoordType, typename CoordinateSystem>
    CoordType
    distance_impl(element_t<ConfigType, point_tag> const & v1,
                  point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(v1.point() - p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, point_tag> const & v1,
                  element_t<ConfigType, point_tag> const & v2)
    {
      return norm_2(v1.point() - v2.point());
    }

    
    
    //
    // Distance between point and line segment
    //
    
    // Implementation: Supposed to work for arbitrary dimensions
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    distance_point_line(point_t<CoordType, CoordinateSystem> const & p,
                        point_t<CoordType, CoordinateSystem> const & line_p1,
                        point_t<CoordType, CoordinateSystem> const & line_p2)
    {
      typedef point_t<CoordType, CoordinateSystem>  PointType;

      //compute t such that projection of p onto [line_p1, line_p2] is given by p' = line_p1 + t * (line_p2 - line_p1)
      CoordType t = viennagrid::inner_prod( (p - line_p1), (line_p2 - line_p1) ) / viennagrid::inner_prod(line_p2 - line_p1, line_p2 - line_p1);
      
      //restrict t to line segment, i.e. t \in [0, 1]
      t = std::max<CoordType>(0, std::min<CoordType>(1, t));
      
      PointType p_prime = line_p1 + t * (line_p2 - line_p1);  //closest point to p on line
      
      return distance_impl(p, p_prime);
    }
    
    
    //convenience overload: point and simplex line
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p,
                  element_t<ConfigType, simplex_tag<1> > const & el)
    {
      return distance_point_line(p, 
                                 viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //convenience overload: point and hypercube line
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p,
                  element_t<ConfigType, hypercube_tag<1> > const & el)
    {
      return distance_point_line(p, 
                                 viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //convenience overload: vertex and simplex line
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, point_tag> const & v,
                  element_t<ConfigType, simplex_tag<1> > const & el)
    {
      return distance_point_line(v.point(), 
                                 viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //convenience overload: vertex and hypercube line
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, point_tag> const & v,
                  element_t<ConfigType, hypercube_tag<1> > const & el)
    {
      return distance_point_line(v.point(), 
                                 viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //
    // Distance between two line segments
    //
    
    // Implementation: Supposed to work for arbitrary dimensions
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    distance_line_line(point_t<CoordType, CoordinateSystem> const & v0, point_t<CoordType, CoordinateSystem> const & v1, //endpoints of first line
                       point_t<CoordType, CoordinateSystem> const & w0, point_t<CoordType, CoordinateSystem> const & w1) //endpoints of second line
    {
      typedef point_t<CoordType, CoordinateSystem>  PointType;

      // write V(s) = v0 + s * (v1 - v0), s \in [0,1]
      //       W(t) = w0 + t * (w1 - w0), t \in [0,1]
      
      // compute s and t assuming V(s) and W(t) to be infinite lines:
      // cf. http://www.softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
      PointType dir_v = v1 - v0;  //direction vector for line V(s)
      PointType dir_w = w1 - w0;  //direction vector for line W(t)
      
      CoordType v_in_v = viennagrid::inner_prod(dir_v, dir_v);
      CoordType v_in_w = viennagrid::inner_prod(dir_v, dir_w);
      CoordType w_in_w = viennagrid::inner_prod(dir_w, dir_w);
      
      CoordType denominator = v_in_v * w_in_w - v_in_w * v_in_w;
                              
      if (denominator < 1e-6 * v_in_v * w_in_w) //lines parallel (up to round-off)
      {
        //Shortest distance is given by one of the distances of vertices from other line
        CoordType dist_to_v = std::min<CoordType>(distance_point_line(w0, v0, v1),
                                                  distance_point_line(w1, v0, v1));
        CoordType dist_to_w = std::min<CoordType>(distance_point_line(v0, w0, w1),
                                                  distance_point_line(v1, w0, w1));
        
        return std::min<CoordType>(dist_to_v, dist_to_w);
      }

      //Lines are not parallel: Compute minimizers s, t:
      PointType dir_distance = v0 - w0;  //any vector connecting two points on V and W
      CoordType v_in_dir_distance = viennagrid::inner_prod(dir_v, dir_distance);
      CoordType w_in_dir_distance = viennagrid::inner_prod(dir_w, dir_distance);
      
      CoordType s = (v_in_w * w_in_dir_distance - v_in_v * v_in_dir_distance) / denominator;
      CoordType t = (v_in_v * w_in_dir_distance - v_in_w * v_in_dir_distance) / denominator;
      
      //Check whether minimizing distance is within line segment, i.e. s,t \in [0,1]
      if (s < 0) //shortest distance from point v0:
        return distance_point_line(v0, w0, w1);  //Note: Feel free to inline code here in order to save some operations

      if (s > 1) //shortest distance from point v1:
        return distance_point_line(v1, w0, w1);  //Note: Feel free to inline code here in order to save some operations
        
      if (t < 0) //shortest distance from point w0:
        return distance_point_line(w0, v0, v1);  //Note: Feel free to inline code here in order to save some operations

      if (t > 1) //shortest distance from point w1:
        return distance_point_line(w1, v0, v1);  //Note: Feel free to inline code here in order to save some operations
      
      // compute points on V(s) an W(t) for which distance is smallest:
      PointType min_dist_point_V = v0 + s * dir_v;
      PointType min_dist_point_W = w0 + t * dir_w;
      
      return distance_impl(min_dist_point_V, min_dist_point_W);   //Note: Feel free to inline code here in order to save some operations
    }
    
    
    //convenience overload: simplex line and simplex line
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, simplex_tag<1> > const & line0,
                  element_t<ConfigType, simplex_tag<1> > const & line1)
    {
      return distance_point_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                 viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }
    
    //convenience overload: hypercube line and simplex line
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, hypercube_tag<1> > const & line0,
                  element_t<ConfigType, simplex_tag<1> > const & line1)
    {
      return distance_point_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                 viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }

    //convenience overload: simplex line and hypercube line
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, simplex_tag<1> > const & line0,
                  element_t<ConfigType, hypercube_tag<1> > const & line1)
    {
      return distance_point_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                 viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }
    
    //convenience overload: hypercube line and hypercube line
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, hypercube_tag<1> > const & line0,
                  element_t<ConfigType, hypercube_tag<1> > const & line1)
    {
      return distance_point_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                 viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }
    

    //
    // Distance between point and triangle
    //
    
    // Implementation: Supposed to work for arbitrary dimensions
    // Projects p onto the plane spanned by the triangle, then computes the shortest distance in the plane and uses Pythagoras for the full distance
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    distance_point_triangle(point_t<CoordType, CoordinateSystem> const & p,
                            point_t<CoordType, CoordinateSystem> const & v0,
                            point_t<CoordType, CoordinateSystem> const & v1,
                            point_t<CoordType, CoordinateSystem> const & v2) //endpoints of second line
    {
      typedef point_t<CoordType, CoordinateSystem>  PointType;

      // write T(s) =  v0 + s * (v1 - v0) + t * (v2 - v0), {s,t} \in [0,1], s+t < 1 for the triangle T
      //            =: v0 + s * u0 + t * u1
      //
      // Projection p' of p is given by the solution of the system
      //
      //  <u0, u0> * s + <u1, u0> * t = <u, u0>
      //  <u0, u1> * s + <u1, u1> * t = <u, u1>,
      //
      // where u = p - v0. This is a 2x2-system that is directly inverted.
      //
      // Write
      //
      // A = ( <u0, u0>   <u1, u0> ) = ( a  b )
      //     ( <u0, u1>   <u1, u1> )   ( c  d )
      //
      // and use b = c because of symmetry of the inner product.
      
      PointType u0 = v1 - v0;
      PointType u1 = v2 - v0;
      PointType u  = p  - v0;
      
      CoordType a = viennagrid::inner_prod(u0, u0);
      CoordType b = viennagrid::inner_prod(u0, u1);
      CoordType d = viennagrid::inner_prod(u1, u1);
      CoordType u_in_u0 = viennagrid::inner_prod(u, u0);
      CoordType u_in_u1 = viennagrid::inner_prod(u, u1);
      
      CoordType denominator = a * d - b * b;
                              
      if (denominator < 1e-6 * a * d) //triangle is VERY thin:
      {
        std::cerr << "ViennaGrid: Warning: Strongly degenerated triangle detected: " << std::endl
                  << "vertex 0: " << v0 << std::endl
                  << "vertex 1: " << v1 << std::endl
                  << "vertex 2: " << v2 << std::endl;
      }

      CoordType s = (  d * u_in_u0 - b * u_in_u1) / denominator;
      CoordType t = (- b * u_in_u0 - a * u_in_u1) / denominator;
      
      PointType p_prime = v0 + s * u0 + p * u1;  //projection of p onto triangular plane

      // since s and t are barycentric coordinates, a few computations can be saved by computing distances to closest edges only
      CoordType projected_distance = 0;
      if (s < 0) //p_prime is outside triangle
      {
        if (t < 0)
          projected_distance = distance_impl(p_prime, v0);
        else if (s + t < 1)
          projected_distance = distance_point_line(p_prime, v0, v2);
        else
          projected_distance = distance_impl(p_prime, v2);
      }
      else if (s + t < 1)
      {
        if (t < 0)
          projected_distance = distance_point_line(p_prime, v0, v1);
        else
          projected_distance = 0; //projected point is inside triangle
      }
      else //s + t >= 1 and s > 0
      {
        if (t < 0)
          projected_distance = distance_impl(p_prime, v1);
        else
          projected_distance = distance_point_line(p_prime, v1, v2);
      }
      
      CoordType dist_complement = distance_impl(p_prime, p);  //the distance normal to the plane
      
      return std::sqrt(dist_complement * dist_complement + projected_distance * projected_distance);
    }
    

    //convenience overload: point 
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p,
                  element_t<ConfigType, simplex_tag<2> > const & el)
    {
      return distance_point_triangle(p, 
                                     viennagrid::ncells<0>(el)[0].point(),
                                     viennagrid::ncells<0>(el)[1].point(),
                                     viennagrid::ncells<0>(el)[2].point());
    }

    //convenience overload: vertex 
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, point_tag> const & v,
                  element_t<ConfigType, simplex_tag<2> > const & el)
    {
      return distance_impl(v.point(), el);
    }
    
    
    
    

    //
    // Distance between point and quadrilateral (using decomposition into two triangles)
    //

    //convenience overload: point 
    //
    // Keep reference orientation in mind:
    //
    //   v[2]        v[3]
    //    * --------- * 
    //    |   \       |
    //    |       \   |
    //    * --------- *
    //   v[0]        v[1]
    //
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p,
                  element_t<ConfigType, hypercube_tag<2> > const & el)
    {
      return std::min<CoordType>(distance_point_triangle(p, 
                                                         viennagrid::ncells<0>(el)[0].point(),
                                                         viennagrid::ncells<0>(el)[1].point(),
                                                         viennagrid::ncells<0>(el)[2].point()),
                                 distance_point_triangle(p, 
                                                         viennagrid::ncells<0>(el)[1].point(),
                                                         viennagrid::ncells<0>(el)[3].point(),
                                                         viennagrid::ncells<0>(el)[2].point())
                                );
    }

    //convenience overload: vertex 
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, point_tag> const & v,
                  element_t<ConfigType, hypercube_tag<2> > const & el)
    {
      return distance_impl(v.point(), el);
    }
    
    
    
    
    
    
    // Distance between elements: As we assume them affine, distance is given by the minimum of all vertex distances to the other element
    /*template <typename ConfigType1, typename ConfigType2,
              typename ElTag1, typename ElTag2>
    typename ConfigType1::numeric_type
    distance_impl(element_t<ConfigType1, ElTag1> const & el1,
                  element_t<ConfigType2, ElTag2> const & el2)
    {
      typedef typename ConfigType1::numeric_type   ScalarType;
      
      typedef element_t<ConfigType1, ElTag1>       ElementType1;
      typedef element_t<ConfigType2, ElTag2>       ElementType2;
      
      typedef typename viennagrid::result_of::point<ConfigType1>::type                   PointType1;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType1, 0>::type   VertexOnCellContainer1;
      typedef typename result_of::iterator<VertexOnCellContainer1>::type                 VertexOnCellIterator1;

      // Step 1: Compute minimum distance from the vertices of el1 to el2
      double dist1 = std::numeric_limits<ScalarType>::max();
      
      VertexOnCellContainer1 vertices1 = ncells<0>(el1);
      for (VertexOnCellIterator1 vocit1  = vertices1.begin();
                                 vocit1 != vertices1.end();
                               ++vocit1)
      {
        dist1 = std::min<ScalarType>(dist1, distance_impl(*vocit1, el1));
      }
      
      typedef typename viennagrid::result_of::point<ConfigType2>::type                   PointType2;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType2, 0>::type   VertexOnCellContainer2;
      typedef typename result_of::iterator<VertexOnCellContainer2>::type                 VertexOnCellIterator1;

      // Step 2: Compute minimum distance from the vertices of el2 to el1
      double dist2 = std::numeric_limits<ScalarType>::max();
      
      VertexOnCellContainer2 vertices2 = ncells<0>(el2);
      for (VertexOnCellIterator1 vocit2  = vertices2.begin();
                                 vocit2 != vertices2.end();
                               ++vocit2)
      {
        dist2 = std::min<ScalarType>(dist2, distance_impl(*vocit2, el2));
      }
      
      // Minimum distance is now given by the minimum of dist1 and dist2
      return std::min<ScalarType>(dist1, dist2);
    } */
    
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the distance between n-cells */
  template <typename ElementType1, typename ElementType2>
  typename ElementType1::config_type::numeric_type
  distance(ElementType1 const & el1, ElementType2 const & el2)
  {
    return detail::distance_impl(el1, el2);
  }
  
  // Point Type requires separate treatment:
  
  
  //special case: domain
  /** @brief Returns the N-dimensional volume of a domain, where the cell type has topological dimension N. */
//   template <typename ConfigType>
//   typename ConfigType::numeric_type
//   volume(domain_t<ConfigType> const & d)
//   {
//     return detail::volume_domainsegment(d);
//   }    
  
  //special case: segment
  /** @brief Returns the N-dimensional volume of a segment, where the cell type has topological dimension N. */
//   template <typename ConfigType>
//   typename ConfigType::numeric_type
//   volume(segment_t<ConfigType> const & d)
//   {
//     return detail::volume_domainsegment(d);
//   }
  
} //namespace viennagrid
#endif
