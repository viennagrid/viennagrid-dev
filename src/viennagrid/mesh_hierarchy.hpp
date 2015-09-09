#ifndef VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP
#define VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP

#include <algorithm>
#include <sstream>
#include <deque>
#include <cstring>

#include "viennagrid/viennagrid.h"

#include "common.hpp"
#include "buffer.hpp"
// #include "mesh.hpp"
#include "region.hpp"
#include "dynamic_sizeof.hpp"

#include "boost/container/flat_map.hpp"

typedef dense_packed_multibuffer<viennagrid_int, viennagrid_int> ViennaGridBoundaryBufferType;
typedef dense_packed_multibuffer<viennagrid_int, viennagrid_numeric> ViennaGridHolePointBufferType;
typedef dense_packed_multibuffer<viennagrid_int, viennagrid_region_id> ViennaGridRegionBufferType;

struct element_key;
struct viennagrid_element_buffer;
struct viennagrid_mesh_hierarchy_;




template<typename T, typename SizeT>
bool unsorted_arrays_equal(T const * lhs, SizeT lhs_size,
                           T const * rhs, SizeT rhs_size)
{
  if (lhs_size != rhs_size)
    return false;

  std::vector<T> lhs_sorted(lhs_size);
  std::vector<T> rhs_sorted(rhs_size);

  std::copy( lhs, lhs+lhs_size, lhs_sorted.begin() );
  std::copy( rhs, rhs+rhs_size, rhs_sorted.begin() );

  std::sort( lhs_sorted.begin(), lhs_sorted.end() );
  std::sort( rhs_sorted.begin(), rhs_sorted.end() );

  return lhs_sorted == rhs_sorted;
}







template<typename T, typename SizeT>
class short_vector
{
public:

  typedef SizeT size_type;

  typedef T * pointer;
  typedef T const * const_pointer;

  typedef pointer iterator;
  typedef const_pointer const_iterator;


  iterator begin() { return values_; }
  iterator end() { return values_+size(); }

  const_iterator cbegin() const { return values_; }
  const_iterator cend() const { return values_+size(); }

  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }



  short_vector() : values_(0), size_(0) {}
  short_vector(SizeT size_in) : values_(size_in ? new T[size_in] : 0), size_(size_in) {}

  short_vector(short_vector const & sv) : values_(0), size_(sv.size())
  {
    if (!sv.empty())
    {
      values_ = new T[size()];
      memcpy( values_, &sv[0], sizeof(T)*size() );
    }
  }

  short_vector & operator=(short_vector const & sv)
  {
    if (values_)
      delete[] values_;

    size_ = sv.size();
    if (!sv.empty())
    {
      values_ = new T[size()];
      memcpy( values_, &sv[0], sizeof(T)*size() );
    }

    return *this;
  }

  ~short_vector()
  {
    if (values_)
      delete[] values_;
  }

  bool empty() const { return size() == 0; }

  SizeT size() const { return size_; }
  void resize(SizeT new_size)
  {
    if (new_size == size_)
      return;

    T * new_values = new T[new_size];
    memcpy( new_values, values_, sizeof(T)*(std::min(size_, new_size)) );
    delete[] values_;

    values_ = new_values;
    size_ = new_size;
  }

  void push_back(T const & t)
  {
    resize( size()+1 );
    (*this)[size()-1] =  t;
  }

  T const & operator[](SizeT index) const { return values_[index]; }
  T & operator[](SizeT index) { return values_[index]; }

  T const & front() const { return values_[0]; }
  T & front() { return values_[0]; }


private:
  T * values_;
  SizeT size_;
};


template<typename T, typename SizeT>
bool operator<( short_vector<T,SizeT> const & lhs, short_vector<T,SizeT> const & rhs )
{
  return std::lexicographical_compare( lhs.begin(), lhs.end(), rhs.begin(), rhs.end() );
}


template<typename T, typename SizeT>
bool operator==( short_vector<T,SizeT> const & lhs, short_vector<T,SizeT> const & rhs )
{
  if (lhs.size() != rhs.size())
    return false;

  return std::equal( lhs.begin(), lhs.end(), rhs.begin() );

  return true;
}




struct element_key
{
  element_key() {}
  element_key(viennagrid_int * ids, viennagrid_int id_count) : vertex_ids(id_count)
  {
    std::copy( ids, ids+id_count, &vertex_ids[0] );
    std::sort( vertex_ids.begin(), vertex_ids.end()   );
  }

  bool operator<(element_key const & rhs) const
  { return vertex_ids < rhs.vertex_ids; }

