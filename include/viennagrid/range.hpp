#ifndef VIENNAGRID_RANGE_HPP
#define VIENNAGRID_RANGE_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/utils.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/element/element.hpp"
#include "viennagrid/mesh/region.hpp"

namespace viennagrid
{

  template<bool is_const>
  struct unpack_element_functor
  {
    typedef element_tag_t element_tag_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<element_t, is_const>::type value_type;
    typedef typename result_of::const_nonconst<element_t, true>::type const_value_type;

    typedef typename result_of::const_nonconst<value_type *, is_const>::type pointer;
    typedef value_type reference;

    unpack_element_functor(element_tag_type element_tag_in, mesh_hierarchy_type mesh_hierarchy_in) : element_tag_(element_tag_in), mesh_hierarchy_(mesh_hierarchy_in) {}

    template<bool other_is_const>
    unpack_element_functor(unpack_element_functor<other_is_const> const & other) : element_tag_(other.element_tag_), mesh_hierarchy_(other.mesh_hierarchy_) {}


    value_type operator()(viennagrid_index & it) const { return value_type(element_tag_, mesh_hierarchy_, it); }
    const_value_type operator()(viennagrid_index const & it) const { return const_value_type(element_tag_, mesh_hierarchy_, it); }

    element_tag_type element_tag_;
    mesh_hierarchy_type mesh_hierarchy_;
  };




  template<typename ViewFunctorT, bool is_const>
  class base_element_range
  {
  public:
    typedef std::size_t size_type;
    typedef element_tag_t element_tag_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, true>::type const_mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<mesh_t, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh_t, true>::type const_mesh_type;

    typedef typename result_of::const_nonconst<viennagrid_index *, is_const>::type index_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_index *, true>::type const_index_pointer_type;

    typedef typename result_of::const_nonconst<element_t, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element_t, false>::type nonconst_element_type;
    typedef typename result_of::const_nonconst<element_t, true>::type const_element_type;


    base_element_range() : mesh_hierarchy_(0), element_index_begin(0), element_index_end(0) {}
    base_element_range(ViewFunctorT view_functor_) : mesh_hierarchy_(0), element_index_begin(0), element_index_end(0), view_functor(view_functor_) {}


    template<bool mesh_is_const>
    void from_mesh(base_mesh<mesh_is_const> mesh, element_tag_type element_tag_in)
    {
      element_tag_ = mesh.unpack_element_tag(element_tag_in);
      mesh_hierarchy_ = mesh.mesh_hierarchy();

      viennagrid_elements_get(mesh.internal(),
                              tag().internal(),
                              const_cast<viennagrid_index **>(&element_index_begin),
                              const_cast<viennagrid_index **>(&element_index_end));
    }

    template<bool element_is_const>
    void boundary_from_element(base_element<element_is_const> element, element_tag_type element_tag_in)
    {
      element_tag_ = element.unpack_element_tag(element_tag_in);
      mesh_hierarchy_ = element.mesh_hierarchy();

      viennagrid_element_boundary_elements(mesh_hierarchy().internal(),
                                          element.tag().internal(),
                                          element.id(),
                                          tag().internal(),
                                          const_cast<viennagrid_index **>(&element_index_begin),
                                          const_cast<viennagrid_index **>(&element_index_end));
    }

    template<bool mesh_is_const, bool element_is_const>
    void coboundary_from_element(base_mesh<mesh_is_const> mesh, base_element<element_is_const> element, element_tag_type coboundary_tag_in)
    {
      element_tag_ = mesh.unpack_element_tag(coboundary_tag_in);
      mesh_hierarchy_ = element.mesh_hierarchy();

      viennagrid_element_coboundary_elements(mesh.internal(),
                                            element.tag().internal(),
                                            element.id(),
                                            tag().internal(),
                                            const_cast<viennagrid_index **>(&element_index_begin),
                                            const_cast<viennagrid_index **>(&element_index_end));
    }

