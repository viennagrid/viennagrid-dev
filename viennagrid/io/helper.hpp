/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#ifndef VIENNAGRID_IO_HELPER_GUARD
#define VIENNAGRID_IO_HELPER_GUARD

#include <fstream>
#include <sstream>
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
      static void write(std::ofstream & writer, PointType const& point)
      {
        writer << point[0];
      }    
    
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
      static void write(std::ofstream & writer, PointType const& point)
      {
        writer << point[0] << " " << point[1];
      }    
    
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
      static void write(std::ofstream & writer, PointType const& point)
      {
        writer << point[0] << " " << point[1] << " " << point[2];
      }

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
    
    
    
    
    
    //
    // IO Exception
    //
    class cannot_open_file_exception : public std::exception
    {
      public:
        virtual const char* what() const throw()
        {
          std::stringstream ss;
          ss << "* ViennaGrid: Cannot open file " << filename_ << "!";
          return ss.str().c_str();
        }
        
        cannot_open_file_exception(std::string file) : filename_(file) {};
        
        virtual ~cannot_open_file_exception() throw() {};
      
      private:
        std::string filename_;
    };
    
    class bad_file_format_exception : public std::exception
    {
      public:
        virtual const char* what() const throw()
        {
          std::stringstream ss;
          if (filename_.size() > 0)
            ss << "* ViennaGrid: Bad file format in file " << filename_ << ": " << message_;
          else
            ss << "* ViennaGrid: Bad file format: " << message_;
          return ss.str().c_str();
        }
        
        bad_file_format_exception(std::string file, std::string message) : filename_(file), message_(message) {};
        bad_file_format_exception(std::string message) : filename_(), message_(message) {};
        
        virtual ~bad_file_format_exception() throw() {};
      
      private:
        std::string filename_;
        std::string message_;
    };
    
    
  } //namespace io
} //namespace  viennagrid

#endif
