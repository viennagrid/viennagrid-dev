#include "viennagrid/viennagrid.h"
#include "../common.hpp"
#include <cmath>





void closest_points_shortest_distance_2(viennagrid_dimension dimension,
                                        viennagrid_numeric const * p1p1,
                                        viennagrid_numeric const * p1p2,
                                        viennagrid_numeric const * p2p1,
                                        viennagrid_numeric const * p2p2,
                                        viennagrid_numeric * result1,
                                        viennagrid_numeric * result2)
{
  if (!result1 && !result2) return;

  viennagrid_numeric dp1;
  viennagrid_numeric dp2;

  viennagrid_point_distance_2(dimension, p1p1, p1p2, &dp1);
  viennagrid_point_distance_2(dimension, p2p1, p2p2, &dp2);

  if (dp1 <= dp2)
  {
    if (result1) viennagrid_point_copy(dimension, p1p1, result1);
    if (result2) viennagrid_point_copy(dimension, p1p2, result2);
  }
  else
  {
    if (result1) viennagrid_point_copy(dimension, p2p1, result1);
    if (result2) viennagrid_point_copy(dimension, p2p2, result2);
  }
}

void closest_points_shortest_distance_3(viennagrid_dimension dimension,
                                        viennagrid_numeric const * p1p1,
                                        viennagrid_numeric const * p1p2,
                                        viennagrid_numeric const * p2p1,
                                        viennagrid_numeric const * p2p2,
                                        viennagrid_numeric const * p3p1,
                                        viennagrid_numeric const * p3p2,
                                        viennagrid_numeric * result1,
                                        viennagrid_numeric * result2)
{
  if (!result1 && !result2) return;

  viennagrid_numeric dp1;
  viennagrid_numeric dp2;
  viennagrid_numeric dp3;

  viennagrid_point_distance_2(dimension, p1p1, p1p2, &dp1);
  viennagrid_point_distance_2(dimension, p2p1, p2p2, &dp2);
  viennagrid_point_distance_2(dimension, p3p1, p3p2, &dp3);

  if ((dp1 <= dp2) && (dp1 <= dp3))
  {
    if (result1) viennagrid_point_copy(dimension, p1p1, result1);
    if (result2) viennagrid_point_copy(dimension, p1p2, result2);
  }
  else if ((dp2 <= dp1) && (dp2 <= dp3))
  {
    if (result1) viennagrid_point_copy(dimension, p2p1, result1);
    if (result2) viennagrid_point_copy(dimension, p2p2, result2);
  }
  else
  {
    if (result1) viennagrid_point_copy(dimension, p3p1, result1);
    if (result2) viennagrid_point_copy(dimension, p3p2, result2);
  }
}

void closest_points_shortest_distance_4(viennagrid_dimension dimension,
                                        viennagrid_numeric const * p1p1,
                                        viennagrid_numeric const * p1p2,
                                        viennagrid_numeric const * p2p1,
                                        viennagrid_numeric const * p2p2,
                                        viennagrid_numeric const * p3p1,
                                        viennagrid_numeric const * p3p2,
                                        viennagrid_numeric const * p4p1,
                                        viennagrid_numeric const * p4p2,
                                        viennagrid_numeric * result1,
                                        viennagrid_numeric * result2)
{
  if (!result1 && !result2) return;

  viennagrid_numeric dp1;
  viennagrid_numeric dp2;
  viennagrid_numeric dp3;
  viennagrid_numeric dp4;

  viennagrid_point_distance_2(dimension, p1p1, p1p2, &dp1);
  viennagrid_point_distance_2(dimension, p2p1, p2p2, &dp2);
  viennagrid_point_distance_2(dimension, p3p1, p3p2, &dp3);
  viennagrid_point_distance_2(dimension, p4p1, p4p2, &dp4);

  if ((dp1 <= dp2) && (dp1 <= dp3) && (dp1 <= dp4))
  {
    if (result1) viennagrid_point_copy(dimension, p1p1, result1);
    if (result2) viennagrid_point_copy(dimension, p1p2, result2);
  }
  else if ((dp2 <= dp1) && (dp2 <= dp3) && (dp2 <= dp4))
  {
    if (result1) viennagrid_point_copy(dimension, p2p1, result1);
    if (result2) viennagrid_point_copy(dimension, p2p2, result2);
  }
  else if ((dp3 <= dp1) && (dp3 <= dp2) && (dp3 <= dp4))
  {
    if (result1) viennagrid_point_copy(dimension, p3p1, result1);
    if (result2) viennagrid_point_copy(dimension, p3p2, result2);
  }
  else
  {
    if (result1) viennagrid_point_copy(dimension, p4p1, result1);
    if (result2) viennagrid_point_copy(dimension, p4p2, result2);
  }
}

