#ifndef VIENNAGRID_RANGE_HPP
#define VIENNAGRID_RANGE_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/element/element.hpp"
#include "viennagrid/mesh/region.hpp"

namespace viennagrid
{

  template<bool is_const>
  struct unpack_element_functor
  {
    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<element, is_const>::type value_type;
    typedef typename result_of::const_nonconst<element, true>::type const_value_type;

    typedef typename result_of::const_nonconst<value_type *, is_const>::type pointer;
    typedef value_type reference;

    unpack_element_functor(element_tag element_tag_in, mesh_hierarchy_type mesh_hierarchy_in) : element_tag_(element_tag_in), mesh_hierarchy_(mesh_hierarchy_in) {}

    template<bool other_is_const>
    unpack_element_functor(unpack_element_functor<other_is_const> const & other) : element_tag_(other.element_tag_), mesh_hierarchy_(other.mesh_hierarchy_) {}


    value_type operator()(viennagrid_index & it) const { return value_type(element_tag_, mesh_hierarchy_, it); }
    const_value_type operator()(viennagrid_index const & it) const { return const_value_type(element_tag_, mesh_hierarchy_, it); }

    element_tag element_tag_;
    mesh_hierarchy_type mesh_hierarchy_;
  };




  struct true_functor
  {
    template<typename T>
    bool operator()(T)
    { return true; }
  };









  template<typename ViewFunctorT, bool is_const>
  class base_element_range
  {
  public:
    typedef std::size_t size_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy, true>::type const_mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh, true>::type const_mesh_type;

    typedef typename result_of::const_nonconst<viennagrid_index *, is_const>::type index_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_index *, true>::type const_index_pointer_type;

    typedef typename result_of::const_nonconst<element, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element, false>::type nonconst_element_type;
    typedef typename result_of::const_nonconst<element, true>::type const_element_type;


    base_element_range() : mesh_hierarchy_(0), element_index_begin(0), element_index_end(0) {}
    base_element_range(ViewFunctorT view_functor_) : mesh_hierarchy_(0), element_index_begin(0), element_index_end(0), view_functor(view_functor_) {}


    template<bool mesh_is_const>
    void from_mesh(base_mesh<mesh_is_const> mesh, element_tag element_tag_in);

    template<bool element_is_const>
    void boundary_from_element(base_element<element_is_const> element, element_tag element_tag_in);

    template<bool mesh_is_const, bool element_is_const>
    void coboundary_from_element(base_mesh<mesh_is_const> mesh, base_element<element_is_const> element, element_tag coboundary_tag_in);


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
    { return element_type(tag(), get_mesh_hierarchy(), element_index_begin[pos]); }
    const_element_type operator[](size_type pos) const
    { return const_element_type(tag(), get_mesh_hierarchy(), element_index_begin[pos]); }


    mesh_hierarchy_type get_mesh_hierarchy() { return mesh_hierarchy_; }
    const_mesh_hierarchy_type get_mesh_hierarchy() const { return mesh_hierarchy_; }

    element_tag tag() const { return element_tag_; }

  protected:
    element_tag element_tag_;
    mesh_hierarchy_type mesh_hierarchy_;
    index_pointer_type element_index_begin;
    index_pointer_type element_index_end;
    ViewFunctorT view_functor;
  };






  template<typename ViewFunctorT, bool range_is_const>
  template<bool mesh_is_const>
  void base_element_range<ViewFunctorT, range_is_const>::from_mesh(base_mesh<mesh_is_const> mesh, element_tag element_tag_in)
  {
    element_tag_ = element_tag_in;
    mesh_hierarchy_ = mesh.get_mesh_hierarchy();

    viennagrid_elements_get(mesh.internal_mesh(),
                            tag().internal_element_tag(),
                            const_cast<viennagrid_index **>(&element_index_begin),
                            const_cast<viennagrid_index **>(&element_index_end));
  }

  template<typename ViewFunctorT, bool range_is_const>
  template<bool element_is_const>
  void base_element_range<ViewFunctorT, range_is_const>::boundary_from_element(base_element<element_is_const> element, element_tag element_tag_in)
  {
    element_tag_ = element_tag_in;
    mesh_hierarchy_ = element.get_mesh_hierarchy();

    viennagrid_element_boundary_elements(get_mesh_hierarchy().internal_mesh_hierarchy(),
                                         element.tag().internal_element_tag(),
                                         element.id(),
                                         tag().internal_element_tag(),
                                         const_cast<viennagrid_index **>(&element_index_begin),
                                         const_cast<viennagrid_index **>(&element_index_end));
  }

