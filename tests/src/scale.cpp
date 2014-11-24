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

#include "viennagrid/core.hpp"
#include "viennagrid/algorithm/geometric_transform.hpp"

inline void fuzzy_check(double a, double b)
{
  if (a < b || a > b)
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

inline void setup_triangular2d_mesh(viennagrid::mesh_t & mesh)
{
  typedef viennagrid::mesh_t                      MeshType;
  typedef viennagrid::triangle_tag                                      CellTag;

  typedef viennagrid::result_of::point<MeshType>::type          PointType;
  typedef viennagrid::result_of::element<MeshType>::type       VertexType;
  typedef viennagrid::result_of::element<MeshType>::type        CellType;

  const size_t s = 4;
  PointType p[s];
  VertexType v[s];

  p[0] = viennagrid::make_point(0.0, 0.0);
  p[1] = viennagrid::make_point(1.0, 0.0);
  p[2] = viennagrid::make_point(1.0, 1.0);
  p[3] = viennagrid::make_point(0.0, 1.0);

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



template<int dimension, typename CellTagT>
void test()
{
  typedef viennagrid::mesh_t                 Mesh;
  typedef viennagrid::result_of::point<Mesh>::type       PointType;


  Mesh mesh;
  viennagrid::result_of::point_accessor<Mesh>::type point_acc(mesh);

  setup_triangular2d_mesh(mesh);

  std::cout << "Scaling domain by a factor of two from origin... " << std::endl;
  viennagrid::scale(mesh, 2.0);
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[0]), viennagrid::make_point(0,0) );
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[1]), viennagrid::make_point(2,0) );
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[2]), viennagrid::make_point(2,2) );
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[3]), viennagrid::make_point(0,2) );


  std::cout << "Scaling domain by a factor of three with respect to (1,1)... " << std::endl;
  viennagrid::scale(mesh, 3.0, viennagrid::make_point(1,1));
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[0]), viennagrid::make_point(-2,-2) );
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[1]), viennagrid::make_point(4,-2) );
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[2]), viennagrid::make_point(4,4) );
  fuzzy_check( point_acc(viennagrid::vertices(mesh)[3]), viennagrid::make_point(-2,4) );
}



int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "==== Testing triangular mesh in 2D ====" << std::endl;
  test<2, viennagrid::triangle_tag>();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
