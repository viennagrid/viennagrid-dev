#include "mesh_hierarchy.hpp"
#include "mesh.hpp"
#include "region.hpp"
#include "dynamic_sizeof.hpp"



viennagrid_mesh_hierarchy_::viennagrid_mesh_hierarchy_(viennagrid_mesh root_in) : geometric_dimension_(0), cell_dimension_(VIENNAGRID_INVALID_TOPOLOGICAL_DIMENSION), root_(root_in), change_counter_(0)
{
  clear();
}

viennagrid_mesh_hierarchy_::~viennagrid_mesh_hierarchy_()
{
  for (std::vector<viennagrid_region>::iterator it = regions.begin(); it != regions.end(); ++it)
    delete *it;
}




viennagrid_error viennagrid_mesh_hierarchy_::set_boundary_layout(viennagrid_int boundary_layout_in)
{
  if ((boundary_layout_in != VIENNAGRID_BOUNDARY_LAYOUT_FULL) && (boundary_layout_in != VIENNAGRID_BOUNDARY_LAYOUT_SPARSE))
    return VIENNAGRID_ERROR_UNSUPPORTED_BOUNDARY_LAYOUT;

  if ((boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_SPARSE) &&
      (boundary_layout_in == VIENNAGRID_BOUNDARY_LAYOUT_FULL))
  {
    if (cell_dimension() != VIENNAGRID_INVALID_TOPOLOGICAL_DIMENSION)
    {
      viennagrid_element_buffer & cell_buffer = element_buffer( cell_dimension() );

      for (viennagrid_element_id cell_id = ID(cell_dimension(),0); cell_id != ID(cell_dimension(), cell_buffer.size()); ++cell_id)
      {
        cell_buffer.reserve_boundary(cell_id);
        cell_buffer.make_boundary(cell_id, root());

        viennagrid_region_id * region_ids_begin = cell_buffer.regions_begin(cell_id);
        viennagrid_region_id * region_ids_end = cell_buffer.regions_end(cell_id);

        for (viennagrid_region_id * region_id_it = region_ids_begin; region_id_it != region_ids_end; ++region_id_it)
        {
          for (viennagrid_dimension boundary_dim = 1; boundary_dim != cell_dimension(); ++boundary_dim)
          {
            viennagrid_element_id * boundary_begin = cell_buffer.boundary_begin(cell_id, boundary_dim);
            viennagrid_element_id * boundary_end = cell_buffer.boundary_end(cell_id, boundary_dim);

            for (viennagrid_element_id * boundary_it = boundary_begin; boundary_it != boundary_end; ++boundary_it)
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
    if (cell_dimension() != VIENNAGRID_INVALID_TOPOLOGICAL_DIMENSION)
    {
      for (viennagrid_dimension i = 1; i != cell_dimension(); ++i)
      {
        element_buffer(i).clear(this);
      }

      element_buffer( cell_dimension() ).clear_boundary();
    }

    boundary_layout_ = boundary_layout_in;
  }

  return VIENNAGRID_SUCCESS;
}





viennagrid_element_id viennagrid_mesh_hierarchy_::make_vertex(const viennagrid_numeric * coords)
{
  viennagrid_element_id id = element_buffer(0).make_element( VIENNAGRID_ELEMENT_TYPE_VERTEX, 0, 0, false);

  viennagrid_int prev_size = vertex_buffer.size();
  vertex_buffer.resize( vertex_buffer.size() + geometric_dimension() );

  if (coords)
    std::copy( coords, coords+geometric_dimension(), &vertex_buffer[0] + prev_size );

  return id;
}

viennagrid_element_id viennagrid_mesh_hierarchy_::make_vertices(viennagrid_int vertex_count, const viennagrid_numeric * coords)
{
  viennagrid_element_id first_id = element_buffer(0).make_vertices(vertex_count);

  viennagrid_int prev_size = vertex_buffer.size();
  vertex_buffer.resize( vertex_buffer.size() + geometric_dimension()*vertex_count );

  if (coords)
    std::copy( coords, coords+geometric_dimension()*vertex_count, &vertex_buffer[0] + prev_size );

  return first_id;
}

viennagrid_element_id viennagrid_mesh_hierarchy_::make_elements(viennagrid_int element_count_,
                                                                viennagrid_element_type * element_types_,
                                                                viennagrid_int * element_vertex_index_offsets_,
                                                                viennagrid_element_id * element_vertex_indices_,
                                                                viennagrid_region_id * region_ids)
{
  viennagrid_dimension topologic_dimension = viennagrid_topological_dimension( element_types_[0] );

  increment_change_counter();
  viennagrid_element_id start_index = element_buffer(topologic_dimension).make_elements( element_count_, element_types_, element_vertex_index_offsets_, element_vertex_indices_ );

  update_cell_dimension(topologic_dimension);

  if (region_ids)
  {
    for (viennagrid_int i = 0; i != element_count_; ++i)
      get_region(region_ids[i])->add_element(start_index+i);

    optimize_memory();
  }

  return start_index;
}



std::pair<viennagrid_element_id, bool> viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_type element_type,
                                                                                    viennagrid_int vertex_count,
                                                                                    viennagrid_element_id * vertex_ids,
                                                                                    viennagrid_mesh mesh,
                                                                                    bool make_boundary)
{
  viennagrid_dimension element_topologic_dimension = viennagrid_topological_dimension(element_type);
  viennagrid_element_id id = element_buffer(element_topologic_dimension).get_element(vertex_ids, vertex_count, element_type);
  if (id != -1)
    return std::make_pair(id, false);

  increment_change_counter();
  id = element_buffer(element_topologic_dimension).make_element(element_type, vertex_ids, vertex_count, full_boundary_layout());

  update_cell_dimension(element_topologic_dimension);

  if (mesh)
    mesh->add_element(id);

  if ( full_boundary_layout() && make_boundary )
  {
    element_buffer(element_topologic_dimension).make_boundary(id, mesh);
  }

  return std::make_pair(id, true);
}















viennagrid_region viennagrid_mesh_hierarchy_::get_or_create_region(viennagrid_region_id region_id)
{
  if (region_id == VIENNAGRID_INVALID_REGION_ID)
    return NULL;

  viennagrid_region tmp = get_region(region_id);
  if (tmp)
    return tmp;

  region_ids.push_back(region_id);
  std::sort( region_ids.begin(), region_ids.end() );
  regions.push_back( new viennagrid_region_(region_id, this) );

  if (region_id >= viennagrid_region_id(region_id_mapping.size()))
    region_id_mapping.resize( region_id+1, VIENNAGRID_INVALID_REGION_ID );

  region_id_mapping[region_id] = regions.size()-1;

  std::stringstream ss;
  ss << (int)region_id;
  regions.back()->set_name(ss.str());

  return regions.back();
}




void viennagrid_mesh_hierarchy_::clear()
{
  boundary_layout_ = VIENNAGRID_BOUNDARY_LAYOUT_FULL;

  for (int i = 0; i != VIENNAGRID_TOPOLOGICAL_DIMENSION_END; ++i)
  {
    element_buffer(i).clear(this);
    element_buffer(i).set_topological_dimension(i);
  }

  geometric_dimension_ = 0;
  cell_dimension_ = VIENNAGRID_INVALID_TOPOLOGICAL_DIMENSION;

  meshes_.clear();
  meshes_.push_back(root());

  vertex_buffer.clear();

  for (std::vector<viennagrid_region>::iterator it = regions.begin(); it != regions.end(); ++it)
    delete *it;
  regions.clear();
  region_ids.clear();
  region_id_mapping.clear();

  change_counter_ = 0;
}



void viennagrid_mesh_hierarchy_::optimize_memory()
{
  for (int i = 0; i != VIENNAGRID_TOPOLOGICAL_DIMENSION_END; ++i)
    element_buffers[i].optimize_memory();

  for (std::size_t i = 0; i != meshes_.size(); ++i)
    meshes_[i]->optimize_memory();

  shrink_to_fit( vertex_buffer );
}




long viennagrid_mesh_hierarchy_::memory_size() const
{
  return viennautils::total_sizeof(*this);
}



