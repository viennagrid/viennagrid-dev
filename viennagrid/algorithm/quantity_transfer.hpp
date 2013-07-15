#ifndef VIENNAGRID_ALGORITHM_QUANTITY_TRANSFER_HPP
#define VIENNAGRID_ALGORITHM_QUANTITY_TRANSFER_HPP

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

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/domain/domain.hpp"

/** @file quantity_transfer.hpp
    @brief Provides routines for transferring quantities defined for elements of one topological dimensions to elements of other topological dimension.
*/

namespace viennagrid
{
  namespace detail
  {
    /** @brief Indicates a transfer from higher to lower topological dimension (boundary operation) */
    struct boundary_quantity_transfer_tag {};

    /** @brief Indicates a transfer from lower to higher topological dimension (coboundary operation) */
    struct coboundary_quantity_transfer_tag {};

    template <typename SourceTag, typename DestinationTag,
              bool less_than = (SourceTag::dim < DestinationTag::dim),
              bool larger_than = (SourceTag::dim > DestinationTag::dim)>
    struct quantity_transfer_dispatcher {};

    template <typename SourceTag, typename DestinationTag>
    struct quantity_transfer_dispatcher<SourceTag, DestinationTag, false, true>
    {
      typedef boundary_quantity_transfer_tag  type;
    };

    template <typename SourceTag, typename DestinationTag>
    struct quantity_transfer_dispatcher<SourceTag, DestinationTag, true, false>
    {
      typedef coboundary_quantity_transfer_tag  type;
    };


    // Implementation for boundary transfer
    template <typename SourceTag, typename DestinationTag,
              typename DomSeg, typename AccessorSrc, typename SetterDest,
              typename Averager, typename FilterSrc, typename FilterDest>
    void quantity_transfer(DomSeg const & domseg, AccessorSrc const & accessor_src, SetterDest & setter_dest,
                           Averager const & averager, FilterSrc const & filter_src, FilterDest const & filter_dest,
                           boundary_quantity_transfer_tag)
    {
        typedef typename viennagrid::result_of::element<DomSeg, SourceTag>::type              SourceElementType;
        typedef typename viennagrid::result_of::element<DomSeg, DestinationTag>::type             DestElementType;

        typedef typename viennagrid::result_of::const_element_range<DomSeg, SourceTag>::type  SourceContainer;
        typedef typename viennagrid::result_of::iterator<SourceContainer>::type           SourceIterator;

        typedef typename viennagrid::result_of::const_element_range<SourceElementType, DestinationTag>::type  DestOnSrcContainer;
        typedef typename viennagrid::result_of::iterator<DestOnSrcContainer>::type                  DestOnSrcIterator;

        typedef typename AccessorSrc::value_type              value_type;

        typedef std::map<DestElementType const *, std::vector<value_type> >                     DestinationValueMap;  //Think about adding customization options for std::vector<double>

        SourceContainer source_cells = viennagrid::elements(domseg);

        DestinationValueMap  values_for_destination_cells;

        // Step 1: Push all values from source cells to their destination boundary.
        //         Note that a coboundary-iteration over destination cells has a higher memory footprint, thus this lightweight-variant using only boundary-iterations is used
        for (SourceIterator sit = source_cells.begin();
                            sit != source_cells.end(); ++sit)
        {
          if ( filter_src(*sit) )
          {
            DestOnSrcContainer dest_on_src = viennagrid::elements(*sit);
            for (DestOnSrcIterator dosit  = dest_on_src.begin();
                                    dosit != dest_on_src.end();
                                  ++dosit)
            {
              if (filter_dest(*dosit))
                values_for_destination_cells[&(*dosit)].push_back(accessor_src(*sit));
            }
          }
        }

        // Step 2: Now average over values on destination cells
        for (typename DestinationValueMap::const_iterator dest_values_it  = values_for_destination_cells.begin();
                                                          dest_values_it != values_for_destination_cells.end();
                                                        ++dest_values_it)
        {
          setter_dest(*(dest_values_it->first),
                      averager(dest_values_it->second));
        }
    }

