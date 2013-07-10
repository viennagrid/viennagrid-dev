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
#include "viennagrid/config/domain_config.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"
#include "viennagrid/domain/neighbour_iteration.hpp"

#include <typeinfo>

int main()
{
  //typedef viennagrid::config::result_of::full_domain_config<viennagrid::hexahedron_tag, PointType, viennagrid::storage::id_handle_tag >::type DomainConfig;

  typedef viennagrid::hexahedron_tag                                   CellTag;
  typedef viennagrid::domain_t< viennagrid::config::hexahedral_3d >    DomainType;
  typedef viennagrid::result_of::domain_view<DomainType>::type         DomainViewType;
  typedef viennagrid::result_of::segmentation<DomainType>::type        SegmentationType;
  typedef SegmentationType::segment_type                               SegmentType;
  
  typedef viennagrid::result_of::point<DomainType>::type          PointType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type     VertexType;
  typedef viennagrid::result_of::element<DomainType, viennagrid::line_tag>::type       EdgeType;
  typedef viennagrid::result_of::element<DomainType, CellTag::facet_tag>::type         FacetType;
  typedef viennagrid::result_of::handle<DomainType,  CellTag::facet_tag>::type         FacetHandleType;
  typedef viennagrid::result_of::element<DomainType, CellTag>::type                    CellType;
                                            
  typedef viennagrid::result_of::element_range<DomainType, viennagrid::vertex_tag>::type  VertexRange;
  typedef viennagrid::result_of::iterator<VertexRange>::type                              VertexIterator;
                                            
  typedef viennagrid::result_of::element_range<DomainType, CellTag::facet_tag>::type      FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type                               FacetIterator;
  typedef viennagrid::result_of::handle_iterator<FacetRange>::type                        FacetHandleIterator;
  
  typedef viennagrid::result_of::coboundary_range<SegmentType, CellTag::facet_tag, CellTag>::type   CellOnFacetRange;
  typedef viennagrid::result_of::iterator<CellOnFacetRange>::type                                   CellOnFacetIterator;
  
  typedef viennagrid::result_of::neighbour_range<SegmentType, CellTag, viennagrid::vertex_tag>::type   NeighbourCellRange;
  typedef viennagrid::result_of::iterator<NeighbourCellRange>::type                                   NeighbourCellIterator;

  typedef viennagrid::result_of::element_range<SegmentType, viennagrid::vertex_tag>::type    VertexOnSegmentRange;
  typedef viennagrid::result_of::iterator<VertexOnSegmentRange>::type                        VertexOnSegmentIterator;
  
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Multi-segment domains --" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  DomainType domain;
  SegmentationType segments(domain);

  //read a multi-segment mesh using the VTK reader:
  viennagrid::io::vtk_reader<DomainType, SegmentationType>  reader;
  reader(domain, segments, "../data/multi_segment_hex_main.pvd");

  // Obtain references to the two segments.
  SegmentType & seg1 = segments[0];
  SegmentType & seg2 = segments[1];
  
  //
  // Iterate over all facets of the domain and find the interface facet:
  // In the same way, one may also traverse interface vertices, etc.
  std::cout << "Facets of the full domain:" << std::endl;
  FacetHandleType interface_facet_handle;
  FacetRange facets = viennagrid::elements<FacetType>(domain);  
  for (FacetHandleIterator fit = facets.begin(); fit != facets.end(); ++fit)
  {
    FacetType & facet = viennagrid::dereference_handle(domain, *fit);
    if (viennagrid::is_interface(seg1, seg2, facet))  //three arguments: The element and the two interfacing segments
      interface_facet_handle = *fit;
    
    std::cout << facet << std::endl;
  }
  
  FacetType & interface_facet = viennagrid::dereference_handle(domain, interface_facet_handle);
  std::cout << "Interface facet: " << std::endl;
  std::cout << interface_facet << std::endl;
  
  //
  // Now iterate over the cells the facet is member of with respect to the two segments:
  // Mind the second argument to the ncells() function, which is the respective segment
  CellOnFacetRange cells_on_facet_seg1 = viennagrid::coboundary_elements<FacetType, CellTag>(seg1, interface_facet_handle);
  std::cout << "Cells that share the interface facet in seg1:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_seg1.begin();
                           cofit != cells_on_facet_seg1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }
  
  CellOnFacetRange cells_on_facet_seg2 = viennagrid::coboundary_elements<FacetType, CellTag>(seg2, interface_facet_handle);
  std::cout << "Cells that share the interface facet in seg2:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_seg2.begin();
                           cofit != cells_on_facet_seg2.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }
  
  
  //
  // Now iterate over all cells neighbouring the first cell
  
  NeighbourCellRange neigbouring_cells_seg1 = viennagrid::neighbour_elements<CellTag, viennagrid::vertex_tag>(seg1, viennagrid::elements<CellTag>(seg1).handle_at(0));
  std::cout << "Neighbour Elements for first cell in seg1 (should be no elements, because only one element per segment)" << std::endl;
  for (NeighbourCellIterator cofit = neigbouring_cells_seg1.begin();
                           cofit != neigbouring_cells_seg1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }
  
  NeighbourCellRange neigbouring_cells_seg2 = viennagrid::neighbour_elements<CellTag, viennagrid::vertex_tag>(seg2, viennagrid::elements<CellTag>(seg2).handle_at(0));
  std::cout << "Neighbour Elements for first cell in seg2 (should be no elements, because only one element per segment)" << std::endl;
  for (NeighbourCellIterator cofit = neigbouring_cells_seg2.begin();
                           cofit != neigbouring_cells_seg2.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }
  

  
  //
  // volume, surface and Voronoi information works for segments as well:
  std::cout << "Volumes of segments: " << viennagrid::volume(seg1) << ", " << viennagrid::volume(seg2) << std::endl;
  std::cout << "Surfaces of segments: " << viennagrid::surface(seg1) << ", " << viennagrid::surface(seg2) << std::endl;
  
  // Compute Voronoi information for each of the two segments:

  typedef viennagrid::result_of::const_handle<DomainType, CellType>::type    ConstCellHandleType;
  
  std::deque<double> interface_areas_seg1;
  std::deque<double> interface_areas_seg2;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > interface_contributions_seg1;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > interface_contributions_seg2;
  
  std::deque<double> vertex_box_volumes_seg1;
  std::deque<double> vertex_box_volumes_seg2;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > vertex_box_volume_contributions_seg1;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > vertex_box_volume_contributions_seg2;
  
  std::deque<double> edge_box_volumes_seg1;
  std::deque<double> edge_box_volumes_seg2;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > edge_box_volume_contributions_seg1;
  std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellHandleType>::type > edge_box_volume_contributions_seg2;
  
  
  // Write Voronoi info to default ViennaData keys:
  viennagrid::apply_voronoi<CellType>(
          seg1,
          viennagrid::make_accessor<EdgeType>(interface_areas_seg1),
          viennagrid::make_accessor<EdgeType>(interface_contributions_seg1),
          viennagrid::make_accessor<VertexType>(vertex_box_volumes_seg1),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions_seg1),
          viennagrid::make_accessor<EdgeType>(edge_box_volumes_seg1),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions_seg1)
  );
  viennagrid::apply_voronoi<CellType>(
          seg2,
          viennagrid::make_accessor<EdgeType>(interface_areas_seg2),
          viennagrid::make_accessor<EdgeType>(interface_contributions_seg2),
          viennagrid::make_accessor<VertexType>(vertex_box_volumes_seg2),
          viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions_seg2),
          viennagrid::make_accessor<EdgeType>(edge_box_volumes_seg2),
          viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions_seg2)
  );

  
  //
  // Finally, iterate over the vertices of the domain and write segment-based vertex data:
  // As data container std::map<std::size_t, double> is used, where the key is used for the segment index 
  std::deque< double > first_segment_data;
  std::deque< double > second_segment_data;

  VertexOnSegmentRange vertices_seg1 = viennagrid::elements<viennagrid::vertex_tag>(seg1);
  for (VertexOnSegmentIterator vosit = vertices_seg1.begin();
                             vosit != vertices_seg1.end();
                           ++vosit)
  {
    viennagrid::make_accessor<VertexType>(first_segment_data)(*vosit) = 1.0;
//     first_segment_data.push_back(1.0);
  }
  
  VertexOnSegmentRange vertices_seg2 = viennagrid::elements<viennagrid::vertex_tag>(seg2);
  for (VertexOnSegmentIterator vosit = vertices_seg2.begin();
                             vosit != vertices_seg2.end();
                           ++vosit)
  {
    viennagrid::make_accessor<VertexType>(second_segment_data)(*vosit) = 1.0;
//     second_segment_data.push_back(2.0);
  }
  
  viennagrid::io::vtk_writer<DomainType, SegmentationType> my_vtk_writer;  
  my_vtk_writer.add_scalar_data_on_vertices(seg1, viennagrid::make_accessor<VertexType>(first_segment_data), "segment_data" );
  my_vtk_writer.add_scalar_data_on_vertices(seg2, viennagrid::make_accessor<VertexType>(second_segment_data), "segment_data" );
  my_vtk_writer(domain, segments, "multi_segment");
  
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