    template<bool mesh_is_const, bool element_is_const>
    void neighbor_from_element(base_mesh<mesh_is_const> mesh, base_element<element_is_const> element, element_tag_type connector_tag_in, element_tag_type neighbor_tag_in)
    {
      element_tag_ = mesh.unpack_element_tag(neighbor_tag_in);
      mesh_hierarchy_ = element.mesh_hierarchy();

      viennagrid_element_neighbor_elements(mesh.internal(),
                                          element.tag().internal(),
                                          element.id(),
                                          connector_tag_in.internal(),
                                          tag().internal(),
                                          const_cast<viennagrid_index **>(&element_index_begin),
                                          const_cast<viennagrid_index **>(&element_index_end));
    }





    typedef unpack_element_functor<is_const> unpack_functor;
    typedef viennagrid::transform_iterator<index_pointer_type, unpack_functor> transform_iterator;
    typedef view_iterator<transform_iterator, ViewFunctorT> iterator;

    typedef unpack_element_functor<true> const_unpack_functor;
    typedef viennagrid::transform_iterator<const_index_pointer_type, const_unpack_functor> const_transform_iterator;
    typedef view_iterator<const_transform_iterator, ViewFunctorT> const_iterator;


    iterator begin()
    {
      return iterator(transform_iterator(element_index_begin,
                                         unpack_functor(element_tag_, mesh_hierarchy_)),
                      transform_iterator(element_index_end,
                                         unpack_functor(element_tag_, mesh_hierarchy_)),
                      view_functor);
    }
    iterator end()
    {
      return iterator(transform_iterator(element_index_end,
                                         unpack_functor(element_tag_, mesh_hierarchy_)),
                      view_functor);

    }


    const_iterator cbegin() const
    {
      return const_iterator(const_transform_iterator(element_index_begin,
                                                     const_unpack_functor(element_tag_, mesh_hierarchy_)),
                            const_transform_iterator(element_index_end,
                                                     const_unpack_functor(element_tag_, mesh_hierarchy_)),
                            view_functor);
    }
    const_iterator cend() const
    {
      return const_iterator(const_transform_iterator(element_index_end,
                                                     const_unpack_functor(element_tag_, mesh_hierarchy_)),
                            view_functor);

    }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }


    size_type size() const { return end()-begin(); }

    element_type operator[](size_type pos)
    { return element_type(tag(), mesh_hierarchy(), element_index_begin[pos]); }
    const_element_type operator[](size_type pos) const
    { return const_element_type(tag(), mesh_hierarchy(), element_index_begin[pos]); }


    mesh_hierarchy_type mesh_hierarchy() { return mesh_hierarchy_; }
    const_mesh_hierarchy_type mesh_hierarchy() const { return mesh_hierarchy_; }

    element_tag_type tag() const { return element_tag_; }

  protected:
    element_tag_type element_tag_;
    mesh_hierarchy_type mesh_hierarchy_;
    index_pointer_type element_index_begin;
    index_pointer_type element_index_end;
    ViewFunctorT view_functor;
  };






