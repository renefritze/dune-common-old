#ifndef __GRAPEWRAPPER__
#define __GRAPEWRAPPER__

#ifdef __cplusplus
extern "C" {
#endif

#define class GrapeClass
#define private GrapePrivate 
#define friend GrapeFriend 
#define explicit GrapeExplicit
  
#define G_CPP
#include <grape.h>  
#undef G_CPP  

#undef class 
#undef private
#undef friend
#undef explicit
  
#ifdef __cplusplus
}
#endif
  
#endif
