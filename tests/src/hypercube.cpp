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

#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"

template <typename DomainType>
void setup(DomainType & domain, viennagrid::hexahedron_tag)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type            CellTag;
  typedef typename viennagrid::result_of::point<DomainType>::type               PointType;
  typedef typename viennagrid::result_of::vertex<DomainType>::type              VertexType;
  typedef typename viennagrid::result_of::vertex_handle<DomainType>::type       VertexHandleType;
  typedef typename viennagrid::result_of::cell<DomainType>::type                CellType;
  
  PointType p0(0.0, 0.0, 0.0);
  PointType p1(1.0, 0.0, 0.0);
  PointType p2(1.0, 1.0, 0.0);
  PointType p3(0.0, 1.0, 0.0);
  PointType p4(0.0, 0.0, 1.0);
  PointType p5(1.0, 0.0, 1.0);
  PointType p6(1.0, 1.0, 1.0);
  PointType p7(0.0, 1.0, 1.0);
  PointType p8(2.0, 0.0, 0.0);
  PointType p9(2.0, 1.0, 0.0);
  PointType p10(2.0, 0.0, 1.0);
  PointType p11(2.0, 1.0, 1.0);
  
  std::cout << "Adding vertices to segment:" << std::endl;

  
  VertexHandleType vh0 = viennagrid::make_vertex( domain, p0 );
  VertexHandleType vh1 = viennagrid::make_vertex( domain, p1 );
  VertexHandleType vh2 = viennagrid::make_vertex( domain, p2 );
  VertexHandleType vh3 = viennagrid::make_vertex( domain, p3 );
  VertexHandleType vh4 = viennagrid::make_vertex( domain, p4 );
  VertexHandleType vh5 = viennagrid::make_vertex( domain, p5 );
  VertexHandleType vh6 = viennagrid::make_vertex( domain, p6 );
  VertexHandleType vh7 = viennagrid::make_vertex( domain, p7 );
  VertexHandleType vh8 = viennagrid::make_vertex( domain, p8 );
  VertexHandleType vh9 = viennagrid::make_vertex( domain, p9 );
  VertexHandleType vh10 = viennagrid::make_vertex( domain, p10 );
  VertexHandleType vh11 = viennagrid::make_vertex( domain, p11 );
  
  
  viennagrid::make_hexahedron( domain, vh0, vh1, vh3, vh2, vh4, vh5, vh7, vh6 );
  viennagrid::make_hexahedron( domain, vh1, vh8, vh2, vh9, vh5, vh10, vh6, vh11 ); 
}

template <typename DomainType>
void setup(DomainType & domain, viennagrid::quadrilateral_tag)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type            CellTag;
  typedef typename viennagrid::result_of::point<DomainType>::type               PointType;
  typedef typename viennagrid::result_of::vertex<DomainType>::type              VertexType;
  typedef typename viennagrid::result_of::vertex_handle<DomainType>::type       VertexHandleType;
  typedef typename viennagrid::result_of::cell<DomainType>::type                CellType;
  
  PointType p0(0.0, 0.0);
  PointType p1(1.0, 0.0);
  PointType p2(2.0, 0.0);
  PointType p3(2.0, 1.0);
  PointType p4(1.0, 1.0);
  PointType p5(0.0, 1.0);
  
  std::cout << "Adding vertices to segment:" << std::endl;
  VertexHandleType vh0 = viennagrid::make_vertex( domain, p0 );
  VertexHandleType vh1 = viennagrid::make_vertex( domain, p1 );
  VertexHandleType vh2 = viennagrid::make_vertex( domain, p2 );
  VertexHandleType vh3 = viennagrid::make_vertex( domain, p3 );
  VertexHandleType vh4 = viennagrid::make_vertex( domain, p4 );
  VertexHandleType vh5 = viennagrid::make_vertex( domain, p5 );
  
  viennagrid::make_quadrilateral( domain, vh0, vh1, vh5, vh4 );
  viennagrid::make_quadrilateral( domain, vh1, vh2, vh4, vh3 ); 
  
}


template <typename DomainType>
void test(std::string outfile)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type            CellTag;
  typedef typename viennagrid::result_of::point<DomainType>::type               PointType;
  typedef typename viennagrid::result_of::vertex<DomainType>::type              VertexType;
  typedef typename viennagrid::result_of::vertex_handle<DomainType>::type       VertexHandleType;
  typedef typename viennagrid::result_of::cell<DomainType>::type                CellType;

  typedef typename viennagrid::result_of::vertex_range<DomainType>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
  
  typedef typename viennagrid::result_of::line_range<DomainType>::type       EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;
  
  typedef typename viennagrid::result_of::facet_range<DomainType>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;
  
  typedef typename viennagrid::result_of::cell_range<DomainType>::type   CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                        CellIterator;
  
  DomainType domain;

  setup(domain, CellTag());  
  
  std::cout << "Vertices: " << std::endl;
  VertexContainer vertices = viennagrid::elements(domain);
  for (VertexIterator vit = vertices.begin();
        vit != vertices.end();
        ++vit)
      std::cout << *vit << std::endl;
  
  std::cout << "Edges: " << std::endl;
  EdgeContainer edges = viennagrid::elements(domain);
  for (EdgeIterator eit = edges.begin();
        eit != edges.end();
        ++eit)
      std::cout << *eit << std::endl;

  std::cout << "Facets: " << std::endl;
  FacetContainer facets = viennagrid::elements(domain);
  for (FacetIterator fit = facets.begin();
        fit != facets.end();
        ++fit)
      std::cout << *fit << std::endl;

  std::cout << "Cells: " << std::endl;
  CellContainer cells = viennagrid::elements(domain);
  for (CellIterator cit = cells.begin();
        cit != cells.end();
        ++cit)
      std::cout << *cit << std::endl;

  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  my_vtk_writer(domain, outfile);
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  test<viennagrid::quadrilateral_2d_domain>("hypercube_2d");
  test<viennagrid::hexahedral_3d_domain>("hypercube_3d");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
