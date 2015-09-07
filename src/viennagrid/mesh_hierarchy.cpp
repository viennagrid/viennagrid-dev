#include "mesh_hierarchy.hpp"

void viennagrid_element_buffer::reserve_boundary(viennagrid_int element_id)
{
  viennagrid_element_type type = element_type(element_id);

  switch (type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      boundary_ids[1].push_back(3);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
    {
      boundary_ids[1].push_back(4);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      boundary_ids[1].push_back(6);
      boundary_ids[2].push_back(4);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      boundary_ids[1].push_back(12);
      boundary_ids[2].push_back(6);
      break;
    }

    default:
      assert(false);
  }
}


void viennagrid_element_buffer::make_boundary(viennagrid_int element_id, viennagrid_mesh mesh)
{
  viennagrid_element_type type = element_type(element_id);

  viennagrid_int * vertex_ids = boundary_ids[0].begin(element_id);

  switch (type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      assert(boundary_ids[0].end(element_id) - vertex_ids == 3);
      viennagrid_int * line_ids = boundary_begin(1, element_id);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 1, 2, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
    {
      assert(boundary_ids[0].end(element_id) - vertex_ids == 4);
      viennagrid_int * line_ids = boundary_begin(1, element_id);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ids[3] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      assert(boundary_ids[0].end(element_id) - vertex_ids == 4);
      viennagrid_int * line_ids = boundary_begin(1, element_id);
      viennagrid_int * triangle_ids = boundary_begin(2, element_id);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 0, 3, mesh);
      line_ids[3] = mesh_hierarchy->get_make_line(vertex_ids, 1, 2, mesh);
      line_ids[4] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ids[5] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);

      triangle_ids[0] = mesh_hierarchy->get_make_triangle(vertex_ids, 0, 1, 2, line_ids, 0, 1, 3, mesh);
      triangle_ids[1] = mesh_hierarchy->get_make_triangle(vertex_ids, 0, 1, 3, line_ids, 0, 2, 4, mesh);
      triangle_ids[2] = mesh_hierarchy->get_make_triangle(vertex_ids, 0, 2, 3, line_ids, 1, 3, 5, mesh);
      triangle_ids[3] = mesh_hierarchy->get_make_triangle(vertex_ids, 1, 2, 3, line_ids, 3, 4, 5, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      assert(boundary_ids[0].end(element_id) - vertex_ids == 8);
      viennagrid_int * line_ptr = boundary_begin(1, element_id);
      viennagrid_int * quad_ptr = boundary_begin(2, element_id);


      line_ptr[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ptr[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ptr[2] = mesh_hierarchy->get_make_line(vertex_ids, 0, 4, mesh);
      line_ptr[3] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ptr[4] = mesh_hierarchy->get_make_line(vertex_ids, 1, 5, mesh);
      line_ptr[5] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);
      line_ptr[6] = mesh_hierarchy->get_make_line(vertex_ids, 2, 5, mesh);
      line_ptr[7] = mesh_hierarchy->get_make_line(vertex_ids, 3, 7, mesh);
      line_ptr[8] = mesh_hierarchy->get_make_line(vertex_ids, 4, 5, mesh);
      line_ptr[9] = mesh_hierarchy->get_make_line(vertex_ids, 4, 6, mesh);
      line_ptr[10] = mesh_hierarchy->get_make_line(vertex_ids, 5, 7, mesh);
      line_ptr[11] = mesh_hierarchy->get_make_line(vertex_ids, 6, 7, mesh);

      quad_ptr[0] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 0, 1, 2, 3, mesh).first;
      quad_ptr[1] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 0, 1, 4, 5, mesh).first;
      quad_ptr[2] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 0, 2, 4, 6, mesh).first;
      quad_ptr[3] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 1, 3, 5, 7, mesh).first;
      quad_ptr[4] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 2, 3, 6, 7, mesh).first;
      quad_ptr[5] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 4, 5, 6, 7, mesh).first;

      break;
    }

    default:
      assert(false);
  }
}












