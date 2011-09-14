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


#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/config/simplex.hpp"


template <typename CellType, typename DomainType>
void setup_cell(CellType & cell,
                DomainType & domain,
                std::size_t id0,
                std::size_t id1,
                std::size_t id2)
{
  typedef typename DomainType::config_type                                 ConfigType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;

  VertexType * cell_vertices[3]; //holds pointers to the respective vertices in the domain
 
  cell_vertices[0] = &(viennagrid::ncells<0>(domain)[id0]);
  cell_vertices[1] = &(viennagrid::ncells<0>(domain)[id1]);
  cell_vertices[2] = &(viennagrid::ncells<0>(domain)[id2]);
  cell.vertices(cell_vertices);
}

//
//    Let us construct the following input domain:
//
//    5---------4---------3
//    | \       | \       |
//    |   \     |   \     |    y
//    |     \   |     \   |     ^
//    |       \ |       \ |     |
//    0---------1---------2     *--> x
//
//    Segment 1 | Segment 2
//
int main()
{
  //
  // Define the necessary types:
  //
  typedef viennagrid::config::triangular_2d                       ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type         Domain;
  typedef viennagrid::result_of::segment<ConfigType>::type        Segment;
  typedef ConfigType::cell_tag                                    CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim>::type   CellType;

  typedef viennagrid::result_of::ncell_range<Segment, CellTag::dim>::type    CellRange;
  typedef viennagrid::result_of::iterator<CellRange>::type                             CellIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a domain with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the domain and create two segments:
  //
  Domain domain;
  domain.segments().resize(2);
  Segment seg0 = domain.segments()[0];
  Segment seg1 = domain.segments()[1];
  
  //
  // Step 2: Add vertices to the domain. 
  //         Note that vertices with IDs are enumerated in the order they are pushed to the domain.
  //
  domain.push_back(PointType(0,0));
  domain.push_back(PointType(1,0));
  domain.push_back(PointType(2,0));
  domain.push_back(PointType(2,1));
  domain.push_back(PointType(1,1));
  domain.push_back(PointType(0,1));
  
  //
  // Step 3: Fill the two segments with cells. 
  //         To do so, each cell must be linked with the defining vertices from the domain (not v0, v1, ...!)
  //
  CellType cell;
  VertexType * cell_vertices[3]; //holds pointers to the respective vertices in the domain

  // First triangle: (do not use v0, v1, etc. for vertex setup!)
  cell_vertices[0] = &(viennagrid::ncells<0>(domain)[0]); //get vertex with ID 0 from domain
  cell_vertices[1] = &(viennagrid::ncells<0>(domain)[1]); //get vertex with ID 1 from domain
  cell_vertices[2] = &(viennagrid::ncells<0>(domain)[5]); //get vertex with ID 5 from domain
  cell.vertices(cell_vertices); //set cell vertices. 
  //Note that vertices are rearranged internally if they are not supplied in mathematically positive order.
  
  seg0.push_back(cell); //copies 'cell' to the domain. 'cell' can be reused for setting up the other cells.
  
  
  // Second triangle:
  setup_cell(cell, domain, 1, 4, 5);  //use the shortcut function defined at the beginning of this tutorial
  seg0.push_back(cell);

  // Third triangle:
  setup_cell(cell, domain, 1, 2, 4);
  seg1.push_back(cell); // Note that we push to 'seg1' now.

  // Fourth triangle:
  setup_cell(cell, domain, 2, 3, 4);
  seg1.push_back(cell);

  //
  // That's it. The domain consisting of two segments is now set up.
  // If no segments are required, one can also directly write domain.push_back(cell);
  //
  
  //
  // Step 4: Output the cells for each segment:
  //
  
  std::cout << "Cells in segment 0:" << std::endl;
  CellRange cells_seg0 = viennagrid::ncells(seg0);
  for (CellIterator cit0 = cells_seg0.begin();
                    cit0 != cells_seg0.end();
                  ++cit0)
  {
    std::cout << *cit0 << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "Cells in segment 1:" << std::endl;
  CellRange cells_seg1 = viennagrid::ncells(seg1);
  for (CellIterator cit1 = cells_seg1.begin();
                    cit1 != cells_seg1.end();
                  ++cit1)
  {
    std::cout << *cit1 << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  
  return EXIT_SUCCESS;
}
