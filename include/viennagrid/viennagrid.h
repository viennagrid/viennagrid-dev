#ifndef VIENNAGRID_H
#define VIENNAGRID_H



/**********************************************************************************************
 *
 *                ViennaGrid - A mesh data structure and mesh algorithm library
 *
 * ViennaGrid is a mesh data structure library which also offers a set of mesh algorithms. The
 * library backend is written in C++, providing a standard C interface which is expose in this
 * header.
 *
 * The ViennaGrid library can be split into the following main feature sets:
 *  - Mesh and mesh hierarchy data structures
 *  - Piecewise linear complexes (PLC)
 *  - Quantity fields
 *  - Serialization and IO
 *  - Algorithms
 *
 *
 * -------------------------------------------------
 * ---- Mesh and mesh hierarchy data structures ----
 * -------------------------------------------------
 *
 *
 * In ViennaGrid, a mesh is represented by a viennagrid_mesh object. A mesh itself contains several mesh elements,
 * where each element has a specific element type, e.g., triangle or vertex. Every vertex can be associated with a point in
 * the n-dimensional space, where n is the geometric dimension of the mesh. Each element (except a vertex) has several boundary
 * elements. For example, a triangle has 3 boundary lines (its edges) and 3 boundary vertices (its corners). A simple example mesh is given here:
 *
 *    5---------4---------3
 *    | \       |         |
 *    |   \     |         |    y
 *    |     \   |         |     ^
 *    |       \ |         |     |
 *    0---------1---------2     *--> x
 *
 * On the left three triangle with the corner vertices 0,1,5 and 1,4,5 can be seen. On the right, there is a quadrilateral with the vertex indices 1,2,3,4. Each element type has an associated topological dimension, indicating the smallest dimension of its extent. A boundary element with topological dimension one less is called a facet.
 * Currently, ViennaGrid supports the following element types:
 *
 *    element type  |  topological dimension  |  facet type    |  facet count
 *  ----------------|-------------------------|----------------|---------------
 *    vertex        |   0                     |  -             |  -
 *    line          |   1                     |  vertex        |  2
 *    triangle      |   2                     |  line          |  3
 *    quadrilateral |   2                     |  line          |  4
 *    polygon       |   2                     |  line          |  k
 *    tetrahedron   |   3                     |  triangle      |  4
 *    hexahedron    |   3                     |  quadrilateral |  6
 *
 * Topological dimension is an important aspect in ViennaGrid and it is used in various API functions. An element within a mesh is identified by using its topological dimension and its ID. Iteration over boundary elements can be achieved using the function viennagrid_element_boundary_elements. Different elements can be connected if they have common boundary elements. Using these connections, other iterations are possible, like co-boundary iteration (viennagrid_element_coboundary_elements) or neighbor iteration (viennagrid_element_neighbor_elements).
 *
 * Every viennagrid_mesh also can have a number of child meshes, where each child mesh represents a refinement of the parent mesh. This parent-child relationship is also reflected in the elements itself, where each element has a parent element id (-1 indicating, that an element has no mesh). If a mesh doesn't have any parent mesh it is called the root mesh. Elements and vertices can only be created in the root mesh and only if the root mesh does not have any child meshes yet (otherwise the child meshes would lack these new created elements).
 *
 * Additionally, each mesh element can be added to one or more regions. Regions can be used for identifying sub-meshes, for example if an object has different materials, each material can be represented by a region.
 *
 *
 * -- Usage --
 *
 * --- Mesh setup and element creation ---
 *
 * At first, a viennagrid_mesh object has to be created:
 *
 *    viennagrid_mesh mesh;
 *    viennagrid_mesh_create(&mesh);
 *
 * If the root mesh doesn't have any child meshes, vertices can be created in the root mesh. However, the geometric dimension has to be set first:
 *
 *    viennagrid_mesh_hierarchy_geometric_dimension_get(mesh_hierarchy, 3);
 *
 * If at some point the geometric dimension has already been set to another value, this function call invalidates all vertex coordinates. Now, vertices can be created one by one using the function viennagrid_mesh_vertex_create or multiple vertices at once using viennagrid_mesh_vertex_batch_create:
 *
 *    viennagrid_int vertex_id;
 *    viennagrid_numeric vertex_coords[3] = { 0.0, 0.0, 0.0 };
 *    viennagrid_mesh_vertex_create(mesh, vertex_coords0, &vertex_id0);
 *
 *
 *    viennagrid_int first_vertex_id;
 *    viennagrid_numeric vertices_coords[9] = { 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0 };
 *    viennagrid_mesh_vertex_batch_create(mesh, 3, vertices_coords, &first_vertex_id);
 *    // the new creates vertices now have the IDs: first_vertex_id, first_vertex_id+1 and first_vertex_id+2
 *
 * Using the vertex IDs, elements can now be created using viennagrid_mesh_element_create for creating one element or viennagrid_mesh_element_batch_create for creating multiple elements:
 *
 *    viennagrid_int triangle_id;
 *    viennagrid_int vertex_ids[3] = { 0, 1, 2 };
 *    viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, vertex_ids, &triangle_id);
 *
 *    viennagrid_int first_triangle_id;
 *    viennagrid_element_type element_types[3] = { VIENNAGRID_ELEMENT_TYPE_TRIANGLE, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, VIENNAGRID_ELEMENT_TYPE_TRIANGLE };
 *    viennagrid_int element_vertex_index_offsets[3] = { 0, 3, 6, 9 };
 *    viennagrid_int element_vertex_indices[9] = { 0, 1, 2, 1, 3, 2, 1, 4, 3 };
 *    viennagrid_mesh_element_batch_create(mesh, 3, element_types, element_vertex_index_offsets, element_vertex_indices, NULL, &first_triangle_id);
 *    // the 6th argument is a pointer to region_ids, one region_id for each element to create. we don't want to add are new element to regions, therefore this argument is NULL
 *    // the new created elements now have the IDs: first_triangle_id, first_triangle_id+1 and first_triangle_id+2
 *
 * New regions can be created using the functions viennagrid_mesh_region_create or viennagrid_mesh_hierarchy_region_get_or_create. While the first just creates a new region with an unused ID, the ladder function takes an additional region ID argument and returns the region with that ID if it is present or creates a new region with that particular ID:
 *
 *    viennagrid_region region0;
 *    viennagrid_mesh_region_create(mesh, &region0);
 *
 *    viennagrid_region region1;
 *    viennagrid_mesh_region_get_or_create(mesh, 42, &region1);
 *
 * Elements can be added to regions using viennagrid_region_element_add:
 *
 *    viennagrid_region_element_add(region0, 2, triangle_id);
 *    // adding element with topological dimension 2 and ID 3 to region0
 *
 *
 * --- Iteration ---
 *
 * In ViennaGrid, the iteration interface uses pointer ranges. Every function returns a begin pointer and an end pointer, in a way that the iteration loop can be done like
 *
 *    for (type * it = begin; it != end; ++it)
 *      // do something with *it
 *
 *
 * To iterate over all elements of a mesh having a specific topological dimension, the function viennagrid_mesh_elements_get can be used:
 *
 *    viennagrid_int * vertex_ids_begin;
 *    viennagrid_int * vertex_ids_end;
 *    viennagrid_mesh_elements_get(mesh, 0, &vertex_ids_begin, &vertex_ids_end);
 *
 *    for (viennagrid_int * vit = vertex_ids_begin; vit != vertex_ids_end; ++vit)
 *    {
 *      viennagrid_int vertex_id = *vit;
 *      // do something
 *    }
 *
 * It is also possible to iterate over all boundary elements of an element having a specific topological dimension. The function viennagrid_element_boundary_elements is used for querying the boundary range:
 *
 *    viennagrid_int * boundary_vertex_ids_begin;
 *    viennagrid_int * boundary_vertex_ids_end;
 *    viennagrid_element_boundary_elements(mesh_hierarchy, 2, triangle_id, 0, boundary_vertex_ids_begin, boundary_vertex_ids_end);
 *    // the second and third parameter is the topological dimension and the ID of the reference element, respectively
 *    // the fifth parameter is the boundary topological dimension
 *
 *    for (viennagrid_int * vit = boundary_vertex_ids_begin; vit != boundary_vertex_ids_end; ++vit)
 *    {
 *      viennagrid_int vertex_id = *vit;
 *      // do something
 *    }
 *
 * Co-boundary and neighbor iteration work similarly, using the functions viennagrid_element_coboundary_elements and viennagrid_element_neighbor_elements:
 *
 *    viennagrid_int coboundary_line_ids_begin;
 *    viennagrid_int coboundary_line_ids_end;
 *    viennagrid_element_coboundary_elements(mesh, 0, vertex_id, 1, &coboundary_line_ids_begin, &coboundary_line_ids_end);
 *    // the second and third parameter is the topological dimension and the ID of the reference element, respectively
 *    // the fifth parameter is the co-boundary topological dimension
 *
 *    for (viennagrid_int * lit = coboundary_line_ids_begin; lit != coboundary_line_ids_end; +lit)
 *    {
 *      viennagrid_int line_id = *lit;
 *      // do something
 *    }
 *
 *
 *    viennagrid_int neighbor_triangle_ids_begin;
 *    viennagrid_int neighbor_triangle_ids_end;
 *    viennagrid_element_neighbor_elements(mesh, 2, triangle_id, 1, 2, &neighbor_triangle_ids_begin, &neighbor_triangle_ids_end);
 *    // the second and third parameter is the topological dimension and the ID of the reference element, respectively
 *    // the fifth parameter is the topological dimension of the elements which are used as connection between the reference element and its neighbors
 *    // the sixth parameter is the neighbor topological dimension
 *
 *    for (viennagrid_int * tit = neighbor_triangle_ids_begin; tit != neighbor_triangle_ids_end; +tit)
 *    {
 *      viennagrid_int triangle_id = *tit;
 *      // do something
 *    }
 *
 *
 * --- Query information ---
 *
 * To
 *
 *
 *
 * ViennaGrid is based on the following main concepts/aspects:
 *  - Hierarchical mesh architecture
 *  - Topological connectivity
 *  - Regions
 *
 **********************************************************************************************/






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