viennagrid_int viennagrid_element_buffer::make_element(viennagrid_element_type element_type,
                                                       viennagrid_int * vertex_ids,
                                                       viennagrid_int vertex_count,
                                                       bool reserve_boundary)
{
  viennagrid_int id = size();
  ++element_count;

  if (topologic_dimension != 0)
    element_types.push_back(element_type);

  viennagrid_int * ptr = 0;

  switch (element_type)
  {
    // topo dim == 0
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
      return id;

    // topo dim == 1
    case VIENNAGRID_ELEMENT_TYPE_LINE:

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      assert((vertex_count == -1) || (vertex_count == viennagrid_boundary_element_count(element_type, 0)));
      ptr = boundary_buffer(0).push_back( vertex_count );
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
    {
      ptr = boundary_buffer(0).push_back(vertex_count);
      break;
    }

    default:
      assert(false);
  }


  if (reserve_boundary)
    this->reserve_boundary(id);

  std::copy( vertex_ids, vertex_ids+vertex_count, ptr );


  viennagrid_int * min_vertex_id = std::min_element(vertex_ids, vertex_ids+vertex_count);
  assert( min_vertex_id != vertex_ids+vertex_count );

  if (*min_vertex_id >= (viennagrid_int)element_map.size())
    element_map.resize(*min_vertex_id+1);

  element_map[*min_vertex_id].push_back(id);


  return id;
}


viennagrid_int viennagrid_element_buffer::make_elements(viennagrid_int element_count_,
                                                        viennagrid_element_type * element_types_,
                                                        viennagrid_int * element_vertex_index_offsets_,
                                                        viennagrid_int * element_vertex_indices_)
{
  viennagrid_int first_id = size();
  element_count += element_count_;

  element_types.resize( element_count_ );
  std::copy( element_types_, element_types_+element_count_, element_types.begin()+first_id );

  boundary_buffer(0).push_back(element_count_, element_vertex_index_offsets_, element_vertex_indices_);

  for (viennagrid_int i = 0; i != element_count_; ++i)
  {
    viennagrid_int * min_vertex_id = std::min_element(element_vertex_indices_ + element_vertex_index_offsets_[i],
                                                      element_vertex_indices_ + element_vertex_index_offsets_[i+1]);
    assert( *min_vertex_id != element_vertex_index_offsets_[i+1] );

    if ( *min_vertex_id >= (viennagrid_int)element_map.size() )
      element_map.resize(*min_vertex_id+1);

    element_map[*min_vertex_id].push_back(first_id+i);
  }

  return first_id;
}


viennagrid_int viennagrid_element_buffer::make_vertices(viennagrid_int vertex_count)
{
  assert( topologic_dimension == 0 );
  viennagrid_int first_id = size();
  element_count += vertex_count;
  return first_id;
}






