#ifndef VIENNAGRID_FORWARDS_HPP
#define VIENNAGRID_FORWARDS_HPP

#include <string>
#include "viennagrid/backend/api.h"

namespace viennagrid
{
  struct true_functor;


  template<bool is_const = false>
  class base_point_proxy;

  typedef base_point_proxy<false> point_proxy;
  typedef base_point_proxy<true> const_point_proxy;




  template<bool is_const = false>
  class base_region;

  typedef base_region<false> region;
  typedef base_region<true> const_region;


  template<bool is_const = false>
  class base_mesh_region;

  typedef base_mesh_region<false> mesh_region;
  typedef base_mesh_region<true> const_mesh_region;





  template<bool is_const = false>
  class base_element;

  typedef base_element<false> element;
  typedef base_element<true> const_element;



  template<bool is_const = false>
  class base_mesh;

  typedef base_mesh<false> mesh;
  typedef base_mesh<true> const_mesh;


  template<bool is_const = false>
  class base_mesh_hierarchy;

  typedef base_mesh_hierarchy<false> mesh_hierarchy;
  typedef base_mesh_hierarchy<true> const_mesh_hierarchy;








  template<typename ViewFunctorT = true_functor, bool is_const = false>
  class base_element_range;




  template<bool is_const>
  class base_region_range;

  typedef base_region_range<false> region_range;
  typedef base_region_range<true> const_region_range;


  template<bool is_const>
  class base_mesh_region_range;

  typedef base_mesh_region_range<false> mesh_region_range;
  typedef base_mesh_region_range<true> const_mesh_region_range;






  class null_type;






  template<viennagrid_element_tag et>
  class static_tag;



  class element_tag
  {
    template<bool is_const>
    friend class base_mesh_hierarchy;

    template<viennagrid_element_tag et>
    friend class static_tag;

  public:


    template<viennagrid_element_tag et>
    element_tag( static_tag<et> ) : element_tag_(et) {}


    element_tag() : element_tag_(VIENNAGRID_ELEMENT_TAG_NO_ELEMENT) {}

    viennagrid_element_tag internal_element_tag() const { return element_tag_; }
    element_tag facet_tag() const { return element_tag( viennagrid_facet_tag(internal_element_tag()) ); }

    static element_tag vertex() { return element_tag(VIENNAGRID_ELEMENT_TAG_VERTEX); }
    static element_tag line() { return element_tag(VIENNAGRID_ELEMENT_TAG_LINE); }
    static element_tag edge() { return element_tag(VIENNAGRID_ELEMENT_TAG_EDGE); }
    static element_tag triangle() { return element_tag(VIENNAGRID_ELEMENT_TAG_TRIANGLE); }
    static element_tag quadrilateral() { return element_tag(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL); }
    static element_tag polygon() { return element_tag(VIENNAGRID_ELEMENT_TAG_POLYGON); }
    static element_tag plc() { return element_tag(VIENNAGRID_ELEMENT_TAG_PLC); }
    static element_tag tetrahedron() { return element_tag(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON); }
    static element_tag hexahedron() { return element_tag(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON); }

    static element_tag cell() { return element_tag(VIENNAGRID_ELEMENT_TAG_CELL); }
    static element_tag facet() { return element_tag(VIENNAGRID_ELEMENT_TAG_FACET); }

    bool operator==(element_tag rhs) const { return element_tag_ == rhs.element_tag_;}
    bool operator!=(element_tag rhs) const { return !(*this == rhs);}

    bool operator<(element_tag rhs) const { return element_tag_ < rhs.element_tag_;}

    element_tag & operator++()
    {
      ++element_tag_;
      return *this;
    }

    element_tag operator++(int)
    {
      element_tag result = *this;
      ++result;
      return result;
    }


    bool is_boundary(element_tag host_tag) const
    { return viennagrid_is_boundary_tag(host_tag.internal_element_tag(), element_tag_) == VIENNAGRID_TRUE; }

    bool is_native() const
    { return viennagrid_native_element_tag(element_tag_) == VIENNAGRID_TRUE; }

    int boundary_element_count(element_tag boundary_tag) const
    { return viennagrid_boundary_element_count_from_element_tag( element_tag_, boundary_tag.internal_element_tag() ); }
    int vertex_count() const { return boundary_element_count( vertex() ); }

