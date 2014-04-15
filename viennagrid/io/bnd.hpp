#ifndef VIENNAGRID_IO_BND_HPP
#define VIENNAGRID_IO_BND_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <utility>

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/predicate.hpp"
#include "boost/algorithm/string/trim.hpp"

#include "boost/lexical_cast.hpp"

// #include "viennautils/convert.hpp"
// #include "viennautils/contio.hpp"

//#define IODEBUG


/*
INFORMATION ON THE BND FILE FORMAT:

--## tested with the generated bnd files of the Synopsis Structure Editor E-2010.12 ##--

ELEMENT TYPES:
type 10: polyhedron used in 3d bnd files for the segment description
  syntax: 10 number_of_faces face-index0 face-index1 ... face-index-n

type 4: polygon used in 2d bnd files for the segment description
  syntax: 4 number_of_faces face-index0 face-index1 ... face-index-n

type 2: triangle informally also known as 'direct' or 'contact' triangle, used in 3d bnd files
  syntax: 2 edge-index0 edge-index1 edge-index2

type 1: line informally also known as 'direct' or 'contact' line, used in 2d bnd files
  syntax: 1 vertex-index0 vertex-index1

CONTACTS:
 in 3d, only faces can be contacts. although the synopsis structure editor
 allows to declare an edge to be a contact, it is not exported to the bnd file.
 a 3d-contact is defined by a type '2' element, which refers to _three_ edges,
 and they again to the points. which makes sense, because the 2d polygon,
 which can be a face in 3d, is a triangle (that's why it's called a simplex, btw ..)
 so, for example, a quardilateral contact consists therefore of two type '2' elements.

 in 2d, respectively, only edges can be contacts.
 a 2d-contact is defined by a type '1' element, which refers to two points, hence an edge
*/







namespace viennautils {

  struct print
  {
    template<typename T1, std::size_t Size>
    void operator()(boost::array<T1, Size> const& vec, std::ostream& ostr = std::cout)
    {
        for(typename boost::array<T1, Size>::const_iterator iter = vec.begin();
            iter != vec.end(); iter++)
        {
          ostr << *iter << "  ";
        }
        std::cout << std::endl;
    }

    template<typename T1, std::size_t Size>
    void operator()(boost::array<T1, Size> & vec, std::ostream& ostr = std::cout)
    {
        for(typename boost::array<T1, Size>::iterator iter = vec.begin();
            iter != vec.end(); iter++)
        {
          ostr << *iter << "  ";
        }
        std::cout << std::endl;
    }

    template<typename T1>
    void operator()(std::vector<T1> const& vec, std::ostream& ostr = std::cout)
    {
        for(typename std::vector<T1>::const_iterator iter = vec.begin();
            iter != vec.end(); iter++)
        {
          ostr << *iter << "  ";
        }
        std::cout << std::endl;
    }

    template<typename T1>
    void operator()(std::vector<T1> & vec, std::ostream& ostr = std::cout)
    {
        for(typename std::vector<T1>::iterator iter = vec.begin();
            iter != vec.end(); iter++)
        {
          ostr << *iter << "  ";
        }
        std::cout << std::endl;
    }

    template<typename T0, typename T1>
    void operator()(std::map<T0, T1> const& vec, std::ostream& ostr = std::cout)
    {
        for(typename std::map<T0, T1>::const_iterator iter = vec.begin();
            iter != vec.end(); iter++)
        {
          ostr << iter->first << " - " << iter->second << std::endl;
        }
    }

    template<typename T0, typename T1>
    void operator()(std::map<T0, T1> & vec, std::ostream& ostr = std::cout)
    {
        for(typename std::map<T0, T1>::iterator iter = vec.begin();
            iter != vec.end(); iter++)
        {
          ostr << iter->first << " - " << iter->second << std::endl;
        }
    }
  };

} // end namespace viennautils




namespace std {

  template<typename T1>
  std::ostream& operator<<(std::ostream& ostr, std::vector<T1> const& vec)
  {
    ostr << "size: " << vec.size() << " entries: "<< std::endl;
    viennautils::print()(vec, ostr);
    return ostr;
  }

  template<typename T1>
  std::ostream& operator<<(std::ostream& ostr, std::vector<T1> & vec)
  {
    ostr << "size: " << vec.size() << " entries: "<< std::endl;
    viennautils::print()(vec, ostr);
    return ostr;
  }