viennagrid_error viennagrid_mesh_hierarchy_::set_boundary_layout(viennagrid_int boundary_layout_in)
{
  if ((boundary_layout_in != VIENNAGRID_BOUNDARY_LAYOUT_FULL) && (boundary_layout_in != VIENNAGRID_BOUNDARY_LAYOUT_SPARSE))
    return VIENNAGRID_ERROR_UNSUPPORTED_BOUNDARY_LAYOUT;

  if ((boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_SPARSE) &&
      (boundary_layout_in == VIENNAGRID_BOUNDARY_LAYOUT_FULL))
  {
//     std::cout << "SWITCHED TO FULL LAYOUT" << std::endl;

    if (cell_dimension() != VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION)
    {
      viennagrid_element_buffer & cell_buffer = element_buffer( cell_dimension() );

      for (viennagrid_int cell_id = 0; cell_id != cell_buffer.size(); ++cell_id)
      {
        cell_buffer.reserve_boundary(cell_id);
        cell_buffer.make_boundary(cell_id, root());

        viennagrid_region_id * region_ids_begin = cell_buffer.regions_begin(cell_id);
        viennagrid_region_id * region_ids_end = cell_buffer.regions_end(cell_id);

        for (viennagrid_region_id * region_id_it = region_ids_begin; region_id_it != region_ids_end; ++region_id_it)
        {
          for (viennagrid_dimension boundary_dim = 1; boundary_dim != cell_dimension(); ++boundary_dim)
          {
            viennagrid_int * boundary_begin = cell_buffer.boundary_begin(boundary_dim, cell_id);
            viennagrid_int * boundary_end = cell_buffer.boundary_end(boundary_dim, cell_id);

            for (viennagrid_int * boundary_it = boundary_begin; boundary_it != boundary_end; ++boundary_it)
              element_buffer(boundary_dim).add_to_region(*boundary_it, *region_id_it);
          }
        }
      }

      boundary_layout_ = boundary_layout_in;
    }
  }


  // switching from full layout to sparse, only allowed when only one mesh (root mesh) is in the hierarchy
  if ((boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_FULL) &&
      (boundary_layout_in == VIENNAGRID_BOUNDARY_LAYOUT_SPARSE) &&
      (mesh_count() == 1))
  {
    if (cell_dimension() != VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION)
    {
      for (viennagrid_dimension i = 1; i != cell_dimension(); ++i)
      {
        element_buffer(i).clear(this);
      }

      element_buffer( cell_dimension() ).clear_boundary();
    }

//     std::cout << "Switching boundary layout of " << this << ": FULL -> SPARSE" << std::endl;

    boundary_layout_ = boundary_layout_in;
  }

  return VIENNAGRID_SUCCESS;
}





viennagrid_int viennagrid_mesh_hierarchy_::make_vertex(const viennagrid_numeric * coords)
{
  viennagrid_int id = element_buffer(0).make_element( VIENNAGRID_ELEMENT_TYPE_VERTEX, 0, 0, false);

  viennagrid_int prev_size = vertex_buffer.size();
  vertex_buffer.resize( vertex_buffer.size() + geometric_dimension() );

  if (coords)
    std::copy( coords, coords+geometric_dimension(), &vertex_buffer[0] + prev_size );

  return id;
}

viennagrid_int viennagrid_mesh_hierarchy_::make_vertices(const viennagrid_numeric * coords, viennagrid_int vertex_count)
{
  viennagrid_int first_id = element_buffer(0).make_vertices(vertex_count);

  viennagrid_int prev_size = vertex_buffer.size();
  vertex_buffer.resize( vertex_buffer.size() + geometric_dimension()*vertex_count );

  if (coords)
    std::copy( coords, coords+geometric_dimension()*vertex_count, &vertex_buffer[0] + prev_size );

  return first_id;
}

viennagrid_int viennagrid_mesh_hierarchy_::make_elements(viennagrid_int element_count_,
                                                         viennagrid_element_type * element_types_,
                                                         viennagrid_int * element_vertex_index_offsets_,
                                                         viennagrid_int * element_vertex_indices_,
                                                         viennagrid_region_id * region_ids)
{
  viennagrid_dimension topologic_dimension = viennagrid_topological_dimension( element_types_[0] );

  increment_change_counter();
  viennagrid_int start_id = element_buffer(topologic_dimension).make_elements( element_count_, element_types_, element_vertex_index_offsets_, element_vertex_indices_ );

  update_cell_dimension(topologic_dimension);
  for (int i = 0; i != element_count_; ++i)
   ++element_counts[ +element_types_[i] ];

  if (region_ids)
  {
    for (viennagrid_int i = 0; i != element_count_; ++i)
    {
      viennagrid_element_add_to_region( this, topologic_dimension, start_id+i, region_ids[i] );
    }
    optimize_memory();
  }


  return start_id;
}



std::pair<viennagrid_int, bool> viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_type element_type,
                                                                             viennagrid_int * vertex_ids,
                                                                             viennagrid_int vertex_count,
                                                                             viennagrid_mesh mesh,
                                                                             bool make_boundary)
{
  viennagrid_dimension element_topologic_dimension = viennagrid_topological_dimension(element_type);
  viennagrid_int id = element_buffer(element_topologic_dimension).get_element(vertex_ids, vertex_count, element_type);
  if (id != -1)
    return std::make_pair(id, false);

  increment_change_counter();
  id = element_buffer(element_topologic_dimension).make_element(element_type, vertex_ids, vertex_count, full_boundary_layout());

  update_cell_dimension(element_topologic_dimension);
  ++element_counts[+element_type];

  if (mesh)
    mesh->add_element(element_topologic_dimension, id);

  if ( full_boundary_layout() && make_boundary )
  {
    element_buffer(element_topologic_dimension).make_boundary(id, mesh);
  }

  return std::make_pair(id, true);
}












