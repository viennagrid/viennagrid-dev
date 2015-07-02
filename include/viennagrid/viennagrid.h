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



/**********************************************************************************************
 *
 *                ViennaGrid types
 *
 **********************************************************************************************/


typedef int viennagrid_bool;
typedef int viennagrid_int;
typedef viennagrid_int viennagrid_error;
typedef double viennagrid_numeric;
typedef unsigned char viennagrid_element_type;
typedef int viennagrid_dimension;
typedef unsigned char viennagrid_flag;


typedef struct viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;
typedef struct viennagrid_mesh_ * viennagrid_mesh;
typedef struct viennagrid_region_ * viennagrid_region;
typedef struct viennagrid_plc_ * viennagrid_plc;
typedef struct viennagrid_quantity_field_t * viennagrid_quantity_field;
typedef struct viennagrid_mesh_io_ * viennagrid_mesh_io;


/**********************************************************************************************
 *
 *                ViennaGrid defines
 *
 **********************************************************************************************/


/* VIENNAGRID ERROR CODES */
#define VIENNAGRID_SUCCESS                                              0
#define VIENNAGRID_ERROR_UNSPECIFIED_ERROR                              1
#define VIENNAGRID_ERROR_OUT_OF_MEMORY                                  2
#define VIENNAGRID_ERROR_INVALID_ARGUMENTS                              3
#define VIENNAGRID_ERROR_UNKNOWN_FLAG                                   4
#define VIENNAGRID_ERROR_UNSUPPORTED_BOUNDARY_LAYOUT                    5
#define VIENNAGRID_ERROR_DESERIALIZE_MAGIC_VALUE_MISSMATCH              6
#define VIENNAGRID_ERROR_DESERIALIZE_VERSION_MISSMATCH                  7
#define VIENNAGRID_ERROR_PLC_MESH_TYPE_NOT_SUPPORTED_FOR_CONVERSION     8
#define VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE                          100
#define VIENNAGRID_ERROR_IO_FILE_EMPTY                                101
#define VIENNAGRID_ERROR_IO_MESH_IS_NOT_ROOT                          102
#define VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED                           103
#define VIENNAGRID_ERROR_IO_EOF_WHILE_READING_VERTICES                104
#define VIENNAGRID_ERROR_IO_VERTEX_DIMENSION_MISMATCH                 105
#define VIENNAGRID_ERROR_IO_EOF_WHILE_READING_CELL_COUNT              106
#define VIENNAGRID_ERROR_IO_EOF_WHILE_READING_CELLS                   107
#define VIENNAGRID_ERROR_IO_UNKNOWN_FILETYPE                          108
#define VIENNAGRID_ERROR_IO_INVALID_VERTEX_COUNT                      109
#define VIENNAGRID_ERROR_IO_INVALID_ATTRIBUTE_COUNT                   110
#define VIENNAGRID_ERROR_IO_INVALID_BOUNDARY_MARKER_COUNT             111
#define VIENNAGRID_ERROR_IO_INVALID_FACET_COUNT                       112
#define VIENNAGRID_ERROR_IO_INVALID_FACET_POLYGON_COUNT               113
#define VIENNAGRID_ERROR_IO_INVALID_FACET_HOLE_POINT_COUNT            114
#define VIENNAGRID_ERROR_IO_INVALID_POLYGON_VERTEX_COUNT              115
#define VIENNAGRID_ERROR_IO_INVALID_VERTEX_ID                         116
#define VIENNAGRID_ERROR_IO_INVALID_HOLE_POINT_COUNT                  117
#define VIENNAGRID_ERROR_IO_INVALID_SEED_POINT_COUNT                  118

/* VIENNAGRID BOOL DEFINES */
#define VIENNAGRID_TRUE                             1
#define VIENNAGRID_FALSE                            0

/* VIENNAGRID OPTION FLAGS */
#define VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT         0

/* VIENNAGRID OPTIONS */
#define VIENNAGRID_BOUNDARY_LAYOUT_FULL             0
#define VIENNAGRID_BOUNDARY_LAYOUT_SPARSE           1

