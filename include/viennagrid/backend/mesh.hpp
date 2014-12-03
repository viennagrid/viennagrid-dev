#ifndef VIENNAGRID_BACKEND_MESH_HPP
#define VIENNAGRID_BACKEND_MESH_HPP

#include <cassert>
#include <set>

#include "viennagrid/backend/forwards.h"
#include "viennagrid/backend/buffer.hpp"
#include "viennagrid/utils.hpp"


typedef sparse_packed_multibuffer<viennagrid_index, viennagrid_index> ViennaGridCoBoundaryBufferType;
typedef sparse_packed_multibuffer<viennagrid_index, viennagrid_index> ViennaGridNeighborBufferType;
typedef sparse_packed_multibuffer<viennagrid_index, viennagrid_index> ViennaGridElementChildrenBufferType;


struct viennagrid_element_handle_buffer
{
public:

  bool element_present(viennagrid_index element_id) const
  {
    return index_map.find(element_id) != index_map.end();
  }

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

  viennagrid_index * ids()
  { return indices.empty() ? 0 : &indices[0]; }
  viennagrid_int count() const { return indices.size(); }

  ViennaGridCoBoundaryBufferType & coboundary_buffer(viennagrid_dimension coboundary_topo_dim)
  { return coboundary_indices[coboundary_topo_dim]; }
  ViennaGridNeighborBufferType & neighbor_buffer(viennagrid_dimension connector_topo_dim,
                                               viennagrid_dimension neighbor_topo_dim)
  { return neighbor_indices[connector_topo_dim][neighbor_topo_dim]; }

private:
  std::vector<viennagrid_index> indices;
  std::map<viennagrid_index, viennagrid_index> index_map;

  ViennaGridCoBoundaryBufferType coboundary_indices[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  ViennaGridNeighborBufferType neighbor_indices[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];
};


struct viennagrid_element_children_
{
public:


  viennagrid_int & change_counter(viennagrid_int element_topo_dim) { return change_counters[element_topo_dim]; }

  ViennaGridElementChildrenBufferType & children_indices_buffer(viennagrid_int element_topo_dim) { return children_indices_buffers[element_topo_dim]; }

private:
  ViennaGridElementChildrenBufferType children_indices_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int change_counters[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
};





struct viennagrid_mesh_
{
public:
  viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in) : hierarchy_(hierarchy_in), parent_(0) { init(); }
  viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in, viennagrid_mesh parent_in) : hierarchy_(hierarchy_in), parent_(parent_in) { init(); }

  ~viennagrid_mesh_()
  {
    for (std::size_t i = 0; i != children.size(); ++i)
      delete children[i];
  }

  viennagrid_mesh_hierarchy mesh_hierarchy() { return hierarchy_; }
  viennagrid_mesh parent() { return parent_; }

  bool is_root() const;

  viennagrid_mesh make_child()
  {
    viennagrid_mesh mesh = new viennagrid_mesh_( mesh_hierarchy(), this );
    children.push_back( mesh );
    element_children.push_back( viennagrid_element_children_() );
    mesh_children_map[ children.back() ] = children.size()-1;
    return mesh;
  }

  viennagrid_int children_count() { return children.size(); }
  viennagrid_mesh get_child(viennagrid_int id) { return children[id]; }


  viennagrid_dimension geometric_dimension();
  viennagrid_dimension cell_dimension();

  viennagrid_int element_count(viennagrid_dimension element_topo_dim)
  { return element_handle_buffer(element_topo_dim).count(); }

  viennagrid_index * elements_begin(viennagrid_dimension element_topo_dim)
  { return element_handle_buffer(element_topo_dim).ids(); }
  viennagrid_index * elements_end(viennagrid_dimension element_topo_dim)
  { return elements_begin(element_topo_dim) + element_count(element_topo_dim); }


  viennagrid_index * boundary_begin(viennagrid_dimension element_topo_dim, viennagrid_index element_id, viennagrid_dimension boundary_topo_dim);
  viennagrid_index * boundary_end(viennagrid_dimension element_topo_dim, viennagrid_index element_id, viennagrid_dimension boundary_topo_dim);

