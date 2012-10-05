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
#include "viennagrid/storage/reference.hpp"

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
    
     typedef typename topology::bndcells<ElementTag, dim>::layout_tag layout_tag;
     static const long array_size = topology::bndcells<ElementTag, dim>::num;
     typedef typename result_of::ncell<ConfigType, dim>::type            element_type;
     
     typedef typename result_of::container<element_type *, layout_tag, array_size>::type      element_container_type;
     typedef typename result_of::container<ElementOrientationType, layout_tag, array_size>::type      orientation_container_type;
     
    
    

    protected:
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter)
      {
          Base::create_bnd_cells(inserter);
          topology::bndcell_generator<ElementTag, dim, typename viennagrid::result_of::ncell<ConfigType, dim>::type>::create_bnd_cells(*this, inserter );
      }

    public:
        
        //typedef Loki::Typelist< typename result_of::ncell<ConfigType, dim>::type, typename Base::required_elements > required_elements;
        
        typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;
        typedef VertexType * VertexReferenceType;
        typedef typename viennameta::typelist::result_of::push_back< typename Base::required_types, typename result_of::ncell<ConfigType, dim>::type >::type required_types;

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
      
      using Base::set_bnd_cell;
      template<typename inserted_iterator_type>
      void set_bnd_cell(const element_type & to_insert, std::pair<inserted_iterator_type, bool> inserted, unsigned int pos)
      {
          elements_[pos] = inserted.first;
          
          if (inserted.second)
              orientations_[pos].setDefaultOrientation();
          else
          {
          typedef typename result_of::const_ncell_range<element_type, 0>::type      VertexOnElementConstRange;
          typedef typename result_of::iterator<VertexOnElementConstRange>::type         VertexOnElementConstIterator;

          typedef typename result_of::ncell_range<element_type, 0>::type      VertexOnElementRange;
          typedef typename result_of::iterator<VertexOnElementRange>::type         VertexOnElementIterator;
              
   
            long i=0; dim_type j=0;
            
            
            //set orientation:
            VertexOnElementRange vertices_on_element = ncells<0>( *elements_[pos] );
            for (VertexOnElementIterator voeit = vertices_on_element.begin();
                    voeit != vertices_on_element.end();
                    ++voeit, ++i)
            {
                
                VertexOnElementConstRange vertices_on_element_2 = ncells<0>( to_insert );
                for (VertexOnElementConstIterator voeit2 = vertices_on_element_2.begin();
                        voeit2 != vertices_on_element_2.end();
                        ++voeit2, ++j)
                {
                    if (voeit->id() == voeit2->id())
                    {
                        orientations_[pos].setPermutation(i,j);
                        break;
                    }
                }
                j=0;
            }
              
          }
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
    //typedef typename result_of::element_container<element_t<ConfigType, ElementTag>, dim, ElementTag::dim>::type      container_type;
    
     typedef typename topology::bndcells<ElementTag, dim>::layout_tag layout_tag;
     static const long array_size = topology::bndcells<ElementTag, dim>::num;
     typedef typename result_of::ncell<ConfigType, dim>::type            element_type;
     
     typedef typename result_of::container<element_type *, layout_tag, array_size>::type      element_container_type;
     
    

    protected:
        
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter)
      {
          Base::create_bnd_cells(inserter);
          topology::bndcell_generator<ElementTag, dim, typename viennagrid::result_of::ncell<ConfigType, dim>::type>::create_bnd_cells(*this, inserter );
      }

    public:
        
        typedef typename result_of::ncell<ConfigType, 0>::type      VertexType;
        typedef VertexType * VertexReferenceType;
        typedef typename viennameta::typelist::result_of::push_back< typename Base::required_types, typename result_of::ncell<ConfigType, dim>::type >::type required_types;

      boundary_ncell_layer( ) 
      {
          std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
      };

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
      
      using Base::set_bnd_cell;
      template<typename inserted_iterator_type>
      void set_bnd_cell(const element_type & to_insert, std::pair<inserted_iterator_type, bool> inserted, unsigned int pos)
      {
         elements_[pos] = inserted.first;
      }

    private: 
      element_container_type elements_;
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
    typedef topology::bndcells<ElementTag, dim>                                LevelSpecs;
    typedef boundary_ncell_layer < ConfigType, ElementTag, dim - 1 >      Base;

    typedef element_t<ConfigType, typename LevelSpecs::tag>  LevelElementType;

    protected:
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter)
      {
          Base::create_bnd_cells(inserter);
      }
      
      using Base::set_bnd_cell;

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
    typedef topology::bndcells<ElementTag, 0>                                      LevelSpecs;

    
    typedef typename result_of::point<ConfigType>::type              PointType;

    typedef typename result_of::iterator< element_t<ConfigType, ElementTag>, 0>::type         VertexIterator;
    
    typedef typename topology::bndcells<ElementTag, 0>::layout_tag layout_tag;
    static const long array_size = topology::bndcells<ElementTag, 0>::num;
    typedef typename result_of::ncell<ConfigType, 0>::type            element_type;
    
    typedef typename result_of::container<element_type *, layout_tag, array_size>::type      element_container_type;
    

    public:
        
        typedef element_t<ConfigType, typename LevelSpecs::tag>         VertexType;
        typedef topology::bndcells<VertexType, 0>                                 VertexSpecs;

    protected:
      //end recursion:
      template <typename DomainType>
      void fill_level(DomainType & dom) {}
      
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter) {}
      
      template<typename inserted_iterator_type>
      void set_bnd_cell(const VertexType & to_insert, std::pair<inserted_iterator_type, bool> inserted, unsigned int pos)
      {
          vertices_[pos] = inserted.first;
      }

    public:
        
        typedef VertexType * VertexReferenceType;
        typedef VIENNAMETA_MAKE_TYPELIST_1( VertexType ) required_types;
        
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
      
      element_container_type & vertices() { return vertices_; }
      const element_container_type & vertices() const { return vertices_; }
      void vertices(VertexType ** vertices_in, size_t num = LevelSpecs::num)
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

