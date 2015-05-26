#ifndef VIENNAGRID_BACKEND_FORWARDS_H
#define VIENNAGRID_BACKEND_FORWARDS_H



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
typedef viennagrid_int viennagrid_index;
typedef unsigned char viennagrid_element_tag;
typedef int viennagrid_dimension;


typedef struct viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;
typedef struct viennagrid_mesh_ * viennagrid_mesh;
typedef struct viennagrid_region_ * viennagrid_region;




#define VIENNAGRID_TRUE 1
#define VIENNAGRID_FALSE 0


/* VIENNAGRID ERROR CODES */
#define VIENNAGRID_SUCCESS 0
#define VIENNAGRID_UNSPECIFIED_ERROR 1
#define VIENNAGRID_OUT_OF_MEMORY 2
#define VIENNAGRID_INVALID_ARGUMENTS 3
#define VIENNAGRID_UNSUPPORTED_BOUNDARY_LAYOUT 4

/* VIENNAGRID OPTION FLAGS */
#define VIENNAGRID_BOUNDARY_LAYOUT_FLAG 0x0001

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
#define VIENNAGRID_ELEMENT_TAG_PLC 5
#define VIENNAGRID_ELEMENT_TAG_TETRAHEDRON 6
#define VIENNAGRID_ELEMENT_TAG_HEXAHEDRON 7
#define VIENNAGRID_ELEMENT_TAG_COUNT 8

#define VIENNAGRID_TOPOLOGIC_DIMENSION_END 4



inline viennagrid_bool viennagrid_is_topological_dimension_valid(viennagrid_int topological_dimension)
{
  if ((topological_dimension >= 0) && (topological_dimension < VIENNAGRID_TOPOLOGIC_DIMENSION_END))
    return VIENNAGRID_TRUE;
  return VIENNAGRID_FALSE;
}

inline viennagrid_int viennagrid_topological_dimension(viennagrid_element_tag et)
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
  case VIENNAGRID_ELEMENT_TAG_PLC:
    return 2;
  case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
  case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
    return 3;
  default:
    return -1;
  }
}

inline viennagrid_bool viennagrid_is_simplex(viennagrid_element_tag et)
{
  return ((et == VIENNAGRID_ELEMENT_TAG_VERTEX) || (et == VIENNAGRID_ELEMENT_TAG_LINE) ||
          (et == VIENNAGRID_ELEMENT_TAG_TRIANGLE) || (et == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)) ?
          VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}


inline viennagrid_element_tag viennagrid_topological_max(viennagrid_element_tag lhs, viennagrid_element_tag rhs)
{
  if (viennagrid_topological_dimension(lhs) >= viennagrid_topological_dimension(rhs))
    return lhs;
  else
    return rhs;
}


inline viennagrid_bool viennagrid_dynamic_storage(viennagrid_element_tag et)
{
  if ((et == VIENNAGRID_ELEMENT_TAG_POLYGON) || (et == VIENNAGRID_ELEMENT_TAG_PLC))
    return VIENNAGRID_TRUE;
  return VIENNAGRID_FALSE;
}


inline viennagrid_bool viennagrid_static_storage(viennagrid_element_tag et)
{
  return 1-viennagrid_dynamic_storage(et);
}





inline viennagrid_int viennagrid_boundary_element_count_from_element_tag( viennagrid_element_tag host, viennagrid_element_tag boundary )
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
  else if (host == VIENNAGRID_ELEMENT_TAG_PLC)
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



