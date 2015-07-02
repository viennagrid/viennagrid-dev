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
typedef unsigned char viennagrid_element_type;
typedef int viennagrid_dimension;
typedef unsigned char viennagrid_flag;


typedef struct viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;
typedef struct viennagrid_mesh_ * viennagrid_mesh;
typedef struct viennagrid_region_ * viennagrid_region;
typedef struct viennagrid_plc_ * viennagrid_plc;
typedef struct viennagrid_quantity_field_t * viennagrid_quantity_field;
typedef struct viennagrid_mesh_io_ * viennagrid_mesh_io;



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
#define VIENNAGRID_FILETYPE_NETGEN_MESH             1

/* QUANTITY FIELD STORAGE TYPES */
#define VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE     0
#define VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE    1




#define VIENNAGRID_MAGIC_VALUE                      0x42F42F42

// TODO move to cmake??
#define VIENNAGRID_VERSION_MAJOR                    3
#define VIENNAGRID_VERSION_MINOR                    0
#define VIENNAGRID_VERSION_PATCH                    0


/**********************************************************************************************
 *
 *                static functions
 *
 **********************************************************************************************/


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

static inline viennagrid_int viennagrid_boundary_element_count( viennagrid_element_type host, viennagrid_dimension boundary_topo_dim )
{
  if (host == VIENNAGRID_ELEMENT_TYPE_LINE)
  {
    if (boundary_topo_dim == 0)
      return 2;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)
  {
    if (boundary_topo_dim == 1)
      return 3;
    if (boundary_topo_dim == 0)
      return 3;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
  {
    if (boundary_topo_dim == 1)
      return 4;
    if (boundary_topo_dim == 0)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_POLYGON)
  {
    if (boundary_topo_dim == 1)
      return -1;
    if (boundary_topo_dim == 0)
      return -1;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON)
  {
    if (boundary_topo_dim == 2)
      return 4;
    if (boundary_topo_dim == 1)
      return 6;
    if (boundary_topo_dim == 0)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON)
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

static inline viennagrid_int viennagrid_boundary_element_count_from_element_type( viennagrid_element_type host, viennagrid_element_type boundary )
{
  if (host == VIENNAGRID_ELEMENT_TYPE_LINE)
  {
    if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX)
      return 2;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)
  {
    if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE)
      return 3;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX)
      return 3;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
  {
    if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE)
      return 4;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_POLYGON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE)
      return -1;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX)
      return -1;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)
      return 4;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE)
      return 6;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX)
      return 4;
    return -1;
  }
  else if (host == VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
      return 6;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE)
      return 12;
    if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX)
      return 8;
    return -1;
  }
  return -1;
}

static inline viennagrid_bool viennagrid_is_boundary_type(viennagrid_element_type host, viennagrid_element_type boundary)
{
  return viennagrid_boundary_element_count_from_element_type(host, boundary) > 0 ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}

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


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_new(viennagrid_int size, void ** ptr);
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

/* set the mesh name, if mesh_name can be NULL to unset a name */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_name_set(viennagrid_mesh mesh,
                                                                    const char * mesh_name);



/* queries the number of element of a specific element type */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_element_count(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   viennagrid_element_type element_type,
                                                                                   viennagrid_int * count);

