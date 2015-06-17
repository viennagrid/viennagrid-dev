#ifndef VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP
#define VIENNAGRID_BACKEND_MESH_HIERARCHY_HPP

#include <algorithm>
#include <sstream>
#include <deque>

#include "viennagrid/viennagrid.h"
#include "buffer.hpp"
#include "mesh.hpp"
#include "region.hpp"



typedef dense_packed_multibuffer<viennagrid_int, viennagrid_int> ViennaGridBoundaryBufferType;
typedef dense_packed_multibuffer<viennagrid_int, viennagrid_numeric> ViennaGridHolePointBufferType;
typedef dense_packed_multibuffer<viennagrid_int, viennagrid_region> ViennaGridRegionBufferType;


struct element_key
{
  element_key() {}
  element_key(viennagrid_int * indices, viennagrid_int index_count) : vertex_indices(index_count)
  {
    std::copy( indices, indices+index_count, &vertex_indices[0] );
    std::sort( vertex_indices.begin(), vertex_indices.end()   );
  }

  bool operator<(element_key const & rhs) const
  { return vertex_indices < rhs.vertex_indices; }

  bool operator==(element_key const & rhs) const
  { return vertex_indices == rhs.vertex_indices; }

  bool operator!=(element_key const & rhs) const
  { return !(*this == rhs); }

  viennagrid_int front() const { return vertex_indices.front(); }

  std::vector<viennagrid_int> vertex_indices;
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


  viennagrid_element_tag element_tag(viennagrid_int element_id) const
  {
    return element_tags[element_id];
  }

  viennagrid_int * boundary_begin(viennagrid_int boundary_topo_dim, viennagrid_int element_id)
  {
    return boundary_buffer(boundary_topo_dim).begin(element_id);
  }

  viennagrid_int * boundary_end(viennagrid_int boundary_topo_dim, viennagrid_int element_id)
  {
    return boundary_buffer(boundary_topo_dim).end(element_id);
  }


  viennagrid_int parent_id(viennagrid_int element_id) const { return parents[element_id]; }
  void set_parent_id(viennagrid_int element_id, viennagrid_int parent_id) { parents[element_id] = parent_id; }

  void add_to_region(viennagrid_int element_id, viennagrid_region region_)
  {
    for (viennagrid_region * it = regions_begin(element_id); it != regions_end(element_id); ++it)
    {
      if (*it == region_)
        return;
    }

    region_buffer[element_id].push_back(region_);
  }

  viennagrid_region * regions_begin(viennagrid_int element_id) { return &region_buffer[element_id][0]; }
  viennagrid_region * regions_end(viennagrid_int element_id) { return &region_buffer[element_id][0] + region_buffer[element_id].size(); }


  void clear(viennagrid_mesh_hierarchy_ * mesh_hierarchy_)
  {
    element_tags.clear();
    parents.clear();
    region_buffer.clear();

    boundary_indices.clear();
    element_map.clear();

    mesh_hierarchy = mesh_hierarchy_;
  }

  viennagrid_int size() const { return parents.size(); }
  viennagrid_element_tag * element_tags_pointer() { return &element_tags[0]; }
  viennagrid_int * vertex_offsets_pointer() { return boundary_buffer(0).offset_pointer(); }
  viennagrid_int * vertex_indices_pointer() { return boundary_buffer(0).values_pointer(); }
  viennagrid_int * parent_id_pointer() { return &parents[0]; }
//   viennagrid_int * region_offsets_pointer() { return region_buffer.offset_pointer(); }
  viennagrid_int * region_offsets_pointer() { return 0; }

  ViennaGridBoundaryBufferType & boundary_buffer(viennagrid_dimension boundary_topo_dim)
  {
    return boundary_indices[boundary_topo_dim];
  }

private:

  viennagrid_int get_element(element_key const & key)
  {
    if (key.front() >= static_cast<viennagrid_int>(element_map.size()))
      return -1;

    std::map<element_key, viennagrid_int>::iterator it = element_map[key.front()].find(key);
    if (it == element_map[key.front()].end())
      return -1;

    return it->second;
  }

  viennagrid_int get_element(viennagrid_int * indices, viennagrid_int index_count)
  {
    return get_element( element_key(indices, index_count) );
  }

  viennagrid_int make_element(viennagrid_mesh_hierarchy mesh_hierarchy,
                                viennagrid_element_tag element_tag_,
                                viennagrid_int * vertex_indices,
                                viennagrid_int vertex_count,
                                bool reserve_boundary);

