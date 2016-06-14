#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HOLE_POINTS_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HOLE_POINTS_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <set>
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/intersect.hpp"


////////////////////////////////////////////////////////////////////////
//                       extract boundary.hpp                         //
////////////////////////////////////////////////////////////////////////
namespace viennagrid
{


  template<typename MeshT, typename ElementT, typename HullIDAccessor>
  void recursively_mark_neighbours( MeshT const & mesh,
                                    ElementT const & element,
                                    int hull_id,
                                    HullIDAccessor & hull_id_accessor,
                                    viennagrid_dimension connector_dimension)
  {
    if (!viennagrid::is_boundary(mesh, element))
      return;

    if (hull_id_accessor.get(element) != -1)
      return;

    hull_id_accessor.set(element, hull_id);

    typedef typename viennagrid::result_of::const_neighbor_range<MeshT>::type NeighbourRangeType;
    typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

    NeighbourRangeType neighbors(mesh, element, connector_dimension, topologic_dimension(mesh)-1);
    for (NeighbourRangeIterator neit = neighbors.begin(); neit != neighbors.end(); ++neit)
      recursively_mark_neighbours(mesh, *neit, hull_id, hull_id_accessor, connector_dimension);
  }


  template<typename IDT>
  bool is_direct_child(std::vector<IDT> const & parent_to_test, std::vector<IDT> child_to_test, IDT current)
  {
    typename std::vector<IDT>::iterator it = std::find(child_to_test.begin(), child_to_test.end(), current);
    if (it != child_to_test.end())
      child_to_test.erase(it);
    return parent_to_test == child_to_test;
  }


  template<typename MeshT, typename HolePointContainerT>
  void extract_hole_points(MeshT const & mesh,
                           HolePointContainerT & hole_points,
                           viennagrid_dimension boundary_dimension)
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;

    typedef typename viennagrid::result_of::element<MeshT>::type ElementType;
    typedef typename viennagrid::result_of::const_element_range<MeshT>::type ConstFacetRangeType;
    typedef typename viennagrid::result_of::iterator<ConstFacetRangeType>::type ConstFacetIteratorType;

    typedef typename viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryRangeType;
    typedef typename viennagrid::result_of::iterator<ConstBoundaryRangeType>::type ConstBoundaryIteratorType;

    ConstFacetRangeType facet_elements(mesh, boundary_dimension);

    std::vector<int> hull_id_container( facet_elements.size(), -1 );
    typename viennagrid::result_of::accessor<std::vector<int>, ElementType>::type hull_id_accessor(hull_id_container);

    int num_hulls = 0;
    for (ConstFacetIteratorType beit = facet_elements.begin(); beit != facet_elements.end(); ++beit)
    {
      if (hull_id_accessor.get(*beit) != -1)
        continue;

      if (!viennagrid::is_boundary(mesh, *beit))
        continue;

      recursively_mark_neighbours(mesh, *beit, num_hulls++, hull_id_accessor, boundary_dimension-1);
    }

    CoordType mesh_size = viennagrid::mesh_size(mesh);

    std::vector< std::vector<int> > hull_parents( num_hulls );

