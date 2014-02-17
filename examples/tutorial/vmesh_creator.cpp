/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
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

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vmesh_writer.hpp"


template<typename MeshT, typename ReaderT>
bool dump_information(std::string const & filename, std::string const & outputfile)
{
  // defining and creating a mesh and a segmentation
  typedef typename viennagrid::result_of::segmentation<MeshT>::type SegmentationType;
  MeshT mesh;
  SegmentationType segmentation(mesh);

  // trying to read the file
  try
  {
    ReaderT reader;
    reader(mesh, segmentation, filename);
  }
  catch (std::exception & e)
  {
    std::cout << "Error reading mesh file: " << filename << std::endl;
    std::cout << e.what() << std::endl;
    return false;
  }

  viennagrid::io::vmesh_writer<MeshT> writer;
  writer(mesh, segmentation, filename, outputfile);

  return true;
}






int main(int argc, char *argv[])
{
  // checking number of arguments
  if (argc != 4)
  {
    std::cout << "Usage: " << argv[0] << " <CELL_TYPE> <INPUT_FILE> <OUTPUT_FILE>" << std::endl;
    std::cout << "  Supported cell types: tet3d, tri2d, tri3d" << std::endl;
    std::cout << "  Supported file format: .mesh (Netgen), .pvd and .vtu (VTK)" << std::endl;
    std::cout << std::endl;
    std::cout << "Example: " << argv[0] << " tet3d test.mesh test.vmesh" << std::endl;
    return EXIT_FAILURE;
  }

  // querying arguments: cell type and filename
  std::string type = argv[1];
  std::string filename = argv[2];
  std::string outputfile = argv[3];
  std::string extension = filename.substr( filename.rfind(".")+1 );

  // check if cell type is supported
  if ((type != "tet3d") && (type != "tri2d") && (type != "tri3d"))
  {
    std::cout << "Cell type not supported: " << extension << std::endl;
    std::cout << "  Supported cell types: tet3d, tri2d, tri3d" << std::endl;
    return EXIT_FAILURE;
  }

  // check if file format is supported
  if ((extension != "mesh") && (extension != "pvd") && (extension != "vtk"))
  {
    std::cout << "File format not supported: " << extension << std::endl;
    std::cout << "  Supported file format: .mesh (Netgen), .pvd and .vtu (VTK)" << std::endl;
    return EXIT_FAILURE;
  }

  // call the corresponding dump_info method using the correct mesh type and reader class
  if (type == "tet3d")
  {
    typedef viennagrid::tetrahedral_3d_mesh MeshType;

    if (extension == "mesh")
      return dump_information<MeshType, viennagrid::io::netgen_reader>(filename, outputfile);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<MeshType, viennagrid::io::vtk_reader<MeshType> >(filename, outputfile);
  }
  else if(type == "tri2d")
  {
    typedef viennagrid::triangular_2d_mesh MeshType;

    if (extension == "mesh")
      return dump_information<MeshType, viennagrid::io::netgen_reader>(filename, outputfile);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<MeshType, viennagrid::io::vtk_reader<MeshType> >(filename, outputfile);
  }
  else if(type == "tri3d")
  {
    typedef viennagrid::triangular_3d_mesh MeshType;

    if (extension == "mesh")
      return dump_information<MeshType, viennagrid::io::netgen_reader>(filename, outputfile);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<MeshType, viennagrid::io::vtk_reader<MeshType> >(filename, outputfile);
  }

  return EXIT_SUCCESS;
}