/* VIENNAGRID ELEMENT TYPES */
#define VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT        255 /* -1 */
#define VIENNAGRID_ELEMENT_TYPE_START               0
#define VIENNAGRID_ELEMENT_TYPE_VERTEX              0
#define VIENNAGRID_ELEMENT_TYPE_LINE                1
#define VIENNAGRID_ELEMENT_TYPE_EDGE                1
#define VIENNAGRID_ELEMENT_TYPE_TRIANGLE            2
#define VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL       3
#define VIENNAGRID_ELEMENT_TYPE_POLYGON             4
#define VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON         5
#define VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON          6
#define VIENNAGRID_ELEMENT_TYPE_COUNT               7

#define VIENNAGRID_TOPOLOGIC_DIMENSION_END          4


/* VIENNAGRID FILETYPES */
#define VIENNAGRID_FILETYPE_UNKNOWN                 0
#define VIENNAGRID_FILETYPE_NETGEN_MESH             1

/* QUANTITY FIELD STORAGE TYPES */
#define VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE     0
#define VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE    1

/* VIENNAGRID MAGIC VALUE FOR SERIALIZATION */
#define VIENNAGRID_MAGIC_VALUE                      0x42F42F42

// TODO move to cmake??
#define VIENNAGRID_VERSION_MAJOR                    _VERSION_MAJOR_
#define VIENNAGRID_VERSION_MINOR                    _VERSION_MINOR_
#define VIENNAGRID_VERSION_PATCH                    _VERSION_PATCH_


/**********************************************************************************************
 *
 *                static functions
 *
 **********************************************************************************************/


/* returns the topological dimension of a given element type */
static inline viennagrid_dimension viennagrid_topological_dimension(viennagrid_element_type et)
{
  switch (et)
  {
  case VIENNAGRID_ELEMENT_TYPE_VERTEX:
    return 0;
  case VIENNAGRID_ELEMENT_TYPE_LINE:
    return 1;
  case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
  case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
  case VIENNAGRID_ELEMENT_TYPE_POLYGON:
    return 2;
  case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
  case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    return 3;
  default:
    return -1;
  }
}

/* returns the boundary element count given a host element type and a boundary topological dimension, -1 is returned if the size is not static (e.g. polygon) */
static inline viennagrid_int viennagrid_boundary_element_count( viennagrid_element_type host, viennagrid_dimension boundary_topo_dim )
{
  switch (host)
  {
    case VIENNAGRID_ELEMENT_TYPE_LINE:
      if (boundary_topo_dim == 0) return 2;

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      if (boundary_topo_dim == 1) return 3;
      if (boundary_topo_dim == 0) return 3;

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      if (boundary_topo_dim == 1) return 4;
      if (boundary_topo_dim == 0) return 4;

    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      if (boundary_topo_dim == 1) return -1;
      if (boundary_topo_dim == 0) return -1;

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      if (boundary_topo_dim == 2) return 4;
      if (boundary_topo_dim == 1) return 6;
      if (boundary_topo_dim == 0) return 4;

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      if (boundary_topo_dim == 2) return 6;
      if (boundary_topo_dim == 1) return 12;
      if (boundary_topo_dim == 0) return 8;
  }

  return 0;
}

/* returns the boundary element count given a host element type and a boundary element type, -1 is returned if the size is not static (e.g. polygon) */
static inline viennagrid_int viennagrid_boundary_element_count_from_element_type( viennagrid_element_type host, viennagrid_element_type boundary )
{
  switch (host)
  {
    case VIENNAGRID_ELEMENT_TYPE_LINE:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 2;

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 3;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 3;

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 4;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 4;

    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return -1;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return -1;

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_TRIANGLE) return 4;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 6;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 4;

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL) return 6;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 12;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 8;
  }

  return 0;
}

/* queries if a given element type is boundary of a given host type */
static inline viennagrid_bool viennagrid_is_boundary_type(viennagrid_element_type host, viennagrid_element_type boundary)
{
  return viennagrid_boundary_element_count_from_element_type(host, boundary) != 0 ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}

