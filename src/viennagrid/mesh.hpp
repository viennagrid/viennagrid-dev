#ifndef VIENNAGRID_BACKEND_MESH_HPP
#define VIENNAGRID_BACKEND_MESH_HPP

#include <cassert>
#include <set>
#include <algorithm>

#include "viennagrid/viennagrid.h"

#include "common.hpp"
#include "buffer.hpp"
#include "dynamic_sizeof.hpp"
#include "mesh_hierarchy.hpp"


typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_int> ViennaGridCoBoundaryBufferType;
typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_int> ViennaGridNeighborBufferType;
typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_int> ViennaGridElementChildrenBufferType;


struct viennagrid_element_handle_buffer
{
  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  viennagrid_element_handle_buffer() { clear(); }

  bool element_present(viennagrid_int element_id) const
  {
    return find(element_id) != end();
  }

  void add_element(viennagrid_int element_id)
  {
    insert( element_id );
  }

  void add_elements(viennagrid_int start_id, viennagrid_int count)
  {
    if (ids_.empty() || (start_id > ids_.back()))
    {
      viennagrid_int old_size = ids_.size();
      ids_.resize( ids_.size() + count );
      for (viennagrid_int i = 0; i != count; ++i)
        ids_[ old_size+i ] = start_id+i;
    }
    else
    {
      // TODO implement
      assert(false);
    }
  }

  viennagrid_int * ids()
  { return ids_.empty() ? 0 : &ids_[0]; }
  viennagrid_int count() const { return ids_.size(); }

  ViennaGridCoBoundaryBufferType & coboundary_buffer(viennagrid_dimension coboundary_topo_dim)
  { return coboundary_ids[+coboundary_topo_dim]; }
  ViennaGridNeighborBufferType & neighbor_buffer(viennagrid_dimension connector_topo_dim,
                                               viennagrid_dimension neighbor_topo_dim)
  { return neighbor_ids[+connector_topo_dim][+neighbor_topo_dim]; }

  void clear()
  {
    ids_.clear();

    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      coboundary_ids[i].clear();

    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      for (int j = 0; j != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++j)
        neighbor_ids[i][j].clear();
  }

  void optimize_memory()
  {
    shrink_to_fit( ids_ );
  }

private:

  std::vector<viennagrid_int> ids_;

  typedef std::vector<viennagrid_int>::iterator iterator;
  typedef std::vector<viennagrid_int>::const_iterator const_iterator;

  iterator begin() { return ids_.begin(); }
  iterator end() { return ids_.end(); }

  const_iterator begin() const { return ids_.begin(); }
  const_iterator end() const { return ids_.end(); }

  iterator find(viennagrid_int id)
  {
    if (ids_.empty() || (id > ids_.back()))
      return end();

    iterator it = std::lower_bound( begin(), end(), id );
    if ( (it != end()) && (*it) == id )
      return it;
    else
      return end();
  }

  const_iterator find(viennagrid_int id) const
  {
    if (ids_.empty() || (id > ids_.back()))
      return end();

    const_iterator it = std::lower_bound(begin(), end(), id);
    if ( (it != end()) && (*it) == id )
      return it;
    else
      return end();
  }

  void insert(viennagrid_int id)
  {
    if (ids_.empty() || (id > ids_.back()))
      ids_.push_back(id);
    else
    {
      iterator it = std::lower_bound(begin(), end(), id);
      if ( (*it) != id )
        ids_.insert(it, id);
    }
  }


  ViennaGridCoBoundaryBufferType coboundary_ids[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  ViennaGridNeighborBufferType neighbor_ids[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];
};


struct viennagrid_element_children_
{
public:


  viennagrid_int & change_counter(viennagrid_int element_topo_dim) { return change_counters[element_topo_dim]; }

  ViennaGridElementChildrenBufferType & children_ids_buffer(viennagrid_int element_topo_dim) { return children_ids_buffers[element_topo_dim]; }

