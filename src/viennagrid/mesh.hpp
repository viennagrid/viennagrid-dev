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


typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_element_id> ViennaGridCoBoundaryBufferType;
typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_element_id> ViennaGridNeighborBufferType;
typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_element_id> ViennaGridElementChildrenBufferType;


struct viennagrid_element_handle_buffer
{
  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  viennagrid_element_handle_buffer() { clear(-1); }

  bool element_present(viennagrid_element_id element_id) const
  {
    return find(element_id) != end();
  }

  void add_element(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id element_id)
  {
    insert( mesh_hierarchy, element_id );
  }

  void add_elements(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id start_id, viennagrid_int count)
  {
    if (ids_.empty() || (start_id > ids_.back()))
    {
      viennagrid_int old_size = ids_.size();
      ids_.resize( ids_.size() + count );
      for (viennagrid_int i = 0; i != count; ++i)
      {
        ids_[ old_size+i ] = start_id+i;
        element_counts[ +mesh_hierarchy->element_buffer(topologic_dimension).element_type(start_id+i) ]++;
      }
    }
    else
    {
      // TODO implement
      assert(false);
    }
  }

  viennagrid_int * ids() { return ids_.empty() ? 0 : &ids_[0]; }
  viennagrid_int count() const { return ids_.size(); }

  viennagrid_int element_count(viennagrid_element_type et) const { return element_counts[+et]; }

  ViennaGridCoBoundaryBufferType & coboundary_buffer(viennagrid_dimension coboundary_topo_dim)
  { return coboundary_ids[+coboundary_topo_dim]; }
  ViennaGridNeighborBufferType & neighbor_buffer(viennagrid_dimension connector_topo_dim,
                                               viennagrid_dimension neighbor_topo_dim)
  { return neighbor_ids[+connector_topo_dim][+neighbor_topo_dim]; }

  void clear(viennagrid_dimension topologic_dimension_)
  {
    topologic_dimension = topologic_dimension_;

    ids_.clear();

    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      coboundary_ids[i].clear();

    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      for (int j = 0; j != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++j)
        neighbor_ids[i][j].clear();

    for (int i = 0; i != VIENNAGRID_ELEMENT_TYPE_COUNT; ++i)
      element_counts[i] = 0;
  }

  void optimize_memory()
  {
    shrink_to_fit( ids_ );
  }

private:

  std::vector<viennagrid_element_id> ids_;

  typedef std::vector<viennagrid_element_id>::iterator iterator;
  typedef std::vector<viennagrid_element_id>::const_iterator const_iterator;

  iterator begin() { return ids_.begin(); }
  iterator end() { return ids_.end(); }

  const_iterator begin() const { return ids_.begin(); }
  const_iterator end() const { return ids_.end(); }

  iterator find(viennagrid_element_id id)
  {
    if (ids_.empty() || (id > ids_.back()))
      return end();

    iterator it = std::lower_bound( begin(), end(), id );
    if ( (it != end()) && (*it) == id )
      return it;
    else
      return end();
  }

  const_iterator find(viennagrid_element_id id) const
  {
    if (ids_.empty() || (id > ids_.back()))
      return end();

    const_iterator it = std::lower_bound(begin(), end(), id);
    if ( (it != end()) && (*it) == id )
      return it;
    else
      return end();
  }

  void insert(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id id)
  {
    if (ids_.empty() || (id > ids_.back()))
    {
      ids_.push_back(id);
      element_counts[ +mesh_hierarchy->element_buffer(topologic_dimension).element_type(id) ]++;
    }
    else
    {
      iterator it = std::lower_bound(begin(), end(), id);
      if ( (*it) != id )
      {
        ids_.insert(it, id);
        element_counts[ +mesh_hierarchy->element_buffer(topologic_dimension).element_type(id) ]++;
      }
    }
  }

  viennagrid_dimension topologic_dimension;

  ViennaGridCoBoundaryBufferType coboundary_ids[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  ViennaGridNeighborBufferType neighbor_ids[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];

  viennagrid_int element_counts[VIENNAGRID_ELEMENT_TYPE_COUNT];
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
  viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in, viennagrid_mesh parent_in);
  ~viennagrid_mesh_();

  viennagrid_mesh_hierarchy mesh_hierarchy() { return hierarchy_; }
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

  viennagrid_mesh get_child(viennagrid_int mesh_index) { return children[mesh_index]; }


  viennagrid_dimension geometric_dimension();
  viennagrid_dimension cell_dimension();

  viennagrid_int element_count(viennagrid_dimension element_topo_dim)
  { return element_handle_buffer(element_topo_dim).count(); }
  viennagrid_int element_count_by_type(viennagrid_element_type element_type)
  { return element_handle_buffer( viennagrid_topological_dimension(element_type) ).element_count(element_type); }

  viennagrid_element_id * elements_begin(viennagrid_dimension element_topo_dim)
  { return element_handle_buffer(element_topo_dim).ids(); }
  viennagrid_element_id * elements_end(viennagrid_dimension element_topo_dim)
  { return elements_begin(element_topo_dim) + element_count(element_topo_dim); }



  viennagrid_element_id * connected_begin(viennagrid_element_id element_id, viennagrid_dimension connector_topo_dim)
  {
    assert( TOPODIM(element_id) != connector_topo_dim );
    if (TOPODIM(element_id) > connector_topo_dim)
      return boundary_begin(element_id, connector_topo_dim);
    else
      return coboundary_begin(element_id, connector_topo_dim);
  }

