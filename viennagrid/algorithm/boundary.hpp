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

#ifndef VIENNAGRID_BOUNDARY_GUARD
#define VIENNAGRID_BOUNDARY_GUARD

#include <vector>
#include "viennagrid/forwards.h"

namespace viennagrid
{


  //helper struct for setting boundary flag of lower level elements of a facet
  template <typename Facet, long topolevel>
  struct BoundarySetter
  {
    template <typename FacetIterator>
    static void apply(FacetIterator & fit)
    {
      typedef typename viennagrid::result_of::ncell_range<Facet, topolevel>::type    ElementOnFacetRange;
      typedef typename result_of::iterator<ElementOnFacetRange>::type                ElementOnFacetIterator;

      ElementOnFacetRange eof_container = ncells<topolevel>(*fit);
      for (ElementOnFacetIterator eofit = eof_container.begin();
            eofit != eof_container.end();
            ++eofit)
      {
        //tag all elements:
        //if ( !(eofit->isOnBoundary()) )
        //  eofit->toggleOnBoundary();

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
      typedef typename result_of::iterator<Segment, topolevel>::type    LevelIterator;

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
    typedef typename Segment::config_type                         DomainConfiguration;
    typedef typename DomainConfiguration::cell_tag                   CellTag;
    typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level-1>::type   FacetType;
    typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;

    typedef typename viennagrid::result_of::ncell_range<Segment, CellTag::topology_level-1>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                 FacetIterator;
      
    typedef typename viennagrid::result_of::ncell_range<Segment, CellTag::topology_level>::type        CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                                  CellIterator;

    typedef typename viennagrid::result_of::ncell_range<CellType, CellTag::topology_level-1>::type     FacetOnCellRange;
    typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                           FacetOnCellIterator;

    //iterate over all cells, over facets there and tag them:
    CellRange cells = viennagrid::ncells<CellTag::topology_level>(seg);
    for (CellIterator cit = cells.begin();
          cit != cells.end();
          ++cit)
    {
      FacetOnCellRange facets_on_cell = ncells<CellTag::topology_level-1>(*cit);
      for (FacetOnCellIterator focit = facets_on_cell.begin();
            focit != facets_on_cell.end();
            ++focit)
      {
        //focit->toggleOnBoundary(); //TODO:Replace by boundary flag from ViennaData
        
        //std::cout << "Toggling boundary on facet" << std::endl;
        //fit->print();
      }
    }
    
    //iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
    FacetRange facets = viennagrid::ncells<CellTag::topology_level-1>(seg);
    for (FacetIterator fit = facets.begin();
          fit != facets.end();
          ++fit)
    {
      //if (fit->isOnBoundary())  //TODO:replace by boundary flag from ViennaData
      if(true)
      {
        //fit->print();
        BoundarySetter<FacetType, CellTag::topology_level-2>::apply(fit);
      }
    }
  }

  template <typename SegmentType>
  void detectBoundary(SegmentType & seg)
  {
    typedef typename SegmentType::config_type::cell_tag                   CellTag;
    typedef typename topology::subcell_desc<CellTag, CellTag::topology_level-1>::handling_tag  HandlingTag;
    detectBoundary_impl(seg, HandlingTag());
  }

}

#endif
