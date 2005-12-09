#ifndef DUNE_ENUMSET_HH
#define DUNE_ENUMSET_HH

#include<iostream>

namespace Dune
{
   /**
   * @file
   * @brief Classes for building sets out of enumeration values.
   * @author Markus Blatt
   */
  /** @addtogroup Common
   *
   * @{
   */

  /**
   * @brief An empty set.
   */
  template<typename TA>
  class EmptySet
  {
  public:
    /**
     * @brief The POD type the set holds.
     */
    typedef TA Type;
    /**
     * @brief Always returns false.
     */
    static bool contains(const Type& attribute);
  };
  
  /**
   * @brief A set consisting only of one item.
   */
  template<typename TA, int item>
  class EnumItem 
  {
  public:
    /**
     * @brief The type the set holds.
     */
    typedef TA Type;
    
    /**
     * @brief Tests whether an item is in the set.
     * @return True if item==Type.
     */
    static bool contains(const Type& attribute);
  };

  /**
   * @brief A set representing a range including the borders.
   */
  template<typename T,int from, int end>
  class EnumRange //: public PODSet<EnumRange<T,from,end>,T>
  {
  public:
    /**
     * @brief The type the set holds.
     */
    typedef T Type;
    static bool contains(const T& item);
  };

  /**
   * @brief The negation of a set.
   * An item is contained in the set if and only if it is not
   * contained in the negated set.
   */
  template<typename S>
  class NegateSet
  {
  public:
    typedef typename S::Type Type;
  
    static bool contains(const Type& item)
    {
      return !S::contains(item);
    }
  };
  
  /**
   * @brief A set combining two other sets.
   */
  template<class TI1, class TI2, typename TA=typename TI1::Type>
  class Combine 
  {
  public:
    static bool contains(const TA& item);
  };
  
  template<typename TA>
  inline bool EmptySet<TA>::contains(const TA& attribute)
  {
    return false;
  }

  template<typename TA,int i>
  inline bool EnumItem<TA,i>::contains(const TA& item)
  {
    return item==i;
  }

  template<typename TA,int i>
  inline std::ostream& operator<<(std::ostream& os, const EnumItem<TA,i>&)
  {
    return os<<i;
  }

  template<typename TA, int from, int to>
  inline bool EnumRange<TA,from,to>::contains(const TA& item)
  {
    return from<=item && item<=to;
  }
  
  template<typename TA, int from, int to>
  inline std::ostream& operator<<(std::ostream& os, const EnumRange<TA,from,to>&)
  {
    return os<<"["<<from<<" - "<<to<<"]";
  }
  
  template<class TI1, class TI2, typename TA>
  inline bool Combine<TI1,TI2,TA>::contains(const TA& item)
  {
    return TI1::contains(item) ||
      TI2::contains(item);
  }
    
  template<class TI1, class TI2>
  inline Combine<TI1,TI2,typename TI1::Type> combine(const TI1& set1, const TI2& set2)
  {
    return Combine<TI1,TI2,typename TI1::Type>();
  }  

  template<class TI1, class TI2, class T>
  inline std::ostream& operator<<(std::ostream& os, const Combine<TI1,TI2,T>&)
  {
    return os << TI1()<<" "<<TI2();
  }
  /** @} */
}

#endif
