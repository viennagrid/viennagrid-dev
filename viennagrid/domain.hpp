/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */

#ifndef VIENNAMESH_DOMAIN_GUARD
#define VIENNAMESH_DOMAIN_GUARD


#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include "viennamesh/forwards.h"
#include "viennamesh/celltags.h"  
#include "viennamesh/point.hpp"
//#include "viennamesh/celltags.h"
#include "viennamesh/element.hpp"
#include "viennamesh/segment.hpp"


namespace viennamesh{


/***************************** Domain Type ***********************/

  //helper class: iterates over all segments on a particular topolevel, shifts the IDs and continues to next topolevel:
  template <typename DomainConfiguration,
              long topolevel,
              typename LevelHandling = typename SegmentConfig<topolevel>::HandlingTag,
              typename IDHandling =
                            typename TopologyLevel<typename DomainConfiguration::CellTag,
                                                    topolevel>::ElementTag::IDHandler
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
  struct SegmentIDShifter <DomainConfiguration, topolevel, TopoLevelFullHandling, ProvideID>
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
      SegmentIDShifter<DomainConfiguration, topolevel, TopoLevelNoHandling, NoID>::apply(domain);
    }

  };

  //stop at vertex-level
  template <typename DomainConfiguration>
  struct SegmentIDShifter<DomainConfiguration, 0, TopoLevelNoHandling, NoID>
  {
    static void apply(typename DomainTypes<DomainConfiguration>::DomainType & domain) {}
  };


  template <typename T_Configuration>
  class domain
  {
      typedef segment<T_Configuration>                            SegmentType;

    public:
      typedef T_Configuration                                       Configuration;
      typedef SegIt< SegmentType,
                      typename std::vector< SegmentType >::iterator
                    >                                                 SegmentIterator;

      typedef ConstSegIt< SegmentType,
                      typename std::vector< SegmentType >::const_iterator
                    >                                                 ConstSegmentIterator;

      SegmentType & add()
      {
        segments.push_back( SegmentType() );
        return segments.back();
      }

      ConstSegmentIterator begin(long level = 0) const
      { return ConstSegmentIterator(segments.begin(), level); };

      ConstSegmentIterator end(long level = 0) const
      { return ConstSegmentIterator(segments.end(), level); };

      SegmentIterator begin(long level = 0)
      { return SegmentIterator(segments.begin(), level); };
      SegmentIterator end(long level = 0)
      { return SegmentIterator(segments.end(), level); };

//       //assures the domain-wide uniqueness of IDs right after segment setup from files.
//       void finishSegmentSetup()
//       {
//         //iterate over segments and set appropriate start-indices for each topology-level:
//         SegmentIDShifter<T_Configuration, T_Configuration::CellTag::TopoLevel>::apply(*this);
//       }

      template <long topolevel>
      long size()
      {
        long ret = 0;
        for (SegmentIterator segit = begin();
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
    enum { dim = Configuration::DimensionTag::dim };


  public:
    typedef point_t< typename Configuration::CoordType,
                      typename Configuration::DimensionTag >                       PointType;

    typedef element< Configuration,
                      typename TopologyLevel<typename Configuration::CellTag, 0>::ElementTag>       VertexType;

    typedef element< Configuration,
                      typename TopologyLevel<typename Configuration::CellTag, 1>::ElementTag >      EdgeType;

    typedef element< Configuration,
                      typename TopologyLevel<typename Configuration::CellTag, dim-1>::ElementTag >  FacetType;

    typedef element< Configuration,
                      typename Configuration::CellTag >                       CellType;

    typedef segment<Configuration>                                            SegmentType;
    typedef domain<Configuration>                                             DomainType;

    //Iterators:
    typedef typename IteratorTypes<SegmentType,0>::ResultType                    VertexIterator;
    typedef typename IteratorTypes<SegmentType,1>::ResultType                    EdgeIterator;
    typedef typename IteratorTypes<SegmentType,
                                    Configuration::CellTag::TopoLevel-1>::ResultType  FacetIterator;
    typedef typename IteratorTypes<SegmentType,
                                    Configuration::CellTag::TopoLevel>::ResultType    CellIterator;
    typedef typename domain<Configuration>::SegmentIterator                    SegmentIterator;

    typedef typename IteratorTypes<CellType,0>::ResultType              VertexOnCellIterator;

  };

}
#endif