  void reserve_boundary(viennagrid_int element_id);
  void make_boundary(viennagrid_int element_id, viennagrid_mesh mesh);


  // the current topologic dimension
  viennagrid_dimension topologic_dimension;

  // tags of the element
  std::vector<viennagrid_element_tag> element_tags;

  // element parent
  std::vector<viennagrid_int> parents;

  // explicit element or implicit?
  std::vector<viennagrid_flag> element_flags;

  // aux_data for elements
//   std::vector<void*> aux_data;

  // region of elements
  std::vector< std::vector<viennagrid_region> > region_buffer;

  // boundary indices
  std::vector<ViennaGridBoundaryBufferType> boundary_indices;

  // element map (based on vertices)
  typedef std::map<element_key, viennagrid_int> ElementMapType;
  std::deque<ElementMapType> element_map;

  // base pointer
  viennagrid_mesh_hierarchy_ * mesh_hierarchy;
};





struct viennagrid_mesh_hierarchy_
{
  friend struct viennagrid_element_buffer;

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


  viennagrid_element_buffer & element_buffer(viennagrid_int topo_dim)
  { return element_buffers[topo_dim]; }



  viennagrid_int boundary_layout() const { return boundary_layout_; }
  void set_boundary_layout(viennagrid_int boundary_layout_in);
  bool full_boundary_layout() const { return boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_FULL; }



  viennagrid_int * boundary_begin(viennagrid_dimension element_topo_dim,
                                    viennagrid_int element_id,
                                    viennagrid_dimension boundary_topo_dim)
  { return element_buffer(element_topo_dim).boundary_begin(boundary_topo_dim, element_id); }
  viennagrid_int * boundary_end(viennagrid_dimension element_topo_dim,
                                  viennagrid_int element_id,
                                  viennagrid_dimension boundary_topo_dim)
  { return element_buffer(element_topo_dim).boundary_end(boundary_topo_dim, element_id); }


  viennagrid_int * make_boundary_indices(viennagrid_element_tag host_tag,
                                           viennagrid_element_tag boundary_tag,
                                           viennagrid_int count)
  {
    return element_buffer(viennagrid_topological_dimension(host_tag)).boundary_buffer(viennagrid_topological_dimension(boundary_tag)).push_back(count);
  }

  viennagrid_int * make_boundary_indices(viennagrid_element_tag host_tag,
                                           viennagrid_element_tag boundary_tag)
  {
    return make_boundary_indices(host_tag, boundary_tag,
                                 viennagrid_boundary_element_count_from_element_tag(host_tag, boundary_tag));
  }



  viennagrid_int make_vertex(const viennagrid_numeric * coords)
  {
    viennagrid_int id = element_buffer(0).make_element(this, VIENNAGRID_ELEMENT_TAG_VERTEX, 0, 0, false);

    viennagrid_int prev_size = vertex_buffer.size();
    vertex_buffer.resize( vertex_buffer.size() + geometric_dimension() );

    if (coords)
      std::copy( coords, coords+geometric_dimension(), &vertex_buffer[0] + prev_size );

    return id;
  }

  viennagrid_numeric * get_vertex_pointer()
  {
    return &vertex_buffer[0];
  }

  viennagrid_numeric * get_vertex(viennagrid_int id)
  {
    return &vertex_buffer[id * geometric_dimension()];
  }

  viennagrid_int vertex_count() const
  {
    return vertex_buffer.size() / geometric_dimension();
  }



  std::pair<viennagrid_int, bool> get_make_element(viennagrid_element_tag element_tag,
                                    viennagrid_int * vertex_indices,
                                    viennagrid_int vertex_count,
                                    viennagrid_mesh mesh,
                                    bool make_boundary = true);





  viennagrid_int delete_element_simple(viennagrid_dimension element_topo_dim, viennagrid_int element_id);
  viennagrid_int delete_element(viennagrid_dimension element_topo_dim, viennagrid_int element_id);

  viennagrid_int region_count()
  {
    return regions.size();
  }

  viennagrid_region get_region(viennagrid_int region_id)
  {
    std::map<viennagrid_int, viennagrid_int>::const_iterator it = region_id_map.find(region_id);
    if (it != region_id_map.end())
      return regions[it->second];
    return NULL;
  }

