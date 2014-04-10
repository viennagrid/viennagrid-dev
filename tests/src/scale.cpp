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
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/algorithm/geometric_transform.hpp"

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

template <typename PointType>
void fuzzy_check(PointType const & a, PointType const & b)
{
  for (std::size_t i=0; i<a.size(); ++i)
    fuzzy_check(a[i], b[i]);
}

//
// Triangular
//

void setup_mesh(viennagrid::triangular_2d_mesh & mesh)
{
  typedef viennagrid::triangular_2d_mesh                      MeshType;
  typedef viennagrid::triangle_tag                                      CellTag;

  typedef viennagrid::result_of::point<MeshType>::type          PointType;
  typedef viennagrid::result_of::handle<MeshType, viennagrid::vertex_tag>::type       VertexHandleType;

  typedef viennagrid::result_of::element<MeshType, CellTag>::type        CellType;

  const size_t s = 4;
  PointType p[s];
  VertexHandleType v[s];

  p[0] = PointType(0.0, 0.0);
  p[1] = PointType(1.0, 0.0);
  p[2] = PointType(1.0, 1.0);
  p[3] = PointType(0.0, 1.0);

  //upgrade to vertex:
  std::cout << "Adding vertices to mesh..." << std::endl;
  for (size_t i = 0; i < s; ++i)
  {
    v[i] = viennagrid::make_vertex( mesh, p[i] );
  }

  std::cout << "Adding cells to mesh..." << std::endl;
  VertexHandleType vertices[3];

  vertices[0] = v[0];
  vertices[1] = v[2];
  vertices[2] = v[3];
  viennagrid::make_element<CellType>( mesh, vertices, vertices+3 );

  vertices[0] = v[0];
  vertices[1] = v[1];
  vertices[2] = v[2];
  viennagrid::make_element<CellType>( mesh, vertices, vertices+3 );
}




void test(viennagrid::triangular_2d_mesh)
{
  typedef viennagrid::triangular_2d_mesh                 Mesh;
  typedef viennagrid::result_of::point<Mesh>::type       PointType;

  viennagrid::result_of::default_point_accessor<Mesh>::type point_acc;
  Mesh mesh;

  setup_mesh(mesh);


  PointType const & p0 = point_acc(viennagrid::elements<viennagrid::vertex_tag>(mesh)[0]);
  PointType const & p1 = point_acc(viennagrid::elements<viennagrid::vertex_tag>(mesh)[1]);
  PointType const & p2 = point_acc(viennagrid::elements<viennagrid::vertex_tag>(mesh)[2]);
  PointType const & p3 = point_acc(viennagrid::elements<viennagrid::vertex_tag>(mesh)[3]);


  std::cout << "Scaling domain by a factor of two from origin... " << std::endl;
  viennagrid::scale(mesh, 2.0);
  fuzzy_check( p0, PointType(0,0) );
  fuzzy_check( p1, PointType(2,0) );
  fuzzy_check( p2, PointType(2,2) );
  fuzzy_check( p3, PointType(0,2) );


  std::cout << "Scaling domain by a factor of three with respect to (1,1)... " << std::endl;
  viennagrid::scale(mesh, 3.0, PointType(1,1));
  fuzzy_check( p0, PointType(-2,-2) );
  fuzzy_check( p1, PointType(4,-2) );
  fuzzy_check( p2, PointType(4,4) );
  fuzzy_check( p3, PointType(-2,4) );

}



int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test(viennagrid::triangular_2d_mesh());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
