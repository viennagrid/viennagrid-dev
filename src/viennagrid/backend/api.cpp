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

viennagrid_error viennagrid_mesh_hierarchy_get_cell_tag(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                        viennagrid_element_tag * cell_tag)
{
  *cell_tag = mesh_hierarchy->cell_tag();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_get_topologic_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_int * topologic_dimension)
{
  *topologic_dimension = mesh_hierarchy->topologic_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_get_geometric_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_int * geometric_dimension)
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


viennagrid_error viennagrid_mesh_make(viennagrid_mesh parent,
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
                                           viennagrid_int id,
                                           viennagrid_mesh * child)
{
  *child = mesh->get_child(id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_vertex_create(viennagrid_mesh_hierarchy hierarchy,
                                        const viennagrid_numeric * coords,
                                        viennagrid_index * vertex_id)
{
  *vertex_id = hierarchy->make_vertex(coords);
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
                                           viennagrid_index * element_index)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);
  *element_index = hierarchy->get_make_element(element_tag, indices, index_count);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy hierarchy,
                                                      viennagrid_element_tag element_tag,
                                                      viennagrid_index element_id,
                                                      viennagrid_element_tag boundary_element_tag,
                                                      viennagrid_index ** boundary_element_index_begin,
                                                      viennagrid_index ** boundary_element_index_end)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);
  boundary_element_tag = hierarchy->unpack_element_tag(boundary_element_tag);

  *boundary_element_index_begin = hierarchy->element_buffer(element_tag).boundary_indices_begin(boundary_element_tag, element_id);
  *boundary_element_index_end = hierarchy->element_buffer(element_tag).boundary_indices_end(boundary_element_tag, element_id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                        viennagrid_element_tag element_tag,
                                                        viennagrid_index element_id,
                                                        viennagrid_element_tag coboundary_element_tag,
                                                        viennagrid_index ** coboundary_element_index_begin,
                                                        viennagrid_index ** coboundary_element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);
  coboundary_element_tag = mesh->unpack_element_tag(coboundary_element_tag);

//   mesh->make_coboundary(element_tag, coboundary_element_tag);
  *coboundary_element_index_begin = mesh->coboundary_begin(element_tag, element_id, coboundary_element_tag);
  *coboundary_element_index_end = mesh->coboundary_end(element_tag, element_id, coboundary_element_tag);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                      viennagrid_element_tag element_tag,
                                                      viennagrid_index element_id,
                                                      viennagrid_element_tag connector_element_tag,
                                                      viennagrid_element_tag neighbor_element_tag,
                                                      viennagrid_index ** neighbor_element_index_begin,
                                                      viennagrid_index ** neighbor_element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);
  connector_element_tag = mesh->unpack_element_tag(connector_element_tag);
  neighbor_element_tag = mesh->unpack_element_tag(neighbor_element_tag);

  *neighbor_element_index_begin = mesh->neighbor_begin(element_tag, element_id, connector_element_tag, neighbor_element_tag);
  *neighbor_element_index_end = mesh->neighbor_end(element_tag, element_id, connector_element_tag, neighbor_element_tag);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh mesh,
                                             viennagrid_element_tag element_tag,
                                             viennagrid_index element_id,
                                             viennagrid_bool * result)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags();
  *result = mesh->is_boundary(element_tag, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_is_boundary_region(viennagrid_region region,
                                               viennagrid_mesh mesh,
                                               viennagrid_element_tag element_tag,
                                               viennagrid_index element_id,
                                               viennagrid_bool * result)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags( const_cast<viennagrid_region>(region) );
  *result = region->is_boundary(element_tag, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_elements_get(viennagrid_mesh mesh,
                                         viennagrid_element_tag element_tag,
                                         viennagrid_index ** element_index_begin,
                                         viennagrid_index ** element_index_end)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  *element_index_begin = mesh->elements_begin(element_tag);
  *element_index_end = mesh->elements_end(element_tag);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_add(viennagrid_mesh mesh,
                                        viennagrid_element_tag element_tag,
                                        viennagrid_index element_id)
{
  element_tag = mesh->unpack_element_tag(element_tag);

  mesh->add_element(element_tag, element_id);
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
                                        viennagrid_element_tag element_tag,
                                        viennagrid_index element_id,
                                        viennagrid_region ** region_begin,
                                        viennagrid_region ** region_end)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);

  *region_begin = hierarchy->element_buffer(element_tag).regions_begin(element_id);
  *region_end = hierarchy->element_buffer(element_tag).regions_end(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy hierarchy,
                                          viennagrid_element_tag element_tag,
                                          viennagrid_index element_id,
                                          viennagrid_region region)
{
  element_tag = hierarchy->unpack_element_tag(element_tag);

  hierarchy->element_buffer(element_tag).add_to_region(element_id, region);
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




