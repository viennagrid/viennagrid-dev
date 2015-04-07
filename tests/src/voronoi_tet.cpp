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


#include "viennagridpp/io/netgen_reader.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
#include "viennagridpp/config/default_configs.hpp"
#include "viennagridpp/algorithm/voronoi.hpp"
#include "viennagridpp/algorithm/circumcenter.hpp"

#include "voronoi_common.hpp"
#include "refinement-common.hpp"


int main()
{
  typedef viennagrid::tetrahedral_3d_mesh          MeshType;
  typedef viennagrid::tetrahedral_3d_segmentation    SegmentationType;

  std::cout << "* main(): Creating device..." << std::endl;
  MeshType mesh;
  SegmentationType segmentation(mesh);

  std::string path = "../examples/data/";

  //create device:
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(mesh, segmentation, path + "cube48.mesh");
  }
  catch (...)
  {
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    return EXIT_FAILURE;
  }

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


  std::cout << std::endl;
  std::cout << viennagrid::cells(mesh)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(viennagrid::cells(mesh)[0]) << std::endl;

  // Check Voronoi volumes:
  voronoi_volume_check(mesh,
          viennagrid::make_field<VertexType>(vertex_box_volumes),
          viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
  );

  //write to vtk:
  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, segmentation, "voronoi_tet");

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}