typedef char            viennagrid_bool;
typedef char            viennagrid_byte;
typedef int             viennagrid_int;
typedef viennagrid_int  viennagrid_error;
typedef viennagrid_int  viennagrid_element_id;
typedef double          viennagrid_numeric;
typedef char            viennagrid_element_type;
typedef char            viennagrid_dimension;
typedef short           viennagrid_region_id;


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
#define VIENNAGRID_ERROR_INVALID_MESH                                   4
#define VIENNAGRID_ERROR_INVALID_REGION                                 5
#define VIENNAGRID_ERROR_INVALID_CHILD_MESH_INDEX                       6
#define VIENNAGRID_ERROR_INVALID_TOPOLOGIC_DIMENSION                    7
#define VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE                           8
#define VIENNAGRID_ERROR_INVALID_ELEMENT_ID                             9
#define VIENNAGRID_ERROR_INVALID_REGION_ID                             10
#define VIENNAGRID_ERROR_INVALID_PLC                                   11
#define VIENNAGRID_ERROR_INVALID_FACET_HOLE_POINT_INDEX                12
#define VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD                        13
#define VIENNAGRID_ERROR_INVALID_STORAGE_LAYOUT                        14
#define VIENNAGRID_ERROR_INVALID_QUANTITLY_VALUE_TYPE                  15
#define VIENNAGRID_ERROR_INVALID_MESH_IO                               16
#define VIENNAGRID_ERROR_INVALID_IO_MESH_INDEX                         17
#define VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD_NAME                   18
#define VIENNAGRID_ERROR_TOPOLOGICAL_DIMENSION_MISMATCH                19
#define VIENNAGRID_ERROR_UNKNOWN_PROPERTY                              20
#define VIENNAGRID_ERROR_UNSUPPORTED_BOUNDARY_LAYOUT                   21
#define VIENNAGRID_ERROR_MESH_IS_NOT_ROOT                              22
#define VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES                         23
#define VIENNAGRID_ERROR_ELEMENT_ALREADY_PRESENT                       24
#define VIENNAGRID_ERROR_DESERIALIZE_MAGIC_VALUE_MISSMATCH             25
#define VIENNAGRID_ERROR_DESERIALIZE_VERSION_MISMATCH                  26
#define VIENNAGRID_ERROR_PLC_MESH_TYPE_NOT_SUPPORTED_FOR_CONVERSION    27
#define VIENNAGRID_ERROR_CANNOT_OPEN_FILE                              28
#define VIENNAGRID_ERROR_FILE_EMPTY                                    29
#define VIENNAGRID_ERROR_EOF_ENCOUNTERED                               30
#define VIENNAGRID_ERROR_EOF_WHILE_READING_VERTICES                    31
#define VIENNAGRID_ERROR_VERTEX_DIMENSION_MISMATCH                     32
#define VIENNAGRID_ERROR_EOF_WHILE_READING_CELL_COUNT                  33
#define VIENNAGRID_ERROR_EOF_WHILE_READING_CELLS                       34
#define VIENNAGRID_ERROR_UNKNOWN_FILETYPE                              35
#define VIENNAGRID_ERROR_INVALID_VERTEX_COUNT                          36
#define VIENNAGRID_ERROR_INVALID_ATTRIBUTE_COUNT                       37
#define VIENNAGRID_ERROR_INVALID_BOUNDARY_MARKER_COUNT                 38
#define VIENNAGRID_ERROR_INVALID_FACET_COUNT                           39
#define VIENNAGRID_ERROR_INVALID_FACET_POLYGON_COUNT                   40
#define VIENNAGRID_ERROR_INVALID_FACET_HOLE_POINT_COUNT                41
#define VIENNAGRID_ERROR_INVALID_POLYGON_VERTEX_COUNT                  42
#define VIENNAGRID_ERROR_INVALID_VERTEX_ID                             43
#define VIENNAGRID_ERROR_INVALID_HOLE_POINT_COUNT                      44
#define VIENNAGRID_ERROR_INVALID_SEED_POINT_COUNT                      45
#define VIENNAGRID_ERROR_UNSUPPORTED_ELEMENT_TYPE                      46
#define VIENNAGRID_ERROR_FILE_MALFORMED                                47
#define VIENNAGRID_ERROR_NO_MESH                                       48
#define VIENNAGRID_ERROR_WRITE_ERROR                                   49

