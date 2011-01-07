/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


#ifndef VIENNAMESH_IO_HELPER_GUARD
#define VIENNAMESH_IO_HELPER_GUARD

#include <fstream>
#include <iostream>
#include <string>
#include "viennamesh/forwards.h"

namespace viennamesh
{
  
  template <typename PointType, int dim>
  struct PointWriter
  {};

  template <typename PointType>
  struct PointWriter<PointType, 1>
  {
    static void write(std::ofstream & writer, PointType & point)
    {
      writer << point.get_x();
    }
  };
  
  template <typename PointType>
  struct PointWriter<PointType, 2>
  {
    static void write(std::ofstream & writer, PointType & point)
    {
      writer << point.get_x() << " " << point.get_y();
    }
  };
  
  template <typename PointType>
  struct PointWriter<PointType, 3>
  {
    static void write(std::ofstream & writer, PointType & point)
    {
      writer << point.get_x() << " " << point.get_y() << " " << point.get_z();
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
  
} //namespace 

#endif
