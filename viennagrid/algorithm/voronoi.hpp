/* =======================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
       ViennaSHE - The Vienna Spherical Harmonics Expansion Simulator
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Markus Bina                        bina@iue.tuwien.ac.at

   license:    To be discussed, see file LICENSE in the ViennaSHE base directory
======================================================================= */

#ifndef VIENNAGRID_ALGORITHM_VORONOI_HPP
#define VIENNAGRID_ALGORITHM_VORONOI_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

#include "viennadata/api.hpp"

/** @file voronoi.hpp
    @brief Computes the Voronoi information for a Delaunay triangulation.
*/


namespace viennagrid
{

    //
    // Voronoi information in two dimensions
    //

    template <typename EdgeLenKey,
              typename InterfaceAreaKey,
              typename BoxVolumeKey,
              typename DomainType>
    void write_voronoi_info(DomainType const & domain, viennagrid::triangle_tag)
    {
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell_type<Config, CellTag::topology_level>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_container<DomainType, CellTag::topology_level>::type    CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type                            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type                                 EdgeOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<EdgeType, 0>::type                            VertexOnEdgeContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                               VertexOnEdgeIterator;

      //
      // Algorithm: Iterate over all cells, compute circumcenter and add interface area to edge, box volume to vertex.
      //
      
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        //iterate over edges:
        EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(*cit);
        for (EdgeOnCellIterator eocit  = edges_on_cell.begin();
                                eocit != edges_on_cell.end();
                              ++eocit)
        {
          PointType edge_midpoint = circumcenter(*eocit);
          
          //compute edge length:
          VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*eocit);
          VertexOnEdgeIterator voeit = vertices_on_edge.begin();
          
          PointType const & v0 = voeit->getPoint();
          ++voeit;
          PointType const & v1 = voeit->getPoint();

          //edge length:
          viennadata::access<EdgeLenKey, double>()(*eocit) = spanned_volume(v0, v1);
          
          // interface contribution:
          viennadata::access<InterfaceAreaKey, double>()(*eocit) += spanned_volume(circ_center, edge_midpoint);
          
          //box volume contribution:
          double edge_contribution = 0;
          for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                    voeit != vertices_on_edge.end();
                                  ++voeit)
          {
            double contribution = spanned_volume(circ_center, edge_midpoint, voeit->getPoint());
            edge_contribution += contribution;
            viennadata::access<BoxVolumeKey, double>()(*voeit) += contribution;
          }
          viennadata::access<BoxVolumeKey, double>()(*eocit) += edge_contribution;
        } //for edges on cells
        
      } //for cells
      
    } //write_voronoi_info(triangle_tag)


    template <typename EdgeLenKey,
              typename InterfaceAreaKey,
              typename BoxVolumeKey,
              typename DomainType>
    void write_voronoi_info(DomainType const & domain, viennagrid::quadrilateral_tag)
    {
      std::cout << "Warning: Voronoi info for quadrilaterals is only correct when having rectangles only." << std::endl;
      write_voronoi_info<EdgeLenKey, InterfaceAreaKey, BoxVolumeKey>(domain, viennagrid::triangle_tag());
    }


    //
    // Voronoi information in three dimensions
    // Note by [KR]: In principle one can implement a general Voronoi algorithm for N dimensional simplex-domains based on recursively iterating over elements of a cell. However, we won't need more than three dimensions by now...
    //

    template <typename EdgeLenKey,
              typename InterfaceAreaKey,
              typename BoxVolumeKey,
              typename DomainType>
    void write_voronoi_info(DomainType const & domain, viennagrid::tetrahedron_tag)
    {
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell_type<Config, 2>::type                         FacetType;
      typedef typename viennagrid::result_of::ncell_type<Config, CellTag::topology_level>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_container<DomainType, CellTag::topology_level>::type    CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_container<CellType, 2>::type                            FacetOnCellContainer;
      typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                                FacetOnCellIterator;

      typedef typename viennagrid::result_of::const_ncell_container<FacetType, 1>::type                           EdgeOnFacetContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnFacetContainer>::type                                EdgeOnFacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<EdgeType, 0>::type                            VertexOnEdgeContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                               VertexOnEdgeIterator;

      //
      // Algorithm: Iterate over all cells, compute circumcenter and add interface area to edge, box volume to vertex.
      //
      
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType cell_center = circumcenter(*cit);

        FacetOnCellContainer facets_on_cell = viennagrid::ncells<2>(*cit);
        for (FacetOnCellIterator focit  = facets_on_cell.begin();
                                focit != facets_on_cell.end();
                              ++focit)
        {
          PointType facet_center = circumcenter(*focit);
          
          //iterate over edges:
          EdgeOnFacetContainer edges_on_facet = viennagrid::ncells<1>(*focit);
          for (EdgeOnFacetIterator eocit  = edges_on_facet.begin();
                                  eocit != edges_on_facet.end();
                                ++eocit)
          {
            PointType edge_midpoint = viennagrid::circumcenter(*eocit);
            
            //compute edge length:
            VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*eocit);
            VertexOnEdgeIterator voeit = vertices_on_edge.begin();
            
            PointType const & v0 = voeit->getPoint();
            ++voeit;
            PointType const & v1 = voeit->getPoint();

            //edge length:
            viennadata::access<EdgeLenKey, double>()(*eocit) = spanned_volume(v0, v1);
            
            // interface contribution:
            viennadata::access<InterfaceAreaKey, double>()(*eocit) += spanned_volume(cell_center, facet_center, edge_midpoint);
            
            //box volume contribution:
            for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                      voeit != vertices_on_edge.end();
                                    ++voeit)
            {
              viennadata::access<BoxVolumeKey, double>()(*voeit) += spanned_volume(cell_center, facet_center, edge_midpoint, voeit->getPoint());
            }
          } //for edges on facet

        } //for facets on cell

      } //for cells
      
    } //write_voronoi_info(triangle_tag)



    template <typename EdgeLenKey,
              typename InterfaceAreaKey,
              typename BoxVolumeKey,
              typename DomainType>
    void write_voronoi_info(DomainType const & domain, viennagrid::hexahedron_tag)
    {
      std::cout << "Warning: Voronoi info for hexahedron is only correct when having regular cuboids only." << std::endl;
      write_voronoi_info<EdgeLenKey, InterfaceAreaKey, BoxVolumeKey>(domain, viennagrid::tetrahedron_tag());
    }



    //
    // The public interface
    //
    template <typename EdgeLenKey,
              typename InterfaceAreaKey,
              typename BoxVolumeKey,
              typename DomainType>
    void write_voronoi_info(DomainType const & domain)
    {
      write_voronoi_info<EdgeLenKey, InterfaceAreaKey, BoxVolumeKey, DomainType>(domain, typename DomainType::config_type::cell_tag());
    }
    
} //namespace viennagrid
#endif
