#ifndef VIENNAGRID_REGION_HPP
#define VIENNAGRID_REGION_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/element/element.hpp"

namespace viennagrid
{


  template<bool is_const>
  class base_region
  {
    template<bool is_other_const>
    friend class base_region;

  public:

    typedef viennagrid_index id_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_type<mesh_hierarchy>::type const_mesh_hierarchy_reference;

    base_region() {}
    base_region(viennagrid_region region_in) : region_(region_in) {}

    template<bool is_other_const>
    base_region(base_region<is_other_const> const & rhs) : region_(rhs.region_) {}


    id_type id() const
    {
      id_type id_;
      viennagrid_region_get_id(region_, &id_);
      return id_;
    }

    std::string name() const
    {
      const char * name_;
      viennagrid_region_get_name(internal_region(), &name_);
      return std::string(name_);
    }

    void set_name(std::string const & name_)
    {
      viennagrid_region_set_name(internal_region(), name_.c_str());
    }

    mesh_hierarchy_type mesh_hierarchy()
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_region_get_mesh_hierarchy(region_, &tmp);
      return mesh_hierarchy_type(tmp);
    }

    const_mesh_hierarchy_reference mesh_hierarchy() const
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_region_get_mesh_hierarchy(region_, &tmp);
      return const_mesh_hierarchy_reference(tmp);
    }

    std::size_t dimension() const { return mesh_hierarchy().dimension(); }
    element_tag cell_tag() const { return mesh_hierarchy().cell_tag(); }

    viennagrid_region internal_region() { return region_; }

  private:

    viennagrid_region region_;
  };






  template<bool is_const>
  struct unpack_region_functor
  {
    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<region, is_const>::type value_type;
    typedef typename result_of::const_nonconst<region, true>::type const_value_type;

    typedef value_type reference;
    typedef value_type * pointer;

    unpack_region_functor(mesh_hierarchy_type mesh_hierarchy_in) : mesh_hierarchy_(mesh_hierarchy_in) {}

    value_type operator()(viennagrid_region & it) { return value_type(it); }
    const_value_type operator()(viennagrid_region const & it) { return const_value_type(it); }

    mesh_hierarchy_type mesh_hierarchy_;
  };



  template<bool is_const>
  class base_region_range
  {
  public:

    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, is_const>::type region_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, true>::type const_region_pointer_type;

    base_region_range(mesh_hierarchy_type mesh_hierarchy_in) : mesh_hierarchy_(mesh_hierarchy_in)
    {
      viennagrid_regions_get(mesh_hierarchy_.internal_mesh_hierarchy(), &begin_, &end_);
    }

    typedef transform_iterator<region_pointer_type, unpack_region_functor<is_const> > iterator;
    typedef transform_iterator<const_region_pointer_type, unpack_region_functor<true> > const_iterator;

    iterator begin() { return iterator(begin_, unpack_region_functor<is_const>(mesh_hierarchy_)); }
    iterator end() { return iterator(end_, unpack_region_functor<is_const>(mesh_hierarchy_)); }

    const_iterator cbegin() const { return const_iterator(begin_, unpack_region_functor<true>(mesh_hierarchy_)); }
    const_iterator cend() const { return const_iterator(end_, unpack_region_functor<true>(mesh_hierarchy_)); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

  private:
    mesh_hierarchy_type mesh_hierarchy_;
    region_pointer_type begin_;
    region_pointer_type end_;
  };







  inline void add( region region, element e )
  {
    viennagrid_add_to_region(e.mesh_hierarchy().internal_mesh_hierarchy(),
                             e.tag().internal_element_tag(),
                             e.id(),
                             region.internal_region() );
  }



  template<bool element_is_const, bool region_is_const>
  bool is_in_region( base_element<element_is_const> element, base_region<region_is_const> region )
  {
    viennagrid_region const * it;
    viennagrid_region const * end;

    viennagrid_get_regions(element.mesh_hierarchy().internal_mesh_hierarchy(),
                           element.tag().internal_element_tag(),
                           element.id(),
                           &it,
                           &end);

    for (; it != end; ++it)
    {
      viennagrid_index id_;
      viennagrid_region_get_id(*it, &id_);

      if (id_ == region.id())
        return true;
    }

    return false;
  }


























  template<bool is_const>
  class base_mesh_region
  {
    template<bool other_is_const>
    friend class base_mesh_region;

  public:

    typedef typename base_region<is_const>::id_type id_type;

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh, true>::type const_mesh_type;

    typedef typename result_of::const_nonconst<region, is_const>::type region_type;
    typedef typename result_of::const_nonconst<region, true>::type const_region_type;

    base_mesh_region(mesh_type mesh_in, region_type region_in) : mesh_(mesh_in), region_(region_in) {}

    template<bool other_is_const>
    base_mesh_region(base_mesh_region<other_is_const> const & rhs) : mesh_(rhs.mesh_), region_(rhs.region_) {}

    mesh_type mesh() { return mesh_; }
    const_mesh_type mesh() const { return mesh_; }

    region_type region() { return region_; }
    const_region_type region() const { return region_; }

    id_type id() const { return region().id(); }
    std::string name() const { return region().name(); }
    void set_name(std::string const & name_) { region().set_name(name_); }
    std::size_t dimension() const { return region().dimension(); }
    element_tag cell_tag() const { return region().cell_tag(); }

  private:

    mesh_type mesh_;
    region_type region_;
  };










  template<bool is_const>
  struct unpack_mesh_region_functor
  {
    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type value_type;
    typedef typename result_of::const_nonconst<mesh_region, true>::type const_value_type;

    typedef value_type reference;
    typedef value_type * pointer;

    unpack_mesh_region_functor(mesh_type mesh_in) : mesh_(mesh_in) {}

    value_type operator()(viennagrid_region & it) { return value_type(mesh_, it); }
    const_value_type operator()(viennagrid_region const & it) { return const_value_type(mesh_, it); }

    mesh_type mesh_;
  };


  template<bool is_const>
  class base_mesh_region_range
  {
  public:

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, is_const>::type region_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, true>::type const_region_pointer_type;

    base_mesh_region_range(mesh_type mesh_in) : mesh_(mesh_in)
    {
      viennagrid_regions_get(mesh_.mesh_hierarchy().internal_mesh_hierarchy(), &begin_, &end_);
    }

    typedef transform_iterator<region_pointer_type, unpack_mesh_region_functor<is_const> > iterator;
    typedef transform_iterator<const_region_pointer_type, unpack_mesh_region_functor<true> > const_iterator;

    iterator begin() { return iterator(begin_, unpack_mesh_region_functor<is_const>(mesh_)); }
    iterator end() { return iterator(end_, unpack_mesh_region_functor<is_const>(mesh_)); }

    const_iterator cbegin() const { return const_iterator(begin_, unpack_mesh_region_functor<true>(mesh_)); }
    const_iterator cend() const { return const_iterator(end_, unpack_mesh_region_functor<true>(mesh_)); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

  private:
    mesh_type mesh_;
    region_pointer_type begin_;
    region_pointer_type end_;
  };





  inline void add( mesh_region mr, element e )
  {
    add( mr.region(), e );
  }

  template<bool element_is_const, bool region_is_const>
  bool is_in_region( base_element<element_is_const> element, base_mesh_region<region_is_const> mesh_region )
  {
    return is_in_region(element, mesh_region.region() );
  }


  template<bool element_is_const, bool region_is_const>
  bool is_boundary( base_mesh_region<region_is_const> r, base_element<element_is_const> e )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_region(r.region().internal_region(), r.mesh().internal_mesh(), e.tag().internal_element_tag(), e.id(), &result);
    return result == VIENNAGRID_TRUE;
  }




  template<typename ElementT>
  typename result_of::point<ElementT>::type point(mesh_region r, ElementT vertex)
  {
    return r.mesh().point(vertex);
  }

  template<typename ElementT>
  typename result_of::const_point<ElementT>::type point(const_mesh_region r, ElementT vertex)
  {
    return r.mesh().point(vertex);
  }



}

#endif
