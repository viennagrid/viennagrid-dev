#ifndef VIENNAGRID_ALGORITHM_GEOMETRIC_TRANSFORM_HPP
#define VIENNAGRID_ALGORITHM_GEOMETRIC_TRANSFORM_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagridpp/forwards.hpp"
#include "viennagridpp/mesh/mesh.hpp"

/** @file viennagrid/algorithm/geometric_transform.hpp
    @brief Provides geometric transformation routines (e.g. scale()) for a mesh.
*/

namespace viennagrid
{
  /** @brief Transforms all points of a mesh based on a functor
   *
   * @param mesh                    The input mesh
   * @param func                    The functor object, has to be a function or provide an operator(). Interface: MeshPointType func(MeshPointType)
   * @param accessor                Point accessor for input points
   */
  template<typename PointAccessorT, typename FunctorT>
  void geometric_transform(PointAccessorT accessor, viennagrid::mesh const & mesh, FunctorT func)
  {
    typedef viennagrid::result_of::vertex_range<viennagrid::mesh>::type   VertexContainer;
    typedef viennagrid::result_of::iterator<VertexContainer>::type                      VertexIterator;

    VertexContainer vertices(mesh);
    for ( VertexIterator vit = vertices.begin();
          vit != vertices.end();
          ++vit )
      accessor.set( *vit, func(accessor.get(*vit)) );
  }

  /** @brief Transforms all points of a mesh based on a functor
   *
   * @param mesh                    The input mesh
   * @param func                    The functor object, has to be a function or provide an operator(). Interface: MeshPointType func(MeshPointType)
   */
  template<typename FunctorT>
  void geometric_transform(viennagrid::mesh const & mesh, FunctorT func)
  {
    geometric_transform(viennagrid::mesh_point_accessor(mesh), mesh, func);
  }


  /** @brief Scale functor with scaling factor and scale center
   *
   * @param MeshT                   The mesh type
   */
  template<typename MeshT>
  struct scale_functor
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type ScalarType;

    scale_functor() {}
    scale_functor(ScalarType factor_, PointType const & scale_center_) :
        factor(factor_), scale_center(scale_center_) {}

    PointType operator()(PointType p) const
    {
      p -= scale_center;
      p *= factor;
      p += scale_center;
      return p;
    }

    ScalarType factor;
    PointType scale_center;
  };

  /** @brief Function for scaling a mesh, uses scale_functor
   *
   * @param mesh                    The input mesh
   * @param factor                  The scaling factor
   * @param scaling_center          The center of the scaling operation
   */
  template<typename ScalarT, typename PointType>
  void scale(viennagrid::mesh const & mesh, ScalarT factor, PointType const & scaling_center)
  {
    scale_functor<viennagrid::mesh> func(factor, scaling_center);
    geometric_transform(mesh, func);
  }

  /** @brief Function for scaling a mesh, uses scale_functor. Scaling center is the origin.
   *
   * @param mesh                    The input mesh
   * @param factor                  The scaling factor
   */
  template<typename ScalarT>
  void scale(viennagrid::mesh const & mesh, ScalarT factor)
  {
    typedef typename viennagrid::result_of::point<viennagrid::mesh>::type PointType;
    scale_functor<viennagrid::mesh> func(factor, PointType(viennagrid::geometric_dimension(mesh)));
    geometric_transform(mesh, func);
  }




  /** @brief Affine transform functor using a ScalarType * as matrix (row major layout) and a translation vector
   *
   * @param MeshT                   The mesh type
   */
  template<typename MeshT>
  struct affine_transform_functor
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type ScalarType;
//     static const unsigned int point_dim = viennagrid::result_of::geometric_dimension<MeshT>::value;

    affine_transform_functor() {}
    affine_transform_functor(ScalarType const * matrix_) : matrix(matrix_), translation(0) {}
    affine_transform_functor(ScalarType const * matrix_, PointType const & translation_) :
        matrix(matrix_), translation(translation_) {}

    PointType operator()(PointType const & p) const
    {
      PointType tmp = translation;
      for (unsigned int row = 0; row != p.size(); ++row)
      {
        for (unsigned int column = 0; column != p.size(); ++column)
          tmp[row] += p[column] * matrix[ row*p.size() + column ];
      }

      return tmp;
    }

    ScalarType const * matrix;
    PointType translation;
  };

  /** @brief Function for an affine transformation of a mesh, uses affine_transform_functor
   *
   * @param mesh                    The input mesh
   * @param matrix                  The matrix representing the linear transformation part, row major layout. Attention! There are no out-of boundary checks, the user is responsible to provide a suitable matrix pointer.
   * @param translation             The translation vector
   */
  inline void affine_transform(viennagrid::mesh const & mesh,
                               viennagrid::result_of::coord<viennagrid::mesh>::type const * matrix,
                               viennagrid::result_of::point<viennagrid::mesh>::type const & translation )
  {
    affine_transform_functor<viennagrid::mesh> func(matrix, translation);
    geometric_transform(mesh, func);
  }

}

#endif
