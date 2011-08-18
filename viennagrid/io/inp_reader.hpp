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

#ifndef VIENNAGRID_IO_INP_READER_GUARD
#define VIENNAGRID_IO_INP_READER_GUARD


#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "viennagrid/domain.hpp"
#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/boundary.hpp"

namespace viennagrid
{
namespace io
{
   //
   // [INFO] the default usage expects each new element section in an 
   // inp file to represent a segment
   // however, there is a special case we internally require which merges 
   // the element sections according to the section names. to trigger this, 
   // one has to activate the merge_domains flag. 
   // related to this special scenario: 
   //   the element section names are expected to look like this
   //   xxxID1_ID2 .. ie: ELE3_4
   //   the prefix, ELE, is skipped. if the prefix happens to be longer, 
   //   for example SUPERELE3_4, simply adapt the skip_prefix parameter 
   //   to the corresponding size, in this case, 8.
   //
   template <typename DomainType>
   struct inp_reader
   {
      typedef std::size_t                                         index_type;
      typedef double                                              numeric_type;

      static const std::size_t DIMG = 3;
      
      std::string segment_set_delimiter;

      inp_reader()
      {
         // this is the character which seperates the segment ids in the 
         // special case scenario, for example, ELE3_4.
         // it probably won't ever be necc to change this seperator, 
         // however, the internals check against this string, so feel free 
         // to alter it, if required.
         //
         segment_set_delimiter = "_";
      }


      int operator()(DomainType & domain, std::string const & filename, 
                     bool merge_domains = false, int skip_prefix = 3) const
      {       
         // geometry storage
         typedef viennagrid::point<numeric_type, DIMG>         point_type;
         typedef std::map<std::size_t, point_type>             point_map_type;
         point_map_type point_map;

         // topology storage
         typedef std::vector<index_type>                       cell_type;
         //typedef std::map<std::size_t, cell_type>              cell_map_type;
         typedef std::vector<cell_type>                        cell_map_type;
         typedef std::map<std::size_t, cell_map_type>          seg_cell_type;
         seg_cell_type  seg_cell;
         
         // import the inputfile and extract data to temporary datastructure
         //    note: this function takes care of the (optional) segment merges
         //          the segments of the temporary datastructure already 
         //          represent the finalized segments - all optional merges are done
         read_data(filename, point_map, seg_cell, merge_domains, skip_prefix);
         
         // transfer pre-processed data from temporary datastructure to viennagrid domain
         transfer_to_domain(domain, point_map, seg_cell);
         
         return EXIT_SUCCESS;
      }
      
