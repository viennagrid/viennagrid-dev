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

#include "refinement-common.hpp"




template <typename DomainType>
int test(DomainType & domain_in)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  typedef typename viennagrid::segment_t<ConfigType>     SegmentType;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 1>::type       EdgeType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                            CellTag::topology_level>::type      CellType;

  typedef typename viennagrid::result_of::ncell_range<DomainType, 0>::type  VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type       VertexIterator;
    
  typedef typename viennagrid::result_of::ncell_range<CellType, 1>::type    EdgeOnCellContainer;
  typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type     EdgeOnCellIterator;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
  
  CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain_in);
  CellIterator cit = cells.begin();
  CellType & cell = *cit; ++cit;
  CellType & cell2 = *cit;
  
  EdgeOnCellContainer edges = viennagrid::ncells<1>(cell);
  
  std::cout << "Volume of reference tetrahedron: " << volume(cell) << std::endl;
  
  //
  // Tag edges systematically for refinement:
  //
  
  std::cout << "Cell under test: " << std::endl;
  cell.print_short();
  
  //1 edge:
  std::cout << "Testing refinement with one tagged edge: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    clear_refinement_tag(cell);
    clear_refinement_tag(cell2);
    viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
    
    DomainType domain_refined;
    domain_refined = viennagrid::refine(domain_in, viennagrid::adaptive_refinement_tag());
    
    if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
    {
      print_refinement_edges(cell);
      return EXIT_FAILURE;
    }
  }
  std::cout << "[PASSED]" << std::endl;
  

  //2 edges:
  std::cout << "Testing refinement with two tagged edges: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    for (EdgeOnCellIterator eocit2 = edges.begin();
                            eocit2 != edges.end();
                          ++eocit2)
    {
      clear_refinement_tag(cell);
      clear_refinement_tag(cell2);
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
      
      DomainType domain_refined;
      domain_refined = viennagrid::refine(domain_in, viennagrid::adaptive_refinement_tag());

      if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
      {
        print_refinement_edges(cell);
        return EXIT_FAILURE;
      }
    }
  }
  std::cout << "[PASSED]" << std::endl;
  
  //3 edges:
  std::cout << "Testing refinement with three tagged edges: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    for (EdgeOnCellIterator eocit2 = edges.begin();
                            eocit2 != edges.end();
                          ++eocit2)
    {
      for (EdgeOnCellIterator eocit3 = edges.begin();
                              eocit3 != edges.end();
                            ++eocit3)
      {
        clear_refinement_tag(cell);
        clear_refinement_tag(cell2);
        viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
        viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
        viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit3) = true;
        
        DomainType domain_refined;
        domain_refined = viennagrid::refine(domain_in, viennagrid::adaptive_refinement_tag());
        
        if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
        {
          print_refinement_edges(cell);
          return EXIT_FAILURE;
        }
      }
    }
  }
  std::cout << "[PASSED]" << std::endl;

  //4 edges:
  std::cout << "Testing refinement with four tagged edges: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    for (EdgeOnCellIterator eocit2 = edges.begin();
                            eocit2 != edges.end();
                          ++eocit2)
    {
      for (EdgeOnCellIterator eocit3 = edges.begin();
                              eocit3 != edges.end();
                            ++eocit3)
      {
        for (EdgeOnCellIterator eocit4 = edges.begin();
                                eocit4 != edges.end();
                              ++eocit4)
        {
          clear_refinement_tag(cell);
          clear_refinement_tag(cell2);
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit3) = true;
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit4) = true;
          
          DomainType domain_refined;
          domain_refined = viennagrid::refine(domain_in, viennagrid::adaptive_refinement_tag());
          
          if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
          {
            print_refinement_edges(cell);
            return EXIT_FAILURE;
          }
        }
      }
    }
  }
  std::cout << "[PASSED]" << std::endl;
  
  //5 edges:
  std::cout << "Testing refinement with five tagged edges: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    for (EdgeOnCellIterator eocit2 = edges.begin();
                            eocit2 != edges.end();
                          ++eocit2)
    {
      for (EdgeOnCellIterator eocit3 = edges.begin();
                              eocit3 != edges.end();
                            ++eocit3)
      {
        for (EdgeOnCellIterator eocit4 = edges.begin();
                                eocit4 != edges.end();
                              ++eocit4)
        {
          for (EdgeOnCellIterator eocit5 = edges.begin();
                                  eocit5 != edges.end();
                                ++eocit5)
          {
            clear_refinement_tag(cell);
            clear_refinement_tag(cell2);
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit3) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit4) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit5) = true;
            
            DomainType domain_refined;
            domain_refined = viennagrid::refine(domain_in, viennagrid::adaptive_refinement_tag());
            
            if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
            {
              print_refinement_edges(cell);
              return EXIT_FAILURE;
            }
          }
        }
      }
    }
  }
  std::cout << "[PASSED]" << std::endl;
  
  //6 edges:
  std::cout << "Testing refinement with six tagged edges: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
  }
  
  {
    DomainType domain_refined;
    domain_refined = viennagrid::refine(domain_in, viennagrid::adaptive_refinement_tag());
    
    if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
    {
      print_refinement_edges(cell);
      return EXIT_FAILURE;
    }
  }  
  
  std::cout << "[PASSED]" << std::endl;
  
  return EXIT_SUCCESS;
}


