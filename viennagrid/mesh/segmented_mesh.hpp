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
      clear();

      viennagrid::vertex_copy_map<mesh_type, mesh_type> vertex_map(mesh);

      typedef typename viennagrid::result_of::cell<mesh_type>::type CellType;
      typedef typename viennagrid::result_of::coord<mesh_type>::type NumericType;

      typedef typename viennagrid::result_of::const_cell_range<mesh_type>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef typename viennagrid::result_of::segment_id_range<segmentation_type, CellType>::type SrcSegmentIDRangeType;
      typedef typename viennagrid::result_of::segment_id<segmentation_type>::type DstSegmentIDType;

      typedef typename viennagrid::result_of::cell_handle<mesh_type>::type CellHandleType;

      ConstCellRangeType cells(other.mesh);
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        CellHandleType cell_handle = vertex_map.copy_element(*cit );
        viennagrid::add( segmentation, cell_handle, viennagrid::segment_ids( other.segmentation, *cit ).begin(), viennagrid::segment_ids( other.segmentation, *cit ).end() );
      }

//       mesh = other.mesh;
//       copy_segmentation( other.mesh, other.segmentation, mesh, segmentation );

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
