#ifndef VIENNAGRID_H
#define VIENNAGRID_H



#if defined(_MSC_VER)
  #if defined(viennagrid_EXPORTS)
    #define  VIENNAGRID_DYNAMIC_EXPORT __declspec(dllexport)
  #else
    #define  VIENNAGRID_DYNAMIC_EXPORT __declspec(dllimport)
  #endif /* viennagrid_EXPORTS */
#else /* defined (_MSC_VER) */
  #ifdef __cplusplus
    #define VIENNAGRID_DYNAMIC_EXPORT extern "C"
  #else
    #define VIENNAGRID_DYNAMIC_EXPORT
  #endif
#endif


typedef int viennagrid_bool;
typedef int viennagrid_int;
typedef viennagrid_int viennagrid_error;
typedef double viennagrid_numeric;
// typedef viennagrid_int viennagrid_index;
typedef unsigned char viennagrid_element_tag;
typedef int viennagrid_dimension;
typedef char viennagrid_flag;


typedef struct viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;
typedef struct viennagrid_mesh_ * viennagrid_mesh;
typedef struct viennagrid_region_ * viennagrid_region;
typedef struct viennagrid_plc_ * viennagrid_plc;



#define VIENNAGRID_TRUE 1
#define VIENNAGRID_FALSE 0


/* VIENNAGRID ERROR CODES */
#define VIENNAGRID_SUCCESS 0
#define VIENNAGRID_UNSPECIFIED_ERROR 1
#define VIENNAGRID_OUT_OF_MEMORY 2
#define VIENNAGRID_INVALID_ARGUMENTS 3
#define VIENNAGRID_UNSUPPORTED_BOUNDARY_LAYOUT 4

/* VIENNAGRID OPTION FLAGS */
#define VIENNAGRID_BOUNDARY_LAYOUT_FLAG 0x01
#define VIENNAGRID_ELEMENT_EXPLICIT 0x01

/* VIENNAGRID OPTIONS */
#define VIENNAGRID_BOUNDARY_LAYOUT_FULL 0
#define VIENNAGRID_BOUNDARY_LAYOUT_SPARSE 1

/* VIENNAGRID ELEMENT TYPES */
#define VIENNAGRID_ELEMENT_TAG_NO_ELEMENT 255 /* -1 */
#define VIENNAGRID_ELEMENT_TAG_START 0
#define VIENNAGRID_ELEMENT_TAG_VERTEX 0
#define VIENNAGRID_ELEMENT_TAG_LINE 1
#define VIENNAGRID_ELEMENT_TAG_EDGE 1
#define VIENNAGRID_ELEMENT_TAG_TRIANGLE 2
#define VIENNAGRID_ELEMENT_TAG_QUADRILATERAL 3
#define VIENNAGRID_ELEMENT_TAG_POLYGON 4
#define VIENNAGRID_ELEMENT_TAG_TETRAHEDRON 5
#define VIENNAGRID_ELEMENT_TAG_HEXAHEDRON 6
#define VIENNAGRID_ELEMENT_TAG_COUNT 7

#define VIENNAGRID_TOPOLOGIC_DIMENSION_END 4



viennagrid_bool viennagrid_is_topological_dimension_valid(viennagrid_int topological_dimension);
viennagrid_int viennagrid_topological_dimension(viennagrid_element_tag et);
viennagrid_bool viennagrid_is_simplex(viennagrid_element_tag et);
viennagrid_element_tag viennagrid_topological_max(viennagrid_element_tag lhs, viennagrid_element_tag rhs);

viennagrid_bool viennagrid_dynamic_storage(viennagrid_element_tag et);
viennagrid_bool viennagrid_static_storage(viennagrid_element_tag et);

viennagrid_int viennagrid_boundary_element_count( viennagrid_element_tag host, viennagrid_dimension boundary_topo_dim );
viennagrid_int viennagrid_boundary_element_count_from_element_tag( viennagrid_element_tag host, viennagrid_element_tag boundary );

