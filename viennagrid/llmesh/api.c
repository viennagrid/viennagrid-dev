#include <stdlib.h>
#include <string.h>

#include "api.h"

#ifndef VIENNAGRID_FUNCTION_IMPL_PREFIX
  #define VIENNAGRID_FUNCTION_IMPL_PREFIX
#endif


typedef struct viennagrid_llmesh_
{
  viennagrid_int_t dimension;

  viennagrid_int_t segment_count;
  char ** segment_names;

  viennagrid_int_t vertex_count;
  viennagrid_numeric_t * vertex_buffer;

  viennagrid_int_t element_count;
  viennagrid_int_t * element_type_buffer;
  viennagrid_int_t * element_vertex_count_buffer;
  viennagrid_int_t * element_vertex_buffer;
  viennagrid_int_t * element_segment_buffer;

  viennagrid_int_t dataset_count;
  viennagrid_int_t * dataset_element_types;
  viennagrid_int_t * dataset_values_per_element;
  viennagrid_numeric_t ** dataset_buffers;
  char ** dataset_names;
} viennagrid_llmesh_;




VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_make_llmesh(viennagrid_int_t dimension, viennagrid_llmesh * mesh)
{
  int i;
  if (!mesh)
    VIENNAGRID_SUCCESS;

  if (dimension > 3 || dimension < 1)
  {
    (*mesh) = NULL;
    return VIENNAGRID_INVALID_ARGUMENTS;
  }

  *mesh = (viennagrid_llmesh)malloc( sizeof(viennagrid_llmesh_) );
  if (!*mesh)
    return VIENNAGRID_OUT_OF_MEMORY;

  (*mesh)->dimension = dimension;
  (*mesh)->segment_count = 0;
  (*mesh)->segment_names = NULL;

  (*mesh)->vertex_count = 0;
  (*mesh)->vertex_buffer = NULL;

  (*mesh)->element_count = 0;

  (*mesh)->element_type_buffer = NULL;
  (*mesh)->element_vertex_count_buffer = NULL;
  (*mesh)->element_vertex_buffer = NULL;
  (*mesh)->element_segment_buffer = NULL;

  (*mesh)->dataset_count = 0;
  (*mesh)->dataset_element_types = NULL;
  (*mesh)->dataset_values_per_element = NULL;
  (*mesh)->dataset_buffers = NULL;
  (*mesh)->dataset_names = NULL;

  return VIENNAGRID_SUCCESS;
}



VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_delete_llmesh(viennagrid_llmesh * mesh)
{
  int i,j;

  if (!mesh || !(*mesh))
  {
    (*mesh) = NULL;
    return VIENNAGRID_SUCCESS;
  }

  if ( (*mesh)->segment_names )
  {
    for (i = 0; i < (*mesh)->segment_count; ++i)
      free( (*mesh)->segment_names[i] );

    free((*mesh)->segment_names);
  }

  free( (*mesh)->vertex_buffer );

  free( (*mesh)->element_type_buffer );
  free( (*mesh)->element_vertex_count_buffer );
  free( (*mesh)->element_vertex_buffer );
  free( (*mesh)->element_segment_buffer );

  free( (*mesh)->dataset_element_types );
  free( (*mesh)->dataset_values_per_element );
  for (i = 0; i < (*mesh)->dataset_count; ++i)
  {
    free( (*mesh)->dataset_buffers[i] );
    free( (*mesh)->dataset_names[i] );
  }

  (*mesh) = NULL;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_dimension(viennagrid_llmesh mesh, viennagrid_int_t * dimension)
{
  if (!dimension)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *dimension = mesh->dimension;
  return VIENNAGRID_SUCCESS;
}


VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_segment_count(viennagrid_llmesh mesh, viennagrid_int_t * segment_count)
{
  if (!segment_count)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *segment_count = mesh->segment_count;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_set_segment_count(viennagrid_llmesh mesh, viennagrid_int_t segment_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  int i;
  if (mesh->segment_names)
  {
    for (i = 0; i < segment_count; ++i)
      free( mesh->segment_names[i] );
  }
  free( mesh->segment_names );

  mesh->segment_count = segment_count;
  mesh->segment_names = (char **)malloc( sizeof(const char *) * segment_count );

  if (!mesh->segment_names)
    return VIENNAGRID_OUT_OF_MEMORY;

  for (i = 0; i < segment_count; ++i)
    mesh->segment_names[i] = NULL;

  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_segment_name(viennagrid_llmesh mesh, viennagrid_int_t segment_index, const char ** segment_name)
{
  if (!segment_name)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (segment_index < 0 || segment_index >= mesh->segment_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *segment_name = mesh->segment_names[segment_index];
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_set_segment_name(viennagrid_llmesh mesh, viennagrid_int_t segment_index, const char * segment_name)
{
  if (!segment_name)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (segment_index < 0 || segment_index >= mesh->segment_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  free( mesh->segment_names[segment_index] );
  if (segment_name)
  {
    mesh->segment_names[segment_index] = (char*)malloc( sizeof(char)*(strlen(segment_name)+1) );
    if (!mesh->segment_names[segment_index])
      return VIENNAGRID_OUT_OF_MEMORY;
    strcpy ( mesh->segment_names[segment_index], segment_name );
  }
  else
    mesh->segment_names[segment_index] = NULL;

  return VIENNAGRID_SUCCESS;
}



VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_create_vertex_buffer(viennagrid_llmesh mesh, viennagrid_int_t vertex_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (vertex_count <= 0)
    return VIENNAGRID_INVALID_ARGUMENTS;

  free( mesh->vertex_buffer );
  mesh->vertex_buffer = (viennagrid_numeric_t*)malloc( sizeof(viennagrid_numeric_t) * mesh->dimension * vertex_count );
  if (!mesh->vertex_buffer)
    return VIENNAGRID_OUT_OF_MEMORY;

  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_vertex_count(viennagrid_llmesh mesh, viennagrid_int_t * vertex_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!vertex_count)
    return VIENNAGRID_SUCCESS;

  *vertex_count = mesh->vertex_count;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_vertex_buffer(viennagrid_llmesh mesh, viennagrid_numeric_t ** vertex_buffer)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!vertex_buffer)
    return VIENNAGRID_SUCCESS;

  *vertex_buffer = mesh->vertex_buffer;
  return VIENNAGRID_SUCCESS;
}





VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_element_count(viennagrid_llmesh mesh, viennagrid_int_t * element_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!element_count)
    return VIENNAGRID_SUCCESS;

  *element_count = mesh->element_count;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_element_count_by_type(viennagrid_llmesh mesh, viennagrid_int_t element_type, viennagrid_int_t * element_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!element_count)
    return VIENNAGRID_SUCCESS;

  *element_count = 0;

  int i;
  for (i = 0; i < mesh->element_count; ++i)
  {
    if (mesh->element_type_buffer[i] == element_type)
      ++(*element_count);
  }

  return VIENNAGRID_SUCCESS;
}


VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_element_type_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_type_buffer)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!element_type_buffer)
    return VIENNAGRID_SUCCESS;

  *element_type_buffer = mesh->element_type_buffer;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_element_vertex_count_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_vertex_count_buffer)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!element_vertex_count_buffer)
    return VIENNAGRID_SUCCESS;

  *element_vertex_count_buffer = mesh->element_vertex_count_buffer;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_element_vertex_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_vertex_buffer)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!element_vertex_buffer)
    return VIENNAGRID_SUCCESS;

  *element_vertex_buffer = mesh->element_vertex_buffer;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_element_segment_buffer(viennagrid_llmesh mesh, viennagrid_int_t ** element_segment_buffer)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!element_segment_buffer)
    return VIENNAGRID_SUCCESS;

  *element_segment_buffer = mesh->element_segment_buffer;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_create_element_buffers(viennagrid_llmesh mesh, viennagrid_int_t element_count, viennagrid_int_t element_vertex_index_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (element_count <= 0 || element_vertex_index_count <= 0)
    return VIENNAGRID_INVALID_ARGUMENTS;

  free( mesh->element_type_buffer );
  free( mesh->element_vertex_count_buffer );
  free( mesh->element_vertex_buffer );
  free( mesh->element_segment_buffer );

  mesh->element_type_buffer = (viennagrid_int_t*)malloc( sizeof(viennagrid_int_t) * element_count );
  mesh->element_vertex_count_buffer = (viennagrid_int_t*)malloc( sizeof(viennagrid_int_t) * element_count );
  mesh->element_vertex_buffer = (viennagrid_int_t*)malloc( sizeof(viennagrid_int_t) * element_vertex_index_count );
  mesh->element_segment_buffer = (viennagrid_int_t*)malloc( sizeof(viennagrid_int_t) * element_count );

  if (!mesh->element_type_buffer || !mesh->element_vertex_count_buffer ||
      !mesh->element_vertex_buffer || !mesh->element_segment_buffer)
    return VIENNAGRID_OUT_OF_MEMORY;

  return VIENNAGRID_SUCCESS;
}



VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_dataset_count(viennagrid_llmesh mesh, viennagrid_int_t * dataset_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (!dataset_count)
    return VIENNAGRID_SUCCESS;

  *dataset_count = mesh->dataset_count;
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_set_dataset_count(viennagrid_llmesh mesh, viennagrid_int_t dataset_count)
{
  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_count < 0)
    return VIENNAGRID_INVALID_ARGUMENTS;

  int i;
  for (i = 0; i < mesh->dataset_count; ++i)
  {
    free( mesh->dataset_buffers[i] );
    free( mesh->dataset_names[i] );
  }
  free( mesh->dataset_buffers );
  free( mesh->dataset_names );


  mesh->dataset_count = dataset_count;

  mesh->dataset_element_types = (viennagrid_int_t*)malloc( sizeof(viennagrid_int_t)*dataset_count );
  mesh->dataset_values_per_element = (viennagrid_int_t*)malloc( sizeof(viennagrid_int_t)*dataset_count );

  if (!mesh->dataset_element_types || !mesh->dataset_values_per_element)
    return VIENNAGRID_OUT_OF_MEMORY;

  for (i = 0; i < dataset_count; ++i)
  {
    mesh->dataset_buffers[i] = NULL;
    mesh->dataset_names[i] = NULL;
  }

  return VIENNAGRID_SUCCESS;
}



VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_dataset_type(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_int_t * dataset_type)
{
  if (!dataset_type)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_index < 0 || dataset_index >= mesh->dataset_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *dataset_type = mesh->dataset_element_types[dataset_index];
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_dataset_values_per_element(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_int_t * values_per_element)
{
  if (!values_per_element)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_index < 0 || dataset_index >= mesh->dataset_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *values_per_element = mesh->dataset_values_per_element[dataset_index];
  return VIENNAGRID_SUCCESS;
}

VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_dataset_buffer(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_numeric_t ** dataset_buffer)
{
  if (!dataset_buffer)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_index < 0 || dataset_index >= mesh->dataset_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *dataset_buffer = mesh->dataset_buffers[dataset_index];
  return VIENNAGRID_SUCCESS;
}




VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_get_dataset_name(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, const char ** dataset_name)
{
  if (!dataset_name)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_index < 0 || dataset_index >= mesh->dataset_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  *dataset_name = mesh->segment_names[dataset_index];
  return VIENNAGRID_SUCCESS;
}


VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_set_dataset_name(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, const char * dataset_name)
{
  if (!dataset_name)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_index < 0 || dataset_index >= mesh->segment_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  free( mesh->segment_names[dataset_index] );
  if (dataset_name)
  {
    mesh->segment_names[dataset_index] = (char*)malloc( sizeof(char)*(strlen(dataset_name)+1) );
    if (!mesh->segment_names[dataset_index])
      return VIENNAGRID_OUT_OF_MEMORY;
    strcpy ( mesh->segment_names[dataset_index], dataset_name );
  }
  else
    mesh->segment_names[dataset_index] = NULL;

  return VIENNAGRID_SUCCESS;
}


VIENNAGRID_FUNCTION_IMPL_PREFIX viennagrid_error_code_t viennagrid_create_dataset(viennagrid_llmesh mesh, viennagrid_int_t dataset_index, viennagrid_int_t dataset_type, viennagrid_int_t values_per_element)
{
  if (!dataset_type)
    return VIENNAGRID_SUCCESS;

  if (!mesh)
    return VIENNAGRID_INVALID_ARGUMENTS;

  if (dataset_index < 0 || dataset_index >= mesh->dataset_count)
    return VIENNAGRID_INVALID_ARGUMENTS;

  mesh->dataset_element_types[dataset_index] = dataset_type;
  mesh->dataset_values_per_element[dataset_index] = values_per_element;


  viennagrid_int_t element_count;
  viennagrid_get_element_count_by_type(mesh, dataset_type, &element_count);

  mesh->dataset_element_types[dataset_index] = dataset_type;
  free( mesh->dataset_buffers[dataset_index] );
  mesh->dataset_buffers[dataset_index] = (viennagrid_numeric_t*)malloc( sizeof(viennagrid_numeric_t)* element_count * values_per_element );

  if (!mesh->dataset_buffers[dataset_index])
    return VIENNAGRID_OUT_OF_MEMORY;

  return VIENNAGRID_SUCCESS;
}

