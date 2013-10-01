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

#include <iostream>
#include <vector>

#include "viennagrid/config/default_configs.hpp"
#include "voronoi_common.hpp"
#include "refinement-common.hpp"

//
// Generate four rectangles
//
template <typename MeshType>
void setup_mesh(MeshType & mesh)
{
  typedef typename viennagrid::result_of::vertex<MeshType>::type                         VertexType;
  typedef typename viennagrid::result_of::vertex_handle<MeshType>::type                         VertexHandleType;
  typedef typename viennagrid::result_of::point<MeshType>::type   PointType;
  
  
  //
  // Step 1: Set up vertices:
  //
  
  VertexHandleType vh0 = viennagrid::make_vertex( mesh, PointType(0, 0) );
  VertexHandleType vh1 = viennagrid::make_vertex( mesh, PointType(2, 0) );
  VertexHandleType vh2 = viennagrid::make_vertex( mesh, PointType(1, 1) );
  VertexHandleType vh3 = viennagrid::make_vertex( mesh, PointType(0, 2) );
  VertexHandleType vh4 = viennagrid::make_vertex( mesh, PointType(-1, 1) );
  VertexHandleType vh5 = viennagrid::make_vertex( mesh, PointType(-2, 0) );
  VertexHandleType vh6 = viennagrid::make_vertex( mesh, PointType(-1, -1) );
  VertexHandleType vh7 = viennagrid::make_vertex( mesh, PointType(0, -2) );
  VertexHandleType vh8 = viennagrid::make_vertex( mesh, PointType(1, -1) );
  
  //
  // Step 2: Set up cells:
  //
  
  viennagrid::make_quadrilateral( mesh, vh0, vh8, vh2, vh1 );
  viennagrid::make_quadrilateral( mesh, vh0, vh2, vh4, vh3 );
  viennagrid::make_quadrilateral( mesh, vh0, vh4, vh6, vh5 );
  viennagrid::make_quadrilateral( mesh, vh0, vh6, vh8, vh7 );
}




int main()
{
  typedef viennagrid::quadrilateral_2d_mesh           MeshType;
  
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
          viennagrid::make_accessor<EdgeType>(interface_areas),
          viennagrid::make_accessor<EdgeType>(interface_contributions),
          viennagrid::make_accessor<VertexType>(vertex_box_volumes),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_accessor<EdgeType>(edge_box_volumes),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions)
  );
  
  //output results:
  output_voronoi_info(mesh,
                      viennagrid::make_accessor<VertexType>(vertex_box_volumes), viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
                      viennagrid::make_accessor<EdgeType>(interface_areas), viennagrid::make_accessor<EdgeType>(interface_contributions));
  
  std::cout << std::endl;
  std::cout << viennagrid::cells(mesh)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(viennagrid::cells(mesh)[0]) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(viennagrid::cells(mesh)[1]) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(viennagrid::cells(mesh)[2]) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(viennagrid::cells(mesh)[3]) << std::endl;
  
  // Check Voronoi volumes:
  voronoi_volume_check(mesh,
          viennagrid::make_accessor<VertexType>(vertex_box_volumes),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions)
  );
  
  //write to vtk:
  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, "voronoi_rect");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

