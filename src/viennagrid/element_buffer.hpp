#ifndef VIENNAGRID_BACKEND_ELEMENT_BUFFER_HPP
#define VIENNAGRID_BACKEND_ELEMENT_BUFFER_HPP

#include "common.hpp"
#include "buffer.hpp"

class viennagrid_mesh_hierarchy_;
typedef viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;



class viennagrid_element_buffer
{
  friend class viennagrid_mesh_hierarchy_;
  friend struct viennagrid_mesh_;

  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  typedef dense_packed_multibuffer<viennagrid_int, viennagrid_element_id> ViennaGridBoundaryBufferType;
  typedef dense_packed_multibuffer<viennagrid_int, viennagrid_region_id> ViennaGridRegionBufferType;

  viennagrid_element_buffer() : topological_dimension(-1), element_count(0), using_packed_region_buffer(false) {}

  void set_topological_dimension(viennagrid_dimension topological_dimension_in)
  {
    topological_dimension = topological_dimension_in;
    boundary_ids.resize(topological_dimension);
  }


  viennagrid_element_type element_type(viennagrid_element_id element_id) const
  {
    if (topological_dimension == 0)
      return VIENNAGRID_ELEMENT_TYPE_VERTEX;
    return element_types[INDEX(element_id)];
  }

  viennagrid_element_id * boundary_begin(viennagrid_element_id element_id, viennagrid_dimension boundary_topological_dimension)
  {
    return boundary_buffer(boundary_topological_dimension).begin( viennagrid_index_from_element_id(INDEX(element_id)) );
  }

  viennagrid_element_id * boundary_end(viennagrid_element_id element_id, viennagrid_dimension boundary_topological_dimension)
  {
    return boundary_buffer(boundary_topological_dimension).end( viennagrid_index_from_element_id(INDEX(element_id)) );
  }


  viennagrid_element_id parent_id(viennagrid_element_id element_id) const
  {
    if ( (topological_dimension == 0) || (INDEX(element_id) >= (viennagrid_element_id)parents.size()) )
      return -1;

    return parents[INDEX(element_id)];
  }

  void set_parent_id(viennagrid_element_id element_id, viennagrid_element_id parent_id)
  {
    if (topological_dimension != 0)
    {
      if ( INDEX(element_id) >= (viennagrid_element_id)parents.size() )
        parents.resize(INDEX(element_id)+1, -1);

      parents[INDEX(element_id)] = parent_id;
    }
  }


  void * aux(viennagrid_element_id element_id)
  {
    if (aux_data.empty())
      return NULL;

    return aux_data[INDEX(element_id)];
  }

  void set_aux(viennagrid_element_id element_id, void * data)
  {
    if ( INDEX(element_id) >= (viennagrid_element_id)aux_data.size() )
      aux_data.resize( INDEX(element_id)+1, NULL );

    aux_data[INDEX(element_id)] = data;
  }



  void add_to_region(viennagrid_element_id element_id, viennagrid_region_id region_id)
  {
    for (viennagrid_region_id * it = regions_begin(element_id); it != regions_end(element_id); ++it)
    {
      if (*it == region_id)
        return;
    }

    if (using_packed_region_buffer)
    {
      if (INDEX(element_id) >= packed_region_buffer.size())
      {
        for (viennagrid_int i = packed_region_buffer.size(); i != INDEX(element_id)+1; ++i)
          packed_region_buffer.push_back(0);
      }

      packed_region_buffer.add( INDEX(element_id), region_id );
    }
    else
    {
      if ( INDEX(element_id) >= (viennagrid_int)region_buffer.size() )
        region_buffer.resize(INDEX(element_id)+1);

      region_buffer[INDEX(element_id)].push_back( region_id );
    }
  }

