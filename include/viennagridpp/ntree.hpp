#ifndef VIENNAGRID_NTREE_HPP
#define VIENNAGRID_NTREE_HPP

namespace viennagrid
{


  template<typename ElementT>
  class element_wrapper
  {
  public:
    typedef ElementT element_type;
    typedef typename viennagrid::result_of::point<ElementT>::type PointType;

    element_wrapper(ElementT const & element_) : element(element_) {}

    ElementT const & unpack() const { return element; }

    template<typename BoxT>
    bool intersect(BoxT const & box) const
    {
      if (element.tag().is_vertex())
      {
        PointType point = viennagrid::get_point(element);
        return (box.min() <= point) && (point < box.max());
      }

      assert(false);
      return false;
    }

  private:
    ElementT element;
  };


  template<typename ElementT>
  class vertex_with_distance_wrapper
  {
  public:
    typedef ElementT element_type;

    typedef typename viennagrid::result_of::point<ElementT>::type PointType;
    typedef typename viennagrid::result_of::coord<ElementT>::type CoordType;

    vertex_with_distance_wrapper(ElementT const & element_, CoordType distance_) : element(element_), distance(distance_) {}

    ElementT const & unpack() const { return element; }

    template<typename BoxT>
    bool intersect(BoxT const & box) const
    {
      assert( element.tag().is_vertex() );
      PointType point = viennagrid::get_point(element);
      PointType p_min = point - PointType( point.size(), distance );
      PointType p_max = point + PointType( point.size(), distance );

      for (std::size_t dim = 0; dim != point.size(); ++dim)
      {
        if ( box.max()[dim] <= p_min[dim] )
          return false;

        if ( p_max[dim] < box.min()[dim] )
          return false;
      }

      return true;
    }

  private:
    ElementT element;
    CoordType distance;
  };







  template<typename WrapperT>
  typename WrapperT::element_type const & unpack(WrapperT const & wrapper)
  {
    return wrapper.unpack();
  }

  template<typename WrapperT, typename BoxT>
  bool intersect(WrapperT const & wrapper, BoxT const & box)
  {
    return wrapper.intersect(box);
  }






  template<typename ElementT>
  class ntree_node
  {
  public:
    typedef ntree_node<ElementT> node_type;
    typedef typename viennagrid::result_of::point<ElementT>::type point_type;

    ntree_node(point_type min_in, point_type max_in, int depth_in = 0) : min_(min_in), max_(max_in), depth_(depth_in) {}
    ~ntree_node()
    {
      clear();
    }

    void clear()
    {
      for (typename std::vector<ntree_node*>::size_type i = 0; i != children.size(); ++i)
        delete children[i];
      children.clear();

      elements_.clear();
    }

    node_type * get(point_type const & point)
    {
      if (children.empty())
        return this;

      for (std::size_t i = 0; i != children.size(); ++i)
      {
        if (children[i]->is_inside(point))
          return children[i]->get(point);
      }

      return 0;
    }

    std::vector<ElementT> const & elements() const { return elements_; }

    bool is_inside(point_type const & point) const
    {
      return (min() <= point) && (point < max());
    }

    template<typename WrapperT>
    bool intersect(WrapperT const & wrapper) const
    {
      return viennagrid::intersect(wrapper, *this);
    }

//     bool intersect(ElementT const & element) const
//     {
//       return intersect( element_wrapper<ElementT>(element) );
//     }

