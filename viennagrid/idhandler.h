/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


  /******************* ID Handling *******************************/

#ifndef VIENNAMESH_IDHANDLER_GUARD
#define VIENNAMESH_IDHANDLER_GUARD

#include "viennamesh/forwards.h"

namespace viennamesh{

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
