#include "viennagrid/api.h"
#include "viennagrid/backend/backend.hpp"

viennagrid_error viennagrid_mesh_hierarchy_make(viennagrid_int dimension,
                                                viennagrid_element_tag cell_tag,
                                                viennagrid_mesh_hierarchy_handle * mesh_hierarchy)
{
  *mesh_hierarchy = new viennagrid_mesh_hierarchy(dimension, cell_tag);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy_const_handle mesh_hierarchy)
{
  if (mesh_hierarchy)
    mesh_hierarchy->retain();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy_const_handle mesh_hierarchy)
{
  if (mesh_hierarchy)
    mesh_hierarchy->release();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_get_cell_tag(viennagrid_mesh_hierarchy_const_handle mesh_hierarchy,
                                                        viennagrid_element_tag * cell_tag)
{
  *cell_tag = mesh_hierarchy->cell_tag();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_get_dimension(viennagrid_mesh_hierarchy_const_handle mesh_hierarchy,
                                                         viennagrid_int * dimension)
{
  *dimension = mesh_hierarchy->dimension();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_hierarchy_get_root(viennagrid_mesh_hierarchy_handle mesh_hierarchy,
                                                    viennagrid_mesh_handle * mesh)
{
  *mesh = mesh_hierarchy->root();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_get_mesh_hierarchy(viennagrid_mesh_handle mesh,
                                                    viennagrid_mesh_hierarchy_handle * mesh_hierarchy)
{
  *mesh_hierarchy = mesh->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_get_mesh_hierarchy_const(viennagrid_mesh_const_handle mesh,
                                                          viennagrid_mesh_hierarchy_const_handle * mesh_hierarchy)
{
  *mesh_hierarchy = mesh->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_make(viennagrid_mesh_handle parent,
                                      viennagrid_mesh_handle * mesh)
{
  *mesh = parent->make_child();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh_handle mesh,
                                                viennagrid_int * children_count)
{
  *children_count = mesh->children_count();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_get_child(viennagrid_mesh_handle mesh,
                                           viennagrid_int id,
                                           viennagrid_mesh_handle * child)
{
  *child = mesh->get_child(id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_make(viennagrid_mesh_hierarchy_handle hierarchy,
                                        const viennagrid_numeric * coords,
                                        viennagrid_index * vertex_id)
{
  *vertex_id = hierarchy->make_vertex(coords);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_make_buffer(viennagrid_mesh_handle mesh)
{
  mesh->make_vertex_buffer();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_delete_buffer(viennagrid_mesh_handle mesh)
{
  mesh->delete_vertex_buffer();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_get(viennagrid_mesh_handle mesh,
                                       viennagrid_index id,
                                       viennagrid_numeric ** coords)
{
  *coords = mesh->get_vertex(id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_get_const(viennagrid_mesh_const_handle mesh,
                                             viennagrid_index id,
                                             viennagrid_numeric const ** coords)
{
  *coords = mesh->get_vertex(id);
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_make(viennagrid_mesh_hierarchy_handle hierarchy,
                                         viennagrid_element_tag element_tag,
                                         viennagrid_int vertex_count,
                                         viennagrid_index * vertex_indices,
                                         viennagrid_index * element_index)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);
  *element_index = hierarchy->get_make_element(element_tag, vertex_indices, vertex_count);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy_handle hierarchy,
                                                      viennagrid_element_tag element_tag,
                                                      viennagrid_index element_id,
                                                      viennagrid_element_tag boundary_element_tag,
                                                      viennagrid_index ** boundary_element_index_begin,
                                                      viennagrid_index ** boundary_element_index_end)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);
  boundary_element_tag = hierarchy->unpack_element_tag(boundary_element_tag);

  *boundary_element_index_begin = hierarchy->element_buffer(element_tag).boundary_buffer(boundary_element_tag).begin(element_id);
  *boundary_element_index_end = hierarchy->element_buffer(element_tag).boundary_buffer(boundary_element_tag).end(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_boundary_elements_const(viennagrid_mesh_hierarchy_const_handle hierarchy,
                                                            viennagrid_element_tag element_tag,
                                                            viennagrid_index element_id,
                                                            viennagrid_element_tag boundary_element_tag,
                                                            viennagrid_index const ** boundary_element_index_begin,
                                                            viennagrid_index const ** boundary_element_index_end)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);
  boundary_element_tag = hierarchy->unpack_element_tag(boundary_element_tag);

  *boundary_element_index_begin = hierarchy->element_buffer(element_tag).boundary_buffer(boundary_element_tag).begin(element_id);
  *boundary_element_index_end = hierarchy->element_buffer(element_tag).boundary_buffer(boundary_element_tag).end(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh_handle mesh,
                                                        viennagrid_element_tag element_tag,
                                                        viennagrid_index element_id,
                                                        viennagrid_element_tag coboundary_element_tag,
                                                        viennagrid_index ** coboundary_element_index_begin,
                                                        viennagrid_index ** coboundary_element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);
  coboundary_element_tag = mesh->unpack_element_tag(coboundary_element_tag);

  mesh->make_coboundary(element_tag, coboundary_element_tag);
  *coboundary_element_index_begin = mesh->coboundary_begin(element_tag, element_id, coboundary_element_tag);
  *coboundary_element_index_end = mesh->coboundary_end(element_tag, element_id, coboundary_element_tag);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_coboundary_elements_const(viennagrid_mesh_const_handle mesh,
                                                              viennagrid_element_tag element_tag,
                                                              viennagrid_index element_id,
                                                              viennagrid_element_tag coboundary_element_tag,
                                                              viennagrid_index const ** coboundary_element_index_begin,
                                                              viennagrid_index const ** coboundary_element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);
  coboundary_element_tag = mesh->unpack_element_tag(coboundary_element_tag);

  const_cast<viennagrid_mesh_handle>(mesh)->make_coboundary(element_tag, coboundary_element_tag);
  *coboundary_element_index_begin = mesh->coboundary_begin(element_tag, element_id, coboundary_element_tag);
  *coboundary_element_index_end = mesh->coboundary_end(element_tag, element_id, coboundary_element_tag);

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh_const_handle mesh,
                                             viennagrid_element_tag element_tag,
                                             viennagrid_index element_id,
                                             viennagrid_bool * result)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  const_cast<viennagrid_mesh_handle>(mesh)->make_boundary_flags();
  *result = mesh->is_boundary(element_tag, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_is_boundary_region(viennagrid_region_const_handle region,
                                               viennagrid_mesh_const_handle mesh,
                                               viennagrid_element_tag element_tag,
                                               viennagrid_index element_id,
                                               viennagrid_bool * result)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  const_cast<viennagrid_mesh_handle>(mesh)->make_boundary_flags( const_cast<viennagrid_region_handle>(region) );
  *result = region->is_boundary(element_tag, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_elements_get(viennagrid_mesh_handle mesh,
                                         viennagrid_element_tag element_tag,
                                         viennagrid_index ** element_index_begin,
                                         viennagrid_index ** element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  *element_index_begin = mesh->elements_begin(element_tag);
  *element_index_end = mesh->elements_end(element_tag);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_elements_get_const(viennagrid_mesh_const_handle mesh,
                                               viennagrid_element_tag element_tag,
                                               viennagrid_index const ** element_index_begin,
                                               viennagrid_index const ** element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  *element_index_begin = mesh->elements_begin(element_tag);
  *element_index_end = mesh->elements_end(element_tag);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_add(viennagrid_mesh_handle mesh,
                                        viennagrid_element_tag element_tag,
                                        viennagrid_index element_id)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  mesh->add_element(element_tag, element_id);
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_make_region(viennagrid_mesh_hierarchy_handle hierarchy,
                                        viennagrid_region_handle * region)
{
  *region = hierarchy->make_region();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_get_make_region(viennagrid_mesh_hierarchy_handle hierarchy,
                                            viennagrid_index region_id,
                                            viennagrid_region_handle * region)
{
  *region = hierarchy->get_make_region(region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_get_region(viennagrid_mesh_hierarchy_const_handle hierarchy,
                                       viennagrid_index region_id,
                                       viennagrid_region_const_handle * region)
{
  *region = hierarchy->get_region(region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_mesh_hierarchy(viennagrid_region_handle region,
                                                      viennagrid_mesh_hierarchy_handle * hierarchy)
{
  *hierarchy = region->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_mesh_hierarchy_const(viennagrid_region_const_handle region,
                                                            viennagrid_mesh_hierarchy_const_handle * hierarchy)
{
  *hierarchy = region->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_region_get_name(viennagrid_region_const_handle region,
                                            const char ** region_name)
{
  *region_name = region->name().c_str();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_set_name(viennagrid_region_handle region,
                                       const char * region_name)
{
  region->set_name( std::string(region_name) );
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_id(viennagrid_region_const_handle region,
                                          viennagrid_index * id)
{
  *id = region->id();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_regions_get(viennagrid_mesh_hierarchy_handle hierarchy,
                                        viennagrid_region_handle ** region_begin,
                                        viennagrid_region_handle ** region_end)
{
  *region_begin = hierarchy->regions_begin();
  *region_end = hierarchy->regions_end();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_regions_get_const(viennagrid_mesh_hierarchy_const_handle hierarchy,
                                              viennagrid_region_const_handle const ** region_begin,
                                              viennagrid_region_const_handle const ** region_end)
{
  *region_begin = hierarchy->regions_begin();
  *region_end = hierarchy->regions_end();
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_get_regions(viennagrid_mesh_hierarchy_const_handle hierarchy,
                                        viennagrid_element_tag element_tag,
                                        viennagrid_index element_id,
                                        viennagrid_region_const_handle const ** region_begin,
                                        viennagrid_region_const_handle const ** region_end)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);

  *region_begin = hierarchy->element_buffer(element_tag).regions_begin(element_id);
  *region_end = hierarchy->element_buffer(element_tag).regions_end(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy_handle hierarchy,
                                          viennagrid_element_tag element_tag,
                                          viennagrid_index element_id,
                                          viennagrid_region_handle region)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);

  hierarchy->element_buffer(element_tag).add_to_region(element_id, region);
  return VIENNAGRID_SUCCESS;
}




