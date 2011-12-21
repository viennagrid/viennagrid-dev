#ifndef VIENNAGRID_IO_SERIALIZATION_HPP
#define VIENNAGRID_IO_SERIALIZATION_HPP

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

/** @file serialization.hpp
 *  @brief    A domain wrapper class which models the Boost Serialization concept: http://www.boost.org/libs/serialization/ 
 */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

namespace viennagrid
{
  namespace io
  {
    /** @brief Domain wrapper which models the Boost serialization concept 
     *
     * @tparam dummy    The Domain-Type has to be provided
     */  
    template<typename DomainT>
    struct domain_serializer
    {
    private: 
      typedef typename DomainT::config_type                                                  ConfigType;
      typedef typename DomainT::segment_type                                                 SegmentType;   
      typedef typename ConfigType::cell_tag                                                  CellTag;   
      typedef typename viennagrid::result_of::const_ncell_range<DomainT, 0>::type            VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                    VertexIterator;
      typedef typename viennagrid::result_of::point<ConfigType>::type                        PointType;   
      typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type                     VertexType;  
      typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type          CellType;    
      typedef typename viennagrid::result_of::ncell_range<SegmentType, CellTag::dim>::type   CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                      CellIterator;   
      typedef typename viennagrid::result_of::ncell_range<CellType, 0>::type                 VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type              VertexOnCellIterator;

      static const int DIMG = ConfigType::coordinate_system_tag::dim;

      friend class boost::serialization::access;

      /** @brief Save function is used to 'read' the data from the domain */
      template<class Archive>
      void save(Archive & ar, const unsigned int version) const
      {
        // -----------------------------------------------
        // the geometry is read and transmitted 
        //
        std::size_t point_size = viennagrid::ncells<0>(domain).size();
        ar & point_size;
        VertexRange vertices = viennagrid::ncells<0>(domain);
        for (VertexIterator vit = vertices.begin();
             vit != vertices.end(); ++vit)
        {
           for(int d = 0; d < DIMG; d++) ar & vit->point()[d];
        }
        // -----------------------------------------------

        // -----------------------------------------------
        // the segment size is read and transmitted 
        //
        std::size_t segment_size = domain.segments().size();
        ar & segment_size;
        // -----------------------------------------------

        // -----------------------------------------------
        // the segment specific cells are read and transmitted 
        //
        for (std::size_t si = 0; si < segment_size; si++)
        {
          SegmentType & seg = domain.segments()[si];
          CellRange cells = viennagrid::ncells<CellTag::dim>(seg);

          std::size_t cell_size = viennagrid::ncells<CellTag::dim>(seg).size();         
          ar & cell_size;

          for (CellIterator cit = cells.begin();
          cit != cells.end(); ++cit)
          {
            VertexOnCellRange vertices_on_cell = viennagrid::ncells<0>(*cit);

            for (VertexOnCellIterator vocit = vertices_on_cell.begin();
            vocit != vertices_on_cell.end();
            ++vocit)
            {
              std::size_t id = vocit->id();
              ar & id;
            }         
          }         
        }
        // -----------------------------------------------
      }
       
      /** @brief Load function is used to 'write' the data to the domain */       
      template<class Archive>
      void load(Archive & ar, const unsigned int version)
      {
        // -----------------------------------------------
        // the geometry is received and stored
        //      
        std::size_t point_size;
        ar & point_size;

        for(std::size_t i = 0; i < point_size; i++)
        {
          VertexType vertex;
          for(int d = 0; d < DIMG; d++)
            ar & vertex.point()[d];
          domain.push_back(vertex);            
        }
        // -----------------------------------------------

        // -----------------------------------------------
        // the segments are received and created
        //      
        std::size_t segment_size;
        ar & segment_size;
        domain.segments().resize(segment_size);
        // -----------------------------------------------        
        
        // -----------------------------------------------
        // the segment specific cells are received and stored
        //      
        for (std::size_t si = 0; si < segment_size; si++)
        {
          SegmentType & seg = domain.segments()[si];
          std::size_t cell_size;
          ar & cell_size;         
          for(std::size_t ci = 0; ci < cell_size; ci++)
          {
            VertexType *vertices[viennagrid::topology::bndcells<CellTag, 0>::num];

            for (int j=0; j<viennagrid::topology::bndcells<CellTag, 0>::num; ++j)
            {
              std::size_t id;
              ar & id;
              vertices[j] = &(viennagrid::ncells<0>(domain)[id]);  
            }
            CellType cell;
            cell.vertices(vertices);
            seg.push_back(cell);
          }
        }
        // -----------------------------------------------        
      }

      // -----------------------------------------------
      // notify Boost Serialization that the save and load 
      // procedures differ
      //      
      BOOST_SERIALIZATION_SPLIT_MEMBER()
      // -----------------------------------------------      
    public:
      /** @brief The constructor expects a domain object and sets the state */           
      domain_serializer(DomainT& domain) : domain(domain) {}
       
      /** @brief The functor returns a reference to the domain state*/              
      DomainT& operator()() { return domain; }

      /** @brief The state holds a reference to the domain */       
      DomainT& domain;
    };

  } //namespace io
} //namespace viennagrid

#endif

