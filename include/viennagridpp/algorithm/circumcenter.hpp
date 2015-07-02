#ifndef VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP
#define VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP

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

#include "viennagridpp/core.hpp"
#include "viennagridpp/algorithm/spanned_volume.hpp"
#include "viennagridpp/algorithm/cross_prod.hpp"
// #include "viennagrid/topology/all.hpp"

/** @file viennagrid/algorithm/circumcenter.hpp
    @brief Computes the circumcenter of n-cells.
*/

namespace viennagrid
{
  namespace detail
  {
    /** @brief Throws a compile time error if the n-cell type is not supported. */
    template <typename ElementT, typename GeometricContainerT, typename ElementTag, typename DimensionTag>
    typename viennagrid::result_of::point<GeometricContainerT>::type
    circumcenter(ElementT const &, GeometricContainerT const &, ElementTag const &, DimensionTag const &)
    {
      typename ElementT::ERROR_COMPUTATION_OF_CIRCUMCENTER_NOT_IMPLEMENTED   error_object;
      (void)error_object;
      return typename viennagrid::result_of::point<GeometricContainerT>::type();
    }


    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a circumcenter for a point (degenerate case) */
    template<typename DimensionTagT, typename PointT>
    PointT circumcenter_points(viennagrid::vertex_tag, DimensionTagT, PointT const & p)
    {
      return p;
    }

    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a circumcenter for a point (degenerate case) */
    template <typename PointAccessorT, typename ElementT, typename DimensionTagT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::vertex_tag, DimensionTagT)
    {
      return circumcenter_points(viennagrid::vertex_tag(), DimensionTagT(), accessor.get(cell));
    }


    //
    // Calculation of circumcenter for a line
    //
    /** @brief Implementation of the calculation of a circumcenter for a line */
    template<typename DimensionTagT, typename PointT>
    PointT circumcenter_points(viennagrid::line_tag, DimensionTagT, PointT const & A, PointT const & B)
    {
      PointT ret = A + B;
      ret /= 2.0;

      return ret;
    }

//     /** @brief Implementation of the calculation of a circumcenter for a line (1-hypercube) */
//     template<typename DimensionTagT, typename PointT>
//     PointT circumcenter_points(viennagrid::hypercube_tag<1>, DimensionTagT, PointT const & A, PointT const & B)
//     {
//       return circumcenter_points(viennagrid::simplex_tag<1>(), DimensionTagT(), A, B);
//     }

    /** @brief Implementation of the calculation of a circumcenter for a line (1-simplex) */
    template <typename PointAccessorT, typename ElementT, typename DimensionTag>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::line_tag, DimensionTag)
    {
      return circumcenter_points(viennagrid::line_tag(),
                                 DimensionTag(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ));
    }


