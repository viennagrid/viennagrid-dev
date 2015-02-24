#ifndef VIENNAGRID_POINT_ACCESSOR_HPP
#define VIENNAGRID_POINT_ACCESSOR_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/accessor.hpp"
#include "viennagrid/mesh/mesh.hpp"

namespace viennagrid
{
  /** @brief Accessor class for the data members appended (injected) to an element */
  template<typename MeshT>
  class point_accessor_t
  {
  public:
    typedef typename viennagrid::result_of::element<MeshT>::type element_type;

    typedef typename viennagrid::result_of::point<MeshT>::type point_type;
//     typedef typename viennagrid::result_of::const_element<MeshT>::type vertex_type;
    typedef typename viennagrid::result_of::const_element<MeshT>::type const_vertex_type;

    point_accessor_t(MeshT mesh_in) : mesh_(mesh_in) {}

    point_type get(const_vertex_type const & element) const { return viennagrid::get_point(mesh_, element); }
    void set(const_vertex_type const & element, point_type const & value) { viennagrid::set_point(mesh_, element, value); }

  private:
    MeshT mesh_;
  };

  class root_mesh_point_accessor_t
  {
  public:
    template<typename ElementT>
    typename viennagrid::result_of::point<ElementT>::type get( ElementT const & element ) const { return viennagrid::get_point(element); }

    template<typename ElementT, typename PointT>
    void set( ElementT element, PointT const & value ) { viennagrid::set_point(element, value); }
  };

  namespace result_of
  {
    template<typename ContainerT, typename AccessT, typename UnpackT, typename ElementT>
    struct point< dense_container_accessor<ContainerT, AccessT, UnpackT>, ElementT >
    {
      typedef typename dense_container_accessor<ContainerT, AccessT, UnpackT>::value_type type;
    };

    template<typename MeshT, typename ElementT>
    struct point<point_accessor_t<MeshT>, ElementT>
    {
      typedef typename point_accessor_t<MeshT>::point_type type;
    };

    template<typename MeshT>
    struct point<point_accessor_t<MeshT>, point_t>
    {
      typedef typename point_accessor_t<MeshT>::point_type type;
    };


    template<typename ElementT>
    struct point<root_mesh_point_accessor_t, ElementT>
    {
      typedef typename viennagrid::result_of::point<ElementT>::type type;
    };

    template<>
    struct point<root_mesh_point_accessor_t, point_t>
    {
      typedef point_t type;
    };



    /** @brief Metafunction returning the default point accessor type for a vertex. */
    template<typename SomethingT>
    struct point_accessor
    {
      typedef viennagrid::point_accessor_t<
        typename viennagrid::result_of::mesh<SomethingT>::type
      > type;
    };
  }

  /** @brief Convenience function for returning the default point accessor (i.e. accessing the point stored in the element's appendix) for a mesh or element. */
  template<bool mesh_is_const>
  typename result_of::point_accessor< base_mesh<mesh_is_const> >::type point_accessor(base_mesh<mesh_is_const> mesh)
  {
    return typename result_of::point_accessor< base_mesh<mesh_is_const> >::type(mesh);
  }

  template<bool mesh_is_const>
  typename result_of::point_accessor<
    typename viennagrid::result_of::mesh< base_mesh_region<mesh_is_const> >::type
  >::type point_accessor(base_mesh_region<mesh_is_const> region)
  {
    return point_accessor(region.mesh());
  }


  inline root_mesh_point_accessor_t root_mesh_point_accessor()
  {
    return root_mesh_point_accessor_t();
  }
}

#endif
