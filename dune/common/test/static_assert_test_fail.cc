#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <dune/common/static_assert.hh>

int main()
{
    dune_static_assert(false, "FAIL");
    return 0;
}
