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

#include "viennagrid/domain.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/io/netgen_reader.hpp"



int main()
{
  typedef viennagrid::config::tetrahedral_3d                      ConfigType;  //use this for a 3d example
  //typedef viennagrid::config::triangular_2d                       ConfigType;  //use this for a 2d example (also change filename below!)
  typedef viennagrid::result_of::domain<ConfigType>::type         DomainType;
  typedef ConfigType::cell_tag                           CellTag;

  //
  // Define the types of the elements in the domain (derived from ConfigType):
  //
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type                          VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type                          EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim-1>::type  FacetType;
  typedef viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type    CellType;

  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Iteration over elements --" << std::endl;
  std::cout << "-------------------------------------------------- " << std::endl;
  std::cout << std::endl;
  
  DomainType domain;
  
  
  //
  // Read domain from Netgen file
  //
  try
  {
    viennagrid::io::netgen_reader reader;
    #ifdef _MSC_VER      //Visual Studio builds in a subfolder
    std::string path = "../../examples/data/";
    #else
    std::string path = "../examples/data/";
    #endif
    reader(domain, path + "cube48.mesh"); //use this for a 3d example
    //reader(domain, path + "square32.mesh"); //use this for a 2d example (also change ConfigType defined above!)
  }
  catch (std::exception & e)
  {
    std::cout << "Error reading Netgen mesh file: " << std::endl;
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  
  

  
  //
  ////////////////     Section 1: Iteration over elements of smaller topological level      ////////////////////////
  //
  
  //
  // Get the types for a global vertex range and the corresponding iterator. 
  // This allows to traverse all vertices in the domain.
  // The first template argument to ncell_range<> denotes the enclosing body (here: the domain),
  // and the second argument denotes the topological dimension of the elements over which to iterate (0...vertices, 1...lines, etc.)
  //
  typedef viennagrid::result_of::ncell_range<DomainType, 0>::type     VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type          VertexIterator;
  
  //
  // Define global edge, facet and cell ranges and their iterator types in the same way:
  //
  typedef viennagrid::result_of::ncell_range<DomainType, 1>::type     EdgeRange;
  typedef viennagrid::result_of::iterator<EdgeRange>::type            EdgeIterator;

  typedef viennagrid::result_of::ncell_range<DomainType, CellTag::dim-1>::type     FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type                                   FacetIterator;
  
  typedef viennagrid::result_of::ncell_range<DomainType, CellTag::dim>::type       CellRange;
  typedef viennagrid::result_of::iterator<CellRange>::type                                    CellIterator;
  
  
  //
  // Iterate over all vertices of the domain:
  // Note: when assigned to a range, the topological level usually required as template argument for ncells<>()
  //       can be deduced automatically and may thus be omitted
  //
  VertexRange vertices = viennagrid::ncells(domain);   
  //VertexRange vertices = viennagrid::ncells<0>(domain); //this is an equivalent way of setting up the vertex range
  
  for (VertexIterator vit = vertices.begin();  //STL-like iteration
                      vit != vertices.end();
                    ++vit)
  {
    std::cout << *vit << std::endl;
  }
  
  
  //
  // Iterate over all edges of the domain.
  // Can be done like above. To show alternatives as well, this time the range is not set up explicitly in a named variable:
  std::size_t num_edges = 0;
  for (EdgeIterator eit = viennagrid::ncells<1>(domain).begin();   //Note that the template parameter '1' is mandatory here to select edges
                    eit != viennagrid::ncells<1>(domain).end();
                  ++eit)
  {
    //do something with *eit here. 
    ++num_edges;  
  }
  std::cout << "Number of edges traversed: " << num_edges << std::endl;
  std::cout << "Number of edges in domain: " << viennagrid::ncells<1>(domain).size() << std::endl;
  
  //
  // Even though not recommended, it is also possible to iterate through the elements using operator[]:
  // However, this does not work for all domain storage configurations. Cells and vertices will always work.
  //
  std::size_t num_cells = 0;
  CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
  for (std::size_t i=0; i<cells.size(); ++i)
  {
    ++num_cells;
  }
  std::cout << "Number of cells traversed: " << num_cells << std::endl;
  std::cout << "Number of cells in domain: " << viennagrid::ncells<CellTag::dim>(domain).size() << std::endl;
  
  //
  // In the same manner, iteration over all facets can be carried out using one of the three methods presented above
  //
  
  
  
  
  //
  ////////////////     Section 2: Iteration over elements of smaller topological level      ////////////////////////
  //
  
  
  //
  // Define some on-element ranges and iterators in the same way.
  // Mind the first template argument of ncell_range<>, which is the enclosing element.
  //
  typedef viennagrid::result_of::ncell_range<FacetType, 0>::type                 VertexOnFacetRange;
  typedef viennagrid::result_of::iterator<VertexOnFacetRange>::type              VertexOnFacetIterator;
  
  typedef viennagrid::result_of::ncell_range<FacetType, 1>::type                EdgeOnFacetRange;
  typedef viennagrid::result_of::iterator<EdgeOnFacetRange>::type               EdgeOnFacetIterator;
  
  typedef viennagrid::result_of::ncell_range<CellType, CellTag::dim-1>::type   FacetOnCellRange;
  typedef viennagrid::result_of::iterator<FacetOnCellRange>::type                         FacetOnCellIterator;

  std::size_t vertices_visited = 0;
  for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit) //iterate over all cells
  {
    //The facets of the cell are obtained by passing the cell as parameter to the ncell() function
    FacetOnCellRange facets_on_cells = viennagrid::ncells(*cit);
    for (FacetOnCellIterator focit = facets_on_cells.begin();
                             focit != facets_on_cells.end();
                           ++focit)
    {
      //Finally, iterate over all vertices of the facet:
      for (VertexOnFacetIterator vofit = viennagrid::ncells<0>(*focit).begin();
                                 vofit != viennagrid::ncells<0>(*focit).end();
                               ++vofit)
      {
        ++vertices_visited;
      }
    }
  }
  std::cout << "Number of vertices visited: " << vertices_visited << std::endl;
  
  
  
  
  //
  ////////////////     Section 3: Iteration over elements of higher topological level      ////////////////////////
  //

  //
  // Ranges and iterators are again obtained from the ncell_range metafunction.
  // As before, the first argument specifies the root of the iteration,
  // the second argument denotes the topological dimension of the elements over which to iterate.
  //
  typedef viennagrid::result_of::ncell_range<VertexType, 1>::type                EdgeOnVertexRange;
  typedef viennagrid::result_of::iterator<EdgeOnVertexRange>::type               EdgeOnVertexIterator;
  
  typedef viennagrid::result_of::ncell_range<EdgeType, CellTag::dim>::type   CellOnEdgeRange;
  typedef viennagrid::result_of::iterator<CellOnEdgeRange>::type                        CellOnEdgeIterator;
  
  
  // Iteration over all edges connected to the first vertex in the domain:
  VertexType & v0 = viennagrid::ncells<0>(domain)[0];
  std::size_t num_v0edges = 0;
  
  // To set up the range, two arguments need to be passed to the ncells() function.
  // The second argument denotes the enclosing complex over which to iterate and is either a segment or the full domain.
  EdgeOnVertexRange edges_on_v0 = viennagrid::ncells(v0, domain);
  for (EdgeOnVertexIterator eovit = edges_on_v0.begin();
                            eovit != edges_on_v0.end();
                          ++eovit)
  {
    ++num_v0edges;
  }
  std::cout << "Edges connected to v0 visited: " << num_v0edges << std::endl;
  std::cout << "Edges connected to v0 available: " << edges_on_v0.size() << std::endl;
  
  //
  // In the same manner, other co-boundary operations are carried out, e.g. cell-on-edge, facet-on-vertex.
  //
  
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


