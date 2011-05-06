/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_IO_SGF_READER_GUARD
#define VIENNAGRID_IO_SGF_READER_GUARD


#include <fstream>
#include <iostream>
#include "viennagrid/forwards.h"

namespace viennagrid
{
  namespace io
  {

    struct sgf_reader
    {
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename) const
      {
        typedef typename DomainType::config_type                     DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename result_of::point_type<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell_type<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;
        //typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;

        typedef typename viennagrid::result_of::ncell_container<DomainType, 0>::type   VertexContainer;
        typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
            
        typedef typename viennagrid::result_of::ncell_container<DomainType, 1>::type   EdgeContainer;
        typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level-1>::type   FacetContainer;
        typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level>::type     CellContainer;
        typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;

        std::ifstream reader(filename.c_str());

        //Segment & segment = *(domain.begin());

        std::cout << "* sgf_reader::operator(): Reading file " << filename << std::endl;

        if (!reader){
          std::cerr << "Cannot open file " << filename << std::endl;
          throw "Cannot open file";
          return EXIT_FAILURE;
        }

        try{
          std::string token;
          long node_num = 0;
          long cell_num = 0;
      
          reader >> token;
      
          //Dimension:
          if (token == "Dimension:"){
            long file_dim = 0;
            reader >> file_dim;
            if (file_dim != DimensionTag::value)
            {
              std::cout << "Dimension incorrect! Got " << file_dim << ", but expected " << DimensionTag::value << ". Exiting..." << std::endl;  
              exit(0);
            }
            //else
            //  std::cout << "Dimension ok" << std::endl;
          } else
            throw;
      
          //Vertices:
          reader >> token;
          if (token == "Nodes:" || token == "Vertices:")
            reader >> node_num;
          else{
            std::cerr << "ERROR: Expected keyword 'Vertices:', but got " << token << std::endl;
            throw;
          }

          //std::cout << "Reading " << node_num << " vertices... " << std::endl;  
          //reserve the memory:
          domain.reserve_vertices(node_num);
          VertexType vertex;
          long node_id;
      
          for (int i=0; i<node_num; i++)
          {
            CoordType coords[DimensionTag::value];
      
            reader >> node_id;
      
            for (int j=0; j<DimensionTag::value; j++)
              reader >> coords[j];
      
            //insert node into segment:
            //std::cout << std::endl << "Adding vertex: " << &vertex << std::endl;
            vertex.getPoint().setCoordinates(coords);
            vertex.setID(node_id);
            domain.add(vertex);
          }
      
          //std::cout << "DONE" << std::endl;
      
          //Cells:
          reader >> token;
          if (token != "Cell-Type:")
          {
            std::cerr << "ERROR: Expected keyword 'Cell-Type:', but got " << token << std::endl;
            throw;
          }
      
          reader >> token;
          if (token != "Simplex" && token != "Ortho")
          {
            std::cerr << "ERROR: Cell-Type " << token << " not supported! You might fix this issue by recompilation." << std::endl;
            throw;
          }
      
          reader >> token;
          if (token != "Cells:")
          {
            std::cerr << "ERROR: Expected keyword 'Cells:', but got " << token << std::endl;
            throw;
          }
      
          //std::cout << "Reading cell-num:" << std::endl;
          reader >> cell_num;
          //std::cout << "Reserve cells:" << std::endl;
          domain.reserve_cells(cell_num);
          long cell_id;
          //std::cout << "Creating cell:" << std::endl;
          CellType cell;
          //std::cout << "Filling cells:" << std::endl;
      
          for (int i=0; i<cell_num; ++i)
          {
            long vertex_num;
            VertexType *vertices[traits::subcell_desc<CellTag, 0>::num_elements];
            reader >> cell_id;
      
            for (int j=0; j<traits::subcell_desc<CellTag, 0>::num_elements; ++j)
            {
              reader >> vertex_num;
              vertices[j] = &(domain.vertex(vertex_num));
            }
      
            //std::cout << std::endl << "Adding cell: " << &cell << " at " << cell_id << std::endl;
            cell.setVertices(&(vertices[0]));
            cell.setID(cell_id);
            domain.add(cell);
      
            //progress info:
            if (i % 50000 == 0 && i > 0)
              std::cout << "* sgf_reader::operator(): " << i << " out of " << cell_num << " cells read." << std::endl;
      
          }
      
          //std::cout << "Reading quantities..." << std::endl;
      
          //Quantities:
          while (reader >> token){
      
            std::string quan_name;
            long quan_num;
            if (token != "Quantity")
            {
              std::cerr << "ERROR: Expected 'Quantity', but got " << token << std::endl;
              throw;
            }
      
            reader >> quan_name;
            reader >> token;
      
            if (token != "on")
            {
              std::cerr << "ERROR: Expected 'on', but got " << token << std::endl;
              throw;
            }
      
            reader >> token;
            reader >> quan_num;
      
            if (token == "Vertex:")
            {
              double quantity;
              long vertexID;
      
              for (long i=0; i<quan_num; ++i)
              {
                reader >> vertexID;
                reader >> quantity;
                //special treatment for boundary values:
    /*            if (quan_name == "bnd_value1")
                  segment.getVertexNC(vertexID).storeQuantity(0, quantity);
                else if (quan_name == "bnd_value2")
                  segment.getVertexNC(vertexID).storeQuantity(1, quantity);
                else if (quan_name == "bnd_value3")
                  segment.getVertexNC(vertexID).storeQuantity(2, quantity);
                else
                  segment.getVertexNC(vertexID).storeQuantity(quan_name, quantity);*/
              }
            }
            //else if (token == "Edge:")
            //{
      
            //}
            else if (token == "Cell:")
            {
              double quantity;
              long cellID;
      
              for (long i=0; i<quan_num; ++i)
              {
                reader >> cellID;
                reader >> quantity;
                //segment.getCellNC(cellID).storeQuantity(quan_name, quantity);
              }
            }
            else
            {
              std::cerr << "ERROR: Quantity type " << token << " not implemented (yet)." << std::endl;
              throw;
            } 
            
          } // while
      
        } catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }
      
        //std::cout << "File-Reader finished!" << std::endl;  
        
        return EXIT_SUCCESS;
      } //operator()
      
    }; //class sgf_reader
  
  } //namespace io
} //namespace viennagrid

#endif
