#ifdef HAVE_CONFIG_H
# include "config.h"     // autoconf defines, needed by the dune headers
#endif
#include"rhs.hh"

template<class Grid>
double
RightHandSide<Grid>::rhsValue(int variable,Point& global,const Point& local) const
{
  return exact.rhsvalue(variable,global);
}
