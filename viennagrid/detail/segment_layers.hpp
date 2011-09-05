#ifndef VIENNAGRID_DETAIL_SEGMENT_LAYERS_HPP
#define VIENNAGRID_DETAIL_SEGMENT_LAYERS_HPP

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


#include <map>
#include <set>
#include <deque>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/detail/element_key.hpp"

namespace viennagrid
{

  /////////////// Segment is a container for elements ////////////////
  
  namespace detail
  {
    struct id_ptr_compare
    {
      template <typename ElementType>
      bool operator()(ElementType * e1, ElementType * e2) const
      {
        return e1->id() < e2->id();
      }
    };
    
    // holds a reference to the domain the segment belongs to
    template <typename Conf>
    class segment_domain_holder
    {
        typedef viennagrid::domain_t<Conf>                         domain_type;
      public:
        void domain(domain_type & d) { domain_ = &d; }

        domain_type & domain() { return * domain_; }
        domain_type const & domain() const { return * domain_; }

        std::size_t id() const 
        {
          for (std::size_t i=0; i<domain_->segments().size(); ++i)
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
                          public segment_layers<Config,
                                                dim - 1, 
                                                typename result_of::subelement_handling<typename Config::cell_tag,
                                                                                        dim-1>::type
                                                >
                          //public segment_domain_holder<Config>
    {
        typedef typename Config::cell_tag                                      CellTag;
        typedef typename viennagrid::topology::subelements<CellTag, dim>::element_tag     ElementTag;
        typedef viennagrid::element_t<Config, ElementTag >                                 LevelElementType;
        typedef typename viennagrid::topology::subelements<CellTag, 0>::element_tag       VertexTag; 
        typedef viennagrid::element_t<Config, VertexTag >                                  VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                   CellType;
        typedef typename viennagrid::result_of::element_container< segment_t<Config>,
                                                      dim>::type                   container_type;
        typedef segment_layers<Config,
                              dim - 1,
                              typename viennagrid::result_of::subelement_handling<typename Config::cell_tag,
                                                                                  dim-1>::type
                              >                  base_type;
        
      public:
        
        void fill(CellType & cell)
        {
          //add vertices to segment: (note that the ncells<0>() is not available here)
          LevelElementType ** level_elements = cell.template container<dim>();
          for (long i=0; i<topology::subelements<typename Config::cell_tag, dim>::num_elements; ++i)
            elements.insert(level_elements[i]);
          
          base_type::fill(cell);
        }
        
        ///////////////////// container retrieval /////////////////////////

        //non-const:
        template <dim_type dim_container>
        typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
        
        template <dim_type dim_container>
        typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(equal_tag) { return &elements; }

        template <dim_type dim_container>
        typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(less_tag) { return base_type::template container<dim_container>(); }

        //const:
        template <dim_type dim_container>
        const typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
        
        template <dim_type dim_container>
        const typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(equal_tag) const { return &elements; }

        template <dim_type dim_container>
        const typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(less_tag) const { return base_type::template container<dim_container>(); }
        
        
      private:
        container_type elements;
    };

    //specialization: no handling of elements by cells on that level. Therefore, don't handle it within segments as well.
    template <typename Config,
              dim_type dim>
    class segment_layers<Config, dim, no_handling_tag> :
                          public segment_layers<Config,
                                                dim - 1,
                                                typename viennagrid::result_of::subelement_handling<typename Config::cell_tag, dim-1>::type
                                                >
    {
        typedef typename Config::cell_tag                                      CellTag;
        typedef typename viennagrid::topology::subelements<CellTag, dim>::element_tag     ElementTag;
        typedef viennagrid::element_t<Config, ElementTag >                                 LevelElementType;
        typedef typename viennagrid::topology::subelements<CellTag, 0>::element_tag       VertexTag; 
        typedef viennagrid::element_t<Config, VertexTag >                                  VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                   CellType;
        typedef typename viennagrid::result_of::element_container< segment_t<Config>,
                                                      dim>::type                   container_type;
        typedef segment_layers<Config,
                              dim - 1,
                              typename viennagrid::result_of::subelement_handling<CellTag,
                                                                                  dim-1>::type
                              >                                                base_type;
        
      public:
        
        void fill(CellType & cell)
        {
          //nothing to do here. Just forward to lower level:
          base_type::fill(cell);
        }
        
        ///////////////////// container retrieval /////////////////////////

        //non-const:
        template <dim_type dim_container>
        typename result_of::element_container< segment_t<Config>, dim_container>::type * 
        container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
        
        template <dim_type dim_container>
        typename result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(equal_tag)
        { 
          typedef typename result_of::subelement_handling<CellTag, dim>::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
          return NULL; 
        }

        template <dim_type dim_container>
        typename result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(less_tag) { return base_type::template container<dim_container>(); }

        //const:
        template <dim_type dim_container>
        const typename result_of::element_container< segment_t<Config>, dim_container>::type * 
        container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
        
        template <dim_type dim_container>
        const typename result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(equal_tag) const
        { 
          typedef typename result_of::subelement_handling<CellTag, dim>::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
          return NULL; 
        }

        template <dim_type dim_container>
        const typename result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(less_tag) const { return base_type::template container<dim_container>(); }
        
    };
    
    //vertex level:
    template <typename Config, typename handling_tag>
    class segment_layers<Config, 0, handling_tag> : public segment_domain_holder<Config>
    {
        typedef typename Config::cell_tag                                 CellTag;
        typedef typename viennagrid::topology::subelements<CellTag, 0>::element_tag    VertexTag;
        typedef viennagrid::element_t<Config, VertexTag >                               VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                CellType;
        typedef typename result_of::element_container< segment_t<Config>, 
                                                      0>::type           ContainerType;
        
      public:
        
        void fill(CellType & cell)
        {
          //add vertices to segment: (note that the ncells<0>() is not available here)
          VertexType ** cell_vertices = cell.template container<0>();
          for (long i=0; i<viennagrid::topology::subelements<typename Config::cell_tag, 0>::num_elements; ++i)
            elements.insert(cell_vertices[i]);
        }
        
        
        template <dim_type dim>
        const ContainerType * container() const { return &elements; }

        template <dim_type dim>
        ContainerType * container() { return &elements; }

      private:
        ContainerType    elements;        //container of elements
    };

    
    template <typename Config,
              dim_type dim>
    class segment_layers_top : public segment_layers<Config,
                                                    dim - 1,
                                                    typename result_of::subelement_handling<typename Config::cell_tag, dim-1>::type
                                                    >
    {
        typedef typename Config::cell_tag                                                        CellTag;
        typedef typename viennagrid::topology::subelements<typename Config::cell_tag, dim>::element_tag     ElementTag;
        typedef viennagrid::element_t<Config, ElementTag >                                                  ElementType;
        typedef typename viennagrid::topology::subelements<typename Config::cell_tag, 0>::element_tag       VertexTag;
        typedef viennagrid::element_t<Config, VertexTag >                                               VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                                 CellType;
        typedef typename viennagrid::result_of::element_container< segment_t<Config>, dim>::type      container_type;
        typedef segment_layers<Config,
                              dim - 1,
                              typename viennagrid::result_of::subelement_handling<typename Config::cell_tag,
                                                                                  dim-1>::type
                              >                                                              base_type;
        
      public:
        
        void push_back(CellType & cell)
        {
          //add cell to domain and keep pointer in segment
          //std::cout << "Adding cell to domain and to segment" << std::endl;
          elements.push_back(base_type::domain_->push_back(cell));
          
          base_type::fill(*elements.back());
        }
        
        
        ///////////////////// container retrieval /////////////////////////

        //non-const:
        template <dim_type dim_container>
        typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
        
        template <dim_type dim_container>
        typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(equal_tag) { return &elements; }

        template <dim_type dim_container>
        typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(less_tag) { return base_type::template container<dim_container>(); }

        //const:
        template <dim_type dim_container>
        const typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
        
        template <dim_type dim_container>
        const typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(equal_tag) const { return &elements; }

        template <dim_type dim_container>
        const typename viennagrid::result_of::element_container< segment_t<Config>, dim_container>::type * 
        container(less_tag) const { return base_type::template container<dim_container>(); }
        
      private:
        container_type elements;
    };
  
  } //namespace detail


  // a few metafunctions returning the correct container for segments:
  namespace result_of
  {
    //at cell level
    template <typename config_type, dim_type cell_level>
    struct element_container< segment_t<config_type>, cell_level, cell_level>
    {
      typedef typename viennagrid::result_of::ncell<config_type, cell_level>::type     element_type;
      
      typedef std::deque< element_type * >      type;
    };

    //at vertex level
    template <typename config_type, dim_type cell_level>
    struct element_container< segment_t<config_type>, 0, cell_level>
    {
      typedef typename viennagrid::result_of::ncell<config_type, 0>::type     element_type;
      
      typedef std::set< element_type *, viennagrid::detail::id_ptr_compare >      type; //note that with the use of deque, pointer comparison does not induce a valid ordering!
    };

    //at any other level:
    template <typename config_type, dim_type dim, dim_type cell_level>
    struct element_container< segment_t<config_type>, dim, cell_level>
    {
      typedef typename viennagrid::result_of::ncell<config_type, dim>::type     element_type;
      
      typedef std::set< element_type * >      type;
    };
    
    template <typename ConfigType>
    struct segment
    {
      typedef segment_t<ConfigType>     type; 
    };
    
    
  } //namespace result_of

} //namespace viennagrid
#endif
