#ifndef VIENNAGRID_ALGORITHM_CLOSEST_POINTS_HPP
#define VIENNAGRID_ALGORITHM_CLOSEST_POINTS_HPP

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
#include <utility>

#include "viennagrid/forwards.h"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"

/** @file closest_points.hpp
    @brief Routines for computing the two points of two different objects being closest to each other.
*/


namespace viennagrid
{
  namespace detail
  {
    
    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> > const &
    point_pair_with_shortest_distance( std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_1,
                                       std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_2)
    {
      if ( norm_2(pair_1.first - pair_1.second) <= norm_2(pair_2.first - pair_2.second) )
        return pair_1;
      
      return pair_2;
    }

    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> > const &
    point_pair_with_shortest_distance( std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_1,
                                       std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_2,
                                       std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_3)
    {
      CoordType dist_pair_1 = norm_2(pair_1.first - pair_1.second);
      CoordType dist_pair_2 = norm_2(pair_2.first - pair_2.second);
      CoordType dist_pair_3 = norm_2(pair_3.first - pair_3.second);
      
      if (   (dist_pair_1 <= dist_pair_2)
          && (dist_pair_1 <= dist_pair_3) )
        return pair_1;

      if (   (dist_pair_2 <= dist_pair_1)
          && (dist_pair_2 <= dist_pair_3) )
        return pair_2;
      
      return pair_3;
    }
    

    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> > const &
    point_pair_with_shortest_distance( std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_1,
                                       std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_2,
                                       std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_3,
                                       std::pair<point_t<CoordType, CoordinateSystem>,
                                                 point_t<CoordType, CoordinateSystem> > const & pair_4)
    {
      // Note: Recursive use of point_pair_with_shortest_distance() has a bit of overhead. Feel free to improve this
      return point_pair_with_shortest_distance(point_pair_with_shortest_distance(pair_1, pair_2),
                                               point_pair_with_shortest_distance(pair_3, pair_4));
    }
    
    
    
    //
    // Closest points between points (trivial)
    //
    
    // Closest points between two points:
    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(point_t<CoordType, CoordinateSystem> const & p1,
                        point_t<CoordType, CoordinateSystem> const & p2)
    {
      return std::make_pair(p1, p2);
    }

    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(point_t<CoordType, CoordinateSystem> const & p1,
                        element_t<ConfigType, point_tag> const & v2)
    {
      return std::make_pair(p1, v2.point());
    }

    template <typename ConfigType, typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(element_t<ConfigType, point_tag> const & v1,
                        point_t<CoordType, CoordinateSystem> const & p2)
    {
      return std::make_pair(v1.point(), p2);
    }
    
    // Closest points between vertices:
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, point_tag> const & v1,
                        element_t<ConfigType, point_tag> const & v2)
    {
      return std::make_pair(v1.point(), v2.point());
    }

    
    
    //
    // Closest points between point and line segment
    //
    
