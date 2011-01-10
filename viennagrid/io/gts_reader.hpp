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

#ifndef VIENNAGRID_IO_GTS_READER_GUARD
#define VIENNAGRID_IO_GTS_READER_GUARD

#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <exception>
#include <fstream>
#include "viennagrid/forwards.h"
#include "viennagrid/boundary.hpp"
#include "gts_list_getter.hpp"

#include "gtsio2/UtilityLibs/gtsio2/src/include/gtsio2.h"

using namespace std;
using namespace GTSIO::Data;
using namespace GTSIO::Reader;
using namespace GTSIO::Reader::CRV;
using namespace GTSIO;

namespace viennagrid
{
  namespace io
  {
    //**************************************************************
    // gts_point_getter
    // ----------------
    // Extracts the related points of a n-th dimensional GTSObject.
    // 1D GTSLine
    // 2D GTSFace
    // 3D GTSPolyhedron
    //**************************************************************
    template<long dim, typename obj>
    struct gts_point_getter;
    
    // one dimension
    template<typename obj>
    struct gts_point_getter<1, obj> {
      static long getPointIdx(obj* pO, long idx) {
        long pointIdx;
        
        if(idx == 0) {
          pointIdx = pO->getIndex();
        } else if(idx == 1) {
          pointIdx = pO->getEndIdx(idx);
        } else {
          throw;
        }

        cout << "Idx: " << pointIdx << "No. of points: " << pO->getNumberOfPoints() << endl;
        cout << "getIndex(): " << pO->getIndex() << endl;
        
        return pointIdx;
      }
    };
    
    // two dimensions
    template<typename obj>
    struct gts_point_getter<2, obj> {
      static long getPointIdx(obj* pO, long idx) {
        return pO->getPointIdx(idx);
      }
    };
    
    // three dimensions
    template<typename obj>
    struct gts_point_getter<3, obj> {
      static long getPointIdx(obj* pO, long idx) {
        return pO->getPointIdx(idx);
      }
    };
    
