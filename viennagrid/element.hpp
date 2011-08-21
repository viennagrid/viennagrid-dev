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


#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

#include "viennagrid/forwards.h"
//#include "viennagrid/topology/celltags.hpp"
#include "viennagrid/topology/point.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/detail/element_orientation.hpp"
#include "viennagrid/detail/lower_level_holder.hpp"

#include <vector>

namespace viennagrid
{
  
  //////////////  Top Level configuration //////////
  template <typename T_Configuration,
            typename ElementTag>
  class element :
      public lower_level_holder < T_Configuration, ElementTag, ElementTag::topology_level - 1>,
      public traits::element_id<ElementTag>::id_handler
  {
      typedef typename T_Configuration::numeric_type                   ScalarType;
      typedef lower_level_holder < T_Configuration, ElementTag, ElementTag::topology_level - 1>             Base;
      typedef typename result_of::point_type<T_Configuration>::type         PointType;
      typedef typename result_of::ncell_type<T_Configuration, 0>::type      VertexType;
      typedef topology::subcell_desc<ElementTag, 0>                                 VertexSpecs;

    public:
      typedef T_Configuration                                       config_type;
      typedef ElementTag                                            element_tag;

      element ( ) {};   //construction of "template" for this type

      element (const element & e2) : Base(e2)
      {
        //std::cout << "Copy constructor Element " << this << std::endl;
        this->id(e2.id());
      };

      template <typename DomainType>
      void fill(DomainType & dom)
      {
        Base::fill_level(dom);
      }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "---- " << ElementTag::name() << " " << this << " ---------" << std::endl;
        Base::print(indent + 1);
      }

      void print_short() const
      {
        std::cout << "---- " << ElementTag::name() << " " << this << " ---------" << std::endl;
        Base::print_short();
      }

      VertexType & getVertex(int pos) const { return *(Base::vertices_[pos]); }

      void setVertices(VertexType **vertices)
      {
        for(int i=0; i<VertexSpecs::num_elements; i++)
        {
          Base::vertices_[i] = vertices[i];
          //std::cout << i << " ";
        }
        //std::cout << std::endl;
      }

      /*bool operator==(element & c2)
      {
        //simple algorithm that checks each vertex for a matching vertex in c2
        //For a small number of vertices this is not too bad.
        bool found = false;
        for (int i=0; i<subcell_traits<ElementTag,0>::num_elements; ++i)
        {
          for (int j=0; j<subcell_traits<ElementTag,0>::num_elements; ++j)
          {
            if (Base::vertices_[i] == c2.vertices_[j])
            {
              found = true;
              break;
            }
          }
          if (found){
            found = false;
            continue;
          }
          //not found:
          return false;
        }
        //all vertices found:
        return true;
      } //operator== */

      ///////////////////// container ////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container()
      { 
        return container<j>(less_tag());
      }


      //const
      template <long j>
      const typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container() const
      { 
        return container<j>(less_tag());
      }

  };

  //vertex-type needs separate treatment:
  template <typename T_Configuration>
  class element <T_Configuration, point_tag> :
    public traits::element_id<point_tag>::id_handler
  {
      typedef typename result_of::point_type<T_Configuration>::type          PointType;
      typedef typename T_Configuration::numeric_type                         CoordType;
      typedef typename result_of::ncell_type<T_Configuration, 0>::type       VertexType;
      typedef typename T_Configuration::cell_tag                             CellTag;

    public:
      typedef T_Configuration                                       config_type;
      typedef point_tag                                             element_tag;
      typedef typename traits::element_id<point_tag>::id_handler           id_handler;

      element() { };

      element(PointType const & p, long id = -1) : point_(p)
      {
        this->id(id);
      };

      element(const element & e2)
      {
        //std::cout << "Copy constructor Element (Vertex) " << this << std::endl;
        point_ = e2.point_;
        this->id(e2.id_);
      }

      element & operator=(const element & other)
      {
        point_ = other.point_;
        this->id(other.id_);                
        return *this;
      }

      PointType & getPoint() { return point_; }
      PointType const & getPoint() const { return point_; }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "Vertex (" << this << ") " << this->id() << ": ";
        std::cout << point_ << std::endl;
      }

      void print_short() const
      {
        std::cout << "Vertex (" << this << ") " << this->id() << ": ";
        std::cout << point_ << std::endl;
      }

      void printAddress() const{
        std::cout << this << std::endl;
      }

      //compare function for operator== (depends on ID-Handling!)
      //bool compare(VertexType & v2, NoID &)
      //{
      //  return v2.point_ == point_;
      //}

      //bool compare(VertexType & v2, ProvideID &)
      //{
      //  return v2.id_ == this->id_;
      //}

      bool operator<(const element e2) const { return point_ < e2.point_; }

      //PointType getMidPoint() { return getPoint(); }

      //bool operator==(VertexType & v2)
      //{
      //  return compare(v2, subcell_traits<typename T_Configuration::cell_tag,0>::id_handler() );
      //}
    private:
      PointType point_;
  };


}


#endif
