#ifndef VIENNAGRID_MESH_HIERARCHY_HPP
#define VIENNAGRID_MESH_HIERARCHY_HPP

namespace viennagrid
{

  template<bool is_const>
  class base_mesh_hierarchy
  {
    template<bool other_is_const>
    friend class base_mesh_hierarchy;

    template<bool other_is_const>
    friend class base_mesh;

  public:

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh, true>::type const_mesh_type;

    typedef typename result_of::const_nonconst<region, is_const>::type region_type;
    typedef typename result_of::const_nonconst<region, true>::type const_region_type;

    typedef typename result_of::const_nonconst<element, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element, true>::type const_element_type;

    typedef viennagrid_index region_id_type;
    typedef element_tag element_tag_type;


    base_mesh_hierarchy(viennagrid_mesh_hierarchy internal_mesh_hierarchy_in) : internal_mesh_hierarchy_(internal_mesh_hierarchy_in)
    {
      retain();
    }

    base_mesh_hierarchy(viennagrid_int dimension_, element_tag_type cell_tag_) : internal_mesh_hierarchy_(0)
    {
      viennagrid_mesh_hierarchy_create(dimension_, cell_tag_.internal_element_tag(), &internal_mesh_hierarchy_);
    }

    template<bool other_is_const>
    base_mesh_hierarchy(base_mesh_hierarchy<other_is_const> mh) : internal_mesh_hierarchy_(mh.internal_mesh_hierarchy_) {
      retain();
    }

    base_mesh_hierarchy(base_mesh_hierarchy<is_const> const & mh) : internal_mesh_hierarchy_(mh.internal_mesh_hierarchy_)
    {
      retain();
    }

    base_mesh_hierarchy<is_const> & operator=( base_mesh_hierarchy<is_const> const & mh )
    {
      release();
      internal_mesh_hierarchy_ = mh.internal_mesh_hierarchy_;
      retain();
      return *this;
    }

    ~base_mesh_hierarchy()
    {
      release();
    }

    mesh_type get_root();

    viennagrid_int dimension() const
    {
      viennagrid_int tmp;
      viennagrid_mesh_hierarchy_get_dimension(internal_mesh_hierarchy(), &tmp);
      return tmp;
    }
    element_tag_type cell_tag() const
    {
      viennagrid_element_tag tmp;
      viennagrid_mesh_hierarchy_get_cell_tag(internal_mesh_hierarchy(), &tmp);
      return element_tag_type(tmp);
    }
    element_tag_type facet_tag() const { return cell_tag().facet_tag(); }
    element_tag_type unpack_element_tag(element_tag_type et)
    {
      if (et == viennagrid::cell_tag())
        return element_tag_type(cell_tag());
      else if (et == viennagrid::facet_tag())
        return element_tag_type(facet_tag());
      return et;
    }

    viennagrid_mesh_hierarchy internal_mesh_hierarchy() const { return const_cast<viennagrid_mesh_hierarchy>(internal_mesh_hierarchy_); }

    std::size_t regions_count() const
    {
      viennagrid_region * begin;
      viennagrid_region * end;
      viennagrid_regions_get(internal_mesh_hierarchy(), &begin, &end);
      return end-begin;
    }
    region_type get_make_region(region_id_type region_id);
    region_type make_region();
    const_region_type get_region(region_id_type region_id) const;

  private:

    void retain() const { viennagrid_mesh_hierarchy_retain(internal_mesh_hierarchy()); }
    void release() const { viennagrid_mesh_hierarchy_release(internal_mesh_hierarchy()); }

    viennagrid_mesh_hierarchy internal_mesh_hierarchy_;
  };

}


#endif
