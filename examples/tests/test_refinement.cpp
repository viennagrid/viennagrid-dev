/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
   This is a test file for the ViennaGrid Library IO-System
======================================================================= */

//***********************************************
// Define the dimension
//***********************************************
//#define THREEDIM
#define TWODIM
//#define ONEDIM

//***********************************************
// Define the input-file format
//***********************************************
//#define SGF
#define NETGEN
//#define VTK
//#define GTS

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennagrid/algorithm/refine.hpp"

struct TestDomainConfig
{
  typedef double                                  numeric_type;
  #ifdef THREEDIM
  typedef viennagrid::three_dimensions_tag        dimension_tag;
  typedef viennagrid::tetrahedron_tag             cell_tag;
  #endif
  #ifdef TWODIM
  typedef viennagrid::two_dimensions_tag          dimension_tag;
  typedef viennagrid::triangle_tag                cell_tag;
  #endif
  #ifdef ONEDIM
  typedef viennagrid::one_dimension_tag           dimension_tag;
  typedef viennagrid::line_tag                    cell_tag;
  #endif
};


void testNewDomain(std::string & infile, std::string & outfile)
{

  typedef viennagrid::domain<TestDomainConfig>        Domain;
  typedef TestDomainConfig::cell_tag                  CellTag;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::result_of::point_type<TestDomainConfig>::type          PointType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig,
                                            CellTag::topology_level>::type   CellType;
  //typedef viennagrid::DomainTypes<TestDomainConfig>::segment_type  Segment;

  typedef viennagrid::result_of::ncell_container<Domain, 0>::type   VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
      
  typedef viennagrid::result_of::ncell_container<Domain, 1>::type   EdgeContainer;
  typedef viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef viennagrid::result_of::ncell_container<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

  typedef viennagrid::result_of::ncell_container<Domain, CellTag::topology_level>::type     CellContainer;
  typedef viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;

  Domain domain;
  
  try{
    #ifdef NETGEN
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(domain, infile);
    #endif
    
    #ifdef GTS
    viennagrid::io::gts_reader my_gts_reader;
    my_gts_reader(domain, infile);
    #endif
    
    #ifdef VTK
    viennagrid::io::vtk_reader my_vtk_reader;
    my_vtk_reader.readDomain(domain, infile);
    #endif
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }

  Domain refined_domain;
  
  std::cout << "Refining domain..." << std::endl;
  refined_domain = viennagrid::refine(domain, viennagrid::uniform_refinement_tag());



  //test writers:
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, outfile + ".vtu");
  my_vtk_writer.writeDomain(refined_domain, outfile + "_refined.vtu");

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return;
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../examples/data/";
  
  #ifdef THREEDIM
  std::string infile = path + "sshape3d.mesh";
  #endif
  #ifdef TWODIM
  std::string infile = path + "sshape2d.mesh";
  #endif
  #ifdef ONEDIM
  std::string infile = path + "line8.mesh";
  #endif
  std::string outfile = "out"; // without ending
  
  testNewDomain(infile, outfile);
  return EXIT_SUCCESS;
}