    std::string name() const { return std::string( viennagrid_element_tag_string(element_tag_) ); }

  private:
    element_tag(viennagrid_element_tag element_tag_in) : element_tag_(element_tag_in) {}

    viennagrid_element_tag element_tag_;
  };




  template<viennagrid_element_tag et>
  class static_tag : public element_tag
  {
  public:
    static_tag() : element_tag(et) {}
  };

  typedef static_tag<VIENNAGRID_ELEMENT_TAG_VERTEX> vertex_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_LINE> line_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_EDGE> edge_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_TRIANGLE> triangle_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_QUADRILATERAL> quadrilateral_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_POLYGON> polygon_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_PLC> plc_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_TETRAHEDRON> tetrahedron_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_HEXAHEDRON> hexahedron_tag;

  typedef static_tag<VIENNAGRID_ELEMENT_TAG_CELL> cell_tag;
  typedef static_tag<VIENNAGRID_ELEMENT_TAG_FACET> facet_tag;









  namespace result_of
  {
    template<typename SomethingT>
    struct const_type;

    template<typename SomethingT>
    struct const_type<SomethingT*>
    {
      typedef SomethingT const * type;
    };

    template<typename SomethingT>
    struct const_type<SomethingT const *>
    {
      typedef SomethingT const * type;
    };

    template<typename SomethingT>
    struct const_type<SomethingT&>
    {
      typedef SomethingT const & type;
    };

    template<typename SomethingT>
    struct const_type<SomethingT const &>
    {
      typedef SomethingT const & type;
    };

    template<bool is_const>
    struct const_type< base_point_proxy<is_const> >
    {
      typedef base_point_proxy<true> type;
    };

    template<bool is_const>
    struct const_type< base_region<is_const> >
    {
      typedef base_region<true> type;
    };

    template<bool is_const>
    struct const_type< base_mesh_region<is_const> >
    {
      typedef base_mesh_region<true> type;
    };

    template<bool is_const>
    struct const_type< base_element<is_const> >
    {
      typedef base_element<true> type;
    };

    template<typename ViewFunctorT, bool is_const>
    struct const_type< base_element_range<ViewFunctorT, is_const> >
    {
      typedef base_element_range<ViewFunctorT, true> type;
    };

    template<bool is_const>
    struct const_type< base_mesh<is_const> >
    {
      typedef base_mesh<true> type;
    };

    template<bool is_const>
    struct const_type< base_mesh_hierarchy<is_const> >
    {
      typedef base_mesh_hierarchy<true> type;
    };








    template<typename SomethingT>
    struct nonconst_type;

    template<typename SomethingT>
    struct nonconst_type<SomethingT*>
    {
      typedef SomethingT * type;
    };

    template<typename SomethingT>
    struct nonconst_type<SomethingT const *>
    {
      typedef SomethingT * type;
    };

    template<typename SomethingT>
    struct nonconst_type<SomethingT&>
    {
      typedef SomethingT & type;
    };

    template<typename SomethingT>
    struct nonconst_type<SomethingT const &>
    {
      typedef SomethingT & type;
    };

    template<bool is_const>
    struct nonconst_type< base_point_proxy<is_const> >
    {
      typedef base_point_proxy<false> type;
    };

    template<bool is_const>
    struct nonconst_type< base_region<is_const> >
    {
      typedef base_region<false> type;
    };

    template<bool is_const>
    struct nonconst_type< base_mesh_region<is_const> >
    {
      typedef base_mesh_region<false> type;
    };

    template<bool is_const>
    struct nonconst_type< base_element<is_const> >
    {
      typedef base_element<false> type;
    };

    template<typename ViewFunctorT, bool is_const>
    struct nonconst_type< base_element_range<ViewFunctorT, is_const> >
    {
      typedef base_element_range<ViewFunctorT, false> type;
    };

    template<bool is_const>
    struct nonconst_type< base_mesh<is_const> >
    {
      typedef base_mesh<false> type;
    };

    template<bool is_const>
    struct nonconst_type< base_mesh_hierarchy<is_const> >
    {
      typedef base_mesh_hierarchy<false> type;
    };





