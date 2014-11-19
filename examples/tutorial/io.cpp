/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/core.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"

struct UserData
{
  std::vector<double>                  vertex_scalar_array;
  std::vector<double>                  cell_scalar_array;
  std::vector<std::vector<double> >    vertex_vector_array;
  std::vector<std::vector<double> >    cell_vector_array;
  std::vector<std::vector<double> >    vertex_normal_array;
  std::vector<std::vector<double> >    cell_normal_array;
};

//
// Shows how to read a mesh from Netgen mesh files
//
template <typename MeshType>
void read_netgen(MeshType & mesh)
{

  viennagrid::io::netgen_reader my_netgen_reader;
  my_netgen_reader(mesh, "../data/cube48.mesh");

  //
  // Note that the Netgen format supports multiple segments, which will be automatically created by the reader
  //
  std::cout << "Number of segments in Netgen file: " << mesh.region_count() << std::endl;

  // do more stuff with the mesh here.
}


//
// Write to IBM Vizualization DataExplorer (OpenDX, http://www.opendx.org/) file
//
template <typename MeshType, typename UserData>
void write_opendx(MeshType const & mesh, UserData & data)
{
  typedef typename viennagrid::result_of::vertex< MeshType >::type        vertex_type;

  // Instantiate writer object:
  viennagrid::io::opendx_writer<MeshType> my_dx_writer;

  // Add scalar vertex data: Note that only the first data is used.
//   viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_dx_writer, "vtk_data", "data_double");

  my_dx_writer.add_scalar_data_on_vertices( viennagrid::make_accessor<vertex_type>(data.vertex_scalar_array), "data_double" );

  // Add scalar cell data: Note that only the first data is used and that no other vertex data must be present!
  //viennagrid::io::add_scalar_data_on_cells<std::string, double>(my_dx_writer, "vtk_data", "cell_data_double");


  my_dx_writer(mesh, "tutorial_io.out");
}


//
// Read the mesh from the VTK File format (XML)
//
template <typename MeshType, typename UserData>
void read_vtk(MeshType & mesh, UserData & data)
{
  typedef typename viennagrid::result_of::cell< MeshType >::type     cell_type;
  typedef typename viennagrid::result_of::vertex< MeshType >::type        vertex_type;

  //
  // Step 1: Instantiate reader object
  //
  viennagrid::io::vtk_reader<MeshType> reader;

  //-----------------------------------------------------

  //
  // Step 2: Specify where to write the vertex data.
  //         By default, data is written as type double or std::vector<double> to ViennaData using a std::string that matches the name in the VTK file.
  //

  // Write scalar vertex data that matches the name 'data_double' to ViennaData as data of type double, using a key of type std::string and value "vtk_data":
  reader.register_vertex_scalar( viennagrid::make_accessor<vertex_type>(data.vertex_scalar_array), "data_double" );

  // Write vector-valued vertex data that matches the name 'data_point' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data":
//   reader.register_vertex_vector_accessor( viennagrid::make_accessor<vertex_type>(data.vertex_vector_array), "data_point" );

  // Write normal (vector-valued) vertex data that matches the name 'data_normal' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data_2":
  //viennagrid::io::add_normal_data_on_vertices<std::string, std::vector<double> >(reader, "vtk_data_2", "data_normal");

  //-----------------------------------------------------

  //
  // Step 3: Specify where to write the cell data.
  //         By default, data is written as type double or std::vector<double> to ViennaData using a std::string that matches the name in the VTK file.
  //

  // Write scalar cell data that matches the name 'data_double' to ViennaData as data of type double, using a key of type std::string and value "vtk_data":
  reader.register_cell_scalar( viennagrid::make_accessor<cell_type>(data.cell_scalar_array), "data_double" );

  // Write vector-valued cell data that matches the name 'data_point' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data":
//   reader.register_cell_vector_accessor( viennagrid::make_accessor<cell_type>(data.cell_vector_array), "data_point" );

  // Write normal (vector-valued) cell data that matches the name 'data_normal' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data_2":
  //viennagrid::io::add_normal_data_on_cells<std::string, std::vector<double> >(reader, "vtk_data_2", "data_normal");


  //-----------------------------------------------------

  //
  // Step 4: Trigger filereader:
  //
  reader(mesh, "../data/tets_with_data_main.pvd");


  //
  // Step 5: Get and print all names read from vtk file(s):
  //

  std::cout << "--- Data read from VTK file: ---" << std::endl;
  std::cout << "* Scalar data on vertices: " << std::endl;
  for (size_t i=0; i<data.vertex_scalar_array.size(); ++i)
    std::cout << (data.vertex_scalar_array)[i] << std::endl;

  std::cout << "* Vector data on vertices: " << std::endl;
  for (size_t i=0; i<data.vertex_vector_array.size(); ++i)
  {
    for (size_t j=0; j<data.vertex_vector_array[i].size(); ++j)
      std::cout << (data.vertex_vector_array)[i][j] << " ";
    std::cout << std::endl;
  }

  std::cout << "* Scalar data on cells: " << std::endl;
  for (size_t i=0; i<data.cell_scalar_array.size(); ++i)
    std::cout << (data.cell_scalar_array)[i] << std::endl;

  std::cout << "* Vector data on cells: " << std::endl;
  for (size_t i=0; i<data.cell_vector_array.size(); ++i)
  {
    for (size_t j=0; j<data.cell_vector_array[i].size(); ++j)
      std::cout << (data.cell_vector_array)[i][j] << " ";
    std::cout << std::endl;
  }
  std::cout << "--- End of data ---" << std::endl;
}




