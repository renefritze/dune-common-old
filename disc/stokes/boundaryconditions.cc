#ifdef HAVE_CONFIG_H
# include "config.h"     // autoconf defines, needed by the dune headers
#endif
#include"boundaryconditions.hh"

template<class Grid>
double DirichletBoundary<Grid>::dirichletValue(int comp, const Point&global, ct local)const
{
  return exact.velocity(comp,global);
}




template<int dim, class ct>
class Example : public ExactSolution<ct, dim>
{
  typedef Dune::FieldVector< ct, dim > Point;
  
public:
  ct velocity(int comp,const Point & glob) const
    {
      if (comp==0) return sin(glob[0]);
	  if (comp==1) return glob[1]*cos(glob[0]);
    }
  ct pressure(const Point & glob) const
  {
	return glob[0]*glob[1];
  }
 
};
