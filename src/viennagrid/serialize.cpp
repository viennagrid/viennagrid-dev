#include "viennagrid/viennagrid.h"

#include <vector>
#include <string>
#include <map>

#include <cassert>
#include <cstring>

#include "mesh_hierarchy.hpp"


class serializer
{
public:

  template<typename T>
  void serialize(T const & value)
  {
    serialize(&value, 1);
  }

  template<typename T>
  void serialize(T const * ptr, std::size_t count)
  {
    std::size_t index = buffer_.size();
    buffer_.resize( index + sizeof(T)*count );
    memcpy( &buffer_[index], ptr, sizeof(T)*count );
  }

  void serialize(std::string const & str)
  {
    if (str.empty())
      serialize<int>(0);
    else
    {
      serialize< viennagrid_int >( str.size() );
      serialize< char >( str.c_str(), str.size() );
    }
  }

  std::size_t buffer_size() const { return buffer_.size(); }
  void const * buffer() const { return &buffer_[0]; }

private:
  std::vector<char> buffer_;
};




viennagrid_error viennagrid_mesh_hierarchy_serialize(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                     void ** blob,
                                                     viennagrid_int * size)
{
  if (!blob || !size)
    return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  serializer s;
  s.serialize<viennagrid_int>( VIENNAGRID_MAGIC_VALUE );

  s.serialize<viennagrid_int>( VIENNAGRID_VERSION_MAJOR );
  s.serialize<viennagrid_int>( VIENNAGRID_VERSION_MINOR );
  s.serialize<viennagrid_int>( VIENNAGRID_VERSION_PATCH );

  viennagrid_dimension geometric_dimension;
  viennagrid_mesh_hierarchy_geometric_dimension_get(mesh_hierarchy, &geometric_dimension);
  s.serialize<viennagrid_dimension>( geometric_dimension );

  s.serialize<viennagrid_int>( mesh_hierarchy->vertex_count() );
  s.serialize<viennagrid_numeric>( mesh_hierarchy->get_vertex(0), mesh_hierarchy->vertex_count()*geometric_dimension );

  viennagrid_dimension cell_dimension = mesh_hierarchy->cell_dimension();
  viennagrid_int cell_count = mesh_hierarchy->element_buffer(cell_dimension).size();

  s.serialize<viennagrid_dimension>( cell_dimension );
  s.serialize<viennagrid_int>( cell_count );
  s.serialize<viennagrid_element_type>( mesh_hierarchy->element_buffer(cell_dimension).element_types_pointer(), cell_count );
  s.serialize<viennagrid_int>( mesh_hierarchy->element_buffer(cell_dimension).vertex_offsets_pointer(), cell_count+1 );
  s.serialize<viennagrid_int>( mesh_hierarchy->element_buffer(cell_dimension).vertex_ids_pointer(), mesh_hierarchy->element_buffer(cell_dimension).vertex_offsets_pointer()[cell_count] );
  if ( mesh_hierarchy->element_buffer(cell_dimension).parent_id_pointer() )
  {
    s.serialize<bool>( true );
    s.serialize<viennagrid_int>( mesh_hierarchy->element_buffer(cell_dimension).parent_id_pointer(), cell_count );
  }
  else
  {
    s.serialize<bool>( false );
  }



  std::vector<viennagrid_int> cell_region_offsets(cell_count+1);
  cell_region_offsets[0] = 0;
  for (viennagrid_int i = 0; i != cell_count; ++i)
  {
    viennagrid_region_id * region_ids_begin = mesh_hierarchy->element_buffer(cell_dimension).regions_begin(i);
    viennagrid_region_id * region_ids_end = mesh_hierarchy->element_buffer(cell_dimension).regions_end(i);
    viennagrid_int count = region_ids_end-region_ids_begin;

    cell_region_offsets[i+1] = cell_region_offsets[i] + count;
  }

  std::vector<viennagrid_int> cell_regions( cell_region_offsets[cell_count] );
  int index = 0;
  for (viennagrid_int i = 0; i != cell_region_offsets[cell_count]; ++i)
  {
    for (viennagrid_region_id * region_id = mesh_hierarchy->element_buffer(cell_dimension).regions_begin(i);
                                region_id != mesh_hierarchy->element_buffer(cell_dimension).regions_end(i);
                              ++region_id, ++index)
         cell_regions[index] = *region_id;
  }

  s.serialize< viennagrid_int >( &cell_region_offsets[0], cell_count+1 );
  s.serialize< viennagrid_int >( &cell_regions[0], cell_region_offsets[cell_count] );





  viennagrid_int mesh_count = mesh_hierarchy->mesh_count();

  std::map<viennagrid_mesh, viennagrid_int> mesh_to_index_map;
  std::map<viennagrid_int, viennagrid_mesh> index_to_mesh_map;

  s.serialize< viennagrid_int >( mesh_count );

  std::vector<viennagrid_int> mesh_parents(mesh_count);
  std::vector<viennagrid_int> mesh_vertex_count(mesh_count);
  std::vector<viennagrid_int> mesh_cell_count(mesh_count);

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh mesh = mesh_hierarchy->mesh(i);
    mesh_to_index_map[mesh] = i;
    index_to_mesh_map[i] = mesh;
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh mesh = mesh_hierarchy->mesh(i);

    viennagrid_mesh parent = mesh->parent();
    if (!parent)
      mesh_parents[i] = -1;
    else
      mesh_parents[i] = mesh_to_index_map[mesh];

    mesh_vertex_count[i] = mesh->element_count(0);
    mesh_cell_count[i] = mesh->element_count(cell_dimension);
  }

  s.serialize< viennagrid_int >( &mesh_parents[0], mesh_count );
  s.serialize< viennagrid_int >( &mesh_vertex_count[0], mesh_count );
  s.serialize< viennagrid_int >( &mesh_cell_count[0], mesh_count );

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    viennagrid_mesh mesh = mesh_hierarchy->mesh(i);
    s.serialize( mesh->name() );
    s.serialize< viennagrid_int >( mesh->elements_begin(0), mesh_vertex_count[i] );
    s.serialize< viennagrid_int >( mesh->elements_begin(cell_dimension), mesh_cell_count[i] );
  }




  viennagrid_int region_count = mesh_hierarchy->region_count();
  s.serialize< viennagrid_int >( region_count );

  index = 0;
  for (viennagrid_region_id * region_id = mesh_hierarchy->region_ids_begin();
                              region_id != mesh_hierarchy->region_ids_end();
                            ++region_id, ++index)
  {
    s.serialize< viennagrid_int >( *region_id );
    s.serialize( mesh_hierarchy->get_region(*region_id)->name() );
  }



  viennagrid_new( s.buffer_size(), blob );
  memcpy( *blob, s.buffer(), s.buffer_size() );
  *size = s.buffer_size();

  return VIENNAGRID_SUCCESS;
}