      template<typename PointT, typename CellComplexT>
      int transfer_to_domain( DomainType&                            domain, 
                              std::map<std::size_t, PointT>&         point_map, 
                              std::map<std::size_t, CellComplexT>&   seg_cell) const
      {
         typedef typename DomainType::config_type                                            domain_config_type;
         typedef typename domain_config_type::cell_tag                                       cell_tag;
         typedef typename domain_config_type::numeric_type                                   coord_type;      
         typedef typename viennagrid::result_of::ncell_type<domain_config_type, 0>::type     vertex_type;         
         typedef typename viennagrid::result_of::ncell_type<domain_config_type, cell_tag::topology_level>::type     cell_type;         

         std::map<std::size_t, std::size_t>  index_map;
      
         //domain.reserve_vertices(point_map.size());

         typedef typename std::map<std::size_t, PointT>::iterator    point_iter_type;
         for(point_iter_type iter = point_map.begin(); iter != point_map.end(); iter++)
         {
            vertex_type    vertex;
            vertex.getPoint()[0] = iter->second[0];
            vertex.getPoint()[1] = iter->second[1];
            vertex.getPoint()[2] = iter->second[2];
            index_map[iter->first] = domain.add(vertex)->id();
            //std::cout << iter->first << " - " << vertex.id() << std::endl;
            //std::cout << vertex.id() << std::endl;
         }
         
         domain.create_segments(seg_cell.size());         
         //std::cout << "seg size: " << seg_cell.size() << std::endl;
//         std::size_t cellsize(0);
         typedef typename std::map<std::size_t, CellComplexT>::iterator seg_iter_type;
//         for(seg_iter_type si = seg_cell.begin(); si != seg_cell.end(); si++)
//         {
//            cellsize += si->second.size();
//         }
         //std::cout << "cellsize: " << cellsize << std::endl;
         //domain.reserve_cells(cellsize);
         
         static const std::size_t CELLSIZE = viennagrid::topology::subcell_desc<cell_tag, 0>::num_elements;         

         //std::cout << "transferring topology .. " << std::endl;
         for(seg_iter_type si = seg_cell.begin(); si != seg_cell.end(); si++)
         {
            typedef typename CellComplexT::iterator   cell_iter_type;
            for(cell_iter_type cit = si->second.begin(); cit != si->second.end(); cit++)
            {
               vertex_type *vertices[CELLSIZE];                    
               for(std::size_t i = 0; i < CELLSIZE; i++)
               {
                  //std::cout << (*cit)[i] << " - " << index_map[(*cit)[i]] << std::endl;
//                  if( index_map.find((*cit)[i]) != index_map.end() )
//                  {
//                     std::cout << "found" << std::endl;
//                  }
//                  else std::cout << "not found" << std::endl;

                  //std::cout << domain.vertex( index_map[(*cit)[i]] ).getPoint() << std::endl; 
                  vertices[i] = &(domain.vertex( index_map[(*cit)[i]] ));               
               }
               //exit(0);
               cell_type cell;
               cell.setVertices(vertices);               
               //std::cout << "si: " << si->first << std::endl;
               
               domain.segment((si->first)).add(cell);            
            }
         }
         
         return EXIT_SUCCESS;
      }      
      
