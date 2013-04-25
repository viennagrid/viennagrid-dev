#ifndef VIENNAGRID_ALGORITHM_CENTROID_HPP
#define VIENNAGRID_ALGORITHM_CENTROID_HPP

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

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/volume.hpp"    //for domain/segment centroid

/** @file centroid.hpp
    @brief Computes the centroid (center of mass) for different cell types
*/


namespace viennagrid
{
  namespace detail
  {
    //
    // Calculation of centroid
    //
    /** @brief Implementation of the calculation of a centroid for a triangle */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::triangle_tag)
    {
      //typedef typename ElementType::config_type             Config;
      typedef typename ElementType::tag             ElementTag;
      typedef typename viennagrid::result_of::point_type<GeometricContainerType>::type                       PointType;
      typedef typename viennagrid::result_of::element<GeometricContainerType, vertex_tag>::type                         VertexType;
      typedef typename viennagrid::result_of::element<GeometricContainerType, line_tag>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_element_range<ElementType, vertex_tag>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type            VertexOnCellIterator;
      
      PointType p0(0.0, 0.0);
      
      VertexOnCellRange vertices = viennagrid::elements<vertex_tag>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        //p0 += vocit->point();
        p0 += viennagrid::point(geometric_container, *vocit);
      }
      
      p0 /= viennagrid::element_topology::boundary_cells<ElementTag, vertex_tag>::num;
      
      return p0;
    }

    //tetrahedron can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a tetrahedron */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::tetrahedron_tag)
    {
      return centroid(cell, geometric_container, viennagrid::triangle_tag());
    }

    //
    // Note: This works for rectangles only, but not for general quadrilateral
    //
    /** @brief Implementation of the calculation of a centroid for a quadrilateral */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::quadrilateral_tag)
    {
      return centroid(cell, geometric_container, viennagrid::triangle_tag());
    }

    //
    // Note: This works for cuboids only, but not for general hexahedra
    //
    /** @brief Implementation of the calculation of a centroid for a hexahedron */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::hexahedron_tag)
    {
      return centroid(cell, geometric_container, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a line (1-simplex) */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::simplex_tag<1>)
    {
      return centroid(cell, geometric_container, viennagrid::triangle_tag());
    }

    /** @brief Implementation of the calculation of a centroid for a line (1-hypercube) */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::hypercube_tag<1>)
    {
      return centroid(cell, geometric_container, viennagrid::triangle_tag());
    }

    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a centroid for a point */
    template <typename ElementType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid(ElementType const & cell, const GeometricContainerType & geometric_container, viennagrid::vertex_tag)
    {
      return cell.point();
    }




    template <typename ElementTypeOrTag, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
    centroid_domain(const GeometricContainerType & geometric_container)
    {
      //typedef typename DomainSegmentType::config_type                                      ConfigType;
      //typedef typename ElementType::tag                                                CellTag;
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTag>::type ElementTag;
      
      typedef typename viennagrid::result_of::point_type<GeometricContainerType>::type                      PointType;
      typedef typename viennagrid::result_of::element<GeometricContainerType, vertex_tag>::type                         VertexType;
      typedef typename viennagrid::result_of::element<GeometricContainerType, line_tag>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_element_range<GeometricContainerType,
                                                                ElementTag>::type  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                         CellIterator;
      
      PointType result = 0;
      double volume = 0;
      
      CellRange cells = viennagrid::elements<ElementTag>(geometric_container);
      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        double vol_cell = viennagrid::volume(*cit);
        result += vol_cell * centroid(*cit);
        volume += vol_cell;
      }
      
      return result / volume;
    }
    
  } //namespace detail


  /** @brief The public interface function for the computation of a centroid
   * 
   * @param cell    The n-cell for which the centroid should be computed
   */
    template <typename CellType, typename GeometricContainerType>
    typename viennagrid::result_of::point_type<GeometricContainerType>::type
  centroid(CellType const & cell, const GeometricContainerType & geometric_container)
  {
    return detail::centroid(cell, geometric_container, typename CellType::tag());
  }
  
  template<typename ElementTypeOrTag, typename vector_type, typename topology_type, typename metainfo_collection_type>
  typename viennagrid::result_of::coord_type<
    typename viennagrid::result_of::point_type<
        domain_t<vector_type, topology_type, metainfo_collection_type>
    >::type
  >::type
  centroid(const domain_t<vector_type, topology_type, metainfo_collection_type> & domain)
  {
      return detail::centroid_domain<ElementTypeOrTag>(domain);
  }


} //namespace viennagrid
#endif
