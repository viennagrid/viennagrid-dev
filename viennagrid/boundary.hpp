/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_BOUNDARY_GUARD
#define VIENNAGRID_BOUNDARY_GUARD

#include <vector>
#include "viennagrid/forwards.h"

namespace viennagrid
{


  template <typename FEMConfig, typename Segment, typename KEY, typename VALUE>
  void clearBoundary(Segment & seg)
  {

  }

  //helper struct for setting boundary flag of lower level elements of a facet
  template <typename Facet, long topolevel>
  struct BoundarySetter
  {
    template <typename FacetIterator>
    static void apply(FacetIterator & fit)
    {
      typedef typename IteratorTypes<Facet, topolevel>::ResultType    ElementOnFacetIterator;

      for (ElementOnFacetIterator eofit = fit->template begin<topolevel>();
            eofit != fit->template end<topolevel>();
            ++eofit)
      {
        //tag all elements:
        if ( !(eofit->isOnBoundary()) )
          eofit->toggleOnBoundary();

        //std::cout << "Boundary element tagged: "; eofit->print();
      }

      //proceed to lower level:
      BoundarySetter<Facet, topolevel-1>::apply(fit);
    }
  };

  //end recursion of topolevel = -1
  template <typename Facet>
  struct BoundarySetter<Facet, -1>
  {
    template <typename FacetIterator>
    static void apply(FacetIterator & fit)
    {
    }
  };


  template <long topolevel, typename TopoLevelHandling>
  struct UnsafeBoundaryDetector
  {
    template <typename Segment>
    static void apply(Segment & seg)
    {
      typedef typename IteratorTypes<Segment, topolevel>::ResultType    LevelIterator;

      std::cout << "Boundary detection on level " << topolevel << std::endl;
      for (LevelIterator lit = seg.template begin<topolevel>();
            lit != seg.template end<topolevel>();
            ++lit)
      {
        if (!lit->isOnBoundary())
          lit->toggleOnBoundary();
      }

      UnsafeBoundaryDetector<topolevel-1,
                             typename SegmentConfig<topolevel-1>::HandlingTag>::apply(seg);
    }
  };

  template <long topolevel>
  struct UnsafeBoundaryDetector<topolevel, TopoLevelNoHandling>
  {
    template <typename Segment>
    static void apply(Segment & seg)
    {
      UnsafeBoundaryDetector<topolevel-1,
                             typename SegmentConfig<topolevel-1>::HandlingTag>::apply(seg);
    }
  };

  template <>
  struct UnsafeBoundaryDetector<-1, TopoLevelNoHandling>
  {
    template <typename Segment>
    static void apply(Segment & seg) {}
  };


  //after segment is read, boundary detection has to be done (implemented here)
  //must be called BEFORE Dirichlet or Neumann boundaries are set.
  template <typename Segment>
  void detectBoundary_impl(Segment & seg, TopoLevelNoHandling)
  {
    //unsafe detection: issue warning:
    std::cout << "WARNING: Boundary detection is not reliable, since no facets are available!" << std::endl;

    //set all elements to boundary:
    UnsafeBoundaryDetector<Segment::Configuration::CellTag::TopoLevel - 2,
               typename SegmentConfig<Segment::Configuration::CellTag::TopoLevel - 2>::HandlingTag
                          >::apply(seg);

  }

  template <typename Segment>
  void detectBoundary_impl(Segment & seg, TopoLevelFullHandling)
  {
    typedef typename Segment::Configuration                         DomainConfiguration;
    typedef typename DomainConfiguration::CellTag                   CellTag;
    typedef typename DomainTypes<DomainConfiguration>::FacetType    Facet;
    typedef typename DomainTypes<DomainConfiguration>::CellType     Cell;

    typedef typename IteratorTypes<Segment, CellTag::TopoLevel-1>::ResultType    FacetIterator;
    typedef typename IteratorTypes<Segment, CellTag::TopoLevel>::ResultType      CellIterator;

    typedef typename IteratorTypes<Cell, CellTag::TopoLevel-1>::ResultType  FacetOnCellIterator;

    //iterate over all cells, over facets there and tag them:
    for (CellIterator cit = seg.template begin<CellTag::TopoLevel>();
          cit != seg.template end<CellTag::TopoLevel>();
          ++cit)
    {
      for (FacetOnCellIterator focit = cit->template begin<CellTag::TopoLevel-1>();
            focit != cit->template end<CellTag::TopoLevel-1>();
            ++focit)
      {
        focit->toggleOnBoundary();
        //std::cout << "Toggling boundary on facet" << std::endl;
        //fit->print();
      }
    }
    //iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
    for (FacetIterator fit = seg.template begin<CellTag::TopoLevel-1>();
          fit != seg.template end<CellTag::TopoLevel-1>();
          ++fit)
    {
      if (fit->isOnBoundary())
      {
        //fit->print();
        BoundarySetter<Facet, CellTag::TopoLevel-2>::apply(fit);
      }
    }
  }

  template <typename Segment>
  void detectBoundary(Segment & seg)
  {
    typedef typename Segment::Configuration::CellTag                   CellTag;
    typedef typename TopologyLevel<CellTag, CellTag::TopoLevel-1>::HandlingTag  HandlingTag;
    detectBoundary_impl(seg, HandlingTag());
  }

}

#endif