  viennagrid_index * coboundary_begin(viennagrid_dimension element_tag, viennagrid_index element_id, viennagrid_dimension coboundary_tag)
  {
    make_coboundary(element_tag, coboundary_tag);
    return element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag).begin(element_id);
  }
  viennagrid_index * coboundary_end(viennagrid_dimension element_tag, viennagrid_index element_id, viennagrid_dimension coboundary_tag)
  {
    make_coboundary(element_tag, coboundary_tag);
    return element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag).end(element_id);
  }



  viennagrid_index * neighbor_begin(viennagrid_dimension element_tag, viennagrid_index element_id, viennagrid_dimension connector_tag, viennagrid_dimension neighbor_tag)
  {
    make_neighbor(element_tag, connector_tag, neighbor_tag);
    return element_handle_buffer(element_tag).neighbor_buffer(connector_tag, neighbor_tag).begin(element_id);
  }
  viennagrid_index * neighbor_end(viennagrid_dimension element_tag, viennagrid_index element_id, viennagrid_dimension connector_tag, viennagrid_dimension neighbor_tag)
  {
    make_neighbor(element_tag, connector_tag, neighbor_tag);
    return element_handle_buffer(element_tag).neighbor_buffer(connector_tag, neighbor_tag).end(element_id);
  }




  viennagrid_index * connected_begin(viennagrid_dimension element_tag, viennagrid_index element_id, viennagrid_dimension connected_tag)
  {
    assert( element_tag != connected_tag );
    if (element_tag > connected_tag)
      return boundary_begin(element_tag, element_id, connected_tag);
    else
      return coboundary_begin(element_tag, element_id, connected_tag);
  }

  viennagrid_index * connected_end(viennagrid_dimension element_tag, viennagrid_index element_id, viennagrid_dimension connected_tag)
  {
    assert( element_tag != connected_tag );
    if (element_tag > connected_tag)
      return boundary_end(element_tag, element_id, connected_tag);
    else
      return coboundary_end(element_tag, element_id, connected_tag);
  }


  void add_element(viennagrid_dimension element_topo_dim,
                   viennagrid_index element_id);


  viennagrid_index make_refined_element(
    viennagrid_dimension      element_topo_dim,
    viennagrid_index          element_id,
    viennagrid_element_tag    refined_element_tag,
    viennagrid_int            refined_element_base_count,
    viennagrid_index *        refined_element_base_indices,
    viennagrid_dimension *    refined_element_base_dimensions,
    viennagrid_int            intersects_count,
    viennagrid_index *        intersect_vertices_indices,
    viennagrid_index *        intersects_indices,
    viennagrid_dimension *    intersects_topo_dims);


  void make_coboundary(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void make_neighbor(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim);


  viennagrid_element_handle_buffer & element_handle_buffer(viennagrid_dimension topo_dim)
  { return element_handle_buffers[topo_dim]; }



  bool is_coboundary_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void set_coboundary_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);

  bool is_neighbor_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_tag, viennagrid_dimension neighbor_tag);
  void set_neighbor_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_tag, viennagrid_dimension neighbor_tag);


  bool is_boundary(viennagrid_dimension element_topo_dim, viennagrid_index element_id)
  { return boundary_elements(element_topo_dim).find(element_id) != boundary_elements(element_topo_dim).end(); }

  void set_boundary(viennagrid_dimension element_topo_dim, viennagrid_index element_id)
  { boundary_elements(element_topo_dim).insert(element_id); }

  void clear_boundary(viennagrid_dimension element_topo_dim)
  { boundary_elements(element_topo_dim).clear(); }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();

  void make_boundary_flags(viennagrid_region region);
  void make_boundary_flags();


  void make_element_children(viennagrid_mesh child, viennagrid_int element_topo_dim);



private:

  void init()
  {
    for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      for (viennagrid_int j = 0; j != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++j)
        coboundary_change_counters[i][j] = 0;

    for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      for (viennagrid_int j = 0; j != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++j)
        for (viennagrid_int k = 0; k != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++k)
          neighbor_change_counters[i][j][k] = 0;
  }

  viennagrid_mesh_hierarchy hierarchy_;

  viennagrid_mesh parent_;
  std::vector<viennagrid_mesh> children;
  std::vector<viennagrid_element_children_> element_children;
  std::map<viennagrid_mesh, viennagrid_int> mesh_children_map;


  viennagrid_int child_mesh_index(viennagrid_mesh child_mesh)
  {
    std::map<viennagrid_mesh, viennagrid_int>::const_iterator it = mesh_children_map.find(child_mesh);
    assert( it != mesh_children_map.end() );
    return it->second;
  }

  viennagrid_int & element_children_change_counter(viennagrid_int child_mesh_index_, viennagrid_int element_topo_dim)
  { return element_children[child_mesh_index_].change_counter(element_topo_dim); }
  viennagrid_int & element_children_change_counter(viennagrid_mesh child_mesh, viennagrid_int element_topo_dim)
  { return element_children_change_counter(child_mesh_index(child_mesh),element_topo_dim); }

  bool is_element_children_obsolete(viennagrid_int child_mesh_index_, viennagrid_int element_topo_dim);
  void set_element_children_uptodate(viennagrid_int child_mesh_index_, viennagrid_int element_topo_dim);

  bool is_element_children_obsolete(viennagrid_mesh child_mesh, viennagrid_int element_topo_dim)
  { return is_element_children_obsolete(child_mesh_index(child_mesh), element_topo_dim); }
  void set_element_children_uptodate(viennagrid_mesh child_mesh, viennagrid_int element_topo_dim)
  { set_element_children_uptodate(child_mesh_index(child_mesh), element_topo_dim); }





  viennagrid_element_handle_buffer element_handle_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];


  viennagrid_int & coboundary_change_counter(viennagrid_dimension element_topo_dim,
                                             viennagrid_dimension coboundary_topo_dim)
  { return coboundary_change_counters[element_topo_dim][coboundary_topo_dim]; }
  viennagrid_int coboundary_change_counters[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];


  viennagrid_int & neighbor_change_counter(viennagrid_dimension element_topo_dim,
                                           viennagrid_dimension connector_topo_dim,
                                           viennagrid_dimension neighbor_topo_dim)
  { return neighbor_change_counters[element_topo_dim][connector_topo_dim][neighbor_topo_dim]; }
  viennagrid_int neighbor_change_counters[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];



  std::set<viennagrid_index> & boundary_elements(viennagrid_dimension element_topo_dim) { return boundary_elements_[element_topo_dim]; }

  std::set<viennagrid_index> boundary_elements_[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_index boundary_elements_change_counter;
};

#endif
