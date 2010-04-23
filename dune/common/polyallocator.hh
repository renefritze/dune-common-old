#ifndef DUNE_POLYALLOCATOR_HH
#define DUNE_POLYALLOCATOR_HH

#include <cstdlib>

namespace Dune
{

  // PolyAllocator
  // -------------

  struct PolyAllocator
  {
    template< class T >
    T *create ( const T &value )
    {
      return new T( value );
    }

    template< class T >
    void destroy ( T *p )
    {
      delete p;
    }
  };

}

#endif // #ifndef DUNE_POLYALLOCATOR_HH