/* VIENNAGRID BOOL DEFINES */
#define VIENNAGRID_TRUE                             1
#define VIENNAGRID_FALSE                            0

/* VIENNAGRID PROPERTY FLAGS */
#define VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT         0

/* VIENNAGRID OPTIONS */
#define VIENNAGRID_BOUNDARY_LAYOUT_FULL             0
#define VIENNAGRID_BOUNDARY_LAYOUT_SPARSE           1

/* VIENNAGRID ELEMENT TYPES */
#define VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT         -1
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
#define VIENNAGRID_FILETYPE_VTK_MESH                2


/* QUANTITY FIELD TYPES */
#define VIENNAGRID_QUANTITY_FIELD_TYPE_BYTE         0
#define VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC      1

/* QUANTITY FIELD STORAGE LAYOUT */
#define VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE     0
#define VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE    1


/* VIENNAGRID MAGIC VALUE FOR SERIALIZATION */
#define VIENNAGRID_MAGIC_VALUE                      0x42F42F42

/* TODO move to cmake?? */
#define VIENNAGRID_VERSION_MAJOR                    _VERSION_MAJOR_
#define VIENNAGRID_VERSION_MINOR                    _VERSION_MINOR_
#define VIENNAGRID_VERSION_PATCH                    _VERSION_PATCH_


