/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_IO_GTS_LIST_GETTER_GUARD
#define VIENNAGRID_IO_GTS_LIST_GETTER_GUARD

#include "gtsio2/UtilityLibs/gtsio2/src/include/gtsio2.h"

using namespace GTSIO::Data;
using namespace GTSIO::Reader;
using namespace GTSIO::Reader::CRV;
using namespace GTSIO;

namespace viennagrid
{
  namespace io
  {
    //*****************************************************************
    // gts_list_getter
    // ---------------
    // Implements a generic approach for 1D, 2D and 3D handling of
    // GTSObjects (GTSLine, GTSFace, GTSPolyhedron) and
    // GTSObjectLists (GTSLineList, GTSFaceList, GTSPolyhedronList).
    //
    //******************************************************************
    template <long i>
    struct gts_list_getter;

    //one dimension
    template <>
    struct gts_list_getter<1>
    {
      typedef GTSLineList       list_type;
      typedef GTSLine           object_type;
      
      static list_type* newList(GTSDevice * pDevice)
      {
        list_type* pLineList = pDevice->NewGTSLineList("Lines");
        pLineList->setName("Lines");
              
        return pLineList;
      }
      
      static list_type* getList(GTSDevice* pDevice)
      {
        list_type* pLineList = pDevice->getGTSLineList("Lines");
        //list_type* pFaceList = pDevice->getGTSLineList(0);
        
        cout << "#LineLists: " << pDevice->getGTSLineListSize() << endl;

        return pLineList;
      }
    };
    
    // two dimensions
    template <>
    struct gts_list_getter<2>
    {
      typedef GTSFaceList       list_type;
      typedef GTSFace           object_type;
      
      static list_type* newList(GTSDevice * pDevice)
      {
        list_type* pFaceList = pDevice->NewGTSFaceList("Faces");
        pFaceList->setName("Faces");
        return pFaceList;
      }    
      
      static list_type* getList(GTSDevice* pDevice)
      {
        list_type* pFaceList = pDevice->getGTSFaceList("Faces");
        return pFaceList;
      }
    };

    // three dimensions
    template <>
    struct gts_list_getter<3>
    {
      typedef GTSPolyhedronList       list_type;
      typedef GTSPolyhedron           object_type;
      
      static list_type* newList(GTSDevice * pDevice)
      {
        list_type* pPolyhedronList = pDevice->NewGTSPolyhedronList("Polyhedrons");
        pPolyhedronList->setName("Polyhedrons");
        return pPolyhedronList;
      }
          
      static list_type* getList(GTSDevice* pDevice)
      {
        list_type* pPolyhedronList = pDevice->getGTSPolyhedronList("Polyhedrons");
        
        cout << "Address pFaceList: " << pPolyhedronList << endl;
        cout << "PolyhedronListSize: " << pDevice->getGTSPolyhedronListSize() << endl;
        
        return pPolyhedronList;
      }
    };
  }
}

#endif