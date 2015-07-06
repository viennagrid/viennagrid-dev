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
    template<typename MeshRegionT, typename UnvisitedCellMapT, typename ElementT>
    void neighbor_mark( MeshRegionT const & mesh, UnvisitedCellMapT & unvisitied_cells, ElementT to_mark )
    {
      bool found = true;
      while (found)
      {
        found = false;

        for (typename UnvisitedCellMapT::iterator cit = unvisitied_cells.begin(); cit != unvisitied_cells.end(); ++cit)
        {
          typedef typename viennagrid::result_of::const_neighbor_range<MeshRegionT>::type NeighborRangeType;
          typedef typename viennagrid::result_of::iterator<NeighborRangeType>::type NeighborIteratorType;

          if ( (*cit).second == to_mark )
            continue;

          NeighborRangeType neighbors( mesh, (*cit).first, topologic_dimension((*cit).first)-1, topologic_dimension((*cit).first) );

          for (NeighborIteratorType ncit = neighbors.begin(); ncit != neighbors.end(); ++ncit)
          {
            typename UnvisitedCellMapT::iterator ncit2 = unvisitied_cells.find(*ncit);
            if (ncit2 != unvisitied_cells.end())
            {
              if ( (*ncit2).second == to_mark )
              {
                (*cit).second = to_mark;
                found = true;
              }
            }
          }
        }

      }
    }


    /** @brief Extracts seed points of a mesh. For each connected part of the mesh, a point which is inside this part is added to the seed_points container.
    *
    * @param mesh                    The input mesh
    * @param seed_points             A container of seed points. The container has to support .push_back() for points of the mesh.
    */
    template<typename MeshRegionT, typename PointContainerT>
    void extract_seed_points_impl( MeshRegionT const & mesh, PointContainerT & seed_points )
    {
      typedef typename viennagrid::result_of::const_element<MeshRegionT>::type ConstCellType;

      typedef typename viennagrid::result_of::const_cell_range<MeshRegionT>::type CellRangeType;
      typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

      CellRangeType cells(mesh);

      if (!cells.empty())
      {
        typedef std::map<ConstCellType, ConstCellType> UnvisitedCellMapType;
        UnvisitedCellMapType unvisited_cells;

        for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          unvisited_cells[*cit] = *cit;

        while (!unvisited_cells.empty())
        {
          for (typename UnvisitedCellMapType::iterator cit = unvisited_cells.begin(); cit != unvisited_cells.end(); )
          {
            seed_points.push_back( viennagrid::centroid( (*cit).first ) );
            neighbor_mark( mesh, unvisited_cells, (*cit).first );

            typename UnvisitedCellMapType::iterator cit2 = cit; ++cit2;
            for (; cit2 != unvisited_cells.end();)
            {
              if ( (*cit2).second == (*cit).first )
                unvisited_cells.erase(cit2++);
              else
                ++cit2;
            }

            unvisited_cells.erase(cit++);
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
  void extract_seed_points( viennagrid::const_mesh const & mesh, SeedPointContainerT & seed_points )
  {
    typedef viennagrid::const_mesh MeshType;
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
