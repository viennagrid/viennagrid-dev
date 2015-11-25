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

#include "viennagrid/viennagrid.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

//Point-based algorithms:
#include "viennagrid/algorithm/spanned_volume.hpp"

//Cell-based algorithms:
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/closest_points.hpp"
#include "viennagrid/algorithm/distance.hpp"

//Mesh-based algorithms:
#include "viennagrid/algorithm/refine.hpp"



int main()
{
  typedef viennagrid::mesh MeshType;
  typedef viennagrid::result_of::point<MeshType>::type            PointType;

  typedef viennagrid::result_of::element<MeshType>::type          CellType;

  typedef viennagrid::result_of::vertex_range<MeshType>::type     VertexRange;


  std::cout << "------------------------------------------------------------ " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Algorithms on points and elements -- " << std::endl;
  std::cout << "------------------------------------------------------------ " << std::endl;
  std::cout << std::endl;

//   MeshType mesh;
//   SegmentationType segmentation(mesh);
  MeshType mesh;

  //
  // Read mesh from Netgen file
  //
  try
  {
    viennagrid::io::netgen_reader reader;
    reader(mesh, "../../examples/data/cube48.mesh");
  }
  catch (std::exception & e)
  {
    std::cout << "Error reading Netgen mesh file: " << std::endl;
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  //
  // Part 1: Point-based algorithms:
  //

  // Extract the first four points of the mesh:
  VertexRange vertices(mesh);

  PointType p0 = viennagrid::get_point(mesh, vertices[0]);
  PointType p1 = viennagrid::get_point(mesh, vertices[1]);
  PointType p2 = viennagrid::get_point(mesh, vertices[2]);
  PointType p3 = viennagrid::get_point(mesh, vertices[3]);

  std::cout << "Point p0: " << p0 << std::endl;
  std::cout << "Point p1: " << p1 << std::endl;
  std::cout << "Point p2: " << p2 << std::endl;
  std::cout << "Point p3: " << p3 << std::endl;

  // Run a few algorithms:
  std::cout << "Cross-product of p1 and p2: " << viennagrid::cross_prod(p1, p2) << std::endl;
  std::cout << "Inner product of p1 and p2: " << viennagrid::inner_prod(p1, p2) << std::endl;
  std::cout << "1-Norm of p2: "               << viennagrid::norm_1(p2) << std::endl;
  std::cout << "2-Norm of p2: "               << viennagrid::norm_2(p2) << std::endl;
  std::cout << "Inf-Norm of p2: "             << viennagrid::norm_inf(p2) << std::endl;
  std::cout << "Length of line [p0, p1]: "    << viennagrid::spanned_volume(p0, p1) << std::endl;
  std::cout << "Area of triangle [p0, p1, p2]: " << viennagrid::spanned_volume(p0, p1, p2) << std::endl;
  std::cout << "Volume of tetrahedron [p0, p1, p2, p3]: " << viennagrid::spanned_volume(p0, p1, p2, p3) << std::endl;

  std::cout << std::endl << "--------------------------------" << std::endl << std::endl;


  //
  // Part 2: Cell-based algorithms:
  //

  // Extract first cell from mesh:
  CellType cell = viennagrid::cells(mesh)[0];

  std::cout << "Cell: " << std::endl;
  std::cout << cell << std::endl;

  // Run algorithms:
  std::cout << "Centroid of cell: "     << viennagrid::centroid(cell) << std::endl;
  std::cout << "Circumcenter of cell: " << viennagrid::circumcenter(cell) << std::endl;
  std::cout << "Surface of cell: "      << viennagrid::surface(cell) << std::endl;
  std::cout << "Volume of cell: "       << viennagrid::volume(cell) << std::endl;
  std::cout << std::endl;

  std::cout << "Volume of mesh: "       << viennagrid::volume(mesh) << std::endl;
  std::cout << "Surface of mesh: "       << viennagrid::surface(mesh) << std::endl;


  //
  // Part 3: Mesh-based algorithms (except interfaces. Refer to the multi-segment tutorial multi_segment.cpp)
  //


//   // Write Voronoi info to default ViennaData keys:
//   typedef viennagrid::result_of::const_cell<MeshType>::type    ConstCellType;
//
//   // Defining container for storing voronoi information
//   std::deque<double> interface_areas;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > interface_contributions;
//
//   std::deque<double> vertex_box_volumes;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > vertex_box_volume_contributions;
//
//   std::deque<double> edge_box_volumes;
//   std::deque< viennagrid::result_of::voronoi_cell_contribution<ConstCellType>::type > edge_box_volume_contributions;
//
//   viennagrid::apply_voronoi<CellType>(
//           mesh,
//           viennagrid::make_field<EdgeType>(interface_areas),
//           viennagrid::make_field<EdgeType>(interface_contributions),
//           viennagrid::make_field<VertexType>(vertex_box_volumes),
//           viennagrid::make_field<VertexType>(vertex_box_volume_contributions),
//           viennagrid::make_field<EdgeType>(edge_box_volumes),
//           viennagrid::make_field<EdgeType>(edge_box_volume_contributions)
//   );
//
//   // Write Voronoi info again, this time using custom keys
//   std::cout << "Voronoi box volume at first vertex: "
//             << viennagrid::make_field<VertexType>(vertex_box_volumes)( vertices[0] )
//             << std::endl;





  //
  // Refine mesh uniformly:
  MeshType uniformly_refined_mesh;
  viennagrid::cell_refine_uniformly(mesh, uniformly_refined_mesh);

  {
    viennagrid::io::vtk_writer<MeshType> writer;
    writer(uniformly_refined_mesh, "uniform_refinement");
  }



  //
  // Refine only specific cells:
  MeshType adaptively_refined_mesh;

  std::vector<viennagrid_element_id> cell_refinement_flags;
  cell_refinement_flags.push_back( viennagrid::cells(mesh)[0].id().internal() );
  cell_refinement_flags.push_back( viennagrid::cells(mesh)[3].id().internal() );
  cell_refinement_flags.push_back( viennagrid::cells(mesh)[8].id().internal() );

  // refining the mesh using the accessor representing the marked cells
  viennagrid::element_refine(mesh, cell_refinement_flags.size(), &cell_refinement_flags[0], adaptively_refined_mesh);

  {
    viennagrid::io::vtk_writer<MeshType> writer;
    writer(adaptively_refined_mesh, "adaptively_refinement");
  }



  //
  // Get boundary information of first vertex with respect to the full mesh:

  for (VertexRange::iterator it = vertices.begin(); it != vertices.end(); ++it)
    std::cout << *it << " " << viennagrid::get_point(mesh, *it) << " "
        << viennagrid::is_boundary(mesh, *it)    //second argument is the enclosing complex (either a mesh or a segment)
        << std::endl << std::endl;



  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
