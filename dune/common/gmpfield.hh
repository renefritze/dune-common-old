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

    // type conversion operators
    operator double () const
    {
      return this->get_d();
    }

    operator float () const
    {
      return this->get_d();
    }

    operator mpf_class () const
    {
      return static_cast<const mpf_class&>(*this);
    }

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
    return out << static_cast<const mpf_class&>(value);
  }

}

namespace std
{
  template< unsigned int precision >
  inline Dune::GMPField< precision >
  sqrt ( const Dune::GMPField< precision > &a )
  {
    return Dune::GMPField< precision >(sqrt(static_cast<const mpf_class&>(a)));
  }
}

#endif // HAVE_GMP

#endif // #ifndef DUNE_MULTIPRECISION_HH
