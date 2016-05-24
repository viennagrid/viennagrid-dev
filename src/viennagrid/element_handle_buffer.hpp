#ifndef VIENNAGRID_BACKEND_ELEMENT_HANDLE_BUFFER_HPP
#define VIENNAGRID_BACKEND_ELEMENT_HANDLE_BUFFER_HPP

#include "common.hpp"
#include "buffer.hpp"

class viennagrid_mesh_hierarchy_;
typedef viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;


class viennagrid_element_handle_buffer
{
  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_element_id> ViennaGridCoBoundaryBufferType;
  typedef sparse_packed_multibuffer<viennagrid_int, viennagrid_element_id> ViennaGridNeighborBufferType;

  viennagrid_element_handle_buffer() { clear(-1); }

  bool element_present(viennagrid_element_id element_id) const
  {
    return find(element_id) != end();
  }

  void add_element(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id element_id);
  void add_elements(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id start_id, viennagrid_int count);
  void save_add_elements(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id start_id, viennagrid_int count);

  viennagrid_int * ids() { return ids_.empty() ? 0 : &ids_[0]; }
  viennagrid_int count() const { return ids_.size(); }

  viennagrid_int element_count(viennagrid_element_type et) const { return element_counts[+et]; }

  ViennaGridCoBoundaryBufferType & coboundary_buffer(viennagrid_dimension coboundary_topological_dimension)
  { return coboundary_ids[+coboundary_topological_dimension]; }
  ViennaGridNeighborBufferType & neighbor_buffer(viennagrid_dimension connector_topological_dimension,
                                               viennagrid_dimension neighbor_topological_dimension)
  { return neighbor_ids[+connector_topological_dimension][+neighbor_topological_dimension]; }

  void clear(viennagrid_dimension topologic_dimension_)
  {
    topologic_dimension = topologic_dimension_;

    ids_.clear();

    for (int i = 0; i != VIENNAGRID_TOPOLOGICAL_DIMENSION_END; ++i)
      coboundary_ids[i].clear();

    for (int i = 0; i != VIENNAGRID_TOPOLOGICAL_DIMENSION_END; ++i)
      for (int j = 0; j != VIENNAGRID_TOPOLOGICAL_DIMENSION_END; ++j)
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

  void insert(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id id);

  viennagrid_dimension topologic_dimension;

  ViennaGridCoBoundaryBufferType coboundary_ids[VIENNAGRID_TOPOLOGICAL_DIMENSION_END];
  ViennaGridNeighborBufferType neighbor_ids[VIENNAGRID_TOPOLOGICAL_DIMENSION_END][VIENNAGRID_TOPOLOGICAL_DIMENSION_END];

  viennagrid_int element_counts[VIENNAGRID_ELEMENT_TYPE_COUNT];
};

#endif
