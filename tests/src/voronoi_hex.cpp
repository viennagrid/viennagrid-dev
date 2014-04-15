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

#include <iostream>
#include <vector>

#include "viennagrid/config/default_configs.hpp"
#include "voronoi_common.hpp"
#include "refinement-common.hpp"

template <typename MeshType>
void add_plane(MeshType & mesh, double z)
{
  typedef typename viennagrid::result_of::point<MeshType>::type PointType;

  viennagrid::make_vertex( mesh, PointType(0,0,z) );
  viennagrid::make_vertex( mesh, PointType(2,0,z) );
  viennagrid::make_vertex( mesh, PointType(1,1,z) );
  viennagrid::make_vertex( mesh, PointType(0,2,z) );
  viennagrid::make_vertex( mesh, PointType(-1,1,z) );
  viennagrid::make_vertex( mesh, PointType(-2,0,z) );
  viennagrid::make_vertex( mesh, PointType(-1,-1,z) );
  viennagrid::make_vertex( mesh, PointType(0,-2,z) );
  viennagrid::make_vertex( mesh, PointType(1,-1,z) );
}


template <typename MeshType>
void add_cellplane(MeshType & mesh, std::size_t offset)
{
  viennagrid::make_hexahedron(mesh,
                              viennagrid::vertices(mesh).handle_at(0+offset),
                              viennagrid::vertices(mesh).handle_at(8+offset),
                              viennagrid::vertices(mesh).handle_at(2+offset),
                              viennagrid::vertices(mesh).handle_at(1+offset),
                              viennagrid::vertices(mesh).handle_at(9+offset),
                              viennagrid::vertices(mesh).handle_at(17+offset),
                              viennagrid::vertices(mesh).handle_at(11+offset),
                              viennagrid::vertices(mesh).handle_at(10+offset));


  viennagrid::make_hexahedron(mesh,
                              viennagrid::vertices(mesh).handle_at(0+offset),
                              viennagrid::vertices(mesh).handle_at(2+offset),
                              viennagrid::vertices(mesh).handle_at(4+offset),
                              viennagrid::vertices(mesh).handle_at(3+offset),
                              viennagrid::vertices(mesh).handle_at(9+offset),
                              viennagrid::vertices(mesh).handle_at(11+offset),
                              viennagrid::vertices(mesh).handle_at(13+offset),
                              viennagrid::vertices(mesh).handle_at(12+offset));


  viennagrid::make_hexahedron(mesh,
                              viennagrid::vertices(mesh).handle_at(0+offset),
                              viennagrid::vertices(mesh).handle_at(4+offset),
                              viennagrid::vertices(mesh).handle_at(6+offset),
                              viennagrid::vertices(mesh).handle_at(5+offset),
                              viennagrid::vertices(mesh).handle_at(9+offset),
                              viennagrid::vertices(mesh).handle_at(13+offset),
                              viennagrid::vertices(mesh).handle_at(15+offset),
                              viennagrid::vertices(mesh).handle_at(14+offset));


  viennagrid::make_hexahedron(mesh,
                              viennagrid::vertices(mesh).handle_at(0+offset),
                              viennagrid::vertices(mesh).handle_at(6+offset),
                              viennagrid::vertices(mesh).handle_at(8+offset),
                              viennagrid::vertices(mesh).handle_at(7+offset),
                              viennagrid::vertices(mesh).handle_at(9+offset),
                              viennagrid::vertices(mesh).handle_at(15+offset),
                              viennagrid::vertices(mesh).handle_at(17+offset),
                              viennagrid::vertices(mesh).handle_at(16+offset));
}

//
// Generate four rectangles
//
template <typename MeshType>
void setup_mesh(MeshType & mesh)
{
  //
  // Step 1: Set up vertices:
  //
  add_plane(mesh, 0.0);
  add_plane(mesh, 1.0);
  add_plane(mesh, 2.0);

  //
  // Step 2: Set up cells:
  //

  add_cellplane(mesh, 0);
  add_cellplane(mesh, 9);
}




int main()
{
  typedef viennagrid::hexahedral_3d_mesh              MeshType;
//   typedef viennagrid::result_of::mesh<Config>::type   DeviceType;

  std::cout << "* main(): Creating device..." << std::endl;
  MeshType mesh;

  //create device:
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
          viennagrid::make_accessor<EdgeType>(interface_areas),
          viennagrid::make_accessor<EdgeType>(interface_contributions),
          viennagrid::make_accessor<VertexType>(vertex_box_volumes),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_accessor<EdgeType>(edge_box_volumes),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions)
  );
//   viennagrid::apply_voronoi(device, interface_key, box_volume_key);

  //output results:
  output_voronoi_info(mesh,
                      viennagrid::make_accessor<VertexType>(vertex_box_volumes), viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
                      viennagrid::make_accessor<EdgeType>(interface_areas), viennagrid::make_accessor<EdgeType>(interface_contributions));

  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(viennagrid::cells(mesh)[0]) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(viennagrid::cells(mesh)[1]) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(viennagrid::cells(mesh)[2]) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(viennagrid::cells(mesh)[3]) << std::endl;
  std::cout << "Circumcenter of cell #4: " << viennagrid::circumcenter(viennagrid::cells(mesh)[4]) << std::endl;
  std::cout << "Circumcenter of cell #5: " << viennagrid::circumcenter(viennagrid::cells(mesh)[5]) << std::endl;
  std::cout << "Circumcenter of cell #6: " << viennagrid::circumcenter(viennagrid::cells(mesh)[6]) << std::endl;
  std::cout << "Circumcenter of cell #7: " << viennagrid::circumcenter(viennagrid::cells(mesh)[7]) << std::endl;
  std::cout << std::endl;

  // Check Voronoi volumes:
  voronoi_volume_check(mesh,
          viennagrid::make_accessor<VertexType>(vertex_box_volumes),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions)
  );

  //check dual box volume of vertex 9: must be 2
  double volume9 = vertex_box_volumes[9];
  if ( fabs(volume9 - 2.0) > 1e-10 )
  {
    std::cerr << "Incorrect dual box volume at vertex #9: " << volume9 << std::endl;
    return EXIT_FAILURE;
  }
  else
    std::cout << "Dual volume check at vertex #9 passed." << std::endl;

  //write to vtk:
  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, "voronoi_hex");

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}

