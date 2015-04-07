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

#include "viennagridpp/core.hpp"
#include "viennagridpp/io/netgen_reader.hpp"
#include "viennagridpp/io/vtk_reader.hpp"
#include "viennagridpp/algorithm/volume.hpp"

template <int dimension, typename CellTagT, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile)
{
  typedef viennagrid::mesh_t MeshType;
  typedef typename viennagrid::result_of::region<MeshType>::type      RegionType;
//   typedef typename viennagrid::result_of::segment_handle<SegmentationType>::type     SegmentHandleType;

  typedef viennagrid::result_of::vertex_range<MeshType>::type        VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type       VertexIterator;

  typedef viennagrid::result_of::facet_range<MeshType>::type         FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type        FacetIterator;

  MeshType mesh;

  //read from file:
  try
  {
    my_reader(mesh, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }

  assert(mesh.region_count() > 1 && "Not enough segments!");

  typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
  typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

  RegionRangeType regions(mesh);
  RegionRangeIterator it = regions.begin();
  RegionType region1 = *it; ++it;
  RegionType region2 = *it;

  //
  // Test 1: Iterate over all interface facets:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 1: Iteration over all facets on the boundary:" << std::endl;
  std::cout << "*" << std::endl;
  double surface = 0;
  FacetContainer facets(mesh);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    if (viennagrid::is_interface(region1, region2, *fit))
    {
      std::cout << *fit << std::endl;
      surface += viennagrid::volume(*fit);
    }
  }

  std::cout << "Volume of interface elements: " << surface << std::endl;

  /*if ( fabs(reference_surface - surface) / reference_surface > 1e-6 )
  {
    std::cout << "Surface area incorrect! Should: " << reference_surface << ", is: " << surface << std::endl;
    exit(EXIT_FAILURE);
  }*/

  //
  // Test 2: Print all vertices on the interface:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all vertices on the interface" << std::endl;
  std::cout << "*" << std::endl;
  bool vertices_found = false;
  VertexContainer vertices(mesh);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    if (viennagrid::is_interface(region1, region2, *vit))
    {
      assert( viennagrid::get_point(mesh ,*vit)[0] == 0.5 && "Invalid point detected on interface!");
      std::cout << *vit << std::endl;
      vertices_found = true;
    }
  }

  if (!vertices_found)
  {
    std::cerr << "ERROR: No vertices found!" << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main()
{
  typedef viennagrid::mesh_t MeshType;

  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  std::cout << "*********** triangular, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<MeshType>  vtk_reader_tri;
  test<2, viennagrid::triangle_tag>(vtk_reader_tri, path + "multi_segment_tri_main.pvd");

  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<MeshType>  vtk_reader_tet;
  test<3, viennagrid::tetrahedron_tag>(vtk_reader_tet, path + "multi_segment_tet_main.pvd");


  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<MeshType>  vtk_reader_quad;
  test<2, viennagrid::quadrilateral_tag>(vtk_reader_quad, path + "multi_segment_quad_main.pvd");

  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<MeshType>  vtk_reader_hex;
  test<3, viennagrid::hexahedron_tag>(vtk_reader_hex, path + "multi_segment_hex_main.pvd");

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