/* return the string of a given element type */
static inline const char * viennagrid_element_type_string(viennagrid_element_type element_type)
{
  switch (element_type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
      return "vertex";

    case VIENNAGRID_ELEMENT_TYPE_LINE:
      return "line";

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      return "triangle";
    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      return "quadrilateral";
    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      return "polygon";

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      return "tetrahedron";
    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      return "hexahedron";

    default:
      return 0;
  }
}


/**********************************************************************************************
 *
 *                general viennagrid functions
 *
 **********************************************************************************************/


/* allocates a buffer */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_new(viennagrid_int size, void ** ptr);

/* deletes a buffer */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_delete(void ** ptr);


/**********************************************************************************************
 *
 *                viennagrid mesh_hierarchy/mesh functions
 *
 **********************************************************************************************/


/* creates a mesh_hierarchy with a mesh root and a reference count of 1 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_create(viennagrid_mesh_hierarchy * mesh_hierarchy);
/* increases the reference counter of a mesh hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy mesh_hierarchy);
/* decreases the reference counter of a mesh hierarchy, if the reference counter is less or equal to zero, the object is deleted */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy mesh_hierarchy);

/* serializes a mesh hierarchy to a binary buffer, the buffer has to be deleted using viennagrid_delete */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_serialize(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                               void ** blob,
                                                                               viennagrid_int * size);
/* deserializes a binary buffer to a mesh hierarchy, the mesh hierarchy is cleared before deserialization */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_deserialize(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                 void * blob,
                                                                                 viennagrid_int size);

/* set/get mesh options */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_property_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                  viennagrid_int property,
                                                                                  viennagrid_int value);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_property_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                  viennagrid_int property,
                                                                                  viennagrid_int * value);



/* queries the topologic dimension of cell elements of a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_cell_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                        viennagrid_dimension * topologic_dimension);

/* queries the geometric dimension of a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                             viennagrid_dimension * geometric_dimension);
/* queries the geometric dimension of the mesh_hierarchy a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_geometric_dimension_get(viennagrid_mesh mesh,
                                                                                   viennagrid_dimension * geometric_dimension);

/* setting geometric dimension of a mesh will invalid all points (if geometric dimension changes) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                             viennagrid_int geometric_dimension);
/* setting geometric dimension of the mesh hierarchy of a mesh will invalid all points (if geometric dimension changes) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_geometric_dimension_set(viennagrid_mesh mesh,
                                                                                   viennagrid_int geometric_dimension);

/* clears a mesh hierarchy (removes all elements, meshes, regions, ... but does not touch reference counter) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_clear(viennagrid_mesh_hierarchy mesh_hierarchy);


/* get the root mesh of the mesh hierarchy */
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

/* gets the parent mesh, if mesh is the root mesh, parent will be NULL */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_parent_get(viennagrid_mesh mesh,
                                                                      viennagrid_mesh * parent);

/* queries mesh name, if mesh name is not set, a NULL pointer is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_name_get(viennagrid_mesh mesh,
                                                                    const char ** mesh_name);

/* set the mesh name, if mesh name can be NULL to unset a name */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_name_set(viennagrid_mesh mesh,
                                                                    const char * mesh_name);



/* queries the number of element of a specific element type */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_element_count(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   viennagrid_element_type element_type,
                                                                                   viennagrid_int * count);

/* creates a vertex in a mesh hierarchy, the ID of the created vertex will be returned in vertex_id (optional, will be ignored if vertex_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_vertex_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   const viennagrid_numeric * coords,
                                                                                   viennagrid_int * vertex_id);
/* creates a vertex in a mesh, the ID of the created vertex will be returned in vertex_id (optional, will be ignored if vertex_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_vertex_create(viennagrid_mesh mesh,
                                                                         const viennagrid_numeric * coords,
                                                                         viennagrid_int * vertex_id);

/* gets the coordinate pointer of all vertices of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_vertex_coords_pointer(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                           viennagrid_numeric ** coords);
/* gets the coordinates of a specific vertex */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_vertex_coords_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                       viennagrid_int id,
                                                                                       viennagrid_numeric ** coords);

