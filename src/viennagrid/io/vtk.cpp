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
#include <vector>

#include "vtk_common.hpp"

/** @file viennagrid/io/vtk.hpp
    @brief Provides reader and writer for VTK files
*/

// (see: http://www.vtk.org/VTK/img/file-formats.pdf, page 9)

//
/////////////////// VTK export ////////////////////////////
//


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



/** @brief Writes vector-valued data defined on vertices (points) to file */
static viennagrid_error writeElementData(viennagrid_mesh mesh, std::ofstream & writer, viennagrid_quantity_field field, viennagrid_dimension element_dimension)
{
  viennagrid_int err;

  const char *quantity_name;
  err = viennagrid_quantity_field_name_get(field, &quantity_name); if (err != VIENNAGRID_SUCCESS) return err;

  viennagrid_int num_components;
  err = viennagrid_quantity_field_values_per_quantity_get(field, &num_components); if (err != VIENNAGRID_SUCCESS) return err;

  if (num_components > 0)
  {
    writer << "    <DataArray type=\"Float32\" Name=\"" << quantity_name << "\" NumberOfComponents=\"" << num_components << "\" format=\"ascii\">" << std::endl;

    viennagrid_int *elements_begin, *elements_end;
    err = viennagrid_mesh_elements_get(mesh, element_dimension, &elements_begin, &elements_end); if (err != VIENNAGRID_SUCCESS) return err;

    for (viennagrid_int *eit = elements_begin; eit != elements_end; ++eit)
    {
      viennagrid_numeric *value;
      err = viennagrid_quantity_field_value_get(field, *eit, reinterpret_cast<void**>(&value));if (err != VIENNAGRID_SUCCESS) return err;

      writer << value[0] << " ";
      if (num_components > 1)
      {
        writer << value[1] << " ";

        // Vector data is always written with three components:
        if (num_components > 2)
          writer << value[2] << " ";
        else
          writer << "0 ";
      }
    }
    writer << std::endl;

    writer << "    </DataArray>" << std::endl;
  }

  return VIENNAGRID_SUCCESS;
} //writeElementData



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



/** @brief Top-level function for writing to VTK.
  *
  * Current limitations: Regions are not considered, but instead the full mesh is written in a single .vtu
  */
viennagrid_error viennagrid_mesh_io_write_vtk(viennagrid_mesh_io mesh_io, const char *filename)
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
    writer << "<?xml version=\"1.0\"?>" << std::endl;
    writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
    writer << " <UnstructuredGrid>" << std::endl;

    viennagrid_dimension cell_dim;
    err = viennagrid_mesh_cell_dimension_get(mesh, &cell_dim); if (err != VIENNAGRID_SUCCESS) return err;

    //
    // Query vertex and cell data, determine the number of quantity fields located on vertices and cells
    //
    viennagrid_int fields;
    err = viennagrid_mesh_io_quantity_field_count(mesh_io, &fields); if (err != VIENNAGRID_SUCCESS) return err;

    std::vector<viennagrid_int> vertex_fields;
    std::vector<viennagrid_int>   cell_fields;
    for (viennagrid_int i = 0; i < fields; ++i)
    {
      viennagrid_quantity_field field;
      err = viennagrid_mesh_io_quantity_field_get_by_index(mesh_io, i, &field); if (err != VIENNAGRID_SUCCESS) return err;

      viennagrid_int field_values_type;
      err = viennagrid_quantity_field_values_type_get(field, &field_values_type); if (err != VIENNAGRID_SUCCESS) return err;

      // If the quantity is a scalar, we include it in the output.
      // (Clearly we do not know here how arbitrary user-defined data can be written to file)
      if (field_values_type == VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC)
      {
        viennagrid_dimension topo_dim;
        err = viennagrid_quantity_field_topological_dimension_get(field, &topo_dim); if (err != VIENNAGRID_SUCCESS) return err;

        if (topo_dim == 0)
          vertex_fields.push_back(i);
        else if (topo_dim == cell_dim)
          cell_fields.push_back(i);
      }
    }

    viennagrid_int *vertex_begin, *vertex_end;
    err = viennagrid_mesh_elements_get(mesh, 0, &vertex_begin, &vertex_end); if (err != VIENNAGRID_SUCCESS) return err;

    viennagrid_int *cell_begin, *cell_end;
    err = viennagrid_mesh_elements_get(mesh, cell_dim, &cell_begin, &cell_end); if (err != VIENNAGRID_SUCCESS) return err;

    writer << "  <Piece NumberOfPoints=\""
           << (vertex_end - vertex_begin)
           << "\" NumberOfCells=\""
           << (cell_end - cell_begin)
           << "\">" << std::endl;

    err = writePoints(mesh, writer); if (err != VIENNAGRID_SUCCESS) return err;

    if (vertex_fields.size() > 0)
    {
      writer << "   <PointData>" << std::endl;
      for (std::size_t i=0; i<vertex_fields.size(); ++i)
      {
        viennagrid_quantity_field field;
        err = viennagrid_mesh_io_quantity_field_get_by_index(mesh_io, vertex_fields[i], &field); if (err != VIENNAGRID_SUCCESS) return err;

        err = writeElementData(mesh, writer, field, 0); if (err != VIENNAGRID_SUCCESS) return err;
      }
      writer << "   </PointData>" << std::endl;
    }

    //
    // Write cells and cell data:
    //
    writeCells(mesh, writer);

    if (cell_fields.size() > 0)
    {
      writer << "   <CellData>" << std::endl;
      for (std::size_t i=0; i<cell_fields.size(); ++i)
      {
        viennagrid_quantity_field field;
        err = viennagrid_mesh_io_quantity_field_get_by_index(mesh_io, cell_fields[i], &field); if (err != VIENNAGRID_SUCCESS) return err;

        err = writeElementData(mesh, writer, field, cell_dim); if (err != VIENNAGRID_SUCCESS) return err;
      }
      writer << "   </CellData>" << std::endl;
    }

    //
    // Final bits:
    //
    writer << "  </Piece>" << std::endl;
    writer << " </UnstructuredGrid>" << std::endl;
    writer << "</VTKFile>" << std::endl;
  }
  catch (std::exception & e)
  {
    return VIENNAGRID_ERROR_WRITE_ERROR;
  }

  return VIENNAGRID_SUCCESS;
}


//
/////////////////// VTK import ////////////////////////////
//


#endif
