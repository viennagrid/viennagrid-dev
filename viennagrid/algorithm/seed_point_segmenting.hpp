#ifndef VIENNAGRID_ALGORITHM_SEED_POINT_SEGMENTING_HPP
#define VIENNAGRID_ALGORITHM_SEED_POINT_SEGMENTING_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


namespace viennagrid
{

    namespace segmentation
    {
        template<typename point_type>
        bool triangle_ray_intersect(point_type const & r, point_type d, point_type const & A, point_type const & B, point_type const & C)
        {
            point_type b = B-A; // vector from A to B
            point_type c = C-A; // vector from A to C

            point_type rhs = r-A;

            // solve the equation:
            // r + lambda*d = A + beta*b + gamma*c
            // beta*b + gamma*c - lambda*d = r-A
            // (b c -d) (beta gamma lambda)^T = r-A
            // (beta gamma lambda)^T = (b c -d)^-1 (r-A)

            double det = viennagrid::geometry::determinant( b, c, -d );

// std::cout << " det = " << det << std::endl;

            if ( std::abs(det) < 1e-6)
            {

                point_type n = viennagrid::cross_prod( B-A, C-A );

                point_type center = (A+B+C)/3.0;
                if( std::abs( viennagrid::inner_prod( n, r-center ) ) < 1e-6 )
                    return true; // r lies on triangle plane, TODO behandeln: kreuzt strahl dreieck?
                else
                    return false;
            }
            else
            {
                point_type im[3]; // inverse matrix (b c -d)^-1

                d = -d; // invert for simplicity

                im[0][0] = c[1]*d[2] - c[2]*d[1];
                im[1][0] = -(b[1]*d[2] - b[2]*d[1]);
                im[2][0] = b[1]*c[2] - b[2]*c[1];

                im[0][1] = -(c[0]*d[2] - c[2]*d[0]);
                im[1][1] = b[0]*d[2] - b[2]*d[0];
                im[2][1] = -(b[0]*c[2] - b[2]*c[0]);

                im[0][2] = c[0]*d[1] - c[1]*d[0];
                im[1][2] = -(b[0]*d[1] - b[1]*d[0]);
                im[2][2] = b[0]*c[1] - b[1]*c[0];

                double beta = viennagrid::inner_prod( rhs, im[0] );
                double gamma = viennagrid::inner_prod( rhs, im[1] );
                double lambda = viennagrid::inner_prod( rhs, im[2] );

                if (det < 0)
                {
                    det = -det;
                    beta = -beta;
                    gamma = -gamma;
                    lambda = -lambda;
                }



                double alpha = det - beta - gamma;

// std::cout << " alpha = " << alpha << std::endl;
// std::cout << " beta = " << beta << std::endl;
// std::cout << " gamma = " << gamma << std::endl;
// std::cout << " lambda = " << lambda << std::endl;


// std::cout << " Found intersection: " << alpha/det << " " << beta/det << " " << gamma/det << " " << lambda/det << std::endl;
                double offset = det * 1e-6;
                double lower = 0 - offset;
                double upper = det + offset;

                if ( (alpha >= lower ) && (beta >= lower) && (gamma >= lower) && (alpha <= upper) && (beta <= upper) && (gamma <= upper) && (lambda >= lower) && (lambda <= upper))
                {
// std::cout << " Triangle: " << A << " / " << B << " / " << C << std::endl;
// std::cout << " Found intersection: " << alpha/det << " " << beta/det << " " << gamma/det << " " << lambda/det << std::endl;
                    return true;
                }
                else
                    return false;
            }

        }



        // adds an element and its neighbour elements to a segment
        // this method is called recursively on all valid neighbour elements
        // if a line is shared by more than 2 triangles, the triangle with the smallest inward angle is selected; this ensures a valid segment complex
        // the triangle with the smallest angle is calculated in this way:
        // for each boundary line set up a coordinate system in 2D where
        // the y-vector is the inverse normal of the current triangle
        // the x-vector is the cross product of the shared line vector and the normal of the current triangle
        // project the vector from line center to the neighbour triangle center onto this cooridanite system
        // calculate the oriented angle between the positive x-axis (=vector to the current triangle) and the projected vector to the neighbour triangle
        // the neighbour triangle with the smallest angle is selected

