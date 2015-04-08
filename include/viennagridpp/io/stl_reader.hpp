#ifndef VIENNAGRID_IO_STL_READER_HPP
#define VIENNAGRID_IO_STL_READER_HPP

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
#include <assert.h>

#include "viennagridpp/forwards.hpp"
#include "viennagridpp/io/helper.hpp"

#include "viennagridpp/mesh/mesh.hpp"

/** @file viennagrid/io/tetgen_poly_reader.hpp
    @brief Provides a reader for STL files. See http://en.wikipedia.org/wiki/STL_(file_format)
*/



namespace viennagrid
{
  namespace result_of
  {
    template<int BitCountV>
    struct integer;

    template<>
    struct integer<sizeof(char)*8>
    {
      typedef char type;
    };

    template<>
    struct integer<sizeof(short)*8>
    {
      typedef short type;
    };

    template<>
    struct integer<sizeof(int)*8>
    {
      typedef int type;
    };

    template<>
    struct integer<sizeof(long)*8>
    {
      typedef long type;
    };



    template<int BitCountV>
    struct unsigned_integer;

    template<>
    struct unsigned_integer<sizeof(unsigned char)*8>
    {
      typedef unsigned char type;
    };

    template<>
    struct unsigned_integer<sizeof(unsigned short)*8>
    {
      typedef unsigned short type;
    };

    template<>
    struct unsigned_integer<sizeof(unsigned int)*8>
    {
      typedef unsigned int type;
    };

    template<>
    struct unsigned_integer<sizeof(unsigned long)*8>
    {
      typedef unsigned long type;
    };
  };


  namespace io
  {




    /** @brief Reader for STL files.
      *
      * See http://en.wikipedia.org/wiki/STL_(file_format) for a description of the file format
      */
    template<typename NumericConfigT = double>
    class stl_reader
    {
    public:

      stl_reader() : nc(1e-6) {}
      stl_reader(NumericConfigT const & nc_) : nc(nc_) {}

      /** @brief The functor interface triggering the read operation. Segmentations are not supported
       *
       * @param mesh          The mesh where the file content is written to
       * @param filename      Name of the file
       */
      template <typename MeshT>
      void operator()(MeshT const & mesh, std::string const & filename) const
      {
        std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
        if (!file)
        {
          throw cannot_open_file_exception("* ViennaGrid: tetgen_poly_reader::operator(): File " + filename + ": Cannot open file!");
        }

        char header[6];
        file.read(header, 5);
        header[5] = '\0';

        file.close();

        if (strcmp(header, "solid") == 0)
          read_ascii(mesh, filename);
        else
          read_binary(mesh, filename);
      }



      template <typename MeshT>
      void read_ascii(MeshT const & /*mesh*/, std::string const & /*filename*/) const
      {
        // TODO implement!!
      }


      /** @brief The functor interface triggering the read operation. Segmentations are not supported in this version.
       *
       * @param mesh          The mesh where the file content is written to
       * @param filename      Name of the file
       */
      template <typename MeshT>
      void read_binary(MeshT const & mesh, std::string const & filename) const
      {
        typedef typename viennagrid::result_of::point<MeshT>::type           PointType;

//         static const std::size_t point_dim = viennagrid::result_of::static_size<PointType>::value;

        typedef typename result_of::element<MeshT>::type         VertexType;
        typedef typename VertexType::id_type VertexIDType;

//         typedef typename result_of::element<MeshT>::type            LineType;

        std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* poly_reader::operator(): Reading file " << filename << std::endl;
        #endif



        char buffer[80];
        file.read(buffer, 80); // skipping header

        typedef result_of::unsigned_integer<32>::type UInt32Type;
        UInt32Type triangle_count;

        file.read( (char*)(&triangle_count), sizeof(UInt32Type) );

        for (UInt32Type i = 0; i < triangle_count; ++i)
        {
          float pt_in[3];
          PointType pt(3);

          file.read( buffer, sizeof(float)*3 ); // skipping normal vector

          VertexType vertices[3];

          for (std::size_t j = 0; j < 3; ++j)
          {
            file.read( (char*)(pt_in), sizeof(float)*3 );
            std::copy(pt_in, pt_in+3, pt.begin());
            vertices[j] = viennagrid::make_unique_vertex(mesh, pt, nc);
          }

          viennagrid::make_element(mesh, viennagrid::triangle_tag(), vertices, vertices+3);

          file.read( buffer, sizeof(char)*2 ); // skipping attribute count
        }


      } //operator()

    private:

      NumericConfigT nc;

    }; //class std_reader

  } //namespace io
} //namespace viennagrid

#endif
