#include "viennagrid/viennagrid.h"

#include <vector>
#include <string>
#include <map>

#include <cassert>
#include <cstring>

#include "mesh.hpp"
#include "mesh_hierarchy.hpp"

class serializer
{
public:

  template<typename T>
  void serialize(T const & value)
  {
    std::size_t index = buffer_.size();
    buffer_.resize( index + sizeof(T) );
    memcpy( &buffer_[index], &value, sizeof(T) );
  }

  template<typename T>
  void serialize(T const * ptr, std::size_t count)
  {
    if (!ptr || count < 0)
      count = 0;

    if (count > 0)
    {
      std::size_t index = buffer_.size();
      buffer_.resize( index + sizeof(std::size_t) + sizeof(T)*count );

      memcpy( &buffer_[index], &count, sizeof(std::size_t) );
      memcpy( &buffer_[index] + sizeof(std::size_t), ptr, sizeof(T)*count );
    }
    else
    {
      std::size_t index = buffer_.size();
      buffer_.resize( index + sizeof(std::size_t) );
      memcpy( &buffer_[index], &count, sizeof(std::size_t) );
    }
  }

  void serialize(std::string const & str)
  {
    if (str.empty())
      serialize<char>(NULL, 0);
    else
      serialize<char>(str.c_str(), str.size());
  }

  std::size_t buffer_size() const { return buffer_.size(); }
  void const * buffer() const { return &buffer_[0]; }

private:
  std::vector<char> buffer_;
};


class deserializer
{
public:

  deserializer(void * buffer_, std::size_t size_) : buffer((char*)buffer_), pos(0), size(size_) {}

  template<typename T>
  void deserialize(T & val)
  {
    memcpy(&val, buffer+pos, sizeof(T));
    pos += sizeof(T);
    assert(pos <= size);
  }

  void deserialize(std::string & str)
  {
    std::size_t count;
    memcpy(&count, buffer+pos, sizeof(std::size_t));
    pos += sizeof(std::size_t);

    if (count == 0)
      str = "";
    else
    {
      str.resize(count);
      memcpy(&str[0], buffer+pos, sizeof(char)*count);
    }

    pos += sizeof(char)*count;
    assert(pos <= size);
  }

  template<typename T, typename Alloc>
  void deserialize(std::vector<T, Alloc> & vec)
  {
    std::size_t count;
    memcpy(&count, buffer+pos, sizeof(std::size_t));
    pos += sizeof(std::size_t);

    vec.resize(count);

    if (!vec.empty())
      memcpy(&vec[0], buffer+pos, sizeof(T)*count);
    pos += sizeof(T)*count;

    assert(pos <= size);
  }

private:
  char * buffer;
  std::size_t pos;
  std::size_t size;
};





