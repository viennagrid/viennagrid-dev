#ifndef VIENNAGRID_ALGORITHM_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_VOLUME_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/core.hpp"
// #include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/accessor.hpp"

/** @file viennagrid/algorithm/volume.hpp
    @brief Computes the volume of different cell types as well as meshs and segments
*/


namespace viennagrid
{
  namespace detail
  {

    /** @brief Computes the volume of topologically zero-dimensional elements (vertices). Degenerate case, returns 1 */


    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<PointAccessorT, ElementT>::type
    volume_impl(PointAccessorT const accessor, ElementT const & element)
    {
      typedef typename viennagrid::result_of::point< PointAccessorT, ElementT >::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type CoordType;


      if (element.tag().is_vertex())
        return CoordType(1);

      typedef typename viennagrid::result_of::const_vertex_range<ElementT>::type ConstVertexRangeType;
      ConstVertexRangeType vertices(element);

      if (element.tag().is_line())
        return norm( accessor.get(vertices[0]) - accessor.get(vertices[1]) );

      if (element.tag().is_triangle())
        return spanned_volume(accessor.get(vertices[0]), accessor.get(vertices[1]), accessor.get(vertices[2]));

      if (element.tag().is_quadrilateral())
        return spanned_volume(accessor.get(vertices[0]), accessor.get(vertices[1]), accessor.get(vertices[3])) +
               spanned_volume(accessor.get(vertices[1]), accessor.get(vertices[2]), accessor.get(vertices[3])); //sum up the two triangular parts

      if (element.tag().is_tetrahedron())
        return spanned_volume( accessor.get(vertices[0]), accessor.get(vertices[1]),
                               accessor.get(vertices[2]), accessor.get(vertices[3]) );

      if (element.tag().is_hexahedron())
      {
        PointType const & p0 = accessor.get( vertices[0] );
        PointType const & p1 = accessor.get( vertices[1] );
        PointType const & p2 = accessor.get( vertices[2] );
        PointType const & p3 = accessor.get( vertices[3] );
        PointType const & p4 = accessor.get( vertices[4] );
        PointType const & p5 = accessor.get( vertices[5] );
        PointType const & p6 = accessor.get( vertices[6] );
        PointType const & p7 = accessor.get( vertices[7] );

        return spanned_volume(p0, p1, p3, p4)
              + spanned_volume(p4, p1, p3, p7)
              + spanned_volume(p4, p1, p7, p5)
              + spanned_volume(p1, p2, p3, p7)
              + spanned_volume(p1, p2, p7, p5)
              + spanned_volume(p5, p2, p7, p6);
      }


      if (element.tag().is_polygon())
      {
        typedef typename viennagrid::result_of::coord< PointType >::type NumericType;
        typedef typename viennagrid::result_of::const_vertex_range<ElementT>::type       VertexOnCellContainer;
        typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                    VertexOnCellIterator;

        VertexOnCellContainer range( element );
        if (range.size() < 3) return 0;
        VertexOnCellIterator it1 = range.begin();
        VertexOnCellIterator it2 = it1; ++it2;

        PointType origin = accessor.get(*it1);

        NumericType volume = 0;

        for (; it2 != range.end(); ++it1, ++it2)
            volume += signed_spanned_volume(origin, accessor.get(*it1), accessor.get(*it2));


        it1 = range.end(); --it1;
        volume += signed_spanned_volume( origin, accessor.get(*it1), accessor.get(*range.begin()) );

        return std::abs(volume);
      }

      assert(false);
      return CoordType(-1);
    }





//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const, ElementT const &, viennagrid::vertex_tag)
//     {
//       return typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type(1);
//     }
//
//     /** @brief Computes the volume of topologically one-dimensional elements (lines, 1-simplex).*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::simplex_tag<1>)
//     {
//       typedef typename PointAccessorT::value_type PointType;
//
//       PointType const & p0 = accessor( vertices(cell)[0] );
//       PointType const & p1 = accessor( vertices(cell)[1] );
//
//       return norm(p0 - p1);
//     }

//     /** @brief Computes the volume of topologically one-dimensional elements (lines, 1-hypercube).*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::hypercube_tag<1>)
//     {
//       return volume_impl(accessor, cell, viennagrid::simplex_tag<1>());
//     }

//     //topologically two-dimensional elements
//     /** @brief Computes the two-dimensional volume of a triangle.*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::triangle_tag)
//     {
//       typedef typename PointAccessorT::value_type PointType;
//
//       PointType const & p0 = accessor( vertices(cell)[0] );
//       PointType const & p1 = accessor( vertices(cell)[1] );
//       PointType const & p2 = accessor( vertices(cell)[2] );
//
//       return spanned_volume(p0, p1, p2);
//     }

//     /** @brief Computes the two-dimensional volume of a quadrilateral.*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::quadrilateral_tag)
//     {
//       typedef typename PointAccessorT::value_type PointType;
//
//       PointType const & p0 = accessor( vertices(cell)[0] );
//       PointType const & p1 = accessor( vertices(cell)[1] );
//       PointType const & p2 = accessor( vertices(cell)[2] );
//       PointType const & p3 = accessor( vertices(cell)[3] );
//
//       return spanned_volume(p0, p1, p3) + spanned_volume(p1, p2, p3); //sum up the two triangular parts
//     }


//     /** @brief Computes the two-dimensional volume of a polygon.*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::polygon_tag)
//     {
//       typedef typename PointAccessorT::value_type PointType;
//       typedef typename viennagrid::result_of::coord< PointType >::type NumericType;
//       typedef typename viennagrid::result_of::const_element_range<ElementT, vertex_tag>::type       VertexOnCellContainer;
//       typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                    VertexOnCellIterator;
//
//
//       VertexOnCellContainer range( cell );
//       if (range.size() < 3) return 0;
//       VertexOnCellIterator it1 = range.begin();
//       VertexOnCellIterator it2 = it1; ++it2;
//
//       PointType origin = it1->point();
//
//       NumericType volume = 0;
//
//       for (; it2 != range.end(); ++it1, ++it2)
//           volume += signed_spanned_volume(origin, accessor(*it1), accessor(*it2));
//
//
//       it1 = range.end(); --it1;
//       volume += signed_spanned_volume( origin, accessor(*it1), accessor(*range.begin()) );
//
//       return std::abs(volume);
//     }


//     //topologically three-dimensional elements
//     /** @brief Computes the three-dimensional volume of a tetrahedron.*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::tetrahedron_tag)
//     {
//       typedef typename PointAccessorT::value_type PointType;
//
//       PointType const & p0 = accessor( vertices(cell)[0] );
//       PointType const & p1 = accessor( vertices(cell)[1] );
//       PointType const & p2 = accessor( vertices(cell)[2] );
//       PointType const & p3 = accessor( vertices(cell)[3] );
//
//       return spanned_volume(p0, p1, p2, p3);
//     }


//     /** @brief Computes the three-dimensional volume of a hexahedron.*/
//     template <typename PointAccessorT, typename ElementT>
//     typename viennagrid::result_of::coord< typename PointAccessorT::value_type >::type
//     volume_impl(PointAccessorT const accessor, ElementT const & cell, viennagrid::hexahedron_tag)
//     {
//       typedef typename PointAccessorT::value_type PointType;
//
//       PointType const & p0 = accessor( vertices(cell)[0] );
//       PointType const & p1 = accessor( vertices(cell)[1] );
//       PointType const & p2 = accessor( vertices(cell)[2] );
//       PointType const & p3 = accessor( vertices(cell)[3] );
//       PointType const & p4 = accessor( vertices(cell)[4] );
//       PointType const & p5 = accessor( vertices(cell)[5] );
//       PointType const & p6 = accessor( vertices(cell)[6] );
//       PointType const & p7 = accessor( vertices(cell)[7] );
//
//       //decompose hexahedron into six tetrahedra
//       return spanned_volume(p0, p1, p3, p4)
//              + spanned_volume(p4, p1, p3, p7)
//              + spanned_volume(p4, p1, p7, p5)
//              + spanned_volume(p1, p2, p3, p7)
//              + spanned_volume(p1, p2, p7, p5)
//              + spanned_volume(p5, p2, p7, p6);
//     }


