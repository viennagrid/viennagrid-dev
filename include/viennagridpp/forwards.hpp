#ifndef VIENNAGRID_FORWARDS_HPP
#define VIENNAGRID_FORWARDS_HPP

#include <string>
#include "viennagrid/viennagrid.h"

namespace viennagrid
{
  /** @brief A trivial functor which always returns true */
  struct true_functor
  {
    typedef bool result_type;

    template<typename ValueT>
    bool operator()(ValueT) const
    { return true; }
  };



  template<int d>
  struct dimension_tag
  {
    static const int value = d;
  };

  class point_t;
  typedef std::pair<point_t, int> seed_point_t;


  template<bool is_const = false>
  class base_region;

  typedef base_region<false> region_t;
  typedef base_region<true> const_region_t;


  template<bool is_const = false>
  class base_mesh_region;

  typedef base_mesh_region<false> mesh_region_t;
  typedef base_mesh_region<true> const_mesh_region_t;





  template<bool is_const = false>
  class base_element;

  typedef base_element<false> element_t;
  typedef base_element<true> const_element_t;



  template<bool is_const = false>
  class base_mesh;

  typedef base_mesh<false>       mesh_t;
  typedef base_mesh<true>  const_mesh_t;


  template<bool is_const = false>
  class base_mesh_hierarchy;

  typedef base_mesh_hierarchy<false> mesh_hierarchy_t;
  typedef base_mesh_hierarchy<true> const_mesh_hierarchy_t;








  template<typename ViewFunctorT = true_functor, bool is_const = false>
  class base_element_range;




  template<bool is_const>
  class base_region_range;

  typedef base_region_range<false> region_range_t;
  typedef base_region_range<true> const_region_range_t;


  template<bool is_const>
  class base_mesh_region_range;

  typedef base_mesh_region_range<false> mesh_region_range_t;
  typedef base_mesh_region_range<true> const_mesh_region_range_t;







