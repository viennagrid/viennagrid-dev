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


template <typename MeshType>
double mesh_surface(MeshType & mesh)
{
  typedef typename viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef typename viennagrid::result_of::facet<MeshType>::type      FacetType;
  typedef typename viennagrid::result_of::cell<MeshType>::type        CellType;

  typedef typename viennagrid::result_of::cell_range<MeshType>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::facet_range<CellType>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::elements(mesh);
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
  
  double mesh_surface = 0;
  for (typename CellFacetMap::iterator cfmit = cell_on_facet_cnt.begin();
                                       cfmit != cell_on_facet_cnt.end();
                                     ++cfmit)
  {
    if (cfmit->second == 1)
    {
      mesh_surface += viennagrid::volume(*(cfmit->first));
    }
  }
  
  return mesh_surface;
}


template <typename MeshType>
int facet_check(MeshType & mesh)
{
  typedef typename viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef typename viennagrid::result_of::facet<MeshType>::type      FacetType;
  typedef typename viennagrid::result_of::cell<MeshType>::type        CellType;

  typedef typename viennagrid::result_of::facet_range<MeshType>::type  FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type         FacetIterator;
                                            
  typedef typename viennagrid::result_of::cell_range<MeshType>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::facet_range<CellType>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::elements(mesh);
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
      
      CellContainer cells = viennagrid::elements(mesh);
      for (CellIterator cit = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        std::cout << "Cell: ";
        std::cout << *cit << std::endl;
      }
      
      FacetContainer facets = viennagrid::elements(mesh);
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


template <typename MeshType>
int surface_check(MeshType & mesh_old, MeshType & mesh_new)
{
  typedef typename viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef typename viennagrid::result_of::facet<MeshType>::type      FacetType;
  typedef typename viennagrid::result_of::cell<MeshType>::type        CellType;

  typedef typename viennagrid::result_of::facet_range<MeshType>::type  FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type         FacetIterator;
                                            
  typedef typename viennagrid::result_of::cell_range<MeshType>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  
  double old_surface = mesh_surface(mesh_old);
  double new_surface = mesh_surface(mesh_new);
  
  if ( (new_surface < 0.9999 * old_surface)
      || (new_surface > 1.0001 * old_surface) )
  {
    CellContainer cells = viennagrid::elements(mesh_new);
    for (CellIterator cit = cells.begin();
                      cit != cells.end();
                    ++cit)
    {
      std::cout << "Cell: ";
      std::cout << *cit << std::endl;
    }
    
    FacetContainer facets = viennagrid::elements(mesh_new);
    for (FacetIterator fit = facets.begin();
                       fit != facets.end();
                     ++fit)
    {
      std::cout << "Facet: ";
      std::cout << *fit << std::endl;
    }
    
    std::cerr << "Surface check failed!" << std::endl;
    std::cerr << "Mesh surface before refinement: " << old_surface << std::endl;
    std::cerr << "Mesh surface after refinement: " << new_surface << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}

template <typename MeshType>
int volume_check(MeshType & mesh_old, MeshType & mesh_new)
{
  double old_volume = viennagrid::volume(mesh_old);
  double new_volume = viennagrid::volume(mesh_new);
  
  if ( (new_volume < 0.9999 * old_volume)
      || (new_volume > 1.0001 * old_volume) )
  {
    std::cerr << "Volume check failed!" << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}

template <typename MeshType>
int sanity_check(MeshType & mesh_old, MeshType & mesh_new)
{
  if (facet_check(mesh_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;  //check for sane topology in new mesh
    
  if (surface_check(mesh_old, mesh_new) != EXIT_SUCCESS)
    return EXIT_FAILURE; //check for same surface
  
  if (volume_check(mesh_old, mesh_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
  