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

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/point.hpp"
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

void setup_domains(viennagrid::triangular_2d_domain & domain0,
                   viennagrid::triangular_2d_segmentation & segmentation0,
                   viennagrid::triangular_2d_domain & domain1,
                   viennagrid::triangular_2d_segmentation & segmentation1)
{
  typedef viennagrid::triangular_2d_domain                      DomainType;
  typedef viennagrid::triangular_2d_segment                     SegmentType;
  typedef viennagrid::triangle_tag                                      CellTag;
  
  typedef viennagrid::result_of::point<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::handle<DomainType, viennagrid::vertex_tag>::type       VertexHandleType;
  
  typedef viennagrid::result_of::element<DomainType, CellTag>::type        CellType;
  
  // Domain d0
  {
    const size_t s = 15;
    PointType p[s];
    VertexHandleType v[s];
      
    p[0] = PointType(0.0, 0.0);
    p[1] = PointType(1.0, 0.0);
    p[2] = PointType(2.0, 0.0);
    
    p[3] = PointType(0.0, 1.0);
    p[4] = PointType(1.0, 1.0);
    p[5] = PointType(2.0, 1.0);

    p[6] = PointType(0.0, 2.0);
    p[7] = PointType(1.0, 2.0);
    p[8] = PointType(2.0, 2.0);

    p[9] = PointType(0.0, 3.0);
    p[10] = PointType(1.0, 3.0);
    p[11] = PointType(2.0, 3.0);

    p[12] = PointType(0.0, 4.0);
    p[13] = PointType(1.0, 4.0);
    p[14] = PointType(2.0, 4.0);
    
    //upgrade to vertex:
    std::cout << "Adding vertices to domain..." << std::endl;
    for (size_t i = 0; i < s; ++i)
    {
        v[i] = viennagrid::make_vertex( domain0, p[i] );
//         viennagrid::point( domain0, v[i] ) = p[i];
    }

    
    std::cout << "Adding cells to domain..." << std::endl;
    VertexHandleType vertices[3];

    
    SegmentType seg0 = segmentation0.make_segment();
    SegmentType seg1 = segmentation0.make_segment();
    
    //segment 0:
    vertices[0] = v[0];
    vertices[1] = v[1];
    vertices[2] = v[3];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );
    
    vertices[0] = v[1];
    vertices[1] = v[4];
    vertices[2] = v[3];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[1];
    vertices[1] = v[2];
    vertices[2] = v[4];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[2];
    vertices[1] = v[5];
    vertices[2] = v[4];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[3];
    vertices[1] = v[4];
    vertices[2] = v[6];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[4];
    vertices[1] = v[7];
    vertices[2] = v[6];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[4];
    vertices[1] = v[5];
    vertices[2] = v[7];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[5];
    vertices[1] = v[8];
    vertices[2] = v[7];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    // segment 1:
    
    vertices[0] = v[9];
    vertices[1] = v[10];
    vertices[2] = v[12];
    viennagrid::make_element<CellType>( seg1, vertices, vertices+3 );

    vertices[0] = v[10];
    vertices[1] = v[13];
    vertices[2] = v[12];
    viennagrid::make_element<CellType>( seg1, vertices, vertices+3 );

    vertices[0] = v[10];
    vertices[1] = v[11];
    vertices[2] = v[13];
    viennagrid::make_element<CellType>( seg1, vertices, vertices+3 );

    vertices[0] = v[11];
    vertices[1] = v[14];
    vertices[2] = v[13];
    viennagrid::make_element<CellType>( seg1, vertices, vertices+3 );
  }
  
  
  // domain d1:
  {
    const size_t s = 6;
    PointType p[s];
    VertexHandleType v[s];
      
    p[0] = PointType(3.0, 0.0);
    p[1] = PointType(4.0, 0.0);
    
    p[2] = PointType(3.0, 1.0);
    p[3] = PointType(4.0, 1.0);
    
    p[4] = PointType(3.0, 2.0);
    p[5] = PointType(4.0, 2.0);
    
    //upgrade to vertex:
    std::cout << "Adding vertices to domain..." << std::endl;
    for (size_t i = 0; i < s; ++i)
    {
        v[i] = viennagrid::make_vertex( domain1, p[i] );
//         viennagrid::point( domain1, v[i] ) = p[i];
    }

    
    std::cout << "Adding cells to domain..." << std::endl;
    VertexHandleType vertices[3];
    
    SegmentType seg0 = segmentation1.make_segment();
    
    //segment 0:
    
    vertices[0] = v[0];
    vertices[1] = v[1];
    vertices[2] = v[2];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );
    
    vertices[0] = v[1];
    vertices[1] = v[3];
    vertices[2] = v[2];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[2];
    vertices[1] = v[3];
    vertices[2] = v[4];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );

    vertices[0] = v[3];
    vertices[1] = v[5];
    vertices[2] = v[4];
    viennagrid::make_element<CellType>( seg0, vertices, vertices+3 );  
  }
  
}




