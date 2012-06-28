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

#include <cmath>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/algorithm/distance.hpp"

void fuzzy_check(double a, double b)
{
  if (a != b)
  {
    if (   (std::abs(a - b) / std::max( std::abs(a), std::abs(b) ) > 1e-10) 
        && (std::abs(a - b) > 1e-10)
    )
    {
      std::cerr << "FAILED!" << std::endl;
      std::cerr << "Result mismatch: " << a << " vs. " << b << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  std::cout << "PASSED! (" << a << ", " << b << ")" << std::endl;
}


//
// Triangular
//

void setup_domains(viennagrid::domain_t<viennagrid::config::triangular_2d> & d0,
                   viennagrid::domain_t<viennagrid::config::triangular_2d> & d1)
{
  typedef viennagrid::config::triangular_2d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
  // Domain d0
  {
    PointType p0(0.0, 0.0);
    PointType p1(1.0, 0.0);
    PointType p2(2.0, 0.0);
    
    PointType p3(0.0, 1.0);
    PointType p4(1.0, 1.0);
    PointType p5(2.0, 1.0);

    PointType p6(0.0, 2.0);
    PointType p7(1.0, 2.0);
    PointType p8(2.0, 2.0);

    PointType p9(0.0, 3.0);
    PointType p10(1.0, 3.0);
    PointType p11(2.0, 3.0);

    PointType p12(0.0, 4.0);
    PointType p13(1.0, 4.0);
    PointType p14(2.0, 4.0);
    
    //upgrade to vertex:
    VertexType v0(p0, 0);
    VertexType v1(p1, 1);
    VertexType v2(p2, 2);
    
    VertexType v3(p3, 3);
    VertexType v4(p4, 4);
    VertexType v5(p5, 5);

    VertexType v6(p6, 6);
    VertexType v7(p7, 7);
    VertexType v8(p8, 8);
    
    VertexType v9(p9, 9);
    VertexType v10(p10, 10);
    VertexType v11(p11, 11);

    VertexType v12(p12, 12);
    VertexType v13(p13, 13);
    VertexType v14(p14, 14);
    
    
    VertexType * vertices[15];
    
    std::cout << "Adding vertices to domain..." << std::endl;
    vertices[0] = d0.push_back(v0);
    vertices[1] = d0.push_back(v1);
    vertices[2] = d0.push_back(v2);
    
    vertices[3] = d0.push_back(v3);
    vertices[4] = d0.push_back(v4);
    vertices[5] = d0.push_back(v5);

    vertices[6] = d0.push_back(v6);
    vertices[7] = d0.push_back(v7);
    vertices[8] = d0.push_back(v8);

    vertices[9] = d0.push_back(v9);
    vertices[10] = d0.push_back(v10);
    vertices[11] = d0.push_back(v11);

    vertices[12] = d0.push_back(v12);
    vertices[13] = d0.push_back(v13);
    vertices[14] = d0.push_back(v14);

    
    std::cout << "Adding cells to domain..." << std::endl;
    CellType simplex;
    
    d0.segments().resize(2);
    
    //segment 0:
    
    vertices[0] = &(viennagrid::ncells<0>(d0)[0]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[1]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[3]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);
    
    vertices[0] = &(viennagrid::ncells<0>(d0)[1]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[4]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[3]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[1]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[2]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[4]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[2]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[5]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[4]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    
    vertices[0] = &(viennagrid::ncells<0>(d0)[3]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[4]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[6]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[4]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[7]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[6]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[4]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[5]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[7]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[5]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[8]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[7]);
    simplex.vertices(vertices);
    d0.segments()[0].push_back(simplex);

    // segment 1:
    
    vertices[0] = &(viennagrid::ncells<0>(d0)[9]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[10]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[12]);
    simplex.vertices(vertices);
    d0.segments()[1].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[10]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[13]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[12]);
    simplex.vertices(vertices);
    d0.segments()[1].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[10]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[11]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[13]);
    simplex.vertices(vertices);
    d0.segments()[1].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d0)[11]);
    vertices[1] = &(viennagrid::ncells<0>(d0)[14]);
    vertices[2] = &(viennagrid::ncells<0>(d0)[13]);
    simplex.vertices(vertices);
    d0.segments()[1].push_back(simplex);
  }
  
  
  // domain d1:
  {
    PointType p0(3.0, 0.0);
    PointType p1(4.0, 0.0);
    
    PointType p2(3.0, 1.0);
    PointType p3(4.0, 1.0);
    
    PointType p4(3.0, 2.0);
    PointType p5(4.0, 2.0);
    
    //upgrade to vertex:
    VertexType v0(p0, 0);
    VertexType v1(p1, 1);

    VertexType v2(p2, 2);
    VertexType v3(p3, 3);
    
    VertexType v4(p4, 4);
    VertexType v5(p5, 5);
    
    VertexType * vertices[6];
    
    std::cout << "Adding vertices to domain..." << std::endl;
    vertices[0] = d1.push_back(v0);
    vertices[1] = d1.push_back(v1);

    vertices[2] = d1.push_back(v2);
    vertices[3] = d1.push_back(v3);
    
    vertices[4] = d1.push_back(v4);
    vertices[5] = d1.push_back(v5);

    
    std::cout << "Adding cells to domain..." << std::endl;
    CellType simplex;
    
    d1.segments().resize(1);
    
    //segment 0:
    
    vertices[0] = &(viennagrid::ncells<0>(d1)[0]);
    vertices[1] = &(viennagrid::ncells<0>(d1)[1]);
    vertices[2] = &(viennagrid::ncells<0>(d1)[2]);
    simplex.vertices(vertices);
    d1.segments()[0].push_back(simplex);
    
    vertices[0] = &(viennagrid::ncells<0>(d1)[1]);
    vertices[1] = &(viennagrid::ncells<0>(d1)[3]);
    vertices[2] = &(viennagrid::ncells<0>(d1)[2]);
    simplex.vertices(vertices);
    d1.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d1)[2]);
    vertices[1] = &(viennagrid::ncells<0>(d1)[3]);
    vertices[2] = &(viennagrid::ncells<0>(d1)[4]);
    simplex.vertices(vertices);
    d1.segments()[0].push_back(simplex);

    vertices[0] = &(viennagrid::ncells<0>(d1)[3]);
    vertices[1] = &(viennagrid::ncells<0>(d1)[5]);
    vertices[2] = &(viennagrid::ncells<0>(d1)[4]);
    simplex.vertices(vertices);
    d1.segments()[0].push_back(simplex);

    
  }
  
}




