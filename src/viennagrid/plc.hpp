#ifndef VIENNAGRID_PLC_HPP
#define VIENNAGRID_PLC_HPP

#include <vector>
#include <set>
#include <string>
#include <algorithm>


#include "viennagrid/viennagrid.h"
#include "buffer.hpp"


struct viennagrid_plc_
{
public:
  viennagrid_plc_() : geometric_dimension_(0) {}


  viennagrid_dimension geometric_dimension() { return geometric_dimension_; }
  void set_geometric_dimension(viennagrid_dimension geometric_dimension_in);

  viennagrid_int element_count(int topologic_dimension);

  viennagrid_int make_vertex(const viennagrid_numeric * coords)
  {
    viennagrid_int id = vertex_count();
    vertex_buffer.resize( vertex_buffer.size() + geometric_dimension() );
    if (coords)
      std::copy(coords, coords + geometric_dimension(), get_vertex(id));
    return id;
  }

  viennagrid_numeric * get_vertex(viennagrid_int vertex_id)
  {
    return &vertex_buffer[vertex_id*geometric_dimension()];
  }

  viennagrid_numeric * vertex_pointer()
  {
    return &vertex_buffer[0];
  }


  viennagrid_int get_make_line(viennagrid_int vertex_index0, viennagrid_int vertex_index1);
  viennagrid_int get_make_facet(viennagrid_int * line_indices, viennagrid_int line_count);


  viennagrid_int * boundary_begin(viennagrid_dimension topologic_dimension,
                                    viennagrid_int id,
                                    viennagrid_dimension boundary_topologic_dimension);
  viennagrid_int * boundary_end(viennagrid_dimension topologic_dimension,
                                  viennagrid_int id,
                                  viennagrid_dimension boundary_topologic_dimension);





  void add_facet_hole_point(viennagrid_int id, const viennagrid_numeric * coords)
  {
    for (viennagrid_dimension i = 0; i != geometric_dimension(); ++i)
      facet_hole_points_.add(id, coords[i]);
  }

  void delete_facet_hole_point(viennagrid_int id, viennagrid_int point_index)
  {
    facet_hole_points_.erase(id, point_index*geometric_dimension(), (point_index+1)*geometric_dimension());
  }

  viennagrid_int facet_hole_point_count(viennagrid_int id)
  {
    return facet_hole_points_.size(id) / geometric_dimension();
  }

  viennagrid_numeric * facet_hole_points(viennagrid_int id)
  {
    return facet_hole_points_.begin(id);
  }



  viennagrid_int read_tetgen_poly(std::string const & filename);


  viennagrid_int hole_point_count()
  {
    return holes_points_.size() / geometric_dimension();
  }

  viennagrid_numeric * get_hole_point(viennagrid_int id)
  {
    return &holes_points_[id*geometric_dimension()];
  }

  void add_hole_point(const viennagrid_numeric * coords)
  {
    viennagrid_int id = hole_point_count();
    holes_points_.resize( holes_points_.size() + geometric_dimension() );

    if (coords)
      std::copy(coords, coords + geometric_dimension(), get_hole_point(id));
  }


  viennagrid_int seed_point_count()
  {
    return seed_point_regions_.size();
  }

  viennagrid_numeric * get_seed_point(viennagrid_int id)
  {
    return &seed_points_[id*geometric_dimension()];
  }

  viennagrid_int get_seed_point_region(viennagrid_int id)
  {
    return seed_point_regions_[id];
  }

  void add_seed_point(const viennagrid_numeric * coords, viennagrid_int region_id)
  {
    viennagrid_int id = seed_point_count();
    seed_points_.resize( seed_points_.size() + geometric_dimension() );
    seed_point_regions_.push_back(region_id);

    if (coords)
      std::copy(coords, coords + geometric_dimension(), get_seed_point(id));
  }




private:

  viennagrid_int vertex_count()
  {
    return vertex_buffer.size() / geometric_dimension();
  }

  std::vector<viennagrid_numeric> vertex_buffer;
  viennagrid_dimension geometric_dimension_;

  dense_packed_multibuffer<viennagrid_int, viennagrid_int> line_vertices_;

  dense_packed_multibuffer<viennagrid_int, viennagrid_int> facet_vertices_;
  dense_packed_multibuffer<viennagrid_int, viennagrid_int> facet_lines_;
  dense_packed_multibuffer<viennagrid_int, viennagrid_numeric> facet_hole_points_;

  std::vector<viennagrid_numeric> holes_points_;

  std::vector<viennagrid_numeric> seed_points_;
  std::vector<viennagrid_int> seed_point_regions_;
};


#endif
