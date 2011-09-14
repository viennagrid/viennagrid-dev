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
                                            CellTag::dim>::type      CellType;

  typedef typename viennagrid::result_of::ncell_range<DomainType, 0>::type  VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type       VertexIterator;
    
  typedef typename viennagrid::result_of::ncell_range<CellType, 1>::type    EdgeOnCellContainer;
  typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type     EdgeOnCellIterator;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::dim>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
  
  CellContainer cells = viennagrid::ncells<CellTag::dim>(domain_in);
  CellIterator cit = cells.begin();
  
  CellType & cell = *cit;
  
  EdgeOnCellContainer edges = viennagrid::ncells<1>(cell);
  
  std::cout << "Volume of reference tetrahedron: " << volume(cell) << std::endl;
  
  //
  // Tag edges systematically for refinement:
  //
  
  std::cout << "Cell under test: " << std::endl;
  std::cout << cell << std::endl;
  
  //1 edge:
  std::cout << "Testing refinement with one tagged edge: ";
  for (EdgeOnCellIterator eocit1 = edges.begin();
                          eocit1 != edges.end();
                        ++eocit1)
  {
    clear_refinement_tag(cell);
    viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
    
    DomainType domain_refined;
    domain_refined = viennagrid::refine(domain_in, viennagrid::local_refinement_tag());
    
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
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
      
      DomainType domain_refined;
      domain_refined = viennagrid::refine(domain_in, viennagrid::local_refinement_tag());

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
        viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
        viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
        viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit3) = true;
        
        DomainType domain_refined;
        domain_refined = viennagrid::refine(domain_in, viennagrid::local_refinement_tag());
        
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
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit3) = true;
          viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit4) = true;
          
          DomainType domain_refined;
          domain_refined = viennagrid::refine(domain_in, viennagrid::local_refinement_tag());
          
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
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit1) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit2) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit3) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit4) = true;
            viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit5) = true;
            
            DomainType domain_refined;
            domain_refined = viennagrid::refine(domain_in, viennagrid::local_refinement_tag());
            
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
    domain_refined = viennagrid::refine(domain_in, viennagrid::local_refinement_tag());
    
    if (sanity_check(domain_in, domain_refined) == EXIT_FAILURE)
    {
      print_refinement_edges(cell);
      return EXIT_FAILURE;
    }
  }  
  
  std::cout << "[PASSED]" << std::endl;
  
  return EXIT_SUCCESS;
}


template <unsigned int N>
struct coord_filler
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points); //no default implementation
};

template <>
struct coord_filler<1>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 0.0; p2[1] = 1.0; p2[2] = 0.0;
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;
  }
};


template <>
struct coord_filler<2>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 9.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 0.0; p2[1] = 1.0; p2[2] = 0.0;
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;
  }
};

template <>
struct coord_filler<3>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 9.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 0.0; p2[1] = 9.0; p2[2] = 0.0;
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;
  }
};

template <>
struct coord_filler<4>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 1.0; p2[1] = 0.1; p2[2] = 0.0;
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;
  }
};

template <>
struct coord_filler<5>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 1.0; p2[1] = 0.1; p2[2] = 0.0;
    p3[0] = 0.45; p3[1] = 0.1; p3[2] = 1.0;
  }
};

template <>
struct coord_filler<6>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 1.0; p2[1] = 0.1; p2[2] = 0.0;
    p3[0] = 0.65; p3[1] = 0.1; p3[2] = 0.1;
  }
};

template <>
struct coord_filler<7>
{
  template <typename PointType>
  static void apply(PointType & p0, PointType & p1, PointType & p2, PointType & p3)
  {
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    p2[0] = 1.0; p2[1] = 0.1; p2[2] = 0.0;
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 0.1;
  }
};


template <unsigned int A, unsigned int B>
struct point_filler
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points); //no default implementation
};

