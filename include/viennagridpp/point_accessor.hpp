#ifndef VIENNAGRID_POINT_ACCESSOR_HPP
#define VIENNAGRID_POINT_ACCESSOR_HPP

#include "viennagridpp/forwards.hpp"
#include "viennagridpp/accessor.hpp"
#include "viennagridpp/mesh/mesh.hpp"

namespace viennagrid
{
//   /** @brief Accessor class for the data members appended (injected) to an element */
//   template<typename MeshT>
//   class point_accessor
//   {
//   public:
//     typedef typename viennagrid::result_of::point<MeshT>::type point_type;
//     typedef typename viennagrid::result_of::element<MeshT>::type element_type;
//
//     typedef point_type value_type;
//
//     point_accessor(MeshT const & mesh_in) : mesh_(mesh_in) {}
//
//     template<bool mesh_is_const>
//     point_accessor(base_mesh_region<mesh_is_const> const & region) : mesh_(region.mesh()) {}
//
//     template<typename ElementT>
//     point_type get(ElementT const & element) const { return viennagrid::get_point(mesh_, element); }
//
//     template<typename ElementT>
//     void set(ElementT const & element, point_type const & value) { viennagrid::set_point(mesh_, element, value); }
//
//   private:
//     MeshT mesh_;
//   };


  /** @brief Accessor class for the data members appended (injected) to an element */
  class mesh_point_accessor
  {
  public:
    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::point<MeshType>::type point_type;
    typedef viennagrid::result_of::element<MeshType>::type element_type;

    typedef point_type value_type;

    mesh_point_accessor(MeshType const & mesh_in) : mesh_(mesh_in) {}

    template<bool mesh_is_const>
    mesh_point_accessor(base_mesh_region<mesh_is_const> const & region) : mesh_(region.mesh()) {}

    template<typename ElementT>
    point_type get(ElementT const & element) const { return viennagrid::get_point(mesh_, element); }

    template<typename ElementT>
    void set(ElementT const & element, point_type const & value) { viennagrid::set_point(mesh_, element, value); }

  private:
    MeshType mesh_;
  };


  class root_mesh_point_accessor
  {
  public:
    typedef viennagrid::point point_type;
    typedef viennagrid::element element_type;

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

    template<typename ElementT>
    struct point<mesh_point_accessor, ElementT>
    {
      typedef typename mesh_point_accessor::point_type type;
    };

    template<>
    struct point<mesh_point_accessor, viennagrid::point>
    {
      typedef typename mesh_point_accessor::point_type type;
    };


    template<typename ElementT>
    struct point<root_mesh_point_accessor, ElementT>
    {
      typedef typename viennagrid::result_of::point<ElementT>::type type;
    };

    template<>
    struct point<root_mesh_point_accessor, viennagrid::point>
    {
      typedef viennagrid::point type;
    };



//     /** @brief Metafunction returning the default point accessor type for a vertex. */
//     template<typename SomethingT>
//     struct point_accessor
//     {
//       typedef viennagrid::point_accessor<
//         typename viennagrid::result_of::mesh<SomethingT>::type
//       > type;
//     };
  }

//   /** @brief Convenience function for returning the default point accessor (i.e. accessing the point stored in the element's appendix) for a mesh or element. */
//   template<bool mesh_is_const>
//   typename result_of::point_accessor< base_mesh<mesh_is_const> >::type point_accessor(base_mesh<mesh_is_const> mesh)
//   {
//     return typename result_of::point_accessor< base_mesh<mesh_is_const> >::type(mesh);
//   }
//
//   template<bool mesh_is_const>
//   typename result_of::point_accessor<
//     typename viennagrid::result_of::mesh< base_mesh_region<mesh_is_const> >::type
//   >::type point_accessor(base_mesh_region<mesh_is_const> region)
//   {
//     return point_accessor(region.mesh());
//   }


//   inline root_mesh_point_accessor root_mesh_point_accessor()
//   {
//     return root_mesh_point_accessor();
//   }
}

#endif