    template<typename SomethingT, bool IsConstV = false>
    struct const_nonconst;

    template<typename SomethingT>
    struct const_nonconst<SomethingT, false>
    {
      typedef typename nonconst_type<SomethingT>::type type;
    };

    template<typename SomethingT>
    struct const_nonconst<SomethingT, true>
    {
      typedef typename const_type<SomethingT>::type type;
    };










    template<typename SomethingT>
    struct region;

    template<>
    struct region<mesh_hierarchy>
    {
      typedef region type;
    };

    template<>
    struct region<mesh>
    {
      typedef mesh_region type;
    };


    template<typename SomethingT>
    struct const_region;

    template<bool is_const>
    struct const_region< base_mesh_hierarchy<is_const> >
    {
      typedef const_region type;
    };

    template<bool is_const>
    struct const_region< base_mesh<is_const> >
    {
      typedef const_mesh_region type;
    };





    template<typename SomethingT>
    struct region_range;

    template<>
    struct region_range<mesh_hierarchy>
    {
      typedef viennagrid::region_range type;
    };

    template<>
    struct region_range<mesh>
    {
      typedef viennagrid::mesh_region_range type;
    };


    template<typename SomethingT>
    struct const_region_range;

    template<bool is_const>
    struct region_range< base_mesh_hierarchy<is_const> >
    {
      typedef viennagrid::const_region_range type;
    };

    template<bool is_const>
    struct region_range< base_mesh<is_const> >
    {
      typedef viennagrid::const_mesh_region_range type;
    };




    template<typename SomethingT>
    struct cell_tag
    {
      typedef viennagrid::cell_tag type;
    };

    template<typename SomethingT>
    struct facet_tag
    {
      typedef viennagrid::facet_tag type;
    };




    template<typename SomethingT, typename ElementTagT = null_type>
    struct element
    {
      typedef viennagrid::element type;
    };

    template<typename SomethingT, typename ElementTagT = null_type>
    struct const_element
    {
      typedef viennagrid::const_element type;
    };


    /** @brief Metafunction for obtaining the id type of an element
     *
     * @tparam ElementT           The host element type
     */
    template<typename ElementT>
    struct id
    {
      typedef typename ElementT::id_type type;
    };



    template<typename SomethingT>
    struct mesh_hierarchy
    {
      typedef viennagrid::mesh_hierarchy type;
    };

    template<typename SomethingT>
    struct mesh
    {
      typedef viennagrid::mesh type;
    };



    template<typename SomethingT>
    struct point
    {
      typedef typename point< typename mesh_hierarchy<SomethingT>::type >::type type;
    };

    template<typename SomethingT>
    struct const_point
    {
      typedef typename const_type< typename point<SomethingT>::type >::type type;
    };


    template<>
    struct point<viennagrid::mesh_hierarchy>
    {
      typedef viennagrid::point_proxy type;
    };

    template<typename SomethingT>
    struct coord
    {
      typedef typename coord<typename point<SomethingT>::type>::type type;
    };





    template<typename SomethingT, typename ElementTagT = null_type>
    struct element_range;

    template<typename SomethingT, typename ElementTagT = null_type>
    struct const_element_range;

    template<typename SomethingT, typename ElementTagT>
    struct element_range<const SomethingT, ElementTagT>
    {
      typedef typename const_element_range<SomethingT, ElementTagT>::type type;
    };


    template<typename SomethingT, typename ElementTagT = null_type>
    struct coboundary_range;

    template<typename SomethingT, typename ElementTagT = null_type>
    struct const_coboundary_range;














    template<typename SomethingT>
    struct cell
    {
      typedef typename element<SomethingT, typename cell_tag<SomethingT>::type>::type type;
    };

    template<typename SomethingT>
    struct facet
    {
      typedef typename element<SomethingT, typename facet_tag<SomethingT>::type>::type type;
    };