viennagrid_bool viennagrid_is_boundary_tag(viennagrid_element_tag host, viennagrid_element_tag boundary);
const char * viennagrid_element_tag_string(viennagrid_element_tag element_tag);



/* creates a mesh_hierarchy with a mesh root */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_create(viennagrid_mesh_hierarchy * mesh_hierarchy);
/* deletes a mesh_hierarchy with all its meshes */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy mesh_hierarchy);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy mesh_hierarchy);

/* set/get mesh options */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_option_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_int flag,
                                                                                viennagrid_int option);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_option_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_int flag,
                                                                                viennagrid_int * option);



/* queries the topologic dimension of a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_cell_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                        viennagrid_dimension * topologic_dimension);

/* queries the geometric dimension of a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                             viennagrid_dimension * geometric_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                             viennagrid_int geometric_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_clear(viennagrid_mesh_hierarchy mesh_hierarchy);


/* get the root mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_root_mesh_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   viennagrid_mesh * mesh);
/* get the mesh_hierarchy of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_mesh_hierarchy_get(viennagrid_mesh mesh,
                                                                              viennagrid_mesh_hierarchy * mesh_hierarchy);

/* creates a mesh using a parent mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_create(viennagrid_mesh parent,
                                                                  viennagrid_mesh * mesh);
/* deletes a mesh and all child meshes */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_delete(viennagrid_mesh mesh);
/* queries the number of children */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh mesh,
                                                                          viennagrid_int * children_count);