  ViennaGridElementChildrenBufferType const & get_children_ids_buffer(int i) const { return children_ids_buffers[i]; }

private:
  ViennaGridElementChildrenBufferType children_ids_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int change_counters[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
};





struct viennagrid_mesh_ : public reference_counted
{
  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;

public:
  viennagrid_mesh_();
  viennagrid_mesh_(viennagrid_mesh_hierarchy_ * hierarchy_in, viennagrid_mesh parent_in);
  ~viennagrid_mesh_();

  viennagrid_mesh_hierarchy_ * mesh_hierarchy() { return hierarchy_; }
  viennagrid_mesh parent() { return parent_; }

  bool is_root() const;

  viennagrid_mesh make_child();

  viennagrid_int children_count() { return children.size(); }
  viennagrid_int total_children_count()
  {
    viennagrid_int count = 0;
    for (viennagrid_int i = 0; i != children_count(); ++i)
      count += get_child(i)->total_children_count();
    count += children_count();
    return count;
  }

  viennagrid_mesh get_child(viennagrid_int id) { return children[id]; }


  viennagrid_dimension geometric_dimension();
  viennagrid_dimension cell_dimension();

  viennagrid_int element_count(viennagrid_dimension element_topo_dim)
  { return element_handle_buffer(element_topo_dim).count(); }

  viennagrid_int * elements_begin(viennagrid_dimension element_topo_dim)
  { return element_handle_buffer(element_topo_dim).ids(); }
  viennagrid_int * elements_end(viennagrid_dimension element_topo_dim)
  { return elements_begin(element_topo_dim) + element_count(element_topo_dim); }


  viennagrid_int * boundary_begin(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_dimension boundary_topo_dim);
  viennagrid_int * boundary_end(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_dimension boundary_topo_dim);

  viennagrid_int * coboundary_begin(viennagrid_dimension element_type, viennagrid_int element_id, viennagrid_dimension coboundary_type)
  {
    make_coboundary(element_type, coboundary_type);
    return element_handle_buffer(element_type).coboundary_buffer(coboundary_type).begin(element_id);
  }
  viennagrid_int * coboundary_end(viennagrid_dimension element_type, viennagrid_int element_id, viennagrid_dimension coboundary_type)
  {
    make_coboundary(element_type, coboundary_type);
    return element_handle_buffer(element_type).coboundary_buffer(coboundary_type).end(element_id);
  }



  viennagrid_int * neighbor_begin(viennagrid_dimension element_type, viennagrid_int element_id, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type)
  {
    make_neighbor(element_type, connector_type, neighbor_type);
    return element_handle_buffer(element_type).neighbor_buffer(connector_type, neighbor_type).begin(element_id);
  }
  viennagrid_int * neighbor_end(viennagrid_dimension element_type, viennagrid_int element_id, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type)
  {
    make_neighbor(element_type, connector_type, neighbor_type);
    return element_handle_buffer(element_type).neighbor_buffer(connector_type, neighbor_type).end(element_id);
  }




  viennagrid_int * connected_begin(viennagrid_dimension element_type, viennagrid_int element_id, viennagrid_dimension connector_type)
  {
    assert( element_type != connector_type );
    if (element_type > connector_type)
      return boundary_begin(element_type, element_id, connector_type);
    else
      return coboundary_begin(element_type, element_id, connector_type);
  }

  viennagrid_int * connected_end(viennagrid_dimension element_type, viennagrid_int element_id, viennagrid_dimension connector_type)
  {
    assert( element_type != connector_type );
    if (element_type > connector_type)
      return boundary_end(element_type, element_id, connector_type);
    else
      return coboundary_end(element_type, element_id, connector_type);
  }


  void add_elements(viennagrid_dimension element_topo_dim,
                    viennagrid_int first_id, viennagrid_int count);

  void add_element(viennagrid_dimension element_topo_dim,
                   viennagrid_int element_id);

