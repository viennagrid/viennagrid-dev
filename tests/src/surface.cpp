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

// #include "viennagrid/algorithm/boundary.hpp"
#include "viennagridpp/algorithm/centroid.hpp"
// #include "viennagrid/config/default_configs.hpp"
#include "viennagridpp/io/vtk_reader.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
// #include "viennagrid/io/opendx_writer.hpp"
#include "viennagridpp/io/netgen_reader.hpp"
#include "viennagridpp/algorithm/volume.hpp"
#include "viennagridpp/algorithm/surface.hpp"


template <typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile)
{
  typedef viennagrid::mesh                                              MeshType;
  MeshType mesh;

  typedef typename viennagrid::result_of::cell_range<MeshType>::type    CellRange;
  typedef typename viennagrid::result_of::iterator<CellRange>::type     CellIterator;

  try
  {
    my_reader(mesh, infile);
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }

  std::cout << "Volume of mesh: " << viennagrid::volume(mesh) << std::endl;
  std::cout << "Surface of mesh: " << viennagrid::surface(mesh) << std::endl;

  CellRange cells(mesh);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                   ++cit)
  {
    //std::cout << *cit << std::endl;
    std::cout << "Surface: " << viennagrid::surface(*cit) << std::endl;
  }
}


int main()
{
  typedef viennagrid::mesh MeshType;

  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  viennagrid::io::netgen_reader my_netgen_reader;

  {
    std::cout << "*********** triangular, 2d ***********" << std::endl;
    test(my_netgen_reader, path + "square32.mesh");
  }

  {
    std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
    test(my_netgen_reader, path + "cube48.mesh");
  }


  {
    std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
    viennagrid::io::vtk_reader<MeshType>  vtk_reader_2d;
    test(vtk_reader_2d, path + "quadrilateral2.vtu");
  }

  {
    std::cout << "*********** hexahedral, 3d ***********" << std::endl;
    viennagrid::io::vtk_reader<MeshType>  vtk_reader_3d;
    test(vtk_reader_3d, path + "hexahedron2.vtu");
  }


  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