    // Implementation: Supposed to work for arbitrary dimensions
    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_point_line(point_t<CoordType, CoordinateSystem> const & p,
                              point_t<CoordType, CoordinateSystem> const & line_p1,
                              point_t<CoordType, CoordinateSystem> const & line_p2)
    {
      typedef point_t<CoordType, CoordinateSystem>  PointType;

      //compute t such that projection of p onto [line_p1, line_p2] is given by p' = line_p1 + t * (line_p2 - line_p1)
      CoordType t = viennagrid::inner_prod( (p - line_p1), (line_p2 - line_p1) ) / viennagrid::inner_prod(line_p2 - line_p1, line_p2 - line_p1);
      
      //restrict t to line segment, i.e. t \in [0, 1]
      t = std::max<CoordType>(0, std::min<CoordType>(1, t));
      
      PointType p_prime = line_p1 + t * (line_p2 - line_p1);  //closest point to p on line
      
      return std::make_pair(p, p_prime);
    }
    
    
    //convenience overload: point and simplex line
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(point_t<CoordType, CoordinateSystem> const & p,
                        element_t<ConfigType, simplex_tag<1> > const & el)
    {
      return closest_points_point_line(p, 
                                       viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //convenience overload: point and hypercube line
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(point_t<CoordType, CoordinateSystem> const & p,
                        element_t<ConfigType, hypercube_tag<1> > const & el)
    {
      return closest_points_point_line(p, 
                                       viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //convenience overload: vertex and simplex line
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, point_tag> const & v,
                        element_t<ConfigType, simplex_tag<1> > const & el)
    {
      return closest_points_point_line(v.point(), 
                                       viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //convenience overload: vertex and hypercube line
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, point_tag> const & v,
                        element_t<ConfigType, hypercube_tag<1> > const & el)
    {
      return closest_points_point_line(v.point(), 
                                       viennagrid::ncells<0>(el)[0].point(), viennagrid::ncells<0>(el)[1].point());
    }
    
    //
    // Distance between two line segments
    //
    
    // Implementation: Supposed to work for arbitrary dimensions
    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_line_line(point_t<CoordType, CoordinateSystem> const & v0, point_t<CoordType, CoordinateSystem> const & v1, //endpoints of first line
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
      //std::cout << "denominator: " << denominator << std::endl;
                              
      if (denominator < 1e-6 * v_in_v * w_in_w) //lines parallel (up to round-off)
      {
        return point_pair_with_shortest_distance(closest_points_point_line(w0, v0, v1),
                                                 closest_points_point_line(w1, v0, v1),
                                                 closest_points_point_line(v0, w0, w1),
                                                 closest_points_point_line(v1, w0, w1));
      }

      //Lines are not parallel: Compute minimizers s, t:
      PointType dir_distance = v0 - w0;  //any vector connecting two points on V and W
      
      //if (inner_prod(dir_distance, dir_distance) / v_in_v < 1e-10)  //v0 and w0 are the same point
      //  return std::make_pair(v0, w0);
      
      CoordType v_in_dir_distance = viennagrid::inner_prod(dir_v, dir_distance);
      CoordType w_in_dir_distance = viennagrid::inner_prod(dir_w, dir_distance);
      
      CoordType s = (v_in_w * w_in_dir_distance - w_in_w * v_in_dir_distance) / denominator;
      CoordType t = (v_in_v * w_in_dir_distance - v_in_w * v_in_dir_distance) / denominator;
      //std::cout << "s = " << s << std::endl;
      //std::cout << "t = " << s << std::endl;
      
      //Check whether minimizing distance is within line segment, i.e. s,t \in [0,1]
      if ( (s < 0) || (s > 1) || (t < 0) || (t > 1) ) //Note: A more fine-grained check is possible for those looking for optimizations
        return point_pair_with_shortest_distance(closest_points_point_line(w0, v0, v1),
                                                 closest_points_point_line(w1, v0, v1),
                                                 closest_points_point_line(v0, w0, w1),
                                                 closest_points_point_line(v1, w0, w1));
      
      // compute points on V(s) an W(t) for which distance is smallest:
      PointType min_dist_point_V = v0 + s * dir_v;
      PointType min_dist_point_W = w0 + t * dir_w;
      
      return std::make_pair(min_dist_point_V, min_dist_point_W);
    }
    
    
    //convenience overload: simplex line and simplex line
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, simplex_tag<1> > const & line0,
                        element_t<ConfigType, simplex_tag<1> > const & line1)
    {
      return closest_points_line_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                      viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }
    
    //convenience overload: hypercube line and simplex line
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, hypercube_tag<1> > const & line0,
                        element_t<ConfigType, simplex_tag<1> > const & line1)
    {
      return closest_points_line_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                      viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }

    //convenience overload: simplex line and hypercube line
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, simplex_tag<1> > const & line0,
                        element_t<ConfigType, hypercube_tag<1> > const & line1)
    {
      return closest_points_line_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                      viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }
    
    //convenience overload: hypercube line and hypercube line
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, hypercube_tag<1> > const & line0,
                        element_t<ConfigType, hypercube_tag<1> > const & line1)
    {
      return closest_points_line_line(viennagrid::ncells<0>(line0)[0].point(), viennagrid::ncells<0>(line0)[1].point(), 
                                      viennagrid::ncells<0>(line1)[0].point(), viennagrid::ncells<0>(line1)[1].point());
    }
    