#define VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION     -1
#define VIENNAGRID_INVALID_REGION_ID               -1


/**********************************************************************************************
 *
 *                static functions
 *
 **********************************************************************************************/


#define VIENNAGRID_ELEMENT_ID_MASK_OFFSET                  28
#define VIENNAGRID_ELEMENT_ID_TOPOLOGICAL_DIMENSION_MASK   ((viennagrid_element_id)3 << VIENNAGRID_ELEMENT_ID_MASK_OFFSET)
#define VIENNAGRID_ELEMENT_ID_INDEX_MASK                   (~((viennagrid_element_id)3 << VIENNAGRID_ELEMENT_ID_MASK_OFFSET))

static inline viennagrid_dimension viennagrid_topological_dimension_from_element_id(viennagrid_element_id element_id)
{
  return (element_id & VIENNAGRID_ELEMENT_ID_TOPOLOGICAL_DIMENSION_MASK) >> VIENNAGRID_ELEMENT_ID_MASK_OFFSET;
}

static inline viennagrid_element_id viennagrid_index_from_element_id(viennagrid_element_id element_id)
{
  return element_id & VIENNAGRID_ELEMENT_ID_INDEX_MASK;
}

static inline viennagrid_element_id viennagrid_compose_element_id(viennagrid_dimension topological_dimension,
                                                                  viennagrid_element_id element_index)
{
  return ((topological_dimension << VIENNAGRID_ELEMENT_ID_MASK_OFFSET) & VIENNAGRID_ELEMENT_ID_TOPOLOGICAL_DIMENSION_MASK) + element_index;
}








/* checks, if an element type is valid */
static inline viennagrid_bool viennagrid_element_type_valid(viennagrid_element_type et)
{ return (et >= VIENNAGRID_ELEMENT_TYPE_START) && (et < VIENNAGRID_ELEMENT_TYPE_COUNT); }

/* checks, if a topological dimension is valid */
static inline viennagrid_bool viennagrid_topological_dimension_valid(viennagrid_dimension topological_dimension)
{ return (topological_dimension >= 0) && (topological_dimension < VIENNAGRID_TOPOLOGIC_DIMENSION_END); }

static inline viennagrid_bool viennagrid_element_id_valid(viennagrid_element_id element_id, viennagrid_element_id max_index)
{
  viennagrid_element_id index = viennagrid_index_from_element_id(element_id);
  return ( viennagrid_topological_dimension_valid(viennagrid_topological_dimension_from_element_id(element_id)) &&
           (index >= 0) && (index < max_index) ) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}






/* returns the topological dimension of a given element type. if element type is invalid, -1 is returned */
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

/* returns the boundary element count given a host element type and a boundary topological dimension, -1 is returned if the size is not static (e.g. polygon). if host element type or boundary topological dimension is invalid, 0 is returned  */
static inline viennagrid_int viennagrid_boundary_element_count( viennagrid_element_type host, viennagrid_dimension boundary_topological_dimension)
{
  switch (host)
  {
    case VIENNAGRID_ELEMENT_TYPE_LINE:
      if (boundary_topological_dimension== 0) return 2;
      return 0;

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      if (boundary_topological_dimension== 1) return 3;
      if (boundary_topological_dimension== 0) return 3;
      return 0;

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      if (boundary_topological_dimension== 1) return 4;
      if (boundary_topological_dimension== 0) return 4;
      return 0;

    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      if (boundary_topological_dimension== 1) return -1;
      if (boundary_topological_dimension== 0) return -1;
      return 0;

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      if (boundary_topological_dimension== 2) return 4;
      if (boundary_topological_dimension== 1) return 6;
      if (boundary_topological_dimension== 0) return 4;
      return 0;

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      if (boundary_topological_dimension== 2) return 6;
      if (boundary_topological_dimension== 1) return 12;
      if (boundary_topological_dimension== 0) return 8;
      return 0;
  }

  return 0;
}

