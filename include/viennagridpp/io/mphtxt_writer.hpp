#ifndef VIENNAGRID_MPHTXT_WRITER_HPP
#define VIENNAGRID_MPHTXT_WRITER_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>
#include <sstream>
#include <fstream>

#include "viennagridpp/algorithm/centroid.hpp"
#include "viennagridpp/algorithm/geometry.hpp"


/** @file viennagrid/io/mphtxt_writer.hpp
    @brief Provides a writer for Comsol .mphtxt files.
*/


namespace viennagrid
{
  namespace io
  {

    template<typename TriangleT>
    struct triangle_information
    {
      triangle_information(TriangleT const & triangle_) : triangle(triangle_), up_index(0), down_index(0), contact_index(-1), segment_index(-1) {}

      TriangleT triangle;

      int up_index;
      int down_index;

      int contact_index;
      int segment_index;
    };




    template<typename RegionT, typename AccessorType, typename TriangleT, typename RegionRangeType>
    void mark_planar_neighbors( RegionT const & region, AccessorType & contact_index_accessor,
                                TriangleT const & triangle, int contact_index,
                                RegionRangeType const & region_range )
    {
      typedef typename viennagrid::result_of::point<RegionT>::type PointType;
      typedef typename viennagrid::result_of::coord<RegionT>::type NumericType;
      PointType normal = viennagrid::normal_vector( triangle );

      typedef typename viennagrid::result_of::const_neighbor_range<RegionT, 0, 2>::type NeigborTriangleRangeType;
      typedef typename viennagrid::result_of::iterator<NeigborTriangleRangeType>::type NeigborTriangleIteratorType;

      NeigborTriangleRangeType neighbor_triangles( region, triangle );
      for (NeigborTriangleIteratorType ntit = neighbor_triangles.begin(); ntit != neighbor_triangles.end(); ++ntit )
      {
        if (contact_index_accessor.get(*ntit) != -1)
          continue;

        if (!viennagrid::is_boundary(region, *ntit))
          continue;

        if (!viennagrid::equal_regions( region_range, viennagrid::regions(region.get_mesh(),*ntit) ))
          continue;

        PointType neighbor_normal = viennagrid::normal_vector(*ntit);

        NumericType dot = viennagrid::inner_prod( normal, neighbor_normal );

        if ( std::abs(dot) > (1.0-1e-6) * viennagrid::norm_2(normal) * viennagrid::norm_2(neighbor_normal) )
        {
          contact_index_accessor.set(*ntit, contact_index);
          mark_planar_neighbors( region, contact_index_accessor, *ntit, contact_index, region_range );
        }
      }
    }




