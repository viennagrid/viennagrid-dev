/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_IO_HELPER_GUARD
#define VIENNAGRID_IO_HELPER_GUARD

#include <fstream>
#include <iostream>
#include <cctype>
#include <string>
#include "viennagrid/forwards.h"

namespace viennagrid
{
  namespace io
  {
    template <int dim>
    struct PointWriter
    {};

    template <>
    struct PointWriter<1>
    {
      template <typename PointType>
      static void write(std::ofstream & writer, PointType & point)
      {
        writer << point[0];
      }
    };
    
    template <>
    struct PointWriter<2>
    {
      template <typename PointType>
      static void write(std::ofstream & writer, PointType & point)
      {
        writer << point[0] << " " << point[1];
      }
    };
    
    template <>
    struct PointWriter<3>
    {
      template <typename PointType>
      static void write(std::ofstream & writer, PointType & point)
      {
        writer << point[0] << " " << point[1] << " " << point[2];
      }
    };
    
    struct strChecker
    {
      //******************************************
      // Checks, if a string is a number or not
      //******************************************
      static bool myIsNumber(std::string& str) {
        bool numberFlag = true;
        long strLen = str.size();
        long idx = 0;
        
        while(numberFlag == true && idx < strLen)
        {
          if(!isdigit(str[idx]))
          {
            numberFlag = false;
          }
          
          idx++;
        }
        
        return numberFlag;
      }
    };
    
  } //namespace io
} //namespace  viennagrid

#endif
