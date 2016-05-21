#ifndef VIENNAGRID_BACKEND_ALGORITHM_REFINE_HPP
#define VIENNAGRID_BACKEND_ALGORITHM_REFINE_HPP

#include "../common.hpp"

viennagrid_error viennagrid_mesh_element_create_with_regions(viennagrid_mesh mesh,
                                                             viennagrid_element_type element_type,
                                                             viennagrid_int vertex_count,
                                                             viennagrid_element_id * vertex_ids,
                                                             viennagrid_region * regions_begin,
                                                             viennagrid_region * regions_end,
                                                             viennagrid_element_id * element_id);

viennagrid_error edge_refine_triangle(viennagrid_mesh         mesh,
                                      viennagrid_bool       * edge_refinement_tags,
                                      viennagrid_element_id * vertex_on_edge_ids,
                                      viennagrid_element_id * old_to_new_vertex_map,
                                      viennagrid_element_id   cell_id,
                                      viennagrid_mesh         output_mesh);

viennagrid_error edge_refine_tetrahedron(viennagrid_mesh         mesh,
                                         viennagrid_bool       * edge_refinement_tags,
                                         viennagrid_element_id * vertex_on_edge_ids,
                                         viennagrid_element_id * old_to_new_vertex_map,
                                         viennagrid_element_id   cell_id,
                                         viennagrid_mesh         output_mesh);

#endif
