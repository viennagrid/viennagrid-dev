/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

//NOTE: This test does not check run-time output at the moment and rather serves as compilation test.

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/core.hpp"
#include "viennagrid/io/netgen_reader.hpp"

template <typename CellTypeOrTag>
void test(int dimension, viennagrid::element_tag_t element_tag, std::string infile)
{
  typedef viennagrid::mesh_t MeshType;
  typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;

  //typedef typename viennagrid::result_of::mesh<ConfigType>::type        Mesh;
  //typedef typename ConfigType::cell_tag                                   CellTag;
//   typedef typename viennagrid::result_of::segmentation<MeshType>::type       SegmentationType;

  typedef typename viennagrid::result_of::vertex<MeshType>::type       VertexType;
  typedef typename viennagrid::result_of::facet<MeshType>::type FacetType;

  typedef typename viennagrid::result_of::vertex_range<MeshType>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
//   typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexHandleIterator;

  typedef typename viennagrid::result_of::facet_range<MeshType>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;
//   typedef typename viennagrid::result_of::hook_iterator<FacetContainer>::type                         FacetHandleIterator;

  MeshType mesh(dimension, element_tag);

  //read from file:
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(mesh, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Test 1: Iterate over all edges of each vertex:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all edges adjacent to each vertex" << std::endl;
  std::cout << "*" << std::endl;
  VertexContainer vertices = viennagrid::elements<viennagrid::vertex_tag>(mesh);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    const VertexType & vertex = *vit;//viennagrid::dereference_hook(mesh, *vit);
    std::cout << vertex << std::endl;
    typedef typename viennagrid::result_of::coboundary_range<MeshType, viennagrid::line_tag>::type          EdgeOnVertexContainer;
    typedef typename viennagrid::result_of::iterator<EdgeOnVertexContainer>::type     EdgeOnVertexIterator;

    EdgeOnVertexContainer edges (mesh, *vit);
    for (EdgeOnVertexIterator eovit = edges.begin();
         eovit != edges.end();
         ++eovit)
    {
      std::cout << *eovit << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }

  //
  // Test 2: Iterate over all cells of each facet:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all cells adjacent to each facet" << std::endl;
  std::cout << "*" << std::endl;
  FacetContainer facets(mesh);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    FacetType facet = *fit; //viennagrid::dereference_hook(mesh, *fit);
    std::cout << facet << std::endl;
    typedef typename viennagrid::result_of::coboundary_range<MeshType, CellTag>::type   CellOnFacetContainer;
    typedef typename viennagrid::result_of::iterator<CellOnFacetContainer>::type                          CellOnFacetIterator;

    CellOnFacetContainer cells(mesh, *fit);
    for (CellOnFacetIterator eovit = cells.begin();
         eovit != cells.end();
         ++eovit)
    {
      std::cout << *eovit << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }

}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  std::cout << "Testing 1d..." << std::endl;
  test<viennagrid::line_tag>(1, viennagrid::line_tag(), path + "line8.mesh");
  std::cout << "Testing 2d..." << std::endl;
  test<viennagrid::triangle_tag>(2, viennagrid::triangle_tag(), path + "square32.mesh");
  std::cout << "Testing 3d..." << std::endl;
  test<viennagrid::tetrahedron_tag>(3, viennagrid::tetrahedron_tag(), path + "cube48.mesh");

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
