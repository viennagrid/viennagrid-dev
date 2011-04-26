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

#ifndef VIENNAGRID_IO_GAU_READER_GUARD
#define VIENNAGRID_IO_GAU_READER_GUARD


#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "viennautils/file.hpp"

#include "viennagrid/domain.hpp"
#include "viennagrid/forwards.h"
#include "viennagrid/boundary.hpp"

#include "boost/lexical_cast.hpp"

//#define IODEBUG

namespace viennagrid
{
namespace io
{
   template <typename DomainType>
   struct gau_reader
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
         std::string file_ending = viennautils::file_extension(filename);

         std::size_t dimension_geometry = boost::lexical_cast<unsigned int>( file_ending.substr(3,1) );
         std::size_t dimension_topology = dimension_geometry;
         if (file_ending.size() > 4)
            dimension_topology = boost::lexical_cast<unsigned int>(file_ending.substr(4,2));

         if(DimensionTag::value != dimension_geometry)
         {
            std::cerr << "Error reading GAU file: geometry dimension does not fit the present the domain" << std::endl;
            return -1;
         }
         if(DimensionTag::value != dimension_geometry)
         {
            std::cerr << "Error reading GAU file: geometry dimension does not fit the present the domain" << std::endl;
            return -1;
         }

         std::ifstream reader(filename.c_str());
         if (!reader){
            std::cerr << "Cannot open file " << filename << std::endl;
            throw "Cannot open file";
            return EXIT_FAILURE;
         }

         std::size_t file_dimension_geometry;
         reader >> file_dimension_geometry;
         if(file_dimension_geometry != dimension_geometry)
         {
            std::cerr << "Error reading GAU file: geometry data dimension does not fit file geometry dimension" << std::endl;
            return -1;
         }



         // ############################ geometry #########################
         
      #ifdef IODEBUG
         std::cout << "processing geometry .. " << std::endl;
      #endif
         std::string trash;
         std::size_t point_size = 0;
         reader >> point_size;   

      #ifdef IODEBUG
         std::cout << "geometry size: " << point_size << std::endl;
      #endif

         domain.reserve_vertices(point_size);

         for (std::size_t i = 0; i < point_size; i++)
         {
            CoordType coords[DimensionTag::value];
            VertexType vertex;

            for (std::size_t dim = 0; dim < DimensionTag::value; ++dim)
            {
               reader >> coords[dim];
            }
         #ifdef IODEBUG
            std::cout << "  id: " << i << " : " << coords[0] << " " << coords[1] << " " << coords[2] << std::endl;
         #endif
            vertex.getPoint().setCoordinates(coords);
            vertex.setID(i);
            domain.add(vertex);
         }

         // ############################ topology #########################
      #ifdef IODEBUG
         std::cout << "processing topology .. " << std::endl;
      #endif
         std::size_t segment_size;
         reader >> segment_size;
      #ifdef IODEBUG
         std::cout << "seg size: " << segment_size << std::endl;
      #endif
         domain.create_segments(segment_size);

         // this container will temporarily store the cells per segment.
         // first we'll read everything into this container, so that we know
         // how many cells in total we have to deal with. 
         // when we do know this, we can actually add the cells to the domain/segments.
         typedef std::vector<CellType>                   cell_cont_type;
         typedef std::map<std::size_t, cell_cont_type >   segment_cell_map_type;
         segment_cell_map_type segment_cell_map;

         std::string segment_name;
         reader >> segment_name;  
      #ifdef IODEBUG
         std::cout << "seg name: " << segment_name << std::endl;
      #endif
         if (segment_name == "Segment:")   reader >> segment_name;

         std::size_t global_cell_size = 0;

         for (std::size_t segment_cnt = 0; segment_cnt < segment_size; ++segment_cnt)
         {
            std::size_t cell_size = 0;
            reader >> cell_size;
            reader >> trash;
         #ifdef IODEBUG
            std::cout << "  seg id: " << segment_cnt << " - cell size: " << cell_size << std::endl;
         #endif
            for (std::size_t nu=0; nu < cell_size; ++nu)
            {
               global_cell_size++;
               VertexType *vertices[viennagrid::subcell_traits<CellTag, 0>::num_elements];        

               if (dimension_topology== 1)
               {
                  std::size_t ind[2];
                  reader >> trash >> ind[0] >> ind[1];
                  
                  vertices[0] = &(domain.vertex(ind[0]));               
                  vertices[1] = &(domain.vertex(ind[1]));
               #ifdef IODEBUG
                  std::cout << "  id: " << " : " << ind[0] << " " << ind[1] << std::endl;
               #endif
               }
               else if (dimension_topology== 2)
               {
                  std::size_t ind[3];
                  reader >> trash >> ind[0] >> ind[1] >> ind[2];
                  vertices[0] = &(domain.vertex(ind[0]));               
                  vertices[1] = &(domain.vertex(ind[1]));
                  vertices[2] = &(domain.vertex(ind[2]));
               #ifdef IODEBUG
                  std::cout << "  id: " << " : " << ind[0] << " " << ind[1] << " " << ind[2] << std::endl;
               #endif
               }
               else if (dimension_topology == 3)
               {
                  std::size_t ind[4];
                  reader >> trash >> ind[0] >> ind[1] >> ind[2] >> ind[3];
                  vertices[0] = &(domain.vertex(ind[0]));               
                  vertices[1] = &(domain.vertex(ind[1]));
                  vertices[2] = &(domain.vertex(ind[2]));
                  vertices[3] = &(domain.vertex(ind[3]));
               #ifdef IODEBUG
                  std::cout << "  id: " << " : " << ind[0] << " " << ind[1] << " " << ind[2] << " " << ind[3] << std::endl;
               #endif
               }
               CellType cell;
               cell.setVertices(vertices);
               segment_cell_map[segment_cnt].push_back(cell);
            }

            // ############################ quantity #########################
            // TODO currently data is only read but -NOT- written on the domain !
         #ifdef IODEBUG
            std::cout << "processing quantity .. " << std::endl;
         #endif
            std::string token;
            while(1)
            { 
               reader >> token;  
               //std::cout << "token: " << token << std::endl;
               if (token != "Quantity:") 
               {
                  segment_name = token;
                  if (segment_name == "Segment:")
                  {
                     reader >> segment_name;
                  }
                  break;
               }
               reader >> token;  
               //std::cout << "quantity type: " << token << std::endl;
               // *********  vertex quantities

               if (token == "vertex")
               {
                  long number_quan;
                  reader >> number_quan;    
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> token; // quan name
                     long numb_val;
                     reader >> numb_val;
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        long index;
                        double tempval2;
                        reader >> index;
                        reader >> tempval2;
                     }
                  } 
               }
               else if (token == "sheaf_vertex")
               {
                  long number_quan;
                  std::string quan_name;
                  reader >> number_quan;    

                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> quan_name; // quan name

                     long numb_val;
                     long data_nb_1 =1, data_nb_2 =1;
       
                     reader >> token; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix

                     reader >> data_nb_1;
                     reader >> data_nb_2;
                     reader >>  numb_val;

                     for (long nv=0; nv < numb_val;++nv)
                     {
                        long index;
                        reader >> index;
                        for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                        {
                           for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
                           {
                              reader >> trash;
                           }
                        }
                     }
                  } 
               }