//   template<typename ViewFunctorT, bool range_is_const>
//   template<bool mesh_is_const>
//   void base_element_range<ViewFunctorT, range_is_const>::from_mesh(base_mesh<mesh_is_const> mesh, element_tag_type element_tag_in)
//   {
//     element_tag_ = mesh.unpack_element_tag(element_tag_in);
//     mesh_hierarchy_ = mesh.mesh_hierarchy();
//
//     viennagrid_elements_get(mesh.internal(),
//                             tag().internal(),
//                             const_cast<viennagrid_index **>(&element_index_begin),
//                             const_cast<viennagrid_index **>(&element_index_end));
//   }
//
//   template<typename ViewFunctorT, bool range_is_const>
//   template<bool element_is_const>
//   void base_element_range<ViewFunctorT, range_is_const>::boundary_from_element(base_element<element_is_const> element, element_tag_type element_tag_in)
//   {
//     element_tag_ = element.unpack_element_tag(element_tag_in);
//     mesh_hierarchy_ = element.mesh_hierarchy();
//
//     viennagrid_element_boundary_elements(mesh_hierarchy().internal(),
//                                          element.tag().internal(),
//                                          element.id(),
//                                          tag().internal(),
//                                          const_cast<viennagrid_index **>(&element_index_begin),
//                                          const_cast<viennagrid_index **>(&element_index_end));
//   }
//
//   template<typename ViewFunctorT, bool range_is_const>
//   template<bool mesh_is_const, bool element_is_const>
//   void base_element_range<ViewFunctorT, range_is_const>::coboundary_from_element(base_mesh<mesh_is_const> mesh, base_element<element_is_const> element, element_tag_type coboundary_tag_in)
//   {
//     element_tag_ = mesh.unpack_element_tag(coboundary_tag_in);
//     mesh_hierarchy_ = element.mesh_hierarchy();
//
//     viennagrid_element_coboundary_elements(mesh.internal(),
//                                            element.tag().internal(),
//                                            element.id(),
//                                            tag().internal(),
//                                            const_cast<viennagrid_index **>(&element_index_begin),
//                                            const_cast<viennagrid_index **>(&element_index_end));
//   }
//
//
//
//
//
//
//   template<typename ViewFunctorT, bool range_is_const>
//   template<bool element_is_const>
//   void base_element_range<ViewFunctorT, range_is_const>::boundary_from_element(base_element<element_is_const> element, element_tag_type element_tag_in)
//   {
//     element_tag_ = element.unpack_element_tag(element_tag_in);
//     mesh_hierarchy_ = element.mesh_hierarchy();
//
//     viennagrid_element_boundary_elements(mesh_hierarchy().internal(),
//                                          element.tag().internal(),
//                                          element.id(),
//                                          tag().internal(),
//                                          const_cast<viennagrid_index **>(&element_index_begin),
//                                          const_cast<viennagrid_index **>(&element_index_end));
//   }
//
//
//   template<typename ViewFunctorT, bool range_is_const>
//   template<bool mesh_is_const, bool element_is_const>
//   void base_element_range<ViewFunctorT, range_is_const>::neighbor_from_element(base_mesh<mesh_is_const> mesh, base_element<element_is_const> element, element_tag_type connector_tag_in, element_tag_type neighbor_tag_in)
//   {
//     element_tag_ = mesh.unpack_element_tag(neighbor_tag_in);
//     mesh_hierarchy_ = element.mesh_hierarchy();
//
//     viennagrid_element_neighbor_elements(mesh.internal(),
//                                          element.tag().internal(),
//                                          element.id(),
//                                          connector_tag_in.internal(),
//                                          tag().internal(),
//                                          const_cast<viennagrid_index **>(&element_index_begin),
//                                          const_cast<viennagrid_index **>(&element_index_end));
//   }






  template<bool is_const>
  class mesh_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    mesh_element_range(mesh_type mesh, element_tag_type element_tag_)
    { this->from_mesh(mesh, element_tag_); }

  private:
  };

  template<typename ElementTagT, bool is_const>
  class static_mesh_element_range : public mesh_element_range<is_const>
  {
  public:

    typedef typename mesh_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename mesh_element_range<is_const>::mesh_type mesh_type;
    typedef typename mesh_element_range<is_const>::element_type element_type;

    static_mesh_element_range(mesh_type mesh) : mesh_element_range<is_const>(mesh, ElementTagT()) {}

  private:
  };



  template<bool is_const>
  class boundary_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    boundary_element_range(element_type element, element_tag_type element_tag_)
    { this->boundary_from_element(element, element_tag_); }

  private:
  };


  template<typename BoundaryTagT, bool is_const>
  class static_boundary_element_range : public boundary_element_range<is_const>
  {
  public:

    typedef typename boundary_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename boundary_element_range<is_const>::mesh_type mesh_type;
    typedef typename boundary_element_range<is_const>::element_type element_type;

    static_boundary_element_range(element_type element) : boundary_element_range<is_const>(element, BoundaryTagT()) {}

  private:
  };



  template<bool is_const>
  class coboundary_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    coboundary_element_range(mesh_type mesh, element_type element, element_tag_type coboundary_tag_)
    { this->coboundary_from_element(mesh, element, coboundary_tag_); }

  private:
  };


  template<typename CoboundaryTagT, bool is_const>
  class static_coboundary_element_range : public coboundary_element_range<is_const>
  {
  public:

    typedef typename coboundary_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename coboundary_element_range<is_const>::mesh_type mesh_type;
    typedef typename coboundary_element_range<is_const>::element_type element_type;

    static_coboundary_element_range(mesh_type mesh, element_type element) : coboundary_element_range<is_const>(mesh, element, CoboundaryTagT()) {}

  private:
  };



  template<bool is_const>
  class neighbor_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    neighbor_element_range(mesh_type mesh, element_type element, element_tag_type connector_tag_, element_tag_type neighbor_tag_)
    { this->neighbor_from_element(mesh, element, connector_tag_, neighbor_tag_); }

  private:
  };


  template<typename ConnectorTagT, typename NeighborTagT, bool is_const>
  class static_neighbor_element_range : public neighbor_element_range<is_const>
  {
  public:

    typedef typename neighbor_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename neighbor_element_range<is_const>::mesh_type mesh_type;
    typedef typename neighbor_element_range<is_const>::element_type element_type;

    static_neighbor_element_range(mesh_type mesh, element_type element) : neighbor_element_range<is_const>(mesh, element, ConnectorTagT(), NeighborTagT()) {}

  private:
  };









  struct region_view_functor
  {
    region_view_functor(const_mesh_region_t mesh_region_) : mesh_region(mesh_region_) {}

    bool operator()(element_t element) const
    { return is_in_region(mesh_region, element); }
    bool operator()(const_element_t element) const
    { return is_in_region(mesh_region, element); }

    const_mesh_region_t mesh_region;
  };


  template<bool is_const>
  class mesh_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<region_view_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type mesh_region_type;

    mesh_region_element_range(mesh_region_type region, element_tag_type element_tag_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->from_mesh(region.mesh(), element_tag_); }

  private:
  };

  template<typename ElementTagT, bool is_const>
  class static_mesh_region_element_range : public mesh_region_element_range<is_const>
  {
  public:

    typedef typename mesh_region_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename mesh_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename mesh_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type mesh_region_type;

    static_mesh_region_element_range(mesh_region_type region) : mesh_region_element_range<is_const>(region, ElementTagT()) {}

  private:
  };


  template<bool is_const>
  class coboundary_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<region_view_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type mesh_region_type;

    coboundary_region_element_range(mesh_region_type region, element_type element, element_tag_type coboundary_tag_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->coboundary_from_element(region.mesh(), element, coboundary_tag_); }

  private:
  };


  template<typename CoboundaryTagT, bool is_const>
  class static_coboundary_region_element_range : public coboundary_region_element_range<is_const>
  {
  public:

    typedef typename coboundary_region_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename coboundary_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename coboundary_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type mesh_region_type;

    static_coboundary_region_element_range(mesh_region_type region, element_type element) : coboundary_region_element_range<is_const>(region, element, CoboundaryTagT()) {}

  private:
  };


  template<bool is_const>
  class neighbor_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef element_tag_t element_tag_type;

    typedef typename base_element_range<region_view_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type mesh_region_type;

    neighbor_region_element_range(mesh_region_type region, element_type element, element_tag_type connector_tag_, element_tag_type neighbor_tag_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->neighbor_from_element(region.mesh(), element, connector_tag_, neighbor_tag_); }

  private:
  };


  template<typename ConnectorTagT, typename NeighborTagT, bool is_const>
  class static_neighbor_region_element_range : public neighbor_region_element_range<is_const>
  {
  public:

    typedef typename neighbor_region_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename neighbor_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename neighbor_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type mesh_region_type;

    static_neighbor_region_element_range(mesh_region_type region, element_type element) : neighbor_region_element_range<is_const>(region, element, ConnectorTagT(), NeighborTagT()) {}

  private:
  };









  namespace result_of
  {
    template<>
    struct element_range<mesh_t, null_type>
    {
      typedef mesh_element_range<false> type;
    };

    template<typename ElementTagT>
    struct element_range<mesh_t, ElementTagT>
    {
      typedef static_mesh_element_range<ElementTagT, false> type;
    };


    template<>
    struct const_element_range<mesh_t, null_type>
    {
      typedef mesh_element_range<true> type;
    };

    template<typename ElementTagT>
    struct const_element_range<mesh_t, ElementTagT>
    {
      typedef static_mesh_element_range<ElementTagT, true> type;
    };







    template<bool is_const>
    struct const_element_range<base_element<is_const>, null_type>
    {
      typedef boundary_element_range<true> type;
    };

    template<typename ElementTagT, bool is_const>
    struct const_element_range<base_element<is_const>, ElementTagT>
    {
      typedef static_boundary_element_range<ElementTagT, true> type;
    };


    template<>
    struct element_range<element_t, null_type>
    {
      typedef boundary_element_range<false> type;
    };

    template<typename ElementTagT>
    struct element_range<element_t, ElementTagT>
    {
      typedef static_boundary_element_range<ElementTagT, false> type;
    };

    template<>
    struct element_range<const_element_t, null_type>
    {
      typedef typename const_element_range<const_element_t>::type type;
    };

    template<typename ElementTagT>
    struct element_range<const_element_t, ElementTagT>
    {
      typedef typename const_element_range<const_element_t, ElementTagT>::type type;
    };









    template<bool is_const>
    struct const_element_range<base_mesh_region<is_const>, null_type>
    {
      typedef mesh_region_element_range<true> type;
    };

    template<typename ElementTagT, bool is_const>
    struct const_element_range<base_mesh_region<is_const>, ElementTagT>
    {
      typedef static_mesh_region_element_range<ElementTagT, true> type;
    };


    template<>
    struct element_range<mesh_region_t, null_type>
    {
      typedef mesh_region_element_range<false> type;
    };

    template<typename ElementTagT>
    struct element_range<mesh_region_t, ElementTagT>
    {
      typedef static_mesh_region_element_range<ElementTagT, false> type;
    };

    template<>
    struct element_range<const_mesh_region_t, null_type>
    {
      typedef typename const_element_range<mesh_region_t>::type type;
    };

    template<typename ElementTagT>
    struct element_range<const_mesh_region_t, ElementTagT>
    {
      typedef typename const_element_range<mesh_region_t, ElementTagT>::type type;
    };







    template<typename CoboundaryTagT>
    struct coboundary_range<mesh_t, CoboundaryTagT>
    {
      typedef static_coboundary_element_range<CoboundaryTagT, false> type;
    };

    template<typename CoboundaryTagT>
    struct const_coboundary_range<mesh_t, CoboundaryTagT>
    {
      typedef static_coboundary_element_range<CoboundaryTagT, true> type;
    };


    template<typename CoboundaryTagT>
    struct coboundary_range<mesh_region_t, CoboundaryTagT>
    {
      typedef static_coboundary_region_element_range<CoboundaryTagT, false> type;
    };

    template<typename CoboundaryTagT>
    struct const_coboundary_range<mesh_region_t, CoboundaryTagT>
    {
      typedef static_coboundary_region_element_range<CoboundaryTagT, true> type;
    };






    template<typename ConnectorTagT, typename NeighborTagT>
    struct neighbor_range<mesh_t, ConnectorTagT, NeighborTagT>
    {
      typedef static_neighbor_element_range<ConnectorTagT, NeighborTagT, false> type;
    };

    template<typename ConnectorTagT, typename NeighborTagT>
    struct const_neighbor_range<mesh_t, ConnectorTagT, NeighborTagT>
    {
      typedef static_neighbor_element_range<ConnectorTagT, NeighborTagT, true> type;
    };


    template<typename ConnectorTagT, typename NeighborTagT>
    struct neighbor_range<mesh_region_t, ConnectorTagT, NeighborTagT>
    {
      typedef static_neighbor_region_element_range<ConnectorTagT, NeighborTagT, false> type;
    };

    template<typename ConnectorTagT, typename NeighborTagT>
    struct const_neighbor_range<mesh_region_t, ConnectorTagT, NeighborTagT>
    {
      typedef static_neighbor_region_element_range<ConnectorTagT, NeighborTagT, true> type;
    };

  }



  template<typename SomethingT>
  typename viennagrid::result_of::element_range<SomethingT>::type elements(SomethingT something, typename viennagrid::result_of::element_tag<SomethingT>::type element_tag)
  { return typename viennagrid::result_of::element_range<SomethingT>::type(something, element_tag); }

  template<typename ElementTagT, typename SomethingT>
  typename viennagrid::result_of::element_range<SomethingT, ElementTagT>::type elements(SomethingT something)
  { return typename viennagrid::result_of::element_range<SomethingT, ElementTagT>::type(something); }






  /** @brief Function for retrieving a cell range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A cell range
    */
  template<typename SomethingT>
  typename result_of::cell_range<SomethingT>::type cells(SomethingT something)
  { return typename result_of::cell_range<SomethingT>::type(something); }

  /** @brief Function for retrieving a facet range object from a host object
    *
    * @tparam ElementT           The host element (cell)
    * @param  element            The host object of type ElementT
    * @return                    A facet range
    */
  template<typename ElementT>
  typename result_of::facet_range<ElementT>::type facets(ElementT element)
  { return typename result_of::facet_range<ElementT>::type(element); }

  /** @brief Function for retrieving a vertex range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A vertex range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, vertex_tag>::type vertices(SomethingT something)
  { return typename result_of::element_range<SomethingT, vertex_tag>::type(something); }

  /** @brief Function for retrieving a line range object from a host object (same as edges)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A line range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, line_tag>::type lines(SomethingT something)
  { return typename result_of::element_range<SomethingT, line_tag>::type(something); }

  /** @brief Function for retrieving a edge range object from a host object (same as lines)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    An edge range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, edge_tag>::type edges(SomethingT something)
  { return typename result_of::element_range<SomethingT, edge_tag>::type(something); }

  /** @brief Function for retrieving a triangle range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A triangle range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, triangle_tag>::type triangles(SomethingT something)
  { return typename result_of::element_range<SomethingT, triangle_tag>::type(something); }

  /** @brief Function for retrieving a quadrilaterals range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A quadrilateral range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, quadrilateral_tag>::type quadrilaterals(SomethingT something)
  { return typename result_of::element_range<SomethingT, quadrilateral_tag>::type(something); }

  /** @brief Function for retrieving a polygon range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A polygon range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, polygon_tag>::type polygons(SomethingT something)
  { return typename result_of::element_range<SomethingT, polygon_tag>::type(something); }

  /** @brief Function for retrieving a PLC range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A PLC range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, plc_tag>::type plcs(SomethingT something)
  { return typename result_of::element_range<SomethingT, plc_tag>::type(something); }

  /** @brief Function for retrieving a tetrahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A tetrahedron range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, tetrahedron_tag>::type tetrahedra(SomethingT something)
  { return typename result_of::element_range<SomethingT, tetrahedron_tag>::type(something); }

  /** @brief Function for retrieving a hexahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A hexahedron range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, hexahedron_tag>::type hexahedra(SomethingT something)
  { return typename result_of::element_range<SomethingT, hexahedron_tag>::type(something); }

}

#endif