//final vertex 3:
template <unsigned int B>
struct point_filler<1, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[0];
    PointType & p1 = points[1];
    PointType & p2 = points[2];
    PointType & p3 = points[3];

    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<2, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[1];
    PointType & p1 = points[2];
    PointType & p2 = points[0];
    PointType & p3 = points[3];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<3, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[2];
    PointType & p1 = points[0];
    PointType & p2 = points[1];
    PointType & p3 = points[3];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

//final vertex 2:
template <unsigned int B>
struct point_filler<4, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[3];
    PointType & p1 = points[1];
    PointType & p2 = points[0];
    PointType & p3 = points[2];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<5, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[1];
    PointType & p1 = points[0];
    PointType & p2 = points[3];
    PointType & p3 = points[2];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<6, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[0];
    PointType & p1 = points[3];
    PointType & p2 = points[1];
    PointType & p3 = points[2];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};


//final vertex 1:
template <unsigned int B>
struct point_filler<7, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[0];
    PointType & p1 = points[2];
    PointType & p2 = points[3];
    PointType & p3 = points[1];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<8, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[2];
    PointType & p1 = points[3];
    PointType & p2 = points[0];
    PointType & p3 = points[1];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<9, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[3];
    PointType & p1 = points[0];
    PointType & p2 = points[2];
    PointType & p3 = points[1];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

//final vertex 0:
template <unsigned int B>
struct point_filler<10, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[1];
    PointType & p1 = points[3];
    PointType & p2 = points[2];
    PointType & p3 = points[0];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<11, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[3];
    PointType & p1 = points[2];
    PointType & p2 = points[1];
    PointType & p3 = points[0];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};

template <unsigned int B>
struct point_filler<12, B>
{
  template <typename PointType>
  static void apply(std::vector<PointType> & points)
  {
    PointType & p0 = points[2];
    PointType & p1 = points[1];
    PointType & p2 = points[3];
    PointType & p3 = points[0];
    
    coord_filler<B>::apply(p0, p1, p2, p3);
  }
};


template <typename DomainType, typename PointFiller>
void fill_domain(DomainType & domain, PointFiller const & filler)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef viennagrid::segment_t<ConfigType>     SegmentType;
  typedef typename ConfigType::cell_tag         CellTag;

  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                     CellTag::dim>::type   CellType;
  
  domain.segments().resize(1);
  SegmentType & seg = domain.segments()[0];
  
  std::vector<PointType> points(4);
  PointFiller::apply(points);

  //upgrade to vertex:
  VertexType v0(points[0], 0);
  VertexType v1(points[1], 1);
  VertexType v2(points[2], 2);
  VertexType v3(points[3], 3);

  VertexType * vertices[4];
  
  //std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = domain.push_back(v0);
  vertices[1] = domain.push_back(v1);
  vertices[2] = domain.push_back(v2);
  vertices[3] = domain.push_back(v3);

  //std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  
  simplex.vertices(vertices);
  seg.push_back(simplex);
}


template <unsigned int A, unsigned int B>
struct domain_tester
{
  static int apply()
  {
    typedef viennagrid::config::tetrahedral_3d    ConfigType;
    typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
   
    std::cout << std::endl;
    std::cout << "Testing domain " << A << "..." << std::endl;
    Domain domain;
    fill_domain(domain, point_filler<A, B>());
    if (test(domain) == EXIT_FAILURE)
    {
      std::cerr << "FAILED!" << std::endl;
      return EXIT_FAILURE;
    }
    
    return domain_tester<A-1, B>::apply();
  }
};

//terminate recursion:
template <unsigned int B>
struct domain_tester<0, B>
{
  static int apply() { return EXIT_SUCCESS; } 
};

int main()
{
#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  std::cout << "Test takes too long with Visual Studio debug build. Skipping..." << std::endl;
  return EXIT_SUCCESS;
#endif

  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  
  if (domain_tester<12, 1>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  if (domain_tester<12, 2>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  if (domain_tester<12, 3>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  if (domain_tester<12, 4>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  if (domain_tester<12, 5>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  if (domain_tester<12, 6>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;

  if (domain_tester<12, 7>::apply() == EXIT_SUCCESS)
  {
    std::cout << "SUCCESS!" << std::endl;
  }
  else
    return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
