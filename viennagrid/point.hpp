/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


/************** All the code for a generic Point-Object ********************/


#ifndef VIENNAGRID_POINT_GUARD
#define VIENNAGRID_POINT_GUARD

#include <math.h>
#include "viennagrid/forwards.h"

namespace viennagrid
{
/***************************** Point Type ************************/


  /********************* Class implementations: *****************************/

  //namespace {

    //forward declarations:
    template <typename NumericT,
              typename DimTag>
    class point;


    //Helper class for overloaded operators:
    template <typename PointType,
              typename ScalarType = typename PointType::numeric_type,
              typename CoordType = typename PointType::numeric_type,
              int DIM = PointType::dimension_tag::value>
    struct point_operator_helper
    {
      static PointType multiply(const PointType & p1, const ScalarType scalar)
      {
        PointType ret;
        for (long i=0; i<DIM; ++i)
          ret.coords_[i] = p1.coords_[i] * scalar;
        return ret;
      }

      static CoordType multiply(const PointType & p1, const PointType & p2)
      {
        CoordType ret = 0.0;
        for (long i=0; i<DIM; ++i)
          ret += p1.coords_[i] * p2.coords_[i];
        return ret;
      }

      static PointType add(const PointType & p1, const PointType & p2)
      {
        PointType ret;
        for (long i=0; i<DIM; ++i)
          ret.coords_[i] = p1.coords_[i] + p2.coords_[i];
        return ret;
      }

      static PointType subtract(const PointType & p1, const PointType & p2)
      {
        PointType ret;
        for (long i=0; i<DIM; ++i)
          ret.coords_[i] = p1.coords_[i] - p2.coords_[i];
        return ret;
      }

      static bool equal(const PointType & p1, const PointType & p2)
      {
        //round-off errors may occur, take into account
        double norm = 0.0;
        for (long i=0; i<DIM; ++i)
          norm += fabs(p1.coords_[i]);

        if (norm == 0.0)
        {
          for (long i=0; i<DIM; ++i)
            norm += fabs(p2.coords_[i]);
          return norm < 1e-13;
        }

        double ret = 0.0;
        for (long i=0; i<DIM; ++i)
          ret += fabs( (p1.coords_[i] - p2.coords_[i]) / norm );
        return ret < 1e-13;
      }

      static bool less(const PointType & p1, const PointType & p2)
      {
        double norm_plus =  fabs(p1.coords_[0]) + fabs(p2.coords_[0]);
        double norm_minus =  fabs(p1.coords_[0] - p2.coords_[0]);

        if ( (norm_plus == 0.0) )
          return false;

        if ( norm_minus / norm_plus < 1e-10)    //fuzzy equal
          return false;

        for (long i=0; i<DIM; ++i)
        {
          if (p1.coords_[i] < p2.coords_[i])
            return true;
          if (p1.coords_[i] > p2.coords_[i])
            return false;
        }
        return false;
      }

      static void print(std::ostream & os, const PointType & p)
      {
        for (long i=0; i<DIM; ++i)
          os << p.coords_[i] << " ";
      }
    };

    //(probably unnecessary) specialisations

    template <typename PointType, typename ScalarType, typename CoordType>
    struct point_operator_helper<PointType, ScalarType, CoordType, 2>
    {
      static PointType multiply(const PointType & p1, const ScalarType scalar)
      {
        return PointType(p1.coords_[0] * scalar, p1.coords_[1] * scalar);
      }

      static CoordType multiply(const PointType & p1, const PointType & p2)
      {
        return p1.coords_[0] * p2.coords_[0] + p1.coords_[1] * p2.coords_[1];
      }

      static PointType add(const PointType & p1, const PointType & p2)
      {
        return PointType(p1.coords_[0] + p2.coords_[0], p1.coords_[1] + p2.coords_[1]);
      }

      static PointType subtract(const PointType & p1, const PointType & p2)
      {
        return PointType(p1.coords_[0] - p2.coords_[0], p1.coords_[1] - p2.coords_[1]);
      }

      static bool equal(const PointType & p1, const PointType & p2)
      {
        //round-off errors may occur, take into account
        double norm = fabs(p1.coords_[0]) + fabs(p1.coords_[1]);

        if (norm == 0.0)
          return fabs(p2.coords_[0]) + fabs(p2.coords_[1]) < 1e-13;

        return (fabs((p1.coords_[0] - p2.coords_[0]) / norm) < 1e-13)
             && (fabs((p1.coords_[1] - p2.coords_[1]) / norm) < 1e-13);
      }

