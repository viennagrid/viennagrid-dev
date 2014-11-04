#ifndef VIENNAGRID_BACKEND_HPP
#define VIENNAGRID_BACKEND_HPP

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <algorithm>

#include <cstring>
#include <cassert>


#include "viennagrid/viennagrid.h"
#include "viennagrid/backend/buffer.hpp"




typedef sparse_multibuffer<viennagrid_index, viennagrid_index> viennagrid_coboundary_buffer;
typedef dense_multibuffer<viennagrid_index, viennagrid_index> viennagrid_boundary_buffer;


struct viennagrid_element_handle_buffer
{
public:

  void add_element(viennagrid_index element_id)
  {
    std::map<viennagrid_index, viennagrid_index>::iterator it = index_map.find(element_id);
    if (it == index_map.end())
    {
      index_map[element_id] = indices.size();
      indices.push_back(element_id);
    }
  }

  bool remove_element(viennagrid_index element_id)
  {
    std::map<viennagrid_index, viennagrid_index>::iterator it = index_map.find(element_id);
    if (it != index_map.end())
    {
      for (std::map<viennagrid_index, viennagrid_index>::iterator jt = index_map.begin(); jt != index_map.end(); ++jt)
      {
        if (jt->second > it->second)
          --jt->second;
      }

      indices.erase( indices.begin()+it->second );
      index_map.erase( it );

      return true;
    }

    return false;
  }

  viennagrid_index * ids() { return &indices[0]; }
  viennagrid_index const * ids() const { return &indices[0]; }

  viennagrid_int count() const { return indices.size(); }
  viennagrid_coboundary_buffer & coboundary_buffer(viennagrid_element_tag coboundary_tag)
  { return coboundary_indices[coboundary_tag]; }
  viennagrid_coboundary_buffer const & coboundary_buffer(viennagrid_element_tag coboundary_tag) const
  { return coboundary_indices[coboundary_tag]; }

private:
  std::vector<viennagrid_index> indices;
  viennagrid_coboundary_buffer coboundary_indices[VIENNAGRID_ELEMENT_TAG_COUNT];
  std::map<viennagrid_index, viennagrid_index> index_map;
};


struct viennagrid_mesh
{
public:
  viennagrid_mesh(viennagrid_mesh_hierarchy_handle hierarchy_in) : hierarchy_(hierarchy_in), parent_(0), vertex_buffer_in_use(false) {}
  viennagrid_mesh(viennagrid_mesh_hierarchy_handle hierarchy_in, viennagrid_mesh_handle parent_in) : hierarchy_(hierarchy_in), parent_(parent_in), vertex_buffer_in_use(false) {}

  ~viennagrid_mesh()
  {
    for (ChildrenContainerType::iterator it = children.begin(); it != children.end(); ++it)
      delete *it;
  }

  viennagrid_mesh_hierarchy_handle mesh_hierarchy() { return hierarchy_; }
  viennagrid_mesh_hierarchy_const_handle mesh_hierarchy() const { return hierarchy_; }
  viennagrid_mesh_handle parent() { return parent_; }
  viennagrid_mesh_const_handle parent() const { return parent_; }

  viennagrid_mesh_handle make_child()
  {
    viennagrid_mesh_handle mesh = new viennagrid_mesh( mesh_hierarchy(), this );
    children.push_back( mesh );
    return mesh;
  }

  viennagrid_int children_count() { return children.size(); }
  viennagrid_mesh_handle get_child(viennagrid_int id) { return children[id]; }

  void make_vertex_buffer()
  {
    if (!vertex_buffer_in_use)
    {
      vertex_buffer_in_use = true;

      viennagrid_mesh_handle mesh = parent();
      while (mesh && !mesh->vertex_buffer_in_use)
        mesh = mesh->parent();

      if (mesh)
        vertex_buffer = mesh->vertex_buffer;
    }
  }
  void delete_vertex_buffer()
  {
    vertex_buffer.clear();
    vertex_buffer_in_use = false;
  }

  viennagrid_index make_vertex(const viennagrid_numeric * coords)
  {
    if (vertex_buffer_in_use)
    {
      viennagrid_int prev_size = vertex_buffer.size();
      vertex_buffer.resize( vertex_buffer.size() + dimension() );

      viennagrid_numeric * end = &vertex_buffer[0] + prev_size;
      std::copy( coords, coords+dimension(), end );
    }

    for (ChildrenContainerType::iterator it = children.begin(); it != children.end(); ++it)
      (*it)->make_vertex(coords);

    return vertex_buffer.size()/dimension()-1;
  }

