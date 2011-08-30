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


#ifndef VIENNAGRID_IO_DATA_ACCESSOR_HPP
#define VIENNAGRID_IO_DATA_ACCESSOR_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include "viennagrid/domain.hpp"
#include "viennagrid/forwards.h"
#include "viennagrid/iterators.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/traits/container.hpp"
//#include "viennagrid/algorithm/cell_normals.hpp"
#include "viennadata/api.hpp"

namespace viennagrid
{
  namespace io
  {
    
    
    //
    // type erasure
    //
    template <typename ElementType>      //either cell type or vertex type
    class data_accessor_interface
    {
      public: 
        virtual std::string operator()(ElementType const & element, std::size_t segment_id) const = 0;
        virtual void operator()(ElementType const & element, std::size_t segment_id, std::size_t k, double value) const = 0;
        
        //virtual bool active_on_segment(size_t i) const { return true; }
        
        virtual data_accessor_interface<ElementType> * clone() const = 0;
    };

    
    
    //
    // Use case 1: Access data for all elements
    //
    template <typename ElementType, typename KeyType, typename DataType>
    class global_scalar_data_accessor : public data_accessor_interface<ElementType>
    {
      typedef global_scalar_data_accessor<ElementType, KeyType, DataType>    self_type;
      
      public:
        global_scalar_data_accessor(KeyType const & k) : key_(k) {}
        //vtk_data_accessor_global() {}

        std::string operator()(ElementType const & element, std::size_t segment_id) const
        {
          std::stringstream ss;
          ss << viennadata::access<KeyType, DataType>(key_)(element);
          return ss.str();
        }
        
        void operator()(ElementType const & element, std::size_t segment_id, std::size_t k, double value) const
        {
          viennadata::access<KeyType, DataType>(key_)(element) = value;
        }
        
        data_accessor_interface<ElementType> * clone() const { return new self_type(key_); }
        
      private:
        KeyType key_;
    };

    template <typename ElementType, typename KeyType, typename DataType>
    class global_vector_data_accessor : public data_accessor_interface<ElementType>
    {
      typedef global_vector_data_accessor<ElementType, KeyType, DataType>    self_type;
      
      public:
        global_vector_data_accessor(KeyType const & k) : key_(k) {}
        //vtk_data_accessor_global() {}

        std::string operator()(ElementType const & element, std::size_t segment_id) const
        {
          std::stringstream ss;
          viennagrid::traits::resize(viennadata::access<KeyType, DataType>(key_)(element), 3);
          ss << viennadata::access<KeyType, DataType>(key_)(element)[0] << " ";
          ss << viennadata::access<KeyType, DataType>(key_)(element)[1] << " ";
          ss << viennadata::access<KeyType, DataType>(key_)(element)[2];
          return ss.str();
        }
        
        void operator()(ElementType const & element, std::size_t segment_id, std::size_t k, double value) const
        {
          viennagrid::traits::resize(viennadata::access<KeyType, DataType>(key_)(element), 3);
          viennadata::access<KeyType, DataType>(key_)(element)[k] = value;
        }
        
        data_accessor_interface<ElementType> * clone() const { return new self_type(key_); }
        
      private:
        KeyType key_;
    };

    
    //
    // Use case 2: Access data based on segments.
    //
    template <typename ElementType, typename KeyType, typename DataType>
    class segment_scalar_data_accessor : public data_accessor_interface<ElementType>
    {
      typedef segment_scalar_data_accessor<ElementType, KeyType, DataType>    self_type;
      
      public:
        segment_scalar_data_accessor(KeyType const & k) : key_(k) {}
        //vtk_data_accessor_global() {}

        std::string operator()(ElementType const & element, std::size_t segment_id) const
        {
          std::stringstream ss;
          ss << viennadata::access<KeyType, DataType>(key_)(element)[segment_id];
          return ss.str();
        }

        void operator()(ElementType const & element, std::size_t segment_id, std::size_t k, double value) const
        {
          viennadata::access<KeyType, DataType>(key_)(element)[segment_id] = value;
        }

        data_accessor_interface<ElementType> * clone() const { return new self_type(key_); }
        
      private:
        KeyType key_;
    };

    template <typename ElementType, typename KeyType, typename DataType>
    class segment_vector_data_accessor : public data_accessor_interface<ElementType>
    {
      typedef segment_vector_data_accessor<ElementType, KeyType, DataType>    self_type;
      
      public:
        segment_vector_data_accessor(KeyType const & k) : key_(k) {}
        //vtk_data_accessor_global() {}

        std::string operator()(ElementType const & element, std::size_t segment_id) const
        {
          std::stringstream ss;
          viennagrid::traits::resize(viennadata::access<KeyType, DataType>(key_)(element)[segment_id], 3);
          //ss << viennadata::access<KeyType, DataType>(key_)(element)[segment_id];
          ss << viennadata::access<KeyType, DataType>(key_)(element)[segment_id][0] << " ";
          ss << viennadata::access<KeyType, DataType>(key_)(element)[segment_id][1] << " ";
          ss << viennadata::access<KeyType, DataType>(key_)(element)[segment_id][2];
          return ss.str();
        }

        void operator()(ElementType const & element, std::size_t segment_id, std::size_t k, double value) const
        {
          viennagrid::traits::resize(viennadata::access<KeyType, DataType>(key_)(element)[segment_id], 3);
          viennadata::access<KeyType, DataType>(key_)(element)[segment_id][k] = value;
        }

        data_accessor_interface<ElementType> * clone() const { return new self_type(key_); }
        
      private:
        KeyType key_;
    };
    
    //
    // The final wrapper class for any IO implementations
    //
    template <typename ElementType>
    class data_accessor_wrapper
    {
      public:
        template <typename T>
        data_accessor_wrapper(T const * t) : functor_(t) {}
        
        data_accessor_wrapper() {}

        data_accessor_wrapper(const data_accessor_wrapper & other) : functor_(other.clone()) {}

        data_accessor_wrapper & operator=(data_accessor_wrapper & other)
        {
          functor_ = other.functor_;
          return *this;
        }
        
        std::string operator()(ElementType const & element, std::size_t segment_id = 0) const
        {   
          return functor_->operator()(element, segment_id); 
        }

        void operator()(ElementType const & element, std::size_t segment_id, std::size_t k, double value) const
        {   
          functor_->operator()(element, segment_id, k, value);
        }

        data_accessor_interface<ElementType> * clone() const { return functor_->clone(); }

      private:
        std::auto_ptr< const data_accessor_interface<ElementType> > functor_;
    };

  } //namespace io
} //namespace viennagrid

#endif
