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
// Generate a ring of six triangles:
//
template <typename DomainType>
void setup_domain(DomainType & domain)
{
  typedef typename viennagrid::result_of::vertex<DomainType>::type                         VertexType;
  typedef typename viennagrid::result_of::vertex_handle<DomainType>::type                         VertexHandleType;
  typedef typename viennagrid::result_of::point<DomainType>::type   PointType;
  
  
  //
  // Step 1: Set up vertices:
  //
  
  VertexHandleType vh[10];
  
  vh[0] = viennagrid::make_vertex( domain, PointType(0, 0) );
  vh[1] = viennagrid::make_vertex( domain, PointType(2, 1) );
  vh[2] = viennagrid::make_vertex( domain, PointType(1, 2) );
  vh[3] = viennagrid::make_vertex( domain, PointType(-1, 2) );
  vh[4] = viennagrid::make_vertex( domain, PointType(-2, 1) );
  vh[5] = viennagrid::make_vertex( domain, PointType(-2, -1) );
  vh[6] = viennagrid::make_vertex( domain, PointType(-1, -2) );
  vh[7] = viennagrid::make_vertex( domain, PointType(1, -2) );
  vh[8] = viennagrid::make_vertex( domain, PointType(2, -1) );
  vh[9] = viennagrid::make_vertex( domain, PointType(1.3, 2.7) ); // for special cell with circumcenter outside:
  
  
  //
  // Step 2: Set up cells:
  //
  
  viennagrid::make_triangle( domain, vh[8], vh[1], vh[0] );
  
  for (size_t i=1; i<8; ++i)
    viennagrid::make_triangle( domain, vh[i], vh[i+1], vh[0] );

  viennagrid::make_triangle( domain, vh[2], vh[1], vh[9] );
}

int main()
{
  typedef viennagrid::triangular_2d_domain           DomainType;
  
  std::cout << "* main(): Creating domain..." << std::endl;
  DomainType domain;
  
  //create domain:
  setup_domain(domain);
  
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
  
  //write to vtk:
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  my_vtk_writer(domain, "voronoi_tri");
  
  std::cout << std::endl;
  std::cout << viennagrid::cells(domain)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(viennagrid::cells(domain)[0]) << std::endl;
  std::cout << "Area of first cell: " << viennagrid::spanned_volume( viennagrid::point( viennagrid::vertices(domain)[0]),
                                                                     viennagrid::point( viennagrid::vertices(domain)[1]),
                                                                     viennagrid::point( viennagrid::vertices(domain)[8])) << std::endl;

  // Check Voronoi volumes:
  voronoi_volume_check(domain,
          viennagrid::make_field<VertexType>(vertex_box_volumes),
          viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
  );

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

