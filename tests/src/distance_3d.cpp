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
#include <iostream>
#include <iomanip>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"

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

double heron_triangle_area(double a, double b, double c)
{
  double s = (a + b + c) / 2.0;

  return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

/* Computes the height on c of a triangle with edge lenghts a, b, c */
double height_from_edge_lengths(double a, double b, double c)
{
  return heron_triangle_area(a, b, c) / c * 2.0;
}

/* Computes the height in d of a tetrahedron with edge lenghts a, b, c, d */
template <typename PointType>
double height_for_tetrahedron(PointType const & A,
                              PointType const & B,
                              PointType const & C,
                              PointType const & D)
{
  double V = std::abs( viennagrid::inner_prod(A - D, viennagrid::cross_prod(B - D, C - D)) / 6.0 );
  double base_area = 0.5 * viennagrid::norm_2(viennagrid::cross_prod(B - A, C - A));

  return 3.0 * V / base_area;
}

template <typename LineType>
double line_distance_via_cross_prod(LineType const & line0, LineType const & line1)
{
  typedef typename LineType::config_type     ConfigType;
  typedef typename viennagrid::result_of::point<ConfigType>::type    PointType;
  
  PointType const & A = viennagrid::ncells<0>(line0)[0].point();
  PointType const & B = viennagrid::ncells<0>(line0)[1].point();
  
  PointType const & C = viennagrid::ncells<0>(line1)[0].point();
  PointType const & D = viennagrid::ncells<0>(line1)[1].point();
  
  PointType dir_0 = B - A;
  PointType dir_1 = D - C;
  
  //compute unit normal vector:
  PointType n = viennagrid::cross_prod(dir_0, dir_1);
  n /= viennagrid::norm_2(n);
  
  return std::abs( viennagrid::inner_prod(C - A, n) );
}

//
// Line 3d
//

void setup_domain(viennagrid::domain_t<viennagrid::config::line_3d> & d)
{
  typedef viennagrid::config::line_3d                             ConfigType;
  typedef ConfigType::cell_tag                                    CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim>::type        CellType;
  
  PointType p0(3.0, 0.0, 1.0);
  PointType p1(1.0, 1.0, 3.0);
  PointType p2(5.0, 1.0, 3.0);
  PointType p3(3.0, 2.0, 2.0);
  PointType p4(0.0, 3.0, 4.0);
  PointType p5(3.0, 3.0, 1.0);
  PointType p6(1.0, 4.0, 3.0);
  PointType p7(2.0, 1.0, 2.0);
  PointType p8(3.0, 2.0, 3.0);

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

  VertexType * vertices[8];
  
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

void test(viennagrid::config::line_3d)
{
  typedef viennagrid::config::line_3d                             ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type               Domain;
  typedef ConfigType::cell_tag                                          CellTag;
  typedef viennagrid::segment_t<ConfigType>                             SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type                PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type             VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type             EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type  CellType;

  Domain domain;

  setup_domain(domain);
  
  CellType line0 = viennagrid::ncells<CellTag::dim>(domain)[0];
  CellType line1 = viennagrid::ncells<CellTag::dim>(domain)[1];
  CellType line2 = viennagrid::ncells<CellTag::dim>(domain)[2];
  CellType line3 = viennagrid::ncells<CellTag::dim>(domain)[3];
  CellType line  = viennagrid::ncells<CellTag::dim>(domain)[4];
  CellType & line4 = line;
  
  double line_length = viennagrid::volume(line);
  
  //
  // Distance checks for points to line
  //
  
  std::cout << "Distance of point A to line... ";
  PointType A(0, 0, 0);
  fuzzy_check( viennagrid::distance(A, line), 3.0 );
  
  std::cout << "Distance of point B to line... ";
  PointType B(1, 0, 1);
  fuzzy_check( viennagrid::distance(B, line), std::sqrt(3.0) );

  std::cout << "Distance of point C to line... ";
  PointType C(2, 0, 3);
  fuzzy_check( viennagrid::distance(C, line), std::sqrt(2.0) );
  
  std::cout << "Distance of point D to line... ";
  PointType D(3, 0, 2);
  fuzzy_check( viennagrid::distance(D, line), 
               height_from_edge_lengths(std::sqrt(2.0), std::sqrt(5.0), line_length) );
  
  std::cout << "Distance of point E to line... ";
  PointType E(4, 0, 1);
  fuzzy_check( viennagrid::distance(E, line),
               height_from_edge_lengths(std::sqrt(6.0), std::sqrt(9.0), line_length) );
  
  std::cout << "Distance of point F to line... ";
  PointType F(0, 1, 1);
  fuzzy_check( viennagrid::distance(F, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point G to line... ";
  PointType G(1, 1, 1);
  fuzzy_check( viennagrid::distance(G, line), std::sqrt(2.0) );
  
  std::cout << "Distance of point H to line... ";
  PointType H(3, 1, 2);
  fuzzy_check( viennagrid::distance(H, line),
               height_from_edge_lengths(std::sqrt(1.0), std::sqrt(2.0), line_length) );
  
  std::cout << "Distance of point I to line... ";
  PointType I(4, 1, 3);
  fuzzy_check( viennagrid::distance(I, line),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(2.0), line_length) );
  
  std::cout << "Distance of point J to line... ";
  PointType J(0, 2, 3);
  fuzzy_check( viennagrid::distance(J, line), std::sqrt(6.0) );
               
  
  std::cout << "Distance of point K to line... ";
  PointType K(2, 2, 2);
  fuzzy_check( viennagrid::distance(K, line),
               height_from_edge_lengths(std::sqrt(1.0), std::sqrt(2.0), line_length) );
  
  std::cout << "Distance of point L to line... ";
  PointType L(5, 2, 1);
  fuzzy_check( viennagrid::distance(L, line), std::sqrt(8.0) );
  
  std::cout << "Distance of point M to line... ";
  PointType M(5, 3, 1);
  fuzzy_check( viennagrid::distance(M, line), std::sqrt(9.0) );
  
  std::cout << "Distance of point N to line... ";
  PointType N(0, 4, 2);
  fuzzy_check( viennagrid::distance(N, line),
               height_from_edge_lengths(std::sqrt(13.0), std::sqrt(14.0), line_length) );
  
  std::cout << "Distance of point O to line... ";
  PointType O(1, 4, 2);
  fuzzy_check( viennagrid::distance(O, line),
               height_from_edge_lengths(std::sqrt(10.0), std::sqrt(9.0), line_length) );
  
  std::cout << "Distance of point P to line... ";
  PointType P(4, 4, 3);
  fuzzy_check( viennagrid::distance(P, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point Q to line... ";
  PointType Q(2, 5, 3);
  fuzzy_check( viennagrid::distance(Q, line), std::sqrt(10.0) );
  
  std::cout << "Distance of point R to line... ";
  PointType R(3, 5, 4);
  fuzzy_check( viennagrid::distance(R, line), std::sqrt(10.0) );
  
  
  //
  // Distance checks line to line
  //
  
  std::cout << "Distance of line0 to line0... ";
  fuzzy_check( viennagrid::distance(line0, line0), 0.0 );

  std::cout << "Distance of line0 to line1... ";
  fuzzy_check( viennagrid::distance(line0, line1), std::sqrt(3.0) );

  std::cout << "Distance of line0 to line2... ";
  fuzzy_check( viennagrid::distance(line0, line2), 0.0 );

  std::cout << "Distance of line0 to line3... ";
  fuzzy_check( viennagrid::distance(line0, line3),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(9.0), viennagrid::volume(line0)) );               
  
  std::cout << "Distance of line0 to line4... ";
  double dist_04 = line_distance_via_cross_prod(line0, line4);
  fuzzy_check( viennagrid::distance(line0, line4), dist_04);
  

  std::cout << "Distance of line1 to line0... ";
  fuzzy_check( viennagrid::distance(line1, line0), std::sqrt(3.0) );

  std::cout << "Distance of line1 to line1... ";
  fuzzy_check( viennagrid::distance(line1, line1), 0.0 );

  std::cout << "Distance of line1 to line2... ";
  fuzzy_check( viennagrid::distance(line1, line2), 0.0 );

  std::cout << "Distance of line1 to line3... ";
  fuzzy_check( viennagrid::distance(line1, line3), std::sqrt(6.0) );
  
  std::cout << "Distance of line1 to line4... ";
  fuzzy_check( viennagrid::distance(line1, line4), std::sqrt(2.0) );
  

  
  std::cout << "Distance of line2 to line0... ";
  fuzzy_check( viennagrid::distance(line2, line0), 0.0 );

  std::cout << "Distance of line2 to line1... ";
  fuzzy_check( viennagrid::distance(line2, line1), 0.0 );

  std::cout << "Distance of line2 to line2... ";
  fuzzy_check( viennagrid::distance(line2, line2), 0.0 );

  std::cout << "Distance of line2 to line3... ";
  fuzzy_check( viennagrid::distance(line2, line3),  
               height_from_edge_lengths(std::sqrt(6.0), std::sqrt(2.0), viennagrid::volume(line2)) );               
  
  std::cout << "Distance of line2 to line4... ";
  PointType p_24_2 = (1.0 - 3.0/8.0) * viennagrid::ncells<0>(line2)[0].point() 
                          + 3.0/8.0  * viennagrid::ncells<0>(line2)[1].point();
  PointType p_24_4 = (1.0 - 1.0/4.0) * viennagrid::ncells<0>(line4)[0].point() 
                          + 1.0/4.0  * viennagrid::ncells<0>(line4)[1].point();
  double dist_24 = viennagrid::distance(p_24_2, p_24_4);
  fuzzy_check( viennagrid::distance(line2, line4), dist_24 );
  
  

  std::cout << "Distance of line3 to line0... ";
  fuzzy_check( viennagrid::distance(line3, line0),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(9.0), viennagrid::volume(line0)) );               

  std::cout << "Distance of line3 to line1... ";
  fuzzy_check( viennagrid::distance(line3, line1), std::sqrt(6.0) );

  std::cout << "Distance of line3 to line2... ";
  fuzzy_check( viennagrid::distance(line3, line2), height_from_edge_lengths(std::sqrt(6.0), std::sqrt(2.0), viennagrid::volume(line2)) );

  std::cout << "Distance of line3 to line3... ";
  fuzzy_check( viennagrid::distance(line3, line3), 0.0 );
  
  std::cout << "Distance of line3 to line4... ";
  double dist_34 = line_distance_via_cross_prod(line3, line4);
  fuzzy_check( viennagrid::distance(line3, line4), dist_34 );
  

  
  std::cout << "Distance of line4 to line0... ";
  fuzzy_check( viennagrid::distance(line4, line0), dist_04 );

  std::cout << "Distance of line4 to line1... ";
  fuzzy_check( viennagrid::distance(line4, line1), std::sqrt(2.0) );

  std::cout << "Distance of line4 to line2... ";
  fuzzy_check( viennagrid::distance(line4, line2), dist_24 );

  std::cout << "Distance of line4 to line3... ";
  fuzzy_check( viennagrid::distance(line4, line3), dist_34 );
  
  std::cout << "Distance of line4 to line4... ";
  fuzzy_check( viennagrid::distance(line4, line4), 0.0 );
  
  
}


//
// Triangular
//

void setup_domain(viennagrid::domain_t<viennagrid::config::triangular_3d> & d)
{
  typedef viennagrid::config::triangular_3d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
  
  PointType p0(2.0, 1.0, 2.0);
  PointType p1(3.0, 2.0, 1.0);
  PointType p2(3.0, 3.0, 3.0);
  PointType p3(1.0, 2.0, 1.0);

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




void test(viennagrid::config::triangular_3d)
{
  typedef viennagrid::config::triangular_3d                             ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type               Domain;
  typedef ConfigType::cell_tag                                          CellTag;
  typedef viennagrid::segment_t<ConfigType>                             SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type                PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type             VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type             EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type  CellType;

  Domain domain;
  
  setup_domain(domain);
  
  PointType A(0, 0, 0);
  PointType B(1, 0, 2);
  PointType C(2, 0, 3);
  PointType D(3, 0, 4);
  PointType E(4, 0, 1);
  
  PointType F(0, 1, 1);
  PointType G(1, 1, 2);
  PointType H(3, 1, 1);
  PointType I(4, 1, 0);
  
  PointType J(0, 2, 4);
  PointType K(2, 2, 2);
  PointType K2(2, 2, 3);
  PointType L(5, 2, 2);

  PointType M(5, 3, 2);
  
  PointType N(0, 4, 1);
  PointType O(1, 4, 2);
  PointType P(4, 4, 3);

  PointType Q(2, 5, 2);
  PointType R(3, 5, 1);

  CellType t0 = viennagrid::ncells<CellTag::dim>(domain)[0];
  CellType t1 = viennagrid::ncells<CellTag::dim>(domain)[1];
  
  //
  // Distance checks for t0
  //
  VertexType const & v0_0 = viennagrid::ncells<0>(t0)[0];
  VertexType const & v0_1 = viennagrid::ncells<0>(t0)[1];
  VertexType const & v0_2 = viennagrid::ncells<0>(t0)[2];
  
  EdgeType const & e0_0 = viennagrid::ncells<1>(t0)[0];
  EdgeType const & e0_1 = viennagrid::ncells<1>(t0)[1];
  EdgeType const & e0_2 = viennagrid::ncells<1>(t0)[2];
  
  std::cout << "Distance of point A to triangle t0... ";
  fuzzy_check( viennagrid::distance(A, t0), viennagrid::distance(A, e0_1) );
  
  std::cout << "Distance of point B to triangle t0... ";
  fuzzy_check( viennagrid::distance(B, t0), viennagrid::distance(B, e0_1) );

  std::cout << "Distance of point C to triangle t0... ";
  fuzzy_check( viennagrid::distance(C, t0), viennagrid::distance(C, v0_0) );
  
  std::cout << "Distance of point D to triangle t0... ";
  fuzzy_check( viennagrid::distance(D, t0), viennagrid::distance(D, e0_0) );
  
  std::cout << "Distance of point E to triangle t0... ";
  fuzzy_check( viennagrid::distance(E, t0), viennagrid::distance(E, e0_0) );
  
  std::cout << "Distance of point F to triangle t0... ";
  fuzzy_check( viennagrid::distance(F, t0), viennagrid::distance(F, e0_1) );
  
  std::cout << "Distance of point G to triangle t0... ";
  fuzzy_check( viennagrid::distance(G, t0), viennagrid::distance(G, e0_1) );
  
  std::cout << "Distance of point H to triangle t0... ";
  fuzzy_check( viennagrid::distance(H, t0), viennagrid::distance(H, e0_0) );
  
  std::cout << "Distance of point I to triangle t0... ";
  fuzzy_check( viennagrid::distance(I, t0), viennagrid::distance(I, e0_0) );
  
  std::cout << "Distance of point J to triangle t0... ";
  fuzzy_check( viennagrid::distance(J, t0), height_for_tetrahedron(v0_0.point(), v0_1.point(), v0_2.point(), J) );
  
  std::cout << "Distance of point K to triangle t0... ";
  fuzzy_check( viennagrid::distance(K, t0), 0.0 );

  std::cout << "Distance of point K2 to triangle t0... ";
  fuzzy_check( viennagrid::distance(K2, t0), height_for_tetrahedron(v0_0.point(), v0_1.point(), v0_2.point(), K2) );
  
  std::cout << "Distance of point L to triangle t0... ";
  fuzzy_check( viennagrid::distance(L, t0), viennagrid::distance(L, e0_0) );
  
  std::cout << "Distance of point M to triangle t0... ";
  fuzzy_check( viennagrid::distance(M, t0), viennagrid::distance(M, v0_1) );
  
  std::cout << "Distance of point N to triangle t0... ";
  fuzzy_check( viennagrid::distance(N, t0), viennagrid::distance(N, e0_2) );
  
  std::cout << "Distance of point O to triangle t0... ";
  fuzzy_check( viennagrid::distance(O, t0), viennagrid::distance(O, e0_2) );
  
  std::cout << "Distance of point P to triangle t0... ";
  fuzzy_check( viennagrid::distance(P, t0), viennagrid::distance(P, v0_1) );
  
  std::cout << "Distance of point Q to triangle t0... ";
  fuzzy_check( viennagrid::distance(Q, t0), viennagrid::distance(Q, e0_2) );
  
  std::cout << "Distance of point R to triangle t0... ";
  fuzzy_check( viennagrid::distance(t0, R), viennagrid::distance(R, e0_2) );
  
  std::cout << std::endl;

  //
  // Distance checks for t1
  //
  VertexType const & v1_0 = viennagrid::ncells<0>(t1)[0];
  VertexType const & v1_1 = viennagrid::ncells<0>(t1)[1];
  VertexType const & v1_2 = viennagrid::ncells<0>(t1)[2];
  
  EdgeType const & e1_0 = viennagrid::ncells<1>(t1)[0];
  EdgeType const & e1_1 = viennagrid::ncells<1>(t1)[1];
  EdgeType const & e1_2 = viennagrid::ncells<1>(t1)[2];
  

  std::cout << "Distance of point A to triangle t1... ";
  fuzzy_check( viennagrid::distance(A, t1), viennagrid::distance(A, v1_0) );
  
  std::cout << "Distance of point B to triangle t1... ";
  fuzzy_check( viennagrid::distance(B, t1), viennagrid::distance(B, v1_0) );

  std::cout << "Distance of point C to triangle t1... ";
  fuzzy_check( viennagrid::distance(C, t1), viennagrid::distance(C, v1_0) );
  
  std::cout << "Distance of point D to triangle t1... ";
  fuzzy_check( viennagrid::distance(D, t1), viennagrid::distance(D, e1_1) );
  
  std::cout << "Distance of point E to triangle t1... ";
  fuzzy_check( viennagrid::distance(E, t1), viennagrid::distance(E, e1_0) );
  
  std::cout << "Distance of point F to triangle t1... ";
  fuzzy_check( viennagrid::distance(F, t1), viennagrid::distance(F, v1_0) );
  
  std::cout << "Distance of point G to triangle t1... ";
  fuzzy_check( viennagrid::distance(G, t1), viennagrid::distance(G, v1_0) );
  
  std::cout << "Distance of point H to triangle t1... ";
  fuzzy_check( viennagrid::distance(H, t1), viennagrid::distance(H, e1_0) );
  
  std::cout << "Distance of point I to triangle t1... ";
  fuzzy_check( viennagrid::distance(I, t1), viennagrid::distance(I, e1_0) );
  
  std::cout << "Distance of point J to triangle t1... ";
  fuzzy_check( viennagrid::distance(J, t1), viennagrid::distance(J, e1_1) );
  
  std::cout << "Distance of point K to triangle t1... ";
  fuzzy_check( viennagrid::distance(K, t1), height_for_tetrahedron(v1_0.point(), v1_1.point(), v1_2.point(), K) );
  
  std::cout << "Distance of point K2 to triangle t1... ";
  fuzzy_check( viennagrid::distance(K2, t1), viennagrid::distance(K2, e1_1) );
  
  std::cout << "Distance of point L to triangle t1... ";
  fuzzy_check( viennagrid::distance(L, t1), viennagrid::distance(L, e1_2) );
  
  std::cout << "Distance of point M to triangle t1... ";
  fuzzy_check( viennagrid::distance(M, t1), viennagrid::distance(M, e1_2) );
  
  std::cout << "Distance of point N to triangle t1... ";
  fuzzy_check( viennagrid::distance(N, t1), height_for_tetrahedron(v1_0.point(), v1_1.point(), v1_2.point(), N) ); 
  
  std::cout << "Distance of point O to triangle t1... ";
  fuzzy_check( viennagrid::distance(O, t1), height_for_tetrahedron(v1_0.point(), v1_1.point(), v1_2.point(), O) );
  
  std::cout << "Distance of point P to triangle t1... ";
  fuzzy_check( viennagrid::distance(P, t1), viennagrid::distance(P, v1_2) );
  
  std::cout << "Distance of point Q to triangle t1... ";
  fuzzy_check( viennagrid::distance(Q, t1), viennagrid::distance(Q, e1_1) );
  
  std::cout << "Distance of point R to triangle t1... ";
  fuzzy_check( viennagrid::distance(R, t1), viennagrid::distance(R, e1_2) );
  
  
}




//
// Quadrilateral
//

void setup_domain(viennagrid::domain_t<viennagrid::config::quadrilateral_3d> & d)
{
  typedef viennagrid::config::quadrilateral_3d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
  
  PointType p0(2.0, 1.0, 2.0);
  PointType p1(3.0, 2.0, 1.0);
  PointType p2(3.0, 3.0, 3.0);
  PointType p3(1.0, 2.0, 1.0);

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

void test(viennagrid::config::quadrilateral_3d)
{
  typedef viennagrid::config::quadrilateral_3d                          ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type               Domain;
  typedef ConfigType::cell_tag                                          CellTag;
  typedef viennagrid::segment_t<ConfigType>                             SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type                PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type             VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type             EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type  CellType;

  Domain domain;
  
  setup_domain(domain);
  
  PointType A(0, 0, 0);
  PointType B(1, 0, 2);
  PointType C(2, 0, 3);
  PointType D(3, 0, 4);
  PointType E(4, 0, 1);
  
  PointType F(0, 1, 1);
  PointType G(1, 1, 2);
  PointType H(3, 1, 1);
  PointType I(4, 1, 0);
  
  PointType J(0, 2, 4);
  PointType K(2, 2, 1);
  PointType K2(2, 2, 2);
  PointType L(5, 2, 2);

  PointType M(5, 3, 2);
  
  PointType N(0, 4, 1);
  PointType O(1, 4, 2);
  PointType P(4, 4, 3);

  PointType Q(2, 5, 2);
  PointType R(3, 5, 1);

  CellType & quad = viennagrid::ncells<CellTag::dim>(domain)[0];
  
  //
  // Distance checks for quadrilateral
  //

  VertexType & v0 = viennagrid::ncells<0>(quad)[0];
  VertexType & v1 = viennagrid::ncells<0>(quad)[1];
  VertexType & v2 = viennagrid::ncells<0>(quad)[2];
  VertexType & v3 = viennagrid::ncells<0>(quad)[3];
  
  EdgeType const & e0 = viennagrid::ncells<1>(quad)[0];
  EdgeType const & e1 = viennagrid::ncells<1>(quad)[1];
  EdgeType const & e2 = viennagrid::ncells<1>(quad)[2];
  EdgeType const & e3 = viennagrid::ncells<1>(quad)[3];
  
  EdgeType e_diag;
  VertexType * diag_vertices[2];
  diag_vertices[0] = &v1;
  diag_vertices[1] = &v2;
  e_diag.vertices(diag_vertices);

  
  std::cout << "Distance of point A to quadrilateral... ";
  fuzzy_check( viennagrid::distance(A, quad), viennagrid::distance(A, e1) );
  
  std::cout << "Distance of point B to quadrilateral... ";
  fuzzy_check( viennagrid::distance(B, quad), viennagrid::distance(e1, B) );

  std::cout << "Distance of point C to quadrilateral... ";
  fuzzy_check( viennagrid::distance(C, quad), viennagrid::distance(v0, C) );
  
  std::cout << "Distance of point D to quadrilateral... ";
  fuzzy_check( viennagrid::distance(D, quad), viennagrid::distance(D, e0) );
  
  std::cout << "Distance of point E to quadrilateral... ";
  fuzzy_check( viennagrid::distance(E, quad), viennagrid::distance(E, e0) );
  
  std::cout << "Distance of point F to quadrilateral... ";
  fuzzy_check( viennagrid::distance(F, quad), viennagrid::distance(F, e1) );
  
  std::cout << "Distance of point G to quadrilateral... ";
  fuzzy_check( viennagrid::distance(G, quad), viennagrid::distance(G, e1) );
  
  std::cout << "Distance of point H to quadrilateral... ";
  fuzzy_check( viennagrid::distance(H, quad), viennagrid::distance(H, e0) );
  
  std::cout << "Distance of point I to quadrilateral... ";
  fuzzy_check( viennagrid::distance(I, quad), viennagrid::distance(I, e0) );
  
  std::cout << "Distance of point J to quadrilateral... ";
  //fuzzy_check( viennagrid::distance(J, quad), height_for_tetrahedron(v0.point(), v1.point(), v2.point(), J) );
  fuzzy_check( viennagrid::distance(J, quad), viennagrid::distance(J, e3) );
  //fuzzy_check( viennagrid::distance(J, quad), height_for_tetrahedron(v0.point(), v1.point(), v2.point(), J) );
  
  std::cout << "Distance of point K to quadrilateral... ";
  fuzzy_check( viennagrid::distance(K, quad), 0.0 );

  std::cout << "Distance of point K2 to quadrilateral... ";
  fuzzy_check( viennagrid::distance(quad, K2), height_for_tetrahedron(v1.point(), v3.point(), v2.point(), K2) );
  
  std::cout << "Distance of point L to quadrilateral... ";
  fuzzy_check( viennagrid::distance(L, quad), viennagrid::distance(L, e2) );
  
  std::cout << "Distance of point M to quadrilateral... ";
  fuzzy_check( viennagrid::distance(M, quad), viennagrid::distance(M, e2) );
  
  std::cout << "Distance of point N to quadrilateral... ";
  fuzzy_check( viennagrid::distance(N, quad), viennagrid::distance(N, e3) );
  
  std::cout << "Distance of point O to quadrilateral... ";
  fuzzy_check( viennagrid::distance(O, quad), viennagrid::distance(O, e3) );
  
  std::cout << "Distance of point P to quadrilateral... ";
  fuzzy_check( viennagrid::distance(P, quad), viennagrid::distance(P, v3) );
  
  std::cout << "Distance of point Q to quadrilateral... ";
  fuzzy_check( viennagrid::distance(Q, quad), viennagrid::distance(Q, e3) );
  
  std::cout << "Distance of point R to quadrilateral... ";
  fuzzy_check( viennagrid::distance(R, quad), height_for_tetrahedron(v1.point(), v3.point(), v2.point(), R) );
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing line mesh in 3D ====" << std::endl;
  test(viennagrid::config::line_3d());

  std::cout << "==== Testing triangular mesh in 3D ====" << std::endl;
  test(viennagrid::config::triangular_3d());

  std::cout << "==== Testing quadrilateral mesh in 3D ====" << std::endl;
  test(viennagrid::config::quadrilateral_3d());
  
/*  std::cout << "==== Testing tetrahedral mesh in 3D ====" << std::endl;
  test(viennagrid::config::tetrahedral_3d());
  
  std::cout << "==== Testing hexahedral mesh in 3D ====" << std::endl;
  test(viennagrid::config::hexahedral_3d());*/
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