  template<typename ViewFunctorT, bool range_is_const>
  template<bool mesh_is_const, bool element_is_const>
  void base_element_range<ViewFunctorT, range_is_const>::coboundary_from_element(base_mesh<mesh_is_const> mesh, base_element<element_is_const> element, element_tag coboundary_tag_in)
  {
    element_tag_ = coboundary_tag_in;
    mesh_hierarchy_ = element.get_mesh_hierarchy();

    viennagrid_element_coboundary_elements(mesh.internal_mesh(),
                                           element.tag().internal_element_tag(),
                                           element.id(),
                                           tag().internal_element_tag(),
                                           &element_index_begin,
                                           &element_index_end);
  }






  template<bool is_const>
  class mesh_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    mesh_element_range(mesh_type mesh, element_tag element_tag_)
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
    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    boundary_element_range(element_type element, element_tag element_tag_)
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
    typedef typename base_element_range<true_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    coboundary_element_range(mesh_type mesh, element_type element, element_tag coboundary_tag_)
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




  struct region_view_functor
  {
    region_view_functor(const_mesh_region mesh_region_) : mesh_region(mesh_region_) {}

    bool operator()(element e) const
    { return is_in_region(e, mesh_region); }
    bool operator()(const_element e) const
    { return is_in_region(e, mesh_region); }

    const_mesh_region mesh_region;
  };


  template<bool is_const>
  class mesh_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type mesh_region_type;

    mesh_region_element_range(mesh_region_type region, element_tag element_tag_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->from_mesh(region.get_mesh(), element_tag_); }