void closest_points_shortest_distance_5(viennagrid_dimension dimension,
                                        viennagrid_numeric const * p1p1,
                                        viennagrid_numeric const * p1p2,
                                        viennagrid_numeric const * p2p1,
                                        viennagrid_numeric const * p2p2,
                                        viennagrid_numeric const * p3p1,
                                        viennagrid_numeric const * p3p2,
                                        viennagrid_numeric const * p4p1,
                                        viennagrid_numeric const * p4p2,
                                        viennagrid_numeric const * p5p1,
                                        viennagrid_numeric const * p5p2,
                                        viennagrid_numeric * result1,
                                        viennagrid_numeric * result2)
{
  if (!result1 && !result2) return;

  viennagrid_numeric dp1;
  viennagrid_numeric dp2;
  viennagrid_numeric dp3;
  viennagrid_numeric dp4;
  viennagrid_numeric dp5;

  viennagrid_point_distance_2(dimension, p1p1, p1p2, &dp1);
  viennagrid_point_distance_2(dimension, p2p1, p2p2, &dp2);
  viennagrid_point_distance_2(dimension, p3p1, p3p2, &dp3);
  viennagrid_point_distance_2(dimension, p4p1, p4p2, &dp4);
  viennagrid_point_distance_2(dimension, p5p1, p5p2, &dp5);

  if ((dp1 <= dp2) && (dp1 <= dp3) && (dp1 <= dp4) && (dp1 <= dp5))
  {
    if (result1) viennagrid_point_copy(dimension, p1p1, result1);
    if (result2) viennagrid_point_copy(dimension, p1p2, result2);
  }
  else if ((dp2 <= dp1) && (dp2 <= dp3) && (dp2 <= dp4) && (dp2 <= dp5))
  {
    if (result1) viennagrid_point_copy(dimension, p2p1, result1);
    if (result2) viennagrid_point_copy(dimension, p2p2, result2);
  }
  else if ((dp3 <= dp1) && (dp3 <= dp2) && (dp3 <= dp4) && (dp3 <= dp5))
  {
    if (result1) viennagrid_point_copy(dimension, p3p1, result1);
    if (result2) viennagrid_point_copy(dimension, p3p2, result2);
  }
  else if ((dp4 <= dp1) && (dp4 <= dp2) && (dp4 <= dp3) && (dp4 <= dp5))
  {
    if (result1) viennagrid_point_copy(dimension, p4p1, result1);
    if (result2) viennagrid_point_copy(dimension, p4p2, result2);
  }
  else
  {
    if (result1) viennagrid_point_copy(dimension, p5p1, result1);
    if (result2) viennagrid_point_copy(dimension, p5p2, result2);
  }
}

