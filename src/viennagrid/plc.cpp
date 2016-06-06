#include "plc.hpp"

#include <fstream>
#include <sstream>
#include <cmath>

#include "common.hpp"



viennagrid_error viennagrid_plc_create(viennagrid_plc * plc)
{
  if (plc)
    *plc = new viennagrid_plc_;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_retain(viennagrid_plc plc)
{
  retain(plc);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_release(viennagrid_plc plc)
{
  release(plc);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_copy(viennagrid_plc src_plc,
                                     viennagrid_plc dst_plc)
{
  if (!src_plc) return VIENNAGRID_ERROR_INVALID_PLC;

  if (dst_plc)
    *dst_plc = *src_plc;

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_plc_clear(viennagrid_plc plc)
{
  if (plc)
    plc->clear();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                              viennagrid_plc dst_plc,
                                              viennagrid_bool copy_hole_points,
                                              viennagrid_bool copy_seed_points)
{
  if (!src_plc) return VIENNAGRID_ERROR_INVALID_PLC;

  viennagrid_dimension geometric_dimension;
  viennagrid_plc_geometric_dimension_get(src_plc, &geometric_dimension);

  viennagrid_plc_clear(dst_plc);
  viennagrid_plc_geometric_dimension_set(dst_plc, geometric_dimension);

  if (copy_hole_points == VIENNAGRID_TRUE)
  {
    viennagrid_numeric * hole_points;
    viennagrid_int hole_point_count;
    viennagrid_plc_volumetric_hole_points_get(src_plc, &hole_point_count, &hole_points);
    for (viennagrid_int i = 0; i != hole_point_count; ++i)
      viennagrid_plc_volumetric_hole_point_add(dst_plc, hole_points + i*geometric_dimension);
  }

  if (copy_seed_points == VIENNAGRID_TRUE)
  {
    viennagrid_numeric * seed_points;
    viennagrid_int * seed_point_regions;
    viennagrid_int seed_point_count;
    viennagrid_plc_seed_points_get(src_plc, &seed_point_count, &seed_points, &seed_point_regions);
    for (viennagrid_int i = 0; i != seed_point_count; ++i)
      viennagrid_plc_seed_point_add(dst_plc, seed_points + i*geometric_dimension, seed_point_regions[i]);
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                        viennagrid_dimension * geometric_dimension)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  if (geometric_dimension)
    *geometric_dimension = plc->geometric_dimension();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                        viennagrid_int geometric_dimension)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  plc->set_geometric_dimension(geometric_dimension);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_element_count(viennagrid_plc plc,
                                              viennagrid_dimension topological_dimension,
                                              viennagrid_int * element_count)
{
  if (!plc)                                                           return VIENNAGRID_ERROR_INVALID_PLC;
  if (!viennagrid_topological_dimension_valid(topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

  if (element_count)
    *element_count = plc->element_count(topological_dimension);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_elements_get(viennagrid_plc plc,
                                             viennagrid_dimension topological_dimension,
                                             viennagrid_element_id * start_id,
                                             viennagrid_element_id * end_id)
{
  if (!plc)                                                           return VIENNAGRID_ERROR_INVALID_PLC;
  if (!viennagrid_topological_dimension_valid(topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

  if (start_id)
    *start_id = ID(topological_dimension, 0);

  if (end_id)
    *end_id = ID(topological_dimension, plc->element_count(topological_dimension));

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_vertex_create(viennagrid_plc plc,
                                              const viennagrid_numeric * coords,
                                              viennagrid_element_id * vertex_id)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  viennagrid_element_id id = plc->make_vertex(coords);

  if (vertex_id)
    *vertex_id = id;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_vertex_coords_pointer(viennagrid_plc plc,
                                                      viennagrid_numeric ** coords)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  if (coords)
    *coords = plc->vertex_pointer();

  return VIENNAGRID_SUCCESS;
}
viennagrid_error viennagrid_plc_vertex_coords_get(viennagrid_plc plc,
                                                  viennagrid_int id,
                                                  viennagrid_numeric ** coords)
{
  if (!plc)                                      return VIENNAGRID_ERROR_INVALID_PLC;
  if ((id < 0) || (id >= plc->element_count(0))) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  if (coords)
    *coords = plc->get_vertex(id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                            viennagrid_int vertex_id0,
                                            viennagrid_int vertex_id1,
                                            viennagrid_int * line_id)
{
  if (!plc)                                                      return VIENNAGRID_ERROR_INVALID_PLC;
  if ((vertex_id0 < 0) || (vertex_id0 >= plc->element_count(0))) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if ((vertex_id1 < 0) || (vertex_id1 >= plc->element_count(0))) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  viennagrid_int id = plc->get_make_line(vertex_id0, vertex_id1);

  if (line_id)
    *line_id = id;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                             viennagrid_int line_count,
                                             viennagrid_int * line_ids,
                                             viennagrid_int * facet_id)
{
  if (!plc)           return VIENNAGRID_ERROR_INVALID_PLC;
  if (line_count < 0) return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
  if (!line_ids)      return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  viennagrid_int id = plc->get_make_facet(line_count, line_ids);

  if (facet_id)
    *facet_id = id;

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_boundary_element_pointer(viennagrid_plc plc,
                                                         viennagrid_dimension element_topological_dimension,
                                                         viennagrid_dimension boundary_topological_dimension,
                                                         viennagrid_int ** boundary_offsets,
                                                         viennagrid_int ** boundary_element_ptr)
{
  if (!plc)                                                       return VIENNAGRID_ERROR_INVALID_PLC;
  if (!viennagrid_topological_dimension_valid(element_topological_dimension))  return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!viennagrid_topological_dimension_valid(boundary_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

  if (boundary_offsets)
    *boundary_offsets = plc->boundary_offsets(element_topological_dimension, boundary_topological_dimension);

  if (boundary_element_ptr)
    *boundary_element_ptr = plc->boundary_ptr(element_topological_dimension, boundary_topological_dimension);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                  viennagrid_element_id element_id,
                                                  viennagrid_dimension boundary_topological_dimension,
                                                  viennagrid_int ** boundary_element_ids_begin,
                                                  viennagrid_int ** boundary_element_ids_end)
{
  if (!plc)                                                                    return VIENNAGRID_ERROR_INVALID_PLC;
  if (!plc->element_id_valid(element_id))                                      return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!viennagrid_topological_dimension_valid(boundary_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

  if (boundary_element_ids_begin)
    *boundary_element_ids_begin = plc->boundary_begin(element_id, boundary_topological_dimension);

  if (boundary_element_ids_end)
    *boundary_element_ids_end = plc->boundary_end(element_id, boundary_topological_dimension);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_point_add(viennagrid_plc plc,
                                                     viennagrid_int facet_id,
                                                     const viennagrid_numeric * coords)
{
  if (!plc)                             return VIENNAGRID_ERROR_INVALID_PLC;
  if (!plc->element_id_valid(facet_id)) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  plc->add_facet_hole_point(facet_id, coords);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_point_delete(viennagrid_plc plc,
                                                        viennagrid_int facet_id,
                                                        viennagrid_int point_id)
{
  if (!plc)                                                                  return VIENNAGRID_ERROR_INVALID_PLC;
  if (!plc->element_id_valid(facet_id))                                      return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if ((point_id < 0) || (point_id >= plc->facet_hole_point_count(facet_id))) return VIENNAGRID_ERROR_INVALID_FACET_HOLE_POINT_INDEX;

  plc->delete_facet_hole_point(facet_id, point_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_points_get(viennagrid_plc plc,
                                                      viennagrid_int facet_id,
                                                      viennagrid_int * hole_point_count,
                                                      viennagrid_numeric ** coords)
{
  if (!plc)                             return VIENNAGRID_ERROR_INVALID_PLC;
  if (!plc->element_id_valid(facet_id)) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  if (coords)
    *coords = plc->facet_hole_points(facet_id);

  if (hole_point_count)
    *hole_point_count = plc->facet_hole_point_count(facet_id);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_volumetric_hole_point_add(viennagrid_plc plc,
                                                          const viennagrid_numeric * coords)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  plc->add_hole_point(coords);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_volumetric_hole_points_get(viennagrid_plc plc,
                                                           viennagrid_int * count,
                                                           viennagrid_numeric ** coords)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  if (coords)
    *coords = plc->get_hole_points();

  if (count)
    *count = plc->hole_point_count();

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_plc_seed_point_add(viennagrid_plc plc,
                                               const viennagrid_numeric * coords,
                                               viennagrid_int region_id)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  plc->add_seed_point(coords, region_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_seed_points_get(viennagrid_plc plc,
                                                viennagrid_int * count,
                                                viennagrid_numeric ** coords,
                                                viennagrid_int ** region_ids)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  if (coords)
    *coords = plc->get_seed_points();

  if (region_ids)
    *region_ids = plc->get_seed_point_regions();

  if (count)
    *count = plc->seed_point_count();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_read_tetgen_poly(viennagrid_plc plc,
                                                 const char * filename)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  return plc->read_tetgen_poly(filename);
}

viennagrid_error viennagrid_plc_write_tetgen_poly(viennagrid_plc plc,
                                                 const char * filename)
{
  if (!plc) return VIENNAGRID_ERROR_INVALID_PLC;

  return plc->write_tetgen_poly(filename);
}

viennagrid_error viennagrid_plc_line_refine(viennagrid_plc plc,
                                            viennagrid_plc output_plc,
                                            viennagrid_numeric line_size)
{
  if (!plc)          return VIENNAGRID_ERROR_INVALID_PLC;
  if (line_size < 0) return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  if (!output_plc)
    return VIENNAGRID_SUCCESS;

  viennagrid_dimension geometric_dimension;
  viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

  viennagrid_plc_init_from_plc(plc, output_plc, VIENNAGRID_TRUE, VIENNAGRID_TRUE);

  viennagrid_int vertex_count;
  viennagrid_plc_element_count(plc, 0, &vertex_count);

  viennagrid_int line_count;
  viennagrid_plc_element_count(plc, 1, &line_count);

  viennagrid_int facet_count;
  viennagrid_plc_element_count(plc, 2, &facet_count);

  std::vector<viennagrid_element_id> vertex_map(vertex_count);
  std::vector< std::vector<viennagrid_element_id> > line_to_lines_map(line_count);

  viennagrid_element_id vertices_begin;
  viennagrid_element_id vertices_end;
  viennagrid_plc_elements_get(plc, 0, &vertices_begin, &vertices_end);

  for (viennagrid_element_id vertex_id = vertices_begin; vertex_id != vertices_end; ++vertex_id)
  {
    viennagrid_numeric * pt;
    viennagrid_plc_vertex_coords_get(plc, vertex_id, &pt);
    viennagrid_plc_vertex_create(output_plc, pt, &vertex_map[INDEX(vertex_id)]);
  }


  viennagrid_element_id lines_begin;
  viennagrid_element_id lines_end;
  viennagrid_plc_elements_get(plc, 1, &lines_begin, &lines_end);

  for (viennagrid_element_id line_id = lines_begin; line_id != lines_end; ++line_id)
  {
    viennagrid_element_id * line_vertices_begin;
    viennagrid_element_id * line_vertices_end;
    viennagrid_plc_boundary_elements(plc, line_id, 0, &line_vertices_begin, &line_vertices_end);

    viennagrid_element_id v0 = *(line_vertices_begin+0);
    viennagrid_element_id v1 = *(line_vertices_begin+1);

    viennagrid_numeric * p0;
    viennagrid_numeric * p1;
    viennagrid_plc_vertex_coords_get(plc, v0, &p0);
    viennagrid_plc_vertex_coords_get(plc, v1, &p1);

    viennagrid_element_id nv0 = vertex_map[INDEX(v0)];
    viennagrid_element_id nv1 = vertex_map[INDEX(v1)];

    std::vector<viennagrid_element_id> new_lines;

    viennagrid_numeric current_line_size;
    viennagrid_point_distance_2(geometric_dimension, p0, p1, &current_line_size);

    if (current_line_size <= line_size)
    {
      viennagrid_element_id line_id;
      viennagrid_plc_line_create( output_plc, nv0, nv1, &line_id );
      new_lines.push_back(line_id);
    }
    else
    {
      int number_of_new_lines = std::ceil(current_line_size / line_size);
      assert(number_of_new_lines > 1);

      std::vector<viennagrid_numeric> offset(geometric_dimension);
      viennagrid_point_subtract(geometric_dimension, p1, p0, &offset[0]);
      viennagrid_point_prod(geometric_dimension, &offset[0], 1.0/number_of_new_lines, &offset[0]);

      std::vector<viennagrid_numeric> tmp(geometric_dimension);
      viennagrid_point_copy(geometric_dimension, p0, &tmp[0]);

      std::vector<viennagrid_element_id> vertices;
      vertices.push_back(nv0);
      for (int i = 1; i != number_of_new_lines; ++i)
      {
        viennagrid_point_add(geometric_dimension, &tmp[0], &offset[0], &tmp[0]);
        viennagrid_element_id vertex_id;
        viennagrid_plc_vertex_create( output_plc, &tmp[0], &vertex_id );
        vertices.push_back(vertex_id);
      }
      vertices.push_back(nv1);

      for (std::size_t i = 0; i != vertices.size()-1; ++i)
      {
        viennagrid_element_id line_id;
        viennagrid_plc_line_create( output_plc, vertices[i], vertices[i+1], &line_id );
        new_lines.push_back(line_id);
      }
    }

    line_to_lines_map[ INDEX(line_id) ] = new_lines;
  }


  viennagrid_element_id facets_begin;
  viennagrid_element_id facets_end;
  viennagrid_plc_elements_get(plc, 2, &facets_begin, &facets_end);

  for (viennagrid_element_id facet_id = facets_begin; facet_id != facets_end; ++facet_id)
  {
    std::vector<viennagrid_element_id> new_lines;

    viennagrid_element_id * lines_begin;
    viennagrid_element_id * lines_end;
    viennagrid_plc_boundary_elements(plc, facet_id, 1, &lines_begin, &lines_end);

    for (viennagrid_element_id * line_it = lines_begin; line_it != lines_end; ++line_it)
    {
      std::vector<viennagrid_element_id> const & current_new_lines = line_to_lines_map[ INDEX(*line_it) ];
      std::copy( current_new_lines.begin(), current_new_lines.end(), std::back_inserter(new_lines) );
    }

    viennagrid_element_id new_facet_id;
    viennagrid_plc_facet_create(output_plc, new_lines.size(), &new_lines[0], &new_facet_id);

    viennagrid_numeric * facet_hole_points;
    viennagrid_int facet_hole_point_count;
    viennagrid_plc_facet_hole_points_get(plc, facet_id, &facet_hole_point_count, &facet_hole_points);
    for (viennagrid_int i = 0; i != facet_hole_point_count; ++i)
      viennagrid_plc_facet_hole_point_add(plc, new_facet_id, facet_hole_points + i*geometric_dimension);
  }

  return VIENNAGRID_SUCCESS;
}

















void viennagrid_plc_::set_geometric_dimension(viennagrid_dimension geometric_dimension_in)
{
  if (geometric_dimension() == geometric_dimension_in)
    return;

  viennagrid_int vertex_count = 0;
  if (geometric_dimension() != 0)
    vertex_count = vertex_buffer.size() / geometric_dimension();

  geometric_dimension_ = geometric_dimension_in;
  vertex_buffer.resize( geometric_dimension() * vertex_count );
}

viennagrid_int viennagrid_plc_::element_count(int topological_dimension) const
{
  if (topological_dimension == 0)
    return vertex_count();

  if (topological_dimension == 1)
    return line_vertices_.size();

  if (topological_dimension == 2)
    return facet_vertices_.size();

  assert(false);
  return -1;
}



viennagrid_element_id viennagrid_plc_::get_make_line(viennagrid_element_id vertex_id0, viennagrid_element_id vertex_id1)
{
  if (vertex_id1 < vertex_id0)
    std::swap(vertex_id0, vertex_id1);

  for (viennagrid_int i = 0; i != line_vertices_.size(); ++i)
  {
    viennagrid_int * vptr = line_vertices_.begin(i);

    if ( (vertex_id0 == *vptr) && (vertex_id1 == *(vptr+1)) )
      return ID(1, i);
  }

  viennagrid_int id = line_vertices_.size();
  viennagrid_int * vptr = line_vertices_.push_back(2);
  *vptr = vertex_id0;
  *(vptr+1) = vertex_id1;
  return ID(1, id);
}


viennagrid_element_id viennagrid_plc_::get_make_facet(viennagrid_int line_count, viennagrid_element_id * line_ids)
{
  std::vector<viennagrid_int> sorted_line_ids(line_count);
  std::copy(line_ids, line_ids+line_count, sorted_line_ids.begin());
  std::sort(sorted_line_ids.begin(), sorted_line_ids.end());

  for (viennagrid_int id = 0; id != facet_lines_.size(); ++id)
  {
    viennagrid_int * lptr_begin = facet_lines_.begin(id);
    viennagrid_int * lptr_end = facet_lines_.end(id);

    if (line_count != (lptr_end-lptr_begin))
      continue;

    int j = 0;
    for (; j != line_count; ++j)
    {
      if ( sorted_line_ids[j] != *(lptr_begin+j) )
        break;
    }

    if (j == line_count)
      return ID(2, id);
  }

  viennagrid_int id = facet_lines_.size();
  viennagrid_int * lptr = facet_lines_.push_back(line_count);
  std::copy( sorted_line_ids.begin(), sorted_line_ids.end(), lptr );

  std::set<viennagrid_int> vertices;
  for (int i = 0; i != line_count; ++i)
  {
    viennagrid_int * vptr_begin = boundary_begin(sorted_line_ids[i], 0);
    viennagrid_int * vptr_end = boundary_end(sorted_line_ids[i], 0);

    for (viennagrid_int * vptr = vptr_begin; vptr != vptr_end; ++vptr)
      vertices.insert(*vptr);
  }
  viennagrid_int * vptr = facet_vertices_.push_back(vertices.size());
  std::copy( vertices.begin(), vertices.end(), vptr );

  facet_hole_points_.push_back(0);

  return ID(2, id);
}




viennagrid_element_id * viennagrid_plc_::boundary_ptr(viennagrid_dimension topological_dimension,
                                                      viennagrid_dimension boundary_topological_dimension)
{
  switch (topological_dimension)
  {
    case 1:
    {
      return line_vertices_.values_pointer();
    }

    case 2:
    {
      switch (boundary_topological_dimension)
      {
        case 0:
          return facet_vertices_.values_pointer();
        case 1:
          return facet_lines_.values_pointer();
      }
    }
  }

  assert(false);
  return 0;
}

viennagrid_int * viennagrid_plc_::boundary_offsets(viennagrid_dimension topological_dimension,
                                  viennagrid_dimension boundary_topological_dimension)
{
  switch (topological_dimension)
  {
    case 1:
    {
      return line_vertices_.offset_pointer();
    }

    case 2:
    {
      switch (boundary_topological_dimension)
      {
        case 0:
          return facet_vertices_.offset_pointer();
        case 1:
          return facet_lines_.offset_pointer();
      }
    }
  }

  assert(false);
  return 0;
}






viennagrid_element_id * viennagrid_plc_::boundary_begin(viennagrid_element_id id,
                                                        viennagrid_dimension boundary_topological_dimension)
{
  switch (TOPODIM(id))
  {
    case 1:
    {
      return line_vertices_.begin(INDEX(id));
    }

    case 2:
    {
      switch (boundary_topological_dimension)
      {
        case 0:
          return facet_vertices_.begin(INDEX(id));
        case 1:
          return facet_lines_.begin(INDEX(id));
      }
    }
  }

  assert(false);
  return 0;
}

viennagrid_element_id * viennagrid_plc_::boundary_end(viennagrid_element_id id,
                                                      viennagrid_dimension boundary_topological_dimension)
{
  switch (TOPODIM(id))
  {
    case 1:
    {
      return line_vertices_.end(INDEX(id));
    }

    case 2:
    {
      switch (boundary_topological_dimension)
      {
        case 0:
          return facet_vertices_.end(INDEX(id));
        case 1:
          return facet_lines_.end(INDEX(id));
      }
    }
  }

  assert(false);
  return 0;
}








template<typename stream_type>
bool get_valid_line( stream_type & stream, std::string & line, char comment_line = '#' )
{
  std::string tmp;

  while (true)
  {
    if (!std::getline(stream, tmp))
      return false;

    std::size_t pos = tmp.find(comment_line);
    if (pos != std::string::npos)
      tmp = tmp.substr(0, pos);

    for (std::size_t i = 0; i != tmp.size(); ++i)
    {
      if ( tmp[i] != ' ' )
      {
        line = tmp.substr(i, std::string::npos);
        return true;
      }
    }
  }
}



viennagrid_error viennagrid_plc_::read_tetgen_poly(std::string const & filename)
{
  typedef std::vector<viennagrid_numeric> PointType;

  std::ifstream reader(filename.c_str());

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* poly_reader::operator(): Reading file " << filename << std::endl;
  #endif

  if (!reader)
    return VIENNAGRID_ERROR_CANNOT_OPEN_FILE;

  std::vector<PointType> hole_points;
  std::vector<PointType> seed_points;

  std::string tmp;
  std::istringstream current_line;

  long node_num = 0;
  std::size_t point_dim = 0;
  long attribute_num = 0;
  long boundary_marker_num = 0;


  if (!reader.good())
    return VIENNAGRID_ERROR_FILE_EMPTY;

  //
  // Read vertices:
  //
  if (!get_valid_line(reader, tmp))
    return VIENNAGRID_ERROR_EOF_ENCOUNTERED;

  current_line.str(tmp); current_line.clear();
  current_line >> node_num >> point_dim >> attribute_num >> boundary_marker_num;

  set_geometric_dimension(point_dim);

  if (node_num < 0)
    return VIENNAGRID_ERROR_INVALID_VERTEX_COUNT;
//         if (dim != point_dim)
//           throw bad_file_format_exception("* ViennaGrid: tetgen_poly_reader::operator(): File " + filename + ": POLY point dimension missmatch");
  if (attribute_num < 0)
    return VIENNAGRID_ERROR_INVALID_ATTRIBUTE_COUNT;

  if ((boundary_marker_num < 0) || (boundary_marker_num > 1))
    return VIENNAGRID_ERROR_INVALID_BOUNDARY_MARKER_COUNT;

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* poly_reader::operator(): Reading " << node_num << " vertices... " << std::endl;
  #endif

  std::map<int, viennagrid_int> vertex_map;

  for (int i=0; i<node_num; i++)
  {
    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_EOF_ENCOUNTERED;

    int id;
//     viennagrid_int vertex;

    current_line.str(tmp); current_line.clear();
    current_line >> id;

    std::vector<viennagrid_numeric> p(point_dim);

    for (std::size_t j=0; j<point_dim; j++)
      current_line >> p[j];

    vertex_map[id] = make_vertex(&p[0]);
  }

  if (!reader.good())
    return VIENNAGRID_ERROR_EOF_ENCOUNTERED;


  //
  // Read facets:
  //
  long facet_num = 0;
  if (!get_valid_line(reader, tmp))
    return VIENNAGRID_ERROR_EOF_ENCOUNTERED;


  current_line.str(tmp); current_line.clear();
  current_line >> facet_num >> boundary_marker_num;

  if (facet_num < 0)
    return VIENNAGRID_ERROR_INVALID_FACET_COUNT;
  if ((boundary_marker_num < 0) || (boundary_marker_num > 1))
    return VIENNAGRID_ERROR_INVALID_BOUNDARY_MARKER_COUNT;

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;
  #endif

  for (int i=0; i<facet_num; ++i)
  {
    long polygon_num;
    long hole_num;

    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_EOF_ENCOUNTERED;


    current_line.str(tmp); current_line.clear();
    current_line >> polygon_num >> hole_num;

    if (polygon_num < 0)
      return VIENNAGRID_ERROR_INVALID_FACET_POLYGON_COUNT;
    if (hole_num < 0)
      return VIENNAGRID_ERROR_INVALID_FACET_HOLE_POINT_COUNT;

    std::vector<viennagrid_int> lines;
    std::vector<viennagrid_int> vertices;

    for (int j = 0; j<polygon_num; ++j)
    {
      long vertex_num;

      if (!get_valid_line(reader, tmp))
        return VIENNAGRID_ERROR_EOF_ENCOUNTERED;


      current_line.str(tmp); current_line.clear();
      current_line >> vertex_num;

      if (vertex_num < 0)
        return VIENNAGRID_ERROR_INVALID_POLYGON_VERTEX_COUNT;

      std::vector<viennagrid_int> local_vertices(vertex_num);

      for (std::size_t k = 0; k<static_cast<std::size_t>(vertex_num); ++k)
      {
        int id;
        current_line >> id;

        std::map<int, viennagrid_int>::iterator it = vertex_map.find(id);
        if (it == vertex_map.end())
          return VIENNAGRID_ERROR_INVALID_VERTEX_ID;

        local_vertices[k] = vertex_map[id];
      }

      if (vertex_num == 1)
      {
        lines.push_back( get_make_line(local_vertices.front(), local_vertices.front()) );
      }
      else if (vertex_num == 2)
      {
        lines.push_back( get_make_line(local_vertices[0], local_vertices[1]) );
      }
      else
      {
        std::vector<viennagrid_int>::iterator it1 = local_vertices.begin();
        std::vector<viennagrid_int>::iterator it2 = it1; ++it2;
        for (; it2 != local_vertices.end(); ++it1, ++it2)
          lines.push_back( get_make_line(*it1, *it2) );
        lines.push_back( get_make_line(local_vertices.back(), local_vertices.front()) );
      }
    }

    std::vector<PointType> facet_hole_points;

    for (int j = 0; j<hole_num; ++j)
    {
      if (!get_valid_line(reader, tmp))
        return VIENNAGRID_ERROR_EOF_ENCOUNTERED;

      long hole_id;

      current_line.str(tmp); current_line.clear();
      current_line >> hole_id;

      PointType p(point_dim);

      for (std::size_t k=0; k<point_dim; k++)
        current_line >> p[k];

      facet_hole_points.push_back(p);
    }


    int plc = get_make_facet( lines.size(), &lines[0] );
    for (std::vector<PointType>::const_iterator hpit = facet_hole_points.begin();
                                                hpit != facet_hole_points.end();
                                              ++hpit)
    {
      add_facet_hole_point(plc, &(*hpit)[0]);
    }
  }



  long hole_num;

  if (!get_valid_line(reader, tmp))
  {
    // no holes -> Okay, SUCCESS xD
    return VIENNAGRID_SUCCESS;
  }

  current_line.str(tmp); current_line.clear();
  current_line >> hole_num;

  if (hole_num < 0)
    return VIENNAGRID_ERROR_INVALID_HOLE_POINT_COUNT;

  for (int i=0; i<hole_num; ++i)
  {
    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_EOF_ENCOUNTERED;

    long hole_number;
    PointType hole_point(point_dim);

    current_line.str(tmp); current_line.clear();
    current_line >> hole_number;

    for (std::size_t j=0; j < point_dim; j++)
      current_line >> hole_point[j];

    add_hole_point( &hole_point[0] );
  }




  long region_num;

  if (!get_valid_line(reader, tmp))
  {
    // no region -> SUCCESS xD
    return VIENNAGRID_SUCCESS;
  }

  current_line.str(tmp); current_line.clear();
  current_line >> region_num;

  if (region_num < 0)
    return VIENNAGRID_ERROR_INVALID_SEED_POINT_COUNT;

  for (int i=0; i<region_num; ++i)
  {
    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_EOF_ENCOUNTERED;

    long region_number;
    PointType seed_point(point_dim);
    int region_id;

    current_line.str(tmp); current_line.clear();
    current_line >> region_number;

    for (std::size_t j=0; j < point_dim; j++)
      current_line >> seed_point[j];

    current_line >> region_id;

    add_seed_point( &seed_point[0], region_id );
  }

  return VIENNAGRID_SUCCESS;
}










viennagrid_error viennagrid_plc_::write_tetgen_poly(std::string const & filename)
{
  typedef std::vector<viennagrid_numeric> PointType;

  std::ofstream writer(filename.c_str());

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* poly_reader::operator(): Reading file " << filename << std::endl;
  #endif

  if (!writer)
    return VIENNAGRID_ERROR_CANNOT_OPEN_FILE;

  writer << "#\n";
  writer << "# .poly file generated by ViennaGrid\n";
  writer << "# http://viennagrid.sourceforge.net\n";
  writer << "#\n";


  viennagrid_dimension geo_dim = geometric_dimension();

  viennagrid_numeric * vertices = vertex_pointer();
  viennagrid_int vertex_count = element_count(0);

  writer << "\n";
  writer << "# Part 1 - node list\n";
  writer << vertex_count << " 3 0 0 \n";
  for (viennagrid_int vid = 0; vid != vertex_count; ++vid)
  {
    writer << vid << " ";
    for (viennagrid_dimension i = 0; i != geo_dim; ++i)
    {
      writer << vertices[INDEX(vid)*geo_dim+i] << " ";
    }

    for (viennagrid_dimension i = geo_dim; i < 3; ++i)
    {
      writer << "0 ";
    }
    writer << "\n";
  }


  viennagrid_int facet_count = element_count(2);

  writer << "\n";
  writer << "# Part 2 - facet list\n";
  writer << facet_count << " 0 0 \n";
  for (viennagrid_int fid = ID(2,0); fid != ID(2,facet_count); ++fid)
  {
    viennagrid_element_id * line_id_start = boundary_begin(fid, 1);
    viennagrid_element_id * line_id_end = boundary_end(fid, 1);

    viennagrid_int facet_line_count = line_id_end - line_id_start;

    writer << facet_line_count << " " << facet_hole_point_count(fid) << " 0\n";

    for (viennagrid_element_id * lit = line_id_start; lit != line_id_end; ++lit)
    {
      writer << "2 " << boundary_begin(*lit,0)[0] << " " << boundary_begin(*lit,0)[1] << "\n";
    }

    for (viennagrid_int hpi = 0; hpi != facet_hole_point_count(fid); ++hpi)
    {
      for (viennagrid_dimension i = 0; i != geo_dim; ++i)
      {
        writer << facet_hole_points(fid)[hpi*geo_dim+i] << " ";
      }

      for (viennagrid_dimension i = geo_dim; i < 3; ++i)
      {
        writer << "0 ";
      }
      writer << "\n";
    }
  }

  writer << "\n";
  writer << "# Part 3 - hole list\n";
  writer << hole_point_count() << "\n";
  for (viennagrid_int hpi = 0; hpi != hole_point_count(); ++hpi)
  {
    writer << hpi << " ";
    for (viennagrid_dimension i = 0; i != geo_dim; ++i)
    {
      writer << get_hole_points()[INDEX(hpi)*geo_dim+i] << " ";
    }

    for (viennagrid_dimension i = geo_dim; i < 3; ++i)
    {
      writer << "0 ";
    }
    writer << "\n";
  }

  writer << "\n";
  writer << "# Part 4 - region list\n";
  writer << seed_point_count() << "\n";
  for (viennagrid_int spi = 0; spi != seed_point_count(); ++spi)
  {
    writer << spi << " ";
    for (viennagrid_dimension i = 0; i != geo_dim; ++i)
    {
      writer << get_seed_points()[spi*geo_dim+i] << " ";
    }

    for (viennagrid_dimension i = geo_dim; i < 3; ++i)
    {
      writer << "0 ";
    }

    writer << get_seed_point_regions()[spi] << "\n";
  }

  return VIENNAGRID_SUCCESS;
}





