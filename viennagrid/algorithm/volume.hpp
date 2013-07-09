#ifndef VIENNAGRID_ALGORITHM_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_VOLUME_HPP

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
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/accessor.hpp"

/** @file volume.hpp
    @brief Computes the volume of different cell types as well as domains and segments
*/


namespace viennagrid
{
  namespace detail
  {
    
    /** @brief Computes the volume of topologically zero-dimensional elements (vertices). Degenerate case, returns 1 */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const, ElementType const &, viennagrid::vertex_tag)
    {
      return typename viennagrid::result_of::coord_type<PointAccessorType>::type(1);
    }
    
    /** @brief Computes the volume of topologically one-dimensional elements (lines, 1-simplex).*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::simplex_tag<1>)
    {
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
      
      PointType const & p0 = accessor( vertices(cell)[0] );
      PointType const & p1 = accessor( vertices(cell)[1] );
      
      return norm(p0 - p1);
    }
    
    /** @brief Computes the volume of topologically one-dimensional elements (lines, 1-hypercube).*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::hypercube_tag<1>)
    {
      return volume_impl(accessor, cell, viennagrid::simplex_tag<1>());
    }

    //topologically two-dimensional elements
    /** @brief Computes the two-dimensional volume of a triangle.*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
      
      PointType const & p0 = accessor( vertices(cell)[0] );
      PointType const & p1 = accessor( vertices(cell)[1] );
      PointType const & p2 = accessor( vertices(cell)[2] );
      
      return spanned_volume(p0, p1, p2);
    }

    /** @brief Computes the two-dimensional volume of a quadrilateral.*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
      
      PointType const & p0 = accessor( vertices(cell)[0] );
      PointType const & p1 = accessor( vertices(cell)[1] );
      PointType const & p2 = accessor( vertices(cell)[2] );
      PointType const & p3 = accessor( vertices(cell)[3] );
      
      return spanned_volume(p0, p1, p3) + spanned_volume(p1, p2, p3); //sum up the two triangular parts
    }
    
    
    /** @brief Computes the two-dimensional volume of a polygon.*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::polygon_tag)
    {
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
      typedef typename viennagrid::result_of::coord_type< PointType >::type NumericType;
      typedef typename viennagrid::result_of::const_element_range<ElementType, vertex_tag>::type       VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type       VertexOnCellIterator;
      
      
      VertexOnCellContainer range = viennagrid::elements( cell );
      if (range.size() < 3) return 0;
      VertexOnCellIterator it1 = range.begin();
      VertexOnCellIterator it2 = it1; ++it2;
      
      PointType origin = it1->point();
      
      NumericType volume = 0;
      
      for (; it2 != range.end(); ++it1, ++it2)
          volume += signed_spanned_volume(origin, accessor(*it1), accessor(*it2));
      

      it1 = range.end(); --it1;
      volume += signed_spanned_volume( origin, accessor(*it1), accessor(*range.begin()) );
      
      return std::abs(volume);
    }
    

    //topologically three-dimensional elements
    /** @brief Computes the three-dimensional volume of a tetrahedron.*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
      
      PointType const & p0 = accessor( vertices(cell)[0] );
      PointType const & p1 = accessor( vertices(cell)[1] );
      PointType const & p2 = accessor( vertices(cell)[2] );
      PointType const & p3 = accessor( vertices(cell)[3] );
      
      return spanned_volume(p0, p1, p2, p3);
    }


    /** @brief Computes the three-dimensional volume of a hexahedron.*/
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    volume_impl(PointAccessorType const accessor, ElementType const & cell, viennagrid::hexahedron_tag)
    {
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
      
      PointType const & p0 = accessor( vertices(cell)[0] );
      PointType const & p1 = accessor( vertices(cell)[1] );
      PointType const & p2 = accessor( vertices(cell)[2] );
      PointType const & p3 = accessor( vertices(cell)[3] );
      PointType const & p4 = accessor( vertices(cell)[4] );
      PointType const & p5 = accessor( vertices(cell)[5] );
      PointType const & p6 = accessor( vertices(cell)[6] );
      PointType const & p7 = accessor( vertices(cell)[7] );

      //decompose hexahedron into six tetrahedra
      return spanned_volume(p0, p1, p3, p4)
             + spanned_volume(p4, p1, p3, p7)
             + spanned_volume(p4, p1, p7, p5)
             + spanned_volume(p1, p2, p3, p7)
             + spanned_volume(p1, p2, p7, p5)
             + spanned_volume(p5, p2, p7, p6);
    }
    

    //
    /** @brief Dispatched function for computing the volume of a domain or segment.*/
    template <typename ElementTypeOrTag, typename DomainType>
    typename viennagrid::result_of::coord_type< DomainType >::type
    volume_domain(DomainType const & domain)
    {
      typedef typename viennagrid::result_of::const_element_range<DomainType, ElementTypeOrTag>::type  CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
      
      typename viennagrid::result_of::coord_type< DomainType >::type new_volume = 0;
      CellContainer new_cells = viennagrid::elements<ElementTypeOrTag>(domain);
      for (CellIterator new_cit = new_cells.begin();
                        new_cit != new_cells.end();
                      ++new_cit)
      {
        new_volume += volume( default_point_accessor(domain), *new_cit);
      }
      return new_volume;
    }
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the n-dimensional volume of a n-cell */
  template <typename PointAccessorType, typename ElementType>
  typename viennagrid::result_of::coord_type<PointAccessorType>::type
  volume(PointAccessorType const accessor, ElementType const & cell)
  {
    return detail::volume_impl( accessor, cell, typename ElementType::tag() );
  }
  
  /** @brief Returns the n-dimensional volume of a n-cell */
  template <typename ElementTag, typename WrappedConfigType>
  typename viennagrid::result_of::coord_type< element_t<ElementTag, WrappedConfigType> >::type
  volume(element_t<ElementTag, WrappedConfigType> const & cell)
  {
    return volume( default_point_accessor(cell), cell );
  }
  
  
  template<typename ElementTypeOrTag, typename WrappedConfigType>
  typename viennagrid::result_of::coord_type< domain_t<WrappedConfigType> >::type
  volume(domain_t<WrappedConfigType> const & domain)
  {
      return detail::volume_domain<ElementTypeOrTag>(domain);
  }
  
  // default Element Tag = Cell Tag
  template<typename WrappedConfigType>
  typename viennagrid::result_of::coord_type< domain_t<WrappedConfigType> >::type
  volume(domain_t<WrappedConfigType> const & domain)
  {
      return detail::volume_domain< typename viennagrid::result_of::cell_tag< domain_t<WrappedConfigType> >::type >(domain);
  }
  
  template<typename SegmentationType>
  typename viennagrid::result_of::coord_type< typename SegmentationType::domain_type >::type
  volume(segment_t<SegmentationType> const & segment)
  {
      return detail::volume_domain< typename viennagrid::result_of::cell_tag< typename SegmentationType::domain_type >::type >(segment);
  }
  

} //namespace viennagrid
#endif
