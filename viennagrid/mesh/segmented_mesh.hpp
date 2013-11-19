#ifndef VIENNAGRID_SEGMENTED_MESH_HPP
#define VIENNAGRID_SEGMENTED_MESH_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"

namespace viennagrid
{

  template<typename MeshT, typename SegmentationT>
  struct segmented_mesh
  {
    typedef MeshT mesh_type;
    typedef SegmentationT segmentation_type;

    mesh_type mesh;
    segmentation_type segmentation;
  };

  template<typename WrappedMeshConfig, typename WrappedSegmentationConfig>
  struct segmented_mesh< viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> >
  {
    typedef viennagrid::mesh<WrappedMeshConfig> mesh_type;
    typedef viennagrid::segmentation<WrappedSegmentationConfig> segmentation_type;

    segmented_mesh() : segmentation(mesh) {}

    mesh_type mesh;
    segmentation_type segmentation;
  };

}

#endif