    //
    /** @brief Dispatched function for computing the volume of a mesh or segment.*/
    template <typename MeshSegmentHandleType>
    typename viennagrid::result_of::coord< MeshSegmentHandleType >::type
    volume_mesh(MeshSegmentHandleType const & mesh_obj,
                viennagrid_int topologic_dimension
//                 viennagrid::element_tag_t cell_tag,
//                 viennagrid::element_tag_t cell_tag_end
               )
    {
      typedef typename viennagrid::result_of::const_element_range<MeshSegmentHandleType>::type  ElementContainerType;
      typedef typename viennagrid::result_of::iterator<ElementContainerType>::type              ElementContainerIterator;

      typename viennagrid::result_of::coord< MeshSegmentHandleType >::type new_volume = 0;
//       for (; cell_tag != cell_tag_end; ++cell_tag)
//       {
        ElementContainerType new_cells(mesh_obj, topologic_dimension);
        for (ElementContainerIterator new_cit = new_cells.begin();
                                      new_cit != new_cells.end();
                                    ++new_cit)
        {
          new_volume += volume( point_accessor(mesh_obj), *new_cit);
        }
//       }

      return new_volume;
    }
  } //namespace detail

  //
  // The public interface functions
  //
  /** @brief Returns the n-dimensional volume of a n-cell */
  template <typename PointAccessorT, bool element_is_const>
  typename viennagrid::result_of::coord<PointAccessorT, base_element<element_is_const> >::type
  volume(PointAccessorT const accessor, base_element<element_is_const> const & cell)
  {
    return detail::volume_impl( accessor, cell );
  }