class deserializer
{
public:

  deserializer(void * buffer_, std::size_t size_) : buffer((char*)buffer_), pos(0), size(size_) {}

  template<typename T>
  void deserialize(T & val)
  {
    deserialize(&val, 1);
  }

  void deserialize(std::string & str)
  {
    int size;
    deserialize<int>(size);
    if (size == 0)
      str = "";
    else
    {
      str.resize(size);
      deserialize<char>( &str[0], size );
    }
  }

  template<typename T>
  void deserialize(T * ptr, std::size_t count)
  {
    memcpy(ptr, buffer+pos, sizeof(T)*count);
    pos += sizeof(T)*count;
    assert(pos <= size);
  }



private:
  char * buffer;
  std::size_t pos;
  std::size_t size;
};




viennagrid_error viennagrid_mesh_hierarchy_deserialize(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                       void * blob,
                                                       viennagrid_int size)
{
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
    return VIENNAGRID_ERROR_DESERIALIZE_VERSION_MISSMATCH;
  }


  viennagrid_dimension geometric_dimension;
  d.deserialize< viennagrid_dimension >(geometric_dimension);

  viennagrid_mesh_hierarchy_clear(mesh_hierarchy);
  viennagrid_mesh_hierarchy_geometric_dimension_set(mesh_hierarchy, geometric_dimension);


  viennagrid_int vertex_count;
  d.deserialize< viennagrid_int >(vertex_count);

  std::vector<viennagrid_numeric> vertex_coords(vertex_count*geometric_dimension);
  d.deserialize< viennagrid_numeric >( &vertex_coords[0], vertex_count*geometric_dimension );


  // deserialize points
  for (viennagrid_int i = 0; i != vertex_count; ++i)
  {
    viennagrid_mesh_hierarchy_vertex_create(mesh_hierarchy, &vertex_coords[0] + geometric_dimension*i, NULL);
  }


  viennagrid_dimension cell_dimension;
  d.deserialize< viennagrid_dimension >(cell_dimension);
  viennagrid_int cell_count;
  d.deserialize< viennagrid_int >(cell_count);

  std::vector<viennagrid_element_type> cell_element_types(cell_count);
  d.deserialize< viennagrid_element_type >( &cell_element_types[0], cell_count );

  std::vector<viennagrid_int> cell_vertex_offsets(cell_count+1);
  d.deserialize< viennagrid_int >( &cell_vertex_offsets[0], cell_count+1 );

  std::vector<viennagrid_int> cell_vertex_ids(cell_vertex_offsets[cell_count]);
  d.deserialize< viennagrid_int >( &cell_vertex_ids[0], cell_vertex_offsets[cell_count] );


  std::vector<viennagrid_int> cell_parent_ids;
  bool cell_parents_available;
  d.deserialize< bool >( cell_parents_available );
  if (cell_parents_available)
  {
    cell_parent_ids.resize(cell_count);
    d.deserialize< viennagrid_int >( &cell_parent_ids[0], cell_count );
  }


  std::vector<viennagrid_int> cell_region_offsets(cell_count+1);
  d.deserialize< viennagrid_int >( &cell_region_offsets[0], cell_count+1 );

  std::vector<viennagrid_int> cell_regions(cell_region_offsets[cell_count]);
  d.deserialize< viennagrid_int >( &cell_regions[0], cell_region_offsets[cell_count] );


  viennagrid_int mesh_count;
  d.deserialize< viennagrid_int >(mesh_count);

  std::vector<viennagrid_int> mesh_parents(mesh_count);
  d.deserialize< viennagrid_int >( &mesh_parents[0], mesh_count );

  std::vector<viennagrid_int> mesh_vertex_count(mesh_count);
  d.deserialize< viennagrid_int >( &mesh_vertex_count[0], mesh_count );

  std::vector<viennagrid_int> mesh_cell_count(mesh_count);
  d.deserialize< viennagrid_int >( &mesh_cell_count[0], mesh_count );

  std::vector< std::vector<viennagrid_int> > mesh_vertices(mesh_count);
  std::vector< std::vector<viennagrid_int> > mesh_cells(mesh_count);


  std::vector<viennagrid_mesh> index_mesh_map(mesh_count);

  viennagrid_mesh_hierarchy_root_mesh_get( mesh_hierarchy, &index_mesh_map[0] );
  for (viennagrid_int i = 1; i != mesh_count; ++i)
  {
    assert( mesh_parents[i] < i );
    viennagrid_mesh parent = index_mesh_map[ mesh_parents[i] ];

    viennagrid_mesh mesh;
    viennagrid_mesh_create( parent, &mesh );

    index_mesh_map[i] = mesh;
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    std::string name;
    d.deserialize(name);
    viennagrid_mesh_name_set( index_mesh_map[i], name.c_str() );

    mesh_vertices[i].resize( mesh_vertex_count[i] );
    d.deserialize< viennagrid_int >( &mesh_vertices[i][0], mesh_vertex_count[i] );

    mesh_cells[i].resize( mesh_cell_count[i] );
    d.deserialize< viennagrid_int >( &mesh_cells[i][0], mesh_cell_count[i] );
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
    viennagrid_mesh_hierarchy_region_get_or_create( mesh_hierarchy, region_id, &region );
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

    viennagrid_mesh_hierarchy_element_create(mesh_hierarchy,
                                             cell_element_types[i],
                                             cell_vertex_count,
                                             &cell_vertex_ids[0] + cell_vertex_offsets[i],
                                             &cell_id);

    if (cell_parents_available)
    {
      viennagrid_element_parent_set(mesh_hierarchy,
                                    cell_dimension,
                                    cell_id,
                                    cell_parent_ids[i]);
    }

    for (viennagrid_int j = cell_region_offsets[i]; j != cell_region_offsets[i+1]; ++j)
    {
      viennagrid_int region_id = cell_regions[j];
      viennagrid_element_add_to_region(mesh_hierarchy,
                                       cell_dimension,
                                       cell_id,
                                       region_id);
    }
  }

  for (viennagrid_int i = 0; i != mesh_count; ++i)
  {
    for (viennagrid_int j = 0; j != mesh_vertex_count[i]; ++j)
    {
      viennagrid_mesh_element_add(index_mesh_map[i], 0, mesh_vertices[i][j]);
    }

    for (viennagrid_int j = 0; j != mesh_cell_count[i]; ++j)
    {
      viennagrid_mesh_element_add(index_mesh_map[i], cell_dimension, mesh_cells[i][j]);
    }
  }

  mesh_hierarchy->increment_change_counter();

  return VIENNAGRID_SUCCESS;
}