/* creates a set of vertices in a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_vertex_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                   const viennagrid_numeric * coords,
                                                                                   viennagrid_int * vertex_id);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_vertex_create(viennagrid_mesh mesh,
                                                                         const viennagrid_numeric * coords,
                                                                         viennagrid_int * vertex_id);

/* gets the coordinates of a vertex of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_vertex_coords_pointer(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                           viennagrid_numeric ** coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_vertex_coords_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                       viennagrid_int id,
                                                                                       viennagrid_numeric ** coords);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_hierarchy_element_create(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                    viennagrid_element_type element_type,
                                                                                    viennagrid_int vertex_count,
                                                                                    viennagrid_int * vertex_ids,
                                                                                    viennagrid_int * element_id);
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
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_create_refinement(
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


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_type_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                       viennagrid_dimension element_topo_dim,
                                                                       viennagrid_int element_id,
                                                                       viennagrid_element_type * element_type);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_element_pointers(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                        viennagrid_dimension element_topo_dim,
                                                                                        viennagrid_dimension boundary_topo_dim,
                                                                                        viennagrid_int ** boundary_offsets,
                                                                                        viennagrid_int ** boundary_ids);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_dimension boundary_topo_dim,
                                                                                viennagrid_int ** boundary_element_id_begin,
                                                                                viennagrid_int ** boundary_element_id_end);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_element_pointers(viennagrid_mesh mesh,
                                                                                          viennagrid_dimension element_topo_dim,
                                                                                          viennagrid_dimension coboundary_topo_dim,
                                                                                          viennagrid_int ** coboundary_element_ids_begin,
                                                                                          viennagrid_int ** coboundary_element_ids_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                                                  viennagrid_dimension element_topo_dim,
                                                                                  viennagrid_int element_id,
                                                                                  viennagrid_dimension coboundary_topo_dim,
                                                                                  viennagrid_int ** coboundary_element_ids_begin,
                                                                                  viennagrid_int ** coboundary_element_ids_end);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements_pointers(viennagrid_mesh mesh,
                                                                                         viennagrid_dimension element_topo_dim,
                                                                                         viennagrid_dimension connector_topo_dim,
                                                                                         viennagrid_dimension neighbor_topo_dim,
                                                                                         viennagrid_int ** neighbor_element_ids_begin,
                                                                                         viennagrid_int ** neighbor_element_ids_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                                                viennagrid_dimension element_topo_dim,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_dimension connector_topo_dim,
                                                                                viennagrid_dimension neighbor_topo_dim,
                                                                                viennagrid_int ** neighbor_element_ids_begin,
                                                                                viennagrid_int ** neighbor_element_ids_end);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_is_mesh_boundary(viennagrid_mesh mesh,
                                                                               viennagrid_dimension element_topo_dim,
                                                                               viennagrid_int element_id,
                                                                               viennagrid_bool * result);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_is_region_boundary(viennagrid_region region,
                                                                                 viennagrid_mesh mesh,
                                                                                 viennagrid_dimension element_topo_dim,
                                                                                 viennagrid_int element_id,
                                                                                 viennagrid_bool * result);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_elements_get(viennagrid_mesh mesh,
                                                                   viennagrid_dimension element_topo_dim,
                                                                   viennagrid_int ** element_ids_begin,
                                                                   viennagrid_int ** element_ids_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_add(viennagrid_mesh mesh,
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

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_get_or_create(viennagrid_mesh_hierarchy mesh_hierarchy,
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

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_add_to_region(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                            viennagrid_dimension element_topo_dim,
                                                                            viennagrid_int element_id,
                                                                            viennagrid_region region);


/**********************************************************************************************
 *
 *                viennagrid PLC functions
 *
 **********************************************************************************************/


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_create(viennagrid_plc * plc);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_retain(viennagrid_plc plc);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_release(viennagrid_plc plc);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_clear(viennagrid_plc plc);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                                                        viennagrid_plc dst_plc,
                                                                        viennagrid_int copy_hole_points,
                                                                        viennagrid_int copy_seed_points);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                                                  viennagrid_dimension * geometric_dimension);