/* creates an element of specific type in a mesh hierarchy. the element is defined by its vertices, the ID of the created element will be returned in element_id (optional, will be ignored if element_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_element_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                    viennagrid_element_type element_type,
                                                                                    viennagrid_int vertex_count,
                                                                                    viennagrid_int * vertex_ids,
                                                                                    viennagrid_int * element_id);
/* creates an element of specific type in a mesh. the element is defined by its vertices, the ID of the created element will be returned in element_id (optional, will be ignored if element_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_create(viennagrid_mesh mesh,
                                                                          viennagrid_element_type element_type,
                                                                          viennagrid_int vertex_count,
                                                                          viennagrid_int * vertex_ids,
                                                                          viennagrid_int * element_id);

/*
 * create one refined element
 *
 * parent element: element_topo_dim/element_id
 * new element type: refined_element_type
 * vertices of new element: refined_element_vertex_count & refined_element_vertex_ids
 * intersection elements (e.g. a line is intersected by a vertex): intersects_topo_dims[i]/intersects_ids[i]
 * vertex, which intersect the elements: intersect_vertices_ids[i]
 * return value, new element id: id
*/
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_refined_element_create(
                                                            viennagrid_mesh mesh,
                                                            viennagrid_dimension element_topo_dim,
                                                            viennagrid_int element_id,
                                                            viennagrid_element_type    refined_element_type,
                                                            viennagrid_int            refined_element_vertex_count,
                                                            viennagrid_int *        refined_element_vertex_ids,
                                                            viennagrid_int            intersects_count,
                                                            viennagrid_int *        intersect_vertices_ids,
                                                            viennagrid_int *        intersects_ids,
                                                            viennagrid_dimension *    intersects_topo_dims,
                                                            viennagrid_int *        id);


/* queries the type of an element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_type_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                       viennagrid_dimension element_topo_dim,
                                                                       viennagrid_int element_id,
                                                                       viennagrid_element_type * element_type);

/* gets the boundary element pointers of a specific topological dimension for a given boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_element_pointers(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                        viennagrid_dimension element_topo_dim,
                                                                                        viennagrid_dimension boundary_topo_dim,
                                                                                        viennagrid_int ** boundary_offsets,
                                                                                        viennagrid_int ** boundary_ids);

/* gets the boundary elements of a specific element for a given boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_dimension boundary_topo_dim,
                                                                                viennagrid_int ** boundary_element_id_begin,
                                                                                viennagrid_int ** boundary_element_id_end);

/* gets the coboundary element pointers of a specific topological dimension for a given coboundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_element_pointers(viennagrid_mesh mesh,
                                                                                          viennagrid_dimension element_topo_dim,
                                                                                          viennagrid_dimension coboundary_topo_dim,
                                                                                          viennagrid_int ** coboundary_element_ids_begin,
                                                                                          viennagrid_int ** coboundary_element_ids_end);

/* gets the coboundary elements of a specific element for a given coboundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                                                  viennagrid_dimension element_topo_dim,
                                                                                  viennagrid_int element_id,
                                                                                  viennagrid_dimension coboundary_topo_dim,
                                                                                  viennagrid_int ** coboundary_element_ids_begin,
                                                                                  viennagrid_int ** coboundary_element_ids_end);


/* gets the neighbor element pointers of a specific topological dimension for a given neighbor and connector topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements_pointers(viennagrid_mesh mesh,
                                                                                         viennagrid_dimension element_topo_dim,
                                                                                         viennagrid_dimension connector_topo_dim,
                                                                                         viennagrid_dimension neighbor_topo_dim,
                                                                                         viennagrid_int ** neighbor_element_ids_begin,
                                                                                         viennagrid_int ** neighbor_element_ids_end);

/* gets the neighbor elements of a specific element for a given neighbor and connector topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_dimension connector_topo_dim,
                                                                                viennagrid_dimension neighbor_topo_dim,
                                                                                viennagrid_int ** neighbor_element_ids_begin,
                                                                                viennagrid_int ** neighbor_element_ids_end);


/* queries if a given element is boundary of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_is_mesh_boundary(viennagrid_mesh mesh,
                                                                               viennagrid_dimension element_topo_dim,
                                                                               viennagrid_int element_id,
                                                                               viennagrid_bool * result);

/* queries if a given element is boundary of a mesh region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_is_region_boundary(viennagrid_region region,
                                                                                 viennagrid_mesh mesh,
                                                                                 viennagrid_dimension element_topo_dim,
                                                                                 viennagrid_int element_id,
                                                                                 viennagrid_bool * result);


/* gets all elements of a mesh for a given topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_elements_get(viennagrid_mesh mesh,
                                                                   viennagrid_dimension element_topo_dim,
                                                                   viennagrid_int ** element_ids_begin,
                                                                   viennagrid_int ** element_ids_end);

/* adds a given element to a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_add(viennagrid_mesh mesh,
                                                                       viennagrid_dimension element_topo_dim,
                                                                       viennagrid_int element_id);

/* queries the parent element of a given element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_int element_id,
                                                                         viennagrid_int * element_parent_id);

/* sets the parent element for a given element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                         viennagrid_dimension element_topo_dim,
                                                                         viennagrid_int element_id,
                                                                         viennagrid_int element_parent_id);





/* creates a new region using the next ID which is available */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                    viennagrid_region * region);

