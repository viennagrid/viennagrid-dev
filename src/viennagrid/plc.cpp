#include "plc.hpp"

#include <fstream>
#include <sstream>




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

viennagrid_int viennagrid_plc_::element_count(int topologic_dimension)
{
  if (topologic_dimension == 0)
    return vertex_count();

  if (topologic_dimension == 1)
    return line_vertices_.size();

  if (topologic_dimension == 2)
    return facet_vertices_.size();

  assert(false);
  return -1;
}



viennagrid_int viennagrid_plc_::get_make_line(viennagrid_int vertex_id0, viennagrid_int vertex_id1)
{
  if (vertex_id1 < vertex_id0)
    std::swap(vertex_id0, vertex_id1);

  for (viennagrid_int i = 0; i != line_vertices_.size(); ++i)
  {
    viennagrid_int * vptr = line_vertices_.begin(i);

    if ( (vertex_id0 == *vptr) && (vertex_id1 == *(vptr+1)) )
      return i;
  }

  viennagrid_int id = line_vertices_.size();
  viennagrid_int * vptr = line_vertices_.push_back(2);
  *vptr = vertex_id0;
  *(vptr+1) = vertex_id1;
  return id;
}


viennagrid_int viennagrid_plc_::get_make_facet(viennagrid_int * line_ids, viennagrid_int line_count)
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
      return id;
  }

  viennagrid_int id = facet_lines_.size();
  viennagrid_int * lptr = facet_lines_.push_back(line_count);
  std::copy( sorted_line_ids.begin(), sorted_line_ids.end(), lptr );

  std::set<viennagrid_int> vertices;
  for (int i = 0; i != line_count; ++i)
  {
    viennagrid_int * vptr_begin = boundary_begin(1, sorted_line_ids[i], 0);
    viennagrid_int * vptr_end = boundary_end(1, sorted_line_ids[i], 0);

    for (viennagrid_int * vptr = vptr_begin; vptr != vptr_end; ++vptr)
      vertices.insert(*vptr);
  }
  viennagrid_int * vptr = facet_vertices_.push_back(vertices.size());
  std::copy( vertices.begin(), vertices.end(), vptr );

  facet_hole_points_.push_back(0);

  return id;
}



viennagrid_int * viennagrid_plc_::boundary_begin(viennagrid_dimension topologic_dimension,
                                  viennagrid_int id,
                                  viennagrid_dimension boundary_topologic_dimension)
{
  switch (topologic_dimension)
  {
    case 1:
    {
      return line_vertices_.begin(id);
    }

    case 2:
    {
      switch (boundary_topologic_dimension)
      {
        case 0:
          return facet_vertices_.begin(id);
        case 1:
          return facet_lines_.begin(id);
      }
    }
  }

  assert(false);
  return 0;
}

viennagrid_int * viennagrid_plc_::boundary_end(viennagrid_dimension topologic_dimension,
                                viennagrid_int id,
                                viennagrid_dimension boundary_topologic_dimension)
{
  switch (topologic_dimension)
  {
    case 1:
    {
      return line_vertices_.end(id);
    }

    case 2:
    {
      switch (boundary_topologic_dimension)
      {
        case 0:
          return facet_vertices_.end(id);
        case 1:
          return facet_lines_.end(id);
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



viennagrid_int viennagrid_plc_::read_tetgen_poly(std::string const & filename)
{
  typedef std::vector<viennagrid_numeric> PointType;

  std::ifstream reader(filename.c_str());

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* poly_reader::operator(): Reading file " << filename << std::endl;
  #endif

  if (!reader)
    return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;

  std::vector<PointType> hole_points;
  std::vector<PointType> seed_points;

  std::string tmp;
  std::istringstream current_line;

  long node_num = 0;
  std::size_t point_dim = 0;
  long attribute_num = 0;
  long boundary_marker_num = 0;


  if (!reader.good())
    return VIENNAGRID_ERROR_IO_FILE_EMPTY;

  //
  // Read vertices:
  //
  if (!get_valid_line(reader, tmp))
    return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;

  current_line.str(tmp); current_line.clear();
  current_line >> node_num >> point_dim >> attribute_num >> boundary_marker_num;

  set_geometric_dimension(point_dim);

  if (node_num < 0)
    return VIENNAGRID_ERROR_IO_INVALID_VERTEX_COUNT;
//         if (dim != point_dim)
//           throw bad_file_format_exception("* ViennaGrid: tetgen_poly_reader::operator(): File " + filename + ": POLY point dimension missmatch");
  if (attribute_num < 0)
    return VIENNAGRID_ERROR_IO_INVALID_ATTRIBUTE_COUNT;

  if ((boundary_marker_num < 0) || (boundary_marker_num > 1))
    return VIENNAGRID_ERROR_IO_INVALID_BOUNDARY_MARKER_COUNT;

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* poly_reader::operator(): Reading " << node_num << " vertices... " << std::endl;
  #endif

  std::map<int, viennagrid_int> vertex_map;

  for (int i=0; i<node_num; i++)
  {
    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;

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
    return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;


  //
  // Read facets:
  //
  long facet_num = 0;
  if (!get_valid_line(reader, tmp))
    return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;


  current_line.str(tmp); current_line.clear();
  current_line >> facet_num >> boundary_marker_num;

  if (facet_num < 0)
    return VIENNAGRID_ERROR_IO_INVALID_FACET_COUNT;
  if ((boundary_marker_num < 0) || (boundary_marker_num > 1))
    return VIENNAGRID_ERROR_IO_INVALID_BOUNDARY_MARKER_COUNT;

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;
  #endif

  for (int i=0; i<facet_num; ++i)
  {
    long polygon_num;
    long hole_num;

    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;


    current_line.str(tmp); current_line.clear();
    current_line >> polygon_num >> hole_num;

    if (polygon_num < 0)
      return VIENNAGRID_ERROR_IO_INVALID_FACET_POLYGON_COUNT;
    if (hole_num < 0)
      return VIENNAGRID_ERROR_IO_INVALID_FACET_HOLE_POINT_COUNT;

    std::vector<viennagrid_int> lines;
    std::vector<viennagrid_int> vertices;

    for (int j = 0; j<polygon_num; ++j)
    {
      long vertex_num;

      if (!get_valid_line(reader, tmp))
        return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;


      current_line.str(tmp); current_line.clear();
      current_line >> vertex_num;

      if (vertex_num < 0)
        return VIENNAGRID_ERROR_IO_INVALID_POLYGON_VERTEX_COUNT;

      std::vector<viennagrid_int> local_vertices(vertex_num);

      for (std::size_t k = 0; k<static_cast<std::size_t>(vertex_num); ++k)
      {
        int id;
        current_line >> id;

        std::map<int, viennagrid_int>::iterator it = vertex_map.find(id);
        if (it == vertex_map.end())
          return VIENNAGRID_ERROR_IO_INVALID_VERTEX_ID;

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
        return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;

      long hole_id;

      current_line.str(tmp); current_line.clear();
      current_line >> hole_id;

      PointType p(point_dim);

      for (std::size_t k=0; k<point_dim; k++)
        current_line >> p[k];

      facet_hole_points.push_back(p);
    }


    int plc = get_make_facet( &lines[0], lines.size() );
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
    return VIENNAGRID_ERROR_IO_INVALID_HOLE_POINT_COUNT;

  for (int i=0; i<hole_num; ++i)
  {
    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;

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
    return VIENNAGRID_ERROR_IO_INVALID_SEED_POINT_COUNT;

  for (int i=0; i<region_num; ++i)
  {
    if (!get_valid_line(reader, tmp))
      return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;

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
