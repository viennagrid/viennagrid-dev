/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_DOMAIN_GUARD
#define VIENNAGRID_DOMAIN_GUARD


#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include "viennagrid/forwards.h"
#include "viennagrid/celltags.h"  
#include "viennagrid/point.hpp"
#include "viennagrid/element.hpp"
#include "viennagrid/segment.hpp"


namespace viennagrid
{


/***************************** Domain Type ***********************/

  //helper class: iterates over all segments on a particular topolevel, shifts the IDs and continues to next topolevel:
  template <typename DomainConfiguration,
              long topolevel,
              typename LevelHandling = typename segment_traits<topolevel>::handling_tag,
              typename IDHandling =
                            typename subcell_traits<typename DomainConfiguration::cell_tag,
                                                    topolevel>::element_tag::id_handler
             >
  struct SegmentIDShifter
  {
    //default case: continue iteration:
    static void apply(typename DomainTypes<DomainConfiguration>::DomainType & domain)
    {
      SegmentIDShifter<DomainConfiguration, topolevel-1>::apply(domain);
    }
  };

  template <typename DomainConfiguration,
             long topolevel>
  struct SegmentIDShifter <DomainConfiguration, topolevel, full_handling_tag, ProvideID>
  {
    //shift IDs:
    static void apply(typename DomainTypes<DomainConfiguration>::DomainType & domain)
    {
      typedef typename DomainTypes<DomainConfiguration>::SegmentIterator  SegmentIterator;
      typedef typename DomainTypes<DomainConfiguration>::SegmentType      SegmentType;
      typedef typename IteratorTypes<SegmentType, topolevel>::ResultType  LevelIterator;

      long id_offset = 0;
      for (SegmentIterator segit = domain.getSegmentBegin();
            segit != domain.getSegmentEnd();
            ++segit)
      {
        for (LevelIterator lit = segit->template begin<topolevel>();
              lit != segit->template end<topolevel>();
              ++lit)
        {
          lit->setID(lit->getID() + id_offset);
        }
        id_offset += segit->template getElementNum<topolevel>();
      }

      //continue iteration: (by using the default behaviour for this level. This makes termination of topolevel-loop easier: no code duplication)
      SegmentIDShifter<DomainConfiguration, topolevel, no_handling_tag, NoID>::apply(domain);
    }

  };

  //stop at vertex-level
  template <typename DomainConfiguration>
  struct SegmentIDShifter<DomainConfiguration, 0, no_handling_tag, NoID>
  {
    static void apply(typename DomainTypes<DomainConfiguration>::DomainType & domain) {}
  };


  template <typename T_Configuration>
  class domain
  {
      typedef segment<T_Configuration>                            SegmentType;

    public:
      typedef T_Configuration                                         config_type;
      typedef SegIt< SegmentType,
                      typename std::vector< SegmentType >::iterator
                    >                                                 segment_iterator;

      typedef ConstSegIt< SegmentType,
                      typename std::vector< SegmentType >::const_iterator
                    >                                                 const_segment_iterator;

      SegmentType & add()
      {
        segments.push_back( SegmentType() );
        return segments.back();
      }

      const_segment_iterator begin(long level = 0) const
      { return const_segment_iterator(segments.begin(), level); };

      const_segment_iterator end(long level = 0) const
      { return const_segment_iterator(segments.end(), level); };

      segment_iterator begin(long level = 0)
      { return segment_iterator(segments.begin(), level); };
      segment_iterator end(long level = 0)
      { return segment_iterator(segments.end(), level); };

//       //assures the domain-wide uniqueness of IDs right after segment setup from files.
//       void finishSegmentSetup()
//       {
//         //iterate over segments and set appropriate start-indices for each topology-level:
//         SegmentIDShifter<T_Configuration, T_Configuration::CellTag::topology_level>::apply(*this);
//       }

      template <long topolevel>
      long size()
      {
        long ret = 0;
        for (segment_iterator segit = begin();
              segit != end();
              ++segit)
        {
          ret += segit->template size<topolevel>();
        }
        return ret;
      }

    private:
      std::vector< SegmentType > segments;

  };



  // Container for all types in a domain for access by other domain elements (classes)
  // This makes it easier to extend types later on: Just change the type definitions here.
  // All template parameters must be deriveable from 'Configuration'
  template <typename Configuration>
  class DomainTypes
  {
    enum { dim = Configuration::dimension_tag::value };

  public:
    typedef point< typename Configuration::numeric_type,
                      typename Configuration::dimension_tag >                       point_type;

    typedef element< Configuration,
                      typename subcell_traits<typename Configuration::cell_tag, 0>::element_tag>       vertex_type;

    typedef element< Configuration,
                      typename subcell_traits<typename Configuration::cell_tag, 1>::element_tag >      edge_type;

    typedef element< Configuration,
                      typename subcell_traits<typename Configuration::cell_tag, dim-1>::element_tag >  facet_type;

    typedef element< Configuration,
                      typename Configuration::cell_tag >                       cell_type;

    typedef segment<Configuration>                                            segment_type;
    typedef domain<Configuration>                                             domain_type;

    //Iterators:
    typedef typename IteratorTypes<segment_type, 0>::result_type                    vertex_iterator;
    typedef typename IteratorTypes<segment_type, 1>::result_type                    edge_iterator;
    typedef typename IteratorTypes<segment_type,
                                    Configuration::cell_tag::topology_level-1>::result_type  facet_iterator;
    typedef typename IteratorTypes<segment_type,
                                    Configuration::cell_tag::topology_level>::result_type    cell_iterator;
    typedef typename domain<Configuration>::segment_iterator                    segment_iterator;

    typedef typename IteratorTypes<cell_type, 0>::result_type              vertex_on_cell_iterator;
    //TODO: Add more iterators here
  };

}
#endif
