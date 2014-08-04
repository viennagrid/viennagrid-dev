#ifndef VIENNAGRID_LLMESH_H
#define VIENNAGRID_LLMESH_H



#if defined(_MSC_VER)
  #if defined(viennagrid_EXPORTS)
    #define  VIENNAGRID_EXPORTED_FUNCTION __declspec(dllexport)
  #else
    #define  VIENNAGRID_EXPORTED_FUNCTION __declspec(dllimport)
  #endif /* viennagrid_EXPORTS */
#else /* defined (_MSC_VER) */
 #define VIENNAGRID_EXPORTED_FUNCTION
#endif




typedef int viennagrid_int_t;
typedef viennagrid_int_t viennagrid_error_code_t;
typedef double viennagrid_numeric_t;



// VIENNAGRID ERROR CODES
#define VIENNAGRID_SUCCESS 0
#define VIENNAGRID_UNSPECIFIED_ERROR 1
#define VIENNAGRID_OUT_OF_MEMORY 2
#define VIENNAGRID_INVALID_ARGUMENTS 3




// VIENNAGRID ELEMENT TYPES (values taken from VTK/VTU)
#define VIENNAGRID_ELEMENT_TYPE_NOT_AN_ELEMENT 0
#define VIENNAGRID_ELEMENT_TYPE_VERTEX 1
#define VIENNAGRID_ELEMENT_TYPE_LINE 3
#define VIENNAGRID_ELEMENT_TYPE_TRIANGLE 5
#define VIENNAGRID_ELEMENT_TYPE_POLYGON 7
#define VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL 9
#define VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON 10
#define VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON 12
#define VIENNAGRID_ELEMENT_TYPE_COUNT 13



//////////////////////////////////
// mesh functionalities
//////////////////////////////////


typedef struct viennagrid_llmesh_ * viennagrid_llmesh; // ViennaGrid Low Level Mesh

#ifdef  __cplusplus
extern "C"
{
#endif

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_make_llmesh(viennagrid_int_t dimension, viennagrid_llmesh * mesh);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_delete_llmesh(viennagrid_llmesh * mesh);

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_dimension(viennagrid_llmesh mesh, viennagrid_int_t * dimension);

  // -1 is a reserved segment which indicates that the element is not in any segment
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_segment_count(viennagrid_llmesh mesh, viennagrid_int_t * segment_count);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_set_segment_count(viennagrid_llmesh mesh, viennagrid_int_t segment_count);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_segment_name(viennagrid_llmesh mesh, viennagrid_int_t segment_index, const char ** segment_name); // non-named segments: return NULL
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_set_segment_name(viennagrid_llmesh mesh, viennagrid_int_t segment_index, const char * segment_name); // non-named segments: segment_name = NULL


  //////////////////////////////////
  // vertex functions
  //////////////////////////////////

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_create_vertex_buffer(viennagrid_llmesh mesh, viennagrid_int_t vertex_count); // returns created buffer. Values are not initialized

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_vertex_count(viennagrid_llmesh mesh, viennagrid_int_t * vertex_count);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_vertex_buffer(viennagrid_llmesh mesh, viennagrid_numeric_t ** vertex_buffer);


  //////////////////////////////////
  // element functions
  //////////////////////////////////

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_element_count(viennagrid_llmesh mesh, viennagrid_int_t * element_count);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_element_count_by_type(viennagrid_llmesh mesh, viennagrid_int_t element_type, viennagrid_int_t * element_count);

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_element_type_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_type_buffer);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_element_vertex_count_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_vertex_count_buffer);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_element_vertex_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_vertex_buffer);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_element_segment_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_segment_buffer);

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_create_element_buffers(viennagrid_llmesh mesh, viennagrid_int_t element_count, viennagrid_int_t element_vertex_index_count); // creates for the element types buffer, the element vertex counts buffer and the element segment buffer with size of element_count and the element vertex buffer index with size of element_vertex_index_count. type buffer is initialized with VIENNAGRID_ELEMENT_TYPE_NOT_AN_ELEMENT, vertex count buffer is initialized with 0, vertex buffer is not initialized, segment buffer is initialized with -1

  //////////////////////////////////
  // dataset functions
  //////////////////////////////////

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_dataset_count(viennagrid_llmesh mesh, viennagrid_int_t * dataset_count);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_set_dataset_count(viennagrid_llmesh mesh, viennagrid_int_t dataset_count);

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_dataset_type(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_int_t * dataset_type);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_dataset_values_per_element(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_int_t * values_per_element);
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_dataset_buffer(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_numeric_t ** dataset_buffer);

  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_get_dataset_name(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, const char ** dataset_name); // non-named dataset: return NULL
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_set_dataset_name(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, const char * dataset_name); // non-named dataset: segment_name = NULL

  // creates a dataset for the element_type and with values_per_element, the dataset buffer will have values_per_element*element_count(element_type) values int total
  VIENNAGRID_EXPORTED_FUNCTION viennagrid_error_code_t viennagrid_create_dataset(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_int_t dataset_type, viennagrid_int_t values_per_element);

#ifdef  __cplusplus
}
#endif

#endif