namespace viennautils
{

  namespace detail
  {
    template<typename IndexT, typename T, typename PointerT, typename ConstPointerT>
    struct dynamic_sizeof_impl< dense_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> >
    {
      static long size(dense_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> const & dpm)
      {
        return dynamic_sizeof(dpm.get_values()) + dynamic_sizeof(dpm.get_offsets());
      }
    };




    template<typename IndexT, typename T, typename PointerT, typename ConstPointerT>
    struct dynamic_sizeof_impl< sparse_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> >
    {
      static long size(sparse_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> const & spm)
      {
        return dynamic_sizeof(spm.get_values()) + dynamic_sizeof(spm.get_offsets());
      }
    };







    template<>
    struct dynamic_sizeof_impl<element_key>
    {
      static long size(element_key const & ek)
      {
        return dynamic_sizeof(ek.vertex_ids);
      }
    };


    template<>
    struct dynamic_sizeof_impl<viennagrid_element_buffer>
    {
      static long size(viennagrid_element_buffer const & eb)
      {
        long size = 0;

//         std::cout << "  eb " << (int)eb.topologic_dimension << std::endl;

        size += dynamic_sizeof( eb.topologic_dimension );

        size += dynamic_sizeof( eb.element_types );
//         std::cout << "    [element_types] " << dynamic_sizeof( eb.element_types ) << std::endl;

        size += dynamic_sizeof( eb.parents );
//         std::cout << "    [parents] " << dynamic_sizeof( eb.parents ) << std::endl;

        size += dynamic_sizeof( eb.region_buffer );
//         std::cout << "    [region_buffer] " << dynamic_sizeof( eb.region_buffer ) << std::endl;
        size += dynamic_sizeof( eb.packed_region_buffer );
//         std::cout << "    [packed_region_buffer] " << dynamic_sizeof( eb.packed_region_buffer ) << std::endl;



        size += dynamic_sizeof( eb.boundary_ids );
//         std::cout << "    [boundary_ids size() = " << eb.boundary_ids.size() <<  "] " << dynamic_sizeof( eb.boundary_ids ) << std::endl;
        for (std::size_t i = 0; i != eb.boundary_ids.size(); ++i)
        {
//           std::cout << "      [boundary_ids[" << i << "] ] " << dynamic_sizeof( eb.boundary_ids[i] ) << "  " <<
//               eb.boundary_ids[i].get_values().capacity() << " " << eb.boundary_ids[i].get_offsets().capacity() << std::endl;
        }

        size += dynamic_sizeof( eb.element_map );
//         std::cout << "    [element_map] " << dynamic_sizeof( eb.element_map ) << std::endl;

        size += dynamic_sizeof( eb.mesh_hierarchy );

        return size;
      }
    };



    template<>
    struct dynamic_sizeof_impl<viennagrid_element_handle_buffer>
    {
      static long size(viennagrid_element_handle_buffer const & ehb)
      {
        long size = 0;

        size += dynamic_sizeof( ehb.ids_ );
//         std::cout << "    [ids_] " << dynamic_sizeof( ehb.ids_ ) << std::endl;

        size += dynamic_sizeof( ehb.coboundary_ids );
//         std::cout << "    [coboundary_ids] " << dynamic_sizeof( ehb.coboundary_ids ) << std::endl;

        size += dynamic_sizeof( ehb.neighbor_ids );
//         std::cout << "    [neighbor_ids] " << dynamic_sizeof( ehb.neighbor_ids ) << std::endl;

        return size;
      }
    };



