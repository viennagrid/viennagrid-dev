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

#ifndef VIENNAGRID_IO_TDR_READER_GUARD
#define VIENNAGRID_IO_TDR_READER_GUARD


// NOT INCLUDED IN RELEASE !!


// system includes
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

// HDF includes
#include "H5Cpp.h"

//
// [JW][TIP] for debugging purposes use the h5dump utility to dump a hdf5 file
//

#define VIENNAGRID_DEBUG_IO

namespace viennagrid
{
  namespace io
  {

    struct tdr_reader
    {
      tdr_reader(std::string const& filename) 
      {
          using namespace H5;

          segment_counter = 0;
          contact_counter = 0;
          
          H5File file( filename, H5F_ACC_RDONLY );
          
          // read tdr file and store in the the tdr_readers 
          // internal datastructure
          //
          read_collection(file.openGroup("collection"));
          
      }

      // transfer data from tdr_reader datastructure
      // to a viennagrid::domain
      //
      template <typename DomainT>
      int operator()(DomainT & domain) const
      {
      #ifdef VIENNAGRID_DEBUG_IO
          std::cout << "transferring to viennagrid domain .. " << std::endl;
      #endif
          typedef typename DomainT::config_type                                            domain_config_type;
          typedef typename domain_config_type::cell_tag                                    cell_tag;      
          typedef typename viennagrid::result_of::ncell<domain_config_type, 0>::type  vertex_type; 
          typedef typename viennagrid::result_of::ncell<domain_config_type, cell_tag::topology_level>::type     cell_type;            
          static const std::size_t CELLSIZE = viennagrid::topology::subcell_desc<cell_tag, 0>::num_elements;         
      
          // transfer geometry
          //    note: point vectors are linearly stored, x1 y1 z1 x2 y2 z2 ...
          //   
          vertex_type  vertex;
          std::size_t  index;
      #ifdef VIENNAGRID_DEBUG_IO      
          std::cout << "  processing geometry .."<< std::endl;
      #endif      
          for (std::size_t i=0; i<geometry_space.size(); i++)
          {
            // 3 is the offset due to three-dimensional geometry space
            index = i * 3; 
            vertex.getPoint()[0] = geometry_space[index];
            vertex.getPoint()[1] = geometry_space[index+1];
            vertex.getPoint()[2] = geometry_space[index+2];
            domain.add(vertex);
          }
          
          // transfer topology
          // 
      #ifdef VIENNAGRID_DEBUG_IO      
          std::cout << "  processing topology .. " << std::endl;
          std::cout << "  segments: " << segment_counter << std::endl;
          std::cout << "  contacts: " << contact_counter << std::endl;
      #endif
          domain.create_segments(segment_counter); 
          std::size_t seg_cnt = 0;
          for (std::map<std::string,region_t>::const_iterator rit = region.begin();  
              rit != region.end(); rit++)
          {   
      #ifdef VIENNAGRID_DEBUG_IO
            std::cout << "    region-name:     " << rit->second.name << 
                        "  material: " << rit->second.material << std::endl;    
      #endif      
            // special treatment for a contact region
            //
            if(rit->second.material == "Contact")
            {
            }
            // the general case of a segment/material region
            //
            else
            {
              vertex_type *vertices[CELLSIZE];
              
              std::vector<std::vector<std::size_t> > const& elements = rit->second.elements;
              for (std::vector<std::vector<std::size_t> >::const_iterator cit = elements.begin(); 
                  cit != elements.end(); cit++)
              {
                std::vector<std::size_t> const& tempcell = *cit;
                
                  vertices[0] = &(domain.vertex(tempcell[0]));
                  vertices[1] = &(domain.vertex(tempcell[1])); 
                  vertices[2] = &(domain.vertex(tempcell[2])); 
                  vertices[3] = &(domain.vertex(tempcell[3]));                                               
                
                  cell_type cell;
                  cell.setVertices(vertices);             
                  domain.segment(seg_cnt).add(cell);
              }
              
              seg_cnt++;
            }
      
          }   
      //        case 5:  for (int i=0; i<4; i++)
      //        region.elements.back().push_back(el[elct++]);
      
          return 0;
      }
      
      
    private:

      struct dataset_t 
      {
        std::string          name, quantity, unit;
        std::size_t          nvalues;
        double               conversion_factor;
        std::vector<double>  values;
      };  
      
      struct region_t 
      {
        std::size_t                      regnr,nelements,npointidx;
        std::string                      name,material;
        std::vector<std::vector<std::size_t> >   elements;
        std::map<std::string,dataset_t>  dataset;
      };

      typedef struct coord2_t 
      {
          double x[3];
      } coord2_t;

