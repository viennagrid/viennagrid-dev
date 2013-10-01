
#include "viennagrid/algorithm/intersect.hpp"
#include "viennagrid/mesh/config.hpp"


template<typename numeric_type>
void test_interval( numeric_type first_0, numeric_type second_0, numeric_type first_1, numeric_type second_1 )
{
    std::cout << "Interval (" << first_0 << ";" << second_0 << ") - Interval (" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_open_tag(), first_1, second_1, viennagrid::geometry::interval::open_open_tag() ) << std::endl;
    std::cout << "Interval (" << first_0 << ";" << second_0 << "] - Interval (" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_closed_tag(), first_1, second_1, viennagrid::geometry::interval::open_open_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << ") - Interval (" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_open_tag(), first_1, second_1, viennagrid::geometry::interval::open_open_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << "] - Interval (" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_closed_tag(), first_1, second_1, viennagrid::geometry::interval::open_open_tag() ) << std::endl;

    std::cout << "Interval (" << first_0 << ";" << second_0 << ") - Interval (" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_open_tag(), first_1, second_1, viennagrid::geometry::interval::open_closed_tag() ) << std::endl;
    std::cout << "Interval (" << first_0 << ";" << second_0 << "] - Interval (" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_closed_tag(), first_1, second_1, viennagrid::geometry::interval::open_closed_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << ") - Interval (" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_open_tag(), first_1, second_1, viennagrid::geometry::interval::open_closed_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << "] - Interval (" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_closed_tag(), first_1, second_1, viennagrid::geometry::interval::open_closed_tag() ) << std::endl;

    std::cout << "Interval (" << first_0 << ";" << second_0 << ") - Interval [" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_open_tag(), first_1, second_1, viennagrid::geometry::interval::closed_open_tag() ) << std::endl;
    std::cout << "Interval (" << first_0 << ";" << second_0 << "] - Interval [" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_closed_tag(), first_1, second_1, viennagrid::geometry::interval::closed_open_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << ") - Interval [" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_open_tag(), first_1, second_1, viennagrid::geometry::interval::closed_open_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << "] - Interval [" << first_1 << "," << second_1 << ") intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_closed_tag(), first_1, second_1, viennagrid::geometry::interval::closed_open_tag() ) << std::endl;

    std::cout << "Interval (" << first_0 << ";" << second_0 << ") - Interval [" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_open_tag(), first_1, second_1, viennagrid::geometry::interval::closed_closed_tag() ) << std::endl;
    std::cout << "Interval (" << first_0 << ";" << second_0 << "] - Interval [" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::open_closed_tag(), first_1, second_1, viennagrid::geometry::interval::closed_closed_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << ") - Interval [" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_open_tag(), first_1, second_1, viennagrid::geometry::interval::closed_closed_tag() ) << std::endl;
    std::cout << "Interval [" << first_0 << ";" << second_0 << "] - Interval [" << first_1 << "," << second_1 << "] intersect " << viennagrid::geometry::interval::interval_intersect( first_0, second_0, viennagrid::geometry::interval::closed_closed_tag(), first_1, second_1, viennagrid::geometry::interval::closed_closed_tag() ) << std::endl;
}


template<typename point_type>
void test_point_line_intersect( point_type const & p, point_type const & q0, point_type const & q1 )
{
    std::cout << "Line ( " << q0 << ")-( " << q1 << ") - Point [ " << p << "] intersect " << viennagrid::geometry::point_line_intersect( p, q0, q1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line ( " << q0 << ")-[ " << q1 << "] - Point [ " << p << "] intersect " << viennagrid::geometry::point_line_intersect( p, q0, q1, viennagrid::geometry::interval::open_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << q0 << "]-( " << q1 << ") - Point [ " << p << "] intersect " << viennagrid::geometry::point_line_intersect( p, q0, q1, viennagrid::geometry::interval::closed_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << q0 << "]-[ " << q1 << "] - Point [ " << p << "] intersect " << viennagrid::geometry::point_line_intersect( p, q0, q1, viennagrid::geometry::interval::closed_closed_tag(), 1e-6 ) << std::endl;
}

