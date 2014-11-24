#ifndef VIENNAGRID_BACKEND_MESH_HPP
#define VIENNAGRID_BACKEND_MESH_HPP

#include <cassert>
#include <set>

#include "viennagrid/backend/forwards.h"
#include "viennagrid/backend/buffer.hpp"
#include "viennagrid/utils.hpp"


typedef sparse_multibuffer<viennagrid_index, viennagrid_index> viennagrid_coboundary_buffer;
typedef sparse_multibuffer<viennagrid_index, viennagrid_index> viennagrid_neighbor_buffer;


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

  viennagrid_index * ids()
  { return indices.empty() ? 0 : &indices[0]; }
  viennagrid_int count() const { return indices.size(); }

  viennagrid_coboundary_buffer & coboundary_buffer(viennagrid_dimension coboundary_topo_dim)
  { return coboundary_indices[coboundary_topo_dim]; }
  viennagrid_neighbor_buffer & neighbor_buffer(viennagrid_dimension connector_topo_dim,
                                               viennagrid_dimension neighbor_topo_dim)
  { return neighbor_indices[connector_topo_dim][neighbor_topo_dim]; }

private:
  std::vector<viennagrid_index> indices;
  std::map<viennagrid_index, viennagrid_index> index_map;

  viennagrid_coboundary_buffer coboundary_indices[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_neighbor_buffer neighbor_indices[VIENNAGRID_TOPOLOGIC_DIMENSION_END][VIENNAGRID_TOPOLOGIC_DIMENSION_END];
};




// struct mixed_element_type_buffer
// {
//   std::vector<viennagrid_index>         element_ids;
//   std::vector<viennagrid_element_tag>   element_tags;
// };


struct viennagrid_mesh_
{
public:
  viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in) : hierarchy_(hierarchy_in), parent_(0) {}
  viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in, viennagrid_mesh parent_in) : hierarchy_(hierarchy_in), parent_(parent_in) {}

  ~viennagrid_mesh_()
  {
    for (ChildrenContainerType::iterator it = children.begin(); it != children.end(); ++it)
      delete *it;
  }

  viennagrid_mesh_hierarchy mesh_hierarchy() { return hierarchy_; }
  viennagrid_mesh parent() { return parent_; }

  viennagrid_mesh make_child()
  {
    viennagrid_mesh mesh = new viennagrid_mesh_( mesh_hierarchy(), this );
    children.push_back( mesh );
    return mesh;
  }

  viennagrid_int children_count() { return children.size(); }
  viennagrid_mesh get_child(viennagrid_int id) { return children[id]; }


  viennagrid_dimension geometric_dimension();
  viennagrid_dimension topologic_dimension();

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
                   viennagrid_index element_id)
  { element_handle_buffer(element_topo_dim).add_element(element_id); }


  void make_coboundary(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void make_neighbor(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim);


  viennagrid_element_handle_buffer & element_handle_buffer(viennagrid_dimension topo_dim)
  { return element_handle_buffers[topo_dim]; }


//   viennagrid_element_tag unpack_element_tag(viennagrid_element_tag et);


  bool is_coboundary_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);
  void set_coboundary_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim);

  bool is_neighbor_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_tag, viennagrid_dimension neighbor_tag);
  void set_neighbor_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_tag, viennagrid_dimension neighbor_tag);


  bool is_boundary(viennagrid_dimension element_topo_dim, viennagrid_index element_id)
  {
//     std::cout << "# boundary elements " << element_topo_dim  << "/" << element_id << " : " << boundary_elements(element_topo_dim).size() << std::endl;
    return boundary_elements(element_topo_dim).find(element_id) != boundary_elements(element_topo_dim).end();
  }

  void set_boundary(viennagrid_dimension element_topo_dim, viennagrid_index element_id)
  {
//     std::cout << "Adding " << element_topo_dim << "/" << element_id << " as boundary" << std::endl;
    boundary_elements(element_topo_dim).insert(element_id);
  }

  void clear_boundary(viennagrid_dimension element_topo_dim)
  { boundary_elements(element_topo_dim).clear(); }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();

  void make_boundary_flags(viennagrid_region region);
  void make_boundary_flags();



private:

  viennagrid_mesh_hierarchy hierarchy_;

  typedef std::vector<viennagrid_mesh> ChildrenContainerType;
  viennagrid_mesh parent_;
  ChildrenContainerType children;

  viennagrid_element_handle_buffer element_handle_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];


  viennagrid_int & coboundary_change_counter(viennagrid_dimension element_topo_dim,
                                             viennagrid_dimension coboundary_topo_dim)
  {
    std::map< std::pair<viennagrid_dimension, viennagrid_dimension>, viennagrid_int >::iterator it = coboundary_change_counters.find( std::make_pair(element_topo_dim, coboundary_topo_dim) );
    if (it != coboundary_change_counters.end())
      return it->second;

    it = coboundary_change_counters.insert( std::make_pair(std::make_pair(element_topo_dim, coboundary_topo_dim), viennagrid_int(0)) ).first;
    return it->second;
  }
  std::map< std::pair<viennagrid_dimension, viennagrid_dimension>, viennagrid_int > coboundary_change_counters;


  viennagrid_int & neighbor_change_counter(viennagrid_dimension element_topo_dim,
                                           viennagrid_dimension connector_topo_dim,
                                           viennagrid_dimension neighbor_topo_dim)
  {
    std::map< viennagrid::triple<viennagrid_dimension, viennagrid_dimension, viennagrid_dimension>, viennagrid_int >::iterator it = neighbor_change_counters.find( viennagrid::make_tripple(element_topo_dim, connector_topo_dim, neighbor_topo_dim) );
    if (it != neighbor_change_counters.end())
      return it->second;

    it = neighbor_change_counters.insert( std::make_pair(viennagrid::make_tripple(element_topo_dim, connector_topo_dim, neighbor_topo_dim), viennagrid_int(0)) ).first;
    return it->second;
  }
  std::map< viennagrid::triple<viennagrid_dimension, viennagrid_dimension, viennagrid_dimension>, viennagrid_int > neighbor_change_counters;



  std::set<viennagrid_index> & boundary_elements(viennagrid_dimension element_topo_dim) { return boundary_elements_[element_topo_dim]; }

  std::set<viennagrid_index> boundary_elements_[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_index boundary_elements_change_counter;
};

#endif