/* returns the boundary element count given a host element type and a boundary element type, -1 is returned if the size is not static (e.g. polygon), if host or boundary element type is invalid, 0 is returned */
static inline viennagrid_int viennagrid_boundary_element_count_from_element_type( viennagrid_element_type host, viennagrid_element_type boundary )
{
  switch (host)
  {
    case VIENNAGRID_ELEMENT_TYPE_LINE:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 2;
      break;

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 3;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 3;
      break;

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 4;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 4;
      break;

    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return -1;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return -1;
      break;

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_TRIANGLE) return 4;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 6;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 4;
      break;

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      if (boundary == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL) return 6;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_LINE) return 12;
      if (boundary == VIENNAGRID_ELEMENT_TYPE_VERTEX) return 8;
      break;
  }

  return 0;
}

/* queries if a given element type is boundary of a given host type, undefined return value if host or boundary is an invalid element type */
static inline viennagrid_bool viennagrid_is_boundary_type(viennagrid_element_type host, viennagrid_element_type boundary)
{
  return viennagrid_boundary_element_count_from_element_type(host, boundary) != 0 ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}

/* return the element type with the larger topological dimension, undefined return value if lhs or rhs is an invalid element type */
static inline viennagrid_element_type viennagrid_topological_max(viennagrid_element_type lhs, viennagrid_element_type rhs)
{
  return (viennagrid_topological_dimension(lhs) < viennagrid_topological_dimension(rhs)) ? rhs : lhs;
}

/* queries if an element type is a simplex */
static inline viennagrid_bool viennagrid_is_simplex(viennagrid_element_type et)
{
  return (et == VIENNAGRID_ELEMENT_TYPE_VERTEX) || (et == VIENNAGRID_ELEMENT_TYPE_LINE) ||
         (et == VIENNAGRID_ELEMENT_TYPE_TRIANGLE) || (et == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON);
}


/* returns the string of a given element type, if element_type is invalid, NULL is returned */
const char * viennagrid_element_type_string(viennagrid_element_type element_type);









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
 *                viennagrid mesh functions
 *
 **********************************************************************************************/


/* creates a mesh with reference count of 1 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_create(viennagrid_mesh * mesh);

/* increases the reference counter of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_retain(viennagrid_mesh mesh);

/* decreases the reference counter of a mesh, if the reference counter is less or equal to zero, the mesh and all its children are deleted */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_release(viennagrid_mesh mesh);


/* optimizes memory usage */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_memory_optimize(viennagrid_mesh mesh);
/* estimates the memory usage of a mesh hierarchy in bytes */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_memory_size(viennagrid_mesh mesh,
                                                                       long * size);


/* serializes a mesh to a binary buffer, the buffer has to be deleted using viennagrid_delete */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_serialize(viennagrid_mesh mesh,
                                                                     void ** blob,
                                                                     viennagrid_int * size);
/* deserializes a binary buffer to a mesh hierarchy, the mesh hierarchy is cleared before deserialization */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_deserialize(void * blob,
                                                                       viennagrid_int size,
                                                                       viennagrid_mesh mesh);

/* set/get mesh options */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_property_set(viennagrid_mesh mesh,
                                                                        viennagrid_int property,
                                                                        viennagrid_int value);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_property_get(viennagrid_mesh mesh,
                                                                        viennagrid_int property,
                                                                        viennagrid_int * value);



/* queries the topological dimension of cell elements of a mesh_hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_cell_dimension_get(viennagrid_mesh mesh,
                                                                              viennagrid_dimension * topological_dimension);

/* queries the geometric dimension of a mesh*/
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_geometric_dimension_get(viennagrid_mesh mesh,
                                                                                   viennagrid_dimension * geometric_dimension);

/* setting geometric dimension of the mesh will invalid all points (if geometric dimension changes) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_geometric_dimension_set(viennagrid_mesh mesh,
                                                                                   viennagrid_dimension geometric_dimension);

/* clears a mesh hierarchy (removes all elements, meshes, regions, ... but does not touch reference counter) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_clear(viennagrid_mesh mesh);


/* get the root mesh of the mesh hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_root_mesh_get(viennagrid_mesh mesh,
                                                                         viennagrid_mesh * root);

/* queries if a mesh is the root mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_is_root(viennagrid_mesh mesh,
                                                                   viennagrid_bool * value);


/* queries the number of children */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh mesh,
                                                                          viennagrid_int * children_count);
/* gets a child mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_child_get(viennagrid_mesh mesh,
                                                                     viennagrid_int index,
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
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_count(viennagrid_mesh mesh,
                                                                         viennagrid_dimension topological_dimension,
                                                                         viennagrid_int * count);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_count_by_type(viennagrid_mesh mesh,
                                                                                 viennagrid_element_type element_type,
                                                                                 viennagrid_int * count);


/* creates a vertex in a mesh, the ID of the created vertex will be returned in vertex_id (optional, will be ignored if vertex_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_vertex_create(viennagrid_mesh mesh,
                                                                         const viennagrid_numeric * coords,
                                                                         viennagrid_element_id * vertex_id);

/* creates multiple vertices, returns the ID of the first vertex, the ID of the second vertex will be first_id+1 and so on. works only on root mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_vertex_batch_create(viennagrid_mesh mesh,
                                                                               viennagrid_int vertex_count,
                                                                               viennagrid_numeric * vertex_coords,
                                                                               viennagrid_element_id * first_id);

/* gets the coordinate pointer of all vertices of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_vertex_coords_pointer(viennagrid_mesh mesh,
                                                                                 viennagrid_numeric ** coords);
/* gets the coordinates of a specific vertex */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_vertex_coords_get(viennagrid_mesh mesh,
                                                                             viennagrid_element_id id,
                                                                             viennagrid_numeric ** coords);