    //
    // Distance between point and triangle
    //
    
    // Implementation: Supposed to work for arbitrary dimensions
    // Projects p onto the plane spanned by the triangle, then computes the shortest distance in the plane and uses Pythagoras for the full distance
    template <typename CoordType, typename CoordinateSystem>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_point_triangle(point_t<CoordType, CoordinateSystem> const & p,
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
      CoordType t = (- b * u_in_u0 + a * u_in_u1) / denominator;
      
      PointType p_prime = v0 + s * u0 + t * u1;  //projection of p onto triangular plane

      // nonzero distance is encountered only if p_prime is outside the triangle
      if (    (s < 0 || s > 1)
           || (t < 0 || t > 1)
           || (s + t > 1) )     //p_prime is outside the triangle
      {
        // safe mode: Compute distances to all edges. Can be optimized further by using information from s, t, etc.
        return point_pair_with_shortest_distance(closest_points_point_line(p_prime, v0, v1),
                                                 closest_points_point_line(p_prime, v0, v2),
                                                 closest_points_point_line(p_prime, v1, v2));
      }
      
      return std::make_pair(p, p_prime);
    }
    

    //convenience overload: point 
    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(point_t<CoordType, CoordinateSystem> const & p,
                        element_t<ConfigType, simplex_tag<2> > const & el)
    {
      return closest_points_point_triangle(p, 
                                           viennagrid::ncells<0>(el)[0].point(),
                                           viennagrid::ncells<0>(el)[1].point(),
                                           viennagrid::ncells<0>(el)[2].point());
    }

    //convenience overload: vertex 
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, point_tag> const & v,
                        element_t<ConfigType, simplex_tag<2> > const & el)
    {
      return closest_points_impl(v.point(), el);
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
    std::pair<point_t<CoordType, CoordinateSystem>,
              point_t<CoordType, CoordinateSystem> >
    closest_points_impl(point_t<CoordType, CoordinateSystem> const & p,
                        element_t<ConfigType, hypercube_tag<2> > const & el)
    {
      return point_pair_with_shortest_distance(closest_points_point_triangle(p, 
                                                                             viennagrid::ncells<0>(el)[0].point(),
                                                                             viennagrid::ncells<0>(el)[1].point(),
                                                                             viennagrid::ncells<0>(el)[2].point()),
                                               closest_points_point_triangle(p, 
                                                                             viennagrid::ncells<0>(el)[1].point(),
                                                                             viennagrid::ncells<0>(el)[3].point(),
                                                                             viennagrid::ncells<0>(el)[2].point())
                                              );
    }

    //convenience overload: vertex 
    template <typename ConfigType>
    std::pair<typename result_of::point<ConfigType>::type,
              typename result_of::point<ConfigType>::type>
    closest_points_impl(element_t<ConfigType, point_tag> const & v,
                        element_t<ConfigType, hypercube_tag<2> > const & el)
    {
      return closest_points_impl(v.point(), el);
    }
    
    

    
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the distance between n-cells */
  template <typename ElementType1, typename ElementType2>
  std::pair<typename result_of::point<ElementType1>::type,
            typename result_of::point<ElementType1>::type>
  closest_points(ElementType1 const & el1, ElementType2 const & el2)
  {
    return detail::closest_points_impl(el1, el2);
  }
  

  
} //namespace viennagrid
#endif