/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
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

#include "viennagridpp/core.hpp"
#include "viennagridpp/io/netgen_reader.hpp"
#include "viennagridpp/io/vtk_reader.hpp"
#include "viennagridpp/io/vtk_writer.hpp"

#include <typeinfo>

int main()
{
  typedef viennagrid::mesh                                              MeshType;
  typedef viennagrid::result_of::region<MeshType>::type                 RegionType;

  typedef viennagrid::result_of::const_element<MeshType>::type          ConstVertexType;
  typedef viennagrid::result_of::element<MeshType>::type                FacetType;

  typedef viennagrid::result_of::facet_range<MeshType>::type            FacetRange;
  typedef viennagrid::result_of::iterator<FacetRange>::type             FacetIterator;

  typedef viennagrid::result_of::coboundary_range<RegionType>::type     CellOnFacetRange;
  typedef viennagrid::result_of::iterator<CellOnFacetRange>::type       CellOnFacetIterator;

  typedef viennagrid::result_of::neighbor_range<RegionType>::type       NeighborCellRange;
  typedef viennagrid::result_of::iterator<NeighborCellRange>::type      NeighborCellIterator;

  typedef viennagrid::result_of::vertex_range<RegionType>::type         VertexOnSegmentRange;
  typedef viennagrid::result_of::iterator<VertexOnSegmentRange>::type   VertexOnSegmentIterator;

  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Multi-regionment meshs --" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << std::endl;

  MeshType mesh;

  //read a multi-regionment mesh using the VTK reader:
  viennagrid::io::vtk_reader<MeshType>  reader;
  reader(mesh, "../data/multi_segment_hex_main.pvd");

  // Obtain references to the two regionments.
  RegionType region1 = mesh.get_region(0);
  RegionType region2 = mesh.get_region(1);

  //
  // Iterate over all facets of the mesh and find the interface facet:
  // In the same way, one may also traverse interface vertices, etc.
  std::cout << "Facets of the full mesh:" << std::endl;
  FacetType interface_facet;
  FacetRange facets(mesh);
  for (FacetIterator fit = facets.begin(); fit != facets.end(); ++fit)
  {
    FacetType facet = *fit;
    if (viennagrid::is_interface(region1, region2, facet))  //three arguments: The element and the two interfacing regionments
      interface_facet = *fit;

    std::cout << facet << std::endl;
  }

  std::cout << "Interface facet: " << std::endl;
  std::cout << interface_facet << std::endl;

  //
  // Now iterate over the cells the facet is member of with respect to the two regionments:
  // Mind the second argument to the ncells() function, which is the respective regionment
  CellOnFacetRange cells_on_facet_region1(region1, interface_facet, viennagrid::cell_dimension(region1));
  std::cout << "Cells that share the interface facet in region1:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_region1.begin();
                           cofit != cells_on_facet_region1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }

  CellOnFacetRange cells_on_facet_region2(region2, interface_facet, viennagrid::cell_dimension(region2));
  std::cout << "Cells that share the interface facet in region2:" << std::endl;
  for (CellOnFacetIterator cofit = cells_on_facet_region2.begin();
                           cofit != cells_on_facet_region2.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }


  //
  // Now iterate over all cells neighboring the first cell

  NeighborCellRange neigbouring_cells_region1(region1, viennagrid::cells(region1)[0], 0, viennagrid::cell_dimension(region1));
  std::cout << "Neighbor Elements for first cell in region1 (should be no element, because only one element per region)" << std::endl;
  for (NeighborCellIterator cofit = neigbouring_cells_region1.begin();
                           cofit != neigbouring_cells_region1.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }

  NeighborCellRange neigbouring_cells_region2(region2, viennagrid::cells(region2)[0],  0, viennagrid::cell_dimension(region1));
  std::cout << "Neighbor Elements for first cell in region2 (should be no element, because only one element per region)" << std::endl;
  for (NeighborCellIterator cofit = neigbouring_cells_region2.begin();
                           cofit != neigbouring_cells_region2.end();
                         ++cofit)
  {
    std::cout << *cofit << std::endl;
  }



  //
  // volume, surface and Voronoi information works for regionments as well:
//   std::cout << "Volumes of regionments: " << viennagrid::volume(region1) << ", " << viennagrid::volume(region2) << std::endl;
//   std::cout << "Surfaces of regionments: " << viennagrid::surface(region1) << ", " << viennagrid::surface(region2) << std::endl;

  // Compute Voronoi information for each of the two regionments:

//   typedef viennagrid::result_of::const_cell<MeshType>::type    ConstCellType;
//
//   std::deque<double> interface_areas_region1;
//   std::deque<double> interface_areas_region2;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > interface_contributions_region1;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > interface_contributions_region2;
//
//   std::deque<double> vertex_box_volumes_region1;
//   std::deque<double> vertex_box_volumes_region2;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > vertex_box_volume_contributions_region1;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > vertex_box_volume_contributions_region2;
//
//   std::deque<double> edge_box_volumes_region1;
//   std::deque<double> edge_box_volumes_region2;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > edge_box_volume_contributions_region1;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > edge_box_volume_contributions_region2;
//
//
//   // Write Voronoi info to default ViennaData keys:
//   viennagrid::apply_voronoi<CellType>(
//           region1,
//           viennagrid::make_accessor<EdgeType>(interface_areas_region1),
//           viennagrid::make_accessor<EdgeType>(interface_contributions_region1),
//           viennagrid::make_accessor<VertexType>(vertex_box_volumes_region1),
//           viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions_region1),
//           viennagrid::make_accessor<EdgeType>(edge_box_volumes_region1),
//           viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions_region1)
//   );
//   viennagrid::apply_voronoi<CellType>(
//           region2,
//           viennagrid::make_accessor<EdgeType>(interface_areas_region2),
//           viennagrid::make_accessor<EdgeType>(interface_contributions_region2),
//           viennagrid::make_accessor<VertexType>(vertex_box_volumes_region2),
//           viennagrid::make_accessor<VertexType>(vertex_box_volume_contributions_region2),
//           viennagrid::make_accessor<EdgeType>(edge_box_volumes_region2),
//           viennagrid::make_accessor<EdgeType>(edge_box_volume_contributions_region2)
//   );


  //
  // Finally, iterate over the vertices of the mesh and write regionment-based vertex data:
  // As data container std::map<std::size_t, double> is used, where the key is used for the regionment index
  std::deque< double > first_regionment_data;
  std::deque< double > second_regionment_data;

  viennagrid::result_of::accessor< std::deque< double >, ConstVertexType >::type first_regionment_accessor(first_regionment_data);
  viennagrid::result_of::accessor< std::deque< double >, ConstVertexType >::type second_regionment_accessor(second_regionment_data);

  VertexOnSegmentRange vertices_region1(region1);
  for (VertexOnSegmentIterator vosit = vertices_region1.begin();
                             vosit != vertices_region1.end();
                           ++vosit)
  {
    first_regionment_accessor.set(*vosit, 1.0);
  }

  VertexOnSegmentRange vertices_region2(region2);
  for (VertexOnSegmentIterator vosit = vertices_region2.begin();
                             vosit != vertices_region2.end();
                           ++vosit)
  {
    second_regionment_accessor.set(*vosit, 2.0);
  }

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer.add_scalar_data_on_vertices(region1, first_regionment_accessor, "regionment_data" );
  my_vtk_writer.add_scalar_data_on_vertices(region2, second_regionment_accessor, "regionment_data" );
  my_vtk_writer(mesh, "multi_regionment");


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
