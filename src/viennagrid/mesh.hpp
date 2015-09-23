#ifndef VIENNAGRID_BACKEND_MESH_HPP
#define VIENNAGRID_BACKEND_MESH_HPP

#include <cassert>
#include <set>
#include <algorithm>

#include "viennagrid/viennagrid.h"

#include "common.hpp"
#include "buffer.hpp"
#include "dynamic_sizeof.hpp"
#include "element_handle_buffer.hpp"


class viennagrid_mesh_hierarchy_;
typedef viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;



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

  viennagrid_int element_count(viennagrid_dimension element_topological_dimension)
  { return element_handle_buffer(element_topological_dimension).count(); }
  viennagrid_int element_count_by_type(viennagrid_element_type element_type)
  { return element_handle_buffer( viennagrid_topological_dimension(element_type) ).element_count(element_type); }

  viennagrid_element_id * elements_begin(viennagrid_dimension element_topological_dimension)
  { return element_handle_buffer(element_topological_dimension).ids(); }
  viennagrid_element_id * elements_end(viennagrid_dimension element_topological_dimension)
  { return elements_begin(element_topological_dimension) + element_count(element_topological_dimension); }



  viennagrid_element_id * connected_begin(viennagrid_element_id element_id, viennagrid_dimension connector_topological_dimension)
  {
    assert( TOPODIM(element_id) != connector_topological_dimension );
    if (TOPODIM(element_id) > connector_topological_dimension)
      return boundary_begin(element_id, connector_topological_dimension);
    else
      return coboundary_begin(element_id, connector_topological_dimension);
  }

  viennagrid_element_id * connected_end(viennagrid_element_id element_id, viennagrid_dimension connector_topological_dimension)
  {
    assert( TOPODIM(element_id) != connector_topological_dimension );
    if (TOPODIM(element_id) > connector_topological_dimension)
      return boundary_end(element_id, connector_topological_dimension);
    else
      return coboundary_end(element_id, connector_topological_dimension);
  }


  viennagrid_element_id make_refined_element(
    viennagrid_element_id       parent_id,
    viennagrid_element_type     refined_element_type,
    viennagrid_int              refined_element_vertex_count,
    viennagrid_element_id *     refined_element_vertex_ids,
    viennagrid_int              intersects_count,
    viennagrid_element_id *     intersect_vertices_ids,
    viennagrid_element_id *     intersecting_element_ids);


  void make_coboundary(viennagrid_dimension element_topological_dimension, viennagrid_dimension coboundary_topological_dimension);
  void make_neighbor(viennagrid_dimension element_topological_dimension, viennagrid_dimension connector_topological_dimension, viennagrid_dimension neighbor_topological_dimension);


  viennagrid_element_handle_buffer & element_handle_buffer(viennagrid_dimension topological_dimension)
  { return element_handle_buffers[+topological_dimension]; }



  bool is_coboundary_obsolete(viennagrid_dimension element_topological_dimension, viennagrid_dimension coboundary_topological_dimension);
  void set_coboundary_uptodate(viennagrid_dimension element_topological_dimension, viennagrid_dimension coboundary_topological_dimension);

  bool is_neighbor_obsolete(viennagrid_dimension element_topological_dimension, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type);
  void set_neighbor_uptodate(viennagrid_dimension element_topological_dimension, viennagrid_dimension connector_type, viennagrid_dimension neighbor_type);




  void set_boundary(viennagrid_element_id element_id)
  { boundary_elements(TOPODIM(element_id)).insert(element_id); }

  void clear_boundary(viennagrid_dimension element_topological_dimension)
  { boundary_elements(element_topological_dimension).clear(); }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();

  void make_boundary_flags(viennagrid_region region);
  void make_boundary_flags();


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
    for (int i = 0; i != VIENNAGRID_TOPOLOGICAL_DIMENSION_END; ++i)
      element_handle_buffers[i].optimize_memory();
  }


  viennagrid_element_id make_vertex(const viennagrid_numeric * coords);
  viennagrid_element_id make_vertices(viennagrid_int vertex_count, const viennagrid_numeric * coords);

  viennagrid_numeric * get_vertex_pointer();
  viennagrid_numeric * get_vertex(viennagrid_element_id id);

  viennagrid_int vertex_count();


  viennagrid_element_type * element_types_pointer(viennagrid_dimension topological_dimension);
  viennagrid_int * vertex_offsets_pointer(viennagrid_dimension topological_dimension);
  viennagrid_element_id * vertex_ids_pointer(viennagrid_dimension topological_dimension);
  viennagrid_element_id * parent_id_pointer(viennagrid_dimension topological_dimension);
  viennagrid_int parent_id_pointer_size(viennagrid_dimension topological_dimension);
  void ** aux_pointer(viennagrid_dimension topological_dimension);
  viennagrid_int aux_pointer_size(viennagrid_dimension topological_dimension);





  viennagrid_int region_count();
  viennagrid_region get_region(viennagrid_region_id region_id);
  viennagrid_region get_or_create_region(viennagrid_region_id region_id);
  viennagrid_region create_region();
  viennagrid_region_id * region_ids_begin();
  viennagrid_region_id * region_ids_end();




  void add_elements(viennagrid_element_id first_id, viennagrid_int count);
  void add_element(viennagrid_element_id element_id);


  bool element_id_valid(viennagrid_element_id element_id);

  viennagrid_element_type element_type(viennagrid_element_id element_id);


  void set_parent(viennagrid_element_id element_id, viennagrid_element_id parent_id);
  viennagrid_element_type parent(viennagrid_element_id element_id);

  viennagrid_element_id * boundary_begin(viennagrid_element_id element_id, viennagrid_dimension boundary_topological_dimension);
  viennagrid_element_id * boundary_end(viennagrid_element_id element_id, viennagrid_dimension boundary_topological_dimension);


  viennagrid_element_id * coboundary_begin(viennagrid_element_id element_id, viennagrid_dimension coboundary_topological_dimension);
  viennagrid_element_id * coboundary_end(viennagrid_element_id element_id, viennagrid_dimension coboundary_topological_dimension);


  viennagrid_element_id * neighbor_begin(viennagrid_element_id element_id, viennagrid_dimension connector_topological_dimension, viennagrid_dimension neighbor_topological_dimension);
  viennagrid_element_id * neighbor_end(viennagrid_element_id element_id, viennagrid_dimension connector_topological_dimension, viennagrid_dimension neighbor_topological_dimension);


  viennagrid_region_id * regions_begin(viennagrid_element_id element_id);
  viennagrid_region_id * regions_end(viennagrid_element_id element_id);




  bool is_boundary(viennagrid_element_id element_id)
  {
    return boundary_elements(TOPODIM(element_id)).find(element_id) != boundary_elements(TOPODIM(element_id)).end();
  }