template <unsigned int A>
struct cell_filler
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points); //no default implementation
};

template <>
struct cell_filler<1>
{
  template <typename CellType, typename VertexType>
  static void apply(CellType & cell, VertexType ** vertices)
  {
    CellType simplex;
    VertexType * reordered_vertices[4];
    
    reordered_vertices[0] = vertices[0];
    reordered_vertices[1] = vertices[1];
    reordered_vertices[2] = vertices[2];
    reordered_vertices[3] = vertices[3];
    
    cell.setVertices(reordered_vertices);
  }
};

template <>
struct cell_filler<2>
{
  template <typename CellType, typename VertexType>
  static void apply(CellType & cell, VertexType ** vertices)
  {
    CellType simplex;
    VertexType * reordered_vertices[4];
    
    reordered_vertices[0] = vertices[1];
    reordered_vertices[1] = vertices[2];
    reordered_vertices[2] = vertices[0];
    reordered_vertices[3] = vertices[3];
    
    cell.setVertices(reordered_vertices);
  }
};

template <>
struct cell_filler<3>
{
  template <typename CellType, typename VertexType>
  static void apply(CellType & cell, VertexType ** vertices)
  {
    CellType simplex;
    VertexType * reordered_vertices[4];
    
    reordered_vertices[0] = vertices[2];
    reordered_vertices[1] = vertices[0];
    reordered_vertices[2] = vertices[1];
    reordered_vertices[3] = vertices[3];
    
    cell.setVertices(reordered_vertices);
  }
};


template <typename DomainType, typename CellFillerA, typename CellFillerB>
void fill_domain(DomainType & domain,
                 CellFillerA const & fillerA,
                 CellFillerB const & fillerB)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef viennagrid::segment_t<ConfigType>     SegmentType;
  typedef typename ConfigType::cell_tag         CellTag;

  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                     CellTag::topology_level>::type   CellType;
  
  domain.segments().resize(1);
  SegmentType & seg = domain.segments()[0];
  
  std::vector<PointType> points(5);
  PointType & p0 = points[0];
  PointType & p1 = points[1];
  PointType & p2 = points[2];
  PointType & p3 = points[3];
  PointType & p4 = points[4];

  p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
  p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
  p2[0] = 0.0; p2[1] = 1.0; p2[2] = 0.0;
  p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;
  p4[0] = 1.0; p4[1] = 1.0; p4[2] = 1.0;
  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);

  VertexType * vertices[5];
  
  //std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = domain.add(v0);
  vertices[1] = domain.add(v1);
  vertices[2] = domain.add(v2);
  vertices[3] = domain.add(v3);
  vertices[4] = domain.add(v4);

  //std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  VertexType * cell_vertices[4];
  
  //first cell:
  cell_vertices[0] = vertices[0];
  cell_vertices[1] = vertices[1];
  cell_vertices[2] = vertices[2];
  cell_vertices[3] = vertices[3];
  CellFillerA::apply(simplex, &(cell_vertices[0]));
  simplex.setVertices(cell_vertices);
  seg.add(simplex);
  
  //second cell:
  cell_vertices[0] = vertices[1];
  cell_vertices[1] = vertices[2];
  cell_vertices[2] = vertices[3];
  cell_vertices[3] = vertices[4];
  CellFillerB::apply(simplex, &(cell_vertices[0]));
  simplex.setVertices(cell_vertices);
  seg.add(simplex);
}


template <unsigned int A>
struct domain_tester
{
  static int apply()
  {
    typedef viennagrid::config::tetrahedral_3d    ConfigType;
    typedef viennagrid::domain<ConfigType>        Domain;
   
    std::cout << std::endl;
    std::cout << "Testing domain " << A << "..." << std::endl;
    
    {
      Domain domain;
      fill_domain(domain, cell_filler<A>(), cell_filler<1>());
      if (test(domain) == EXIT_FAILURE)
      {
        std::cerr << "FAILED!" << std::endl;
        return EXIT_FAILURE;
      }
    }
    {
      Domain domain;
      fill_domain(domain, cell_filler<A>(), cell_filler<2>());
      if (test(domain) == EXIT_FAILURE)
      {
        std::cerr << "FAILED!" << std::endl;
        return EXIT_FAILURE;
      }
    }
    {
      Domain domain;
      fill_domain(domain, cell_filler<A>(), cell_filler<3>());
      if (test(domain) == EXIT_FAILURE)
      {
        std::cerr << "FAILED!" << std::endl;
        return EXIT_FAILURE;
      }
    }
    
    return domain_tester<A-1>::apply();
  }
};

//terminate recursion:
template <>
struct domain_tester<0>
{
  static int apply() { return EXIT_SUCCESS; } 
};

int main()
{
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  
  if (domain_tester<3>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  
  return EXIT_SUCCESS;
}
