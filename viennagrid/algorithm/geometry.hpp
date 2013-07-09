#ifndef VIENNAGRID_ALGORITHM_GEOMETRY_HPP
#define VIENNAGRID_ALGORITHM_GEOMETRY_HPP

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

#include <limits>
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/intersect.hpp"

namespace viennagrid 
{
    
    
//     template<typename domain_type, typename element_type>
//     typename viennagrid::result_of::point_type<domain_type>::type normal_vector( domain_type const & domain, element_type const & element )
//     {
//         typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
//         
//         point_type const & p0 = viennagrid::point( domain, viennagrid::vertices(element)[0] );
//         point_type const & p1 = viennagrid::point( domain, viennagrid::vertices(element)[1] );
//         point_type const & p2 = viennagrid::point( domain, viennagrid::vertices(element)[2] );
//         
//         return viennagrid::cross_prod( p1-p0, p2-p0 );
//     }
//     
//     
//   
//     namespace geometry
//     {
//         
//         template<typename point_type>
//         typename viennagrid::result_of::coord_type<point_type>::type determinant( point_type const & p0, point_type const & p1, point_type const & p2 )
//         {
//             return p0[0]*p1[1]*p2[2] + p1[0]*p2[1]*p0[2] + p2[0]*p0[1]*p1[2] - p0[2]*p1[1]*p2[0] - p1[2]*p2[1]*p0[0] - p2[2]*p0[1]*p1[0];
//         }
//         
//         template<typename point_iterator_type>
//         std::pair<
//             typename std::iterator_traits<point_iterator_type>::value_type,
//             typename std::iterator_traits<point_iterator_type>::value_type
//         > bounding_box( point_iterator_type it, point_iterator_type const & it_end )
//         {
//             typedef typename std::iterator_traits<point_iterator_type>::value_type PointType;
//             typedef typename viennagrid::result_of::coord_type<PointType>::type NumericType;
//                         
//             PointType lower_left;
//             PointType upper_right;
//             
//             std::fill( lower_left.begin(), lower_left.end(), std::numeric_limits<NumericType>::max() );
//             std::fill( upper_right.begin(), upper_right.end(), - std::numeric_limits<NumericType>::max() );
//             //std::fill( upper_right.begin(), upper_right.end(), std::numeric_limits<NumericType>::lowest() );    C++11
//             
//             for (; it != it_end; ++it )
//             {
//                 lower_left = viennagrid::min( lower_left, *it );
//                 upper_right = viennagrid::max( upper_right, *it );
//             }
//             
//             return std::make_pair( lower_left, upper_right );
//         }
//         
//         
//         
//         
//         template<typename iterator_type>
//         iterator_type circular_next(iterator_type it, iterator_type const & start_it, iterator_type const & end_it)
//         {
//             if (++it == end_it)
//                 return start_it;
//             else
//                 return it;
//         }
//         
//         template<typename iterator_type>
//         iterator_type circular_prev(iterator_type it, iterator_type const & start_it, iterator_type const & end_it)
//         {
//             if (it == start_it)
//                 it = end_it;
//             
//             return --it;
//         }
//         
//         
//         
// 
//         template<typename point_iterator_type, typename polygon_tag_type, typename point_type, typename numeric_config>
//         bool is_inside( point_iterator_type const & it_start, point_iterator_type it_end, polygon_tag_type polygon_tag,
//                         point_type const & point_to_test,
//                         numeric_config nc)
//         {
//             typedef typename std::iterator_traits<point_iterator_type>::value_type PolygonPointType;
//             std::pair<PolygonPointType, PolygonPointType> poly_bounding_box = bounding_box(it_start, it_end);
//             
//             PolygonPointType outer_point;
//             outer_point[0] = point_to_test[0];
//             outer_point[1] = poly_bounding_box.first[1] - 1;
//             bool is_inside = false;
//             
//             point_iterator_type it_prev = it_end; --it_prev;
//             point_iterator_type it_cur = it_start;
//             
//             for ( ;it_cur != it_end ; ++it_cur, it_prev = circular_next(it_prev, it_start, it_end) )
//             {
//                 PolygonPointType const & q0 = *it_prev;
//                 PolygonPointType const & q1 = *it_cur;
//                 
//                 // is inner point on polygon line?
//                 if ( point_line_intersect( point_to_test, q0, q1, interval::closed_open_tag(), nc ) )
//                     return !is_open(polygon_tag);
//                 
//                 
//                 // is current line on test line?
//                 if ( !numeric::is_equal(nc, q0[0], point_to_test[0]) || !numeric::is_equal(nc, q1[0], point_to_test[0]) )
//                 {
//                     if ( line_line_intersect( q0, q1, interval::open_open_tag(), point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                     {
//                         is_inside = !is_inside;
//                     }
//                 }
//             }
//             
//             
//             // find point which is not on the testing line
//             point_iterator_type it = it_start;
//             while ( point_line_intersect( *it, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//             {
//                 it = circular_prev(it, it_start, it_end);
//                 if (it == it_start)
//                     break;
//             }
//             
//             // no point found -> no intersection
//             if ( point_line_intersect( *it, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                 return false;
//             
//             
//             point_iterator_type circular_start_it = it;
//             it_prev = it;
//             it = circular_next(it, it_start, it_end);
//             
//             // iterating over all points
//             while (it != circular_start_it)
//             {
//                 // is point on testing line?
//                 if ( point_line_intersect( *it, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                 {
//                     // find next point which is not on testing line
//                     point_iterator_type it_next = circular_next(it, it_start, it_end);
//                     while ( point_line_intersect( *it_next, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                         it_next = circular_next(it_next, it_start, it_end);
//                     
//                     // check if the the lines/points are an ear
//                     if ( ((*it_prev)[0] - (*it)[0]) * ((*it_next)[0] - (*it)[0]) < 0 )
//                     {
//                         is_inside = !is_inside;
//                     }
//                     
//                     it_prev = it;
//                     it = it_next;
//                 }
//                 else
//                 {
//                     it_prev = it;
//                     it = circular_next(it, it_start, it_end);
//                 }
//             }
//             
//             return is_inside;
//         }
//         
//         
//         
//         
//         
//         template<typename iterator_type, typename vector_type>
//         vector_type orthogonalize_one_vector( iterator_type it, const iterator_type & end, vector_type vec )
//         {
//             for (; it != end; ++it)
//                 vec -= viennagrid::inner_prod( vec, *it ) / viennagrid::inner_prod( *it, *it ) * (*it);
//             
//             return vec;
//         }
// 
// 
//         template<typename iterator_type, typename numeric_config>
//         bool orthogonalize( iterator_type start, iterator_type end, numeric_config nc )
//         {
//             typedef typename std::iterator_traits<iterator_type>::value_type vector_type;
//             typedef typename viennagrid::result_of::coord_type<vector_type>::type coord_type;
//             
//             for (iterator_type n = start; n != end; ++n)
//             {
//                 *n = orthogonalize_one_vector(start, n, *n);
//                 
//                 if ( viennagrid::norm_1(*n) < numeric::absolute_tolerance<coord_type>(nc) )
//                     return false;
//             }
//             
//             return true;
//         }
//         
// 
//         template<typename point_iterator_type, typename numeric_config, typename out_point_type, typename out_point_iterator_type>
//         bool projection_matrix(const point_iterator_type & start, const point_iterator_type & end, numeric_config nc,
//                                out_point_type & center, out_point_iterator_type projection_matrix_start)
//         {
//             typedef typename std::iterator_traits<point_iterator_type>::value_type point_type;
//             typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
//             typedef typename numeric::result_of::numeric_type<numeric_config, coord_type>::type numeric_type;
//             
//             out_point_iterator_type projection_matrix_end = projection_matrix_start;
//             ++projection_matrix_end; ++projection_matrix_end;
//             
//             std::fill( projection_matrix_start, projection_matrix_end, point_type() );
//             
//             if (start == end) return false;     // too few points
//             
//             
//             point_iterator_type pit;
//             
//             
//             // calculating the center
//             pit = start;
//             unsigned int num_points = 1;
//             center = *pit; ++pit;
//             
//             for (; pit != end; ++pit)
//             {
//                 center += *pit;
//                 ++num_points;
//             }
//             
//             if (num_points < 3) return false;   // too few points
//             center /= num_points;
//             
//             
//             // setting up a map of vectors from the center to all points, sorted descending by the length of that vector
//             typedef std::multimap<numeric_type, point_type, std::greater<numeric_type> > vector_map_type;
//             vector_map_type sorted_vectors;
//             pit = start;
//             for (; pit != end; ++pit)
//             {
                point_type vector = center - *pit;
                typename vector_map_type::iterator it = sorted_vectors.insert( std::make_pair( viennagrid::norm_2( vector ), vector ) );
                it->second /= it->first;    // normalizing the vector
            }
            
            
            // finding 2 non-liner dependent vectors
            unsigned int projection_matrix_index = 0;
            typename vector_map_type::iterator it = sorted_vectors.begin();
            while (projection_matrix_index < 2)
            {
                if ( it->first < numeric::absolute_tolerance<coord_type>(nc) )
                    return false;               // points are too close together
                
                // check linear dependency with other vectors in projection matrix
                unsigned int index = 0;
                out_point_iterator_type pmit = projection_matrix_start;
                for (; index < projection_matrix_index; ++index, ++pmit)
                {
                    numeric_type angle_cos = viennagrid::inner_prod( it->second, *pmit );
                    if ( std::abs(angle_cos) > 1 - numeric::absolute_tolerance<coord_type>(nc))
                        break;
                }
                
                if ( index == projection_matrix_index)
                {
                    *pmit = it->second;
                    ++projection_matrix_index;
                }
                
                ++it;
            }

            // orthogonalize vectors
            orthogonalize( projection_matrix_start, projection_matrix_end, nc );
            
            // normalize vectors
            for (out_point_iterator_type it = projection_matrix_start; it != projection_matrix_end; ++it)
                *it /= viennagrid::norm_2(*it);
            
            return true;
        }


        template<typename point_iterator_type, typename out_point_iterator_type, typename point_type, typename projection_point_iterator_type>
        void project(point_iterator_type in, const point_iterator_type & in_end,
                    out_point_iterator_type out,
                    point_type center,
                    projection_point_iterator_type const & proj_start, projection_point_iterator_type const & proj_end)
        {
            for ( ; in != in_end; ++in, ++out)
            {
                point_type tmp = *in - center;
                
                size_t index = 0;
                for (projection_point_iterator_type it = proj_start; it != proj_end; ++it, ++index)
                    (*out)[index] = viennagrid::inner_prod( tmp, *it );
            }
        }
        
        
        
        template<typename point_iterator_type, typename out_point_iterator_type, typename numeric_config>
        bool project_onto_2dplane(point_iterator_type in, const point_iterator_type & in_end,
                                  out_point_iterator_type out,
                                  numeric_config nc)
        {
            typedef typename std::iterator_traits<point_iterator_type>::value_type InPointType;
            typedef typename std::iterator_traits<out_point_iterator_type>::value_type OutPointType;
            
            size_t dimension = (*in).size();
            std::vector<InPointType> projection_matrix(dimension);
            InPointType center;
            
            if (!projection_matrix(in, in_end, projection_matrix.begin(), center, nc))
                return false;
            
            project(in, in_end, out, center, projection_matrix.begin(), projection_matrix.begin() + 2);
        }

    }

} 

#endif