      void read_collection(H5::Group const& collection)
      {
          int i;
          i = read_int(collection,"number of geometries");
          if(i != 1)
        {
          std::cerr << "more than one geometries" << std::endl;
          throw;
        }
        i = read_int(collection, "number of plots");
        if(i != 0)
        {
          std::cerr << "plots available" << std::endl;
        }
        read_geometry(collection.openGroup("geometry_0"));
      }
      
      int read_int(H5::H5Object const& g, std::string const& name)
      {
        int i;
        H5::Attribute a=g.openAttribute(name);
        if (a.getTypeClass()!=H5T_INTEGER)
        {
           std::cerr << "wrong class in atrribute" << std::endl;
          throw;
        }
        a.read(a.getDataType(),&i);
        return i;
      }   
      
      double read_double(const H5::H5Object &g, const std::string name)
      {
         double i;
         H5::Attribute a=g.openAttribute(name);
         //if (a.getTypeClass()!=H5T_DOUBLE)
         if (a.getTypeClass()!=H5T_FLOAT)
         {
            std::cerr << "wrong class in atrribute" << std::endl;
            throw;
         }         
         a.read(a.getDataType(),&i);
         return i;
      }      
      
      std::string read_string(H5::H5Object const& g, std::string const& name)
      {
        H5::Attribute a=g.openAttribute(name);
        if (a.getTypeClass()!=H5T_STRING)
        {
           std::cerr << "wrong class in atrribute" << std::endl;
          throw;
        }
          
        // [JW] switched to dynamic array
        //char buf[a.getDataType().getSize()+1];
        char *buf;
        buf = (char *)malloc( (a.getDataType().getSize()+1) * sizeof(char) );
        a.read(a.getDataType(),buf);
        buf[a.getDataType().getSize()]='\0';
        std::string bufstr(buf);
        free(buf);
        return bufstr;
      }   
      
      void read_geometry(H5::Group const& geometry)
      {
        int type;
        char name[100];
        type = read_int(geometry,"type");
          if(type != 1)
          {
          std::cerr << "geometry type not supported" << std::endl;
          throw;      
          }

        dim=read_int(geometry,"dimension");
      #ifdef VIENNAGRID_DEBUG_IO
        std::cout << "Dimension: " << dim << std::endl;
      #endif

        nvertices=read_int(geometry,"number of vertices");
      #ifdef VIENNAGRID_DEBUG_IO
        std::cerr << "nvertices: " << nvertices << std::endl;
      #endif
        nregions=read_int(geometry,"number of regions");
        //std::cerr << "nregions: " << nregions << std::endl;

        nstates=read_int(geometry,"number of states");
          if(nstates != 1)
          {
          std::cerr << "number of states is not 1" << std::endl;
          throw;      
          }
          
        for (std::size_t i=0; i<nregions; i++)
        {
          sprintf(name,"region_%ld",i);
          const H5::Group &reg = geometry.openGroup(name);
          read_region(i,reg);
        }

        const H5::Group &trans=geometry.openGroup("transformation");
        read_transformation(trans);
        const H5::DataSet &vert=geometry.openDataSet("vertex");
        read_vertex(vert);
        
        read_attribs0(geometry.openGroup("state_0"));
      }
      
      void read_attribs0(const H5::Group &state)
      {
         int i;
         ndatasets=read_int(state,"number of datasets");
      #ifdef VIENNAGRID_DEBUG_IO         
         std::cout << "ndatasets: " << ndatasets << std::endl;
      #endif
         i=read_int(state,"number of plots");
         if (i!=0)
         {
            std::cerr << "number of plots not equal 0" << std::endl;
            throw;      
         }         
         i=read_int(state,"number of string streams");
         if (i!=0)
         {
            std::cerr << "number of string-streams not equal 0" << std::endl;
            throw;      
         }         
         i=read_int(state,"number of xy plots");
         if (i!=0)
         {
            std::cerr << "number of xy-plots not equal 0" << std::endl;
            throw;      
         }         

         for (std::size_t i=0; i<ndatasets; i++)
         {
            char a[100];
            sprintf(a,"dataset_%ld",i);
         #ifdef VIENNAGRID_DEBUG_IO
            std::cout << "Attribute: " << a << std::endl;
         #endif
            read_dataset(state.openGroup(a));
         }
      }      
      
