#ifndef VIENNAGRID_ALGORITHM_VORONOI_HPP
#define VIENNAGRID_ALGORITHM_VORONOI_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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
#include "viennagrid/algorithm/inner_prod.hpp"

#include "viennadata/api.hpp"

/** @file voronoi.hpp
    @brief Computes the Voronoi information for a Delaunay triangulation.
*/


namespace viennagrid
{
  namespace result_of
  {
    template <typename CellType>
    struct voronoi_cell_contribution
    {
      typedef std::vector< std::pair<CellType const *, double> >   type;
    };
  }
  
  
  namespace detail
  {
    /** @brief Adds a pair [CellPtr, contribution] to the Voronoi quantities stored in the container. If data for the particular Cell are already stored, no new element is inserted, but existing 'contribution' is updated. 
     */
    template <typename ContainerType, typename PairType>
    void voronoi_unique_quantity_update(ContainerType & container,
                                        PairType const & update_pair)
    {
      bool found = false;
      for (typename ContainerType::iterator it = container.begin();
                                            it != container.end();
                                          ++it)
      {
        if (it->first == update_pair.first)
        {
          it->second += update_pair.second;
          found = true;
          break;
        }
      }
      
      if (!found)
      {
        container.push_back(update_pair);
      }
    }
    
    
    /** @brief Implementation of the computation of Voronoi quantities for a quadrilateral domain */
    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void clear_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key)
    {
      //std::cout << "Warning: Voronoi info for quadrilaterals is only correct when having rectangles only." << std::endl;
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type              CellType;

      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type               VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                          VertexIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 1>::type               EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                            EdgeIterator;
      
      typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;

      // Ensure that everything is clean:
      VertexRange vertices = viennagrid::ncells<0>(domain);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(*vit) = 0;
        viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*vit).clear();
      }
      
      EdgeRange edges = viennagrid::ncells<1>(domain);
      for (EdgeIterator eit  = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        viennadata::access<InterfaceAreaKey, double>(interface_key)(*eit) = 0;
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eit) = 0;
        viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eit).clear();
        viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eit).clear();
      }

    }
    
    /** @brief Implementation of the computation of Voronoi quantities for a one-dimensional domain (line, 1-simplex) */
    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void write_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key,
                            viennagrid::simplex_tag<1>)
    {
      typedef typename DomainType::config_type           Config;
      typedef typename Config::cell_tag                  CellTag;
      
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type              CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type               VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                          VertexIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                            CellIterator;
      
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type           VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type              VertexOnCellIterator;
      
      
      typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;

      // Ensure that everything is clean:
      clear_voronoi_info(domain, interface_key, box_volume_key);
      
      
      //
      // Write Voronoi information
      //
      
      CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        viennadata::access<InterfaceAreaKey, double>(interface_key)(*cit) = 1;
        viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*cit).push_back( std::make_pair( &(*cit), 1) );
        
        double edge_contribution = 0;
        VertexOnCellRange vertices_on_cell = viennagrid::ncells<0>(*cit);
        for (VertexOnCellIterator vocit  = vertices_on_cell.begin();
                                  vocit != vertices_on_cell.end();
                                ++vocit)
        {
          double contribution = volume(*cit) / 2.0;
          edge_contribution += contribution;
          viennadata::access<BoxVolumeKey, double>(box_volume_key)(*vocit) += contribution;
          viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*vocit).push_back( std::make_pair( &(*cit), contribution) );
        }

        viennadata::access<BoxVolumeKey, double>(box_volume_key)(*cit) = edge_contribution;
        viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*cit).push_back( std::make_pair( &(*cit), edge_contribution) );

      }
    }
    
    /** @brief Implementation of the computation of Voronoi quantities for a one-dimensional domain (line, 1-hypercube) */
    template <typename DomainType,
              typename InterfaceAreaKey,
              typename BoxVolumeKey>
    void write_voronoi_info(DomainType const & domain,
                            InterfaceAreaKey const & interface_key,
                            BoxVolumeKey const & box_volume_key,
                            viennagrid::hypercube_tag<1>)
    {
      write_voronoi_info(domain, interface_key, box_volume_key, viennagrid::simplex_tag<1>());
    }
    
    //
    // Voronoi information in two (topological) dimensions
    //
    /** @brief Implementation of the computation of Voronoi quantities for a quadrilateral domain */
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
      typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type   CellType;

      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type               VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                          VertexIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 1>::type               EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                            EdgeIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                            CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                 EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                      EdgeOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                 VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                    VertexOnEdgeIterator;
      
      typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;

      // Ensure that everything is clean:
      clear_voronoi_info(domain, interface_key, box_volume_key);

      
      //
      // Algorithm: Iterate over all cells, compute circumcenter and add interface area to edge, box volume to vertex.
      //
      
      CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
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
          double interface_contribution = spanned_volume(circ_center, edge_midpoint);
          viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += interface_contribution;
          viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eocit).push_back( std::make_pair( &(*cit), interface_contribution) );

          
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
            viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit).push_back( std::make_pair( &(*cit), contribution) );
          }
          viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += edge_contribution;
          viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit).push_back( std::make_pair( &(*cit), edge_contribution) );
        } //for edges on cells
        
      } //for cells
      
    } //write_voronoi_info(triangle_tag)
    
    
    /** @brief Converts a point to local (barycentric) coordinates (lambda_1, lambda_2). lambda_3 = 1 - lambda_1 - lambda_2 is not stored explicitly  */
    template <typename PointType, typename ConfigType>
    PointType point_to_local_coordinates(PointType const & p, viennagrid::element_t<ConfigType, viennagrid::simplex_tag<2> > const & triangle)
    {
      typedef viennagrid::element_t<ConfigType, viennagrid::simplex_tag<2> >                     CellType;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type               VertexRange;
      typedef typename PointType::value_type                   value_type;
      
      VertexRange vertices = viennagrid::ncells<0>(triangle);
      
      PointType const & a = vertices[0].point();
      PointType const & b = vertices[1].point();
      PointType const & c = vertices[2].point();
      
      PointType v0 = b - a;
      PointType v1 = c - a;
      PointType vp = p - a;
      
      double det = v0[0] * v1[1] - v0[1] * v1[0];
      
      return PointType( (v1[1] * vp[0] - v1[0] * vp[1]) / det,
                        (v0[0] * vp[1] - v0[1] * vp[0]) / det  );
    }
    

    /** @brief Computes the intersection of the two lines [a,b] and [c,d] */
    template <typename PointType>
    PointType line_intersection(PointType const & a, PointType const & b,
                                PointType const & c, PointType const & d)
    {
      double det = (a[0] - b[0])*(c[1] - d[1]) - (a[1] - b[1])*(c[0] - d[0]);
      
      if (det == 0)
        std::cerr << "viennagrid::detail::line_intersection(): Warning: Determinant is zero!" << std::endl;
      
      double px =   (a[0]*b[1] - a[1]*b[0]) * (c[0] - d[0])
                  - (a[0] - b[0]) * (c[0]*d[1] - c[1]*d[0]);
      double py =   (a[0]*b[1] - a[1]*b[0]) * (c[1] - d[1])
                  - (a[1] - b[1]) * (c[0]*d[1] - c[1]*d[0]);
                  
      return PointType(px / det, py / det);
    }


    /** @brief Implementation of the computation of Voronoi quantities for a triangular domain */
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
      typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type               CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 1>::type                          EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                       EdgeIterator;

      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type                            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                               VertexOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, CellTag::dim>::type                 CellOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<CellOnEdgeRange>::type                                 CellOnEdgeIterator;

      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                 EdgeOnCellIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;

      // Ensure that everything is clean:
      clear_voronoi_info(domain, interface_key, box_volume_key);


      CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        
        PointType circ_center_local = point_to_local_coordinates(circ_center, *cit);
        
        if (    (circ_center_local[0] < 0)
             || (circ_center_local[1] < 0)
             || (circ_center_local[0] + circ_center_local[1] > 1.0) )   //circumcenter is outside triangle - things get complicated
        {
          EdgeOnCellRange edges_on_cell = viennagrid::ncells<1>(*cit);
          VertexOnCellRange vertices_on_cell = viennagrid::ncells<0>(*cit);
          
          //
          // Step 1: Get intersected edge, find opposite vertex and the 'other' triangle
          //
          EdgeType const * intersected_edge_ptr = NULL;
          VertexType const * opposite_vertex_ptr = NULL;
          if (circ_center_local[1] < 0)
          {
            intersected_edge_ptr = &(edges_on_cell[0]);
            opposite_vertex_ptr = &(vertices_on_cell[2]);
          }
          else if (circ_center_local[0] < 0)
          {
            intersected_edge_ptr = &(edges_on_cell[1]);
            opposite_vertex_ptr = &(vertices_on_cell[1]);
          }
          else
          {
            intersected_edge_ptr = &(edges_on_cell[2]);
            opposite_vertex_ptr = &(vertices_on_cell[0]);
          }
          
          //find 'other' triangle
          CellType const * other_cell = NULL;
          CellOnEdgeRange other_cells = viennagrid::ncells(*intersected_edge_ptr, domain);
          for (CellOnEdgeIterator coeit  = other_cells.begin();
                                  coeit != other_cells.end();
                                ++coeit)
          {
            if ( &(*coeit) != &(*cit) )
            {
              other_cell = &(*coeit);
              break;
            }
          }
          
          //
          // Step 2: Precompute intersection of opposite vertex with intersected edge
          //
          VertexOnEdgeRange vertices_on_intersected_edge = viennagrid::ncells<0>(*intersected_edge_ptr);
          PointType opposite_vertex_edge_intersection = line_intersection( opposite_vertex_ptr->point(), circ_center,
                                                                           vertices_on_intersected_edge[0].point(), vertices_on_intersected_edge[1].point());
          
          //
          // Step 3: Compute contributions
          //
          for (EdgeOnCellIterator eocit  = edges_on_cell.begin();
                                  eocit != edges_on_cell.end();
                                ++eocit)
          {
            PointType edge_midpoint = circumcenter(*eocit);
            
            if ( intersected_edge_ptr != &(*eocit) )  // non-intersected edge: Split contributions into contributions from cell itself and contribution from other cell
            {
              PointType edge_intersection = line_intersection( edge_midpoint, circ_center,
                                                               vertices_on_intersected_edge[0].point(), vertices_on_intersected_edge[1].point());
              
              //
              // Interface contributions (two contributions: From edge midpoint to intersected edge, and from intersected edge to circumcenter)
              //

              double interface_contribution = 0;
              
              // contribution from cell:
              interface_contribution = spanned_volume(edge_intersection, edge_midpoint);
              viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += interface_contribution;
              voronoi_unique_quantity_update(viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eocit),
                                             std::make_pair( &(*cit), interface_contribution) );

              // contribution from other cell containing the circumcenter:
              interface_contribution = spanned_volume(edge_intersection, circ_center);
              viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += interface_contribution;
              if ( other_cell != NULL)
                voronoi_unique_quantity_update(viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eocit),
                                               std::make_pair( other_cell, interface_contribution) );

              
              
              //
              // Box volume contribution:
              //
              double edge_contribution = 0;
                
              // contribution from cell itself:  
              VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eocit);
              for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                        voeit != vertices_on_edge.end();
                                      ++voeit)
              {
                double contribution = spanned_volume(edge_intersection, edge_midpoint, voeit->point());
                edge_contribution += contribution;
                viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution;
                voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit),
                                               std::make_pair( &(*cit), contribution) );
                
                if ( &(*voeit) != opposite_vertex_ptr )  // non-splitted contribution
                {
                  if (other_cell != NULL)
                  {
                    double contribution_other = spanned_volume(circ_center, edge_intersection, voeit->point());
                    viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution_other;
                    voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit),
                                                   std::make_pair(other_cell, contribution_other) );
                    viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += contribution_other;
                    voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit),
                                                   std::make_pair(other_cell, contribution_other) );
                  }
                }
                else //splitted contribution around center of the edge. Contributes to opposite vertex
                {
                  double contribution_cell = spanned_volume(opposite_vertex_edge_intersection, edge_intersection, voeit->point());
                  viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution_cell;
                  voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit),
                                                 std::make_pair( &(*cit), contribution_cell) );
                  viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += contribution_cell;
                  voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit),
                                                 std::make_pair( &(*cit), contribution_cell) );
                  
                  if (other_cell != NULL)
                  {
                    double contribution_other = spanned_volume(circ_center, edge_intersection, opposite_vertex_edge_intersection);
                    viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution_other;
                    voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit),
                                                   std::make_pair(other_cell, contribution_other) );
                    viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += contribution_other;
                    voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit),
                                                   std::make_pair(other_cell, contribution_other) );
                  }
                }
                
                
              }
              viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += edge_contribution;
              voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit),
                                             std::make_pair( &(*cit), edge_contribution) );

            }
            else if (other_cell != NULL)// intersected edge: Write negative contributions to other cell
            {
              // interface contribution:
              double interface_contribution = spanned_volume(circ_center, edge_midpoint);
              viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) -= interface_contribution;
              voronoi_unique_quantity_update(viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eocit),
                                             std::make_pair(other_cell, -1.0 * interface_contribution) );

              
              //box volume contribution:
              double edge_contribution = 0;
              VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eocit);
              for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                        voeit != vertices_on_edge.end();
                                      ++voeit)
              {
                double contribution = spanned_volume(circ_center, edge_midpoint, voeit->point());
                edge_contribution += contribution;
                viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) -= contribution;
                voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit),
                                               std::make_pair(other_cell, -1.0 * contribution) );
              }
              viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) -= edge_contribution;
              voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit),
                                             std::make_pair(other_cell, -1.0 * edge_contribution) );
            }
            // else {}    //nothing to do for a boundary edge
            
          } //for edges on cells
          
        }
        else    // circumcenter  is inside triangle (everything is simple... ;-) )
        {

          //iterate over edges:
          EdgeOnCellRange edges_on_cell = viennagrid::ncells<1>(*cit);
          for (EdgeOnCellIterator eocit  = edges_on_cell.begin();
                                  eocit != edges_on_cell.end();
                                ++eocit)
          {
            PointType edge_midpoint = circumcenter(*eocit);

            // interface contribution:
            double interface_contribution = spanned_volume(circ_center, edge_midpoint);
            viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += interface_contribution;
            voronoi_unique_quantity_update(viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eocit),
                                           std::make_pair( &(*cit), interface_contribution) );

            
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
              voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit),
                                             std::make_pair( &(*cit), contribution) );
            }
            viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += edge_contribution;
            voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit),
                                           std::make_pair( &(*cit), edge_contribution) );
          } //for edges on cells
          
        }
        
      } //for cells
      
    }


    //
    // Voronoi information in three dimensions
    //

    /** @brief Implementation of the computation of Voronoi quantities for a tetrahedral domain
     *
     *  Note: Interface contributions per cell are only approximate for tetrahedral meshes, while they are precise for triangular meshes.
     *        They are consistent in the sense that 
     *          total_interface_volume = sum_over_cells(interface_volume_per_cell).
     *        As a consequence, the same holds true for volume contributions.
     *
     */
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
      typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type              CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim-1>::type  FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type                                      FacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, 1>::type                          EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                       EdgeIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, CellTag::dim-1>::type    FacetOnCellRange;
      typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                                FacetOnCellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<FacetType, 1>::type                           EdgeOnFacetRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnFacetRange>::type                                EdgeOnFacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;
      
      typedef std::pair<PointType, CellType const *>             PointWithCellInfo;
      typedef std::pair<std::pair<PointType, PointType>,
                        CellType const *>                        EdgePointsWithCellInfo;
      typedef std::vector< PointWithCellInfo >                   CircumcenterContainer;
      
      
      //
      // Step one: Write circumcenters to facets
      //
      std::map< FacetType const *, CircumcenterContainer>      circumcenters_on_facets;
      std::map< EdgeType const *,
                std::vector< EdgePointsWithCellInfo >
              >                                                interface_boundaries_on_edges;
                                   
      
      CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        PointType circ_center = circumcenter(*cit);
        
        FacetOnCellRange facets_on_cell = viennagrid::ncells<CellTag::dim-1>(*cit);
        for (FacetOnCellIterator focit  = facets_on_cell.begin();
                                focit != facets_on_cell.end();
                              ++focit)
        {
          circumcenters_on_facets[&(*focit)].push_back( PointWithCellInfo(circ_center, &(*cit)) );
        } //for edges on cells
      } //for cells


      //
      // Step two: Write lines connecting circumcenters to edges
      //
      FacetRange facets = viennagrid::ncells<CellTag::dim-1>(domain);
      for (FacetIterator fit  = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
        CircumcenterContainer & circ_centers = circumcenters_on_facets[&(*fit)];
        
        EdgeOnFacetRange edges_on_facet = viennagrid::ncells<1>(*fit);
        for (EdgeOnFacetIterator eofit  = edges_on_facet.begin();
                                eofit != edges_on_facet.end();
                              ++eofit)
        {
          if (circ_centers.size() == 1)
          {
            interface_boundaries_on_edges[&(*eofit)].push_back(std::make_pair( std::make_pair(circ_centers[0].first, circumcenter(*fit)),
                                                                               circ_centers[0].second)
                                                              );
            interface_boundaries_on_edges[&(*eofit)].push_back(std::make_pair( std::make_pair(circumcenter(*eofit), circumcenter(*fit)),
                                                                               circ_centers[0].second)
                                                              );
            /*viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> > 
                              >(interface_key)(*eofit).push_back(std::make_pair(circ_centers[0], circumcenter(*fit)));
            viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> >
                              >(interface_key)(*eofit).push_back(std::make_pair(circumcenter(*eofit), circumcenter(*fit))); */
          }
          else if (circ_centers.size() == 2)
          {
            PointType edge_mid = circ_centers[0].first + circ_centers[1].first;
            edge_mid /= 2.0;
            
            interface_boundaries_on_edges[&(*eofit)].push_back(std::make_pair( std::make_pair(circ_centers[0].first, edge_mid),
                                                                               circ_centers[0].second)
                                                              );
            interface_boundaries_on_edges[&(*eofit)].push_back(std::make_pair( std::make_pair(edge_mid, circ_centers[1].first),
                                                                               circ_centers[1].second)
                                                              );
            /* viennadata::access<InterfaceAreaKey,
                               std::vector<std::pair<PointType, PointType> >
                              >(interface_key)(*eofit).push_back(std::make_pair(circ_centers[0], circ_centers[1])); */
          }
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
        
        double edge_length = spanned_volume(v0.point(), v1.point());
        
        std::vector< EdgePointsWithCellInfo > & interface_segments = interface_boundaries_on_edges[&(*eit)];

        //
        // determine inner point of convex interface polygon:
        //
        PointType inner_point = (interface_segments[0].first.first + interface_segments[0].first.second) / 2.0;
        for (std::size_t i=1; i<interface_segments.size(); ++i)
        {
          inner_point += (interface_segments[i].first.first + interface_segments[i].first.second) / 2.0;
        }
        inner_point /= interface_segments.size();
        //std::cout << "Inner point: " << inner_point << std::endl;
        
        //
        // compute interface area
        //
        double interface_area = 0.0;
        for (std::size_t i=0; i<interface_segments.size(); ++i)
        {
          // interface area:
          double interface_contribution = spanned_volume(interface_segments[i].first.first, interface_segments[i].first.second, inner_point);
          if (interface_contribution > 0)
          {
            voronoi_unique_quantity_update(viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eit),
                                           std::make_pair(interface_segments[i].second, interface_contribution) );
            interface_area += interface_contribution;
            
            // box volume:
            double volume_contribution = interface_contribution * edge_length / 6.0;
            voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eit),
                                           std::make_pair(interface_segments[i].second, 2.0 * volume_contribution) ); //volume contribution of both box volumes associated with the edge
            voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(v0),
                                           std::make_pair( interface_segments[i].second, volume_contribution) );
            voronoi_unique_quantity_update(viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(v1),
                                           std::make_pair( interface_segments[i].second, volume_contribution) );
          }
        }
        
        //
        // Write Voronoi info:
        //
        
        //std::cout << "Interface area: " << interface_area << std::endl;
        viennadata::access<InterfaceAreaKey, double>(interface_key)(*eit) = interface_area;
        double volume_contribution = interface_area * edge_length / 6.0;
        //std::cout << "Volume contribution: " << volume_contribution << std::endl;
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eit) = 2.0 * volume_contribution; //volume contribution of both box volumes associated with the edge
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(v0) += volume_contribution;
        viennadata::access<BoxVolumeKey, double>(box_volume_key)(v1) += volume_contribution;
      } //for edges

    } //write_voronoi_info(tetrahedron_tag)



    /** @brief Implementation of the computation of Voronoi quantities for a hexahedral domain */
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
      typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type   CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                       CellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<CellType, 2>::type                            FacetOnCellRange;
      typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                                FacetOnCellIterator;

      typedef typename viennagrid::result_of::const_ncell_range<FacetType, 1>::type                           EdgeOnFacetRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnFacetRange>::type                                EdgeOnFacetIterator;
      
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                            VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                               VertexOnEdgeIterator;

      typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;
      
      //
      // Algorithm: Iterate over all cells, compute circumcenter and add interface area to edge, box volume to vertex.
      //
      
      CellRange cells = viennagrid::ncells<CellTag::dim>(domain);
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
            double interface_contribution = spanned_volume(cell_center, facet_center, edge_midpoint);
            viennadata::access<InterfaceAreaKey, CellContributionType>(interface_key)(*eocit).push_back(std::make_pair(&(*cit), interface_contribution) );   //Note: Due to iteration over cells there is no need to use a voronoi_unique_quantity_update() here
            viennadata::access<InterfaceAreaKey, double>(interface_key)(*eocit) += interface_contribution;
            
            //box volume contribution:
            double edge_contribution = 0;
            VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eocit);
            for (VertexOnEdgeIterator voeit  = vertices_on_edge.begin();
                                      voeit != vertices_on_edge.end();
                                    ++voeit)
            {
              double contribution = spanned_volume(cell_center, facet_center, edge_midpoint, voeit->point());
              viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*voeit).push_back(std::make_pair( &(*cit), contribution) );
              viennadata::access<BoxVolumeKey, double>(box_volume_key)(*voeit) += contribution;
              edge_contribution += contribution;
            }
            viennadata::access<BoxVolumeKey, CellContributionType>(box_volume_key)(*eocit).push_back(std::make_pair( &(*cit), edge_contribution) );
            viennadata::access<BoxVolumeKey, double>(box_volume_key)(*eocit) += edge_contribution;
          } //for edges on facet

        } //for facets on cell

      } //for cells
      
    }

  } //namespace detail

  //
  // The public interface
  //
  
  /** @brief Writes Voronoi information to the domain or segment using the provided keys
   * 
   * @param domain              A ViennaGrid domain or segment
   * @param interface_area_key  The ViennaData key used for storing the interface area on edges
   * @param box_volume_key      The ViennaData key used for storing the box volumes on edges and vertices
   */
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
    
  /** @brief Convenience overload for storing Voronoi information on a domain or segment. Uses the default keys for interface areas and box volumes. */
  template <typename DomainType>
  void apply_voronoi(DomainType const & domain)
  {
    apply_voronoi(domain, viennagrid::voronoi_interface_area_key(), viennagrid::voronoi_box_volume_key());
  }
    
} //namespace viennagrid
#endif
