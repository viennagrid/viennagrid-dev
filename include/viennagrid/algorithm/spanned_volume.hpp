#ifndef VIENNAGRID_ALGORITHM_SPANNED_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_SPANNED_VOLUME_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */



//#include <math.h>
#include "viennagrid/core/forwards.hpp"


/** @file viennagrid/algorithm/spanned_volume.hpp
    @brief Computes the volume of n-simplices spanned by points
*/

namespace viennagrid
{
  template<typename PointT>
  viennagrid_numeric signed_spanned_volume(PointT const & p0, PointT const & p1)
  {
    assert( p0.size() == p1.size() );

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_signed_spanned_volume_2(p0.size(), &p0[0], &p1[0], &result) );
    return result;
  }

  template<typename PointT>
  viennagrid_numeric signed_spanned_volume(PointT const & p0, PointT const & p1, PointT const & p2)
  {
    assert( p0.size() == p1.size() );
    assert( p0.size() == p2.size() );

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_signed_spanned_volume_3(p0.size(), &p0[0], &p1[0], &p2[0], &result) );
    return result;
  }

  template<typename PointT>
  viennagrid_numeric signed_spanned_volume(PointT const & p0, PointT const & p1, PointT const & p2, PointT const & p3)
  {
    assert( p0.size() == p1.size() );
    assert( p0.size() == p2.size() );
    assert( p0.size() == p3.size() );

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_signed_spanned_volume_4(p0.size(), &p0[0], &p1[0], &p2[0], &p3[0], &result) );
    return result;
  }





  template<typename PointT>
  viennagrid_numeric spanned_volume(PointT const & p0, PointT const & p1)
  {
    assert( p0.size() == p1.size() );

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_spanned_volume_2(p0.size(), &p0[0], &p1[0], &result) );
    return result;
  }

  template<typename PointT>
  viennagrid_numeric spanned_volume(PointT const & p0, PointT const & p1, PointT const & p2)
  {
    assert( p0.size() == p1.size() );
    assert( p0.size() == p2.size() );

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_spanned_volume_3(p0.size(), &p0[0], &p1[0], &p2[0], &result) );
    return result;
  }

  template<typename PointT>
  viennagrid_numeric spanned_volume(PointT const & p0, PointT const & p1, PointT const & p2, PointT const & p3)
  {
    assert( p0.size() == p1.size() );
    assert( p0.size() == p2.size() );
    assert( p0.size() == p3.size() );

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_spanned_volume_4(p0.size(), &p0[0], &p1[0], &p2[0], &p3[0], &result) );
    return result;
  }


}
#endif
