#include <cstring>

#include "viennagrid/viennagrid.h"
#include "mesh_hierarchy.hpp"
#include "mesh.hpp"
#include "plc.hpp"
#include "common.hpp"




const char * viennagrid_element_type_string(viennagrid_element_type element_type)
{
  switch (element_type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
      return "vertex";

    case VIENNAGRID_ELEMENT_TYPE_LINE:
      return "line";

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      return "triangle";
    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      return "quadrilateral";
    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      return "polygon";

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      return "tetrahedron";
    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      return "hexahedron";
    case VIENNAGRID_ELEMENT_TYPE_WEDGE:
      return "wedge";
    case VIENNAGRID_ELEMENT_TYPE_PYRAMID:
      return "pyramid";

    default:
      return 0;
  }
}



const char * viennagrid_error_string(viennagrid_error error)
{
  switch (error)
  {
    case VIENNAGRID_ERROR_UNSPECIFIED_ERROR:
      return "unspecified error";
    case VIENNAGRID_ERROR_OUT_OF_MEMORY:
      return "out of memory";
    case VIENNAGRID_ERROR_INVALID_ARGUMENTS:
      return "invalid arguments";
    case VIENNAGRID_ERROR_INVALID_MESH:
      return "invalid mesh";
    case VIENNAGRID_ERROR_INVALID_REGION:
      return "invalid region";
    case VIENNAGRID_ERROR_INVALID_CHILD_MESH_INDEX:
      return "invalid child mesh index";
    case VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION:
      return "invalid topological dimension";
    case VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE:
      return "invalid element type";
    case VIENNAGRID_ERROR_INVALID_ELEMENT_ID:
      return "invalid element ID";
    case VIENNAGRID_ERROR_INVALID_REGION_ID:
      return "invalid region ID";
    case VIENNAGRID_ERROR_INVALID_PLC:
      return "invalid PLC";
    case VIENNAGRID_ERROR_INVALID_FACET_HOLE_POINT_INDEX:
      return "invalid facet hole point index";
    case VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD:
      return "invalid quantity field";
    case VIENNAGRID_ERROR_INVALID_STORAGE_LAYOUT:
      return "invalid storage layout";
    case VIENNAGRID_ERROR_INVALID_QUANTITLY_VALUE_TYPE:
      return "invalid quantity value type";
    case VIENNAGRID_ERROR_INVALID_MESH_IO:
      return "invalid mesh IO";
    case VIENNAGRID_ERROR_INVALID_IO_MESH_INDEX:
      return "invalid mesh IO mesh index";
    case VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD_NAME:
      return "invalid quantity field name";
    case VIENNAGRID_ERROR_QUANTITY_FIELD_NAME_ALREADY_IN_USE:
      return "quantity field name already in use";
    case VIENNAGRID_ERROR_TOPOLOGICAL_DIMENSION_MISMATCH:
      return "topological dimension mismatch";
    case VIENNAGRID_ERROR_UNKNOWN_PROPERTY:
      return "unknown property";
    case VIENNAGRID_ERROR_UNSUPPORTED_BOUNDARY_LAYOUT:
      return "unsupported boundary layout";
    case VIENNAGRID_ERROR_MESH_IS_NOT_ROOT:
      return "mesh is not root";
    case VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES:
      return "mesh has child meshes";
    case VIENNAGRID_ERROR_ELEMENT_ALREADY_PRESENT:
      return "element already present";
    case VIENNAGRID_ERROR_DESERIALIZE_MAGIC_VALUE_MISMATCH:
      return "deserialize magic value mismatch";
    case VIENNAGRID_ERROR_DESERIALIZE_VERSION_MISMATCH:
      return "deserialize version mismatch";
    case VIENNAGRID_ERROR_DESERIALIZE_ARRAY_SIZE_MISMATCH:
      return "deserialize array size mismatch";
    case VIENNAGRID_ERROR_PLC_MESH_TYPE_NOT_SUPPORTED_FOR_CONVERSION:
      return "PLC mesh type not supported for conversion";
    case VIENNAGRID_ERROR_CANNOT_OPEN_FILE:
      return "cannot open file";
    case VIENNAGRID_ERROR_FILE_EMPTY:
      return "file is empty";
    case VIENNAGRID_ERROR_EOF_ENCOUNTERED:
      return "EOF encountered";
    case VIENNAGRID_ERROR_EOF_WHILE_READING_VERTICES:
      return "EOF while reading vertices";
    case VIENNAGRID_ERROR_VERTEX_DIMENSION_MISMATCH:
      return "vertex dimension mismatch";
    case VIENNAGRID_ERROR_EOF_WHILE_READING_CELL_COUNT:
      return "EOF while reading cell count";
    case VIENNAGRID_ERROR_EOF_WHILE_READING_CELLS:
      return "EOF while reading cells";
    case VIENNAGRID_ERROR_UNKNOWN_FILETYPE:
      return "unknown filetype";
    case VIENNAGRID_ERROR_INVALID_VERTEX_COUNT:
      return "invalid vertex count";
    case VIENNAGRID_ERROR_INVALID_ATTRIBUTE_COUNT:
      return "invalid attribute count";
    case VIENNAGRID_ERROR_INVALID_BOUNDARY_MARKER_COUNT:
      return "invalid boundary marker count";
    case VIENNAGRID_ERROR_INVALID_FACET_COUNT:
      return "invalid facet count";
    case VIENNAGRID_ERROR_INVALID_FACET_POLYGON_COUNT:
      return "invalid facet polygon count";
    case VIENNAGRID_ERROR_INVALID_FACET_HOLE_POINT_COUNT:
      return "invalid facet hole point count";
    case VIENNAGRID_ERROR_INVALID_POLYGON_VERTEX_COUNT:
      return "invalid polygon vertex count";
    case VIENNAGRID_ERROR_INVALID_VERTEX_ID:
      return "invalid vertex ID";
    case VIENNAGRID_ERROR_INVALID_HOLE_POINT_COUNT:
      return "invalid hole point count";
    case VIENNAGRID_ERROR_INVALID_SEED_POINT_COUNT:
      return "invalid seed point count";
    case VIENNAGRID_ERROR_UNSUPPORTED_ELEMENT_TYPE:
      return "unsupported element type";
    case VIENNAGRID_ERROR_FILE_MALFORMED:
      return "file malformed";
    case VIENNAGRID_ERROR_NO_MESH:
      return "no mesh";
    case VIENNAGRID_ERROR_WRITE_ERROR:
      return "write error";
    case VIENNAGRID_ERROR_READ_ERROR:
      return "read error";
    case VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT:
      return "mesh has sparse boundary storage layout";
    case VIENNAGRID_ERROR_INVALID_GEOMETRIC_DIMENSION:
      return "invalid geometric dimension";
    case VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO:
      return "numeric close to zero";
    case VIENNAGRID_ERROR_GEOMETRIC_DIMENSION_MISMATCH:
      return "geometric dimension mismatch";
    case VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD_VALUE_TYPE:
      return "invalid quantity field value type";

    default:
      return 0;
  }
}