void closest_points_shortest_distance_6(viennagrid_dimension dimension,
                                        viennagrid_numeric const * p1p1,
                                        viennagrid_numeric const * p1p2,
                                        viennagrid_numeric const * p2p1,
                                        viennagrid_numeric const * p2p2,
                                        viennagrid_numeric const * p3p1,
                                        viennagrid_numeric const * p3p2,
                                        viennagrid_numeric const * p4p1,
                                        viennagrid_numeric const * p4p2,
                                        viennagrid_numeric const * p5p1,
                                        viennagrid_numeric const * p5p2,
                                        viennagrid_numeric const * p6p1,
                                        viennagrid_numeric const * p6p2,
                                        viennagrid_numeric * result1,
                                        viennagrid_numeric * result2)
{
  if (!result1 && !result2) return;

  viennagrid_numeric dp1;
  viennagrid_numeric dp2;
  viennagrid_numeric dp3;
  viennagrid_numeric dp4;
  viennagrid_numeric dp5;
  viennagrid_numeric dp6;

  viennagrid_point_distance_2(dimension, p1p1, p1p2, &dp1);
  viennagrid_point_distance_2(dimension, p2p1, p2p2, &dp2);
  viennagrid_point_distance_2(dimension, p3p1, p3p2, &dp3);
  viennagrid_point_distance_2(dimension, p4p1, p4p2, &dp4);
  viennagrid_point_distance_2(dimension, p5p1, p5p2, &dp5);
  viennagrid_point_distance_2(dimension, p5p1, p5p2, &dp6);

  if ((dp1 <= dp2) && (dp1 <= dp3) && (dp1 <= dp4) && (dp1 <= dp5) && (dp1 <= dp6))
  {
    if (result1) viennagrid_point_copy(dimension, p1p1, result1);
    if (result2) viennagrid_point_copy(dimension, p1p2, result2);
  }
  else if ((dp2 <= dp1) && (dp2 <= dp3) && (dp2 <= dp4) && (dp2 <= dp5) && (dp2 <= dp6))
  {
    if (result1) viennagrid_point_copy(dimension, p2p1, result1);
    if (result2) viennagrid_point_copy(dimension, p2p2, result2);
  }
  else if ((dp3 <= dp1) && (dp3 <= dp2) && (dp3 <= dp4) && (dp3 <= dp5) && (dp3 <= dp6))
  {
    if (result1) viennagrid_point_copy(dimension, p3p1, result1);
    if (result2) viennagrid_point_copy(dimension, p3p2, result2);
  }
  else if ((dp4 <= dp1) && (dp4 <= dp2) && (dp4 <= dp3) && (dp4 <= dp5) && (dp4 <= dp6))
  {
    if (result1) viennagrid_point_copy(dimension, p4p1, result1);
    if (result2) viennagrid_point_copy(dimension, p4p2, result2);
  }
  else if ((dp5 <= dp1) && (dp5 <= dp2) && (dp5 <= dp3) && (dp5 <= dp4) && (dp5 <= dp6))
  {
    if (result1) viennagrid_point_copy(dimension, p5p1, result1);
    if (result2) viennagrid_point_copy(dimension, p5p2, result2);
  }
  else
  {
    if (result1) viennagrid_point_copy(dimension, p6p1, result1);
    if (result2) viennagrid_point_copy(dimension, p6p2, result2);
  }
}







