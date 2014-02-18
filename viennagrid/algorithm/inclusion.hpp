#ifndef VIENNAGRID_ALGORITHM_INCLUSION_HPP
#define VIENNAGRID_ALGORITHM_INCLUSION_HPP

#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"
#include "viennagrid/algorithm/interval.hpp"

namespace viennagrid
{
  namespace detail
  {
    // http://www.blackpawn.com/texts/pointinpoly/
    template<typename PointAccessorT, typename ElementT, typename CoordType, typename CoordinateSystem, typename NumericConfigT>
    bool is_inside_impl( PointAccessorT const accessor,
                         ElementT const & element, viennagrid::triangle_tag,
                         spatial_point<CoordType, CoordinateSystem> const & p,
                         NumericConfigT numeric_config )
    {
      typedef spatial_point<CoordType, CoordinateSystem> PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & a = accessor( viennagrid::vertices(element)[0] );
      PointType const & b = accessor( viennagrid::vertices(element)[1] );
      PointType const & c = accessor( viennagrid::vertices(element)[2] );

      PointType v0 = c-a;
      PointType v1 = b-a;
      PointType v2 = p-a;

      NumericType dot00 = viennagrid::inner_prod(v0, v0);
      NumericType dot01 = viennagrid::inner_prod(v0, v1);
      NumericType dot02 = viennagrid::inner_prod(v0, v2);
      NumericType dot11 = viennagrid::inner_prod(v1, v1);
      NumericType dot12 = viennagrid::inner_prod(v1, v2);

      NumericType denom = static_cast<NumericType>(1) / (dot00 * dot11 - dot01 * dot01);
      NumericType u = (dot11*dot02 - dot01*dot12) * denom;
      NumericType v = (dot00*dot12 - dot01*dot02) * denom;

      NumericType abs_eps = absolute_tolerance<NumericType>(numeric_config);
      return (u >= -abs_eps) && (v >= -abs_eps) && (u+v <= static_cast<NumericType>(1) + 2*abs_eps );
    }


    template<typename PointAccessorT, typename ElementT, typename CoordType, typename CoordinateSystem, typename NumericConfigT>
    bool is_inside_impl( PointAccessorT const accessor,
                         ElementT const & element, viennagrid::tetrahedron_tag,
                         spatial_point<CoordType, CoordinateSystem> const & p,
                         NumericConfigT numeric_config )
    {
      typedef spatial_point<CoordType, CoordinateSystem> PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & a = accessor( viennagrid::vertices(element)[0] );
      PointType const & b = accessor( viennagrid::vertices(element)[1] );
      PointType const & c = accessor( viennagrid::vertices(element)[2] );
      PointType const & d = accessor( viennagrid::vertices(element)[3] );


      NumericType denom = static_cast<NumericType>(1) / spanned_volume(a,b,c,d);

      NumericType A = spanned_volume(p,b,c,d) * denom;
      NumericType B = spanned_volume(a,p,c,d) * denom;
      NumericType C = spanned_volume(a,b,p,d) * denom;
      NumericType D = spanned_volume(a,b,c,p) * denom;

      NumericType abs_eps = absolute_tolerance<NumericType>(numeric_config);
      return (A >= -abs_eps) && (B >= -abs_eps) && (C >= -abs_eps) && (D >= -abs_eps) && (A+B+C+D <= static_cast<NumericType>(1) + 2*abs_eps);
    }
  }



  template<typename PointAccessorT, typename ElementT, typename CoordType, typename CoordinateSystem, typename NumericConfigT>
  bool is_inside( PointAccessorT const accessor, ElementT const & element,
                  spatial_point<CoordType, CoordinateSystem> const & point,
                  NumericConfigT numeric_config )
  {
    return detail::is_inside_impl( accessor, element, typename ElementT::tag(), point, numeric_config );
  }


  template<typename ElementT, typename CoordType, typename CoordinateSystem, typename NumericConfigT>
  bool is_inside( ElementT const & element, spatial_point<CoordType, CoordinateSystem> const & point, NumericConfigT numeric_config )
  {
    return is_inside( viennagrid::default_point_accessor(element), element, point, numeric_config );
  }



  template<typename ElementT, typename CoordType, typename CoordinateSystem>
  bool is_inside( ElementT const & element, spatial_point<CoordType, CoordinateSystem> const & point )
  {
    return is_inside( element, point,  10*std::numeric_limits<CoordType>::epsilon() );
  }




}




#endif
