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
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/domain/accessor.hpp"

// //Point-based algorithms:
// #include "viennagrid/algorithm/cross_prod.hpp"
// #include "viennagrid/algorithm/inner_prod.hpp"
// #include "viennagrid/algorithm/norm.hpp"
// #include "viennagrid/algorithm/spanned_volume.hpp"
// 
// //Cell-based algorithms:
#include "viennagrid/algorithm/centroid.hpp"
// #include "viennagrid/algorithm/circumcenter.hpp"
// #include "viennagrid/algorithm/surface.hpp"
// #include "viennagrid/algorithm/volume.hpp"
// #include "viennagrid/algorithm/closest_points.hpp"
// #include "viennagrid/algorithm/distance.hpp"
// 
// //Domain-based algorithms:
// #include "viennagrid/algorithm/boundary.hpp"
// #include "viennagrid/algorithm/refine.hpp"
// #include "viennagrid/algorithm/voronoi.hpp"
#include <vector>



int main()
{
  typedef viennagrid::tetrahedral_3d_domain                         DomainType;
  typedef viennagrid::result_of::segmentation<DomainType>::type     SegmentationType;
  typedef viennagrid::result_of::segment<SegmentationType>::type    SegmentType;
  
  typedef viennagrid::result_of::point<DomainType>::type            PointType;
  
  typedef viennagrid::result_of::cell<DomainType>::type             CellType;
  typedef viennagrid::result_of::triangle<DomainType>::type         TriangleType;
  typedef viennagrid::result_of::edge<DomainType>::type             EdgeType;
  typedef viennagrid::result_of::vertex<DomainType>::type           VertexType;
  typedef viennagrid::result_of::vertex_handle<DomainType>::type    VertexHandleType;
    
  typedef viennagrid::result_of::vertex_range<DomainType>::type     VertexRange;
  typedef viennagrid::result_of::cell_range<DomainType>::type       CellRange;

                      
  std::cout << "------------------------------------------------------------ " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Algorithms on points and elements -- " << std::endl;
  std::cout << "------------------------------------------------------------ " << std::endl;
  std::cout << std::endl;
  
  DomainType domain;
  SegmentationType segmentation(domain);
  
  //
  // Read domain from Netgen file
  //
  try
  {
    viennagrid::io::netgen_reader reader;
    reader(domain, segmentation, "../../examples/data/cube48.mesh");
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
  some_cell_data_accessor( viennagrid::cells(domain)[0] ) = 42.0;
  some_cell_data_accessor( viennagrid::cells(domain)[1] ) = 3.14;
  double some_value = some_cell_data_accessor( viennagrid::cells(domain)[1] );
  
  // A helper function for creating an accessor is also provided
  viennagrid::make_accessor<CellType>(some_cell_data_container)( viennagrid::cells(domain)[3] ) = 5.0;
  
  
  
  // Accessor and field differ in their behaviour at access
  // While accessor will fail when accessing the data of an element using a const accessor, a field will return a default value
  std::vector<double> another_cell_data_container;
  // default value will is -1
  viennagrid::result_of::field< std::vector<double>, CellType >::type some_cell_data_field(another_cell_data_container, -1.0);
  
  some_cell_data_field( viennagrid::cells(domain)[0] ) = 12.0;
  some_cell_data_field( viennagrid::cells(domain)[1] ) = 13.0;
  
  // A helper function for creating a field
  viennagrid::make_field<CellType>(some_cell_data_container)( viennagrid::cells(domain)[3] ) = 5.0;
  
  
  // Similar to above, a container can be defined using the accessor_container meta function
  // the default container type is std::vector
  typedef viennagrid::result_of::accessor_container<CellType, double>::type CellDataContainerType;
  CellDataContainerType one_more_cell_data_container;
  viennagrid::result_of::field< CellDataContainerType, CellType >::type one_more_cell_data_accessor(one_more_cell_data_container);
  
  one_more_cell_data_accessor( viennagrid::cells(domain)[0] ) = 1.0;
  one_more_cell_data_accessor( viennagrid::cells(domain)[1] ) = -1.0;
  
  
  // Using an std::map as the underlying container type
  typedef viennagrid::result_of::accessor_container<CellType, double, viennagrid::storage::std_map_tag>::type CellDataMapContainerType;
  CellDataMapContainerType cell_data_map;
  viennagrid::result_of::field< CellDataMapContainerType, CellType >::type cell_data_map_accessor(cell_data_map);
  
  cell_data_map_accessor( viennagrid::cells(domain)[0] ) = 2.5;
  cell_data_map_accessor( viennagrid::cells(domain)[1] ) = -2.5;
  
  
  //
  // Accessing points are also possible using accessor
  //
  viennagrid::result_of::default_point_accessor<DomainType>::type default_point_accessor;
  std::cout << default_point_accessor( viennagrid::vertices(domain)[0] ) << std::endl;
  
  
  
  
  // Many algorithms require accessor for retrieving or storing data
  // e.g. the point accessor can be specified explicitly
  // Otherwise a default point accessor is created for the domain
  
  // A short example where we shift all points
  
  // Defining the container where the new points are stored
  std::vector<PointType> points;
  
  // Creating an accessor for the points
  viennagrid::result_of::accessor< std::vector<PointType>, VertexType >::type point_accessor(points);
  
  typedef viennagrid::result_of::vertex_range<DomainType>::type VertexRangeType;
  typedef viennagrid::result_of::iterator<VertexRangeType>::type VertexRangeIterator;
  
  // Iterating over all vertices
  VertexRangeType vertices = viennagrid::elements(domain);
  for (VertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    point_accessor(*vit) = default_point_accessor(*vit) + PointType(10, 10, 10); // shit point
  
  // Centroid of the domain using the defaul accessor
  std::cout << viennagrid::centroid(domain, default_point_accessor) << std::endl;
  // The same as above
  std::cout << viennagrid::centroid(domain) << std::endl;
  
  // Centroid of the domain using our shifted points
  std::cout << viennagrid::centroid(domain, point_accessor) << std::endl;

  
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