      void read_dataset(const H5::Group &dataset)
      {
         std::string name = read_string(dataset,"name");
         if (name.find("Stress")!=name.npos)
            return;

         std::string quantity = read_string(dataset,"quantity");
         int regnr   = read_int(dataset,"region");
         int nvalues = read_int(dataset,"number of values");
         double conversion_factor = read_double(dataset,"conversion factor");
         if (read_int(dataset,"location type")!=0)
         {
            std::cerr << "Dataset " << name << " location type not 0" << std::endl;
            throw;      
         }                  
         if (read_int(dataset,"structure type") != 0)
         {
            std::cerr << "Dataset " << name << " structure type not 0" << std::endl;
            throw;      
         }                  
         if (read_int(dataset,"value type") != 2)
         {
            std::cerr << "Dataset " << name << " value type not 2" << std::endl;
            throw;      
         }                  
      #ifdef VIENNAGRID_DEBUG_IO
         std::cout << "Dataset: " << name << " " << nvalues << std::endl;
      #endif
         // In this dataset we have a group 
         // tag_group_0???
         // units: take only the unit name
         // Dataset values: the actual values

         std::string unit;
         int n=dataset.getNumObjs(),i;
         for (i=0; i<n; i++)   
         {
         #ifdef VIENNAGRID_DEBUG_IO
            std::cout << dataset.getObjnameByIdx(i) << std::endl;
         #endif
            if (dataset.getObjnameByIdx(i)=="unit")
            {
               const H5::Group &u=dataset.openGroup("unit");
               unit=read_string(u,"name");
               break;
            }
         }
         if (i==n)
         {
            unit=read_string(dataset,"unit:name");
         }

         region_t &region=find_region(regnr);
         region.dataset[name].name=name;
         region.dataset[name].quantity=quantity;
         region.dataset[name].nvalues=nvalues;
         region.dataset[name].conversion_factor=conversion_factor;
         region.dataset[name].unit=unit;

         read_values(region.dataset[name],dataset.openDataSet("values"));
      }      
      
