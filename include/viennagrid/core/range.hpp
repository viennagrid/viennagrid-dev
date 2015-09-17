#ifndef VIENNAGRID_RANGE_HPP
#define VIENNAGRID_RANGE_HPP

#include "viennagrid/core/forwards.hpp"
#include "viennagrid/core/utils.hpp"
#include "viennagrid/core/iterators.hpp"
#include "viennagrid/mesh/element.hpp"
#include "viennagrid/mesh/region.hpp"

namespace viennagrid
{

  template<bool is_const>
  struct unpack_element_functor
  {
    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;

    typedef typename result_of::const_nonconst<element, is_const>::type value_type;
    typedef typename result_of::const_nonconst<element, true>::type const_value_type;

    typedef typename result_of::const_nonconst<value_type *, is_const>::type pointer;
    typedef value_type reference;

    unpack_element_functor(viennagrid_mesh mesh_in) : mesh_(mesh_in) {}

    template<bool other_is_const>
    unpack_element_functor(unpack_element_functor<other_is_const> const & other) : mesh_(other.mesh_) {}


    value_type operator()(viennagrid_element_id & id) const { return value_type(mesh_, id); }
    const_value_type operator()(viennagrid_element_id const & id) const { return const_value_type(mesh_, id); }

    viennagrid_mesh mesh_;
  };




  template<typename ViewFunctorT, bool is_const>
  class base_element_range
  {
  public:
    typedef std::size_t size_type;

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh, true>::type const_mesh_type;

    typedef typename result_of::const_nonconst<viennagrid_element_id *, is_const>::type index_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_element_id *, true>::type const_index_pointer_type;

    typedef typename result_of::const_nonconst<element, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element, false>::type nonconst_element_type;
    typedef typename result_of::const_nonconst<element, true>::type const_element_type;


    base_element_range() : mesh_(0), element_index_begin(0), element_index_end(0) {}
    base_element_range(ViewFunctorT view_functor_) : mesh_(0), element_index_begin(0), element_index_end(0), view_functor(view_functor_) {}


    void from_mesh(viennagrid_mesh mesh, viennagrid_dimension topologic_dimension_in)
    {
      topologic_dimension_ = topologic_dimension_in;
      mesh_ = mesh;

      if ( !((topologic_dimension() == 0) || (topologic_dimension() == cell_dimension(get_mesh()))) )
        viennagrid_mesh_property_set(mesh_, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);

      viennagrid_mesh_elements_get(internal_mesh(),
                                   topologic_dimension(),
                                   const_cast<viennagrid_element_id **>(&element_index_begin),
                                   const_cast<viennagrid_element_id **>(&element_index_end));
    }

    template<bool element_is_const>
    void boundary_from_element(base_element<element_is_const> const & element, viennagrid_dimension topologic_dimension_in)
    {
      topologic_dimension_ = topologic_dimension_in;
      mesh_= element.internal_mesh();

      if (topologic_dimension() != 0)
        viennagrid_mesh_property_set(mesh_, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);

      viennagrid_element_boundary_elements(internal_mesh(),
                                           element.id().internal(),
                                           topologic_dimension(),
                                           const_cast<viennagrid_element_id **>(&element_index_begin),
                                           const_cast<viennagrid_element_id **>(&element_index_end));
    }

    template<bool element_is_const>
    void coboundary_from_element(viennagrid_mesh mesh, base_element<element_is_const> const & element, viennagrid_dimension coboundary_topological_dimension_in)
    {
      topologic_dimension_ = coboundary_topological_dimension_in;
      mesh_ = mesh;


      if ( !((viennagrid::topologic_dimension(element) == 0) && (coboundary_topological_dimension_in == cell_dimension(get_mesh()))) )
        viennagrid_mesh_property_set(internal_mesh(), VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);

      viennagrid_element_coboundary_elements(mesh,
                                             element.id().internal(),
                                             topologic_dimension(),
                                             const_cast<viennagrid_element_id **>(&element_index_begin),
                                             const_cast<viennagrid_element_id **>(&element_index_end));
    }

