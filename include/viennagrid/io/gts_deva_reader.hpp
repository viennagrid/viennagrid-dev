#ifndef VIENNAGRID_IO_GTS_DEVA_READER_HPP
#define VIENNAGRID_IO_GTS_DEVA_READER_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "boost/algorithm/string.hpp"

namespace viennagrid
{
  namespace io
  {
    namespace gts_deva
    {
      template<typename stream_type>
      bool next_valid_line( stream_type & stream, std::string & line, std::string const & comment_line )
      {
        std::string tmp;

        while (true)
        {
          if (!std::getline(stream, tmp))
            return false;

          std::size_t pos = tmp.find(comment_line);
          if (pos != std::string::npos)
            tmp = tmp.substr(0, pos);

          for (std::size_t i = 0; i != tmp.size(); ++i)
          {
            if ( tmp[i] != ' ' )
            {
              line = tmp.substr(i, std::string::npos);
              return true;
            }
          }
        }
      }

      struct token
      {
        token() : parent(NULL) {}

        ~token()
        {
          for (unsigned int i = 0; i < children.size(); ++i)
            delete children[i];
        }

        void print(int depth = 0)
        {
          for (int i = 0; i < depth; ++i)
            std::cout << "  ";
          std::cout << name << std::endl;

          for (unsigned int i = 0; i < children.size(); ++i)
            children[i]->print(depth+1);
        }

        typedef std::vector< token * >::iterator iterator;
        typedef std::vector< token * >::const_iterator const_iterator;

        iterator begin() { return children.begin(); }
        iterator end() { return children.end(); }

        const_iterator begin() const { return children.begin(); }
        const_iterator end() const { return children.end(); }

        token * find( std::string const & name )
        {
          for (iterator it = begin(); it != end(); ++it)
            if ( (*it)->name.find( name ) != std::string::npos )
              return *it;

          return NULL;
        }

        token const * find( std::string const & name ) const
        {
          for (const_iterator it = begin(); it != end(); ++it)
            if ( (*it)->name.find( name ) != std::string::npos )
              return *it;

          return NULL;
        }

        std::string attribute(unsigned int position) const
        {
          std::size_t pos = name.find('(');
          if (pos == std::string::npos)
            return "";

          std::string attrib_string = name.substr( pos+1, name.rfind(')')-pos );

          std::list<std::string> attribs;
          boost::algorithm::split( attribs, attrib_string, boost::is_any_of(",") );
//           = stringtools::split_string( attrib_string, "," );

          if (position > attribs.size())
            return "";

          std::list<std::string>::iterator it = attribs.begin();
          std::advance( it, position );

          pos = (*it).find_first_not_of(" \"");

          if (pos != std::string::npos)
            return (*it).substr( pos, (*it).find_last_not_of(" \"") );
          else
            return (*it).substr( 0, (*it).find_last_not_of(" \"") );
        }


        std::string name;
        std::string text;

        token * parent;
        std::vector< token * > children;
      };



      template<typename stream_type>
      token * parse( stream_type & stream )
      {
        token * root = new token();
        token * current = root;

        std::string tmp;
        while ( next_valid_line(stream, tmp, "//") ) //, "/*", "*/") )
        {
          std::size_t start = 0;
          std::size_t pos = tmp.find_first_of("{}");
          while (pos != std::string::npos)
          {
            if (tmp[pos] == '{')
            {
              current->text += tmp.substr(start, pos-start);

              token * tmp = new token();

              std::size_t newline_pos = current->text.find_last_not_of(" \n");
              if (newline_pos != std::string::npos)
                newline_pos = current->text.find_last_of("\n", newline_pos);

              if (newline_pos != std::string::npos)
                tmp->name = current->text.substr(newline_pos+1);
              else
                tmp->name = current->text;

              newline_pos = tmp->name.find_last_not_of(" \n");
              if (newline_pos != std::string::npos)
                tmp->name.erase(newline_pos+1);


              current->children.push_back(tmp);
              tmp->parent = current;
              current = tmp;
            }

            if (tmp[pos] == '}')
            {
              current->text += tmp.substr(start, pos-start);

              current = current->parent;
            }


            start = pos+1;
            pos = tmp.find_first_of("{}", start);
          }

          current->text += tmp.substr(start) + "\n";
        }

        return root;
      }

      namespace result_of
      {
        template<typename CellTagT>
        struct gts_cell_type;

        template<>
        struct gts_cell_type<viennagrid::polygon_tag>
        {
          static const int value = 9;
        };
      }

    }


    using viennagrid::io::cannot_open_file_exception;
    using viennagrid::io::bad_file_format_exception;


    struct gts_deva_reader
    {
      /** @brief The functor interface triggering the read operation. Segmentations are not supported in this version.
       *
       * @param mesh_obj      The mesh where the file content is written to
       * @param filename      Name of the file
       */

      template <typename MeshT>
      int operator()(MeshT const & mesh, std::string const & filename, bool load_geometry = false) const
      {
        typedef typename viennagrid::result_of::point<MeshT>::type           PointType;
        typedef typename viennagrid::result_of::element<MeshT>::type         ElementType;

        std::ifstream reader(filename.c_str());

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* poly_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception(filename);
          return EXIT_FAILURE;
        }

        if (!reader.good())
          throw bad_file_format_exception("gts_deva_geometry_reader::operator(): File \"" + filename + "\" is empty");


        gts_deva::token * root = gts_deva::parse(reader);

        gts_deva::token * inner_root = root->children[0];
//         inner_root->print();

        gts_deva::token * segments = inner_root->find( "Segments" );
        gts_deva::token * pointlists = inner_root->find( "Pointlists" );
        gts_deva::token * facelists = inner_root->find( "Facelists" );

