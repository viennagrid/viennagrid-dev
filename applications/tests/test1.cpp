/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
   
   This is a test file for the ViennaMesh Library IO-System
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
//#define SGF
//#define VTK
#define GTS

#include "viennamesh/domain.hpp"
#include "viennamesh/segment.hpp"
#include "viennamesh/boundary.hpp"
#include "viennamesh/io/sgf_reader.hpp"
#include "viennamesh/io/sgf_writer.hpp"
#include "viennamesh/io/vtk_reader.hpp"
#include "viennamesh/io/vtk_writer.hpp"
#include "viennamesh/io/opendx_writer.hpp"
#include "viennamesh/io/gts_reader.hpp"
#include "viennamesh/io/gts_writer.hpp"

using namespace std;
using namespace viennamesh;

struct NewDomainTesting
{
  typedef double                                              CoordType;
  #ifdef THREEDIM
  typedef ThreeDimensionsTag                                  DimensionTag;
  typedef TetrahedronTag                                       CellTag;
  #endif
  #ifdef TWODIM
  typedef TwoDimensionsTag                                    DimensionTag;
  typedef TriangleTag                                         CellTag;
  #endif
  #ifdef ONEDIM
  typedef OneDimensionTag                                     DimensionTag;
  typedef LineTag                                             CellTag;
  #endif

  //multigrid:
  //typedef FullMultigridTag                                  MultigridTag;
  typedef NoMultigridTag                                      MultigridTag;


  /**************************************************************/
  /***********   handler for topological elements: **************/

  //Read boundary from file?
  //yes: use BoundaryKeyType<id>
  //no:  use NoBoundaryRead
  //typedef BoundaryKeyType<11>                                  BoundaryReadTag;
  typedef NoBoundaryRead                                  BoundaryReadTag;
};

  //template <typename DomainConfiguration>
//   struct Laplace_Config 
//   {
//     typedef ScalarTag                         ResultDimension;
//     typedef TypeErasureTag                       BaseFunTreatmentTag;
//     typedef NoBoundaryMappingTag              MappingTag;
// 
//     typedef QuadraticBasisfunctionTag            BasisfunctionTag;
// 
//     typedef MappingKeyType<0>                 MappingKey;
//     typedef BoundaryKeyType<0>                BoundaryKey;
// 
//     typedef BoundaryKeyType<11>                BoundaryData;
//     typedef BoundaryKeyType<12>                BoundaryData2;
//     typedef BoundaryKeyType<13>                BoundaryData3;
//   };


