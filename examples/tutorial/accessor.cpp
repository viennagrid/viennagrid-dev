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

#include "viennagrid/core.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennagrid/algorithm/centroid.hpp"



int main()
{
  typedef viennagrid::mesh_t                                      MeshType;
//   typedef viennagrid::result_of::segmentation<MeshType>::type     SegmentationType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;

  typedef viennagrid::result_of::element<MeshType>::type          CellType;
  typedef viennagrid::result_of::element<MeshType>::type          VertexType;

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


  // Often additional data has to be stored on elements: physical data, refinement flags, ...
  // Accessors and fields wrap dada access on container were this data is stored

  // Create a container for double values
  std::vector<double> some_cell_data_container;
  // Create an accessor which wrapps access for cells
  // - the first argument in the meta function is the wrapped container
  // - the second argument in the meta function is the type on which the data is stored
  // The constructor of the accessor takes the container
  viennagrid::result_of::accessor< std::vector<double>, CellType >::type some_cell_data_accessor(some_cell_data_container);

  // simply using the accessor with operator()
  some_cell_data_accessor( viennagrid::cells(mesh)[0] ) = 42.0;
  some_cell_data_accessor( viennagrid::cells(mesh)[1] ) = 3.14;
  double some_value = some_cell_data_accessor( viennagrid::cells(mesh)[1] );
  std::cout << "Value for first cell (should be 3.14) = " << some_value << std::endl;

  // A helper function for creating an accessor is also provided
  viennagrid::make_accessor<CellType>(some_cell_data_container)( viennagrid::cells(mesh)[3] ) = 5.0;



  // Accessor and field differ in their behaviour at access
  // While accessor will fail when accessing the data of an element using a const accessor, a field will return a default value
  std::vector<double> another_cell_data_container;
  // default value will is -1
  viennagrid::result_of::field< std::vector<double>, CellType >::type some_cell_data_field(another_cell_data_container, -1.0);

  some_cell_data_field( viennagrid::cells(mesh)[0] ) = 12.0;
  some_cell_data_field( viennagrid::cells(mesh)[1] ) = 13.0;

  // A helper function for creating a field
  viennagrid::make_field<CellType>(some_cell_data_container)( viennagrid::cells(mesh)[3] ) = 5.0;


  // Similar to above, a container can be defined using the accessor_container meta function
  // the default container type is std::vector
  typedef viennagrid::result_of::accessor_container<CellType, double>::type CellDataContainerType;
  CellDataContainerType one_more_cell_data_container;
  viennagrid::result_of::field< CellDataContainerType, CellType >::type one_more_cell_data_accessor(one_more_cell_data_container);

  one_more_cell_data_accessor( viennagrid::cells(mesh)[0] ) = 1.0;
  one_more_cell_data_accessor( viennagrid::cells(mesh)[1] ) = -1.0;


  // Using an std::map as the underlying container type
  typedef viennagrid::result_of::accessor_container<CellType, double, viennagrid::std_map_tag>::type CellDataMapContainerType;
  CellDataMapContainerType cell_data_map;
  viennagrid::result_of::field< CellDataMapContainerType, CellType >::type cell_data_map_accessor(cell_data_map);

  cell_data_map_accessor( viennagrid::cells(mesh)[0] ) = 2.5;
  cell_data_map_accessor( viennagrid::cells(mesh)[1] ) = -2.5;


  //
  // Accessing points are also possible using accessor
  //
  viennagrid::result_of::point_accessor<MeshType>::type mesh_point_accessor = viennagrid::point_accessor(mesh);
  std::cout << mesh_point_accessor( viennagrid::vertices(mesh)[0] ) << std::endl;




  // Many algorithms require accessor for retrieving or storing data
  // e.g. the point accessor can be specified explicitly
  // Otherwise a default point accessor is created for the mesh

  // A short example where we shift all points

  // Defining the container where the new points are stored
  std::vector<PointType> points;

  // Creating an accessor for the points
  viennagrid::result_of::accessor< std::vector<PointType>, VertexType >::type point_accessor(points);

  typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
  typedef viennagrid::result_of::iterator<VertexRangeType>::type VertexRangeIterator;

  // Iterating over all vertices
  VertexRangeType vertices(mesh);// = viennagrid::elements(mesh);
  for (VertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
    point_accessor(*vit).resize(3);
    point_accessor(*vit) = mesh_point_accessor(*vit) + viennagrid::make_point(10, 10, 10); // shit point
  }

  // Centroid of the mesh using the defaul accessor
  std::cout << viennagrid::centroid(mesh_point_accessor, mesh) << std::endl;

  // Centroid of the mesh using our shifted points
  std::cout << viennagrid::centroid(point_accessor, mesh) << std::endl;



  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
