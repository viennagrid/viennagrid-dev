#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include "viennagrid/forwards.h"
//#include "viennagrid/topology/point.hpp"
//#include "viennagrid/iterators.hpp"
//#include "viennagrid/detail/element_orientation.hpp"
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/detail/lower_level_holder.hpp"

#include <vector>

namespace viennagrid
{
  
  //////////////  Top Level configuration //////////
  template <typename T_Configuration,
            typename ElementTag>
  class element_t :
      public lower_level_holder < T_Configuration, ElementTag, ElementTag::topology_level - 1>,
      public traits::element_id<ElementTag>::id_handler
  {
      typedef typename T_Configuration::numeric_type                   ScalarType;
      typedef lower_level_holder < T_Configuration, ElementTag, ElementTag::topology_level - 1>             Base;
      typedef typename result_of::point<T_Configuration>::type         PointType;
      typedef typename result_of::ncell<T_Configuration, 0>::type      VertexType;
      typedef topology::subelements<ElementTag, 0>                                 VertexSpecs;

    public:
      typedef T_Configuration                                       config_type;
      typedef ElementTag                                            element_tag;

      element_t ( ) {};   //construction of "template" for this type

      element_t (const element_t & e2) : Base(e2)
      {
        //std::cout << "Copy constructor Element " << this << std::endl;
        this->id(e2.id());
      };

      template <typename DomainType>
      void fill(DomainType & dom)
      {
        Base::fill_level(dom);
      }

      void vertices(VertexType **vertices_)
      {
        for(int i=0; i<VertexSpecs::num_elements; i++)
        {
          Base::vertices_[i] = vertices_[i];
          //std::cout << i << " ";
        }
        //std::cout << std::endl;
      }

      ///////////////////// container ////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element_t<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element_t<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container()
      { 
        return container<j>(less_tag());
      }


      //const
      template <long j>
      const typename result_of::element_container< element_t<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element_t<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container() const
      { 
        return container<j>(less_tag());
      }

  };

  template <typename ConfigType, typename ElementTag>
  std::ostream & operator<<(std::ostream & os, element_t<ConfigType, ElementTag> const & el)
  {
    typedef element_t<ConfigType, ElementTag>                ElementType;
    typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type    VertexRange;
    typedef typename viennagrid::result_of::iterator<VertexRange>::type          VertexIterator;
    
    os << "-- element<" << ElementTag::topology_level << "> " << el.id() << " --";
    VertexRange vertices = viennagrid::ncells(el);
    for (VertexIterator vit  = vertices.begin();
                        vit != vertices.end();
                      ++vit)
      os << std::endl << " " << vit->point();
    
    return os;
  }

  
  
  //vertex-type needs separate treatment:
  template <typename T_Configuration>
  class element_t <T_Configuration, point_tag> :
    public traits::element_id<point_tag>::id_handler
  {
      typedef typename result_of::point<T_Configuration>::type          PointType;
      typedef typename T_Configuration::numeric_type                         CoordType;
      typedef typename result_of::ncell<T_Configuration, 0>::type       VertexType;
      typedef typename T_Configuration::cell_tag                             CellTag;

    public:
      typedef T_Configuration                                       config_type;
      typedef point_tag                                             element_tag;
      typedef typename traits::element_id<point_tag>::id_handler           id_handler;

      element_t() { };

      element_t(PointType const & p, long id = -1) : point_(p)
      {
        this->id(id);
      };

      element_t(const element_t & e2)
      {
        //std::cout << "Copy constructor Element (Vertex) " << this << std::endl;
        point_ = e2.point_;
        this->id(e2.id_);
      }

      element_t & operator=(const element_t & other)
      {
        point_ = other.point_;
        this->id(other.id_);                
        return *this;
      }

      PointType & point() { return point_; }
      PointType const & point() const { return point_; }
      
      //convenience access to coordinates of the vertex:
      CoordType & operator[](std::size_t i) { return point_[i]; }
      CoordType const & operator[](std::size_t i) const { return point_[i]; }
      
      std::size_t size() const { return point_.size(); }

      bool operator<(const element_t e2) const { return point_ < e2.point_; }
      bool operator>(const element_t e2) const { return point_ > e2.point_; }

    private:
      PointType point_;
  };

  template <typename ConfigType>
  std::ostream & operator<<(std::ostream & os, element_t<ConfigType, point_tag> const & el)
  {
    os << "-- element<0> " << el.id() << " --" << std::endl;
    os << " " << el.point();
    
    return os;
  }

  
}


#endif
