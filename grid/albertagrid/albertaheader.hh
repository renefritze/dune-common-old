#ifndef __DUNE_ALBERTAHEADER_HH__
#define __DUNE_ALBERTAHEADER_HH__

// if we have ALBERTA C++ lib define namespace for ALBERTA
#ifdef __ALBERTApp__
#define ALBERTA Alberta:: 
#else
#define ALBERTA
#endif

// the keyword ALBERTA stands for ALBERTA routines 
#ifndef __ALBERTApp__
extern "C" 
{
#endif

// we dont use the el->index, its for debugging
#ifndef EL_INDEX 
#define EL_INDEX 0
#else
#if EL_INDEX != 0
#warning "EL_INDEX != 0, but not used in interface implementation!\n"
#endif
#endif
  
  
#ifndef NEIGH_IN_EL
// neighbours were calculated on walkthrough
#define NEIGH_IN_EL 0
#else 
#if NEIGH_IN_EL != 0
#error "NEIGH_IN_EL != 0 is not supported by this implementation!\n"  
#endif
#endif

// MAX, MIN, and ABS are defined macros of ALBERTA
// if they are not defined elsewhere, they are undefined here
#ifndef MAX 
#define _MAX_NOT_DEFINED_
#endif
  
#ifndef MIN 
#define _MIN_NOT_DEFINED_
#endif
  
#ifndef ABS 
#define _ABS_NOT_DEFINED_
#endif

#include <alberta.h>
  
#ifdef _ABS_NOT_DEFINED_
#undef ABS
#endif
  
#ifdef _MIN_NOT_DEFINED_
#undef MIN
#endif
  
#ifdef _MAX_NOT_DEFINED_
#undef MAX
#endif

#ifndef __ALBERTApp__
} // end extern "C"
#endif



#endif
