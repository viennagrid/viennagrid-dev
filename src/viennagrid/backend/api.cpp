#include <cstring>

#include "viennagrid/backend/api.h"
#include "viennagrid/backend/backend.hpp"

viennagrid_error viennagrid_mesh_hierarchy_create(viennagrid_mesh_hierarchy * mesh_hierarchy)
{
  *mesh_hierarchy = new viennagrid_mesh_hierarchy_();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  if (mesh_hierarchy)
    mesh_hierarchy->retain();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  if (mesh_hierarchy)
    mesh_hierarchy->release();

  return VIENNAGRID_SUCCESS;
}

// viennagrid_error viennagrid_mesh_hierarchy_get_cell_tag(viennagrid_mesh_hierarchy mesh_hierarchy,
//                                                         viennagrid_element_tag * cell_tag)
// {
//   *cell_tag = mesh_hierarchy->cell_tag();
//   return VIENNAGRID_SUCCESS;
// }

viennagrid_error viennagrid_mesh_hierarchy_get_cell_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                              viennagrid_dimension * cell_dimension)
{
  *cell_dimension = mesh_hierarchy->cell_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_get_geometric_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_dimension * geometric_dimension)
{
  *geometric_dimension = mesh_hierarchy->geometric_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_set_geometric_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_index geometric_dimension)
{
  mesh_hierarchy->set_geometric_dimension(geometric_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_clear(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  mesh_hierarchy->clear();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_hierarchy_get_root(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                    viennagrid_mesh * mesh)
{
  *mesh = mesh_hierarchy->root();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_get_mesh_hierarchy(viennagrid_mesh mesh,
                                                    viennagrid_mesh_hierarchy * mesh_hierarchy)
{
  *mesh_hierarchy = mesh->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_create(viennagrid_mesh parent,
                                        viennagrid_mesh * mesh)
{
  *mesh = parent->make_child();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh mesh,
                                                viennagrid_int * children_count)
{
  *children_count = mesh->children_count();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_get_child(viennagrid_mesh mesh,
                                           viennagrid_index id,
                                           viennagrid_mesh * child)
{
  *child = mesh->get_child(id);
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_mesh_hierarchy_element_count(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                         viennagrid_element_tag element_tag,
                                                         viennagrid_int * count)
{
  *count = mesh_hierarchy->element_count(element_tag);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_create(viennagrid_mesh_hierarchy hierarchy,
                                        const viennagrid_numeric * coords,
                                        viennagrid_index * vertex_id)
{
  *vertex_id = hierarchy->make_vertex(coords);

  viennagrid_mesh root = hierarchy->root();
  root->add_element(0, *vertex_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                       viennagrid_index id,
                                       viennagrid_numeric ** coords)
{
  *coords = mesh_hierarchy->get_vertex(id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_create(viennagrid_mesh_hierarchy hierarchy,
                                           viennagrid_element_tag element_tag,
                                           viennagrid_int index_count,
                                           viennagrid_index * indices,
                                           viennagrid_dimension * topo_dims,
                                           viennagrid_index * element_id)
{
  *element_id = hierarchy->get_make_element(element_tag, indices, topo_dims, index_count);

  viennagrid_mesh root = hierarchy->root();
  viennagrid_dimension element_topo_dim = viennagrid_topological_dimension(element_tag);

  root->add_element(element_topo_dim, *element_id);

  for (viennagrid_dimension boundary_topo_dim = 0; boundary_topo_dim != element_topo_dim; ++boundary_topo_dim)
  {
    viennagrid_index * boundary = hierarchy->boundary_begin(element_topo_dim, *element_id, boundary_topo_dim);
    viennagrid_index * boundary_end = hierarchy->boundary_end(element_topo_dim, *element_id, boundary_topo_dim);

    for (; boundary != boundary_end; ++boundary)
      root->add_element(boundary_topo_dim, *boundary);
  }

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_create_refinement(viennagrid_mesh mesh,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_index element_id,
                                                      viennagrid_element_tag    refined_element_tag,
                                                      viennagrid_int            refined_element_base_count,
                                                      viennagrid_index *        refined_element_base_indices,
                                                      viennagrid_dimension *    refined_element_base_dimensions,
                                                      viennagrid_int            intersects_count,
                                                      viennagrid_index *        intersect_vertices_indices,
                                                      viennagrid_index *        intersects_indices,
                                                      viennagrid_dimension *    intersects_topo_dims,
                                                      viennagrid_index *        id)
{
  *id = mesh->make_refined_element(element_topo_dim, element_id,
                                   refined_element_tag, refined_element_base_count,
                                   refined_element_base_indices, refined_element_base_dimensions,
                                   intersects_count, intersect_vertices_indices,
                                   intersects_indices, intersects_topo_dims);
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_get_tag(viennagrid_mesh_hierarchy mesh_hierarchy,
                                            viennagrid_dimension element_topo_dim,
                                            viennagrid_index element_id,
                                            viennagrid_element_tag * element_tag)
{
  *element_tag = mesh_hierarchy->element_buffer(element_topo_dim).element_tag(element_id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy hierarchy,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_index element_id,
                                                      viennagrid_dimension boundary_topo_dim,
                                                      viennagrid_index ** boundary_element_index_begin,
                                                      viennagrid_index ** boundary_element_index_end)
{
//   element_tag = hierarchy->unpack_element_tag(element_tag);
//   boundary_element_tag = hierarchy->unpack_element_tag(boundary_element_tag);

  *boundary_element_index_begin = hierarchy->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);
  *boundary_element_index_end = hierarchy->boundary_end(element_topo_dim, element_id, boundary_topo_dim);

//   hierarchy->element_buffer(element_topo_dim).boundary_indices_begin(boundary_topo_dim, element_id);
//   *boundary_element_index_end = hierarchy->element_buffer(element_topo_dim).boundary_indices_end(boundary_topo_dim, element_id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                        viennagrid_dimension element_topo_dim,
                                                        viennagrid_index element_id,
                                                        viennagrid_dimension coboundary_topo_dim,
                                                        viennagrid_index ** coboundary_element_index_begin,
                                                        viennagrid_index ** coboundary_element_index_end)
{
//   element_tag = mesh->unpack_element_tag(element_tag);
//   coboundary_element_tag = mesh->unpack_element_tag(coboundary_element_tag);

//   mesh->make_coboundary(element_tag, coboundary_element_tag);
  *coboundary_element_index_begin = mesh->coboundary_begin(element_topo_dim, element_id, coboundary_topo_dim);
  *coboundary_element_index_end = mesh->coboundary_end(element_topo_dim, element_id, coboundary_topo_dim);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_index element_id,
                                                      viennagrid_dimension connector_topo_dim,
                                                      viennagrid_dimension neighbor_topo_dim,
                                                      viennagrid_index ** neighbor_element_index_begin,
                                                      viennagrid_index ** neighbor_element_index_end)
{
//   element_tag = mesh->unpack_element_tag(element_tag);
//   connector_element_tag = mesh->unpack_element_tag(connector_element_tag);
//   neighbor_element_tag = mesh->unpack_element_tag(neighbor_element_tag);

  *neighbor_element_index_begin = mesh->neighbor_begin(element_topo_dim, element_id, connector_topo_dim, neighbor_topo_dim);
  *neighbor_element_index_end = mesh->neighbor_end(element_topo_dim, element_id, connector_topo_dim, neighbor_topo_dim);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh mesh,
                                             viennagrid_dimension element_topo_dim,
                                             viennagrid_index element_id,
                                             viennagrid_bool * result)
{
//   element_tag = mesh->unpack_element_tag(element_tag);

  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags();
  *result = mesh->is_boundary(element_topo_dim, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_is_boundary_region(viennagrid_region region,
                                               viennagrid_mesh mesh,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_index element_id,
                                               viennagrid_bool * result)
{
//   element_tag = mesh->unpack_element_tag(element_tag);

  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags( const_cast<viennagrid_region>(region) );
  *result = region->is_boundary(element_topo_dim, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_elements_get(viennagrid_mesh mesh,
                                         viennagrid_dimension element_topo_dim,
                                         viennagrid_index ** element_index_begin,
                                         viennagrid_index ** element_index_end)
{
//   element_tag = mesh->unpack_element_tag(element_tag);

  if (viennagrid_is_topological_dimension_valid(element_topo_dim) == VIENNAGRID_FALSE)
  {
    *element_index_begin = 0;
    *element_index_end = 0;
  }
  else
  {
    *element_index_begin = mesh->elements_begin(element_topo_dim);
    *element_index_end = mesh->elements_end(element_topo_dim);
  }
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_add(viennagrid_mesh mesh,
                                        viennagrid_dimension element_topo_dim,
                                        viennagrid_index element_id)
{
//   element_tag = mesh->unpack_element_tag(element_tag);

  mesh->add_element(element_topo_dim, element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_index element_id,
                                               viennagrid_index * element_parent_id)
{
  *element_parent_id = mesh_hierarchy->element_buffer(element_topo_dim).parent_id(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_index element_id,
                                               viennagrid_index element_parent_id)
{
  mesh_hierarchy->element_buffer(element_topo_dim).set_parent_id(element_id, element_parent_id);
  return VIENNAGRID_SUCCESS;
}






viennagrid_error viennagrid_region_create(viennagrid_mesh_hierarchy hierarchy,
                                          viennagrid_region * region)
{
  *region = hierarchy->make_region();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_create(viennagrid_mesh_hierarchy hierarchy,
                                              viennagrid_index region_id,
                                              viennagrid_region * region)
{
  *region = hierarchy->get_make_region(region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get(viennagrid_mesh_hierarchy hierarchy,
                                       viennagrid_index region_id,
                                       viennagrid_region * region)
{
  *region = hierarchy->get_region(region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_mesh_hierarchy(viennagrid_region region,
                                                      viennagrid_mesh_hierarchy * hierarchy)
{
  *hierarchy = region->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_region_get_name(viennagrid_region region,
                                            const char ** region_name)
{
  *region_name = region->name().c_str();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_set_name(viennagrid_region region,
                                       const char * region_name)
{
  region->set_name( std::string(region_name) );
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_id(viennagrid_region region,
                                          viennagrid_index * id)
{
  *id = region->id();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_regions_get(viennagrid_mesh_hierarchy hierarchy,
                                        viennagrid_region ** region_begin,
                                        viennagrid_region ** region_end)
{
  *region_begin = hierarchy->regions_begin();
  *region_end = hierarchy->regions_end();
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_get_regions(viennagrid_mesh_hierarchy hierarchy,
                                        viennagrid_dimension element_topo_dim,
                                        viennagrid_index element_id,
                                        viennagrid_region ** region_begin,
                                        viennagrid_region ** region_end)
{
//   element_tag = hierarchy->unpack_element_tag(element_tag);

  *region_begin = hierarchy->element_buffer(element_topo_dim).regions_begin(element_id);
  *region_end = hierarchy->element_buffer(element_topo_dim).regions_end(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy hierarchy,
                                          viennagrid_dimension element_topo_dim,
                                          viennagrid_index element_id,
                                          viennagrid_region region)
{
//   element_tag = hierarchy->unpack_element_tag(element_tag);

  hierarchy->element_buffer(element_topo_dim).add_to_region(element_id, region);

  for (viennagrid_dimension boundary_topo_dim = 0; boundary_topo_dim != element_topo_dim; ++boundary_topo_dim)
  {
    viennagrid_index * boundary = hierarchy->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);
    viennagrid_index * boundary_end = hierarchy->boundary_end(element_topo_dim, element_id, boundary_topo_dim);

    for (; boundary != boundary_end; ++boundary)
      hierarchy->element_buffer(boundary_topo_dim).add_to_region(*boundary, region);
  }


  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_plc_add_hole_point(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_index plc_id,
                                               viennagrid_numeric const * coords)
{
  mesh_hierarchy->add_hole_point(plc_id, coords);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_get_hole_points(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                viennagrid_index plc_id,
                                                viennagrid_numeric const ** hole_points_begin,
                                                viennagrid_numeric const ** hole_points_end)
{
  *hole_points_begin = mesh_hierarchy->hole_points_begin(plc_id);
  *hole_points_end = mesh_hierarchy->hole_points_end(plc_id);
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_quantity_field_make(viennagrid_quantity_field * quantity_field)
{
  *quantity_field = new viennagrid_quantity_field_t;

  (*quantity_field)->name = NULL;
  (*quantity_field)->unit = NULL;
  (*quantity_field)->topologic_dimension = -1;

  (*quantity_field)->value_count = 0;
  (*quantity_field)->values_dimension = -1;
  (*quantity_field)->values = NULL;

  (*quantity_field)->change_counter = 1;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field * quantity_field)
{
  ++(*quantity_field)->change_counter;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field)
{
  if ( --(quantity_field->change_counter) <= 0 )
  {
    if (!quantity_field)
      return VIENNAGRID_SUCCESS;

    if (quantity_field->name)
      delete[] quantity_field->name;
    if (quantity_field->unit)
      delete[] quantity_field->unit;

    if (quantity_field->values)
      delete[] quantity_field->values;

    delete quantity_field;
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantities_set_name(viennagrid_quantity_field quantity_field,
                                   const char * name)
{
  size_t string_length = strlen(name);

  if (quantity_field->name)
    delete[] quantity_field->name;
  quantity_field->name = new char[string_length+1];

  memcpy( quantity_field->name, name, (string_length+1)*sizeof(char) );

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantities_get_name(viennagrid_quantity_field quantity_field,
                                   const char ** name)
{
  *name = quantity_field->name;
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantities_set_unit(viennagrid_quantity_field quantity_field,
                                   const char * unit)
{
  size_t string_length = strlen(unit);

  if (quantity_field->unit)
    delete[] quantity_field->unit;
  quantity_field->unit = new char[string_length+1];

  memcpy( quantity_field->unit, unit, (string_length+1)*sizeof(char) );

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantities_get_unit(viennagrid_quantity_field quantity_field,
                                   const char ** unit)
{
  *unit = quantity_field->unit;
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantities_set_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                  viennagrid_dimension topologic_dimension)
{
  quantity_field->topologic_dimension = topologic_dimension;

  if (quantity_field->values)
  {
    delete[] quantity_field->values;
    quantity_field->values = NULL;
  }

  quantity_field->value_count = 0;
  quantity_field->values_dimension = -1;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantities_get_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                  viennagrid_dimension * topologic_dimension)
{
  *topologic_dimension = quantity_field->topologic_dimension;
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantities_set_values_dimension(viennagrid_quantity_field quantity_field,
                                                  viennagrid_dimension values_dimension)
{
  quantity_field->values_dimension = values_dimension;

  if (quantity_field->values)
  {
    delete[] quantity_field->values;
    quantity_field->values = NULL;
  }

  quantity_field->value_count = 0;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantities_get_values_dimension(viennagrid_quantity_field quantity_field,
                                                  viennagrid_dimension * values_dimension)
{
  *values_dimension = quantity_field->values_dimension;
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantities_resize(viennagrid_quantity_field quantity_field,
                                 viennagrid_int value_count)
{
  if (quantity_field->value_count == value_count)
    return VIENNAGRID_SUCCESS;

  int min_size = std::min( quantity_field->value_count * quantity_field->values_dimension,
                           value_count * quantity_field->values_dimension );

  quantity_field->value_count = value_count;
  viennagrid_numeric * tmp = new viennagrid_numeric[ quantity_field->values_dimension * quantity_field->value_count ];
  if (quantity_field->values)
  {
    memcpy(tmp, quantity_field->values, min_size * sizeof(char));
    delete[] quantity_field->values;
    quantity_field->values = tmp;
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantities_size(viennagrid_quantity_field quantity_field,
                               viennagrid_int * value_count)
{
  *value_count = quantity_field->value_count;
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantities_set_value(viennagrid_quantity_field quantity_field,
                                    viennagrid_index pos,
                                    viennagrid_numeric * values)
{
  assert( pos < quantity_field->value_count );

  memcpy(quantity_field->values + pos*quantity_field->values_dimension,
         values,
         quantity_field->values_dimension*sizeof(char));

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantities_get_value(viennagrid_quantity_field quantity_field,
                                    viennagrid_index pos,
                                    viennagrid_numeric * values)
{
  assert( pos < quantity_field->value_count );

  memcpy(values,
         quantity_field->values + pos*quantity_field->values_dimension,
         quantity_field->values_dimension*sizeof(char));

  return VIENNAGRID_SUCCESS;
}



















viennagrid_error viennagrid_new(viennagrid_int size,
                                void ** ptr)
{
  *ptr = malloc(size);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_delete(void * ptr)
{
  free(ptr);
  ptr = NULL;
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_serialized_mesh_hierarchy_make(
                viennagrid_serialized_mesh_hierarchy * serialized_mesh_hierarchy)
{
  viennagrid_new( sizeof(viennagrid_serialized_mesh_hierarchy_), (void**)serialized_mesh_hierarchy );


  (*serialized_mesh_hierarchy)->geometric_dimension = 0;
  (*serialized_mesh_hierarchy)->vertex_count = 0;
  (*serialized_mesh_hierarchy)->points = NULL;
  (*serialized_mesh_hierarchy)->hole_point_element_count = 0;
  (*serialized_mesh_hierarchy)->hole_points_offsets = NULL;
  (*serialized_mesh_hierarchy)->hole_points = NULL;

  (*serialized_mesh_hierarchy)->cell_count = 0;
  (*serialized_mesh_hierarchy)->cell_dimension = 0;
  (*serialized_mesh_hierarchy)->cell_element_tags = NULL;
  (*serialized_mesh_hierarchy)->cell_vertex_offsets = NULL;
  (*serialized_mesh_hierarchy)->cell_vertices = NULL;
  (*serialized_mesh_hierarchy)->cell_parents = NULL;
  (*serialized_mesh_hierarchy)->cell_region_offsets = NULL;
  (*serialized_mesh_hierarchy)->cell_regions = NULL;

  (*serialized_mesh_hierarchy)->mesh_count = 0;
  (*serialized_mesh_hierarchy)->mesh_parents = NULL;
  (*serialized_mesh_hierarchy)->mesh_cell_count = NULL;
  (*serialized_mesh_hierarchy)->mesh_cells = NULL;

  (*serialized_mesh_hierarchy)->region_count = 0;
  (*serialized_mesh_hierarchy)->region_ids = NULL;
  (*serialized_mesh_hierarchy)->region_names = NULL;

  (*serialized_mesh_hierarchy)->data_owned = VIENNAGRID_TRUE;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_serialized_mesh_hierarchy_delete(
                viennagrid_serialized_mesh_hierarchy serialized_mesh_hierarchy)
{
  if (serialized_mesh_hierarchy->data_owned == VIENNAGRID_TRUE)
  {
    viennagrid_delete(serialized_mesh_hierarchy->points);
    viennagrid_delete(serialized_mesh_hierarchy->hole_points_offsets);
    viennagrid_delete(serialized_mesh_hierarchy->hole_points);


    viennagrid_delete(serialized_mesh_hierarchy->cell_element_tags);
    viennagrid_delete(serialized_mesh_hierarchy->cell_vertex_offsets);
    viennagrid_delete(serialized_mesh_hierarchy->cell_vertices);
    viennagrid_delete(serialized_mesh_hierarchy->cell_parents);
    viennagrid_delete(serialized_mesh_hierarchy->cell_region_offsets);

    for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->mesh_count; ++i)
    {
      viennagrid_delete(serialized_mesh_hierarchy->mesh_cells[i]);
    }

    for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->region_count; ++i)
    {
      viennagrid_delete( (void*)serialized_mesh_hierarchy->region_names[i] );
    }
  }

  viennagrid_delete(serialized_mesh_hierarchy->mesh_parents);
  viennagrid_delete(serialized_mesh_hierarchy->mesh_cell_count);
  viennagrid_delete(serialized_mesh_hierarchy->cell_regions);
  viennagrid_delete(serialized_mesh_hierarchy->mesh_cells);
  viennagrid_delete(serialized_mesh_hierarchy->region_names);

  viennagrid_delete(serialized_mesh_hierarchy);

  return VIENNAGRID_SUCCESS;
}



template<typename T>
T * set(T * src, viennagrid_int size, viennagrid_bool copy_data)
{
  if (copy_data == VIENNAGRID_TRUE)
  {
    T * tmp;
    viennagrid_new( sizeof(T)*size, (void**)&tmp );

    std::copy( src, src+size, tmp );
    return tmp;
  }
  else
    return src;
}

template<typename T>
T const * set(T const * src, viennagrid_int size, viennagrid_bool copy_data)
{
  if (copy_data == VIENNAGRID_TRUE)
  {
    T * tmp;
    viennagrid_new( sizeof(T)*size, (void**)&tmp );

    std::copy( src, src+size, tmp );
    return tmp;
  }
  else
    return src;
}


viennagrid_error viennagrid_serialize_mesh_hierarchy(
                viennagrid_mesh_hierarchy mesh_hierarchy,
                viennagrid_serialized_mesh_hierarchy serialized_mesh_hierarchy,
                viennagrid_bool copy_data)
{
  serialized_mesh_hierarchy->data_owned = copy_data;

  // geometric information and vertices
  serialized_mesh_hierarchy->geometric_dimension = mesh_hierarchy->geometric_dimension();
  serialized_mesh_hierarchy->vertex_count = mesh_hierarchy->vertex_count();
  serialized_mesh_hierarchy->points = set(mesh_hierarchy->get_vertex(0),
                                          serialized_mesh_hierarchy->geometric_dimension*serialized_mesh_hierarchy->vertex_count,
                                          copy_data);

  serialized_mesh_hierarchy->hole_point_element_count = mesh_hierarchy->hole_point_element_count();
  serialized_mesh_hierarchy->hole_points_offsets = set(mesh_hierarchy->hole_points_offsets(),
                                                       serialized_mesh_hierarchy->hole_point_element_count,
                                                       copy_data);
  serialized_mesh_hierarchy->hole_points = set(mesh_hierarchy->hole_points_pointer(),
                                               serialized_mesh_hierarchy->hole_points_offsets[serialized_mesh_hierarchy->hole_point_element_count+1],
                                               copy_data);

  // cell information
  viennagrid_dimension cell_dimension = mesh_hierarchy->cell_dimension();
  viennagrid_int cell_count = mesh_hierarchy->element_buffer(cell_dimension).size();
  serialized_mesh_hierarchy->cell_count = cell_count;
  serialized_mesh_hierarchy->cell_dimension = cell_dimension;
  serialized_mesh_hierarchy->cell_element_tags = set(mesh_hierarchy->element_buffer(cell_dimension).element_tags_pointer(),
                                                     cell_count,
                                                     copy_data);
  serialized_mesh_hierarchy->cell_vertex_offsets = set(mesh_hierarchy->element_buffer(cell_dimension).vertex_offsets_pointer(),
                                                       cell_count+1,
                                                       copy_data);
  serialized_mesh_hierarchy->cell_vertices = set(mesh_hierarchy->element_buffer(cell_dimension).vertex_indices_pointer(),
                                                 serialized_mesh_hierarchy->cell_vertex_offsets[cell_count],
                                                 copy_data);
  serialized_mesh_hierarchy->cell_parents = set(mesh_hierarchy->element_buffer(cell_dimension).parent_id_pointer(),
                                                cell_count,
                                                copy_data);
  serialized_mesh_hierarchy->cell_region_offsets = set(mesh_hierarchy->element_buffer(cell_dimension).region_offsets_pointer(),
                                                cell_count+1,
                                                copy_data);


  viennagrid_new( serialized_mesh_hierarchy->cell_region_offsets[cell_count]*sizeof(viennagrid_index),
                  (void**)&serialized_mesh_hierarchy->cell_regions );
  int index = 0;
  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->cell_region_offsets[cell_count]; ++i)
  {
    for (viennagrid_region * region = mesh_hierarchy->element_buffer(cell_dimension).regions_begin(i);
                             region != mesh_hierarchy->element_buffer(cell_dimension).regions_end(i);
                           ++region, ++index)
         serialized_mesh_hierarchy->cell_regions[index] = (*region)->id();
  }



  // mesh information
  viennagrid_int mesh_count = mesh_hierarchy->mesh_count();
  serialized_mesh_hierarchy->mesh_count = mesh_count;

  std::map<viennagrid_mesh, viennagrid_int> mesh_to_index_map;
  std::map<viennagrid_int, viennagrid_mesh> index_to_mesh_map;



  viennagrid_new( mesh_count*sizeof(viennagrid_index), (void**)&serialized_mesh_hierarchy->mesh_parents );
  viennagrid_new( mesh_count*sizeof(viennagrid_int), (void**)&serialized_mesh_hierarchy->mesh_cell_count );
  viennagrid_new( mesh_count*sizeof(viennagrid_index), (void**)&serialized_mesh_hierarchy->mesh_cells );

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh mesh = mesh_hierarchy->mesh(i);
    mesh_to_index_map[mesh] = i;
    index_to_mesh_map[i] = mesh;
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh mesh = mesh_hierarchy->mesh(i);

    viennagrid_mesh parent = mesh->parent();
    if (!parent)
      serialized_mesh_hierarchy->mesh_parents[i] = -1;
    else
      serialized_mesh_hierarchy->mesh_parents[i] = mesh_to_index_map[mesh];

    serialized_mesh_hierarchy->mesh_cell_count[i] = mesh->element_count(cell_dimension);

    serialized_mesh_hierarchy->mesh_cells[i] = set(mesh->elements_begin(cell_dimension),
                                                   serialized_mesh_hierarchy->mesh_cell_count[i],
                                                   copy_data);
  }


  // region information
  viennagrid_int region_count = mesh_hierarchy->region_count();
  serialized_mesh_hierarchy->region_count = region_count;


  viennagrid_new( region_count*sizeof(viennagrid_index), (void**)&serialized_mesh_hierarchy->region_ids );
  viennagrid_new( region_count*sizeof(const char *), (void**)&serialized_mesh_hierarchy->region_names );

  index = 0;
  for (viennagrid_region * region = mesh_hierarchy->regions_begin();
                           region != mesh_hierarchy->regions_end();
                         ++region, ++index)
  {
    serialized_mesh_hierarchy->region_ids[index] = (*region)->id();
    serialized_mesh_hierarchy->region_names[index] = set((*region)->name().c_str(),
                                                         (*region)->name().size()+1,
                                                         copy_data);
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_deserialize_mesh_hierarchy(
                viennagrid_serialized_mesh_hierarchy serialized_mesh_hierarchy,
                viennagrid_mesh_hierarchy * mesh_hierarchy
                                                      )
{
  viennagrid_mesh_hierarchy_create(mesh_hierarchy);
  viennagrid_mesh_hierarchy_set_geometric_dimension(*mesh_hierarchy, serialized_mesh_hierarchy->geometric_dimension);


  // deserialize points
  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->vertex_count; ++i)
  {
    viennagrid_index vertex_index;
    viennagrid_vertex_create(*mesh_hierarchy,
                             serialized_mesh_hierarchy->points + serialized_mesh_hierarchy->geometric_dimension*i,
                             &vertex_index);
  }

  for (viennagrid_int plc_id = 0; plc_id != serialized_mesh_hierarchy->hole_point_element_count; ++plc_id)
  {
    for (viennagrid_index j = serialized_mesh_hierarchy->hole_points_offsets[plc_id];
                          j != serialized_mesh_hierarchy->hole_points_offsets[plc_id+1];
                          j += serialized_mesh_hierarchy->geometric_dimension)
    {
      viennagrid_plc_add_hole_point(*mesh_hierarchy,
                                    plc_id,
                                    serialized_mesh_hierarchy->hole_points + j);
    }
  }

  // deserialize meshes
  if (serialized_mesh_hierarchy->mesh_parents[0] != -1)
  {
    return VIENNAGRID_UNSPECIFIED_ERROR;
  }

  std::vector<viennagrid_mesh> index_mesh_map(serialized_mesh_hierarchy->mesh_count);

  viennagrid_mesh root_mesh;
  viennagrid_mesh_hierarchy_get_root( *mesh_hierarchy, &root_mesh );
  index_mesh_map[0] = root_mesh;


  for (viennagrid_int i = 1; i != serialized_mesh_hierarchy->mesh_count; ++i)
  {
    assert( serialized_mesh_hierarchy->mesh_parents[i] < i );
    viennagrid_mesh parent = index_mesh_map[serialized_mesh_hierarchy->mesh_parents[i]];

    viennagrid_mesh mesh;
    viennagrid_mesh_create( parent, &mesh );

    index_mesh_map[i] = mesh;
  }

  // deserialize regions
  std::map<viennagrid_int, viennagrid_region> index_region_map;
  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->region_count; ++i)
  {
    viennagrid_region region;
    viennagrid_region_get_create( *mesh_hierarchy, serialized_mesh_hierarchy->region_ids[i], &region );
    index_region_map[ serialized_mesh_hierarchy->region_ids[i] ] = region;

    viennagrid_region_set_name( region, serialized_mesh_hierarchy->region_names[i] );
  }

  // deserialize cells
  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->cell_count; ++i)
  {
    viennagrid_index cell_index;
    viennagrid_int cell_vertex_count = serialized_mesh_hierarchy->cell_vertex_offsets[i+1] - serialized_mesh_hierarchy->cell_vertex_offsets[i];

    std::vector<viennagrid_index> vertex_topo_dim(cell_vertex_count, 0);

    viennagrid_element_create(*mesh_hierarchy,
                              serialized_mesh_hierarchy->cell_element_tags[i],
                              cell_vertex_count,
                              serialized_mesh_hierarchy->cell_vertices + serialized_mesh_hierarchy->cell_vertex_offsets[i],
                              &vertex_topo_dim[0],
                              &cell_index);

    viennagrid_element_parent_set(*mesh_hierarchy,
                                  serialized_mesh_hierarchy->cell_dimension,
                                  cell_index,
                                  serialized_mesh_hierarchy->cell_parents[i]);

    for (viennagrid_index j = serialized_mesh_hierarchy->cell_region_offsets[i];
                          j != serialized_mesh_hierarchy->cell_region_offsets[i+1];
                        ++j)
    {
      viennagrid_index region_id = serialized_mesh_hierarchy->cell_regions[j];
      viennagrid_add_to_region(*mesh_hierarchy,
                               serialized_mesh_hierarchy->cell_dimension,
                               cell_index,
                               index_region_map[region_id]);
    }
  }

  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->mesh_count; ++i)
  {
    for (viennagrid_int j = 0; j != serialized_mesh_hierarchy->mesh_cell_count[i]; ++j)
    {
      viennagrid_element_add(index_mesh_map[i],
                             serialized_mesh_hierarchy->cell_dimension,
                             serialized_mesh_hierarchy->mesh_cells[i][j]);
    }
  }



  return VIENNAGRID_SUCCESS;
}
