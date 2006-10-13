// $Id$

#ifndef DUNE_COMMON_HELPERTEMPLATES
#define DUNE_COMMON_HELPERTEMPLATES

/** \file
    \brief Compile-time constraint checks from the Stroustrup FAQ
 */

/**
  Derived_from<A,B>();

  or
  
  template<class T> class Container : Derived_from<T,Mybase> {
    ...
  };
*/
template<class T, class B> struct Derived_from {
    static void constraints(T* p) { B* pb = p; }
    Derived_from() { void(*p)(T*) = constraints; }
};

/** \todo Please doc me! */
template<class T1, class T2> struct Can_copy {
    static void constraints(T1 a, T2 b) { T2 c = a; b = a; }
    Can_copy() { void(*p)(T1,T2) = constraints; }
};

/** \todo Please doc me! */
template<class T1, class T2 = T1> struct Can_compare {
    static void constraints(T1 a, T2 b) { a==b; a!=b; a<b; }
    Can_compare() { void(*p)(T1,T2) = constraints; }
};

/** \todo Please doc me! */
template<class T1, class T2, class T3 = T1> struct Can_multiply {
    static void constraints(T1 a, T2 b, T3 c) { c = a*b; }
    Can_multiply() { void(*p)(T1,T2,T3) = constraints; }
};

/** 
    \brief Helper template so that compilation fails if condition is not true.
    
    If the condition is true a static function yes is available, othewise the
    only function available is no().

    Example for compile time check whether two types are the same:
    \code
    IsTrue<SameType<int,int>::value>::yes(); // 
    IsTrue<SameType<bool,int>::value>::yes(); // false, will trigger a compile time error
    \endcode
    
    A test that trigger a compile time error if condition is true:
    \code
    IsTrue<condition>::no()
    \endcode
*/
template <bool condition>
struct IsTrue
{
  static void no() {};
};

template <>
struct IsTrue<true>
{
  static void yes() {};
};

#endif