  viennagrid_element_id * connected_end(viennagrid_element_id element_id, viennagrid_dimension connector_topo_dim)
  {
    assert( TOPODIM(element_id) != connector_topo_dim );
    if (TOPODIM(element_id) > connector_topo_dim)
      return boundary_end(element_id, connector_topo_dim);
    else
      return coboundary_end(element_id, connector_topo_dim);
  }


  viennagrid_element_id make_refined_element(
    viennagrid_element_id       parent_id,
    viennagrid_element_type     refined_element_type,
    viennagrid_int              refined_element_vertex_count,
    viennagrid_element_id *     refined_element_vertex_ids,
    viennagrid_int              intersects_count,
    viennagrid_element_id *     intersect_vertices_ids,
    viennagrid_element_id *     intersecting_element_ids);


  void make_coboundary(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void make_neighbor(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim);


  viennagrid_element_handle_buffer & element_handle_buffer(viennagrid_dimension topo_dim)
  { return element_handle_buffers[+topo_dim]; }



  bool is_coboundary_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void set_coboundary_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);

  bool is_neighbor_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type);
  void set_neighbor_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type);




  void set_boundary(viennagrid_element_id element_id)
  { boundary_elements(TOPODIM(element_id)).insert(element_id); }

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







  viennagrid_element_id make_vertex(const viennagrid_numeric * coords)
  {
    return viennagrid_compose_element_id(0, mesh_hierarchy()->make_vertex(coords));
  }

  viennagrid_element_id make_vertices(viennagrid_int vertex_count, const viennagrid_numeric * coords)
  {
    return viennagrid_compose_element_id(0, mesh_hierarchy()->make_vertices( vertex_count, coords ));
  }



  void add_elements(viennagrid_element_id first_id, viennagrid_int count);
  void add_element(viennagrid_element_id element_id);


  bool element_id_valid(viennagrid_element_id element_id)
  {
    return mesh_hierarchy()->element_id_valid(element_id);
  }

  viennagrid_element_type element_type(viennagrid_element_id element_id)
  {
    return mesh_hierarchy()->element_buffer(TOPODIM(element_id)).element_type(element_id);
  }


  void set_parent(viennagrid_element_id element_id, viennagrid_element_id parent_id)
  {
    mesh_hierarchy()->element_buffer(TOPODIM(element_id)).set_parent_id(element_id, parent_id);
  }

  viennagrid_element_type parent(viennagrid_element_id element_id)
  {
    return mesh_hierarchy()->element_buffer(TOPODIM(element_id)).parent_id(element_id);
  }

  viennagrid_element_id * boundary_begin(viennagrid_element_id element_id, viennagrid_dimension boundary_topo_dim)
  {
    return mesh_hierarchy()->boundary_begin(element_id, boundary_topo_dim);
  }

  viennagrid_element_id * boundary_end(viennagrid_element_id element_id, viennagrid_dimension boundary_topo_dim)
  {
    return mesh_hierarchy()->boundary_end(element_id, boundary_topo_dim);
  }



  viennagrid_element_id * coboundary_begin(viennagrid_element_id element_id, viennagrid_dimension coboundary_topo_dim)
  {
    make_coboundary(TOPODIM(element_id), coboundary_topo_dim);
    return element_handle_buffer(TOPODIM(element_id)).coboundary_buffer(coboundary_topo_dim).begin(INDEX(element_id));
  }

  viennagrid_element_id * coboundary_end(viennagrid_element_id element_id, viennagrid_dimension coboundary_topo_dim)
  {
    make_coboundary(TOPODIM(element_id), coboundary_topo_dim);
    return element_handle_buffer(TOPODIM(element_id)).coboundary_buffer(coboundary_topo_dim).end(INDEX(element_id));
  }



  viennagrid_element_id * neighbor_begin(viennagrid_element_id element_id, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
  {
    make_neighbor(TOPODIM(element_id), connector_topo_dim, neighbor_topo_dim);
    return element_handle_buffer(TOPODIM(element_id)).neighbor_buffer(connector_topo_dim, neighbor_topo_dim).begin(INDEX(element_id));
  }

  viennagrid_element_id * neighbor_end(viennagrid_element_id element_id, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
  {
    make_neighbor(TOPODIM(element_id), connector_topo_dim, neighbor_topo_dim);
    return element_handle_buffer(TOPODIM(element_id)).neighbor_buffer(connector_topo_dim, neighbor_topo_dim).end(INDEX(element_id));
  }



  viennagrid_region_id * regions_begin(viennagrid_element_id element_id)
  {
    return mesh_hierarchy()->element_buffer(TOPODIM(element_id)).regions_begin(element_id);
  }

  viennagrid_region_id * regions_end(viennagrid_element_id element_id)
  {
    return mesh_hierarchy()->element_buffer(TOPODIM(element_id)).regions_end(element_id);
  }




  bool is_boundary(viennagrid_element_id element_id)
  {
    return boundary_elements(TOPODIM(element_id)).find(element_id) != boundary_elements(TOPODIM(element_id)).end();
  }


private:

  viennagrid_mesh_hierarchy hierarchy_;

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
