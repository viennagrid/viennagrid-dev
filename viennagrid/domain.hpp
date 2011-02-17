/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_DOMAIN_GUARD
#define VIENNAGRID_DOMAIN_GUARD


#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include "viennagrid/forwards.h"
#include "viennagrid/celltags.hpp"  
#include "viennagrid/point.hpp"
#include "viennagrid/element.hpp"
#include "viennagrid/element_key.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/domain_configs.hpp"

namespace viennagrid
{


/***************************** Domain Type ***********************/

  namespace result_of
  {
    template <typename config_type,
              dim_type dim,
              dim_type cell_level /* see forwards.h for default argument */>
    struct element_container< domain<config_type>, dim, cell_level >
    {
      typedef typename result_of::ncell_type<config_type, dim>::type            element_type;
      
      typedef std::map< element_key<element_type>, element_type >      type;
    };

    //at point level:
    template <typename config_type, dim_type cell_level>
    struct element_container< domain<config_type>, 0, cell_level>
    {
      typedef typename result_of::ncell_type<config_type, 0>::type              element_type;
      
      typedef std::vector< element_type >      type;
    };

    //at cell level:
    template <typename config_type, dim_type cell_level>
    struct element_container< domain<config_type>, cell_level, cell_level>
    {
      typedef typename result_of::ncell_type<config_type, cell_level>::type     element_type;
      
      typedef std::vector< element_type >      type;
    };
    
  }


  template <typename Config, // config class
            dim_type dim,  // dimension of the elements covered here
            bool is_cell = false,                   // whether this layer holds the cells (i.e. highest topological element)
            typename STOR = full_handling_tag       //Storage scheme: Full storage, or ignore layer
           >
  class domain_layers  : public domain_layers<Config, dim-1>
  {
      //typedef typename result_of::element_tag<typename Config::cell_tag, dim>::type    element_tag;
      typedef domain<Config>                                                          domain_type;
      typedef typename subcell_traits<typename Config::cell_tag, dim>::element_tag    element_tag;
      typedef element<Config, element_tag >                                              element_type;
      typedef element<Config, typename Config::cell_tag>                                   cell_type;
      typedef typename result_of::element_container<domain_type, dim, Config::cell_tag::topology_level>::type           container_type;
      typedef domain_layers<Config, dim-1>                                               base_type;
    
    public:
      typedef Config                                    config_type;
      
      using base_type::add;
      
      /*
      element_type * add(element_type & e)
      {
        e.setID(elements.size());
        elements.push_back(e);
        elements.back().fill(*this);
        return &(elements.back());
      } */

      element_type *
      add(element_type & elem, ElementOrientation * orientation) {

        typedef typename std::map< element_key<element_type>, element_type >::iterator  ElementIterator;
        
        typedef typename result_of::ncell_container<element_type, 0>::type  VertexOnElementContainer;
        typedef typename result_of::iterator<element_type, 0>::type         VertexOnElementIterator;

        element_key<element_type> epc(elem);
        //check whether already inserted:
        ElementIterator elit = elements.find(epc);
        //std::cout << "Candidate: "; elem.print_short();

        if (elit == elements.end())
        {
          //provide ID for element:
          elem.setID(elements.size());

          //std::cout << "ACCEPTED " << std::endl;

          //set default orientation:
          orientation->setDefaultOrientation();

          std::pair<element_key<element_type>, element_type> p(epc, elem);
          return &((elements.insert(p).first)->second);
        }

        //std::cout << "REJECTED" << std::endl;
        long i=0; long j=0;
        
        
        //set orientation:
        VertexOnElementContainer vertices_on_element = ncells<0>(elem);
        for (VertexOnElementIterator voeit = vertices_on_element.begin();
              voeit != vertices_on_element.end();
              ++voeit, ++i)
        {
            
            VertexOnElementContainer vertices_on_element_2 = ncells<0>(elit->second);
            for (VertexOnElementIterator voeit2 = vertices_on_element_2.begin();
                  voeit2 != vertices_on_element_2.end();
                  ++voeit2, ++j)
            {
              if (voeit->getID() == voeit2->getID())
              {
                //orientation->setPermutation(i,j);   //local (elem) to global (elit->second)
                orientation->setPermutation(j,i);   //global (elit->second) to local (elem)
                break;
              }
            }
            j=0;
        }

        return &(elit->second);
      }


