#include <cstring>
#include <cmath>

#include "viennagrid/viennagrid.h"
#include "mesh_hierarchy.hpp"
#include "plc.hpp"





viennagrid_bool viennagrid_is_topological_dimension_valid(viennagrid_int topological_dimension)
{
  if ((topological_dimension >= 0) && (topological_dimension < VIENNAGRID_TOPOLOGIC_DIMENSION_END))
    return VIENNAGRID_TRUE;
  return VIENNAGRID_FALSE;
}

viennagrid_int viennagrid_topological_dimension(viennagrid_element_tag et)
{
  switch (et)
  {
  case VIENNAGRID_ELEMENT_TAG_VERTEX:
    return 0;
  case VIENNAGRID_ELEMENT_TAG_LINE:
    return 1;
  case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
  case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
  case VIENNAGRID_ELEMENT_TAG_POLYGON:
    return 2;
  case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
  case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
    return 3;
  default:
    return -1;
  }
}

viennagrid_bool viennagrid_is_simplex(viennagrid_element_tag et)
{
  return ((et == VIENNAGRID_ELEMENT_TAG_VERTEX) || (et == VIENNAGRID_ELEMENT_TAG_LINE) ||
          (et == VIENNAGRID_ELEMENT_TAG_TRIANGLE) || (et == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)) ?
          VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}


viennagrid_element_tag viennagrid_topological_max(viennagrid_element_tag lhs, viennagrid_element_tag rhs)
{
  if (viennagrid_topological_dimension(lhs) >= viennagrid_topological_dimension(rhs))
    return lhs;
  else
    return rhs;
}


viennagrid_bool viennagrid_dynamic_storage(viennagrid_element_tag et)
{
  if (et == VIENNAGRID_ELEMENT_TAG_POLYGON)
    return VIENNAGRID_TRUE;
  return VIENNAGRID_FALSE;
}


viennagrid_bool viennagrid_static_storage(viennagrid_element_tag et)
{
  return 1-viennagrid_dynamic_storage(et);
}








