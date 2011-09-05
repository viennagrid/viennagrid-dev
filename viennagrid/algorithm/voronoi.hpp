#ifndef VIENNAGRID_ALGORITHM_VORONOI_HPP
#define VIENNAGRID_ALGORITHM_VORONOI_HPP

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

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/volume.hpp"

#include "viennadata/api.hpp"

/** @file voronoi.hpp
    @brief Computes the Voronoi information for a Delaunay triangulation.
*/


namespace viennagrid
{
  namespace detail
  {
    
    //
    // Voronoi information in two (topological) dimensions
    //
    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void write_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key,
                            viennagrid::quadrilateral_tag)
    {
      //std::cout << "Warning: Voronoi info for quadrilaterals is only correct when having rectangles only." << std::endl;
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                 EdgeOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      //
      // Algorithm: Iterate over all cells, compute circumcenter and add interface area to edge, box volume to vertex.
      //
      
      CellRange cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        //iterate over edges:
        EdgeOnCellRange edges_on_cell = viennagrid::ncells<1>(*cit);
        for (EdgeOnCellIterator eocit  = edges_on_cell.begin();
                                eocit != edges_on_cell.end();
                              ++eocit)
        {
          PointType edge_midpoint = circumcenter(*eocit);

          // interface contribution:
          viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += spanned_volume(circ_center, edge_midpoint);
          
          //box volume contribution:
          double edge_contribution = 0;
          VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eocit);
          for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                    voeit != vertices_on_edge.end();
                                  ++voeit)
          {
            double contribution = spanned_volume(circ_center, edge_midpoint, voeit->point());
            edge_contribution += contribution;
            viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution;
          }
          viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += edge_contribution;
        } //for edges on cells
        
      } //for cells
      
    } //write_voronoi_info(triangle_tag)


    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void write_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key,
                            viennagrid::triangle_tag)
    {
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 1>::type                          EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                       EdgeIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                 EdgeOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      //
      // Phase 1: Compute circumcenter of cells and store them on each of the edges (avoids coboundary operations)
      //
      
      CellRange cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        // store circumcenter on edges
        EdgeOnCellRange edges_on_cell = viennagrid::ncells<1>(*cit);
        for (EdgeOnCellIterator eocit  = edges_on_cell.begin();
                                eocit != edges_on_cell.end();
                              ++eocit)
        {
          viennadata::access<InterfaceAreaKey, std::vector<PointType> >(interface_key)(*eocit).push_back(circ_center);
        } //for edges on cells
      } //for cells


      //
      // Phase 2: Compute edge lengths, interface areas and box volumes from the information stored on edges:
      //
      EdgeRange edges = viennagrid::ncells<1>(domain);
      for (EdgeIterator eit  = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        double edge_length = volume(*eit);
        
        {
          std::vector<PointType> & circ_centers = viennadata::access<InterfaceAreaKey, std::vector<PointType> >(interface_key)(*eit);
          
          double interface_area = -42.0;
          if (circ_centers.size() == 1)  //edge is located on the domain boundary
            interface_area = spanned_volume(circ_centers[0], circumcenter(*eit));
          else if (circ_centers.size() == 2)
            interface_area = spanned_volume(circ_centers[0], circ_centers[1]);
          else
            throw "More than two circum centers for an edge in two dimensions!"; 
          
          viennadata::access<InterfaceAreaKey, double>(interface_key)(*eit) = interface_area;
          double volume_contribution = interface_area * edge_length / 4.0;
          viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eit) = 2.0 * volume_contribution; //volume contribution of both box volumes associated with the edge
          viennadata::access<BoxVolumeKey, double>(box_volume_key)(viennagrid::ncells<0>(*eit)[0]) += volume_contribution;
          viennadata::access<BoxVolumeKey, double>(box_volume_key)(viennagrid::ncells<0>(*eit)[1]) += volume_contribution;
        }
        
        //delete circ_centers:
        viennadata::erase<InterfaceAreaKey, std::vector<PointType> >()(*eit);
      }

    }


    //
    // Voronoi information in three dimensions
    //

    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void write_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key,
                            viennagrid::tetrahedron_tag)
    {
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell<Config, 2>::type                         FacetType;
      typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level-1>::type  FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type                                      FacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 1>::type                          EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                       EdgeIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, CellTag::topology_level-1>::type    FacetOnCellRange;
      typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                                FacetOnCellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<FacetType, 1>::type                           EdgeOnFacetRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnFacetRange>::type                                EdgeOnFacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      //
      // Step one: Write circumcenters to facets
      //
      
      CellRange cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        FacetOnCellRange facets_on_cell = viennagrid::ncells<CellTag::topology_level-1>(*cit);
        for (FacetOnCellIterator focit  = facets_on_cell.begin();
                                focit != facets_on_cell.end();
                              ++focit)
        {
          viennadata::access<InterfaceAreaKey, std::vector<PointType> >(interface_key)(*focit).push_back(circ_center);
        } //for edges on cells
      } //for cells


      //
      // Step two: Write lines connecting circumcenters to edges
      //
      FacetRange facets = viennagrid::ncells<CellTag::topology_level-1>(domain);
      for (FacetIterator fit  = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
        std::vector<PointType> & circ_centers = viennadata::access<InterfaceAreaKey, std::vector<PointType> >(interface_key)(*fit);
        
        EdgeOnFacetRange edges_on_facet = viennagrid::ncells<1>(*fit);
        for (EdgeOnFacetIterator eofit  = edges_on_facet.begin();
                                eofit != edges_on_facet.end();
                              ++eofit)
        {
          viennadata::access<InterfaceAreaKey, std::vector<PointType> >(interface_key)(*eofit).push_back(circ_centers[0]);
          
          if (circ_centers.size() == 1)
          {
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> > 
                              >(interface_key)(*eofit).push_back(std::make_pair(circ_centers[0], circumcenter(*fit)));
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> >
                              >(interface_key)(*eofit).push_back(std::make_pair(circumcenter(*eofit), circumcenter(*fit)));
          }
          else if (circ_centers.size() == 2)
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> >
                              >(interface_key)(*eofit).push_back(std::make_pair(circ_centers[0], circ_centers[1]));
          else
          {
            std::cerr << "circ_centers.size() = " << circ_centers.size() << std::endl;
            std::cerr << "*fit: " << *fit << std::endl;
            throw "More than two circumcenters for a facet in three dimensions!"; 
          }
            
        } //for edges on cells
      }


      //
      // Step three: Compute Voronoi information:
      //
      EdgeRange edges = viennagrid::ncells<1>(domain);
      for (EdgeIterator eit  = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        //get vertices of edge:
        VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eit);
        VertexOnEdgeIterator voeit = vertices_on_edge.begin();
        
        VertexType const & v0 = *voeit;
        ++voeit;
        VertexType const & v1 = *voeit;
        
        PointType const & p0 = v0.point();
        PointType const & p1 = v1.point();

        //write edge length
        double edge_length = spanned_volume(p0, p1);
        
        std::vector<std::pair<PointType, PointType> > & interface_segments =
          viennadata::access<InterfaceAreaKey, std::vector<std::pair<PointType, PointType> > >(interface_key)(*eit);

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
        viennadata::access<InterfaceAreaKey, double>(interface_key)(*eit) = interface_area;
        double volume_contribution = interface_area * edge_length / 6.0;
        //std::cout << "Volume contribution: " << volume_contribution << std::endl;
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eit) = 2.0 * volume_contribution; //volume contribution of both box volumes associated with the edge
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(v0) += volume_contribution;
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(v1) += volume_contribution;
      }

    } //write_voronoi_info(tetrahedron_tag)



    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void write_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key,
                            viennagrid::hexahedron_tag)
    {
      //std::cout << "Warning: Voronoi info for hexahedron is only correct when having regular cuboids only." << std::endl;
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell<Config, 2>::type                         FacetType;
      typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<CellType, 2>::type                            FacetOnCellRange;
      typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                                FacetOnCellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<FacetType, 1>::type                           EdgeOnFacetRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnFacetRange>::type                                EdgeOnFacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      //
      // Algorithm: Iterate over all cells, compute circumcenter and add interface area to edge, box volume to vertex.
      //
      
      CellRange cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType cell_center = circumcenter(*cit);

        FacetOnCellRange facets_on_cell = viennagrid::ncells<2>(*cit);
        for (FacetOnCellIterator focit  = facets_on_cell.begin();
                                focit != facets_on_cell.end();
                              ++focit)
        {
          PointType facet_center = circumcenter(*focit);
          
          //iterate over edges:
          EdgeOnFacetRange edges_on_facet = viennagrid::ncells<1>(*focit);
          for (EdgeOnFacetIterator eocit  = edges_on_facet.begin();
                                  eocit != edges_on_facet.end();
                                ++eocit)
          {
            PointType edge_midpoint = viennagrid::circumcenter(*eocit);
            
            // interface contribution:
            viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += spanned_volume(cell_center, facet_center, edge_midpoint);
            
            //box volume contribution:
            double edge_contribution = 0;
            VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eocit);
            for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                      voeit != vertices_on_edge.end();
                                    ++voeit)
            {
              double contribution = spanned_volume(cell_center, facet_center, edge_midpoint, voeit->point());
              edge_contribution += contribution;
              viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution;
            }
            viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += edge_contribution;
          } //for edges on facet

        } //for facets on cell

      } //for cells
      
    }

  } //namespace detail

  //
  // The public interface
  //
  template <typename DomainType,
            typename InterfaceAreaKey,
            typename BoxVolumeKey>
  void apply_voronoi(DomainType const & domain,
                     InterfaceAreaKey const & interface_area_key = viennagrid::voronoi_interface_area_key(),
                     BoxVolumeKey const & box_volume_key = viennagrid::voronoi_box_volume_key())
  {
    detail::write_voronoi_info(domain,
                               interface_area_key,
                               box_volume_key,
                               typename DomainType::config_type::cell_tag());
  }
    
  template <typename DomainType>
  void apply_voronoi(DomainType const & domain)
  {
    apply_voronoi(domain, viennagrid::voronoi_interface_area_key(), viennagrid::voronoi_box_volume_key());
  }
    
} //namespace viennagrid
#endif