  viennagrid_int make_refined_element(
    viennagrid_dimension      element_topo_dim,
    viennagrid_int          element_id,
    viennagrid_element_type    refined_element_type,
    viennagrid_int            refined_element_refined_count,
    viennagrid_int *        refined_element_refined_ids,
    viennagrid_int            intersects_count,
    viennagrid_int *        intersect_vertices_ids,
    viennagrid_int *        intersects_ids,
    viennagrid_dimension *    intersects_topo_dims);


  void make_coboundary(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void make_neighbor(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim);


  viennagrid_element_handle_buffer & element_handle_buffer(viennagrid_dimension topo_dim)
  { return element_handle_buffers[+topo_dim]; }



  bool is_coboundary_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void set_coboundary_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);

  bool is_neighbor_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type);
  void set_neighbor_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type);


  bool is_boundary(viennagrid_dimension element_topo_dim, viennagrid_int element_id)
  { return boundary_elements(element_topo_dim).find(element_id) != boundary_elements(element_topo_dim).end(); }

  void set_boundary(viennagrid_dimension element_topo_dim, viennagrid_int element_id)
  { boundary_elements(element_topo_dim).insert(element_id); }

  void clear_boundary(viennagrid_dimension element_topo_dim)
  { boundary_elements(element_topo_dim).clear(); }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();

  void make_boundary_flags(viennagrid_region region);
  void make_boundary_flags();


  void make_element_children(viennagrid_mesh child, viennagrid_int element_topo_dim);

  void clear();


  void set_name(std::string const & name_in)
  {
    name_ = name_in;
  }

  std::string name() const
  {
    return name_;
  }

  void optimize_memory()
  {
    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      element_handle_buffers[i].optimize_memory();
  }


private:

  viennagrid_mesh_hierarchy_ * hierarchy_;

  viennagrid_mesh parent_;
  std::vector<viennagrid_mesh> children;
  std::vector<viennagrid_element_children_> element_children;
  std::map<viennagrid_mesh, viennagrid_int> mesh_children_map;

  std::string name_;


  viennagrid_int child_mesh_id(viennagrid_mesh child_mesh)
  {
    std::map<viennagrid_mesh, viennagrid_int>::const_iterator it = mesh_children_map.find(child_mesh);
    assert( it != mesh_children_map.end() );
    return it->second;
  }

  viennagrid_int & element_children_change_counter(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim)
  { return element_children[child_mesh_id_].change_counter(element_topo_dim); }
  viennagrid_int & element_children_change_counter(viennagrid_mesh child_mesh, viennagrid_int element_topo_dim)
  { return element_children_change_counter(child_mesh_id(child_mesh),element_topo_dim); }

  bool is_element_children_obsolete(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim);
  void set_element_children_uptodate(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim);

  bool is_element_children_obsolete(viennagrid_mesh child_mesh, viennagrid_int element_topo_dim)
  { return is_element_children_obsolete(child_mesh_id(child_mesh), element_topo_dim); }
  void set_element_children_uptodate(viennagrid_mesh child_mesh, viennagrid_int element_topo_dim)
  { set_element_children_uptodate(child_mesh_id(child_mesh), element_topo_dim); }





  viennagrid_element_handle_buffer element_handle_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];


  viennagrid_int & coboundary_change_counter(viennagrid_dimension element_topo_dim,
                                             viennagrid_dimension coboundary_topo_dim)
  { return coboundary_change_counters[+element_topo_dim][+coboundary_topo_dim]; }
  viennagrid_int coboundary_change_counters[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];


  viennagrid_int & neighbor_change_counter(viennagrid_dimension element_topo_dim,
                                           viennagrid_dimension connector_topo_dim,
                                           viennagrid_dimension neighbor_topo_dim)
  { return neighbor_change_counters[+element_topo_dim][+connector_topo_dim][+neighbor_topo_dim]; }
  viennagrid_int neighbor_change_counters[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];



  std::set<viennagrid_int> & boundary_elements(viennagrid_dimension element_topo_dim) { return boundary_elements_[+element_topo_dim]; }

  std::set<viennagrid_int> boundary_elements_[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int boundary_elements_change_counter;
};

#endif
