#ifndef __MISC_HH__
#define __MISC_HH__

#include <iostream>

//! Check condition at compilation time 
template <bool flag> class CompileTimeChecker;

//! it exists only an implementation for true so the compiler throws an
//! error if the condition is false 
template <> class CompileTimeChecker<true> { };


namespace Dune {
/** @addtogroup Common

        @{
 */

template<class T>
T SQR (T t)
{
  return t*t;
}

//********************************************************************
//
// generate filenames with timestep number in it 
//
//********************************************************************

template <typename T>
inline const char *genFilename(T *path, T *fn, int ntime, int precision = 6)
{
  static char  name[256];
  char         *cp;
  
  if (path == 0 || path[0] == '\0')
  {
    sprintf(name, "%s", fn);
  }
  else 
  {
    const char *cp = path;
    while (*cp) 
      cp++;
    cp--;
    if (*cp == '/')
      sprintf(name, "%s%s", path, fn);
    else
      sprintf(name, "%s/%s", path, fn);
  } 
  cp = name;
  while (*cp)
    cp++;

  switch(precision)
  {
    case 2  : { sprintf(cp, "%02d", ntime); break; }
    case 3  : { sprintf(cp, "%03d", ntime); break; }
    case 4  : { sprintf(cp, "%04d", ntime); break; }
    case 5  : { sprintf(cp, "%05d", ntime); break; }
    case 6  : { sprintf(cp, "%06d", ntime); break; }
    case 7  : { sprintf(cp, "%07d", ntime); break; }
    case 8  : { sprintf(cp, "%08d", ntime); break; }
    case 9  : { sprintf(cp, "%09d", ntime); break; }
    case 10 : { sprintf(cp, "%010d", ntime); break; }
    default: 
      {
        std::cerr << "Couldn't gernerate filename with precision = "<<precision << ", file = " << __FILE__ << ", line = " << __LINE__ << "\n";
        abort();
      }
  }

  return( (T *) name);
}
    
/** @} */

}


#endif
