#ifndef VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP
#define VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP

#include <algorithm>

#include "viennagrid/backend/forwards.h"
#include "viennagrid/backend/buffer.hpp"
#include "viennagrid/backend/mesh.hpp"
#include "viennagrid/backend/region.hpp"
#include "viennagrid/utils.hpp"



typedef dense_multibuffer<viennagrid_index, viennagrid_index> viennagrid_boundary_buffer;
typedef dense_multibuffer<viennagrid_index, viennagrid_numeric> viennagrid_hole_point_buffer;


struct element_key
{
  element_key() {}
  element_key(viennagrid_index * indices, viennagrid_int index_count) : vertex_indices(index_count)
  {
    std::copy( indices, indices+index_count, &vertex_indices[0] );
    std::sort(vertex_indices.begin(), vertex_indices.end());
  }

  bool operator<(element_key const & rhs) const
  { return vertex_indices < rhs.vertex_indices; }

  std::vector<viennagrid_index> vertex_indices;
};



struct viennagrid_element_buffer
{
  friend struct viennagrid_mesh_hierarchy_;
  friend struct viennagrid_mesh_;
public:

  viennagrid_element_buffer() : topologic_dimension(-1) {}

  void set_topologic_dimension(viennagrid_dimension topo_dim_in)
  {
    topologic_dimension = topo_dim_in;
    boundary_indices.resize(topologic_dimension);
  }


  viennagrid_element_tag element_tag(viennagrid_index element_id) const
  {
    return element_tags[element_id];
  }

  viennagrid_index * boundary_indices_begin(viennagrid_index boundary_topo_dim, viennagrid_index element_id)
  {
    return boundary_buffer(boundary_topo_dim).begin(element_id);
  }

  viennagrid_index * boundary_indices_end(viennagrid_index boundary_topo_dim, viennagrid_index element_id)
  {
    return boundary_buffer(boundary_topo_dim).end(element_id);
  }


  viennagrid_index parend_id(viennagrid_index element_id) const { return parents[element_id]; }

  void add_to_region(viennagrid_index element_id, viennagrid_region region_)
  {
    for (viennagrid_region * it = region_buffer.begin(element_id); it != region_buffer.end(element_id); ++it)
    {
      if (*it == region_)
        return;
    }

    region_buffer.add(element_id, region_);
  }

  viennagrid_region * regions_begin(viennagrid_index element_id) { return region_buffer.begin(element_id); }
  viennagrid_region * regions_end(viennagrid_index element_id) { return region_buffer.end(element_id); }


private:

  viennagrid_boundary_buffer & boundary_buffer(viennagrid_dimension boundary_topo_dim)
  { return boundary_indices[boundary_topo_dim]; }
  viennagrid_boundary_buffer const & boundary_buffer(viennagrid_dimension boundary_topo_dim) const
  { return boundary_indices[boundary_topo_dim]; }

  viennagrid_index get_element(element_key const & key)
  {
    std::map<element_key, viennagrid_index>::iterator it = element_map.find(key);
    if (it == element_map.end())
      return -1;

    return it->second;
  }

  viennagrid_index get_element(viennagrid_index * indices, viennagrid_int index_count)
  {
    return get_element( element_key(indices, index_count) );
  }

  viennagrid_index make_element(viennagrid_mesh_hierarchy mesh_hierarchy,
                                viennagrid_element_tag element_tag_,
                                viennagrid_index * indices,
                                viennagrid_int index_count);

  viennagrid_index size() const { return parents.size(); }


  viennagrid_dimension topologic_dimension;

  std::vector<viennagrid_element_tag> element_tags;
  std::vector<viennagrid_index> parents;
  dense_multibuffer<viennagrid_index, viennagrid_region> region_buffer;


  std::vector<viennagrid_boundary_buffer> boundary_indices;
  std::map<element_key, viennagrid_index> element_map;
};





