#include <stdexcept>
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#define VIENNAGRID_LLMESH_HEADER_ONLY
#include "viennagrid/llmesh/api.hpp"






namespace viennagrid
{
  class llmesh_exception : public std::runtime_error
  {
  public:
    llmesh_exception(std::string const & message) : std::runtime_error(message) {}
    virtual ~llmesh_exception() throw() {}
  };




  namespace result_of
  {
    template<typename ElementTagT>
    struct viennagrid_llmesh_element_type;

    template<>
    struct viennagrid_llmesh_element_type<viennagrid::vertex_tag>
    {
      static const int value = VIENNAGRID_ELEMENT_TYPE_VERTEX;
    };

    template<>
    struct viennagrid_llmesh_element_type<viennagrid::line_tag>
    {
      static const int value = VIENNAGRID_ELEMENT_TYPE_LINE;
    };

    template<>
    struct viennagrid_llmesh_element_type<viennagrid::triangle_tag>
    {
      static const int value = VIENNAGRID_ELEMENT_TYPE_TRIANGLE;
    };

    template<>
    struct viennagrid_llmesh_element_type<viennagrid::quadrilateral_tag>
    {
      static const int value = VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL;
    };

    template<>
    struct viennagrid_llmesh_element_type<viennagrid::tetrahedron_tag>
    {
      static const int value = VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON;
    };

    template<>
    struct viennagrid_llmesh_element_type<viennagrid::hexahedron_tag>
    {
      static const int value = VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON;
    };
  }








