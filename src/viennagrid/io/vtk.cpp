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
#include <map>

#include "vtk_common.hpp"
#include "xml_tag.hpp"

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

struct point_less
{
  template <typename PointType>
  bool operator()(PointType const & p1, PointType const & p2) const
  {
    for (std::size_t i=0; i<p1.size(); ++i)
    {
      if (p1[i] < p2[i])
        return true;
      else if (p1[i] > p2[i])
        return false;
    }
    return false;
  }
};


struct element_data
{
  viennagrid_numeric data[3];
};

struct vtu_data
{
  std::vector<viennagrid_element_id> local_to_global_map; // local point ID in region to global point ID in mesh (exploiting that element IDs for vertices start at 0)

  std::map<std::string, std::vector<viennagrid_numeric> > point_data; // [name, data]. Dimension is obtained from length of array

  std::vector<viennagrid_int>         cell_connectivity;
  std::vector<viennagrid_int>         cell_offsets;
  std::vector<viennagrid_int>         cell_types;
  std::vector<viennagrid_element_id>  cell_ids;

  std::map<std::string, std::vector<viennagrid_numeric> > cell_data; // [name, data]. Dimension is obtained from length of array
};

struct pvd_data
{
  std::vector<vtu_data>                      region_data;

  viennagrid_dimension                       point_dim;

  typedef std::map<std::vector<viennagrid_numeric>, viennagrid_element_id, point_less>  VertexSingletonContainer;
  VertexSingletonContainer vertex_singleton;  // ensures that there is only one point on a given spatial location
};


static viennagrid_error viennagrid_mesh_io_read_vtu_data(std::ifstream & reader,
                                                         xml_tag<> & tag,
                                                         std::map<std::string, std::vector<viennagrid_numeric> > & element_data,
                                                         std::size_t element_count)
{
  tag.parse(reader);
  while (tag.name() == "dataarray")
  {
    tag.check_attribute("name");
    std::string quantity_name = tag.get_value("name");

    std::size_t components = 1;
    if (tag.has_attribute("numberofcomponents"))
      components = static_cast<std::size_t>(atoi(tag.get_value("numberofcomponents").c_str()));

    if (components < 1 || components > 3)
      throw std::runtime_error("ViennaGrid: VTK reader: Number of components for data invalid!");

    std::vector<viennagrid_numeric> & data = element_data[quantity_name];
    data.resize(element_count*components);
    for (std::size_t i=0; i<element_count*components; ++i)
      reader >> data.at(i);

    tag.parse(reader);

    if (tag.name() != "/dataarray")
      throw std::runtime_error("ViennaGrid: viennagrid_mesh_io_read_vtu_data: XML Parse error: Expected </DataArray>!");
    tag.parse(reader);
  }

  return VIENNAGRID_SUCCESS;
}

