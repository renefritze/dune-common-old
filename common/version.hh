#ifndef DUNE_VERSION_HH
#define DUNE_VERSION_HH

#define DUNE_VERSION_JOIN(module,type) DUNE_##module##_VERSION_##type

#define DUNE_VERSION_EQUAL(module,major,minor) \
    ((DUNE_VERSION_JOIN(module,MAJOR) == major) && \
     (DUNE_VERSION_JOIN(module,MINOR) == minor))

#define DUNE_VERSION_EQUAL_REV(module,major,minor,revision) \
    ( DUNE_VERSION_EQUAL(module,major,minor) && \
     (DUNE_VERSION_JOIN(module,REVISION) == revision))

#define DUNE_VERSION_NEWER(module,major,minor,revision) \
  ((DUNE_VERSION_JOIN(module,MAJOR) > major) \
   || ((DUNE_VERSION_JOIN(module,MAJOR) == major) && (DUNE_VERSION_JOIN(module,MINOR) >= minor)))

#define DUNE_VERSION_NEWER_REV(module,major,minor,revision) \
  ((DUNE_VERSION_JOIN(module,MAJOR) > major) \
   || ((DUNE_VERSION_JOIN(module,MAJOR) == major) && (DUNE_VERSION_JOIN(module,MINOR) > minor)) \
   || ((DUNE_VERSION_JOIN(module,MAJOR) == major) && (DUNE_VERSION_JOIN(module,MINOR) == minor) \
       && (DUNE_VERSION_JOIN(module,REVISION) >= revision)))

#define DUNE_VERSION_ID(module) \
  (unsigned int( \
    (DUNE_VERSION_JOIN(module,MAJOR) << 24) + \
    (DUNE_VERSION_JOIN(module,MINOR) << 16) + \
    (DUNE_VERSION_JOIN(module,REVISION)) \
  ))

#endif