      static bool less(const PointType & p1, const PointType & p2)
      {
        double norm_plus =  fabs(p1.coords_[0]) + fabs(p2.coords_[0])
                          + fabs(p1.coords_[1]) + fabs(p2.coords_[1]);
        double norm_minus =  fabs(p1.coords_[0] - p2.coords_[0])
                          + fabs(p1.coords_[1] - p2.coords_[1]);

        if ( (norm_plus == 0.0) )
          return false;

        if ( norm_minus / norm_plus < 1e-10)    //fuzzy equal
          return false;


        if (p1.coords_[0] < p2.coords_[0])
          return true;
        if (p1.coords_[0] > p2.coords_[0])
          return false;

        if (p1.coords_[1] < p2.coords_[1])
          return true;
        else
          return false;

        return false;
      }

      static void print(std::ostream & os, const PointType & p)
      {
        os << p.coords_[0] << " " << p.coords_[1];
      }

    };

    template <typename PointType,
              typename ScalarType,
              typename CoordType>
    struct point_operator_helper<PointType, ScalarType, CoordType, 3>
    {
      static PointType multiply(const PointType & p1, const ScalarType scalar)
      {
        return PointType(p1.coords_[0] * scalar, p1.coords_[1] * scalar, p1.coords_[2] * scalar);
      }

      static CoordType multiply(const PointType & p1, const PointType & p2)
      {
        return p1.coords_[0] * p2.coords_[0] + p1.coords_[1] * p2.coords_[1] + p1.coords_[2] * p2.coords_[2];
      }

      static PointType add(const PointType & p1, const PointType & p2)
      {
        return PointType(p1.coords_[0] + p2.coords_[0], p1.coords_[1] + p2.coords_[1], p1.coords_[2] + p2.coords_[2]);
      }

      static PointType subtract(const PointType & p1, const PointType & p2)
      {
        return PointType(p1.coords_[0] - p2.coords_[0], p1.coords_[1] - p2.coords_[1], p1.coords_[2] - p2.coords_[2]);
      }

      static bool equal(const PointType & p1, const PointType & p2)
      {
        //round-off errors may occur, take into account
        double norm = fabs(p1.coords_[0]) +
                      fabs(p1.coords_[1]) +
                      fabs(p1.coords_[2]);

        if (norm == 0.0)
        {
          return fabs(p2.coords_[0]) + fabs(p2.coords_[1]) + fabs(p2.coords_[2]) < 1e-13;
        }

        return (fabs((p1.coords_[0] - p2.coords_[0]) / norm) < 1e-13)
             && (fabs((p1.coords_[1] - p2.coords_[1]) / norm) < 1e-13)
             && (fabs((p1.coords_[2] - p2.coords_[2]) / norm) < 1e-13);
      }

      static bool less(const PointType & p1, const PointType & p2)
      {
        double norm_plus =  fabs(p1.coords_[0]) + fabs(p2.coords_[0])
                          + fabs(p1.coords_[1]) + fabs(p2.coords_[1])
                          + fabs(p1.coords_[2]) + fabs(p2.coords_[2]);
        double norm_minus =  fabs(p1.coords_[0] - p2.coords_[0])
                           + fabs(p1.coords_[1] - p2.coords_[1])
                           + fabs(p1.coords_[2] - p2.coords_[2]);

        if ( (norm_plus == 0.0) )
          return false;

        if ( norm_minus / norm_plus < 1e-12)    //fuzzy equal
          return false;

        if (p1.coords_[0] < p2.coords_[0])
          return true;
        if (p1.coords_[0] > p2.coords_[0])
          return false;

        if (p1.coords_[1] < p2.coords_[1])
          return true;
        if (p1.coords_[1] > p2.coords_[1])
          return false;

        if (p1.coords_[2] < p2.coords_[2])
          return true;

        return false;
      }

      static void print(std::ostream & os, const PointType & p)
      {
        os << p.coords_[0] << " " << p.coords_[1] << " " << p.coords_[2];
      }

    };

    //overloaded operators:
    template <typename T, typename DTag, typename ScalarType>
    point<T, DTag> operator*(const point<T, DTag> & p1, ScalarType scalar)
    {
      return point_operator_helper<point<T, DTag>, ScalarType>::multiply(p1, scalar);
    }
  
    template <typename T, typename DTag>
    typename point<T, DTag>::CoordType operator*(const point<T, DTag> & p1, const point<T, DTag> & p2)
    {
      return point_operator_helper<point<T, DTag> >::multiply(p1, p2);
    }
  
