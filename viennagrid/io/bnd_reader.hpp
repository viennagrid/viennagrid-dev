/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                    weinbub@iue.tuwien.ac.at
               Karl Rupp                           rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_IO_BND_READER_GUARD
#define VIENNAGRID_IO_BND_READER_GUARD


#include <fstream>
#include <iostream>

#include "viennagrid/forwards.h"
#include "viennagrid/boundary.hpp"

namespace viennagrid
{
  namespace io
  {
    template <typename DomainType>
    struct bnd_reader
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


      int operator()(DomainType & domain, std::string const & filename) const
      {
        std::ifstream reader(filename.c_str());



        if (!reader){
          std::cerr << "Cannot open file " << filename << std::endl;
          throw "Cannot open file";
          return EXIT_FAILURE;
        }

        try{
        
          std::string token;
            
          std::size_t vertices = 0;
        
          while(1)
          {
            reader >> token;
            if(token == "Vertices")
            {
              break;
            }
          }
          reader >> token;     // (
          reader >> vertices;  // number of vertices
          reader >> token;     // )
          reader >> token;     // {

          domain.reserve_vertices(vertices);
          VertexType vertex;

          for(std::size_t vi = 0; vi < vertices; vi++)
          {
            CoordType coords[DimensionTag::value];
            for (int j=0; j<DimensionTag::value; j++)
              reader >> coords[j];

            vertex.getPoint().setCoordinates(coords);
            vertex.setID(vi);
            domain.add(vertex);
//            std::cout << coords[0] << " " << coords[1] << " " << coords[2] << std::endl;
//            if(vi == 10) exit(0);
          }
          while(1)
          {
            reader >> token;
            if(token == "Faces")
            {
              break;
            }
          }
          std::size_t faces = 0;          
          reader >> token;     // (
          reader >> faces;  // number of faces
          reader >> token;     // )
          reader >> token;     // {

          // note: in principle we support facets of varying dimensions .. 
          // but we restrict ourselves for triangles for now .. look for the assert 
          // in a couple of lines ..
          std::vector< std::vector< long > > face_container(faces);   
          std::size_t face_dim = 0;
          
          for(std::size_t fi = 0; fi < faces; fi++)
          {
            reader >> face_dim; // number of vertices of this face

            //std::cout << "face dim: " << face_dim << std::endl;

            assert(face_dim == 3); // only triangular facets supported ..

            face_container[fi].resize(face_dim);

            for (int j=0; j < face_dim; j++)
            {
              reader >> face_container[fi][j];
            }
//            std::cout << face_container[fi][0] << " " << face_container[fi][1] << " " << face_container[fi][2] << std::endl;
//            if(fi == 10) exit(0);
          }
      
        } catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }
        return EXIT_SUCCESS;
      } //operator()
      
    }; //class bnd_reader
  
    template < typename DomainType > 
    int importBND(DomainType & domain, std::string const & filename)
    {
      bnd_reader<DomainType> reader;
      return reader(domain, filename);
    }  
  
  } //namespace io
} //namespace viennagrid

#endif