viennagrid_error viennagrid_new(viennagrid_int size, void ** ptr)
{
  if (ptr)
    *ptr = malloc(size);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_delete(void ** ptr)
{
  if (ptr)
  {
    free(*ptr);
    *ptr = NULL;
  }

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_mesh_create(viennagrid_mesh * mesh)
{
  if (mesh)
    *mesh = new viennagrid_mesh_();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_retain(viennagrid_mesh mesh)
{
  retain(mesh);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_release(viennagrid_mesh mesh)
{
  release(mesh);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_memory_optimize(viennagrid_mesh mesh)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root()) return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  if (mesh)
    mesh->mesh_hierarchy()->optimize_memory();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_memory_size(viennagrid_mesh mesh,
                                             long * size)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root()) return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  if (size)
    *size = mesh->mesh_hierarchy()->memory_size();

  return VIENNAGRID_SUCCESS;
}






viennagrid_error viennagrid_mesh_property_set(viennagrid_mesh mesh,
                                              viennagrid_int property,
                                              viennagrid_int value)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  switch (property)
  {
    case VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT:
    {
      if (!mesh->is_root()) return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
      mesh->mesh_hierarchy()->set_boundary_layout(value);
      break;
    }

    default:
      return VIENNAGRID_ERROR_UNKNOWN_PROPERTY;
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_property_get(viennagrid_mesh mesh,
                                              viennagrid_int property,
                                              viennagrid_int * value)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  switch (property)
  {
    case VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT:
      if (!mesh->is_root()) return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
      if (value)
        *value = mesh->mesh_hierarchy()->boundary_layout();
      break;

    default:
      return VIENNAGRID_ERROR_UNKNOWN_PROPERTY;
  }

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_cell_dimension_get(viennagrid_mesh mesh,
                                                    viennagrid_dimension * cell_dimension)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (cell_dimension)
    *cell_dimension = mesh->mesh_hierarchy()->cell_dimension();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_geometric_dimension_get(viennagrid_mesh mesh,
                                                         viennagrid_dimension * geometric_dimension)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (geometric_dimension)
    *geometric_dimension = mesh->mesh_hierarchy()->geometric_dimension();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_geometric_dimension_set(viennagrid_mesh mesh,
                                                         viennagrid_dimension geometric_dimension)
{
  if (!mesh)            return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root()) return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  mesh->mesh_hierarchy()->set_geometric_dimension(geometric_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_clear(viennagrid_mesh mesh)
{
  if (!mesh)            return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root()) return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  if (mesh)
    mesh->clear();

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_root_mesh_get(viennagrid_mesh mesh,
                                               viennagrid_mesh * root)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (root)
    *root = mesh->mesh_hierarchy()->root();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_is_root(viennagrid_mesh mesh,
                                         viennagrid_bool * value)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (value)
    *value = mesh->is_root() ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh mesh,
                                                viennagrid_int * children_count)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (children_count)
    *children_count = mesh->children_count();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_child_get(viennagrid_mesh mesh,
                                           viennagrid_int index,
                                           viennagrid_mesh * child)
{
  if (!mesh)                                            return VIENNAGRID_ERROR_INVALID_MESH;
  if ((index < 0) || (index >= mesh->children_count())) return VIENNAGRID_ERROR_INVALID_CHILD_MESH_INDEX;

  if (child)
    *child = mesh->get_child(index);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_parent_get(viennagrid_mesh mesh,
                                            viennagrid_mesh * parent)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (parent)
    *parent = mesh->parent();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_name_get(viennagrid_mesh mesh,
                                          const char ** mesh_name)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (mesh_name)
  {
    if (!mesh->name().empty())
      *mesh_name = mesh->name().c_str();
    else
      *mesh_name = NULL;
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_name_set(viennagrid_mesh mesh,
                                          const char * mesh_name)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (mesh_name)
    mesh->set_name(mesh_name);
  else
    mesh->set_name( std::string() );

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_element_count(viennagrid_mesh mesh,
                                               viennagrid_dimension topological_dimension,
                                               viennagrid_int * count)
{
  if (!mesh)                                                          return VIENNAGRID_ERROR_INVALID_MESH;
  if (!viennagrid_topological_dimension_valid(topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(topological_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (count)
    *count = mesh->element_count(topological_dimension);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_element_count_by_type(viennagrid_mesh mesh,
                                                       viennagrid_element_type element_type,
                                                       viennagrid_int * count)
{
  if (!mesh)                                                                         return VIENNAGRID_ERROR_INVALID_MESH;
  if (!viennagrid_element_type_valid(element_type))                                  return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
  if (!mesh->valid_sparse_dimension(viennagrid_topological_dimension(element_type))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (count)
    *count = mesh->element_count_by_type(element_type);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_vertex_create(viennagrid_mesh mesh,
                                               const viennagrid_numeric * coords,
                                               viennagrid_element_id * vertex_id)
{
  if (!mesh)                       return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())            return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if (mesh->children_count() != 0) return VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES;

  viennagrid_element_id vid = mesh->make_vertex(coords);
  mesh->add_element(vid);

  if (vertex_id)
    *vertex_id = vid;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_vertex_batch_create(viennagrid_mesh mesh,
                                                     viennagrid_int vertex_count,
                                                     viennagrid_numeric * vertex_coords,
                                                     viennagrid_element_id * first_id)
{
  if (!mesh)                       return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())            return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if (mesh->children_count() != 0) return VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES;
  if (vertex_count < 0)            return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
  if (vertex_count == 0)           return VIENNAGRID_SUCCESS;
  if (!vertex_coords)              return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  viennagrid_element_id first_vid = mesh->make_vertices( vertex_count, vertex_coords );
  mesh->add_elements( first_vid, vertex_count );

  if (first_id)
    *first_id = first_vid;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_vertex_coords_pointer(viennagrid_mesh mesh,
                                                       viennagrid_numeric ** coords)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (coords)
    *coords = mesh->mesh_hierarchy()->get_vertex_pointer();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_vertex_coords_get(viennagrid_mesh mesh,
                                                   viennagrid_element_id vertex_id,
                                                   viennagrid_numeric ** coords)
{
  if (!mesh)                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(vertex_id)) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  if (coords)
    *coords = mesh->mesh_hierarchy()->get_vertex(vertex_id);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_element_create(viennagrid_mesh mesh,
                                                viennagrid_element_type element_type,
                                                viennagrid_int vertex_count,
                                                viennagrid_int * vertex_ids,
                                                viennagrid_element_id * element_id)
{
  if (!mesh)                                       return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())                            return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if (mesh->children_count() != 0)                 return VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES;
  if(!viennagrid_element_type_valid(element_type)) return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
  if (vertex_count < 0)                            return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
  if (!vertex_ids)                                 return VIENNAGRID_ERROR_INVALID_ARGUMENTS;;

  std::pair<viennagrid_element_id, bool> tmp = mesh->mesh_hierarchy()->get_make_element(element_type, vertex_count, vertex_ids, mesh);
  if (!tmp.second)                                 return VIENNAGRID_ERROR_ELEMENT_ALREADY_PRESENT;

  mesh->add_element(tmp.first);

  if (element_id)
    *element_id = tmp.first;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_element_batch_create(viennagrid_mesh mesh,
                                                      viennagrid_int element_count,
                                                      viennagrid_element_type * element_types,
                                                      viennagrid_int * element_vertex_index_offsets,
                                                      viennagrid_int * element_vertex_indices,
                                                      viennagrid_region_id * region_ids,
                                                      viennagrid_element_id * first_id)
{
  if (!mesh)                         return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())              return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if (mesh->children_count() != 0)   return VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES;
  if (!element_types)                return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
  if (!element_vertex_index_offsets) return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
  if (!element_vertex_indices)       return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  viennagrid_dimension topologic_dimension = viennagrid_topological_dimension( element_types[0] );

  for (viennagrid_int i = 0; i != element_count; ++i)
  {
    if ( viennagrid_topological_dimension( element_types[i] ) != topologic_dimension )
      return VIENNAGRID_ERROR_TOPOLOGICAL_DIMENSION_MISMATCH;
  }

  viennagrid_element_id first_eid = mesh->mesh_hierarchy()->make_elements( element_count, element_types, element_vertex_index_offsets, element_vertex_indices, region_ids );
  mesh->add_elements( first_eid, element_count );

  if (first_id)
    *first_id = first_eid;

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_type_pointer(viennagrid_mesh mesh,
                                                 viennagrid_dimension topologic_dimension,
                                                 viennagrid_element_type ** element_type)
{
  if (!mesh)                                                        return VIENNAGRID_ERROR_INVALID_MESH;
  if (!viennagrid_topological_dimension_valid(topologic_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(topologic_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (element_type)
    *element_type = mesh->mesh_hierarchy()->element_buffer(topologic_dimension).element_types_pointer();

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_type_get(viennagrid_mesh mesh,
                                             viennagrid_element_id element_id,
                                             viennagrid_element_type * element_type)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (element_type)
    *element_type = mesh->element_type(element_id);

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_aux_set(viennagrid_mesh mesh,
                                            viennagrid_element_id element_id,
                                            void * aux)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  mesh->mesh_hierarchy()->element_buffer(TOPODIM(element_id)).set_aux(element_id, aux);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_aux_get(viennagrid_mesh mesh,
                                            viennagrid_element_id element_id,
                                            void ** aux)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (aux)
    *aux = mesh->mesh_hierarchy()->element_buffer(TOPODIM(element_id)).aux(element_id);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_boundary_element_pointers(viennagrid_mesh mesh,
                                                              viennagrid_dimension element_topological_dimension,
                                                              viennagrid_dimension boundary_topological_dimension,
                                                              viennagrid_int ** boundary_offsets,
                                                              viennagrid_element_id ** boundary_ids)
{
  if (!mesh)                                                                   return VIENNAGRID_ERROR_INVALID_MESH;
  if (!viennagrid_topological_dimension_valid(element_topological_dimension))  return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!viennagrid_topological_dimension_valid(boundary_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(element_topological_dimension))            return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;
  if (!mesh->valid_sparse_dimension(boundary_topological_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (boundary_offsets)
    *boundary_offsets = mesh->mesh_hierarchy()->element_buffer(element_topological_dimension).boundary_buffer(boundary_topological_dimension).offset_pointer();

  if (boundary_ids)
    *boundary_ids = mesh->mesh_hierarchy()->element_buffer(element_topological_dimension).boundary_buffer(boundary_topological_dimension).values_pointer();

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh mesh,
                                                      viennagrid_element_id element_id,
                                                      viennagrid_dimension boundary_topological_dimension,
                                                      viennagrid_element_id ** boundary_element_ids_begin,
                                                      viennagrid_element_id ** boundary_element_ids_end)
{
  if (!mesh)                                                                   return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                                     return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!viennagrid_topological_dimension_valid(boundary_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id)))                      return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;
  if (!mesh->valid_sparse_dimension(boundary_topological_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (boundary_element_ids_begin)
    *boundary_element_ids_begin = mesh->boundary_begin(element_id, boundary_topological_dimension);

  if (boundary_element_ids_end)
    *boundary_element_ids_end = mesh->boundary_end(element_id, boundary_topological_dimension);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                        viennagrid_element_id element_id,
                                                        viennagrid_dimension coboundary_topological_dimension,
                                                        viennagrid_element_id ** coboundary_element_ids_begin,
                                                        viennagrid_element_id ** coboundary_element_ids_end)
{
  if (!mesh)                                                                     return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                                       return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!viennagrid_topological_dimension_valid(coboundary_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id)))                        return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;
  if (!mesh->valid_sparse_dimension(coboundary_topological_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (coboundary_element_ids_begin)
    *coboundary_element_ids_begin = mesh->coboundary_begin(element_id, coboundary_topological_dimension);

  if (coboundary_element_ids_end)
    *coboundary_element_ids_end = mesh->coboundary_end(element_id, coboundary_topological_dimension);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                      viennagrid_element_id element_id,
                                                      viennagrid_dimension connector_topological_dimension,
                                                      viennagrid_dimension neighbor_topological_dimension,
                                                      viennagrid_element_id ** neighbor_element_ids_begin,
                                                      viennagrid_element_id ** neighbor_element_ids_end)
{
  if (!mesh)                                                                    return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                                      return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!viennagrid_topological_dimension_valid(connector_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!viennagrid_topological_dimension_valid(neighbor_topological_dimension))  return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id)))                       return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;
  if (!mesh->valid_sparse_dimension(connector_topological_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;
  if (!mesh->valid_sparse_dimension(neighbor_topological_dimension))            return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (neighbor_element_ids_begin)
    *neighbor_element_ids_begin = mesh->neighbor_begin(element_id, connector_topological_dimension, neighbor_topological_dimension);

  if (neighbor_element_ids_end)
    *neighbor_element_ids_end = mesh->neighbor_end(element_id, connector_topological_dimension, neighbor_topological_dimension);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_element_is_mesh_boundary(viennagrid_mesh mesh,
                                                     viennagrid_element_id element_id,
                                                     viennagrid_bool * result)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags();

  if (result)
    *result = mesh->is_boundary(element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_is_region_boundary(viennagrid_region region,
                                                       viennagrid_mesh mesh,
                                                       viennagrid_element_id element_id,
                                                       viennagrid_bool * result)
{
  if (!region)                                            return VIENNAGRID_ERROR_INVALID_REGION;
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags( const_cast<viennagrid_region>(region) );

  if (result)
    *result = region->is_boundary(element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_is_any_boundary(viennagrid_mesh mesh,
                                                    viennagrid_element_id element_id,
                                                    viennagrid_bool * result)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (result)
  {
    RETURN_ON_ERROR( viennagrid_element_is_mesh_boundary(mesh, element_id, result) );
    if (*result == VIENNAGRID_TRUE)
      return VIENNAGRID_SUCCESS;

    viennagrid_region_id * region_ids_begin;
    viennagrid_region_id * region_ids_end;
    RETURN_ON_ERROR( viennagrid_element_regions_get(mesh, element_id, &region_ids_begin, &region_ids_end) );
    for (viennagrid_region_id * rit = region_ids_begin; rit != region_ids_end; ++rit)
    {
      viennagrid_region region;
      RETURN_ON_ERROR( viennagrid_mesh_region_get(mesh, *rit, &region) );
      RETURN_ON_ERROR( viennagrid_element_is_region_boundary(region, mesh, element_id, result) );

      if (*result == VIENNAGRID_TRUE)
        return VIENNAGRID_SUCCESS;
    }

    *result = VIENNAGRID_FALSE;
  }

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_elements_get(viennagrid_mesh mesh,
                                              viennagrid_dimension element_topological_dimension,
                                              viennagrid_element_id ** element_ids_begin,
                                              viennagrid_element_id ** element_ids_end)
{
  if (!mesh)                                                                  return VIENNAGRID_ERROR_INVALID_MESH;
  if (!viennagrid_topological_dimension_valid(element_topological_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(element_topological_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (element_ids_begin)
    *element_ids_begin = mesh->elements_begin(element_topological_dimension);

  if (element_ids_end)
    *element_ids_end = mesh->elements_end(element_topological_dimension);

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_parent_pointer(viennagrid_mesh mesh,
                                                   viennagrid_dimension topologic_dimension,
                                                   viennagrid_element_id ** element_parent_id)
{
  if (!mesh)                                                        return VIENNAGRID_ERROR_INVALID_MESH;
  if (!viennagrid_topological_dimension_valid(topologic_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;
  if (!mesh->valid_sparse_dimension(topologic_dimension))           return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (element_parent_id)
    *element_parent_id = mesh->mesh_hierarchy()->element_buffer(topologic_dimension).parent_id_pointer();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_get(viennagrid_mesh mesh,
                                               viennagrid_element_id element_id,
                                               viennagrid_element_id * element_parent_id)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (element_parent_id)
    *element_parent_id = mesh->parent(element_id);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_mesh_region_create(viennagrid_mesh mesh,
                                               viennagrid_region * region)
{
  if (!mesh)                       return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())            return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if (mesh->children_count() != 0) return VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES;

  viennagrid_region tmp = mesh->mesh_hierarchy()->create_region();

  if (region)
    *region = tmp;

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_region_get_or_create(viennagrid_mesh mesh,
                                                      viennagrid_region_id region_id,
                                                      viennagrid_region * region)
{
  if (!mesh)                       return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())            return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if (mesh->children_count() != 0) return VIENNAGRID_ERROR_MESH_HAS_CHILD_MESHES;
  if (region_id < 0)               return VIENNAGRID_ERROR_INVALID_REGION_ID;

  viennagrid_region tmp = mesh->mesh_hierarchy()->get_or_create_region(region_id);;

  if (region)
    *region = tmp;

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_region_get(viennagrid_mesh mesh,
                                            viennagrid_region_id region_id,
                                            viennagrid_region * region)
{
  if (!mesh)         return VIENNAGRID_ERROR_INVALID_MESH;
  if (region_id < 0) return VIENNAGRID_ERROR_INVALID_REGION_ID;

  if (region)
  {
    viennagrid_region tmp = mesh->mesh_hierarchy()->get_region(region_id);

    if (tmp)
      *region = tmp;
    else
      return VIENNAGRID_ERROR_INVALID_REGION_ID;
  }

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_region_root_mesh_get(viennagrid_region region,
                                                 viennagrid_mesh * root)
{
  if (!region) return VIENNAGRID_ERROR_INVALID_REGION;

  if (root)
    *root = region->mesh_hierarchy()->root();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_region_name_get(viennagrid_region region,
                                            const char ** region_name)
{
  if (!region) return VIENNAGRID_ERROR_INVALID_REGION;

  if (region_name)
    *region_name = region->name().c_str();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_name_set(viennagrid_region region,
                                       const char * region_name)
{
  if (!region)      return VIENNAGRID_ERROR_INVALID_REGION;
  if (!region_name) return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  region->set_name( std::string(region_name) );

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_id_get(viennagrid_region region,
                                          viennagrid_region_id * id)
{
  if (!region) return VIENNAGRID_ERROR_INVALID_REGION;

  if (id)
    *id = region->id();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_regions_get(viennagrid_mesh mesh,
                                             viennagrid_region_id ** region_ids_begin,
                                             viennagrid_region_id ** region_ids_end)
{
  if (!mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  if (region_ids_begin)
    *region_ids_begin = mesh->mesh_hierarchy()->region_ids_begin();

  if (region_ids_end)
    *region_ids_end = mesh->mesh_hierarchy()->region_ids_end();

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_regions_get(viennagrid_mesh mesh,
                                                viennagrid_element_id element_id,
                                                viennagrid_region_id ** region_id_begin,
                                                viennagrid_region_id ** region_id_end)
{
  if (!mesh)                                              return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!mesh->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (region_id_begin)
    *region_id_begin = mesh->regions_begin(element_id);

  if (region_id_end)
    *region_id_end = mesh->regions_end(element_id);

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_region_element_add(viennagrid_region region,
                                               viennagrid_element_id element_id)
{
  if (!region)                                              return VIENNAGRID_ERROR_INVALID_REGION;
  if (!region->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!region->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  region->add_element(element_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_copy_region_information(viennagrid_mesh src_mesh,
                                                            viennagrid_element_id src_element_id,
                                                            viennagrid_mesh dst_mesh,
                                                            viennagrid_element_id dst_element_id)
{
  if (!src_mesh || !dst_mesh)                                     return VIENNAGRID_ERROR_INVALID_MESH;
  if (!src_mesh->element_id_valid(src_element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!dst_mesh->element_id_valid(dst_element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!src_mesh->valid_sparse_dimension(TOPODIM(src_element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;
  if (!dst_mesh->valid_sparse_dimension(TOPODIM(dst_element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if ((src_mesh == dst_mesh) && (src_element_id == dst_element_id)) return VIENNAGRID_SUCCESS;

  viennagrid_region_id * region_ids_begin;
  viennagrid_region_id * region_ids_end;
  RETURN_ON_ERROR( viennagrid_element_regions_get(src_mesh, src_element_id, &region_ids_begin, &region_ids_end) );

  for (viennagrid_region_id * rit = region_ids_begin; rit != region_ids_end; ++rit)
  {
    viennagrid_region region;
    RETURN_ON_ERROR( viennagrid_mesh_region_get_or_create(dst_mesh, *rit, &region) );
    RETURN_ON_ERROR( viennagrid_region_element_add(region, dst_element_id) );
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_contains_element(viennagrid_region region,
                                                    viennagrid_element_id element_id,
                                                    viennagrid_bool * value)
{
  if (!region)                                              return VIENNAGRID_ERROR_INVALID_REGION;
  if (!region->element_id_valid(element_id))                return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;
  if (!region->valid_sparse_dimension(TOPODIM(element_id))) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  if (value)
    *value = region->contains_element(element_id);

  return VIENNAGRID_SUCCESS;
}
