    template <typename T, typename DTag>
    point<T, DTag> operator+(const point<T, DTag> & p1, const point<T, DTag> & p2)
    {
      return point_operator_helper<point<T, DTag> >::add(p1, p2);
    }
  
    template <typename T, typename DTag>
    point<T, DTag> operator-(const point<T, DTag> & p1, const point<T, DTag> & p2)
    {
      return point_operator_helper<point<T, DTag> >::subtract(p1, p2);
    }

    template <typename T, typename DTag>
    bool operator==(const point<T, DTag> & p1, const point<T, DTag> & p2)
    {
      return point_operator_helper<point<T, DTag> >::equal(p1, p2);
    }
  
    template <typename T, typename DTag>
    bool operator<(const point<T, DTag> & p1, const point<T, DTag> & p2)
    {
      return point_operator_helper<point<T, DTag> >::less(p1, p2);
    }

    template <typename T, typename DTag>
    std::ostream& operator << (std::ostream& os, const point<T, DTag> & p)
    {
      point_operator_helper<point<T, DTag> >::print(os, p);
      return os;
    }

    template <typename NumericT, typename DimTag>
    class point_base
    {
      public:
        typedef NumericT numeric_type;
        typedef DimTag  dimension_tag;

        point_base() {};

        //copy constructor:
        point_base(const point_base & p2)
        {
          for (long i=0; i<dimension_tag::value; ++i)
            coords_[i] = p2.coords_[i];
        }

        void setCoordinates(numeric_type *array){
          for (int i=0; i<dimension_tag::value; ++i)
            coords_[i] = array[i];
        }

        numeric_type getCoordinate(long i) const
        { return coords_[i]; }
        void setCoordinate(long i, numeric_type val) { coords_[i] = val; }

        numeric_type get_x() const { return coords_[0]; };


        //operators:
        template <typename PointType, typename ScalarType, typename CoordType, int DIM>
        friend struct point_operator_helper;

        template <typename T, typename DTag, typename ScalarType>
        friend point<T, DTag> operator*(const point<T, DTag> &, ScalarType);

        template <typename T, typename DTag>
        friend typename point<T, DTag>::numeric_type operator*(const point<T, DTag> &, const point<T, DTag> &);
  
        template <typename T, typename DTag>
        friend point<T, DTag> operator+(const point<T, DTag> &, const point<T, DTag> &);
        template <typename T, typename DTag>
        friend point<T, DTag> operator-(const point<T, DTag> &, const point<T, DTag> &);

        template <typename T, typename DTag>
        friend bool operator==(const point<T, DTag> &, const point<T, DTag> &);

        template <typename T, typename DTag>
        friend bool operator< (const point<T, DTag> &, const point<T, DTag> &);
  
        template <typename T, typename DTag>
        friend std::ostream& operator<< (std::ostream &, const point<T, DTag> &);

        point_base & operator+=(const point_base & p)
        {
          for (long i=0; i<dimension_tag::value; ++i)
            coords_[i] += p.coords_[i];
          return *this;
        }

        point_base & operator+=(numeric_type offset)
        {
          for (long i=0; i<dimension_tag::value; ++i)
            coords_[i] += offset;
          return *this;
        }

        point_base & operator-=(const point_base & p)
        {
          for (long i=0; i<dimension_tag::value; ++i)
            coords_[i] -= p.coords_[i];
          return *this;
        }

        point_base & operator/=(numeric_type val)
        {
          for (long i=0; i<dimension_tag::value; ++i)
            coords_[i] /= val;
          return *this;
        }

        point_base & operator*=(numeric_type val)
        {
          for (long i=0; i<dimension_tag::value; ++i)
            coords_[i] *= val;
          return *this;
        }

        numeric_type length() const
        {
          numeric_type len = 0.0;

          for (long i=0; i<dimension_tag::value; ++i)
            len += coords_[i] * coords_[i];

          return sqrt(len);
        }

      protected:
        numeric_type coords_[dimension_tag::value];

    };

  //}


  template <typename T_Coord, typename DimensionsTag>
  class point //: public point_t_base<T_Coord, DimensionsTag>
  {  };

  template <typename T_Coord>
  class point<T_Coord, one_dimension_tag> : public point_base<T_Coord, one_dimension_tag>
  {
    typedef point_base<T_Coord, one_dimension_tag>       base;

    public:

