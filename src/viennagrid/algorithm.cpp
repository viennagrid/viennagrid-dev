/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <vector>
#include <cmath>

#include "viennagrid/viennagrid.h"
#include "common.hpp"



viennagrid_error viennagrid_copy(viennagrid_dimension       dimension,
                                 viennagrid_numeric const * v,
                                 viennagrid_numeric       * result)
{
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v[+i];
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_add(viennagrid_dimension dimension,
                                viennagrid_numeric const * v1,
                                viennagrid_numeric const * v2,
                                viennagrid_numeric       * result)
{
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v1[+i] + v2[+i];
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_subtract(viennagrid_dimension dimension,
                                     viennagrid_numeric const * v1,
                                     viennagrid_numeric const * v2,
                                     viennagrid_numeric       * result)
{
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v1[+i] + v2[+i];
  return VIENNAGRID_SUCCESS;
}




/* computes the cross-product of two vectors: result = v1 x v2  (3d only) */
viennagrid_error viennagrid_cross_prod(viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the inner product of two vectors: result = (v1, v2) */
viennagrid_error viennagrid_inner_prod(viennagrid_dimension dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_dimension i = 0; i < dimension; ++i)
      *result += v1[+i] * v2[+i];
  }

  return VIENNAGRID_SUCCESS;
}


/* computes the l1-norm of a vector: result = ||v1||_1 */
viennagrid_error viennagrid_norm_1(viennagrid_dimension dimension,
                                   viennagrid_numeric const * v1,
                                   viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_dimension i = 0; i < dimension; ++i)
      *result +=  std::abs(v1[+i]);
  }

  return VIENNAGRID_SUCCESS;
}


/* computes the l2-norm of a vector: result = ||v1||_2 */
viennagrid_error viennagrid_norm_2(viennagrid_dimension dimension,
                                   viennagrid_numeric const * v1,
                                   viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_dimension i = 0; i < dimension; ++i)
      *result += v1[+i] * v1[+i];
    *result = std::sqrt(*result);
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the l2-norm of a vector: result = ||v1||_2 */
viennagrid_error viennagrid_norm_inf(viennagrid_dimension dimension,
                                     viennagrid_numeric const * v1,
                                     viennagrid_numeric       * result)
{
  if (result && dimension > 0)
  {
    *result = std::abs(v1[0]);
    for (viennagrid_dimension i = 1; i < dimension; ++i)
      *result = std::max( *result, std::abs(v1[+i]) );
  }

  return VIENNAGRID_SUCCESS;
}



/* computes the l1-distance of two vectors: result = ||v1-v2||_1 */
viennagrid_error viennagrid_distance_1(viennagrid_dimension dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    std::vector<viennagrid_numeric> v1mv2(dimension);
    viennagrid_subtract(dimension, v1, v2, &v1mv2[0]);
    viennagrid_norm_1(dimension, &v1mv2[0], result);
  }

  return VIENNAGRID_SUCCESS;
}


/* computes the l2-distance of two vectors: result = ||v1-v2||_2 */
viennagrid_error viennagrid_distance_2(viennagrid_dimension dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    std::vector<viennagrid_numeric> v1mv2(dimension);
    viennagrid_subtract(dimension, v1, v2, &v1mv2[0]);
    viennagrid_norm_2(dimension, &v1mv2[0], result);
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the inf-distance of two vectors: result = ||v1-v2||_inf */
viennagrid_error viennagrid_distance_inf(viennagrid_dimension dimension,
                                         viennagrid_numeric const * v1,
                                         viennagrid_numeric const * v2,
                                         viennagrid_numeric       * result)
{
  if (result && dimension > 0)
  {
    std::vector<viennagrid_numeric> v1mv2(dimension);
    viennagrid_subtract(dimension, v1, v2, &v1mv2[0]);
    viennagrid_norm_inf(dimension, &v1mv2[0], result);
  }

  return VIENNAGRID_SUCCESS;
}



