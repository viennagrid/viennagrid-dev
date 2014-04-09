#ifndef VIENNAGRID_ALGORITHM_GEOMETRIC_TRANSFORM_HPP
#define VIENNAGRID_ALGORITHM_GEOMETRIC_TRANSFORM_HPP

namespace viennagrid
{
  template<typename MeshT, typename FunctorT, typename PointAccessorT>
  void geometric_transform(MeshT & mesh, FunctorT func, PointAccessorT accessor)
  {
    typedef typename viennagrid::result_of::element_range<MeshT, viennagrid::vertex_tag>::type   VertexContainer;
    typedef typename viennagrid::result_of::iterator<VertexContainer>::type                      VertexIterator;

    VertexContainer vertices(mesh);
    for ( VertexIterator vit = vertices.begin();
          vit != vertices.end();
          ++vit )
      accessor(*vit) = func( accessor(*vit) );
  }

  template<typename MeshT, typename FunctorT>
  void geometric_transform(MeshT & mesh, FunctorT func)
  {
    geometric_transform(mesh, func, viennagrid::default_point_accessor(mesh));
  }


  template<typename MeshT>
  struct scale_functor
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type ScalarType;

    scale_functor() {}
    scale_functor(ScalarType factor_) : factor(factor_), scale_center(0) {}
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


  template<typename MeshT, typename ScalarT, typename PointType>
  void scale(MeshT & mesh, ScalarT factor, PointType const & scaling_center)
  {
    scale_functor<MeshT> func(factor, scaling_center);
    geometric_transform(mesh, func);
  }

  template<typename MeshT, typename ScalarT, typename PointType>
  void scale(MeshT & mesh, ScalarT factor)
  {
    scale_functor<MeshT> func(factor);
    geometric_transform(mesh, func);
  }





  template<typename MeshT>
  struct affine_transform_functor
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type ScalarType;
    static const unsigned int point_dim = viennagrid::result_of::geometric_dimension<MeshT>::value;

    affine_transform_functor() {}
    affine_transform_functor(ScalarType const * matrix_) : matrix(matrix_), translation(0) {}
    affine_transform_functor(ScalarType const * matrix_, PointType const & translation_) :
        matrix(matrix_), translation(translation_) {}

    PointType operator()(PointType const & p) const
    {
      PointType tmp = translation;
      for (unsigned int row = 0; row != point_dim; ++row)
      {
        tmp[row] = 0;
        for (unsigned int column = 0; column != point_dim; ++column)
          tmp[row] += p[column] * matrix[ row*point_dim + column ];
      }

      return tmp;
    }

    ScalarType const * matrix;
    PointType translation;
  };

  template<typename MeshT>
  void affine_transform( MeshT & mesh,
                         typename viennagrid::result_of::coord<MeshT>::type const * matrix,
                         typename viennagrid::result_of::point<MeshT>::type const & translation )
  {
    affine_transform_functor<MeshT> func(matrix, translation);
    geometric_transform(mesh, func);
  }

}

#endif
