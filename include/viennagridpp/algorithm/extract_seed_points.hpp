#ifndef VIENNAGRID_ALGORITHM_EXTRACT_SEED_POINTS_HPP
#define VIENNAGRID_ALGORITHM_EXTRACT_SEED_POINTS_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagridpp/algorithm/centroid.hpp"

/** @file viennagrid/algorithm/extract_seed_points.hpp
    @brief Extraction of seed points per segment of a mesh.
*/

namespace viennagrid
{
  namespace detail
  {
    /** @brief For internal use only. */
    template<typename MeshT, typename UnvisitedCellMapT>
    void neighbor_mark( MeshT const & mesh, UnvisitedCellMapT & unvisitied_cells )
    {
      bool found = true;
      while (found)
      {
        found = false;

        for (typename UnvisitedCellMapT::iterator ucit = unvisitied_cells.begin(); ucit != unvisitied_cells.end(); )
        {
          typedef typename viennagrid::result_of::const_neighbor_range<MeshT>::type NeighborRangeType;
          typedef typename viennagrid::result_of::iterator<NeighborRangeType>::type NeighborIteratorType;

          NeighborRangeType neighbors( mesh, (*ucit).second,
                                       topologic_dimension((*ucit).second)-1, topologic_dimension((*ucit).second) );
          NeighborIteratorType ncit = neighbors.begin();
          for (; ncit != neighbors.end(); ++ncit)
          {
            typename UnvisitedCellMapT::iterator ucit2 = unvisitied_cells.find( (*ncit).id() );
            if (ucit2 == unvisitied_cells.end())
              break;
          }

          if (ncit != neighbors.end())
          {
            found = true;
            unvisitied_cells.erase( ucit++ );
          }
          else
            ++ucit;
        }
      }
    }


    /** @brief Extracts seed points of a mesh. For each connected part of the mesh, a point which is inside this part is added to the seed_points container.
    *
    * @param mesh                    The input mesh
    * @param seed_points             A container of seed points. The container has to support .push_back() for points of the mesh.
    */
    template<typename MeshSegmentT, typename PointContainerT>
    void extract_seed_points_impl( MeshSegmentT const & mesh, PointContainerT & seed_points )
    {
      typedef typename viennagrid::result_of::element_id<MeshSegmentT>::type CellIDType;
      typedef typename viennagrid::result_of::const_element<MeshSegmentT>::type ConstCellType;

      typedef typename viennagrid::result_of::const_cell_range<MeshSegmentT>::type CellRangeType;
      typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

      CellRangeType cells(mesh);

      if (!cells.empty())
      {
        typedef std::map<CellIDType, ConstCellType> UnvisitedCellMapType;
        UnvisitedCellMapType unvisited_cells;

        for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          unvisited_cells[ (*cit).id() ] = *cit;

        while (!unvisited_cells.empty())
        {
          for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          {
            typename UnvisitedCellMapType::iterator ucit = unvisited_cells.find( (*cit).id() );
            if (ucit == unvisited_cells.end())
              continue;

            seed_points.push_back( viennagrid::centroid(*cit) );
            unvisited_cells.erase( ucit );

            neighbor_mark( mesh, unvisited_cells );
          }
        }
      }
    }

  }



  /** @brief Extracts seed points of a mesh with segmentation. For each segment, seed points are extracted.
   *
   * @param mesh                    The input mesh
   * @param segmentation            The input segmentation
   * @param seed_points             A container of seed points and segment ids. The container has to support .push_back() for std::pair<MeshPointType,int>
   */
  template<typename SeedPointContainerT>
  void extract_seed_points( viennagrid::const_mesh_t const & mesh, SeedPointContainerT & seed_points )
  {
    typedef viennagrid::const_mesh_t MeshType;
    typedef typename viennagrid::result_of::point<MeshType>::type PointType;

    viennagrid_int num_regions = mesh.region_count();
    if (num_regions <= 1)
    {
      std::vector<PointType> points;
      detail::extract_seed_points_impl(mesh, points);
      for (unsigned int i = 0; i < points.size(); ++i)
        seed_points.push_back( std::make_pair(points[i], 0) );
    }
    else
    {
      typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
      typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;
      RegionRangeType regions(mesh);
      for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
      {
        std::vector<PointType> points;
        detail::extract_seed_points_impl( *rit, points );
        for (unsigned int i = 0; i < points.size(); ++i)
          seed_points.push_back( std::make_pair(points[i], (*rit).id()) );
      }
    }
  }
}


#endif
