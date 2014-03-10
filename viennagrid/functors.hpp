#ifndef VIENNAGRID_FUNCTORS_HPP
#define VIENNAGRID_FUNCTORS_HPP

namespace viennagrid
{

  struct true_functor
  {
    typedef bool result_type;

    template<typename ValueT>
    bool operator()(ValueT const &) const
    { return true; }
  };


}


#endif
