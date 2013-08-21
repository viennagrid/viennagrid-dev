#ifndef VIENNAGRID_META_ALGORITHM_HPP
#define VIENNAGRID_META_ALGORITHM_HPP

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
  namespace meta
  {
    namespace typelist
    {
        template<typename typelist>
        struct for_each_impl {};

        template<typename head, typename tail>
        struct for_each_impl< viennagrid::meta::typelist_t<head, tail> >
        {
            template<typename functor>
            static void exec( functor & f)
            {
                f( tag<head>() );
                for_each_impl<tail>::exec(f);
            }

            template<typename functor>
            static void exec( const functor & f)
            {
                f( tag<head>() );
                for_each_impl<tail>::exec(f);
            }
        };

        template<>
        struct for_each_impl< viennagrid::meta::null_type >
        {
            template<typename functor> static void exec( functor & ) {}
            template<typename functor> static void exec( const functor & ) {}
        };


        template<typename typelist, typename functor>
        void for_each(functor & f)
        { for_each_impl<typelist>::exec(f); }

        template<typename typelist, typename functor>
        void for_each(const functor & f)
        { for_each_impl<typelist>::exec(f); }




        template<template<typename> class functor, typename typelist>
        struct TRANSFORM;


        template<template<typename> class functor>
        struct TRANSFORM<functor, viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<template<typename> class functor, typename head, typename tail>
        struct TRANSFORM< functor, viennagrid::meta::typelist_t<head, tail> >
        {
            typedef viennagrid::meta::typelist_t<
                typename functor<head>::type,
                typename TRANSFORM<functor, tail>::type
            > type;

        };

    }
  }
}

#endif