/* queries the region with a given ID, if the region is not present, a new region with that ID is created */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_or_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                           viennagrid_int region_id,
                                                                           viennagrid_region * region);
/* queries the region with a given ID, if the region is not present, NULL is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                 viennagrid_int region_id,
                                                                 viennagrid_region * region);
/* queries the mesh hierarchy of a given region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_mesh_hierarchy_get(viennagrid_region region,
                                                                                viennagrid_mesh_hierarchy * mesh_hierarchy);



/* queries region name, if region name is not set, a NULL pointer is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_name_get(viennagrid_region region,
                                                                      const char ** region_name);
/* set the region name, if region name can be NULL to unset a name */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_name_set(viennagrid_region region,
                                                                      const char * region_name);
/* queries the ID of a given region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_id_get(viennagrid_region region,
                                                                    viennagrid_int * id);
/* get the region pointers (all regions) of a mesh hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_regions_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                 viennagrid_region ** region_begin,
                                                                                 viennagrid_region ** region_end);

/* get the region pointers (all regions) of a given element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_regions_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                          viennagrid_dimension element_topo_dim,
                                                                          viennagrid_int element_id,
                                                                          viennagrid_region ** region_begin,
                                                                          viennagrid_region ** region_end);
/* adds an element to a region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_add_to_region(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                            viennagrid_dimension element_topo_dim,
                                                                            viennagrid_int element_id,
                                                                            viennagrid_region region);


/**********************************************************************************************
 *
 *                viennagrid PLC functions
 *
 **********************************************************************************************/


/* creates a PLC with a reference count of 1 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_create(viennagrid_plc * plc);
/* increases the reference counter of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_retain(viennagrid_plc plc);
/* decreases the reference counter of a PLC, if the reference counter is less or equal to zero, the object is deleted */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_release(viennagrid_plc plc);

