#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dune/common/nullptr.hh>

void basic_tests()
{
    typedef nullptr_t NULLPTR_T;
    char* ch = nullptr;  // ch has the null pointer value
    char* ch2 = 0;       // ch2 has the null pointer value
#ifdef FAIL
    int n = nullptr;     // error
#endif
    int n2 = 0;          // n2 is zero
    if( ch == 0 );       // evaluates to true
    if( ch == nullptr ); // evaluates to true
    if( ch );            // evaluates to false
    if( n2 == 0 );       // evaluates to true
    ch = ch2;
#ifdef FAIL
    if( n2 == nullptr ); // error
    if( nullptr );       // error, no conversion to bool
    if( nullptr == 0 );  // error
    // arithmetic
    nullptr = 0;         // error, nullptr is not an lvalue
    nullptr + 2;         // error
#endif
}

int main()
{
    basic_tests();
    return 0;
}
