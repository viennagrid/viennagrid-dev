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
// Triangular
//

void setup_meshs(viennagrid::triangular_2d_mesh & mesh0,
                   viennagrid::triangular_2d_segmentation & segmentation0,
                   viennagrid::triangular_2d_mesh & mesh1,
                   viennagrid::triangular_2d_segmentation & segmentation1)
{
  typedef viennagrid::triangular_2d_mesh                      MeshType;
  typedef viennagrid::triangular_2d_segment_handle                     SegmentHandleType;
  typedef viennagrid::triangle_tag                                      CellTag;
  
  typedef viennagrid::result_of::point<MeshType>::type          PointType;
  typedef viennagrid::result_of::element<MeshType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::handle<MeshType, viennagrid::vertex_tag>::type       VertexHandleType;
  
  typedef viennagrid::result_of::element<MeshType, CellTag>::type        CellType;
  
  // Mesh d0
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
    std::cout << "Adding vertices to mesh..." << std::endl;
    for (size_t i = 0; i < s; ++i)
    {
        v[i] = viennagrid::make_vertex( mesh0, p[i] );
//         viennagrid::point( mesh0, v[i] ) = p[i];
    }

    
    std::cout << "Adding cells to mesh..." << std::endl;
    VertexHandleType vertices[3];

    
    SegmentHandleType seg0 = segmentation0.make_segment();
    SegmentHandleType seg1 = segmentation0.make_segment();
    
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
  
  
  // mesh d1:
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
    std::cout << "Adding vertices to mesh..." << std::endl;
    for (size_t i = 0; i < s; ++i)
    {
        v[i] = viennagrid::make_vertex( mesh1, p[i] );
//         viennagrid::point( mesh1, v[i] ) = p[i];
    }

    
    std::cout << "Adding cells to mesh..." << std::endl;
    VertexHandleType vertices[3];
    
    SegmentHandleType seg0 = segmentation1.make_segment();
    
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




void test(viennagrid::triangular_2d_mesh)
{
  typedef viennagrid::triangular_2d_mesh                      Mesh;
  typedef viennagrid::triangular_2d_segmentation                Segmentation;
  typedef viennagrid::triangular_2d_segment_handle                     Segment;
  typedef viennagrid::triangle_tag                                      CellTag;
  
  typedef viennagrid::result_of::point<Mesh>::type                PointType;
  typedef viennagrid::result_of::element<Mesh, viennagrid::vertex_tag>::type             VertexType;
  typedef viennagrid::result_of::element<Mesh, viennagrid::line_tag>::type             EdgeType;
  typedef viennagrid::result_of::element<Mesh, CellTag>::type  CellType;

  Mesh mesh0;
  Segmentation segmentation0(mesh0);
  Mesh mesh1;
  Segmentation segmentation1(mesh1);
  
  setup_meshs(mesh0, segmentation0, mesh1, segmentation1);
  
  PointType A(-1.0, -1.0);
  

  /*CellType & t0_d0 =*/ viennagrid::elements<CellTag>(mesh0)[0];
  /*CellType & t8_d0 =*/ viennagrid::elements<CellTag>(mesh0)[8];
  /*CellType & t1_d1 =*/ viennagrid::elements<CellTag>(mesh1)[1];
  
  //
  // Distance checks
  //

  // point to segment/mesh 
  
  std::cout << "Distance of point A to segment0 in mesh0... ";
  fuzzy_check( viennagrid::boundary_distance(A, segmentation0(0)), std::sqrt(2.0) );
  
  std::cout << "Distance of point A to segment1 in mesh0... ";
  fuzzy_check( viennagrid::boundary_distance(A, segmentation0(1)), std::sqrt(17.0) );

  std::cout << "Distance of point A to segment0 in mesh1... ";
  fuzzy_check( viennagrid::boundary_distance(A, segmentation1(0)),  std::sqrt(17.0) );
  
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
  test(viennagrid::triangular_2d_mesh());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
