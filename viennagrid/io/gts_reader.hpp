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
#include "viennagrid/io/gts_list_getter.hpp"

#include "gtsio2/UtilityLibs/gtsio2/src/include/gtsio2.h"

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
        typedef typename DomainType::config_type                     DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename DomainTypes<DomainConfiguration>::point_type    Point;
        typedef typename DomainTypes<DomainConfiguration>::vertex_type   Vertex;
        typedef typename DomainTypes<DomainConfiguration>::cell_type     Cell;
        typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;

        typedef typename viennagrid::result_of::ncell_container<DomainType, 0>::type   VertexContainer;
        typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
            
        typedef typename viennagrid::result_of::ncell_container<DomainType, 1>::type   EdgeContainer;
        typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level-1>::type   FacetContainer;
        typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level>::type     CellContainer;
        typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;
  
        typedef typename gts_list_getter<DimensionTag::value>::list_type         GTSObjList;
        typedef typename gts_list_getter<DimensionTag::value>::object_type       GTSObj;
        

        std::cout << "Reading file " << filename << std::endl;
        
        /* using the gtsio2-library to read in a .gts-File in viennamesh */
        GTSIO::DeviceFileReader* pReader = new GTSIO::DeviceFileReader();
        GTSDevice* pDevice = new GTSDevice();
        try {
          pReader->loadDeviceFromFile(filename.c_str(), pDevice);
        } 
        catch (GTSIO::GtsioException ex) {
          std::cerr << ex.toString() << std::endl << std::endl;
        }
        catch (std::exception stdEx) {
          std::cerr << stdEx.what() << std::endl << std::endl;
        }
        
        try
        {
          if (pDevice == NULL) {
            std::cerr << "ERROR: Can't show device info!" << std::endl;
            throw;
          }
          
          std::cout << "File-Name: " << filename << std::endl;
          std::cout << "Device-Name: " << pDevice->getName() << std::endl;
          std::cout << "Dimension: " << pDevice->getDimension() << std::endl;
          
          std::cout << "Start reading the vertices!" << std::endl;
          
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
          domain.reserve_vertices(lenPointList);
          Vertex vertex;
          
          std::cout << "# Vertices: " << lenPointList << std::endl;
          
          for(i = 0; i < lenPointList; i++)
          {
            CoordType coords[DimensionTag::value]; // coordinates of a vertex (viennamesh)
            
            std::cout << "Point No. " << i << ": ";
            
            pointList->get(i,pointCoords);
            pointDim = pointCoords.size();
            
            /* copy the coordinates from GTS to viennamesh */
            for(j = 0; j < pointDim; j++)
            {
              std::cout.precision(3);
              std::cout << pointCoords[j] << " ";
              
              coords[j] = pointCoords[j];
            }
            
            //insert node into segment:
            vertex.getPoint().setCoordinates(coords);
            vertex.setID(i);
            domain.add(vertex);
            
            pointCoords.clear();
            std::cout << std::endl;
          }
          
          std::cout << "Reading the vertices finished!" << std::endl;
          
          std::cout << "Start reading the cells!" << std::endl;
              
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
          GTSObjList* objList = gts_list_getter<DimensionTag::value>::getList(pDevice);
        
          std::cout << "List was read in!" << std::endl;
          
          long lenObjList = 0;  // number of objects (lines, faces, polyhedrons) 
                                // defined in the .gts-file
          long pointsPerObj   = 0; // number of points per object
          long pointIdx    = 0; // index of a point in the 'pointList'
          
          std::cout << "Address Device: " << pDevice << ", Address List: " << objList << std::endl;
          
          lenObjList = objList->size();
          
          domain.reserve_cells(lenObjList);
          Cell cell;
              
          std::cout << "No. of faces: " << lenObjList << std::endl;
          
          for(i = 0; i < lenObjList; i++) {
            GTSObj* pObject = objList->get(i);
            pointsPerObj = pObject->getNumberOfPoints(); // number of points per obj. (f.e. 3 in 2 dimensions)
            
            Vertex *vertices[subcell_traits<CellTag, 0>::num_elements]; // every cell is build up of
                                                                    // several vertices
            
            std::cout << "#" << i << ": ";
            
            /* read out the points of an obj. (GTS) and copy it to the cell (viennamesh) */
            for(j = 0; j < pointsPerObj; j++) {
              pointIdx = gts_point_getter<DimensionTag::value,GTSObj>::getPointIdx(pObject,j);
              
              vertices[j] = &(domain.vertex(pointIdx));
              
              std::cout << pointIdx << " ";
            }
            
            /* add the cell */
            cell.setVertices(&(vertices[0]));
            cell.setID(i);
            domain.add(cell);
            
            std::cout << std::endl;
          }
          
          std::cout << "Reading the cells finished!" << std::endl;
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
