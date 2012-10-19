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
#include "viennagrid/storage/hook.hpp"

/** @file boundary_ncell_layer.hpp
    @brief Provides the topological layers for n-cells
*/


namespace viennagrid
{
    
    
    
    namespace viennagrid_ng
    {
               
        template<typename _id_type = long>
        struct id_element
        {
        public:
            typedef _id_type id_type;
            
            void id(id_type _id) { id_ = _id; }
            id_type id() const { return id_; }

        private:
            id_type id_;
        };
        
        
        
        template<typename element_tag, typename bnd_cell_container_type__, typename orientation_container_type__>
        class boundary_ncell_layer
        {
        public:
            
            typedef bnd_cell_container_type__ bnd_cell_container_type;
            typedef typename bnd_cell_container_type::value_type bnd_cell_type;

            typedef typename bnd_cell_type::tag bnd_cell_tag;
            enum { dim = bnd_cell_tag::dim };
            
            typedef orientation_container_type__ orientation_container_type;
            typedef typename orientation_container_type::value_type orientation_type;
            
            
            
            boundary_ncell_layer( ) 
            {
                //std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
            };

            boundary_ncell_layer( const boundary_ncell_layer & llh) : elements_(llh.elements_), orientations_(llh.orientations_) {}
            
        protected:
            
            template<typename inserter_type>
            void create_bnd_cells(inserter_type & inserter)
            {
                topology::bndcell_generator<element_tag, dim, bnd_cell_type>::create_bnd_cells(*this, inserter );
            }

            bnd_cell_container_type &
            container(dimension_tag<dim>)
            { 
                return elements_;
            }

            //const:     
            const bnd_cell_container_type &
            container(dimension_tag<dim>) const
            { 
                return elements_;
            }

