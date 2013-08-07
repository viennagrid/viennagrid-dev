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

#include "refinement-common.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

template <typename DomainT>
void test(std::string & infile, std::string & outfile)
{
  typedef typename viennagrid::result_of::cell_tag<DomainT>::type               CellTag;
  typedef typename viennagrid::result_of::segmentation<DomainT>::type           SegmentationType;
  typedef typename viennagrid::result_of::segment<SegmentationType>::type       SegmentType;
  
  typedef typename viennagrid::result_of::point<DomainT>::type          PointType;
  typedef typename viennagrid::result_of::vertex<DomainT>::type       VertexType;
  typedef typename viennagrid::result_of::line<DomainT>::type       EdgeType;
  typedef typename viennagrid::result_of::cell<DomainT>::type   CellType;


  typedef typename viennagrid::result_of::vertex_range<DomainT>::type           VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
      
  typedef typename viennagrid::result_of::line_range<DomainT>::type           EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef typename viennagrid::result_of::facet_range<DomainT>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  typedef typename viennagrid::result_of::cell_range<DomainT>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;

  DomainT domain;
  SegmentationType segmentation(domain);
  
  try{
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(domain, segmentation, infile);
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }


  double input_domain_volume = viennagrid::volume(domain);
  double input_domain_surface = domain_surface(domain);
  
  std::cout << "Volume of input domain: " << input_domain_volume << std::endl;
  std::cout << "Surface of input domain: " << input_domain_surface << std::endl;
  
  std::vector<bool> cell_refinement_tag_container1;
  typename viennagrid::result_of::field<std::vector<bool>, CellType>::type cell_refinement_tag_field1(cell_refinement_tag_container1);

  //Testing: Tag some cells with centroid at x \in [2,3]:
  CellContainer cells = viennagrid::elements(domain);
  for (CellIterator cit  = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    PointType centroid = viennagrid::centroid(*cit);
    
    if ( (centroid[0] >= 2.0) 
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0) 
        && (centroid[1] <= 1.0))
    {
      cell_refinement_tag_field1(*cit) = true;
    }
  }

  DomainT refined_domain;
  SegmentationType refined_segmentation(refined_domain);
  
  viennagrid::cell_refine( domain, segmentation, refined_domain, refined_segmentation, cell_refinement_tag_field1 );

  std::cout << "Volume of refined domain: " << viennagrid::volume(refined_domain) << std::endl;
  std::cout << "Surface of refined domain: " << domain_surface(refined_domain) << std::endl;
  
  if ( fabs(viennagrid::volume(refined_domain) - input_domain_volume) / input_domain_volume > 1e-3 )
  {
    std::cerr << "Error in check: Domain volumes mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( fabs(domain_surface(refined_domain) - input_domain_surface) / input_domain_surface > 1e-3 )
  {
    std::cerr << "Error in check: Domain surfaces mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Second pass:
  //
  std::vector<bool> cell_refinement_tag_container2;
  typename viennagrid::result_of::field<std::vector<bool>, CellType>::type cell_refinement_tag_field2(cell_refinement_tag_container2);
  
  CellContainer cells_refined = viennagrid::elements(refined_domain);
  for (CellIterator cit  = cells_refined.begin();
                    cit != cells_refined.end();
                  ++cit)
  {
    PointType centroid = viennagrid::centroid(*cit);
    
    if ( (centroid[0] >= 2.0) 
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0) 
        && (centroid[1] <= 1.0))
    {
      cell_refinement_tag_field2(*cit) = true;
    }
  }

  DomainT double_refined_domain;
  SegmentationType double_refined_segmentation(double_refined_domain);
  viennagrid::cell_refine( refined_domain, refined_segmentation, double_refined_domain, double_refined_segmentation, cell_refinement_tag_field2 );
  
  std::cout << "Volume of double refined domain: " << viennagrid::volume(double_refined_domain) << std::endl;
  std::cout << "Surface of double refined domain: " << domain_surface(double_refined_domain) << std::endl;
  
  if ( fabs(viennagrid::volume(double_refined_domain) - input_domain_volume) / input_domain_volume > 1e-3 )
  {
    std::cerr << "Error in check: Domain volumes mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( fabs(domain_surface(double_refined_domain) - input_domain_surface) / input_domain_surface > 1e-3 )
  {
    std::cerr << "Error in check: Domain surfaces mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  //test writers:
  std::cout << "Writing domains..." << std::endl;
  viennagrid::io::vtk_writer<DomainT> my_vtk_writer;
  my_vtk_writer(domain, outfile + ".vtu");
  my_vtk_writer(refined_domain, outfile + "_refined.vtu");
  my_vtk_writer(double_refined_domain, outfile + "_double_refined.vtu");

  return;
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  std::string infile = path + "interconnect3d.mesh";
  std::string outfile = "out"; // without ending
  
  test<viennagrid::tetrahedral_3d_domain>(infile, outfile);
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
