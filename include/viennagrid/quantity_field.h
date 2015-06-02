#ifndef QUANTITY_FIELD_H
#define QUANTITY_FIELD_H

#include "viennagrid/forwards.h"


#define QUANTITY_FIELD_STORAGE_DENSE 0
#define QUANTITY_FIELD_STORAGE_SPARSE 1

typedef struct viennagrid_quantity_field_t * viennagrid_quantity_field;


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_make(viennagrid_quantity_field * quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                                          viennagrid_dimension topologic_dimension,
                                                                          viennagrid_dimension values_dimension,
                                                                          viennagrid_int storage_layout);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                                                             viennagrid_dimension * topologic_dimension);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_storage_layout(viennagrid_quantity_field quantity_field,
                                                                                        viennagrid_int * storage_layout);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_values_dimension(viennagrid_quantity_field quantity_field,
                                                                                          viennagrid_dimension * values_dimension);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_set_name(viennagrid_quantity_field quantity_field,
                                                                              const char * name);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_name(viennagrid_quantity_field quantity_field,
                                                                              const char ** name);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_set_unit(viennagrid_quantity_field quantity_field,
                                                                              const char * unit);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_unit(viennagrid_quantity_field quantity_field,
                                                                              const char ** unit);


VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                                                             viennagrid_int value_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                                                           viennagrid_int * value_count);

VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_set_value(viennagrid_quantity_field quantity_field,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_numeric * values);
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_quantity_field_get_value(viennagrid_quantity_field quantity_field,
                                                                                viennagrid_int element_id,
                                                                                viennagrid_numeric ** values);

#endif