viennagrid_error viennagrid_mesh_serialize(viennagrid_mesh mesh,
                                           void ** blob,
                                           viennagrid_int * size)
{
  if (!mesh->is_root())
    return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  if (!blob || !size)
    return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  serializer s;
  s.serialize<viennagrid_int>( VIENNAGRID_MAGIC_VALUE );

  s.serialize<viennagrid_int>( VIENNAGRID_VERSION_MAJOR );
  s.serialize<viennagrid_int>( VIENNAGRID_VERSION_MINOR );
  s.serialize<viennagrid_int>( VIENNAGRID_VERSION_PATCH );

  viennagrid_dimension geometric_dimension;
  viennagrid_mesh_geometric_dimension_get(mesh, &geometric_dimension);
  s.serialize<viennagrid_dimension>( geometric_dimension );

  s.serialize<viennagrid_int>( mesh->vertex_count() );
  s.serialize<viennagrid_numeric>( mesh->get_vertex(0), mesh->vertex_count()*geometric_dimension );

  viennagrid_dimension cell_dimension = mesh->cell_dimension();
  viennagrid_int cell_count = mesh->mesh_hierarchy()->element_count(cell_dimension);

  s.serialize<viennagrid_dimension>( cell_dimension );
  s.serialize<viennagrid_int>( cell_count );
  s.serialize<viennagrid_element_type>( mesh->element_types_pointer(cell_dimension), cell_count);
  s.serialize<viennagrid_int>( mesh->vertex_offsets_pointer(cell_dimension), cell_count+1);
  s.serialize<viennagrid_element_id>( mesh->vertex_ids_pointer(cell_dimension), mesh->vertex_offsets_pointer(cell_dimension)[cell_count]);

  s.serialize<viennagrid_int>( mesh->parent_id_pointer(cell_dimension), mesh->parent_id_pointer_size(cell_dimension) );
  s.serialize<void*>( mesh->aux_pointer(cell_dimension), mesh->aux_pointer_size(cell_dimension) );




  std::vector<viennagrid_int> cell_region_offsets(cell_count+1);
  cell_region_offsets[0] = 0;
  for (viennagrid_int index = 0; index != cell_count; ++index)
  {
    viennagrid_region_id * region_ids_begin = mesh->regions_begin( viennagrid_compose_element_id(cell_dimension, index) );
    viennagrid_region_id * region_ids_end   = mesh->regions_end( viennagrid_compose_element_id(cell_dimension, index) );
    viennagrid_int count = region_ids_end-region_ids_begin;

    cell_region_offsets[index+1] = cell_region_offsets[index] + count;
  }

  std::vector<viennagrid_int> cell_regions( cell_region_offsets[cell_count] );
  int index = 0;
  for (viennagrid_int i = 0; i != cell_region_offsets[cell_count]; ++i)
  {
    viennagrid_region_id * region_ids_begin = mesh->regions_begin( viennagrid_compose_element_id(cell_dimension, index) );
    viennagrid_region_id * region_ids_end   = mesh->regions_end( viennagrid_compose_element_id(cell_dimension, index) );

    for (viennagrid_region_id * region_id = region_ids_begin; region_id != region_ids_end; ++region_id, ++index)
      cell_regions[index] = *region_id;
  }

  s.serialize< viennagrid_int >( &cell_region_offsets[0], cell_count+1 );
  s.serialize< viennagrid_int >( &cell_regions[0], cell_region_offsets[cell_count] );





  viennagrid_int mesh_count = mesh->mesh_hierarchy()->mesh_count();

  std::map<viennagrid_mesh, viennagrid_int> mesh_to_index_map;
  std::map<viennagrid_int, viennagrid_mesh> index_to_mesh_map;

  s.serialize< viennagrid_int >( mesh_count );

  std::vector<viennagrid_int> mesh_parents(mesh_count);

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh tmp = mesh->mesh_hierarchy()->mesh(i);
    mesh_to_index_map[tmp] = i;
    index_to_mesh_map[i] = tmp;
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh tmp = mesh->mesh_hierarchy()->mesh(i);

    viennagrid_mesh parent = tmp->parent();
    if (!parent)
      mesh_parents[i] = -1;
    else
      mesh_parents[i] = mesh_to_index_map[tmp];
  }

  s.serialize< viennagrid_int >( &mesh_parents[0], mesh_count );

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh tmp = mesh->mesh_hierarchy()->mesh(i);
    s.serialize( tmp->name() );
    s.serialize< viennagrid_int >( tmp->elements_begin(0), tmp->element_count(0) );
    s.serialize< viennagrid_int >( tmp->elements_begin(cell_dimension), tmp->element_count(cell_dimension) );
  }




  viennagrid_int region_count = mesh->region_count();
  s.serialize< viennagrid_int >( region_count );

  index = 0;
  for (viennagrid_region_id * region_id = mesh->region_ids_begin();
                              region_id != mesh->region_ids_end();
                            ++region_id, ++index)
  {
    s.serialize< viennagrid_int >( *region_id );
    s.serialize( mesh->get_region(*region_id)->name() );
  }



  viennagrid_new( s.buffer_size(), blob );
  memcpy( *blob, s.buffer(), s.buffer_size() );
  *size = s.buffer_size();

  return VIENNAGRID_SUCCESS;
}









