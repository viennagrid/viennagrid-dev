#ifndef VIENNAGRID_CONTAINER_HPP
#define VIENNAGRID_CONTAINER_HPP

namespace viennagrid
{
    template<template<typename,typename> class container_template, typename value_type>
    struct std_container
    {
        typedef container_template<value_type, std::allocator<value_type> > type;
    };
}
        
#endif