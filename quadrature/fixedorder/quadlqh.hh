#ifndef DUNE_QUADLQH_HH
#define DUNE_QUADLQH_HH

#include <dune/common/misc.hh>
#include "gaussquadimp.hh"

namespace Dune{
  
//************************************************************************
//
//! Quadratures for lines, quadrilaterals and hexahedrons 
//! see gaussquadimp.hh for detailed description 
//
//************************************************************************
//
//! specialization for lines  
//
//************************************************************************
template <class Domain, class RangeField, int polOrd >
struct QuadraturePoints<Domain,RangeField,line, polOrd> 
{
  enum { identifier = 5*(polOrd+1) };
  enum { numberOfQuadPoints_ = GaussQuadrature<Domain,RangeField,1,polOrd>::n };
  static int numberOfQuadPoints ();
  static int order ();                  
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

template <class Domain, class RangeField , int polOrd >
int QuadraturePoints<Domain,RangeField,line,polOrd>::
numberOfQuadPoints()
{
  return numberOfQuadPoints_;
}

template <class Domain, class RangeField , int polOrd >
int QuadraturePoints<Domain,RangeField,line,polOrd>::
order ()
{
  return polOrd;
}

template <class Domain, class RangeField , int polOrd >
RangeField QuadraturePoints<Domain,RangeField,line,polOrd>::getWeight(int i)
{
  GaussQuadrature<Domain,RangeField,1,polOrd> gaussquad;
  return gaussquad.w(i);
}

template <class Domain, class RangeField , int polOrd>
Domain QuadraturePoints<Domain,RangeField,line,polOrd>::getPoint(int i)
{
  // check whether dimension is 1 or not 
  //CompileTimeChecker < Domain::dimension == 1 > check;
  GaussQuadrature<Domain,RangeField,1,polOrd> gaussquad;
  return gaussquad.ip(i);
}

//**************************************************************************
//
//! specialization for quadrilaterals 
//
//**************************************************************************
template <class Domain, class RangeField, int polOrd >
struct QuadraturePoints<Domain,RangeField,quadrilateral, polOrd> 
{
  enum { identifier = 6*(polOrd+1) };
  enum { numberOfQuadPoints_ = GaussQuadrature<Domain,RangeField,2,polOrd>::n };
  static int numberOfQuadPoints ();
  static int order ();                  
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

template <class Domain, class RangeField , int polOrd >
int QuadraturePoints<Domain,RangeField,quadrilateral,polOrd>::
numberOfQuadPoints()
{
  return numberOfQuadPoints_;
}

template <class Domain, class RangeField , int polOrd >
int QuadraturePoints<Domain,RangeField,quadrilateral,polOrd>::
order ()
{
  return polOrd;
}

template <class Domain, class RangeField , int polOrd >
RangeField QuadraturePoints<Domain,RangeField,quadrilateral,polOrd>::getWeight(int i)
{
  GaussQuadrature<Domain,RangeField,2,polOrd> gaussquad;
  return gaussquad.w(i);
}

template <class Domain, class RangeField , int polOrd>
Domain QuadraturePoints<Domain,RangeField,quadrilateral,polOrd>::getPoint(int i)
{
  // check whether dimension is 2 or not 
  //CompileTimeChecker < Domain::dimension == 2 > check;
  GaussQuadrature<Domain,RangeField,2,polOrd> gaussquad;
  return gaussquad.ip(i);
}

//**************************************************************************
//
//! specialization for hexahedron 
//
//**************************************************************************
template <class Domain, class RangeField, int polOrd >
struct QuadraturePoints<Domain,RangeField,hexahedron, polOrd> 
{
  enum { identifier = 7*(polOrd+1) };
  enum { numberOfQuadPoints_ = GaussQuadrature<Domain,RangeField,3,polOrd>::n };
  static int numberOfQuadPoints ();
  static int order ();                  
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

template <class Domain, class RangeField , int polOrd >
int QuadraturePoints<Domain,RangeField,hexahedron,polOrd>::
numberOfQuadPoints()
{
  return numberOfQuadPoints_;
}

template <class Domain, class RangeField , int polOrd >
int QuadraturePoints<Domain,RangeField,hexahedron,polOrd>::
order ()
{
  return polOrd;
}
template <class Domain, class RangeField , int polOrd >
RangeField QuadraturePoints<Domain,RangeField,hexahedron,polOrd>::getWeight(int i)
{
  GaussQuadrature<Domain,RangeField,3,polOrd> gaussquad;
  return gaussquad.w(i);
}

template <class Domain, class RangeField , int polOrd>
Domain QuadraturePoints<Domain,RangeField,hexahedron,polOrd>::getPoint(int i)
{
  // check whether dimension is 3 or not 
  //CompileTimeChecker < Domain::dimension == 3 > dim_is_not_equal_3;
  GaussQuadrature<Domain,RangeField,3,polOrd> gaussquad;
  return gaussquad.ip(i);
}


} // end namespace Dune 

#endif
