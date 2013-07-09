#ifndef VIENNAGRID_ALGORITHM_INTERSECT_HPP
#define VIENNAGRID_ALGORITHM_INTERSECT_HPP

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

#include <assert.h>

#include "viennagrid/forwards.hpp"
#include "viennagrid/algorithm/numeric.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"

namespace viennagrid 
{
  
//     namespace geometry
//     {
// 
//         namespace interval
//         {
//         
//             struct open_tag {};
//             struct closed_tag {};
//             
//             
//             template<typename tag0, typename tag1>
//             open_tag intersect_tag( tag0, tag1 )
//             { return open_tag(); }
//             
//             inline closed_tag intersect_tag ( closed_tag, closed_tag )
//             { return closed_tag(); }
//             
//             inline bool is_open(open_tag) { return true; }
//             inline bool is_open(closed_tag) { return false; }
//             
//             inline bool is_closed(open_tag) { return false; }
//             inline bool is_closed(closed_tag) { return true; }
//             
//             struct open_open_tag
//             {
//                 typedef open_tag first_tag;
//                 typedef open_tag second_tag;
//             };
//             
//             struct open_closed_tag
//             {
//                 typedef open_tag first_tag;
//                 typedef closed_tag second_tag;
//             };
//             
//             struct closed_open_tag
//             {
//                 typedef closed_tag first_tag;
//                 typedef open_tag second_tag;
//             };
//             
//             struct closed_closed_tag
//             {
//                 typedef closed_tag first_tag;
//                 typedef closed_tag second_tag;
//             };
//             
//             template<typename dual_tag>
//             typename dual_tag::first_tag first_tag( dual_tag )
//             { return typename dual_tag::first_tag(); }
//             
//             template<typename dual_tag>
//             typename dual_tag::second_tag second_tag( dual_tag )
//             { return typename dual_tag::second_tag(); }
//             
//             inline open_open_tag make_tag( open_tag, open_tag )
//             { return open_open_tag(); }
//             
//             inline open_closed_tag make_tag( open_tag, closed_tag )
//             { return open_closed_tag(); }
//             
//             inline closed_open_tag make_tag( closed_tag, open_tag )
//             { return closed_open_tag(); }
//             
//             inline closed_closed_tag make_tag( closed_tag, closed_tag )
//             { return closed_closed_tag(); }
// 
//             
//             template<typename numeric_type>
//             bool greater( numeric_type ref, numeric_type to_test, open_tag )
//             { return ref < to_test; }
// 
//             template<typename numeric_type>
//             bool greater( numeric_type ref, numeric_type to_test, closed_tag )
//             { return ref <= to_test; }
//             
//             template<typename numeric_type>
//             bool less( numeric_type ref, numeric_type to_test, open_tag )
//             { return ref > to_test; }
// 
//             template<typename numeric_type>
//             bool less( numeric_type ref, numeric_type to_test, closed_tag )
//             { return ref >= to_test; }
//             
//             
//             
//             
//             template<typename numeric_type, typename first_tag_type, typename second_tag_type, typename numeric_config>
//             bool point_in_interval( numeric_type first, first_tag_type first_tag,
//                                     numeric_type second, second_tag_type second_tag,
//                                     numeric_type to_test,
//                                     numeric_config nc
//                                   )
//             {
//                 if ( numeric::is_equal(nc, first, to_test) )
//                     return is_closed(first_tag);
//                 
//                 if ( numeric::is_equal(nc, second, to_test) )
//                     return is_closed(second_tag);
// 
//                 return (first <= second) ?
//                     (greater(first, to_test, first_tag) && less(second, to_test, second_tag)) :
//                     (greater(second, to_test, second_tag) && less(first, to_test, first_tag));
//             }
//             
//             template<typename numeric_type, typename dual_tag, typename numeric_config>
//             bool point_in_interval( numeric_type first, numeric_type second, dual_tag tag, numeric_type to_test, numeric_config nc )
//             {
//                 return point_in_interval(first, first_tag(tag), second, second_tag(tag), to_test, nc);
//             }
//             
//             
//             template<typename numeric_type, typename interval_0_tag_type, typename interval_1_tag_type, typename numeric_config>
//             bool interval_intersect(numeric_type first_0, numeric_type second_0, interval_0_tag_type interval_0_tag,
//                                     numeric_type first_1, numeric_type second_1, interval_1_tag_type interval_1_tag,
//                                     numeric_config nc)
//             {
//                 return point_in_interval( first_0, intersect_tag(first_tag(interval_0_tag), first_tag(interval_1_tag)), second_0, intersect_tag(second_tag(interval_0_tag), first_tag(interval_1_tag)), first_1, nc ) ||
//                     point_in_interval( first_0, intersect_tag(first_tag(interval_0_tag), second_tag(interval_1_tag)), second_0, intersect_tag(second_tag(interval_0_tag), second_tag(interval_1_tag)), second_1, nc ) ||
//                     point_in_interval( first_1, intersect_tag(first_tag(interval_1_tag), first_tag(interval_0_tag)), second_1, intersect_tag(second_tag(interval_1_tag), first_tag(interval_0_tag)), first_0, nc ) ||
//                     point_in_interval( first_1, intersect_tag(first_tag(interval_1_tag), second_tag(interval_0_tag)), second_1, intersect_tag(second_tag(interval_1_tag), second_tag(interval_0_tag)), second_0, nc ) ||
//                     ( (first_0 == first_1) && (second_0 == second_1) && (std::abs(second_0 - first_0) > 0) ) ||
//                     ( (first_0 == second_1) && (second_0 == first_1) && (std::abs(second_0 - first_0) > 0) );
//             }
//         
//         }
//         
//         
//         
//         class intersection_result
//         {
//             intersection_result() : dim_(0) {}
//             intersection_result(int intersection_dimension_) : dim_(intersection_dimension_ + 1) {}
//             
//         public:
//             
//             int dim() const { return dim_-1; }
//             
//             operator bool () const
//             {
//                 return dim_;
//             }
//             
//             static intersection_result no_intersection()
//             {
//                 return intersection_result();
//             }
//             
//             static intersection_result intersection_dimension(int intersection_dimension_)
//             {
//                 assert(intersection_dimension_ >= 0);
//                 return intersection_result(intersection_dimension_);
//             }
//             
//         private:
//             int dim_;
//         };
//         
//         
//         struct trivial_point_line_intersect_functor
//         {
//             template<typename point_type, typename line_tag>
//             void operator() (point_type const & p, point_type const & q0, point_type const & q1, line_tag tag) const {}
//         };
//         
//         
//         
//         template<typename point_type, typename numeric_config, typename line_tag, typename intersection_functor_type>
//         intersection_result point_line_intersect(point_type const & p,
//                                        point_type const & q0, point_type const & q1, line_tag tag,
//                                        numeric_config nc, intersection_functor_type intersection_functor )
//         {
//             typedef typename viennagrid::result_of::coord_type< point_type >::type coord_type;
//             typedef typename numeric::result_of::numeric_type<numeric_config, coord_type>::type inner_numeric_type;
//             
//             point_type dir = q1 - q0;
//             point_type lhs = p - q0;
//             
//             inner_numeric_type relative_eps = numeric::relative_tolerance(nc, viennagrid::norm_1(dir) );
//             
// //             std::cout << "  " << dir[0] << " " << lhs [0] << std::endl;
//             
//             if ( norm_1(dir) < relative_eps )
//                 return intersection_result::no_intersection();
//             
//             
//             if ( std::abs(dir[0] * lhs[1] - lhs[0] * dir[1]) < relative_eps )
//             {
//                 intersection_functor(p, q0, q1, tag);
//                 
//                 if ((std::abs(dir[0]) < relative_eps) ?
//                     interval::point_in_interval(coord_type(0), dir[1], tag, lhs[1], nc) :
//                     interval::point_in_interval(coord_type(0), dir[0], tag, lhs[0], nc))
//                 {
//                     return intersection_result::intersection_dimension(0);
//                 }
//             }
// 
//             return intersection_result::no_intersection();
//         }
//         
//         
//         template<typename point_type, typename numeric_config, typename line_tag>
//         intersection_result point_line_intersect(point_type const & p,
//                                        point_type const & q0, point_type const & q1, line_tag tag,
//                                        numeric_config nc)
//         {
//             trivial_point_line_intersect_functor f;
//             return point_line_intersect(p, q0, q1, tag, nc, f);
//         }
// 
//         
//         
//         
//         struct trivial_point_ray_intersect_functor
//         {
//             template<typename point_type>
//             void operator() (point_type const & p, point_type const & q0, point_type const & q1) const {}
//         };
//         
//         
//         template<typename point_type, typename numeric_config, typename intersection_functor_type>
//         intersection_result point_ray_intersect(point_type const & p,
//                                        point_type const & q0, point_type const & q1,
//                                        numeric_config nc, intersection_functor_type intersection_functor )
//         {
//             typedef typename viennagrid::result_of::coord_type< point_type >::type coord_type;
//             typedef typename numeric::result_of::numeric_type<numeric_config, coord_type>::type inner_numeric_type;
//             
//             point_type dir = q1 - q0;
//             point_type lhs = p - q0;
//             
//             inner_numeric_type relative_eps = numeric::relative_tolerance(nc, viennagrid::norm_1(dir) );
//             
//             if ( norm_1(dir) < relative_eps )
//                 return intersection_result::no_intersection();
//             
//             
//             if ( std::abs(dir[0] * lhs[1] - lhs[0] * dir[1]) < relative_eps )
//             {
//                 intersection_functor(p, q0, q1);
//                 return intersection_result::intersection_dimension(0);
//             }
// 
//             return intersection_result::no_intersection();
//         }
//         
//         template<typename point_type, typename numeric_config>
//         intersection_result point_ray_intersect(point_type const & p,
//                                        point_type const & q0, point_type const & q1,
//                                        numeric_config nc)
//         {
//             trivial_point_ray_intersect_functor f;
//             return point_ray_intersect(p, q0, q1, nc, f);
//         }
//         
//         
//         
//         
//         struct trivial_line_line_intersect_point_intersection_functor
//         {
//             template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
//             void operator() (point_type const & v0, point_type const & v1 , line1_tag tag1,
//                              point_type const & w0, point_type const & w1, line2_tag tag2,
//                              coord_type s, coord_type t, coord_type denominator) const {}
//         };
//         
//         struct trivial_line_line_intersect_overlapping_lines_functor
//         {
//             template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
//             void operator() (point_type const & v0, point_type const & v1 , line1_tag tag1,
//                              point_type const & w0, point_type const & w1, line2_tag tag2,
//                              coord_type first1, coord_type second1,
//                              coord_type first2, coord_type second2) const {}
//         };
//         
//         
//         
//         template <typename point_type, typename numeric_config, typename line1_tag, typename line2_tag, typename point_intersection_functor_type, typename overlapping_lines_functor_type>
//         intersection_result line_line_intersect(
//                                 point_type const & v0, point_type const & v1, line1_tag tag1, //endpoints of first line
//                                 point_type const & w0, point_type const & w1, line2_tag tag2, //endpoints of second line
//                                 numeric_config nc,
//                                 point_intersection_functor_type point_intersection_functor, overlapping_lines_functor_type overlapping_lines_functor)
//         {
//             typedef typename viennagrid::result_of::coord_type< point_type >::type coord_type;
//             typedef typename numeric::result_of::numeric_type<numeric_config, coord_type>::type inner_numeric_type;
// 
//             // write V(s) = v0 + s * (v1 - v0), s \in [0,1]
//             //       W(t) = w0 + t * (w1 - w0), t \in [0,1]
//             
//             // compute s and t assuming V(s) and W(t) to be infinite lines:
//             // cf. http://www.softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
//             point_type dir_v = v1 - v0;  //direction vector for line V(s)
//             point_type dir_w = w1 - w0;  //direction vector for line W(t)
//             
//             
//             
//             coord_type v_in_v = viennagrid::inner_prod(dir_v, dir_v);
//             coord_type v_in_w = viennagrid::inner_prod(dir_v, dir_w);
//             coord_type w_in_w = viennagrid::inner_prod(dir_w, dir_w);
//             
//             coord_type denominator = v_in_v * w_in_w - v_in_w * v_in_w;
//                                     
//             if ( std::abs(denominator) < numeric::relative_tolerance(nc, v_in_v * w_in_w) ) //lines parallel (up to round-off)
//             {
//                 point_type lhs_v = w0 - v0;
//                 inner_numeric_type relative_eps = numeric::relative_tolerance(nc, viennagrid::norm_1(dir_v) );
//                 
//                 if ( std::abs(dir_v[0] * lhs_v[1] - lhs_v[0] * dir_v[1]) < relative_eps ) // lines share a common ray
//                 {
//                     std::size_t index = (std::abs(dir_v[0]) < relative_eps) ? 1 : 0;
// 
//                     coord_type w_first = lhs_v[index];
//                     coord_type w_second = w1[index] - v0[index];
//                         
//                     if (interval::interval_intersect( coord_type(0), dir_v[index], tag1, w_first, w_second, tag2, nc ))
//                     {
//                         overlapping_lines_functor(v0, v1, tag1, w0, w1, tag2, coord_type(0), dir_v[index], w_first, w_second);
//                         return intersection_result::intersection_dimension(1);
//                     }
//                     else
//                         return intersection_result::no_intersection();
//                 }
//                 else
//                     return intersection_result::no_intersection();
//             }
// 
//             //Lines are not parallel: Compute minimizers s, t:
//             point_type dir_distance = v0 - w0;  //any vector connecting two points on V and W
//             
//             coord_type v_in_dir_distance = viennagrid::inner_prod(dir_v, dir_distance);
//             coord_type w_in_dir_distance = viennagrid::inner_prod(dir_w, dir_distance);
//             
//             coord_type s = (v_in_w * w_in_dir_distance - w_in_w * v_in_dir_distance);
//             coord_type t = (v_in_v * w_in_dir_distance - v_in_w * v_in_dir_distance);
//             
//             if (point_in_interval(coord_type(0), denominator, tag1, s, nc) && point_in_interval(coord_type(0), denominator, tag2, t, nc))
//             {
//                 point_intersection_functor(v0, v1, tag1, w0, w1, tag2, s, t, denominator);
//                 return intersection_result::intersection_dimension(0);
//             }
//             else
//                 return intersection_result::no_intersection();
//         }
//         
//         template <typename point_type, typename numeric_config, typename line1_tag, typename line2_tag>
//         intersection_result line_line_intersect(point_type const & v0, point_type const & v1, line1_tag tag1, //endpoints of first line
//                                  point_type const & w0, point_type const & w1, line2_tag tag2, //endpoints of second line
//                                  numeric_config nc)
//         {
//             trivial_line_line_intersect_point_intersection_functor f1;
//             trivial_line_line_intersect_overlapping_lines_functor f2;
//             return line_line_intersect(v0, v1, tag1, w0, w1, tag2, nc, f1, f2);
//         }
//         
//         
//         
//         template<typename point_iterator_type, typename linestrip_tag_type, typename point_type, typename line_tag_type, typename numeric_config>
//         bool line_linestrip_intersect(point_iterator_type it1, const point_iterator_type & it_end, linestrip_tag_type linestrip_tag,
//                                       const point_type & p0, const point_type & p1, line_tag_type line_tag,
//                                       numeric_config nc )
//         {
//             if (it1 == it_end) return false;
//             point_iterator_type it2 = it1++;
//             
//             for (; it2 != it_end; ++it1, ++it2)
//             {
//                 if (line_line_intersect( p0, p1, line_tag, *it1, *it2, linestrip_tag, nc ))
//                     return true;
//             }
//             
//             return false;        
//         }
//         
//         
//         template<typename point_iterator_type, typename linestrip_tag_type, typename point_type, typename line_tag_type, typename numeric_config>
//         bool line_polygon_intersect(point_iterator_type it1, point_iterator_type it_end, linestrip_tag_type linestrip_tag,
//                                     const point_type & p0, const point_type & p1, line_tag_type line_tag,
//                                     numeric_config nc )
//         {
//             if (line_linestrip_intersect(it1, it_end, linestrip_tag, p0, p1, line_tag, nc)) return true;
//             else return line_line_intersect( p0, p1, line_tag, *(--it_end), *it1, linestrip_tag, nc );
//         }
//       
//     }

} 

#endif

