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
#include <cstdlib>
#include <vector>


#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/algorithm/volume.hpp"


template <typename MeshtType,
          typename VertexBoxVolumeAccessorT, typename VertexBoxVolumeContributionAccessorT,
          typename InterfaceAreaAccessorT, typename InterfaceContributionAccessorT>
void output_voronoi_info(MeshtType const & d,
                         VertexBoxVolumeAccessorT const vertex_box_volume_accessor, VertexBoxVolumeContributionAccessorT const vertex_box_volume_contribution_accessor,
                         InterfaceAreaAccessorT const interface_area_accessor, InterfaceContributionAccessorT const interface_contribution_accessor)
{
  typedef typename viennagrid::result_of::const_vertex_range<MeshtType>::type    VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type         VertexIterator;

  typedef typename viennagrid::result_of::const_line_range<MeshtType>::type    EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type           EdgeIterator;

//   typedef std::vector< std::pair<CellType const *, double> >      CellContributionType;
  typedef typename VertexBoxVolumeContributionAccessorT::value_type VertexBoxVolumeContributionType;
  typedef typename InterfaceContributionAccessorT::value_type InterfaceContributionType;

//   long counter = 0;

//   viennagrid::voronoi_interface_area_key interface_key;
//   viennagrid::voronoi_box_volume_key box_volume_key;

  std::cout << "-" << std::endl;
  std::cout << "- Vertex box volume information: " << std::endl;
  std::cout << "-" << std::endl;
  VertexContainer vertices(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
//     std::cout << "Vertex " << counter++ << ": " << viennadata::access<viennagrid::voronoi_box_volume_key, double>(box_volume_key)(*vit) << std::endl;
    std::cout << vertex_box_volume_accessor(*vit) << std::endl;

//     CellContributionType const & contrib = viennadata::access<viennagrid::voronoi_box_volume_key, CellContributionType>(box_volume_key)(*vit);
    VertexBoxVolumeContributionType const & contrib = vertex_box_volume_contribution_accessor(*vit);

    for (std::size_t i=0; i<contrib.size(); ++i)
      std::cout << "From cell " << contrib[i].first << ": " << contrib[i].second << std::endl;
  }

  std::cout << "-" << std::endl;
  std::cout << "- Edge Information: " << std::endl;
  std::cout << "-" << std::endl;
//   counter = 0;
  EdgeContainer edges(d);
  for (EdgeIterator eit  = edges.begin();
                    eit != edges.end();
                  ++eit)
  {
    std::cout << *eit << std::endl;
    std::cout << "Length: "    << viennagrid::volume(*eit)            << std::endl;
//     std::cout << "Interface: " << viennadata::access<viennagrid::voronoi_interface_area_key, double>(interface_key)(*eit) << std::endl;
    std::cout << "Interface: " << interface_area_accessor(*eit) << std::endl;

//     CellContributionType const & contrib = viennadata::access<viennagrid::voronoi_interface_area_key, CellContributionType>(interface_key)(*eit);
    InterfaceContributionType const & contrib = interface_contribution_accessor(*eit);

    for (std::size_t i=0; i<contrib.size(); ++i)
      std::cout << "From cell " << contrib[i].first << ": " << contrib[i].second << std::endl;
  }

}


//
// Test for Voronoi correctness: Volume of cells must equal volume of boxes
//
template <typename MeshType, typename VertexBoxVolumeAccessorT>
double voronoi_volume(MeshType const & d,
                      VertexBoxVolumeAccessorT const vertex_box_volume_accessor)
{
  typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type                          VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                     VertexIterator;

//   viennagrid::voronoi_box_volume_key box_volume_key;

  double boxed_volume = 0;
  VertexContainer vertices(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
//     boxed_volume += viennadata::access<viennagrid::voronoi_box_volume_key, double>(box_volume_key)(*vit);
    boxed_volume += vertex_box_volume_accessor(*vit);
  }

  return boxed_volume;
}



