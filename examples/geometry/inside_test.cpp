
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/domain/config.hpp"




int main()
{
    typedef viennagrid::config::point_type_2d PointType2D;
    
    std::vector<PointType2D> poly;
    
    poly.push_back( PointType2D(0, 0) );
    poly.push_back( PointType2D(10, 0) );
    poly.push_back( PointType2D(10, 1) );
    poly.push_back( PointType2D(20, 10) );
    poly.push_back( PointType2D(20, 20) );
    poly.push_back( PointType2D(10, 20) );
    poly.push_back( PointType2D(0, 10) );
    poly.push_back( PointType2D(5, 5) );
    

    std::cout << "0 - " << viennagrid::geometry::is_inside( poly.begin(), poly.end(), viennagrid::geometry::interval::open_tag(), PointType2D(0,0), 1e-6 ) << std::endl;
    std::cout << "1 - " << viennagrid::geometry::is_inside( poly.begin(), poly.end(), viennagrid::geometry::interval::closed_tag(), PointType2D(0,0), 1e-6 ) << std::endl;
    
    std::cout << "0 - " << viennagrid::geometry::is_inside( poly.begin(), poly.end(), viennagrid::geometry::interval::open_tag(), PointType2D(1,1), 1e-6 ) << std::endl;
    std::cout << "1 - " << viennagrid::geometry::is_inside( poly.begin(), poly.end(), viennagrid::geometry::interval::closed_tag(), PointType2D(1,1), 1e-6 ) << std::endl;
    
    std::cout << "1 - " << viennagrid::geometry::is_inside( poly.begin(), poly.end(), viennagrid::geometry::interval::open_tag(), PointType2D(10,5), 1e-6 ) << std::endl;
    std::cout << "1 - " << viennagrid::geometry::is_inside( poly.begin(), poly.end(), viennagrid::geometry::interval::closed_tag(), PointType2D(10,5), 1e-6 ) << std::endl;
    
    
    
    std::vector<PointType2D> poly2;
    
    poly2.push_back( PointType2D(10, 10) );
    poly2.push_back( PointType2D(12, 10) );
    poly2.push_back( PointType2D(10, 12) );
    
    std::cout << "0 - " << viennagrid::geometry::is_inside( poly2.begin(), poly2.end(), viennagrid::geometry::interval::open_tag(), PointType2D(10,20), 1e-6 ) << std::endl;
    
    
    
    std::vector<PointType2D> poly3;
    
    poly3.push_back( PointType2D(0, 0) );
    poly3.push_back( PointType2D(10, 0) );
    poly3.push_back( PointType2D(10, 10) );
    poly3.push_back( PointType2D(5, 10) );
    poly3.push_back( PointType2D(5, 8) );
    poly3.push_back( PointType2D(8, 8) );
    poly3.push_back( PointType2D(8, 7) );
    poly3.push_back( PointType2D(5, 7) );
    poly3.push_back( PointType2D(5, 5) );
    poly3.push_back( PointType2D(0, 5) );
    
    std::cout << "0 - " << viennagrid::geometry::is_inside( poly3.begin(), poly3.end(), viennagrid::geometry::interval::open_tag(), PointType2D(5,20), 1e-6 ) << std::endl;
    std::cout << "0 - " << viennagrid::geometry::is_inside( poly3.begin(), poly3.end(), viennagrid::geometry::interval::open_tag(), PointType2D(5,7.5), 1e-6 ) << std::endl;
    std::cout << "0 - " << viennagrid::geometry::is_inside( poly3.begin(), poly3.end(), viennagrid::geometry::interval::open_tag(), PointType2D(5,6.5), 1e-6 ) << std::endl;
    std::cout << "1 - " << viennagrid::geometry::is_inside( poly3.begin(), poly3.end(), viennagrid::geometry::interval::closed_tag(), PointType2D(5,6.5), 1e-6 ) << std::endl;
    
    
    return 0;
}

