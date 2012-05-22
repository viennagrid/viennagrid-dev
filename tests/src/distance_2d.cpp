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
// Line 2d
//

void setup_domain(viennagrid::domain_t<viennagrid::config::line_2d> & d)
{
  typedef viennagrid::config::line_2d                             ConfigType;
  typedef ConfigType::cell_tag                                    CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim>::type        CellType;
  
  PointType p0(3.0, 0.0);
  PointType p1(1.0, 1.0);
  PointType p2(5.0, 1.0);
  PointType p3(3.0, 2.0);
  PointType p4(0.0, 3.0);
  PointType p5(3.0, 3.0);
  PointType p6(1.0, 4.0);
  PointType p7(2.0, 1.0);
  PointType p8(3.0, 2.0);

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

  VertexType * vertices[9];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = d.push_back(v0);
  vertices[1] = d.push_back(v1);
  vertices[2] = d.push_back(v2);
  vertices[3] = d.push_back(v3);
  vertices[4] = d.push_back(v4);
  vertices[5] = d.push_back(v5);
  vertices[6] = d.push_back(v6);
  vertices[7] = d.push_back(v7);
  vertices[8] = d.push_back(v8);

  std::cout << "Adding cells to domain..." << std::endl;
  CellType line;
  
  vertices[0] = &(viennagrid::ncells<0>(d)[0]);
  vertices[1] = &(viennagrid::ncells<0>(d)[6]);
  line.vertices(vertices);
  d.push_back(line);

  vertices[0] = &(viennagrid::ncells<0>(d)[1]);
  vertices[1] = &(viennagrid::ncells<0>(d)[4]);
  line.vertices(vertices);
  d.push_back(line);

  vertices[0] = &(viennagrid::ncells<0>(d)[1]);
  vertices[1] = &(viennagrid::ncells<0>(d)[5]);
  line.vertices(vertices);
  d.push_back(line);

  vertices[0] = &(viennagrid::ncells<0>(d)[2]);
  vertices[1] = &(viennagrid::ncells<0>(d)[3]);
  line.vertices(vertices);
  d.push_back(line);
  
  vertices[0] = &(viennagrid::ncells<0>(d)[7]);
  vertices[1] = &(viennagrid::ncells<0>(d)[8]);
  line.vertices(vertices);
  d.push_back(line);
  
}

