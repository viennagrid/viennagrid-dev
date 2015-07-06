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
// Triangular
//

inline void setup_meshs(viennagrid::mesh & mesh0,
                        viennagrid::mesh & mesh1)
{
  typedef viennagrid::mesh                                  MeshType;
  typedef viennagrid::result_of::region<MeshType>::type     RegionType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    VertexType;

  // Mesh d0
  {
    const size_t s = 15;
    PointType p[s];
    VertexType v[s];

    p[0] = viennagrid::make_point(0.0, 0.0);
    p[1] = viennagrid::make_point(1.0, 0.0);
    p[2] = viennagrid::make_point(2.0, 0.0);

    p[3] = viennagrid::make_point(0.0, 1.0);
    p[4] = viennagrid::make_point(1.0, 1.0);
    p[5] = viennagrid::make_point(2.0, 1.0);

    p[6] = viennagrid::make_point(0.0, 2.0);
    p[7] = viennagrid::make_point(1.0, 2.0);
    p[8] = viennagrid::make_point(2.0, 2.0);

    p[9] = viennagrid::make_point(0.0, 3.0);
    p[10] = viennagrid::make_point(1.0, 3.0);
    p[11] = viennagrid::make_point(2.0, 3.0);

    p[12] = viennagrid::make_point(0.0, 4.0);
    p[13] = viennagrid::make_point(1.0, 4.0);
    p[14] = viennagrid::make_point(2.0, 4.0);

    //upgrade to vertex:
    std::cout << "Adding vertices to mesh..." << std::endl;
    for (size_t i = 0; i < s; ++i)
    {
        v[i] = viennagrid::make_vertex( mesh0, p[i] );
//         viennagrid::point( mesh0, v[i] ) = p[i];
    }


    std::cout << "Adding cells to mesh..." << std::endl;
    VertexType vertices[3];


    RegionType region0 = mesh0.create_region();
    RegionType region1 = mesh0.create_region();

    //segment 0:
    vertices[0] = v[0];
    vertices[1] = v[1];
    vertices[2] = v[3];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[1];
    vertices[1] = v[4];
    vertices[2] = v[3];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[1];
    vertices[1] = v[2];
    vertices[2] = v[4];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[2];
    vertices[1] = v[5];
    vertices[2] = v[4];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[3];
    vertices[1] = v[4];
    vertices[2] = v[6];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[4];
    vertices[1] = v[7];
    vertices[2] = v[6];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[4];
    vertices[1] = v[5];
    vertices[2] = v[7];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[5];
    vertices[1] = v[8];
    vertices[2] = v[7];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    // segment 1:

    vertices[0] = v[9];
    vertices[1] = v[10];
    vertices[2] = v[12];
    viennagrid::make_element<viennagrid::triangle_tag>( region1, vertices, vertices+3 );

    vertices[0] = v[10];
    vertices[1] = v[13];
    vertices[2] = v[12];
    viennagrid::make_element<viennagrid::triangle_tag>( region1, vertices, vertices+3 );

    vertices[0] = v[10];
    vertices[1] = v[11];
    vertices[2] = v[13];
    viennagrid::make_element<viennagrid::triangle_tag>( region1, vertices, vertices+3 );

    vertices[0] = v[11];
    vertices[1] = v[14];
    vertices[2] = v[13];
    viennagrid::make_element<viennagrid::triangle_tag>( region1, vertices, vertices+3 );
  }


  // mesh d1:
  {
    const size_t s = 6;
    PointType p[s];
    VertexType v[s];

    p[0] = viennagrid::make_point(3.0, 0.0);
    p[1] = viennagrid::make_point(4.0, 0.0);

    p[2] = viennagrid::make_point(3.0, 1.0);
    p[3] = viennagrid::make_point(4.0, 1.0);

    p[4] = viennagrid::make_point(3.0, 2.0);
    p[5] = viennagrid::make_point(4.0, 2.0);

    //upgrade to vertex:
    std::cout << "Adding vertices to mesh..." << std::endl;
    for (size_t i = 0; i < s; ++i)
    {
        v[i] = viennagrid::make_vertex( mesh1, p[i] );
//         viennagrid::point( mesh1, v[i] ) = p[i];
    }


    std::cout << "Adding cells to mesh..." << std::endl;
    VertexType vertices[3];

    RegionType region0 = mesh1.create_region();

    //segment 0:

    vertices[0] = v[0];
    vertices[1] = v[1];
    vertices[2] = v[2];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[1];
    vertices[1] = v[3];
    vertices[2] = v[2];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[2];
    vertices[1] = v[3];
    vertices[2] = v[4];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );

    vertices[0] = v[3];
    vertices[1] = v[5];
    vertices[2] = v[4];
    viennagrid::make_element<viennagrid::triangle_tag>( region0, vertices, vertices+3 );
  }

}