    // Implementation for coboundary transfer
    template <typename SourceTag, typename DestinationTag,
              typename DomSeg, typename AccessorSrc, typename SetterDest,
              typename Averager, typename FilterSrc, typename FilterDest>
    void quantity_transfer(DomSeg const & domseg, AccessorSrc const & accessor_src, SetterDest & setter_dest,
                           Averager const & averager, FilterSrc const & filter_src, FilterDest const & filter_dest,
                           coboundary_quantity_transfer_tag)
    {
        typedef typename viennagrid::result_of::element<DomSeg, SourceTag>::type              SourceElementType;
        typedef typename viennagrid::result_of::element<DomSeg, DestinationTag>::type             DestElementType;

        typedef typename viennagrid::result_of::const_element_range<DomSeg, DestinationTag>::type DestContainer;
        typedef typename viennagrid::result_of::iterator<DestContainer>::type             DestIterator;

        typedef typename viennagrid::result_of::const_element_range<DestElementType, SourceTag>::type  SrcOnDestContainer;
        typedef typename viennagrid::result_of::iterator<SrcOnDestContainer>::type               SrcOnDestIterator;

        typedef typename AccessorSrc::value_type              value_type;

        DestContainer dest_cells = viennagrid::elements(domseg);

        // Iterate over all dest n-cells, push values from source cell to container, then compute final value
        for (DestIterator dit = dest_cells.begin(); dit != dest_cells.end(); ++dit)
        {
          if ( filter_dest(*dit) )   //assumption: lattice temperature outside semiconductor is not relevant for simulation
          {
            std::vector<value_type> destination_value_container;

            // Push all values from adjacent source cells to the container
            SrcOnDestContainer src_on_dest = viennagrid::elements(*dit);
            for (SrcOnDestIterator sodit  = src_on_dest.begin();
                                    sodit != src_on_dest.end();
                                  ++sodit)
            {
              if (filter_src(*sodit))
                destination_value_container.push_back(accessor_src(*dit));
            }

            //
            setter_dest(*dit, averager(destination_value_container));
          }
        }
    }

  }

  /** @brief Transfers data defined on elements of topological dimension 'dim_src' to elements of topological dimension 'dim_dest'. For example, values defined on cells are tranferred to vertices.
   *
   * Even though this functionality is sometimes referred to as interpolation, it is not an interpolation in the strict mathematical sense.
   *
   * @tparam dim_src           Topological dimension of the source elements
   * @tparam dim_dest          Topological dimension of the destination elements
   * @param domseg             A domain or segment, in which the source and destination elements reside
   * @param accessor_src       An accessor functor for retrieving the data defined on each source element
   * @param setter_dest        A setter for storing the data on each destination element (first argument is the destination n-cell, second argument is the value)
   * @param averager           A functor which computes the value of the destination element from an STL-compatible container holding the values of all adjacent source elements
   * @param filter_src         A functor which returns true for all source elements considered for the transfer, false otherwise
   * @param filter_dest        A functor which returns true for all destination elements considered for the transfer, false otherwise
   */
  template <typename SourceTag, typename DestinationTag,
            typename DomSeg, typename AccessorSrc, typename SetterDest,
            typename Averager, typename FilterSrc, typename FilterDest>
  void quantity_transfer(DomSeg const & domseg, AccessorSrc const & accessor_src, SetterDest & setter_dest,
                         Averager const & averager, FilterSrc const & filter_src, FilterDest const & filter_dest)
  {
    detail::quantity_transfer<SourceTag, DestinationTag>(domseg, accessor_src, setter_dest,
                                                 averager, filter_src, filter_dest,
                                                 typename detail::quantity_transfer_dispatcher<SourceTag, DestinationTag>::type());
  }

}

#endif
