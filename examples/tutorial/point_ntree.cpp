#include "viennagridpp/core.hpp"
#include "viennagridpp/io/vtk_reader.hpp"
#include "viennagridpp/io/netgen_reader.hpp"
#include "viennagridpp/ntree.hpp"


int main()
{
  typedef viennagrid::mesh_t MeshType;

  MeshType mesh;

  viennagrid::io::netgen_reader reader;
  reader(mesh, "../data/square32.mesh");    //use this for a 3d example

  typedef viennagrid::result_of::point<MeshType>::type PointType;
  typedef viennagrid::result_of::element<MeshType>::type ElementType;
  typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
  typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;


  PointType min( viennagrid::geometric_dimension(mesh) );
  PointType max( viennagrid::geometric_dimension(mesh) );

  ConstVertexRangeType vertices(mesh);
  std::cout << "Number of vertices: " << vertices.size() << std::endl;
  for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
    PointType p = viennagrid::get_point(*vit);
    min = viennagrid::min(min, p);
    max = viennagrid::max(max, p);
  }

  PointType middle = (max+min)/2;
  PointType size = max-min;

  typedef viennagrid::ntree_node<ElementType> NodeType;

  NodeType * root = new NodeType( middle-size/2*1.1 , middle+size/2*1.1 );
  for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
//     root->add( viennagrid::element_wrapper<ElementType>(*vit), 5, 5);
    root->add( viennagrid::vertex_with_distance_wrapper<ElementType>(*vit, 0.1), 5, 5);
  }

  root->print();

  std::cout << "Vertex count = " << vertices.size() << std::endl;
  std::cout << "Element count = " << root->element_count() << std::endl;

  return EXIT_SUCCESS;
}