    template<bool element_is_const>
    void neighbor_from_element(viennagrid_mesh mesh, base_element<element_is_const> const & element, viennagrid_dimension connector_topological_dimension_in, viennagrid_dimension neighbor_topological_dimension_in)
    {
      topologic_dimension_ = neighbor_topological_dimension_in;
      mesh_ = mesh;

      if ( !((viennagrid::topologic_dimension(element) == cell_dimension(mesh_)) &&
            (connector_topological_dimension_in == 0) &&
            (neighbor_topological_dimension_in == cell_dimension(mesh_))) )
        viennagrid_mesh_property_set(internal_mesh(), VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);

      viennagrid_element_neighbor_elements(mesh,
                                           element.id().internal(),
                                           connector_topological_dimension_in,
                                           topologic_dimension(),
                                           const_cast<viennagrid_element_id **>(&element_index_begin),
                                           const_cast<viennagrid_element_id **>(&element_index_end));
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
                                         unpack_functor(internal_mesh())),
                      transform_iterator(element_index_end,
                                         unpack_functor(internal_mesh())),
                      view_functor);
    }
    iterator end()
    {
      return iterator(transform_iterator(element_index_end,
                                         unpack_functor(internal_mesh())),
                      view_functor);

    }


    const_iterator cbegin() const
    {
      return const_iterator(const_transform_iterator(element_index_begin,
                                                     const_unpack_functor(internal_mesh())),
                            const_transform_iterator(element_index_end,
                                                     const_unpack_functor(internal_mesh())),
                            view_functor);
    }
    const_iterator cend() const
    {
      return const_iterator(const_transform_iterator(element_index_end,
                                                     const_unpack_functor(internal_mesh())),
                            view_functor);

    }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }


    size_type size() const { return end()-begin(); }
    bool empty() const { return size() == 0; }

    element_type operator[](size_type pos)
    { return *viennagrid::advance(begin(), pos); }
    const_element_type operator[](size_type pos) const
    { return *viennagrid::advance(begin(), pos); }


    mesh_type get_mesh()
    {
      return mesh_type(mesh_);
    }
    const_mesh_type get_mesh() const
    {
      return const_mesh_type(mesh_);
    }

    viennagrid_mesh internal_mesh() const { return const_cast<viennagrid_mesh>(mesh_); }

    viennagrid_int topologic_dimension() const { return topologic_dimension_; }


    index_pointer_type begin_ptr() { return element_index_begin; }
    index_pointer_type end_ptr() { return element_index_end; }

  protected:
    viennagrid_int topologic_dimension_;
    viennagrid_mesh mesh_;
    index_pointer_type element_index_begin;
    index_pointer_type element_index_end;
    ViewFunctorT view_functor;
  };






  template<bool is_const>
  class mesh_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    mesh_element_range(mesh_type const & mesh, viennagrid_int element_topological_dimension_)
    { this->from_mesh(mesh.internal(), element_topological_dimension_); }

  private:
  };

  template<bool is_const>
  class mesh_cell_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    mesh_cell_range(mesh_type const & mesh)
    { this->from_mesh(mesh.internal(), viennagrid::cell_dimension(mesh)); }

  private:
  };

  template<bool is_const>
  class mesh_facet_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    mesh_facet_range(mesh_type const & mesh)
    { this->from_mesh(mesh.internal(), viennagrid::cell_dimension(mesh)-1); }

  private:
  };

  template<int topologic_dimension, bool is_const>
  class static_mesh_element_range : public mesh_element_range<is_const>
  {
  public:
    typedef typename mesh_element_range<is_const>::mesh_type mesh_type;
    typedef typename mesh_element_range<is_const>::element_type element_type;

    static_mesh_element_range(mesh_type const & mesh) : mesh_element_range<is_const>(mesh, topologic_dimension) {}

  private:
  };



  template<bool is_const>
  class boundary_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    boundary_element_range(element_type const & element, viennagrid_int boundary_topological_dimension)
    { this->boundary_from_element(element, boundary_topological_dimension); }

  private:
  };

  template<bool is_const>
  class boundary_facet_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    boundary_facet_range(element_type const & element)
    { this->boundary_from_element(element, viennagrid::topologic_dimension(element)-1); }

  private:
  };

  template<int boundary_topologic_dimension, bool is_const>
  class static_boundary_element_range : public boundary_element_range<is_const>
  {
  public:
    typedef typename boundary_element_range<is_const>::mesh_type mesh_type;
    typedef typename boundary_element_range<is_const>::element_type element_type;

    static_boundary_element_range(element_type const & element) : boundary_element_range<is_const>(element, boundary_topologic_dimension) {}

  private:
  };



  template<bool is_const>
  class coboundary_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    coboundary_element_range(mesh_type const & mesh, element_type const & element, viennagrid_int coboundary_topological_dimension_)
    { this->coboundary_from_element(mesh.internal(), element, coboundary_topological_dimension_); }

  private:
  };


  template<int coboundary_topologic_dimension, bool is_const>
  class static_coboundary_element_range : public coboundary_element_range<is_const>
  {
  public:
    typedef typename coboundary_element_range<is_const>::mesh_type mesh_type;
    typedef typename coboundary_element_range<is_const>::element_type element_type;

    static_coboundary_element_range(mesh_type const & mesh, element_type const & element) : coboundary_element_range<is_const>(mesh.internal(), element, coboundary_topologic_dimension) {}

  private:
  };



  template<bool is_const>
  class neighbor_element_range : public base_element_range<true_functor, is_const>
  {
  public:
    typedef typename base_element_range<true_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<true_functor, is_const>::element_type element_type;

    neighbor_element_range(mesh_type const & mesh, element_type const & element, viennagrid_int connector_topological_dimension_, viennagrid_int neighbor_topological_dimension_)
    { this->neighbor_from_element(mesh.internal(), element, connector_topological_dimension_, neighbor_topological_dimension_); }

  private:
  };


  template<int connector_topologic_dimension, int neighbor_topologic_dimension, bool is_const>
  class static_neighbor_element_range : public neighbor_element_range<is_const>
  {
  public:
    typedef typename neighbor_element_range<is_const>::mesh_type mesh_type;
    typedef typename neighbor_element_range<is_const>::element_type element_type;

    static_neighbor_element_range(mesh_type const & mesh, element_type const & element) : neighbor_element_range<is_const>(mesh, element, connector_topologic_dimension, neighbor_topologic_dimension) {}

  private:
  };









  struct region_view_functor
  {
    region_view_functor(const_region const & region_) : region(region_.internal()) {}

    bool operator()(element const & element) const
    { return is_in_region(region, element); }
    bool operator()(const_element const & element) const
    { return is_in_region(region, element); }

    viennagrid_region region;
  };


  template<bool is_const>
  class region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    region_element_range(mesh_region_type const & region, viennagrid_int element_topological_dimension_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->from_mesh(region.internal_mesh(), element_topological_dimension_); }

  private:
  };


  template<bool is_const>
  class region_cell_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    region_cell_range(mesh_region_type const & region) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->from_mesh(region.internal_mesh(), viennagrid::cell_dimension(region)); }

  private:
  };

  template<bool is_const>
  class region_facet_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    region_facet_range(mesh_region_type const & region) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->from_mesh(region.internal_mesh(), viennagrid::facet_dimension(region)); }

  private:
  };


  template<int topologic_dimension, bool is_const>
  class static_region_element_range : public region_element_range<is_const>
  {
  public:
    typedef typename region_element_range<is_const>::mesh_type mesh_type;
    typedef typename region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    static_region_element_range(mesh_region_type const & region) : region_element_range<is_const>(region, topologic_dimension) {}

  private:
  };




  template<bool is_const>
  class coboundary_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    coboundary_region_element_range(mesh_region_type const & region, element_type const & element, viennagrid_int coboundary_topological_dimension_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->coboundary_from_element(region.internal_mesh(), element, coboundary_topological_dimension_); }

  private:
  };


  template<int coboundary_topologic_dimension, bool is_const>
  class static_coboundary_region_element_range : public coboundary_region_element_range<is_const>
  {
  public:
    typedef typename coboundary_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename coboundary_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    static_coboundary_region_element_range(mesh_region_type const & region, element_type const & element) : coboundary_region_element_range<is_const>(region, element, coboundary_topologic_dimension) {}

  private:
  };


  template<bool is_const>
  class neighbor_region_element_range : public base_element_range<region_view_functor, is_const>
  {
  public:
    typedef typename base_element_range<region_view_functor, is_const>::mesh_type mesh_type;
    typedef typename base_element_range<region_view_functor, is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    neighbor_region_element_range(mesh_region_type const & region, element_type element, viennagrid_int connector_topological_dimension_, viennagrid_int neighbor_topological_dimension_) : base_element_range<region_view_functor, is_const>( region_view_functor(region) )
    { this->neighbor_from_element(region.internal_mesh(), element, connector_topological_dimension_, neighbor_topological_dimension_); }

  private:
  };


  template<int connector_topologic_dimension, int neighbor_topologic_dimension, bool is_const>
  class static_neighbor_region_element_range : public neighbor_region_element_range<is_const>
  {
  public:
    typedef typename neighbor_region_element_range<is_const>::mesh_type mesh_type;
    typedef typename neighbor_region_element_range<is_const>::element_type element_type;

    typedef typename result_of::const_nonconst<region, is_const>::type mesh_region_type;

    static_neighbor_region_element_range(mesh_region_type const & region, element_type const & element) : neighbor_region_element_range<is_const>(region, element, connector_topologic_dimension, neighbor_topologic_dimension) {}

  private:
  };









  namespace result_of
  {
    template<>
    struct element_range<viennagrid::mesh, -1>
    {
      typedef mesh_element_range<false> type;
    };

    template<int topologic_dimension>
    struct element_range<viennagrid::mesh, topologic_dimension>
    {
      typedef static_mesh_element_range<topologic_dimension, false> type;
    };


    template<bool mesh_is_const>
    struct const_element_range<base_mesh<mesh_is_const>, -1>
    {
      typedef mesh_element_range<true> type;
    };

    template<bool mesh_is_const, int topologic_dimension>
    struct const_element_range<base_mesh<mesh_is_const>, topologic_dimension>
    {
      typedef static_mesh_element_range<topologic_dimension, true> type;
    };




    template<>
    struct cell_range<viennagrid::mesh>
    {
      typedef mesh_cell_range<false> type;
    };

    template<bool is_const>
    struct const_cell_range< base_mesh<is_const> >
    {
      typedef mesh_cell_range<false> type;
    };

    template<>
    struct cell_range<viennagrid::const_mesh>
    {
      typedef const_cell_range<const_mesh>::type type;
    };


    template<>
    struct facet_range<viennagrid::mesh>
    {
      typedef mesh_facet_range<false> type;
    };

    template<bool is_const>
    struct const_facet_range< base_mesh<is_const> >
    {
      typedef mesh_facet_range<false> type;
    };

    template<>
    struct facet_range<viennagrid::const_mesh>
    {
      typedef const_facet_range<const_mesh>::type type;
    };











    template<bool is_const>
    struct const_element_range<base_element<is_const>, -1>
    {
      typedef boundary_element_range<true> type;
    };

    template<int topologic_dimension, bool is_const>
    struct const_element_range<base_element<is_const>, topologic_dimension>
    {
      typedef static_boundary_element_range<topologic_dimension, true> type;
    };


    template<>
    struct element_range<viennagrid::element, -1>
    {
      typedef boundary_element_range<false> type;
    };

    template<int topologic_dimension>
    struct element_range<viennagrid::element, topologic_dimension>
    {
      typedef static_boundary_element_range<topologic_dimension, false> type;
    };

    template<>
    struct element_range<viennagrid::const_element, -1>
    {
      typedef const_element_range<viennagrid::const_element>::type type;
    };

    template<int topologic_dimension>
    struct element_range<viennagrid::const_element, topologic_dimension>
    {
      typedef typename const_element_range<viennagrid::const_element, topologic_dimension>::type type;
    };



    template<>
    struct facet_range<viennagrid::element>
    {
      typedef boundary_facet_range<false> type;
    };

    template<bool is_const>
    struct const_facet_range< base_element<is_const> >
    {
      typedef boundary_facet_range<false> type;
    };

    template<>
    struct facet_range<viennagrid::const_element>
    {
      typedef const_facet_range<viennagrid::const_element>::type type;
    };









    template<bool is_const>
    struct const_element_range<base_region<is_const>, -1>
    {
      typedef region_element_range<true> type;
    };

    template<int topologic_dimension, bool is_const>
    struct const_element_range<base_region<is_const>, topologic_dimension>
    {
      typedef static_region_element_range<topologic_dimension, true> type;
    };


    template<>
    struct element_range<viennagrid::region, -1>
    {
      typedef region_element_range<false> type;
    };

    template<int topologic_dimension>
    struct element_range<viennagrid::region, topologic_dimension>
    {
      typedef static_region_element_range<topologic_dimension, false> type;
    };

    template<>
    struct element_range<viennagrid::const_region, -1>
    {
      typedef const_element_range<viennagrid::region>::type type;
    };

    template<int topologic_dimension>
    struct element_range<viennagrid::const_region, topologic_dimension>
    {
      typedef typename const_element_range<viennagrid::region, topologic_dimension>::type type;
    };



    template<>
    struct cell_range<viennagrid::region>
    {
      typedef region_cell_range<false> type;
    };

    template<bool is_const>
    struct const_cell_range< base_region<is_const> >
    {
      typedef region_cell_range<false> type;
    };

    template<>
    struct cell_range<viennagrid::const_region>
    {
      typedef const_cell_range<viennagrid::const_region>::type type;
    };


    template<>
    struct facet_range<viennagrid::region>
    {
      typedef region_facet_range<false> type;
    };

    template<bool is_const>
    struct const_facet_range< base_region<is_const> >
    {
      typedef region_facet_range<false> type;
    };

    template<>
    struct facet_range<viennagrid::const_region>
    {
      typedef const_facet_range<viennagrid::const_region>::type type;
    };







    template<>
    struct coboundary_range<viennagrid::mesh, -1>
    {
      typedef coboundary_element_range<false> type;
    };

    template<bool mesh_is_const>
    struct const_coboundary_range<base_mesh<mesh_is_const>, -1>
    {
      typedef coboundary_element_range<true> type;
    };


    template<int coboundary_topologic_dimension>
    struct coboundary_range<viennagrid::mesh, coboundary_topologic_dimension>
    {
      typedef static_coboundary_element_range<coboundary_topologic_dimension, false> type;
    };

    template<bool mesh_is_const, int coboundary_topologic_dimension>
    struct const_coboundary_range<base_mesh<mesh_is_const>, coboundary_topologic_dimension>
    {
      typedef static_coboundary_element_range<coboundary_topologic_dimension, true> type;
    };


    template<>
    struct coboundary_range<viennagrid::region, -1>
    {
      typedef coboundary_region_element_range<false> type;
    };

    template<bool mesh_is_const>
    struct const_coboundary_range<base_region<mesh_is_const>, -1>
    {
      typedef coboundary_region_element_range<true> type;
    };

    template<int coboundary_topologic_dimension>
    struct coboundary_range<viennagrid::region, coboundary_topologic_dimension>
    {
      typedef static_coboundary_region_element_range<coboundary_topologic_dimension, false> type;
    };

    template<int coboundary_topologic_dimension>
    struct const_coboundary_range<viennagrid::region, coboundary_topologic_dimension>
    {
      typedef static_coboundary_region_element_range<coboundary_topologic_dimension, true> type;
    };





    template<>
    struct const_neighbor_range<viennagrid::mesh, -1, -1>
    {
      typedef neighbor_element_range<true> type;
    };

    template<>
    struct const_neighbor_range<viennagrid::const_mesh, -1, -1>
    {
      typedef neighbor_element_range<true> type;
    };

    template<>
    struct neighbor_range<viennagrid::mesh, -1, -1>
    {
      typedef neighbor_element_range<false> type;
    };

    template<>
    struct neighbor_range<viennagrid::const_mesh, -1, -1>
    {
      typedef const_neighbor_range<const_mesh>::type type;
    };





    template<int connector_topologic_dimension, int neighbor_topologic_dimension>
    struct neighbor_range<viennagrid::mesh, connector_topologic_dimension, neighbor_topologic_dimension>
    {
      typedef static_neighbor_element_range<connector_topologic_dimension, neighbor_topologic_dimension, false> type;
    };

    template<int connector_topologic_dimension, int neighbor_topologic_dimension>
    struct const_neighbor_range<viennagrid::mesh, connector_topologic_dimension, neighbor_topologic_dimension>
    {
      typedef static_neighbor_element_range<connector_topologic_dimension, neighbor_topologic_dimension, true> type;
    };






    template<bool mesh_region_is_const>
    struct const_neighbor_range<base_region<mesh_region_is_const>, -1>
    {
      typedef neighbor_region_element_range<false> type;
    };

    template<>
    struct neighbor_range<viennagrid::region, -1>
    {
      typedef neighbor_region_element_range<false> type;
    };

    template<>
    struct neighbor_range<viennagrid::const_region, -1>
    {
      typedef const_neighbor_range<viennagrid::const_region>::type type;
    };




    template<int connector_topologic_dimension, int neighbor_topologic_dimension>
    struct neighbor_range<viennagrid::region, connector_topologic_dimension, neighbor_topologic_dimension>
    {
      typedef static_neighbor_region_element_range<connector_topologic_dimension, neighbor_topologic_dimension, false> type;
    };

    template<int connector_topologic_dimension, int neighbor_topologic_dimension>
    struct const_neighbor_range<viennagrid::region, connector_topologic_dimension, neighbor_topologic_dimension>
    {
      typedef static_neighbor_region_element_range<connector_topologic_dimension, neighbor_topologic_dimension, true> type;
    };

  }



  template<typename SomethingT>
  typename viennagrid::result_of::element_range<SomethingT>::type elements(SomethingT & something, viennagrid_int topologic_dimension)
  { return typename viennagrid::result_of::element_range<SomethingT>::type(something, topologic_dimension); }

  template<int topologic_dimension, typename SomethingT>
  typename viennagrid::result_of::element_range<SomethingT, topologic_dimension>::type elements(SomethingT &  something)
  { return typename viennagrid::result_of::element_range<SomethingT, topologic_dimension>::type(something); }


  template<typename SomethingT>
  typename viennagrid::result_of::const_element_range<SomethingT>::type elements(SomethingT const & something, viennagrid_int topologic_dimension)
  { return typename viennagrid::result_of::const_element_range<SomethingT>::type(something, topologic_dimension); }

  template<int topologic_dimension, typename SomethingT>
  typename viennagrid::result_of::const_element_range<SomethingT, topologic_dimension>::type elements(SomethingT const &  something)
  { return typename viennagrid::result_of::const_element_range<SomethingT, topologic_dimension>::type(something); }





  template<typename SomethingT, typename ElementT>
  typename viennagrid::result_of::const_coboundary_range<SomethingT>::type coboundary_elements(SomethingT const & something, ElementT const & element, viennagrid_int topologic_dimension)
  { return typename viennagrid::result_of::const_coboundary_range<SomethingT>::type(something, element, topologic_dimension); }

  template<typename SomethingT, typename ElementT>
  typename viennagrid::result_of::coboundary_range<SomethingT>::type coboundary_elements(SomethingT & something, ElementT & element, viennagrid_int topologic_dimension)
  { return typename viennagrid::result_of::coboundary_range<SomethingT>::type(something, element, topologic_dimension); }



  /** @brief Function for retrieving a cell range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A cell range
    */
  template<typename SomethingT>
  typename result_of::cell_range<SomethingT>::type cells(SomethingT & something)
  { return typename result_of::cell_range<SomethingT>::type(something); }

  template<typename SomethingT>
  typename result_of::const_cell_range<SomethingT>::type cells(SomethingT const & something)
  { return typename result_of::const_cell_range<SomethingT>::type(something); }


  /** @brief Function for retrieving a facet range object from a host object
    *
    * @tparam ElementT           The host element (cell)
    * @param  element            The host object of type ElementT
    * @return                    A facet range
    */
  template<typename ElementT>
  typename result_of::facet_range<ElementT>::type facets(ElementT & element)
  { return typename result_of::facet_range<ElementT>::type(element); }

  template<typename ElementT>
  typename result_of::const_facet_range<ElementT>::type facets(ElementT const & element)
  { return typename result_of::const_facet_range<ElementT>::type(element); }

  /** @brief Function for retrieving a vertex range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a mesh, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A vertex range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT>::type vertices(SomethingT & something)
  { return typename result_of::element_range<SomethingT>::type(something,0); }

  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT>::type vertices(SomethingT const & something)
  { return typename result_of::const_element_range<SomethingT>::type(something,0); }

}

#endif
