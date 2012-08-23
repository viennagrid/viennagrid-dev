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
    
    typedef typename topology::bndcells<ElementTag, dim>::layout_tag layout_tag;
    static const long array_size = topology::bndcells<ElementTag, dim>::num;
    typedef typename result_of::ncell<ConfigType, dim>::type            element_type;
    
    typedef typename result_of::container<element_type *, layout_tag, array_size>::type      element_container_type;
    typedef typename result_of::container<ElementOrientationType, layout_tag, array_size>::type      orientation_container_type;
    
    typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower level first:
        Base::fill_level(dom);

        //for (long i=0; i<LevelSpecs::num; ++i)
        //  orientations_[i].resize(VertexOnElementSpecs::num);

        topology::bndcell_filler<ElementTag, dim>::fill(elements_,
                                                           Base::vertices_,
                                                           orientations_,
                                                           dom);
      }

    public:

      boundary_ncell_layer( ) 
      {
          std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
      };

      boundary_ncell_layer( const boundary_ncell_layer & llh) : Base (llh), elements_(llh.elements_), orientations_(llh.orientations_) {}

      /////////////////// access container: ////////////////////

      using Base::container;

      //non-const:
      element_container_type *
      container(dimension_tag<dim>)
      { 
        return &elements_;
      }
      
      //const:
      
      const element_container_type *
      container(dimension_tag<dim>) const
      { 
        return &elements_;
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
      
      void fill_vertices(VertexType ** vertices_, size_t num)
      {
            Base::fill_vertices(vertices_, num);
      }

    private: 
      element_container_type elements_;
      orientation_container_type orientations_;
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
    
    typedef typename topology::bndcells<ElementTag, dim>::layout_tag layout_tag;
    static const long array_size = topology::bndcells<ElementTag, dim>::num;
    typedef typename result_of::ncell<ConfigType, dim>::type            element_type;
    
    typedef typename result_of::container<element_type *, layout_tag, array_size>::type      element_container_type;
    
    typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        typedef ElementOrientationType *    OrientationPointer;
        //fill lower level first:
        Base::fill_level(dom);

        topology::bndcell_filler<ElementTag, dim>::fill(elements_,
                                                           Base::vertices_,
                                                           dom);
      }

    public:

      boundary_ncell_layer( ) 
      {
          std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
      }

      boundary_ncell_layer( const boundary_ncell_layer & llh) : Base (llh), elements_(llh.elements) {}

      /////////////////// access container: ////////////////////

      using Base::container;

      //non-const:
      element_container_type *
      container(dimension_tag<dim>)
      { 
        return &elements_;
      }
      
      //const:
      const element_container_type *
      container(dimension_tag<dim>) const
      { 
        return &elements_;
      }
      
      void fill_vertices(VertexType ** vertices_, size_t num)
      {
            Base::fill_vertices(vertices_, num);
      }

    private: 
      element_container_type elements_;//[LevelSpecs::num];
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
    
    typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower topological levels only:
        Base::fill_level(dom);
      }
      
      void fill_vertices(VertexType ** vertices_, size_t num)
      {
            Base::fill_vertices(vertices_, num);
      }

    public:

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

    typedef element_t<ConfigType, typename LevelSpecs::tag>         VertexType;
    typedef typename result_of::point<ConfigType>::type              PointType;

    typedef typename result_of::iterator< element_t<ConfigType, ElementTag>, 0>::type         VertexIterator;

    typedef typename topology::bndcells<ElementTag, 0>::layout_tag layout_tag;
    static const long array_size = topology::bndcells<ElementTag, 0>::num;
    typedef typename result_of::ncell<ConfigType, 0>::type            element_type;
    
    typedef typename result_of::container<element_type *, layout_tag, array_size>::type      element_container_type;
    
    protected:
      //end recursion:
      template <typename DomainType>
      void fill_level(DomainType & dom) {}

    public:
      boundary_ncell_layer() {};

      boundary_ncell_layer( const boundary_ncell_layer & llh) : vertices_(llh.vertices_) {}

      ////////////////// container access: /////////////////////////
      
      //non-const:
      element_container_type *
      container(dimension_tag<0>)
      { 
        return &vertices_;
      }

      //const:
      const element_container_type *
      container(dimension_tag<0>) const
      { 
        return &vertices_;
      }
      
      void fill_vertices(VertexType ** vertices_in, size_t num)
      {
            assert( num <= LevelSpecs::num );
            vertices_.resize(num);
            std::copy( vertices_in, vertices_in + num, vertices_.begin() );
      }


    protected:
      element_container_type vertices_;
  };

  

}



#endif

