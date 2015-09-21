#ifndef VIENNAGRID_IO_VTK_WRITER_HPP
#define VIENNAGRID_IO_VTK_WRITER_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <set>

#include "vtk_common.hpp"

/** @file viennagrid/io/vtk.hpp
    @brief Provides reader and writer for VTK files
*/

// (see: http://www.vtk.org/VTK/img/file-formats.pdf, page 9)

//
/////////////////// VTK export ////////////////////////////
//


/** @brief Writes the XML file header */
static void writeVTKHeader(std::ofstream & writer)
{
  writer << "<?xml version=\"1.0\"?>" << std::endl;
  writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
  writer << " <UnstructuredGrid>" << std::endl;
}

/** @brief Writes all the vertices of the mesh */
static viennagrid_error writePoints(viennagrid_mesh mesh, std::ofstream & writer)
{
  writer << "   <Points>" << std::endl;
  writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

  viennagrid_dimension geo_dim;
  viennagrid_error err = viennagrid_mesh_geometric_dimension_get(mesh, &geo_dim); if (err != VIENNAGRID_SUCCESS) return err;

  viennagrid_int *vertex_begin, *vertex_end;
  err = viennagrid_mesh_elements_get(mesh, 0, &vertex_begin, &vertex_end); if (err != VIENNAGRID_SUCCESS) return err;

  viennagrid_numeric *coords;
  for (viennagrid_int *vit = vertex_begin; vit != vertex_end; ++vit)
  {
    err = viennagrid_mesh_vertex_coords_get(mesh, *vit, &coords); if (err != VIENNAGRID_SUCCESS) return err;

    // x-component
    writer << coords[0] << " ";

    // y-component
    if (geo_dim > 1)
      writer << coords[1] << " ";
    else
      writer << "0 ";

    // z-component
    if (geo_dim > 2)
      writer << coords[2] << " ";
    else
      writer << "0 ";

    writer << std::endl;
  }
  writer << std::endl;
  writer << "    </DataArray>" << std::endl;
  writer << "   </Points> " << std::endl;

  return VIENNAGRID_SUCCESS;
} //writePoints()

/** @brief Writes the mesh's cells to the file */
static viennagrid_error writeCells(viennagrid_mesh mesh, std::ofstream & writer)
{
  writer << "   <Cells> " << std::endl;

  viennagrid_dimension cell_dim;
  viennagrid_error err = viennagrid_mesh_cell_dimension_get(mesh, &cell_dim); if (err != VIENNAGRID_SUCCESS) return err;

  //
  // Write array containing connectivity information. Vertex IDs (0-based) consecutively for each cell.
  //
  writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
  viennagrid_int *cell_begin, *cell_end;
  err = viennagrid_mesh_elements_get(mesh, cell_dim, &cell_begin, &cell_end); if (err != VIENNAGRID_SUCCESS) return err;
  for (viennagrid_int *cit = cell_begin; cit != cell_end; ++cit)
  {
    viennagrid_int *cell_vertices_begin, *cell_vertices_end;
    err = viennagrid_element_boundary_elements(mesh, *cit, 0, &cell_vertices_begin, &cell_vertices_end); if (err != VIENNAGRID_SUCCESS) return err;

    viennagrid_element_type cell_type;
    err = viennagrid_element_type_get(mesh, *cit, &cell_type); if (err != VIENNAGRID_SUCCESS) return err;

    switch (cell_type)
    {
    case VIENNAGRID_ELEMENT_TYPE_EDGE:
      writer << cell_vertices_begin[0] << " " << cell_vertices_begin[1] << " "; break;
    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      writer << cell_vertices_begin[0] << " " << cell_vertices_begin[1] << " " << cell_vertices_begin[2] << " "; break;
    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      writer << cell_vertices_begin[0] << " " << cell_vertices_begin[1] << " " << cell_vertices_begin[3] << " " << cell_vertices_begin[2] << " "; break;
    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      writer << cell_vertices_begin[0] << " " << cell_vertices_begin[1] << " " << cell_vertices_begin[2] << " " << cell_vertices_begin[3] << " "; break;
    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      writer << cell_vertices_begin[0] << " " << cell_vertices_begin[1] << " " << cell_vertices_begin[3] << " " << cell_vertices_begin[2] << " ";
      writer << cell_vertices_begin[4] << " " << cell_vertices_begin[5] << " " << cell_vertices_begin[7] << " " << cell_vertices_begin[6] << " ";
      break;
    default:
      return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
    }

    writer << std::endl;
  }
  writer << std::endl;
  writer << "    </DataArray>" << std::endl;


  //
  // Write offsets from first entry in cell-vertex map for each cell. (CSR-like datastructure)
  // First cell has implicit offset 0
  //
  writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
  std::size_t offset = 0;
  for (viennagrid_int *cit = cell_begin; cit != cell_end; ++cit)
  {
    viennagrid_int *cell_vertices_begin, *cell_vertices_end;
    err = viennagrid_element_boundary_elements(mesh, *cit, 0, &cell_vertices_begin, &cell_vertices_end); if (err != VIENNAGRID_SUCCESS) return err;

    offset += (cell_vertices_end - cell_vertices_begin);
    writer << offset << " ";
  }
  writer << std::endl;
  writer << "    </DataArray>" << std::endl;

  //
  // Write cell types (mind different IDs for ViennaGrid and VTK)
  //
  writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
  for (viennagrid_int *cit = cell_begin; cit != cell_end; ++cit)
  {
    viennagrid_element_type cell_type;
    err = viennagrid_element_type_get(mesh, *cit, &cell_type); if (err != VIENNAGRID_SUCCESS) return err;

    writer << vtk_element_type(cell_type) << " ";
  }

  writer << std::endl;
  writer << "    </DataArray>" << std::endl;
  writer << "   </Cells>" << std::endl;

  return VIENNAGRID_SUCCESS;
}








