#ifndef RHS_HH
#define RHS_HH
#include "testfunctions.hh"

template<class Grid>
class RightHandSide
{
  enum{dim=Grid::dimension};
  typedef typename Grid::ctype ct;
public:
  
   RightHandSide(ExactSolution<ct,dim>& ex): exact(ex){}
  
   typedef Dune::FieldVector<ct,dim> Point;
  double rhsValue(int variable,  Point& global, const Point& local) const;
  // void u_rhs(const double&, const double&, double&);
//   void v_rhs(const double&, const double&, double&);
//   void p_rhs(const double&, const double&, double&);
protected:
   ExactSolution<ct,dim>& exact;
};




// void RightHandSide::u_rhs(const double& x, const double& y, double& f1)
// {
//   f1=sin(x)+y;
//   //f1=-1;
//   //f1=0;
//   return ;
// }
// void RightHandSide::v_rhs(const double& x, const double& y, double& f2)
// {
//   f2=-y*cos(x)+x;
//   //f2=0.0;
//   return ;
// }

// void RightHandSide::p_rhs(const double& x, const double& y, double& f3)
// {
  
//   f3=0.0;
//   return ;
// }

#include"rhs.cc"

#endif