  viennagrid_region_id * regions_begin(viennagrid_element_id element_id)
  {
    if (using_packed_region_buffer)
      return packed_region_buffer.begin(INDEX(element_id));
    else
      return (((viennagrid_int)region_buffer.size() <= INDEX(element_id)) || region_buffer[INDEX(element_id)].empty()) ? 0 : &region_buffer[INDEX(element_id)][0];
  }
  viennagrid_region_id * regions_end(viennagrid_element_id element_id)
  {
    if (using_packed_region_buffer)
      return packed_region_buffer.end(INDEX(element_id));
    else
      return (((viennagrid_int)region_buffer.size() <= INDEX(element_id)) || region_buffer[INDEX(element_id)].empty()) ? 0 : &region_buffer[INDEX(element_id)][0] + region_buffer[INDEX(element_id)].size();
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

  viennagrid_element_type * element_types_pointer() { return topological_dimension == 0 ? 0 : &element_types[0]; }
  viennagrid_int * vertex_offsets_pointer() { return boundary_buffer(0).offset_pointer(); }
  viennagrid_element_id * vertex_ids_pointer() { return boundary_buffer(0).values_pointer(); }
  viennagrid_element_id * parent_id_pointer() { return parents.empty() ? 0 : &parents[0]; }
  viennagrid_int parent_id_pointer_size() { return parents.size(); }
  void ** aux_pointer() { return aux_data.empty() ? 0 : &aux_data[0]; }
  viennagrid_int aux_pointer_size() { return aux_data.size(); }

  ViennaGridBoundaryBufferType & boundary_buffer(viennagrid_dimension boundary_topological_dimension)
  {
    return boundary_ids[boundary_topological_dimension];
  }

  void optimize_memory()
  {
    shrink_to_fit(element_types);
    shrink_to_fit(parents);
    shrink_to_fit(aux_data);

    shrink_to_fit(boundary_ids);
    for (std::size_t i = 0; i != boundary_ids.size(); ++i)
      boundary_ids[i].shrink_to_fit();

    shrink_to_fit(element_map);

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

  viennagrid_element_id get_element(viennagrid_element_id * ids,
                                    viennagrid_int id_count,
                                    viennagrid_element_type et)
  {
    viennagrid_element_id * min_vertex_id = std::min_element(ids, ids+id_count);
    assert( min_vertex_id != ids+id_count );

    if (INDEX(*min_vertex_id) >= (viennagrid_int)element_map.size())
      return -1;

    viennagrid_element_id * begin_element_ids = element_map[INDEX(*min_vertex_id)].begin();
    viennagrid_element_id * end_element_ids = element_map[INDEX(*min_vertex_id)].end();

    for (viennagrid_element_id * eit = begin_element_ids; eit != end_element_ids; ++eit)
    {
      viennagrid_element_id * vertex_ids = boundary_begin(*eit, 0);
      viennagrid_element_id vertex_count = boundary_end(*eit, 0)-vertex_ids;

      if ( unsorted_arrays_equal(ids, id_count, vertex_ids, vertex_count) && (element_type(*eit) == et) )
        return *eit;
    }

    return -1;
  }

  viennagrid_element_id make_element(viennagrid_element_type element_type_,
                                     viennagrid_element_id * vertex_ids,
                                     viennagrid_int vertex_count,
                                     bool reserve_boundary);

  viennagrid_element_id make_elements(viennagrid_int element_count_,
                                      viennagrid_element_type * element_types_,
                                      viennagrid_int * element_vertex_index_offsets_,
                                      viennagrid_element_id * element_vertex_indices_);

  viennagrid_element_id make_vertices(viennagrid_int vertex_count);


  void reserve_boundary(viennagrid_element_id element_id);
  void make_boundary(viennagrid_element_id element_id, viennagrid_mesh mesh);


  // the current topological dimension
  viennagrid_dimension topological_dimension;

  // the element count
  viennagrid_int element_count;

  // types of the element
  std::vector<viennagrid_element_type> element_types;

  // element parent
  std::vector<viennagrid_element_id> parents;

  // aux_data for elements
  std::vector<void*> aux_data;

  // region of elements
  std::vector< short_vector<viennagrid_region_id, viennagrid_region_id> > region_buffer;
  ViennaGridRegionBufferType packed_region_buffer;
  bool using_packed_region_buffer;

  // boundary ids
  std::vector<ViennaGridBoundaryBufferType> boundary_ids;

  // element map (based on vertices, using non-ordered boundary-vertices)
  std::vector< short_vector<viennagrid_element_id, unsigned short> > element_map;

  // base pointer
  viennagrid_mesh_hierarchy mesh_hierarchy;
};

#endif
