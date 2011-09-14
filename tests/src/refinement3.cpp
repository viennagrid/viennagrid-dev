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

template <typename ConfigType>
void test(std::string & infile, std::string & outfile)
{
  typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef typename ConfigType::cell_tag           CellTag;
  typedef viennagrid::segment_t<ConfigType>       SegmentType;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 1>::type       EdgeType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::dim>::type   CellType;


  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type           VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
      
  typedef typename viennagrid::result_of::ncell_range<Domain, 1>::type           EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::dim-1>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::dim>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;

  Domain domain;
  
  try{
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(domain, infile);
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }


  double input_domain_volume = viennagrid::volume(domain);
  double input_domain_surface = domain_surface(domain);
  
  std::cout << "Volume of input domain: " << input_domain_volume << std::endl;
  std::cout << "Surface of input domain: " << input_domain_surface << std::endl;

  //Testing: Tag some cells with centroid at x \in [2,3]:
  CellContainer cells = viennagrid::ncells<CellTag::dim>(domain);
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
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*cit) = true;
    }
  }

  Domain refined_domain;
  refined_domain = viennagrid::refine(domain, viennagrid::local_refinement_tag());

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
  CellContainer cells_refined = viennagrid::ncells<CellTag::dim>(refined_domain);
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
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*cit) = true;
    }
  }

  Domain double_refined_domain;
  double_refined_domain = viennagrid::refine(refined_domain, viennagrid::local_refinement_tag());

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
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
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
  
  test<viennagrid::config::tetrahedral_3d>(infile, outfile);
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