//     /** @brief Implementation of the calculation of a circumcenter for a line (1-hypercube) */
//     template <typename PointAccessorT, typename ElementT, typename DimensionTag>
//     typename PointAccessorT::value_type
//     circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::hypercube_tag<1>, DimensionTag)
//     {
//       return circumcenter(accessor, cell, viennagrid::simplex_tag<1>(), DimensionTag());
//     }

    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    /** @brief Implementation of the calculation of a circumcenter for a triangle in two dimensions */
    template<typename PointT>
    PointT circumcenter_points(viennagrid::triangle_tag, viennagrid::dimension_tag<2>, PointT const & A, PointT const & B, PointT const & C)
    {
      PointT circ_cent(2);

      double D = 2.0 * (   A[0] * (B[1] - C[1])
                         + B[0] * (C[1] - A[1])
                         + C[0] * (A[1] - B[1]) );

      double x =  (   (A[1] * A[1] + A[0] * A[0]) * (B[1] - C[1])
                    + (B[1] * B[1] + B[0] * B[0]) * (C[1] - A[1])
                    + (C[1] * C[1] + C[0] * C[0]) * (A[1] - B[1])  ) / D;

      double y =  (   (A[1] * A[1] + A[0] * A[0]) * (C[0] - B[0])
                    + (B[1] * B[1] + B[0] * B[0]) * (A[0] - C[0])
                    + (C[1] * C[1] + C[0] * C[0]) * (B[0] - A[0])  ) / D;

      circ_cent[0] = x;
      circ_cent[1] = y;

      return circ_cent;
    }


    /** @brief Implementation of the calculation of a circumcenter for a triangle in two dimensions */
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::triangle_tag, viennagrid::dimension_tag<2>)
    {
      return circumcenter_points(viennagrid::triangle_tag(),
                                 viennagrid::dimension_tag<2>(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ),
                                 accessor.get( vertices(cell)[2] ));
    }


    //
    // TODO: This works for rectangles only, but not for general quadrilaterals
    //
    /** @brief Implementation of the calculation of a circumcenter for a quadrilateral in two dimensions. Mind that the user has to ensure that the quadrilateral actually has a circumcenter! */
    template<typename DimensionTagT, typename PointT>
    PointT circumcenter_points(viennagrid::quadrilateral_tag, DimensionTagT, PointT const & A, PointT const & B, PointT const & C, PointT const & D)
    {
      PointT p = A;
      p += B;
      p += C;
      p += D;

      return p / 4;
    }

    /** @brief Implementation of the calculation of a circumcenter for a quadrilateral in two dimensions. Mind that the user has to ensure that the quadrilateral actually has a circumcenter! */
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::quadrilateral_tag, viennagrid::dimension_tag<2>)
    {
      return circumcenter_points(viennagrid::quadrilateral_tag(),
                                 viennagrid::dimension_tag<2>(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ),
                                 accessor.get( vertices(cell)[2] ),
                                 accessor.get( vertices(cell)[3] ));
    }



    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    /** @brief Implementation of the calculation of a circumcenter for a triangle in three dimensions */
    template<typename PointT>
    PointT circumcenter_points(viennagrid::triangle_tag, viennagrid::dimension_tag<3>, PointT const & A, PointT const & B, PointT const & C)
    {
      double denominator = 2.0 * viennagrid::inner_prod(viennagrid::cross_prod(A-B, B-C), viennagrid::cross_prod(A-B, B-C));

      double alpha = viennagrid::inner_prod(B - C, B - C) * viennagrid::inner_prod(A - B, A - C);
      double beta  = viennagrid::inner_prod(A - C, A - C) * viennagrid::inner_prod(B - A, B - C);
      double gamma = viennagrid::inner_prod(A - B, A - B) * viennagrid::inner_prod(C - A, C - B);

      PointT circ_cent = A * (alpha/denominator) + B * (beta/denominator) + C * (gamma/denominator);
      return circ_cent;
    }

    /** @brief Implementation of the calculation of a circumcenter for a triangle in three dimensions */
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::triangle_tag, viennagrid::dimension_tag<3>)
    {
      return circumcenter_points(viennagrid::triangle_tag(),
                                 viennagrid::dimension_tag<3>(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ),
                                 accessor.get( vertices(cell)[2] ));
    }


    //
    // Note: This works for rectangles only, but not for general quadrilaterals
    //
    /** @brief Implementation of the calculation of a circumcenter for a quadrilateral in three dimensions. Mind that the user has to ensure that the quadrilateral actually has a circumcenter! */
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::quadrilateral_tag, viennagrid::dimension_tag<3>)
    {
      return circumcenter_points(viennagrid::quadrilateral_tag(),
                                 viennagrid::dimension_tag<2>(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ),
                                 accessor.get( vertices(cell)[2] ),
                                 accessor.get( vertices(cell)[3] ));
    }


    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    /** @brief Implementation of the calculation of a circumcenter for a tetrahedron in three dimensions */
    template<typename PointT>
    PointT circumcenter_points(viennagrid::tetrahedron_tag, viennagrid::dimension_tag<3>, PointT const & p0, PointT const & p1, PointT const & p2, PointT const & p3)
    {
      PointT const & O = p0;
      PointT const & A = p1 - O;
      PointT const & B = p2 - O;
      PointT const & C = p3 - O;

      PointT circ_cent = (cross_prod(A, B) * viennagrid::inner_prod(C, C)
                             + cross_prod(C, A) * viennagrid::inner_prod(B, B)
                             + cross_prod(B, C) * viennagrid::inner_prod(A, A)
                             ) / (viennagrid::inner_prod(A, viennagrid::cross_prod(B, C)) * 2.0);
      PointT cprodBC = viennagrid::cross_prod(B, C);
      if (std::fabs(viennagrid::inner_prod(A, cprodBC)) < 1e-10 * viennagrid::norm(cprodBC) )
      {
        std::cerr << "Near singularity in circum center calculation!" << std::endl;
        std::cerr << "A: " << A << std::endl;
        std::cerr << "B: " << B << std::endl;
        std::cerr << "C: " << C << std::endl;
        std::cerr << "B x C: " << viennagrid::cross_prod(B, C) << std::endl;
        throw std::runtime_error("Near singularity in circum center calculation!");
      }
      return circ_cent + O;
    }




    /** @brief Implementation of the calculation of a circumcenter for a tetrahedron in three dimensions */
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::tetrahedron_tag, viennagrid::dimension_tag<3>)
    {
      return circumcenter_points(viennagrid::tetrahedron_tag(),
                                 viennagrid::dimension_tag<3>(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ),
                                 accessor.get( vertices(cell)[2] ),
                                 accessor.get( vertices(cell)[3] ));
    }

    //
    // Note: This works for rectangles only, but not for general quadrilaterals
    //
    /** @brief Implementation of the calculation of a circumcenter for a hexahedron in three dimensions.  Mind that the user has to ensure that the quadrilateral actually has a circumcenter! */
    template<typename PointT>
    PointT circumcenter_points(viennagrid::hexahedron_tag, viennagrid::dimension_tag<3>, PointT const & p0, PointT const & p1, PointT const & p2, PointT const & p3, PointT const & p4, PointT const & p5, PointT const & p6, PointT const & p7)
    {
      PointT p = p0;
      p += p1;
      p += p2;
      p += p3;
      p += p4;
      p += p5;
      p += p6;
      p += p7;

      return p / 8;
    }


    /** @brief Implementation of the calculation of a circumcenter for a hexahedron in three dimensions.  Mind that the user has to ensure that the quadrilateral actually has a circumcenter! */
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & cell, viennagrid::hexahedron_tag, viennagrid::dimension_tag<3>)
    {
      return circumcenter_points(viennagrid::hexahedron_tag(),
                                 viennagrid::dimension_tag<3>(),
                                 accessor.get( vertices(cell)[0] ),
                                 accessor.get( vertices(cell)[1] ),
                                 accessor.get( vertices(cell)[2] ),
                                 accessor.get( vertices(cell)[3] ),
                                 accessor.get( vertices(cell)[4] ),
                                 accessor.get( vertices(cell)[5] ),
                                 accessor.get( vertices(cell)[6] ),
                                 accessor.get( vertices(cell)[7] ) );
    }






    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & element, viennagrid::dimension_tag<1>)
    {
      if (element.is_vertex())
        return circumcenter(accessor, element, viennagrid::vertex_tag(), viennagrid::dimension_tag<1>());
      if (element.is_line())
        return circumcenter(accessor, element, viennagrid::line_tag(), viennagrid::dimension_tag<1>());

      return typename viennagrid::result_of::point< PointAccessorT, ElementT >::type( 1 );
    }

    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & element, viennagrid::dimension_tag<2>)
    {
      if (element.is_vertex())
        return circumcenter(accessor, element, viennagrid::vertex_tag(), viennagrid::dimension_tag<2>());
      if (element.is_line())
        return circumcenter(accessor, element, viennagrid::line_tag(), viennagrid::dimension_tag<2>());
      if (element.is_triangle())
        return circumcenter(accessor, element, viennagrid::triangle_tag(), viennagrid::dimension_tag<2>());
      if (element.is_quadrilateral())
        return circumcenter(accessor, element, viennagrid::quadrilateral_tag(), viennagrid::dimension_tag<2>());

      return typename viennagrid::result_of::point< PointAccessorT, ElementT >::type( 2 );
    }

    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    circumcenter(PointAccessorT const accessor, ElementT const & element, viennagrid::dimension_tag<3>)
    {
      if (element.is_vertex())
        return circumcenter(accessor, element, viennagrid::vertex_tag(), viennagrid::dimension_tag<3>());
      if (element.is_line())
        return circumcenter(accessor, element, viennagrid::line_tag(), viennagrid::dimension_tag<3>());
      if (element.is_triangle())
        return circumcenter(accessor, element, viennagrid::triangle_tag(), viennagrid::dimension_tag<3>());
      if (element.is_quadrilateral())
        return circumcenter(accessor, element, viennagrid::quadrilateral_tag(), viennagrid::dimension_tag<3>());
      if (element.is_tetrahedron())
        return circumcenter(accessor, element, viennagrid::tetrahedron_tag(), viennagrid::dimension_tag<3>());
      if (element.is_hexahedron())
        return circumcenter(accessor, element, viennagrid::hexahedron_tag(), viennagrid::dimension_tag<3>());

      return typename viennagrid::result_of::point< PointAccessorT, ElementT >::type( 3 );
    }


  } //namespace detail



  template<typename ElementTagT, typename DimensionTagT, typename PointT>
  PointT circumcenter_points(ElementTagT, DimensionTagT, PointT const & p0)
  {
    return detail::circumcenter_points(ElementTagT(),
                                       DimensionTagT(),
                                       p0);
  }

  template<typename ElementTagT, typename DimensionTagT, typename PointT>
  PointT circumcenter_points(ElementTagT, DimensionTagT, PointT const & p0, PointT const & p1)
  {
    return detail::circumcenter_points(ElementTagT(),
                                       DimensionTagT(),
                                       p0, p1);
  }

  template<typename ElementTagT, typename DimensionTagT, typename PointT>
  PointT circumcenter_points(ElementTagT, DimensionTagT, PointT const & p0, PointT const & p1, PointT const & p2)
  {
    return detail::circumcenter_points(ElementTagT(),
                                       DimensionTagT(),
                                       p0, p1, p2);
  }

  template<typename ElementTagT, typename DimensionTagT, typename PointT>
  PointT circumcenter_points(ElementTagT, DimensionTagT, PointT const & p0, PointT const & p1, PointT const & p2, PointT const & p3)
  {
    return detail::circumcenter_points(ElementTagT(),
                                       DimensionTagT(),
                                       p0, p1, p2, p3);
  }

  template<typename ElementTagT, typename DimensionTagT, typename PointT>
  PointT circumcenter_points(ElementTagT, DimensionTagT, PointT const & p0, PointT const & p1, PointT const & p2, PointT const & p3, PointT const & p4, PointT const & p5, PointT const & p6, PointT const & p7)
  {
    return detail::circumcenter_points(ElementTagT(),
                                       DimensionTagT(),
                                       p0, p1, p2, p3, p4, p5, p6, p7);
  }


  /** @brief The public interface function for the calculation of the circumcenter with explicit point accessor.
   *
   * @param element       The element for which the circumcenter should be computed
   * @param accessor      The point accessor providing point information for geometric calculation
   */
  template <typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
  circumcenter(PointAccessorT const accessor, ElementT const & element)
  {
    typedef typename viennagrid::result_of::point< PointAccessorT, ElementT >::type PointType;

    switch ( viennagrid::geometric_dimension(element) )
    {
      case 1:
        return detail::circumcenter(accessor,
                        element,
                        viennagrid::dimension_tag<1>());
      case 2:
        return detail::circumcenter(accessor,
                        element,
                        viennagrid::dimension_tag<2>());
      case 3:
        return detail::circumcenter(accessor,
                        element,
                        viennagrid::dimension_tag<3>());

    }

    assert(false);
    return PointType( viennagrid::geometric_dimension(element) );
  }


  /** @brief The public interface function for the calculation of the circumcenter.
   *
   * @param element       The element for which the circumcenter should be computed
   */
  template<typename ElementT>
  typename viennagrid::result_of::point< ElementT >::type
  circumcenter(ElementT const & element)
  {
   return circumcenter( root_mesh_point_accessor(), element );
  }


} //namespace viennagrid
#endif