  bool operator==(element_key const & rhs) const
  { return vertex_ids == rhs.vertex_ids; }

  bool operator!=(element_key const & rhs) const
  { return !(*this == rhs); }

  viennagrid_int front() const { return vertex_ids.front(); }

  short_vector<viennagrid_int, unsigned char> vertex_ids;
};








struct viennagrid_element_buffer
{
  friend struct viennagrid_mesh_hierarchy_;
  friend struct viennagrid_mesh_;

  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  viennagrid_element_buffer() : topologic_dimension(-1), element_count(0), using_packed_region_buffer(false) {}

  void set_topologic_dimension(viennagrid_dimension topo_dim_in)
  {
    topologic_dimension = topo_dim_in;
    boundary_ids.resize(topologic_dimension);
  }


  viennagrid_element_type element_type(viennagrid_int element_id) const
  {
    if (topologic_dimension == 0)
      return VIENNAGRID_ELEMENT_TYPE_VERTEX;
    return element_types[element_id];
  }

  viennagrid_int * boundary_begin(viennagrid_int boundary_topo_dim, viennagrid_int element_id)
  {
    return boundary_buffer(boundary_topo_dim).begin(element_id);
  }

  viennagrid_int * boundary_end(viennagrid_int boundary_topo_dim, viennagrid_int element_id)
  {
    return boundary_buffer(boundary_topo_dim).end(element_id);
  }


  viennagrid_int parent_id(viennagrid_int element_id) const
  {
    if ((topologic_dimension == 0) || (parents.empty()))
      return -1;
    return parents[element_id];
  }

  void set_parent_id(viennagrid_int element_id, viennagrid_int parent_id)
  {
    if (topologic_dimension != 0)
    {
      if (parents.empty())
      {
        parents.resize(element_id+1, -1);
        parents[element_id] = parent_id;
      }
    }


  }

  void add_to_region(viennagrid_int element_id, viennagrid_region_id region_id)
  {
    for (viennagrid_region_id * it = regions_begin(element_id); it != regions_end(element_id); ++it)
    {
      if (*it == region_id)
        return;
    }

    if (using_packed_region_buffer)
    {
      if (element_id >= packed_region_buffer.size())
      {
        for (viennagrid_int i = packed_region_buffer.size(); i != element_id+1; ++i)
          packed_region_buffer.push_back(0);
      }

      packed_region_buffer.add( element_id, region_id );
    }
    else
    {
      if ( element_id >= (viennagrid_int)region_buffer.size() )
        region_buffer.resize(element_id+1);

      region_buffer[element_id].push_back( region_id );
    }
  }

  viennagrid_region_id * regions_begin(viennagrid_int element_id)
  {
    if (using_packed_region_buffer)
      return packed_region_buffer.begin(element_id);
    else
      return (((viennagrid_int)region_buffer.size() <= element_id) || region_buffer[element_id].empty()) ? 0 : &region_buffer[element_id][0];
  }
  viennagrid_region_id * regions_end(viennagrid_int element_id)
  {
    if (using_packed_region_buffer)
      return packed_region_buffer.end(element_id);
    else
      return (((viennagrid_int)region_buffer.size() <= element_id) || region_buffer[element_id].empty()) ? 0 : &region_buffer[element_id][0] + region_buffer[element_id].size();
  }


  void clear(viennagrid_mesh_hierarchy_ * mesh_hierarchy_)
  {
    element_types.clear();
    parents.clear();
    region_buffer.clear();

    boundary_ids.clear();
    element_map.clear();

    mesh_hierarchy = mesh_hierarchy_;

    element_count = 0;
  }

  void clear_boundary()
  {
    for (std::size_t i = 1; i != boundary_ids.size(); ++i)
    {
      boundary_ids[i].clear();
    }
  }

  viennagrid_int size() const { return element_count; }

  viennagrid_element_type * element_types_pointer() { return topologic_dimension == 0 ? 0 : &element_types[0]; }
  viennagrid_int * vertex_offsets_pointer() { return boundary_buffer(0).offset_pointer(); }
  viennagrid_int * vertex_ids_pointer() { return boundary_buffer(0).values_pointer(); }
  viennagrid_int * parent_id_pointer() { return parents.empty() == 0 ? 0 : &parents[0]; }

  ViennaGridBoundaryBufferType & boundary_buffer(viennagrid_dimension boundary_topo_dim)
  {
    return boundary_ids[boundary_topo_dim];
  }

