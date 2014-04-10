#ifndef VIENNAGRID_SEGMENTED_MESH_HPP
#define VIENNAGRID_SEGMENTED_MESH_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/mesh_operations.hpp"

namespace viennagrid
{
  /** @brief A segmented_mesh is a mesh together with a segmentation.
   *
   * @tparam MeshT            The mesh type
   * @tparam SegmentationT    The segmentation type
   */
  template<typename MeshT, typename SegmentationT>
  struct segmented_mesh
  {
    typedef MeshT mesh_type;
    typedef SegmentationT segmentation_type;

    mesh_type mesh;
    segmentation_type segmentation;
  };


  /** @brief A specialization for viennagrid meshes and segmentation */
  template<typename WrappedMeshConfig, typename WrappedSegmentationConfig>
  struct segmented_mesh< viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> >
  {
    typedef segmented_mesh< viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> > SelfType;

    typedef viennagrid::mesh<WrappedMeshConfig> mesh_type;
    typedef viennagrid::segmentation<WrappedSegmentationConfig> segmentation_type;

    segmented_mesh() : segmentation(mesh) {}

    segmented_mesh(SelfType const & other) : segmentation(mesh)
    {
      *this = other;
    }


    SelfType & operator=(SelfType const & other)
    {
      copy( other.mesh, other.segmentation, mesh, segmentation, true_functor() );
      return *this;
    }

    void clear()
    {
      mesh.clear();
      segmentation.clear();
    }


    mesh_type mesh;
    segmentation_type segmentation;

  private:
  };

}

#endif
