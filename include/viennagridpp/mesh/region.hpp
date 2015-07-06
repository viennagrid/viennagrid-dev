#ifndef VIENNAGRID_REGION_HPP
#define VIENNAGRID_REGION_HPP

#include "viennagridpp/mesh/mesh.hpp"
#include "viennagridpp/element/element.hpp"

namespace viennagrid
{


  template<bool is_const>
  class base_region
  {
    template<bool is_other_const>
    friend class base_region;

  public:

    typedef viennagrid_int id_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_type<mesh_hierarchy>::type const_mesh_hierarchy_type;


    base_region() {}
    base_region(viennagrid_region region_in) : region_(region_in) {}

    template<bool is_other_const>
    base_region(base_region<is_other_const> const & rhs) : region_(rhs.region_) {}


    id_type id() const
    {
      id_type id_;
      viennagrid_region_id_get(region_, &id_);
      return id_;
    }

    std::string name() const
    {
      const char * name_;
      viennagrid_region_name_get(internal(), &name_);
      return std::string(name_);
    }

    void set_name(std::string const & name_)
    {
      viennagrid_region_name_set(internal(), name_.c_str());
    }

    mesh_hierarchy_type mesh_hierarchy()
    {
      return mesh_hierarchy_type( internal_mesh_hierarchy() );
    }

    const_mesh_hierarchy_type mesh_hierarchy() const
    {
      return const_mesh_hierarchy_type( internal_mesh_hierarchy() );
    }

    viennagrid_mesh_hierarchy internal_mesh_hierarchy() const
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_region_mesh_hierarchy_get(region_, &tmp);
      return tmp;
    }

    std::size_t geometric_dimension() const { return mesh_hierarchy().geometric_dimension(); }
    element_tag cell_tag() const { return mesh_hierarchy().cell_tag(); }

    viennagrid_region internal() const { return const_cast<viennagrid_region>(region_); }

  private:

