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


#ifndef VIENNAGRID_IO_XML_TAG_HPP
#define VIENNAGRID_IO_XML_TAG_HPP

/** @file xml_tag.hpp
 *  @brief    A very simple parser for individual XML tags. Eases vtk_reader a bit.
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <string>


namespace viennagrid
{
  namespace io
  {
    
    //functor for conversion to lowercase (avoid ::tolower())
    char char_to_lower(char c)
    {
      if(c <= 'Z' && c >= 'A')
        return c - ('Z'-'z');
      return c;
    } 
    
    char to_lower(char c)
    {
      return char_to_lower(c);
    }
    
    std::string to_lower(std::string const & s)
    {
      std::string ret = s;
      std::transform(ret.begin(), ret.end(), ret.begin(), char_to_lower);
      //std::transform(s2.begin(), s2.end(), s2_lower.begin(), to_lower);
      
      return ret;
    }
  
    
    template <typename dummy = bool> 
    class xml_tag
    {
        typedef std::pair<std::string, std::string>  AttributeType;
        typedef std::list<AttributeType>             AttributeContainer;
      
      public:
        
        template <typename InputStream>
        void parse(InputStream & reader)
        {
          reader.unsetf(std::ios_base::skipws);
          clear();
          
          char c = ' ';
          
          //go to start of tag:
          while (c != '<' && reader.good())
            reader >> c;
          
          //read tag name:
          while ( (c != ' ' && c != '>') && reader.good() )
          {
            reader >> c;
            name_.append(1,  make_lower(c));
          }
          
          //strip whitespace or closing tag at the end
          name_.resize(name_.size()-1);
          
          std::cout << name_ << std::endl;
          
          if (c == '>') //we are all done
            return;
          
          //get attributes:
          bool end_of_attribute = false;
          bool inside_string = false;
          std::string token;
          while (c != '>' && reader.good())
          {
            reader >> c;

            if (inside_string && c == '"') //terminate string
            {
              end_of_attribute = true;
            }
            else if (!inside_string && c == '"') //start of string
            {
              inside_string = true;
            }
            else if (!inside_string && c == ' ')
            {
              //do nothing
              //throw bad_file_format_exception("filename", "Parse error: XML attribute ends prematurely.");
            }

            if (inside_string)
              token.append(1, c); //do not transform values to lower-case (e.g. filenames may get invalid)
            else if (c != ' ')
              token.append(1, make_lower(c));
            
            
            if (end_of_attribute)
            {
              std::size_t pos = token.find_first_of('=');
              if (pos == std::string::npos)
              {
                throw bad_file_format_exception("filename", "Parse error: XML attribute does not have '=' sign.");
              }
              else if (pos == 0)
              {
                throw bad_file_format_exception("filename", "Parse error: XML attribute name missing.");
              }
              
              
              std::string name = token.substr(0, pos);
              std::string value = token.substr(pos+2, token.size());
              
              pos = value.rfind('"');
              if (pos == std::string::npos || pos == 0)
                throw bad_file_format_exception("filename", "Internal XML parse error: XML attribute string not terminated.");
              
              std::cout << name << ":" << value.substr(0, pos) << std::endl;
              attributes_.push_back(std::make_pair(name, value.substr(0, pos)));
              
              token.clear();
              
              end_of_attribute = false;
              inside_string = false;
            }
          }
          
          reader.setf(std::ios_base::skipws);
        }

        void check_name(std::string const & expected_name,
                        std::string const & filename = std::string())
        {
          if (name_ != to_lower(expected_name))
          {
            std::stringstream ss;
            ss << "XML Parse error: " << expected_name << " expected, but got " << name_ << std::endl;
            throw bad_file_format_exception(filename,  ss.str());
          }
        }

        template <typename InputStream>
        void parse_and_check_name(InputStream & reader,
                                  std::string const & expected_name,
                                  std::string const & filename = std::string())
        {
          parse(reader);
          check_name(expected_name, filename);
        }
        
        std::string name() const { return name_; }
        
        bool has_attribute(std::string const & attrib_name) const
        {
          for (typename AttributeContainer::const_iterator it  = attributes_.begin();
                                                           it != attributes_.end();
                                                         ++it)
          {
            if (it->first == attrib_name)
              return true;
          }
          return false;
        }

        void check_attribute(std::string const & attrib_name,
                             std::string const & filename) const
        {
          if (!has_attribute(attrib_name))
          {
            std::stringstream ss;
            ss << "XML Parse error: Attribute " << attrib_name << " missing!" << std::endl;
            throw bad_file_format_exception(filename,  ss.str());
          }
        }        
        
        std::string get_value(std::string const & attrib_name) const
        {
          for (typename AttributeContainer::const_iterator it  = attributes_.begin();
                                                           it != attributes_.end();
                                                         ++it)
          {
            if (it->first == attrib_name)
              return it->second;
          }
          return "";
        }
        
        void clear()
        {
          name_ = std::string();
          attributes_.clear();
        }
      private:
        
        char make_lower(char c) const
        {
          if(c <= 'Z' && c >= 'A')
            return c - ('Z'-'z');
          return c;
        } 

        std::string name_;
        AttributeContainer attributes_;
    };
    

  } //namespace io
} //namespace viennagrid

#endif
