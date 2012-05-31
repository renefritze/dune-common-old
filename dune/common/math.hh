// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=4 sts=2:
#ifndef DUNE_MATH_HH
#define DUNE_MATH_HH

/** \file
 * \brief Some useful basic math stuff
 */

#include <cmath>

namespace Dune
{

  /**
     \brief Provides commonly used mathematical constants.
     
     a struct that is specialized for types repesenting real or complex
     numbers. I provides commonly used mathematical constants with the
     required accuary for the specified type.
   */
  template< class Field >
  struct MathematicalConstants;

  /**
     \brief Standard implementation of MathematicalConstants.

     This implementation will work with all built-in floating point
     types. It provides

     * e as std::exp(1.0)
     * pi as std::acos(-1.0)
     
  */
  template< class T >
  struct StandardMathematicalConstants
  {
    static T e ()
    {
      static const T e = std::exp( T( 1 ) );
      return e;
    }

    static T pi ()
    {
      static const T pi = std::acos( T( -1 ) );
      return pi;
    }
  };


#ifndef DOXYGEN
  // MathematicalConstants for float
  // -------------------------------

  template<>
  struct MathematicalConstants< float >
  : public StandardMathematicalConstants< float >
  {};



  // MathematicalConstants for double
  // --------------------------------

  template<>
  struct MathematicalConstants< double >
  : public StandardMathematicalConstants< double >
  {};



  // MathematicalConstants for long double
  // -------------------------------------

  template<>
  struct MathematicalConstants< long double >
  : public StandardMathematicalConstants< long double >
  {};
#endif // DOXYGEN

}

#endif // #ifndef DUNE_MATH_HH