/* setting geometric dimension will invalid all points (if geometric dimension changes) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                                                  viennagrid_int geometric_dimension);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_element_count_get(viennagrid_plc plc,
                                                                            viennagrid_dimension topologic_dimension,
                                                                            viennagrid_int * element_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_create(viennagrid_plc plc,
                                                                        const viennagrid_numeric * coords,
                                                                        viennagrid_int * vertex_id);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_coords_pointer(viennagrid_plc plc,
                                                                        viennagrid_numeric ** coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_vertex_coords_get(viennagrid_plc plc,
                                                                    viennagrid_int id,
                                                                    viennagrid_numeric ** coords);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                                                      viennagrid_int vertex_id0,
                                                                      viennagrid_int vertex_id1,
                                                                      viennagrid_int * line_id);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                                                       viennagrid_int * line_ids,
                                                                       viennagrid_int line_count,
                                                                       viennagrid_int * facet_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                                            viennagrid_dimension element_topo_dim,
                                                                            viennagrid_int element_id,
                                                                            viennagrid_dimension boundary_topo_dim,
                                                                            viennagrid_int ** boundary_element_ids_begin,
                                                                            viennagrid_int ** boundary_element_ids_end);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_point_add(viennagrid_plc plc,
                                                                               viennagrid_int facet_id,
                                                                               const viennagrid_numeric * coords);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_hole_point_delete(viennagrid_plc plc,
                                                                                  viennagrid_int facet_id,
                                                                                  viennagrid_int point_id);
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

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_refine_lines(viennagrid_plc plc,
                                                                       viennagrid_plc output_plc,
                                                                       viennagrid_numeric line_size);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_from_mesh(viennagrid_mesh mesh,
                                                                    viennagrid_plc plc); // TODO implement


/**********************************************************************************************
 *
 *                viennagrid quantity field functions
 *
 **********************************************************************************************/


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_create(viennagrid_quantity_field * quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                                          viennagrid_dimension topologic_dimension,
                                                                          viennagrid_int size_of_value,
                                                                          viennagrid_int storage_layout);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                                                             viennagrid_dimension * topologic_dimension);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_storage_layout(viennagrid_quantity_field quantity_field,
                                                                                        viennagrid_int * storage_layout);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_size_of_value(viennagrid_quantity_field quantity_field,
                                                                                       viennagrid_dimension * size_of_value);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_set_name(viennagrid_quantity_field quantity_field,
                                                                              const char * name);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_name(viennagrid_quantity_field quantity_field,
                                                                              const char ** name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_set_unit(viennagrid_quantity_field quantity_field,
                                                                              const char * unit);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_unit(viennagrid_quantity_field quantity_field,
                                                                              const char ** unit);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                                                            viennagrid_int value_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                                                           viennagrid_int * value_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_set_value(viennagrid_quantity_field quantity_field,
                                                                               viennagrid_int element_id,
                                                                               void * value);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_value(viennagrid_quantity_field quantity_field,
                                                                               viennagrid_int element_id,
                                                                               void ** value);


/**********************************************************************************************
 *
 *                viennagrid IO functions
 *
 **********************************************************************************************/


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_create(viennagrid_mesh_io * mesh_io);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_retain(viennagrid_mesh_io mesh_io);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_release(viennagrid_mesh_io mesh_io);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_clear(viennagrid_mesh_io mesh_io);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_mesh_set(viennagrid_mesh_io mesh_io, viennagrid_mesh mesh);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_mesh_get(viennagrid_mesh_io mesh_io, viennagrid_mesh * mesh);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_count(viennagrid_mesh_io mesh_io,
                                                                             viennagrid_int * count);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_get_by_index(viennagrid_mesh_io mesh_io,
                                                                                          viennagrid_int index,
                                                                                          viennagrid_quantity_field * quantity_field);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_get(viennagrid_mesh_io mesh_io,
                                                                                 const char * quantity_name,
                                                                                 viennagrid_quantity_field * quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_quantity_field_set(viennagrid_mesh_io mesh_io,
                                                                                 const char * quantity_name,
                                                                                 viennagrid_quantity_field quantity_field);



VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_filetype_from_filename(const char * filename,
                                                                                     viennagrid_int * filetype);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_read_with_filetype(viennagrid_mesh_io mesh_io,
                                                                                 const char * filename,
                                                                                 viennagrid_int filetype);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_read(viennagrid_mesh_io mesh_io,
                                                                   const char * filename);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_write_with_filetype(viennagrid_mesh_io mesh_io,
                                                                                  const char * filename,
                                                                                  viennagrid_int filetype);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_io_write(viennagrid_mesh_io mesh_io,
                                                                    const char * filename);





VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_read_tetgen_poly(viennagrid_plc plc,
                                                                           const char * filename);


#endif
