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


#ifndef VIENNAGRID_SEGMENT_HPP
#define VIENNAGRID_SEGMENT_HPP

#include <map>
#include <set>
#include <deque>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/detail/element_key.hpp"

namespace viennagrid
{
  
  /////////////// Segment is a container for elements ////////////////

  namespace result_of
  {
    //at cell level
    template <typename config_type, dim_type cell_level>
    struct element_container< segment_t<config_type>, cell_level, cell_level>
    {
      typedef typename result_of::ncell<config_type, cell_level>::type     element_type;
      
      typedef std::deque< element_type * >      type;
    };

    //at vertex level
    template <typename config_type, dim_type cell_level>
    struct element_container< segment_t<config_type>, 0, cell_level>
    {
      typedef typename result_of::ncell<config_type, 0>::type     element_type;
      
      typedef std::set< element_type * >      type;
    };

    //at any other level:
    template <typename config_type, dim_type dim, dim_type cell_level>
    struct element_container< segment_t<config_type>, dim, cell_level>
    {
      typedef typename result_of::ncell<config_type, dim>::type     element_type;
      
      typedef std::set< element_type * >      type;
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

  
  // holds a reference to the domain the segment belongs to
  template <typename Conf>
  class segment_domain_holder
  {
      typedef domain<Conf>                                                             domain_type;
    public:
      void set_domain(domain_type & d) { domain_ = &d; }

      domain_type & get_domain() { return * domain_; }
      domain_type const & get_domain() const { return * domain_; }

      size_t id() const 
      {
        for (size_t i=0; i<domain_->segments().size(); ++i)
        {
          if (this == &(domain_->segments()[i]))
            return i;
        }
        
        assert(false && "Segment ID cannot be found!");
        return domain_->segments().size();
      }
      
    protected:
      domain_type * domain_;
  };
  
  
  template <typename Config,
            dim_type dim,
            typename handling_tag>
  class segment_layers : //public segment_layers<Config, 0, handling_tag>
                         public segment_layers<Config, dim - 1, handling_tag>
                         //public segment_domain_holder<Config>
  {
      typedef typename topology::subcell_desc<typename Config::cell_tag, dim>::element_tag     element_tag;
      typedef element<Config, element_tag >                                                  LevelElementType;
      typedef typename topology::subcell_desc<typename Config::cell_tag, 0>::element_tag       vertex_tag;
      typedef element<Config, vertex_tag >                                                   VertexType;
      typedef element<Config, typename Config::cell_tag>                                     CellType;
      typedef typename result_of::element_container< segment_t<Config>, dim, Config::cell_tag::topology_level>::type      container_type;
      typedef segment_layers<Config, dim - 1, handling_tag>                              base_type;
      //typedef segment_layers<Config, 0, handling_tag>                                    base_type;
      
    public:
      
      void fill(CellType & cell)
      {
        //add vertices to segment: (note that the ncells<0>() is not available here)
        LevelElementType ** level_elements = cell.template container<dim>();
        for (long i=0; i<topology::subcell_desc<typename Config::cell_tag, dim>::num_elements; ++i)
          elements.insert(level_elements[i]);
        
        base_type::fill(cell);
      }
      
      ///////////////////// container retrieval /////////////////////////

      //non-const:
      template <dim_type dim_container>
      typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
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
    
      //CellType cells(size_t i) { return *(elements[i]); }
      //CellType cells(size_t i) const { return *(elements[i]); }      
      
    private:
      container_type elements;
  };
  
  //vertex level:
  template <typename Config, typename handling_tag>
  class segment_layers<Config, 0, handling_tag> : public segment_domain_holder<Config>
  {
      typedef typename Config::cell_tag                                 CellTag;
      typedef typename topology::subcell_desc<CellTag, 0>::element_tag    VertexTag;
      typedef element<Config, VertexTag >                               VertexType;
      typedef element<Config, typename Config::cell_tag>                CellType;
      typedef typename result_of::element_container< segment_t<Config>, 
                                                     0,
                                                     CellTag::topology_level>::type           RangeType;
      
    public:
      
      void fill(CellType & cell)
      {
        //add vertices to segment: (note that the ncells<0>() is not available here)
        VertexType ** cell_vertices = cell.template container<0>();
        for (long i=0; i<topology::subcell_desc<typename Config::cell_tag, 0>::num_elements; ++i)
          elements.insert(cell_vertices[i]);
      }
       
      
      template <dim_type dim>
      const RangeType * container() const { return &elements; }

      template <dim_type dim>
      RangeType * container() { return &elements; }

      ////////////////////// size ////////////////////////
      template <dim_type j>
      size_t size(equal_tag) const { return elements.size(); }
      
      template <dim_type j>
      size_t size() const
      {
        return size<j>( typename level_discriminator<0, j>::result_type() );
      }
      
    private:
      RangeType    elements;        //container of elements
  };

  
  template <typename Config,
            dim_type dim,
            typename handling_tag>
  class segment_layers_top : //public segment_layers<Config, 0, handling_tag>
                             public segment_layers<Config, dim - 1, handling_tag>
                             //public segment_domain_holder<Config>
  {
      typedef typename topology::subcell_desc<typename Config::cell_tag, dim>::element_tag     element_tag;
      typedef element<Config, element_tag >                                                  element_type;
      typedef typename topology::subcell_desc<typename Config::cell_tag, 0>::element_tag       vertex_tag;
      typedef element<Config, vertex_tag >                                               vertex_type;
      typedef element<Config, typename Config::cell_tag>                                 cell_type;
      typedef typename result_of::element_container< segment_t<Config>, dim, Config::cell_tag::topology_level>::type      container_type;
      typedef segment_layers<Config, dim - 1, handling_tag>                              base_type;
      //typedef segment_layers<Config, 0, handling_tag>                                    base_type;
      
    public:
      
      void add(cell_type & cell)
      {
        //add cell to domain and keep pointer in segment
        //std::cout << "Adding cell to domain and to segment" << std::endl;
        elements.push_back(base_type::domain_->add(cell));
        
        base_type::fill(*elements.back());
      }
      
      
      ///////////////////// container retrieval /////////////////////////

      //non-const:
      template <dim_type dim_container>
      typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::element_container< segment_t<Config>, dim_container, Config::cell_tag::topology_level>::type * 
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
    
      //cell_type cells(size_t i) { return *(elements[i]); }
      //cell_type cells(size_t i) const { return *(elements[i]); }      
      
    private:
      container_type elements;
  };
  
  
  
  
  //Segment type:
  template <typename Conf>
  class segment_t : public segment_layers_top<Conf, Conf::cell_tag::topology_level, full_handling_tag>
  {
    typedef element<Conf, typename Conf::cell_tag>                                   cell_type;
    typedef segment_layers_top<Conf, Conf::cell_tag::topology_level, full_handling_tag>  base_type;
    typedef domain<Conf>                                                             domain_type;
    
    public:
      typedef Conf                config_type;
      
      //void add(cell_type & e) { base_type::add(e); }      
      
      using base_type::add;

  };

  
  
  
}
#endif
