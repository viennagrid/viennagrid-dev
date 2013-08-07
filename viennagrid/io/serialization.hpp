#ifndef VIENNAGRID_IO_SERIALIZATION_HPP
#define VIENNAGRID_IO_SERIALIZATION_HPP

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

/** @file serialization.hpp
 *  @brief    A domain wrapper class which models the Boost Serialization concept: http://www.boost.org/libs/serialization/
 */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/shared_ptr.hpp>

namespace viennagrid
{
  namespace io
  {
    /** @brief Domain wrapper which models the Boost serialization concept
     *
     */
    template<typename DomainT, typename SegmentationT>
    struct domain_serializer
    {
      typedef boost::shared_ptr<DomainT>                                                     DomainSPT;
      typedef boost::shared_ptr<SegmentationT>                                               SegmentationSPT;
      
    private:
      typedef typename viennagrid::result_of::segment<SegmentationT>::type                   SegmentType;
      typedef typename viennagrid::result_of::cell_tag<DomainT>::type                        CellTag;
      typedef typename viennagrid::result_of::vertex_range<DomainT>::type                    VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                    VertexIterator;
      typedef typename viennagrid::result_of::point<DomainT>::type                           PointType;
      typedef typename viennagrid::result_of::vertex<DomainT>::type                          VertexType;
      typedef typename viennagrid::result_of::cell<DomainT>::type                            CellType;
      typedef typename viennagrid::result_of::cell_range<SegmentType>::type                  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                      CellIterator;
      typedef typename viennagrid::result_of::vertex_range<CellType>::type                   VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type              VertexOnCellIterator;

      static const int DIMG = PointType::dim;

      friend class boost::serialization::access;

      /** @brief Save function is used to 'read' the data from the domain */
      template<class Archive>
      void save(Archive & ar, const unsigned int version) const
      {
        // -----------------------------------------------
        // the geometry is read and transmitted
        //
        std::size_t point_size = viennagrid::vertices(*domainsp).size();
        ar & point_size;
        VertexRange vertices = viennagrid::elements(*domainsp);
        for (VertexIterator vit = vertices.begin();
             vit != vertices.end(); ++vit)
        {
           for(int d = 0; d < DIMG; d++)
             ar & viennagrid::point(*vit)[d];
        }
        // -----------------------------------------------

        // -----------------------------------------------
        // the segment size is read and transmitted
        //
        std::size_t segment_size = (*domainsp).segments().size();
        ar & segment_size;
        // -----------------------------------------------

        // -----------------------------------------------
        // the segment specific cells are read and transmitted
        //
        for (std::size_t si = 0; si < segment_size; si++)
        {
          SegmentType & seg = (*domainsp).segments()[si];
          CellRange cells = viennagrid::elements(seg);

          std::size_t cell_size = viennagrid::cells(seg).size();
          ar & cell_size;

          for (CellIterator cit = cells.begin();
          cit != cells.end(); ++cit)
          {
            VertexOnCellRange vertices_on_cell = viennagrid::elements(*cit);

            for (VertexOnCellIterator vocit = vertices_on_cell.begin();
            vocit != vertices_on_cell.end();
            ++vocit)
            {
              std::size_t id = vocit->id().get();
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
          PointType p;
          for(int d = 0; d < DIMG; d++)
            ar & p[d];
          
          viennagrid::make_vertex( *domainsp, p );
        }
        // -----------------------------------------------

        // -----------------------------------------------
        // the segments are received and created
        //
        std::size_t segment_size;
        ar & segment_size;
        (*domainsp).segments().resize(segment_size);
        // -----------------------------------------------

        // -----------------------------------------------
        // the segment specific cells are received and stored
        //
        for (std::size_t si = 0; si < segment_size; si++)
        {
          SegmentType & seg = (*domainsp).segments()[si];
          std::size_t cell_size;
          ar & cell_size;
          for(std::size_t ci = 0; ci < cell_size; ci++)
          {
            VertexType *vertices[viennagrid::topology::bndcells<CellTag, 0>::num];

            for (int j=0; j<viennagrid::topology::bndcells<CellTag, 0>::num; ++j)
            {
              std::size_t id;
              ar & id;
              vertices[j] = &(viennagrid::ncells<0>(*domainsp)[id]);
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
      /** @brief The default constructor*/
      domain_serializer() : domainsp(new DomainT) {}

      /** @brief The constructor expects a shared pointer on a domain object and sets the state */
      domain_serializer(DomainSPT& domainsp) : domainsp(domainsp) {}

      /** @brief The load function enables to associate a domain with the serialzer after
      a serializer object has been constructed. */
      inline void load(DomainSPT other) { domainsp = other;  }

      /** @brief The get function enables to retrieve the domain pointer
      */
      inline DomainSPT get() { return domainsp; }

      /** @brief The functor returns a reference to the domain state*/
      DomainT& operator()() { return *domainsp; }

      /** @brief The state holds a shared pointer to the domain */
      DomainSPT  domainsp;
      //DomainT& domain;
    };

  } //namespace io
} //namespace viennagrid

#endif