  private:
  };

  template<typename ElementTagT, bool is_const>
  class static_mesh_region_element_range : public mesh_region_element_range<is_const>
  {
  public:

    typedef typename mesh_region_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename mesh_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename mesh_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type mesh_region_type;

    static_mesh_region_element_range(mesh_region_type region) : mesh_region_element_range<is_const>(region, ElementTagT()) {}

  private:
  };


  template<bool is_const>
  class coboundary_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type mesh_region_type;

    coboundary_region_element_range(mesh_region_type region, element_type element, element_tag coboundary_tag_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->coboundary_from_element(region.get_mesh(), element, coboundary_tag_); }

  private:
  };


  template<typename CoboundaryTagT, bool is_const>
  class static_coboundary_region_element_range : public coboundary_region_element_range<is_const>
  {
  public:

    typedef typename coboundary_region_element_range<is_const>::mesh_hierarchy_type mesh_hierarchy_type;
    typedef typename coboundary_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename coboundary_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type mesh_region_type;

    static_coboundary_region_element_range(mesh_region_type region, element_type element) : coboundary_region_element_range<is_const>(region, element, CoboundaryTagT()) {}

  private:
  };






  namespace result_of
  {
    template<>
    struct element_range<viennagrid::mesh, null_type>
    {
      typedef mesh_element_range<false> type;
    };

    template<typename ElementTagT>
    struct element_range<viennagrid::mesh, ElementTagT>
    {
      typedef static_mesh_element_range<ElementTagT, false> type;
    };


    template<>
    struct const_element_range<viennagrid::mesh, null_type>
    {
      typedef mesh_element_range<true> type;
    };

    template<typename ElementTagT>
    struct const_element_range<viennagrid::mesh, ElementTagT>
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
    struct element_range<viennagrid::element, null_type>
    {
      typedef boundary_element_range<false> type;
    };

    template<typename ElementTagT>
    struct element_range<viennagrid::element, ElementTagT>
    {
      typedef static_boundary_element_range<ElementTagT, false> type;
    };

    template<>
    struct element_range<viennagrid::const_element, null_type>
    {
      typedef typename const_element_range<viennagrid::const_element>::type type;
    };

    template<typename ElementTagT>
    struct element_range<viennagrid::const_element, ElementTagT>
    {
      typedef typename const_element_range<viennagrid::const_element, ElementTagT>::type type;
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
    struct element_range<mesh_region, null_type>
    {
      typedef mesh_region_element_range<false> type;
    };

    template<typename ElementTagT>
    struct element_range<mesh_region, ElementTagT>
    {
      typedef static_mesh_region_element_range<ElementTagT, false> type;
    };

    template<>
    struct element_range<const_mesh_region, null_type>
    {
      typedef typename const_element_range<mesh_region>::type type;
    };

    template<typename ElementTagT>
    struct element_range<const_mesh_region, ElementTagT>
    {
      typedef typename const_element_range<mesh_region, ElementTagT>::type type;
    };







    template<typename ElementTagT>
    struct coboundary_range<viennagrid::mesh, ElementTagT>
    {
      typedef static_coboundary_element_range<ElementTagT, false> type;
    };

    template<typename ElementTagT>
    struct const_coboundary_range<viennagrid::mesh, ElementTagT>
    {
      typedef static_coboundary_element_range<ElementTagT, true> type;
    };


    template<typename ElementTagT>
    struct coboundary_range<viennagrid::mesh_region, ElementTagT>
    {
      typedef static_coboundary_region_element_range<ElementTagT, false> type;
    };

    template<typename ElementTagT>
    struct const_coboundary_range<viennagrid::mesh_region, ElementTagT>
    {
      typedef static_coboundary_region_element_range<ElementTagT, true> type;
    };

  }



  template<typename SomethingT>
  typename viennagrid::result_of::element_range<SomethingT>::type elements(SomethingT something, mesh::element_tag_type element_tag)
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
  typename result_of::cell_range<SomethingT>::type cells(SomethingT & something)
  { return typename result_of::cell_range<SomethingT>::type(something); }

  /** @brief Function for retrieving a facet range object from a host object
    *
    * @tparam ElementT           The host element (cell)
    * @param  element            The host object of type ElementT
    * @return                    A facet range
    */
  template<typename ElementT>
  typename result_of::facet_range<ElementT>::type facets(ElementT & element)
  { return typename result_of::facet_range<ElementT>::type(element); }

  /** @brief Function for retrieving a vertex range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A vertex range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, vertex_tag>::type vertices(SomethingT & something)
  { return typename result_of::element_range<SomethingT, vertex_tag>::type(something); }

  /** @brief Function for retrieving a line range object from a host object (same as edges)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A line range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, line_tag>::type lines(SomethingT & something)
  { return typename result_of::element_range<SomethingT, line_tag>::type(something); }

  /** @brief Function for retrieving a edge range object from a host object (same as lines)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    An edge range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, edge_tag>::type edges(SomethingT & something)
  { return typename result_of::element_range<SomethingT, edge_tag>::type(something); }

  /** @brief Function for retrieving a triangle range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A triangle range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, triangle_tag>::type triangles(SomethingT & something)
  { return typename result_of::element_range<SomethingT, triangle_tag>::type(something); }

  /** @brief Function for retrieving a quadrilaterals range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A quadrilateral range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, quadrilateral_tag>::type quadrilaterals(SomethingT & something)
  { return typename result_of::element_range<SomethingT, quadrilateral_tag>::type(something); }

  /** @brief Function for retrieving a polygon range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A polygon range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, polygon_tag>::type polygons(SomethingT & something)
  { return typename result_of::element_range<SomethingT, polygon_tag>::type(something); }

  /** @brief Function for retrieving a PLC range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A PLC range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, plc_tag>::type plcs(SomethingT & something)
  { return typename result_of::element_range<SomethingT, plc_tag>::type(something); }

  /** @brief Function for retrieving a tetrahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A tetrahedron range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, tetrahedron_tag>::type tetrahedra(SomethingT & something)
  { return typename result_of::element_range<SomethingT, tetrahedron_tag>::type(something); }

  /** @brief Function for retrieving a hexahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A hexahedron range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, hexahedron_tag>::type hexahedra(SomethingT & something)
  { return typename result_of::element_range<SomethingT, hexahedron_tag>::type(something); }



//   /** @brief Function for retrieving a const element range or a boundary element range from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @tparam ElementTypeOrTagT  The element type/tag for the requested element range
//     * @param  something          The host object of type SomethingT
//     * @return                    A const element range
//     */
//   template<typename ElementTypeOrTagT, typename SomethingT>
//   typename result_of::const_element_range<SomethingT, ElementTypeOrTagT>::type elements(SomethingT const & something);
//
//   /** @brief Function for retrieving an element range or a boundary element range from a mesh. Const version.
//     *
//     * @tparam WrappedConfigType  The host mesh configuration class (providing the typemap as 'type' member type)
//     * @tparam ElementTypeOrTagT  The element type/tag for the requested element range
//     * @param  mesh_obj           The mesh object
//     * @return                    An element range
//     */
//   template<typename ElementTypeOrTagT, typename WrappedConfigType>
//   typename result_of::const_element_range<viennagrid::mesh<WrappedConfigType>, ElementTypeOrTagT>::type
//   elements(viennagrid::mesh<WrappedConfigType> const & mesh_obj);
//
//
//   /** @brief Function for retrieving a const element range or a boundary element range from a segment. Const-version.
//     *
//     * @tparam SegmentationT      The host segmentation
//     * @tparam ElementTypeOrTagT  The element type/tag for the requested element range
//     * @param  segment            The host object of type SomethingT
//     * @return                    A const element range
//     */
//   template<typename ElementTypeOrTagT, typename SegmentationT>
//   typename result_of::const_element_range< viennagrid::segment_handle<SegmentationT>, ElementTypeOrTagT>::type
//   elements( viennagrid::segment_handle<SegmentationT> const & segment)
//   { return elements<ElementTypeOrTagT>( segment.view() ); }
//
//
//   /** @brief Function for retrieving an element range or a boundary element range from a segmentation. Const version.
//     *
//     * @tparam ElementTypeOrTagT  The element type/tag for the requested element range
//     * @tparam WrappedConfigT     The host mesh configuration class (providing the typemap as 'type' member type)
//     * @param  segm               The hosting segmentation object
//     * @return                    An element range
//     */
//   template<typename ElementTypeOrTagT, typename WrappedConfigT>
//   typename result_of::const_element_range<viennagrid::segmentation<WrappedConfigT>, ElementTypeOrTagT>::type
//   elements(viennagrid::segmentation<WrappedConfigT> const & segm)
//   { return elements<ElementTypeOrTagT>( segm.all_elements() ); }
//
//
//   /** @brief Function for retrieving a const cell range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const cell range
//     */
//   template<typename SomethingT>
//   typename result_of::const_cell_range<SomethingT>::type cells( SomethingT const & something)
//   { return elements<typename result_of::cell_tag<SomethingT>::type>(something); }
//
//   /** @brief Function for retrieving a const facet range object from a host object
//     *
//     * @tparam ElementT         The host element type
//     * @param  element          The host object of type SomethingT
//     * @return                  A const facet range
//     */
//   template<typename ElementT>
//   typename result_of::const_facet_range<ElementT>::type facets(const ElementT & element)
//   { return elements< typename result_of::facet_tag<ElementT>::type >(element); }
//
//   /** @brief Function for retrieving a const vertex range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const vertex range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, vertex_tag>::type vertices(SomethingT const & something)
//   { return elements<vertex_tag>(something); }
//
//   /** @brief Function for retrieving a const line range object from a host object (same as edges)
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const line range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, line_tag>::type lines(SomethingT const & something)
//   { return elements<line_tag>(something); }
//
//   /** @brief Function for retrieving a const edge range object from a host object (same as lines)
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const edge range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, edge_tag>::type edges(SomethingT const & something)
//   { return elements<edge_tag>(something); }
//
//   /** @brief Function for retrieving a const triangles range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const triangle range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, triangle_tag>::type triangles(SomethingT const & something)
//   { return elements<triangle_tag>(something); }
//
//   /** @brief Function for retrieving a const quadrilateral range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const quadrilateral range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, quadrilateral_tag>::type quadrilaterals(SomethingT const & something)
//   { return elements<quadrilateral_tag>(something); }
//
//   /** @brief Function for retrieving a const polygon range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const polygon range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, polygon_tag>::type polygons(SomethingT const & something)
//   { return elements<polygon_tag>(something); }
//
//   /** @brief Function for retrieving a const PLC range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const PLC range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, plc_tag>::type plcs(SomethingT const & something)
//   { return elements<plc_tag>(something); }
//
//   /** @brief Function for retrieving a const tetrahedron range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const tetrahedron range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, tetrahedron_tag>::type tetrahedra(SomethingT const & something)
//   { return elements<tetrahedron_tag>(something); }
//
//   /** @brief Function for retrieving a const hexahedron range object from a host object
//     *
//     * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
//     * @param  something          The host object of type SomethingT
//     * @return                    A const hexahedron range
//     */
//   template<typename SomethingT>
//   typename result_of::const_element_range<SomethingT, hexahedron_tag>::type hexahedra(SomethingT const & something)
//   { return elements<hexahedron_tag>(something); }





}

#endif