  void optimize_memory()
  {
    shrink_to_fit(element_types);
    shrink_to_fit(parents);

    shrink_to_fit(boundary_ids);
    for (std::size_t i = 0; i != boundary_ids.size(); ++i)
      boundary_ids[i].shrink_to_fit();

    shrink_to_fit(element_map);
//     for (int i = 0; i != element_map.size(); ++i)
//       element_map[i].shrink_to_fit();
//     packed_element_map.shrink_to_fit();

    switch_to_packed_region_buffer_layout();
  }

  void switch_to_packed_region_buffer_layout()
  {
    if (!using_packed_region_buffer)
    {
      long total_region_count = 0;
      for (std::size_t i = 0; i != region_buffer.size(); ++i)
        total_region_count += region_buffer[i].size();

      packed_region_buffer.reserve( region_buffer.size()+1, total_region_count );

      for (std::size_t i = 0; i != region_buffer.size(); ++i)
      {
        viennagrid_region_id * region_ptr = packed_region_buffer.push_back( region_buffer[i].size() );
        std::copy( region_buffer[i].begin(), region_buffer[i].end(), region_ptr );
      }

      packed_region_buffer.shrink_to_fit();

      region_buffer.clear();
      shrink_to_fit( region_buffer );

      using_packed_region_buffer = true;
    }
  }



private:

  viennagrid_int get_element(viennagrid_int * ids,
                             viennagrid_int id_count,
                             viennagrid_element_type et)
  {
    viennagrid_int * min_vertex_id = std::min_element(ids, ids+id_count);
    assert( min_vertex_id != ids+id_count );

    if (*min_vertex_id >= (viennagrid_int)element_map.size())
      return -1;

    viennagrid_int * begin_element_ids = element_map[*min_vertex_id].begin();
    viennagrid_int * end_element_ids = element_map[*min_vertex_id].end();

    for (viennagrid_int * eit = begin_element_ids; eit != end_element_ids; ++eit)
    {
      viennagrid_int * vertex_ids = boundary_begin(0, *eit);
      viennagrid_int vertex_count = boundary_end(0, *eit)-vertex_ids;

      if ( unsorted_arrays_equal(ids, id_count, vertex_ids, vertex_count) && (element_types[*eit] == et) )
        return *eit;
    }

    return -1;
  }

  viennagrid_int make_element(viennagrid_element_type element_type_,
                              viennagrid_int * vertex_ids,
                              viennagrid_int vertex_count,
                              bool reserve_boundary);

  viennagrid_int make_elements(viennagrid_int element_count_,
                               viennagrid_element_type * element_types_,
                               viennagrid_int * element_vertex_index_offsets_,
                               viennagrid_int * element_vertex_indices_);

  viennagrid_int make_vertices(viennagrid_int vertex_count);


  void reserve_boundary(viennagrid_int element_id);
  void make_boundary(viennagrid_int element_id, viennagrid_mesh mesh);


  // the current topologic dimension
  viennagrid_dimension topologic_dimension;

  // the element count
  viennagrid_int element_count;

  // types of the element
  std::vector<viennagrid_element_type> element_types;

  // element parent
  std::vector<viennagrid_int> parents;

  // explicit element or implicit?
//   std::vector<viennagrid_flag> element_flags;

  // aux_data for elements
//   std::vector<void*> aux_data;

  // region of elements
  std::vector< short_vector<viennagrid_region_id, viennagrid_region_id> > region_buffer;
  ViennaGridRegionBufferType packed_region_buffer;
  bool using_packed_region_buffer;

  // boundary ids
  std::vector<ViennaGridBoundaryBufferType> boundary_ids;

  // element map (based on vertices, using non-ordered boundary-vertices)
  std::vector< short_vector<viennagrid_int, unsigned short> > element_map;

  // base pointer
  viennagrid_mesh_hierarchy_ * mesh_hierarchy;
};













struct viennagrid_mesh_hierarchy_
{
  friend struct viennagrid_element_buffer;

  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  viennagrid_mesh_hierarchy_(viennagrid_mesh root_in);
  ~viennagrid_mesh_hierarchy_();

  viennagrid_mesh root() { return root_; }
  void add_mesh(viennagrid_mesh mesh_)
  {
    meshes_.push_back(mesh_);
  }

  viennagrid_dimension geometric_dimension() const { return geometric_dimension_; }
  void set_geometric_dimension(viennagrid_dimension geometric_dimension_in)
  {
    if (geometric_dimension() == geometric_dimension_in)
      return;

    viennagrid_int vertex_count = 0;
    if (geometric_dimension() != 0)
      vertex_count = vertex_buffer.size() / geometric_dimension();

    geometric_dimension_ = geometric_dimension_in;
    vertex_buffer.resize( geometric_dimension() * vertex_count );
  }

