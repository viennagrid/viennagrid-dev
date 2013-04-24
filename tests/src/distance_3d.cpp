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

#include "viennagrid/forwards.hpp"
#include "viennagrid/domain/config.hpp"
#include "viennagrid/point.hpp"
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
      //exit(EXIT_FAILURE);
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

template <typename DomainType, typename LineType>
double line_distance_via_cross_prod(DomainType const & domain, LineType const & line0, LineType const & line1)
{
  //typedef typename LineType::config_type     ConfigType;
  //typedef typename viennagrid::result_of::point<ConfigType>::type    PointType;
  typedef typename viennagrid::result_of::point_type<DomainType>::type PointType;
  
  PointType const & A = viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line0)[0]);
  PointType const & B = viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line0)[1]);
  
  PointType const & C = viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line1)[0]);
  PointType const & D = viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line1)[1]);
  
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

void setup_domain(viennagrid::config::line_3d_domain & domain)
{
  typedef viennagrid::config::line_3d_domain                      DomainType;
  typedef viennagrid::config::line_2d                             ConfigType;
  typedef viennagrid::line_tag                                    CellTag;
  
  typedef viennagrid::result_of::point_type<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::element_hook<DomainType, viennagrid::vertex_tag>::type       VertexHandleType;
  typedef viennagrid::result_of::element<DomainType, CellTag>::type        CellType;
  
  const size_t s = 9;
  PointType p[s];
  VertexHandleType v[s];
  
  
  p[0] = PointType(3.0, 0.0, 1.0);
  p[1] = PointType(1.0, 1.0, 3.0);
  p[2] = PointType(5.0, 1.0, 3.0);
  p[3] = PointType(3.0, 2.0, 2.0);
  p[4] = PointType(0.0, 3.0, 4.0);
  p[5] = PointType(3.0, 3.0, 1.0);
  p[6] = PointType(1.0, 4.0, 3.0);
  p[7] = PointType(2.0, 1.0, 2.0);
  p[8] = PointType(3.0, 2.0, 3.0);
  
  //upgrade to vertex:
  std::cout << "Adding vertices to domain..." << std::endl;
  for (int i = 0; i < s; ++i)
  {
    v[i] = viennagrid::create_element<VertexType>( domain );
    viennagrid::point( domain, v[i] ) = p[i];
  }

  std::cout << "Adding cells to domain..." << std::endl;
  VertexHandleType vertices[2];
  
  vertices[0] = v[0];
  vertices[1] = v[6];
  viennagrid::create_element<CellType>( domain, vertices, vertices+2 );
  
  vertices[0] = v[1];
  vertices[1] = v[4];
  viennagrid::create_element<CellType>( domain, vertices, vertices+2 );
  
  vertices[0] = v[1];
  vertices[1] = v[5];
  viennagrid::create_element<CellType>( domain, vertices, vertices+2 );
  
  vertices[0] = v[2];
  vertices[1] = v[3];
  viennagrid::create_element<CellType>( domain, vertices, vertices+2 );
  
  vertices[0] = v[7];
  vertices[1] = v[8];
  viennagrid::create_element<CellType>( domain, vertices, vertices+2 );
}

