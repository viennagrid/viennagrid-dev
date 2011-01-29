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

#ifndef VIENNAMESH_IO_GTS_WRITER_GUARD
#define VIENNAMESH_IO_GTS_WRITER_GUARD

#include <fstream>
#include <iostream>
#include <vector>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/gts_list_getter.hpp"
#include "viennagrid/point.hpp"

#include "gtsio2/UtilityLibs/gtsio2/src/include/gtsio2.h"


namespace viennagrid
{
  namespace io
  {
    template<long dim>
    struct gts_coords_getter;
    
    //********************************************************
    // struct gts_coords_getter
    // ------------------------
    // Gives the coords of a vertex in ViennaMesh
    //********************************************************
    // one dimension
    template<>
    struct gts_coords_getter<1> {
      static std::vector<double> getCoords(point<double,one_dimension_tag>& p) {
        std::vector<double> coords;
        coords.push_back(p.get_x());
        return coords;
      }
    };
    
    // two dimensions
    template<>
    struct gts_coords_getter<2> {
      static std::vector<double> getCoords(point<double,two_dimensions_tag>& p) {
        std::vector<double> coords;
        coords.push_back(p.get_x());
        coords.push_back(p.get_y());
        return coords;
      }
    };
    
    // three dimensions
    template<>
    struct gts_coords_getter<3> {
      static std::vector<double> getCoords(point<double,three_dimensions_tag>& p) {
        std::vector<double> coords;
        coords.push_back(p.get_x());
        coords.push_back(p.get_y());
        coords.push_back(p.get_z());
        return coords;
      }
    };
    
    //**************************************************
    // struct gts_point_adder
    // ----------------------
    // Adds a vertex to a GTSObject:
    // 1D GTSLine
    // 2D GTSFace
    // 3D GTSPolyhedron
    //**************************************************
    template<long dim, typename obj>
    struct gts_point_adder;
    
    // one dimension
    template<typename obj>
    struct gts_point_adder<1, obj> {
      static void addPoints(obj* pO, std::vector<int> points) {
        pO->Init(points[0], points[1]);
      }
    };
    
    // two dimensions
    template<typename obj>
    struct gts_point_adder<2, obj> {
      static void addPoints(obj* pO, std::vector<int> points) {
        pO->Init(points);
      }
    };
    
    // three dimensions
    template<typename obj>
    struct gts_point_adder<3, obj> {
      static void addPoints(obj* pO, std::vector<int> points) {
        pO->Init(points);
      }
    };
    
    //***********************************
    // .gts-file writer
    //***********************************
    struct gts_writer
    {
      template <typename DomainType>
      void operator()(DomainType & domain, std::string const & filename)
      {
        typedef typename DomainType::config_type                     DomainConfiguration;
      
        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;
        //typedef typename DomainConfiguration::BoundaryReadTag           BoundaryReadTag;
      
        typedef typename result_of::point_type<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell_type<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;
        //typedef typename DomainTypes<DomainConfiguration>::segment_type     Segment;
      
        typedef typename viennagrid::result_of::ncell_container<DomainType, 0>::type   VertexContainer;
        typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
        
        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level>::type     CellContainer;
        typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;

        typedef typename viennagrid::IteratorTypes<CellType, 0>::result_type      VertexOnCellIterator;
        
        typedef typename gts_list_getter<DimensionTag::value>::list_type       GTSObjList;
        typedef typename gts_list_getter<DimensionTag::value>::object_type     GTSObj;
        
        try {
          //Segment & segment = *(domain.begin());
            
          GTSIO::DeviceFileWriter* pWriter = new GTSIO::DeviceFileWriter();
          GTSDevice*               pDevice = new GTSDevice();
          
          /* Set up the basic device properties */
          pDevice->setDimension(DimensionTag::value);
          pDevice->setComment("This file is auto-generated from ViennaMesh.");
          pDevice->setName("Device");
          pDevice->setVersion("0.8");    

          std::cout << "Start writing the vertices! (GTSWriter)" << std::endl;
          std::cout << "Dimension: " << DimensionTag::value << std::endl;
          
          //***************************************************************
          // Writing the vertices
          //***************************************************************
          GTSPointList* pPointList = pDevice->NewGTSPointList("Vertices");
          
          VertexContainer vertices = viennagrid::ncells<0>(domain);
          for (VertexIterator vit = vertices.begin();
              vit != vertices.end();
              ++vit)
          {
            VertexType & vertex = *vit;
            std::vector<double> pointCoords;
            
            std::cout << "Vertex Id: " << vertex.getID() << ", ";
            std::cout << vertex.getPoint() << std::endl;

            pointCoords = gts_coords_getter<DimensionTag::value>::getCoords(vertex.getPoint());
            
            pPointList->add(pointCoords);
          }
            
          std::cout << "End writing the vertices!" << std::endl;
          std::cout << "Start writing the cells!" << std::endl;
            
          //***********************************************************
          // Writing the cells
          // ---------------------------------------------------------    
          // In GTS there are several concepts for a
          // cell in ViennaMesh depending on the dimension
          // ---------------------------------------------------------
          //                 1D           2D           3D
          // GTS            line         face      polyhedron
          // ViennaMesh     cell         cell         cell
          //***********************************************************   
          GTSObjList* pObjList = gts_list_getter<DimensionTag::value>::newList(pDevice);
          
          long pointID;
          
          //segment.template size<CellTag::topology_level>();
          CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
            CellType & cell = *cit;
            GTSObj* pObj = new GTSObj();
            std::vector<int> points;
            
            std::cout << "Cell Id: " << cell.getID() << ":" << std::endl;

            for (VertexOnCellIterator vocit = cell.template begin<0>();
              vocit != cell.template end<0>();
              ++vocit)
            {	  
              pointID = vocit->getID();
              points.push_back(pointID);
              
              std::cout << "Point Id: " << pointID << " ";
            }

            gts_point_adder<DimensionTag::value, GTSObj>::addPoints(pObj, points);
            
            pObjList->add(pObj);

            std::cout << "Address: " << pObj << std::endl;
            std::cout << std::endl;
          }
          
          // defines that the obj-list (lines, faces, polyhedrons) is build up of points (vertices)
          pObjList->setParentList(pPointList);
            
          std::cout << "End writing the cells!" << std::endl;     
            
          pWriter->WriteDeviceToFile("gts", filename.c_str(), pDevice);
          
          delete pWriter;
          delete pDevice;
        } catch(...) {
          cerr << "Error: Problem while writing the file!";
        }
      } //operator()
    }; // gts_writer
    
  } //io
} //namespace

#endif


