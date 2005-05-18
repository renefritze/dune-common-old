#ifndef DUNE_GCD_HH
#define DUNE_GCD_HH

#include"helpertemplates.hh"
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
      IsTrue<b<a>::yes();
      IsTrue<0<b>::yes();
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