    template<typename T, typename SizeT>
    struct dynamic_sizeof_impl< short_vector<T, SizeT> >
    {
      static long size(short_vector<T, SizeT> const & sv)
      {
        long size = sv.size()*sizeof(T);
        for (typename short_vector<T, SizeT>::size_type i = 0; i != sv.size(); ++i)
          size += dynamic_sizeof(sv[i]);
        return size;
      }
    };



    template<>
    struct dynamic_sizeof_impl<viennagrid_element_children_>
    {
      static long size(viennagrid_element_children_ const & ec)
      {
        long size = 0;
        for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
          size += dynamic_sizeof( ec.get_children_ids_buffer(i) );
        return size;
      }
    };

    template<>
    struct dynamic_sizeof_impl<viennagrid_mesh_>
    {
      static long size(viennagrid_mesh_ const & m)
      {
        long size = 0;

        size += dynamic_sizeof( m.hierarchy_ );
        size += dynamic_sizeof( m.parent_ );
        size += dynamic_sizeof( m.children );

        size += dynamic_sizeof( m.element_children );
//         std::cout << "  [element_children] " << dynamic_sizeof( m.element_children ) << std::endl;

        size += dynamic_sizeof( m.mesh_children_map );
        size += dynamic_sizeof( m.name_ );
        size += dynamic_sizeof( m.element_handle_buffers );
        size += dynamic_sizeof( m.boundary_elements_ );
        size += dynamic_sizeof( m.boundary_elements_change_counter );

        return size;
      }
    };

    template<>
    struct dynamic_sizeof_impl<viennagrid_region_>
    {
      static long size(viennagrid_region_ const & r)
      {
        long size = 0;

        size += dynamic_sizeof( r.name_ );
        size += dynamic_sizeof( r.id_ );
        size += dynamic_sizeof( r.boundary_elements_ );
        size += dynamic_sizeof( r.boundary_elements_change_counter );
        size += dynamic_sizeof( r.hierarchy_ );

        return size;
      }
    };

    template<>
    struct dynamic_sizeof_impl<viennagrid_mesh_hierarchy_>
    {
      static long size(viennagrid_mesh_hierarchy_ const & mh)
      {
        long size = 0;

//         std::cout << "sizeof(short_vector<viennagrid_region, viennagrid_region_id>) = " << sizeof(short_vector<viennagrid_region, viennagrid_region_id>) << std::endl;



        size += dynamic_sizeof( mh.boundary_layout_ );

        long element_buffers_size = dynamic_sizeof( mh.element_buffers );
        size += element_buffers_size;
//         std::cout << "[element_buffers] " << element_buffers_size << std::endl;

        size += dynamic_sizeof( mh.element_counts );
        size += dynamic_sizeof( mh.geometric_dimension_ );
        size += dynamic_sizeof( mh.cell_dimension_ );

        size += dynamic_sizeof( mh.meshes_ );
        long mesh_size = 0;
        for (std::size_t i = 0; i != mh.meshes_.size(); ++i)
          mesh_size += dynamic_sizeof( *(mh.meshes_[i]) );
        size += mesh_size;
//         std::cout << "[meshes] " << mesh_size << std::endl;

        size += dynamic_sizeof( mh.root_ );

        long vertex_buffer_size = dynamic_sizeof( mh.vertex_buffer );
        size += vertex_buffer_size;
//         std::cout << "[vertex_buffer] " << vertex_buffer_size << std::endl;

        size += dynamic_sizeof( mh.regions );
        long region_size = 0;
        for (std::size_t i = 0; i != mh.regions.size(); ++i)
          region_size += dynamic_sizeof( *(mh.regions[i]) );
        size += region_size;
//         std::cout << "[regions] " << region_size << std::endl;


//         size += dynamic_sizeof( mh.highest_region_id );
        size += dynamic_sizeof( mh.region_id_mapping );
        size += dynamic_sizeof( mh.change_counter_ );
        size += dynamic_sizeof( mh.retain_release_count );
        size += dynamic_sizeof( mh.reference_counter );

        return size;
      }
    };

  }

}



long viennagrid_mesh_hierarchy_::memory_size() const
{
  return viennautils::total_sizeof(*this);
}



