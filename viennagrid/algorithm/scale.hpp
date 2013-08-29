#ifndef VIENNAGRID_ALGORITHM_SCALE_HPP
#define VIENNAGRID_ALGORITHM_SCALE_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

namespace viennagrid
{
  namespace detail
  {
    template<typename DomainType, typename ScalarType, typename PointAccessorType>
    void scale_impl(DomainType& domain, ScalarType factor, PointAccessorType accessor)
    {
      typedef typename viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type         VertexType;
      typedef typename viennagrid::result_of::element_range<DomainType, viennagrid::vertex_tag>::type   VertexContainer;
      typedef typename viennagrid::result_of::iterator<VertexContainer>::type                           VertexIterator;

      VertexContainer vertices = viennagrid::elements<VertexType>(domain);
      for ( VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit )
      {
          accessor(*vit) *= factor;
      }
    }
  } // detail

  template<typename DomainType, typename ScalarType>
  void scale(DomainType& domain, ScalarType factor)
  {
    viennagrid::detail::scale_impl(domain, factor, viennagrid::default_point_accessor(domain));
  }

} // viennagrid

#endif
