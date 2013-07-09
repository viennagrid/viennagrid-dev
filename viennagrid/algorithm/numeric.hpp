#ifndef VIENNAGRID_ALGORITHM_NUMERIC_HPP
#define VIENNAGRID_ALGORITHM_NUMERIC_HPP

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

#include <algorithm>
#include <cmath>

namespace viennagrid 
{
//     namespace numeric
//     {
//         namespace result_of
//         {
//             template<typename numeric_config, typename outer_numeric_type>
//             struct numeric_type
//             {
//                 typedef typename numeric_config::numeric_type type;
//             };
//             
//             template<typename outer_numeric_type>
//             struct numeric_type<double, outer_numeric_type>
//             {
//                 typedef double type;
//             };
//         }
//         
//         template<typename outer_numeric_type, typename numeric_config>
//         typename result_of::numeric_type<numeric_config, outer_numeric_type>::type absolute_tolerance( numeric_config config )
//         {
//             typedef typename result_of::numeric_type<numeric_config, outer_numeric_type>::type numeric_type;
//             return static_cast<numeric_type>(config.eps);
//         }
//         
//         template<typename outer_numeric_type, typename numeric_config>
//         typename result_of::numeric_type<numeric_config, outer_numeric_type>::type relative_tolerance( numeric_config config, outer_numeric_type base )
//         {
//             typedef typename result_of::numeric_type<numeric_config, outer_numeric_type>::type numeric_type;
//             return std::max( static_cast<numeric_type>(config.eps) * static_cast<numeric_type>(base), absolute_tolerance<outer_numeric_type>(config) );
//         }
//         
//         template<typename outer_numeric_type>
//         typename result_of::numeric_type<double, outer_numeric_type>::type absolute_tolerance( double eps )
//         {
//             typedef typename result_of::numeric_type<double, outer_numeric_type>::type numeric_type;
//             return static_cast<numeric_type>(eps);
//         }
//         
//         template<typename outer_numeric_type>
//         typename result_of::numeric_type<double, outer_numeric_type>::type relative_tolerance( double eps, outer_numeric_type base )
//         {
//             typedef typename result_of::numeric_type<double, outer_numeric_type>::type numeric_type;
//             return std::max( static_cast<numeric_type>(eps) * base, absolute_tolerance<outer_numeric_type>(eps) );
//         }
//         
//         
//         template<typename numeric_config, typename numeric_type>
//         bool is_equal( numeric_config nc, numeric_type first, numeric_type second )
//         {
//             return (std::abs(first-second) < relative_tolerance(nc, first));
//         }
//         
//         template<typename numeric_config, typename numeric_type>
//         bool is_not_equal( numeric_config nc, numeric_type first, numeric_type second )
//         {
//             return !is_equal(nc, first, second);
//         }
//         
//     }

} 

#endif

