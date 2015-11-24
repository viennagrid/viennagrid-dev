#include "viennagrid/viennagrid.h"
#include "../common.hpp"
#include <cmath>


viennagrid_error viennagrid_point_in_simplex_2(viennagrid_dimension dimension,
                                               viennagrid_numeric const * point,
                                               viennagrid_numeric const * line_point0,
                                               viennagrid_numeric const * line_point1,
                                               viennagrid_numeric tolerance,
                                               viennagrid_bool * is_inside)
{
  if (is_inside)
  {
    viennagrid_numeric line_length;
    viennagrid_numeric distance_line_point0;
    viennagrid_numeric distance_line_point1;

    RETURN_ON_ERROR( viennagrid_distance_2(dimension, line_point0, line_point1, &line_length) );
    RETURN_ON_ERROR( viennagrid_distance_2(dimension, line_point0, point, &distance_line_point0) );
    RETURN_ON_ERROR( viennagrid_distance_2(dimension, line_point1, point, &distance_line_point1) );

    *is_inside = std::abs(line_length - (distance_line_point0+distance_line_point1)) < tolerance * line_length;
  }

  return VIENNAGRID_SUCCESS;
}



viennagrid_numeric sign_2(viennagrid_numeric const * p1, viennagrid_numeric const * p2, viennagrid_numeric const* p3)
{
    return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1]);
}