    void split_node()
    {
      point_type s = node_size();

      if (geometric_dimension() == 1)
      {
        for (int x = 0; x != 2; ++x)
        {
          point_type new_min = min() + viennagrid::make_point( s[0]*(x+0) )/2;
          point_type new_max = min() + viennagrid::make_point( s[0]*(x+1) )/2;

          children.push_back( new node_type(new_min, new_max, depth()+1) );
        }
      }
      else if (geometric_dimension() == 2)
      {
        for (int y = 0; y != 2; ++y)
          for (int x = 0; x != 2; ++x)
          {
            point_type new_min = min() + viennagrid::make_point( s[0]*(x+0), s[1]*(y+0) )/2;
            point_type new_max = min() + viennagrid::make_point( s[0]*(x+1), s[1]*(y+1) )/2;

            children.push_back( new node_type(new_min, new_max, depth()+1) );
          }
      }
      else if (geometric_dimension() == 3)
      {
        for (int z = 0; z != 2; ++z)
          for (int y = 0; y != 2; ++y)
            for (int x = 0; x != 2; ++x)
            {
              point_type new_min = min() + viennagrid::make_point( s[0]*(x+0), s[1]*(y+0), s[2]*(z+0) )/2;
              point_type new_max = min() + viennagrid::make_point( s[0]*(x+1), s[1]*(y+1), s[2]*(z+1) )/2;

              children.push_back( new node_type(new_min, new_max, depth()+1) );
            }
      }
      else
      {
        assert(false);
      }

      // pass elements to children
      for (std::size_t i = 0; i != elements().size(); ++i)
      {
        for (std::size_t j = 0; j != children.size(); ++j)
        {
          if (children[j]->intersect( element_wrapper<ElementT>(elements_[i]) ))
            children[j]->add_element_impl( element_wrapper<ElementT>(elements_[i]) );
        }
      }
      elements_.clear();
    }


    template<typename WrapperT>
    void add(WrapperT const & wrapper, std::size_t max_elements_per_node, int max_depth)
    {
      // if node has children: pass to children
      if (!children.empty())
      {
        for (std::size_t i = 0; i != children.size(); ++i)
        {
          if (children[i]->intersect(wrapper))
            children[i]->add(wrapper, max_elements_per_node, max_depth);
        }

        return;
      }

      if (!intersect(wrapper))
        return;

      if ( (elements().size() < max_elements_per_node) || (depth() >= max_depth) )
      {
        add_element_impl(wrapper);
        return;
      }

      split_node();
      add(wrapper, max_elements_per_node, max_depth);
    }

//     void add(ElementT const & element, int max_elements_per_node, int max_depth)
//     {
//       add( element_wrapper<ElementT>(element), max_elements_per_node, max_depth );
//     }



    viennagrid_dimension geometric_dimension() const { return min().size(); }

    void print() const
    {
      indent(); std::cout << "Node with depth = " << depth() << std::endl;
      indent(); std::cout << " min = " << min() << std::endl;
      indent(); std::cout << " max = " << max() << std::endl;
      for (std::size_t i = 0; i != elements().size(); ++i)
      {
        indent(); std::cout << elements()[i] << std::endl;
      }

      for (std::size_t i = 0; i != children.size(); ++i)
        children[i]->print();
    }

    int element_count() const
    {
      int size = elements().size();
      for (std::size_t i = 0; i != children.size(); ++i)
        size += children[i]->element_count();
      return size;
    }

    int total_depth() const
    {
      if (children.empty())
        return depth;

      int d = 0;
      for (std::size_t i = 0; i != children.size(); ++i)
        d = std::max(children[i], d);
      return d;
    }

    bool erase(ElementT const & to_erase)
    {
      for (std::size_t i = 0; i != elements_.size(); ++i)
      {
        if (elements_[i] == to_erase)
        {
          elements_.erase( elements_.begin()+i );
          return true;
        }
      }

      for (std::size_t i = 0; i != children.size(); ++i)
        children[i]->erase(to_erase);

      return false;
    }


    int depth() const { return depth_; }

    point_type const & min() const { return min_; }
    point_type const & max() const { return max_; }

    point_type node_center() const { return (min()+max())/2; }
    point_type node_size() const { return max()-min(); }


  private:

    template<typename WrapperT>
    void add_element_impl(WrapperT const & wrapper)
    {
      assert( viennagrid::intersect(wrapper, *this) );
      elements_.push_back( unpack(wrapper) );
    }

    void indent() const
    {
      for (int i = 0; i != depth(); ++i)
        std::cout << " ";
    }

    point_type min_;
    point_type max_;
    int depth_;

    std::vector<node_type*> children;
    std::vector<ElementT> elements_;
  };

}

#endif