      ///////////////////// container retrieval /////////////////////////

      //non-const:
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
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
      container_type    elements;        //container of elements
  };
  
  
  // special handling for cells:
  template <typename Config,
            dim_type dim,
            typename STOR >
  class domain_layers<Config, dim, true, STOR> : public domain_layers<Config, dim-1, false, STOR>
  {
      //typedef typename result_of::element_tag<typename Config::cell_tag, 0>::type   element_tag;
      typedef domain<Config>                                                          domain_type;
      typedef element<Config, typename Config::cell_tag >                                         element_type;
      typedef typename result_of::element_container<domain_type,
                                                    Config::cell_tag::topology_level,
                                                    Config::cell_tag::topology_level>::type                container_type;
      typedef domain_layers<Config, dim-1>                                               base_type;
    
    public:
      typedef Config    config_type;

      void reserve_cells(size_t num) { elements.reserve(num); }
      
      
      using base_type::add;
      
      element_type * add(element_type & e)
      {
        assert(elements.capacity() > elements.size() && "Not enough memory for cells reserved!");
        
        elements.push_back(e);
        elements.back().setID(elements.size());
        //std::cout << "Filling cell for domain " << this << std::endl;
        elements.back().fill(*this);
        return &(elements.back());
      }
      
      //non-const:
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
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
      
      element_type cells(size_t i) { return elements[i]; }
      element_type cells(size_t i) const { return elements[i]; }      
      
    private:
      container_type    elements;        //container of elements
  };
  
  //terminate recursion at vertex level:
  template <typename Config,
            bool is_cell,
            typename STOR >
  class domain_layers<Config, 0, is_cell, STOR>
  {
      //typedef typename result_of::element_tag<typename Config::cell_tag, 0>::type   element_tag;
      typedef domain<Config>                                                          domain_type;
      typedef element<Config, point_tag >                                             element_type;
      typedef element<Config, typename Config::cell_tag >                             cell_type;
      typedef typename result_of::element_container<domain_type,
                                                    0,
                                                    Config::cell_tag::topology_level
                                                    >::type                            container_type;
    
    public:
      typedef Config    config_type;
      
      void reserve_vertices(size_t num) { elements.reserve(num); }
      
      element_type * add(element_type & e)
      {
        assert(elements.capacity() > elements.size() && "Not enough memory for vertices reserved!");
        e.setID(elements.size());
        elements.push_back(e);
        return &(elements.back());
      }
      
      element_type & vertex(size_t id) { return elements[id]; }
      
      template <dim_type dim>
      container_type * container() { return &elements; }
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

  
  
  

  template <typename Config>
  class domain : public domain_layers<Config,
                                      Config::cell_tag::topology_level,
                                      true>  //we start with cells
  {
      typedef domain_layers<Config, Config::cell_tag::topology_level, true>           base_type;
      typedef segment_t<Config>                                                         segment_type;
    
    public:
      typedef Config                                    config_type;
      
      using base_type::add;
      
      void create_segments(size_t num)
      {
        assert(segments.size() == 0 || "Segments in domain already created!");
        segments.resize(num);
        
        for (size_t i=0; i<segments.size(); ++i)
          segments[i].set_domain(*this);
      }
      
      segment_type & segment(size_t seg_index)
      {
        assert(seg_index < segments.size() || "Segment index out of bounds!");
        return segments[seg_index];
      }
    
      const std::vector< segment_type > * segment_container() const { return & segments; }
      const std::vector< segment_type > * segment_container()       { return & segments; }      
    
    private:
      //store segments here
      std::vector< segment_type > segments;      
  };

  namespace result_of
  {
    
    template <typename Config,
              dim_type dim,
              dim_type cell_level /* see forwards.h for default type */>
    struct ncell_type
    {
      typedef element<Config, 
                      typename subcell_traits<typename Config::cell_tag,
                                              dim>::element_tag
                      > type;
    };
    
    template <typename Config,
              dim_type cell_level>
    struct ncell_type <Config, cell_level, cell_level>
    {
      typedef element<Config, 
                      typename Config::cell_tag>       type;
    };
    
  }
  
}
#endif