/* creates an element of specific type in a mesh. the element is defined by its vertices, the ID of the created element will be returned in element_id (optional, will be ignored if element_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_create(viennagrid_mesh mesh,
                                                                          viennagrid_element_type element_type,
                                                                          viennagrid_int vertex_count,
                                                                          viennagrid_element_id * vertex_ids,
                                                                          viennagrid_element_id * element_id);

/* creates multiple elements, returns the ID of the first element, the ID of the second element will be first_id+1 and so on. element_types is an array which indicates the element types, one entry for each element to create. element_vertex_index_offsets and element_vertex_indices are arrays for the vertex id for each element to create. The size of element_vertex_index_offsets should be element_count+1, the vertex indices for the i-th element are stored in the array ranging from element_vertex_indices+element_vertex_index_offsets[i] to element_vertex_indices+element_vertex_index_offsets[i+1]. region_ids is an optional array indicating the region in which each element is. */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_element_batch_create(viennagrid_mesh mesh,
                                                                                viennagrid_int element_count,
                                                                                viennagrid_element_type * element_types,
                                                                                viennagrid_element_id * element_vertex_index_offsets,
                                                                                viennagrid_int * element_vertex_indices,
                                                                                viennagrid_region_id * region_ids,
                                                                                viennagrid_element_id * first_id);



/**********************************************************************************************
 *
 *                viennagrid element functions
 *
 **********************************************************************************************/


/* gets the element type pointer of a specific topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_type_pointer(viennagrid_mesh mesh,
                                                                           viennagrid_dimension topological_dimension,
                                                                           viennagrid_element_type ** element_type_pointer);

/* queries the type of an element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_type_get(viennagrid_mesh mesh,
                                                                       viennagrid_element_id element_id,
                                                                       viennagrid_element_type * element_type);

/* gets the boundary element pointers of a specific topological dimension for a given boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_element_pointers(viennagrid_mesh mesh,
                                                                                        viennagrid_dimension topological_dimension,
                                                                                        viennagrid_dimension boundary_topological_dimension,
                                                                                        viennagrid_element_id ** boundary_offsets,
                                                                                        viennagrid_element_id ** boundary_ids);

/* gets the boundary elements of a specific element for a given boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh mesh,
                                                                                viennagrid_element_id element_id,
                                                                                viennagrid_dimension boundary_topological_dimension,
                                                                                viennagrid_element_id ** boundary_element_id_begin,
                                                                                viennagrid_element_id ** boundary_element_id_end);

/* gets the coboundary element pointers of a specific topological dimension for a given coboundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_element_pointers(viennagrid_mesh mesh,
                                                                                          viennagrid_dimension topological_dimension,
                                                                                          viennagrid_dimension coboundary_topological_dimension,
                                                                                          viennagrid_element_id ** coboundary_element_ids_begin,
                                                                                          viennagrid_element_id ** coboundary_element_ids_end);

/* gets the coboundary elements of a specific element for a given coboundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                                                  viennagrid_element_id element_id,
                                                                                  viennagrid_dimension coboundary_topological_dimension,
                                                                                  viennagrid_element_id ** coboundary_element_ids_begin,
                                                                                  viennagrid_element_id ** coboundary_element_ids_end);


/* gets the neighbor element pointers of a specific topological dimension for a given neighbor and connector topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements_pointers(viennagrid_mesh mesh,
                                                                                         viennagrid_dimension topological_dimension,
                                                                                         viennagrid_dimension connector_topological_dimension,
                                                                                         viennagrid_dimension neighbor_topological_dimension,
                                                                                         viennagrid_element_id ** neighbor_element_ids_begin,
                                                                                         viennagrid_element_id ** neighbor_element_ids_end);

/* gets the neighbor elements of a specific element for a given neighbor and connector topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                                                viennagrid_element_id element_id,
                                                                                viennagrid_dimension connector_topological_dimension,
                                                                                viennagrid_dimension neighbor_topological_dimension,
                                                                                viennagrid_element_id ** neighbor_element_ids_begin,
                                                                                viennagrid_element_id ** neighbor_element_ids_end);


/* queries if a given element is boundary of a mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_is_mesh_boundary(viennagrid_mesh mesh,
                                                                               viennagrid_element_id element_id,
                                                                               viennagrid_bool * result);

/* queries if a given element is boundary of a mesh region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_is_region_boundary(viennagrid_region region,
                                                                                 viennagrid_mesh mesh,
                                                                                 viennagrid_element_id element_id,
                                                                                 viennagrid_bool * result);


/* gets all elements of a mesh for a given topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_elements_get(viennagrid_mesh mesh,
                                                                        viennagrid_dimension topological_dimension,
                                                                        viennagrid_element_id ** element_ids_begin,
                                                                        viennagrid_element_id ** element_ids_end);

/* gets the parent pointer of a specific topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_pointer(viennagrid_mesh mesh,
                                                                             viennagrid_dimension topological_dimension,
                                                                             viennagrid_element_id ** element_parent_id_pointer);

/* queries the parent element of a given element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_parent_get(viennagrid_mesh mesh,
                                                                         viennagrid_element_id element_id,
                                                                         viennagrid_element_id * element_parent_id);


/**********************************************************************************************
 *
 *                viennagrid region functions
 *
 **********************************************************************************************/



