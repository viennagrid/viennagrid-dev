#ifndef VIENNAGRID_ALGORITHM_ANGLE_HPP
#define VIENNAGRID_ALGORITHM_ANGLE_HPP

#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include <CGAL/Plane_3.h>

namespace viennagrid
{
    
    template<typename point_type>
    typename viennagrid::result_of::coord_type<point_type>::type spanned_angle( point_type const & v0, point_type const & v1 )
    {
        return std::acos( viennagrid::inner_prod(v0, v1) / (viennagrid::norm_2(v0) * viennagrid::norm_2(v1)) );
    }
    
    // http://en.wikipedia.org/wiki/Solid_angle#Tetrahedron
    template<typename point_type>
    typename viennagrid::result_of::coord_type<point_type>::type spanned_angle( point_type const & a, point_type const & b, point_type const & c )
    {
        typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
        
        coord_type det = viennagrid::geometry::determinant(a, b, c);
        
        coord_type al = viennagrid::norm_2(a);
        coord_type bl = viennagrid::norm_2(b);
        coord_type cl = viennagrid::norm_2(c);
        
        coord_type div = al*bl*cl + viennagrid::inner_prod(a,b)*cl + viennagrid::inner_prod(a,c)*bl + viennagrid::inner_prod(b,c)*al;
        coord_type at = std::atan2(det, div);
        
        return std::abs(2*at);
    }
    
    

    template<typename point_type>
    typename viennagrid::result_of::coord_type<point_type>::type dihedral_angle( point_type const & p0, point_type const & p1, point_type const & p2,
                                                                                 point_type const & q0, point_type const & q1, point_type const & q2)
    {
        point_type np = viennagrid::cross_prod( p1-p0, p2-p0 );
        point_type nq = viennagrid::cross_prod( q1-q0, q2-q0 );
        
        return spanned_angle(np, nq);
    }
    
    
    
    
    template<typename point_type>
    typename viennagrid::result_of::coord_type<point_type>::type angle( point_type const & origin, point_type const & p0, point_type const & p1 )
    {
        point_type v0 = p0 - origin;
        point_type v1 = p1 - origin;
        
        return spanned_angle(v0, v1);
    }
    
    template<typename point_type>
    typename viennagrid::result_of::coord_type<point_type>::type angle( point_type const & origin, point_type const & p0, point_type const & p1, point_type const & p2 )
    {
        point_type v0 = p0 - origin;
        point_type v1 = p1 - origin;
        point_type v2 = p2 - origin;
        
        return spanned_angle(v0, v1, v2);
    }
    
    // same as above, http://en.wikipedia.org/wiki/Solid_angle#Tetrahedron
    template<typename point_type>
    typename viennagrid::result_of::coord_type<point_type>::type angle2( point_type const & origin, point_type const & a, point_type const & b, point_type const & c )
    {
        double da_ab = dihedral_angle( origin, a, c, origin, b, c );
        double da_ac = dihedral_angle( origin, a, b, origin, c, b );
        double da_bc = dihedral_angle( origin, b, a, origin, c, a );
        
        return da_ab + da_ac + da_bc - M_PI;
    }

}

#endif