        std::map<std::string, std::vector<ElementType> > vertex_map;
        std::map<std::string, std::vector< std::vector<ElementType> > > line_map;

        for (gts_deva::token::iterator sik = segments->begin(); sik != segments->end(); ++sik)
        {
          std::string name = (*sik)->name.substr( (*sik)->name.find('\"')+1 );
          name.erase( name.find('\"') );

          bool region_is_geometry = (name.find("_simple") != std::string::npos);

          if (load_geometry == region_is_geometry)
          {
//             std::cout << "Using Region " << name << std::endl;

            std::size_t pos = (*sik)->text.find("Ref(Facelist,");
            pos = (*sik)->text.find('"', pos);
            std::string facelist_name = (*sik)->text.substr( pos, (*sik)->text.find('"', pos+1)-pos+1 );

//             std::cout << "Using facelist with name " << facelist_name << std::endl;


            typename std::map<std::string, std::vector< std::vector<ElementType> > >::iterator flit = line_map.find( facelist_name );
            if (flit == line_map.end())
            {
              gts_deva::token * current_facelist = facelists->find( facelist_name );

              pos = current_facelist->text.find("Ref(Pointlist,");
              pos = current_facelist->text.find('"', pos);
              std::string pointlist_name = current_facelist->text.substr( pos, current_facelist->text.find('"', pos+1)-pos+1 );

//               std::cout << "Using pointlist with name " << pointlist_name << std::endl;



              typename std::map<std::string, std::vector<ElementType> >::iterator plit = vertex_map.find( pointlist_name );
              if (plit == vertex_map.end())
              {
                gts_deva::token * current_pointlist = pointlists->find( pointlist_name );

                int dimension = atoi( current_pointlist->attribute(2).c_str() );
                int num_points = atoi( current_pointlist->attribute(3).c_str() );

//                 std::cout << "DIM = " << dimension << std::endl;
//                 std::cout << "#points = " << num_points << std::endl;

                std::vector<ElementType> & vertices = vertex_map[ pointlist_name ];
                vertices.resize(num_points);

                gts_deva::token * intern = current_pointlist->find("Intern");

                std::list<std::string> values;
                boost::algorithm::split( values, intern->text, boost::is_any_of(",") );

                if (static_cast<int>(values.size()) != num_points*dimension)
                {
                  std::cout << "ERROR!! Point size missmatch!!" << std::endl;
                }

                std::list<std::string>::iterator vit = values.begin();
                for (int i = 0; i < num_points; ++i)
                {
                  PointType point(dimension);
                  point[0] = atof( (*vit++).c_str() );
                  point[1] = atof( (*vit++).c_str() );

                  vertices[i] = viennagrid::make_vertex( mesh, point );
                }
              }

              std::vector<ElementType> & vertices = vertex_map[ pointlist_name ];
              std::vector<std::vector<ElementType> > & faces = line_map[ facelist_name ];

              int num_faces = atoi( current_facelist->attribute(1).c_str() );
//               std::cout << "#faces = " << num_faces << std::endl;

              faces.resize(num_faces);

              gts_deva::token * intern = current_facelist->find("Intern");
              std::list<std::string> values;
              boost::algorithm::split( values, intern->text, boost::is_any_of(",") );
              std::list<std::string>::iterator vit = values.begin();

              for (int i = 0; i < num_faces; ++i)
              {
                int face_type = atoi( (*vit++).c_str() );

                switch (face_type)
                {
                  case 2:
                  {
                    int i0 = atoi( (*vit++).c_str() );
                    int i1 = atoi( (*vit++).c_str() );
                    int i2 = atoi( (*vit++).c_str() );

//                     std::cout << "Added triangle " << i0 << " " << i1 << " " << i2 << std::endl;

                    faces[i].resize(1);
                    faces[i][0] = viennagrid::make_triangle( mesh, vertices[i0], vertices[i1], vertices[i2] );
                    break;
                  }

                  case 9:
                  {
                    int num_indices = atoi( (*vit++).c_str() );

                    std::vector<int> indices(num_indices);

                    for (int j = 0; j < num_indices; ++j)
                      indices[j] = atoi( (*vit++).c_str() );

                    faces[i].resize(num_indices);

                    for (int j = 0; j < num_indices-1; ++j)
                      faces[i][j] = viennagrid::make_line( mesh, vertices[indices[j]], vertices[indices[j+1]] );

                    faces[i][num_indices-1] = viennagrid::make_line( mesh, vertices[indices.back()], vertices[indices.front()] );
                    break;
                  }

                  default:
                    assert(false);
                }
              }
            }

            std::vector<std::vector<ElementType> > & faces = line_map[ facelist_name ];

            gts_deva::token * mesh_element_list = (*sik)->find("MeshElements");
//             int num_elements = atoi( mesh_element_list->attribute(0).c_str() );

            gts_deva::token * intern = mesh_element_list->find("Intern");
            std::list<std::string> values;
            boost::algorithm::split( values, intern->text, boost::is_any_of(",") );
//             = stringtools::split_string( intern->text, "," );

            typedef typename viennagrid::result_of::region<MeshT>::type RegionType;
            RegionType current_region = mesh.create_region();

            for (std::list<std::string>::iterator it = values.begin(); it != values.end(); ++it)
            {
              int face_index = atoi( (*it).c_str() );
              for (unsigned int j = 0; j < faces[face_index].size(); ++j)
                viennagrid::add( current_region, faces[face_index][j] );
            }
          }
        }



        delete root;

        return EXIT_SUCCESS;
      } //operator()

    };


  }
}

#endif
