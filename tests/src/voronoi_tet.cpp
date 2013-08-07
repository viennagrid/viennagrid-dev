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


#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"

#include "voronoi_common.hpp"
#include "refinement-common.hpp"


int main(int argc, char *argv[])
{
  typedef viennagrid::tetrahedral_3d_domain          DomainType;
  typedef viennagrid::tetrahedral_3d_segmentation    SegmentationType;
  
  std::cout << "* main(): Creating device..." << std::endl;
  DomainType domain;
  SegmentationType segmentation(domain);

  std::string path = "../../examples/data/";
  
  //create device:
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(domain, segmentation, path + "cube48.mesh");
  }
  catch (...)
  {
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    return EXIT_FAILURE;
  }
  
  typedef viennagrid::result_of::vertex<DomainType>::type    VertexType;
  typedef viennagrid::result_of::line<DomainType>::type    EdgeType;
  typedef viennagrid::result_of::cell<DomainType>::type    CellType;
  typedef viennagrid::result_of::const_cell_handle<DomainType>::type    ConstCellHandleType;
  
  std::deque<double> interface_areas;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > interface_contributions;
  
  std::deque<double> vertex_box_volumes;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > vertex_box_volume_contributions;
  
  std::deque<double> edge_box_volumes;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > edge_box_volume_contributions;
  
  
  //set up dual grid info:
  viennagrid::apply_voronoi<CellType>(
          domain,
          viennagrid::make_field<EdgeType>(interface_areas),
          viennagrid::make_field<EdgeType>(interface_contributions),
          viennagrid::make_field<VertexType>(vertex_box_volumes),
          viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_field<EdgeType>(edge_box_volumes),
          viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
  );
  
  //output results:
  output_voronoi_info(domain,
                      viennagrid::make_field<VertexType>(vertex_box_volumes), viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
                      viennagrid::make_field<EdgeType>(interface_areas), viennagrid::make_field<EdgeType>(interface_contributions));
  
  
  std::cout << std::endl;
  std::cout << viennagrid::cells(domain)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(viennagrid::cells(domain)[0]) << std::endl;

  // Check Voronoi volumes:
  voronoi_volume_check(domain,
          viennagrid::make_field<VertexType>(vertex_box_volumes),
          viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
  );
  
  //write to vtk:
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  my_vtk_writer(domain, segmentation, "voronoi_tet");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