  template<bool is_const>
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_mesh<is_const> const & mesh );
  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_mesh<is_const> const & mesh );

  template<bool is_const>
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_region<is_const> const & region );
  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_region<is_const> const & region );

  template<bool is_const>
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_mesh_region<is_const> const & region );
  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_mesh_region<is_const> const & region );

  template<bool is_const>
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_element<is_const> const & element );
  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_element<is_const> const & element );




  class null_type;






  template<viennagrid_element_tag et>
  class static_tag_t;



  class element_tag_t
  {
    template<bool is_const>
    friend class base_element;

    template<bool is_const>
    friend class base_mesh_hierarchy;

    template<viennagrid_element_tag et>
    friend class static_tag_t;

  public:


    template<viennagrid_element_tag et>
    element_tag_t( static_tag_t<et> ) : element_tag_(et) {}


    element_tag_t() : element_tag_(VIENNAGRID_ELEMENT_TAG_NO_ELEMENT) {}

    viennagrid_element_tag internal() const { return element_tag_; }

    static element_tag_t vertex() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_VERTEX); }
    static element_tag_t line() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_LINE); }
    static element_tag_t edge() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_EDGE); }
    static element_tag_t triangle() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_TRIANGLE); }
    static element_tag_t quadrilateral() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL); }
    static element_tag_t polygon() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_POLYGON); }
    static element_tag_t tetrahedron() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON); }
    static element_tag_t hexahedron() { return element_tag_t(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON); }

    bool valid() const { return internal() != VIENNAGRID_ELEMENT_TAG_NO_ELEMENT; }
    bool is_vertex() const { return *this == vertex(); }
    bool is_line() const { return *this == line(); }
    bool is_edge() const { return *this == edge(); }
    bool is_triangle() const { return *this == triangle(); }
    bool is_quadrilateral() const { return *this == quadrilateral(); }
    bool is_polygon() const { return *this == polygon(); }
    bool is_tetrahedron() const { return *this == tetrahedron(); }
    bool is_hexahedron() const { return *this == hexahedron(); }

    bool is_simplex() const { return viennagrid_is_simplex(internal()) == VIENNAGRID_TRUE; }

    bool operator==(element_tag_t rhs) const { return internal() == rhs.internal();}
    bool operator!=(element_tag_t rhs) const { return !(*this == rhs);}

    bool operator<(element_tag_t rhs) const { return internal() < rhs.internal();}
    bool operator<=(element_tag_t rhs) const { return !(rhs < *this);}
    bool operator>(element_tag_t rhs) const { return rhs < *this;}
    bool operator>=(element_tag_t rhs) const { return !(*this < rhs);}

    viennagrid_dimension topologic_dimension() const
    { return viennagrid_topological_dimension(internal()); }


    bool is_boundary(element_tag_t host_tag) const
    { return viennagrid_is_boundary_tag(host_tag.internal(), internal()) == VIENNAGRID_TRUE; }

    viennagrid_int boundary_element_count(element_tag_t boundary_tag) const
    { return viennagrid_boundary_element_count_from_element_tag( internal(), boundary_tag.internal() ); }
    viennagrid_int vertex_count() const { return boundary_element_count( vertex() ); }

    std::string name() const { return std::string( viennagrid_element_tag_string(internal()) ); }

    static element_tag_t from_internal(viennagrid_element_tag element_tag_in)
    { return element_tag_t(element_tag_in); }

  private:
    element_tag_t(viennagrid_element_tag element_tag_in) : element_tag_(element_tag_in) {}

    viennagrid_element_tag element_tag_;
  };




  template<viennagrid_element_tag et>
  class static_tag_t : public element_tag_t
  {
  public:
    static_tag_t() : element_tag_t(et) {}
  };

  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_VERTEX> vertex_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_LINE> line_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_EDGE> edge_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_TRIANGLE> triangle_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_QUADRILATERAL> quadrilateral_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_POLYGON> polygon_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_TETRAHEDRON> tetrahedron_tag;
  typedef static_tag_t<VIENNAGRID_ELEMENT_TAG_HEXAHEDRON> hexahedron_tag;






  namespace detail
  {
    template<typename ElementTagT>
    struct element_refinement;
  }






  namespace result_of
  {
    /** @brief Metafunction for obtaining the id type of an element
     *
     * @tparam ElementT           The host element type
     */
    template<typename SomethingT>
    struct id
    {
      typedef typename SomethingT::id_type type;
    };





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

//     template<bool is_const>
//     struct const_type< base_point<is_const> >
//     {
//       typedef base_point<true> type;
//     };

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

//     template<bool is_const>
//     struct nonconst_type< base_point<is_const> >
//     {
//       typedef base_point<false> type;
//     };

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
    struct element_tag
    {
      typedef element_tag_t type;
    };

    template<viennagrid_element_tag et>
    struct element_tag< static_tag_t<et> >
    {
      typedef static_tag_t<et> type;
    };





    template<typename SomethingT>
    struct region;

    template<>
    struct region<mesh_hierarchy_t>
    {
      typedef region_t type;
    };

    template<>
    struct region<mesh_t>
    {
      typedef mesh_region_t type;
    };

    template<>
    struct region<const_mesh_hierarchy_t>
    {
      typedef const_region_t type;
    };

    template<>
    struct region<const_mesh_t>
    {
      typedef const_mesh_region_t type;
    };

    template<typename SomethingT>
    struct region_id
    {
      typedef typename id< typename region<SomethingT>::type >::type type;
    };


    template<typename SomethingT>
    struct const_region;

    template<bool is_const>
    struct const_region< base_mesh_hierarchy<is_const> >
    {
      typedef const_region_t type;
    };

    template<bool is_const>
    struct const_region< base_mesh<is_const> >
    {
      typedef const_mesh_region_t type;
    };





    template<typename SomethingT, typename ElementT = null_type>
    struct region_range;

    template<typename SomethingT, typename ElementT = null_type>
    struct const_region_range;





    template<typename SomethingT, typename ElementTagT = null_type>
    struct element
    {
      typedef viennagrid::element_t type;
    };

    template<typename SomethingT, typename ElementTagT = null_type>
    struct const_element
    {
      typedef viennagrid::const_element_t type;
    };


    template<typename SomethingT>
    struct mesh_hierarchy;

    template<bool is_const>
    struct mesh_hierarchy< base_mesh_hierarchy<is_const> >
    {
      typedef base_mesh_hierarchy<is_const> type;
    };

    template<bool is_const>
    struct mesh_hierarchy< base_mesh<is_const> >
    {
      typedef base_mesh_hierarchy<is_const> type;
    };

    template<bool is_const>
    struct mesh_hierarchy< base_region<is_const> >
    {
      typedef base_mesh_hierarchy<is_const> type;
    };

    template<bool is_const>
    struct mesh_hierarchy< base_mesh_region<is_const> >
    {
      typedef base_mesh_hierarchy<is_const> type;
    };

    template<bool is_const>
    struct mesh_hierarchy< base_element<is_const> >
    {
      typedef base_mesh_hierarchy<is_const> type;
    };




    template<typename SomethingT>
    struct mesh
    {
      typedef typename mesh< typename mesh_hierarchy<SomethingT>::type >::type type;
    };

    template<bool is_const>
    struct mesh< base_mesh_hierarchy<is_const> >
    {
      typedef base_mesh<is_const> type;
    };



    template<typename SomethingT, typename ElementT = null_type>
    struct point;

    template<typename SomethingT, typename ElementT>
    struct point
    {
      typedef typename point<
        typename mesh_hierarchy<SomethingT>::type,
        typename element<ElementT>::type
      >::type type;
    };

    template<typename SomethingT>
    struct point<SomethingT, null_type>
    {
      typedef typename point< typename mesh_hierarchy<SomethingT>::type, null_type >::type type;
    };

    template<typename SomethingT>
    struct point<SomethingT, point_t>
    {
      typedef point_t type;
    };

//     template<typename SomethingT, typename ElementT = null_type>
//     struct const_point
//     {
//       typedef typename const_type< typename point<SomethingT>::type, ElementT >::type type;
//     };




    template<bool is_const>
    struct point< base_mesh_hierarchy<is_const>, null_type >
    {
      typedef point_t type;
    };


    template<typename SomethingT, typename ElementT = null_type>
    struct coord
    {
      typedef typename coord<typename point<SomethingT, ElementT>::type>::type type;
    };





    template<typename SomethingT, int topologic_dimension = -1>
    struct element_range;

    template<typename SomethingT, int topologic_dimension = -1>
    struct const_element_range;

    template<typename SomethingT, int topologic_dimension>
    struct element_range<const SomethingT, topologic_dimension>
    {
      typedef typename const_element_range<SomethingT, topologic_dimension>::type type;
    };


    template<typename SomethingT, int coboundary_topologic_dimension = -1>
    struct coboundary_range;

    template<typename SomethingT, int coboundary_topologic_dimension = -1>
    struct const_coboundary_range;



    template<typename SomethingT, int connector_topologic_dimension = -1, int neighbor_topologic_dimension = -1>
    struct neighbor_range;

    template<typename SomethingT, int connector_topologic_dimension = -1, int neighbor_topologic_dimension = -1>
    struct const_neighbor_range;





    /** @brief Convenience metafunction for the type retrieval of a vertex ID
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct element_id
    {
      typedef typename id< typename element<SomethingT>::type >::type type;
    };


    /** @brief Convenience metafunction for the type retrieval of a cell range. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cell_range;

    template<typename SomethingT>
    struct facet_range;

    /** @brief Convenience metafunction for the type retrieval of a vertex range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex_range
    {
      typedef typename element_range<SomethingT, 0>::type type;
    };




    /** @brief Convenience metafunction for the type retrieval of a cell range. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_cell_range;

    template<typename SomethingT>
    struct const_facet_range;


    /** @brief Convenience metafunction for the type retrieval of a vertex range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a mesh, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_vertex_range
    {
      typedef typename const_element_range<SomethingT, 0>::type type;
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