      region_t &find_region(std::size_t regnr)
      {
         for (std::map<std::string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
            if (R->second.regnr==regnr)
               return R->second;
               
         std::cerr << "region " << regnr << " not found" << std::endl;
         throw;      
      }      
      
      void read_values(dataset_t &dataset,const H5::DataSet &values)
      {
         const H5::DataSpace &dataspace = values.getSpace();
         int rank = dataspace.getSimpleExtentNdims();
         hsize_t dims[10];
         int ndims = dataspace.getSimpleExtentDims( dims, NULL);
      #ifdef VIENNAGRID_DEBUG_IO         
         std::cout << "dataspace: " << rank << " " << ndims << " " << dims[0] << "," << dims[1] << std::endl;
         std::cout << "nvalues: " << dataset.nvalues << std::endl;
      #endif
         if (dataset.nvalues != dims[0] || ndims!=1)
         {
            std::cerr << "Dataset " << dataset.name << " should have " << dataset.nvalues 
                      << " values, but has " << dims[0] << " with dimension " << ndims << std::endl;
            throw;               
         }

         // [JW] switched to dynamic memory alloc
         //double v[dims[0]];
         double *v;
         v = (double *)malloc( dims[0] * sizeof(double) );
         values.read( v, H5::PredType::NATIVE_DOUBLE);
         dataset.values.insert(dataset.values.end(),&v[0],&v[dims[0]]);
         free(v);
      }      
      
      void read_transformation(const H5::Group &trans)
      {
        const H5::DataSet &A=trans.openDataSet("A");
        const H5::DataSet &b=trans.openDataSet("b");
        A.read( trans_matrix, H5::PredType::NATIVE_DOUBLE);
        b.read( trans_move, H5::PredType::NATIVE_DOUBLE);
      }  
      
      void read_vertex(const H5::DataSet &vert)
      {
        const H5::DataSpace &dataspace = vert.getSpace();
        int rank = dataspace.getSimpleExtentNdims();
        hsize_t dims[2];
        int ndims = dataspace.getSimpleExtentDims( dims, NULL);
      #ifdef VIENNAGRID_DEBUG_IO
        std::cout << "dataspace: " << rank << " " << ndims << " " << dims[0] << "," << dims[1] << std::endl;
        std::cout << "nvertices: " << nvertices << std::endl;
      #endif
        if (nvertices!=dims[0])
          {
          std::cerr << "nvertices not equal vertices.dim" << std::endl;
          throw;      
          }
              
        H5::CompType mtype2( sizeof(coord2_t) );
        const H5std_string MEMBER1( "x" );
        const H5std_string MEMBER2( "y" );
        const H5std_string MEMBER3( "z" );
          mtype2.insertMember( "x", HOFFSET(coord2_t, x[0]), H5::PredType::NATIVE_DOUBLE);
        if (dim>1)
          mtype2.insertMember( "y", HOFFSET(coord2_t, x[1]), H5::PredType::NATIVE_DOUBLE);
        if (dim>2)
          mtype2.insertMember( "z", HOFFSET(coord2_t, x[2]), H5::PredType::NATIVE_DOUBLE);
          
         // [JC] Read two fields x and y from s2 dataset. Fields in the file
         // are found by their names "x_name" and "y_name".
         //
         //coord2_t s2[dims[0]];   // [JW] switched to dynamic memory alloc
         coord2_t *s2;
         s2 = (coord2_t *)malloc( dims[0] * sizeof(coord2_t) );
         vert.read( s2, mtype2 );

        for (std::size_t i=0; i<dims[0]; i++)
        {
          geometry_space.push_back(s2[i].x[0]);
          if (dim>1)
            geometry_space.push_back(s2[i].x[1]);
          if (dim>2)
            geometry_space.push_back(s2[i].x[2]);
            
            // [JW] dump the geometry information
            //std::cout << s2[i].x[0] << " " << s2[i].x[1] << " " << s2[i].x[2] << std::endl;
        }
        free(s2);
      }  
      
      void read_elements(region_t &region, const H5::DataSet &elem)
      { 
      #ifdef VIENNAGRID_DEBUG_IO    
        std::cout << "nelements: " << region.nelements << std::endl;
      #endif
        const H5::DataSpace &dataspace = elem.getSpace();
        int rank = dataspace.getSimpleExtentNdims();
        hsize_t dims[2];
        int ndims = dataspace.getSimpleExtentDims( dims, NULL);

      #ifdef VIENNAGRID_DEBUG_IO
        std::cout << "dataspace: " << rank << " " << ndims << " " << dims[0] << "," << dims[1] << std::endl;
      #endif
        if (rank!=1)
          {
          std::cerr << "rank of elements in region " << region.name << " is not one" << std::endl;
          throw;      
          }
        if (ndims!=1)
          {
          std::cerr << "ndims of elements in region " << region.name << " is not one" << std::endl;
          throw;      
          }

         // [JW] switched to dynamic memory alloc
        //int el[dims[0]];
        int *el;
        el = (int *)malloc( dims[0] * sizeof(int) );
        elem.read( el, H5::PredType::NATIVE_INT);

        std::size_t elct=0;
        while (elct<dims[0])
        {
          region.elements.push_back(std::vector<std::size_t>());
          switch (el[elct++])
          {
            case 1:  for (int i=0; i<2; i++)
                region.elements.back().push_back(el[elct++]);
              break;
            case 2:  for (int i=0; i<3; i++)
                region.elements.back().push_back(el[elct++]);
              break;
            case 5:  for (int i=0; i<4; i++)
                region.elements.back().push_back(el[elct++]);
              break;
            default:
                std::cerr << "Element type " << el[elct-1] << " in region " << region.name << " not known" << std::endl;
                throw;      
          }
        }
        free(el);
      }  
      
      void read_region(const int regnr, const H5::Group &reg)
      {
        std::string name0 = read_string(reg,"name");
        std::string name;
        std::size_t i;
        while ((i=name0.find_first_of("_."))!=name0.npos)
        {
          name=name+name0.substr(0,i);
          name0=name0.substr(i+1);
        }
        name=name+name0;
        std::string material;
        const int typ=read_int(reg,"type");
        switch (typ)
        {
          case 0: // we have a nD region
            material=read_string(reg,"material");
            segment_counter++;
          #ifdef VIENNAGRID_DEBUG_IO
            std::cout << "found material: " << material << std::endl;
          #endif
            break;
          case 1: // we have a nD-1 region (contact... drain, source, ...)
          #ifdef VIENNAGRID_DEBUG_IO
            std::cout << "found contact .. " << std::endl;
          #endif
            contact_counter++;
            material="Contact";
            // attribute "part 0", no idea for what
            break;
          default : 
            std::cerr << "Unknown type " << typ << " in region " << read_string(reg,"name") << std::endl;
            throw;
        }
        const int nparts=read_int(reg,"number of parts");
        if (nparts!=1)
          {
          std::cerr << "number of parts is not 1" << std::endl;
          throw;      
          }

        region[name].regnr=regnr;
        region[name].name=name;
        region[name].material=material;
        const int n=reg.getNumObjs();
        for (int i=0; i<n; i++)
        {
          if (reg.getObjnameByIdx(i)=="elements_0")
          {
            const H5::DataSet &ds=reg.openDataSet("elements_0");
            region[name].nelements=read_int(ds,"number of elements");
            read_elements(region[name],ds);
            return;
          }
          if (reg.getObjnameByIdx(i)=="part_0")
          {
            const H5::Group &part=reg.openGroup("part_0");
            region[name].nelements=read_int(part,"number of elements");
            read_elements(region[name],part.openDataSet("elements"));
            return;
          }
        }
        std::cerr << "no elements found on region" << std::endl;
        throw;          
      }
      
      int dim;
      std::size_t nvertices,nregions,ndatasets,nstates, segment_counter, contact_counter;
      double trans_matrix[9],trans_move[3];
      std::vector<double> geometry_space;  
      std::map<std::string,region_t>   region;
    };

    template < typename DomainType > 
    int importTDR(DomainType & domain, std::string const & filename)
    {          
      viennagrid::io::tdr_reader reader(filename);
      return reader(domain);
    }     

  } // end namespace io
} // end namespace viennagrid

#endif



