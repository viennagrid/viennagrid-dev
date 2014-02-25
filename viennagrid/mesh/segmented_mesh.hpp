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



  template<typename SourceMeshT, typename SourceSegmentationT,
           typename DestinationMeshT, typename DestinationSegmentationT>
  struct copy_segmentation_functor
  {
    copy_segmentation_functor(SourceMeshT const & src_mesh_, SourceSegmentationT const & src_segmentation_,
                              DestinationMeshT & dst_mesh_, DestinationSegmentationT & dst_segmentation_) :
                              src_mesh(src_mesh_), src_segmentation(src_segmentation_),
                              dst_mesh(dst_mesh_), dst_segmentation(dst_segmentation_) {}

    template<typename DestinationElementT>
    void operator()( DestinationElementT & dst_element )
    {
      typedef typename viennagrid::result_of::element_tag<DestinationElementT>::type ElementTag;
      typedef typename viennagrid::result_of::const_element_range<SourceMeshT, ElementTag>::type ConstSourceElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstSourceElementRangeType>::type ConstSourceElementIteratorType;

      ConstSourceElementIteratorType src_eit = viennagrid::find( src_mesh, dst_element.id() );

      typedef typename viennagrid::result_of::element<SourceMeshT, ElementTag>::type SourceElementType;
      typedef typename viennagrid::result_of::segment_id_range<SourceSegmentationT, SourceElementType>::type SegmentIDRangeType;
      typedef typename viennagrid::result_of::iterator<SegmentIDRangeType>::type SegmentIDIteratorType;
      SegmentIDRangeType segment_ids = viennagrid::segment_ids(src_segmentation, *src_eit);
      for (SegmentIDIteratorType sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
      {
        viennagrid::add( dst_segmentation.get_make_segment(*sit), dst_element );
      }
    }

    SourceMeshT const & src_mesh;
    SourceSegmentationT const & src_segmentation;

    DestinationMeshT & dst_mesh;
    DestinationSegmentationT & dst_segmentation;
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

      mesh = other.mesh;

      copy_segmentation_functor<mesh_type, segmentation_type, mesh_type, segmentation_type> functor( other.mesh, other.segmentation, mesh, segmentation );
      viennagrid::for_each(mesh, functor);

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
