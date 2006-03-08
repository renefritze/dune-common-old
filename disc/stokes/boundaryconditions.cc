#ifdef HAVE_CONFIG_H
# include "config.h"     // autoconf defines, needed by the dune headers
#endif
#include"boundaryconditions.hh"

template<class Grid>
double
DirichletBoundary<Grid>::dirichletValue(int variable,  const Point& global, Point& local) const
{
  return exact.velocity(variable,global);
}


 