  viennagrid_region get_make_region(viennagrid_int region_id)
  {
    std::map<viennagrid_int, viennagrid_int>::iterator it = region_id_map.find(region_id);
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

  bool is_in_region(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_int region_id)
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
//     boundary_layout_ = VIENNAGRID_BOUNDARY_LAYOUT_SPARSE;
    boundary_layout_ = VIENNAGRID_BOUNDARY_LAYOUT_FULL;

    for (int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
    {
      element_buffer(i).clear(this);
      element_buffer(i).set_topologic_dimension(i);
      element_counts[i] = 0;
    }

    geometric_dimension_ = 0;
    cell_dimension_ = -1;

    root_->clear();
    meshes_.clear();
    meshes_.push_back(root());

    vertex_buffer.clear();

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
  viennagrid_mesh mesh(viennagrid_int i) { return meshes_[i]; }



private:



  viennagrid_int get_make_line(viennagrid_int * vertex_indices, viennagrid_int vi0, viennagrid_int vi1, viennagrid_mesh mesh)
  {
    viennagrid_int tmp[2] = { vertex_indices[vi0], vertex_indices[vi1] };
    return get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, tmp, 2, mesh, false).first;
  }

  viennagrid_int get_make_triangle(
        viennagrid_int * vertex_indices, viennagrid_int vi0, viennagrid_int vi1, viennagrid_int vi2,
        viennagrid_int * line_indices, viennagrid_int li0, viennagrid_int li1, viennagrid_int li2,
        viennagrid_mesh mesh)
  {
    viennagrid_int tmp[3] = { vertex_indices[vi0], vertex_indices[vi1], vertex_indices[vi2] };

    std::pair<viennagrid_int, bool> triangle = get_make_element(VIENNAGRID_ELEMENT_TAG_TRIANGLE, tmp, 3, mesh, false);

    if (triangle.second && full_boundary_layout())
    {
      viennagrid_int * local_line_indices = boundary_begin(2, triangle.first, 1);
      local_line_indices[0] = line_indices[li0];
      local_line_indices[1] = line_indices[li1];
      local_line_indices[2] = line_indices[li2];
    }

    return triangle.first;
//     return get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, tmp, 2, mesh, false).first;
  }



  std::pair<viennagrid_int, bool> get_make_element_2(viennagrid_element_tag element_tag,
                                    viennagrid_int * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1,
                                    viennagrid_mesh mesh,
                                    bool make_boundary = true)
  {
    viennagrid_int tmp[2];

    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    return get_make_element(element_tag, tmp, 2, mesh, make_boundary);
  }

  std::pair<viennagrid_int, bool> get_make_element_3(viennagrid_element_tag element_tag,
                                    viennagrid_int * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1, viennagrid_int i2,
                                    viennagrid_mesh mesh,
                                    bool make_boundary = true)
  {
    viennagrid_int tmp[3];

    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    tmp[2] = vertex_indices[i2];
    return get_make_element(element_tag, tmp, 3, mesh, make_boundary);
  }

  std::pair<viennagrid_int, bool> get_make_element_4(viennagrid_element_tag element_tag,
                                    viennagrid_int * vertex_indices,
                                    viennagrid_int i0, viennagrid_int i1, viennagrid_int i2, viennagrid_int i3,
                                    viennagrid_mesh mesh,
                                    bool make_boundary = true)
  {
    viennagrid_int tmp[4];

    tmp[0] = vertex_indices[i0];
    tmp[1] = vertex_indices[i1];
    tmp[2] = vertex_indices[i2];
    tmp[3] = vertex_indices[i3];
    return get_make_element(element_tag, tmp, 4, mesh, make_boundary);
  }


  viennagrid_int boundary_layout_;


  viennagrid_element_buffer element_buffers[VIENNAGRID_TOPOLOGIC_DIMENSION_END];
  viennagrid_int element_counts[VIENNAGRID_ELEMENT_TAG_COUNT];

  viennagrid_dimension geometric_dimension_;
  viennagrid_dimension cell_dimension_;

  std::vector<viennagrid_mesh> meshes_;
  viennagrid_mesh root_;

  std::vector<viennagrid_numeric> vertex_buffer;

  std::vector<viennagrid_region> regions;
  viennagrid_int highest_region_id;
  std::map<viennagrid_int, viennagrid_int> region_id_map;

  viennagrid_int change_counter_;
  mutable viennagrid_int use_count_;
  mutable viennagrid_int retain_release_count;
};

#endif
