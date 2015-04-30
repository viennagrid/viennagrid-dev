#ifndef VIENNAGRID_ALGORITHM_SIMPLEXIFY_HPP
#define VIENNAGRID_ALGORITHM_SIMPLEXIFY_HPP


#include "viennagridpp/element/element.hpp"
#include "viennagridpp/mesh/element_creation.hpp"
#include "viennagridpp/mesh/mesh_operations.hpp"
#include "viennagridpp/algorithm/volume.hpp"


namespace viennagrid
{

  namespace detail
  {
    template<bool element_is_const, typename CopyMapT>
    void simplexify_impl(viennagrid::base_element<element_is_const> const & element,
                    viennagrid::quadrilateral_tag,
                    CopyMapT & copy_map)
    {
      typedef viennagrid::base_element<element_is_const> ElementType;

      ElementType vertices[4];
      for (int i = 0; i != 4; ++i)
        vertices[i] = copy_map( viennagrid::vertices(element)[i] );

      ElementType t0 = viennagrid::make_triangle( copy_map.dst_mesh(), vertices[0], vertices[1], vertices[2] );
      ElementType t1 = viennagrid::make_triangle( copy_map.dst_mesh(), vertices[1], vertices[2], vertices[3] );

      copy_region_information(element, t0);
      copy_region_information(element, t1);

      assert( viennagrid::detail::is_equal(1e-6,
                                          viennagrid::volume(element),
                                          viennagrid::volume(t0)+viennagrid::volume(t1)) );
    }


    template<bool element_is_const, typename CopyMapT>
    void simplexify_impl(viennagrid::base_element<element_is_const> const & element,
                    CopyMapT & copy_map)
    {
      if (element.tag().is_quadrilateral())
        simplexify_impl(element, viennagrid::quadrilateral_tag(), copy_map);
      else
      {
        assert(false);
      }
    }
  }



  void simplexify(viennagrid::mesh_t const & src_mesh, viennagrid::mesh_t const & dst_mesh)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIterator;

    typedef viennagrid::result_of::element_copy_map<>::type CopyMapType;

    CopyMapType copy_map(dst_mesh);
    ConstCellRangeType cells(src_mesh);
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( (*cit).tag().is_simplex() )
        copy_map( *cit );
      else
        detail::simplexify_impl( *cit, copy_map );
    }
  }



}

#endif