  template<typename T0, std::size_t S>
  std::ostream& operator<<(std::ostream& ostr, boost::array<T0, S> const& vec)
  {
    ostr << "size: " << vec.size() << " entries: "<< std::endl;
    viennautils::print()(vec, ostr);
    return ostr;
  }

  template<typename T0, std::size_t S>
  std::ostream& operator<<(std::ostream& ostr, boost::array<T0, S> & vec)
  {
    ostr << "size: " << vec.size() << " entries: "<< std::endl;
    viennautils::print()(vec, ostr);
    return ostr;
  }

  template<typename T0, typename T1>
  std::ostream& operator<<(std::ostream& ostr, std::map<T0, T1> const& map)
  {
    ostr << "size: " << map.size() << " entries: "<< std::endl;
    viennautils::print()(map, ostr);
    return ostr;
  }

  template<typename T0, typename T1>
  std::ostream& operator<<(std::ostream& ostr, std::map<T0, T1> & map)
  {
    ostr << "size: " << map.size() << " entries: " << std::endl;
    viennautils::print()(map, ostr);
    return ostr;
  }

} // end namespace std



namespace viennautils
{


  template<typename Target>
  struct convert
  {
    typedef Target result_type; // model the std::result_of concept

    template<typename Source>
    result_type operator()(Source const& source, typename boost::enable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      return source;
    }

    template<typename Source>
    result_type operator()(Source const& source, typename boost::disable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      return boost::lexical_cast<result_type>(source);
    }
  };

  template<>
  struct convert<float>
  {
    typedef float result_type; // model the std::result_of concept

    template<typename Source>
    result_type operator()(Source const& source, typename boost::enable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      return source;
    }

    template<typename Source>
    result_type operator()(Source const& source, typename boost::disable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      sstr << source;
      sstr >> target;
      return target;
    }
  private:
    result_type target;
    std::stringstream sstr;
  };

  template<>
  struct convert<double>
  {
    typedef double result_type; // model the std::result_of concept

    template<typename Source>
    result_type operator()(Source const& source, typename boost::enable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      return source;
    }

    template<typename Source>
    result_type operator()(Source const& source, typename boost::disable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      sstr << source;
      sstr >> target;
      return target;
    }
  private:
    result_type target;
    std::stringstream sstr;
  };


  template<>
  struct convert<std::string>
  {
    typedef std::string result_type; // model the std::result_of concept

    template<typename Source>
    result_type operator()(Source const& source, typename boost::enable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      return source;
    }

    template<typename Source>
    result_type operator()(Source const& source, typename boost::disable_if<boost::is_same<Source,result_type> >::type* dummy = 0)
    {
      sstr << source;
      sstr >> target;
      return target;
    }
  private:
    result_type target;
    std::stringstream sstr;
  };




  namespace io
  {
    struct bnd_reader
    {

      typedef double                            numeric_type;
      //typedef boost::array<numeric_type, DIMG>  point_type;
      typedef std::vector<numeric_type>         point_type;
      typedef std::vector<point_type>           geometry_container_type;

      typedef long                              index_type;

      typedef std::vector< index_type >         edge_type;
      typedef std::vector<edge_type>            edge_container_type;


      typedef std::vector< index_type >         polygon_type;
      typedef std::vector< polygon_type >       polygon_container_type;

      // a contact can be a line (2d) or a triangle (3d)
      typedef std::vector< index_type >         contact_face_type;
      typedef std::vector< contact_face_type >  contact_face_container_type;
      typedef std::vector< contact_face_container_type > contact_groups_type;

      // segment name, material container
      typedef std::vector<std::pair<std::string, std::string> >          segment_id_container_type;

      // contact name container
      typedef std::vector<std::string>          contact_id_container_type;

      // a domain-segment is a set of polygons
      //
      typedef std::vector<polygon_container_type>   domain_type;