  viennagrid_dimension cell_dimension() const { return cell_dimension_; }
  viennagrid_dimension facet_dimension() const { return cell_dimension()-1; }
  void update_cell_dimension(viennagrid_dimension topologic_dimension)
  {
    if (cell_dimension() == VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION)
      cell_dimension_ = topologic_dimension;

    cell_dimension_ = std::max( cell_dimension_, topologic_dimension );
  }


  viennagrid_element_buffer & element_buffer(viennagrid_int topo_dim)
  { return element_buffers[topo_dim]; }



  viennagrid_int boundary_layout() const { return boundary_layout_; }
  viennagrid_error set_boundary_layout(viennagrid_int boundary_layout_in);
  bool full_boundary_layout() const { return boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_FULL; }



  viennagrid_int * boundary_begin(viennagrid_dimension element_topo_dim,
                                    viennagrid_int element_id,
                                    viennagrid_dimension boundary_topo_dim)
  { return element_buffer(element_topo_dim).boundary_begin(boundary_topo_dim, element_id); }
  viennagrid_int * boundary_end(viennagrid_dimension element_topo_dim,
                                  viennagrid_int element_id,
                                  viennagrid_dimension boundary_topo_dim)
  { return element_buffer(element_topo_dim).boundary_end(boundary_topo_dim, element_id); }


  viennagrid_int * make_boundary_ids(viennagrid_element_type host_type,
                                         viennagrid_element_type boundary_type,
                                         viennagrid_int count)
  {
    return element_buffer(viennagrid_topological_dimension(host_type)).boundary_buffer(viennagrid_topological_dimension(boundary_type)).push_back(count);
  }

  viennagrid_int * make_boundary_ids(viennagrid_element_type host_type,
                                         viennagrid_element_type boundary_type)
  {
    return make_boundary_ids(host_type, boundary_type,
                                 viennagrid_boundary_element_count_from_element_type(host_type, boundary_type));
  }



  viennagrid_int make_vertex(const viennagrid_numeric * coords);
  viennagrid_int make_vertices(const viennagrid_numeric * coords, viennagrid_int vertex_count);


  viennagrid_numeric * get_vertex_pointer()
  {
    return &vertex_buffer[0];
  }

  viennagrid_numeric * get_vertex(viennagrid_int id)
  {
    return &vertex_buffer[id * geometric_dimension()];
  }

  viennagrid_int vertex_count() const
  {
    return vertex_buffer.size() / geometric_dimension();
  }



  viennagrid_int make_elements(viennagrid_int element_count_,
                               viennagrid_element_type * element_types_,
                               viennagrid_int * element_vertex_index_offsets_,
                               viennagrid_int * element_vertex_indices_,
                               viennagrid_region_id * region_ids);

  std::pair<viennagrid_int, bool> get_make_element(viennagrid_element_type element_type,
                                    viennagrid_int * vertex_ids,
                                    viennagrid_int vertex_count,
                                    viennagrid_mesh mesh,
                                    bool make_boundary = true);





  viennagrid_int delete_element_simple(viennagrid_dimension element_topo_dim, viennagrid_int element_id);
  viennagrid_int delete_element(viennagrid_dimension element_topo_dim, viennagrid_int element_id);

  viennagrid_int region_count()
  {
    return regions.size();
  }

  viennagrid_region get_region(viennagrid_region_id region_id)
  {
    if ( (region_id >= (viennagrid_region_id)region_id_mapping.size()) ||
         (region_id == VIENNAGRID_INVALID_REGION_ID) ||
         (region_id_mapping[region_id] == VIENNAGRID_INVALID_REGION_ID) )
    {
      return NULL;
    }
    return regions[ region_id_mapping[region_id] ];
  }

  viennagrid_region get_or_create_region(viennagrid_region_id region_id);

  viennagrid_region create_region()
  {
    if (region_id_mapping.empty())
      return get_or_create_region(0);

    for (viennagrid_int i = 0; i != (viennagrid_int)region_id_mapping.size(); ++i)
    {
      if ( region_id_mapping[i] == VIENNAGRID_INVALID_REGION_ID )
        return get_or_create_region(i);
    }

    return get_or_create_region( region_id_mapping.size() );
  }

  viennagrid_region_id * region_ids_begin() { return &region_ids[0]; }
  viennagrid_region_id * region_ids_end() { return &region_ids[0] + region_ids.size(); }