/* clears a PLC (removes all elements, meshes, regions, ... but does not touch reference counter) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_clear(viennagrid_plc plc);

/* initialize a PLC from another PLC (copy geometric dimension, hole points and seed points) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                                                        viennagrid_plc dst_plc,
                                                                        viennagrid_int copy_hole_points,
                                                                        viennagrid_int copy_seed_points);

/* queries the geometric dimension of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                                                  viennagrid_dimension * geometric_dimension);

/* setting geometric dimension will invalid all points (if geometric dimension changes) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                                                  viennagrid_int geometric_dimension);

/* queries the element count for a given topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_element_count_get(viennagrid_plc plc,
                                                                            viennagrid_dimension topo_dim,
                                                                            viennagrid_int * element_count);

/* creates a vertex in a PLC, the created vertex ID is returned (optional, ignored if vertex_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_create(viennagrid_plc plc,
                                                                        const viennagrid_numeric * coords,
                                                                        viennagrid_int * vertex_id);
/* gets the coordinates pointer of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_coords_pointer(viennagrid_plc plc,
                                                                                viennagrid_numeric ** coords);
/* gets the coordinates of a given vertex in a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_coords_get(viennagrid_plc plc,
                                                                            viennagrid_int id,
                                                                            viennagrid_numeric ** coords);

/* creates a line in a PLC using two vertex IDs, the created line ID is returned (optional, ignored if line_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                                                      viennagrid_int vertex_id0,
                                                                      viennagrid_int vertex_id1,
                                                                      viennagrid_int * line_id);
/* creates a line in a PLC using a buffer of line IDs, the created facet ID is returned (optional, ignored if facet_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                                                       viennagrid_int * line_ids,
                                                                       viennagrid_int line_count,
                                                                       viennagrid_int * facet_id);

/* get the boundary elements of a specific element given a boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                                            viennagrid_dimension element_topo_dim,
                                                                            viennagrid_int element_id,
                                                                            viennagrid_dimension boundary_topo_dim,
                                                                            viennagrid_int ** boundary_element_ids_begin,
                                                                            viennagrid_int ** boundary_element_ids_end);

/* adds a hole point to a given facet */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_point_add(viennagrid_plc plc,
                                                                               viennagrid_int facet_id,
                                                                               const viennagrid_numeric * coords);
/* deletes a hole point from a given facet */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_point_delete(viennagrid_plc plc,
                                                                                  viennagrid_int facet_id,
                                                                                  viennagrid_int point_id);
/* gets the coordinates and the count of a all hole points on a facet */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_points_get(viennagrid_plc plc,
                                                                                viennagrid_int facet_id,
                                                                                viennagrid_numeric ** coords,
                                                                                viennagrid_int * hole_point_count);

/* adds a volumetric hole point to a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_volumetric_hole_point_add(viennagrid_plc plc,
                                                                                    const viennagrid_numeric * coords);
/* deletes a volumetric hole point from a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_volumetric_hole_point_delete(viennagrid_plc plc,
                                                                                       viennagrid_int point_id);
/* gets the coordinates and the count of a all volumetric hole points of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_volumetric_hole_points_get(viennagrid_plc plc,
                                                                                     viennagrid_numeric ** coords,
                                                                                     viennagrid_int * count);

/* adds a seed hole point to a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_point_add(viennagrid_plc plc,
                                                                         const viennagrid_numeric * coords,
                                                                         viennagrid_int region_id);
/* deletes a seed point from a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_point_delete(viennagrid_plc plc,
                                                                            viennagrid_int point_id);
/* gets the coordinates, the region_ids and the count of a all seed hole points of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_points_get(viennagrid_plc plc,
                                                                          viennagrid_numeric ** coords,
                                                                          viennagrid_int ** region_ids,
                                                                          viennagrid_int * count);

/* refines the lines of a PLC that no line in output_plc is greater than line_size */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_line_refine(viennagrid_plc plc,
                                                                      viennagrid_plc output_plc,
                                                                      viennagrid_numeric line_size);

/* converts a mesh to a PLC (if possible) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_convert_mesh_to_plc(viennagrid_mesh mesh,
                                                                          viennagrid_plc plc);


/**********************************************************************************************
 *
 *                viennagrid quantity field functions
 *
 **********************************************************************************************/


/* creates a quantity field with a reference count of 1 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_create(viennagrid_quantity_field * quantity_field);
/* increases the reference counter of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field);
/* decreases the reference counter of a quantity field, if the reference counter is less or equal to zero, the object is deleted */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field);

/* initializes a quantity field with a given topological dimension, size of a value and storage layout */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                                          viennagrid_dimension topo_dim,
                                                                          viennagrid_int size_of_value,
                                                                          viennagrid_int storage_layout);

/* queries the topologic dimension of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_topologic_dimension_get(viennagrid_quantity_field quantity_field,
                                                                                             viennagrid_dimension * topo_dim);
/* queries the storage layout of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_storage_layout_get(viennagrid_quantity_field quantity_field,
                                                                                        viennagrid_int * storage_layout);
/* queries the size of a value of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_size_of_value_get(viennagrid_quantity_field quantity_field,
                                                                                       viennagrid_dimension * size_of_value);


/* set the name of a quantity field, the name can be NULL to unset a name */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_name_set(viennagrid_quantity_field quantity_field,
                                                                              const char * name);