void test(viennagrid::config::triangular_2d)
{
  typedef viennagrid::config::triangular_2d                             ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type               Domain;
  typedef ConfigType::cell_tag                                          CellTag;
  typedef viennagrid::segment_t<ConfigType>                             SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type                PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type             VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type             EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type  CellType;

  Domain domain0;
  Domain domain1;
  
  setup_domains(domain0, domain1);
  
  PointType A(-1.0, -1.0);
  

  CellType t0_d0 = viennagrid::ncells<CellTag::dim>(domain0)[0];
  CellType t8_d0 = viennagrid::ncells<CellTag::dim>(domain0)[8];
  CellType t1_d1 = viennagrid::ncells<CellTag::dim>(domain1)[1];
  
  //
  // Distance checks
  //

  // point to segment/domain 
  
  std::cout << "Distance of point A to segment0 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain0.segments()[0]), std::sqrt(2.0) );
  
  std::cout << "Distance of point A to segment1 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain0.segments()[1]), std::sqrt(17.0) );

  std::cout << "Distance of point A to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain1.segments()[0]),  std::sqrt(17.0) );
  
  std::cout << "Distance of point A to domain0... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain0),  std::sqrt(2.0) );

  std::cout << "Distance of point A to domain1... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain1),  std::sqrt(17.0) );

  // triangle to segment/domain
  
  std::cout << "Boundary distance of triangle 0 in domain0 to segment0 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t0_d0, domain0.segments()[0]),  0.0 );

  std::cout << "Boundary distance of triangle 0 in domain0 to segment1 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t0_d0, domain0.segments()[1]),  2.0 );

  std::cout << "Boundary distance of triangle 0 in domain0 to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(t0_d0, domain1.segments()[0]),  2.0 );

  std::cout << "Boundary distance of triangle 0 in domain0 to domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t0_d0, domain0),  0.0 );

  std::cout << "Boundary distance of triangle 0 in domain0 to domain1... ";
  fuzzy_check( viennagrid::boundary_distance(t0_d0, domain1),  2.0 );

  
  std::cout << "Boundary distance of triangle 8 in domain0 to segment0 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t8_d0, domain0.segments()[0]),  1.0 );

  std::cout << "Boundary distance of triangle 8 in domain0 to segment1 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t8_d0, domain0.segments()[1]),  0.0 );
  
  std::cout << "Boundary distance of triangle 8 in domain0 to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(t8_d0, domain1.segments()[0]),  std::sqrt(5.0) );

  std::cout << "Boundary distance of triangle 8 in domain0 to domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t8_d0, domain0),  0.0 );

  std::cout << "Boundary distance of triangle 8 in domain0 to domain1... ";
  fuzzy_check( viennagrid::boundary_distance(t8_d0, domain1),  std::sqrt(5.0) );
  

  std::cout << "Boundary distance of triangle 1 in domain1 to segment0 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t1_d1, domain0.segments()[0]),  1.0 );

  std::cout << "Boundary distance of triangle 1 in domain1 to segment1 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t1_d1, domain0.segments()[1]),  std::sqrt(5.0) );
  
  std::cout << "Boundary distance of triangle 1 in domain1 to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(t1_d1, domain1.segments()[0]),  0.0 );

  std::cout << "Boundary distance of triangle 1 in domain1 to domain0... ";
  fuzzy_check( viennagrid::boundary_distance(t1_d1, domain0),  1.0 );

  std::cout << "Boundary distance of triangle 1 in domain1 to domain1... ";
  fuzzy_check( viennagrid::boundary_distance(t1_d1, domain1),  0.0 );
  
  
  // Distances between domains and segments
  
  std::cout << "Boundary distance of segment0 to segment1 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(domain0.segments()[0], domain0.segments()[1]),  1.0 );

  std::cout << "Boundary distance of segment1 in domain0 to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(domain0.segments()[1], domain1.segments()[0]),  std::sqrt(2.0) );

  std::cout << "Boundary distance of domain0 to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(domain0, domain1.segments()[0]),  1.0 );
  
  std::cout << "Boundary distance of domain0 to domain1... ";
  fuzzy_check( viennagrid::boundary_distance(domain0, domain1),  1.0 );
  
}




int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test(viennagrid::config::triangular_2d());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