viennagrid_error viennagrid_mesh_deserialize(void * blob,
                                             viennagrid_int size,
                                             viennagrid_mesh mesh)
{
  if (!mesh->is_root())
    return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  viennagrid_mesh_hierarchy_ * mesh_hierarchy = mesh->mesh_hierarchy();

  deserializer d(blob, size);

  viennagrid_int magic_value;
  d.deserialize< viennagrid_int >(magic_value);

  if (magic_value != VIENNAGRID_MAGIC_VALUE)
    return VIENNAGRID_ERROR_DESERIALIZE_MAGIC_VALUE_MISSMATCH;


  viennagrid_int version_major;
  d.deserialize< viennagrid_int >(version_major);
  viennagrid_int version_minor;
  d.deserialize< viennagrid_int >(version_minor);
  viennagrid_int version_patch;
  d.deserialize< viennagrid_int >(version_patch);

  if ( (version_major != VIENNAGRID_VERSION_MAJOR) ||
       (version_minor != VIENNAGRID_VERSION_MINOR) ||
       (version_patch != VIENNAGRID_VERSION_PATCH) )
  {
    return VIENNAGRID_ERROR_DESERIALIZE_VERSION_MISMATCH;
  }


  viennagrid_dimension geometric_dimension;
  d.deserialize< viennagrid_dimension >(geometric_dimension);

  viennagrid_mesh_clear(mesh);
  viennagrid_mesh_geometric_dimension_set(mesh, geometric_dimension);


  viennagrid_int vertex_count;
  d.deserialize< viennagrid_int >(vertex_count);

  std::vector<viennagrid_numeric> vertex_coords;
  d.deserialize< viennagrid_numeric >( vertex_coords );
  if ((viennagrid_int)vertex_coords.size() != vertex_count*geometric_dimension)
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;


  // deserialize points
  viennagrid_mesh_vertex_batch_create(mesh, vertex_count, &vertex_coords[0], NULL);

  viennagrid_dimension cell_dimension;
  d.deserialize< viennagrid_dimension >(cell_dimension);
  viennagrid_int cell_count;
  d.deserialize< viennagrid_int >(cell_count);

  std::vector<viennagrid_element_type> cell_element_types;
  d.deserialize< viennagrid_element_type >( cell_element_types );
  if ((viennagrid_int)cell_element_types.size() != cell_count)
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;

  std::vector<viennagrid_int> cell_vertex_offsets;
  d.deserialize< viennagrid_int >( cell_vertex_offsets );
  if ((viennagrid_int)cell_vertex_offsets.size() != cell_count+1)
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;

  std::vector<viennagrid_int> cell_vertex_ids;
  d.deserialize< viennagrid_int >( cell_vertex_ids );
  if ((viennagrid_int)cell_vertex_ids.size() != cell_vertex_offsets[cell_count])
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;

  std::vector<viennagrid_int> cell_parent_ids;
  d.deserialize< viennagrid_int >( cell_parent_ids );

  std::vector<void*> aux_data;
  d.deserialize< void* >( aux_data );


  std::vector<viennagrid_int> cell_region_offsets;
  d.deserialize< viennagrid_int >( cell_region_offsets );
  if ((viennagrid_int)cell_region_offsets.size() != cell_count+1)
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;

  std::vector<viennagrid_int> cell_regions;
  d.deserialize< viennagrid_int >( cell_regions );
  if ((viennagrid_int)cell_regions.size() != cell_region_offsets[cell_count])
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;


  viennagrid_int mesh_count;
  d.deserialize< viennagrid_int >(mesh_count);

  std::vector<viennagrid_int> mesh_parents;
  d.deserialize< viennagrid_int >( mesh_parents );
  if ((viennagrid_int)mesh_parents.size() != mesh_count)
    return VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH;

  std::vector< std::vector<viennagrid_element_id> > mesh_vertices(mesh_count);
  std::vector< std::vector<viennagrid_element_id> > mesh_cells(mesh_count);


  std::vector<viennagrid_mesh> index_mesh_map(mesh_count);

  index_mesh_map[0] = mesh;
  for (viennagrid_int i = 1; i != mesh_count; ++i)
  {
    assert( mesh_parents[i] < i );
    viennagrid_mesh parent = index_mesh_map[ mesh_parents[i] ];
    index_mesh_map[i] = parent->make_child();
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    std::string name;
    d.deserialize(name);
    viennagrid_mesh_name_set( index_mesh_map[i], name.c_str() );

    d.deserialize< viennagrid_element_id >( mesh_vertices[i] );
    d.deserialize< viennagrid_element_id >( mesh_cells[i] );
  }

  // deserialize regions
  viennagrid_int region_count;
  d.deserialize< viennagrid_int >(region_count);
  std::map<viennagrid_int, viennagrid_region> id_region_map;
  for (viennagrid_int i = 0; i != region_count; ++i)
  {
    viennagrid_int region_id;
    d.deserialize< viennagrid_int >(region_id);

    viennagrid_region region;
    viennagrid_mesh_region_get_or_create( mesh, region_id, &region );
    id_region_map[ region_id ] = region;

    std::string name;
    d.deserialize(name);
    viennagrid_region_name_set( region, name.c_str() );
  }

  // deserialize cells
  for (viennagrid_int i = 0; i != cell_count; ++i)
  {
    viennagrid_int cell_id;
    viennagrid_int cell_vertex_count = cell_vertex_offsets[i+1] - cell_vertex_offsets[i];

    viennagrid_mesh_element_create(mesh,
                                   cell_element_types[i],
                                   cell_vertex_count,
                                   &cell_vertex_ids[0] + cell_vertex_offsets[i],
                                   &cell_id);

    if (INDEX(cell_id) < (viennagrid_element_id)cell_parent_ids.size() )
    {
      mesh_hierarchy->element_buffer(cell_dimension).set_parent_id(cell_id, cell_parent_ids[i]);
    }

    if (INDEX(cell_id) < (viennagrid_element_id)aux_data.size() )
    {
      mesh_hierarchy->element_buffer(cell_dimension).set_aux(cell_id, aux_data[i]);
    }

    for (viennagrid_int j = cell_region_offsets[i]; j != cell_region_offsets[i+1]; ++j)
    {
      viennagrid_int region_id = cell_regions[j];

      viennagrid_region_element_add( id_region_map[region_id], cell_id );
    }
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    for (viennagrid_int j = 0; j != (viennagrid_int)mesh_vertices[i].size(); ++j)
    {
      index_mesh_map[i]->add_element(mesh_vertices[i][j]);
    }

    for (viennagrid_int j = 0; j != (viennagrid_int)mesh_cells[i].size(); ++j)
    {
      index_mesh_map[i]->add_element(mesh_cells[i][j]);
    }
  }

  mesh_hierarchy->increment_change_counter();

  return VIENNAGRID_SUCCESS;
}

