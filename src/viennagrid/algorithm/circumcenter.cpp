#include "viennagrid/viennagrid.h"
#include "../common.hpp"
#include <cmath>

viennagrid_error viennagrid_element_circumcenter(viennagrid_mesh mesh,
                                                 viennagrid_element_id element_id,
                                                 viennagrid_numeric * coords)
{
  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dimension) );

  viennagrid_element_type element_type;
  RETURN_ON_ERROR( viennagrid_element_type_get(mesh, element_id, &element_type) );


  if (coords)
  {
    for (viennagrid_dimension i = 0; i != dimension; ++i)
      coords[+i] = 0;

    if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
    {
      viennagrid_numeric * point_coords;
      RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, element_id, &point_coords) );
      viennagrid_copy(dimension, point_coords, coords);
      return VIENNAGRID_SUCCESS;
    }

    viennagrid_element_id *vertex_ids_begin, *vertex_ids_end;
    RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end) );

    std::vector<viennagrid_numeric *> point_coords(vertex_ids_end - vertex_ids_begin);
    for (viennagrid_element_id *it = vertex_ids_begin; it<vertex_ids_end; ++it)
      RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *it, &(point_coords[it - vertex_ids_begin])) );


    switch (element_type)
    {
      case VIENNAGRID_ELEMENT_TYPE_LINE:
      {
        for (viennagrid_dimension i = 0; i<dimension; ++i)
          coords[+i] = (point_coords[0][+i] + point_coords[1][+i])/2.0;
          return VIENNAGRID_SUCCESS;
      }

      case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      {
        if (dimension == 2)
        {
          viennagrid_numeric * A = point_coords[0];
          viennagrid_numeric * B = point_coords[1];
          viennagrid_numeric * C = point_coords[2];

          viennagrid_numeric D = 2.0 * ( A[0] * (B[1] - C[1])
                                        + B[0] * (C[1] - A[1])
                                        + C[0] * (A[1] - B[1]) );

          coords[0] = ( (A[1] * A[1] + A[0] * A[0]) * (B[1] - C[1])
                      + (B[1] * B[1] + B[0] * B[0]) * (C[1] - A[1])
                      + (C[1] * C[1] + C[0] * C[0]) * (A[1] - B[1])  ) / D;

          coords[1] = ( (A[1] * A[1] + A[0] * A[0]) * (C[0] - B[0])
                      + (B[1] * B[1] + B[0] * B[0]) * (A[0] - C[0])
                      + (C[1] * C[1] + C[0] * C[0]) * (B[0] - A[0])  ) / D;

          return VIENNAGRID_SUCCESS;
        }
        else if (dimension == 3)
        {
          viennagrid_numeric * A = point_coords[0];
          viennagrid_numeric * B = point_coords[1];
          viennagrid_numeric * C = point_coords[2];

          viennagrid_numeric AmB[3];
          viennagrid_numeric AmC[3];
          viennagrid_numeric BmA[3];
          viennagrid_numeric BmC[3];
          viennagrid_numeric CmA[3];
          viennagrid_numeric CmB[3];

          viennagrid_subtract(3, A, B, AmB);
          viennagrid_subtract(3, A, C, AmC);
          viennagrid_subtract(3, B, A, BmA);
          viennagrid_subtract(3, B, C, BmC);
          viennagrid_subtract(3, C, A, CmA);
          viennagrid_subtract(3, C, B, CmB);


//             double denominator = 2.0 * viennagrid::inner_prod(viennagrid::cross_prod(A-B, B-C), viennagrid::cross_prod(A-B, B-C));
          viennagrid_numeric tmp_vec[3];
          viennagrid_cross_prod(AmB, BmC, tmp_vec);
          viennagrid_numeric denominator;
          viennagrid_inner_prod(3, tmp_vec, tmp_vec, &denominator);
          denominator *= 2.0;


          viennagrid_numeric tmp0;
          viennagrid_numeric tmp1;

//             double alpha = viennagrid::inner_prod(B - C, B - C) * viennagrid::inner_prod(A - B, A - C);
          viennagrid_inner_prod(3, BmC, BmC, &tmp0);
          viennagrid_inner_prod(3, AmB, AmC, &tmp1);
          viennagrid_numeric alpha = tmp0*tmp1;

//             double beta  = viennagrid::inner_prod(A - C, A - C) * viennagrid::inner_prod(B - A, B - C);
          viennagrid_inner_prod(3, AmC, AmC, &tmp0);
          viennagrid_inner_prod(3, BmA, BmC, &tmp1);
          viennagrid_numeric beta = tmp0*tmp1;

//             double gamma = viennagrid::inner_prod(A - B, A - B) * viennagrid::inner_prod(C - A, C - B);
          viennagrid_inner_prod(3, AmB, AmB, &tmp0);
          viennagrid_inner_prod(3, CmA, CmB, &tmp1);
          viennagrid_numeric gamma = tmp0*tmp1;

//             PointT circ_cent = A * (alpha/denominator) + B * (beta/denominator) + C * (gamma/denominator);
          for (viennagrid_dimension i = 0; i<dimension; ++i)
            coords[+i] = (A[+i]*alpha + B[+i]*beta + C[+i]*gamma) / denominator;

          return VIENNAGRID_SUCCESS;
        }
        else
          return VIENNAGRID_ERROR_INVALID_GEOMETRIC_DIMENSION;
      }

      case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      {
        if (dimension != 3)
          return VIENNAGRID_ERROR_INVALID_GEOMETRIC_DIMENSION;

        viennagrid_numeric * O = point_coords[0];
        viennagrid_numeric A[3];
        viennagrid_subtract(3, point_coords[1], O, A);

        viennagrid_numeric B[3];
        viennagrid_subtract(3, point_coords[2], O, A);

        viennagrid_numeric C[3];
        viennagrid_subtract(3, point_coords[3], O, A);


        viennagrid_numeric AcB[3];
        viennagrid_numeric CcA[3];
        viennagrid_numeric BcC[3];

        viennagrid_cross_prod(A, B, AcB);
        viennagrid_cross_prod(C, A, CcA);
        viennagrid_cross_prod(B, C, BcC);


        viennagrid_numeric ABcC;
        viennagrid_numeric norm_BcC;
        viennagrid_cross_prod(A, BcC, &ABcC);
        viennagrid_norm_2(3, BcC, &norm_BcC);
        if (std::abs(ABcC) < VIENNAGRID_TOLERANCE_EPSILON * norm_BcC)
          return VIENNAGRID_ERROR_NUMERIC_CLOSE_TO_ZERO;


        viennagrid_numeric AA;
        viennagrid_numeric BB;
        viennagrid_numeric CC;

        viennagrid_inner_prod(3, A, A, &AA);
        viennagrid_inner_prod(3, B, B, &BB);
        viennagrid_inner_prod(3, C, C, &CC);

        viennagrid_numeric denominator;
        viennagrid_inner_prod(3, A, BcC, &denominator);
        denominator *= 2;


        for (viennagrid_dimension i = 0; i<dimension; ++i)
          coords[+i] = O[+i] + (AcB[+i]*CC + CcA[+i]*BB + BcC[+i]*AA) / denominator;


//           PointT circ_cent = (cross_prod(A, B) * viennagrid::inner_prod(C, C)
//                                 + cross_prod(C, A) * viennagrid::inner_prod(B, B)
//                                 + cross_prod(B, C) * viennagrid::inner_prod(A, A)
//                                 ) / (viennagrid::inner_prod(A, viennagrid::cross_prod(B, C)) * 2.0);
//           PointT cprodBC = viennagrid::cross_prod(B, C);
//           if (std::fabs(viennagrid::inner_prod(A, cprodBC)) < 1e-10 * viennagrid::norm_2(cprodBC) )
//           {
//             std::cerr << "Near singularity in circum center calculation!" << std::endl;
//             std::cerr << "A: " << A << std::endl;
//             std::cerr << "B: " << B << std::endl;
//             std::cerr << "C: " << C << std::endl;
//             std::cerr << "B x C: " << viennagrid::cross_prod(B, C) << std::endl;
//             throw std::runtime_error("Near singularity in circum center calculation!");
//           }
        return VIENNAGRID_SUCCESS;
      }

      default:
        return VIENNAGRID_ERROR_UNSUPPORTED_ELEMENT_TYPE;
    }
  }

  return VIENNAGRID_SUCCESS;
}
