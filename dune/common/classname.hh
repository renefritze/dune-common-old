#ifndef DUNE_CLASSNAME_HH
#define DUNE_CLASSNAME_HH

/** \file
 * \brief A free function to provide the demangled class name 
 *        of a given object or type as a string
 */

#include <cstdlib>
#include <string>
#include <typeinfo>

#if defined(__GNUC__) && ! defined(__clang__)
#include <cxxabi.h>
#endif // #ifdef __GNUC__

namespace Dune {
 
    /** \brief Provide the demangled class name of a given object as a string */
    template <class T>
    std::string className ( T &t )
    {
        std::string className = typeid( t ).name();
#if defined(__GNUC__) && ! defined(__clang__)
        int status;
        char *demangled = abi::__cxa_demangle( className.c_str(), 0, 0, &status );
        if( demangled )
        {
          className = demangled;
          std::free( demangled );
        }
#endif // #ifdef __GNUC__
        return className;
    }

    /** \brief Provide the demangled class name of a type T as a string */
    template <class T>
    std::string className ()
    {
        std::string className = typeid( T ).name();
#if defined(__GNUC__) && ! defined(__clang__)
        int status;
        char *demangled = abi::__cxa_demangle( className.c_str(), 0, 0, &status );
        if( demangled )
        {
          className = demangled;
          std::free( demangled );
        }
#endif // #ifdef __GNUC__
        return className;
    }
    
} // namespace Dune

#endif  // DUNE_CLASSNAME_HH
