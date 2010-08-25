#ifndef DUNE_GCD_HH
#define DUNE_GCD_HH

#include"static_assert.hh"
namespace Dune
{
  /** 
   * @addtogroup Common 
   * @{
   */
  /**
   * @file
   * This file provides template constructs for calculation the 
   * greatest common divisor.
   */

#ifndef DOXYGEN
  /**
   * @brief Helper for calculating the gcd.
   */
  template<long a, long b, bool bo>
  struct GcdHelper
  {};
  
  
  template<long a, long b>
  struct GcdHelper<a,b,true>
  {
    /**
     * @brief Check that a>b.
     */
    static void conceptCheck()
    {
      dune_static_assert(b<a, "b<a must hold!");
      dune_static_assert(0<b, "b must be positive");
    }

    
    /**
     * @brief The greatest common divisor of the numbers a and b.
     */
     const static long gcd = GcdHelper<b,a%b,true>::gcd;
  };
  
  template<long a, long b>
  struct GcdHelper<a,b,false>
  {
    /**
     * @brief The greatest common divisor of the numbers a and b.
     */
    const static long gcd = GcdHelper<b,a,true>::gcd;
  };
  template<long a>
  struct GcdHelper<a,0,true>
  {
    const static long gcd=a;
  };
  
#endif
  
  /**
   * @brief Calculator of the greatest common divisor.
   */
  template<long a, long b>
  struct Gcd
  {
    /**
     * @brief The greatest common divisior of a and b. */
    const static long value = GcdHelper<a,b,(a>b)>::gcd;
  };
  
  /**
   * @}
   */
}

#endif