      point(T_Coord x = 0)
      {
        base::coords_[0] = x;
      }
      point(const point & p2): base(p2) {};

      T_Coord get_x() const { return base::coords_[0]; };

      void set_x(T_Coord value) { base::coords_[0] = value; };

      point operator=(const point & p2)
      {
        base::coords_[0] = p2.coords_[0];
        return *this;
      };

  };

  template <typename T_Coord>
  class point<T_Coord, two_dimensions_tag> : public point_base<T_Coord, two_dimensions_tag>
  {
    typedef point_base<T_Coord, two_dimensions_tag>       base;

    public:

      point(T_Coord x = 0, T_Coord y = 0, T_Coord dummy = 0)
      {
        base::coords_[0] = x;
        base::coords_[1] = y;
      }

      point(const point & p2): base(p2) {};


      T_Coord get_y() const { return base::coords_[1]; };

      void set_x(T_Coord value) { base::coords_[0] = value; };
      void set_y(T_Coord value) { base::coords_[1] = value; };

      template <typename T>
      friend double spannedVolume(const point<T, two_dimensions_tag> & p1,
                                    const point<T, two_dimensions_tag> & p2);

      point operator=(const point & p2)
      {
        base::coords_[0] = p2.coords_[0];
        base::coords_[1] = p2.coords_[1];
        return *this;
      };

    private:
  };

  template <typename T_Coord>
  class point<T_Coord, three_dimensions_tag> : public point_base<T_Coord, three_dimensions_tag>
  {
    typedef point_base<T_Coord, three_dimensions_tag>       base;

    public:

      point(T_Coord x = 0, T_Coord y = 0, T_Coord z = 0)
      {
        base::coords_[0] = x;
        base::coords_[1] = y;
        base::coords_[2] = z;
      };

      point(const point & p2): base(p2) {};

      T_Coord get_y() const { return base::coords_[1]; };
      T_Coord get_z() const { return base::coords_[2]; };

      void set_x(T_Coord value) { base::coords_[0] = value; };
      void set_y(T_Coord value) { base::coords_[1] = value; };
      void set_z(T_Coord value) { base::coords_[2] = value; };

      point operator=(const point & p2)
      {
        base::coords_[0] = p2.coords_[0];
        base::coords_[1] = p2.coords_[1];
        base::coords_[2] = p2.coords_[2];
        return *this;
      };

      template <typename T>
      friend double spannedVolume(const point<T, three_dimensions_tag> & p1,
                                    const point<T, three_dimensions_tag> & p2);
      template <typename T>
      friend double spannedVolume(const point<T, three_dimensions_tag> & p1,
                                  const point<T, three_dimensions_tag> & p2,
                                  const point<T, three_dimensions_tag> & p3);


  };

  //calculates the volume of rhomboid induced by the vectors p1 and p2:
  // 'ordinary 2D'
  template <typename T>
  double spannedVolume(const point<T, two_dimensions_tag> & p1,
                       const point<T, two_dimensions_tag> & p2)
  {
    return p1.coords_[0] * p2.coords_[1] - p1.coords_[1] * p2.coords_[0];
  }

  // '2D-object in 3D'
  template <typename T>
  double spannedVolume(const point<T, three_dimensions_tag> & p1,
                       const point<T, three_dimensions_tag> & p2)
  {
    double c1 = p1.coords_[0] * p2.coords_[1] - p1.coords_[1] * p2.coords_[0];
    double c2 = - p1.coords_[0] * p2.coords_[2] + p1.coords_[2] * p2.coords_[0];
    double c3 = p1.coords_[1] * p2.coords_[2] - p1.coords_[2] * p2.coords_[1];

    return sqrt( c1 * c1 + c2 * c2 + c3 * c3 );
  }

  template <typename T>
  double spannedVolume(const point<T, three_dimensions_tag> & p1,
                       const point<T, three_dimensions_tag> & p2,
                       const point<T, three_dimensions_tag> & p3)
  {
    //return det|p1 p2 p3|
    return p1.coords_[0] * p2.coords_[1] * p3.coords_[2]
          + p2.coords_[0] * p3.coords_[1] * p1.coords_[2]
          + p3.coords_[0] * p1.coords_[1] * p2.coords_[2]
          - p1.coords_[2] * p2.coords_[1] * p3.coords_[0]
          - p2.coords_[2] * p3.coords_[1] * p1.coords_[0]
          - p3.coords_[2] * p1.coords_[1] * p2.coords_[0];
  }

  
}
#endif
