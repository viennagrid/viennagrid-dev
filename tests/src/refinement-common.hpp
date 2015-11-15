/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

//***********************************************
// Define the input-file format
//***********************************************

#include <cstdio>
#include <iostream>
#include <map>

#include "viennagrid/viennagrid.h"

#define ERROR_CHECK(err) do { if (err != VIENNAGRID_SUCCESS) { printf("Return value %ld invalid in line %ld \n", (long)err, (long)__LINE__); return (int)err; } } while (0);


template <typename MeshType>
int facet_check(MeshType & mesh)
{
  typedef std::map<viennagrid_int, std::size_t>  CellFacetMap;

  CellFacetMap cell_on_facet_cnt;
  viennagrid_error err;

  viennagrid_dimension cell_dim;
  err = viennagrid_mesh_cell_dimension_get(mesh, &cell_dim); ERROR_CHECK(err);

  viennagrid_int *cells_begin, *cells_end;
  err = viennagrid_mesh_elements_get(mesh, cell_dim, &cells_begin, &cells_end); ERROR_CHECK(err);
  for (viennagrid_int *cit  = cells_begin;
                       cit != cells_end;
                     ++cit)
  {
    viennagrid_int *facets_begin, *facets_end;
    err = viennagrid_element_boundary_elements(mesh, *cit, cell_dim-1, &facets_begin, &facets_end); ERROR_CHECK(err);
    for (viennagrid_int *focit  = facets_begin;
                         focit != facets_end;
                       ++focit)
    {
      cell_on_facet_cnt[*focit] += 1;
    }
  }

  for (typename CellFacetMap::iterator cfmit  = cell_on_facet_cnt.begin();
                                       cfmit != cell_on_facet_cnt.end();
                                     ++cfmit)
  {
    if (cfmit->second > 2)
    {
      std::cerr << "Topology problem for facet " << cfmit->first << ": " << std::endl;

      for (viennagrid_int *cit  = cells_begin;
                           cit != cells_end;
                         ++cit)
      {
        std::cout << "Cell " << *cit << " with vertices: ";
        viennagrid_int *vertices_begin, *vertices_end;
        err = viennagrid_element_boundary_elements(mesh, *cit, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
        for (viennagrid_int *vocit  = vertices_begin;
                             vocit != vertices_end;
                           ++vocit)
          std::cout << *vocit << " " << std::endl;
      }

      viennagrid_int *facets_begin, *facets_end;
      err = viennagrid_mesh_elements_get(mesh, cell_dim-1, &facets_begin, &facets_end); ERROR_CHECK(err);
      for (viennagrid_int *fit  = facets_begin;
                           fit != facets_end;
                         ++fit)
      {
        std::cout << "Facet: " << *fit << " with vertices: ";
        viennagrid_int *vertices_begin, *vertices_end;
        err = viennagrid_element_boundary_elements(mesh, *fit, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
        for (viennagrid_int *vofit  = vertices_begin;
                             vofit != vertices_end;
                           ++vofit)
          std::cout << *vofit << " " << std::endl;
      }

      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}


int surface_check(viennagrid_mesh mesh_old, viennagrid_mesh mesh_new)
{
  viennagrid_error err;
  viennagrid_numeric old_surface, new_surface;

  err = viennagrid_mesh_surface(mesh_old, &old_surface); ERROR_CHECK(err);
  err = viennagrid_mesh_surface(mesh_new, &new_surface); ERROR_CHECK(err);

  viennagrid_dimension max_topo_dim;
  err = viennagrid_mesh_cell_dimension_get(mesh_old, &max_topo_dim);  ERROR_CHECK(err);

  if ( (new_surface < 0.9999 * old_surface)
      || (new_surface > 1.0001 * old_surface) )
  {
    viennagrid_int *cells_begin, *cells_end;
    err = viennagrid_mesh_elements_get(mesh_new, max_topo_dim, &cells_begin, &cells_end); ERROR_CHECK(err);
    for (viennagrid_int *cit  = cells_begin;
                         cit != cells_end;
                       ++cit)
    {
      std::cout << "Cell " << *cit << " with vertices: ";
      viennagrid_int *vertices_begin, *vertices_end;
      err = viennagrid_element_boundary_elements(mesh_new, *cit, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
      for (viennagrid_int *vocit  = vertices_begin;
                           vocit != vertices_end;
                         ++vocit)
        std::cout << *vocit << " " << std::endl;
    }

    viennagrid_int *facets_begin, *facets_end;
    err = viennagrid_mesh_elements_get(mesh_new, max_topo_dim-1, &facets_begin, &facets_end); ERROR_CHECK(err);
    for (viennagrid_int *fit  = facets_begin;
                         fit != facets_end;
                       ++fit)
    {
      std::cout << "Facet " << *fit << " with vertices: ";
      viennagrid_int *vertices_begin, *vertices_end;
      err = viennagrid_element_boundary_elements(mesh_new, *fit, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
      for (viennagrid_int *vofit  = vertices_begin;
                           vofit != vertices_end;
                         ++vofit)
        std::cout << *vofit << " " << std::endl;
    }

    std::cerr << "Surface check failed!" << std::endl;
    std::cerr << "Mesh surface before refinement: " << old_surface << std::endl;
    std::cerr << "Mesh surface after refinement: " << new_surface << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int volume_check(viennagrid_mesh mesh_old, viennagrid_mesh mesh_new)
{
  viennagrid_error err;
  viennagrid_numeric old_volume, new_volume;
  err = viennagrid_mesh_volume(mesh_old, &old_volume); ERROR_CHECK(err);
  err = viennagrid_mesh_volume(mesh_new, &new_volume); ERROR_CHECK(err);

  if ( (new_volume < 0.9999 * old_volume)
      || (new_volume > 1.0001 * old_volume) )
  {
    std::cerr << "Volume check failed!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int sanity_check(viennagrid_mesh mesh_old, viennagrid_mesh mesh_new)
{
  if (facet_check(mesh_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;  //check for sane topology in new mesh

  if (surface_check(mesh_old, mesh_new) != EXIT_SUCCESS)
    return EXIT_FAILURE; //check for same surface

  if (volume_check(mesh_old, mesh_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