//
// Write the mesh to VTK File format (XML)
//
template <typename MeshType, typename UserData>
void write_vtk(MeshType & mesh, UserData & data)
{
  typedef typename viennagrid::result_of::cell< MeshType >::type     cell_type;
  typedef typename viennagrid::result_of::vertex< MeshType >::type        vertex_type;

  //
  // Step 1: Instantiate a writer object:
  //
  viennagrid::io::vtk_writer<MeshType> writer;

  //-----------------------------------------------------

  //
  // Step 2: Add vertex data to the writer. In the following, keys of type std::string and value "vtk_data" are used as the ViennaData accessor.
  //         The third function argument is the name of the quantity as it will appear in the VTK file.
  //

  // Write data of type data that is stored with ViennaData for keys of type std::string with value "vtk_data" and call it "data_double" in the VTK file:
  //viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "vtk_data", "data_double");
  writer.add_scalar_data_on_vertices( viennagrid::make_accessor<vertex_type>(data.vertex_scalar_array), "data_double" );

  // Same as above, but with data of type long, which is then called "data_long" in the VTK file:
  //viennagrid::io::add_scalar_data_on_vertices<std::string, long>(my_vtk_writer, "vtk_data", "data_long");

  // Vector-valued data of type std::vector<double> is written to the VTK file and named "data_vector". Data is accessed with ViennaData using a std::string "vtk_data" as key.
  //viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_point");
//   writer.add_vector_data_on_vertices( viennagrid::make_accessor<vertex_type>(data.vertex_vector_array), "data_point" );

  // Same as vector-data, but with the constraint that each vector has length 1
  //viennagrid::io::add_normal_data_on_vertices<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_normal");

  //-----------------------------------------------------

  //
  // Step 3: Add cell data to the writer. Works in the same way as vertex data
  //

  // Write data of type double, using the std::string key "vtk_data". Name in VTK file is "data_double" (there is no name collision with vertex data here).
  //viennagrid::io::add_scalar_data_on_cells<std::string, double>(my_vtk_writer, "vtk_data", "data_double");
  writer.add_scalar_data_on_cells( viennagrid::make_accessor<cell_type>(data.cell_scalar_array), "data_double" );

  // Vector valued data on cells. Similar to vertex case
  //viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_vector");
//   writer.add_vector_data_on_cells( viennagrid::make_accessor<cell_type>(data.cell_vector_array), "data_vector" );

  // Vector valued data on cells. Just like vector data, but with a normalization requirement to length 1.
  //viennagrid::io::add_normal_data_on_cells<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_normal");

  //-----------------------------------------------------

  //
  // Final Step: Trigger the write process:
  //
  writer(mesh, "tutorial_io.vtu");

}

/*
template <typename CellTypeOrTag, typename MeshType>
void write_data(MeshType & mesh)
{
  typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;

  typedef typename viennagrid::result_of::element_range<MeshType, viennagrid::vertex_tag>::type           VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;

  typedef typename viennagrid::result_of::element_range<MeshType, CellTag>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;


  //write x-component to each vertex
  VertexContainer vertices = viennagrid::elements<viennagrid::vertex_tag>(mesh);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    viennadata::access<std::string, double>("vtk_data")(*vit) = viennagrid::point(mesh, *vit)[0];
  }

  //write x-component of circumcenter to each cell
  CellContainer cells = viennagrid::elements<CellTag>(mesh);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    viennadata::access<std::string, double>("vtk_data")(*cit) = viennagrid::circumcenter(*cit, mesh)[0];
  }
} */

/*struct my_mesh_config
{
  typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > PointType;  //use this for a 3d examples

  typedef viennagrid::config::result_of::full_mesh_config< viennagrid::tetrahedron_tag, PointType, viennagrid::storage::id_handle_tag >::type   type;
};*/

int main()
{
  //typedef viennagrid::mesh< my_mesh_config >          Mesh;
  typedef viennagrid::mesh_t     MeshType;
//   typedef viennagrid::result_of::segmentation<Mesh>::type              Segmentation;

  std::cout << "----------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: IO operations --" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << std::endl;

  MeshType mesh;
//   Segmentation segments(mesh);

  //
  // Use-case 1: Read from Netgen mesh files
  //
  read_netgen(mesh);

  //write a bit of data to mesh:
  //write_data<viennagrid::tetrahedron_tag>(mesh);


  //
  // Use-case 2: Read VTK file(s)
  //
  MeshType vtk_mesh;
//   Segmentation vtk_segments(vtk_mesh);
  UserData data;
  read_vtk(vtk_mesh, data);


  //
  // Use-case 3: Write to OpenDX
  //
  write_opendx(vtk_mesh, data);


  //
  // Use-case 4: Write VTK file(s)
  //
  write_vtk(vtk_mesh, data);

  std::cout << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