    //*****************************************************************
    // Implementation of a .gts-file reader using the gtsio2-library
    //*****************************************************************
    struct gts_reader
    {
      template <typename DomainType>
      void operator()(DomainType & domain, std::string const & filename) const
      {
        typedef typename DomainType::Configuration                     DomainConfiguration;

        typedef typename DomainConfiguration::CoordType                 CoordType;
        typedef typename DomainConfiguration::DimensionTag              DimensionTag;
        typedef typename DomainConfiguration::CellTag                   CellTag;
        typedef typename DomainConfiguration::BoundaryReadTag           BoundaryReadTag;

        typedef typename DomainTypes<DomainConfiguration>::PointType    Point;
        typedef typename DomainTypes<DomainConfiguration>::VertexType   Vertex;
        typedef typename DomainTypes<DomainConfiguration>::CellType     Cell;
        typedef typename DomainTypes<DomainConfiguration>::SegmentType  Segment;

        typedef typename DomainTypes<DomainConfiguration>::VertexIterator      VertexIterator;
        typedef typename DomainTypes<DomainConfiguration>::CellIterator        CellIterator;
  
        typedef typename gts_list_getter<DimensionTag::dim>::list_type         GTSObjList;
        typedef typename gts_list_getter<DimensionTag::dim>::object_type       GTSObj;
        
        Segment & segment = *(domain.begin());

        std::cout << "Reading file " << filename << std::endl;
        
        /* using the gtsio2-library to read in a .gts-File in viennamesh */
        GTSIO::DeviceFileReader* pReader = new GTSIO::DeviceFileReader();
        GTSDevice* pDevice = new GTSDevice();
        try {
          pReader->loadDeviceFromFile(filename.c_str(), pDevice);
        } 
        catch (GTSIO::GtsioException ex) {
            cerr << ex.toString() << endl << endl;
        }
        catch (exception stdEx) {
          cerr << stdEx.what() << endl << endl;
        }
        
        try
        {
          if (pDevice == NULL) {
            cerr << "ERROR: Can't show device info!" << endl;
            throw;
          }
          
          cout << "File-Name: " << filename << endl;
          cout << "Device-Name: " << pDevice->getName() << endl;
          cout << "Dimension: " << pDevice->getDimension() << endl;
          
          cout << "Start reading the vertices!" << endl;
          
          //*********************************************************
          // Reading in the vertices (points)
          // Note: There's a different notation in the gtsio2-lib 
          //       (GTS) and the viennamesh-lib.
          //       - 'points' in GTS are 'vertices' in viennamesh
          //*********************************************************
          IGTSPointList* pointList = pDevice->getGTSPointList("Vertices");
          std::vector<double> pointCoords;      // coord. of a point in GTS
          long lenPointList   = 0;              // number of points defined in
                                                // the .gts-file
          long pointDim = 0; // dimension of a point (e.g.: 1D, 2D or 3D)   
          long i, j;         // index variables (used in 'for'-loops)
          
          lenPointList = pDevice->getGTSPointList("Vertices")->size();
          
          //reserve the memory:
          segment.template reserveVertices(lenPointList);
          Vertex vertex;
          
          cout << "# Vertices: " << lenPointList << endl;
          
          for(i = 0; i < lenPointList; i++)
          {
            CoordType coords[DimensionTag::dim]; // coordinates of a vertex (viennamesh)
            
            cout << "Point No. " << i << ": ";
            
            pointList->get(i,pointCoords);
            pointDim = pointCoords.size();
            
            /* copy the coordinates from GTS to viennamesh */
            for(j = 0; j < pointDim; j++)
            {
              cout.precision(3);
              cout << pointCoords[j] << " ";
              
              coords[j] = pointCoords[j];
            }
            
            //insert node into segment:
            vertex.getPoint().setCoordinates(coords);
            vertex.setID(i);
            segment.template add<0>(i, vertex);
            
            pointCoords.clear();
            cout << endl;
          }
          
          cout << "Reading the vertices finished!" << endl;
          
          cout << "Start reading the cells!" << endl;
              
          //***********************************************************
          // Reading in the cells
          // --------------------------------------------------------
          // In GTS there are several concepts for a
          // cell in viennamesh depending on dimension
          // --------------------------------------------------------
          //                 1D           2D           3D
          // GTS            line         face      polyhedron
          // viennamesh     cell         cell         cell
          //***********************************************************   
          GTSObjList* objList = gts_list_getter<DimensionTag::dim>::getList(pDevice);
        
          cout << "List was read in!" << endl;
          
          long lenObjList = 0;  // number of objects (lines, faces, polyhedrons) 
                                // defined in the .gts-file
          long pointsPerObj   = 0; // number of points per object
          long pointIdx    = 0; // index of a point in the 'pointList'
          
          cout << "Address Device: " << pDevice << ", Address List: " << objList << endl;
          
          lenObjList = objList->size();
          
          segment.reserveCells(lenObjList);
          Cell cell;
              
          cout << "No. of faces: " << lenObjList << endl;
          
          for(i = 0; i < lenObjList; i++) {
            GTSObj* pObject = objList->get(i);
            pointsPerObj = pObject->getNumberOfPoints(); // number of points per obj. (f.e. 3 in 2 dimensions)
            
            Vertex *vertices[TopologyLevel<CellTag, 0>::ElementNum]; // every cell is build up of
                                                                    // several vertices
            
            cout << "#" << i << ": ";
            
            /* read out the points of an obj. (GTS) and copy it to the cell (viennamesh) */
            for(j = 0; j < pointsPerObj; j++) {
              pointIdx = gts_point_getter<DimensionTag::dim,GTSObj>::getPointIdx(pObject,j);
              
              vertices[j] = segment.getVertexAddress(pointIdx);
              
              cout << pointIdx << " ";
            }
            
            /* add the cell */
            cell.setVertices(&(vertices[0]));
            cell.setID(i);
            segment.template add<CellTag::TopoLevel>(i, cell);
            
            cout << endl;
          }
          
          cout << "Reading the cells finished!" << endl;
        } catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }
      
        std::cout << "File-Reader finished!" << std::endl;
        
        delete pReader;
        delete pDevice;
      } //operator()
      
    }; //class gts_reader
  
  } //namespace io
} //namespace viennagrid

#endif
