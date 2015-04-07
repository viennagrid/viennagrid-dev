#include <iostream>

#include "viennagridpp/core.hpp"

#include "viennagridpp/io/vtk_reader.hpp"
#include "viennagridpp/io/vtk_writer.hpp"




int main()
{
  typedef viennagrid::mesh_t MeshType;
  typedef viennagrid::result_of::region<MeshType>::type MeshRegionType;

  typedef viennagrid::result_of::point<MeshType>::type PointType;
  typedef viennagrid::result_of::element<MeshType>::type VertexType;
  typedef viennagrid::result_of::element<MeshType>::type LineType;
  typedef viennagrid::result_of::element<MeshType>::type TriangleType;
  typedef viennagrid::result_of::element<MeshType>::type CellType;

  typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
  typedef viennagrid::result_of::element_range<MeshType, 2>::type TriangleRangeType;
  typedef viennagrid::result_of::vertex_range<TriangleType>::type VertexOnTriangleRangeType;
  typedef viennagrid::result_of::element_range<TriangleType, 1>::type LineOnTriangleRangeType;
  typedef viennagrid::result_of::vertex_range<LineType>::type VertexOnLineRangeType;



//   MeshType mesh;
//
//   VertexType vertex_handles[5];
//
//   vertex_handles[0] = viennagrid::make_vertex( mesh, viennagrid::make_point(0,0,0) );
//   vertex_handles[1] = viennagrid::make_vertex( mesh, viennagrid::make_point(1,0,0) );
//   vertex_handles[2] = viennagrid::make_vertex( mesh, viennagrid::make_point(0,1,0) );
//   vertex_handles[3] = viennagrid::make_vertex( mesh, viennagrid::make_point(0,0,1) );
//   vertex_handles[4] = viennagrid::make_vertex( mesh, viennagrid::make_point(0,0,-1) );
//
//
//
//   VertexType tets[4];
//   tets[0] = vertex_handles[0];
//   tets[1] = vertex_handles[1];
//   tets[2] = vertex_handles[2];
//   tets[3] = vertex_handles[3];
//
//   CellType t0 = viennagrid::make_element<viennagrid::tetrahedron_tag>(mesh, tets, tets+4);
//
//
//   tets[0] = vertex_handles[0];
//   tets[1] = vertex_handles[1];
//   tets[2] = vertex_handles[2];
//   tets[3] = vertex_handles[4];
//
//   CellType t1 = viennagrid::make_element<viennagrid::tetrahedron_tag>(mesh, tets, tets+4);
//
//   MeshRegionType r0 = mesh.make_region();
//   MeshRegionType r1 = mesh.make_region();
//
//
//   viennagrid::add( r0, t0 );
//   viennagrid::add( r1, t1 );
//
//
//   {
//     VertexRangeType vertex_range(mesh);
//     std::cout << "Vertex count = " << vertex_range.size() << std::endl;
//   }
//
//   {
//     TriangleRangeType triangle_range(mesh);
//     std::cout << "Triangle count = " << triangle_range.size() << std::endl;
//
//     VertexOnTriangleRangeType vertex_range( triangle_range[1] );
//     std::cout << "Vertex count = " << vertex_range.size() << std::endl;
//
//     for (VertexOnTriangleRangeType::iterator vit = vertex_range.begin(); vit != vertex_range.end(); ++vit)
//       std::cout << (*vit).id() << "  " << viennagrid::get_point(mesh, *vit) << std::endl;
//   }
//
//
//   {
//     TriangleRangeType triangle_range(mesh);
//     for (TriangleRangeType::iterator tit = triangle_range.begin(); tit != triangle_range.end(); ++tit)
//     {
//       std::cout << "Triangle ID = " << (*tit).id() << std::endl;
//       LineOnTriangleRangeType line_range( *tit );
//
//       for (LineOnTriangleRangeType::iterator lit = line_range.begin(); lit != line_range.end(); ++lit)
//       {
//         std::cout << "  Line ID = " << (*lit).id() << std::endl;
//         VertexOnLineRangeType vtx_on_lines(*lit);
//
//         for (VertexOnLineRangeType::iterator it = vtx_on_lines.begin(); it != vtx_on_lines.end(); ++it)
//           std::cout << "    " << (*it).id() << " " << viennagrid::get_point(mesh, *it) << std::endl;
//       }
//     }
//   }
//
//   {
//     typedef viennagrid::result_of::const_cell_range<MeshType>::type CellRangeType;
//     CellRangeType cells(mesh);
//     for (VertexRangeType::const_iterator it = cells.begin(); it != cells.end(); ++it)
//     {
//       std::cout << "Cell: " << (*it).id() << std::endl;
//     }
//   }
//
//
//   std::cout << std::endl;
//   std::cout << std::endl;

//   {
//     typedef viennagrid::result_of::const_facet_range<MeshType>::type FacetRangeType;
//     FacetRangeType facets(mesh);
//     for (VertexRangeType::const_iterator it = facets.begin(); it != facets.end(); ++it)
//     {
//       std::cout << *it << std::endl;
//       std::cout << "Facet: " << (*it).id() << std::endl;
//       std::cout << "  is boundary of region 0: " << std::boolalpha << viennagrid::is_boundary(r0, *it) << std::endl;
//       std::cout << "  is boundary of region 1: " << std::boolalpha << viennagrid::is_boundary(r1, *it) << std::endl;
//       std::cout << std::endl;
//     }
//   }
//
//   std::cout << std::endl;
//   std::cout << std::endl;
//
//   {
//     typedef viennagrid::result_of::const_line_range<MeshType>::type LineRangeType;
//     LineRangeType lines(mesh);
//     for (VertexRangeType::const_iterator it = lines.begin(); it != lines.end(); ++it)
//     {
//       std::cout << *it << std::endl;
//       std::cout << "Facet: " << (*it).id() << std::endl;
//       std::cout << "  is boundary of region 0: " << std::boolalpha << viennagrid::is_boundary(r0, *it) << std::endl;
//       std::cout << "  is boundary of region 1: " << std::boolalpha << viennagrid::is_boundary(r1, *it) << std::endl;
//       std::cout << std::endl;
//     }
//   }
//
//   std::cout << std::endl;
//   std::cout << std::endl;
//
//
//   {
//     typedef viennagrid::result_of::coboundary_range<MeshType, viennagrid::cell_tag>::type CoboundaryTriangleRangeType;
//
//     VertexRangeType vertex_range(mesh);
//     for (VertexRangeType::iterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
//     {
//       std::cout << "Vertex " << (*it).id() << std::endl;
//       CoboundaryTriangleRangeType coboundary_triangles(mesh, *it);
//       for (CoboundaryTriangleRangeType::iterator jt = coboundary_triangles.begin(); jt != coboundary_triangles.end(); ++jt)
//       {
//         std::cout << "  coboundary " << (*jt).id() << "  region=";
//
//         if (viennagrid::is_in_region(r0, *jt))
//           std::cout << "0";
//         if (viennagrid::is_in_region(r1, *jt))
//           std::cout << "1";
//
//         std::cout << std::endl;
//       }
//     }
//   }
//
//
//
//
//
//   {
//     typedef viennagrid::result_of::coboundary_range<MeshType, viennagrid::cell_tag>::type CoboundaryTriangleRangeType;
//
//     TriangleRangeType triangle_range(mesh);
//     for (TriangleRangeType::iterator it = triangle_range.begin(); it != triangle_range.end(); ++it)
//     {
//       std::cout << "Triangle " << (*it).id() << std::endl;
//       CoboundaryTriangleRangeType coboundary_triangles(mesh, *it);
//       for (CoboundaryTriangleRangeType::iterator jt = coboundary_triangles.begin(); jt != coboundary_triangles.end(); ++jt)
//       {
//         std::cout << "  coboundary " << (*jt).id() << "  region=";
//
//         if (viennagrid::is_in_region(r0, *jt))
//           std::cout << "0";
//         if (viennagrid::is_in_region(r1, *jt))
//           std::cout << "1";
//
//         std::cout << std::endl;
//       }
//     }
//   }
//
//
//
//   typedef viennagrid::result_of::region<MeshType>::type RegionType;
//   typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
//   RegionRangeType regions(mesh);
//   for (RegionRangeType::iterator pit = regions.begin(); pit != regions.end(); ++pit)
//   {
//     typedef viennagrid::result_of::tetrahedron_range<RegionType>::type TetrahedronRangeType;
//
//     std::cout << "Partition " << (*pit).region().id() << std::endl;
//     TetrahedronRangeType tets(*pit);
//     for (TetrahedronRangeType::iterator it = tets.begin(); it != tets.end(); ++it)
//       std::cout << (*it).id() << std::endl;
//   }




















  {
    MeshType vtk_root;


    viennagrid::io::vtk_reader<MeshType> reader;
    reader(vtk_root, "../data/tets_with_data_main.pvd");

//     std::cout << viennagrid::vertices(vtk_root).size() << std::endl;
//     std::cout << viennagrid::cells(vtk_root).size() << std::endl;
//     std::cout << vtk_root.region_count() << std::endl;

//     viennagrid::mesh_region_t vtk_region0 = vtk_root.get_make_region(0);
//     viennagrid::mesh_region_t vtk_region1 = vtk_root.get_make_region(1);


//     std::cout << std::endl;
//     std::cout << std::endl;
//
//     int count = 0;
//     {
//       typedef viennagrid::result_of::const_facet_range<MeshType>::type FacetRangeType;
//       FacetRangeType facets(vtk_root);
//       for (VertexRangeType::const_iterator it = facets.begin(); it != facets.end(); ++it)
//       {
//         if ( !viennagrid::is_boundary(vtk_root, *it) )
//           continue;
//
//         std::cout << *it << std::endl;
//         std::cout << std::endl;
//         ++count;
//       }
//     }
//
//     std::cout << std::endl;
//     std::cout << std::endl;
//
//     std::cout << "COUNT = " << count << std::endl;

//     viennagrid::io::vtk_writer<MeshType> writer;
//     writer(vtk_root, "test");
  }


  return -1;
}
