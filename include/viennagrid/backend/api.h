#ifndef VIENNAGRID_BACKEND_API_H
#define VIENNAGRID_BACKEND_API_H

#include "viennagrid/backend/forwards.h"



// creates a mesh_hierarchy with a mesh root
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_create(viennagrid_mesh_hierarchy * mesh_hierarchy);
// deletes a mesh_hierarchy with all its meshes
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy mesh_hierarchy);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy mesh_hierarchy);

// queries the topologic dimension of a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_get_cell_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                              viennagrid_dimension * topologic_dimension);

// queries the geometric dimension of a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_get_geometric_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                              viennagrid_dimension * geometric_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_set_geometric_dimension(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_index geometric_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_clear(viennagrid_mesh_hierarchy mesh_hierarchy);


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
                                                                     viennagrid_index id,
                                                                     viennagrid_mesh * child);





VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_element_count(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   viennagrid_element_tag element_tag,
                                                                                   viennagrid_int * count);

// creates a set of vertices in a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    const viennagrid_numeric * coords,
                                                                    viennagrid_index * vertex_id);
// gets the coordinates of a vertex of a mesh
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                 viennagrid_index id,
                                                                 viennagrid_numeric ** coords);


// indices are vertex indices for all elements but PLCs, where they are line indices
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_element_tag element_tag,
                                                                    viennagrid_int index_count,
                                                                    viennagrid_index * indices,
                                                                    viennagrid_dimension * topo_dims,
                                                                    viennagrid_index * element_index);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_create_refinement(
                                                            viennagrid_mesh mesh,
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
                                                            viennagrid_index *        id);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_get_tag(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                      viennagrid_dimension element_topo_dim,
                                                                      viennagrid_index element_id,
                                                                      viennagrid_element_tag * element_tag);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_index element_id,
                                                                                viennagrid_dimension boundary_topo_dim,
                                                                                viennagrid_index ** boundary_element_index_begin,
                                                                                viennagrid_index ** boundary_element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                                                  viennagrid_dimension element_topo_dim,
                                                                                  viennagrid_index element_id,
                                                                                  viennagrid_dimension coboundary_topo_dim,
                                                                                  viennagrid_index ** coboundary_element_index_begin,
                                                                                  viennagrid_index ** coboundary_element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_index element_id,
                                                                                viennagrid_dimension connector_topo_dim,
                                                                                viennagrid_dimension neighbor_topo_dim,
                                                                                viennagrid_index ** neighbor_element_index_begin,
                                                                                viennagrid_index ** neighbor_element_index_end);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh mesh,
                                                                       viennagrid_dimension element_topo_dim,
                                                                       viennagrid_index element_id,
                                                                       viennagrid_bool * result);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_is_boundary_region(viennagrid_region region,
                                                                         viennagrid_mesh mesh,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_index element_id,
                                                                         viennagrid_bool * result);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_elements_get(viennagrid_mesh mesh,
                                                                   viennagrid_dimension element_topo_dim,
                                                                   viennagrid_index ** element_index_begin,
                                                                   viennagrid_index ** element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_add(viennagrid_mesh mesh,
                                                                  viennagrid_dimension element_topo_dim,
                                                                  viennagrid_index element_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_index element_id,
                                                                         viennagrid_index * element_parent_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_index element_id,
                                                                         viennagrid_index element_parent_id);







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
                                                                  viennagrid_dimension element_topo_dim,
                                                                  viennagrid_index element_id,
                                                                  viennagrid_region ** region_begin,
                                                                  viennagrid_region ** region_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_dimension element_topo_dim,
                                                                    viennagrid_index element_id,
                                                                    viennagrid_region region);



// creates a set of vertices in a mesh_hierarchy
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_add_hole_point(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_index plc_id,
                                                                         viennagrid_numeric const * coords);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_get_hole_points(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                          viennagrid_index plc_id,
                                                                          viennagrid_numeric const ** hole_points_begin,
                                                                          viennagrid_numeric const ** hole_points_end);





// quantities
struct viennagrid_quantity_field_t
{
  char * name;                                  // initialized with NULL
  char * unit;                                  // initialized with NULL
  viennagrid_dimension topologic_dimension;     // initialized with -1