  /** @brief Returns the n-dimensional volume of a n-cell */
  template <bool element_is_const>
  typename viennagrid::result_of::coord< base_element<element_is_const> >::type
  volume(base_element<element_is_const> const & cell)
  {
    return volume( root_mesh_point_accessor(), cell );
  }


  /** @brief Returns the n-dimensional volume of a whole mesh */
  template<typename ElementTagT, bool mesh_is_const>
  typename viennagrid::result_of::coord< base_mesh<mesh_is_const> >::type
  volume(base_mesh<mesh_is_const> const & mesh_obj)
  {
      return detail::volume_mesh(mesh_obj, ElementTagT(), ElementTagT()+1);
  }

  /** @brief Returns the n-dimensional volume of a whole mesh */
  template<bool mesh_is_const>
  typename viennagrid::result_of::coord< base_mesh<mesh_is_const> >::type
  volume(base_mesh<mesh_is_const> const & mesh_obj)
  {
    return detail::volume_mesh(mesh_obj, viennagrid::topologic_dimension(mesh_obj));
  }

  // default Element Tag = Cell Tag
  /** @brief Returns the n-dimensional volume of a segment */
  template<typename ElementTagT, bool mesh_region_is_const>
  typename viennagrid::result_of::coord< base_mesh_region<mesh_region_is_const> >::type
  volume(base_mesh_region<mesh_region_is_const> const & region)
  {
      return detail::volume_mesh(region, ElementTagT());
  }

  template<bool mesh_region_is_const>
  typename viennagrid::result_of::coord< base_mesh_region<mesh_region_is_const> >::type
  volume(base_mesh_region<mesh_region_is_const> const & region)
  {
      return detail::volume_mesh(region, region.mesh().cell_tag());
  }


} //namespace viennagrid
#endif
