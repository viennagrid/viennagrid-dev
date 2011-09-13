#ifndef VIENNAGRID_DETAIL_LOWER_LEVEL_HOLDER_HPP
#define VIENNAGRID_DETAIL_LOWER_LEVEL_HOLDER_HPP

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


#include <iostream>

#include "viennagrid/forwards.h"
//#include "viennagrid/topology/point.hpp"
//#include "viennagrid/iterators.hpp"
#include "viennagrid/detail/element_orientation.hpp"

#include <vector>

namespace viennagrid
{
  
  /************** Level 1: Elements contained by a higher-level element *******/

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

    public:

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

    public:

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

    private: 
      container_type elements_[LevelSpecs::num];
  };

  
  
  //
  // No storage of boundary elements:
  //
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

    public:

      boundary_ncell_layer( ) {};

      boundary_ncell_layer( const boundary_ncell_layer & llh) : Base (llh) {}
      

  };


  //at level 0, i.e. vertex level, recursion ends:
  template <typename ConfigType, typename ElementTag, typename handling_tag, typename orienter_tag>
  class boundary_ncell_layer <ConfigType, ElementTag, 0, handling_tag, orienter_tag, true> 
  {
    //array of pointers to elements of class 'dim' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<ConfigType>::segment_type               SegmentType;
    typedef topology::bndcells<ElementTag, 0>                                      LevelSpecs;

    typedef element_t<ConfigType, typename LevelSpecs::tag>         VertexType;
    typedef typename result_of::point<ConfigType>::type              PointType;

    typedef typename result_of::iterator< element_t<ConfigType, ElementTag>, 0>::type         VertexIterator;

    protected:
      //end recursion:
      template <typename DomainType>
      void fill_level(DomainType & dom) {}

    public:
      boundary_ncell_layer() {};

      boundary_ncell_layer( const boundary_ncell_layer & llh)
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          vertices_[i] = llh.vertices_[i];
      }

      ////////////////// container access: /////////////////////////
      
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