/** @brief Writes vector-valued data defined on vertices (points) to file * /
template <typename RegionT, typename IOAccessorType>
void writePointData(RegionT const &, std::ofstream & writer, std::string const & name, IOAccessorType const & accessor, region_id_type region_id)
{
  typedef typename IOAccessorType::value_type ValueType;

  std::map< VertexIDType, ElementType > & current_used_vertex_map = used_vertex_map[region_id];
  for (typename std::map< VertexIDType, ElementType >::iterator it = current_used_vertex_map.begin(); it != current_used_vertex_map.end(); ++it)
  {
    if (!accessor.valid(it->second))
    {
      //TODO we need some kind of reporting here
      return;
    }
  }

  writer << "    <DataArray type=\"" << ValueTypeInformation<ValueType>::type_name() << "\" Name=\"" << name <<
    "\" NumberOfComponents=\"" << ValueTypeInformation<ValueType>::num_components() << "\" format=\"ascii\">" << std::endl;

  for (typename std::map< VertexIDType, ElementType >::iterator it = current_used_vertex_map.begin(); it != current_used_vertex_map.end(); ++it)
  {
    ValueTypeInformation<ValueType>::write(writer, accessor.get(it->second));
    writer << " ";
  }
  writer << std::endl;

  writer << "    </DataArray>" << std::endl;
} //writePointDataScalar
*/

/** @brief Writes vector-valued data defined on vertices (points) to file * /
template <typename RegionT, typename IOAccessorType>
void writeCellData(RegionT const &, std::ofstream & writer, std::string const & name, IOAccessorType const & accessor, region_id_type region_id)
{
  typedef typename IOAccessorType::value_type ValueType;

  writer << "    <DataArray type=\"" << ValueTypeInformation<ValueType>::type_name() << "\" Name=\"" << name <<
    "\" NumberOfComponents=\"" << ValueTypeInformation<ValueType>::num_components() << "\" format=\"ascii\">" << std::endl;


  std::set< ElementType > & current_used_cells_map = used_cell_map[region_id];
  for (typename std::set< ElementType >::iterator it = current_used_cells_map.begin(); it != current_used_cells_map.end(); ++it)

  {
    //step 1: Write vertex indices in ViennaGrid orientation to array:
//           ElementType const & cell = viennagrid::dereference_handle(region, it->second);
//             ElementType const & cell = *cit;

    ValueTypeInformation<ValueType>::write(writer, accessor.get(*it));
    writer << " ";
  }
  writer << std::endl;

  writer << "    </DataArray>" << std::endl;
} //writePointDataScalar
*/


