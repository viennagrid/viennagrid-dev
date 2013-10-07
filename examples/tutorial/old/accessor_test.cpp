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
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/netgen_reader.hpp"

//Mesh-based algorithms:
#include "viennagrid/algorithm/boundary.hpp"

// #include "viennadata/api.hpp"


int main()
{


  typedef viennagrid::tetrahedral_3d_mesh             Mesh;
  typedef viennagrid::result_of::segmentation<Mesh>::type Segmentation;
//   typedef viennagrid::result_of::mesh_view<Mesh>::type   Segment;

  typedef viennagrid::tetrahedron_tag CellTag;

  typedef viennagrid::result_of::element<Mesh, viennagrid::tetrahedron_tag>::type    CellType;
  typedef viennagrid::result_of::element<Mesh, viennagrid::triangle_tag>::type       TriangleType;
  typedef viennagrid::result_of::element<Mesh, viennagrid::line_tag>::type           EdgeType;
  typedef viennagrid::result_of::element<Mesh, viennagrid::vertex_tag>::type         VertexType;
  typedef viennagrid::result_of::handle<Mesh, viennagrid::vertex_tag>::type    VertexHandleType;

  typedef viennagrid::result_of::element_range<Mesh, viennagrid::vertex_tag>::type       VertexRange;
  typedef viennagrid::result_of::element_range<Mesh, viennagrid::tetrahedron_tag>::type       CellRange;


  std::cout << "------------------------------------------------------------ " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Algorithms on points and elements -- " << std::endl;
  std::cout << "------------------------------------------------------------ " << std::endl;
  std::cout << std::endl;

  Mesh mesh;
  Segmentation segmentation(mesh);
//   std::vector<Segment> segments;

  //
  // Read mesh from Netgen file
  //
  try
  {
    viennagrid::io::netgen_reader reader;
    reader(mesh, segmentation, "../../examples/data/cube48.mesh");
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
  VertexRange vertices = viennagrid::elements(mesh);



  //
  // Get boundary information of first vertex with respect to the full mesh:

//   viennagrid::detect_boundary<viennagrid::tetrahedron_tag>(mesh);

  for (VertexRange::iterator it = vertices.begin(); it != vertices.end(); ++it)
    std::cout << *it << " " << viennagrid::point(mesh, *it) << " "
            << viennagrid::is_boundary(mesh, *it)    //second argument is the enclosing complex (either a mesh or a segment)
            << std::endl << std::endl;




  std::vector<bool> facet_boundary_marker;
  viennadata::container_accessor<
    std::vector<bool>,
    viennagrid::result_of::facet< viennagrid::result_of::cell<Mesh>::type >::type,
    viennadata::id_access_tag> acc(facet_boundary_marker);

  std::vector<bool> vertex_boundary_marker;
  viennadata::container_accessor<
    std::vector<bool>,
    viennagrid::result_of::vertex<Mesh>::type,
    viennadata::id_access_tag> acc_vertex(vertex_boundary_marker);




//   typedef viennameta::make_typemap< viennagrid::vertex_tag, bool, viennagrid::line_tag, bool, viennagrid::triangle_tag, bool >::type metainfo_collection_element_list;
//   typedef viennagrid::result_of::metainfo_container_typemap<metainfo_collection_element_list, viennagrid::storage::default_container_config, Mesh>::type metainfo_typemap;
//   typedef viennagrid::storage::collection_t<metainfo_typemap> metainfo_collection_type;
//
//   metainfo_collection_type metainfo_collection;

//   viennagrid::metainfo_collection_accessor<metainfo_collection_type> accessor(metainfo_collection);
  viennagrid::detect_boundary(mesh, acc);


  viennagrid::transfer_boundary_information( mesh, acc, acc_vertex );

  for (VertexRange::iterator it = vertices.begin(); it != vertices.end(); ++it)
    std::cout << *it << " " << viennagrid::point(mesh, *it) << " "
            << viennagrid::is_boundary(acc_vertex, *it)    //second argument is the enclosing complex (either a mesh or a segment)
            << std::endl << std::endl;


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
