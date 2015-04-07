#ifndef VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP
#define VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP

#include <algorithm>
#include <sstream>

#include "viennagrid/forwards.h"
#include "buffer.hpp"
#include "mesh.hpp"
#include "region.hpp"
// #include "utils.hpp"



typedef dense_packed_multibuffer<viennagrid_index, viennagrid_index> ViennaGridBoundaryBufferType;
typedef dense_packed_multibuffer<viennagrid_index, viennagrid_numeric> ViennaGridHolePointBufferType;
typedef dense_packed_multibuffer<viennagrid_index, viennagrid_region> ViennaGridRegionBufferType;

// typedef dense_multibuffer<viennagrid_index, viennagrid_index> ViennaGridBoundaryBufferType;
// typedef dense_multibuffer<viennagrid_index, viennagrid_numeric> ViennaGridHolePointBufferType;
// typedef dense_multibuffer<viennagrid_index, viennagrid_region> ViennaGridRegionBufferType;


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

  viennagrid_index * boundary_begin(viennagrid_index boundary_topo_dim, viennagrid_index element_id)
  {
    return boundary_buffer(boundary_topo_dim).begin(element_id);
  }

  viennagrid_index * boundary_end(viennagrid_index boundary_topo_dim, viennagrid_index element_id)
  {
    return boundary_buffer(boundary_topo_dim).end(element_id);
  }


  viennagrid_index parent_id(viennagrid_index element_id) const { return parents[element_id]; }
  void set_parent_id(viennagrid_index element_id, viennagrid_index parent_id) { parents[element_id] = parent_id; }

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

  void clear()
  {
    element_tags.clear();
    parents.clear();
    region_buffer.clear();

    boundary_indices.clear();
    element_map.clear();
  }

  viennagrid_int size() const { return parents.size(); }
  viennagrid_element_tag * element_tags_pointer() { return &element_tags[0]; }
  viennagrid_index * vertex_offsets_pointer() { return boundary_buffer(0).offset_pointer(); }
  viennagrid_index * vertex_indices_pointer() { return boundary_buffer(0).values_pointer(); }
  viennagrid_index * parent_id_pointer() { return &parents[0]; }
  viennagrid_index * region_offsets_pointer() { return region_buffer.offset_pointer(); }


private:

  ViennaGridBoundaryBufferType & boundary_buffer(viennagrid_dimension boundary_topo_dim)
  { return boundary_indices[boundary_topo_dim]; }
//   viennagrid_boundary_buffer const & boundary_buffer(viennagrid_dimension boundary_topo_dim) const
//   { return boundary_indices[boundary_topo_dim]; }

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


  viennagrid_dimension topologic_dimension;

  std::vector<viennagrid_element_tag> element_tags;
  std::vector<viennagrid_index> parents;
//   dense_multibuffer<viennagrid_index, viennagrid_region> region_buffer;
  dense_packed_multibuffer<viennagrid_index, viennagrid_region> region_buffer;


  std::vector<ViennaGridBoundaryBufferType> boundary_indices;
  std::map<element_key, viennagrid_index> element_map;
};





struct viennagrid_mesh_hierarchy_
{
public:
  viennagrid_mesh_hierarchy_() : geometric_dimension_(0), cell_dimension_(-1),
  root_( new viennagrid_mesh_(this) ), highest_region_id(0), change_counter_(0), use_count_(1), retain_release_count(0)
  {
    clear();
//     std::cout << "NEW HIERARCHY " << this << std::endl;
  }

  ~viennagrid_mesh_hierarchy_()
  {
//     std::cout << "DELETE HIERARCHY " << this << " (retain-release count=" << retain_release_count << ")" << std::endl;

    delete root_;

    for (std::vector<viennagrid_region>::iterator it = regions.begin(); it != regions.end(); ++it)
      delete *it;
  }

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


  viennagrid_element_buffer & element_buffer(viennagrid_index topo_dim)
  { return element_buffers[topo_dim]; }



  viennagrid_index * boundary_begin(viennagrid_dimension element_topo_dim,
                                    viennagrid_index element_id,
                                    viennagrid_dimension boundary_topo_dim)
  { return element_buffer(element_topo_dim).boundary_begin(boundary_topo_dim, element_id); }
  viennagrid_index * boundary_end(viennagrid_dimension element_topo_dim,
                                  viennagrid_index element_id,
                                  viennagrid_dimension boundary_topo_dim)
  { return element_buffer(element_topo_dim).boundary_end(boundary_topo_dim, element_id); }


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

