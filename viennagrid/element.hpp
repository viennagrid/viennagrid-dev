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
#include "viennagrid/detail/boundary_ncell_layer.hpp"

#include <vector>

namespace viennagrid
{
  
  //////////////  Top Level configuration //////////
  template <typename ConfigType,
            typename ElementTag>
  class element_t :
      public boundary_ncell_layer < ConfigType, ElementTag, ElementTag::dim - 1>,
      public result_of::element_id_handler<ConfigType, ElementTag>::type
  {
      typedef typename ConfigType::numeric_type                   ScalarType;
      typedef boundary_ncell_layer < ConfigType, ElementTag, ElementTag::dim - 1>             Base;
      typedef typename result_of::point<ConfigType>::type         PointType;
      typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;
      typedef topology::bndcells<ElementTag, 0>                                 VertexSpecs;

    public:
      typedef ConfigType                                       config_type;
      typedef ElementTag                                       tag;
      typedef typename result_of::element_id_handler<ConfigType, point_tag>::type::id_type    id_type;

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
        for(int i=0; i<VertexSpecs::num; i++)
        {
          Base::vertices_[i] = vertices_[i];
          //std::cout << i << " ";
        }
        //std::cout << std::endl;
      }

  };

  template <typename ConfigType, typename ElementTag>
  std::ostream & operator<<(std::ostream & os, element_t<ConfigType, ElementTag> const & el)
  {
    typedef element_t<ConfigType, ElementTag>                ElementType;
    typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type    VertexRange;
    typedef typename viennagrid::result_of::iterator<VertexRange>::type          VertexIterator;
    
    os << "-- " << ElementTag::name() << ", ID: " << el.id() << " --";
    VertexRange vertices = viennagrid::ncells(el);
    for (VertexIterator vit  = vertices.begin();
                        vit != vertices.end();
                      ++vit)
      os << std::endl << "  " << *vit;
    
    return os;
  }

  
  
  //vertex-type needs separate treatment:
  template <typename ConfigType>
  class element_t <ConfigType, point_tag> :
    public result_of::element_id_handler<ConfigType, point_tag>::type
  {
      typedef typename result_of::point<ConfigType>::type          PointType;
      typedef typename ConfigType::numeric_type                    CoordType;
      typedef typename result_of::ncell<ConfigType, 0>::type       VertexType;
      typedef typename ConfigType::cell_tag                        CellTag;

    public:
      typedef ConfigType                                       config_type;
      typedef point_tag                                        tag;
      typedef typename result_of::element_id_handler<ConfigType, point_tag>::type::id_type    id_type;

      element_t() { };

      element_t(PointType const & p, long id = -1) : point_(p)
      {
        this->id(id);
      };

      element_t(const element_t & e2)
      {
        //std::cout << "Copy constructor Element (Vertex) " << this << std::endl;
        point_ = e2.point_;
        this->id(e2.id());
      }

      //clean up any data associated with the element if it is destroyed:
      ~element_t() { viennadata::erase<viennadata::all, viennadata::all>()(*this); }

      element_t & operator=(const element_t & other)
      {
        point_ = other.point_;
        this->id(other.id());                
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
    os << "-- Vertex, ID: " << el.id() << "; Point: " << el.point();
    
    return os;
  }

  
}


#endif
