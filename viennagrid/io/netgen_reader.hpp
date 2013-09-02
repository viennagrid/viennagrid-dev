#ifndef VIENNAGRID_IO_NETGEN_READER_GUARD
#define VIENNAGRID_IO_NETGEN_READER_GUARD

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include <fstream>
#include <iostream>
#include <assert.h>
#include "viennagrid/forwards.hpp"
#include "viennagrid/io/helper.hpp"

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/element_creation.hpp"

/** @file netgen_reader.hpp
    @brief Provides a reader for Netgen files
*/

namespace viennagrid
{
  namespace io
  {

    /** @brief Reader for Netgen files obtained from the 'Export mesh...' menu item. Tested with Netgen version 4.9.12. */
//     template<typename CellTypeOrTag>
    struct netgen_reader
    {
      /** @brief The functor interface triggering the read operation.
       *
       * @param domain    A ViennaGrid domain
       * @param filename  Name of the file
       */
      template <typename DomainType, typename SegmentationType>
      int operator()(DomainType & domain, SegmentationType & segmentation, std::string const & filename) const
      {
        typedef typename SegmentationType::segment_type SegmentType;

        typedef typename viennagrid::result_of::point<DomainType>::type    PointType;
        typedef typename viennagrid::result_of::coord< PointType >::type         CoordType;

        const int point_dim = viennagrid::traits::static_size<PointType>::value;

        typedef typename result_of::cell_tag<DomainType>::type CellTag;
        typedef typename result_of::element<DomainType, CellTag>::type CellType;
        typedef typename result_of::handle<DomainType, CellTag>::type                           CellHandleType;

        typedef typename result_of::element<DomainType, vertex_tag>::type                           VertexType;
        typedef typename result_of::handle<DomainType, vertex_tag>::type                           VertexHandleType;

        typedef typename viennagrid::result_of::element_range<DomainType, vertex_tag>::type   VertexRange;
        typedef typename viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;

        typedef typename viennagrid::result_of::element_range<DomainType, line_tag>::type   EdgeRange;
        typedef typename viennagrid::result_of::iterator<EdgeRange>::type          EdgeIterator;

        typedef typename viennagrid::result_of::element_range<DomainType, typename CellType::tag::facet_tag >::type   FacetRange;
        typedef typename viennagrid::result_of::iterator<FacetRange>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::element_range<DomainType, CellTag>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                  CellIterator;

        std::ifstream reader(filename.c_str());

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception(filename);
          return EXIT_FAILURE;
        }

        std::string token;
        long node_num = 0;
        long cell_num = 0;

        if (!reader.good())
          throw bad_file_format_exception(filename, "File is empty.");

        //
        // Read vertices:
        //
        reader >> node_num;
        assert(node_num > 0);

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << node_num << " vertices... " << std::endl;
        #endif



        for (int i=0; i<node_num; i++)
        {
          if (!reader.good())
            throw bad_file_format_exception(filename, "EOF encountered while reading vertices.");

          PointType p;

          for (int j=0; j<point_dim; j++)
            reader >> p[j];

          viennagrid::make_vertex_with_id( domain, typename VertexType::id_type(i), p );
        }

        if (!reader.good())
          throw bad_file_format_exception(filename, "EOF encountered when reading number of cells.");


        //
        // Read cells:
        //
        reader >> cell_num;

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;
        #endif

        for (int i=0; i<cell_num; ++i)
        {
          long vertex_num;
          viennagrid::storage::static_array<VertexHandleType, boundary_elements<CellTag, vertex_tag>::num> cell_vertex_handles;

          if (!reader.good())
            throw bad_file_format_exception(filename, "EOF encountered while reading cells (segment index expected).");

          std::size_t segment_index;
          reader >> segment_index;

          for (int j=0; j<boundary_elements<CellTag, vertex_tag>::num; ++j)
          {
            if (!reader.good())
              throw bad_file_format_exception(filename, "EOF encountered while reading cells (cell ID expected).");

            reader >> vertex_num;
            cell_vertex_handles[j] = viennagrid::get_vertex_handle(domain, vertex_num - 1);
          }

          viennagrid::make_element_with_id<CellType>(segmentation[segment_index], cell_vertex_handles.begin(), cell_vertex_handles.end(), typename CellType::id_type(i));
        }

        return EXIT_SUCCESS;
      } //operator()
      
      
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename)
      {
        typedef typename viennagrid::result_of::segmentation<DomainType>::type SegmentationType;
        SegmentationType tmp(domain);
        return (*this)(domain, tmp, filename);
      }

    }; //class netgen_reader

  } //namespace io
} //namespace viennagrid

#endif
