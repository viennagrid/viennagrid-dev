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
// Configure ViennaGrid not to store anything other than vertices on cells
//
namespace viennagrid
{
  namespace result_of
  {
    //disable edges on tetrahedrons:
    template <>
    struct subelement_handling< viennagrid::tetrahedron_tag, 1>
    {
      typedef no_handling_tag    type;
    };

    template <>
    struct subelement_handling< viennagrid::triangle_tag, 1>
    {
      typedef no_handling_tag    type;
    };
    
    
    template <typename T>
    struct subelement_handling< viennagrid::domain_t<T>, 1>
    {
      typedef no_handling_tag    type;
    };
    
    //disable faces:
    /*
    template <>
    struct handling_tag< viennagrid::tetrahedron_tag, 2>
    {
      typedef no_handling_tag    type;
    };
    */
    
  }  
}






int main()
{
  typedef viennagrid::config::tetrahedral_3d                       ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type         Domain;
  typedef ConfigType::cell_tag                                    CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::topology_level>::type   CellType;

  typedef viennagrid::result_of::ncell_range<Domain, 0>::type           VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;

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
  reader(domain, path + "cube48.mesh");
  
  //
  // Iterate over vertices:
  //
  std::size_t num_elements = 0;
  VertexRange vertices = viennagrid::ncells(domain);
  for (VertexIterator vit = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
    if (viennagrid::is_boundary(*vit, domain))
    {
      std::cout << *vit << std::endl;
      ++num_elements;
    }
  }
  std::cout << "Elements on boundary: " << num_elements << " out of " << vertices.size() << " total." << std::endl;

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
