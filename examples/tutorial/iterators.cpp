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
#include <fstream>
#include <vector>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"




int main()
{
  //
  // Define the domain and segmentation types
  //
  typedef viennagrid::tetrahedral_3d_domain                             DomainType;
//   typedef viennagrid::triangular_2d_domain                              DomainType; // use this for a 2d domain
  typedef viennagrid::result_of::segmentation<DomainType>::type         Segmentation;
  typedef viennagrid::result_of::cell_tag<DomainType>::type             CellTag;
  
  //
  // Define the types of the elements in the domain (derived from DomainType):
  //
  typedef viennagrid::result_of::point<DomainType>::type                PointType;
  typedef viennagrid::result_of::vertex<DomainType>::type               VertexType;
  typedef viennagrid::result_of::vertex_handle<DomainType>::type        VertexHandleType;
  typedef viennagrid::result_of::edge<DomainType>::type                 EdgeType;
  typedef viennagrid::result_of::facet<DomainType>::type                FacetType;
  typedef viennagrid::result_of::cell<DomainType>::type                 CellType;

  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Iteration over elements --" << std::endl;
  std::cout << "-------------------------------------------------- " << std::endl;
  std::cout << std::endl;
  
  DomainType    domain;
  Segmentation  segmentation(domain);
  
  
  //
  // Read domain from Netgen file
  //
  try
  {
    viennagrid::io::netgen_reader reader;
    reader(domain, segmentation, "../examples/data/cube6.mesh");    //use this for a 3d example
    //reader(domain, segmentation, "../examples/data/square8.mesh"); //use this for a 2d example (also change DomainType defined above!)
  }
  catch (std::exception & e)
  {
    std::cout << "Error reading Netgen mesh file: " << std::endl;
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  
  

  
  //
  ////////////////     Section 1: Iteration over n-cells in the domain/segment      ////////////////////////
  //
  
  //
  // Get the types for a global vertex range and the corresponding iterator. 
  // This allows to traverse all vertices in the domain.
  // The first template argument to *_range<> denotes the enclosing body (here: the domain),
  //
  typedef viennagrid::result_of::vertex_range<DomainType>::type     VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
  
  //
  // Define global edge, facet and cell ranges and their iterator types in the same way:
  //
  typedef viennagrid::result_of::edge_range<DomainType>::type       EdgeRange;
  typedef viennagrid::result_of::iterator<EdgeRange>::type          EdgeIterator;

  typedef viennagrid::result_of::facet_range<DomainType>::type      FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type         FacetIterator;
  
  typedef viennagrid::result_of::cell_range<DomainType>::type       CellRange;
  typedef viennagrid::result_of::iterator<CellRange>::type          CellIterator;
  
  
  //
  // Iterate over all vertices of the domain:
  // Note: when assigned to a range, no template argument is required for elements<>()
  //
  VertexRange vertices = viennagrid::elements(domain);   
  
  for (VertexIterator vit = vertices.begin();  //STL-like iteration
                      vit != vertices.end();
                    ++vit)
  {
    std::cout << *vit << std::endl;
  }
  
  
  //
  // Iterate over all edges of the domain.
  // Can be done like above. To show alternatives as well, this time the range is not set up explicitly in a named variable:
  // Note: the result of elements<>() is not assigned to a range, so an element tag is required!
  std::size_t num_edges = 0;
  for (EdgeIterator eit = viennagrid::elements<viennagrid::line_tag>(domain).begin();   //Note that the template parameter '1' is mandatory here to select edges
                    eit != viennagrid::elements<viennagrid::line_tag>(domain).end();
                  ++eit)
  {
    //do something with *eit here. 
    ++num_edges;  
  }
  std::cout << "Number of edges traversed: " << num_edges << std::endl;
  std::cout << "Number of edges in domain: " << viennagrid::elements<viennagrid::line_tag>(domain).size() << std::endl;
  
  //
  // Even though not recommended, it is also possible to iterate through the elements using operator[]:
  // However, this does not work for all domain storage configurations. Elements in a domain will always work, segments and view might not work
  //
  std::size_t num_cells = 0;
  CellRange cells = viennagrid::elements(domain);
  for (std::size_t i=0; i<cells.size(); ++i)
  {
    // do something with cells[i]
    ++num_cells;
  }
  std::cout << "Number of cells traversed: " << num_cells << std::endl;
  std::cout << "Number of cells in domain: " << viennagrid::elements<CellTag>(domain).size() << std::endl;
  
  //
  // In the same manner, iteration over all facets can be carried out using one of the three methods presented above
  //
  
  
  
  
  //
  ////////////////     Section 2: Iteration over boundary cells of smaller topological level      ////////////////////////
  //
  
  
  //
  // Define some on-element ranges and iterators in the same way.
  //
  typedef viennagrid::result_of::vertex_range<FacetType>::type        VertexOnFacetRange;
  typedef viennagrid::result_of::iterator<VertexOnFacetRange>::type   VertexOnFacetIterator;
  
  typedef viennagrid::result_of::edge_range<FacetType>::type          EdgeOnFacetRange;
  typedef viennagrid::result_of::iterator<EdgeOnFacetRange>::type     EdgeOnFacetIterator;
  
  typedef viennagrid::result_of::facet_range<CellType>::type          FacetOnCellRange;
  typedef viennagrid::result_of::iterator<FacetOnCellRange>::type     FacetOnCellIterator;

  for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit) //iterate over all cells
  {
    std::cout << *cit << std::endl;
    
    //
    // The facets of the cell are obtained by passing the cell as parameter to the elements<>() function
    // Note: again, assignment to range doesn't require to pass the element tag to elements<>()
    //
    FacetOnCellRange facets_on_cells = viennagrid::elements(*cit);
    for (FacetOnCellIterator focit = facets_on_cells.begin();
                             focit != facets_on_cells.end();
                           ++focit)
    {
      FacetType & facet = *focit;
        
      // Iterate over all vertices of the facet:
      std::cout << "Vertices in global orientation: " << std::endl;
      for (VertexOnFacetIterator vofit = viennagrid::vertices(facet).begin();
                                 vofit != viennagrid::vertices(facet).end();
                               ++vofit)
      {
        std::cout << *vofit << std::endl;
      }

      // Same again, but using the orientation imposed by the cell
      std::cout << "Vertices in local orientation: " << std::endl;
      for (std::size_t i=0; i<viennagrid::vertices(facet).size(); ++i)
        std::cout << viennagrid::local_vertex(domain, *cit, focit.handle(), i) << std::endl;
    }
    
    std::cout << std::endl << "---------------" << std::endl << std::endl;
  }

  
  
  //
  ////////////////     Section 3: Iteration over elements of larger topological level      ////////////////////////
  //

  //
  // Ranges and iterators are again obtained from the coboundary_range metafunction.
  // The first argument specifies the root of the iteration,
  // the second argument denotes the element tag of the element from which the coboundary elements are requested
  // the third argument denotes the element tag of the elements over which to iterate.
  //
  typedef viennagrid::result_of::coboundary_range<DomainType, viennagrid::vertex_tag, viennagrid::line_tag>::type     EdgeOnVertexRange;
  typedef viennagrid::result_of::iterator<EdgeOnVertexRange>::type                                                    EdgeOnVertexIterator;
  
  typedef viennagrid::result_of::coboundary_range<DomainType, viennagrid::line_tag, CellTag>::type                    CellOnEdgeRange;
  typedef viennagrid::result_of::iterator<CellOnEdgeRange>::type                                                      CellOnEdgeIterator;
  
  
  // Iteration over all edges connected to the first vertex in the domain:
  VertexHandleType vh0 = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at(0);
  std::size_t num_v0edges = 0;
  
  // To set up the range, two arguments need to be passed to the ncells() function.
  // The second argument denotes the enclosing complex over which to iterate and is either a segment or the full domain.
  EdgeOnVertexRange edges_on_v0 = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::line_tag>(domain, vh0);
  for (EdgeOnVertexIterator eovit = edges_on_v0.begin();
                            eovit != edges_on_v0.end();
                          ++eovit)
  {
    ++num_v0edges;
  }
  std::cout << "Edges connected to vh0 visited: " << num_v0edges << std::endl;
  std::cout << "Edges connected to vh0 available: " << edges_on_v0.size() << std::endl;
  
  //
  // In the same manner, other co-boundary operations are carried out, e.g. cell-on-edge, facet-on-vertex.
  //
  
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


