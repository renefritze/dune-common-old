#ifndef __DUNE_LOCICTRAITS_H__
#define __DUNE_LOCICTRAITS_H__

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
