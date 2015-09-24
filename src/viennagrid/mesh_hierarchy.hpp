#ifndef VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP
#define VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP

#include <algorithm>
#include <sstream>
#include <deque>
#include <cstring>

#include "viennagrid/viennagrid.h"

#include "common.hpp"
#include "buffer.hpp"
#include "region.hpp"
#include "element_buffer.hpp"

#include "boost/container/flat_map.hpp"

typedef dense_packed_multibuffer<viennagrid_int, viennagrid_numeric> ViennaGridHolePointBufferType;

class viennagrid_mesh_hierarchy_;
typedef viennagrid_mesh_hierarchy_ * viennagrid_mesh_hierarchy;




class viennagrid_mesh_hierarchy_
{
  friend class viennagrid_element_buffer;

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
    if (cell_dimension() == VIENNAGRID_INVALID_DIMENSION)
      cell_dimension_ = topologic_dimension;

    cell_dimension_ = std::max( cell_dimension_, topologic_dimension );
  }


  viennagrid_element_buffer & element_buffer(viennagrid_int topological_dimension)
  { return element_buffers[topological_dimension]; }



  viennagrid_int boundary_layout() const { return boundary_layout_; }
  viennagrid_error set_boundary_layout(viennagrid_int boundary_layout_in);
  bool full_boundary_layout() const { return boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_FULL; }
  bool sparse_layout() { return boundary_layout() == VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE; }



  viennagrid_element_id * boundary_begin(viennagrid_element_id element_id,
                                         viennagrid_dimension boundary_topological_dimension)
  { return element_buffer(TOPODIM(element_id)).boundary_begin(element_id, boundary_topological_dimension); }
  viennagrid_element_id * boundary_end(viennagrid_element_id element_id,
                                       viennagrid_dimension boundary_topological_dimension)
  { return element_buffer(TOPODIM(element_id)).boundary_end(element_id, boundary_topological_dimension); }


  viennagrid_element_id * make_boundary_ids(viennagrid_element_type host_type,
                                            viennagrid_element_type boundary_type,
                                            viennagrid_int count)
  {
    return element_buffer(viennagrid_topological_dimension(host_type)).boundary_buffer(viennagrid_topological_dimension(boundary_type)).push_back(count);
  }

  viennagrid_element_id * make_boundary_ids(viennagrid_element_type host_type,
                                            viennagrid_element_type boundary_type)
  {
    return make_boundary_ids(host_type, boundary_type,
                                 viennagrid_boundary_element_count_from_element_type(host_type, boundary_type));
  }



  viennagrid_element_id make_vertex(const viennagrid_numeric * coords);
  viennagrid_element_id make_vertices(viennagrid_int vertex_count, const viennagrid_numeric * coords);


  viennagrid_numeric * get_vertex_pointer()
  {
    return &vertex_buffer[0];
  }

  viennagrid_numeric * get_vertex(viennagrid_element_id id)
  {
    return &vertex_buffer[INDEX(id) * geometric_dimension()];
  }

  viennagrid_int vertex_count() const
  {
    return vertex_buffer.size() / geometric_dimension();
  }



  viennagrid_element_id make_elements(viennagrid_int element_count_,
                                       viennagrid_element_type * element_types_,
                                       viennagrid_int * element_vertex_index_offsets_,
                                       viennagrid_element_id * element_vertex_indices_,
                                       viennagrid_region_id * region_ids);

  std::pair<viennagrid_element_id, bool> get_make_element(viennagrid_element_type element_type,
                                                          viennagrid_int vertex_count,
                                                          viennagrid_element_id * vertex_ids,
                                                          viennagrid_mesh mesh,
                                                          bool make_boundary = true);