      int operator()(std::string const & filename)
      {
        std::ifstream reader(filename.c_str());

        if (!reader){
          std::cerr << "Cannot open file " << filename << std::endl;
          throw "Cannot open file";
          return EXIT_FAILURE;
        }

        try{

          std::string token;

        #ifdef IODEBUG
          std::cout << "reading geometry dimension .. " << std::endl;
        #endif
          int DIMG;
          while(1)
          {
            reader >> token;
            if(token == "dimension")
            {
               reader >> token;
               reader >> DIMG;
               break;
            }
          }
        #ifdef IODEBUG
          std::cout << "geometry dimension: " << DIMG << std::endl;
        #endif
          dim_geometry = DIMG;

        #ifdef IODEBUG
          std::cout << "reading region information .. " << std::endl;
        #endif
          while(1)
          {
            reader >> token;
            if(token == "nb_regions")
            {
               reader >> token;
               reader >> number_of_regions;
               break;
            }
          }
        #ifdef IODEBUG
          std::cout << "number of regions: " << number_of_regions << std::endl;
        #endif



          std::size_t vertices = 0;

        #ifdef IODEBUG
          std::cout << "reading geometry information .. " << std::endl;
        #endif

          while(1)
          {
            std::getline(reader, token);

            if(token.find("Vertices") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");
              std::string vertices_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              vertices = viennautils::convert<std::size_t>()(vertices_str);
              break;
            }
          }

        #ifdef IODEBUG
          std::cout << "vertices: " << vertices << std::endl;
        #endif

          geometry.resize(vertices);

          point_type coords(DIMG);

          for(std::size_t vi = 0; vi < vertices; vi++)
          {
            for (int j=0; j < DIMG; j++)
            {
              reader >> coords[j];
            }
            geometry[vi] = coords;
          }

        #ifdef IODEBUG
          std::cout << "  finished loading point vectors .. " <<  std::endl;
        #endif

        #ifdef IODEBUG
          std::cout << "reading topology information .. " << std::endl;
        #endif


        #ifdef IODEBUG
          std::cout << "reading edge information .. " << std::endl;
        #endif

          std::size_t edges = 0;

          while(1)
          {
            std::getline(reader, token);

            if(token.find("Edges") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");
              std::string edges_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              edges = viennautils::convert<std::size_t>()(edges_str);
              break;
            }
          }

        #ifdef IODEBUG
          std::cout << "edges: " << edges << std::endl;
        #endif

          edge_cont.resize(edges);

          edge_type edge(2);
          for(std::size_t i = 0; i < edges; i++)
          {
            reader >> edge[0];
            reader >> edge[1];
            edge_cont[i] = edge;
          }

        #ifdef IODEBUG
          std::cout << "  finished loading edges .. " <<  std::endl;
        #endif

          if(DIMG == 3)
          {
             std::size_t faces = 0;

             while(1)
             {
               std::getline(reader, token);

               if(token.find("Faces") != std::string::npos)
               {
                 std::string::size_type delimiter_left = token.find("(")+1;
                 std::string::size_type delimiter_right = token.find(")");
                 std::string faces_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
                 faces = viennautils::convert<std::size_t>()(faces_str);
                 break;
               }
             }

           #ifdef IODEBUG
             std::cout << "faces: " << faces << std::endl;
           #endif

             polygon_cont.resize(faces);

             std::size_t face_dim = 0;

             for(std::size_t fi = 0; fi < faces; fi++)
             {
               reader >> face_dim; // number of vertices of this face

               //std::cout << "face dim: " << face_dim << std::endl;

               polygon_cont[fi].resize(face_dim);

               for (std::size_t j=0; j < face_dim; j++)
               {
                 reader >> polygon_cont[fi][j];
               }
             }

           #ifdef IODEBUG
             std::cout << "  finished loading faces .. " <<  std::endl;
           #endif
        } // end DIMG == 3

        #ifdef IODEBUG
          std::cout << "reading segment information .. " << std::endl;
        #endif

          std::size_t elements = 0;

          while(1)
          {
            std::getline(reader, token);

            if(token.find("Elements") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");
              std::string elements_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              elements = viennautils::convert<std::size_t>()(elements_str);
              break;
            }
          }

        #ifdef IODEBUG
          std::cout << "elements: " << elements << std::endl;
        #endif

          domain.resize(elements);

          for(std::size_t i = 0; i < elements; i++)
          {
            reader >> token; // element type, ie, 10 ... polyhedron

            if(token == "10") // aka a 3d bnd file segment
            {
              std::size_t element_faces = 0;
              reader >> element_faces;

            #ifdef IODEBUG
              std::cout << "  type: segment-polyhedron" << std::endl;
              std::cout << "  element: " << i << " - faces: " << element_faces << std::endl;
            #endif

              for(std::size_t f = 0; f < element_faces; f++)
              {
                index_type face;
                reader >> face;
                index_type mapped_face = map(face);

                polygon_type polygon = polygon_cont[mapped_face];

                polygon_type polygon_new;
                std::map<index_type, bool>  uniquer;

                for(std::size_t vi = 0; vi < polygon.size(); vi++)
                {
                  index_type vertex = polygon[vi];
                  index_type mapped_vertex = map(vertex);

                  edge_type edge = edge_cont[mapped_vertex];

                  if(vertex < 0) std::reverse(edge.begin(),edge.end());

                  if(!uniquer[edge[0]])
                  {
                    polygon_new.push_back(edge[0]);
                    uniquer[edge[0]] = true;
                  }
                  if(!uniquer[edge[1]])
                  {
                    polygon_new.push_back(edge[1]);
                    uniquer[edge[1]] = true;
                  }
                }
                //std::cout << "adapt: " << polygon_new;
                domain[i].push_back(polygon_new);
              }
            }
            else if(token == "4")  // aka a 2d bnd file segment
            {
              std::size_t element_faces = 0;
              reader >> element_faces;

            #ifdef IODEBUG
              std::cout << "  type: segment-polygon" << std::endl;
              std::cout << "  element: " << i << " - linesegments: " << element_faces << std::endl;
            #endif

              for(std::size_t f = 0; f < element_faces; f++)
              {
                index_type face;
                reader >> face;
                index_type mapped_face = map(face);

                edge_type edge = edge_cont[mapped_face];

                //std::cout << "adapt: " << polygon_new;
                //std::cout << "adding edge: " << edge[0] << " " << edge[1] << std::endl;
                domain[i].push_back(edge);
              }

            }
            else if(token == "2") // aka a 3d bnd file contact
            {
//               reader >> token;
//               reader >> token;
//               reader >> token;
//               domain.resize(domain.size()-1);
            #ifdef IODEBUG
              std::cout << "  type: contact-triangle" << std::endl;
            #endif
              contact_face_type contact_face(3);
              reader >> contact_face[0];
              reader >> contact_face[1];
              reader >> contact_face[2];
              //std::cout << contact_face[0] << " " << contact_face[1] << " " << contact_face[2] << std::endl;
              contact_face_cont.push_back(contact_face);

              // we have to reduce the domain size, as this is not a valid segment
              domain.resize(domain.size()-1);
            }
            else if(token == "1") // aka a 2d bnd file contact
            {
//               reader >> token;
//               reader >> token;
//               domain.resize(domain.size()-1);
            #ifdef IODEBUG
              std::cout << "  type: contact-line" << std::endl;
            #endif
              contact_face_type contact_face(2);
              reader >> contact_face[0];
              reader >> contact_face[1];
              //std::cout << contact_face[0] << " " << contact_face[1] << std::endl;
              contact_face_cont.push_back(contact_face);

              // we have to reduce the domain size, as this is not a valid segment
              domain.resize(domain.size()-1);
            }
            else
            {
              std::cerr << "BND-READER-Error: Elements of type: " << token << " are not supported .." << std::endl;
              exit(-1);
            }
          }

        #ifdef IODEBUG
          std::cout << "  finished loading elements .. " <<  std::endl;
        #endif

        #ifdef IODEBUG
          std::cout << "reading material and contact information .. " << std::endl;
        #endif
          // the domainsize acts as an offset for the number of elements
          std::size_t domainsize = domain.size();
          segment_id_cont.resize(domainsize);
          std::size_t region_cnt = 0;

          contact_groups.resize(number_of_regions-domainsize);

          while(1)
          {
            std::getline(reader, token);

            if(token.find("Region") != std::string::npos)
            {

              region_cnt++;

              std::string::size_type delimiter_left = token.find("(\"")+2;
              std::string::size_type delimiter_right = token.find("\")");
              std::string name = token.substr(delimiter_left, delimiter_right-delimiter_left);
              boost::trim(name);

              while(1)
              {
                std::getline(reader, token);

                if(token.find("material") != std::string::npos)
                {
                  std::string::size_type delimiter_left = token.find("=")+1;
                  std::string::size_type delimiter_right = token.find("\n");
                  std::string material = token.substr(delimiter_left, delimiter_right-delimiter_left);
                  boost::trim(material);

                  //std::cout << "name: -" << name << "- material: -" << material << "-" << std::endl;

                 std::size_t elements = 0;
                  while(1)
                  {
                    std::getline(reader, token);

                    if(token.find("Elements") != std::string::npos)
                    {
                      std::string::size_type delimiter_left = token.find("(")+1;
                      std::string::size_type delimiter_right = token.find(")");
                      std::string elements_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
                      elements = viennautils::convert<std::size_t>()(elements_str);
                      break;
                    }
                  }
                  std::size_t ei;
                  if( material == "Contact" )
                  {
                    // store the contact name
                    contact_id_cont.push_back(name);

                    for(std::size_t i = 0; i < elements; i++)
                    {
                      // read the elment id
                      reader >> ei;

                      // determine the contact cells
                      if(DIMG == 2)
                      {
                        contact_groups[contact_id_cont.size()-1].push_back(contact_face_cont[ei-domainsize]);
                      }
                      else // 3D
                      {
                        contact_face_type edge_index_cont = contact_face_cont[ei-domainsize];

                        std::map<index_type, bool>  uniquer;
                        contact_face_type contact_triangle;

                        for(std::size_t ei = 0; ei < edge_index_cont.size(); ei++)
                        {
                          index_type edge_index = edge_index_cont[ei];
                          index_type mapped_edge_index = map(edge_index);
                          edge_type edge = edge_cont[mapped_edge_index];
                          if(edge_index < 0) std::reverse(edge.begin(),edge.end());

                          if(!uniquer[edge[0]])
                          {
                            contact_triangle.push_back(edge[0]);
                            uniquer[edge[0]] = true;
                          }
                          if(!uniquer[edge[1]])
                          {
                            contact_triangle.push_back(edge[1]);
                            uniquer[edge[1]] = true;
                          }
                        }
                        //std::cout << contact_triangle << std::endl;
                        contact_groups[contact_id_cont.size()-1].push_back(contact_triangle);
                      }
                    }
                  }
                  else
                  {
                    for(std::size_t i = 0; i < elements; i++)
                    {
                      // read the elment id
                      reader >> ei;

                      // store the name and material info on the related segment id
                      segment_id_cont[ei] = std::pair<std::string, std::string>(name, material);
                    }
                  }
                  break;
                }
              }
            }
            if(region_cnt == number_of_regions) break;
          }

        } catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }

        return EXIT_SUCCESS;
      } //operator()

      struct negate
      {
        template<typename T>
        void operator()(T& x) { x*=-1; }
      };

      void dump(std::ostream& ostr = std::cout)
      {
        ostr << "## GEOMETRY" << std::endl;
        for(std::size_t gi = 0; gi < geometry.size(); gi++)
        {
          ostr << "  point - id: " << gi << " - data: ";
          viennautils::print()(geometry[gi], ostr);
        }

        ostr << "## TOPOLOGY" << std::endl;
        for(std::size_t si = 0; si < domain.size(); si++)
        {
          ostr << "  segment: " << si << std::endl;
          for(std::size_t fi = 0; fi < domain[si].size(); fi++)
          {
            ostr << "    face - id: " << fi << " - size: " << domain[si][fi].size() << " - eles: ";
            viennautils::print()(domain[si][fi], ostr);
          }
        }
      }

      point_type& point(std::size_t index)
      {
         return geometry[index];
      }

      std::size_t geometry_size()
      {
         return geometry.size();
      }

      std::size_t segment_size()
      {
         return domain.size();
      }

      polygon_container_type& segment(std::size_t index)
      {
         return domain[index];
      }

      std::size_t dim_geom()
      {
         return dim_geometry;
      }

      std::size_t contact_size()
      {
        return contact_id_cont.size();
      }

