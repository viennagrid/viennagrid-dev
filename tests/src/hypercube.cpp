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
#include "viennagridpp/io/vtk_writer.hpp"

template <typename MeshType>
void setup(MeshType & mesh, viennagrid::element_tag et)
{
  typedef typename viennagrid::result_of::point<MeshType>::type               PointType;
  typedef typename viennagrid::result_of::element<MeshType>::type       VertexType;

  if (et.is_hexahedron())
  {
    PointType p0 = viennagrid::make_point(0.0, 0.0, 0.0);
    PointType p1 = viennagrid::make_point(1.0, 0.0, 0.0);
    PointType p2 = viennagrid::make_point(1.0, 1.0, 0.0);
    PointType p3 = viennagrid::make_point(0.0, 1.0, 0.0);
    PointType p4 = viennagrid::make_point(0.0, 0.0, 1.0);
    PointType p5 = viennagrid::make_point(1.0, 0.0, 1.0);
    PointType p6 = viennagrid::make_point(1.0, 1.0, 1.0);
    PointType p7 = viennagrid::make_point(0.0, 1.0, 1.0);
    PointType p8 = viennagrid::make_point(2.0, 0.0, 0.0);
    PointType p9 = viennagrid::make_point(2.0, 1.0, 0.0);
    PointType p10 = viennagrid::make_point(2.0, 0.0, 1.0);
    PointType p11 = viennagrid::make_point(2.0, 1.0, 1.0);

    std::cout << "Adding vertices to segment:" << std::endl;


    VertexType v0 = viennagrid::make_vertex( mesh, p0 );
    VertexType v1 = viennagrid::make_vertex( mesh, p1 );
    VertexType v2 = viennagrid::make_vertex( mesh, p2 );
    VertexType v3 = viennagrid::make_vertex( mesh, p3 );
    VertexType v4 = viennagrid::make_vertex( mesh, p4 );
    VertexType v5 = viennagrid::make_vertex( mesh, p5 );
    VertexType v6 = viennagrid::make_vertex( mesh, p6 );
    VertexType v7 = viennagrid::make_vertex( mesh, p7 );
    VertexType v8 = viennagrid::make_vertex( mesh, p8 );
    VertexType v9 = viennagrid::make_vertex( mesh, p9 );
    VertexType v10 = viennagrid::make_vertex( mesh, p10 );
    VertexType v11 = viennagrid::make_vertex( mesh, p11 );


    viennagrid::make_hexahedron( mesh, v0, v1, v3, v2, v4, v5, v7, v6 );
    viennagrid::make_hexahedron( mesh, v1, v8, v2, v9, v5, v10, v6, v11 );
  }

  if (et.is_quadrilateral())
  {
    PointType p0 = viennagrid::make_point(0.0, 0.0);
    PointType p1 = viennagrid::make_point(1.0, 0.0);
    PointType p2 = viennagrid::make_point(2.0, 0.0);
    PointType p3 = viennagrid::make_point(2.0, 1.0);
    PointType p4 = viennagrid::make_point(1.0, 1.0);
    PointType p5 = viennagrid::make_point(0.0, 1.0);

    std::cout << "Adding vertices to segment:" << std::endl;
    VertexType v0 = viennagrid::make_vertex( mesh, p0 );
    VertexType v1 = viennagrid::make_vertex( mesh, p1 );
    VertexType v2 = viennagrid::make_vertex( mesh, p2 );
    VertexType v3 = viennagrid::make_vertex( mesh, p3 );
    VertexType v4 = viennagrid::make_vertex( mesh, p4 );
    VertexType v5 = viennagrid::make_vertex( mesh, p5 );

    viennagrid::make_quadrilateral( mesh, v0, v1, v5, v4 );
    viennagrid::make_quadrilateral( mesh, v1, v2, v4, v3 );
  }
}


void test(std::string outfile, viennagrid::element_tag et)
{
  typedef viennagrid::mesh                                                MeshType;

  typedef viennagrid::result_of::vertex_range<MeshType>::type             VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type          VertexIterator;

  typedef viennagrid::result_of::element_range<MeshType, 1>::type         EdgeContainer;
  typedef viennagrid::result_of::iterator<EdgeContainer>::type            EdgeIterator;

  typedef viennagrid::result_of::facet_range<MeshType>::type              FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type           FacetIterator;

  typedef viennagrid::result_of::cell_range<MeshType>::type               CellContainer;
  typedef viennagrid::result_of::iterator<CellContainer>::type            CellIterator;

  MeshType mesh;

  setup(mesh, et);

  std::cout << "Vertices: " << std::endl;
  VertexContainer vertices(mesh);
  for (VertexIterator vit = vertices.begin();
        vit != vertices.end();
        ++vit)
      std::cout << *vit << std::endl;

  std::cout << "Edges: " << std::endl;
  EdgeContainer edges(mesh);
  for (EdgeIterator eit = edges.begin();
        eit != edges.end();
        ++eit)
      std::cout << *eit << std::endl;

  std::cout << "Facets: " << std::endl;
  FacetContainer facets(mesh);
  for (FacetIterator fit = facets.begin();
        fit != facets.end();
        ++fit)
      std::cout << *fit << std::endl;

  std::cout << "Cells: " << std::endl;
  CellContainer cells(mesh);
  for (CellIterator cit = cells.begin();
        cit != cells.end();
        ++cit)
      std::cout << *cit << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, outfile);

}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  test("hypercube_2d", viennagrid::quadrilateral_tag());
  test("hypercube_3d", viennagrid::hexahedron_tag());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
