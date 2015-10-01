#include "viennagrid/viennagrid.h"
#include "../common.hpp"

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_interpolate(viennagrid_mesh mesh,
                                                                           viennagrid_quantity_field src_quantity_field,
                                                                           viennagrid_dimension dst_topological_dimension,
                                                                           viennagrid_quantity_field dst_quantity_field)
{
  viennagrid_dimension src_quantity_field_dimension;
  RETURN_ON_ERROR( viennagrid_quantity_field_topological_dimension_get(src_quantity_field, &src_quantity_field_dimension) );

  viennagrid_dimension src_cell_dimension;
  RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &src_cell_dimension) );

  viennagrid_int values_type;
  RETURN_ON_ERROR( viennagrid_quantity_field_values_type_get(src_quantity_field, &values_type) );

  viennagrid_int values_per_quantity;
  RETURN_ON_ERROR( viennagrid_quantity_field_values_per_quantity_get(src_quantity_field, &values_per_quantity) );

  if (values_type != VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD_VALUE_TYPE;

  RETURN_ON_ERROR( viennagrid_quantity_field_init(dst_quantity_field, dst_topological_dimension, values_type, values_per_quantity, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE) );

  viennagrid_element_id * dst_elements_begin;
  viennagrid_element_id * dst_elements_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, dst_topological_dimension, &dst_elements_begin, &dst_elements_end) );

  for (viennagrid_element_id * deit = dst_elements_begin; deit != dst_elements_end; ++deit)
  {
    viennagrid_numeric total_volume = 0;
    std::vector<viennagrid_numeric> dst_data(values_per_quantity, 0);


    viennagrid_element_id * src_elements_begin;
    viennagrid_element_id * src_elements_end;
    if (dst_topological_dimension > src_quantity_field_dimension)
    {
      RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, *deit, src_cell_dimension, &src_elements_begin, &src_elements_end) );
    }
    else
    {
      RETURN_ON_ERROR( viennagrid_element_coboundary_elements(mesh, *deit, src_cell_dimension, &src_elements_begin, &src_elements_end) );
    }

    for (viennagrid_element_id * seit = src_elements_begin; seit != src_elements_end; ++seit)
    {
      viennagrid_numeric current_volume;
      RETURN_ON_ERROR( viennagrid_element_volume(mesh, *seit, &current_volume) );

      viennagrid_numeric * src_data;
      RETURN_ON_ERROR( viennagrid_quantity_field_value_get(src_quantity_field, *seit, (void**)&src_data) );

      total_volume += current_volume;
      for (viennagrid_int i = 0; i != values_per_quantity; ++i)
        dst_data[i] += src_data[i]*current_volume;
    }

    for (viennagrid_int i = 0; i != values_per_quantity; ++i)
      dst_data[i] /= total_volume;

    RETURN_ON_ERROR( viennagrid_quantity_field_value_set(dst_quantity_field, *deit, &dst_data[0]) );
  }

  return VIENNAGRID_SUCCESS;
}







VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_interpolate_simplex_to_point(viennagrid_mesh mesh,
                                                                                            viennagrid_element_id element_id,
                                                                                            viennagrid_quantity_field quantity_field,
                                                                                            viennagrid_numeric const * point,
                                                                                            viennagrid_numeric * data)
{
  viennagrid_element_type element_type;
  viennagrid_element_type_get(mesh, element_id, &element_type);

  viennagrid_dimension dimension;
  viennagrid_mesh_geometric_dimension_get(mesh, &dimension);

  viennagrid_int values_count;
  viennagrid_quantity_field_values_per_quantity_get(quantity_field, &values_count);

  if ((element_type != VIENNAGRID_ELEMENT_TYPE_LINE) ||
      (element_type != VIENNAGRID_ELEMENT_TYPE_TRIANGLE) ||
      (element_type != VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON))
    return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;


  viennagrid_element_id *vertex_ids_begin, *vertex_ids_end;
  RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end) );

  std::vector<viennagrid_numeric const *> point_coords(vertex_ids_end - vertex_ids_begin);
  for (viennagrid_element_id *it = vertex_ids_begin; it<vertex_ids_end; ++it)
    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *it, (viennagrid_numeric**)&(point_coords[it - vertex_ids_begin])) );

  {
    viennagrid_numeric total_volume;
    viennagrid_spanned_volume(dimension, &point_coords[0], &point_coords[0] + point_coords.size(), &total_volume);

    if (std::abs(total_volume) < VIENNAGRID_TOLERANCE_EPSILON)
      return VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO;

    for (viennagrid_int i = 0; i != values_count; ++i)
      data[i] = 0;

    for (viennagrid_int i = 0; i != (viennagrid_int)point_coords.size(); ++i)
    {
      viennagrid_numeric current_volume;

      std::swap( point, point_coords[i] );
      viennagrid_spanned_volume(dimension, &point_coords[0], &point_coords[0] + point_coords.size(), &current_volume);
      std::swap( point, point_coords[i] );

      current_volume /= total_volume;
      viennagrid_numeric * current_data;
      viennagrid_quantity_field_value_get(quantity_field, *(vertex_ids_begin+i), (void**)&current_data);

      for (viennagrid_int j = 0; j != values_count; ++j)
        data[j] += current_volume * current_data[j];
    }
  }

  return VIENNAGRID_SUCCESS;
}


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_interpolate_to_mesh(viennagrid_mesh src_mesh,
                                                                                   viennagrid_quantity_field src_quantity_field,
                                                                                   viennagrid_mesh dst_mesh,
                                                                                   viennagrid_dimension dst_topological_dimension,
                                                                                   viennagrid_quantity_field dst_quantity_field,
                                                                                   viennagrid_numeric const * default_value)
{
  viennagrid_dimension src_quantity_field_dimension;
  RETURN_ON_ERROR( viennagrid_quantity_field_topological_dimension_get(src_quantity_field, &src_quantity_field_dimension) );

  viennagrid_dimension src_cell_dimension;
  RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(src_mesh, &src_cell_dimension) );

  viennagrid_int values_type;
  RETURN_ON_ERROR( viennagrid_quantity_field_values_type_get(src_quantity_field, &values_type) );

  viennagrid_int values_per_quantity;
  RETURN_ON_ERROR( viennagrid_quantity_field_values_per_quantity_get(src_quantity_field, &values_per_quantity) );

  if (dst_topological_dimension != 0) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (src_quantity_field_dimension != src_cell_dimension) return VIENNAGRID_ERROR_TOPOLOGICAL_DIMENSION_MISMATCH;
  if (values_type != VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD_VALUE_TYPE;

  RETURN_ON_ERROR( viennagrid_quantity_field_init(dst_quantity_field, dst_topological_dimension, values_type, values_per_quantity, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE) );

  viennagrid_element_id * src_element_begin;
  viennagrid_element_id * src_element_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(src_mesh, src_cell_dimension, &src_element_begin, &src_element_end) );

  viennagrid_element_id * dst_element_begin;
  viennagrid_element_id * dst_element_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(dst_mesh, dst_topological_dimension, &dst_element_begin, &dst_element_end) );

  for (viennagrid_element_id * deit = dst_element_begin; deit != dst_element_end; ++deit)
  {
    viennagrid_numeric * coords;
    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(dst_mesh, src_cell_dimension, &coords) );

    viennagrid_element_id * seit = src_element_begin;
    for (; seit != src_element_end; ++seit)
    {
      viennagrid_bool is_inside;
      RETURN_ON_ERROR( viennagrid_point_in_element(src_mesh, *seit, coords, VIENNAGRID_TOLERANCE_EPSILON, &is_inside) );
      if (is_inside == VIENNAGRID_TRUE)
        break;
    }

    if (seit == src_element_end)
    {
      if (default_value)
        RETURN_ON_ERROR( viennagrid_quantity_field_value_set(dst_quantity_field, *deit, default_value) );
    }
    else
    {
      viennagrid_numeric * data;
      RETURN_ON_ERROR( viennagrid_quantity_field_value_get_or_create(dst_quantity_field, *deit, (void**)&data) );
      RETURN_ON_ERROR( viennagrid_quantity_interpolate_simplex_to_point(src_mesh, *seit, src_quantity_field, coords, data) );
    }
  }

  return VIENNAGRID_SUCCESS;
}
