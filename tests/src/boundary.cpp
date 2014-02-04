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
//#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/volume.hpp"

template <typename CellTypeOrTag, typename Mesh, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile, double reference_surface)
{
  //typedef typename viennagrid::result_of::mesh< MeshConfig >::type Mesh;
  typedef typename viennagrid::result_of::segmentation<Mesh>::type Segmentation;
//   typedef typename viennagrid::result_of::segment<Mesh>::type Segment;

  typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
  //typedef viennagrid::result_of::element<Mesh, CellTag>::type CellType;


  //typedef typename viennagrid::result_of::mesh<ConfigType>::type        Mesh;
  //typedef typename ConfigType::cell_tag                                   CellTag;
  //typedef typename viennagrid::result_of::segment<ConfigType>::type       SegmentType;

  typedef typename viennagrid::result_of::element_range<Mesh, viennagrid::vertex_tag>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;

  typedef typename viennagrid::result_of::element_range<Mesh, typename CellTag::facet_tag>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  Mesh mesh;
  Segmentation segmentation(mesh);

  //read from file:
  try
  {
    my_reader(mesh, segmentation, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }


  //
  // Test 1: Iterate over all cells of each facet:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 1: Iteration over all facets on the boundary:" << std::endl;
  std::cout << "*" << std::endl;
  double surface = 0;
  FacetContainer facets = viennagrid::elements<typename CellTag::facet_tag>(mesh);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    if (viennagrid::is_boundary(mesh, *fit))
    {
      std::cout << *fit << std::endl;
      surface += viennagrid::volume(*fit);
    }
  }

  std::cout << "Volume of surface elements: " << surface << std::endl;

  if ( fabs(reference_surface - surface) / reference_surface > 1e-6 )
  {
    std::cout << "Surface area incorrect! Should: " << reference_surface << ", is: " << surface << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Test 2: Print all vertices on the boundary:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all vertices on the boundary" << std::endl;
  std::cout << "*" << std::endl;
  VertexContainer vertices(mesh);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    if (viennagrid::is_boundary(mesh, *vit))
      std::cout << *vit << std::endl;
  }

}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  //viennagrid::io::netgen_reader my_netgen_reader;

  std::cout << "*********** line, 1d ***********" << std::endl;
  viennagrid::io::netgen_reader line_1d_reader;
  test<viennagrid::line_tag, viennagrid::line_1d_mesh>(line_1d_reader, path + "line8.mesh", 2);


  std::cout << "*********** triangular, 2d ***********" << std::endl;
  viennagrid::io::netgen_reader triangle_2d_reader;
  test<viennagrid::triangle_tag, viennagrid::triangular_2d_mesh>(triangle_2d_reader, path + "square8.mesh", 4);

  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  viennagrid::io::netgen_reader tetrahedra_3d_reader;
  test<viennagrid::tetrahedron_tag, viennagrid::tetrahedral_3d_mesh>(tetrahedra_3d_reader, path + "cube48.mesh", 6);


  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<viennagrid::quadrilateral_2d_mesh>  quadrilateral_2d_reader;
  test<viennagrid::quadrilateral_tag, viennagrid::quadrilateral_2d_mesh>(quadrilateral_2d_reader, path + "quadrilateral2.vtu", 6);

  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<viennagrid::hexahedral_3d_mesh>  hexahedron_3d_reader;
  test<viennagrid::hexahedron_tag, viennagrid::hexahedral_3d_mesh>(hexahedron_3d_reader, path + "hexahedron2.vtu", 10);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