  viennagrid_numeric * get_vertex(viennagrid_int id)
  {
    if (!vertex_buffer_in_use)
      return parent()->get_vertex(id);

    return &vertex_buffer[id * dimension()];
  }

  viennagrid_numeric const * get_vertex(viennagrid_int id) const
  {
    if (!vertex_buffer_in_use)
      return parent()->get_vertex(id);

    return &vertex_buffer[id * dimension()];
  }

  viennagrid_int dimension() const;

  viennagrid_int element_count(viennagrid_element_tag element_tag) const
  { return element_handle_buffer(element_tag).count(); }

  viennagrid_index * elements_begin(viennagrid_element_tag element_tag)
  { return element_handle_buffer(element_tag).ids(); }
  viennagrid_index * elements_end(viennagrid_element_tag element_tag)
  { return elements_begin(element_tag) + element_count(element_tag); }

  viennagrid_index const * elements_begin(viennagrid_element_tag element_tag) const
  { return element_handle_buffer(element_tag).ids(); }
  viennagrid_index const * elements_end(viennagrid_element_tag element_tag) const
  { return elements_begin(element_tag) + element_count(element_tag); }


  viennagrid_index * coboundary_begin(viennagrid_element_tag element_tag, viennagrid_index element_id, viennagrid_element_tag coboundary_tag)
  { return element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag).begin(element_id); }
  viennagrid_index * coboundary_end(viennagrid_element_tag element_tag, viennagrid_index element_id, viennagrid_element_tag coboundary_tag)
  { return element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag).end(element_id); }

  viennagrid_index const * coboundary_begin(viennagrid_element_tag element_tag, viennagrid_index element_id, viennagrid_element_tag coboundary_tag) const
  { return element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag).begin(element_id); }
  viennagrid_index const * coboundary_end(viennagrid_element_tag element_tag, viennagrid_index element_id, viennagrid_element_tag coboundary_tag) const
  { return element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag).end(element_id); }


  void add_element(viennagrid_element_tag element_tag,
                   viennagrid_index element_id)
  { element_handle_buffer(element_tag).add_element(element_id); }


  void make_coboundary(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag);


  viennagrid_element_handle_buffer & element_handle_buffer(viennagrid_element_tag element_tag)
  { return element_handle_buffers[ element_tag ]; }
  viennagrid_element_handle_buffer const & element_handle_buffer(viennagrid_element_tag element_tag) const
  { return element_handle_buffers[ element_tag ]; }


  viennagrid_element_tag unpack_element_tag(viennagrid_element_tag et) const;


  bool is_coboundary_obsolete(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag);
  void set_coboundary_uptodate(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag);



  bool is_boundary(viennagrid_element_tag element_tag, viennagrid_index element_id) const
  { return boundary_elements[element_tag].find(element_id) != boundary_elements[element_tag].end(); }

  void set_boundary(viennagrid_element_tag element_tag, viennagrid_index element_id)
  { boundary_elements[element_tag].insert(element_id); }

  void clear_boundary(viennagrid_element_tag element_tag)
  { boundary_elements[element_tag].clear(); }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();

  void make_boundary_flags(viennagrid_region_handle region);
  void make_boundary_flags();



private:

  viennagrid_mesh_hierarchy_handle hierarchy_;

  typedef std::vector<viennagrid_mesh_handle> ChildrenContainerType;
  viennagrid_mesh_handle parent_;
  ChildrenContainerType children;

  bool vertex_buffer_in_use;
  std::vector<viennagrid_numeric> vertex_buffer;

  viennagrid_element_handle_buffer element_handle_buffers[VIENNAGRID_ELEMENT_TAG_COUNT];


  viennagrid_int & coboundary_change_counter(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag)
  {
    std::map< std::pair<viennagrid_element_tag, viennagrid_element_tag>, viennagrid_int >::iterator it = coboundary_change_counters.find( std::make_pair(element_tag, coboundary_tag) );
    if (it != coboundary_change_counters.end())
      return it->second;

    it = coboundary_change_counters.insert( std::make_pair(std::make_pair(element_tag, coboundary_tag), viennagrid_int(0)) ).first;
    return it->second;
  }

  std::map< std::pair<viennagrid_element_tag, viennagrid_element_tag>, viennagrid_int > coboundary_change_counters;



  std::set<viennagrid_index> boundary_elements[VIENNAGRID_ELEMENT_TAG_COUNT];
  viennagrid_index boundary_elements_change_counter;
};



