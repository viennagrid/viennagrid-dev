#ifndef VIENNAGRID_BACKEND_API_H
#define VIENNAGRID_BACKEND_API_H

#include "viennagrid/backend/forwards.h"



// creates a mesh_hierarchy with a mesh root
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_create(viennagrid_int dimension,
                                                                            viennagrid_element_tag cell_tag,
                                                                            viennagrid_mesh_hierarchy * mesh_hierarchy);
// deletes a mesh_hierarchy with all its meshes
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy mesh_hierarchy);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy mesh_hierarchy);

// queries the cell tag of a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_get_cell_tag(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                             viennagrid_element_tag * cell_tag);
// queries the geometric dimension of a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_get_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                              viennagrid_int * dimension);

// get the root mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_get_root(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_mesh * mesh);
// get the mesh_hierarchy of a mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_get_mesh_hierarchy(viennagrid_mesh mesh,
                                                                              viennagrid_mesh_hierarchy * mesh_hierarchy);

// creates a mesh using a parent mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_create(viennagrid_mesh parent,
                                                                  viennagrid_mesh * mesh);
// deletes a mesh and all child meshes
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_delete(viennagrid_mesh mesh);
// queries the number of children
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh mesh,
                                                                          viennagrid_int * children_count);
// gets a child mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_get_child(viennagrid_mesh mesh,
                                                                     viennagrid_int id,
                                                                     viennagrid_mesh * child);



// creates a set of vertices in a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    const viennagrid_numeric * coords,
                                                                    viennagrid_index * vertex_id);
// deletes a set of vertices in a mesh_hierarchy TODO: implement
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_delete_vertices(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                      viennagrid_index * vertex_indices);
// creates a separate vertex buffer for a mesh, coordinates are copied from the parent mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_buffer_create(viennagrid_mesh mesh);
// deletes the separate vertex buffer for a mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_buffer_delete(viennagrid_mesh mesh);
// gets the coordinates of a vertex of a mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_get(viennagrid_mesh mesh,
                                                                 viennagrid_index id,
                                                                 viennagrid_numeric ** coords);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_element_tag element_tag,
                                                                    viennagrid_int vertex_count,
                                                                    viennagrid_index * vertex_indices,
                                                                    viennagrid_index * element_index);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_element_tag element_tag,
                                                                                viennagrid_index element_id,
                                                                                viennagrid_element_tag boundary_element_tag,
                                                                                viennagrid_index ** boundary_element_index_begin,
                                                                                viennagrid_index ** boundary_element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                                                  viennagrid_element_tag element_tag,
                                                                                  viennagrid_index element_id,
                                                                                  viennagrid_element_tag coboundary_element_tag,
                                                                                  viennagrid_index ** coboundary_element_index_begin,
                                                                                  viennagrid_index ** coboundary_element_index_end);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh mesh,
                                                                       viennagrid_element_tag element_tag,
                                                                       viennagrid_index element_id,
                                                                       viennagrid_bool * result);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_is_boundary_region(viennagrid_region region,
                                                                         viennagrid_mesh mesh,
                                                                         viennagrid_element_tag element_tag,
                                                                         viennagrid_index element_id,
                                                                         viennagrid_bool * result);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_elements_get(viennagrid_mesh mesh,
                                                                   viennagrid_element_tag element_tag,
                                                                   viennagrid_index ** element_index_begin,
                                                                   viennagrid_index ** element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_add(viennagrid_mesh mesh,
                                                                  viennagrid_element_tag element_tag,
                                                                  viennagrid_index element_id);






VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_region * region);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                        viennagrid_index region_id,
                                                                        viennagrid_region * region);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                 viennagrid_index region_id,
                                                                 viennagrid_region * region);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_mesh_hierarchy(viennagrid_region region,
                                                                                viennagrid_mesh_hierarchy * mesh_hierarchy);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_name(viennagrid_region region,
                                                                      const char ** region_name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_set_name(viennagrid_region region,
                                                                      const char * region_name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_id(viennagrid_region region,
                                                                    viennagrid_index * id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_regions_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                  viennagrid_region ** region_begin,
                                                                  viennagrid_region ** region_end);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_get_regions(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                  viennagrid_element_tag element_tag,
                                                                  viennagrid_index element_id,
                                                                  viennagrid_region const ** region_begin,
                                                                  viennagrid_region const ** region_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_element_tag element_tag,
                                                                    viennagrid_index element_id,
                                                                    viennagrid_region region);




#endif
