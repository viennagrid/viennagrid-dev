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
//#define TWODIM
#define ONEDIM

//***********************************************
// Define the input-file format
//***********************************************
#define SGF
//#define VTK
//#define GTS

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/boundary.hpp"
#include "viennagrid/io/sgf_reader.hpp"
#include "viennagrid/io/sgf_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
//#include "viennagrid/io/gts_reader.hpp"
//#include "viennagrid/io/gts_writer.hpp"

struct TestDomainConfig
{
  typedef double                                 numeric_type;
  #ifdef THREEDIM
  typedef viennagrid::ThreeDimensionsTag         dimension_tag;
  typedef viennagrid::TetrahedronTag             cell_tag;
  #endif
  #ifdef TWODIM
  typedef viennagrid::TwoDimensionsTag           dimension_tag;
  typedef viennagrid::TriangleTag                cell_tag;
  #endif
  #ifdef ONEDIM
  typedef viennagrid::OneDimensionTag            dimension_tag;
  typedef viennagrid::LineTag                    cell_tag;
  #endif

  //multigrid:
  //typedef FullMultigridTag                       multigrid_tag;
  typedef viennagrid::NoMultigridTag             multigrid_tag;
};


void testNewDomain(std::string & infile, std::string & outfile)
{

  typedef viennagrid::domain<TestDomainConfig>        Domain;
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::DomainTypes<TestDomainConfig>::point_type    PointType;
  typedef viennagrid::DomainTypes<TestDomainConfig>::vertex_type   VertexType;

  typedef viennagrid::DomainTypes<TestDomainConfig>::facet_type    FacetType;
  typedef viennagrid::DomainTypes<TestDomainConfig>::cell_type     CellType;
  typedef viennagrid::DomainTypes<TestDomainConfig>::segment_type  Segment;

  typedef viennagrid::IteratorTypes<Segment, 0>::ResultType                                         VertexIterator;
  typedef viennagrid::IteratorTypes<Segment, 1>::ResultType                                         EdgeIterator;
  typedef viennagrid::IteratorTypes<Segment, TestDomainConfig::cell_tag::topology_level-1>::ResultType    FacetIterator;
  typedef viennagrid::IteratorTypes<Segment, TestDomainConfig::cell_tag::topology_level>::ResultType      CellIterator;

  Domain domain;
  
   Segment & seg = domain.add();
  
  try{
    #ifdef SGF
    viennagrid::io::sgf_reader my_sgf_reader;
    my_sgf_reader(domain, infile);
    #endif
    
    #ifdef GTS
    viennagrid::io::gts_reader my_gts_reader;
    my_gts_reader(domain, infile);
    #endif
    
    #ifdef VTK
    viennagrid::io::Vtk_reader my_vtk_reader;
    my_vtk_reader.readDomain(domain, infile);
    #endif
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }

  viennagrid::detectBoundary(seg);
  
  //test writers:
  viennagrid::io::sgf_writer my_sgf_writer;
  my_sgf_writer(domain, outfile + ".sgf");
   
  viennagrid::io::Vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, outfile + ".vtk");
   
  //gts_writer my_gts_writer;
  //my_gts_writer(domain, outfile);
  
  viennagrid::io::opendx_writer my_dx_writer;
  my_dx_writer(domain, outfile + ".odx");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return;
  
  //compilation tests:
  //viennagrid::io::gts_reader my_gts_reader;
  //my_gts_reader(domain, infile);

  //viennagrid::io::sgf_reader my_sgf_reader;
  //my_sgf_reader(domain, infile);

  //viennagrid::io::Vtk_reader<Domain> my_vtk_reader;
  //my_vtk_reader.readDomain(domain, infile);
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../applications/data/";
  
  std::string infile = path + "line8.sgf";
  std::string outfile = path + "out"; // without ending
  
  testNewDomain(infile, outfile);
  return EXIT_SUCCESS;
}