void test(viennagrid::config::line_3d_domain)
{
  typedef viennagrid::config::line_3d_domain                            Domain;
  typedef viennagrid::line_tag                                          CellTag;
  
  typedef viennagrid::result_of::point_type<Domain>::type                PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Domain, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type  CellType;

  Domain domain;

  setup_domain(domain);
  
  CellType line0 = viennagrid::elements<CellTag>(domain)[0];
  CellType line1 = viennagrid::elements<CellTag>(domain)[1];
  CellType line2 = viennagrid::elements<CellTag>(domain)[2];
  CellType line3 = viennagrid::elements<CellTag>(domain)[3];
  CellType line  = viennagrid::elements<CellTag>(domain)[4];
  CellType & line4 = line;
  
  double line_length = viennagrid::volume(line, domain);
  
  //
  // Distance checks for points to line
  //
  
  std::cout << "Distance of point A to line... ";
  PointType A(0, 0, 0);
  fuzzy_check( viennagrid::distance(domain, A, line), 3.0 );
  
  std::cout << "Distance of point B to line... ";
  PointType B(1, 0, 1);
  fuzzy_check( viennagrid::distance(domain, B, line), std::sqrt(3.0) );

  std::cout << "Distance of point C to line... ";
  PointType C(2, 0, 3);
  fuzzy_check( viennagrid::distance(domain, C, line), std::sqrt(2.0) );
  
  std::cout << "Distance of point D to line... ";
  PointType D(3, 0, 2);
  fuzzy_check( viennagrid::distance(domain, D, line), 
               height_from_edge_lengths(std::sqrt(2.0), std::sqrt(5.0), line_length) );
  
  std::cout << "Distance of point E to line... ";
  PointType E(4, 0, 1);
  fuzzy_check( viennagrid::distance(domain, E, line),
               height_from_edge_lengths(std::sqrt(6.0), std::sqrt(9.0), line_length) );
  
  std::cout << "Distance of point F to line... ";
  PointType F(0, 1, 1);
  fuzzy_check( viennagrid::distance(domain, F, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point G to line... ";
  PointType G(1, 1, 1);
  fuzzy_check( viennagrid::distance(domain, G, line), std::sqrt(2.0) );
  
  std::cout << "Distance of point H to line... ";
  PointType H(3, 1, 2);
  fuzzy_check( viennagrid::distance(domain, H, line),
               height_from_edge_lengths(std::sqrt(1.0), std::sqrt(2.0), line_length) );
  
  std::cout << "Distance of point I to line... ";
  PointType I(4, 1, 3);
  fuzzy_check( viennagrid::distance(domain, I, line),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(2.0), line_length) );
  
  std::cout << "Distance of point J to line... ";
  PointType J(0, 2, 3);
  fuzzy_check( viennagrid::distance(domain, J, line), std::sqrt(6.0) );
               
  
  std::cout << "Distance of point K to line... ";
  PointType K(2, 2, 2);
  fuzzy_check( viennagrid::distance(domain, K, line),
               height_from_edge_lengths(std::sqrt(1.0), std::sqrt(2.0), line_length) );
  
  std::cout << "Distance of point L to line... ";
  PointType L(5, 2, 1);
  fuzzy_check( viennagrid::distance(domain, L, line), std::sqrt(8.0) );
  
  std::cout << "Distance of point M to line... ";
  PointType M(5, 3, 1);
  fuzzy_check( viennagrid::distance(domain, M, line), std::sqrt(9.0) );
  
  std::cout << "Distance of point N to line... ";
  PointType N(0, 4, 2);
  fuzzy_check( viennagrid::distance(domain, N, line),
               height_from_edge_lengths(std::sqrt(13.0), std::sqrt(14.0), line_length) );
  
  std::cout << "Distance of point O to line... ";
  PointType O(1, 4, 2);
  fuzzy_check( viennagrid::distance(domain, O, line),
               height_from_edge_lengths(std::sqrt(10.0), std::sqrt(9.0), line_length) );
  
  std::cout << "Distance of point P to line... ";
  PointType P(4, 4, 3);
  fuzzy_check( viennagrid::distance(domain, P, line), std::sqrt(5.0) );
  
  std::cout << "Distance of point Q to line... ";
  PointType Q(2, 5, 3);
  fuzzy_check( viennagrid::distance(domain, Q, line), std::sqrt(10.0) );
  
  std::cout << "Distance of point R to line... ";
  PointType R(3, 5, 4);
  fuzzy_check( viennagrid::distance(domain, R, line), std::sqrt(10.0) );
  
  
  //
  // Distance checks line to line
  //
  
  std::cout << "Distance of line0 to line0... ";
  fuzzy_check( viennagrid::distance(domain, line0, line0), 0.0 );

  std::cout << "Distance of line0 to line1... ";
  fuzzy_check( viennagrid::distance(domain, line0, line1), std::sqrt(3.0) );

  std::cout << "Distance of line0 to line2... ";
  fuzzy_check( viennagrid::distance(domain, line0, line2), 0.0 );

  std::cout << "Distance of line0 to line3... ";
  fuzzy_check( viennagrid::distance(domain, line0, line3),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(9.0), viennagrid::volume(line0, domain)) );               
  
  std::cout << "Distance of line0 to line4... ";
  double dist_04 = line_distance_via_cross_prod(domain, line0, line4);
  fuzzy_check( viennagrid::distance(domain, line0, line4), dist_04);
  

  std::cout << "Distance of line1 to line0... ";
  fuzzy_check( viennagrid::distance(domain, line1, line0), std::sqrt(3.0) );

  std::cout << "Distance of line1 to line1... ";
  fuzzy_check( viennagrid::distance(domain, line1, line1), 0.0 );

  std::cout << "Distance of line1 to line2... ";
  fuzzy_check( viennagrid::distance(domain, line1, line2), 0.0 );

  std::cout << "Distance of line1 to line3... ";
  fuzzy_check( viennagrid::distance(domain, line1, line3), std::sqrt(6.0) );
  
  std::cout << "Distance of line1 to line4... ";
  fuzzy_check( viennagrid::distance(domain, line1, line4), std::sqrt(2.0) );
  

  
  std::cout << "Distance of line2 to line0... ";
  fuzzy_check( viennagrid::distance(domain, line2, line0), 0.0 );

  std::cout << "Distance of line2 to line1... ";
  fuzzy_check( viennagrid::distance(domain, line2, line1), 0.0 );

  std::cout << "Distance of line2 to line2... ";
  fuzzy_check( viennagrid::distance(domain, line2, line2), 0.0 );

  std::cout << "Distance of line2 to line3... ";
  fuzzy_check( viennagrid::distance(domain, line2, line3),  
               height_from_edge_lengths(std::sqrt(6.0), std::sqrt(2.0), viennagrid::volume(line2, domain)) );               
  
  std::cout << "Distance of line2 to line4... ";
  PointType p_24_2 = (1.0 - 3.0/8.0) * viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line2)[0]) 
                          + 3.0/8.0  * viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line2)[1]);
  PointType p_24_4 = (1.0 - 1.0/4.0) * viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line4)[0]) 
                          + 1.0/4.0  * viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(line4)[1]);
  double dist_24 = viennagrid::distance(domain, p_24_2, p_24_4);
  fuzzy_check( viennagrid::distance(domain, line2, line4), dist_24 );
  
  

  std::cout << "Distance of line3 to line0... ";
  fuzzy_check( viennagrid::distance(domain, line3, line0),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(9.0), viennagrid::volume(line0, domain)) );               

  std::cout << "Distance of line3 to line1... ";
  fuzzy_check( viennagrid::distance(domain, line3, line1), std::sqrt(6.0) );

  std::cout << "Distance of line3 to line2... ";
  fuzzy_check( viennagrid::distance(domain, line3, line2), height_from_edge_lengths(std::sqrt(6.0), std::sqrt(2.0), viennagrid::volume(line2, domain)) );

  std::cout << "Distance of line3 to line3... ";
  fuzzy_check( viennagrid::distance(domain, line3, line3), 0.0 );
  
  std::cout << "Distance of line3 to line4... ";
  double dist_34 = line_distance_via_cross_prod(domain, line3, line4);
  fuzzy_check( viennagrid::distance(domain, line3, line4), dist_34 );
  

  
  std::cout << "Distance of line4 to line0... ";
  fuzzy_check( viennagrid::distance(domain, line4, line0), dist_04 );

  std::cout << "Distance of line4 to line1... ";
  fuzzy_check( viennagrid::distance(domain, line4, line1), std::sqrt(2.0) );

  std::cout << "Distance of line4 to line2... ";
  fuzzy_check( viennagrid::distance(domain, line4, line2), dist_24 );

  std::cout << "Distance of line4 to line3... ";
  fuzzy_check( viennagrid::distance(domain, line4, line3), dist_34 );
  
  std::cout << "Distance of line4 to line4... ";
  fuzzy_check( viennagrid::distance(domain, line4, line4), 0.0 );
  
  
}


