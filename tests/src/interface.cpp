/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/volume.hpp"

template <typename DomainType, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile)
{

  typedef typename viennagrid::result_of::cell_tag<DomainType>::type          CellTag;
  typedef typename viennagrid::result_of::segmentation<DomainType>::type      SegmentationType;
  typedef typename viennagrid::result_of::segment<SegmentationType>::type     SegmentType;
  
  
  typedef typename viennagrid::result_of::point<DomainType>::type             PointType;
  typedef typename viennagrid::result_of::vertex<DomainType>::type            VertexType;
  typedef typename viennagrid::result_of::line<DomainType>::type              EdgeType;
  typedef typename viennagrid::result_of::facet<DomainType>::type             FacetType;
  typedef typename viennagrid::result_of::cell<DomainType>::type              CellType;
                                            
  typedef typename viennagrid::result_of::vertex_range<DomainType>::type        VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type       VertexIterator;
                                            
  typedef typename viennagrid::result_of::line_range<DomainType>::type          EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type         EdgeIterator;

  typedef typename viennagrid::result_of::facet_range<DomainType>::type         FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type        FacetIterator;

  typedef typename viennagrid::result_of::cell_range<DomainType>::type          CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
  
  typedef typename viennagrid::result_of::vertex_range<EdgeType>::type          VertexOnEdgeContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type VertexOnEdgeIterator;
  
  
  DomainType domain;
  SegmentationType segmentation(domain);

  //read from file:
  try
  {
    my_reader(domain, segmentation, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }

  assert(segmentation.size() > 1 && "Not enough segments!");
  
  typename SegmentationType::iterator it = segmentation.begin();
  SegmentType const & seg1 = *it; ++it;
  SegmentType const & seg2 = *it;

  //
  // Test 1: Iterate over all interface facets:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 1: Iteration over all facets on the boundary:" << std::endl;
  std::cout << "*" << std::endl;
  double surface = 0;
  FacetContainer facets = viennagrid::elements(domain);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    if (viennagrid::is_interface(seg1, seg2, *fit))
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
  VertexContainer vertices = viennagrid::elements(domain);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    if (viennagrid::is_interface(seg1, seg2, *vit))
    {
      assert( viennagrid::point(domain ,*vit)[0] == 0.5 && "Invalid point detected on interface!");
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
  typedef viennagrid::triangular_2d_domain        DomainTri;
  typedef viennagrid::tetrahedral_3d_domain       DomainTet;
  typedef viennagrid::quadrilateral_2d_domain     DomainQuad;
  typedef viennagrid::hexahedral_3d_domain        DomainHex;
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  std::cout << "*********** triangular, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainTri>  vtk_reader_tri;
  test<DomainTri>(vtk_reader_tri, path + "multi_segment_tri_main.pvd");
  
  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainTet>  vtk_reader_tet;
  test<DomainTet>(vtk_reader_tet, path + "multi_segment_tet_main.pvd");

  
  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainQuad>  vtk_reader_quad;
  test<DomainQuad>(vtk_reader_quad, path + "multi_segment_quad_main.pvd");
  
  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainHex>  vtk_reader_hex;
  test<DomainHex>(vtk_reader_hex, path + "multi_segment_hex_main.pvd");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