void test(viennagrid::triangular_2d_domain)
{
  typedef viennagrid::triangular_2d_domain                      Domain;
  typedef viennagrid::triangular_2d_segmentation                Segmentation;
  typedef viennagrid::triangular_2d_segment                     Segment;
  typedef viennagrid::triangle_tag                                      CellTag;
  
  typedef viennagrid::result_of::point<Domain>::type                PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Domain, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type  CellType;

  Domain domain0;
  Segmentation segmentation0(domain0);
  Domain domain1;
  Segmentation segmentation1(domain1);
  
  setup_domains(domain0, segmentation0, domain1, segmentation1);
  
  PointType A(-1.0, -1.0);
  

  /*CellType & t0_d0 =*/ viennagrid::elements<CellTag>(domain0)[0];
  /*CellType & t8_d0 =*/ viennagrid::elements<CellTag>(domain0)[8];
  /*CellType & t1_d1 =*/ viennagrid::elements<CellTag>(domain1)[1];
  
  //
  // Distance checks
  //

  // point to segment/domain 
  
  std::cout << "Distance of point A to segment0 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(A, segmentation0(0)), std::sqrt(2.0) );
  
  std::cout << "Distance of point A to segment1 in domain0... ";
  fuzzy_check( viennagrid::boundary_distance(A, segmentation0(1)), std::sqrt(17.0) );

  std::cout << "Distance of point A to segment0 in domain1... ";
  fuzzy_check( viennagrid::boundary_distance(A, segmentation1(0)),  std::sqrt(17.0) );
  
  std::cout << "Distance of point A to domain0... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain0),  std::sqrt(2.0) );

  std::cout << "Distance of point A to domain1... ";
  fuzzy_check( viennagrid::boundary_distance(A, domain1),  std::sqrt(17.0) );

  // triangle to segment/domain
  
//   std::cout << "Boundary distance of triangle 0 in domain0 to segment0 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t0_d0, seg0),  0.0 );
// 
//   std::cout << "Boundary distance of triangle 0 in domain0 to segment1 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t0_d0, seg1),  2.0 );
// 
//   std::cout << "Boundary distance of triangle 0 in domain0 to segment0 in domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain1, t0_d0, seg0),  2.0 );
// 
//   std::cout << "Boundary distance of triangle 0 in domain0 to domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t0_d0, domain0),  0.0 );
// 
//   std::cout << "Boundary distance of triangle 0 in domain0 to domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain1, t0_d0, domain1),  2.0 );
// 
//   
//   std::cout << "Boundary distance of triangle 8 in domain0 to segment0 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t8_d0, seg0),  1.0 );
// 
//   std::cout << "Boundary distance of triangle 8 in domain0 to segment1 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t8_d0, seg1),  0.0 );
//   
//   std::cout << "Boundary distance of triangle 8 in domain0 to segment0 in domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain1, t8_d0, seg0),  std::sqrt(5.0) );
// 
//   std::cout << "Boundary distance of triangle 8 in domain0 to domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t8_d0, domain0),  0.0 );
// 
//   std::cout << "Boundary distance of triangle 8 in domain0 to domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain1, t8_d0, domain1),  std::sqrt(5.0) );
//   
// 
//   std::cout << "Boundary distance of triangle 1 in domain1 to segment0 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t1_d1, seg0),  1.0 );
// 
//   std::cout << "Boundary distance of triangle 1 in domain1 to segment1 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t1_d1, seg1),  std::sqrt(5.0) );
//   
//   std::cout << "Boundary distance of triangle 1 in domain1 to segment0 in domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain1, t1_d1, seg0),  0.0 );
// 
//   std::cout << "Boundary distance of triangle 1 in domain1 to domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, t1_d1, domain0),  1.0 );
// 
//   std::cout << "Boundary distance of triangle 1 in domain1 to domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain1, t1_d1, domain1),  0.0 );
  
  
  // Distances between domains and segments
  
//   std::cout << "Boundary distance of segment0 to segment1 in domain0... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, seg0, seg1),  1.0 );
// 
//   std::cout << "Boundary distance of segment1 in domain0 to segment0 in domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(seg1, seg0),  std::sqrt(2.0) );
// 
//   std::cout << "Boundary distance of domain0 to segment0 in domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, seg0),  1.0 );
//   
//   std::cout << "Boundary distance of domain0 to domain1... ";
//   fuzzy_check( viennagrid::boundary_distance(domain0, domain1),  1.0 );
  
}




int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test(viennagrid::triangular_2d_domain());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