void test(viennagrid::config::line_2d)
{
  typedef viennagrid::config::line_2d                             ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type               Domain;
  typedef ConfigType::cell_tag                                          CellTag;
  typedef viennagrid::segment_t<ConfigType>                             SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type                PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type             VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type             EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type  CellType;

  Domain domain;
  
  setup_domain(domain);
  
  PointType A(0, 0);
  PointType B(1, 0);
  PointType C(2, 0);
  PointType D(3, 0);
  PointType E(4, 0);
  
  PointType F(0, 1);
  PointType G(1, 1);
  PointType H(3, 1);
  PointType I(4, 1);
  
  PointType J(0, 2);
  PointType K(2, 2);
  PointType L(5, 2);

  PointType M(5, 3);
  
  PointType N(0, 4);
  PointType O(1, 4);
  PointType P(4, 4);

  PointType Q(2, 5);
  PointType R(3, 5);

  CellType line0 = viennagrid::ncells<CellTag::dim>(domain)[0];
  CellType line1 = viennagrid::ncells<CellTag::dim>(domain)[1];
  CellType line2 = viennagrid::ncells<CellTag::dim>(domain)[2];
  CellType line3 = viennagrid::ncells<CellTag::dim>(domain)[3];
  CellType line  = viennagrid::ncells<CellTag::dim>(domain)[4];
  
  //
  // Distance checks for points to line
  //
  
  std::cout << "Distance of point A to line... ";
  fuzzy_check( viennagrid::distance(A, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point B to line... ";
  fuzzy_check( viennagrid::distance(B, line), std::sqrt(2.0) );

  std::cout << "Distance of point C to line... ";
  fuzzy_check( viennagrid::distance(C, line), 1.0 );
  
  std::cout << "Distance of point D to line... ";
  fuzzy_check( viennagrid::distance(D, line), std::sqrt(2.0) );
  
  std::cout << "Distance of point E to line... ";
  fuzzy_check( viennagrid::distance(E, line), 3.0 * std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point F to line... ";
  fuzzy_check( viennagrid::distance(F, line), 2.0 );
  
  std::cout << "Distance of point G to line... ";
  fuzzy_check( viennagrid::distance(G, line), 1.0 );
  
  std::cout << "Distance of point H to line... ";
  fuzzy_check( viennagrid::distance(H, line), std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point I to line... ";
  fuzzy_check( viennagrid::distance(I, line), std::sqrt(2.0) );
  
  std::cout << "Distance of point J to line... ";
  fuzzy_check( viennagrid::distance(J, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point K to line... ";
  fuzzy_check( viennagrid::distance(K, line), std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point L to line... ";
  fuzzy_check( viennagrid::distance(L, line), 2.0 );
  
  std::cout << "Distance of point M to line... ";
  fuzzy_check( viennagrid::distance(M, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point N to line... ";
  fuzzy_check( viennagrid::distance(N, line), 5.0 * std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point O to line... ";
  fuzzy_check( viennagrid::distance(O, line), 2.0 * std::sqrt(2.0) );
  
  std::cout << "Distance of point P to line... ";
  fuzzy_check( viennagrid::distance(P, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point Q to line... ";
  fuzzy_check( viennagrid::distance(Q, line), std::sqrt(10.0) );
  
  std::cout << "Distance of point R to line... ";
  fuzzy_check( viennagrid::distance(R, line), 3.0 );
  
  
  //
  // Distance checks line to line
  //
  
  std::cout << "Distance of line0 to line0... ";
  fuzzy_check( viennagrid::distance(line0, line0), 0.0 );

  std::cout << "Distance of line0 to line1... ";
  double s01 = ( 3.0 + std::sqrt(5.0) + std::sqrt(20.0) ) / 2.0;
  double dist01 = std::sqrt( s01 * (s01 - 3.0) * (s01 - std::sqrt(5.0)) * (s01 - std::sqrt(20.0)) ) * 2.0 / std::sqrt(20.0);
  fuzzy_check( viennagrid::distance(line0, line1), dist01 );

  std::cout << "Distance of line0 to line2... ";
  fuzzy_check( viennagrid::distance(line0, line2), 0.0 );

  std::cout << "Distance of line0 to line3... ";
  double s03 = ( 2.0 + std::sqrt(8.0) + std::sqrt(20.0) ) / 2.0;
  double dist03 = std::sqrt( s03 * (s03 - 2.0) * (s03 - std::sqrt(8.0)) * (s03 - std::sqrt(20.0)) ) * 2.0 / std::sqrt(20.0);
  fuzzy_check( viennagrid::distance(line0, line3), dist03 );
               
  
  

  std::cout << "Distance of line1 to line0... ";
  fuzzy_check( viennagrid::distance(line1, line0), dist01 );

  std::cout << "Distance of line1 to line1... ";
  fuzzy_check( viennagrid::distance(line1, line1), 0.0 );

  std::cout << "Distance of line1 to line2... ";
  fuzzy_check( viennagrid::distance(line1, line2), 0.0 );

  std::cout << "Distance of line1 to line3... ";
  fuzzy_check( viennagrid::distance(line1, line3), std::sqrt(5.0) );
  

  
  std::cout << "Distance of line2 to line0... ";
  fuzzy_check( viennagrid::distance(line2, line0), 0.0 );

  std::cout << "Distance of line2 to line1... ";
  fuzzy_check( viennagrid::distance(line2, line1), 0.0 );

  std::cout << "Distance of line2 to line2... ";
  fuzzy_check( viennagrid::distance(line2, line2), 0.0 );

  std::cout << "Distance of line2 to line3... ";
  fuzzy_check( viennagrid::distance(line2, line3), std::sqrt(2.0) / 2.0 );
  
  

  std::cout << "Distance of line3 to line0... ";
  fuzzy_check( viennagrid::distance(line3, line0), dist03 );

  std::cout << "Distance of line3 to line1... ";
  fuzzy_check( viennagrid::distance(line3, line1), std::sqrt(5.0) );

  std::cout << "Distance of line3 to line2... ";
  fuzzy_check( viennagrid::distance(line3, line2), std::sqrt(2.0) / 2.0 );

  std::cout << "Distance of line3 to line3... ";
  fuzzy_check( viennagrid::distance(line3, line3), 0.0 );
  
  
}





//
// Triangular
//

void setup_domain(viennagrid::domain_t<viennagrid::config::triangular_2d> & d)
{
  typedef viennagrid::config::triangular_2d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
  
  PointType p0(2.0, 1.0);
  PointType p1(3.0, 2.0);
  PointType p2(3.0, 3.0);
  PointType p3(1.0, 2.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  VertexType * vertices[4];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = d.push_back(v0);
  vertices[1] = d.push_back(v1);
  vertices[2] = d.push_back(v2);
  vertices[3] = d.push_back(v3);

  std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  
  vertices[0] = &(viennagrid::ncells<0>(d)[0]);
  vertices[1] = &(viennagrid::ncells<0>(d)[2]);
  vertices[2] = &(viennagrid::ncells<0>(d)[3]);
  simplex.vertices(vertices);
  d.push_back(simplex);
  
  vertices[0] = &(viennagrid::ncells<0>(d)[0]);
  vertices[1] = &(viennagrid::ncells<0>(d)[1]);
  vertices[2] = &(viennagrid::ncells<0>(d)[2]);
  simplex.vertices(vertices);
  d.push_back(simplex);
  
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

  Domain domain;
  
  setup_domain(domain);
  
  PointType A(0, 0);
  PointType B(1, 0);
  PointType C(2, 0);
  PointType D(3, 0);
  PointType E(4, 0);
  
  PointType F(0, 1);
  PointType G(1, 1);
  PointType H(3, 1);
  PointType I(4, 1);
  
  PointType J(0, 2);
  PointType K(2, 2);
  PointType L(5, 2);

  PointType M(5, 3);
  
  PointType N(0, 4);
  PointType O(1, 4);
  PointType P(4, 4);

  PointType Q(2, 5);
  PointType R(3, 5);

  CellType t0 = viennagrid::ncells<CellTag::dim>(domain)[0];
  CellType t1 = viennagrid::ncells<CellTag::dim>(domain)[1];
  
  //
  // Distance checks for t0
  //
  
  std::cout << "Distance of point A to triangle t0... ";
  fuzzy_check( viennagrid::distance(A, t0), 3.0 * std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point B to triangle t0... ";
  fuzzy_check( viennagrid::distance(B, t0), std::sqrt(2.0) );

  std::cout << "Distance of point C to triangle t0... ";
  fuzzy_check( viennagrid::distance(C, t0), 1.0 );
  
  std::cout << "Distance of point D to triangle t0... ";
  fuzzy_check( viennagrid::distance(D, t0), std::sqrt(2.0) );
  
  std::cout << "Distance of point E to triangle t0... ";
  fuzzy_check( viennagrid::distance(E, t0), std::sqrt(5.0) );
  
  std::cout << "Distance of point F to triangle t0... ";
  fuzzy_check( viennagrid::distance(F, t0), std::sqrt(2.0) );
  
  std::cout << "Distance of point G to triangle t0... ";
  fuzzy_check( viennagrid::distance(G, t0), std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point H to triangle t0... ";
  fuzzy_check( viennagrid::distance(H, t0), 2.0 / std::sqrt(5.0) );
  
  std::cout << "Distance of point I to triangle t0... ";
  fuzzy_check( viennagrid::distance(I, t0), 4.0 / std::sqrt(5.0) );
  
  std::cout << "Distance of point J to triangle t0... ";
  fuzzy_check( viennagrid::distance(J, t0), 1.0 );
  
  std::cout << "Distance of point K to triangle t0... ";
  fuzzy_check( viennagrid::distance(K, t0), 0.0 );
  
  std::cout << "Distance of point L to triangle t0... ";
  fuzzy_check( viennagrid::distance(L, t0), std::sqrt(5.0) );
  
  std::cout << "Distance of point M to triangle t0... ";
  fuzzy_check( viennagrid::distance(M, t0), 2.0 );
  
  std::cout << "Distance of point N to triangle t0... ";
  fuzzy_check( viennagrid::distance(N, t0), std::sqrt(5.0) );
  
  std::cout << "Distance of point O to triangle t0... ";
  fuzzy_check( viennagrid::distance(O, t0), 4.0 / std::sqrt(5.0) );
  
  std::cout << "Distance of point P to triangle t0... ";
  fuzzy_check( viennagrid::distance(P, t0), std::sqrt(2.0) );
  
  std::cout << "Distance of point Q to triangle t0... ";
  fuzzy_check( viennagrid::distance(Q, t0), std::sqrt(5.0) );
  
  std::cout << "Distance of point R to triangle t0... ";
  fuzzy_check( viennagrid::distance(R, t0), 2.0 );
  
  std::cout << std::endl;

  //
  // Distance checks for t1
  //

  std::cout << "Distance of point A to triangle t1... ";
  fuzzy_check( viennagrid::distance(A, t1), std::sqrt(5.0) );
  
  std::cout << "Distance of point B to triangle t1... ";
  fuzzy_check( viennagrid::distance(B, t1), std::sqrt(2.0) );

  std::cout << "Distance of point C to triangle t1... ";
  fuzzy_check( viennagrid::distance(C, t1), 1.0 );
  
  std::cout << "Distance of point D to triangle t1... ";
  fuzzy_check( viennagrid::distance(D, t1), std::sqrt(2.0) );
  
  std::cout << "Distance of point E to triangle t1... ";
  fuzzy_check( viennagrid::distance(E, t1), 3.0 * std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point F to triangle t1... ";
  fuzzy_check( viennagrid::distance(F, t1), 2.0 );
  
  std::cout << "Distance of point G to triangle t1... ";
  fuzzy_check( viennagrid::distance(G, t1), 1.0 );
  
  std::cout << "Distance of point H to triangle t1... ";
  fuzzy_check( viennagrid::distance(H, t1), std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point I to triangle t1... ";
  fuzzy_check( viennagrid::distance(I, t1), std::sqrt(2.0) );
  
  std::cout << "Distance of point J to triangle t1... ";
  fuzzy_check( viennagrid::distance(J, t1), std::sqrt(5.0) );
  
  std::cout << "Distance of point K to triangle t1... ";
  fuzzy_check( viennagrid::distance(K, t1), 0.5 / std::sqrt(1.25) );
  
  std::cout << "Distance of point L to triangle t1... ";
  fuzzy_check( viennagrid::distance(L, t1), 2.0 );
  
  std::cout << "Distance of point M to triangle t1... ";
  fuzzy_check( viennagrid::distance(M, t1), 2.0 );
  
  std::cout << "Distance of point N to triangle t1... ";
  double s = ( std::sqrt(5.0) + std::sqrt(10.0) + std::sqrt(13.0) ) / 2.0; //semiperimeter
  fuzzy_check( viennagrid::distance(N, t1), std::sqrt( s * (s - std::sqrt(5.0)) * (s - std::sqrt(10.0)) * (s - std::sqrt(13.0)) ) * 2.0 / std::sqrt(5.0) ); //Heron's formula
  
  std::cout << "Distance of point O to triangle t1... ";
  fuzzy_check( viennagrid::distance(O, t1), std::sqrt(5.0) );
  
  std::cout << "Distance of point P to triangle t1... ";
  fuzzy_check( viennagrid::distance(P, t1), std::sqrt(2.0) );
  
  std::cout << "Distance of point Q to triangle t1... ";
  fuzzy_check( viennagrid::distance(Q, t1), std::sqrt(5.0) );
  
  std::cout << "Distance of point R to triangle t1... ";
  fuzzy_check( viennagrid::distance(R, t1), 2.0 );
  
  
}


//
// Quadrilateral
//

void setup_domain(viennagrid::domain_t<viennagrid::config::quadrilateral_2d> & d)
{
  typedef viennagrid::config::quadrilateral_2d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
  
  PointType p0(2.0, 1.0);
  PointType p1(3.0, 2.0);
  PointType p2(3.0, 3.0);
  PointType p3(1.0, 2.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  VertexType * vertices[4];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = d.push_back(v0);
  vertices[1] = d.push_back(v1);
  vertices[2] = d.push_back(v2);
  vertices[3] = d.push_back(v3);

  std::cout << "Adding cells to domain..." << std::endl;
  CellType quadrilateral;
  
  vertices[0] = &(viennagrid::ncells<0>(d)[0]);
  vertices[1] = &(viennagrid::ncells<0>(d)[1]);
  vertices[2] = &(viennagrid::ncells<0>(d)[3]);
  vertices[3] = &(viennagrid::ncells<0>(d)[2]);
  quadrilateral.vertices(vertices);
  d.push_back(quadrilateral);
  
  
}

void test(viennagrid::config::quadrilateral_2d)
{
  typedef viennagrid::config::quadrilateral_2d                          ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type               Domain;
  typedef ConfigType::cell_tag                                          CellTag;
  typedef viennagrid::segment_t<ConfigType>                             SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type                PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type             VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type             EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type  CellType;

  Domain domain;
  
  setup_domain(domain);
  
  PointType A(0, 0);
  PointType B(1, 0);
  PointType C(2, 0);
  PointType D(3, 0);
  PointType E(4, 0);
  
  PointType F(0, 1);
  PointType G(1, 1);
  PointType H(3, 1);
  PointType I(4, 1);
  
  PointType J(0, 2);
  PointType K(2, 2);
  PointType L(5, 2);

  PointType M(5, 3);
  
  PointType N(0, 4);
  PointType O(1, 4);
  PointType P(4, 4);

  PointType Q(2, 5);
  PointType R(3, 5);

  CellType quad = viennagrid::ncells<CellTag::dim>(domain)[0];
  
  //
  // Distance checks for quadrilateral
  //
  
  std::cout << "Distance of point A to quadrilateral... ";
  fuzzy_check( viennagrid::distance(A, quad), 3.0 * std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point B to quadrilateral... ";
  fuzzy_check( viennagrid::distance(B, quad), std::sqrt(2.0) );

  std::cout << "Distance of point C to quadrilateral... ";
  fuzzy_check( viennagrid::distance(C, quad), 1.0 );
  
  std::cout << "Distance of point D to quadrilateral... ";
  fuzzy_check( viennagrid::distance(D, quad), std::sqrt(2.0) );
  
  std::cout << "Distance of point E to quadrilateral... ";
  fuzzy_check( viennagrid::distance(E, quad), 3.0 * std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point F to quadrilateral... ";
  fuzzy_check( viennagrid::distance(F, quad), std::sqrt(2.0) );
  
  std::cout << "Distance of point G to quadrilateral... ";
  fuzzy_check( viennagrid::distance(G, quad), std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point H to quadrilateral... ";
  fuzzy_check( viennagrid::distance(H, quad), std::sqrt(2.0) / 2.0 );
  
  std::cout << "Distance of point I to quadrilateral... ";
  fuzzy_check( viennagrid::distance(I, quad), std::sqrt(2.0) );
  
  std::cout << "Distance of point J to quadrilateral... ";
  fuzzy_check( viennagrid::distance(J, quad), 1.0 );
  
  std::cout << "Distance of point K to quadrilateral... ";
  fuzzy_check( viennagrid::distance(K, quad), 0.0 );
  
  std::cout << "Distance of point L to quadrilateral... ";
  fuzzy_check( viennagrid::distance(L, quad), 2.0 );
  
  std::cout << "Distance of point M to quadrilateral... ";
  fuzzy_check( viennagrid::distance(M, quad), 2.0 );
  
  std::cout << "Distance of point N to quadrilateral... ";
  fuzzy_check( viennagrid::distance(N, quad), std::sqrt(5.0) );
  
  std::cout << "Distance of point O to quadrilateral... ";
  fuzzy_check( viennagrid::distance(O, quad), 4.0 / std::sqrt(5.0) );
  
  std::cout << "Distance of point P to quadrilateral... ";
  fuzzy_check( viennagrid::distance(P, quad), std::sqrt(2.0) );
  
  std::cout << "Distance of point Q to quadrilateral... ";
  fuzzy_check( viennagrid::distance(Q, quad), std::sqrt(5.0) );
  
  std::cout << "Distance of point R to quadrilateral... ";
  fuzzy_check( viennagrid::distance(R, quad), 2.0 );
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing line mesh in 2D ====" << std::endl;
  test(viennagrid::config::line_2d());

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test(viennagrid::config::triangular_2d());

  std::cout << "==== Testing quadrilateral mesh in 2D ====" << std::endl;
  test(viennagrid::config::quadrilateral_2d());
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