/* gets a child mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_child_get(viennagrid_mesh mesh,
                                                                     viennagrid_int id,
                                                                     viennagrid_mesh * child);





VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_element_count(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   viennagrid_element_tag element_tag,
                                                                                   viennagrid_int * count);

/* creates a set of vertices in a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_vertex_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    const viennagrid_numeric * coords,
                                                                    viennagrid_int * vertex_id);
/* gets the coordinates of a vertex of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_point_pointer(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                     viennagrid_numeric ** coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_point_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                viennagrid_int id,
                                                                viennagrid_numeric ** coords);


/* indices are vertex indices for all elements but PLCs, where they are line indices */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_element_tag element_tag,
                                                                    viennagrid_int vertex_count,
                                                                    viennagrid_int * vertex_indices,
                                                                    viennagrid_int * element_index);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_create_refinement(
                                                            viennagrid_mesh mesh,
                                                            viennagrid_dimension element_topo_dim,
                                                            viennagrid_int element_id,
                                                            viennagrid_element_tag    refined_element_tag,
                                                            viennagrid_int            refined_element_base_count,
                                                            viennagrid_int *        refined_element_base_indices,
                                                            viennagrid_int            intersects_count,
                                                            viennagrid_int *        intersect_vertices_indices,
                                                            viennagrid_int *        intersects_indices,
                                                            viennagrid_dimension *    intersects_topo_dims,
                                                            viennagrid_int *        id);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_tag_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                      viennagrid_dimension element_topo_dim,
                                                                      viennagrid_int element_id,
                                                                      viennagrid_element_tag * element_tag);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_pointers(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_dimension boundary_topo_dim,
                                                                                viennagrid_int ** boundary_offsets,
                                                                                viennagrid_int ** boundary_indices);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_dimension boundary_topo_dim,
                                                                                viennagrid_int ** boundary_element_index_begin,
                                                                                viennagrid_int ** boundary_element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                                                  viennagrid_dimension element_topo_dim,
                                                                                  viennagrid_int element_id,
                                                                                  viennagrid_dimension coboundary_topo_dim,
                                                                                  viennagrid_int ** coboundary_element_index_begin,
                                                                                  viennagrid_int ** coboundary_element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_dimension connector_topo_dim,
                                                                                viennagrid_dimension neighbor_topo_dim,
                                                                                viennagrid_int ** neighbor_element_index_begin,
                                                                                viennagrid_int ** neighbor_element_index_end);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_is_boundary_mesh(viennagrid_mesh mesh,
                                                                       viennagrid_dimension element_topo_dim,
                                                                       viennagrid_int element_id,
                                                                       viennagrid_bool * result);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_is_boundary_region(viennagrid_region region,
                                                                         viennagrid_mesh mesh,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_int element_id,
                                                                         viennagrid_bool * result);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_elements_get(viennagrid_mesh mesh,
                                                                   viennagrid_dimension element_topo_dim,
                                                                   viennagrid_int ** element_index_begin,
                                                                   viennagrid_int ** element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_add(viennagrid_mesh mesh,
                                                                  viennagrid_dimension element_topo_dim,
                                                                  viennagrid_int element_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_int element_id,
                                                                         viennagrid_int * element_parent_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_int element_id,
                                                                         viennagrid_int element_parent_id);







VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_region * region);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                        viennagrid_int region_id,
                                                                        viennagrid_region * region);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                 viennagrid_int region_id,
                                                                 viennagrid_region * region);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_mesh_hierarchy_get(viennagrid_region region,
                                                                                viennagrid_mesh_hierarchy * mesh_hierarchy);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_name_get(viennagrid_region region,
                                                                      const char ** region_name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_name_set(viennagrid_region region,
                                                                      const char * region_name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_id_get(viennagrid_region region,
                                                                    viennagrid_int * id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_regions_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                 viennagrid_region ** region_begin,
                                                                                 viennagrid_region ** region_end);




VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_regions_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                          viennagrid_dimension element_topo_dim,
                                                                          viennagrid_int element_id,
                                                                          viennagrid_region ** region_begin,
                                                                          viennagrid_region ** region_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_add_to_region(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_dimension element_topo_dim,
                                                                    viennagrid_int element_id,
                                                                    viennagrid_region region);









#define VIENNAGRID_PLC_POLY_READER_CANNOT_OPEN_FILE 100
#define VIENNAGRID_PLC_POLY_READER_FILE_EMPTY 101
#define VIENNAGRID_PLC_POLY_READER_EOF_ENCOUNTERED 102
#define VIENNAGRID_PLC_POLY_READER_INVALID_VERTEX_COUNT 103
#define VIENNAGRID_PLC_POLY_READER_INVALID_ATTRIBUTE_COUNT 104
#define VIENNAGRID_PLC_POLY_READER_INVALID_BOUNDARY_MARKER_COUNT 105
#define VIENNAGRID_PLC_POLY_READER_INVALID_FACET_COUNT 106
#define VIENNAGRID_PLC_POLY_READER_INVALID_FACET_POLYGON_COUNT 107
#define VIENNAGRID_PLC_POLY_READER_INVALID_FACET_HOLE_POINT_COUNT 108
#define VIENNAGRID_PLC_POLY_READER_INVALID_POLYGON_VERTEX_COUNT 109
#define VIENNAGRID_PLC_POLY_READER_INVALID_VERTEX_ID 110
#define VIENNAGRID_PLC_POLY_READER_INVALID_HOLE_POINT_COUNT 111
#define VIENNAGRID_PLC_POLY_READER_INVALID_SEED_POINT_COUNT 112
#define VIENNAGRID_PLC_MESH_TYPE_NOT_SUPPORTED_FOR_CONVERSION 120


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_make(viennagrid_plc * plc);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_delete(viennagrid_plc plc);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_clear(viennagrid_plc plc);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                                                        viennagrid_plc dst_plc,
                                                                        viennagrid_int copy_hole_points,
                                                                        viennagrid_int copy_seed_points);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                                                  viennagrid_dimension * geometric_dimension);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                                                  viennagrid_int geometric_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_element_count_get(viennagrid_plc plc,
                                                                            viennagrid_dimension topologic_dimension,
                                                                            viennagrid_int * element_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_create(viennagrid_plc plc,
                                                                        const viennagrid_numeric * coords,
                                                                        viennagrid_int * vertex_id);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_point_pointer(viennagrid_plc plc,
                                                                        viennagrid_numeric ** coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_point_get(viennagrid_plc plc,
                                                                    viennagrid_int id,
                                                                    viennagrid_numeric ** coords);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                                                      viennagrid_int vertex_index0,
                                                                      viennagrid_int vertex_index1,
                                                                      viennagrid_int * line_id);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                                                       viennagrid_int * line_indices,
                                                                       viennagrid_int line_count,
                                                                       viennagrid_int * facet_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                                            viennagrid_dimension element_topo_dim,
                                                                            viennagrid_int element_id,
                                                                            viennagrid_dimension boundary_topo_dim,
                                                                            viennagrid_int ** boundary_element_index_begin,
                                                                            viennagrid_int ** boundary_element_index_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_point_add(viennagrid_plc plc,
                                                                               viennagrid_int facet_id,
                                                                               const viennagrid_numeric * coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_point_delete(viennagrid_plc plc,
                                                                                  viennagrid_int facet_id,
                                                                                  viennagrid_int point_index);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_points_get(viennagrid_plc plc,
                                                                                viennagrid_int facet_id,
                                                                                viennagrid_numeric ** coords,
                                                                                viennagrid_int * hole_point_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_hole_point_add(viennagrid_plc plc,
                                                                         const viennagrid_numeric * coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_hole_points_get(viennagrid_plc plc,
                                                                          viennagrid_numeric ** coords,
                                                                          viennagrid_int * count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_point_add(viennagrid_plc plc,
                                                                         const viennagrid_numeric * coords,
                                                                         viennagrid_int region_id);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_points_get(viennagrid_plc plc,
                                                                          viennagrid_numeric ** coords,
                                                                          viennagrid_int ** region_ids,
                                                                          viennagrid_int * count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_read_tetgen_poly(viennagrid_plc plc,
                                                                           const char * filename);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_refine_lines(viennagrid_plc plc,
                                                                       viennagrid_plc output_plc,
                                                                       viennagrid_numeric line_size);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_from_mesh(viennagrid_mesh mesh,
                                                                    viennagrid_plc plc); // TODO implement




/* serialization/de-serialization */

struct viennagrid_serialized_mesh_hierarchy_
{
  /* geometric information and vertices */
  viennagrid_dimension geometric_dimension;
  viennagrid_int vertex_count;
  viennagrid_numeric * points;                  /* may be shared with mesh hierarchy */

  /* cell information */
  viennagrid_int cell_count;
  viennagrid_dimension cell_dimension;
  viennagrid_element_tag * cell_element_tags;   /* may be shared with mesh hierarchy */
  viennagrid_int * cell_vertex_offsets;       /* may be shared with mesh hierarchy */
  viennagrid_int * cell_vertices;             /* may be shared with mesh hierarchy */
  viennagrid_int * cell_parents;              /* may be shared with mesh hierarchy */
  viennagrid_int * cell_region_offsets;
  viennagrid_int * cell_regions;

  /* mesh information */
  viennagrid_int mesh_count;
  viennagrid_int * mesh_parents;
  viennagrid_int * mesh_cell_count;
  viennagrid_int ** mesh_cells;               /* array pointers may be shared with mesh hierarchy */

  /* region information */
  viennagrid_int region_count;
  viennagrid_int * region_ids;
  const char ** region_names;                         /* strings may be shared with mesh hierarchy */

  /* for internal use */
  viennagrid_bool data_owned;
};

typedef struct viennagrid_serialized_mesh_hierarchy_ * viennagrid_serialized_mesh_hierarchy;

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_new(viennagrid_int size,
                                                          void ** ptr);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_delete(void ** ptr);

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
