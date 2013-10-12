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

#include <iostream>
#include <vector>

#include "viennagrid/config/default_configs.hpp"
#include "voronoi_common.hpp"
#include "refinement-common.hpp"

//
// Generate a ring of six triangles:
//
template <typename MeshType>
void setup_mesh(MeshType & mesh)
{
  typedef typename viennagrid::result_of::vertex_handle<MeshType>::type                         VertexHandleType;
  typedef typename viennagrid::result_of::point<MeshType>::type   PointType;


  //
  // Step 1: Set up vertices:
  //

  VertexHandleType vh[10];

  vh[0] = viennagrid::make_vertex( mesh, PointType(0, 0) );
  vh[1] = viennagrid::make_vertex( mesh, PointType(2, 1) );
  vh[2] = viennagrid::make_vertex( mesh, PointType(1, 2) );
  vh[3] = viennagrid::make_vertex( mesh, PointType(-1, 2) );
  vh[4] = viennagrid::make_vertex( mesh, PointType(-2, 1) );
  vh[5] = viennagrid::make_vertex( mesh, PointType(-2, -1) );
  vh[6] = viennagrid::make_vertex( mesh, PointType(-1, -2) );
  vh[7] = viennagrid::make_vertex( mesh, PointType(1, -2) );
  vh[8] = viennagrid::make_vertex( mesh, PointType(2, -1) );
  vh[9] = viennagrid::make_vertex( mesh, PointType(1.3, 2.7) ); // for special cell with circumcenter outside:


  //
  // Step 2: Set up cells:
  //

  viennagrid::make_triangle( mesh, vh[8], vh[1], vh[0] );

  for (size_t i=1; i<8; ++i)
    viennagrid::make_triangle( mesh, vh[i], vh[i+1], vh[0] );

  viennagrid::make_triangle( mesh, vh[2], vh[1], vh[9] );
}

int main()
{
  typedef viennagrid::triangular_2d_mesh           MeshType;

  std::cout << "* main(): Creating mesh..." << std::endl;
  MeshType mesh;

  //create mesh:
  setup_mesh(mesh);

  typedef viennagrid::result_of::vertex<MeshType>::type    VertexType;
  typedef viennagrid::result_of::line<MeshType>::type    EdgeType;
  typedef viennagrid::result_of::cell<MeshType>::type    CellType;
  typedef viennagrid::result_of::const_cell_handle<MeshType>::type    ConstCellHandleType;

  std::deque<double> interface_areas;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > interface_contributions;

  std::deque<double> vertex_box_volumes;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > vertex_box_volume_contributions;

  std::deque<double> edge_box_volumes;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > edge_box_volume_contributions;


  //set up dual grid info:
  viennagrid::apply_voronoi<CellType>(
          mesh,
          viennagrid::make_field<EdgeType>(interface_areas),
          viennagrid::make_field<EdgeType>(interface_contributions),
          viennagrid::make_field<VertexType>(vertex_box_volumes),
          viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_field<EdgeType>(edge_box_volumes),
          viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
  );

  //output results:
  output_voronoi_info(mesh,
                      viennagrid::make_field<VertexType>(vertex_box_volumes), viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
                      viennagrid::make_field<EdgeType>(interface_areas), viennagrid::make_field<EdgeType>(interface_contributions));

  //write to vtk:
  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, "voronoi_tri");

  std::cout << std::endl;
  std::cout << viennagrid::cells(mesh)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(viennagrid::cells(mesh)[0]) << std::endl;
  std::cout << "Area of first cell: " << viennagrid::spanned_volume( viennagrid::point( viennagrid::vertices(mesh)[0]),
                                                                     viennagrid::point( viennagrid::vertices(mesh)[1]),
                                                                     viennagrid::point( viennagrid::vertices(mesh)[8])) << std::endl;

  // Check Voronoi volumes:
  voronoi_volume_check(mesh,
          viennagrid::make_field<VertexType>(vertex_box_volumes),
          viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
  );

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}