void testNewDomain(std::string & infile, std::string & outfile)
{
  //typedef Laplace_Config                              FEMConfig;

  typedef domain<NewDomainTesting>                    Domain;
  typedef NewDomainTesting::DimensionTag              DimensionTag;
  typedef DomainTypes<NewDomainTesting>::PointType    PointType;
  typedef DomainTypes<NewDomainTesting>::VertexType   VertexType;

  typedef DomainTypes<NewDomainTesting>::FacetType    FacetType;
  typedef DomainTypes<NewDomainTesting>::CellType     CellType;
  typedef DomainTypes<NewDomainTesting>::SegmentType  Segment;

  typedef IteratorTypes<Segment, 0>::ResultType       VertexIterator;
  typedef IteratorTypes<Segment, 1>::ResultType       EdgeIterator;
  typedef IteratorTypes<Segment, NewDomainTesting::CellTag::TopoLevel-1>::ResultType       FacetIterator;
  typedef IteratorTypes<Segment, NewDomainTesting::CellTag::TopoLevel>::ResultType       CellIterator;

  Domain domain;
  
// #ifdef THREEDIM
//   PointType p0(0.0, 0.0, 0.0);
//   PointType p1(1.0, 0.0, 0.0);
//   PointType p2(0.0, 1.0, 0.0);
//   PointType p3(0.0, 0.0, 1.0);
// #else
//   PointType p0(0.0, 0.0);
//   PointType p1(1.0, 0.0);
//   PointType p2(0.0, 1.0);
//   PointType p3(1.0, 1.0);
// #endif
//   
// 
   //domain.addSegment();
   Segment & seg = domain.add();
// 
//   
//   //upgrade to vertex:
//   VertexType v0(p0, 0);
//   VertexType v1(p1, 1);
//   VertexType v2(p2, 2);
//   VertexType v3(p3, 3);
// 
//   seg.reserveVertices(4);
//   std::cout << "Adding vertices to segment:" << std::endl;
//   seg.add<0>(0, v0);
//   seg.add<0>(1, v1);
//   seg.add<0>(2, v2);
//   seg.add<0>(3, v3);
// 
//   seg.reserveCells(2);
// 
//   CellType simplex;
//   
//   VertexType * vertices[4];
// 
//   vertices[0] = seg.getVertexAddress(0);
//   vertices[1] = seg.getVertexAddress(1);
//   vertices[2] = seg.getVertexAddress(2);
//   vertices[3] = seg.getVertexAddress(3);
//   simplex.setVertices(vertices);
//   seg.add<NewDomainTesting::CellTag::TopoLevel>(0, simplex);
// 
//   vertices[0] = seg.getVertexAddress(1);
//   vertices[1] = seg.getVertexAddress(3);
//   vertices[2] = seg.getVertexAddress(2);
//   vertices[3] = seg.getVertexAddress(0);
//   simplex.setVertices(vertices);
//   seg.add<NewDomainTesting::CellTag::TopoLevel>(1, simplex);
//   
//   
//   std::cout << "Vertices: " << std::endl;
//   for (VertexIterator vit = seg.begin<0>();
//         vit != seg.end<0>();
//         ++vit)
//       vit->print();
// 
//   std::cout << "Edges: " << std::endl;
//   for (EdgeIterator eit = seg.begin<1>();
//         eit != seg.end<1>();
//         ++eit)
//       eit->print();
// 
//   std::cout << "Facets: " << std::endl;
//   for (FacetIterator fit = seg.begin<NewDomainTesting::CellTag::TopoLevel-1>();
//         fit != seg.end<NewDomainTesting::CellTag::TopoLevel-1>();
//         ++fit)
//       fit->print();
// 
//   std::cout << "Cells: " << std::endl;
//   for (CellIterator cit = seg.begin<NewDomainTesting::CellTag::TopoLevel>();
//         cit != seg.end<NewDomainTesting::CellTag::TopoLevel>();
//         ++cit)
//       cit->print();
// 
// 
//   std::cout << "Vertices: " << std::endl;
//   for (VertexIterator vit = seg.begin<0>();
//         vit != seg.end<0>();
//         ++vit)
//       vit->print();
// 
//   std::cout << "Edges: " << std::endl;
//   for (EdgeIterator eit = seg.begin<1>();
//         eit != seg.end<1>();
//         ++eit)
//       eit->print();
// 
//   std::cout << "Facets: " << std::endl;
//   for (FacetIterator fit = seg.begin<NewDomainTesting::CellTag::TopoLevel-1>();
//         fit != seg.end<NewDomainTesting::CellTag::TopoLevel-1>();
//         ++fit)
//       fit->print();
// 
//   std::cout << "Cells: " << std::endl;
//   for (CellIterator cit = seg.begin<NewDomainTesting::CellTag::TopoLevel>();
//         cit != seg.end<NewDomainTesting::CellTag::TopoLevel>();
//         ++cit)
//       cit->print();
  
  
  try{
    #ifdef SGF
    sgf_reader my_sgf_reader;
    my_sgf_reader(domain, infile);
    #endif
    
    #ifdef GTS
    gts_reader my_gts_reader;
    my_gts_reader(domain, infile);
    #endif
    
    #ifdef VTK
    Vtk_reader my_vtk_reader;
    my_vtk_reader.readDomain(domain, infile);
    #endif
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }

  detectBoundary(seg);
  
  //test writers:
  sgf_writer my_sgf_writer;
  my_sgf_writer(domain, outfile + ".sgf");
   
  Vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, outfile + ".vtk");
   
  gts_writer my_gts_writer;
  my_gts_writer(domain, outfile);
  
  opendx_writer my_dx_writer;
  my_dx_writer(domain, outfile + ".odx");
  
//  profiler_class<mysql_writer, std_timer> my_profiler;
//  profiler_class<oracle_writer, bad_timer> my_profiler;
//  profiler_class<xml_writer, high_precision_timer> my_profiler;

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  string path = "../applications/data/";
  
  string infile = path + "in1d.gts";
  string outfile = path + "out"; // without ending
  
  testNewDomain(infile, outfile);
  return EXIT_SUCCESS;
}
