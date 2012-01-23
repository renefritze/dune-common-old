#ifndef DUNE_COMMON_UNUSED_HH
#define DUNE_COMMON_UNUSED_HH

#ifndef HAS_ATTRIBUTE_UNUSED
#define DUNE_UNUSED
#else
#define DUNE_UNUSED __attribute__((unused))
#endif

#endif
