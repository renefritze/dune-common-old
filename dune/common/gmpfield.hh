#ifndef DUNE_GMPFIELD_HH
#define DUNE_GMPFIELD_HH

#include <iostream>

#if HAVE_GMP

#include <gmpxx.h>

namespace Dune
{

  template< unsigned int precision >
  class GMPField
  : public mpf_class
  {
    typedef mpf_class Base;

  public:
    GMPField ()
    : Base(0,precision)
    {}

    template< class T >
    GMPField ( const T &v )
    : Base( v,precision )
    {}

    /*
    GMPField &operator=(const GMPField &other)
    {
      Base(*this) = Base(other);
      return *this;
    }
    */
  };



  template< unsigned int precision >
  inline GMPField< precision >
  operator+ ( const GMPField< precision > &a, const GMPField< precision > &b )
  {
    typedef mpf_class F;
    return ((const F &)a + (const F &)b);
  }

  template< unsigned int precision >
  inline GMPField< precision >
  operator- ( const GMPField< precision > &a, const GMPField< precision > &b )
  {
    typedef mpf_class F;
    return ((const F &)a - (const F &)b);
  }

  template< unsigned int precision >
  inline GMPField< precision >
  operator- ( const GMPField< precision > &a )
  {
    typedef mpf_class F;
    return -((const F &)a);
  }

  template< unsigned int precision >
  inline GMPField< precision >
  operator* ( const GMPField< precision > &a, const GMPField< precision > &b )
  {
    typedef mpf_class F;
    return ((const F &)a * (const F &)b);
  }

  template< unsigned int precision >
  inline GMPField< precision >
  operator/ ( const GMPField< precision > &a, const GMPField< precision > &b )
  {
    typedef mpf_class F;
    return ((const F &)a / (const F &)b);
  }



  template< unsigned int precision >
  inline std::ostream &
  operator<< ( std::ostream &out, const GMPField< precision > &value )
  {
    return out << value.get_d();
  }

}

#endif // HAVE_GMP

#endif // #ifndef DUNE_MULTIPRECISION_HH