template<typename point_type>
void test_line_line_intersect( point_type const & p0, point_type const & p1, point_type const & q0, point_type const & q1 )
{
    std::cout << "Line ( " << p0 << ")-( " << p1 << ") - Line ( " << q0 << ")-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_open_tag(), q0, q1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line ( " << p0 << ")-[ " << p1 << "] - Line ( " << q0 << ")-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_closed_tag(), q0, q1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-( " << p1 << ") - Line ( " << q0 << ")-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_open_tag(), q0, q1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-[ " << p1 << "] - Line ( " << q0 << ")-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_closed_tag(), q0, q1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ) << std::endl;

    std::cout << "Line ( " << p0 << ")-( " << p1 << ") - Line ( " << q0 << ")-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_open_tag(), q0, q1, viennagrid::geometry::interval::open_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line ( " << p0 << ")-[ " << p1 << "] - Line ( " << q0 << ")-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_closed_tag(), q0, q1, viennagrid::geometry::interval::open_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-( " << p1 << ") - Line ( " << q0 << ")-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_open_tag(), q0, q1, viennagrid::geometry::interval::open_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-[ " << p1 << "] - Line ( " << q0 << ")-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_closed_tag(), q0, q1, viennagrid::geometry::interval::open_closed_tag(), 1e-6 ) << std::endl;

    std::cout << "Line ( " << p0 << ")-( " << p1 << ") - Line [ " << q0 << "]-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_open_tag(), q0, q1, viennagrid::geometry::interval::closed_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line ( " << p0 << ")-[ " << p1 << "] - Line [ " << q0 << "]-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_closed_tag(), q0, q1, viennagrid::geometry::interval::closed_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-( " << p1 << ") - Line [ " << q0 << "]-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_open_tag(), q0, q1, viennagrid::geometry::interval::closed_open_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-[ " << p1 << "] - Line [ " << q0 << "]-( " << q1 << ") intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_closed_tag(), q0, q1, viennagrid::geometry::interval::closed_open_tag(), 1e-6 ) << std::endl;

    std::cout << "Line ( " << p0 << ")-( " << p1 << ") - Line [ " << q0 << "]-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_open_tag(), q0, q1, viennagrid::geometry::interval::closed_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line ( " << p0 << ")-[ " << p1 << "] - Line [ " << q0 << "]-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::open_closed_tag(), q0, q1, viennagrid::geometry::interval::closed_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-( " << p1 << ") - Line [ " << q0 << "]-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_open_tag(), q0, q1, viennagrid::geometry::interval::closed_closed_tag(), 1e-6 ) << std::endl;
    std::cout << "Line [ " << p0 << "]-[ " << p1 << "] - Line [ " << q0 << "]-[ " << q1 << "] intersect " << viennagrid::geometry::line_line_intersect( p0, p1, viennagrid::geometry::interval::closed_closed_tag(), q0, q1, viennagrid::geometry::interval::closed_closed_tag(), 1e-6 ) << std::endl;
}    



int main()
{
    typedef viennagrid::config::point_type_2d PointType2D;
    
//     test_interval( 0.0, 1.0, 0.0, 1.0 );
//     std::cout << std::endl;
//     test_interval( 0.0, 1.0, 1.0, 2.0 );
//     std::cout << std::endl;
//     test_interval( 1.0, 0.0, 1.0, 2.0 );
//     std::cout << std::endl;
//     test_interval( 1.0, 2.0, 0.0, 1.0 );
//     std::cout << std::endl;
//     test_interval( 0.0, 1.0, 0.5, 1.5 );
//     std::cout << std::endl;
//     test_interval( 0.5, 1.5, 0.0, 1.0 );
//     std::cout << std::endl;
//     test_interval( 0.0, 1.0, 2.0, 3.0 );
//     std::cout << std::endl;
//     test_interval( 0.0, 1.0, -1.0, 2.0 );
//     std::cout << std::endl;
//     test_interval( -1.0, 2.0, 0.0, 1.0 );
//     
//     std::cout << std::endl << std::endl;
//     
//     test_point_line_intersect( PointType2D(0,0), PointType2D(-1,-1), PointType2D(0,0) );
//     std::cout << std::endl;
//     test_point_line_intersect( PointType2D(0,0), PointType2D(0,0), PointType2D(1,1) );
//     std::cout << std::endl;
//     test_point_line_intersect( PointType2D(1,1), PointType2D(0,0), PointType2D(1,1) );
//     std::cout << std::endl;
//     test_point_line_intersect( PointType2D(0.5,0.5), PointType2D(0,0), PointType2D(1,1) );
//     std::cout << std::endl;
//     test_point_line_intersect( PointType2D(-1,1), PointType2D(0,0), PointType2D(1,1) );
//         
//     std::cout << std::endl << std::endl;
//     
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(0,0), PointType2D(-1,1) );
//     std::cout << std::endl;
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(1,0), PointType2D(0,1) );
//     std::cout << std::endl;
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(0,1), PointType2D(1,2) );
//     std::cout << std::endl;
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(-1,-1), PointType2D(0,0) );
//     std::cout << std::endl;
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(-1,-1), PointType2D(0.5,0.5) );
//     std::cout << std::endl;
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(0,1), PointType2D(0.5,0.5) );
//     std::cout << std::endl;
//     test_line_line_intersect( PointType2D(0,0), PointType2D(1,1), PointType2D(0.5,0.5), PointType2D(0,1) );
//     std::cout << std::endl;
    test_line_line_intersect( PointType2D(0,0), PointType2D(0,10), PointType2D(0,1), PointType2D(0,2) );

    return 0;
}
