#ifndef VIENNAGRID_PLC_HPP
#define VIENNAGRID_PLC_HPP

#include <vector>
#include <set>
#include <string>
#include <algorithm>


#include "viennagrid/viennagrid.h"
#include "buffer.hpp"
#include "common.hpp"


struct viennagrid_plc_
{
public:
  viennagrid_plc_() : geometric_dimension_(0), reference_counter(1) {}


  viennagrid_dimension geometric_dimension() const { return geometric_dimension_; }
  void set_geometric_dimension(viennagrid_dimension geometric_dimension_in);

  viennagrid_int element_count(int topologic_dimension) const;

  viennagrid_element_id make_vertex(const viennagrid_numeric * coords)
  {
    viennagrid_int id = vertex_count();
    vertex_buffer.resize( vertex_buffer.size() + geometric_dimension() );
    if (coords)
      std::copy(coords, coords + geometric_dimension(), get_vertex(id));
    return ID(0, id);
  }

  viennagrid_numeric * get_vertex(viennagrid_int vertex_id)
  {
    return &vertex_buffer[vertex_id*geometric_dimension()];
  }

  viennagrid_numeric * vertex_pointer()
  {
    return &vertex_buffer[0];
  }


  viennagrid_element_id get_make_line(viennagrid_int vertex_id0, viennagrid_int vertex_id1);
  viennagrid_element_id get_make_facet(viennagrid_int line_count, viennagrid_int * line_ids);


  viennagrid_element_id * boundary_ptr(viennagrid_dimension topologic_dimension,
                                       viennagrid_dimension boundary_topologic_dimension);
  viennagrid_int * boundary_offsets(viennagrid_dimension topologic_dimension,
                                    viennagrid_dimension boundary_topologic_dimension);


  viennagrid_element_id * boundary_begin(viennagrid_int id,
                                         viennagrid_dimension boundary_topologic_dimension);
  viennagrid_element_id * boundary_end(viennagrid_int id,
                                       viennagrid_dimension boundary_topologic_dimension);





  void add_facet_hole_point(viennagrid_element_id id, const viennagrid_numeric * coords)
  {
    for (viennagrid_dimension i = 0; i != geometric_dimension(); ++i)
      facet_hole_points_.add(INDEX(id), coords[+i]);
  }

  void delete_facet_hole_point(viennagrid_element_id id, viennagrid_int point_id)
  {
    facet_hole_points_.erase(INDEX(id), point_id*geometric_dimension(), (point_id+1)*geometric_dimension());
  }

  viennagrid_int facet_hole_point_count(viennagrid_element_id id)
  {
    return facet_hole_points_.size(INDEX(id)) / geometric_dimension();
  }

  viennagrid_numeric * facet_hole_points(viennagrid_element_id id)
  {
    return facet_hole_points_.begin(INDEX(id));
  }



  viennagrid_error read_tetgen_poly(std::string const & filename);
  viennagrid_error write_tetgen_poly(std::string const & filename);


  viennagrid_int hole_point_count()
  {
    return holes_points_.size() / geometric_dimension();
  }

  viennagrid_numeric * get_hole_points()
  {
    return holes_points_.empty() ? NULL : &holes_points_[0];
  }

  void add_hole_point(const viennagrid_numeric * coords)
  {
    viennagrid_int id = hole_point_count();
    holes_points_.resize( holes_points_.size() + geometric_dimension() );

    if (coords)
      std::copy(coords, coords + geometric_dimension(), &holes_points_[id*geometric_dimension()]);
  }




  viennagrid_int seed_point_count()
  {
    return seed_point_regions_.size();
  }

  viennagrid_numeric * get_seed_points()
  {
    return seed_points_.empty() ? NULL : &seed_points_[0];
  }

  viennagrid_int * get_seed_point_regions()
  {
    return seed_point_regions_.empty() ? NULL : &seed_point_regions_[0];
  }

  void add_seed_point(const viennagrid_numeric * coords, viennagrid_int region_id)
  {
    viennagrid_int id = seed_point_count();
    seed_points_.resize( seed_points_.size() + geometric_dimension() );
    seed_point_regions_.push_back(region_id);

    if (coords)
      std::copy(coords, coords + geometric_dimension(), &seed_points_[id*geometric_dimension()]);
  }


  bool element_id_valid(viennagrid_element_id element_id) const
  {
    return viennagrid_topological_dimension_valid(TOPODIM(element_id)) &&
           (INDEX(element_id) >= 0) &&
           (INDEX(element_id) < element_count(TOPODIM(element_id)));
  }


  void clear()
  {
    geometric_dimension_ = 0;
    vertex_buffer.clear();

    line_vertices_.clear();

    facet_vertices_.clear();
    facet_lines_.clear();
    facet_hole_points_.clear();

    holes_points_.clear();

    seed_points_.clear();
    seed_point_regions_.clear();
  }

private:

  viennagrid_int vertex_count() const
  {
    return vertex_buffer.size() / geometric_dimension();
  }

  std::vector<viennagrid_numeric> vertex_buffer;
  viennagrid_dimension geometric_dimension_;

  dense_packed_multibuffer<viennagrid_int, viennagrid_element_id> line_vertices_;

  dense_packed_multibuffer<viennagrid_int, viennagrid_element_id> facet_vertices_;
  dense_packed_multibuffer<viennagrid_int, viennagrid_element_id> facet_lines_;
  dense_packed_multibuffer<viennagrid_int, viennagrid_numeric> facet_hole_points_;

  std::vector<viennagrid_numeric> holes_points_;

  std::vector<viennagrid_numeric> seed_points_;
  std::vector<viennagrid_int> seed_point_regions_;

public:
  viennagrid_int reference_counter;
};


#endif