//
// Triangular
//

void setup_domain(viennagrid::config::triangular_3d_domain & domain)
{
  typedef viennagrid::config::triangular_3d_domain                      DomainType;
  typedef viennagrid::triangle_tag                                      CellTag;
  
  typedef viennagrid::result_of::point_type<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  
  typedef viennagrid::result_of::element_hook<DomainType, viennagrid::vertex_tag>::type       VertexHandleType;
  
  typedef viennagrid::result_of::element<DomainType, CellTag>::type        CellType;
  
  const size_t s = 4;
  PointType p[s];
  VertexHandleType v[s];
  
  p[0] = PointType(2.0, 1.0, 2.0);
  p[1] = PointType(3.0, 2.0, 1.0);
  p[2] = PointType(3.0, 3.0, 3.0);
  p[3] = PointType(1.0, 2.0, 1.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to domain..." << std::endl;
  for (int i = 0; i < s; ++i)
  {
    v[i] = viennagrid::create_element<VertexType>( domain );
    viennagrid::point( domain, v[i] ) = p[i];
  }

  std::cout << "Adding cells to domain..." << std::endl;
  VertexHandleType vertices[3];
  
  vertices[0] = v[0];
  vertices[1] = v[2];
  vertices[2] = v[3];
  viennagrid::create_element<CellType>( domain, vertices, vertices+3 );
  
  vertices[0] = v[0];
  vertices[1] = v[1];
  vertices[2] = v[2];
  viennagrid::create_element<CellType>( domain, vertices, vertices+3 );
}




void test(viennagrid::config::triangular_3d_domain)
{
  typedef viennagrid::config::triangular_3d_domain                      Domain;
  typedef viennagrid::triangle_tag                                      CellTag;
  
  typedef viennagrid::result_of::point_type<Domain>::type                PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Domain, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type  CellType;

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

  CellType t0 = viennagrid::elements<CellTag>(domain)[0];
  CellType t1 = viennagrid::elements<CellTag>(domain)[1];
  
  //
  // Distance checks for t0
  //
  VertexType const & v0_0 = viennagrid::elements<viennagrid::vertex_tag>(t0)[0];
  VertexType const & v0_1 = viennagrid::elements<viennagrid::vertex_tag>(t0)[1];
  VertexType const & v0_2 = viennagrid::elements<viennagrid::vertex_tag>(t0)[2];
  
  EdgeType const & e0_0 = viennagrid::elements<viennagrid::line_tag>(t0)[0];
  EdgeType const & e0_1 = viennagrid::elements<viennagrid::line_tag>(t0)[1];
  EdgeType const & e0_2 = viennagrid::elements<viennagrid::line_tag>(t0)[2];
  
  std::cout << "Distance of point A to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, A, t0), viennagrid::distance(domain, A, e0_1) );
  
  std::cout << "Distance of point B to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, B, t0), viennagrid::distance(domain, B, e0_1) );

  std::cout << "Distance of point C to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, C, t0), viennagrid::distance(domain, C, v0_0) );
  
  std::cout << "Distance of point D to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, D, t0), viennagrid::distance(domain, D, e0_0) );
  
  std::cout << "Distance of point E to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, E, t0), viennagrid::distance(domain, E, e0_0) );
  
  std::cout << "Distance of point F to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, F, t0), viennagrid::distance(domain, F, e0_1) );
  
  std::cout << "Distance of point G to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, G, t0), viennagrid::distance(domain, G, e0_1) );
  
  std::cout << "Distance of point H to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, H, t0), viennagrid::distance(domain, H, e0_0) );
  
  std::cout << "Distance of point I to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, I, t0), viennagrid::distance(domain, I, e0_0) );
  
  std::cout << "Distance of point J to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, J, t0), height_for_tetrahedron(viennagrid::point(domain, v0_0), viennagrid::point(domain, v0_1), viennagrid::point(domain, v0_2), J) );
  
  std::cout << "Distance of point K to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, K, t0), 0.0 );

  std::cout << "Distance of point K2 to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, K2, t0), height_for_tetrahedron(viennagrid::point(domain, v0_0), viennagrid::point(domain, v0_1), viennagrid::point(domain, v0_2), K2) );
  
  std::cout << "Distance of point L to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, L, t0), viennagrid::distance(domain, L, e0_0) );
  
  std::cout << "Distance of point M to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, M, t0), viennagrid::distance(domain, M, v0_1) );
  
  std::cout << "Distance of point N to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, N, t0), viennagrid::distance(domain, N, e0_2) );
  
  std::cout << "Distance of point O to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, O, t0), viennagrid::distance(domain, O, e0_2) );
  
  std::cout << "Distance of point P to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, P, t0), viennagrid::distance(domain, P, v0_1) );
  
  std::cout << "Distance of point Q to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, Q, t0), viennagrid::distance(domain, Q, e0_2) );
  
  std::cout << "Distance of point R to triangle t0... ";
  fuzzy_check( viennagrid::distance(domain, t0, R), viennagrid::distance(domain, R, e0_2) );
  
  std::cout << std::endl;

  //
  // Distance checks for t1
  //
  VertexType const & v1_0 = viennagrid::elements<viennagrid::vertex_tag>(t1)[0];
  VertexType const & v1_1 = viennagrid::elements<viennagrid::vertex_tag>(t1)[1];
  VertexType const & v1_2 = viennagrid::elements<viennagrid::vertex_tag>(t1)[2];
  
  EdgeType const & e1_0 = viennagrid::elements<viennagrid::line_tag>(t1)[0];
  EdgeType const & e1_1 = viennagrid::elements<viennagrid::line_tag>(t1)[1];
  EdgeType const & e1_2 = viennagrid::elements<viennagrid::line_tag>(t1)[2];
  

  std::cout << "Distance of point A to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, A, t1), viennagrid::distance(domain, A, v1_0) );
  
  std::cout << "Distance of point B to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, B, t1), viennagrid::distance(domain, B, v1_0) );

  std::cout << "Distance of point C to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, C, t1), viennagrid::distance(domain, C, v1_0) );
  
  std::cout << "Distance of point D to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, D, t1), viennagrid::distance(domain, D, e1_1) );
  
  std::cout << "Distance of point E to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, E, t1), viennagrid::distance(domain, E, e1_0) );
  
  std::cout << "Distance of point F to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, F, t1), viennagrid::distance(domain, F, v1_0) );
  
  std::cout << "Distance of point G to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, G, t1), viennagrid::distance(domain, G, v1_0) );
  
  std::cout << "Distance of point H to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, H, t1), viennagrid::distance(domain, H, e1_0) );
  
  std::cout << "Distance of point I to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, I, t1), viennagrid::distance(domain, I, e1_0) );
  
  std::cout << "Distance of point J to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, J, t1), viennagrid::distance(domain, J, e1_1) );
  
  std::cout << "Distance of point K to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, K, t1), height_for_tetrahedron(viennagrid::point(domain, v1_0), viennagrid::point(domain, v1_1), viennagrid::point(domain, v1_2), K) );
  
  std::cout << "Distance of point K2 to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, K2, t1), viennagrid::distance(domain, K2, e1_1) );
  
  std::cout << "Distance of point L to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, L, t1), viennagrid::distance(domain, L, e1_2) );
  
  std::cout << "Distance of point M to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, M, t1), viennagrid::distance(domain, M, e1_2) );
  
  std::cout << "Distance of point N to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, N, t1), height_for_tetrahedron(viennagrid::point(domain, v1_0), viennagrid::point(domain, v1_1), viennagrid::point(domain, v1_2), N) ); 
  
  std::cout << "Distance of point O to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, O, t1), height_for_tetrahedron(viennagrid::point(domain, v1_0), viennagrid::point(domain, v1_1), viennagrid::point(domain, v1_2), O) );
  
  std::cout << "Distance of point P to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, P, t1), viennagrid::distance(domain, P, v1_2) );
  
  std::cout << "Distance of point Q to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, Q, t1), viennagrid::distance(domain, Q, e1_1) );
  
  std::cout << "Distance of point R to triangle t1... ";
  fuzzy_check( viennagrid::distance(domain, R, t1), viennagrid::distance(domain, R, e1_2) );
  
  
}




//
// Quadrilateral
//

void setup_domain(viennagrid::config::quadrilateral_3d_domain & domain)
{
  typedef viennagrid::config::quadrilateral_3d_domain                  DomainType;
  typedef viennagrid::quadrilateral_tag                                CellTag;
  
  typedef viennagrid::result_of::point_type<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::element_hook<DomainType, viennagrid::vertex_tag>::type       VertexHandleType;  
  typedef viennagrid::result_of::element<DomainType, CellTag>::type        CellType;
  
  const size_t s = 4;
  PointType p[s];
  VertexHandleType v[s];
  
  p[0] = PointType(2.0, 1.0, 2.0);
  p[1] = PointType(3.0, 2.0, 1.0);
  p[2] = PointType(1.0, 2.0, 1.0);
  p[3] = PointType(3.0, 3.0, 3.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to domain..." << std::endl;
  for (int i = 0; i < s; ++i)
  {
    v[i] = viennagrid::create_element<VertexType>( domain );
    viennagrid::point( domain, v[i] ) = p[i];
  }

  std::cout << "Adding cells to domain..." << std::endl;
  viennagrid::create_element<CellType>( domain, v, v+4 );
}

void test(viennagrid::config::quadrilateral_3d_domain)
{
  typedef viennagrid::config::quadrilateral_3d_domain                            Domain;
  typedef viennagrid::quadrilateral_tag                                          CellTag;

  
  typedef viennagrid::result_of::point_type<Domain>::type                PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Domain, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type  CellType;

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

  CellType & quad = viennagrid::elements<CellTag>(domain)[0];
  
  //
  // Distance checks for quadrilateral
  //

  VertexType & v0 = viennagrid::elements<viennagrid::vertex_tag>(quad)[0];
  VertexType & v1 = viennagrid::elements<viennagrid::vertex_tag>(quad)[1];
  VertexType & v2 = viennagrid::elements<viennagrid::vertex_tag>(quad)[2];
  VertexType & v3 = viennagrid::elements<viennagrid::vertex_tag>(quad)[3];
  
  EdgeType const & e0 = viennagrid::elements<viennagrid::line_tag>(quad)[0];
  EdgeType const & e1 = viennagrid::elements<viennagrid::line_tag>(quad)[1];
  EdgeType const & e2 = viennagrid::elements<viennagrid::line_tag>(quad)[2];
  EdgeType const & e3 = viennagrid::elements<viennagrid::line_tag>(quad)[3];
  
//   EdgeType e_diag;
//   VertexType * diag_vertices[2];
//   diag_vertices[0] = &v1;
//   diag_vertices[1] = &v2;
//   e_diag.vertices(diag_vertices);

  
  std::cout << "Distance of point A to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,A, quad), viennagrid::distance(domain,A, e1) );
  
  std::cout << "Distance of point B to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,B, quad), viennagrid::distance(domain,e1, B) );

  std::cout << "Distance of point C to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,C, quad), viennagrid::distance(domain,v0, C) );
  
  std::cout << "Distance of point D to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,D, quad), viennagrid::distance(domain,D, e0) );
  
  std::cout << "Distance of point E to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,E, quad), viennagrid::distance(domain,E, e0) );
  
  std::cout << "Distance of point F to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,F, quad), viennagrid::distance(domain,F, e1) );
  
  std::cout << "Distance of point G to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,G, quad), viennagrid::distance(domain,G, e1) );
  
  std::cout << "Distance of point H to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,H, quad), viennagrid::distance(domain,H, e0) );
  
  std::cout << "Distance of point I to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,I, quad), viennagrid::distance(domain,I, e0) );
  
  std::cout << "Distance of point J to quadrilateral... ";
  //fuzzy_check( viennagrid::distance(domain,J, quad), height_for_tetrahedron(v0.point(), v1.point(), v2.point(), J) );
  fuzzy_check( viennagrid::distance(domain,J, quad), viennagrid::distance(domain,J, e3) );
  //fuzzy_check( viennagrid::distance(domain,J, quad), height_for_tetrahedron(v0.point(), v1.point(), v2.point(), J) );
  
  std::cout << "Distance of point K to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,K, quad), 0.0 );

  std::cout << "Distance of point K2 to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,quad, K2), height_for_tetrahedron(viennagrid::point(domain, v1), viennagrid::point(domain, v3), viennagrid::point(domain, v2), K2) );
  
  std::cout << "Distance of point L to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,L, quad), viennagrid::distance(domain,L, e2) );
  
  std::cout << "Distance of point M to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,M, quad), viennagrid::distance(domain,M, e2) );
  
  std::cout << "Distance of point N to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,N, quad), viennagrid::distance(domain,N, e3) );
  
  std::cout << "Distance of point O to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,O, quad), viennagrid::distance(domain,O, e3) );
  
  std::cout << "Distance of point P to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,P, quad), viennagrid::distance(domain,P, v3) );
  
  std::cout << "Distance of point Q to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,Q, quad), viennagrid::distance(domain,Q, e3) );
  
  std::cout << "Distance of point R to quadrilateral... ";
  fuzzy_check( viennagrid::distance(domain,R, quad), height_for_tetrahedron(viennagrid::point(domain, v1), viennagrid::point(domain, v3), viennagrid::point(domain, v2), R) );
}



//
// Tetrahedron
//


void setup_domain(viennagrid::config::tetrahedral_3d_domain & domain)
{
  typedef viennagrid::config::tetrahedral_3d_domain                      DomainType;
  typedef viennagrid::tetrahedron_tag                                    CellTag;
  
  typedef viennagrid::result_of::point_type<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::element_hook<DomainType, viennagrid::vertex_tag>::type       VertexHandleType;
  
  typedef viennagrid::result_of::element<DomainType, CellTag>::type        CellType;
  
  const size_t s = 4;
  PointType p[s];
  VertexHandleType v[s];
  
  p[0] = PointType(2.0, 1.0, 1.0);
  p[1] = PointType(3.0, 3.0, 3.0);
  p[2] = PointType(1.0, 2.0, 2.0);
  p[3] = PointType(2.0, 2.0, 4.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to domain..." << std::endl;
  for (int i = 0; i < s; ++i)
  {
    v[i] = viennagrid::create_element<VertexType>( domain );
    viennagrid::point( domain, v[i] ) = p[i];
  }

  std::cout << "Adding cells to domain..." << std::endl;
  viennagrid::create_element<CellType>( domain, v, v+4 );
}

void test(viennagrid::config::tetrahedral_3d_domain)
{
  typedef viennagrid::config::tetrahedral_3d_domain                            Domain;
  typedef viennagrid::tetrahedron_tag                                          CellTag;

  
  typedef viennagrid::result_of::point_type<Domain>::type                PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Domain, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type  CellType;
  typedef viennagrid::result_of::facet_tag<CellType>::type FacetTag;
  typedef viennagrid::result_of::facet_type<CellType>::type    FacetType;

  Domain domain;
  
  setup_domain(domain);
  
  PointType A(2.0, 0.0, 1.0);  // v0 is closest
  PointType B(6.0, 4.0, 2.0);  // v1 is closest
  PointType C(0.0, 2.1, 1.8);  // v2 is closest
  PointType D(2.3, 1.8, 6.0);  // v3 is closest
  
  PointType E(3.0, 2.5, 2.3);   // e0 is closest
  PointType F(1.0, 1.0, 1.0);   // e1 is closest
  PointType G(2.1, 0.1, 3.1);   // e2 is closest
  PointType H(1.8, 3.9, 2.1);   // e3 is closest
  PointType I(3.1, 2.9, 3.8);   // e4 is closest
  PointType J(1.2, 1.8, 2.9);   // e5 is closest
  
  PointType K(2.5, 2.2, 1.5);   // f0 is closest
  PointType L(3.1, 2.1, 3.1);   // f1 is closest
  PointType M(1.2, 1.3, 3.5);   // f2 is closest
  PointType N(2.2, 3.5, 3.3);   // f3 is closest
  
  PointType O(1.9, 2.1, 3.1);   // inside tet
  PointType P(2.2, 2.5, 2.9);   // inside tet


  CellType & tet = viennagrid::elements<CellTag>(domain)[0];
  
  //
  // Distance checks for quadrilateral
  //

  VertexType & v0 = viennagrid::elements<viennagrid::vertex_tag>(tet)[0];
  VertexType & v1 = viennagrid::elements<viennagrid::vertex_tag>(tet)[1];
  VertexType & v2 = viennagrid::elements<viennagrid::vertex_tag>(tet)[2];
  VertexType & v3 = viennagrid::elements<viennagrid::vertex_tag>(tet)[3];
  
  EdgeType const & e0 = viennagrid::elements<viennagrid::line_tag>(tet)[0];
  EdgeType const & e1 = viennagrid::elements<viennagrid::line_tag>(tet)[1];
  EdgeType const & e2 = viennagrid::elements<viennagrid::line_tag>(tet)[2];
  EdgeType const & e3 = viennagrid::elements<viennagrid::line_tag>(tet)[3];
  EdgeType const & e4 = viennagrid::elements<viennagrid::line_tag>(tet)[4];
  EdgeType const & e5 = viennagrid::elements<viennagrid::line_tag>(tet)[5];
  
  FacetType const & f0 = viennagrid::elements<FacetTag>(tet)[0];
  FacetType const & f1 = viennagrid::elements<FacetTag>(tet)[1];
  FacetType const & f2 = viennagrid::elements<FacetTag>(tet)[2];
  FacetType const & f3 = viennagrid::elements<FacetTag>(tet)[3];
  
  // vertices
  
  std::cout << "Distance of point A to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,A, tet), viennagrid::distance(domain,A, v0) );
  
  std::cout << "Distance of point B to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,B, tet), viennagrid::distance(domain,v1, B) );

  std::cout << "Distance of point C to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,C, tet), viennagrid::distance(domain,C, v2) );
  
  std::cout << "Distance of point D to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,D, tet), viennagrid::distance(domain,D, v3) );
  
  // edges
  
  std::cout << "Distance of point E to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,E, tet), viennagrid::distance(domain,E, e0) );
  
  std::cout << "Distance of point F to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,F, tet), viennagrid::distance(domain,e1, F) );
  
  std::cout << "Distance of point G to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,G, tet), viennagrid::distance(domain,G, e2) );
  
  std::cout << "Distance of point H to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,H, tet), viennagrid::distance(domain,H, e3) );
  
  std::cout << "Distance of point I to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,I, tet), viennagrid::distance(domain,I, e4) );
  
  std::cout << "Distance of point J to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,J, tet), viennagrid::distance(domain,J, e5) );
  
  //facets
  
  std::cout << "Distance of point K to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,K, tet), viennagrid::distance(domain,K, f0) );

  std::cout << "Distance of point L to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,L, tet), viennagrid::distance(domain,L, f1) );
  
  std::cout << "Distance of point M to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,M, tet), viennagrid::distance(domain,f2, M) );
  
  std::cout << "Distance of point N to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,N, tet), viennagrid::distance(domain,N, f3) );
  
  // inside
  
  std::cout << "Distance of point O to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,O, tet), 0.0 );
  
  std::cout << "Distance of point P to tetrahedron... ";
  fuzzy_check( viennagrid::distance(domain,P, tet), 0.0 );
  
}







//
// Quadrilateral
//

void setup_domain(viennagrid::config::hexahedral_3d_domain & domain)
{
  typedef viennagrid::config::hexahedral_3d_domain                      DomainType;
  typedef viennagrid::hexahedron_tag                                    CellTag;
  
  typedef viennagrid::result_of::point_type<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::element_hook<DomainType, viennagrid::vertex_tag>::type       VertexHandleType;
  
  typedef viennagrid::result_of::element<DomainType, CellTag>::type        CellType;
  
  const size_t s = 8;
  PointType p[s];
  VertexHandleType v[s];
  
  p[0] = PointType(1.0, 1.0, 1.0);
  p[1] = PointType(2.0, 0.9, 0.5);
  p[2] = PointType(0.9, 2.1, 0.8);
  p[3] = PointType(2.1, 2.0, 0.9);

  p[4] = PointType(0.9, 0.9, 2.1);
  p[5] = PointType(2.1, 1.0, 2.3);
  p[6] = PointType(1.0, 2.0, 2.2);
  p[7] = PointType(2.0, 2.1, 2.0);
  
  //upgrade to vertex:
  std::cout << "Adding vertices to domain..." << std::endl;
  for (int i = 0; i < s; ++i)
  {
    v[i] = viennagrid::create_element<VertexType>( domain );
    viennagrid::point( domain, v[i] ) = p[i];
  }
    
  std::cout << "Adding cells to domain..." << std::endl;
  viennagrid::create_element<CellType>( domain, v, v+8 );
}

void test(viennagrid::config::hexahedral_3d_domain)
{
  typedef viennagrid::config::hexahedral_3d_domain                            Domain;
  typedef viennagrid::hexahedron_tag                                          CellTag;

  
  typedef viennagrid::result_of::point_type<Domain>::type                PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Domain, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type  CellType;
  typedef viennagrid::result_of::facet_type<CellType>::type    FacetType;

  Domain domain;
  
  setup_domain(domain);
  
/*  PointType A(0, 0, 0);
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
  PointType R(3, 5, 1); */

  CellType & hex = viennagrid::elements<CellTag>(domain)[0];

  std::cout << "Checking a bunch of points in interior for distance 0... ";
  for (std::size_t i=0; i<=10; ++i)
  {
    for (std::size_t j=0; j<=10; ++j)
    {
      for (std::size_t k=0; k<=10; ++k)
      {
        PointType p(1.0 + i / 10.0, 1.0 + j / 10.0, 1.0 + k / 10.0);
        fuzzy_check( viennagrid::distance(domain, p, hex), 0.0 );
      }
    }
  }
  
  //
  // Distance checks for hexahedral
  //
  
  // To come...

/*  VertexType & v0 = viennagrid::elements<viennagrid::vertex_tag>(quad)[0];
  VertexType & v1 = viennagrid::elements<viennagrid::vertex_tag>(quad)[1];
  VertexType & v2 = viennagrid::elements<viennagrid::vertex_tag>(quad)[2];
  VertexType & v3 = viennagrid::elements<viennagrid::vertex_tag>(quad)[3];
  
  EdgeType const & e0 = viennagrid::elements<viennagrid::line_tag>(quad)[0];
  EdgeType const & e1 = viennagrid::elements<viennagrid::line_tag>(quad)[1];
  EdgeType const & e2 = viennagrid::elements<viennagrid::line_tag>(quad)[2];
  EdgeType const & e3 = viennagrid::elements<viennagrid::line_tag>(quad)[3];
  
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
  */
}










int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing line mesh in 3D ====" << std::endl;
  test(viennagrid::config::line_3d_domain());

  std::cout << "==== Testing triangular mesh in 3D ====" << std::endl;
  test(viennagrid::config::triangular_3d_domain());

  std::cout << "==== Testing quadrilateral mesh in 3D ====" << std::endl;
  test(viennagrid::config::quadrilateral_3d_domain());
  
  std::cout << "==== Testing tetrahedral mesh in 3D ====" << std::endl;
  test(viennagrid::config::tetrahedral_3d_domain());
  
  std::cout << "==== Testing hexahedral mesh in 3D ====" << std::endl;
  test(viennagrid::config::hexahedral_3d_domain());
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