inline void test()
{
  typedef viennagrid::mesh                                Mesh;

  typedef viennagrid::result_of::point<Mesh>::type        PointType;

  Mesh mesh0;
  Mesh mesh1;

  setup_meshs(mesh0, mesh1);

  PointType A = viennagrid::make_point(-1.0, -1.0);


  /*CellType & t0_d0 =*/ viennagrid::cells(mesh0)[0];
  /*CellType & t8_d0 =*/ viennagrid::cells(mesh0)[8];
  /*CellType & t1_d1 =*/ viennagrid::cells(mesh1)[1];

  //
  // Distance checks
  //

  // point to segment/mesh

  std::cout << "Distance of point A to segment0 in mesh0... ";
  fuzzy_check( viennagrid::boundary_distance(A, mesh0.get_region(0)), std::sqrt(2.0) );

  std::cout << "Distance of point A to segment1 in mesh0... ";
  fuzzy_check( viennagrid::boundary_distance(A, mesh0.get_region(1)), std::sqrt(17.0) );

  std::cout << "Distance of point A to segment0 in mesh1... ";
  fuzzy_check( viennagrid::boundary_distance(A, mesh1.get_region(0)),  std::sqrt(17.0) );

  std::cout << "Distance of point A to mesh0... ";
  fuzzy_check( viennagrid::boundary_distance(A, mesh0),  std::sqrt(2.0) );

  std::cout << "Distance of point A to mesh1... ";
  fuzzy_check( viennagrid::boundary_distance(A, mesh1),  std::sqrt(17.0) );

  // triangle to segment/mesh

//   std::cout << "Boundary distance of triangle 0 in mesh0 to segment0 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t0_d0, seg0),  0.0 );
//
//   std::cout << "Boundary distance of triangle 0 in mesh0 to segment1 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t0_d0, seg1),  2.0 );
//
//   std::cout << "Boundary distance of triangle 0 in mesh0 to segment0 in mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh1, t0_d0, seg0),  2.0 );
//
//   std::cout << "Boundary distance of triangle 0 in mesh0 to mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t0_d0, mesh0),  0.0 );
//
//   std::cout << "Boundary distance of triangle 0 in mesh0 to mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh1, t0_d0, mesh1),  2.0 );
//
//
//   std::cout << "Boundary distance of triangle 8 in mesh0 to segment0 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t8_d0, seg0),  1.0 );
//
//   std::cout << "Boundary distance of triangle 8 in mesh0 to segment1 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t8_d0, seg1),  0.0 );
//
//   std::cout << "Boundary distance of triangle 8 in mesh0 to segment0 in mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh1, t8_d0, seg0),  std::sqrt(5.0) );
//
//   std::cout << "Boundary distance of triangle 8 in mesh0 to mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t8_d0, mesh0),  0.0 );
//
//   std::cout << "Boundary distance of triangle 8 in mesh0 to mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh1, t8_d0, mesh1),  std::sqrt(5.0) );
//
//
//   std::cout << "Boundary distance of triangle 1 in mesh1 to segment0 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t1_d1, seg0),  1.0 );
//
//   std::cout << "Boundary distance of triangle 1 in mesh1 to segment1 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t1_d1, seg1),  std::sqrt(5.0) );
//
//   std::cout << "Boundary distance of triangle 1 in mesh1 to segment0 in mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh1, t1_d1, seg0),  0.0 );
//
//   std::cout << "Boundary distance of triangle 1 in mesh1 to mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, t1_d1, mesh0),  1.0 );
//
//   std::cout << "Boundary distance of triangle 1 in mesh1 to mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh1, t1_d1, mesh1),  0.0 );


  // Distances between meshs and segments

//   std::cout << "Boundary distance of segment0 to segment1 in mesh0... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, seg0, seg1),  1.0 );
//
//   std::cout << "Boundary distance of segment1 in mesh0 to segment0 in mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(seg1, seg0),  std::sqrt(2.0) );
//
//   std::cout << "Boundary distance of mesh0 to segment0 in mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, seg0),  1.0 );
//
//   std::cout << "Boundary distance of mesh0 to mesh1... ";
//   fuzzy_check( viennagrid::boundary_distance(mesh0, mesh1),  1.0 );

}




int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
