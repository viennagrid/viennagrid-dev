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

#include "viennagridpp/core.hpp"
#include "viennagridpp/algorithm/distance.hpp"

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
      exit(EXIT_FAILURE);
    }
  }
  std::cout << "PASSED! (" << a << ", " << b << ")" << std::endl;
}


//
// Line 2d
//

inline void setup_line2d_mesh(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 9;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(3.0, 0.0);
  p[1] = viennagrid::make_point(1.0, 1.0);
  p[2] = viennagrid::make_point(5.0, 1.0);
  p[3] = viennagrid::make_point(3.0, 2.0);
  p[4] = viennagrid::make_point(0.0, 3.0);
  p[5] = viennagrid::make_point(3.0, 3.0);
  p[6] = viennagrid::make_point(1.0, 4.0);
  p[7] = viennagrid::make_point(2.0, 1.0);
  p[8] = viennagrid::make_point(3.0, 2.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
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

inline void test_line2d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;
  setup_line2d_mesh(mesh);

  PointType A = viennagrid::make_point(0, 0);
  PointType B = viennagrid::make_point(1, 0);
  PointType C = viennagrid::make_point(2, 0);
  PointType D = viennagrid::make_point(3, 0);
  PointType E = viennagrid::make_point(4, 0);

  PointType F = viennagrid::make_point(0, 1);
  PointType G = viennagrid::make_point(1, 1);
  PointType H = viennagrid::make_point(3, 1);
  PointType I = viennagrid::make_point(4, 1);

  PointType J = viennagrid::make_point(0, 2);
  PointType K = viennagrid::make_point(2, 2);
  PointType L = viennagrid::make_point(5, 2);

  PointType M = viennagrid::make_point(5, 3);

  PointType N = viennagrid::make_point(0, 4);
  PointType O = viennagrid::make_point(1, 4);
  PointType P = viennagrid::make_point(4, 4);

  PointType Q = viennagrid::make_point(2, 5);
  PointType R = viennagrid::make_point(3, 5);

  CellType line0 = viennagrid::cells(mesh)[0];
  CellType line1 = viennagrid::cells(mesh)[1];
  CellType line2 = viennagrid::cells(mesh)[2];
  CellType line3 = viennagrid::cells(mesh)[3];
  CellType line  = viennagrid::cells(mesh)[4];

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

inline void setup_triangular3d_mesh(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                    MeshType;

  typedef viennagrid::result_of::point<MeshType>::type        PointType;
  typedef viennagrid::result_of::element<MeshType>::type      VertexType;

  const size_t s = 4;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(2.0, 1.0);
  p[1] = viennagrid::make_point(3.0, 2.0);
  p[2] = viennagrid::make_point(3.0, 3.0);
  p[3] = viennagrid::make_point(1.0, 2.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
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




inline void test_triangular2d()
{
  typedef viennagrid::mesh                                  Mesh;

  typedef viennagrid::result_of::point<Mesh>::type          PointType;
  typedef viennagrid::result_of::element<Mesh>::type        CellType;

  Mesh mesh;
  setup_triangular3d_mesh(mesh);

  PointType A = viennagrid::make_point(0, 0);
  PointType B = viennagrid::make_point(1, 0);
  PointType C = viennagrid::make_point(2, 0);
  PointType D = viennagrid::make_point(3, 0);
  PointType E = viennagrid::make_point(4, 0);

  PointType F = viennagrid::make_point(0, 1);
  PointType G = viennagrid::make_point(1, 1);
  PointType H = viennagrid::make_point(3, 1);
  PointType I = viennagrid::make_point(4, 1);

  PointType J = viennagrid::make_point(0, 2);
  PointType K = viennagrid::make_point(2, 2);
  PointType L = viennagrid::make_point(5, 2);

  PointType M = viennagrid::make_point(5, 3);

  PointType N = viennagrid::make_point(0, 4);
  PointType O = viennagrid::make_point(1, 4);
  PointType P = viennagrid::make_point(4, 4);

  PointType Q = viennagrid::make_point(2, 5);
  PointType R = viennagrid::make_point(3, 5);

  CellType t0 = viennagrid::cells(mesh)[0];
  CellType t1 = viennagrid::cells(mesh)[1];

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
  fuzzy_check( viennagrid::distance(t0, R), 2.0 );

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

inline void setup_quadrilateral2d_mesh(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                  MeshType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  const size_t s = 4;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(2.0, 1.0);
  p[1] = viennagrid::make_point(3.0, 2.0);
  p[2] = viennagrid::make_point(1.0, 2.0);
  p[3] = viennagrid::make_point(3.0, 3.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
//     viennagrid::point( mesh, v[i] ) = p[i];
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_element<viennagrid::quadrilateral_tag>( mesh, v, v+4 );
}

inline void test_quadrilateral2d()
{
  typedef viennagrid::mesh                              Mesh;

  typedef viennagrid::result_of::point<Mesh>::type      PointType;
  typedef viennagrid::result_of::element<Mesh>::type    CellType;

  Mesh mesh;

  setup_quadrilateral2d_mesh(mesh);

  PointType A = viennagrid::make_point(0, 0);
  PointType B = viennagrid::make_point(1, 0);
  PointType C = viennagrid::make_point(2, 0);
  PointType D = viennagrid::make_point(3, 0);
  PointType E = viennagrid::make_point(4, 0);

  PointType F = viennagrid::make_point(0, 1);
  PointType G = viennagrid::make_point(1, 1);
  PointType H = viennagrid::make_point(3, 1);
  PointType I = viennagrid::make_point(4, 1);

  PointType J = viennagrid::make_point(0, 2);
  PointType K = viennagrid::make_point(2, 2);
  PointType L = viennagrid::make_point(5, 2);

  PointType M = viennagrid::make_point(5, 3);

  PointType N = viennagrid::make_point(0, 4);
  PointType O = viennagrid::make_point(1, 4);
  PointType P = viennagrid::make_point(4, 4);

  PointType Q = viennagrid::make_point(2, 5);
  PointType R = viennagrid::make_point(3, 5);

  CellType quad = viennagrid::cells(mesh)[0];

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
  fuzzy_check( viennagrid::distance(quad, R), 2.0 );

}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing line mesh in 2D ====" << std::endl;
  test_line2d();

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test_triangular2d();

  std::cout << "==== Testing quadrilateral mesh in 2D ====" << std::endl;
  test_quadrilateral2d();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
