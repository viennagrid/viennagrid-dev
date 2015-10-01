#ifndef VIENNAGRID_BACKEND_ALGORITHM_HPP
#define VIENNAGRID_BACKEND_ALGORITHM_HPP

#include "viennagrid/viennagrid.h"

typedef struct viennagrid_copy_map_ * viennagrid_copy_map;

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_create(viennagrid_mesh src_mesh,
                                                                      viennagrid_mesh dst_mesh,
                                                                      viennagrid_copy_map * copy_map);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_retain(viennagrid_copy_map copy_map);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_release(viennagrid_copy_map copy_map);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_copy_region_information_set(viennagrid_copy_map copy_map,
                                                                                           viennagrid_bool copy);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_copy_region_information_get(viennagrid_copy_map copy_map,
                                                                                           viennagrid_bool * copy);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_max_vertex_distance_set(viennagrid_copy_map copy_map,
                                                                                       viennagrid_numeric max_vertex_distance);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_max_vertex_distance_get(viennagrid_copy_map copy_map,
                                                                                       viennagrid_numeric * max_vertex_distance);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_copy_map_element_copy(viennagrid_copy_map copy_map,
                                                                            viennagrid_element_id source_element_id,
                                                                            viennagrid_element_id * destination_element_id);

#endif