private:

  viennagrid_mesh_hierarchy hierarchy_;

  viennagrid_mesh parent_;
  std::vector<viennagrid_mesh> children;
  std::map<viennagrid_mesh, viennagrid_int> mesh_children_map;

  std::string name_;


  viennagrid_int child_mesh_id(viennagrid_mesh child_mesh)
  {
    std::map<viennagrid_mesh, viennagrid_int>::const_iterator it = mesh_children_map.find(child_mesh);
    assert( it != mesh_children_map.end() );
    return it->second;
  }


  viennagrid_element_handle_buffer element_handle_buffers[VIENNAGRID_TOPOLOGICAL_DIMENSION_END];


  viennagrid_int & coboundary_change_counter(viennagrid_dimension element_topological_dimension,
                                             viennagrid_dimension coboundary_topological_dimension)
  { return coboundary_change_counters[+element_topological_dimension][+coboundary_topological_dimension]; }
  viennagrid_int coboundary_change_counters[VIENNAGRID_TOPOLOGICAL_DIMENSION_END][VIENNAGRID_TOPOLOGICAL_DIMENSION_END];


  viennagrid_int & neighbor_change_counter(viennagrid_dimension element_topological_dimension,
                                           viennagrid_dimension connector_topological_dimension,
                                           viennagrid_dimension neighbor_topological_dimension)
  { return neighbor_change_counters[+element_topological_dimension][+connector_topological_dimension][+neighbor_topological_dimension]; }
  viennagrid_int neighbor_change_counters[VIENNAGRID_TOPOLOGICAL_DIMENSION_END][VIENNAGRID_TOPOLOGICAL_DIMENSION_END][VIENNAGRID_TOPOLOGICAL_DIMENSION_END];



  std::set<viennagrid_int> & boundary_elements(viennagrid_dimension element_topological_dimension) { return boundary_elements_[+element_topological_dimension]; }

  std::set<viennagrid_int> boundary_elements_[VIENNAGRID_TOPOLOGICAL_DIMENSION_END];
  viennagrid_int boundary_elements_change_counter;
};

#endif
