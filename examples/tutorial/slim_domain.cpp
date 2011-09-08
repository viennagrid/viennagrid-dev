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

#ifdef _MSC_VER      //Visual Studio complains about the use of 'this' in member initialization, which is perfectly legal in our context
  #pragma warning( disable : 4355 )
#endif


#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/algorithm/boundary.hpp"

//
// Disable edges for a tetrahedron (but not its triangular facets):
// Only valid for domains using the configuration class viennagrid::config::tetrahedral_3d
//
VIENNAGRID_DISABLE_BOUNDARY_NCELL(viennagrid::config::tetrahedral_3d, viennagrid::tetrahedron_tag, 1)

//
// Disable edges for all triangles for all possible domain types
// (no matter whether it is a cell or the boundary of a higher-dimensional simplex)
//
VIENNAGRID_GLOBAL_DISABLE_BOUNDARY_NCELL(viennagrid::triangle_tag, 1)

//
// Disable triangular facets of a tetrahedron for all possible domain types
//
VIENNAGRID_GLOBAL_DISABLE_BOUNDARY_NCELL(tetrahedron_tag, 2)

/*
//
// For documentation purposes, the three macros above result in the following code:
// (this allows you to inject your own custom meta-functions if desired)
//
namespace viennagrid
{
  namespace result_of
  {
    template <>
    struct subelement_handling<viennagrid::config::tetrahedral_3d, viennagrid::tetrahedron_tag, 1>
    {
      typedef no_handling_tag    type;
    };

    template <typename ConfigType>
    struct subelement_handling<ConfigType,  viennagrid::triangle_tag, 1>
    {
      typedef no_handling_tag    type;
    };
    
    template <typename ConfigType>
    struct subelement_handling<ConfigType,  viennagrid::tetrahedron_tag, 2>
    {
      typedef no_handling_tag    type;
    };
  }  
}
*/





int main()
{
  typedef viennagrid::config::tetrahedral_3d                       ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type         Domain;
  typedef ConfigType::cell_tag                                    CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim>::type   CellType;

  typedef viennagrid::result_of::ncell_range<Domain, 0>::type       VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type         EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType, 2>::type         FacetType;
  typedef viennagrid::result_of::ncell<ConfigType, 3>::type         CellType;

  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Domain without storage of edges --" << std::endl;
  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  Domain domain;
  
  //
  // Read domain from Netgen file
  //
  viennagrid::io::netgen_reader reader;
  #ifdef _MSC_VER      //Visual Studio builds in a subfolder
  std::string path = "../../examples/data/";
  #else
  std::string path = "../examples/data/";
  #endif
  reader(domain, path + "cube384.mesh");

  //
  // Print a summary of memory consumption:
  //
  std::cout << "Memory for vertices: " 
            << (viennagrid::ncells<0>(domain).size() * sizeof(VertexType)) / 1024.0 << " KB"
            << " for " << viennagrid::ncells<0>(domain).size() << " elements ("
            << sizeof(VertexType) << " Bytes each)" << std::endl;
  std::cout << "Memory for edges: " 
            << (viennagrid::ncells<1>(domain).size() * sizeof(EdgeType)) / 1024.0 << " KB"
            << " for " << viennagrid::ncells<1>(domain).size() << " elements ("
            << sizeof(EdgeType) << " Bytes each)" << std::endl;
  std::cout << "Memory for facets: " 
            << (viennagrid::ncells<2>(domain).size() * sizeof(FacetType)) / 1024.0 << " KB"
            << " for " << viennagrid::ncells<2>(domain).size() << " elements ("
            << sizeof(FacetType) << " Bytes each)" << std::endl;
  std::cout << "Memory for cells: " 
            << (viennagrid::ncells<3>(domain).size() * sizeof(CellType)) / 1024.0 << " KB"
            << " for " << viennagrid::ncells<3>(domain).size() << " elements ("
            << sizeof(CellType) << " Bytes each)" << std::endl;
  std::cout << std::endl;

  std::cout << "Total memory required: "
            << (viennagrid::ncells<0>(domain).size() * sizeof(VertexType)) / 1024.0
             + (viennagrid::ncells<1>(domain).size() * sizeof(EdgeType)) / 1024.0
             + (viennagrid::ncells<2>(domain).size() * sizeof(FacetType)) / 1024.0
             + (viennagrid::ncells<3>(domain).size() * sizeof(CellType)) / 1024.0 << " KB" << std::endl;
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