viennagrid_error viennagrid_closest_point_on_line(viennagrid_dimension dimension,
                                                  viennagrid_numeric const * point,
                                                  viennagrid_numeric const * line_point1,
                                                  viennagrid_numeric const * line_point2,
                                                  viennagrid_numeric * result)
{
  std::vector<viennagrid_numeric> pmp1(dimension);
  std::vector<viennagrid_numeric> p2mp1(dimension);

  viennagrid_point_subtract(dimension, point, line_point1, &pmp1[0]);
  viennagrid_point_subtract(dimension, line_point2, line_point1, &p2mp1[0]);

  viennagrid_numeric t;
  viennagrid_numeric tmp1;

//   coord_type t = viennagrid::inner_prod( (p - line_p1), (line_p2 - line_p1) ) / viennagrid::inner_prod(line_p2 - line_p1, line_p2 - line_p1);
  viennagrid_point_inner_prod(dimension, &pmp1[0], &p2mp1[0], &t);
  viennagrid_point_inner_prod(dimension, &p2mp1[0], &p2mp1[0], &tmp1);
  t /= tmp1;


  t = std::max<viennagrid_numeric>(0, std::min<viennagrid_numeric>(1, t));

  // LinePointT p_prime = line_p1 + t * (line_p2 - line_p1);  //closest point to p on line
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = line_point1[+i] + t * (line_point2[+i] - line_point1[+i]);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_closest_points_line_line(viennagrid_dimension dimension,
                                                     viennagrid_numeric const * line1_point1,
                                                     viennagrid_numeric const * line1_point2,
                                                     viennagrid_numeric const * line2_point1,
                                                     viennagrid_numeric const * line2_point2,
                                                     viennagrid_numeric * result1,
                                                     viennagrid_numeric * result2)
{
  viennagrid_numeric const * v0 = line1_point1;
  viennagrid_numeric const * v1 = line1_point2;

  viennagrid_numeric const * w0 = line2_point1;
  viennagrid_numeric const * w1 = line2_point2;


  std::vector<viennagrid_numeric> dir_v(dimension);
  std::vector<viennagrid_numeric> dir_w(dimension);

  viennagrid_point_subtract(dimension, v1, v0, &dir_v[0]);
  viennagrid_point_subtract(dimension, w1, w0, &dir_w[0]);


  viennagrid_numeric v_in_v;
  viennagrid_numeric v_in_w;
  viennagrid_numeric w_in_w;

  viennagrid_point_inner_prod(dimension, &dir_v[0], &dir_v[0], &v_in_v);
  viennagrid_point_inner_prod(dimension, &dir_v[0], &dir_w[0], &v_in_w);
  viennagrid_point_inner_prod(dimension, &dir_w[0], &dir_w[0], &w_in_w);

  viennagrid_numeric denominator = v_in_v * w_in_w - v_in_w * v_in_w;


  if (denominator < VIENNAGRID_TOLERANCE_EPSILON * v_in_v * w_in_w) //lines parallel (up to round-off)
  {
    std::vector<viennagrid_numeric> p0(dimension);
    std::vector<viennagrid_numeric> p1(dimension);
    std::vector<viennagrid_numeric> p2(dimension);
    std::vector<viennagrid_numeric> p3(dimension);

    viennagrid_closest_point_on_line(dimension, w0, v0, v1, &p0[0]);
    viennagrid_closest_point_on_line(dimension, w1, v0, v1, &p1[0]);
    viennagrid_closest_point_on_line(dimension, v0, w0, w1, &p2[0]);
    viennagrid_closest_point_on_line(dimension, v1, w0, w1, &p3[0]);

    closest_points_shortest_distance_4(dimension,
                                       w0, &p0[0],
                                       w1, &p1[0],
                                       v0, &p2[0],
                                       v1, &p3[0],
                                       result1, result2);
    return VIENNAGRID_SUCCESS;
  }


  std::vector<viennagrid_numeric> dir_distance(dimension);
  viennagrid_point_subtract(dimension, v0, w0, &dir_distance[0]);

  viennagrid_numeric v_in_dir_distance;
  viennagrid_numeric w_in_dir_distance;

  viennagrid_point_inner_prod(dimension, &dir_v[0], &dir_distance[0], &v_in_dir_distance);
  viennagrid_point_inner_prod(dimension, &dir_w[0], &dir_distance[0], &w_in_dir_distance);

  viennagrid_numeric s = (v_in_w * w_in_dir_distance - w_in_w * v_in_dir_distance) / denominator;
  viennagrid_numeric t = (v_in_v * w_in_dir_distance - v_in_w * v_in_dir_distance) / denominator;


  if ( (s < 0) || (s > 1) || (t < 0) || (t > 1) ) //Note: A more fine-grained check is possible for those looking for optimizations
  {
    std::vector<viennagrid_numeric> p0(dimension);
    std::vector<viennagrid_numeric> p1(dimension);
    std::vector<viennagrid_numeric> p2(dimension);
    std::vector<viennagrid_numeric> p3(dimension);

    viennagrid_closest_point_on_line(dimension, w0, v0, v1, &p0[0]);
    viennagrid_closest_point_on_line(dimension, w1, v0, v1, &p1[0]);
    viennagrid_closest_point_on_line(dimension, v0, w0, w1, &p2[0]);
    viennagrid_closest_point_on_line(dimension, v1, w0, w1, &p3[0]);

    closest_points_shortest_distance_4(dimension,
                                       w0, &p0[0],
                                       w1, &p1[0],
                                       v0, &p2[0],
                                       v1, &p3[0],
                                       result1, result2);
    return VIENNAGRID_SUCCESS;
  }


  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result1[+i] = v0[+i] + s * dir_v[+i];

  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result2[+i] = w0[+i] + s * dir_w[+i];

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_closest_point_on_triangle(viennagrid_dimension dimension,
                                                      viennagrid_numeric const * point,
                                                      viennagrid_numeric const * triangle_point1,
                                                      viennagrid_numeric const * triangle_point2,
                                                      viennagrid_numeric const * triangle_point3,
                                                      viennagrid_numeric * result)
{

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

  viennagrid_numeric const * p = point;
  viennagrid_numeric const * v0 = triangle_point1;
  viennagrid_numeric const * v1 = triangle_point2;
  viennagrid_numeric const * v2 = triangle_point3;

  std::vector<viennagrid_numeric> u0(dimension);
  std::vector<viennagrid_numeric> u1(dimension);
  std::vector<viennagrid_numeric> u(dimension);

  viennagrid_point_subtract(dimension, v1, v0, &u0[0]);
  viennagrid_point_subtract(dimension, v2, v0, &u1[0]);
  viennagrid_point_subtract(dimension, p, v0, &u[0]);


  viennagrid_numeric a;
  viennagrid_numeric b;
  viennagrid_numeric d;
  viennagrid_numeric u_in_u0;
  viennagrid_numeric u_in_u1;

  viennagrid_point_inner_prod(dimension, &u0[0], &u0[0], &a);
  viennagrid_point_inner_prod(dimension, &u0[0], &u1[0], &b);
  viennagrid_point_inner_prod(dimension, &u1[0], &u1[0], &d);
  viennagrid_point_inner_prod(dimension, &u[0], &u0[0], &u_in_u0);
  viennagrid_point_inner_prod(dimension, &u[0], &u1[0], &u_in_u1);

  viennagrid_numeric denominator = a * d - b * b;

  if (denominator < VIENNAGRID_TOLERANCE_EPSILON * a * d) //triangle is VERY thin
    return VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO;

  viennagrid_numeric s = (  d * u_in_u0 - b * u_in_u1) / denominator;
  viennagrid_numeric t = (- b * u_in_u0 + a * u_in_u1) / denominator;

  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v0[+i] + s*u0[+i] + t*u1[+i];

  // nonzero distance is encountered only if p_prime is outside the triangle
  if (    (s < 0 || s > 1)
        || (t < 0 || t > 1)
        || (s + t > 1) )     //p_prime is outside the triangle
  {
    std::vector<viennagrid_numeric> p0;
    std::vector<viennagrid_numeric> p1;
    std::vector<viennagrid_numeric> p2;

    viennagrid_closest_point_on_line(dimension, result, v0, v1, &p0[0]);
    viennagrid_closest_point_on_line(dimension, result, v0, v2, &p1[0]);
    viennagrid_closest_point_on_line(dimension, result, v1, v2, &p2[0]);

    closest_points_shortest_distance_3(dimension, p, &p0[0], p, &p1[0], p, &p2[0], NULL, result);
  }

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_closest_point_on_quadrilateral(viennagrid_dimension dimension,
                                                           viennagrid_numeric const * point,
                                                           viennagrid_numeric const * quadrilateral_point0,
                                                           viennagrid_numeric const * quadrilateral_point1,
                                                           viennagrid_numeric const * quadrilateral_point2,
                                                           viennagrid_numeric const * quadrilateral_point3,
                                                           viennagrid_numeric * result)
{
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

  std::vector<viennagrid_numeric> p0(dimension);
  std::vector<viennagrid_numeric> p1(dimension);

  viennagrid_closest_point_on_triangle(dimension, point, quadrilateral_point0, quadrilateral_point1, quadrilateral_point2, &p0[0]);
  viennagrid_closest_point_on_triangle(dimension, point, quadrilateral_point1, quadrilateral_point2, quadrilateral_point3, &p1[0]);

  closest_points_shortest_distance_2(dimension, point, &p0[0], point, &p1[0], NULL, result);

  return VIENNAGRID_SUCCESS;
}







viennagrid_error viennagrid_closest_point_on_tetrahedron(viennagrid_dimension dimension,
                                                         viennagrid_numeric const * point,
                                                         viennagrid_numeric const * tetrahedron_point0,
                                                         viennagrid_numeric const * tetrahedron_point1,
                                                         viennagrid_numeric const * tetrahedron_point2,
                                                         viennagrid_numeric const * tetrahedron_point3,
                                                         viennagrid_numeric * result)
{
  // write T(s) =  v0 + r * (v1 - v0) + s * (v2 - v0) + t * (v3 - v1), {r,s,t} \in [0,1], r+s+t < 1 for the tetrahedron T
  //            =: v0 + r * u0 + s * u1 + t * u2
  //
  // Projection p' of p is given by the solution of the system
  //
  //  <u0, u0> * r + <u1, u0> * s + <u2, u0> * t = <u, u0>
  //  <u0, u1> * r + <u1, u1> * s + <u2, u1> * t = <u, u1>
  //  <u0, u2> * r + <u1, u2> * s + <u2, u2> * t = <u, u2>
  //
  // where u = p - v0. This is a 3x3-system that is directly inverted using Cramer's rule.
  //

  viennagrid_numeric const * p = point;
  viennagrid_numeric const * v0 = tetrahedron_point0;
  viennagrid_numeric const * v1 = tetrahedron_point1;
  viennagrid_numeric const * v2 = tetrahedron_point2;
  viennagrid_numeric const * v3 = tetrahedron_point3;

  std::vector<viennagrid_numeric> u0(dimension);
  std::vector<viennagrid_numeric> u1(dimension);
  std::vector<viennagrid_numeric> u2(dimension);
  std::vector<viennagrid_numeric> u(dimension);

  viennagrid_point_subtract(dimension, v1, v0, &u0[0]);
  viennagrid_point_subtract(dimension, v2, v0, &u1[0]);
  viennagrid_point_subtract(dimension, v3, v0, &u2[0]);
  viennagrid_point_subtract(dimension, p, v0, &u[0]);


  viennagrid_numeric a_00;
  viennagrid_numeric a_01;
  viennagrid_numeric a_02;

  viennagrid_point_inner_prod(dimension, &u0[0], &u0[0], &a_00);
  viennagrid_point_inner_prod(dimension, &u1[0], &u0[0], &a_01);
  viennagrid_point_inner_prod(dimension, &u2[0], &u0[0], &a_02);

  viennagrid_numeric a_10 = a_01;
  viennagrid_numeric a_11;
  viennagrid_numeric a_12;

  viennagrid_point_inner_prod(dimension, &u1[0], &u1[0], &a_11);
  viennagrid_point_inner_prod(dimension, &u1[0], &u2[0], &a_12);

  viennagrid_numeric a_20 = a_02;
  viennagrid_numeric a_21 = a_12;
  viennagrid_numeric a_22;

  viennagrid_point_inner_prod(dimension, &u2[0], &u2[0], &a_22);



  viennagrid_numeric u_in_u0;
  viennagrid_numeric u_in_u1;
  viennagrid_numeric u_in_u2;

  viennagrid_point_inner_prod(dimension, &u[0], &u0[0], &u_in_u0);
  viennagrid_point_inner_prod(dimension, &u[0], &u0[0], &u_in_u1);
  viennagrid_point_inner_prod(dimension, &u[0], &u0[0], &u_in_u2);


  viennagrid_numeric det_A =  a_00 * a_11 * a_22 + a_01 * a_12 * a_20 + a_02 * a_10 * a_21
                            - a_20 * a_11 * a_02 - a_21 * a_12 * a_00 - a_22 * a_10 * a_01;

  if (det_A < VIENNAGRID_TOLERANCE_EPSILON * std::sqrt(a_00 * a_11 * a_22)) //tetrahedron is VERY thin:
    return VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO;



  //     | <u, u0>   a_01   a_02  |
  // r = | <u, u1>   a_11   a_12  |  / det_A  and similarly for s and t
  //     | <u, u2>   a_21   a_22  |
  viennagrid_numeric r = (  u_in_u0 * a_11 * a_22 + a_01 * a_12 * u_in_u2 + a_02 * u_in_u1 * a_21
                          - u_in_u2 * a_11 * a_02 - a_21 * a_12 * u_in_u0 - a_22 * u_in_u1 * a_01 ) / det_A;
  viennagrid_numeric s = (  a_00 * u_in_u1 * a_22 + u_in_u0 * a_12 * a_20 + a_02 * a_10 * u_in_u2
                          - a_20 * u_in_u1 * a_02 - u_in_u2 * a_12 * a_00 - a_22 * a_10 * u_in_u0 ) / det_A;
  viennagrid_numeric t = (  a_00 * a_11 * u_in_u2 + a_01 * u_in_u1 * a_20 + u_in_u0 * a_10 * a_21
                          - a_20 * a_11 * u_in_u0 - a_21 * u_in_u1 * a_00 - u_in_u2 * a_10 * a_01 ) / det_A;



  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v0[+i] + r*u0[+i] + s*u1[+i] + t*u2[+i];

  // nonzero distance is encountered only if p_prime is outside the triangle
  if (    (r < 0 || r > 1)
        || (s < 0 || s > 1)
        || (t < 0 || t > 1)
        || (r + s + t > 1) )     //p_prime is outside the triangle
  {
    std::vector<viennagrid_numeric> p0;
    std::vector<viennagrid_numeric> p1;
    std::vector<viennagrid_numeric> p2;
    std::vector<viennagrid_numeric> p3;

    viennagrid_closest_point_on_triangle(dimension, result, v0, v1, v2, &p0[0]);
    viennagrid_closest_point_on_triangle(dimension, result, v0, v1, v3, &p1[0]);
    viennagrid_closest_point_on_triangle(dimension, result, v0, v2, v3, &p2[0]);
    viennagrid_closest_point_on_triangle(dimension, result, v1, v2, v3, &p3[0]);

    closest_points_shortest_distance_4(dimension, p, &p0[0], p, &p1[0], p, &p2[0], p, &p3[0], NULL, result);
  }

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_closest_point_on_hexahedron(viennagrid_dimension dimension,
                                                        viennagrid_numeric const * point,
                                                        viennagrid_numeric const * hexahedron_point0,
                                                        viennagrid_numeric const * hexahedron_point1,
                                                        viennagrid_numeric const * hexahedron_point2,
                                                        viennagrid_numeric const * hexahedron_point3,
                                                        viennagrid_numeric const * hexahedron_point4,
                                                        viennagrid_numeric const * hexahedron_point5,
                                                        viennagrid_numeric const * hexahedron_point6,
                                                        viennagrid_numeric const * hexahedron_point7,
                                                        viennagrid_numeric * result)
{
  std::vector<viennagrid_numeric> p0(dimension);
  std::vector<viennagrid_numeric> p1(dimension);
  std::vector<viennagrid_numeric> p2(dimension);
  std::vector<viennagrid_numeric> p3(dimension);
  std::vector<viennagrid_numeric> p4(dimension);
  std::vector<viennagrid_numeric> p5(dimension);

  viennagrid_closest_point_on_tetrahedron(dimension, point, hexahedron_point0, hexahedron_point1, hexahedron_point3, hexahedron_point7, &p0[0]);
  viennagrid_closest_point_on_tetrahedron(dimension, point, hexahedron_point0, hexahedron_point1, hexahedron_point7, hexahedron_point5, &p1[0]);
  viennagrid_closest_point_on_tetrahedron(dimension, point, hexahedron_point0, hexahedron_point2, hexahedron_point6, hexahedron_point7, &p2[0]);
  viennagrid_closest_point_on_tetrahedron(dimension, point, hexahedron_point0, hexahedron_point3, hexahedron_point2, hexahedron_point7, &p3[0]);
  viennagrid_closest_point_on_tetrahedron(dimension, point, hexahedron_point0, hexahedron_point5, hexahedron_point7, hexahedron_point4, &p4[0]);
  viennagrid_closest_point_on_tetrahedron(dimension, point, hexahedron_point0, hexahedron_point6, hexahedron_point4, hexahedron_point7, &p5[0]);

  closest_points_shortest_distance_6(dimension, point, &p0[0], point, &p1[0], point, &p2[0], point, &p3[0], point, &p4[0], point, &p5[0], NULL, result);

  return VIENNAGRID_SUCCESS;
}






viennagrid_error viennagrid_closest_point_on_element(viennagrid_mesh mesh,
                                                     viennagrid_numeric const * point,
                                                     viennagrid_element_id element_id,
                                                     viennagrid_numeric * result)
{
  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dimension) );

  viennagrid_element_type element_type;
  RETURN_ON_ERROR( viennagrid_element_type_get(mesh, element_id, &element_type) );

  if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
  {
    viennagrid_numeric * point_coords;
    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, element_id, &point_coords) );
    viennagrid_point_copy(dimension, point_coords, result);
    return VIENNAGRID_SUCCESS;
  }


  viennagrid_element_id *vertex_ids_begin, *vertex_ids_end;
  RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end) );

  std::vector<viennagrid_numeric *> point_coords(vertex_ids_end - vertex_ids_begin);
  for (viennagrid_element_id *it = vertex_ids_begin; it<vertex_ids_end; ++it)
    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *it, &(point_coords[it - vertex_ids_begin])) );


  switch (element_type)
  {
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      viennagrid_closest_point_on_line(dimension, point, point_coords[0], point_coords[1], result);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      viennagrid_closest_point_on_triangle(dimension, point, point_coords[0], point_coords[1], point_coords[2], result);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
    {
      viennagrid_closest_point_on_quadrilateral(dimension, point, point_coords[0], point_coords[1], point_coords[2], point_coords[3], result);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      viennagrid_closest_point_on_tetrahedron(dimension, point, point_coords[0], point_coords[1], point_coords[2], point_coords[3], result);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      viennagrid_closest_point_on_hexahedron(dimension, point, point_coords[0], point_coords[1], point_coords[2], point_coords[3],
                                                               point_coords[4], point_coords[5], point_coords[6], point_coords[7], result);
      break;
    }

    default:
      return VIENNAGRID_ERROR_UNSUPPORTED_ELEMENT_TYPE;
  }

  return VIENNAGRID_SUCCESS;
}






