  viennagrid_int vertex_count() const
  {
    return vertex_buffer.size() / geometric_dimension();
  }



  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * indices,
                                    viennagrid_dimension * topo_dims,
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

  viennagrid_int hole_point_element_count() const { return hole_point_buffer.size(); }
  viennagrid_index * hole_points_offsets() { return hole_point_buffer.offset_pointer(); }
  viennagrid_numeric * hole_points_pointer() { return hole_point_buffer.values_pointer(); }








  viennagrid_index delete_element_simple(viennagrid_dimension element_topo_dim, viennagrid_index element_id);
  viennagrid_index delete_element(viennagrid_dimension element_topo_dim, viennagrid_index element_id);

  viennagrid_index region_count()
  {
    return regions.size();
  }

  viennagrid_region get_region(viennagrid_index region_id)
  {
    std::map<viennagrid_index, viennagrid_index>::const_iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];
    return NULL;
  }

  viennagrid_region get_make_region(viennagrid_index region_id)
  {
    std::map<viennagrid_index, viennagrid_index>::iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];

    regions.push_back( new viennagrid_region_(region_id, this) );
    region_id_map[region_id] = regions.size()-1;
    highest_region_id = region_id+1;

    std::stringstream ss;
    ss << region_id;
    regions.back()->set_name(ss.str());


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


  bool is_obsolete( viennagrid_int change_counter_to_check ) const { return change_counter_to_check != change_counter_; }
  void update_change_counter( viennagrid_int & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
  void increment_change_counter() { ++change_counter_; }


  viennagrid_int element_count( viennagrid_element_tag element_tag ) const { return element_counts[element_tag]; }


  void clear()
  {
    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
    {
      element_buffer(i).clear();
      element_buffer(i).set_topologic_dimension(i);
      element_counts[i] = 0;
    }

    geometric_dimension_ = 0;
    cell_dimension_ = -1;

    root_->clear();
    meshes_.clear();
    meshes_.push_back(root());

    vertex_buffer.clear();
    hole_point_buffer.clear();

    for (std::vector<viennagrid_region>::iterator it = regions.begin(); it != regions.end(); ++it)
      delete *it;
    regions.clear();
    highest_region_id = 0;
    region_id_map.clear();

    change_counter_ = 0;
//     use_count_ = 1;
//     retain_release_count = 0;
  }

  void retain() const
  {
    ++use_count_;
    ++retain_release_count;
//     std::cout << "MESH HIERARCHY RETAIN (retain-release count=" << retain_release_count << ")" << std::endl;
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


  viennagrid_int mesh_count() const { return meshes_.size(); }
  viennagrid_mesh mesh(viennagrid_index i) { return meshes_[i]; }



private:

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1)
  {
    viennagrid_int tmp[2];
    viennagrid_dimension dimensions[2];
    std::fill(dimensions, dimensions+2, 0);

    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    return get_make_element(element_tag, tmp, dimensions, 2);
  }

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1, viennagrid_int i2)
  {
    viennagrid_int tmp[3];
    viennagrid_dimension dimensions[3];
    std::fill(dimensions, dimensions+3, 0);

    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    tmp[2] = vertex_indices[i2];
    return get_make_element(element_tag, tmp, dimensions, 3);
  }

  viennagrid_index get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_index * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1, viennagrid_int i2, viennagrid_int i3)
  {
    viennagrid_int tmp[4];
    viennagrid_dimension dimensions[4];
    std::fill(dimensions, dimensions+4, 0);

    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    tmp[2] = vertex_indices[i2];
    tmp[3] = vertex_indices[i3];
    return get_make_element(element_tag, tmp, dimensions, 4);
  }



  viennagrid_element_buffer element_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int element_counts[VIENNAGRID_ELEMENT_TAG_COUNT];

  viennagrid_dimension geometric_dimension_;
  viennagrid_dimension cell_dimension_;

//   viennagrid_element_tag cell_tag_;
  std::vector<viennagrid_mesh> meshes_;
  viennagrid_mesh root_;

  std::vector<viennagrid_numeric> vertex_buffer;
  ViennaGridHolePointBufferType hole_point_buffer;

  std::vector<viennagrid_region> regions;
  viennagrid_index highest_region_id;
  std::map<viennagrid_index, viennagrid_index> region_id_map;

  viennagrid_int change_counter_;
  mutable viennagrid_int use_count_;
  mutable viennagrid_int retain_release_count;
};

#endif