  viennagrid_int value_count;                   // initialized with 0
  viennagrid_dimension values_dimension;        // initialized with -1
  viennagrid_numeric * values;                  // initialized with NULL

  viennagrid_int use_count;                     // initialized with 1
};


typedef struct viennagrid_quantity_field_t * viennagrid_quantity_field;


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_make(viennagrid_quantity_field * quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_set_name(viennagrid_quantity_field quantity_field,
                                                                          const char * name);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_get_name(viennagrid_quantity_field quantity_field,
                                                                          const char ** name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_set_unit(viennagrid_quantity_field quantity_field,
                                                                          const char * unit);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_get_unit(viennagrid_quantity_field quantity_field,
                                                                          const char ** unit);

// deletes all values data, sets topologic values dimension to -1 and size to 0
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_set_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                                                         viennagrid_dimension topologic_dimension);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_get_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                                                         viennagrid_dimension * topologic_dimension);

// deletes all data, sets size to 0
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_set_values_dimension(viennagrid_quantity_field quantity_field,
                                                                                      viennagrid_dimension values_dimension);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_get_values_dimension(viennagrid_quantity_field quantity_field,
                                                                                      viennagrid_dimension * values_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_resize(viennagrid_quantity_field quantity_field,
                                                                        viennagrid_int value_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_size(viennagrid_quantity_field quantity_field,
                                                                      viennagrid_int * value_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_set_value(viennagrid_quantity_field quantity_field,
                                                                           viennagrid_index pos,
                                                                           viennagrid_numeric * values);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantities_get_value(viennagrid_quantity_field quantity_field,
                                                                           viennagrid_index pos,
                                                                           viennagrid_numeric ** values);



// serialization/de-serialization

struct viennagrid_serialized_mesh_hierarchy_
{
  // geometric information and vertices
  viennagrid_dimension geometric_dimension;
  viennagrid_int vertex_count;
  viennagrid_numeric * points;                  // may be shared with mesh hierarchy
  viennagrid_int hole_point_element_count;
  viennagrid_index * hole_points_offsets;
  viennagrid_numeric * hole_points;

  // cell information
  viennagrid_int cell_count;
  viennagrid_dimension cell_dimension;
  viennagrid_element_tag * cell_element_tags;   // may be shared with mesh hierarchy
  viennagrid_index * cell_vertex_offsets;       // may be shared with mesh hierarchy
  viennagrid_index * cell_vertices;             // may be shared with mesh hierarchy
  viennagrid_index * cell_parents;              // may be shared with mesh hierarchy
  viennagrid_index * cell_region_offsets;       // may be shared with mesh hierarchy
  viennagrid_index * cell_regions;

  // mesh information
  viennagrid_int mesh_count;
  viennagrid_index * mesh_parents;
  viennagrid_int * mesh_cell_count;
  viennagrid_index ** mesh_cells;               // array pointers may be shared with mesh hierarchy

  // region information
  viennagrid_int region_count;
  viennagrid_index * region_ids;
  const char ** region_names;                         // strings may be shared with mesh hierarchy

  // for internal use
  viennagrid_bool data_owned;
};

typedef struct viennagrid_serialized_mesh_hierarchy_ * viennagrid_serialized_mesh_hierarchy;

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_new(viennagrid_int size,
                                                          void ** ptr);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_delete(void * ptr);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_serialized_mesh_hierarchy_make(
                viennagrid_serialized_mesh_hierarchy * serialized_mesh_hierarchy);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_serialized_mesh_hierarchy_delete(
                viennagrid_serialized_mesh_hierarchy serialized_mesh_hierarchy);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_serialize_mesh_hierarchy(
                viennagrid_mesh_hierarchy mesh_hierarchy,
                viennagrid_serialized_mesh_hierarchy serialized_mesh_hierarchy,
                viennagrid_bool copy_data);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_deserialize_mesh_hierarchy(
                viennagrid_serialized_mesh_hierarchy serialized_mesh_hierarchy,
                viennagrid_mesh_hierarchy * mesh_hierarchy);





#endif
