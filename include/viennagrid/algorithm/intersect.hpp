#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_INTERSECT_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_INTERSECT_HPP

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

namespace viennagrid
{
  namespace detail
  {


    template<typename PointAccessorT, typename ElementT, typename ElementTagT, typename DimensionTagT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const /*point_accessor*/,
                            ElementT const & /*element*/,
                            ElementTagT,
                            DimensionTagT,
                            PointT const & /*line_start*/,
                            PointT const & /*line_end*/,
                            NumericConfigT /*nc*/)
    {
      assert(false);
      // TODO throw
      return false;
    }




    template<typename PointAccessorT, typename ElementT, typename DimensionTagT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            viennagrid::vertex_tag,
                            DimensionTagT,
                            PointT const & line_start,
                            PointT const & line_end,
                            NumericConfigT nc)
    {

      PointT point = point_accessor.get(element);

      viennagrid_dimension geo_dim = point.size();
      PointT dir = line_end - line_start;


      viennagrid_dimension dim = 0;
      for (; dim != geo_dim; ++dim)
      {
        if (std::abs(dir[dim]) > detail::absolute_tolerance<viennagrid_numeric>(nc))
          break;
      }

      assert(dim != geo_dim);

      viennagrid_numeric alpha = (point[dim]-line_start[dim])/dir[dim];

      return detail::is_equal(nc, line_start + alpha*dir, point);
    }





    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            viennagrid::line_tag,
                            viennagrid::dimension_tag<2>,
                            PointT const & line_start,
                            PointT const & line_end,
                            NumericConfigT nc)
    {
      typedef typename viennagrid::result_of::coord< PointT >::type CoordType;
      typedef typename detail::result_of::numeric_type<NumericConfigT, CoordType>::type inner_numeric_type;

      PointT v0 = point_accessor.get(viennagrid::vertices(element)[0]);
      PointT v1 = point_accessor.get(viennagrid::vertices(element)[1]);

      PointT const & w0 = line_start;
      PointT const & w1 = line_end;

      // write V(s) = v0 + s * (v1 - v0), s \in [0,1]
      //       W(t) = w0 + t * (w1 - w0), t \in [0,1]

      // compute s and t assuming V(s) and W(t) to be infinite lines:
      // cf. http://www.softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
      PointT dir_v = v1 - v0;  //direction vector for line V(s)
      PointT dir_w = w1 - w0;  //direction vector for line W(t)



      CoordType v_in_v = viennagrid::inner_prod(dir_v, dir_v);
      CoordType v_in_w = viennagrid::inner_prod(dir_v, dir_w);
      CoordType w_in_w = viennagrid::inner_prod(dir_w, dir_w);

      CoordType denominator = v_in_v * w_in_w - v_in_w * v_in_w;

      if ( std::abs(denominator) < detail::relative_tolerance(nc, v_in_v * w_in_w) ) //lines parallel (up to round-off)
      {
        PointT lhs_v = w0 - v0;
        inner_numeric_type relative_eps = detail::relative_tolerance(nc, viennagrid::norm_1(dir_v) );

        if ( std::abs(dir_v[0] * lhs_v[1] - lhs_v[0] * dir_v[1]) < relative_eps ) // lines share a common ray
        {
          std::size_t index = (std::abs(dir_v[0]) < relative_eps) ? 1 : 0;

          CoordType w_first = lhs_v[index];
          CoordType w_second = w1[index] - v0[index];
          if (w_first > w_second)
            std::swap(w_first, w_second);

          CoordType v_first = 0;
          CoordType v_second = dir_v[index];
          if (v_first > v_second)
            std::swap(v_first, v_second);

          if (w_second < v_first || v_second < w_first)
            return false;
          else
            return true;

//           if (interval_intersect( coord_type(0), dir_v[index], tag1, w_first, w_second, tag2, nc ))
//           {
//             overlapping_lines_functor(v0, v1, tag1, w0, w1, tag2, coord_type(0), dir_v[index], w_first, w_second);
//             return intersection_result::intersection_dimension(1);
//           }
//           else
//             return intersection_result::no_intersection();
        }
        else
          return false;
//           return intersection_result::no_intersection();
      }

      //Lines are not parallel: Compute minimizers s, t:
      PointT dir_distance = v0 - w0;  //any vector connecting two points on V and W

      CoordType v_in_dir_distance = viennagrid::inner_prod(dir_v, dir_distance);
      CoordType w_in_dir_distance = viennagrid::inner_prod(dir_w, dir_distance);

      CoordType s = (v_in_w * w_in_dir_distance - w_in_w * v_in_dir_distance);
      CoordType t = (v_in_v * w_in_dir_distance - v_in_w * v_in_dir_distance);

      if (denominator < 0)
      {
        denominator = -denominator;
        s = -s;
        t = -t;
      }

//       if (point_in_interval(coord_type(0), denominator, tag1, s, nc) && point_in_interval(coord_type(0), denominator, tag2, t, nc))
      if (0 < s && s < denominator && 0 < t && t < denominator)
        return true;
      else
        return false;
//       {
//         point_intersection_functor(v0, v1, tag1, w0, w1, tag2, s, t, denominator);
//         return intersection_result::intersection_dimension(0);
//       }
//       else
//         return intersection_result::no_intersection();
    }



    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            viennagrid::triangle_tag,
                            viennagrid::dimension_tag<3>,
                            PointT const & line_start,
                            PointT const & line_end,
                            NumericConfigT nc)
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;

      PointT A = point_accessor.get(viennagrid::vertices(element)[0]);
      PointT B = point_accessor.get(viennagrid::vertices(element)[1]);
      PointT C = point_accessor.get(viennagrid::vertices(element)[2]);

      PointT r = line_start;
      PointT d = line_end - line_start;

      PointT b = B-A; // vector from A to B
      PointT c = C-A; // vector from A to C

      PointT rhs = r-A;

      // solve the equation:
      // r + lambda*d = A + beta*b + gamma*c
      // beta*b + gamma*c - lambda*d = r-A
      // (b c -d) (beta gamma lambda)^T = r-A
      // (beta gamma lambda)^T = (b c -d)^-1 (r-A)

      NumericType det = viennagrid::determinant( b, c, -d );

      if ( std::abs(det) < detail::absolute_tolerance<NumericType>(nc) )
      {
        PointT n = viennagrid::cross_prod( B-A, C-A );

        PointT center = (A+B+C)/3.0;
        if( std::abs( viennagrid::inner_prod( n, r-center ) ) < detail::absolute_tolerance<NumericType>(nc) )
          return true; // r lies on triangle plane, TODO behandeln: kreuzt strahl dreieck?
        else
          return false;
      }
      else
      {
        PointT im[3]; // inverse matrix (b c -d)^-1

        for (int i = 0; i < 3; ++i)
          im[i].resize(3);

        d = -d; // invert for simplicity

//         im[0] = viennagrid::make_point(  c[1]*d[2] - c[2]*d[1],
//                                        -(c[0]*d[2] - c[2]*d[0]),
//                                          c[0]*d[1] - c[1]*d[0] );
//
//         im[1] = viennagrid::make_point(-(b[1]*d[2] - b[2]*d[1]),
//                                          b[0]*d[2] - b[2]*d[0],
//                                        -(b[0]*d[1] - b[1]*d[0]) );
//
//         im[2] = viennagrid::make_point(  b[1]*c[2] - b[2]*c[1],
//                                        -(b[0]*c[2] - b[2]*c[0]),
//                                          b[0]*c[1] - b[1]*c[0] );

        im[0][0] = c[1]*d[2] - c[2]*d[1];
        im[1][0] = -(b[1]*d[2] - b[2]*d[1]);
        im[2][0] = b[1]*c[2] - b[2]*c[1];

        im[0][1] = -(c[0]*d[2] - c[2]*d[0]);
        im[1][1] = b[0]*d[2] - b[2]*d[0];
        im[2][1] = -(b[0]*c[2] - b[2]*c[0]);

        im[0][2] = c[0]*d[1] - c[1]*d[0];
        im[1][2] = -(b[0]*d[1] - b[1]*d[0]);
        im[2][2] = b[0]*c[1] - b[1]*c[0];

        NumericType beta   = viennagrid::inner_prod( rhs, im[0] );
        NumericType gamma  = viennagrid::inner_prod( rhs, im[1] );
        NumericType lambda = viennagrid::inner_prod( rhs, im[2] );

        if (det < 0)
        {
          det    = -det;
          beta   = -beta;
          gamma  = -gamma;
          lambda = -lambda;
        }

        NumericType alpha  = det - beta - gamma;
        NumericType offset = detail::relative_tolerance(nc, det);
        NumericType lower  = 0 - offset;
        NumericType upper  = det + offset;

        if ( (alpha >= lower ) && (beta >= lower) && (gamma >= lower) && (alpha <= upper) && (beta <= upper) && (gamma <= upper) && (lambda >= lower) && (lambda <= upper))
        {
          return true;
        }
        else
          return false;
      }
    }




    template<typename PointAccessorT, typename ElementT, typename DimensionTagT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            DimensionTagT,
                            PointT const & line_start,
                            PointT const & line_end,
                            NumericConfigT nc)
    {
      switch (element.tag().internal())
      {
        case VIENNAGRID_ELEMENT_TYPE_VERTEX:
          return element_line_intersect_impl(point_accessor, element, vertex_tag(), DimensionTagT(), line_start, line_end, nc);

        case VIENNAGRID_ELEMENT_TYPE_LINE:
          return element_line_intersect_impl(point_accessor, element, line_tag(), DimensionTagT(), line_start, line_end, nc);

        case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
          return element_line_intersect_impl(point_accessor, element, triangle_tag(), DimensionTagT(), line_start, line_end, nc);

        default:
          // TODO throw
          return false;
      }
    }


    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            PointT const & line_start,
                            PointT const & line_end,
                            NumericConfigT nc)
    {
      switch (line_start.size())
      {
        case 0:
          // TODO throw
          return false;

        case 1:
          return element_line_intersect_impl(point_accessor, element, viennagrid::dimension_tag<1>(), line_start, line_end, nc);

        case 2:
          return element_line_intersect_impl(point_accessor, element, viennagrid::dimension_tag<2>(), line_start, line_end, nc);

        case 3:
          return element_line_intersect_impl(point_accessor, element, viennagrid::dimension_tag<3>(), line_start, line_end, nc);

        default:
          // TODO throw
          return false;
      }
    }
  }


  template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
  bool element_line_intersect(PointAccessorT const point_accessor,
                      ElementT const & element,
                      PointT const & line_start,
                      PointT const & line_end,
                      NumericConfigT nc)
  {
    return detail::element_line_intersect_impl(point_accessor,
                                              element,
                                              line_start, line_end,
                                              nc);
  }


  template<typename ElementT, typename PointT, typename NumericConfigT>
  bool element_line_intersect( ElementT const & element,
                      PointT const & line_start,
                      PointT const & line_end,
                      NumericConfigT nc)
  {
    return element_line_intersect( root_mesh_point_accessor(), element, line_start, line_end, nc );
  }
}

#endif
