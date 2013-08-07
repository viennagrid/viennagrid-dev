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
// Define the input-file format
//***********************************************

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/algorithm/refine.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/volume.hpp"


// Helper: Remove all refinement tags on a cell
// template <typename CellType>
// void clear_refinement_tag(CellType & cell)
// {
//   typedef typename CellType::config_type      ConfigType;
//   typedef typename viennagrid::result_of::vertex<ConfigType>::type       VertexType;
//   typedef typename viennagrid::result_of::line_range<CellType>::type       EdgeOnCellContainer;
//   typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type    EdgeOnCellIterator;
//   
//   EdgeOnCellContainer edges = viennagrid::elements(cell);
//   for(EdgeOnCellIterator eocit  = edges.begin();
//                          eocit != edges.end();
//                        ++eocit)
//   {
//     viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit) = false;
//   }
//   
//   viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(cell) = false;
// }

// Helper: Remove all refinement tags on a cell
template <typename CellType, typename EdgeRefinementTagAccessorT>
void print_refinement_edges(CellType & cell, EdgeRefinementTagAccessorT const edge_refinement_tag_accessor)
{
//   typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::vertex<CellType>::type       VertexType;
  typedef typename viennagrid::result_of::line_range<CellType>::type       EdgeOnCellContainer;
  typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type    EdgeOnCellIterator;
  
  EdgeOnCellContainer edges = viennagrid::elements(cell);
  for(EdgeOnCellIterator eocit  = edges.begin();
                         eocit != edges.end();
                       ++eocit)
  {
//     if (viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit) == true)
    if (edge_refinement_tag_accessor(*eocit))
      std::cout << *eocit << std::endl;
      //eocit->print_short();
  }
}


template <typename DomainType>
double domain_surface(DomainType & domain)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type         CellTag;
  typedef typename viennagrid::result_of::facet<DomainType>::type      FacetType;
  typedef typename viennagrid::result_of::cell<DomainType>::type        CellType;

  typedef typename viennagrid::result_of::cell_range<DomainType>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::facet_range<CellType>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::elements(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    FacetOnCellContainer facets = viennagrid::elements(*cit);
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
      domain_surface += viennagrid::volume(*(cfmit->first));
    }
  }
  
  return domain_surface;
}


template <typename DomainType>
int facet_check(DomainType & domain)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type         CellTag;
  typedef typename viennagrid::result_of::facet<DomainType>::type      FacetType;
  typedef typename viennagrid::result_of::cell<DomainType>::type        CellType;

  typedef typename viennagrid::result_of::facet_range<DomainType>::type  FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type         FacetIterator;
                                            
  typedef typename viennagrid::result_of::cell_range<DomainType>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::facet_range<CellType>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::elements(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    FacetOnCellContainer facets = viennagrid::elements(*cit);
    for (FacetOnCellIterator focit = facets.begin(); 
                      focit != facets.end();
                    ++focit)
    {
      cell_on_facet_cnt[&(*focit)] += 1;
    }
  }
  
  for (typename CellFacetMap::iterator cfmit = cell_on_facet_cnt.begin();
                                       cfmit != cell_on_facet_cnt.end();
                                     ++cfmit)
  {
    if (cfmit->second > 2)
    {
      std::cerr << "Topology problem for facet: " << std::endl;
      std::cout << *(cfmit->first) << std::endl;
      
      CellContainer cells = viennagrid::elements(domain);
      for (CellIterator cit = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        std::cout << "Cell: ";
        std::cout << *cit << std::endl;
      }
      
      FacetContainer facets = viennagrid::elements(domain);
      for (FacetIterator fit = facets.begin();
                        fit != facets.end();
                      ++fit)
      {
        std::cout << "Facet: ";
        std::cout << *fit << std::endl;
      }
      
      return EXIT_FAILURE;
    }
  }
  
  return EXIT_SUCCESS;
}


template <typename DomainType>
int surface_check(DomainType & domain_old, DomainType & domain_new)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type         CellTag;
  typedef typename viennagrid::result_of::facet<DomainType>::type      FacetType;
  typedef typename viennagrid::result_of::cell<DomainType>::type        CellType;

  typedef typename viennagrid::result_of::facet_range<DomainType>::type  FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type         FacetIterator;
                                            
  typedef typename viennagrid::result_of::cell_range<DomainType>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  
  double old_surface = domain_surface(domain_old);
  double new_surface = domain_surface(domain_new);
  
  if ( (new_surface < 0.9999 * old_surface)
      || (new_surface > 1.0001 * old_surface) )
  {
    CellContainer cells = viennagrid::elements(domain_new);
    for (CellIterator cit = cells.begin();
                      cit != cells.end();
                    ++cit)
    {
      std::cout << "Cell: ";
      std::cout << *cit << std::endl;
    }
    
    FacetContainer facets = viennagrid::elements(domain_new);
    for (FacetIterator fit = facets.begin();
                       fit != facets.end();
                     ++fit)
    {
      std::cout << "Facet: ";
      std::cout << *fit << std::endl;
    }
    
    std::cerr << "Surface check failed!" << std::endl;
    std::cerr << "Domain surface before refinement: " << old_surface << std::endl;
    std::cerr << "Domain surface after refinement: " << new_surface << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}

template <typename DomainType>
int volume_check(DomainType & domain_old, DomainType & domain_new)
{
  double old_volume = viennagrid::volume(domain_old);
  double new_volume = viennagrid::volume(domain_new);
  
  if ( (new_volume < 0.9999 * old_volume)
      || (new_volume > 1.0001 * old_volume) )
  {
    std::cerr << "Volume check failed!" << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}

template <typename DomainType>
int sanity_check(DomainType & domain_old, DomainType & domain_new)
{
  if (facet_check(domain_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;  //check for sane topology in new domain
    
  if (surface_check(domain_old, domain_new) != EXIT_SUCCESS)
    return EXIT_FAILURE; //check for same surface
  
  if (volume_check(domain_old, domain_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
  