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



  template<viennagrid_dimension d>
  struct dimension_tag
  {
    static const viennagrid_dimension value = d;
  };

  class point;
  typedef std::pair<point, viennagrid_int> seed_point;


  template<bool is_const = false>
  class base_region;

  typedef base_region<false> region;
  typedef base_region<true> const_region;




  template<bool is_const = false>
  class base_element;

  typedef base_element<false> element;
  typedef base_element<true> const_element;



  template<bool is_const = false>
  class base_mesh;

  typedef base_mesh<false>       mesh;
  typedef base_mesh<true>  const_mesh;







  template<typename ViewFunctorT = true_functor, bool is_const = false>
  class base_element_range;




  template<bool is_const>
  class base_region_range;

  typedef base_region_range<false> region_range_t;
  typedef base_region_range<true> const_region_range_t;

  class null_type;






  template<viennagrid_element_type et>
  class static_element_tag;



  class element_tag
  {
    template<bool is_const>
    friend class base_element;

    template<viennagrid_element_type et>
    friend class static_element_tag;

  public:


    template<viennagrid_element_type et>
    element_tag( static_element_tag<et> ) : element_tag_(et) {}


    element_tag() : element_tag_(VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT) {}

    viennagrid_element_type internal() const { return element_tag_; }

    static element_tag vertex() { return element_tag(VIENNAGRID_ELEMENT_TYPE_VERTEX); }
    static element_tag line() { return element_tag(VIENNAGRID_ELEMENT_TYPE_LINE); }
    static element_tag edge() { return element_tag(VIENNAGRID_ELEMENT_TYPE_EDGE); }
    static element_tag triangle() { return element_tag(VIENNAGRID_ELEMENT_TYPE_TRIANGLE); }
    static element_tag quadrilateral() { return element_tag(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL); }
    static element_tag polygon() { return element_tag(VIENNAGRID_ELEMENT_TYPE_POLYGON); }
    static element_tag tetrahedron() { return element_tag(VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON); }
    static element_tag hexahedron() { return element_tag(VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON); }

    bool valid() const { return internal() != VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT; }
    bool is_vertex() const { return *this == vertex(); }
    bool is_line() const { return *this == line(); }
    bool is_edge() const { return *this == edge(); }
    bool is_triangle() const { return *this == triangle(); }
    bool is_quadrilateral() const { return *this == quadrilateral(); }
    bool is_polygon() const { return *this == polygon(); }
    bool is_tetrahedron() const { return *this == tetrahedron(); }
    bool is_hexahedron() const { return *this == hexahedron(); }

    bool is_simplex() const { return viennagrid_is_simplex(internal()) == VIENNAGRID_TRUE; }

    bool operator==(element_tag rhs) const { return internal() == rhs.internal();}
    bool operator!=(element_tag rhs) const { return !(*this == rhs);}

    bool operator<(element_tag rhs) const { return internal() < rhs.internal();}
    bool operator<=(element_tag rhs) const { return !(rhs < *this);}
    bool operator>(element_tag rhs) const { return rhs < *this;}
    bool operator>=(element_tag rhs) const { return !(*this < rhs);}

    viennagrid_dimension topologic_dimension() const
    { return viennagrid_topological_dimension(internal()); }

    bool is_boundary(element_tag host_tag) const
    { return viennagrid_is_boundary_type(host_tag.internal(), internal()) == VIENNAGRID_TRUE; }

    viennagrid_int boundary_element_count(element_tag boundary_tag) const
    { return viennagrid_boundary_element_count_from_element_type( internal(), boundary_tag.internal() ); }
    viennagrid_int vertex_count() const { return boundary_element_count( vertex() ); }

    std::string name() const
    {
      const char * tmp = viennagrid_element_type_string(internal());
      if (tmp)
        return tmp;
      return std::string();
    }

    static element_tag from_internal(viennagrid_element_type element_tag_in)
    { return element_tag(element_tag_in); }

  private:
    explicit element_tag(viennagrid_element_type element_tag_in) : element_tag_(element_tag_in) {}

    viennagrid_element_type element_tag_;
  };




  template<viennagrid_element_type et>
  class static_element_tag : public element_tag
  {
  public:
    static_element_tag() : element_tag(et) {}
  };

  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_VERTEX> vertex_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_LINE> line_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_EDGE> edge_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_TRIANGLE> triangle_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL> quadrilateral_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_POLYGON> polygon_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON> tetrahedron_tag;
  typedef static_element_tag<VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON> hexahedron_tag;






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

    template<bool is_const>
    struct const_type< base_region<is_const> >
    {
      typedef base_region<true> type;
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
    struct nonconst_type< base_region<is_const> >
    {
      typedef base_region<false> type;
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
      typedef element_tag type;
    };

    template<viennagrid_element_type et>
    struct element_tag< static_element_tag<et> >
    {
      typedef static_element_tag<et> type;
    };





    template<typename SomethingT>
    struct region;

    template<>
    struct region<mesh>
    {
      typedef viennagrid::region type;
    };

    template<>
    struct region<const_mesh>
    {
      typedef viennagrid::const_region type;
    };

    template<typename SomethingT>
    struct region_id
    {
      typedef typename id< typename region<SomethingT>::type >::type type;
    };


    template<typename SomethingT>
    struct const_region;

    template<bool is_const>
    struct const_region< base_mesh<is_const> >
    {
      typedef const_region type;
    };





    template<typename SomethingT>
    struct region_range;

    template<typename SomethingT>
    struct const_region_range;





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




    template<typename SomethingT>
    struct mesh;

    template<bool is_const>
    struct mesh< base_mesh<is_const> >
    {
      typedef base_mesh<is_const> type;
    };

    template<bool is_const>
    struct mesh< base_element<is_const> >
    {
      typedef base_mesh<is_const> type;
    };

    template<bool is_const>
    struct mesh< base_region<is_const> >
    {
      typedef base_mesh<is_const> type;
    };



    template<typename SomethingT, typename ElementT = null_type>
    struct point;

    template<typename SomethingT, typename ElementT>
    struct point
    {
      typedef typename point<
        typename mesh<SomethingT>::type,
        typename element<ElementT>::type
      >::type type;
    };

    template<typename SomethingT>
    struct point<SomethingT, null_type>
    {
      typedef typename point< typename mesh<SomethingT>::type, null_type >::type type;
    };

    template<typename SomethingT>
    struct point<SomethingT, viennagrid::point>
    {
      typedef viennagrid::point type;
    };




    template<bool is_const>
    struct point< base_mesh<is_const>, null_type >
    {
      typedef viennagrid::point type;
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
