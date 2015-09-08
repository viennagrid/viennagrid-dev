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

#include "viennagrid/viennagrid.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"


using std::string;


template<typename ReaderT, typename MeshT>
bool dump_information(MeshT & mesh, string const & filename)
{
  // trying to read the file
  try
  {
    ReaderT reader;
    reader(mesh, filename);
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
  std::cout << "Topologic Dimension : " << viennagrid::cell_dimension(mesh) << std::endl;
  std::cout << "Geometric Dimension : " << viennagrid::geometric_dimension(mesh) << std::endl;
  std::cout << "Number of vertices  : " << viennagrid::vertices(mesh).size() << std::endl;
  std::cout << "Number of cells     : " << viennagrid::cells(mesh).size() << std::endl;
  std::cout << std::endl;

  // dump segment information
  std::cout << "Number of segments: " << mesh.region_count() << std::endl;
  typedef typename viennagrid::result_of::const_region_range<MeshT>::type ConstRegionRangeType;
  typedef typename viennagrid::result_of::iterator<ConstRegionRangeType>::type ConstRegionRangeIterator;

  ConstRegionRangeType regions(mesh);
//   for (typename SegmentationType::iterator it = segmentation.begin(); it != segmentation.end(); ++it)
  for (ConstRegionRangeIterator it = regions.begin(); it != regions.end(); ++it)
  {
    std::cout << "  Segment " << std::endl;
    std::cout << "    Segment ID        : " << (*it).id() << std::endl;
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
  typedef viennagrid::mesh MeshType;

  if (type == "tet3d")
  {
    MeshType mesh;

    if (extension == "mesh")
      return dump_information<viennagrid::io::netgen_reader>(mesh, filename);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<viennagrid::io::vtk_reader<MeshType> >(mesh, filename);
  }
  else if(type == "tri2d")
  {
    MeshType mesh;

    if (extension == "mesh")
      return dump_information<viennagrid::io::netgen_reader>(mesh, filename);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<viennagrid::io::vtk_reader<MeshType> >(mesh, filename);
  }
  else if(type == "tri3d")
  {
    MeshType mesh;

    if (extension == "mesh")
      return dump_information<viennagrid::io::netgen_reader>(mesh, filename);
    else if ((extension == "pvd") || (extension == "vtk"))
      return dump_information<viennagrid::io::vtk_reader<MeshType> >(mesh, filename);
  }

  return EXIT_SUCCESS;
}