/* creates a new region using the next ID which is available */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_region_create(viennagrid_mesh mesh,
                                                                                   viennagrid_region * region);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_region_create(viennagrid_mesh mesh,
                                                                         viennagrid_region * region);


/* queries the region with a given ID, if the region is not present, a new region with that ID is created */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_region_get_or_create(viennagrid_mesh mesh,
                                                                                          viennagrid_region_id region_id,
                                                                                          viennagrid_region * region);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_region_get_or_create(viennagrid_mesh mesh,
                                                                                viennagrid_region_id region_id,
                                                                                viennagrid_region * region);

/* queries the region with a given ID, if the region is not present, NULL is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_region_get(viennagrid_mesh mesh,
                                                                                viennagrid_region_id region_id,
                                                                                viennagrid_region * region);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_region_get(viennagrid_mesh mesh,
                                                                      viennagrid_region_id region_id,
                                                                      viennagrid_region * region);





/* queries region name, if region name is not set, a NULL pointer is returned */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_name_get(viennagrid_region region,
                                                                      const char ** region_name);
/* set the region name, if region name can be NULL to unset a name */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_name_set(viennagrid_region region,
                                                                      const char * region_name);
/* queries the ID of a given region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_id_get(viennagrid_region region,
                                                                    viennagrid_region_id * id);
/* get the region pointers (all regions) of a mesh hierarchy */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_regions_get(viennagrid_mesh mesh,
                                                                       viennagrid_region_id ** region_ids_begin,
                                                                       viennagrid_region_id ** region_ids_end);

/* get the region pointers (all regions) of a given element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_regions_get(viennagrid_mesh mesh,
                                                                          viennagrid_element_id element_id,
                                                                          viennagrid_region_id ** region_ids_begin,
                                                                          viennagrid_region_id ** region_ids_end);
/* adds an element to a region */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_element_add(viennagrid_region region,
                                                                         viennagrid_element_id element_id);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_region_contains_element(viennagrid_region region,
                                                                              viennagrid_element_id element_id,
                                                                              viennagrid_bool * value);


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

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_copy(viennagrid_plc src_plc,
                                                               viennagrid_plc dst_plc);

/* clears a PLC (removes all elements, meshes, regions, ... but does not touch reference counter) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_clear(viennagrid_plc plc);

/* initialize a PLC from another PLC (copy geometric dimension, hole points and seed points) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                                                        viennagrid_plc dst_plc,
                                                                        viennagrid_bool copy_hole_points,
                                                                        viennagrid_bool copy_seed_points);

/* queries the geometric dimension of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                                                  viennagrid_dimension * geometric_dimension);

/* setting geometric dimension will invalid all points (if geometric dimension changes) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                                                  viennagrid_int geometric_dimension);

/* queries the element count for a given topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_element_count_get(viennagrid_plc plc,
                                                                            viennagrid_dimension topological_dimension,
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
                                                                            viennagrid_element_id vertex_id,
                                                                            viennagrid_numeric ** coords);

/* creates a line in a PLC using two vertex IDs, the created line ID is returned (optional, ignored if line_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                                                      viennagrid_int vertex_id0,
                                                                      viennagrid_int vertex_id1,
                                                                      viennagrid_int * line_id);
/* creates a line in a PLC using a buffer of line IDs, the created facet ID is returned (optional, ignored if facet_id is NULL) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                                                       viennagrid_int line_count,
                                                                       viennagrid_int * line_ids,
                                                                       viennagrid_int * facet_id);


/* gets the boundary element pointers of a specific topological dimension for a given boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_boundary_element_pointer(viennagrid_plc plc,
                                                                                   viennagrid_dimension topological_dimension,
                                                                                   viennagrid_dimension boundary_topological_dimension,
                                                                                   viennagrid_int ** boundary_element_offsets,
                                                                                   viennagrid_int ** boundary_element_ptr);

/* get the boundary elements of a specific element given a boundary topological dimension */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                                            viennagrid_dimension topological_dimension,
                                                                            viennagrid_int element_id,
                                                                            viennagrid_dimension boundary_topological_dimension,
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
                                                                                viennagrid_int * hole_point_count,
                                                                                viennagrid_numeric ** coords);