/** @brief Reads one .vtu file and sets up the mesh. */
static viennagrid_error viennagrid_mesh_io_read_vtu(std::string filename, pvd_data & mesh_data, std::size_t region_index)
{
  viennagrid_error err;
  std::ifstream reader(filename.c_str());

  if (!reader)
    return VIENNAGRID_ERROR_CANNOT_OPEN_FILE;

  xml_tag<> tag;

  tag.parse(reader);
  if (tag.name() != "?xml" && tag.name() != "?xml?")
    return VIENNAGRID_ERROR_READ_ERROR;

  tag.parse_and_check_name(reader, "vtkfile");
  tag.parse_and_check_name(reader, "unstructuredgrid");

  tag.parse_and_check_name(reader, "piece");

  tag.check_attribute("numberofpoints");

  std::size_t nodeNum = static_cast<std::size_t>(atoi(tag.get_value("numberofpoints").c_str()));

  tag.check_attribute("numberofcells");

  std::size_t cellNum = static_cast<std::size_t>(atoi(tag.get_value("numberofcells").c_str()));

  tag.parse_and_check_name(reader, "points");

  tag.parse_and_check_name(reader, "dataarray");
  tag.check_attribute("numberofcomponents");

  //
  // Read node coordinates
  //
  std::size_t numberOfComponents = static_cast<std::size_t>(atoi(tag.get_value("numberofcomponents").c_str()));
  vtu_data & current_vtu_data = mesh_data.region_data.at(region_index);
  current_vtu_data.local_to_global_map.resize(nodeNum);

  if (mesh_data.point_dim < 1)
    mesh_data.point_dim = viennagrid_dimension(numberOfComponents);
  else if (mesh_data.point_dim != viennagrid_dimension(numberOfComponents))
    return VIENNAGRID_ERROR_READ_ERROR;

  std::vector<viennagrid_numeric> p(mesh_data.point_dim);
  for(std::size_t i = 0; i < nodeNum; i++)
  {
    for(std::size_t j = 0; j < numberOfComponents; j++)
    {
      reader >> p[j];
    }

    std::map<std::vector<viennagrid_numeric>, viennagrid_element_id, point_less>::const_iterator global_point_iter = mesh_data.vertex_singleton.find(p);
    if (global_point_iter == mesh_data.vertex_singleton.end())
    {
      viennagrid_element_id new_global_id = viennagrid_compose_element_id(0, viennagrid_element_id(mesh_data.vertex_singleton.size()));
      mesh_data.vertex_singleton.insert( std::make_pair(p, new_global_id) );
      current_vtu_data.local_to_global_map.at(i) = new_global_id;

      //if (!use_local_points())
      mesh_data.vertex_singleton.insert( std::make_pair(p, new_global_id) );
    }
    else
    {
      current_vtu_data.local_to_global_map.at(i) = global_point_iter->second;
    }
  }

  tag.parse_and_check_name(reader, "/dataarray");
  tag.parse_and_check_name(reader, "/points");

  //
  // Read point data if available
  //
  tag.parse(reader);
  if (tag.name() == "pointdata")
  {
    err = viennagrid_mesh_io_read_vtu_data(reader, tag, current_vtu_data.point_data, nodeNum); if (err != VIENNAGRID_SUCCESS) return err;

    if (tag.name() != "/pointdata")
        throw std::runtime_error("ViennaGrid: viennagrid_mesh_io_read_vtu_data: XML Parse error: Expected </PointData>!");
    tag.parse(reader);
  }

  //
  // Read cells in three steps: Connectivity information, offsets, and cell types
  //
  if (tag.name() != "cells")
    throw std::runtime_error("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: Expected Cells tag!");

  for (std::size_t k=0; k<3; ++k)
  {
    tag.parse_and_check_name(reader, "dataarray");
    tag.check_attribute("name");

    if (tag.get_value("name") == "connectivity")
    {
      current_vtu_data.cell_connectivity.reserve(2*cellNum);
      std::string token;
      reader >> token;

      while (token != "</DataArray>" && token != "</dataarray>")
      {
        viennagrid_int cellNode = static_cast<std::size_t>(atoi(token.c_str()));
        current_vtu_data.cell_connectivity.push_back(cellNode);

        reader >> token;
      }
    }
    else if (tag.get_value("name") == "offsets")
    {
      current_vtu_data.cell_offsets.resize(cellNum);
      for (std::size_t i=0; i<cellNum; ++i)
        reader >> current_vtu_data.cell_offsets.at(i);
      tag.parse_and_check_name(reader, "/dataarray");
    }
    else if (tag.get_value("name") == "types")
    {
      current_vtu_data.cell_types.resize(cellNum);
      for (std::size_t i=0; i<cellNum; ++i)
        reader >> current_vtu_data.cell_types.at(i);
      tag.parse_and_check_name(reader, "/dataarray");
    }
    else
      throw std::runtime_error("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: <DataArray> is not named 'connectivity', 'offsets' or 'types'!");
  }

  tag.parse_and_check_name(reader, "/cells");

  //
  // Read cell data if available
  //
  tag.parse(reader);
  if (tag.name() == "celldata")
  {
    err = viennagrid_mesh_io_read_vtu_data(reader, tag, current_vtu_data.cell_data, cellNum); if (err != VIENNAGRID_SUCCESS) return err;
    tag.parse(reader);
  }

  //
  // Make sure we approach the end of the file
  //
  if (tag.name() != "/piece")
    throw std::runtime_error("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: Expected </Piece> tag!");

  tag.parse_and_check_name(reader, "/unstructuredgrid");
  tag.parse_and_check_name(reader, "/vtkfile");

  reader.close();

  return VIENNAGRID_SUCCESS;
}




