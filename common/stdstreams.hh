// $Id$

/*

  Declaration of standard Dune-library streams

*/

#include "debugstream.hh"

namespace Dune {

/*! debug streams with level below MINIMAL_DEBUG_LEVEL will collapse
  to doing nothing if output is requested. 

  For a Dune-Release this should be set to 5 so that only the grave
  warnings and error messages are active. Otherwise, the
  Dune-developers will adapt this setting to their debugging needs
  locally

  \todo autoconf-option for this level?
*/
  static const DebugLevel MINIMAL_DEBUG_LEVEL = 3;
    
  // static const DebugLevel MINIMAL_ACTIVE_OUTPUT_LEVEL = 0;

  typedef DebugStream<1, MINIMAL_DEBUG_LEVEL> DVVerbType;
  extern DVVerbType dvverb;

  typedef DebugStream<2, MINIMAL_DEBUG_LEVEL> DVerbType; 
  extern DVerbType dverb;

  typedef DebugStream<3, MINIMAL_DEBUG_LEVEL> DInfoType;
  extern DInfoType dinfo;

  typedef DebugStream<4, MINIMAL_DEBUG_LEVEL> DWarnType;
  extern DWarnType dwarn;

  typedef DebugStream<5, MINIMAL_DEBUG_LEVEL> DGraveType; 
  extern DGraveType dgrave;

  typedef DebugStream<1> DErrType; 
  extern DErrType derr;
}
