#ifndef DUNE_BARTONNACKMANIFCHECK_HH
#define DUNE_BARTONNACKMANIFCHECK_HH

/** @file
  @author Robert Kloefkorn
  @brief Provides check for implementation of interface methods when using 
  static polymorphism, i.e. Barton-Nackman trick.
  Use by invoking CHECK_INTERFACE_IMPLEMENTATION(asImp().methodToCheck())
  and for template methods double (
  CHECK_INTERFACE_IMPLEMENTATION((asImp().template methodToCheck<param> ())).
**/

//- Dune includes 
#include <dune/common/exceptions.hh>

namespace Dune {
#ifndef NDEBUG
#define CHECK_INTERFACE_IMPLEMENTATION(__interface_method_to_call__) \
  {\
    static bool call = false; \
    if( call == true ) \
      DUNE_THROW(NotImplemented,"Interface method not implemented!");\
    call = true; \
    (__interface_method_to_call__); \
    call = false; \
  }
#else
#define CHECK_INTERFACE_IMPLEMENTATION(dummy) 
#endif
} // end namespace Dune 
#endif
