#ifndef DUNE_COMMON_UNUSED_HH
#define DUNE_COMMON_UNUSED_HH

/** \file
 * \brief Definition of the DUNE_UNUSED macro for the case that config.h
 *      is not available
 */

#ifndef HAS_ATTRIBUTE_UNUSED
#define DUNE_UNUSED
#else
#define DUNE_UNUSED __attribute__((unused))
#endif

#endif