  template<typename MeshT, typename SegmentationT>
  void copy( MeshT const & src_mesh, SegmentationT const & src_segmentation, viennagrid_llmesh & dst_mesh )
  {
    const int dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;
    viennagrid_error_code_t error;

    error = viennagrid_make_llmesh( dimension, &dst_mesh );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_make_llmesh error");

    error = viennagrid_set_segment_count( dst_mesh, src_segmentation.size() );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_set_segment_count error");

    viennagrid_int_t index = 0;
    for (typename SegmentationT::const_iterator it = src_segmentation.begin(); it != src_segmentation.end(); ++it, ++index)
    {
      error = viennagrid_set_segment_name( dst_mesh, index, (*it).name().c_str() );
      if (error != VIENNAGRID_SUCCESS)
        throw llmesh_exception("viennagrid_set_segment_name error");
    }




    viennagrid_int_t vertex_count = viennagrid::vertices(src_mesh).size();
    error = viennagrid_create_vertex_buffer( dst_mesh, vertex_count );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_create_vertex_buffer error");

    viennagrid_numeric_t * vertex_buffer;
    error = viennagrid_get_vertex_buffer( dst_mesh, &vertex_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_vertex_buffer error");

    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;
    typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    std::map<VertexIDType, viennagrid_int_t> vertex_index_map;

    ConstVertexRangeType vertices(src_mesh);
    index = 0;
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      vertex_index_map[ (*vit).id() ] = index;
      PointType const & point = viennagrid::point(*vit);
      for (int i = 0; i < dimension; ++i)
        vertex_buffer[dimension*index+i] = point[i];
    }


    typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;
    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
    viennagrid_int_t cell_count = viennagrid::cells(src_mesh).size();
    const int cell_vertex_count = viennagrid::boundary_elements<CellTag, viennagrid::vertex_tag>::num;

    error = viennagrid_create_element_buffers( dst_mesh, cell_count, cell_count*cell_vertex_count );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_create_element_buffers error");

    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;


    viennagrid_int_t * element_type_buffer;
    viennagrid_int_t * element_vertex_count_buffer;
    viennagrid_int_t * element_vertex_buffer;
    viennagrid_int_t * element_segment_buffer;

    error = viennagrid_get_element_type_buffer( dst_mesh, &element_type_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_type_buffer error");

    error = viennagrid_get_element_vertex_count_buffer( dst_mesh, &element_vertex_count_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_vertex_count_buffer error");

    error = viennagrid_get_element_vertex_buffer( dst_mesh, &element_vertex_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_vertex_buffer error");

    error = viennagrid_get_element_segment_buffer( dst_mesh, &element_segment_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_segment_buffer error");


    ConstCellRangeType cells(src_mesh);
    index = 0;
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit, ++index)
    {
      typedef typename viennagrid::result_of::const_vertex_range<CellType>::type ConstVertexOnCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexOnCellRangeType>::type ConstVertexOnCellRangeIterator;

      element_type_buffer[index] = result_of::viennagrid_llmesh_element_type<CellTag>::value;
      element_vertex_count_buffer[index] = cell_vertex_count;

      ConstVertexOnCellRangeType vertices_on_cell(*cit);
      int i = 0;
      for (ConstVertexOnCellRangeIterator vocit = vertices_on_cell.begin(); vocit != vertices_on_cell.end(); ++vocit, ++i)
      {
        element_vertex_buffer[ index*cell_vertex_count+i ] = vertex_index_map[ (*vocit).id() ];
      }
    }


    index = 0;
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit, ++index)
    {
      typedef typename viennagrid::result_of::segment_id_range<SegmentationT, CellType>::type SegmentIDRangeType;

      SegmentIDRangeType segment_ids = viennagrid::segment_ids(src_segmentation, *cit);
      if (segment_ids.size() == 1)
        element_segment_buffer[index] = *(segment_ids.begin());
    }
  }














  template<typename MeshT, typename SegmentationT>
  void copy( viennagrid_llmesh src_mesh, MeshT & dst_mesh, SegmentationT & dst_segmentation )
  {
    typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;
    const int dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;
    viennagrid_error_code_t error;

    viennagrid_int_t llmesh_dimension;
    error = viennagrid_get_dimension( src_mesh, &llmesh_dimension );
    if (dimension != llmesh_dimension)
      throw llmesh_exception("Dimension mismatch");

    viennagrid_int_t total_cell_count;
    viennagrid_int_t cell_count;

    const int cell_type = result_of::viennagrid_llmesh_element_type<CellTag>::value;

    error = viennagrid_get_element_count( src_mesh, &total_cell_count );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_count error");

    error = viennagrid_get_element_count_by_type( src_mesh, cell_type, &cell_count );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_count_by_type error");


    if (total_cell_count != cell_count)
      throw llmesh_exception("ViennaGrid llmesh includes element types not support by the ViennaGrid mesh");


    typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;
    viennagrid_int_t segment_count;
    error = viennagrid_get_segment_count( src_mesh, &segment_count );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_segment_count error");

    for (viennagrid_int_t i = 0; i < segment_count; ++i)
    {
//       SegmentHandleType segment = dst_segmentation.make_segment();

      char const * segment_name;
      error = viennagrid_get_segment_name( src_mesh, i, &segment_name );
      if (error != VIENNAGRID_SUCCESS)
        throw llmesh_exception("viennagrid_get_segment_name error");

//       if (segment_name)
//         segment.set_name( segment_name );
    }



    viennagrid_int_t vertex_count;
    error = viennagrid_get_vertex_count( src_mesh, &vertex_count );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_vertex_count error");


    viennagrid_numeric_t * vertex_buffer;
    error = viennagrid_get_vertex_buffer( src_mesh, &vertex_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_vertex_buffer error");

    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
    typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;

    std::vector<VertexHandleType> vertices(vertex_count);

    for (viennagrid_int_t i = 0; i < vertex_count; ++i)
    {
      PointType point;
      for (int j = 0; j < dimension; ++j)
        point[j] = vertex_buffer[dimension*i+j];

      vertices[i] = viennagrid::make_vertex( dst_mesh, point );
    }



    viennagrid_int_t * element_type_buffer;
    viennagrid_int_t * element_vertex_count_buffer;
    viennagrid_int_t * element_vertex_buffer;
    viennagrid_int_t * element_segment_buffer;

    error = viennagrid_get_element_type_buffer( src_mesh, &element_type_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_type_buffer error");

    error = viennagrid_get_element_vertex_count_buffer( src_mesh, &element_vertex_count_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_vertex_count_buffer error");

    error = viennagrid_get_element_vertex_buffer( src_mesh, &element_vertex_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_vertex_buffer error");

    error = viennagrid_get_element_segment_buffer( src_mesh, &element_segment_buffer );
    if (error != VIENNAGRID_SUCCESS)
      throw llmesh_exception("viennagrid_get_element_segment_buffer error");

    viennagrid_int_t offset = 0;
    for (viennagrid_int_t i = 0; i < cell_count; ++i)
    {
      viennagrid_int_t cell_vertex_count = element_vertex_count_buffer[i];

      std::vector<VertexHandleType> vertex_handles(cell_vertex_count);
      for (viennagrid_int_t j = 0; j != cell_vertex_count; ++j)
        vertex_handles[j] = vertices[ element_vertex_buffer[offset+j] ];

      offset += cell_vertex_count;

      CellHandleType cell_handle = viennagrid::make_cell( dst_mesh, vertex_handles.begin(), vertex_handles.end() );

      if (element_segment_buffer[i] >= segment_count)
        throw llmesh_exception("Invalid segment index");

      if (element_segment_buffer[i] >= 0)
        viennagrid::add( dst_segmentation[element_segment_buffer[i]], cell_handle );
    }
  }

}






int main()
{
  typedef viennagrid::triangular_2d_mesh                  MeshType;
  typedef viennagrid::result_of::segmentation<MeshType>::type SegmentationType;
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type SegmentHandleType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;

  MeshType mesh;
  SegmentationType segmentation(mesh);

  SegmentHandleType seg0 = segmentation.make_segment();
  SegmentHandleType seg1 = segmentation.make_segment();

  VertexHandleType vh0 = viennagrid::make_vertex(mesh, PointType(0,0)); // id = 0
  VertexHandleType vh1 = viennagrid::make_vertex(mesh, PointType(1,0)); // id = 1
  VertexHandleType vh2 = viennagrid::make_vertex(mesh, PointType(2,0));
  VertexHandleType vh3 = viennagrid::make_vertex(mesh, PointType(2,1));
  VertexHandleType vh4 = viennagrid::make_vertex(mesh, PointType(1,1));
  VertexHandleType vh5 = viennagrid::make_vertex(mesh, PointType(0,1)); // id = 5

  viennagrid::make_triangle(seg0, vh0, vh1, vh5); // creates an element with vertex handles
  viennagrid::make_triangle(seg0, vh1, vh4, vh5);  //use the shortcut function
  viennagrid::make_triangle(seg1, vh1, vh2, vh4); // Note that we create in 'seg1' now.
  viennagrid::make_triangle(seg1, vh2, vh3, vh4 );


  viennagrid_llmesh llmesh;
  viennagrid::copy( mesh, segmentation, llmesh );



  MeshType mesh2;
  SegmentationType segmentation2(mesh2);
  viennagrid::copy( llmesh, mesh2, segmentation2 );

  viennagrid::io::vtk_writer<MeshType, SegmentationType> writer;
  writer(mesh2, segmentation2, "test");


  viennagrid_delete_llmesh(&llmesh);



  return EXIT_SUCCESS;
}