    viennagrid_region region_;
  };






  template<bool is_const>
  struct unpack_region_functor
  {
    typedef typename result_of::const_nonconst<region, is_const>::type value_type;
    typedef typename result_of::const_nonconst<region, true>::type const_value_type;

    typedef value_type reference;
    typedef value_type * pointer;

    unpack_region_functor() {}

    value_type operator()(viennagrid_region & it) { return value_type(it); }
    const_value_type operator()(viennagrid_region const & it) { return const_value_type(it); }
  };



  template<bool is_const>
  class base_region_range
  {
  public:

    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<element, is_const>::type element_type;

    typedef typename result_of::const_nonconst<viennagrid_region *, is_const>::type region_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, true>::type const_region_pointer_type;

    base_region_range(mesh_hierarchy_type const & mesh_hierarchy_in)
    {
      viennagrid_mesh_hierarchy_regions_get(internal_mesh_hierarchy(mesh_hierarchy_in),
                                            const_cast<viennagrid_region **>(&begin_),
                                            const_cast<viennagrid_region **>(&end_));
    }

    base_region_range(element_type const & element_)
    {
      viennagrid_element_regions_get(internal_mesh_hierarchy(element_),
                                     viennagrid::topologic_dimension(element_),
                                     element_.id(),
                                     const_cast<viennagrid_region **>(&begin_),
                                     const_cast<viennagrid_region **>(&end_));
    }

    typedef transform_iterator<region_pointer_type, unpack_region_functor<is_const> > iterator;
    typedef transform_iterator<const_region_pointer_type, unpack_region_functor<true> > const_iterator;

    iterator begin() { return iterator(begin_, unpack_region_functor<is_const>()); }
    iterator end() { return iterator(end_, unpack_region_functor<is_const>()); }

    const_iterator cbegin() const { return const_iterator(begin_, unpack_region_functor<true>()); }
    const_iterator cend() const { return const_iterator(end_, unpack_region_functor<true>()); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

    std::size_t size() const { return end() - begin(); }

  private:
    region_pointer_type begin_;
    region_pointer_type end_;
  };


  namespace result_of
  {
    template<bool mesh_hierarchy_is_const>
    struct region_range< base_mesh_hierarchy<mesh_hierarchy_is_const>, null_type >
    {
      typedef base_region_range<mesh_hierarchy_is_const> type;
    };

    template<bool mesh_hierarchy_is_const, bool element_is_const>
    struct region_range< base_mesh_hierarchy<mesh_hierarchy_is_const>, base_element<element_is_const> >
    {
      typedef base_region_range<true> type;
    };

    template<>
    struct region_range< base_mesh_hierarchy<false>, base_element<false> >
    {
      typedef base_region_range<false> type;
    };

    template<bool is_const, typename ElementT>
    struct const_region_range< base_mesh_hierarchy<is_const>, ElementT >
    {
      typedef base_region_range<true> type;
    };
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

    base_mesh_region(viennagrid_mesh mesh_in, viennagrid_region region_in) : mesh_(mesh_in), region_(region_in) {}

    template<bool other_is_const>
    base_mesh_region(base_mesh_region<other_is_const> const & rhs) : mesh_(rhs.mesh_), region_(rhs.region_) {}

    viennagrid_mesh internal_mesh() const { return const_cast<viennagrid_mesh>(mesh_); }
    mesh_type mesh() { return mesh_type(mesh_); }
    const_mesh_type mesh() const { return const_mesh_type(mesh_); }

    viennagrid_region internal_region() const { return const_cast<viennagrid_region>(region_); }
    region_type region() { return region_type(region_); }
    const_region_type region() const { return const_region_type(region_); }

    viennagrid_mesh_hierarchy internal_mesh_hierarchy() const
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_region_mesh_hierarchy_get(region_, &tmp);
      return tmp;
    }

    id_type id() const { return region().id(); }
    std::string name() const { return region().name(); }
    void set_name(std::string const & name_) { region().set_name(name_); }
    std::size_t geometric_dimension() const { return region().geometric_dimension(); }
    element_tag cell_tag() const { return region().cell_tag(); }

  private:

    viennagrid_mesh mesh_;
    viennagrid_region region_;
  };










  template<bool is_const>
  struct unpack_mesh_region_functor
  {
    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type value_type;
    typedef typename result_of::const_nonconst<mesh_region, true>::type const_value_type;

    typedef value_type reference;
    typedef value_type * pointer;

    unpack_mesh_region_functor(viennagrid_mesh mesh_in) : mesh_(mesh_in) {}

    value_type operator()(viennagrid_region & it) { return value_type(mesh_, it); }
    const_value_type operator()(viennagrid_region const & it) { return const_value_type(mesh_, it); }

    viennagrid_mesh mesh_;
  };


  template<bool is_const>
  class base_mesh_region_range
  {
  public:

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, is_const>::type region_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_region *, true>::type const_region_pointer_type;

    base_mesh_region_range(mesh_type const & mesh_in) : mesh_(mesh_in.internal())
    {
      viennagrid_mesh_hierarchy_regions_get(internal_mesh_hierarchy(mesh_in),
                                            const_cast<viennagrid_region **>(&begin_),
                                            const_cast<viennagrid_region **>(&end_));
    }

    base_mesh_region_range(mesh_type const & mesh_in, element const & element_) : mesh_(mesh_in.internal())
    {
      viennagrid_element_regions_get(internal_mesh_hierarchy(mesh_in),
                                     viennagrid::topologic_dimension(element_),
                                     element_.id(),
                                     const_cast<viennagrid_region **>(&begin_),
                                     const_cast<viennagrid_region **>(&end_));
    }

    typedef transform_iterator<region_pointer_type, unpack_mesh_region_functor<is_const> > iterator;
    typedef transform_iterator<const_region_pointer_type, unpack_mesh_region_functor<true> > const_iterator;

    iterator begin() { return iterator(begin_, unpack_mesh_region_functor<is_const>(mesh_)); }
    iterator end() { return iterator(end_, unpack_mesh_region_functor<is_const>(mesh_)); }

    const_iterator cbegin() const { return const_iterator(begin_, unpack_mesh_region_functor<true>(mesh_)); }
    const_iterator cend() const { return const_iterator(end_, unpack_mesh_region_functor<true>(mesh_)); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

    std::size_t size() const { return end() - begin(); }

  private:
    viennagrid_mesh mesh_;
    region_pointer_type begin_;
    region_pointer_type end_;
  };



  namespace result_of
  {
    template<bool mesh_is_const>
    struct region_range< base_mesh<mesh_is_const>, null_type >
    {
      typedef base_mesh_region_range<mesh_is_const> type;
    };

    template<bool mesh_is_const, bool element_is_const>
    struct region_range< base_mesh<mesh_is_const>, base_element<element_is_const> >
    {
      typedef base_mesh_region_range<true> type;
    };

    template<>
    struct region_range< base_mesh<false>, base_element<false> >
    {
      typedef base_mesh_region_range<false> type;
    };

    template<bool is_const,  typename ElementT>
    struct const_region_range< base_mesh<is_const>, ElementT >
    {
      typedef base_mesh_region_range<true> type;
    };
  }




//   template<bool is_const>
//   base_mesh_hierarchy<is_const> mesh_hierarchy( base_region<is_const> const & region )
//   {
//     return region.mesh_hierarchy();
//   }

  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_region<is_const> const & region )
  {
    return region.internal_mesh_hierarchy();
  }



