// $ Id $
#ifndef DUNE_LOCICTRAITS_H
#define DUNE_LOCICTRAITS_H

namespace Dune{ 
  /** @addtogroup Common
   *
   * @{
   */
  template<bool T1, bool T2>
  struct And
  {
    const static bool value=T1 && T2;
  };
  
  template<bool T1, bool T2>
  struct Or
  {
    const static bool value=T1 || T2;
  };
  
  /** @} */
}
#endif
