#include <config.h>
#include <dune/common/static_assert.hh>

int main()
{
    dune_static_assert(true, "OK");
    dune_static_assert(false, "FAIL");
    return 0;
}
