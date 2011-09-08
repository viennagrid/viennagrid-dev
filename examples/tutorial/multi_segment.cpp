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
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/voronoi.hpp"


int main()
{
  typedef viennagrid::config::hexahedral_3d                                            ConfigType;
  typedef viennagrid::result_of::domain<viennagrid::config::hexahedral_3d>::type       DomainHex;
  
  typedef viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef ConfigType::cell_tag                                   CellTag;
  typedef viennagrid::result_of::segment<ConfigType>::type       SegmentType;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell<ConfigType, 1>::type       EdgeType;
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim-1>::type FacetType;
  typedef viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim>::type   CellType;
                                            
  typedef viennagrid::result_of::ncell_range<Domain, 0>::type       VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
                                            
  typedef viennagrid::result_of::ncell_range<Domain, CellTag::dim-1>::type      FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type                                FacetIterator;
  
  typedef viennagrid::result_of::ncell_range<FacetType, CellTag::dim>::type     CellOnFacetRange;
  typedef viennagrid::result_of::iterator<CellOnFacetRange>::type                          CellOnFacetIterator;
  
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Multi-segment domains --" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  DomainHex domain;

  //read a multi-segment mesh using the VTK reader:
  viennagrid::io::vtk_reader<DomainHex>  reader;
  #ifdef _MSC_VER      //Visual Studio builds in a subfolder
  std::string path = "../../examples/data/";
  #else
  std::string path = "../examples/data/";
  #endif
  reader(domain, path + "multi_segment_hex_main.pvd");

  // Obtain references to the two segments.
  SegmentType & seg1 = domain.segments()[0];
  SegmentType & seg2 = domain.segments()[1];
  
  //
  // Iterate over all facets of the domain and find the interface facet:
  // In the same way, one may also traverse interface vertices, etc.
  std::cout << "Facets of the full domain:" << std::endl;
  FacetType * interface_facet = NULL;
  FacetRange facets = viennagrid::ncells<CellTag::dim-1>(domain);
  for (FacetIterator fit = facets.begin(); fit != facets.end(); ++fit)
  {
    if (viennagrid::is_interface(*fit, seg1, seg2))  //three arguments: The element and the two interfacing segments
      interface_facet = &(*fit);
    
    std::cout << *fit << std::endl;
  }
  
  std::cout << "Interface facet: " << std::endl;
  std::cout << *interface_facet << std::endl;
  
  //
  // Now iterate over the cells the facet is member of with respect to the two segments:
  // Mind the second argument to the ncells() function, which is the respective segment
  CellOnFacetRange cells_on_facet_seg1 = viennagrid::ncells(*interface_facet, seg1);
  std::cout << "Cells that share the interface facet in seg1:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_seg1.begin();
                           cofit != cells_on_facet_seg1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }
  
  CellOnFacetRange cells_on_facet_seg2 = viennagrid::ncells(*interface_facet, seg2);
  std::cout << "Cells that share the interface facet in seg2:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_seg2.begin();
                           cofit != cells_on_facet_seg2.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }

  
  //
  // volume, surface and Voronoi information works for segments as well:
  std::cout << "Volumes of segments: " << viennagrid::volume(seg1) << ", " << viennagrid::volume(seg2) << std::endl;
  std::cout << "Surfaces of segments: " << viennagrid::surface(seg1) << ", " << viennagrid::surface(seg2) << std::endl;
  
  // Mind that the default Voronoi keys operate on a domain-wide manner and are thus overwritten at the interface of the two segments
  // As a remedy, we use custom keys for the second segment
  std::string interface_key_seg2 = "interface";
  std::string volume_key_seg2 = "box";
  viennagrid::apply_voronoi(seg1);
  viennagrid::apply_voronoi(seg2, interface_key_seg2, volume_key_seg2);
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
