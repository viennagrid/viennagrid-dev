#ifndef VIENNAGRID_ALGORITHM_QUANTITY_INTERPOLATE_HPP
#define VIENNAGRID_ALGORITHM_QUANTITY_INTERPOLATE_HPP


#include "viennagridpp/algorithm/spanned_volume.hpp"
#include "viennagridpp/algorithm/inclusion.hpp"


namespace viennagrid
{


  namespace detail
  {


    template<typename ElementT, typename PointT, typename AccessorT>
    typename AccessorT::value_type interpolate_triangle(ElementT const & element,
                                                        PointT const & point,
                                                        AccessorT const & accessor)
    {
//       typedef typename AccessorT::value_type ValueType;
//       typedef viennagrid_numeric NumericType;
      typedef typename viennagrid::result_of::point<ElementT>::type PointType;

      ElementT v0 = viennagrid::vertices(element)[0];
      ElementT v1 = viennagrid::vertices(element)[1];
      ElementT v2 = viennagrid::vertices(element)[2];

      PointType t0 = viennagrid::get_point(v0);
      PointType t1 = viennagrid::get_point(v1);
      PointType t2 = viennagrid::get_point(v2);

      return
        ( viennagrid::spanned_volume(point, t1, t2) * accessor.get(v0) +
          viennagrid::spanned_volume(t0, point, t2) * accessor.get(v1) +
          viennagrid::spanned_volume(t0, t1, point) * accessor.get(v2) ) *
          1.0 / viennagrid::spanned_volume(t0, t1, t2);
    }

    template<typename ElementT, typename PointT, typename AccessorT>
    typename AccessorT::value_type interpolate_tetrahedron(ElementT const & element,
                                                           PointT const & point,
                                                           AccessorT const & accessor)
    {
//       typedef typename AccessorT::value_type ValueType;
//       typedef viennagrid_numeric NumericType;
      typedef typename viennagrid::result_of::point<ElementT>::type PointType;

      ElementT v0 = viennagrid::vertices(element)[0];
      ElementT v1 = viennagrid::vertices(element)[1];
      ElementT v2 = viennagrid::vertices(element)[2];
      ElementT v3 = viennagrid::vertices(element)[2];

      PointType t0 = viennagrid::get_point(v0);
      PointType t1 = viennagrid::get_point(v1);
      PointType t2 = viennagrid::get_point(v2);
      PointType t3 = viennagrid::get_point(v3);


      return
        ( viennagrid::spanned_volume(point, t1, t2, t3) * accessor.get(v0) +
          viennagrid::spanned_volume(t0, point, t2, t3) * accessor.get(v1) +
          viennagrid::spanned_volume(t0, t1, point, t3) * accessor.get(v2) +
          viennagrid::spanned_volume(t0, t1, t2, point) * accessor.get(v3) ) *
          1.0 / viennagrid::spanned_volume(t0, t1, t2, t3);
    }

  }



  template<typename ElementT, typename PointT, typename AccessorT>
  typename AccessorT::value_type interpolate(ElementT const & element,
                                             PointT const & point,
                                             AccessorT const & accessor)
  {
    if (element.tag().is_triangle())
      return detail::interpolate_triangle(element, point, accessor);
    if (element.tag().is_tetrahedron())
      return detail::interpolate_tetrahedron(element, point, accessor);

    assert(false);
  }







  template<typename InputMeshT, typename InputAccessorT,
           typename OutputMeshT, typename OutputAccessorT,
           typename ValueT>
  void interpolate_vertex_quantity(InputMeshT const & input_mesh,
                                   InputAccessorT const & input_accessor,
                                   OutputMeshT const & output_mesh,
                                   OutputAccessorT output_accessor,
                                   ValueT default_value)
  {
    typedef typename viennagrid::result_of::const_cell_range<InputMeshT>::type ConstInputCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstInputCellRangeType>::type ConstInputCellRangeIterator;

    typedef typename viennagrid::result_of::vertex_range<OutputMeshT>::type OutputVertexRangeType;
    typedef typename viennagrid::result_of::iterator<OutputVertexRangeType>::type OutputVertexRangeIterator;


    ConstInputCellRangeType input_cells(input_mesh);
    OutputVertexRangeType output_vertices(output_mesh);

    for (OutputVertexRangeIterator vit = output_vertices.begin();
                                    vit != output_vertices.end();
                                  ++vit)
    {

      ConstInputCellRangeIterator cit = input_cells.begin();
      for (; cit != input_cells.end(); ++cit)
      {
        if (viennagrid::is_inside(*cit, viennagrid::get_point(*vit)))
          break;
      }

      if (cit == input_cells.end())
      {
        output_accessor.set( *vit, default_value );
      }
      else
      {
        output_accessor.set( *vit, interpolate(*cit, viennagrid::get_point(*vit), input_accessor) );
      }
    }

  }





}


#endif
