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


#include "viennagrid/forwards.hpp"
#include "viennagrid/domain/geometric_domain.hpp"
#include "viennagrid/point.hpp"


template <typename CellType, typename DomainType, typename SegmentType>
void setup_cell(DomainType & domain,
                SegmentType & segment,
                std::size_t id0,
                std::size_t id1,
                std::size_t id2)
{
  typedef typename viennagrid::result_of::handle<DomainType, viennagrid::vertex_tag>::type       VertexHookType;
  
  viennagrid::storage::static_array<VertexHookType, 3> vertices;
  vertices[0] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at( id0 );
  vertices[1] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at( id1 );
  vertices[2] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at( id2 );
  
  viennagrid::create_element<CellType>(segment, vertices);
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
  
  typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > PointType;
  
  typedef viennagrid::result_of::geometric_domain_config< viennagrid::triangle_tag, PointType, viennagrid::storage::id_handle_tag >::type DomainConfig;
  
  //typedef viennagrid::config::triangular_2d                       ConfigType;
  //typedef viennagrid::result_of::domain<ConfigType>::type         Domain;
  //typedef viennagrid::result_of::segment<ConfigType>::type        Segment;
  
  typedef viennagrid::result_of::geometric_domain< DomainConfig >::type Domain;  
  typedef viennagrid::result_of::geometric_view<Domain>::type Segment;
  typedef viennagrid::triangle_tag                                    CellTag;
  
  //typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::handle<Domain, viennagrid::vertex_tag>::type       VertexHookType;
  typedef viennagrid::result_of::element<Domain, CellTag>::type   CellType;

  typedef viennagrid::result_of::element_range<Segment, CellTag>::type    CellRange;
  typedef viennagrid::result_of::iterator<CellRange>::type                             CellIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a domain with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the domain and create two segments:
  //
  Domain domain;
//   std::vector<Segment> segments;
//   
//   segments.resize(2);
//   segments[0] = viennagrid::create_view<Segment>(domain);
//   segments[1] = viennagrid::create_view<Segment>(domain);
//   
  
  //domain.segments().resize(2);
  Segment seg0 = viennagrid::create_view<Segment>(domain);
  Segment seg1 = viennagrid::create_view<Segment>(domain);
  
  //
  // Step 2: Add vertices to the domain. 
  //         Note that vertices with IDs are enumerated in the order they are pushed to the domain.
  //
  viennagrid::point( domain, viennagrid::create_element<VertexType>(domain) ) = PointType(0,0);
  viennagrid::point( domain, viennagrid::create_element<VertexType>(domain) ) = PointType(1,0);
  viennagrid::point( domain, viennagrid::create_element<VertexType>(domain) ) = PointType(2,0);
  viennagrid::point( domain, viennagrid::create_element<VertexType>(domain) ) = PointType(2,1);
  viennagrid::point( domain, viennagrid::create_element<VertexType>(domain) ) = PointType(1,1);
  viennagrid::point( domain, viennagrid::create_element<VertexType>(domain) ) = PointType(0,1);

  
  //
  // Step 3: Fill the two segments with cells. 
  //         To do so, each cell must be linked with the defining vertices from the domain (not v0, v1, ...!)
  //
  
  viennagrid::storage::static_array<VertexHookType, 3> vertices;
  
  
  //CellType cell;
  //VertexType * cell_vertices[3]; //holds pointers to the respective vertices in the domain

  // First triangle: (do not use v0, v1, etc. for vertex setup!)
  vertices[0] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at(0);
  vertices[1] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at(1);
  vertices[2] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at(5);
  //Note that vertices are rearranged internally if they are not supplied in mathematically positive order.
  
  viennagrid::create_element<CellType>(seg0, vertices); //copies 'cell' to the domain. 'cell' can be reused for setting up the other cells.
  
  
  // Second triangle:
  setup_cell<CellType>(domain, seg0, 1, 4, 5);  //use the shortcut function defined at the beginning of this tutorial

  // Third triangle:
  setup_cell<CellType>(domain, seg1, 1, 2, 4); // Note that we push to 'seg1' now.

  // Fourth triangle:
  setup_cell<CellType>(domain, seg1, 2, 3, 4);

  //
  // That's it. The domain consisting of two segments is now set up.
  // If no segments are required, one can also directly write domain.push_back(cell);
  //
  
  //
  // Step 4: Output the cells for each segment:
  //
  
  std::cout << "Cells in segment 0:" << std::endl;
  CellRange cells_seg0 = viennagrid::elements<CellType>(seg0);
  for (CellIterator cit0 = cells_seg0.begin();
                    cit0 != cells_seg0.end();
                  ++cit0)
  {
    std::cout << *cit0 << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "Cells in segment 1:" << std::endl;
  CellRange cells_seg1 = viennagrid::elements<CellType>(seg1);
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