/* queries the name of a quantity field, if the name is not set, a NULL pointer is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_name_get(viennagrid_quantity_field quantity_field,
                                                                              const char ** name);

/* set the unit of a quantity field, the unit can be NULL to unset the unit */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_unit_set(viennagrid_quantity_field quantity_field,
                                                                              const char * unit);
/* queries the unit of a quantity field, if the unit is not set, a NULL pointer is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_unit_get(viennagrid_quantity_field quantity_field,
                                                                              const char ** unit);

/* resizes the quantity field, is ignored is storage layout is sparse */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                                                            viennagrid_int value_count);

/* returns the values stored */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                                                           viennagrid_int * value_count);

/* sets the value for a specific element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_value_set(viennagrid_quantity_field quantity_field,
                                                                               viennagrid_int element_id,
                                                                               void * value);
/* gets the pointer to the value for a specific element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_value_get(viennagrid_quantity_field quantity_field,
                                                                               viennagrid_int element_id,
                                                                               void ** value);


/**********************************************************************************************
 *
 *                viennagrid IO functions
 *
 **********************************************************************************************/


/* creates a mesh IO object with a reference count of 1 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_create(viennagrid_mesh_io * mesh_io);
/* increases the reference counter of a mesh IO object */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_retain(viennagrid_mesh_io mesh_io);
/* decreases the reference counter of a mesh IO object, if the reference counter is less or equal to zero, the object is deleted */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_release(viennagrid_mesh_io mesh_io);

/* clears a mesh IO object (removes the mesh and all quantities, ... but does not touch reference counter) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_clear(viennagrid_mesh_io mesh_io);

/* sets the mesh of the mesh IO object */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_mesh_set(viennagrid_mesh_io mesh_io, viennagrid_mesh mesh);
/* gets the mesh of the mesh IO object */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_mesh_get(viennagrid_mesh_io mesh_io, viennagrid_mesh * mesh);

/* queries the number of quantity fields */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_count(viennagrid_mesh_io mesh_io,
                                                                                   viennagrid_int * count);
/* gets a quantity field by index, if the index is invalid, NULL is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_get_by_index(viennagrid_mesh_io mesh_io,
                                                                                          viennagrid_int index,
                                                                                          viennagrid_quantity_field * quantity_field);

/* gets a quantity field by its name, if no quantity field with the given name is present, NULL is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_get(viennagrid_mesh_io mesh_io,
                                                                                 const char * quantity_name,
                                                                                 viennagrid_quantity_field * quantity_field);
/* sets a quantity field, the name of the quantity field is used for identification */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_set(viennagrid_mesh_io mesh_io,
                                                                                 viennagrid_quantity_field quantity_field);
/* unsets a quantity field with a given quantity name */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_unset(viennagrid_mesh_io mesh_io,
                                                                                   const char * quantity_name);


/* tries to determine the filetype using a filename, VIENNAGRID_FILETYPE_UNKNOWN is returned if not possible */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_filetype_from_filename(const char * filename,
                                                                                     viennagrid_int * filetype);

/* reads a mesh from a file with a given filetype */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_read_with_filetype(viennagrid_mesh_io mesh_io,
                                                                                 const char * filename,
                                                                                 viennagrid_int filetype);

/* reads a mesh from a file, automatically tries to determine the filetype from the filename */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_read(viennagrid_mesh_io mesh_io,
                                                                   const char * filename);


/* writes a mesh to a file with a given filetype */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_write_with_filetype(viennagrid_mesh_io mesh_io,
                                                                                  const char * filename,
                                                                                  viennagrid_int filetype);

/* writes a mesh to a file, automatically tries to determine the filetype from the filename */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_write(viennagrid_mesh_io mesh_io,
                                                                    const char * filename);


/* reads a PLC using tetgen .poly file format */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_read_tetgen_poly(viennagrid_plc plc,
                                                                           const char * filename);


#endif