//   viennagrid_int delete_element_simple(viennagrid_dimension element_topological_dimension, viennagrid_int element_id);
//   viennagrid_int delete_element(viennagrid_dimension element_topological_dimension, viennagrid_int element_id);

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

  viennagrid_region_id * region_ids_begin() { return region_ids.empty() ? NULL : &region_ids[0]; }
  viennagrid_region_id * region_ids_end() { return region_ids.empty() ? NULL : &region_ids[0] + region_ids.size(); }

  bool is_in_region(viennagrid_element_id element_id, viennagrid_region_id region_id)
  {
    viennagrid_region_id * it = element_buffer(TOPODIM(element_id)).regions_begin(element_id);
    viennagrid_region_id * regions_end = element_buffer(TOPODIM(element_id)).regions_end(element_id);

    for (; it != regions_end; ++it)
      if (region_id == *it)
        return true;

    return false;
  }


  bool is_obsolete( viennagrid_int change_counter_to_check ) const { return change_counter_to_check != change_counter_; }
  void update_change_counter( viennagrid_int & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
  void increment_change_counter() { ++change_counter_; }

  void clear();

  viennagrid_int mesh_count() const { return meshes_.size(); }
  viennagrid_mesh mesh(viennagrid_int i) { return meshes_[i]; }


  void optimize_memory();
  long memory_size() const;



  bool element_id_valid(viennagrid_element_id element_id)
  {
    return ( viennagrid_topological_dimension_valid(TOPODIM(element_id)) && (INDEX(element_id) >= 0) && (INDEX(element_id) < element_buffer(TOPODIM(element_id)).size()) );
  }

  bool valid_sparse_dimension(viennagrid_dimension topological_dimension)
  {
    if (sparse_layout())
      return (topological_dimension == 0) || (topological_dimension == cell_dimension());
    else
      return true;
  }


  viennagrid_int element_count(viennagrid_dimension topological_dimension)
  {
    return element_buffer(topological_dimension).size();
  }



private:



  viennagrid_element_id get_make_line(viennagrid_element_id * vertex_ids, viennagrid_int vi0, viennagrid_int vi1, viennagrid_mesh mesh)
  {
    viennagrid_element_id tmp[2] = { vertex_ids[vi0], vertex_ids[vi1] };
    return get_make_element(VIENNAGRID_ELEMENT_TYPE_LINE, 2, tmp, mesh, false).first;
  }

  viennagrid_element_id get_make_triangle(
        viennagrid_element_id * vertex_ids, viennagrid_int vi0, viennagrid_int vi1, viennagrid_int vi2,
        viennagrid_element_id * line_ids, viennagrid_int li0, viennagrid_int li1, viennagrid_int li2,
        viennagrid_mesh mesh)
  {
    viennagrid_element_id tmp[3] = { vertex_ids[vi0], vertex_ids[vi1], vertex_ids[vi2] };

    std::pair<viennagrid_element_id, bool> triangle = get_make_element(VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, tmp, mesh, false);

    if (triangle.second && full_boundary_layout())
    {
      viennagrid_element_id * local_line_ids = boundary_begin(triangle.first, 1);
      local_line_ids[0] = line_ids[li0];
      local_line_ids[1] = line_ids[li1];
      local_line_ids[2] = line_ids[li2];
    }

    return triangle.first;
  }

  viennagrid_element_id get_make_quadrilateral(
        viennagrid_element_id * vertex_ids, viennagrid_int vi0, viennagrid_int vi1, viennagrid_int vi2, viennagrid_int vi3,
        viennagrid_element_id * line_ids, viennagrid_int li0, viennagrid_int li1, viennagrid_int li2, viennagrid_int li3,
        viennagrid_mesh mesh)
  {
    viennagrid_element_id tmp[4] = { vertex_ids[vi0], vertex_ids[vi1], vertex_ids[vi2], vertex_ids[vi3] };

    std::pair<viennagrid_element_id, bool> quad = get_make_element(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, 4, tmp, mesh, false);

    if (quad.second && full_boundary_layout())
    {
      viennagrid_element_id * local_line_ids = boundary_begin(quad.first, 1);
      local_line_ids[0] = line_ids[li0];
      local_line_ids[1] = line_ids[li1];
      local_line_ids[2] = line_ids[li2];
      local_line_ids[3] = line_ids[li3];
    }

    return quad.first;
  }





  std::pair<viennagrid_element_id, bool> get_make_element_2(viennagrid_element_type element_type,
                                                     viennagrid_element_id * vertex_ids,
                                                     viennagrid_int i0, viennagrid_int i1,
                                                     viennagrid_mesh mesh,
                                                     bool make_boundary = true)
  {
    viennagrid_element_id tmp[2];

    tmp[0] = vertex_ids[i0];
    tmp[1] = vertex_ids[i1];
    return get_make_element(element_type, 2, tmp, mesh, make_boundary);
  }

  std::pair<viennagrid_element_id, bool> get_make_element_3(viennagrid_element_type element_type,
                                                     viennagrid_element_id * vertex_ids,
                                                     viennagrid_int i0, viennagrid_int i1, viennagrid_int i2,
                                                     viennagrid_mesh mesh,
                                                     bool make_boundary = true)
  {
    viennagrid_element_id tmp[3];

    tmp[0] = vertex_ids[i0];
    tmp[1] = vertex_ids[i1];
    tmp[2] = vertex_ids[i2];
    return get_make_element(element_type, 3, tmp, mesh, make_boundary);
  }

  std::pair<viennagrid_element_id, bool> get_make_element_4(viennagrid_element_type element_type,
                                                     viennagrid_element_id * vertex_ids,
                                                     viennagrid_int i0, viennagrid_int i1, viennagrid_int i2, viennagrid_int i3,
                                                     viennagrid_mesh mesh,
                                                     bool make_boundary = true)
  {
    viennagrid_element_id tmp[4];

    tmp[0] = vertex_ids[i0];
    tmp[1] = vertex_ids[i1];
    tmp[2] = vertex_ids[i2];
    tmp[3] = vertex_ids[i3];
    return get_make_element(element_type, 4, tmp, mesh, make_boundary);
  }


  viennagrid_int boundary_layout_;


  viennagrid_element_buffer element_buffers[VIENNAGRID_TOPOLOGICAL_DIMENSION_END];

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