  bool is_in_region(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_int region_id)
  {
    viennagrid_region_id * it = element_buffer(element_topo_dim).regions_begin(element_id);
    viennagrid_region_id * regions_end = element_buffer(element_topo_dim).regions_end(element_id);

    for (; it != regions_end; ++it)
      if (region_id == *it)
        return true;

    return false;
  }


  bool is_obsolete( viennagrid_int change_counter_to_check ) const { return change_counter_to_check != change_counter_; }
  void update_change_counter( viennagrid_int & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
  void increment_change_counter() { ++change_counter_; }


  viennagrid_int element_count( viennagrid_element_type element_type ) const { return element_counts[+element_type]; }


  void clear();


  viennagrid_int mesh_count() const { return meshes_.size(); }
  viennagrid_mesh mesh(viennagrid_int i) { return meshes_[i]; }


  void optimize_memory();
  long memory_size() const;


private:



  viennagrid_int get_make_line(viennagrid_int * vertex_ids, viennagrid_int vi0, viennagrid_int vi1, viennagrid_mesh mesh)
  {
    viennagrid_int tmp[2] = { vertex_ids[vi0], vertex_ids[vi1] };
    return get_make_element(VIENNAGRID_ELEMENT_TYPE_LINE, tmp, 2, mesh, false).first;
  }

  viennagrid_int get_make_triangle(
        viennagrid_int * vertex_ids, viennagrid_int vi0, viennagrid_int vi1, viennagrid_int vi2,
        viennagrid_int * line_ids, viennagrid_int li0, viennagrid_int li1, viennagrid_int li2,
        viennagrid_mesh mesh)
  {
    viennagrid_int tmp[3] = { vertex_ids[vi0], vertex_ids[vi1], vertex_ids[vi2] };

    std::pair<viennagrid_int, bool> triangle = get_make_element(VIENNAGRID_ELEMENT_TYPE_TRIANGLE, tmp, 3, mesh, false);

    if (triangle.second && full_boundary_layout())
    {
      viennagrid_int * local_line_ids = boundary_begin(2, triangle.first, 1);
      local_line_ids[0] = line_ids[li0];
      local_line_ids[1] = line_ids[li1];
      local_line_ids[2] = line_ids[li2];
    }

    return triangle.first;
  }



  std::pair<viennagrid_int, bool> get_make_element_2(viennagrid_element_type element_type,
                                                     viennagrid_int * vertex_ids,
                                                     viennagrid_int i0, viennagrid_int i1,
                                                     viennagrid_mesh mesh,
                                                     bool make_boundary = true)
  {
    viennagrid_int tmp[2];

    tmp[0] = vertex_ids[i0];
    tmp[1] = vertex_ids[i1];
    return get_make_element(element_type, tmp, 2, mesh, make_boundary);
  }

  std::pair<viennagrid_int, bool> get_make_element_3(viennagrid_element_type element_type,
                                                     viennagrid_int * vertex_ids,
                                                     viennagrid_int i0, viennagrid_int i1, viennagrid_int i2,
                                                     viennagrid_mesh mesh,
                                                     bool make_boundary = true)
  {
    viennagrid_int tmp[3];

    tmp[0] = vertex_ids[i0];
    tmp[1] = vertex_ids[i1];
    tmp[2] = vertex_ids[i2];
    return get_make_element(element_type, tmp, 3, mesh, make_boundary);
  }

  std::pair<viennagrid_int, bool> get_make_element_4(viennagrid_element_type element_type,
                                                     viennagrid_int * vertex_ids,
                                                     viennagrid_int i0, viennagrid_int i1, viennagrid_int i2, viennagrid_int i3,
                                                     viennagrid_mesh mesh,
                                                     bool make_boundary = true)
  {
    viennagrid_int tmp[4];

    tmp[0] = vertex_ids[i0];
    tmp[1] = vertex_ids[i1];
    tmp[2] = vertex_ids[i2];
    tmp[3] = vertex_ids[i3];
    return get_make_element(element_type, tmp, 4, mesh, make_boundary);
  }


  viennagrid_int boundary_layout_;


  viennagrid_element_buffer element_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int element_counts[VIENNAGRID_ELEMENT_TYPE_COUNT];

  viennagrid_dimension geometric_dimension_;
  viennagrid_dimension cell_dimension_;

  std::vector<viennagrid_mesh> meshes_;
  viennagrid_mesh root_;

  std::vector<viennagrid_numeric> vertex_buffer;

  std::vector<viennagrid_region> regions;
  std::vector<viennagrid_region_id> region_ids;
  std::vector<viennagrid_region_id> region_id_mapping;

  viennagrid_int change_counter_;
};


#endif
