// $Id$
#ifndef DUNE_ALIGNMENT_HH
#define DUNE_ALIGNMENT_HH
#include<cstddef>
#if HAVE_TYPE_TRAITS
#include<type_traits>
#elif HAVE_TR1_TYPE_TRAITS
#include<tr1/type_traits>
#endif

namespace Dune
{
  
  /** @addtogroup Common
   *
   * @{
   */
  /**
   * @file 
   * @brief This file implements a template class to determine alignment
   * requirements at compile time.
   * @author Markus Blatt
   */

  namespace
  {
    
  /** 
   * @brief Helper class to meassure alignment requirement. 
   * @tparam T The type we want to meassure the alignment requirement for. 
   */
  template<class T>
  struct AlignmentStruct
  {
    char c;
    T t;
    void hack();
  };

  /** 
   * @brief Helper class to meassure alignment requirement. 
   * @tparam T The type we want to meassure the alignment requirement for. 
   */
  template<class T, std::size_t N>
  struct AlignmentHelper
  {
    enum { N2 = sizeof(AlignmentStruct<T>) - sizeof(T) - N };
    char padding1[N];
    T t;
    char padding2[N2];
  };

#define ALIGNMENT_MODULO(a, b)   (a % b == 0 ? \
                                     static_cast<std::size_t>(b) : \
                                     static_cast<std::size_t>(a % b))
#define ALIGNMENT_MIN(a, b)      (static_cast<std::size_t>(a) <   \
                                     static_cast<std::size_t>(b) ? \
                                     static_cast<std::size_t>(a) : \
                                     static_cast<std::size_t>(b))
    /**  @brief does the actual calculations. */
  template <class T, std::size_t N>
  struct AlignmentTester
  {
    typedef AlignmentStruct<T>        s;
    typedef AlignmentHelper<T, N>     h;
    typedef AlignmentTester<T, N - 1> next;
    enum
      {
	a1       = ALIGNMENT_MODULO(N        , sizeof(T)),
	a2       = ALIGNMENT_MODULO(h::N2    , sizeof(T)),
	a3       = ALIGNMENT_MODULO(sizeof(h), sizeof(T)),
	a        = sizeof(h) == sizeof(s) ? ALIGNMENT_MIN(a1, a2) : a3,
	result   = ALIGNMENT_MIN(a, next::result)
      };
  };

    /**  @brief does the actual calculations. */
  template <class T>
  struct AlignmentTester<T, 0>
  {
    enum 
      {
	result = ALIGNMENT_MODULO(sizeof(AlignmentStruct<T>), sizeof(T))
      };
  };
  } //end anonymous namspace

  /**
   * @brief Calculates the alignment requirement of a type.
   *
   * This will be a safe value and not an optimal one.
   * If TR1 is available it falls back to std::alignment_of.
   */
  template <class T>
  struct AlignmentOf
  {
    
    enum
      {
	/** @brief The alginment requirement. */
#ifdef HAVE_TYPE_TRAITS 
	value = std::alignment_of<T>::value
#elif HAVE_TR1_TYPETRAITS
	value = std::tr1::alignment_of<T>::value
#else
	value = AlignmentTester<T, sizeof(AlignmentStruct<T>) - sizeof(T) -1>::result
#endif
      };
  };
  
  /** @} */
}
#endif
