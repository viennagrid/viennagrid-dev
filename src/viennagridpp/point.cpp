#include "viennagrid/core/point.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"


namespace viennagrid
{

  std::ostream& operator<< (std::ostream & os, point const & p)
  {
    typedef point::size_type      size_type;
    os << "(";
    for (size_type i=0; i< p.size(); ++i)
      os << p[i] << (i == p.size()-1 ? "" :" ");
    os << ")";
    return os;
  }

  std::istream& operator>> (std::istream & is, point & pt)
  {
    std::string tmp;
    char c;
    while (is.get(c))
    {
      if (c == '(')
        break;
    }

    if (c != '(')
      return is;

    while (is.get(c))
    {
      if (c == ')')
        break;
      else
        tmp += c;
    }

    typedef std::vector<std::string> StringContainerType;
    StringContainerType elements;
    boost::algorithm::split( elements, tmp, boost::is_any_of("(),") );

    pt.clear();
    for (StringContainerType::const_iterator eit = elements.begin(); eit != elements.end(); ++eit)
      pt.push_back( boost::lexical_cast<viennagrid_numeric>(*eit) );

    return is;
  }

}