               // *********  edge quantities

               else if (token == "edge")
               {
                  long number_quan;
                  reader >> number_quan;    
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> token; // quan name
                     long numb_val;
                     reader >> numb_val;
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        long index;
                        double tempval2;
                        reader >> index;
                        reader >> tempval2;

                     }
                  } 
               }
               else if (token == "sheaf_edge")
               {
                  long number_quan;
                  std::string quan_name;
                  reader >> number_quan;    
                  //std::cout << "    ## sheaf edge .. number: "<< number_quan << std::endl;          
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader    >> quan_name; // quan name

                     //std::cout << "    ## sheaf edge .. quanname: "<< quan_name << std::endl;          

                     long numb_val;
                     long data_nb_1 =1, data_nb_2 =1;
                     
                     reader >> token; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix
                     reader >> data_nb_1;
                     reader >> data_nb_2;
                     reader >>  numb_val;
                     
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        reader >> token;
                        //std::cout << "  edge handle: " << index << std::endl;

                        for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                        {
                           for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
                           {
                              reader >> trash;
                           }
                        }
                     }
                  } 
               }

               // *********  face quantities

               else if (token == "facet")
               {
                  long number_quan;
                  reader >> number_quan;    
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> token; // quan name
                     //std::cout << "  quan: " << tempstr;
                     long numb_val;
                     reader >> numb_val;
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        reader >> trash;
                        reader >> trash;

                        
                     }
                  } 
               }
               else if (token == "sheaf_facet")
               {
                  long number_quan;
                  std::string quan_name;
                  reader >> number_quan;    
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> quan_name; // quan name
                     
                     long numb_val;
                     long data_nb_1 =1, data_nb_2 =1;
                     
                     reader >> token; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix
                     reader >> data_nb_1;
                     reader >> data_nb_2;
                     reader >>  numb_val;
                     
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        reader >> token;
                        
                        for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                        {
                           for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
                           {
                              reader >> token;
                           }
                        }
                     }
                  } 
               }
               
               // *********  cell quantities

               else if (token == "cell")
               {
                  long number_quan;
                  reader >> number_quan;    
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> token; // quan name
                     //std::cout << "  quan: " << tempstr;
                     long numb_val;
                     reader >> numb_val;
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        long index;
                        double tempval2;
                        reader >> index;
                        reader >> tempval2;
                     }
                  } 
               }
               else if (token == "sheaf_cell")
               {  
                  long number_quan;
                  std::string quan_name;
                  reader >> number_quan;    
                  for (long quani=0; quani < number_quan; ++quani)
                  {
                     reader >> quan_name; // quan name

                     long numb_val;
                     long data_nb_1 =1, data_nb_2 =1;
                     
                     reader >> token; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix
                     reader >> data_nb_1;
                     reader >> data_nb_2;
                     reader >>  numb_val;
                     
                     for (long nv=0; nv < numb_val;++nv)
                     {
                        long index;
                        reader >> index;
                        
                        for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                        {
                           for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
                           {
                              reader >> token;
                           }
                        }
                     }
                  } 
               }
               
               else
               {
                  // Use the entry which was just read in for the next
                  // segment               
                  segment_name = token;
               } 
            } // end while loop
         } // end segment traversal
      #ifdef IODEBUG
         std::cout << "creating domain cells .. " << std::endl;
      #endif
         // now that all segments and cells have been read, and due to that 
         // we are aware of how many cells in total there are, 
         // we can actually create the cells within the input domain
         domain.reserve_cells(global_cell_size);

         for(typename segment_cell_map_type::iterator sit = segment_cell_map.begin();
             sit != segment_cell_map.end(); sit++)
         {
            for(typename cell_cont_type::iterator cit = sit->second.begin();
                cit != sit->second.end(); cit++)
            {
               domain.segment(sit->first).add(*cit);            
            }
         }
      }
   };

   template < typename DomainType > 
   int importGAU(DomainType & domain, std::string const & filename)
   {
      viennagrid::io::gau_reader<DomainType> reader;
      return reader(domain, filename);
   }  

} // end namespace io
} // end namespace viennagrid

#endif

