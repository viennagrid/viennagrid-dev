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
template <typename DomainType>
void setup_domain(DomainType & domain)
{
  typedef typename viennagrid::result_of::point<DomainType>::type                            PointType;
  typedef typename viennagrid::result_of::vertex<DomainType>::type                         VertexType;
  typedef typename viennagrid::result_of::vertex_handle<DomainType>::type                         VertexHandleType;
  
  //
  // Step 1: Set up vertices:
  //
  VertexHandleType vh0 = viennagrid::make_vertex( domain, PointType(0) );
  VertexHandleType vh1 = viennagrid::make_vertex( domain, PointType(0.25) );
  VertexHandleType vh2 = viennagrid::make_vertex( domain, PointType(0.5) );
  VertexHandleType vh3 = viennagrid::make_vertex( domain, PointType(0.75) );
  VertexHandleType vh4 = viennagrid::make_vertex( domain, PointType(1.0) );
  //
  // Step 2: Set up cells:
  //
  
  viennagrid::make_line( domain, vh0, vh1 );
  viennagrid::make_line( domain, vh1, vh2 );
  viennagrid::make_line( domain, vh2, vh3 );
  viennagrid::make_line( domain, vh3, vh4 );
}




int main()
{
  typedef viennagrid::line_1d_domain                   DomainType;
  
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
          viennagrid::make_accessor<EdgeType>(interface_areas),
          viennagrid::make_accessor<EdgeType>(interface_contributions),
          viennagrid::make_accessor<VertexType>(vertex_box_volumes),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_accessor<EdgeType>(edge_box_volumes),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions)
  );
                   
  
  //output results:
  output_voronoi_info(domain,
                      viennagrid::make_accessor<VertexType>(vertex_box_volumes), viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
                      viennagrid::make_accessor<EdgeType>(interface_areas), viennagrid::make_accessor<EdgeType>(interface_contributions));
  
  std::cout << std::endl;
  std::cout << viennagrid::cells(domain)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(viennagrid::cells(domain)[0]) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(viennagrid::cells(domain)[1]) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(viennagrid::cells(domain)[2]) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(viennagrid::cells(domain)[3]) << std::endl;
  std::cout << "-----------------------" << std::endl;
  std::cout << "Centroid of cell #0: " << viennagrid::centroid(viennagrid::cells(domain)[0]) << std::endl;
  std::cout << "Centroid of cell #1: " << viennagrid::centroid(viennagrid::cells(domain)[1]) << std::endl;
  std::cout << "Centroid of cell #2: " << viennagrid::centroid(viennagrid::cells(domain)[2]) << std::endl;
  std::cout << "Centroid of cell #3: " << viennagrid::centroid(viennagrid::cells(domain)[3]) << std::endl;
  
  // Check Voronoi volumes:
  voronoi_volume_check(domain,
          viennagrid::make_accessor<VertexType>(vertex_box_volumes),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions)
  );
  
  //write to vtk:
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  my_vtk_writer(domain, "voronoi_line");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

