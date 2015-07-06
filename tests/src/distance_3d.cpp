/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include <cmath>
#include <iostream>
#include <iomanip>

#include "viennagridpp/core.hpp"
#include "viennagridpp/algorithm/distance.hpp"
#include "viennagridpp/algorithm/volume.hpp"
#include "viennagridpp/algorithm/cross_prod.hpp"
#include "viennagridpp/algorithm/norm.hpp"

inline void fuzzy_check(double a, double b)
{
  if (a > b || a < b)
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

inline double heron_triangle_area(double a, double b, double c)
{
  double s = (a + b + c) / 2.0;

  return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

/* Computes the height on c of a triangle with edge lenghts a, b, c */
inline double height_from_edge_lengths(double a, double b, double c)
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

template <typename MeshType, typename LineType>
double line_distance_via_cross_prod(MeshType const & mesh, LineType const & line0, LineType const & line1)
{
  //typedef typename LineType::config_type     ConfigType;
  //typedef typename viennagrid::result_of::point<ConfigType>::type    PointType;
  typedef typename viennagrid::result_of::point<MeshType>::type PointType;

  PointType A = viennagrid::get_point(mesh, viennagrid::vertices(line0)[0]);
  PointType B = viennagrid::get_point(mesh, viennagrid::vertices(line0)[1]);

  PointType C = viennagrid::get_point(mesh, viennagrid::vertices(line1)[0]);
  PointType D = viennagrid::get_point(mesh, viennagrid::vertices(line1)[1]);

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

inline void setup_line3d(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 9;
  PointType p[s];
  VertexType v[s];


  p[0] = viennagrid::make_point(3.0, 0.0, 1.0);
  p[1] = viennagrid::make_point(1.0, 1.0, 3.0);
  p[2] = viennagrid::make_point(5.0, 1.0, 3.0);
  p[3] = viennagrid::make_point(3.0, 2.0, 2.0);
  p[4] = viennagrid::make_point(0.0, 3.0, 4.0);
  p[5] = viennagrid::make_point(3.0, 3.0, 1.0);
  p[6] = viennagrid::make_point(1.0, 4.0, 3.0);
  p[7] = viennagrid::make_point(2.0, 1.0, 2.0);
  p[8] = viennagrid::make_point(3.0, 2.0, 3.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
//     viennagrid::point( mesh, v[i] ) = p[i];
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  VertexType vertices[2];

  vertices[0] = v[0];
  vertices[1] = v[6];
  viennagrid::make_element<viennagrid::line_tag>( mesh, vertices, vertices+2 );

  vertices[0] = v[1];
  vertices[1] = v[4];
  viennagrid::make_element<viennagrid::line_tag>( mesh, vertices, vertices+2 );

  vertices[0] = v[1];
  vertices[1] = v[5];
  viennagrid::make_element<viennagrid::line_tag>( mesh, vertices, vertices+2 );

  vertices[0] = v[2];
  vertices[1] = v[3];
  viennagrid::make_element<viennagrid::line_tag>( mesh, vertices, vertices+2 );

  vertices[0] = v[7];
  vertices[1] = v[8];
  viennagrid::make_element<viennagrid::line_tag>( mesh, vertices, vertices+2 );
}

inline void test_line3d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;

  setup_line3d(mesh);

  CellType line0 = viennagrid::cells(mesh)[0];
  CellType line1 = viennagrid::cells(mesh)[1];
  CellType line2 = viennagrid::cells(mesh)[2];
  CellType line3 = viennagrid::cells(mesh)[3];
  CellType line  = viennagrid::cells(mesh)[4];
  CellType & line4 = line;

  double line_length = viennagrid::volume(line);

  //
  // Distance checks for points to line
  //

  std::cout << "Distance of point A to line... ";
  PointType A = viennagrid::make_point(0, 0, 0);
  fuzzy_check( viennagrid::distance(A, line), 3.0 );

  std::cout << "Distance of point B to line... ";
  PointType B = viennagrid::make_point(1, 0, 1);
  fuzzy_check( viennagrid::distance(B, line), std::sqrt(3.0) );

  std::cout << "Distance of point C to line... ";
  PointType C = viennagrid::make_point(2, 0, 3);
  fuzzy_check( viennagrid::distance(C, line), std::sqrt(2.0) );

  std::cout << "Distance of point D to line... ";
  PointType D = viennagrid::make_point(3, 0, 2);
  fuzzy_check( viennagrid::distance(D, line),
               height_from_edge_lengths(std::sqrt(2.0), std::sqrt(5.0), line_length) );

  std::cout << "Distance of point E to line... ";
  PointType E = viennagrid::make_point(4, 0, 1);
  fuzzy_check( viennagrid::distance(E, line),
               height_from_edge_lengths(std::sqrt(6.0), std::sqrt(9.0), line_length) );

  std::cout << "Distance of point F to line... ";
  PointType F = viennagrid::make_point(0, 1, 1);
  fuzzy_check( viennagrid::distance(F, line), std::sqrt(5.0) );

  std::cout << "Distance of point G to line... ";
  PointType G = viennagrid::make_point(1, 1, 1);
  fuzzy_check( viennagrid::distance(G, line), std::sqrt(2.0) );

  std::cout << "Distance of point H to line... ";
  PointType H = viennagrid::make_point(3, 1, 2);
  fuzzy_check( viennagrid::distance(H, line),
               height_from_edge_lengths(std::sqrt(1.0), std::sqrt(2.0), line_length) );

  std::cout << "Distance of point I to line... ";
  PointType I = viennagrid::make_point(4, 1, 3);
  fuzzy_check( viennagrid::distance(I, line),
               height_from_edge_lengths(std::sqrt(5.0), std::sqrt(2.0), line_length) );

  std::cout << "Distance of point J to line... ";
  PointType J = viennagrid::make_point(0, 2, 3);
  fuzzy_check( viennagrid::distance(J, line), std::sqrt(6.0) );


  std::cout << "Distance of point K to line... ";
  PointType K = viennagrid::make_point(2, 2, 2);
  fuzzy_check( viennagrid::distance(K, line),
               height_from_edge_lengths(std::sqrt(1.0), std::sqrt(2.0), line_length) );

  std::cout << "Distance of point L to line... ";
  PointType L = viennagrid::make_point(5, 2, 1);
  fuzzy_check( viennagrid::distance(L, line), std::sqrt(8.0) );

  std::cout << "Distance of point M to line... ";
  PointType M = viennagrid::make_point(5, 3, 1);
  fuzzy_check( viennagrid::distance(M, line), std::sqrt(9.0) );

  std::cout << "Distance of point N to line... ";
  PointType N = viennagrid::make_point(0, 4, 2);
  fuzzy_check( viennagrid::distance(N, line),
               height_from_edge_lengths(std::sqrt(13.0), std::sqrt(14.0), line_length) );

  std::cout << "Distance of point O to line... ";
  PointType O = viennagrid::make_point(1, 4, 2);
  fuzzy_check( viennagrid::distance(O, line),
               height_from_edge_lengths(std::sqrt(10.0), std::sqrt(9.0), line_length) );

  std::cout << "Distance of point P to line... ";
  PointType P = viennagrid::make_point(4, 4, 3);
  fuzzy_check( viennagrid::distance(P, line), std::sqrt(5.0) );

  std::cout << "Distance of point Q to line... ";
  PointType Q = viennagrid::make_point(2, 5, 3);
  fuzzy_check( viennagrid::distance(Q, line), std::sqrt(10.0) );

  std::cout << "Distance of point R to line... ";
  PointType R = viennagrid::make_point(3, 5, 4);
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
  double dist_04 = line_distance_via_cross_prod(mesh, line0, line4);
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
  PointType p_24_2 = (1.0 - 3.0/8.0) * viennagrid::get_point(mesh, viennagrid::vertices(line2)[0])
                          + 3.0/8.0  * viennagrid::get_point(mesh, viennagrid::vertices(line2)[1]);
  PointType p_24_4 = (1.0 - 1.0/4.0) * viennagrid::get_point(mesh, viennagrid::vertices(line4)[0])
                          + 1.0/4.0  * viennagrid::get_point(mesh, viennagrid::vertices(line4)[1]);
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
  double dist_34 = line_distance_via_cross_prod(mesh, line3, line4);
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

inline void setup_triangular3d(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 4;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(2.0, 1.0, 2.0);
  p[1] = viennagrid::make_point(3.0, 2.0, 1.0);
  p[2] = viennagrid::make_point(3.0, 3.0, 3.0);
  p[3] = viennagrid::make_point(1.0, 2.0, 1.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
//     viennagrid::point( mesh, v[i] ) = p[i];
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  VertexType vertices[3];

  vertices[0] = v[0];
  vertices[1] = v[2];
  vertices[2] = v[3];
  viennagrid::make_element<viennagrid::triangle_tag>( mesh, vertices, vertices+3 );

  vertices[0] = v[0];
  vertices[1] = v[1];
  vertices[2] = v[2];
  viennagrid::make_element<viennagrid::triangle_tag>( mesh, vertices, vertices+3 );
}




inline void test_triangular3d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    VertexType;
  typedef viennagrid::result_of::element<Mesh>::type    EdgeType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;

  setup_triangular3d(mesh);

  PointType A = viennagrid::make_point(0, 0, 0);
  PointType B = viennagrid::make_point(1, 0, 2);
  PointType C = viennagrid::make_point(2, 0, 3);
  PointType D = viennagrid::make_point(3, 0, 4);
  PointType E = viennagrid::make_point(4, 0, 1);

  PointType F = viennagrid::make_point(0, 1, 1);
  PointType G = viennagrid::make_point(1, 1, 2);
  PointType H = viennagrid::make_point(3, 1, 1);
  PointType I = viennagrid::make_point(4, 1, 0);

  PointType J = viennagrid::make_point(0, 2, 4);
  PointType K = viennagrid::make_point(2, 2, 2);
  PointType K2 = viennagrid::make_point(2, 2, 3);
  PointType L = viennagrid::make_point(5, 2, 2);

  PointType M = viennagrid::make_point(5, 3, 2);

  PointType N = viennagrid::make_point(0, 4, 1);
  PointType O = viennagrid::make_point(1, 4, 2);
  PointType P = viennagrid::make_point(4, 4, 3);

  PointType Q = viennagrid::make_point(2, 5, 2);
  PointType R = viennagrid::make_point(3, 5, 1);

  CellType t0 = viennagrid::cells(mesh)[0];
  CellType t1 = viennagrid::cells(mesh)[1];

  //
  // Distance checks for t0
  //
  VertexType v0_0 = viennagrid::vertices(t0)[0];
  VertexType v0_1 = viennagrid::vertices(t0)[1];
  VertexType v0_2 = viennagrid::vertices(t0)[2];

  EdgeType e0_0 = viennagrid::elements<1>(t0)[0];
  EdgeType e0_1 = viennagrid::elements<1>(t0)[1];
  EdgeType e0_2 = viennagrid::elements<1>(t0)[2];

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
  fuzzy_check( viennagrid::distance(J, t0), height_for_tetrahedron(viennagrid::get_point(mesh, v0_0), viennagrid::get_point(mesh, v0_1), viennagrid::get_point(mesh, v0_2), J) );

  std::cout << "Distance of point K to triangle t0... ";
  fuzzy_check( viennagrid::distance(K, t0), 0.0 );

  std::cout << "Distance of point K2 to triangle t0... ";
  fuzzy_check( viennagrid::distance(K2, t0), height_for_tetrahedron(viennagrid::get_point(mesh, v0_0), viennagrid::get_point(mesh, v0_1), viennagrid::get_point(mesh, v0_2), K2) );

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
  VertexType v1_0 = viennagrid::vertices(t1)[0];
  VertexType v1_1 = viennagrid::vertices(t1)[1];
  VertexType v1_2 = viennagrid::vertices(t1)[2];

  EdgeType e1_0 = viennagrid::elements<1>(t1)[0];
  EdgeType e1_1 = viennagrid::elements<1>(t1)[1];
  EdgeType e1_2 = viennagrid::elements<1>(t1)[2];


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
  fuzzy_check( viennagrid::distance(K, t1), height_for_tetrahedron(viennagrid::get_point(mesh, v1_0), viennagrid::get_point(mesh, v1_1), viennagrid::get_point(mesh, v1_2), K) );

  std::cout << "Distance of point K2 to triangle t1... ";
  fuzzy_check( viennagrid::distance(K2, t1), viennagrid::distance(K2, e1_1) );

  std::cout << "Distance of point L to triangle t1... ";
  fuzzy_check( viennagrid::distance(L, t1), viennagrid::distance(L, e1_2) );

  std::cout << "Distance of point M to triangle t1... ";
  fuzzy_check( viennagrid::distance(M, t1), viennagrid::distance(M, e1_2) );

  std::cout << "Distance of point N to triangle t1... ";
  fuzzy_check( viennagrid::distance(N, t1), height_for_tetrahedron(viennagrid::get_point(mesh, v1_0), viennagrid::get_point(mesh, v1_1), viennagrid::get_point(mesh, v1_2), N) );

  std::cout << "Distance of point O to triangle t1... ";
  fuzzy_check( viennagrid::distance(O, t1), height_for_tetrahedron(viennagrid::get_point(mesh, v1_0), viennagrid::get_point(mesh, v1_1), viennagrid::get_point(mesh, v1_2), O) );

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

inline void setup_quadrilateral3d(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 4;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(2.0, 1.0, 2.0);
  p[1] = viennagrid::make_point(3.0, 2.0, 1.0);
  p[2] = viennagrid::make_point(1.0, 2.0, 1.0);
  p[3] = viennagrid::make_point(3.0, 3.0, 3.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh,p[i] );
//     viennagrid::point( mesh, v[i] ) = p[i];
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_element<viennagrid::quadrilateral_tag>( mesh, v, v+4 );
}

inline void test_quadrilateral3d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    ElementType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;

  setup_quadrilateral3d(mesh);

  PointType A = viennagrid::make_point(0, 0, 0);
  PointType B = viennagrid::make_point(1, 0, 2);
  PointType C = viennagrid::make_point(2, 0, 3);
  PointType D = viennagrid::make_point(3, 0, 4);
  PointType E = viennagrid::make_point(4, 0, 1);

  PointType F = viennagrid::make_point(0, 1, 1);
  PointType G = viennagrid::make_point(1, 1, 2);
  PointType H = viennagrid::make_point(3, 1, 1);
  PointType I = viennagrid::make_point(4, 1, 0);

  PointType J = viennagrid::make_point(0, 2, 4);
  PointType K = viennagrid::make_point(2, 2, 1);
  PointType K2 = viennagrid::make_point(2, 2, 2);
  PointType L = viennagrid::make_point(5, 2, 2);

  PointType M = viennagrid::make_point(5, 3, 2);

  PointType N = viennagrid::make_point(0, 4, 1);
  PointType O = viennagrid::make_point(1, 4, 2);
  PointType P = viennagrid::make_point(4, 4, 3);

  PointType Q = viennagrid::make_point(2, 5, 2);
  PointType R = viennagrid::make_point(3, 5, 1);

  CellType quad = viennagrid::cells(mesh)[0];

  //
  // Distance checks for quadrilateral
  //

  ElementType v0 = viennagrid::vertices(quad)[0];
  ElementType v1 = viennagrid::vertices(quad)[1];
  ElementType v2 = viennagrid::vertices(quad)[2];
  ElementType v3 = viennagrid::vertices(quad)[3];

  ElementType e0 = viennagrid::elements<1>(quad)[0];
  ElementType e1 = viennagrid::elements<1>(quad)[1];
  ElementType e2 = viennagrid::elements<1>(quad)[2];
  ElementType e3 = viennagrid::elements<1>(quad)[3];


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
  fuzzy_check( viennagrid::distance(quad, K2), height_for_tetrahedron(viennagrid::get_point(mesh, v1), viennagrid::get_point(mesh, v3), viennagrid::get_point(mesh, v2), K2) );

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
  fuzzy_check( viennagrid::distance(R, quad), height_for_tetrahedron(viennagrid::get_point(mesh, v1), viennagrid::get_point(mesh, v3), viennagrid::get_point(mesh, v2), R) );
}



//
// Tetrahedron
//


inline void setup_tetrahedral3d(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 4;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(2.0, 1.0, 1.0);
  p[1] = viennagrid::make_point(3.0, 3.0, 3.0);
  p[2] = viennagrid::make_point(1.0, 2.0, 2.0);
  p[3] = viennagrid::make_point(2.0, 2.0, 4.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
//     viennagrid::point( mesh, v[i] ) = p[i];
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_element<viennagrid::tetrahedron_tag>( mesh, v, v+4 );
}

inline void test_tetrahedral3d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    ElementType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;

  setup_tetrahedral3d(mesh);

  PointType A = viennagrid::make_point(2.0, 0.0, 1.0);  // v0 is closest
  PointType B = viennagrid::make_point(6.0, 4.0, 2.0);  // v1 is closest
  PointType C = viennagrid::make_point(0.0, 2.1, 1.8);  // v2 is closest
  PointType D = viennagrid::make_point(2.3, 1.8, 6.0);  // v3 is closest

  PointType E = viennagrid::make_point(3.0, 2.5, 2.3);   // e0 is closest
  PointType F = viennagrid::make_point(1.0, 1.0, 1.0);   // e1 is closest
  PointType G = viennagrid::make_point(2.1, 0.1, 3.1);   // e2 is closest
  PointType H = viennagrid::make_point(1.8, 3.9, 2.1);   // e3 is closest
  PointType I = viennagrid::make_point(3.1, 2.9, 3.8);   // e4 is closest
  PointType J = viennagrid::make_point(1.2, 1.8, 2.9);   // e5 is closest

  PointType K = viennagrid::make_point(2.5, 2.2, 1.5);   // f0 is closest
  PointType L = viennagrid::make_point(3.1, 2.1, 3.1);   // f1 is closest
  PointType M = viennagrid::make_point(1.2, 1.3, 3.5);   // f2 is closest
  PointType N = viennagrid::make_point(2.2, 3.5, 3.3);   // f3 is closest

  PointType O = viennagrid::make_point(1.9, 2.1, 3.1);   // inside tet
  PointType P = viennagrid::make_point(2.2, 2.5, 2.9);   // inside tet


  CellType tet = viennagrid::cells(mesh)[0];

  //
  // Distance checks for quadrilateral
  //

  ElementType v0 = viennagrid::vertices(tet)[0];
  ElementType v1 = viennagrid::vertices(tet)[1];
  ElementType v2 = viennagrid::vertices(tet)[2];
  ElementType v3 = viennagrid::vertices(tet)[3];

  ElementType e0 = viennagrid::elements<1>(tet)[0];
  ElementType e1 = viennagrid::elements<1>(tet)[1];
  ElementType e2 = viennagrid::elements<1>(tet)[2];
  ElementType e3 = viennagrid::elements<1>(tet)[3];
  ElementType e4 = viennagrid::elements<1>(tet)[4];
  ElementType e5 = viennagrid::elements<1>(tet)[5];

  ElementType f0 = viennagrid::facets(tet)[0];
  ElementType f1 = viennagrid::facets(tet)[1];
  ElementType f2 = viennagrid::facets(tet)[2];
  ElementType f3 = viennagrid::facets(tet)[3];

  // vertices

  std::cout << "Distance of point A to tetrahedron... ";
  fuzzy_check( viennagrid::distance(A, tet), viennagrid::distance(A, v0) );

  std::cout << "Distance of point B to tetrahedron... ";
  fuzzy_check( viennagrid::distance(B, tet), viennagrid::distance(v1, B) );

  std::cout << "Distance of point C to tetrahedron... ";
  fuzzy_check( viennagrid::distance(C, tet), viennagrid::distance(C, v2) );

  std::cout << "Distance of point D to tetrahedron... ";
  fuzzy_check( viennagrid::distance(D, tet), viennagrid::distance(D, v3) );

  // edges

  std::cout << "Distance of point E to tetrahedron... ";
  fuzzy_check( viennagrid::distance(E, tet), viennagrid::distance(E, e0) );

  std::cout << "Distance of point F to tetrahedron... ";
  fuzzy_check( viennagrid::distance(F, tet), viennagrid::distance(e1, F) );

  std::cout << "Distance of point G to tetrahedron... ";
  fuzzy_check( viennagrid::distance(G, tet), viennagrid::distance(G, e2) );

  std::cout << "Distance of point H to tetrahedron... ";
  fuzzy_check( viennagrid::distance(H, tet), viennagrid::distance(H, e3) );

  std::cout << "Distance of point I to tetrahedron... ";
  fuzzy_check( viennagrid::distance(I, tet), viennagrid::distance(I, e4) );

  std::cout << "Distance of point J to tetrahedron... ";
  fuzzy_check( viennagrid::distance(J, tet), viennagrid::distance(J, e5) );

  //facets

  std::cout << "Distance of point K to tetrahedron... ";
  fuzzy_check( viennagrid::distance(K, tet), viennagrid::distance(K, f0) );

  std::cout << "Distance of point L to tetrahedron... ";
  fuzzy_check( viennagrid::distance(L, tet), viennagrid::distance(L, f1) );

  std::cout << "Distance of point M to tetrahedron... ";
  fuzzy_check( viennagrid::distance(M, tet), viennagrid::distance(f2, M) );

  std::cout << "Distance of point N to tetrahedron... ";
  fuzzy_check( viennagrid::distance(N, tet), viennagrid::distance(N, f3) );

  // inside

  std::cout << "Distance of point O to tetrahedron... ";
  fuzzy_check( viennagrid::distance(O, tet), 0.0 );

  std::cout << "Distance of point P to tetrahedron... ";
  fuzzy_check( viennagrid::distance(P, tet), 0.0 );

}







//
// Quadrilateral
//

inline void setup_hexahedral3d(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 8;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(1.0, 1.0, 1.0);
  p[1] = viennagrid::make_point(2.0, 0.9, 0.5);
  p[2] = viennagrid::make_point(0.9, 2.1, 0.8);
  p[3] = viennagrid::make_point(2.1, 2.0, 0.9);

  p[4] = viennagrid::make_point(0.9, 0.9, 2.1);
  p[5] = viennagrid::make_point(2.1, 1.0, 2.3);
  p[6] = viennagrid::make_point(1.0, 2.0, 2.2);
  p[7] = viennagrid::make_point(2.0, 2.1, 2.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_element<viennagrid::hexahedron_tag>( mesh, v, v+8 );
}

inline void test_hexahedral3d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;

  setup_hexahedral3d(mesh);

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

  CellType hex = viennagrid::cells(mesh)[0];

  std::cout << "Checking a bunch of points in interior for distance 0... ";
  for (std::size_t i=0; i<=10; ++i)
  {
    for (std::size_t j=0; j<=10; ++j)
    {
      for (std::size_t k=0; k<=10; ++k)
      {
        PointType p = viennagrid::make_point(1.0 + i / 10.0, 1.0 + j / 10.0, 1.0 + k / 10.0);
        fuzzy_check( viennagrid::distance(p, hex), 0.0 );
      }
    }
  }

  //
  // Distance checks for hexahedral
  //

  // To come...

/*  VertexType & v0 = viennagrid::vertices(quad)[0];
  VertexType & v1 = viennagrid::vertices(quad)[1];
  VertexType & v2 = viennagrid::vertices(quad)[2];
  VertexType & v3 = viennagrid::vertices(quad)[3];

  EdgeType const & e0 = viennagrid::elements<1>(quad)[0];
  EdgeType const & e1 = viennagrid::elements<1>(quad)[1];
  EdgeType const & e2 = viennagrid::elements<1>(quad)[2];
  EdgeType const & e3 = viennagrid::elements<1>(quad)[3];

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
  test_line3d();

  std::cout << "==== Testing triangular mesh in 3D ====" << std::endl;
  test_triangular3d();

  std::cout << "==== Testing quadrilateral mesh in 3D ====" << std::endl;
  test_quadrilateral3d();

  std::cout << "==== Testing tetrahedral mesh in 3D ====" << std::endl;
  test_tetrahedral3d();

  std::cout << "==== Testing hexahedral mesh in 3D ====" << std::endl;
  test_hexahedral3d();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
