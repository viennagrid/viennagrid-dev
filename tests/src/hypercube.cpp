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
  #pragma warning( disable : 4355 )
#endif

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/io/vtk_writer.hpp"

template <typename DomainType>
void setup(DomainType & domain, viennagrid::hexahedron_tag)
{
  typedef typename DomainType::config_type                                 ConfigType;
  typedef typename ConfigType::cell_tag                                    CellTag;
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::dim>::type   CellType;
  
  CellType hypercube0;
  CellType hypercube1;
  
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
  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);
  VertexType v5(p5, 5);
  VertexType v6(p6, 6);
  VertexType v7(p7, 7);
  VertexType v8(p8, 8);
  VertexType v9(p9, 9);
  VertexType v10(p10, 10);
  VertexType v11(p11, 11);
  
  std::cout << "Adding vertices to segment:" << std::endl;
  domain.push_back(v0);
  domain.push_back(v1);
  domain.push_back(v2);
  domain.push_back(v3);
  domain.push_back(v4);
  domain.push_back(v5);
  domain.push_back(v6);
  domain.push_back(v7);
  domain.push_back(v8);
  domain.push_back(v9);
  domain.push_back(v10);
  domain.push_back(v11);
  
  VertexType * vertices0[8];
  VertexType * vertices1[8];
  
  vertices0[0] = &(viennagrid::ncells<0>(domain)[0]);
  vertices0[1] = &(viennagrid::ncells<0>(domain)[1]);
  vertices0[2] = &(viennagrid::ncells<0>(domain)[2]);
  vertices0[3] = &(viennagrid::ncells<0>(domain)[3]);
  vertices0[4] = &(viennagrid::ncells<0>(domain)[4]);
  vertices0[5] = &(viennagrid::ncells<0>(domain)[5]);
  vertices0[6] = &(viennagrid::ncells<0>(domain)[6]);
  vertices0[7] = &(viennagrid::ncells<0>(domain)[7]);

  vertices1[0] = &(viennagrid::ncells<0>(domain)[1]);
  vertices1[1] = &(viennagrid::ncells<0>(domain)[8]);
  vertices1[2] = &(viennagrid::ncells<0>(domain)[9]);
  vertices1[3] = &(viennagrid::ncells<0>(domain)[2]);
  vertices1[4] = &(viennagrid::ncells<0>(domain)[5]);
  vertices1[5] = &(viennagrid::ncells<0>(domain)[10]);
  vertices1[6] = &(viennagrid::ncells<0>(domain)[11]);
  vertices1[7] = &(viennagrid::ncells<0>(domain)[6]);
 
  hypercube0.vertices(vertices0);
  domain.push_back(hypercube0);
  
  hypercube1.vertices(vertices1);  
  domain.push_back(hypercube1);
  
}

template <typename DomainType>
void setup(DomainType & domain, viennagrid::quadrilateral_tag)
{
  typedef typename DomainType::config_type                                 ConfigType;
  typedef typename ConfigType::cell_tag                                    CellTag;
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::dim>::type   CellType;

  CellType hypercube0;
  CellType hypercube1;
  
  PointType p0(0.0, 0.0);
  PointType p1(1.0, 0.0);
  PointType p2(2.0, 0.0);
  PointType p3(2.0, 1.0);
  PointType p4(1.0, 1.0);
  PointType p5(0.0, 1.0);
  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);
  VertexType v5(p5, 5);

  std::cout << "Adding vertices to segment:" << std::endl;
  domain.push_back(v0);
  domain.push_back(v1);
  domain.push_back(v2);
  domain.push_back(v3);
  domain.push_back(v4);
  domain.push_back(v5);
  
  VertexType * vertices0[4];
  VertexType * vertices1[4];
  
  vertices0[0] = &(viennagrid::ncells<0>(domain)[0]);
  vertices0[1] = &(viennagrid::ncells<0>(domain)[1]);
  vertices0[2] = &(viennagrid::ncells<0>(domain)[4]);
  vertices0[3] = &(viennagrid::ncells<0>(domain)[5]);

  vertices1[0] = &(viennagrid::ncells<0>(domain)[1]);
  vertices1[1] = &(viennagrid::ncells<0>(domain)[2]);
  vertices1[2] = &(viennagrid::ncells<0>(domain)[3]);
  vertices1[3] = &(viennagrid::ncells<0>(domain)[4]);
  
  hypercube0.vertices(vertices0);
  domain.push_back(hypercube0);
  
  hypercube1.vertices(vertices1);  
  domain.push_back(hypercube1);
  
}


template <typename ConfigType>
void test(std::string outfile)
{

  typedef typename viennagrid::result_of::domain<ConfigType>::type              Domain;
  typedef typename ConfigType::cell_tag                                         CellTag;
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::dim>::type   CellType;

  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
  
  typedef typename viennagrid::result_of::ncell_range<Domain, 1>::type       EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;
  
  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::dim-1>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;
  
  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::dim>::type   CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                        CellIterator;
  
  Domain domain;

  setup(domain, CellTag());  
  
  std::cout << "Vertices: " << std::endl;
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
        vit != vertices.end();
        ++vit)
      std::cout << *vit << std::endl;
  
  std::cout << "Edges: " << std::endl;
  EdgeContainer edges = viennagrid::ncells<1>(domain);
  for (EdgeIterator eit = edges.begin();
        eit != edges.end();
        ++eit)
      std::cout << *eit << std::endl;

  std::cout << "Facets: " << std::endl;
  FacetContainer facets = viennagrid::ncells(domain);
  for (FacetIterator fit = facets.begin();
        fit != facets.end();
        ++fit)
      std::cout << *fit << std::endl;

  std::cout << "Cells: " << std::endl;
  CellContainer cells = viennagrid::ncells(domain);
  for (CellIterator cit = cells.begin();
        cit != cells.end();
        ++cit)
      std::cout << *cit << std::endl;

  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer(domain, outfile + ".vtu");
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  test<viennagrid::config::quadrilateral_2d>("hypercube_2d");
  test<viennagrid::config::hexahedral_3d>("hypercube_3d");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
