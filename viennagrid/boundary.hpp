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


  template <long topolevel, typename topology_levelHandling>
  struct UnsafeBoundaryDetector
  {
    template <typename Segment>
    static void apply(Segment & seg)
    {
      typedef typename IteratorTypes<Segment, topolevel>::result_type    LevelIterator;

      std::cout << "Boundary detection on level " << topolevel << std::endl;
      for (LevelIterator lit = seg.template begin<topolevel>();
            lit != seg.template end<topolevel>();
            ++lit)
      {
        if (!lit->isOnBoundary())
          lit->toggleOnBoundary();
      }

      UnsafeBoundaryDetector<topolevel-1,
                             typename segment_traits<topolevel-1>::handling_tag>::apply(seg);
    }
  };

  template <long topolevel>
  struct UnsafeBoundaryDetector<topolevel, no_handling_tag>
  {
    template <typename Segment>
    static void apply(Segment & seg)
    {
      UnsafeBoundaryDetector<topolevel-1,
                             typename segment_traits<topolevel-1>::handling_tag>::apply(seg);
    }
  };

  template <>
  struct UnsafeBoundaryDetector<-1, no_handling_tag>
  {
    template <typename Segment>
    static void apply(Segment & seg) {}
  };


  //after segment is read, boundary detection has to be done (implemented here)
  //must be called BEFORE Dirichlet or Neumann boundaries are set.
  template <typename Segment>
  void detectBoundary_impl(Segment & seg, no_handling_tag)
  {
    //unsafe detection: issue warning:
    std::cout << "WARNING: Boundary detection is not reliable, since no facets are available!" << std::endl;

    //set all elements to boundary:
    UnsafeBoundaryDetector<Segment::Configuration::cell_tag::topology_level - 2,
                           typename segment_traits<Segment::Configuration::cell_tag::topology_level - 2>::handling_tag
                          >::apply(seg);

  }

  template <typename Segment>
  void detectBoundary_impl(Segment & seg, full_handling_tag)
  {
    typedef typename Segment::Configuration                         DomainConfiguration;
    typedef typename DomainConfiguration::cell_tag                   CellTag;
    typedef typename DomainTypes<DomainConfiguration>::facet_type    Facet;
    typedef typename DomainTypes<DomainConfiguration>::cell_type     Cell;

    typedef typename IteratorTypes<Segment, CellTag::topology_level-1>::result_type    FacetIterator;
    typedef typename IteratorTypes<Segment, CellTag::topology_level>::result_type      CellIterator;

    typedef typename IteratorTypes<Cell, CellTag::topology_level-1>::result_type  FacetOnCellIterator;

    //iterate over all cells, over facets there and tag them:
    for (CellIterator cit = seg.template begin<CellTag::topology_level>();
          cit != seg.template end<CellTag::topology_level>();
          ++cit)
    {
      for (FacetOnCellIterator focit = cit->template begin<CellTag::topology_level-1>();
            focit != cit->template end<CellTag::topology_level-1>();
            ++focit)
      {
        //focit->toggleOnBoundary(); //TODO:Replace by boundary flag from ViennaData
        
        //std::cout << "Toggling boundary on facet" << std::endl;
        //fit->print();
      }
    }
    //iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
    for (FacetIterator fit = seg.template begin<CellTag::topology_level-1>();
          fit != seg.template end<CellTag::topology_level-1>();
          ++fit)
    {
      //if (fit->isOnBoundary())  //TODO:replace by boundary flag from ViennaData
      if(true)
      {
        //fit->print();
        BoundarySetter<Facet, CellTag::topology_level-2>::apply(fit);
      }
    }
  }

  template <typename SegmentType>
  void detectBoundary(SegmentType & seg)
  {
    typedef typename SegmentType::Configuration::cell_tag                   CellTag;
    typedef typename subcell_traits<CellTag, CellTag::topology_level-1>::handling_tag  HandlingTag;
    detectBoundary_impl(seg, HandlingTag());
  }

}

#endif