static viennagrid_error viennagrid_mesh_io_read_vtu_pvd(std::string pvd_filename, std::vector<std::string> & vtu_filenames)
{
  //extract path from .pvd file:
  std::string::size_type pos = pvd_filename.rfind("/");
  if (pos == std::string::npos)
    pos = pvd_filename.rfind("\\"); //a tribute to Windows... ;-)

  std::string path_to_pvd;
  if (pos != std::string::npos)
    path_to_pvd = pvd_filename.substr(0, pos + 1);

  std::ifstream reader(pvd_filename.c_str());

  //
  // Step 1: Get regions from pvd file:
  //
  xml_tag<> tag;

  tag.parse(reader);
  if (tag.name() != "?xml" && tag.name() != "?xml?")
    throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: No opening <?xml?> tag!");

  tag.parse(reader);
  if (tag.name() != "vtkfile")
    throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: VTKFile tag expected!");

  if (!tag.has_attribute("type"))
    throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: VTKFile tag has no attribute 'type'!");

  if (string_to_lower(tag.get_value("type")) != "collection")
    throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: Type-attribute of VTKFile tag is not 'Collection'!");

  tag.parse(reader);
  if (tag.name() != "collection")
    throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: Collection tag expected!");

  while (reader.good())
  {
    tag.parse(reader);

    if (tag.name() == "/collection")
      break;

    if (tag.name() != "dataset")
      throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: DataSet tag expected!");

    if (!tag.has_attribute("file"))
      throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: DataSet tag has no file attribute!");

    vtu_filenames.push_back(path_to_pvd + tag.get_value("file"));
  }

  tag.parse(reader);
  if (tag.name() != "/vtkfile")
    throw std::runtime_error("* ViennaGrid: vtk_reader::process_pvd(): Parse error: Closing VTKFile tag expected!");

  reader.close();

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_io_read_vtk_push_data(viennagrid_mesh_io mesh_io,
                                                       std::map<std::string, std::vector<viennagrid_numeric> > element_data,
                                                       std::vector<viennagrid_element_id> element_ids,
                                                       std::size_t num_elements,
                                                       viennagrid_dimension element_dim)
{
  viennagrid_error err;

  for (std::map<std::string, std::vector<viennagrid_numeric> >::const_iterator it  = element_data.begin();
                                                                               it != element_data.end();
                                                                             ++it)
  {
    viennagrid_quantity_field field;
    err = viennagrid_mesh_io_quantity_field_get(mesh_io, it->first.c_str(), &field); if (err != VIENNAGRID_SUCCESS) return err;
    viennagrid_dimension num_components = viennagrid_dimension(it->second.size() / num_elements);

    // create new field if not already present
    if (!field)
    {
      err = viennagrid_quantity_field_create(&field); if (err != VIENNAGRID_SUCCESS) return err;
      err = viennagrid_quantity_field_init(field, element_dim, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, num_components, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); if (err != VIENNAGRID_SUCCESS) return err;
      err = viennagrid_quantity_field_name_set(field, it->first.c_str()); if (err != VIENNAGRID_SUCCESS) return err;
      err = viennagrid_mesh_io_quantity_field_set(mesh_io, field); if (err != VIENNAGRID_SUCCESS) return err;
      err = viennagrid_quantity_field_release(field); if (err != VIENNAGRID_SUCCESS) return err; // field in mesh_io called retain(), so field will not be destroyed yet.
    }

    // push data:
    for (std::size_t i=0; i<num_elements; ++i)
    {
      err = viennagrid_quantity_field_value_set(field, viennagrid_index_from_element_id(element_ids[i]), const_cast<viennagrid_numeric*>(&(it->second[num_components * i]))); if (err != VIENNAGRID_SUCCESS) return err;
    }

  }

  return VIENNAGRID_SUCCESS;
}


/** @brief Top-level function for reading from VTK.
  *
  * If a .pvd file is supplied, the individual pieces are read from the .vtu files and placed in different regions.
  * If only a .vtu file is provided, a mesh with a single region is set up.
  */
viennagrid_error viennagrid_mesh_io_read_vtk(viennagrid_mesh_io mesh_io, std::string filename)
{
  viennagrid_error err;

  std::string::size_type pos  = filename.rfind(".") + 1;
  std::string extension = filename.substr(pos, filename.size());

  try
  {
    pvd_data mesh_data;
    mesh_data.point_dim = 0;

    if (extension == "vtu")
    {
      mesh_data.region_data.resize(1);
      err = viennagrid_mesh_io_read_vtu(filename, mesh_data, 0); if (err != VIENNAGRID_SUCCESS) return err;
    }
    else if (extension == "pvd")
    {
      // get number of segments and their filenames:
      std::vector<std::string> list_of_vtu_filenames;
      err = viennagrid_mesh_io_read_vtu_pvd(filename, list_of_vtu_filenames); if (err != VIENNAGRID_SUCCESS) return err;

      // read individual .vtu files:
      mesh_data.region_data.resize(list_of_vtu_filenames.size());
      for (std::size_t i=0; i<list_of_vtu_filenames.size(); ++i)
      {
        err = viennagrid_mesh_io_read_vtu(list_of_vtu_filenames[i], mesh_data, i); if (err != VIENNAGRID_SUCCESS) return err;
      }
    }
    else
      return VIENNAGRID_ERROR_UNKNOWN_FILETYPE;

    //
    // Now build up ViennaGrid mesh from data in 'mesh_data':
    //
    viennagrid_mesh mesh;
    err = viennagrid_mesh_io_mesh_get(mesh_io, &mesh); if (err != VIENNAGRID_SUCCESS) return err;

    err= viennagrid_mesh_geometric_dimension_set(mesh, mesh_data.point_dim); if (err != VIENNAGRID_SUCCESS) return err;

    // push vertices:
    std::vector<std::vector<viennagrid_numeric> > vertex_coords(mesh_data.vertex_singleton.size());

    for (pvd_data::VertexSingletonContainer::const_iterator it = mesh_data.vertex_singleton.begin(); it != mesh_data.vertex_singleton.end(); ++it)
      vertex_coords[it->second] = it->first;

    for (std::size_t i=0; i<vertex_coords.size(); ++i)
    {
      err = viennagrid_mesh_vertex_create(mesh, &(vertex_coords[i][0]), NULL ); if (err != VIENNAGRID_SUCCESS) return err;
    }

    // push vertex data:
    for (std::size_t k=0; k<mesh_data.region_data.size(); ++k)
    {
      err = viennagrid_mesh_io_read_vtk_push_data(mesh_io,
                                                  mesh_data.region_data[k].point_data,
                                                  mesh_data.region_data[k].local_to_global_map,
                                                  mesh_data.region_data[k].local_to_global_map.size(),
                                                  0);
      if (err != VIENNAGRID_SUCCESS) return err;
    }


    // push individual cells:
    for (std::size_t k=0; k<mesh_data.region_data.size(); ++k)
    {
      viennagrid_region region;
      err = viennagrid_mesh_region_create(mesh, &region); if (err != VIENNAGRID_SUCCESS) return err;

      vtu_data & current_vtu_data = mesh_data.region_data[k];
      current_vtu_data.cell_ids.resize(current_vtu_data.cell_types.size());
      for (std::size_t i=0; i<current_vtu_data.cell_types.size(); ++i)
      {
        viennagrid_element_type cell_type = from_vtk_element_type(current_vtu_data.cell_types[i]);
        viennagrid_int cell_offset = (i > 0) ? current_vtu_data.cell_offsets[i-1] : 0;
        viennagrid_int vertex_count = current_vtu_data.cell_offsets[i] - cell_offset;

        viennagrid_element_id element_id;
        err = viennagrid_mesh_element_create(mesh, cell_type, vertex_count, &(current_vtu_data.cell_connectivity[cell_offset]), &element_id); if (err != VIENNAGRID_SUCCESS) return err;
        err = viennagrid_region_element_add(region, element_id); if (err != VIENNAGRID_SUCCESS) return err;
        current_vtu_data.cell_ids[i] = element_id;
      }
    }

    // push cell data:
    for (std::size_t k=0; k<mesh_data.region_data.size(); ++k)
    {
      viennagrid_dimension topo_dim;
      err = viennagrid_mesh_cell_dimension_get(mesh, &topo_dim); if (err != VIENNAGRID_SUCCESS) return err;
      err = viennagrid_mesh_io_read_vtk_push_data(mesh_io,
                                                  mesh_data.region_data[k].cell_data,
                                                  mesh_data.region_data[k].cell_ids,
                                                  mesh_data.region_data[k].cell_ids.size(),
                                                  topo_dim);
      if (err != VIENNAGRID_SUCCESS) return err;
    }

  }
  catch (std::exception & e)
  {
    return VIENNAGRID_ERROR_READ_ERROR;
  }

  return VIENNAGRID_SUCCESS;
}

#endif
