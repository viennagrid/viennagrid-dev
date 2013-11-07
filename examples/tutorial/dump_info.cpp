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
#include <boost/concept_check.hpp>



using std::string;


template<typename MeshT, typename ReaderT>
bool dump_information(string const & filename)
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

  // dump mesh information
  std::cout << "Information for Mesh '" << filename << "'" << std::endl;
  std::cout << std::endl;
  std::cout << "Cell Type           : " << viennagrid::result_of::cell_tag<MeshT>::type::name() << std::endl;
  std::cout << "Geometric Dimension : " << typename viennagrid::result_of::point<MeshT>::type().size() << std::endl;
  std::cout << "Number of vertices  : " << viennagrid::vertices(mesh).size() << std::endl;
  std::cout << "Number of cells     : " << viennagrid::cells(mesh).size() << std::endl;
  std::cout << std::endl;

  // dump segment information
  std::cout << "Number of segments: " << segmentation.size() << std::endl;
  for (typename SegmentationType::iterator it = segmentation.begin(); it != segmentation.end(); ++it)
  {
    std::cout << "  Segment " << std::endl;
    std::cout << "    Segment ID        : " << it->id() << std::endl;
    std::cout << "    Number of vertices: " << viennagrid::vertices(*it).size() << std::endl;
    std::cout << "    Number of cells   : " << viennagrid::cells(*it).size() << std::endl;
  }

  return true;
}






int main(int argc, char *argv[])
{
  // checking number of arguments
  if (argc != 3)
  {
    std::cout << "Usage: dump_info <CELL_TYPE> <FILE_NAME>" << std::endl;
    std::cout << "  Supported cell types: tet3d, tri2d, tri3d" << std::endl;
    std::cout << "  Supported file format: .mesh (Netgen), .pvd and .vtu (VTK)" << std::endl;
    std::cout << std::endl;
    std::cout << "Example: dump_info tet3d test.mesh" << std::endl;
    return EXIT_FAILURE;
  }

  // querying arguments: cell type and filename
  string type = argv[1];
  string filename = argv[2];
  string extension = filename.substr( filename.rfind(".")+1 );

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
      return dump_information<MeshType, viennagrid::io::netgen_reader>(filename);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<MeshType, viennagrid::io::vtk_reader<MeshType> >(filename);
  }
  else if(type == "tri2d")
  {
    typedef viennagrid::triangular_2d_mesh MeshType;

    if (extension == "mesh")
      return dump_information<MeshType, viennagrid::io::netgen_reader>(filename);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<MeshType, viennagrid::io::vtk_reader<MeshType> >(filename);
  }
  else if(type == "tri3d")
  {
    typedef viennagrid::triangular_3d_mesh MeshType;

    if (extension == "mesh")
      return dump_information<MeshType, viennagrid::io::netgen_reader>(filename);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<MeshType, viennagrid::io::vtk_reader<MeshType> >(filename);
  }

  return EXIT_SUCCESS;
}











