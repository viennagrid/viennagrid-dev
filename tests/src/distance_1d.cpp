/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
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

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
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
// Line 2d
//

void setup_mesh(viennagrid::line_1d_mesh & mesh)
{
  typedef viennagrid::line_1d_mesh                      MeshType;
  typedef viennagrid::line_tag                                    CellTag;

  typedef viennagrid::result_of::point<MeshType>::type          PointType;
  typedef viennagrid::result_of::handle<MeshType, viennagrid::vertex_tag>::type       VertexHandleType;
  typedef viennagrid::result_of::element<MeshType, CellTag>::type        CellType;

  const size_t s = 6;
  PointType p[s];
  VertexHandleType v[s];

  p[0] = PointType(2.0);
  p[1] = PointType(3.0);
  p[2] = PointType(5.0);
  p[3] = PointType(6.0);
  p[4] = PointType(2.5);
  p[5] = PointType(5.5);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_element<CellType>( mesh, v, v+2 );
  viennagrid::make_element<CellType>( mesh, v+2, v+4 );
  viennagrid::make_element<CellType>( mesh, v+4, v+6 );
}

void test(viennagrid::line_1d_mesh)
{
  typedef viennagrid::line_1d_mesh                            Mesh;
  typedef viennagrid::line_tag                                          CellTag;

  typedef viennagrid::result_of::point<Mesh>::type                PointType;
  typedef viennagrid::result_of::element<Mesh, CellTag>::type  CellType;

  Mesh mesh;

  setup_mesh(mesh);

  PointType A(0);
  PointType B(1.0);
  PointType C(1.5);
  PointType D(2.0);
  PointType E(2.5);
  PointType F(3.0);
  PointType G(3.5);
  PointType H(4.0);

  CellType line0 = viennagrid::elements<CellTag>(mesh)[0];
  CellType line1 = viennagrid::elements<CellTag>(mesh)[1];
  CellType line2 = viennagrid::elements<CellTag>(mesh)[2];

  //
  // Distance checks for t0
  //

  std::cout << "Distance of point A to line0... ";
  fuzzy_check( viennagrid::distance(A, line0), 2.0 );

  std::cout << "Distance of point B to line0... ";
  fuzzy_check( viennagrid::distance(B, line0), 1.0 );

  std::cout << "Distance of point C to line0... ";
  fuzzy_check( viennagrid::distance(C, line0), 0.5 );

  std::cout << "Distance of point D to line0... ";
  fuzzy_check( viennagrid::distance(D, line0), 0 );

  std::cout << "Distance of point E to line0... ";
  fuzzy_check( viennagrid::distance(E, line0), 0 );

  std::cout << "Distance of point F to line0... ";
  fuzzy_check( viennagrid::distance(F, line0), 0 );

  std::cout << "Distance of point G to line0... ";
  fuzzy_check( viennagrid::distance(G, line0), 0.5 );

  std::cout << "Distance of point H to line0... ";
  fuzzy_check( viennagrid::distance(H, line0), 1.0 );

  std::cout << "Distance of point H to line0... ";
  fuzzy_check( viennagrid::distance(H, line0), 1.0 );

  std::cout << std::endl;

  //
  // Distance line to line
  //

  std::cout << "Distance of line0 to line0... ";
  fuzzy_check( viennagrid::distance(line0, line0), 0.0 );

  std::cout << "Distance of line0 to line1... ";
  fuzzy_check( viennagrid::distance(line0, line1), 2.0 );

  std::cout << "Distance of line0 to line2... ";
  fuzzy_check( viennagrid::distance(line0, line2), 0.0 );


  std::cout << "Distance of line1 to line0... ";
  fuzzy_check( viennagrid::distance(line1, line0), 2.0 );

  std::cout << "Distance of line1 to line1... ";
  fuzzy_check( viennagrid::distance(line1, line1), 0.0 );

  std::cout << "Distance of line1 to line2... ";
  fuzzy_check( viennagrid::distance(line1, line2), 0.0 );


  std::cout << "Distance of line1 to line0... ";
  fuzzy_check( viennagrid::distance(line2, line0), 0.0 );

  std::cout << "Distance of line1 to line1... ";
  fuzzy_check( viennagrid::distance(line2, line1), 0.0 );

  std::cout << "Distance of line1 to line2... ";
  fuzzy_check( viennagrid::distance(line2, line2), 0.0 );


}



int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing line mesh in 1D ====" << std::endl;
  test(viennagrid::line_1d_mesh());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