struct element_key
{
  element_key() {}
  element_key(viennagrid_index * indices, viennagrid_int index_count) : vertex_indices(index_count)
  {
    std::copy( indices, indices+index_count, &vertex_indices[0] );
    std::sort(vertex_indices.begin(), vertex_indices.end());
  }

  bool operator<(element_key const & rhs) const
  {
    assert( vertex_indices.size() == rhs.vertex_indices.size() );

    for (std::size_t i=0; i < vertex_indices.size(); ++i)
    {
      if ( vertex_indices[i] > rhs.vertex_indices[i] )
        return false;
      else if ( vertex_indices[i] < rhs.vertex_indices[i] )
        return true;
    }
    return false;
  }

  std::vector<viennagrid_index> vertex_indices;
};








struct viennagrid_element_buffer
{
  friend struct viennagrid_mesh_hierarchy;
public:

  viennagrid_element_buffer() : element_tag(VIENNAGRID_ELEMENT_TAG_NO_ELEMENT) {}

  void set_tag(viennagrid_element_tag element_tag_)
  {
    element_tag = element_tag_;
    boundary_indices.resize( viennagrid_boundary_element_tag_count(element_tag_) );
  }


  viennagrid_index * boundary_indices_begin(viennagrid_element_tag boundary_tag, viennagrid_index element_id)
  { return boundary_buffer(boundary_tag).begin(element_id); }
  viennagrid_index * boundary_indices_end(viennagrid_element_tag boundary_tag, viennagrid_index element_id)
  { return boundary_buffer(boundary_tag).end(element_id); }


  viennagrid_index parend_id(viennagrid_index element_id) const { return parents[element_id]; }

  void add_to_region(viennagrid_index element_id, viennagrid_region_handle region_)
  {
    for (viennagrid_region_handle * it = region_buffer.begin(element_id); it != region_buffer.end(element_id); ++it)
    {
      if (*it == region_)
        return;
    }

    region_buffer.push_back(element_id, region_);
  }

  viennagrid_region_handle * regions_begin(viennagrid_index element_id) { return region_buffer.begin(element_id); }
  viennagrid_region_handle * regions_end(viennagrid_index element_id) { return region_buffer.end(element_id); }

  viennagrid_region_const_handle const * regions_cbegin(viennagrid_index element_id) const { return region_buffer.begin(element_id); }
  viennagrid_region_const_handle const * regions_cend(viennagrid_index element_id) const { return region_buffer.end(element_id); }

  viennagrid_region_const_handle const * regions_begin(viennagrid_index element_id) const { return regions_cbegin(element_id); }
  viennagrid_region_const_handle const * regions_end(viennagrid_index element_id) const { return regions_cend(element_id); }


  viennagrid_boundary_buffer & boundary_buffer(viennagrid_element_tag boundary_tag)
  { return boundary_indices[boundary_index(boundary_tag)]; }
  viennagrid_boundary_buffer const & boundary_buffer(viennagrid_element_tag boundary_tag) const
  { return boundary_indices[boundary_index(boundary_tag)]; }

private:



  viennagrid_index get_element(element_key const & key)
  {
    std::map<element_key, viennagrid_index>::iterator it = element_map.find(key);
    if (it == element_map.end())
      return -1;

    return it->second;
  }

  viennagrid_index get_element(viennagrid_index * vertex_indices, viennagrid_int index_count)
  {
    return get_element( element_key(vertex_indices, index_count) );
  }

  viennagrid_index make_element(viennagrid_index * vertex_indices, viennagrid_int index_count)
  {
    viennagrid_index id = size();

    parents.push_back(-1);
    region_buffer.push_back(0);

    if (vertex_indices)
    {
      viennagrid_index * ptr = boundary_buffer(VIENNAGRID_ELEMENT_TAG_VERTEX).push_back( index_count );
      std::copy( vertex_indices, vertex_indices+index_count, ptr );

      element_map[ element_key(vertex_indices, index_count) ] = id;
    }

    return id;
  }

