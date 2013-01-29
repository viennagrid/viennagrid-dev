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
//#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"

#include <typeinfo>

int main()
{
    typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > PointType;  //use this for a 3d examples
    typedef viennagrid::hexahedron_tag                           CellTag;
    typedef viennagrid::result_of::default_topologic_config<viennagrid::hexahedron_tag, viennagrid::storage::pointer_hook_tag>::type TopologicConfig;
    typedef viennagrid::result_of::geometric_domain_config<viennagrid::hexahedron_tag, PointType, viennagrid::storage::id_hook_tag >::type DomainConfig;
    typedef viennagrid::result_of::geometric_domain< DomainConfig >::type DomainType;
    typedef viennagrid::result_of::geometric_view<DomainType>::type SegmentType;
  
  typedef viennagrid::result_of::point_type<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type       VertexType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::line_tag>::type       EdgeType;
  typedef viennagrid::result_of::element<DomainType, CellTag::facet_tag>::type FacetType;
  typedef viennagrid::result_of::element_hook<DomainType, CellTag::facet_tag>::type FacetHookType;
  typedef viennagrid::result_of::element<DomainType, CellTag>::type   CellType;
                                            
  typedef viennagrid::result_of::element_range<DomainType, viennagrid::vertex_tag>::type       VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
                                            
  typedef viennagrid::result_of::element_range<DomainType, CellTag::facet_tag>::type      FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type                     FacetIterator;
  typedef viennagrid::result_of::hook_iterator<FacetRange>::type                     FacetHookIterator;
  
  typedef viennagrid::result_of::coboundary_range<SegmentType, CellTag>::type     CellOnFacetRange;
  typedef viennagrid::result_of::iterator<CellOnFacetRange>::type               CellOnFacetIterator;

  typedef viennagrid::result_of::element_range<SegmentType, viennagrid::vertex_tag>::type              VertexOnSegmentRange;
  typedef viennagrid::result_of::iterator<VertexOnSegmentRange>::type           VertexOnSegmentIterator;
  
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Multi-segment domains --" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  DomainType domain;
  std::vector<SegmentType> segments;

  //read a multi-segment mesh using the VTK reader:
  viennagrid::io::vtk_reader<CellTag, DomainType>  reader;
  #ifdef _MSC_VER      //Visual Studio builds in a subfolder
  std::string path = "../../examples/data/";
  #else
  std::string path = "../examples/data/";
  #endif
  reader(domain, segments, path + "multi_segment_hex_main.pvd");

  // Obtain references to the two segments.
  SegmentType & seg1 = segments[0];
  SegmentType & seg2 = segments[1];
  
  //
  // Iterate over all facets of the domain and find the interface facet:
  // In the same way, one may also traverse interface vertices, etc.
  std::cout << "Facets of the full domain:" << std::endl;
  FacetHookType interface_facet_hook;
  FacetRange facets = viennagrid::elements<FacetType>(domain);  
  for (FacetHookIterator fit = facets.begin(); fit != facets.end(); ++fit)
  {
    FacetType & facet = viennagrid::dereference_hook(domain, *fit);
    if (viennagrid::is_interface<CellTag>(facet, seg1, seg2))  //three arguments: The element and the two interfacing segments
      interface_facet_hook = *fit;
    
    std::cout << facet << std::endl;
  }
  
  FacetType & interface_facet = viennagrid::dereference_hook(domain, interface_facet_hook);
  std::cout << "Interface facet: " << std::endl;
  std::cout << interface_facet << std::endl;
  
  //
  // Now iterate over the cells the facet is member of with respect to the two segments:
  // Mind the second argument to the ncells() function, which is the respective segment
  CellOnFacetRange cells_on_facet_seg1 = viennagrid::coboundary_elements<CellTag>(seg1, interface_facet_hook);
  std::cout << "Cells that share the interface facet in seg1:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_seg1.begin();
                           cofit != cells_on_facet_seg1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }
  
  CellOnFacetRange cells_on_facet_seg2 = viennagrid::coboundary_elements<CellTag>(seg2, interface_facet_hook);
  std::cout << "Cells that share the interface facet in seg2:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_seg2.begin();
                           cofit != cells_on_facet_seg2.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }

  
  //
  // volume, surface and Voronoi information works for segments as well:
  std::cout << "Volumes of segments: " << viennagrid::volume<CellTag>(seg1) << ", " << viennagrid::volume<CellTag>(seg2) << std::endl;
  std::cout << "Surfaces of segments: " << viennagrid::surface<CellTag>(seg1) << ", " << viennagrid::surface<CellTag>(seg2) << std::endl;
  
  // Mind that the default Voronoi keys operate on a domain-wide manner and are thus overwritten at the interface of the two segments
  // As a remedy, we use custom keys for the second segment
  std::string interface_key_seg2 = "interface";
  std::string volume_key_seg2 = "box";
  viennagrid::apply_voronoi<CellTag>(seg1);
  viennagrid::apply_voronoi<CellTag>(seg2, interface_key_seg2, volume_key_seg2);

  
  //
  // Finally, iterate over the vertices of the domain and write segment-based vertex data:
  // As data container std::map<std::size_t, double> is used, where the key is used for the segment index 
  VertexOnSegmentRange vertices_seg1 = viennagrid::elements<viennagrid::vertex_tag>(seg1);
  for (VertexOnSegmentIterator vosit = vertices_seg1.begin();
                             vosit != vertices_seg1.end();
                           ++vosit)
  {
    viennadata::access<std::string, std::map<std::size_t, double> >("segment_data")(*vosit)[0] = 1.0;
  }
  
  VertexOnSegmentRange vertices_seg2 = viennagrid::elements<viennagrid::vertex_tag>(seg2);
  for (VertexOnSegmentIterator vosit = vertices_seg2.begin();
                             vosit != vertices_seg2.end();
                           ++vosit)
  {
    viennadata::access<std::string, std::map<std::size_t, double> >("segment_data")(*vosit)[1] = 2.0;
  }
  
  viennagrid::io::vtk_writer<DomainType, CellType> my_vtk_writer;
  viennagrid::io::add_scalar_data_on_vertices_per_segment<std::string, 
                                                          std::map<std::size_t, double> >
                                                         (my_vtk_writer,
                                                          "segment_data",
                                                          "segment_data");
  my_vtk_writer(domain, segments, "multi_segment");                                                       
  
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