        template<typename mesh_type, typename VisibleStateAccessorT, typename SegmentT, typename triangle_handle_type>
        void mark_facing_shortes_angle( mesh_type & mesh, VisibleStateAccessorT visited_state_accessor, SegmentT & segment, triangle_handle_type triangle_handle, bool triangle_faces_outward )
        {
            typedef typename viennagrid::result_of::point<mesh_type>::type point_type;
            typedef typename viennagrid::result_of::coord<point_type>::type coord_type;

            typedef typename viennagrid::result_of::element<mesh_type, viennagrid::line_tag>::type line_type;
            typedef typename viennagrid::result_of::element<mesh_type, viennagrid::triangle_tag>::type triangle_type;
            typedef typename viennagrid::result_of::handle<mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;

            typedef viennagrid::storage::static_array<vertex_handle_type, 3> handle_array;

            triangle_type & triangle = viennagrid::dereference_handle(segment, triangle_handle);

            // if this triangle has already been visited -> nothing to do
            if (visited_state_accessor(triangle))
// if (viennamesh::was_visited(triangle))
                return;

            // add this triangle to the segment
// viennamesh::add_face_to_segment( mesh, triangle, segment_id, triangle_faces_outward );
            {
                viennagrid::add( segment, triangle );
                *viennagrid::segment_element_info( segment, triangle ) = triangle_faces_outward;

              
//                 typedef typename segmentation_type::element_segment_info_type element_segment_info_type;
//                 element_segment_info_type current = segmentation.segment_info( triangle );
//                 if (triangle_faces_outward)
//                     current.positive_orientation_segment_id = segment_id;
//                 else
//                     current.negative_orientation_segment_id = segment_id;
//                 segmentation.set_segment_info( triangle, current );
            }

            // ... and set the visited flag
            visited_state_accessor(triangle) = true;
// viennamesh::set_visited(triangle);

            handle_array vtx;
            vtx[0] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(0);
            vtx[1] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(1);
            vtx[2] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(2);


            viennagrid::storage::static_array<point_type,3> p;
            p[0] = viennagrid::point( mesh, vtx[0] );
            p[1] = viennagrid::point( mesh, vtx[1] );
            p[2] = viennagrid::point( mesh, vtx[2] );

            // calculate the center of the triangle
            point_type center = (p[0]+p[1]+p[2])/3.0;

            // ... and its normal
            point_type normal = viennagrid::cross_prod( p[1]-p[0], p[2]-p[0] );
            // correcting the normal direction
            if (!triangle_faces_outward) normal = -normal;
            // normalizing the normal vector
            normal /= viennagrid::norm_2(normal);


        // std::cout << " --- Triangle " << std::endl;
        // std::cout << " adding to segment " << segment_id << std::endl;
        // std::cout << " " << p[0] << std::endl;
        // std::cout << " " << p[1] << std::endl;
        // std::cout << " " << p[2] << std::endl;
        // std::cout << " normal = " << normal << std::endl;



            typedef typename viennagrid::result_of::element_range<triangle_type, viennagrid::line_tag>::type lines_on_triangle_range_type;
            typedef typename viennagrid::result_of::iterator<lines_on_triangle_range_type>::type lines_on_triangle_range_iterator;

            // iterating over all boundary lines
            lines_on_triangle_range_type lines = viennagrid::elements<viennagrid::line_tag>( triangle );
            for ( lines_on_triangle_range_iterator lit = lines.begin(); lit != lines.end(); ++lit )
            {
                line_type & line = *lit;

//                 viennagrid::storage::static_array<vertex_handle_type, 2> lvtx;
//                 lvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(0);
//                 lvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(1);
// 
                viennagrid::storage::static_array<point_type, 2> lp;
//                 lp[0] = viennagrid::point( mesh, lvtx[0] );
//                 lp[1] = viennagrid::point( mesh, lvtx[1] );
                
                lp[0] = viennagrid::point( mesh, viennagrid::vertices(line)[0] );
                lp[1] = viennagrid::point( mesh, viennagrid::vertices(line)[1] );


                // calculating the line vector
                point_type line_vector = lp[1]-lp[0];
                // ... and normalizing it
                line_vector /= viennagrid::norm_2(line_vector);

                // calculating the center of the boundary line
                point_type line_center = (lp[0]+lp[1]) / 2.0;
                // calculate the vector facing towards the current triangle
                point_type line_to_triangle_vector = viennagrid::cross_prod( -normal, line_vector );
                // ... and normalizing it
                line_to_triangle_vector /= viennagrid::norm_2(line_to_triangle_vector);

                // check and correnct the orentietion of the vector facing towards the current triangle
                if (viennagrid::inner_prod(line_to_triangle_vector, center - line_center) < 0)
                    line_to_triangle_vector = -line_to_triangle_vector;


// std::cout << " line: " << lp[0] << " - " << lp[1] << std::endl;


                typedef typename viennagrid::result_of::coboundary_range<mesh_type, viennagrid::line_tag, viennagrid::triangle_tag>::type coboundary_range_type;
                typedef typename viennagrid::result_of::iterator<coboundary_range_type>::type coboudnary_iterator;

                coboundary_range_type coboundary_triangles = viennagrid::coboundary_elements<viennagrid::line_tag, viennagrid::triangle_tag>(mesh, lit.handle());

// if ( neighbour_triangles.size() != 2 )
// {
        // std::cout << " INFO - more than two triangle share the line" << std::endl;
        // std::cout << "ERROR there is something wrong... A line has not 2 coboundary triangles: " << neighbour_triangles.size() << std::endl;
        // std::cout << viennagrid::point(mesh, viennagrid::elements<viennagrid::vertex_tag>(triangle)[0]) << " / " <<
        // viennagrid::point(mesh, viennagrid::elements<viennagrid::vertex_tag>(triangle)[1]) << " / " <<
        // viennagrid::point(mesh, viennagrid::elements<viennagrid::vertex_tag>(triangle)[2]) << std::endl;
        // return ;
// }


                // smallest angle, triangle with smallest angle and facing outward flag of this triangle
                coord_type smallest_angle = std::numeric_limits<coord_type>::max();
                triangle_handle_type smallest_angle_triangle;
                bool smallest_angle_triangle_faces_outward;

                // iterating over all coboundary triangles of the current line
                for (coboudnary_iterator it = coboundary_triangles.begin(); it != coboundary_triangles.end(); ++it)
                {
                    triangle_handle_type handle = it.handle();
                    triangle_type & neighbour_triangle = *it;

        // std::cout << " neighbour " << neighbour_triangle << std::endl;

                    // is the coboundary triangle the current triangle -> skipping
                    if (handle == triangle_handle)
                        continue;

        // if (viennamesh::was_visited(neighbour_triangle))
        // continue;

                    handle_array nvtx;
                    nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(0);
                    nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(1);
                    nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(2);

                    viennagrid::storage::static_array<point_type,3> np;
                    np[0] = viennagrid::point( mesh, nvtx[0] );
                    np[1] = viennagrid::point( mesh, nvtx[1] );
                    np[2] = viennagrid::point( mesh, nvtx[2] );

                    // calculating the center of the neighbour triangle
                    point_type neighbour_center = (np[0]+np[1]+np[2])/3.0;
                    // calculating the vector from the line center towards the neighbour triangle
                    point_type line_to_neighbour_triangle_vector = neighbour_center - line_center;
                    // ... and normalizing it
                    line_to_neighbour_triangle_vector /= viennagrid::norm_2(line_to_neighbour_triangle_vector);

                    // projecting the vector facing to the neighbour triangle onto the 2D coordinate system
                    coord_type x = viennagrid::inner_prod( line_to_triangle_vector, line_to_neighbour_triangle_vector );
                    coord_type y = viennagrid::inner_prod( -normal, line_to_neighbour_triangle_vector );

                    // normalizing the 2D vector
                    coord_type tmp = std::sqrt( x*x + y*y );
                    x /= tmp;
                    y /= tmp;



                    // calculate the angle
                    coord_type angle_center_vectors = std::acos( x );
                    // calculate the corrected oriented angle; if y < 0 the angle has to be corrected
                    coord_type oriented_angle = (y > 0) ? angle_center_vectors : 2.0*M_PI - angle_center_vectors;

        // std::cout << " --- Neighbour Triangle " << std::endl;
        // std::cout << " " << np[0] << std::endl;
        // std::cout << " " << np[1] << std::endl;
        // std::cout << " " << np[2] << std::endl;
        // std::cout << " normal = " << neighbour_normal << std::endl;
        // // std::cout << " line_vector = " << line_vector << std::endl;
        // std::cout << " vector triangle = " << line_to_triangle_vector << std::endl;
        // std::cout << " vector neighbour triangle = " << line_to_neighbour_triangle_vector << std::endl;
        // std::cout << " angle orientation = " << angle_orientation << std::endl;
        // std::cout << " angle center vectors = " << angle_center_vectors << std::endl;
        // std::cout << " orientated angle = " << oriented_angle << std::endl;


                    // if the corrent angle is smaller than the best -> yay! better choice found
                    if (oriented_angle < smallest_angle)
                    {
                        smallest_angle = oriented_angle;
                        smallest_angle_triangle = handle;

                        typename handle_array::iterator vtx_it;
                        typename handle_array::iterator nvtx_it;

                        for (vtx_it = vtx.begin(); vtx_it != vtx.end(); ++vtx_it)
                        {
                            nvtx_it = std::find(nvtx.begin(), nvtx.end(), *vtx_it);
                            if (nvtx_it != nvtx.end())
                                break;
                        }


                        // topology check
                        if ( ( *viennagrid::geometry::circular_next(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
                            ( *viennagrid::geometry::circular_prev(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) )
                        {
                            smallest_angle_triangle_faces_outward = !triangle_faces_outward;

                        }
                        else if ( ( *viennagrid::geometry::circular_next(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
                            ( *viennagrid::geometry::circular_prev(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) )
                        {
                            smallest_angle_triangle_faces_outward = triangle_faces_outward;
                        }
                        else
                        {
                            std::cout << "Something went wrong..." << std::endl;
                        }
                    }
                }

                // is a triangle found -> call mark_facing_shortes_angle recursively
                if (smallest_angle != std::numeric_limits<coord_type>::max())
                {
// triangle_type & best = viennagrid::dereference_handle(mesh, smallest_angle_triangle);
//
// handle_array nvtx;
// nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(0);
// nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(1);
// nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(2);
//
// viennagrid::storage::static_array<point_type,3> np;
// np[0] = viennagrid::point( mesh, nvtx[0] );
// np[1] = viennagrid::point( mesh, nvtx[1] );
// np[2] = viennagrid::point( mesh, nvtx[2] );

        // std::cout << " Best neighbour triangle" << std::endl;
        // std::cout << " " << np[0] << std::endl;
        // std::cout << " " << np[1] << std::endl;
        // std::cout << " " << np[2] << std::endl;
        // std::cout << " faces outward = " << smallest_angle_triangle_faces_outward << std::endl;

                    mark_facing_shortes_angle( mesh, visited_state_accessor, segment, smallest_angle_triangle, smallest_angle_triangle_faces_outward );
                }

            }
        }



        // this function detects and marks a face segment based on a seed point within it
        // is searches for all triangle which vector from its center towards the seed points does not intersect any other triangle
        // these triangles is for sure in the this segment
        // mark_facing_shortes_angle is called on those triangles to complete the segments

        template< typename mesh_type, typename SegmentT, typename vector_type >
        void detect_and_mark_face_segment( mesh_type & mesh, SegmentT & segment_, vector_type seed_point )
        {
            typedef typename viennagrid::result_of::point<mesh_type>::type point_type;
            typedef typename viennagrid::result_of::coord<point_type>::type coord_type;

            typedef typename viennagrid::result_of::element<mesh_type, viennagrid::triangle_tag>::type triangle_type;
            typedef typename viennagrid::result_of::handle<mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;
            typedef typename viennagrid::result_of::element_range<mesh_type, viennagrid::triangle_tag>::type triangle_range_type;
            typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

//             segmentation_.init( mesh );
            std::vector<bool> visited_state( viennagrid::id_upper_bound<triangle_type>(mesh).get(), false );
            typename viennagrid::result_of::accessor< std::vector<bool>, triangle_type >::type visible_state_accessor = viennagrid::make_accessor<triangle_type>(visited_state);

            // iteratin over all triangles
            triangle_range_type triangles = viennagrid::elements( mesh );
            for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
            {
                triangle_type const & triangle = *it; //viennagrid::dereference_handle( mesh, *it );

                // has this triangle already been visited? -> skipping
                if (visible_state_accessor(triangle))
// if (viennamesh::was_visited(triangle))
                    continue;

                point_type const & p0 = viennagrid::point( mesh, viennagrid::elements<viennagrid::vertex_tag>(triangle)[0] );
                point_type const & p1 = viennagrid::point( mesh, viennagrid::elements<viennagrid::vertex_tag>(triangle)[1] );
                point_type const & p2 = viennagrid::point( mesh, viennagrid::elements<viennagrid::vertex_tag>(triangle)[2] );

// std::cout << std::endl << std::endl << std::endl;
// std::cout << "Triangle: " << p0 << " / " << p1 << " / " << p2 << std::endl;

                // calculating the center of the triangle
                point_type r = (p0+p1+p2)/3.0;

                // calculating the normale vector of the triangle
                point_type n = viennagrid::cross_prod( p1-p0, p2-p0 );
                // ... and normalizing it
                n /= viennagrid::norm_2(n);

                // calculating the ray vector from the center of the triangle the the seed point
                point_type d = seed_point - r;

// std::cout << " Seed point: " << seed_point << std::endl;
// std::cout << " Triangle center: " << r << std::endl;
// std::cout << " vector to seed point: " << d << std::endl;

                // projecting the normalized ray vector onto the normal vector
                coord_type p = viennagrid::inner_prod( d, n ) / viennagrid::norm_2(d);

                // if the projection if near zero (happens when the ray vector and the triangle are co-linear) -> skip this triangle
                if ( std::abs(p) < 1e-6 )
                {
//     std::cout << " Line from triangle center to seed point is orthogonal to triangle normal -> skipping" << std::endl;
                    continue;
                }

                // if the projection if negative, the triangle faces outward
                bool faces_outward = p < 0;

// bool is_inside = false;
                // iterating over all triangles and check for intersection
                triangle_range_iterator jt = triangles.begin();
                for (; jt != triangles.end(); ++jt)
                {
                    if (it == jt) continue; // no self intersect test

                    triangle_type const & to_test = *jt;

                    point_type const & A = viennagrid::point( mesh, viennagrid::elements<viennagrid::vertex_tag>(to_test)[0] );
                    point_type const & B = viennagrid::point( mesh, viennagrid::elements<viennagrid::vertex_tag>(to_test)[1] );
                    point_type const & C = viennagrid::point( mesh, viennagrid::elements<viennagrid::vertex_tag>(to_test)[2] );

// std::cout << " Triangle intersection test: " << A << " / " << B << " / " << C << std::endl;

                    if (segmentation::triangle_ray_intersect(r, d, A, B, C)) // TODO: scale by bounding box to ensure a ray outside the mesh
                    {
// std::cout << " INTERSECTION!" << std::endl;
                        break;
                    }
                    else
                    {
// std::cout << " no intersection" << std::endl;
                    }
                }

                // if there was no intersection -> mark this triangle and all neighbour triangles recursively
                if (jt == triangles.end())
                {
                    if (!faces_outward) n = -n;
// std::cout << " YAY! triangle has visible line to seed point " << seed_point << " " << n << std::endl;

    // viennamesh::add_face_to_segment(triangle, segment_index, faces_outward);
                    segmentation::mark_facing_shortes_angle( mesh, visible_state_accessor, segment_, it.handle(), faces_outward );
    // break;
                }
            }
        }

    }



    template< typename mesh_type, typename segmentation_type, typename seed_point_iterator_type >
    void mark_face_segments( mesh_type & mesh, segmentation_type & segmentation_, seed_point_iterator_type seed_points_it, seed_point_iterator_type const & seed_points_end )
    {
        for ( ; seed_points_it != seed_points_end; ++seed_points_it)
            segmentation::detect_and_mark_face_segment(mesh, segmentation_(seed_points_it->first), seed_points_it->second);
    }

}



#endif