  viennagrid_index size() const { return parents.size(); }
  viennagrid_index boundary_index(viennagrid_element_tag boundary_tag) const
  { return viennagrid_boundary_buffer_index_from_element_tag(element_tag, boundary_tag); }


  viennagrid_element_tag element_tag;

  std::vector<viennagrid_index> parents;
  dense_multibuffer<viennagrid_index, viennagrid_region_handle, viennagrid_region_handle *, viennagrid_region_const_handle const *> region_buffer;
  std::vector<viennagrid_boundary_buffer> boundary_indices;
  std::map<element_key, viennagrid_index> element_map;
};





struct viennagrid_region
{
  friend struct viennagrid_mesh_hierarchy;

public:

  viennagrid_region(viennagrid_index id_in, viennagrid_mesh_hierarchy_handle hierarchy_in) : id_(id_in), boundary_elements_change_counter(0), hierarchy_(hierarchy_in) {}

  viennagrid_mesh_hierarchy_handle mesh_hierarchy() { return hierarchy_; }
  viennagrid_mesh_hierarchy_const_handle mesh_hierarchy() const { return hierarchy_; }

  void set_name(std::string const & name_in) { name_= name_in; }
  std::string const & name() const { return name_; }

  bool is_boundary(viennagrid_element_tag element_tag, viennagrid_index element_id) const
  { return boundary_elements[element_tag].find(element_id) != boundary_elements[element_tag].end(); }

  void set_boundary(viennagrid_element_tag element_tag, viennagrid_index element_id)
  { boundary_elements[element_tag].insert(element_id); }

  void clear_boundary(viennagrid_element_tag element_tag)
  { boundary_elements[element_tag].clear(); }

  viennagrid_index id() const { return id_; }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();



private:

  std::string name_;
  viennagrid_index id_;

  std::set<viennagrid_index> boundary_elements[VIENNAGRID_ELEMENT_TAG_COUNT];
  viennagrid_index boundary_elements_change_counter;

  viennagrid_mesh_hierarchy_handle hierarchy_;
};


struct viennagrid_mesh_hierarchy
{
public:
  viennagrid_mesh_hierarchy(viennagrid_int dimension_in, viennagrid_element_tag cell_tag) : dimension_(dimension_in), cell_tag_(cell_tag), root_( new viennagrid_mesh(this) ), highest_region_id(0), change_counter_(0), use_count_(1)
  {
    root_->make_vertex_buffer();
    for (viennagrid_int et = VIENNAGRID_ELEMENT_TAG_START; et < VIENNAGRID_ELEMENT_TAG_COUNT; ++et)
      element_buffer(et).set_tag(et);
  }
  ~viennagrid_mesh_hierarchy()
  {
    delete root_;

    for (std::vector<viennagrid_region_handle>::iterator it = regions.begin(); it != regions.end(); ++it)
      delete *it;
  }

  viennagrid_mesh_handle root() { return root_; }
  viennagrid_mesh_const_handle root() const { return root_; }
  viennagrid_int dimension() const { return dimension_; }
  viennagrid_element_tag cell_tag() const { return cell_tag_; }


  viennagrid_element_buffer & element_buffer(viennagrid_element_tag element_tag)
  { return element_buffers[ element_tag ]; }
  viennagrid_element_buffer const & element_buffer(viennagrid_element_tag element_tag) const
  { return element_buffers[ element_tag ]; }



  viennagrid_index * make_boundary_indices(viennagrid_element_tag host_tag, viennagrid_element_tag boundary_tag)
  {
    return element_buffer(host_tag).boundary_buffer(boundary_tag).push_back( viennagrid_boundary_element_count_from_element_tag(host_tag, boundary_tag) );
  }