inline viennagrid_element_tag viennagrid_facet_tag(viennagrid_element_tag et)
{
  if (et == VIENNAGRID_ELEMENT_TAG_LINE)
    return VIENNAGRID_ELEMENT_TAG_VERTEX;

  else if (et == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
    return VIENNAGRID_ELEMENT_TAG_LINE;
  else if (et == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
    return VIENNAGRID_ELEMENT_TAG_LINE;
  else if (et == VIENNAGRID_ELEMENT_TAG_POLYGON)
    return VIENNAGRID_ELEMENT_TAG_LINE;
  else if (et == VIENNAGRID_ELEMENT_TAG_PLC)
    return VIENNAGRID_ELEMENT_TAG_LINE;

  else if (et == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)
    return VIENNAGRID_ELEMENT_TAG_TRIANGLE;
  else if (et == VIENNAGRID_ELEMENT_TAG_HEXAHEDRON)
    return VIENNAGRID_ELEMENT_TAG_QUADRILATERAL;

  return VIENNAGRID_ELEMENT_TAG_NO_ELEMENT;
}



inline viennagrid_bool viennagrid_is_boundary_tag(viennagrid_element_tag host, viennagrid_element_tag boundary)
{
  return viennagrid_boundary_element_count_from_element_tag(host, boundary) > 0 ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
}



inline viennagrid_int viennagrid_boundary_element_tag_count(viennagrid_element_tag host)
{
  if (host == VIENNAGRID_ELEMENT_TAG_LINE)
    return 1;
  if (host == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
    return 2;
  if (host == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
    return 2;
  if (host == VIENNAGRID_ELEMENT_TAG_POLYGON)
    return 2;
  if (host == VIENNAGRID_ELEMENT_TAG_PLC)
    return 2;
  if (host == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)
    return 3;
  if (host == VIENNAGRID_ELEMENT_TAG_HEXAHEDRON)
    return 3;

  return 0;
}


inline viennagrid_index viennagrid_boundary_buffer_index_from_element_tag(viennagrid_element_tag host, viennagrid_element_tag boundary)
{
  if (host == VIENNAGRID_ELEMENT_TAG_LINE)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_POLYGON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_PLC)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
      return 2;
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_HEXAHEDRON)
  {
    if (boundary == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
      return 2;
    if (boundary == VIENNAGRID_ELEMENT_TAG_LINE)
      return 1;
    if (boundary == VIENNAGRID_ELEMENT_TAG_VERTEX)
      return 0;
  }

  return -1;
}




inline viennagrid_element_tag viennagrid_boundary_element_tag_from_index(viennagrid_element_tag host, viennagrid_int boundary_index)
{
  if (host == VIENNAGRID_ELEMENT_TAG_LINE)
  {
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_TRIANGLE)
  {
    if (boundary_index == 1)
      return VIENNAGRID_ELEMENT_TAG_LINE;
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_QUADRILATERAL)
  {
    if (boundary_index == 1)
      return VIENNAGRID_ELEMENT_TAG_LINE;
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_POLYGON)
  {
    if (boundary_index == 1)
      return VIENNAGRID_ELEMENT_TAG_LINE;
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_PLC)
  {
    if (boundary_index == 1)
      return VIENNAGRID_ELEMENT_TAG_LINE;
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_TETRAHEDRON)
  {
    if (boundary_index == 2)
      return VIENNAGRID_ELEMENT_TAG_TRIANGLE;
    if (boundary_index == 1)
      return VIENNAGRID_ELEMENT_TAG_LINE;
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }
  if (host == VIENNAGRID_ELEMENT_TAG_HEXAHEDRON)
  {
    if (boundary_index == 2)
      return VIENNAGRID_ELEMENT_TAG_QUADRILATERAL;
    if (boundary_index == 1)
      return VIENNAGRID_ELEMENT_TAG_LINE;
    if (boundary_index == 0)
      return VIENNAGRID_ELEMENT_TAG_VERTEX;
  }

  return VIENNAGRID_ELEMENT_TAG_NO_ELEMENT;
}




inline viennagrid_int viennagrid_boundary_element_count_from_boundary_index( viennagrid_element_tag host, viennagrid_int boundary_index )
{
  return viennagrid_boundary_element_count_from_element_tag(host, viennagrid_boundary_element_tag_from_index(host, boundary_index));
}







inline const char * viennagrid_element_tag_string(viennagrid_element_tag element_tag)
{
  switch (element_tag)
  {
//     case VIENNAGRID_ELEMENT_TAG_FACET:
//       return "facet";
//     case VIENNAGRID_ELEMENT_TAG_CELL:
//       return "cell";

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
    case VIENNAGRID_ELEMENT_TAG_PLC:
      return "plc";

    case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
      return "tetrahedron";
    case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
      return "hexahedron";

    default:
      return "no-element";
  }
}



#endif