viennagrid_int viennagrid_boundary_element_count( viennagrid_element_tag host, viennagrid_dimension boundary_topo_dim )
{
  if (host == VIENNAGRID_ELEMENT_TAG_LINE)
  {
    if (boundary_topo_dim == 0)
      return 2;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
  {
    if (boundary_topo_dim == 1)
      return 3;
    if (boundary_topo_dim == 0)
      return 3;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
  {
    if (boundary_topo_dim == 1)
      return 4;
    if (boundary_topo_dim == 0)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_POLYGON)
  {
    if (boundary_topo_dim == 1)
      return -1;
    if (boundary_topo_dim == 0)
      return -1;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)
  {
    if (boundary_topo_dim == 2)
      return 4;
    if (boundary_topo_dim == 1)
      return 6;
    if (boundary_topo_dim == 0)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_HEXAHEDRON)
  {
    if (boundary_topo_dim == 2)
      return 6;
    if (boundary_topo_dim == 1)
      return 12;
    if (boundary_topo_dim == 0)
      return 8;
    return -1;
  }
  return -1;
}



viennagrid_int viennagrid_boundary_element_count_from_element_tag( viennagrid_element_tag host, viennagrid_element_tag boundary )
{
  if (host == VIENNAGRID_ELEMENT_TAG_LINE)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 2;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 3;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 3;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 4;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_POLYGON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return -1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return -1;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
      return 4;
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 6;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TAG_HEXAHEDRON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
      return 6;
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 12;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 8;
    return -1;
  }
  return -1;
}



viennagrid_bool viennagrid_is_boundary_tag(viennagrid_element_tag host, viennagrid_element_tag boundary)
{
  return viennagrid_boundary_element_count_from_element_tag(host, boundary) > 0 ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}


const char * viennagrid_element_tag_string(viennagrid_element_tag element_tag)
{
  switch (element_tag)
  {
    case VIENNAGRID_ELEMENT_TAG_VERTEX:
      return "vertex";

    case VIENNAGRID_ELEMENT_TAG_LINE:
      return "line";

    case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
      return "triangle";
    case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
      return "quadrilateral";
    case VIENNAGRID_ELEMENT_TAG_POLYGON:
      return "polygon";

    case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
      return "tetrahedron";
    case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
      return "hexahedron";

    default:
      return "no-element";
  }
}










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

viennagrid_error viennagrid_mesh_hierarchy_cell_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                              viennagrid_dimension * cell_dimension)
{
  *cell_dimension = mesh_hierarchy->cell_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_dimension * geometric_dimension)
{
  *geometric_dimension = mesh_hierarchy->geometric_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_int geometric_dimension)
{
  mesh_hierarchy->set_geometric_dimension(geometric_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_clear(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  mesh_hierarchy->clear();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_hierarchy_root_mesh_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                         viennagrid_mesh * mesh)
{
  *mesh = mesh_hierarchy->root();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_mesh_hierarchy_get(viennagrid_mesh mesh,
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

viennagrid_error viennagrid_mesh_child_get(viennagrid_mesh mesh,
                                           viennagrid_int id,
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
                                        viennagrid_int * vertex_id)
{
  viennagrid_int tmp = hierarchy->make_vertex(coords);

  if (vertex_id)
    *vertex_id = tmp;

  viennagrid_mesh root = hierarchy->root();
  root->add_element(0, *vertex_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_point_pointer(viennagrid_mesh_hierarchy mesh_hierarchy,
                                          viennagrid_numeric ** coords)
{
  *coords = mesh_hierarchy->get_vertex_pointer();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_point_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                      viennagrid_int id,
                                      viennagrid_numeric ** coords)
{
  *coords = mesh_hierarchy->get_vertex(id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_create(viennagrid_mesh_hierarchy hierarchy,
                                           viennagrid_element_tag element_tag,
                                           viennagrid_int vertex_count,
                                           viennagrid_int * vertex_indices,
                                           viennagrid_int * element_id)
{
  viennagrid_mesh root = hierarchy->root();

  std::pair<viennagrid_int, bool> tmp = hierarchy->get_make_element(element_tag, vertex_indices, vertex_count, root);

  if (element_id)
    *element_id = tmp.first;

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_create_refinement(viennagrid_mesh mesh,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_int element_id,
                                                      viennagrid_element_tag    refined_element_tag,
                                                      viennagrid_int            refined_element_base_count,
                                                      viennagrid_int *        refined_element_base_indices,
                                                      viennagrid_int            intersects_count,
                                                      viennagrid_int *        intersect_vertices_indices,
                                                      viennagrid_int *        intersects_indices,
                                                      viennagrid_dimension *    intersects_topo_dims,
                                                      viennagrid_int *        id)
{
  *id = mesh->make_refined_element(element_topo_dim, element_id,
                                   refined_element_tag, refined_element_base_count,
                                   refined_element_base_indices,
                                   intersects_count, intersect_vertices_indices,
                                   intersects_indices, intersects_topo_dims);
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_tag_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                            viennagrid_dimension element_topo_dim,
                                            viennagrid_int element_id,
                                            viennagrid_element_tag * element_tag)
{
  *element_tag = mesh_hierarchy->element_buffer(element_topo_dim).element_tag(element_id);
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_element_boundary_pointers(viennagrid_mesh_hierarchy hierarchy,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_dimension boundary_topo_dim,
                                                      viennagrid_int ** boundary_offsets,
                                                      viennagrid_int ** boundary_indices)
{
  *boundary_offsets = hierarchy->element_buffer(element_topo_dim).boundary_buffer(boundary_topo_dim).offset_pointer();
  *boundary_indices = hierarchy->element_buffer(element_topo_dim).boundary_buffer(boundary_topo_dim).values_pointer();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy hierarchy,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_int element_id,
                                                      viennagrid_dimension boundary_topo_dim,
                                                      viennagrid_int ** boundary_element_index_begin,
                                                      viennagrid_int ** boundary_element_index_end)
{
  *boundary_element_index_begin = hierarchy->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);
  *boundary_element_index_end = hierarchy->boundary_end(element_topo_dim, element_id, boundary_topo_dim);

//   hierarchy->element_buffer(element_topo_dim).boundary_indices_begin(boundary_topo_dim, element_id);
//   *boundary_element_index_end = hierarchy->element_buffer(element_topo_dim).boundary_indices_end(boundary_topo_dim, element_id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                        viennagrid_dimension element_topo_dim,
                                                        viennagrid_int element_id,
                                                        viennagrid_dimension coboundary_topo_dim,
                                                        viennagrid_int ** coboundary_element_index_begin,
                                                        viennagrid_int ** coboundary_element_index_end)
{
//   mesh->make_coboundary(element_tag, coboundary_element_tag);
  *coboundary_element_index_begin = mesh->coboundary_begin(element_topo_dim, element_id, coboundary_topo_dim);
  *coboundary_element_index_end = mesh->coboundary_end(element_topo_dim, element_id, coboundary_topo_dim);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_int element_id,
                                                      viennagrid_dimension connector_topo_dim,
                                                      viennagrid_dimension neighbor_topo_dim,
                                                      viennagrid_int ** neighbor_element_index_begin,
                                                      viennagrid_int ** neighbor_element_index_end)
{
  *neighbor_element_index_begin = mesh->neighbor_begin(element_topo_dim, element_id, connector_topo_dim, neighbor_topo_dim);
  *neighbor_element_index_end = mesh->neighbor_end(element_topo_dim, element_id, connector_topo_dim, neighbor_topo_dim);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh mesh,
                                             viennagrid_dimension element_topo_dim,
                                             viennagrid_int element_id,
                                             viennagrid_bool * result)
{
  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags();
  *result = mesh->is_boundary(element_topo_dim, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_is_boundary_region(viennagrid_region region,
                                               viennagrid_mesh mesh,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_int element_id,
                                               viennagrid_bool * result)
{
  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags( const_cast<viennagrid_region>(region) );
  *result = region->is_boundary(element_topo_dim, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_elements_get(viennagrid_mesh mesh,
                                         viennagrid_dimension element_topo_dim,
                                         viennagrid_int ** element_index_begin,
                                         viennagrid_int ** element_index_end)
{
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
                                        viennagrid_int element_id)
{
  mesh->add_element(element_topo_dim, element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_int element_id,
                                               viennagrid_int * element_parent_id)
{
  *element_parent_id = mesh_hierarchy->element_buffer(element_topo_dim).parent_id(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_int element_id,
                                               viennagrid_int element_parent_id)
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
                                              viennagrid_int region_id,
                                              viennagrid_region * region)
{
  *region = hierarchy->get_make_region(region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get(viennagrid_mesh_hierarchy hierarchy,
                                       viennagrid_int region_id,
                                       viennagrid_region * region)
{
  *region = hierarchy->get_region(region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_mesh_hierarchy_get(viennagrid_region region,
                                                      viennagrid_mesh_hierarchy * hierarchy)
{
  *hierarchy = region->mesh_hierarchy();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_region_name_get(viennagrid_region region,
                                            const char ** region_name)
{
  *region_name = region->name().c_str();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_name_set(viennagrid_region region,
                                       const char * region_name)
{
  region->set_name( std::string(region_name) );
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_id_get(viennagrid_region region,
                                          viennagrid_int * id)
{
  *id = region->id();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_regions_get(viennagrid_mesh_hierarchy hierarchy,
                                                       viennagrid_region ** region_begin,
                                                       viennagrid_region ** region_end)
{
  *region_begin = hierarchy->regions_begin();
  *region_end = hierarchy->regions_end();
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_regions_get(viennagrid_mesh_hierarchy hierarchy,
                                                viennagrid_dimension element_topo_dim,
                                                viennagrid_int element_id,
                                                viennagrid_region ** region_begin,
                                                viennagrid_region ** region_end)
{
  *region_begin = hierarchy->element_buffer(element_topo_dim).regions_begin(element_id);
  *region_end = hierarchy->element_buffer(element_topo_dim).regions_end(element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy hierarchy,
                                          viennagrid_dimension element_topo_dim,
                                          viennagrid_int element_id,
                                          viennagrid_region region)
{
  hierarchy->element_buffer(element_topo_dim).add_to_region(element_id, region);

  for (viennagrid_dimension boundary_topo_dim = 0; boundary_topo_dim != element_topo_dim; ++boundary_topo_dim)
  {
    viennagrid_int * boundary = hierarchy->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);
    viennagrid_int * boundary_end = hierarchy->boundary_end(element_topo_dim, element_id, boundary_topo_dim);

    for (; boundary != boundary_end; ++boundary)
      hierarchy->element_buffer(boundary_topo_dim).add_to_region(*boundary, region);
  }


  return VIENNAGRID_SUCCESS;
}











viennagrid_error viennagrid_plc_make(viennagrid_plc * plc)
{
  *plc = new viennagrid_plc_;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_delete(viennagrid_plc plc)
{
  delete plc;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_clear(viennagrid_plc plc)
{
  plc->clear();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                              viennagrid_plc dst_plc,
                                              viennagrid_int copy_hole_points,
                                              viennagrid_int copy_seed_points)
{
  viennagrid_dimension geometric_dimension;
  viennagrid_plc_geometric_dimension_get(src_plc, &geometric_dimension);

  viennagrid_plc_clear(dst_plc);
  viennagrid_plc_geometric_dimension_set(dst_plc, geometric_dimension);

  if (copy_hole_points == VIENNAGRID_TRUE)
  {
    viennagrid_numeric * hole_points;
    viennagrid_int hole_point_count;
    viennagrid_plc_hole_points_get(src_plc, &hole_points, &hole_point_count);
    for (viennagrid_int i = 0; i != hole_point_count; ++i)
      viennagrid_plc_hole_point_add(dst_plc, hole_points + i*geometric_dimension);
  }

  if (copy_seed_points == VIENNAGRID_TRUE)
  {
    viennagrid_numeric * seed_points;
    viennagrid_int * seed_point_regions;
    viennagrid_int seed_point_count;
    viennagrid_plc_seed_points_get(src_plc, &seed_points, &seed_point_regions, &seed_point_count);
    for (viennagrid_int i = 0; i != seed_point_count; ++i)
      viennagrid_plc_seed_point_add(dst_plc, seed_points + i*geometric_dimension, seed_point_regions[i]);
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                        viennagrid_dimension * geometric_dimension)
{
  *geometric_dimension = plc->geometric_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                        viennagrid_int geometric_dimension)
{
  plc->set_geometric_dimension(geometric_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_element_count_get(viennagrid_plc plc,
                                                  viennagrid_dimension topologic_dimension,
                                                  viennagrid_int * element_count)
{
  *element_count = plc->element_count(topologic_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_vertex_create(viennagrid_plc plc,
                                              const viennagrid_numeric * coords,
                                              viennagrid_int * vertex_id)
{
  viennagrid_int id = plc->make_vertex(coords);

  if (vertex_id)
    *vertex_id = id;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_point_pointer(viennagrid_plc plc,
                                               viennagrid_numeric ** coords)
{
  *coords = plc->vertex_pointer();
  return VIENNAGRID_SUCCESS;
}
viennagrid_error viennagrid_plc_point_get(viennagrid_plc plc,
                                           viennagrid_int id,
                                           viennagrid_numeric ** coords)
{
  *coords = plc->get_vertex(id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                            viennagrid_int vertex_index0,
                                            viennagrid_int vertex_index1,
                                            viennagrid_int * line_id)
{
  viennagrid_int id = plc->get_make_line(vertex_index0, vertex_index1);

  if (line_id)
    *line_id = id;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                             viennagrid_int * line_indices,
                                             viennagrid_int line_count,
                                             viennagrid_int * facet_id)
{
  viennagrid_int id = plc->get_make_facet(line_indices, line_count);

  if (facet_id)
    *facet_id = id;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                  viennagrid_dimension element_topo_dim,
                                                  viennagrid_int element_id,
                                                  viennagrid_dimension boundary_topo_dim,
                                                  viennagrid_int ** boundary_element_index_begin,
                                                  viennagrid_int ** boundary_element_index_end)
{
  *boundary_element_index_begin = plc->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);
  *boundary_element_index_end = plc->boundary_end(element_topo_dim, element_id, boundary_topo_dim);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_point_add(viennagrid_plc plc,
                                                     viennagrid_int facet_id,
                                                     const viennagrid_numeric * coords)
{
  plc->add_facet_hole_point(facet_id, coords);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_point_delete(viennagrid_plc plc,
                                                        viennagrid_int facet_id,
                                                        viennagrid_int point_index)
{
  plc->delete_facet_hole_point(facet_id, point_index);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_points_get(viennagrid_plc plc,
                                                      viennagrid_int facet_id,
                                                      viennagrid_numeric ** coords,
                                                      viennagrid_int * hole_point_count)
{
  *coords = plc->facet_hole_points(facet_id);
  *hole_point_count = plc->facet_hole_point_count(facet_id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_hole_point_add(viennagrid_plc plc,
                                               const viennagrid_numeric * coords)
{
  plc->add_hole_point(coords);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_hole_points_get(viennagrid_plc plc,
                                                viennagrid_numeric ** coords,
                                                viennagrid_int * count)
{
  *coords = plc->get_hole_points();
  *count = plc->hole_point_count();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_plc_seed_point_add(viennagrid_plc plc,
                                               const viennagrid_numeric * coords,
                                               viennagrid_int region_id)
{
  plc->add_seed_point(coords, region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_seed_points_get(viennagrid_plc plc,
                                                viennagrid_numeric ** coords,
                                                viennagrid_int ** region_ids,
                                                viennagrid_int * count)
{
  *coords = plc->get_seed_points();
  *region_ids = plc->get_seed_point_regions();
  *count = plc->seed_point_count();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_read_tetgen_poly(viennagrid_plc plc,
                                                 const char * filename)
{
  return plc->read_tetgen_poly(filename);
}

viennagrid_error viennagrid_plc_refine_lines(viennagrid_plc plc,
                                             viennagrid_plc output_plc,
                                             viennagrid_numeric line_size)
{
  viennagrid_dimension geometric_dimension;
  viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

  viennagrid_plc_init_from_plc(plc, output_plc, VIENNAGRID_TRUE, VIENNAGRID_TRUE);

  viennagrid_int line_count;
  viennagrid_plc_element_count_get(plc, 1, &line_count);

  viennagrid_int facet_count;
  viennagrid_plc_element_count_get(plc, 2, &facet_count);

  std::map<viennagrid_int, viennagrid_int> vertex_map;
  std::vector< std::vector<viennagrid_int> > line_to_lines_map(line_count);

  for (viennagrid_int line_id = 0; line_id != line_count; ++line_id)
  {
    viennagrid_int * vertices_begin;
    viennagrid_int * vertices_end;
    viennagrid_plc_boundary_elements(plc, 1, line_id, 0, &vertices_begin, &vertices_end);

    viennagrid_int v0 = *(vertices_begin+0);
    viennagrid_int v1 = *(vertices_begin+1);


    viennagrid_numeric * p0;
    viennagrid_plc_point_get(plc, v0, &p0);

    viennagrid_numeric * p1;
    viennagrid_plc_point_get(plc, v1, &p1);


    viennagrid_int nv0;
    std::map<viennagrid_int, viennagrid_int>::iterator it0 = vertex_map.find(v0);
    if (it0 == vertex_map.end())
    {
      viennagrid_plc_vertex_create(output_plc, p0, &nv0);
      vertex_map[v0] = nv0;
    }
    else
      nv0 = it0->second;

    viennagrid_int nv1;
    std::map<viennagrid_int, viennagrid_int>::iterator it1 = vertex_map.find(v1);
    if (it1 == vertex_map.end())
    {
      viennagrid_plc_vertex_create(output_plc, p1, &nv1);
      vertex_map[v1] = nv1;
    }
    else
      nv1 = it1->second;


    std::vector<viennagrid_int> new_lines;

    double current_line_size = 0.0;
    for (viennagrid_dimension i = 0; i != geometric_dimension; ++i)
      current_line_size += (p0[i]-p1[i])*(p0[i]-p1[i]);
    current_line_size = std::sqrt(current_line_size);

    if (current_line_size <= line_size)
    {
      viennagrid_int line_id;
      viennagrid_plc_line_create( output_plc, nv0, nv1, &line_id );
      new_lines.push_back(line_id);
    }
    else
    {
      int number_of_new_lines = std::ceil(current_line_size / line_size);
      assert(number_of_new_lines > 1);

      std::vector<viennagrid_numeric> offset(geometric_dimension);
      for (viennagrid_dimension i = 0; i != geometric_dimension; ++i)
        offset[i] = (p1[i]-p0[i]) / number_of_new_lines;


      std::vector<viennagrid_int> vertices;
      vertices.push_back(nv0);
      for (int i = 1; i != number_of_new_lines; ++i)
      {
        std::vector<viennagrid_numeric> tmp(geometric_dimension);
        for (viennagrid_dimension j = 0; j != geometric_dimension; ++j)
          tmp[j] = p0[j] + i*offset[j];

        viennagrid_int vertex_id;
        viennagrid_plc_vertex_create( output_plc, &tmp[0], &vertex_id);
        vertices.push_back(vertex_id);
      }
      vertices.push_back(nv1);

      for (std::size_t i = 0; i != vertices.size()-1; ++i)
      {
        viennagrid_int line_id;
        viennagrid_plc_line_create( output_plc, vertices[i], vertices[i+1], &line_id );
        new_lines.push_back(line_id);
      }
    }

    line_to_lines_map[line_id] = new_lines;
  }



  for (viennagrid_int facet_id = 0; facet_id != facet_count; ++facet_id)
  {
    std::vector<viennagrid_int> new_lines;

    viennagrid_int * lines_begin;
    viennagrid_int * lines_end;
    viennagrid_plc_boundary_elements(plc, 2, facet_id, 1, &lines_begin, &lines_end);

    for (viennagrid_int * line_it = lines_begin; line_it != lines_end; ++line_it)
    {
      std::vector<viennagrid_int> const & current_new_lines = line_to_lines_map[*line_it];
      std::copy( current_new_lines.begin(), current_new_lines.end(), std::back_inserter(new_lines) );
    }

    viennagrid_int new_facet_id;
    viennagrid_plc_facet_create(output_plc, &new_lines[0], new_lines.size(), &new_facet_id);

    viennagrid_numeric * facet_hole_points;
    viennagrid_int facet_hole_point_count;
    viennagrid_plc_facet_hole_points_get(plc, facet_id, &facet_hole_points, &facet_hole_point_count);
    for (viennagrid_int i = 0; i != facet_hole_point_count; ++i)
      viennagrid_plc_facet_hole_point_add(plc, new_facet_id, facet_hole_points + i*geometric_dimension);
  }

  return VIENNAGRID_SUCCESS;
}

// viennagrid_error viennagrid_plc_from_mesh(viennagrid_mesh mesh,
//                                           viennagrid_plc plc)
// {
//   viennagrid_mesh_hierarchy mh;
//   viennagrid_mesh_get_mesh_hierarchy(mesh, &mh);
//
//   viennagrid_dimension geometric_dimension;
//   viennagrid_mesh_hierarchy_get_geometric_dimension(mh, &geometric_dimension);
//
//   viennagrid_dimension cell_dimension;
//   viennagrid_mesh_hierarchy_get_cell_dimension(mh, &cell_dimension);
//
//   if ( ((geometric_dimension != 2) || (geometric_dimension != 3)) && (cell_dimension != 2) )
//     return VIENNAGRID_PLC_MESH_TYPE_NOT_SUPPORTED_FOR_CONVERSION;
//
//   viennagrid_plc_clear(plc);
//
//   return VIENNAGRID_SUCCESS;
// }








viennagrid_error viennagrid_new(viennagrid_int size,
                                void ** ptr)
{
  *ptr = malloc(size);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_delete(void ** ptr)
{
  free(*ptr);
  *ptr = NULL;
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_serialized_mesh_hierarchy_make(
                viennagrid_serialized_mesh_hierarchy * serialized_mesh_hierarchy)
{
  viennagrid_new( sizeof(viennagrid_serialized_mesh_hierarchy_), (void**)serialized_mesh_hierarchy );


  (*serialized_mesh_hierarchy)->geometric_dimension = 0;
  (*serialized_mesh_hierarchy)->vertex_count = 0;
  (*serialized_mesh_hierarchy)->points = NULL;

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
    viennagrid_delete( (void**)&serialized_mesh_hierarchy->points );

    viennagrid_delete( (void**)&serialized_mesh_hierarchy->cell_element_tags );
    viennagrid_delete( (void**)&serialized_mesh_hierarchy->cell_vertex_offsets );
    viennagrid_delete( (void**)&serialized_mesh_hierarchy->cell_vertices );
    viennagrid_delete( (void**)&serialized_mesh_hierarchy->cell_parents );
    viennagrid_delete( (void**)&serialized_mesh_hierarchy->cell_region_offsets );

    for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->mesh_count; ++i)
    {
      viennagrid_delete( (void**)&serialized_mesh_hierarchy->mesh_cells[i] );
    }

    for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->region_count; ++i)
    {
      viennagrid_delete( (void**)&serialized_mesh_hierarchy->region_names[i] );
    }
  }

  viennagrid_delete( (void**)&serialized_mesh_hierarchy->mesh_parents );
  viennagrid_delete( (void**)&serialized_mesh_hierarchy->mesh_cell_count );
  viennagrid_delete( (void**)&serialized_mesh_hierarchy->cell_regions );
  viennagrid_delete( (void**)&serialized_mesh_hierarchy->mesh_cells );
  viennagrid_delete( (void**)&serialized_mesh_hierarchy->region_names );

  viennagrid_delete( (void**)&serialized_mesh_hierarchy );

  return VIENNAGRID_SUCCESS;
}



template<typename T>
T * set(T * src, viennagrid_int size, viennagrid_bool copy_data)
{
  if (copy_data == VIENNAGRID_TRUE)
  {
    T * tmp;
    viennagrid_new( sizeof(T)*size, (void**)&tmp );
    memcpy( tmp, src, sizeof(T)*size );
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
    memcpy( tmp, src, sizeof(T)*size );
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


  viennagrid_new( serialized_mesh_hierarchy->cell_region_offsets[cell_count]*sizeof(viennagrid_int),
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



  viennagrid_new( mesh_count*sizeof(viennagrid_int), (void**)&serialized_mesh_hierarchy->mesh_parents );
  viennagrid_new( mesh_count*sizeof(viennagrid_int), (void**)&serialized_mesh_hierarchy->mesh_cell_count );
  viennagrid_new( mesh_count*sizeof(viennagrid_int*), (void**)&serialized_mesh_hierarchy->mesh_cells );

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
//     std::cout << serialized_mesh_hierarchy->mesh_cell_count[i] << std::endl;

//     viennagrid_int * tmp;
//     viennagrid_new( sizeof(viennagrid_int)*mesh->element_count(cell_dimension), (void**)&tmp );
//     memcpy( tmp, mesh->elements_begin(cell_dimension), sizeof(viennagrid_int)*mesh->element_count(cell_dimension) );

    serialized_mesh_hierarchy->mesh_cells[i] = set(mesh->elements_begin(cell_dimension),
                                                   serialized_mesh_hierarchy->mesh_cell_count[i],
                                                   copy_data);
  }


  // region information
  viennagrid_int region_count = mesh_hierarchy->region_count();
  serialized_mesh_hierarchy->region_count = region_count;


  viennagrid_new( region_count*sizeof(viennagrid_int), (void**)&serialized_mesh_hierarchy->region_ids );
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
  viennagrid_mesh_hierarchy_geometric_dimension_set(*mesh_hierarchy, serialized_mesh_hierarchy->geometric_dimension);


  // deserialize points
  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->vertex_count; ++i)
  {
    viennagrid_int vertex_index;
    viennagrid_vertex_create(*mesh_hierarchy,
                             serialized_mesh_hierarchy->points + serialized_mesh_hierarchy->geometric_dimension*i,
                             &vertex_index);
  }

  // deserialize meshes
  if (serialized_mesh_hierarchy->mesh_parents[0] != -1)
  {
    return VIENNAGRID_UNSPECIFIED_ERROR;
  }

  std::vector<viennagrid_mesh> index_mesh_map(serialized_mesh_hierarchy->mesh_count);

  viennagrid_mesh root_mesh;
  viennagrid_mesh_hierarchy_root_mesh_get( *mesh_hierarchy, &root_mesh );
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

    viennagrid_region_name_set( region, serialized_mesh_hierarchy->region_names[i] );
  }

  // deserialize cells
  for (viennagrid_int i = 0; i != serialized_mesh_hierarchy->cell_count; ++i)
  {
    viennagrid_int cell_index;
    viennagrid_int cell_vertex_count = serialized_mesh_hierarchy->cell_vertex_offsets[i+1] - serialized_mesh_hierarchy->cell_vertex_offsets[i];

    viennagrid_element_create(*mesh_hierarchy,
                              serialized_mesh_hierarchy->cell_element_tags[i],
                              cell_vertex_count,
                              serialized_mesh_hierarchy->cell_vertices + serialized_mesh_hierarchy->cell_vertex_offsets[i],
                              &cell_index);

    viennagrid_element_parent_set(*mesh_hierarchy,
                                  serialized_mesh_hierarchy->cell_dimension,
                                  cell_index,
                                  serialized_mesh_hierarchy->cell_parents[i]);

    for (viennagrid_int j = serialized_mesh_hierarchy->cell_region_offsets[i];
                          j != serialized_mesh_hierarchy->cell_region_offsets[i+1];
                        ++j)
    {
      viennagrid_int region_id = serialized_mesh_hierarchy->cell_regions[j];
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
