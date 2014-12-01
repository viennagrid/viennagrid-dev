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
//   element_tag = hierarchy->unpack_element_tag(element_tag);
  *element_id = hierarchy->get_make_element(element_tag, indices, topo_dims, index_count);

  viennagrid_mesh root = hierarchy->root();
  viennagrid_dimension element_topo_dim = viennagrid_topological_dimension(element_tag);

  root->add_element(element_topo_dim, *element_id);

  for (viennagrid_dimension boundary_topo_dim = 0; boundary_topo_dim != element_topo_dim; ++boundary_topo_dim)
  {
    viennagrid_index * boundary = hierarchy->boundary_begin(element_topo_dim, *element_id, boundary_topo_dim);
    viennagrid_index * boundary_end = hierarchy->boundary_end(element_topo_dim, *element_id, boundary_topo_dim);

//     ->element_buffer(element_topo_dim).boundary_indices_begin(boundary_topo_dim, *element_id);
//     viennagrid_index * boundary_end = hierarchy->element_buffer(element_topo_dim).boundary_indices_end(boundary_topo_dim, *element_id);

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

  *element_index_begin = mesh->elements_begin(element_topo_dim);
  *element_index_end = mesh->elements_end(element_topo_dim);
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




