#ifndef VIENNAGRID_ALGORITHM_EXTRACT_SEED_POINTS_HPP
#define VIENNAGRID_ALGORITHM_EXTRACT_SEED_POINTS_HPP

#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"

namespace viennagrid
{
  namespace detail
  {
    template<typename MeshT, typename UnvisitedCellMapT>
    void neighbor_mark( MeshT const & mesh, UnvisitedCellMapT & unvisitied_cells )
    {
      bool found = true;
      while (found)
      {
        found = false;

        for (typename UnvisitedCellMapT::iterator ucit = unvisitied_cells.begin(); ucit != unvisitied_cells.end(); )
        {
          typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTagType;
          typedef typename viennagrid::result_of::facet_tag<MeshT>::type FacetTagType;
          typedef typename viennagrid::result_of::const_neighbor_range<MeshT, CellTagType, FacetTagType>::type NeighborRangeType;
          typedef typename viennagrid::result_of::iterator<NeighborRangeType>::type NeighborIteratorType;

          NeighborRangeType neighbors( mesh, ucit->second );
          NeighborIteratorType ncit = neighbors.begin();
          for (; ncit != neighbors.end(); ++ncit)
          {
            typename UnvisitedCellMapT::iterator ucit2 = unvisitied_cells.find( ncit->id() );
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
  }

  template<typename MeshSegmentT, typename SeedPointContainerT>
  void extract_seed_points( MeshSegmentT const & mesh, SeedPointContainerT & seed_points, int segment_id )
  {
    typedef typename viennagrid::result_of::cell_id<MeshSegmentT>::type CellIDType;
    typedef typename viennagrid::result_of::const_cell_handle<MeshSegmentT>::type ConstCellHandleType;

    typedef typename viennagrid::result_of::const_cell_range<MeshSegmentT>::type CellRangeType;
    typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

    CellRangeType cells(mesh);

    if (!cells.empty())
    {
      typedef std::map<CellIDType, ConstCellHandleType> UnvisitedCellMapType;
      UnvisitedCellMapType unvisited_cells;

      for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        unvisited_cells[ cit->id() ] = cit.handle();

      while (!unvisited_cells.empty())
      {
        for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          typename UnvisitedCellMapType::iterator ucit = unvisited_cells.find( cit->id() );
          if (ucit == unvisited_cells.end())
            continue;

          seed_points.push_back( std::make_pair(viennagrid::centroid(*cit), segment_id) );
          unvisited_cells.erase( ucit );

          neighbor_mark( mesh, unvisited_cells );
        }
      }
    }
  }

  template<typename MeshSegmentT, typename SegmentationT, typename SeedPointContainerT>
  void extract_seed_points( MeshSegmentT const & mesh, SegmentationT const & segmentation, SeedPointContainerT & seed_points )
  {
    if (segmentation.empty())
      extract_seed_points( mesh, seed_points, 0);
    else
      for (typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit)
        extract_seed_points( *sit, seed_points, sit->id() );
  }
}


#endif