    for (int i = 0; i < num_hulls; ++i)
    {
      // finding an element which is in hull i
      ConstFacetIteratorType beit = facet_elements.begin();
      for (; beit != facet_elements.end(); ++beit)
      {
        if (hull_id_accessor.get(*beit) == i)
          break;
      }

      PointType centroid = viennagrid::centroid(*beit);
      PointType normal = viennagrid::normal_vector(*beit);
      normal /= viennagrid::norm_2(normal);
      normal *= mesh_size;

      for (int j = 0; j < num_hulls; ++j)
      {
        if (i == j)
          continue;


        std::set<ElementType> hull_vertices;
        std::vector<ElementType> hull_facets;


        for (ConstFacetIteratorType beit2 = facet_elements.begin(); beit2 != facet_elements.end(); ++beit2)
        {
          if (hull_id_accessor.get(*beit2) == j)
          {
            hull_facets.push_back(*beit2);

            ConstBoundaryRangeType vertices(*beit2, 0);
            for (ConstBoundaryIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
              hull_vertices.insert(*vit);
          }
        }


        std::set<ElementType> intersection_hull_vertices;
        int intersect_count = 0;
        for (typename std::set<ElementType>::const_iterator it = hull_vertices.begin(); it != hull_vertices.end(); ++it)
        {
          if (element_line_intersect(*it, centroid, centroid+normal, 1e-8))
          {
            ++intersect_count;
            intersection_hull_vertices.insert(*it);
          }
        }


        for (std::size_t i = 0; i != hull_facets.size(); ++i)
        {
          bool vertices_intersected = false;

          ConstBoundaryRangeType vertices(hull_facets[i], 0);
          for (ConstBoundaryIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
          {
            if (intersection_hull_vertices.find(*vit) != intersection_hull_vertices.end())
            {
              vertices_intersected = true;
              break;
            }
          }

          if (vertices_intersected)
            continue;


          if (element_line_intersect(hull_facets[i], centroid, centroid+normal, 1e-8))
          {
            ++intersect_count;
          }
        }

        if (intersect_count % 2 == 1)
          hull_parents[i].push_back(j);
      }
    }

    for (int i = 0; i < num_hulls; ++i)
      std::sort(hull_parents[i].begin(), hull_parents[i].end());

    std::vector< std::vector<int> > direct_hull_children( num_hulls );
    for (int child = 0; child < num_hulls; ++child)
    {
      for (int parent = 0; parent < num_hulls; ++parent)
      {
        if (child == parent)
          continue;

        std::vector<int> diff;
        std::set_difference( hull_parents[child].begin(), hull_parents[child].end(),
                             hull_parents[parent].begin(), hull_parents[parent].end(),
                             std::back_inserter(diff) );

        if (diff.size() == 1 && diff[0] == parent)
          direct_hull_children[parent].push_back(child);
      }
    }

    for (int hull = 0; hull < num_hulls; ++hull)
    {
      if (hull_parents[hull].size() % 2 != 0)
      {
        bool found_hole_point = false;
        for (ConstBoundaryIteratorType beit = facet_elements.begin(); beit != facet_elements.end(); ++beit)
        {
          if (hull_id_accessor.get(*beit) == hull)
          {

            for (ConstBoundaryIteratorType beit2 = facet_elements.begin(); beit2 != facet_elements.end(); ++beit2)
            {
              if (*beit2 == *beit)
                continue;

              std::size_t j = 0;
              for (; j < direct_hull_children[hull].size(); ++j)
              {
                if (hull_id_accessor.get(*beit2) == direct_hull_children[hull][j])
                  break;
              }

              if (j == direct_hull_children[hull].size() && hull_id_accessor.get(*beit2) != hull)
                continue;

              PointType centroid0 = viennagrid::centroid(*beit);
              PointType centroid1 = viennagrid::centroid(*beit2);

              PointType lv0 = viennagrid::get_point(viennagrid::vertices(*beit)[0]) - viennagrid::get_point(viennagrid::vertices(*beit)[1]);
              PointType lv1 = viennagrid::get_point(viennagrid::vertices(*beit2)[0]) - viennagrid::get_point(viennagrid::vertices(*beit2)[1]);
              lv0.normalize();
              lv1.normalize();

              if (std::abs(viennagrid::inner_prod(lv0, lv1)) > 1-1e-8)
                continue;

              ConstBoundaryIteratorType beit3 = facet_elements.begin();
              for (; beit3 != facet_elements.end(); ++beit3)
              {
                if (!viennagrid::is_boundary(mesh, *beit3))
                  continue;

                if ( (*beit3 == *beit) || (*beit3 == *beit2) )
                  continue;

                if (viennagrid::element_line_intersect(*beit3, centroid0, centroid1, 1e-8))
                {
                  break;
                }
              }

              if (beit3 == facet_elements.end())
              {
                hole_points.push_back( (centroid0+centroid1)/2.0 );
                found_hole_point = true;
                break;
              }
            }

            if (found_hole_point)
              break;
          }
        }

//         assert( found_hole_point );
      }
    }
  }


  template<typename MeshT, typename HolePointContainerT>
  void extract_hole_points(MeshT const & mesh, HolePointContainerT & hole_points)
  {
    extract_hole_points(mesh, hole_points, viennagrid::facet_dimension(mesh));
  }
}

#endif