      template<typename PointT, typename CellComplexT>
      int read_data(std::string const& filename, 
                     std::map<std::size_t, PointT>&         point_map, 
                     std::map<std::size_t, CellComplexT>&   seg_cell, 
                     bool                                   merge_domains, 
                     int                                    skip_prefix) const
      {
         std::ifstream reader(filename.c_str());
         if (!reader){
            std::cerr << "Cannot open file " << filename << std::endl;
            throw "Cannot open file";
            return EXIT_FAILURE;
         }      

         std::string   token;
         index_type    index;
         index_type    si(0); // only required if the domains are not merged 
      
         PointT point;
         typename CellComplexT::value_type  cell;      
      
         reader >> token;
         while(1)
         {
            if(token.find("NODE") != std::string::npos)
            {
               while(reader >> token)
               {
                  // read until '*' has been read ..
                  if(token.find("*") != std::string::npos) 
                  {
                     reader >> token; // prepare token for next round
                     break;
                  }
                  // read in the leading index number
                  index = atol(token.c_str()); // atol removes the ','
                  // read in the x,y,z coordinate values
                  for(std::size_t i = 0; i < DIMG; i++)
                  {
                     reader >> token;
                     point[i] = atof(token.c_str()); // atof removes the ','
                  }

                  // store the point in a map with ordered indices
                  point_map[index] = point;

//                  std::cout << "point: " << index << " - "; 
//                  for(int i = 0; i < DIMG; i++)
//                  {
//                     std::cout << point[i] << " ";
//                  }
//                  std::cout << std::endl;                  
               }
            }
            if(token.find("ELEMENT") != std::string::npos)
            {
               reader >> token; // TYPE

               index_type  cellsize;

               // ABAQUS element: S3 -> shell element with 3 nodes
               // aka: triangle 
               if(token.find("S3") != std::string::npos)
               {
                  cellsize = 3; // ... 3 nodes ..
               }
               else
               {
                  std::cerr << "INP reader does not support this element type: " << token << std::endl;
                  exit(-1);
               }

               CellComplexT cell_complex;
               cell.resize(cellsize);
               reader >> token; // segment name

               std::vector<index_type>    segment_indices;

               // if merge domains  is true, 
               // we expect a certain setup of the domain name:
               // segid1_segid2
               // we have to copy the data in the current cell complex run to 
               // both segments mentioned in the segment name
               //
               if(merge_domains)
               {
                  //std::cout << "token: " << token << std::endl;
                  if(token.find(segment_set_delimiter) != std::string::npos)
                  {
                     // about the offset here: 
                     // as the result of the find function points at the beginning of string
                     // which starts with '=', we don't want the equal sign, so we have to increment
                     // furthermore, we might expect some additional string infront of the actual 
                     // information - we don't need this, so skip it
                     //
                     std::string::size_type value_begin = token.find("=")+1+skip_prefix;
                     std::string value = token.substr(value_begin, std::string::npos);
                     //std::cout << "value: " << value << std::endl;

                     std::string::size_type delimiter = value.find(segment_set_delimiter);
                     std::string id1 = value.substr(0, delimiter);
                     std::string id2 = value.substr(delimiter+1, std::string::npos);                    
                     
                     segment_indices.push_back(atol(id1.c_str()));
                     segment_indices.push_back(atol(id2.c_str()));                     
                  }
               }

               while(reader >> token)
               {                  
                  //std::cout << "token: " << token << std::endl;
                  // read until '*' has been read ..
                  if(token.find("*") != std::string::npos)
                  {
                     // token already contains the next valid entry, 
                     // do don't read the next token ..
                     break;
                  }
                  // read in the leading index number
                  index = atol(token.c_str());

                  // read in the x,y,z coordinate values
                  for(std::size_t i = 0; i < cellsize; i++)
                  {
                     reader >> token;
                     cell[i] = atol(token.c_str());
                  }
                  // store the point in a map with ordered indices
                  cell_complex.push_back(cell);
//                  std::cout << "cell: " << index << " - "; 
//                  for(int i = 0; i < cellsize; i++)
//                  {
//                     std::cout << cell[i] << " ";
//                  }
//                  std::cout << std::endl;
               }
               // we are done reading this segment/cell-complex
               // store it with the respective segment id
               if(!merge_domains)
               {
                  seg_cell[si] = cell_complex;
                  si++;
               }
               else
               {
                  // assign the currently read cell complex to all segments 
                  // which are indicated by the cell complex name
                  for(std::vector<index_type>::iterator iter = segment_indices.begin();
                      iter != segment_indices.end(); iter++)
                  {
                     std::size_t segment_index = *iter;
                     // only if the id is not zero, as this is typically air .. 
                     if(segment_index != 0)
                     {
                        // the segment index is 1 based, decrement it to
                        // get a 0 based index
                        segment_index--;
                                             
                        // if the segment has not been added before, initiate
                        // a new one
                        if(seg_cell.find(segment_index) == seg_cell.end())
                        {
                           seg_cell[segment_index] = cell_complex;
                        }
                        // if in fact it has been added before, add this cell complex 
                        // to the cell complex already associated with the segment
                        else
                        {
                           seg_cell[segment_index].insert(seg_cell[segment_index].end(), cell_complex.begin(), cell_complex.end());
                        }
                        
                     }
                  }
               }
            }            
            else
            {
               // prepare for next round
               reader >> token; 
            }
            if(reader.eof()) break;
         }      
         return EXIT_SUCCESS;
      }

   };

   template < typename DomainType > 
   int importINP(DomainType & domain, std::string const & filename, 
                 bool merge_domains = false, int skip_prefix = 3)
   {          
      viennagrid::io::inp_reader<DomainType> reader;
      return reader(domain, filename, merge_domains, skip_prefix);
   }  

} // end namespace io
} // end namespace viennagrid

#endif