    /** @brief Convenience metafunction for the type retrieval of a vertex
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex
    {
      typedef typename element<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line (same as edge)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct line
    {
      typedef typename element<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a edge (same as line)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct edge
    {
      typedef typename element<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct triangle
    {
      typedef typename element<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct quadrilateral
    {
      typedef typename element<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct polygon
    {
      typedef typename element<SomethingT, polygon_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a plc
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct plc
    {
      typedef typename element<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct tetrahedron
    {
      typedef typename element<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct hexahedron
    {
      typedef typename element<SomethingT, hexahedron_tag>::type type;
    };











    template<typename SomethingT>
    struct const_cell
    {
      typedef typename const_element<SomethingT, typename cell_tag<SomethingT>::type>::type type;
    };


    /** @brief Convenience metafunction for the type retrieval of a vertex
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_vertex
    {
      typedef typename const_element<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line (same as edge)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_line
    {
      typedef typename const_element<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a edge (same as line)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_edge
    {
      typedef typename const_element<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_triangle
    {
      typedef typename const_element<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_quadrilateral
    {
      typedef typename const_element<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_polygon
    {
      typedef typename const_element<SomethingT, polygon_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a plc
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_plc
    {
      typedef typename const_element<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_tetrahedron
    {
      typedef typename const_element<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_hexahedron
    {
      typedef typename const_element<SomethingT, hexahedron_tag>::type type;
    };












    /** @brief Convenience metafunction for the type retrieval of a vertex ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex_id
    {
      typedef typename id< typename vertex<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line ID (same as edge_id)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct line_id
    {
      typedef typename id< typename line<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line ID (same as line_id)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct edge_id
    {
      typedef typename id< typename edge<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct triangle_id
    {
      typedef typename id< typename triangle<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct quadrilateral_id
    {
      typedef typename id< typename quadrilateral<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon v
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct polygon_id
    {
      typedef typename id< typename polygon<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a plc ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct plc_id
    {
      typedef typename id< typename plc<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct tetrahedron_id
    {
      typedef typename id< typename tetrahedron<SomethingT>::type >::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct hexahedron_id
    {
      typedef typename id< typename hexahedron<SomethingT>::type >::type type;
    };




    /** @brief Convenience metafunction for the type retrieval of a cell range. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cell_range
    {
      typedef typename cell_tag<SomethingT>::type cell_tag;
      typedef typename element_range<SomethingT, cell_tag>::type type;
    };

    template<typename SomethingT>
    struct facet_range
    {
      typedef typename facet_tag<SomethingT>::type cell_tag;
      typedef typename element_range<SomethingT, cell_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a vertex range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex_range
    {
      typedef typename element_range<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line range (same as edge_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct line_range
    {
      typedef typename element_range<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a edge range (same as line_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct edge_range
    {
      typedef typename element_range<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct triangle_range
    {
      typedef typename element_range<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct quadrilateral_range
    {
      typedef typename element_range<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct polygon_range
    {
      typedef typename element_range<SomethingT, polygon_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a PLC range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct plc_range
    {
      typedef typename element_range<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct tetrahedron_range
    {
      typedef typename element_range<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct hexahedron_range
    {
      typedef typename element_range<SomethingT, hexahedron_tag>::type type;
    };








    /** @brief Convenience metafunction for the type retrieval of a cell range. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_cell_range
    {
      typedef typename cell_tag<SomethingT>::type cell_tag;
      typedef typename element_range<SomethingT, cell_tag>::type type;
    };

    template<typename SomethingT>
    struct const_facet_range
    {
      typedef typename facet_tag<SomethingT>::type cell_tag;
      typedef typename element_range<SomethingT, cell_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a vertex range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_vertex_range
    {
      typedef typename element_range<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line range (same as edge_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_line_range
    {
      typedef typename element_range<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a edge range (same as line_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_edge_range
    {
      typedef typename element_range<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_triangle_range
    {
      typedef typename element_range<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_quadrilateral_range
    {
      typedef typename element_range<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_polygon_range
    {
      typedef typename element_range<SomethingT, polygon_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a PLC range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_plc_range
    {
      typedef typename element_range<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_tetrahedron_range
    {
      typedef typename element_range<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_hexahedron_range
    {
      typedef typename element_range<SomethingT, hexahedron_tag>::type type;
    };








    template<typename SomethingT>
    struct iterator
    {
      typedef typename SomethingT::iterator type;
    };

    template<typename SomethingT>
    struct const_iterator
    {
      typedef typename SomethingT::const_iterator type;
    };


  }


}

#endif