struct viennagrid_mesh_hierarchy_
{
public:
  viennagrid_mesh_hierarchy_() : geometric_dimension_(0), topologic_dimension_(-1),
//   cell_tag_(VIENNAGRID_ELEMENT_TAG_NO_ELEMENT),
  root_( new viennagrid_mesh_(this) ), highest_region_id(0), change_counter_(0), use_count_(1)
  {
    for (viennagrid_int i = 0; i < VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
      element_buffer(i).set_topologic_dimension(i);
  }

  ~viennagrid_mesh_hierarchy_()
  {
//     std::cout << "DELETE HIERARCHY" << std::endl;

    delete root_;

    for (std::vector<viennagrid_region>::iterator it = regions.begin(); it != regions.end(); ++it)
      delete *it;
  }

  viennagrid_mesh root() { return root_; }

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

  viennagrid_dimension topologic_dimension() const { return topologic_dimension_; }
  viennagrid_dimension facet_topological_dimension() const { return topologic_dimension()-1; }

  viennagrid_element_tag cell_tags_begin() const { return viennagrid_element_tag_of_topological_dimension_begin(topologic_dimension()); }
  viennagrid_element_tag cell_tags_end() const { return viennagrid_element_tag_of_topological_dimension_begin(topologic_dimension()); }

  viennagrid_element_tag facet_tags_begin() const { return viennagrid_element_tag_of_topological_dimension_begin(facet_topological_dimension()); }
  viennagrid_element_tag facet_tags_end() const { return viennagrid_element_tag_of_topological_dimension_begin(facet_topological_dimension()); }

//   viennagrid_element_tag cell_tag() const { return cell_tag_; }


  viennagrid_element_buffer & element_buffer(viennagrid_index topo_dim)
  { return element_buffers[topo_dim]; }



  viennagrid_index * make_boundary_indices(viennagrid_element_tag host_tag,
                                           viennagrid_element_tag boundary_tag,
                                           viennagrid_int count)
  {
    return element_buffer(viennagrid_topological_dimension(host_tag)).boundary_buffer(viennagrid_topological_dimension(boundary_tag)).push_back(count);
  }

  viennagrid_index * make_boundary_indices(viennagrid_element_tag host_tag,
                                           viennagrid_element_tag boundary_tag)
  {
    return make_boundary_indices(host_tag, boundary_tag,
                                 viennagrid_boundary_element_count_from_element_tag(host_tag, boundary_tag));
  }



  viennagrid_index make_vertex(const viennagrid_numeric * coords)
  {
    viennagrid_index id = element_buffer(0).make_element(this, VIENNAGRID_ELEMENT_TAG_VERTEX, 0, 0);

    viennagrid_int prev_size = vertex_buffer.size();
    vertex_buffer.resize( vertex_buffer.size() + geometric_dimension() );

    if (coords)
      std::copy( coords, coords+geometric_dimension(), &vertex_buffer[0] + prev_size );

    return id;
  }

  viennagrid_numeric * get_vertex(viennagrid_int id)
  {
    return &vertex_buffer[id * geometric_dimension()];
  }



  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * indices,
                                    viennagrid_int count);
  viennagrid_index get_make_plc(viennagrid_index * indices,
                                viennagrid_int count);






  void make_plc()
  {
    hole_point_buffer.push_back(0);
  }

  viennagrid_numeric * hole_points_begin(viennagrid_int plc_id)
  {
    return hole_point_buffer.begin(plc_id);
  }
  viennagrid_numeric * hole_points_end(viennagrid_int plc_id)
  {
    return hole_point_buffer.end(plc_id);
  }

  void add_hole_point(viennagrid_int plc_id, viennagrid_numeric const * hole_point)
  {
    viennagrid_index old_count = hole_point_buffer.size(plc_id);
    viennagrid_numeric * hp = hole_point_buffer.resize(plc_id, old_count+geometric_dimension());
    std::copy(hole_point, hole_point+geometric_dimension(), hp+old_count);
  }








  viennagrid_index delete_element_simple(viennagrid_dimension element_topo_dim, viennagrid_index element_id);
  viennagrid_index delete_element(viennagrid_dimension element_topo_dim, viennagrid_index element_id);

  viennagrid_region get_region(viennagrid_index region_id)
  {
    std::map<viennagrid_index, viennagrid_index>::const_iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];
    return 0;
  }

  viennagrid_region get_make_region(viennagrid_index region_id)
  {
    std::map<viennagrid_index, viennagrid_index>::iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];

    regions.push_back( new viennagrid_region_(region_id, this) );
    region_id_map[region_id] = regions.size()-1;
    highest_region_id = region_id+1;

    return regions.back();
  }

  viennagrid_region make_region()
  {
    return get_make_region(highest_region_id++);
  }

  viennagrid_region * regions_begin() { return &regions[0]; }
  viennagrid_region * regions_end() { return &regions[0] + regions.size(); }

  bool is_in_region(viennagrid_dimension element_topo_dim, viennagrid_index element_id, viennagrid_index region_id)
  {
    viennagrid_region * it = element_buffer(element_topo_dim).regions_begin(element_id);
    viennagrid_region * regions_end = element_buffer(element_topo_dim).regions_end(element_id);

    for (; it != regions_end; ++it)
      if (region_id == (*it)->id())
        return true;

    return false;
  }

//   viennagrid_element_tag unpack_element_tag(viennagrid_element_tag et) const
//   {
//     if (et == VIENNAGRID_ELEMENT_TAG_CELL)
//       return cell_tag();
//     else if (et == VIENNAGRID_ELEMENT_TAG_FACET)
//       return viennagrid_facet_tag(cell_tag());
//     else
//       return et;
//   }


  bool is_obsolete( viennagrid_int change_counter_to_check ) const { return change_counter_to_check != change_counter_; }
  void update_change_counter( viennagrid_int & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
  void increment_change_counter() { ++change_counter_; }






  void retain() const
  {
//     std::cout << "MESH HIERARCHY RETAIN" << std::endl;
    ++use_count_;
  }

  bool release() const
  {
//     std::cout << "MESH HIERARCHY RELEASE" << std::endl;
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



  viennagrid_element_buffer element_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];

  viennagrid_dimension geometric_dimension_;
  viennagrid_dimension topologic_dimension_;

//   viennagrid_element_tag cell_tag_;
  viennagrid_mesh root_;

  std::vector<viennagrid_numeric> vertex_buffer;
  viennagrid_hole_point_buffer hole_point_buffer;

  std::vector<viennagrid_region> regions;
  viennagrid_index highest_region_id;
  std::map<viennagrid_index, viennagrid_index> region_id_map;

  viennagrid_int change_counter_;
  mutable viennagrid_int use_count_;
};

#endif
