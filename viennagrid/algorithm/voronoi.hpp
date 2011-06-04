/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
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
    void write_voronoi_info(DomainType const & domain, viennagrid::quadrilateral_tag)
    {
      std::cout << "Warning: Voronoi info for quadrilaterals is only correct when having rectangles only." << std::endl;
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

      typedef typename viennagrid::result_of::const_ncell_container<DomainType, 1>::type                          EdgeContainer;
      typedef typename viennagrid::result_of::iterator<EdgeContainer>::type                                       EdgeIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type                            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type                                 EdgeOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<EdgeType, 0>::type                            VertexOnEdgeContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                               VertexOnEdgeIterator;

      //
      // Phase 1: Compute circumcenter of cells and store them on each of the edges (avoids coboundary operations)
      //
      
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        // store circumcenter on edges
        EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(*cit);
        for (EdgeOnCellIterator eocit  = edges_on_cell.begin();
                                eocit != edges_on_cell.end();
                              ++eocit)
        {
          viennadata::access<InterfaceAreaKey, std::vector<PointType> >()(*eocit).push_back(circ_center);
        } //for edges on cells
        
      } //for cells


      //
      // Phase 2: Compute edge lengths, interface areas and box volumes from the information stored on edges:
      //
      EdgeContainer edges = viennagrid::ncells<1>(domain);
      for (EdgeIterator eit  = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        
        //get vertices of edge:
        VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*eit);
        VertexOnEdgeIterator voeit = vertices_on_edge.begin();
        
        VertexType const & v0 = *voeit;
        ++voeit;
        VertexType const & v1 = *voeit;
        
        PointType const & p0 = v0.getPoint();
        PointType const & p1 = v1.getPoint();

        double edge_length = spanned_volume(p0, p1);
        viennadata::access<EdgeLenKey, double>()(*eit) = edge_length;
        
        {
          std::vector<PointType> & circ_centers = viennadata::access<InterfaceAreaKey, std::vector<PointType> >()(*eit);
          
          double interface_area = -42.0;
          if (circ_centers.size() == 1)  //edge is located on the domain boundary
          {
            interface_area = spanned_volume(circ_centers[0], circumcenter(*eit));
            //std::cout << "Edge on interface " << interface_area << std::endl;
          }
          else if (circ_centers.size() == 2)
          {
            interface_area = spanned_volume(circ_centers[0], circ_centers[1]);
            //std::cout << "Edge on interior " << interface_area << std::endl;
          }
          else
          {
            throw "More than two circum centers for an edge in two dimensions!"; 
          }
          
          viennadata::access<InterfaceAreaKey, double>()(*eit) = interface_area;
          double volume_contribution = interface_area * edge_length / 4.0;
          viennadata::access<BoxVolumeKey, double>()(*eit) = 2.0 * volume_contribution; //volume contribution of both box volumes associated with the edge
          viennadata::access<BoxVolumeKey, double>()(v0) += volume_contribution;
          viennadata::access<BoxVolumeKey, double>()(v1) += volume_contribution;
        }
        
        //delete circ_centers:
        viennadata::erase<InterfaceAreaKey, std::vector<PointType> >()(*eit);
      }

    }


    //
    // Voronoi information in three dimensions
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

      typedef typename viennagrid::result_of::const_ncell_container<DomainType, CellTag::topology_level-1>::type  FacetContainer;
      typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                      FacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<DomainType, 1>::type                          EdgeContainer;
      typedef typename viennagrid::result_of::iterator<EdgeContainer>::type                                       EdgeIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<CellType, CellTag::topology_level-1>::type    FacetOnCellContainer;
      typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                                FacetOnCellIterator;

      typedef typename viennagrid::result_of::const_ncell_container<FacetType, 1>::type                           EdgeOnFacetContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnFacetContainer>::type                                EdgeOnFacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_container<EdgeType, 0>::type                            VertexOnEdgeContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                               VertexOnEdgeIterator;

      //
      // Step one: Write circumcenters to facets
      //
      
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        FacetOnCellContainer facets_on_cell = viennagrid::ncells<CellTag::topology_level-1>(*cit);
        for (FacetOnCellIterator focit  = facets_on_cell.begin();
                                focit != facets_on_cell.end();
                              ++focit)
        {
          viennadata::access<InterfaceAreaKey, std::vector<PointType> >()(*focit).push_back(circ_center);
        } //for edges on cells

      } //for cells


      //
      // Step two: Write lines connecting circumcenters to edges
      //
      FacetContainer facets = viennagrid::ncells<CellTag::topology_level-1>(domain);
      for (FacetIterator fit  = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
        std::vector<PointType> & circ_centers = viennadata::access<InterfaceAreaKey, std::vector<PointType> >()(*fit);
        
        EdgeOnFacetContainer edges_on_facet = viennagrid::ncells<1>(*fit);
        for (EdgeOnFacetIterator eofit  = edges_on_facet.begin();
                                eofit != edges_on_facet.end();
                              ++eofit)
        {
          viennadata::access<InterfaceAreaKey, std::vector<PointType> >()(*eofit).push_back(circ_centers[0]);
          
          if (circ_centers.size() == 1)
          {
            //std::cout << "Circumcenter fit: " << circumcenter(*fit) << std::endl;
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> > >()(*eofit).push_back(std::make_pair(circ_centers[0],
                                                                                                                  circumcenter(*fit)));
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> > >()(*eofit).push_back(std::make_pair(circumcenter(*eofit),
                                                                                                                  circumcenter(*fit)));
          }
          else if (circ_centers.size() == 2)
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> > >()(*eofit).push_back(std::make_pair(circ_centers[0],
                                                                                                                  circ_centers[1]));
          else
          {
            throw "More than two circumcenters for a facet in three dimensions!"; 
          }
            
        } //for edges on cells
      }


      //
      // Step three: Compute Voronoi information:
      //
      EdgeContainer edges = viennagrid::ncells<1>(domain);
      for (EdgeIterator eit  = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        //eit->print_short();
        
        //get vertices of edge:
        VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*eit);
        VertexOnEdgeIterator voeit = vertices_on_edge.begin();
        
        VertexType const & v0 = *voeit;
        ++voeit;
        VertexType const & v1 = *voeit;
        
        PointType const & p0 = v0.getPoint();
        PointType const & p1 = v1.getPoint();

        //write edge length
        double edge_length = spanned_volume(p0, p1);
        viennadata::access<EdgeLenKey, double>()(*eit) = edge_length;
        
        std::vector<std::pair<PointType, PointType> > & interface_segments =
          viennadata::access<InterfaceAreaKey, std::vector<std::pair<PointType, PointType> > >()(*eit);

        //determine inner point of convex interface polygon:
        PointType inner_point = (interface_segments[0].first + interface_segments[0].second) / 2.0;
        for (size_t i=1; i<interface_segments.size(); ++i)
        {
          inner_point += (interface_segments[i].first + interface_segments[i].second) / 2.0;
        }
        inner_point /= interface_segments.size();
        //std::cout << "Inner point: " << inner_point << std::endl;
        
        //compute interface area
        double interface_area = 0.0;
        for (size_t i=0; i<interface_segments.size(); ++i)
        {
          //std::cout << "Interface line segment: " << interface_segments[i].first << " to " << interface_segments[i].second << std::endl;
          interface_area += spanned_volume(interface_segments[i].first, interface_segments[i].second, inner_point);
        }
        //std::cout << "Interface area: " << interface_area << std::endl;
        viennadata::access<InterfaceAreaKey, double>()(*eit) = interface_area;
        double volume_contribution = interface_area * edge_length / 6.0;
        //std::cout << "Volume contribution: " << volume_contribution << std::endl;
        viennadata::access<BoxVolumeKey, double>()(*eit) = 2.0 * volume_contribution; //volume contribution of both box volumes associated with the edge
        viennadata::access<BoxVolumeKey, double>()(v0) += volume_contribution;
        viennadata::access<BoxVolumeKey, double>()(v1) += volume_contribution;
      }

    } //write_voronoi_info(tetrahedron_tag)



    template <typename EdgeLenKey,
              typename InterfaceAreaKey,
              typename BoxVolumeKey,
              typename DomainType>
    void write_voronoi_info(DomainType const & domain, viennagrid::hexahedron_tag)
    {
      std::cout << "Warning: Voronoi info for hexahedron is only correct when having regular cuboids only." << std::endl;
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
            /*std::cout << cell_center << ", " << facet_center << ", " << edge_midpoint << std::endl;
            std::cout << "Interface contrib: " << spanned_volume(cell_center, facet_center, edge_midpoint) << std::endl;*/
            viennadata::access<InterfaceAreaKey, double>()(*eocit) += spanned_volume(cell_center, facet_center, edge_midpoint);
            
            //box volume contribution:
            double edge_contribution = 0;
            for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                      voeit != vertices_on_edge.end();
                                    ++voeit)
            {
              double contribution = spanned_volume(cell_center, facet_center, edge_midpoint, voeit->getPoint());
              edge_contribution += contribution;
              viennadata::access<BoxVolumeKey, double>()(*voeit) += contribution;
              
            }
            viennadata::access<BoxVolumeKey, double>()(*eocit) += edge_contribution;
          } //for edges on facet

        } //for facets on cell

      } //for cells
      
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