            template<typename inserted_iterator_type>
            void set_bnd_cell(const bnd_cell_type & to_insert, std::pair<inserted_iterator_type, bool> inserted, unsigned int pos)
            {
                elements_[pos] = inserted.first;

                if (inserted.second)
                    orientations_[pos].setDefaultOrientation();
                else
                {
                    typedef typename result_of::const_ncell_range<bnd_cell_type, 0>::type      VertexOnElementConstRange;
                    typedef typename result_of::iterator<VertexOnElementConstRange>::type         VertexOnElementConstIterator;

                    typedef typename result_of::ncell_range<bnd_cell_type, 0>::type      VertexOnElementRange;
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
            
        private:
            
            bnd_cell_container_type elements_;
            orientation_container_type orientations_;
        };
        
        
        
        
        template<typename element_tag, typename bnd_cell_container_type__>
        class boundary_ncell_layer<element_tag, bnd_cell_container_type__, viennameta::null_type>
        {
        public:
            
            typedef bnd_cell_container_type__ bnd_cell_container_type;
            typedef typename bnd_cell_container_type::value_type bnd_cell_type;
            typedef typename bnd_cell_type::tag bnd_cell_tag;
            enum { dim = bnd_cell_tag::dim };
            
            
            
            boundary_ncell_layer( ) 
            {
                //std::fill( elements_.begin(), elements_.end(), static_cast<element_type *>(NULL) );
            };

            boundary_ncell_layer( const boundary_ncell_layer & llh) : elements_(llh.elements_) {}
            
        protected:
            
            template<typename inserter_type>
            void create_bnd_cells(inserter_type & inserter)
            {
                topology::bndcell_generator<element_tag, dim, bnd_cell_type>::create_bnd_cells(*this, inserter );
            }

            bnd_cell_container_type &
            container(dimension_tag<dim>)
            { 
                return elements_;
            }

            //const:     
            const bnd_cell_container_type &
            container(dimension_tag<dim>) const
            { 
                return elements_;
            }

            template<typename inserted_iterator_type>
            void set_bnd_cell(const bnd_cell_type & to_insert, std::pair<inserted_iterator_type, bool> inserted, unsigned int pos)
            {
                elements_[pos] = inserted.first;
            }
            
        private:
            
            bnd_cell_container_type elements_;
        };
        
        
        
        template<typename element_tag, typename orienter_container_type__>
        class boundary_ncell_layer<element_tag, viennameta::null_type, orienter_container_type__>
        {
        public:
        protected:
            template<typename inserter_type>
            void create_bnd_cells(inserter_type & inserter)
            {}
            
        private:
        };
        
        
        
        
        
        
        template<typename element_tag, typename bnd_cell_container_typelist>
        class boundary_ncell_layer_unwrapper
        {};
        
        template<typename element_tag, typename head, typename tail>
        class boundary_ncell_layer_unwrapper<element_tag, viennameta::typelist_t<head, tail> > :
            public boundary_ncell_layer<element_tag, typename head::first, typename head::second>
        {
        
        private:
            
            typedef boundary_ncell_layer<element_tag, typename head::first, typename head::second> base;
            
        public:
            
            using base::create_bnd_cells;
            using base::container;
            using base::set_bnd_cell;
            
        protected:
        };
        
        template<typename element_tag>
        class boundary_ncell_layer_unwrapper<element_tag, viennameta::null_type>
        {};
        
        
        
        
        template<typename element_tag, typename bnd_cell_container_typelist__>
        class element_t : public id_element<>,
            public boundary_ncell_layer_unwrapper<element_tag, bnd_cell_container_typelist__>
        {
            typedef boundary_ncell_layer_unwrapper<element_tag, bnd_cell_container_typelist__> base;
            
        public:
            
            typedef bnd_cell_container_typelist__ bnd_cell_container_typelist;
            typedef element_tag tag;
            
            
            template<typename inserter_type>
            void insert_callback( inserter_type & inserter, bool inserted )
            {
                if (inserted) base::create_bnd_cells(inserter);
            }
            
        protected:
            
        private:
        };
        
        
//         /** @brief Overload for the output streaming operator */
//         template <typename ConfigType, typename ElementTag>
//         std::ostream & operator<<(std::ostream & os, element_t<ConfigType, ElementTag> const & el)
//         {
//             typedef element_t<ConfigType, ElementTag>                ElementType;
//             typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type    VertexRange;
//             typedef typename viennagrid::result_of::iterator<VertexRange>::type          VertexIterator;
// 
//             os << "-- " << ElementTag::name() << ", ID: " << el.id() << " --";
//             VertexRange vertices = viennagrid::ncells<0>(el);
//             for (VertexIterator vit  = vertices.begin();
//                                 vit != vertices.end();
//                                 ++vit)
//             os << std::endl << "  " << *vit;
// 
//             return os;
//         }
        
        
        template<typename point_type__>
        class vertex_t : public id_element<>
        {
        public:
            
            typedef point_type__ point_type;
            typedef point_tag tag;
            typedef typename point_type::value_type coord_type;
            enum { dim = 0 };
            
            template<typename inserter_type>
            void insert_callback( inserter_type & inserter, bool inserted )
            {}
            
            /** @brief Provide access to the geometrical point object defining the location of the vertex in space */
            point_type & point() { return point_; }
            /** @brief Provide access to the geometrical point object defining the location of the vertex in space. const-version. */
            point_type const & point() const { return point_; }
            
            //convenience access to coordinates of the vertex:
            /** @brief Convenience access to the coordinates of the point */
            coord_type & operator[](std::size_t i) { return point_[i]; }
            /** @brief Convenience access to the coordinates of the point. const-version*/
            coord_type const & operator[](std::size_t i) const { return point_[i]; }
            
            /** @brief Convenience forward for obtaining the geometrical dimension of the underlying Euclidian space. */
            std::size_t size() const { return point_.size(); }

            /** @brief Convenience less-than comparison function, forwarding to the point */
            bool operator<(const vertex_t e2) const { return point_ < e2.point_; }
            /** @brief Convenience greater-than comparison function, forwarding to the point */
            bool operator>(const vertex_t e2) const { return point_ > e2.point_; }
            
        protected:
        private:
            point_type point_;
        };
    
    
        /** @brief Overload for the output streaming operator for the vertex type */
        template <typename point_type>
        std::ostream & operator<<(std::ostream & os, vertex_t<point_type> const & el)
        {
            os << "-- Vertex, ID: " << el.id() << "; Point: " << el.point();

            return os;
        }

        
        
        
        namespace result_of
        {
            
            template<typename typelist, long dim>
            struct container_of_dimension;
            
            template<long dim>
            struct container_of_dimension< viennameta::null_type, dim >
            {
                typedef viennameta::null_type type;
            };
            
            template<typename container_pair, typename tail, long dim>
            struct container_of_dimension< viennameta::typelist_t<container_pair, tail>, dim >
            {
                typedef typename container_pair::first container_type;
                typedef typename container_type::value_type value_type;
                //typedef typename viennagrid::storage::reference::value_type_from_reference_type<reference_type>::type value_type;

                
                typedef typename viennameta::_if<
                    value_type::dim == dim,
                    container_type,
                    typename container_of_dimension<tail, dim>::type
                >::type type;
            };
                        
            template<typename element_tag, typename bnd_cell_container_typelist, long dim>
            struct container_of_dimension< element_t<element_tag, bnd_cell_container_typelist>, dim >
            {
                typedef typename container_of_dimension<bnd_cell_container_typelist, dim>::type type;
            };
        }
        
        
        template<long dim, typename element_type>
        typename result_of::container_of_dimension<element_type, dim>::type & ncells(element_type & element)
        {
            return element.container( dimension_tag<dim>() );
        }
        
    
    
    }
    
    
    
    
    
    
    
    
    
  
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
      template<typename inserter_type>
      void create_bnd_cells(inserter_type & inserter) {}
      
      template<typename inserted_iterator_type>
      void set_bnd_cell(const VertexType & to_insert, std::pair<inserted_iterator_type, bool> inserted, unsigned int pos)
      {
          vertices_[pos] = inserted.first;
      }

    public:
        
        typedef VertexType * VertexReferenceType;
        //typedef VIENNAMETA_MAKE_TYPELIST_1( VertexType ) required_types;
        typedef typename viennameta::make_typelist< VertexType >::type required_types;
        
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

