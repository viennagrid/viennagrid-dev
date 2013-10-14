/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include "viennagrid/mesh/neighbour_iteration.hpp"

#include <typeinfo>

int main()
{
  typedef viennagrid::mesh< viennagrid::config::hexahedral_3d >     MeshType;
  typedef viennagrid::result_of::segmentation<MeshType>::type         SegmentationType;
  typedef SegmentationType::segment_handle_type                                SegmentType;

  typedef viennagrid::result_of::cell_tag<MeshType>::type             CellTag;
  typedef viennagrid::result_of::facet_tag<MeshType>::type            FacetTag;

  typedef viennagrid::result_of::vertex<MeshType>::type               VertexType;
  typedef viennagrid::result_of::edge<MeshType>::type                 EdgeType;
  typedef viennagrid::result_of::facet<MeshType>::type                FacetType;
  typedef viennagrid::result_of::facet_handle<MeshType>::type         FacetHandleType;
  typedef viennagrid::result_of::cell<MeshType>::type                 CellType;

  typedef viennagrid::result_of::facet_range<MeshType>::type          FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type             FacetIterator;

  typedef viennagrid::result_of::coboundary_range<SegmentType, FacetTag, CellTag>::type                 CellOnFacetRange;
  typedef viennagrid::result_of::iterator<CellOnFacetRange>::type                                       CellOnFacetIterator;

  typedef viennagrid::result_of::neighbour_range<SegmentType, CellTag, viennagrid::vertex_tag>::type    NeighbourCellRange;
  typedef viennagrid::result_of::iterator<NeighbourCellRange>::type                                     NeighbourCellIterator;

  typedef viennagrid::result_of::vertex_range<SegmentType>::type        VertexOnSegmentRange;
  typedef viennagrid::result_of::iterator<VertexOnSegmentRange>::type   VertexOnSegmentIterator;

  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Multi-segment meshs --" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << std::endl;

  MeshType mesh;
  SegmentationType segments(mesh);

  //read a multi-segment mesh using the VTK reader:
  viennagrid::io::vtk_reader<MeshType, SegmentationType>  reader;
  reader(mesh, segments, "../data/multi_segment_hex_main.pvd");

  // Obtain references to the two segments.
  SegmentType & seg1 = segments[0];
  SegmentType & seg2 = segments[1];

  //
  // Iterate over all facets of the mesh and find the interface facet:
  // In the same way, one may also traverse interface vertices, etc.
  std::cout << "Facets of the full mesh:" << std::endl;
  FacetHandleType interface_facet_handle;
  FacetRange facets = viennagrid::elements<FacetType>(mesh);
  for (FacetIterator fit = facets.begin(); fit != facets.end(); ++fit)
  {
    FacetType & facet = viennagrid::dereference_handle(mesh, fit.handle());
    if (viennagrid::is_interface(seg1, seg2, facet))  //three arguments: The element and the two interfacing segments
      interface_facet_handle = fit.handle();

    std::cout << facet << std::endl;
  }

  FacetType & interface_facet = viennagrid::dereference_handle(mesh, interface_facet_handle);
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
  std::cout << "Neighbour Elements for first cell in seg1 (should be no element, because only one element per segment)" << std::endl;
  for (NeighbourCellIterator cofit = neigbouring_cells_seg1.begin();
                           cofit != neigbouring_cells_seg1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }

  NeighbourCellRange neigbouring_cells_seg2 = viennagrid::neighbour_elements<CellTag, viennagrid::vertex_tag>(seg2, viennagrid::elements<CellTag>(seg2).handle_at(0));
  std::cout << "Neighbour Elements for first cell in seg2 (should be no element, because only one element per segment)" << std::endl;
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

  typedef viennagrid::result_of::const_cell_handle<MeshType>::type    ConstCellHandleType;

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
  // Finally, iterate over the vertices of the mesh and write segment-based vertex data:
  // As data container std::map<std::size_t, double> is used, where the key is used for the segment index
  std::deque< double > first_segment_data;
  std::deque< double > second_segment_data;

  viennagrid::result_of::field< std::deque< double >, VertexType >::type first_segment_field(first_segment_data);
  viennagrid::result_of::field< std::deque< double >, VertexType >::type second_segment_field(second_segment_data);

  VertexOnSegmentRange vertices_seg1 = viennagrid::elements<viennagrid::vertex_tag>(seg1);
  for (VertexOnSegmentIterator vosit = vertices_seg1.begin();
                             vosit != vertices_seg1.end();
                           ++vosit)
  {
    first_segment_field(*vosit) = 1.0;
  }

  VertexOnSegmentRange vertices_seg2 = viennagrid::elements<viennagrid::vertex_tag>(seg2);
  for (VertexOnSegmentIterator vosit = vertices_seg2.begin();
                             vosit != vertices_seg2.end();
                           ++vosit)
  {
    second_segment_field(*vosit) = 2.0;
  }

  viennagrid::io::vtk_writer<MeshType, SegmentationType> my_vtk_writer;
  my_vtk_writer.add_scalar_data_on_vertices(seg1, first_segment_field, "segment_data" );
  my_vtk_writer.add_scalar_data_on_vertices(seg2, second_segment_field, "segment_data" );
  my_vtk_writer(mesh, segments, "multi_segment");


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
