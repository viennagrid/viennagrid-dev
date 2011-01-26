/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


  /******************* ID Handling *******************************/

#ifndef VIENNAGRID_IDHANDLER_GUARD
#define VIENNAGRID_IDHANDLER_GUARD

#include "viennagrid/forwards.h"

namespace viennagrid
{

  struct NoID
  {
    NoID() {};
    //NoID(long id) {};

    //for compatibility:
    void setID(long id) { };
    const NoID * getID() const { return this; };

  };

  class ProvideID
  {
    public:
      ProvideID() : id_(-1) {};
      //ProvideID(long id): id_(id) {}

      long getID() const { return id_; };
      void setID(long id) { id_ = id; };

    protected:
      long id_;
  };

}

#endif
