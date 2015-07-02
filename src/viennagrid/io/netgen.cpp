#include "viennagrid/viennagrid.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iterator>
#include <algorithm>

viennagrid_error viennagrid_mesh_io_read_netgen(viennagrid_mesh_io mesh_io,
                                                const char * filename)
{
  std::ifstream reader(filename);
  std::string tmp;

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* netgen_reader::operator(): Reading file " << filename << std::endl;
  #endif

  if (!reader)
    return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;

  long node_num = 0;
  long cell_num = 0;

  if (!reader.good())
    return VIENNAGRID_ERROR_IO_FILE_EMPTY;

  //
  // Read vertices:
  //
  {
    getline( reader, tmp );
    std::istringstream line(tmp);

    line >> node_num;
    assert(node_num > 0);
  }

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* netgen_reader::operator(): Reading " << node_num << " vertices... " << std::endl;
  #endif


  viennagrid_mesh mesh;
  viennagrid_mesh_io_mesh_get( mesh_io, &mesh );

  viennagrid_dimension geometric_dimension = -1;

  for (int i=0; i<node_num; i++)
  {
    if (!reader.good())
      return VIENNAGRID_ERROR_IO_EOF_WHILE_READING_VERTICES;

    getline( reader, tmp );
    std::istringstream line(tmp);

    std::vector<viennagrid_numeric> p;
    std::copy( std::istream_iterator<viennagrid_numeric>(line),
               std::istream_iterator<viennagrid_numeric>(),
               std::back_inserter(p) );

    if (geometric_dimension < 0)
    {
      viennagrid_mesh_geometric_dimension_set(mesh, p.size());
      geometric_dimension = p.size();
    }
    else if (geometric_dimension != (viennagrid_dimension)p.size())
    {
      return VIENNAGRID_ERROR_IO_VERTEX_DIMENSION_MISMATCH;
    }

    viennagrid_mesh_vertex_create( mesh, &p[0], NULL );
  }

  if (!reader.good())
    return VIENNAGRID_ERROR_IO_EOF_WHILE_READING_CELL_COUNT;


  //
  // Read cells:
  //
  {
    getline( reader, tmp );
    std::istringstream line(tmp);

    line >> cell_num;
    assert(cell_num > 0);
  }

  #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
  std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;
  #endif

  for (int i=0; i<cell_num; ++i)
  {
    if (!reader.good())
      return VIENNAGRID_ERROR_IO_EOF_WHILE_READING_CELLS;

    getline( reader, tmp );
    std::istringstream line(tmp);

    int segment_index;
    std::vector<viennagrid_int> vertex_ids;
    line >> segment_index;

    while (line.good())
    {
      viennagrid_int index;
      line >> index;
      vertex_ids.push_back(index);
    }

    for (std::size_t j=0; j < vertex_ids.size(); ++j)
      --vertex_ids[j];


    viennagrid_element_type et;
    if (vertex_ids.size() == 2)
      et = VIENNAGRID_ELEMENT_TYPE_LINE;
    else if (vertex_ids.size() == 3)
      et = VIENNAGRID_ELEMENT_TYPE_TRIANGLE;
    else if (vertex_ids.size() == 4)
      et = VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON;
    else
      return VIENNAGRID_ERROR_IO_UNSUPPORTED_ELEMENT_TYPE;

    viennagrid_mesh_element_create(mesh, et, vertex_ids.size(), &vertex_ids[0], NULL);
  }

  return VIENNAGRID_SUCCESS;
}