  viennagrid_index make_vertex(const viennagrid_numeric * coords)
  {
    viennagrid_index id = element_buffer(VIENNAGRID_ELEMENT_TAG_VERTEX).make_element(0, 0);
    root_->make_vertex(coords);
    return id;
  }

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int vertex_count);

  viennagrid_region_const_handle get_region(viennagrid_index region_id) const
  {
    std::map<viennagrid_index, viennagrid_index>::const_iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];
    return 0;
  }

  viennagrid_region_handle get_make_region(viennagrid_index region_id)
  {
    std::map<viennagrid_index, viennagrid_index>::iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];

    regions.push_back( new viennagrid_region(region_id, this) );
    region_id_map[region_id] = regions.size()-1;

    return regions.back();
  }

  viennagrid_region_handle make_region()
  {
    return get_make_region(highest_region_id++);
  }

  viennagrid_region_handle * regions_begin() { return &regions[0]; }
  viennagrid_region_handle * regions_end() { return &regions[0] + regions.size(); }

  viennagrid_region_const_handle const * regions_cbegin() const { return &regions[0]; }
  viennagrid_region_const_handle const * regions_cend() const { return &regions[0] + regions.size(); }

  viennagrid_region_const_handle const * regions_begin() const { return regions_cbegin(); }
  viennagrid_region_const_handle const * regions_end() const { return regions_cend(); }


  bool is_in_region(viennagrid_element_tag element_tag, viennagrid_index element_id, viennagrid_index region_id) const
  {
    viennagrid_region_const_handle const * it = element_buffer(element_tag).regions_begin(element_id);
    viennagrid_region_const_handle const * regions_end = element_buffer(element_tag).regions_end(element_id);

    for (; it != regions_end; ++it)
      if (region_id == (*it)->id())
        return true;

    return false;
  }

  viennagrid_element_tag unpack_element_tag(viennagrid_element_tag et) const
  {
    if (et == VIENNAGRID_ELEMENT_TAG_CELL)
      return cell_tag();
    else if (et == VIENNAGRID_ELEMENT_TAG_FACET)
      return viennagrid_facet_tag(cell_tag());
    else
      return et;
  }


  bool is_obsolete( viennagrid_int change_counter_to_check ) const { return change_counter_to_check != change_counter_; }
  void update_change_counter( viennagrid_int & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
  void increment_change_counter() { ++change_counter_; }




  void retain() const
  {
    ++use_count_;
  }
  bool release() const
  {
    if (--use_count_ <= 0)
    {
      delete this;
      return false;
    }

    return true;
  }

private:

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1)
  {
    viennagrid_int tmp[2];
    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    return get_make_element(element_tag, tmp, 2);
  }

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1, viennagrid_int i2)
  {
    viennagrid_int tmp[3];
    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    tmp[2] = vertex_indices[i2];
    return get_make_element(element_tag, tmp, 3);
  }

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1, viennagrid_int i2, viennagrid_int i3)
  {
    viennagrid_int tmp[4];
    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    tmp[2] = vertex_indices[i2];
    tmp[3] = vertex_indices[i3];
    return get_make_element(element_tag, tmp, 4);
  }



  viennagrid_element_buffer element_buffers[VIENNAGRID_ELEMENT_TAG_COUNT];

  viennagrid_int dimension_;
  viennagrid_element_tag cell_tag_;
  viennagrid_mesh_handle root_;


  std::vector<viennagrid_region_handle> regions;
  viennagrid_index highest_region_id;
  std::map<viennagrid_index, viennagrid_index> region_id_map;

  viennagrid_int change_counter_;
  mutable viennagrid_int use_count_;
};


inline viennagrid_int viennagrid_mesh::dimension() const { return hierarchy_->dimension(); }
inline viennagrid_element_tag viennagrid_mesh::unpack_element_tag(viennagrid_element_tag et) const
{ return mesh_hierarchy()->unpack_element_tag(et); }

inline bool viennagrid_mesh::is_coboundary_obsolete(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag)
{ return mesh_hierarchy()->is_obsolete( coboundary_change_counter(element_tag, coboundary_tag) ); }
inline void viennagrid_mesh::set_coboundary_uptodate(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag)
{ mesh_hierarchy()->update_change_counter( coboundary_change_counter(element_tag, coboundary_tag) ); }


inline bool viennagrid_mesh::are_boundary_flags_obsolete()
{ return hierarchy_->is_obsolete( boundary_elements_change_counter ); }
inline void viennagrid_mesh::set_boundary_flags_uptodate()
{ hierarchy_->update_change_counter( boundary_elements_change_counter ); }


inline bool viennagrid_region::are_boundary_flags_obsolete()
{ return mesh_hierarchy()->is_obsolete( boundary_elements_change_counter ); }
inline void viennagrid_region::set_boundary_flags_uptodate()
{ mesh_hierarchy()->update_change_counter( boundary_elements_change_counter ); }



#endif
