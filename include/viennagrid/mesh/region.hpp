#ifndef VIENNAGRID_REGION_HPP
#define VIENNAGRID_REGION_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element.hpp"

namespace viennagrid
{

  template<bool is_const>
  class base_region
  {
    template<bool other_is_const>
    friend class base_region;

  public:

    typedef viennagrid_region_id id_type;

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh, true>::type const_mesh_type;

    base_region() : mesh_(0), region_(0) {}

    base_region(viennagrid_mesh mesh_in, viennagrid_region region_in) : mesh_(mesh_in), region_(region_in) {}

    template<bool other_is_const>
    base_region(base_region<other_is_const> const & rhs) : mesh_(rhs.mesh_), region_(rhs.region_) {}

    viennagrid_mesh internal_mesh() const { return const_cast<viennagrid_mesh>(mesh_); }
    mesh_type get_mesh() { return mesh_type(mesh_); }
    const_mesh_type get_mesh() const { return const_mesh_type(mesh_); }

    viennagrid_region internal() const { return const_cast<viennagrid_region>(region_); }

    id_type id() const
    {
      id_type id_;
      THROW_ON_ERROR( viennagrid_region_id_get(region_, &id_) );
      return id_;
    }

    std::string get_name() const
    {
      const char * name_;
      THROW_ON_ERROR( viennagrid_region_name_get(internal(), &name_) );
      return std::string(name_);
    }

    void set_name(std::string const & name_)
    {
      THROW_ON_ERROR( viennagrid_region_name_set(internal(), name_.c_str()) );
    }

    viennagrid_dimension geometric_dimension() const
    {
      viennagrid_dimension tmp;
      THROW_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh_, &tmp) );
      return tmp;
    }

  private:
    viennagrid_mesh mesh_;
    viennagrid_region region_;
  };










  template<bool is_const>
  struct unpack_region_functor
  {
    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;

    typedef typename result_of::const_nonconst<region, is_const>::type value_type;
    typedef typename result_of::const_nonconst<region, true>::type const_value_type;

    typedef value_type reference;
    typedef value_type * pointer;

    unpack_region_functor(viennagrid_mesh mesh_in) : mesh_(mesh_in) {}

    value_type operator()(viennagrid_region_id & id)
    {
      viennagrid_region region;
      THROW_ON_ERROR( viennagrid_mesh_region_get(mesh_, id, &region) );
      return value_type(mesh_, region);
    }
    const_value_type operator()(viennagrid_region_id const & id)
    {
      viennagrid_region region;
      THROW_ON_ERROR( viennagrid_mesh_region_get(mesh_, id, &region) );
      return const_value_type(mesh_, region);
    }

    viennagrid_mesh mesh_;
  };


  template<bool is_const>
  class base_region_range
  {
  public:

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<viennagrid_region_id *, is_const>::type region_pointer_type;
    typedef typename result_of::const_nonconst<viennagrid_region_id *, true>::type const_region_pointer_type;

    base_region_range(mesh_type const & mesh_in) : mesh_(mesh_in.internal())
    {
      THROW_ON_ERROR( viennagrid_mesh_regions_get(internal_mesh(mesh_in),
                                                  const_cast<viennagrid_region_id **>(&begin_),
                                                  const_cast<viennagrid_region_id **>(&end_)) );
    }

    base_region_range(element const & element_) : mesh_(element_.internal_mesh())
    {
      THROW_ON_ERROR( viennagrid_element_regions_get(mesh_,
                                                     element_.id().internal(),
                                                     const_cast<viennagrid_region_id **>(&begin_),
                                                     const_cast<viennagrid_region_id **>(&end_)) );
    }

    typedef transform_iterator<region_pointer_type, unpack_region_functor<is_const> > iterator;
    typedef transform_iterator<const_region_pointer_type, unpack_region_functor<true> > const_iterator;

    iterator begin() { return iterator(begin_, unpack_region_functor<is_const>(mesh_)); }
    iterator end() { return iterator(end_, unpack_region_functor<is_const>(mesh_)); }

    const_iterator cbegin() const { return const_iterator(begin_, unpack_region_functor<true>(mesh_)); }
    const_iterator cend() const { return const_iterator(end_, unpack_region_functor<true>(mesh_)); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

    std::size_t size() const { return end() - begin(); }
    bool empty() const { return size() == 0; }

  private:
    viennagrid_mesh mesh_;
    region_pointer_type begin_;
    region_pointer_type end_;
  };



  namespace result_of
  {
    template<bool mesh_is_const>
    struct region_range< base_mesh<mesh_is_const> >
    {
      typedef base_region_range<mesh_is_const> type;
    };

    template<bool element_is_const>
    struct region_range< base_element<element_is_const> >
    {
      typedef base_region_range<element_is_const> type;
    };

    template<>
    struct region_range< base_mesh<false> >
    {
      typedef base_region_range<false> type;
    };

    template<>
    struct region_range< base_element<false> >
    {
      typedef base_region_range<false> type;
    };


    template<bool is_const>
    struct const_region_range< base_mesh<is_const> >
    {
      typedef base_region_range<true> type;
    };

    template<bool is_const>
    struct const_region_range< base_element<is_const> >
    {
      typedef base_region_range<true> type;
    };
  }




  template<bool is_const>
  viennagrid_mesh internal_mesh( base_region<is_const> const & region )
  {
    return region.internal_mesh();
  }


  template<typename SomethingT>
  typename viennagrid::result_of::region_range<SomethingT>::type regions(SomethingT const & something)
  {
    return typename viennagrid::result_of::region_range<SomethingT>::type(something);
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


  template<bool element_is_const, bool region_is_const>
  bool is_boundary( base_region<region_is_const> const & region, base_element<element_is_const> const & e );



  template<bool element_is_const>
  void copy_region_information(base_element<element_is_const> const & src_element, element const & dst_element)
  {
    typedef base_element<element_is_const> SrcElementType;

    typedef typename viennagrid::result_of::region_range<SrcElementType>::type ElementRegionType;
    typedef typename viennagrid::result_of::iterator<ElementRegionType>::type ElementRegionIterator;

    ElementRegionType regions(src_element);
    for (ElementRegionIterator rit = regions.begin(); rit != regions.end(); ++rit)
      viennagrid::add( dst_element.get_mesh().get_or_create_region((*rit).id()), dst_element );
  }


}

#endif
