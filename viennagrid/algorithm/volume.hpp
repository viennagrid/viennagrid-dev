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

#ifndef VIENNAGRID_ALGORITHM_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_VOLUME_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

/** @file volume.hpp
    @brief Computes the volume of different cell types as well as domains and segments
*/


namespace viennagrid
{
    

    //topologically one-dimensional elements
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::line_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].getPoint();
      PointType const & p1 = ncells<0>(cell)[1].getPoint();
      
      return norm(p0 - p1);
    }

    //topologically two-dimensional elements
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].getPoint();
      PointType const & p1 = ncells<0>(cell)[1].getPoint();
      PointType const & p2 = ncells<0>(cell)[2].getPoint();
      
      return spanned_volume(p0, p1, p2);
    }

    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].getPoint();
      PointType const & p1 = ncells<0>(cell)[1].getPoint();
      PointType const & p2 = ncells<0>(cell)[2].getPoint();
      PointType const & p3 = ncells<0>(cell)[3].getPoint();
      
      return spanned_volume(p0, p1, p3) + spanned_volume(p1, p2, p3); //sum up the two triangular parts
    }

    //topologically three-dimensional elements
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].getPoint();
      PointType const & p1 = ncells<0>(cell)[1].getPoint();
      PointType const & p2 = ncells<0>(cell)[2].getPoint();
      PointType const & p3 = ncells<0>(cell)[3].getPoint();
      
      return spanned_volume(p0, p1, p2, p3);
    }


    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume_impl(ElementType const & cell, viennagrid::hexahedron_tag)
    {
      typedef typename ElementType::config_type      ConfigType;
      typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type       VertexOnCellContainer;
      
      PointType const & p0 = ncells<0>(cell)[0].getPoint();
      PointType const & p1 = ncells<0>(cell)[1].getPoint();
      PointType const & p2 = ncells<0>(cell)[2].getPoint();
      PointType const & p3 = ncells<0>(cell)[3].getPoint();
      PointType const & p4 = ncells<0>(cell)[4].getPoint();
      PointType const & p5 = ncells<0>(cell)[5].getPoint();
      PointType const & p6 = ncells<0>(cell)[6].getPoint();
      PointType const & p7 = ncells<0>(cell)[7].getPoint();
      
      //decompose hexahedron into six tetrahedra
      return spanned_volume(p0, p1, p3, p4)
             + spanned_volume(p4, p1, p3, p7)
             + spanned_volume(p4, p1, p7, p5)
             + spanned_volume(p1, p2, p3, p7)
             + spanned_volume(p1, p2, p7, p5)
             + spanned_volume(p5, p2, p7, p6);
    }
    

    //
    template <typename ContainerType>
    typename ContainerType::config_type::numeric_type
    volume_domainsegment(ContainerType const & d)
    {
      typedef ContainerType                                      DomainType;
      typedef typename ContainerType::config_type::cell_tag      CellTag;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
      
      typename ContainerType::config_type::numeric_type new_volume = 0;
      CellContainer new_cells = viennagrid::ncells<CellTag::topology_level>(d);
      for (CellIterator new_cit = new_cells.begin();
                        new_cit != new_cells.end();
                      ++new_cit)
      {
        new_volume += volume(*new_cit);
      }
      return new_volume;
    }

    //
    // The public interface functions
    //
    template <typename ElementType>
    typename ElementType::config_type::numeric_type
    volume(ElementType const & cell)
    {
      return volume_impl(cell, typename ElementType::element_tag());
    }
    
    //special case: domain
    template <typename ConfigType>
    typename ConfigType::numeric_type
    volume(domain<ConfigType> const & d)
    {
      return volume_domainsegment(d);
    }    
    //special case: segment
    template <typename ConfigType>
    typename ConfigType::numeric_type
    volume(segment_t<ConfigType> const & d)
    {
      return volume_domainsegment(d);
    }
    
} //namespace viennashe
#endif
