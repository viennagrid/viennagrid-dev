#ifndef VIENNAGRID_REMOVE_POINTER_HPP
#define VIENNAGRID_REMOVE_POINTER_HPP

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