//
// Test for Voronoi correctness: Volume of cells must equal volume of boxes
//
template <typename MeshType, typename VertexBoxVolumeContributionAccessorT>
double voronoi_volume_vertex_detailed(MeshType const & d,
                                      VertexBoxVolumeContributionAccessorT const vertex_box_volume_contribution_accessor)
{
  typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type                          VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                     VertexIterator;

//   typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;

//   viennagrid::voronoi_box_volume_key box_volume_key;
  typedef typename VertexBoxVolumeContributionAccessorT::value_type VertexBoxVolumeContributionType;

  double boxed_volume = 0;
  VertexContainer vertices(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
//     CellContributionType const & contributions = viennadata::access<viennagrid::voronoi_box_volume_key, CellContributionType>(box_volume_key)(*vit);
    VertexBoxVolumeContributionType const & contributions = vertex_box_volume_contribution_accessor(*vit);
    for (std::size_t i=0; i<contributions.size(); ++i)
    {
      boxed_volume += contributions[i].second;
    }
  }

  return boxed_volume;
}


template <typename MeshType, typename EdgeBoxVolumeContributionAccessorT>
double voronoi_volume_edge_detailed(MeshType const & d,
                                    EdgeBoxVolumeContributionAccessorT const edge_box_volume_contribution_accessor)
{
  typedef typename viennagrid::result_of::const_line_range<MeshType>::type               EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type                        EdgeIterator;

//   typedef typename viennagrid::result_of::voronoi_cell_contribution<CellType>::type      CellContributionType;

//   viennagrid::voronoi_box_volume_key box_volume_key;
  typedef typename EdgeBoxVolumeContributionAccessorT::value_type EdgeBoxVolumeContributionType;

  double boxed_volume = 0;
  EdgeContainer edges(d);
  for (EdgeIterator eit  = edges.begin();
                    eit != edges.end();
                  ++eit)
  {
//     CellContributionType const & contributions = viennadata::access<viennagrid::voronoi_box_volume_key, CellContributionType>(box_volume_key)(*eit);
    EdgeBoxVolumeContributionType const & contributions = edge_box_volume_contribution_accessor(*eit);
    for (std::size_t i=0; i<contributions.size(); ++i)
    {
      boxed_volume += contributions[i].second;
    }
  }

  return boxed_volume;
}

template <typename MeshType,
          typename VertexBoxVolumeAccessorT,
          typename VertexBoxVolumeContributionAccessorT,
          typename EdgeBoxVolumeContributionAccessorT>
void voronoi_volume_check(MeshType const & mesh,
                          VertexBoxVolumeAccessorT const vertex_box_volume_accessor,
                          VertexBoxVolumeContributionAccessorT const vertex_box_volume_contribution_accessor,
                          EdgeBoxVolumeContributionAccessorT const edge_box_volume_contribution_accessor)
{
  double voronoi_vol = voronoi_volume(mesh, vertex_box_volume_accessor);
  double voronoi_vol_vertices = voronoi_volume_vertex_detailed(mesh, vertex_box_volume_contribution_accessor);
  double voronoi_vol_edges = voronoi_volume_edge_detailed(mesh, edge_box_volume_contribution_accessor);
  double mesh_vol = viennagrid::volume(mesh);

  if ( fabs(voronoi_vol - mesh_vol) / mesh_vol > 1e-10 )
  {
    std::cerr << "Mismatch of volumes: " << voronoi_vol << " vs " << mesh_vol << std::endl;
    exit(EXIT_FAILURE);
  }
  else
    std::cout << "Volume check passed: " << voronoi_vol << " vs " << mesh_vol << std::endl;

  if ( fabs(voronoi_vol_vertices - mesh_vol) / mesh_vol > 1e-10 )
  {
    std::cerr << "Mismatch of volumes (detailed, vertex): " << voronoi_vol_vertices << " vs " << mesh_vol << std::endl;
    exit(EXIT_FAILURE);
  }
  else
    std::cout << "Detailed vertices volume check passed: " << voronoi_vol_vertices << " vs " << mesh_vol << std::endl;

  if ( fabs(voronoi_vol_edges - mesh_vol) / mesh_vol > 1e-10 )
  {
    std::cerr << "Mismatch of volumes (detailed, edge): " << voronoi_vol_edges << " vs " << mesh_vol << std::endl;
    exit(EXIT_FAILURE);
  }
  else
    std::cout << "Detailed edge volume check passed: " << voronoi_vol_vertices << " vs " << mesh_vol << std::endl;

}