//   template<bool is_const>
//   base_mesh_hierarchy<is_const> mesh_hierarchy( base_mesh_region<is_const> const & region )
//   {
//     return mesh_hierarchy( region.mesh() );
//   }

  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_mesh_region<is_const> const & region )
  {
    return region.internal_mesh_hierarchy();
  }



  template<typename SomethingT>
  typename viennagrid::result_of::region_range<SomethingT>::type regions(SomethingT const & something)
  {
    return typename viennagrid::result_of::region_range<SomethingT>::type(something);
  }

  template<typename SomethingT, typename ElementT>
  typename viennagrid::result_of::region_range<SomethingT, ElementT>::type regions(SomethingT const & something, ElementT const & element)
  {
    return typename viennagrid::result_of::region_range<SomethingT, ElementT>::type(something, element);
  }



  template<typename RegionRangeT1, typename RegionRangeT2>
  bool equal_regions(RegionRangeT1 const & region1, RegionRangeT2 const & region2)
  {
    if (region1.size() != region2.size())
      return false;

    for (typename RegionRangeT1::const_iterator r1it = region1.begin(); r1it != region1.end(); ++r1it)
    {
      typename RegionRangeT2::const_iterator r2it = region2.begin();
      for (; r2it != region2.end(); ++r2it)
      {
        if ( (*r1it).id() == (*r2it).id() )
          break;
      }

      if (r2it == region2.end())
        return false;
    }

    return true;
  }














  void add(viennagrid_region region, element const & e);
  template<bool element_is_const>
  bool is_in_region( viennagrid_region region, base_element<element_is_const> const & e );


  inline void add( region const & region, element const & e )
  { add(region.internal(), e); }
  template<bool region_is_const, bool element_is_const>
  bool is_in_region( base_region<region_is_const> const & region, base_element<element_is_const> const & e )
  { return is_in_region( region.internal(), e ); }


  inline void add( mesh_region const & mr, element const & e )
  { add(mr.internal_region(), e); }
  template<bool region_is_const, bool element_is_const>
  bool is_in_region( base_mesh_region<region_is_const> const & mesh_region, base_element<element_is_const> const & e )
  { return is_in_region(mesh_region.region(), e); }


  template<bool element_is_const, bool region_is_const>
  bool is_boundary( base_mesh_region<region_is_const> const & region, base_element<element_is_const> const & e );



  template<bool element_is_const>
  void copy_region_information(base_element<element_is_const> const & src_element, element const & dst_element)
  {
    typedef base_element<element_is_const> SrcElementType;
    typedef typename viennagrid::result_of::mesh_hierarchy<SrcElementType>::type SrcMeshHierarchyType;

    typedef typename viennagrid::result_of::region_range<SrcMeshHierarchyType, SrcElementType>::type ElementRegionType;
    typedef typename viennagrid::result_of::iterator<ElementRegionType>::type ElementRegionIterator;

    ElementRegionType regions(src_element);
    for (ElementRegionIterator rit = regions.begin(); rit != regions.end(); ++rit)
      viennagrid::add( dst_element.mesh_hierarchy().get_make_region((*rit).id()), dst_element );
  }





















}

#endif