    template<typename MeshT, typename AccessorType>
    void mark_segment_hull_contacts( MeshT const & mesh, AccessorType & contact_index_accessor )
    {
      typedef typename viennagrid::result_of::region<MeshT>::type RegionType;
      typedef typename viennagrid::result_of::const_element_range<MeshT, 2>::type ConstTriangleRangeType;
      typedef typename viennagrid::result_of::iterator<ConstTriangleRangeType>::type ConstTriangleIteratorType;

      ConstTriangleRangeType triangles(mesh);
      for (ConstTriangleIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
        contact_index_accessor.set(*tit, -1);


      typedef typename viennagrid::result_of::region_range<MeshT>::type RegionRangeType;
      typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

      int current_contact_index = 0;
      RegionRangeType region_range(mesh);
      for (RegionRangeIterator rit = region_range.begin(); rit != region_range.end(); ++rit)
      {
        typedef typename viennagrid::result_of::const_element_range<RegionType, 2>::type ConstTriangleOnSegmentRangeType;
        typedef typename viennagrid::result_of::iterator<ConstTriangleOnSegmentRangeType>::type ConstTriangleOnSegmentIteratorType;

        ConstTriangleOnSegmentRangeType triangles_on_segment(*rit);
        for (ConstTriangleOnSegmentIteratorType tsit = triangles_on_segment.begin(); tsit != triangles_on_segment.end(); ++tsit)
        {
          if ( viennagrid::is_boundary(*rit, *tsit) && (contact_index_accessor.get(*tsit) == -1) )
          {
            contact_index_accessor.set(*tsit, current_contact_index);

            typedef typename viennagrid::result_of::element<MeshT>::type TriangleType;
            typedef typename viennagrid::result_of::const_region_range<MeshT, TriangleType>::type TriangleRegionRangeType;

            TriangleRegionRangeType triangle_regions = viennagrid::regions(mesh, *tsit);
            mark_planar_neighbors(*rit, contact_index_accessor, *tsit, current_contact_index, triangle_regions);

            ++current_contact_index;
          }
        }
      }

      std::cout << "Number of contacts: " << current_contact_index << std::endl;
    }



    /** @brief Writer for Comsol .mphtxt files.
      *
      * See http://chemelab.ucsd.edu/CAPE/comsol/Comsol_Reference.pdf, page 396-441 for a description of the file format
      */
    class mphtxt_writer
    {
    public:

      /** @brief The functor interface triggering the read operation. Segmentations are not supported in this version.
       *
       * @param mesh            The mesh which is to be written
       * @param segmentation    The segmentation which is to be written
       * @param filename        Name of the file
       */
      template <typename MeshT>
      void operator()(MeshT const & mesh, std::string const & filename) const
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennagrid::result_of::element_id<MeshT>::type VertexIDType;
        typedef typename viennagrid::result_of::element_id<MeshT>::type TriangleIDType;

        typedef typename viennagrid::result_of::element<MeshT>::type TriangleType;


        typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

        typedef typename viennagrid::result_of::const_element_range<MeshT, 2>::type ConstTriangleRangeType;

        typedef typename viennagrid::result_of::const_element_range<MeshT, 3>::type ConstTetrahedronRangeType;
        typedef typename viennagrid::result_of::iterator<ConstTetrahedronRangeType>::type ConstTetrahedronIteratorType;



        std::stringstream ss;
        if(filename.substr( filename.rfind(".")+1 ) == "mphtxt")
          ss << filename;
        else
          ss << filename << ".mphtxt";
        std::ofstream writer(ss.str().c_str());

        // writing the file header
        writer << "# Created by ViennaGrid mphtxt writer\n";
        writer << "\n";

        // MPHTXT version
        writer << "# Major & minor version\n";
        writer << "0 1\n";

        // one tag: viennagrid_mesh
        writer << "1 # number of tags\n";
        writer << "# Tags\n";
        writer << "15 viennagrid_mesh\n";


        writer << "1 # number of types\n";
        writer << "# Types\n";
        writer << "3 obj\n";

        writer << "\n";
        writer << "# --------- Object 0 ----------\n";
        writer << "\n";

        writer << "0 0 1\n";

        // specifying the type (4 = Mesh)
        writer << "4 Mesh # class\n";
        // mesh version = 2
        writer << "2 # version\n";
        // geometric dimension
        writer << viennagrid::geometric_dimension(mesh) << " # sdim\n";
        // number of vertices
        writer << viennagrid::vertices(mesh).size() << " # number of mesh points\n";
        // lowest index = 0
        writer << "0 # lowest mesh point index\n";
        writer << "\n";


        // writing the points and the coordinates
        writer << "# Mesh point coordinates\n";
        ConstVertexRangeType vertices(mesh);
        std::map<VertexIDType, int> vertex_index_map;

        std::size_t vertex_index = 0;
        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++vertex_index)
        {
          vertex_index_map[ (*vit).id() ] = vertex_index;

          writer.precision( std::numeric_limits<typename PointType::value_type>::digits10 );
          PointType point = viennagrid::get_point(*vit);
          for (std::size_t i = 0; i < point.size(); ++i)
            writer << point[i] << " ";
          writer << "\n";
        }
        writer << "\n";

        writer << "\n";
        // 2 element types: tetrahedra and triangles
        writer << "2 # number of element types\n";


        // triangle section
        writer << "\n";
        writer << "3 tri\n";
        writer << "3 # nodes per element\n";
        writer << "\n";

        ConstTriangleRangeType triangles(mesh);

        typedef typename viennagrid::result_of::const_element<MeshT>::type ConstTriangleType;
        typedef triangle_information<ConstTriangleType> TriInfoType;

        std::map<TriangleIDType, TriInfoType> used_triangles;
        typedef typename viennagrid::result_of::region<MeshT>::type RegionType;

        // saving only boundary and interface triangles explicitly
        // calculating the orientation for all boundary triangles according to the segments

        typedef typename viennagrid::result_of::region_range<MeshT>::type RegionRangeType;
        typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

        RegionRangeType regions = viennagrid::regions(mesh);
        for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
        {
          typedef typename viennagrid::result_of::const_element_range<RegionType, 2>::type TriangleOnRegionRangeType;
          typedef typename viennagrid::result_of::iterator<TriangleOnRegionRangeType>::type TriangleOnRegionIteratorType;

          TriangleOnRegionRangeType triangles(*rit);
          for (TriangleOnRegionIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
          {
            if ( viennagrid::is_boundary(*rit, *tit) )
            {
              typename std::map<TriangleIDType, TriInfoType>::iterator utit = used_triangles.find( (*tit).id() );
              if (utit == used_triangles.end())
                utit = used_triangles.insert( std::make_pair((*tit).id(), TriInfoType(*tit)) ).first;

              PointType triangle_normal = viennagrid::normal_vector(*tit);
              PointType triangle_center = viennagrid::centroid(*tit);

              typedef typename viennagrid::result_of::const_coboundary_range<RegionType, 3>::type CoboundaryTetrahedronRangeType;

              CoboundaryTetrahedronRangeType coboundary_tetrahedrons(*rit, *tit);
              if (coboundary_tetrahedrons.size() != 1)
              {
                std::cout << "FEHLER!!! there is more than one on co-boundary tetrahedron" << std::endl;
              }

              PointType tetrahedron_center = viennagrid::centroid( coboundary_tetrahedrons[0] );

              bool orientation = viennagrid::inner_prod( triangle_center-tetrahedron_center, triangle_normal ) > 0;

              if (orientation)
              {
                if (utit->second.up_index != 0)
                {
                  std::cout << "FEHLER!!! 1" << std::endl;
                }

                utit->second.up_index = (*rit).id()+1;
              }
              else
              {
                if (utit->second.down_index != 0)
                {
                  std::cout << "FEHLER!!! 2" << std::endl;
                }

                utit->second.down_index = (*rit).id()+1;
              }
            }
          }
        }


        std::vector<int> contact_index_array;
        typename viennagrid::result_of::accessor< std::vector<int>, TriangleType>::type contact_index_accessor(contact_index_array);

        // automatic calculating all contacts, a contact is a set of coplanar boundary/interface triangles
        // a contact can be selected by COMSOL for e.g. boundary value specification
        mark_segment_hull_contacts( mesh, contact_index_accessor );


        // writing the triangles to the file
        writer << used_triangles.size() << " # number of triangles\n";
        writer << "\n";

        // writing the vertex indices of the triangles
        for (typename std::map<TriangleIDType, TriInfoType>::const_iterator utit = used_triangles.begin(); utit != used_triangles.end(); ++utit)
        {
          typedef typename viennagrid::result_of::element<MeshT>::type TriangleType;
          typedef typename viennagrid::result_of::const_vertex_range<TriangleType>::type VertexOnTriangleRangeType;
          typedef typename viennagrid::result_of::iterator<VertexOnTriangleRangeType>::type VertexOnTriangleIteratorType;

          VertexOnTriangleRangeType vertices_on_triangle( utit->second.triangle );
          for (VertexOnTriangleIteratorType vtit = vertices_on_triangle.begin(); vtit != vertices_on_triangle.end(); ++vtit)
          {
            writer << vertex_index_map[ (*vtit).id() ] << " ";
          }
          writer << "\n";
        }

        writer << "\n";
        writer << "3\n";
        writer << "0\n";

        // writing the contact ID for each triangle
        writer << used_triangles.size() << " # number of triangles\n";
        for (typename std::map<TriangleIDType, TriInfoType>::const_iterator utit = used_triangles.begin(); utit != used_triangles.end(); ++utit)
          writer << contact_index_accessor.get( utit->second.triangle ) << "\n";

        // writing the orientation for each triangle, the first value is the segment on the positive side of the triangle, the second value the segment on the negative side;
        // segments start with 1, 0 is reserved for no segment
        writer << "\n";
        writer << used_triangles.size() << " # number of triangles\n";
        writer << "\n";
        for (typename std::map<TriangleIDType, TriInfoType>::const_iterator utit = used_triangles.begin(); utit != used_triangles.end(); ++utit)
          writer << utit->second.up_index << " " << utit->second.down_index << "\n";





        // writing the tetrahedra
        writer << "\n";
        writer << "3 tet\n";
        writer << "4 # nodes per element\n";
        writer << "\n";
        // number of tetrahedra
        writer << viennagrid::cells(mesh).size() << " # number of tetrahedron\n";
        writer << "\n";

        // writing the vertex indices of the tetrahedra
        ConstTetrahedronRangeType tetrahedrons(mesh);
        for (ConstTetrahedronIteratorType tit = tetrahedrons.begin(); tit != tetrahedrons.end(); ++tit)
        {
          typedef typename viennagrid::result_of::element<MeshT>::type TetrahedronType;
          typedef typename viennagrid::result_of::const_vertex_range<TetrahedronType>::type VertexOnTetrahedronRangeType;
          typedef typename viennagrid::result_of::iterator<VertexOnTetrahedronRangeType>::type VertexOnTetrahedronIteratorType;

          VertexOnTetrahedronRangeType vertices_on_tetrahedron(*tit);
          for (VertexOnTetrahedronIteratorType vtit = vertices_on_tetrahedron.begin(); vtit != vertices_on_tetrahedron.end(); ++vtit)
          {
            writer << vertex_index_map[ (*vtit).id() ] << " ";
          }
          writer << "\n";
        }

        // writing the segment information for the tetrahedra
        // segment IDs start with 1, 0 is reserved for no segment
        writer << "\n";
        writer << "4\n";
        writer << "0\n";
        writer << "\n";
        writer << viennagrid::cells(mesh).size() << " # number of tetrahedron\n";

        for (ConstTetrahedronIteratorType tit = tetrahedrons.begin(); tit != tetrahedrons.end(); ++tit)
        {
          typedef typename viennagrid::result_of::element<MeshT>::type TetrahedronType;
          typedef typename viennagrid::result_of::const_region_range<MeshT, TetrahedronType>::type TetrahedronRegionRangeType;

          TetrahedronRegionRangeType tetrahedron_regions = viennagrid::regions(mesh, *tit);
          writer << (*tetrahedron_regions.begin()).id()+1 << "\n";
        }

        writer << "\n";
        writer << "0\n";
      }



    private:

    };



  }
}



#endif