/** @brief Writes the XML footer */
void writeFooter(std::ofstream & writer)
{
  writer << " </UnstructuredGrid>" << std::endl;
  writer << "</VTKFile>" << std::endl;
}



/** @brief Top-level function for writing to VTK.
  *
  * Current limitations: Regions are not considered, but instead the full mesh is written in a single .vtu
  */
viennagrid_error viennagrid_mesh_io_read_vtk(viennagrid_mesh_io mesh_io, const char *filename)
{
  viennagrid_error err;

  std::stringstream ss;
  ss << filename << ".vtu";
  std::ofstream writer(ss.str().c_str());

  if (!writer)
    return VIENNAGRID_ERROR_CANNOT_OPEN_FILE;
    //throw cannot_open_file_exception("* ViennaGrid: vtk_writer::operator(): File " + filename + ": Cannot open file!");

  viennagrid_mesh mesh;
  err = viennagrid_mesh_io_mesh_get(mesh_io, &mesh); if (err != VIENNAGRID_SUCCESS) return err;

  try
  {
    writeVTKHeader(writer);
  }
  catch (std::exception & e)
  {
    return VIENNAGRID_ERROR_WRITE_ERROR;
  }

  viennagrid_dimension cell_dim;
  err = viennagrid_mesh_cell_dimension_get(mesh, &cell_dim); if (err != VIENNAGRID_SUCCESS) return err;

  viennagrid_int *vertex_begin, *vertex_end;
  err = viennagrid_mesh_elements_get(mesh, 0, &vertex_begin, &vertex_end); if (err != VIENNAGRID_SUCCESS) return err;

  viennagrid_int *cell_begin, *cell_end;
  err = viennagrid_mesh_elements_get(mesh, cell_dim, &cell_begin, &cell_end); if (err != VIENNAGRID_SUCCESS) return err;

  writer << "  <Piece NumberOfPoints=\""
         << (vertex_end - vertex_begin)
         << "\" NumberOfCells=\""
         << (cell_end - cell_begin)
         << "\">" << std::endl;

  writePoints(mesh, writer);

  /*if (vertex_scalar_data.size() > 0 || vertex_vector_data.size() > 0)
  {
    writer << "   <PointData>" << std::endl;

      for (typename VertexScalarOutputAccessorContainer::const_iterator it = vertex_scalar_data.begin(); it != vertex_scalar_data.end(); ++it)
        writePointData( mesh_obj, writer, it->first, *(it->second), tmp_id );
      for (typename VertexVectorOutputAccessorContainer::const_iterator it = vertex_vector_data.begin(); it != vertex_vector_data.end(); ++it)
        writePointData( mesh_obj, writer, it->first, *(it->second), tmp_id );

    writer << "   </PointData>" << std::endl;
  } */

  writeCells(mesh, writer);
  /*if (cell_scalar_data.size() > 0 || cell_vector_data.size() > 0)
  {
    writer << "   <CellData>" << std::endl;

      for (typename CellScalarOutputAccessorContainer::const_iterator it = cell_scalar_data.begin(); it != cell_scalar_data.end(); ++it)
        writeCellData( mesh_obj, writer, it->first, *(it->second), tmp_id );
      for (typename CellVectorOutputAccessorContainer::const_iterator it = cell_vector_data.begin(); it != cell_vector_data.end(); ++it)
        writeCellData( mesh_obj, writer, it->first, *(it->second), tmp_id );

    writer << "   </CellData>" << std::endl;
  }*/

  writer << "  </Piece>" << std::endl;
  writeFooter(writer);

  return VIENNAGRID_SUCCESS;
}


//
/////////////////// VTK import ////////////////////////////
//


#endif
