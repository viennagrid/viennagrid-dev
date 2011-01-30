/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_SEGMENT_HPP
#define VIENNAGRID_SEGMENT_HPP

#include <map>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/element_key.hpp"

namespace viennagrid
{
  
  /////////////// Segment is a container for elements ////////////////

  namespace result_of
  {
    template <typename T,
              dim_type dim,
              dim_type cell_level = T::element_tag::topology_level>
    struct segment_subcell_container
    {
      typedef typename result_of::element_tag<T, dim>::type    element_tag;
      typedef typename result_of::config<T>::type              config_type;
      typedef element<config_type, element_tag >            element_type;
      
      typedef std::map< element_key<element_type>, element_type *>      type;
    };

    //at point level:
    template <typename T, dim_type cell_level>
    struct segment_subcell_container <T, 0, cell_level>
    {
      typedef typename result_of::element_tag<T, 0>::type    element_tag;
      typedef typename result_of::config<T>::type              config_type;
      typedef element<config_type, element_tag >            element_type;
      
      typedef std::vector< element_type *>      type;
    };

    //at cell level:
    template <typename T, dim_type cell_level>
    struct segment_subcell_container <T, cell_level, cell_level>
    {
      typedef typename result_of::element_tag<T, T::element_tag::topology_level>::type    element_tag;
      typedef typename result_of::config<T>::type              config_type;
      typedef element<config_type, element_tag >            element_type;
      
      typedef std::vector< element_type *>      type;
    };
    
  }




  
  //Segment traits for topology handling:
  /** @brief Specifies the handling at the topological level 'dim' */
  template <dim_type dim>
  struct segment_traits
  {
    //default: Full handling
    typedef full_handling_tag    handling_tag;
  };

  
  
  template <typename Conf,
            dim_type dim,
            typename handling_tag>
  class segment_layers : public segment_layers<Conf, dim - 1, handling_tag>
  {
      typedef typename subcell_traits<typename Conf::cell_tag, dim>::element_tag       element_tag;
      typedef element<Conf, element_tag >                                              element_type;
      typedef element<Conf, typename Conf::cell_tag>                                   cell_type;
      typedef typename result_of::segment_subcell_container<cell_type, dim>::type      container_type;
      typedef segment_layers<Conf, dim - 1, handling_tag>                              base_type;
      
    public:
      
      void add(cell_type & e)
      {
        base_type::add(e);
      }
      
      
      ///////////////////// container retrieval /////////////////////////

      //non-const:
      template <dim_type dim_container>
      typename result_of::segment_subcell_container<element_type, dim_container>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::segment_subcell_container<element_type, dim_container>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::segment_subcell_container<element_type, dim_container>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::segment_subcell_container<element_type, dim_container>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::segment_subcell_container<element_type, dim_container>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::segment_subcell_container<element_type, dim_container>::type * 
      container(less_tag) const { return base_type::template container<dim_container>(); }
      
      
      ////////////////////// size ////////////////////////
      template <dim_type j>
      size_t size(less_tag) const { return base_type::template size<j>(); }
      template <dim_type j>
      size_t size(equal_tag) const { return elements.size(); }
      template <dim_type j>
      size_t size() const
      {
        return size<j>( typename level_discriminator<dim, j>::result_type() );
      }
    
    private:
      container_type elements;
  };
  
  
  template <typename Conf, typename handling_tag>
  class segment_layers<Conf, 0, handling_tag>
  {
      typedef typename subcell_traits<typename Conf::cell_tag, 0>::element_tag    element_tag;
      typedef element<Conf, element_tag >                                              element_type;
      typedef element<Conf, typename Conf::cell_tag>                                   cell_type;
      typedef typename result_of::segment_subcell_container<cell_type, 0>::type           container_type;
    public:
      
      void add(cell_type & e)
      {
        //elements.push_back(&(*e.template begin<0>()));
        //TODO: Fill segments with vertices, avoid duplicates!
      }
      
      template <dim_type dim>
      const container_type * container() const { return &elements; }
      
      ////////////////////// size ////////////////////////
      template <dim_type j>
      size_t size(equal_tag) const { return elements.size(); }
      template <dim_type j>
      size_t size() const
      {
        return size<j>( typename level_discriminator<0, j>::result_type() );
      }
      
    private:
      container_type    elements;        //container of elements
  };
  
  
  //Segment type:
  template <typename Conf>
  class segment : public segment_layers<Conf, Conf::cell_tag::topology_level, full_handling_tag>
  {
    typedef element<Conf, typename Conf::cell_tag>                                   cell_type;
    typedef segment_layers<Conf, Conf::cell_tag::topology_level, full_handling_tag>  base_type;
    
    public:
      typedef Conf                config_type;
      
      
      void add(cell_type & e)
      {
        base_type::add(e);
      }      
      
  };

  
  
  
}
#endif