#ifndef VIENNAGRID_REMOVE_POINTER_HPP
#define VIENNAGRID_REMOVE_POINTER_HPP

namespace viennagrid
{
    namespace utils
    {

        template<typename pointer_type>
        struct remove_pointer;

        template<typename pointer_type>
        struct remove_pointer<pointer_type*>
        {
            typedef pointer_type type;
        };


    }

}

#endif