      std::string contact_name(std::size_t ci)
      {
        return contact_id_cont[ci];
      }

      contact_face_container_type contact_faces(std::size_t ci)
      {
        return contact_groups[ci];
      }

      std::string segment_name(std::size_t ci)
      {
        return segment_id_cont[ci].first;
      }

      std::string segment_material(std::size_t ci)
      {
        return segment_id_cont[ci].second;
      }


      void clear()
      {
         geometry.clear();
         edge_cont.clear();
         polygon_cont.clear();
         contact_face_cont.clear();
         contact_groups.clear();
         domain.clear();
         segment_id_cont.clear();
         contact_id_cont.clear();
      }

    private:
      index_type map(index_type index)
      {
        if(index >= 0) return index;
        else           return -(index)-1;  // this is the specific mapping for negativ face and element values
      }

      geometry_container_type       geometry;
      edge_container_type           edge_cont;
      polygon_container_type        polygon_cont;
      contact_face_container_type   contact_face_cont;
      contact_groups_type           contact_groups;
      domain_type                   domain;
      segment_id_container_type     segment_id_cont;
      contact_id_container_type     contact_id_cont;
      std::size_t                   dim_geometry;
      std::size_t                   number_of_regions;

    }; //class bnd_reader


  } //namespace io
} //namespace viennagrid

#endif
