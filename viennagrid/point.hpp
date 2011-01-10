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


//   template <typename T_Coord>
//   class point_t<T_Coord, TwoDimensionsTag>;
// 
//   template <typename T_Coord>
//   class point_t<T_Coord, ThreeDimensionsTag>;

  /********************* Class implementations: *****************************/

  //namespace {

    //forward declarations:
    template <typename T_Coord, typename DimensionsTag>
    class point_t;


    //Helper class for overloaded operators:
    template <typename PointType, typename ScalarType = typename PointType::CoordType, typename CoordType = typename PointType::CoordType, int DIM = PointType::DimensionsTag::dim>
    struct point_t_operator_helper
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
    struct point_t_operator_helper<PointType, ScalarType, CoordType, 2>
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

    template <typename PointType, typename ScalarType, typename CoordType>
    struct point_t_operator_helper<PointType, ScalarType, CoordType, 3>
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
    point_t<T, DTag> operator*(const point_t<T, DTag> & p1, ScalarType scalar)
    {
      return point_t_operator_helper<point_t<T, DTag>, ScalarType>::multiply(p1, scalar);
    }
  
    template <typename T, typename DTag>
    typename point_t<T, DTag>::CoordType operator*(const point_t<T, DTag> & p1, const point_t<T, DTag> & p2)
    {
      return point_t_operator_helper<point_t<T, DTag> >::multiply(p1, p2);
    }
  
    template <typename T, typename DTag>
    point_t<T, DTag> operator+(const point_t<T, DTag> & p1, const point_t<T, DTag> & p2)
    {
      return point_t_operator_helper<point_t<T, DTag> >::add(p1, p2);
    }
  
    template <typename T, typename DTag>
    point_t<T, DTag> operator-(const point_t<T, DTag> & p1, const point_t<T, DTag> & p2)
    {
      return point_t_operator_helper<point_t<T, DTag> >::subtract(p1, p2);
    }

    template <typename T, typename DTag>
    bool operator==(const point_t<T, DTag> & p1, const point_t<T, DTag> & p2)
    {
      return point_t_operator_helper<point_t<T, DTag> >::equal(p1, p2);
    }
  
    template <typename T, typename DTag>
    bool operator<(const point_t<T, DTag> & p1, const point_t<T, DTag> & p2)
    {
      return point_t_operator_helper<point_t<T, DTag> >::less(p1, p2);
    }

    template <typename T, typename DTag>
    std::ostream& operator << (std::ostream& os, const point_t<T, DTag> & p)
    {
      point_t_operator_helper<point_t<T, DTag> >::print(os, p);
      return os;
    }

    template <typename T_Coord, typename DimTag>
    class point_t_base
    {
      public:
        typedef T_Coord CoordType;
        typedef DimTag  DimensionsTag;

        point_t_base() {};

        //copy constructor:
        point_t_base(const point_t_base & p2)
        {
          for (long i=0; i<DimensionsTag::dim; ++i)
            coords_[i] = p2.coords_[i];
        }

        void setCoordinates(CoordType *array){
          for (int i=0; i<DimensionsTag::dim; ++i)
            coords_[i] = array[i];
        }

        T_Coord getCoordinate(long i) const
        { return coords_[i]; }
        void setCoordinate(long i, T_Coord val) { coords_[i] = val; }

        T_Coord get_x() const { return coords_[0]; };


        //operators:
        template <typename PointType, typename ScalarType, typename CoordType, int DIM>
        friend struct point_t_operator_helper;

        template <typename T, typename DTag, typename ScalarType>
        friend point_t<T, DTag> operator*(const point_t<T, DTag> &, ScalarType);

        template <typename T, typename DTag>
        friend typename point_t<T, DTag>::CoordType operator*(const point_t<T, DTag> &, const point_t<T, DTag> &);
  
        template <typename T, typename DTag>
        friend point_t<T, DTag> operator+(const point_t<T, DTag> &, const point_t<T, DTag> &);
        template <typename T, typename DTag>
        friend point_t<T, DTag> operator-(const point_t<T, DTag> &, const point_t<T, DTag> &);

        template <typename T, typename DTag>
        friend bool operator==(const point_t<T, DTag> &, const point_t<T, DTag> &);

        template <typename T, typename DTag>
        friend bool operator< (const point_t<T, DTag> &, const point_t<T, DTag> &);
  
        template <typename T, typename DTag>
        friend std::ostream& operator<< (std::ostream &, const point_t<T, DTag> &);

        point_t_base & operator+=(const point_t_base & p)
        {
          for (long i=0; i<DimensionsTag::dim; ++i)
            coords_[i] += p.coords_[i];
          return *this;
        }

        point_t_base & operator+=(T_Coord offset)
        {
          for (long i=0; i<DimensionsTag::dim; ++i)
            coords_[i] += offset;
          return *this;
        }

        point_t_base & operator-=(const point_t_base & p)
        {
          for (long i=0; i<DimensionsTag::dim; ++i)
            coords_[i] -= p.coords_[i];
          return *this;
        }

        point_t_base & operator/=(T_Coord val)
        {
          for (long i=0; i<DimensionsTag::dim; ++i)
            coords_[i] /= val;
          return *this;
        }

        point_t_base & operator*=(T_Coord val)
        {
          for (long i=0; i<DimensionsTag::dim; ++i)
            coords_[i] *= val;
          return *this;
        }

        CoordType length() const
        {
          CoordType len = 0.0;

          for (long i=0; i<DimensionsTag::dim; ++i)
            len += coords_[i] * coords_[i];

          return sqrt(len);
        }

      protected:
        CoordType coords_[DimensionsTag::dim];

    };

  //}


  template <typename T_Coord, typename DimensionsTag>
  class point_t //: public point_t_base<T_Coord, DimensionsTag>
  {  };

  template <typename T_Coord>
  class point_t<T_Coord, OneDimensionTag> : public point_t_base<T_Coord, OneDimensionTag>
  {
    typedef point_t_base<T_Coord, OneDimensionTag>       base;

    public:

      point_t(T_Coord x = 0)
      {
        base::coords_[0] = x;
      }
      point_t(const point_t & p2): base(p2) {};

      T_Coord get_x() const { return base::coords_[0]; };

      void set_x(T_Coord value) { base::coords_[0] = value; };

      point_t operator=(const point_t & p2)
      {
        base::coords_[0] = p2.coords_[0];
        return *this;
      };

  };

  template <typename T_Coord>
  class point_t<T_Coord, TwoDimensionsTag> : public point_t_base<T_Coord, TwoDimensionsTag>
  {
    typedef point_t_base<T_Coord, TwoDimensionsTag>       base;

    public:

      point_t(T_Coord x = 0, T_Coord y = 0, T_Coord dummy = 0)
      {
        base::coords_[0] = x;
        base::coords_[1] = y;
      }

      point_t(const point_t & p2): base(p2) {};


      T_Coord get_y() const { return base::coords_[1]; };

      void set_x(T_Coord value) { base::coords_[0] = value; };
      void set_y(T_Coord value) { base::coords_[1] = value; };

      template <typename T>
      friend double spannedVolume(const point_t<T, TwoDimensionsTag> & p1,
                                    const point_t<T, TwoDimensionsTag> & p2);

      point_t operator=(const point_t & p2)
      {
        base::coords_[0] = p2.coords_[0];
        base::coords_[1] = p2.coords_[1];
        return *this;
      };

    private:
  };

  template <typename T_Coord>
  class point_t<T_Coord, ThreeDimensionsTag> : public point_t_base<T_Coord, ThreeDimensionsTag>
  {
    typedef point_t_base<T_Coord, ThreeDimensionsTag>       base;

    public:

      point_t(T_Coord x = 0, T_Coord y = 0, T_Coord z = 0)
      {
        base::coords_[0] = x;
        base::coords_[1] = y;
        base::coords_[2] = z;
      };

      point_t(const point_t & p2): base(p2) {};

      T_Coord get_y() const { return base::coords_[1]; };
      T_Coord get_z() const { return base::coords_[2]; };

      void set_x(T_Coord value) { base::coords_[0] = value; };
      void set_y(T_Coord value) { base::coords_[1] = value; };
      void set_z(T_Coord value) { base::coords_[2] = value; };

      point_t operator=(const point_t & p2)
      {
        base::coords_[0] = p2.coords_[0];
        base::coords_[1] = p2.coords_[1];
        base::coords_[2] = p2.coords_[2];
        return *this;
      };

      template <typename T>
      friend double spannedVolume(const point_t<T, ThreeDimensionsTag> & p1,
                                    const point_t<T, ThreeDimensionsTag> & p2);
      template <typename T>
      friend double spannedVolume(const point_t<T, ThreeDimensionsTag> & p1, const point_t<T, ThreeDimensionsTag> & p2, const point_t<T, ThreeDimensionsTag> & p3);


  };

  //calculates the volume of rhomboid induced by the vectors p1 and p2:
  // 'ordinary 2D'
  template <typename T>
  double spannedVolume(const point_t<T, TwoDimensionsTag> & p1,
                       const point_t<T, TwoDimensionsTag> & p2)
  {
    return p1.coords_[0] * p2.coords_[1] - p1.coords_[1] * p2.coords_[0];
  }

  // '2D-object in 3D'
  template <typename T>
  double spannedVolume(const point_t<T, ThreeDimensionsTag> & p1,
                       const point_t<T, ThreeDimensionsTag> & p2)
  {
    double c1 = p1.coords_[0] * p2.coords_[1] - p1.coords_[1] * p2.coords_[0];
    double c2 = - p1.coords_[0] * p2.coords_[2] + p1.coords_[2] * p2.coords_[0];
    double c3 = p1.coords_[1] * p2.coords_[2] - p1.coords_[2] * p2.coords_[1];

    return sqrt( c1 * c1 + c2 * c2 + c3 * c3 );
  }

  template <typename T>
  double spannedVolume(const point_t<T, ThreeDimensionsTag> & p1,
                       const point_t<T, ThreeDimensionsTag> & p2,
                       const point_t<T, ThreeDimensionsTag> & p3)
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
