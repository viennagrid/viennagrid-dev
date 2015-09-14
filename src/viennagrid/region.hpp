#ifndef VIENNAGRID_BACKEND_REGION_HPP
#define VIENNAGRID_BACKEND_REGION_HPP

#include <set>
#include <string>

#include "viennagrid/viennagrid.h"
#include "common.hpp"
#include "dynamic_sizeof.hpp"


struct viennagrid_mesh_hierarchy_;
typedef viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;

struct viennagrid_region_
{
  friend struct viennagrid_mesh_hierarchy_;

  template<typename T>
  friend struct viennautils::detail::dynamic_sizeof_impl;
public:

  viennagrid_region_(viennagrid_int id_in, viennagrid_mesh_hierarchy hierarchy_in) : id_(id_in), boundary_elements_change_counter(0), hierarchy_(hierarchy_in) {}

  viennagrid_mesh_hierarchy mesh_hierarchy() { return hierarchy_; }

  void set_name(std::string const & name_in) { name_= name_in; }
  std::string const & name() const { return name_; }

  void set_boundary(viennagrid_element_id element_id)
  { boundary_elements(TOPODIM(element_id)).insert(element_id); }

  void clear_boundary(viennagrid_dimension element_topo_dim)
  { boundary_elements(element_topo_dim).clear(); }

  viennagrid_int id() const { return id_; }

  bool are_boundary_flags_obsolete();
  void set_boundary_flags_uptodate();








  bool element_id_valid(viennagrid_element_id element_id);

  bool is_boundary(viennagrid_element_id element_id)
  {
    return boundary_elements(TOPODIM(element_id)).find(element_id) != boundary_elements(TOPODIM(element_id)).end();
  }

  void add_element(viennagrid_element_id element_id);
  bool contains_element(viennagrid_element_id element_id);


private:

  std::set<viennagrid_element_id> & boundary_elements(viennagrid_dimension element_topo_dim) { return boundary_elements_[+element_topo_dim]; }

  std::string name_;
  viennagrid_int id_;

  std::set<viennagrid_element_id> boundary_elements_[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int boundary_elements_change_counter;

  viennagrid_mesh_hierarchy hierarchy_;
};

#endif