viennagrid_error viennagrid_point_in_simplex_3(viennagrid_dimension dimension,
                                               viennagrid_numeric const * point,
                                               viennagrid_numeric const * triangle_point0,
                                               viennagrid_numeric const * triangle_point1,
                                               viennagrid_numeric const * triangle_point2,
                                               viennagrid_numeric tolerance,
                                               viennagrid_bool * is_inside)
{
//   PointT v0 = tp2-tp0;
//   PointT v1 = tp1-tp0;
//   PointT v2 = p-tp0;
//
//   NumericType dot00 = viennagrid::inner_prod(v0, v0);
//   NumericType dot01 = viennagrid::inner_prod(v0, v1);
//   NumericType dot02 = viennagrid::inner_prod(v0, v2);
//   NumericType dot11 = viennagrid::inner_prod(v1, v1);
//   NumericType dot12 = viennagrid::inner_prod(v1, v2);
//
//   NumericType denom = static_cast<NumericType>(1) / (dot00 * dot11 - dot01 * dot01);
//   NumericType u = (dot11*dot02 - dot01*dot12) * denom;
//   NumericType v = (dot00*dot12 - dot01*dot02) * denom;
//
//   NumericType abs_eps = absolute_tolerance<NumericType>(numeric_config);
//   return (u >= -abs_eps) && (v >= -abs_eps) && (u+v <= static_cast<NumericType>(1) + NumericType(2.0)*abs_eps );

  if (is_inside)
  {
    if (dimension == 2)
    {
      bool b1, b2, b3;

      b1 = sign_2(point, triangle_point0, triangle_point1) < 0;
      b2 = sign_2(point, triangle_point1, triangle_point2) < 0;
      b3 = sign_2(point, triangle_point2, triangle_point0) < 0;

      *is_inside = ((b1 == b2) && (b2 == b3));
    }
    else
    {
      std::vector<viennagrid_numeric> v0(dimension);
      std::vector<viennagrid_numeric> v1(dimension);
      std::vector<viennagrid_numeric> v2(dimension);

      RETURN_ON_ERROR( viennagrid_subtract(dimension, triangle_point2, triangle_point0, &v0[0]) );
      RETURN_ON_ERROR( viennagrid_subtract(dimension, triangle_point1, triangle_point0, &v1[0]) );
      RETURN_ON_ERROR( viennagrid_subtract(dimension, point, triangle_point0, &v2[0]) );

      viennagrid_numeric dot00;
      viennagrid_numeric dot01;
      viennagrid_numeric dot02;
      viennagrid_numeric dot11;
      viennagrid_numeric dot12;

      RETURN_ON_ERROR( viennagrid_inner_prod(dimension, &v0[0], &v0[0], &dot00) );
      RETURN_ON_ERROR( viennagrid_inner_prod(dimension, &v0[0], &v1[0], &dot01) );
      RETURN_ON_ERROR( viennagrid_inner_prod(dimension, &v0[0], &v2[0], &dot02) );
      RETURN_ON_ERROR( viennagrid_inner_prod(dimension, &v1[0], &v1[0], &dot11) );
      RETURN_ON_ERROR( viennagrid_inner_prod(dimension, &v1[0], &v2[0], &dot12) );

      viennagrid_numeric denom = (dot00*dot11 - dot01*dot01);
      if (std::abs(denom) < VIENNAGRID_TOLERANCE_EPSILON)
        return VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO;

      viennagrid_numeric u = (dot11*dot02 - dot01*dot12) / denom;
      viennagrid_numeric v = (dot00*dot12 - dot01*dot02) / denom;

      *is_inside = (u >= -tolerance) && (v >= -tolerance) && (u+v <= 1+2*tolerance);
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_point_in_simplex_4(viennagrid_dimension dimension,
                                               viennagrid_numeric const * point,
                                               viennagrid_numeric const * tetrahedron_point0,
                                               viennagrid_numeric const * tetrahedron_point1,
                                               viennagrid_numeric const * tetrahedron_point2,
                                               viennagrid_numeric const * tetrahedron_point3,
                                               viennagrid_numeric tolerance,
                                               viennagrid_bool * is_inside)
{
//   typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
//
//   NumericType denom = static_cast<NumericType>(1) / spanned_volume(tp0, tp1, tp2, tp3);
//
//   NumericType A = spanned_volume(  p,tp1,tp2,tp3) * denom;
//   NumericType B = spanned_volume(tp0,  p,tp2,tp3) * denom;
//   NumericType C = spanned_volume(tp0,tp1,  p,tp3) * denom;
//   NumericType D = spanned_volume(tp0,tp1,tp2,  p) * denom;
//
//   NumericType abs_eps = absolute_tolerance<NumericType>(numeric_config);
//   return (A >= -abs_eps) && (B >= -abs_eps) && (C >= -abs_eps) && (D >= -abs_eps) && (A+B+C+D <= static_cast<NumericType>(1) + NumericType(2.0)*abs_eps);

  if (is_inside)
  {
    viennagrid_numeric denom;
    viennagrid_numeric A;
    viennagrid_numeric B;
    viennagrid_numeric C;
    viennagrid_numeric D;

    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_4(dimension, tetrahedron_point0, tetrahedron_point1, tetrahedron_point2, tetrahedron_point3, &denom) );
    if (std::abs(denom) < VIENNAGRID_TOLERANCE_EPSILON)
      return VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO;

    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_4(dimension, point, tetrahedron_point1, tetrahedron_point2, tetrahedron_point3, &A) );
    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_4(dimension, tetrahedron_point0, point, tetrahedron_point2, tetrahedron_point3, &B) );
    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_4(dimension, tetrahedron_point0, tetrahedron_point1, point, tetrahedron_point3, &C) );
    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_4(dimension, tetrahedron_point0, tetrahedron_point1, tetrahedron_point2, point, &D) );

    A /= denom;
    B /= denom;
    C /= denom;
    D /= denom;

    *is_inside = (A >= -tolerance) && (B >= -tolerance) && (C >= -tolerance) && (D >= -tolerance) && (A+B+C+D <= 1+4*tolerance);
  }

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_point_in_element(viennagrid_mesh mesh,
                                             viennagrid_element_id element_id,
                                             viennagrid_numeric const * point,
                                             viennagrid_numeric tolerance,
                                             viennagrid_bool * is_inside)
{
  viennagrid_element_type element_type;
  RETURN_ON_ERROR( viennagrid_element_type_get(mesh, element_id, &element_type) );

  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dimension) );

  if (is_inside)
  {
    if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
    {
      viennagrid_numeric * coords;
      viennagrid_numeric distance;
      RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, element_id, &coords) );
      RETURN_ON_ERROR( viennagrid_distance_2(dimension, point, coords, &distance) );
      *is_inside = distance < tolerance;
    }
    else
    {
      viennagrid_element_id *vertex_ids_begin, *vertex_ids_end;
      RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end) );

      std::vector<viennagrid_numeric *> point_coords(vertex_ids_end - vertex_ids_begin);
      for (viennagrid_element_id *it = vertex_ids_begin; it<vertex_ids_end; ++it)
        RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *it, &(point_coords[it - vertex_ids_begin])) );

      switch (element_type)
      {
        case VIENNAGRID_ELEMENT_TYPE_LINE:
        {
          RETURN_ON_ERROR( viennagrid_point_in_simplex_2(dimension, point, point_coords[0], point_coords[1], tolerance, is_inside) );
          break;
        }

        case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
        {
          RETURN_ON_ERROR( viennagrid_point_in_simplex_3(dimension, point, point_coords[0], point_coords[1], point_coords[2], tolerance, is_inside) );
          break;
        }

        case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
        {
          RETURN_ON_ERROR( viennagrid_point_in_simplex_3(dimension, point, point_coords[0], point_coords[1], point_coords[2], tolerance, is_inside) );
          if (!is_inside)
            RETURN_ON_ERROR( viennagrid_point_in_simplex_3(dimension, point, point_coords[1], point_coords[2], point_coords[3], tolerance, is_inside) );
          break;
        }

        case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
        {
          RETURN_ON_ERROR( viennagrid_point_in_simplex_4(dimension, point, point_coords[0], point_coords[1], point_coords[2], point_coords[3], tolerance, is_inside) );
          break;
        }

        default:
        {
          return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
        }
      }
    }
  }

  return VIENNAGRID_SUCCESS;
}
