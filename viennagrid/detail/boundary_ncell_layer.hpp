#ifndef VIENNAGRID_DETAIL_LOWER_LEVEL_HOLDER_HPP
#define VIENNAGRID_DETAIL_LOWER_LEVEL_HOLDER_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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


#include <iostream>

#include "viennagrid/forwards.h"
//#include "viennagrid/topology/point.hpp"
//#include "viennagrid/iterators.hpp"
#include "viennagrid/detail/element_orientation.hpp"

#include <vector>

#include "viennagrid/meta/typelist.hpp"

/** @file boundary_ncell_layer.hpp
    @brief Provides the topological layers for n-cells
*/


namespace viennagrid
{
  
  /************** Level 1: Elements contained by a higher-level element *******/

  /** @brief A class holding all information about boundary k-cells of a n-cell
   * 
   * @tparam ConfigType       The configuration class
   * @tparam ElementTag       The n-cell tag
   * @tparam dim              Topological dimension k of the boundary k-cells
   * @tparam handling_tag     Whether or not to store references to boundary k-cells
   * @tparam orienter_tag     Whether or not to store orientations of k-cells with respect to the n-cell
   * @tparam LevelNull        Helper parameter to avoid ambiguities at vertex level.
   */
  template <typename ConfigType,
              typename ElementTag,
              unsigned long dim,
              typename handling_tag = typename result_of::bndcell_handling<ConfigType, ElementTag, dim>::type,
              typename orienter_tag = typename result_of::bndcell_orientation<ConfigType, ElementTag, dim>::type,
              bool LevelNull = (dim == 0)>
  class boundary_ncell_layer  { };

  
  //
  // Full storage of boundary cell, including orientation
  //
  /** @brief Implementation of full storage of k-cells including orientations */
  template <typename ConfigType, typename ElementTag, unsigned long dim>
  class boundary_ncell_layer <ConfigType, ElementTag, dim, full_handling_tag, full_handling_tag, false> :
    public boundary_ncell_layer <ConfigType, ElementTag, dim - 1>
  {
    //requirements:
    //array of pointers to elements of class 'dim' and a integer representing the orientation within the cell relative to the element it points to.
    typedef topology::bndcells<ElementTag, dim>                         LevelSpecs;
    typedef topology::bndcells<typename LevelSpecs::tag, 0>  VertexOnElementSpecs;
    typedef boundary_ncell_layer <ConfigType, ElementTag, dim - 1 >      Base;

    typedef element_t<ConfigType, typename LevelSpecs::tag>  LevelElementType;
    typedef element_orientation<VertexOnElementSpecs::num>     ElementOrientationType;
    typedef typename result_of::element_container<element_t<ConfigType, ElementTag>, dim, ElementTag::dim>::type      container_type;
    
    
    

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower level first:
        Base::fill_level(dom);

        //for (long i=0; i<LevelSpecs::num; ++i)
        //  orientations_[i].resize(VertexOnElementSpecs::num);

        topology::bndcell_filler<ElementTag, dim>::fill(&(elements_[0]),
                                                           &(Base::vertices_[0]),
                                                           &(orientations_[0]),
                                                           dom);
      }
      
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter)
      {
          Base::create_bnd_cells(inserter);
          //topology::bndcell_filler<ElementTag, dim>::template create_bnd_cells(*this, inserter, typename viennagrid::result_of::ncell<ConfigType, dim>::type() );
          topology::bndcell_generator<ElementTag, dim, typename viennagrid::result_of::ncell<ConfigType, dim>::type>::create_bnd_cells(*this, inserter );
      }

    public:
        
        //typedef Loki::Typelist< typename result_of::ncell<ConfigType, dim>::type, typename Base::required_elements > required_elements;
        
        typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;
        typedef VertexType * VertexReferenceType;
        typedef typename viennameta::typelist::result_of::push_back< typename Base::required_types, typename result_of::ncell<ConfigType, dim>::type >::type required_types;

      boundary_ncell_layer( ) 
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          elements_[i] = NULL;
      };

      boundary_ncell_layer( const boundary_ncell_layer & llh) : Base (llh)
      {
        //for (long i=0; i < LevelSpecs::num; ++i)
        //  elements_[i] = llh.elements_[i];
        std::copy(llh.elements_,     llh.elements_ + LevelSpecs::num,     elements_);
        std::copy(llh.orientations_, llh.orientations_ + LevelSpecs::num, orientations_);
      }

      /////////////////// access container: ////////////////////

      using Base::container;

      //non-const:
      container_type *
      container(dimension_tag<dim>)
      { 
        return &(elements_[0]);
      }
      
      //const:
      
      const container_type *
      container(dimension_tag<dim>) const
      { 
        return &(elements_[0]);
      }
      
      using Base::set_element;
      void set_element(const container_type & to_insert, container_type & inserted, unsigned int pos)
      {
         elements_[pos] = inserted;
      }

      ////////////////// orientation: ////////////////////
      std::size_t global_to_local_orientation(LevelElementType const & el, long index) const
      { 
        for (std::size_t i=0; i<LevelSpecs::num; ++i)
        {
          if (elements_[i] == &el)
            return orientations_[i](index);
        }
        assert(false && "Provided k-cell is not a boundary element of the hosting n-cell!");
        return index;
      }

    private: 
      container_type elements_[LevelSpecs::num];
      ElementOrientationType orientations_[LevelSpecs::num];
  };

  
  
  
  //
  // Full storage of boundary cell, but no orientation
  //
  /** @brief Implementation of boundary k-cell storage without orientation */
  template <typename ConfigType, typename ElementTag, unsigned long dim>
  class boundary_ncell_layer <ConfigType, ElementTag, dim, full_handling_tag, no_handling_tag, false> :
    public boundary_ncell_layer <ConfigType, ElementTag, dim - 1>
  {
    //requirements:
    //array of pointers to elements of class 'dim' and a integer representing the orientation within the cell relative to the element it points to.
    typedef topology::bndcells<ElementTag, dim>                         LevelSpecs;
    typedef topology::bndcells<typename LevelSpecs::tag, 0>  VertexOnElementSpecs;
    typedef boundary_ncell_layer <ConfigType, ElementTag, dim - 1 >      Base;

    typedef element_t<ConfigType, typename LevelSpecs::tag>  LevelElementType;
    typedef element_orientation<VertexOnElementSpecs::num>     ElementOrientationType;
    typedef typename result_of::element_container<element_t<ConfigType, ElementTag>, dim, ElementTag::dim>::type      container_type;
    
    

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        typedef ElementOrientationType *    OrientationPointer;
        //fill lower level first:
        Base::fill_level(dom);

        topology::bndcell_filler<ElementTag, dim>::fill(&(elements_[0]),
                                                           &(Base::vertices_[0]),
                                                           OrientationPointer(NULL),
                                                           dom);
      }
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter)
      {
          Base::create_bnd_cells(inserter);
          //topology::bndcell_filler<ElementTag, dim>::template create_bnd_cells(*this, inserter, typename viennagrid::result_of::ncell<ConfigType, dim>::type());
          topology::bndcell_generator<ElementTag, dim, typename viennagrid::result_of::ncell<ConfigType, dim>::type>::create_bnd_cells(*this, inserter );
      }

    public:
        
        //typedef Loki::Typelist< typename result_of::ncell<ConfigType, dim>::type, typename Base::required_elements > required_elements;
        
        typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;
        typedef VertexType * VertexReferenceType;
        typedef typename viennameta::typelist::result_of::push_back< typename Base::required_types, typename result_of::ncell<ConfigType, dim>::type >::type required_types;

      boundary_ncell_layer( ) 
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          elements_[i] = NULL;
      };

      boundary_ncell_layer( const boundary_ncell_layer & llh) : Base (llh)
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          elements_[i] = llh.elements_[i];
      }

      /////////////////// access container: ////////////////////

      using Base::container;

      //non-const:
      container_type *
      container(dimension_tag<dim>)
      { 
        return &(elements_[0]);
      }
      
      //const:
      const container_type *
      container(dimension_tag<dim>) const
      { 
        return &(elements_[0]);
      }
      
      using Base::set_element;
      void set_element(const container_type & to_insert, container_type & inserted, unsigned int pos)
      {
         elements_[pos] = inserted;
      }

    private: 
      container_type elements_[LevelSpecs::num];
  };

  
  
  //
  // No storage of boundary elements:
  //
  /** @brief Implementation of the case that boundary k-cells are not stored at all */
  template <typename ConfigType, typename ElementTag, unsigned long dim, typename orienter_tag>
  class boundary_ncell_layer <ConfigType, ElementTag, dim, no_handling_tag, orienter_tag, false> :
    public boundary_ncell_layer < ConfigType, ElementTag, dim - 1 >
  {
    //requirements:
    //array of pointers to elements of class 'dim' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<ConfigType>::segment_type        SegmentType;
    typedef topology::bndcells<ElementTag, dim>                                LevelSpecs;
    typedef boundary_ncell_layer < ConfigType, ElementTag, dim - 1 >      Base;

    typedef element_t<ConfigType, typename LevelSpecs::tag>  LevelElementType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower topological levels only:
        Base::fill_level(dom);
      }
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter)
      {
          Base::create_bnd_cells(inserter);
      }
      
      using Base::set_element;

    public:
        
    typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;
    typedef VertexType * VertexReferenceType;
    typedef typename Base::required_elements required_elements;

      boundary_ncell_layer( ) {};

      boundary_ncell_layer( const boundary_ncell_layer & llh) : Base (llh) {}
      

  };


  //at level 0, i.e. vertex level, recursion ends:
  /** @brief Specialization for the vertex level. Recursion ends here */
  template <typename ConfigType, typename ElementTag, typename handling_tag, typename orienter_tag>
  class boundary_ncell_layer <ConfigType, ElementTag, 0, handling_tag, orienter_tag, true> 
  {
    //array of pointers to elements of class 'dim' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<ConfigType>::segment_type               SegmentType;
    typedef topology::bndcells<ElementTag, 0>                                      LevelSpecs;

    
    typedef typename result_of::point<ConfigType>::type              PointType;

    typedef typename result_of::iterator< element_t<ConfigType, ElementTag>, 0>::type         VertexIterator;

    protected:
      //end recursion:
      template <typename DomainType>
      void fill_level(DomainType & dom) {}
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter) {}
      
      void set_element(const VertexType & to_insert, VertexType & inserted, unsigned int pos)
      {
         elements_[pos] = inserted;
      }

    public:
        
        typedef element_t<ConfigType, typename LevelSpecs::tag>         VertexType;
        typedef VertexType * VertexReferenceType;
        typedef VIENNAMETA_MAKE_TYPELIST_1( VertexType ) required_types;
        
      boundary_ncell_layer() {};

      boundary_ncell_layer( const boundary_ncell_layer & llh)
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          vertices_[i] = llh.vertices_[i];
      }

      ////////////////// container access: /////////////////////////
      
      VertexType * vertices( unsigned int pos ) { return vertices_[pos]; }
      
      //non-const:
      VertexType * *
      container(dimension_tag<0>)
      { 
        return &(vertices_[0]);
      }

      //const:
      VertexType * const *
      container(dimension_tag<0>) const
      { 
        return &(vertices_[0]);
      }


    protected:
      VertexType * vertices_[LevelSpecs::num];
  };



}


#endif

