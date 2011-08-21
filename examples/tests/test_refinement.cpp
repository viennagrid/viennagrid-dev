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

//***********************************************
// Define the dimension
//***********************************************
#define THREEDIM
//#define TWODIM
//#define ONEDIM

//***********************************************
// Define the input-file format
//***********************************************
//#define SGF
#define NETGEN
//#define VTK
//#define GTS

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

#include "viennagrid/algorithm/refine.hpp"

struct TestDomainConfig
{
  typedef double                                  numeric_type;
  #ifdef THREEDIM
  typedef viennagrid::three_dimensions_tag        dimension_tag;
  typedef viennagrid::tetrahedron_tag             cell_tag;
  #endif
  #ifdef TWODIM
  typedef viennagrid::two_dimensions_tag          dimension_tag;
  typedef viennagrid::triangle_tag                cell_tag;
  #endif
  #ifdef ONEDIM
  typedef viennagrid::one_dimension_tag           dimension_tag;
  typedef viennagrid::line_tag                    cell_tag;
  #endif
};


template <typename CellType>
double volume(CellType & cell)
{
  typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell_range<CellType, 0>::type       VertexOnCellContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type    VertexOnCellIterator;
  
  VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
  VertexOnCellIterator vit = vertices.begin();
  
  VertexType & v0 = *vit; ++vit;
  VertexType & v1 = *vit; ++vit;
  VertexType & v2 = *vit; ++vit;
  VertexType & v3 = *vit; ++vit;
  
  return spanned_volume(v0.getPoint(), v1.getPoint(), v2.getPoint(), v3.getPoint());
}

template <typename CellType>
double volume_triangle(CellType & cell)
{
  typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell_range<CellType, 0>::type       VertexOnCellContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type    VertexOnCellIterator;
  
  VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
  VertexOnCellIterator vit = vertices.begin();
  
  VertexType & v0 = *vit; ++vit;
  VertexType & v1 = *vit; ++vit;
  VertexType & v2 = *vit; ++vit;
  
  return spanned_volume(v0.getPoint(), v1.getPoint(), v2.getPoint());
}


template <typename DomainType>
double domain_volume(DomainType & domain)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
  
  double new_volume = 0;
  CellContainer new_cells = viennagrid::ncells<CellTag::topology_level>(domain);
  for (CellIterator new_cit = new_cells.begin();
                    new_cit != new_cells.end();
                  ++new_cit)
  {
    //std::cout << "adding up!" << std::endl;
    double cell_volume = volume(*new_cit);
    assert(cell_volume > 0);
    new_volume += cell_volume;
    //new_cit->print_short();
  }
  
  return new_volume;
}

template <typename DomainType>
double domain_surface(DomainType & domain)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level-1>::type      FacetType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level>::type        CellType;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::ncell_range<CellType, CellTag::topology_level-1>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    FacetOnCellContainer facets = viennagrid::ncells<CellTag::topology_level-1>(*cit);
    for (FacetOnCellIterator focit = facets.begin(); 
                      focit != facets.end();
                    ++focit)
    {
      cell_on_facet_cnt[&(*focit)] += 1;
    }
  }
  
  double domain_surface = 0;
  for (typename CellFacetMap::iterator cfmit = cell_on_facet_cnt.begin();
                                       cfmit != cell_on_facet_cnt.end();
                                     ++cfmit)
  {
    if (cfmit->second == 1)
    {
      domain_surface += volume_triangle(*(cfmit->first));
    }
  }
  
  return domain_surface;
}



void testNewDomain(std::string & infile, std::string & outfile)
{

  typedef viennagrid::domain<TestDomainConfig>        Domain;
  typedef TestDomainConfig::cell_tag                  CellTag;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::result_of::point_type<TestDomainConfig>::type          PointType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig,
                                            CellTag::topology_level>::type   CellType;
  //typedef viennagrid::DomainTypes<TestDomainConfig>::segment_type  Segment;

  typedef viennagrid::result_of::ncell_range<Domain, 0>::type   VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
      
  typedef viennagrid::result_of::ncell_range<Domain, 1>::type   EdgeContainer;
  typedef viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef viennagrid::result_of::ncell_range<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

  typedef viennagrid::result_of::ncell_range<Domain, CellTag::topology_level>::type     CellContainer;
  typedef viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;

  Domain domain;
  
  try{
    #ifdef NETGEN
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(domain, infile);
    #endif
    
    #ifdef GTS
    viennagrid::io::gts_reader my_gts_reader;
    my_gts_reader(domain, infile);
    #endif
    
    #ifdef VTK
    viennagrid::io::vtk_reader my_vtk_reader;
    my_vtk_reader.readDomain(domain, infile);
    #endif
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }

  std::cout << "Volume of input domain: " << domain_volume(domain) << std::endl;
  std::cout << "Surface of input domain: " << domain_surface(domain) << std::endl;

  //Testing: Tag some cells with centroid at x \in [2,3]:
  CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
  for (CellIterator cit  = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    PointType centroid = viennagrid::centroid(*cit);
    
    if ( (centroid[0] >= 2.0) 
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0) 
        && (centroid[1] <= 1.0))
    {
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*cit) = true;
    }
  }

  Domain refined_domain;
  refined_domain = viennagrid::refine(domain, viennagrid::adaptive_refinement_tag());

  std::cout << "Volume of refined domain: " << domain_volume(refined_domain) << std::endl;
  std::cout << "Surface of refined domain: " << domain_surface(refined_domain) << std::endl;
  
  //
  // Second pass:
  //
  CellContainer cells_refined = viennagrid::ncells<CellTag::topology_level>(refined_domain);
  for (CellIterator cit  = cells_refined.begin();
                    cit != cells_refined.end();
                  ++cit)
  {
    PointType centroid = viennagrid::centroid(*cit);
    
    if ( (centroid[0] >= 2.0) 
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0) 
        && (centroid[1] <= 1.0))
    {
      viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*cit) = true;
    }
  }

  Domain double_refined_domain;
  double_refined_domain = viennagrid::refine(refined_domain, viennagrid::adaptive_refinement_tag());

  std::cout << "Volume of double refined domain: " << domain_volume(double_refined_domain) << std::endl;
  std::cout << "Surface of double refined domain: " << domain_surface(double_refined_domain) << std::endl;
  

  //test writers:
  std::cout << "Writing domains..." << std::endl;
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, outfile + ".vtu");
  my_vtk_writer.writeDomain(refined_domain, outfile + "_refined.vtu");
  my_vtk_writer.writeDomain(double_refined_domain, outfile + "_double_refined.vtu");

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return;
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../examples/data/";
  
  #ifdef THREEDIM
  std::string infile = path + "sshape3d-pimped.mesh";
  #endif
  #ifdef TWODIM
  std::string infile = path + "sshape2d.mesh";
  #endif
  #ifdef ONEDIM
  std::string infile = path + "line8.mesh";
  #endif
  std::string outfile = "out"; // without ending
  
  testNewDomain(infile, outfile);
  return EXIT_SUCCESS;
}