/* adds a volumetric hole point to a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_volumetric_hole_point_add(viennagrid_plc plc,
                                                                                    const viennagrid_numeric * coords);
/* deletes a volumetric hole point from a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_volumetric_hole_point_delete(viennagrid_plc plc,
                                                                                       viennagrid_int point_id);
/* gets the coordinates and the count of a all volumetric hole points of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_volumetric_hole_points_get(viennagrid_plc plc,
                                                                                     viennagrid_int * count,
                                                                                     viennagrid_numeric ** coords);

/* adds a seed hole point to a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_point_add(viennagrid_plc plc,
                                                                         const viennagrid_numeric * coords,
                                                                         viennagrid_int region_id);
/* deletes a seed point from a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_point_delete(viennagrid_plc plc,
                                                                            viennagrid_int point_id);
/* gets the coordinates, the region_ids and the count of a all seed hole points of a PLC */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_plc_seed_points_get(viennagrid_plc plc,
                                                                          viennagrid_int * count,
                                                                          viennagrid_numeric ** coords,
                                                                          viennagrid_int ** region_ids);



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
                                                                          viennagrid_dimension topological_dimension,
                                                                          viennagrid_int values_type,
                                                                          viennagrid_int values_per_quantity,
                                                                          viennagrid_int storage_layout);

/* queries the toplogical dimension of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_topological_dimension_get(viennagrid_quantity_field quantity_field,
                                                                                             viennagrid_dimension * topological_dimension);
/* queries the storage layout of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_storage_layout_get(viennagrid_quantity_field quantity_field,
                                                                                        viennagrid_int * storage_layout);
/* queries the storage layout of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_values_type_get(viennagrid_quantity_field quantity_field,
                                                                                     viennagrid_int * values_type);
/* queries the size of a value of a quantity field */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_values_per_quantity_get(viennagrid_quantity_field quantity_field,
                                                                                             viennagrid_int * values_per_quantity);


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
                                                                            viennagrid_int quantity_count);

/* returns the values stored */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                                                           viennagrid_int * quantity_count);

/* sets the value for a specific element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_value_set(viennagrid_quantity_field quantity_field,
                                                                               viennagrid_element_id element_id,
                                                                               void * value);
/* gets the pointer to the value for a specific element */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_value_get(viennagrid_quantity_field quantity_field,
                                                                               viennagrid_element_id element_id,
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

/**********************************************************************************************
 *
 *                ViennaGrid miscellaneous routines
 *
 **********************************************************************************************/

/* computes the cross-product of two vectors: result = v1 x v2  (3d only) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_cross_prod(viennagrid_numeric const * v1,
                                                                 viennagrid_numeric const * v2,
                                                                 viennagrid_numeric       * result);

/* computes the inner product of two vectors: result = (v1, v2) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_inner_prod(viennagrid_int dimension,
                                                                 viennagrid_numeric const * v1,
                                                                 viennagrid_numeric const * v2,
                                                                 viennagrid_numeric       * result);

/* computes the l2-norm of a vector: result = ||v1||_2 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_norm(viennagrid_int dimension,
                                                           viennagrid_numeric const * v1,
                                                           viennagrid_numeric       * result);

/* computes the volume (length) of the line spanned by two points */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_spanned_volume_2(viennagrid_int dimension,
                                                                       viennagrid_numeric const * p1,
                                                                       viennagrid_numeric const * p2,
                                                                       viennagrid_numeric       * volume);

/* computes the volume (area) of a triangle defined by three points (2d or 3d) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_spanned_volume_3(viennagrid_int dimension,
                                                                       viennagrid_numeric const * p1,
                                                                       viennagrid_numeric const * p2,
                                                                       viennagrid_numeric const * p3,
                                                                       viennagrid_numeric       * volume);

/* computes the volume of a tetrahedron defined by four points (3d) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_spanned_volume_4(viennagrid_int dimension,
                                                                       viennagrid_numeric const * p1,
                                                                       viennagrid_numeric const * p2,
                                                                       viennagrid_numeric const * p3,
                                                                       viennagrid_numeric const * p4,
                                                                       viennagrid_numeric       * volume);

/* computes the volume (area/length) of a particular element of the mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_volume(viennagrid_mesh mesh,
                                                                     viennagrid_element_id element_id,
                                                                     viennagrid_numeric * volume);

/* computes the surface of a particular element of the mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_surface(viennagrid_mesh mesh,
                                                                      viennagrid_element_id element_id,
                                                                      viennagrid_numeric * surface);

/* computes the volume (area/length) of the mesh (given by the sum of all cells inside the mesh) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_volume(viennagrid_mesh mesh,
                                                                  viennagrid_numeric * volume);

/* computes the surface of the mesh (given by the sum of all facets with only one connected cell) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_surface(viennagrid_mesh mesh,
                                                                   viennagrid_numeric * surface);

/* computes the centroid of a particular element of the mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_centroid(viennagrid_mesh mesh,
                                                                       viennagrid_element_id element_id,
                                                                       viennagrid_numeric * coords);

#endif
