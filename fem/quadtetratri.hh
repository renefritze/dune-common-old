#ifndef __DUNE_QUADTETRATRI_HH__
#define __DUNE_QUADTETRATRI_HH__

#include <dune/grid/common/grid.hh>
#include "common/quadrature.hh"

namespace Dune{
  
//******************************************************************
//
//!  Memorization of the number of quadrature points
//
//******************************************************************

//! specialization triangles and polOrd = 1
template <class Domain, class RangeField>
struct QuadraturePoints<Domain,RangeField,triangle, 1> 
{
  enum { identifier = 3 };
  enum { numberOfQuadPoints = 1 };
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

//! the weight is the volume of the reference element
template <class Domain, class RangeField >
RangeField QuadraturePoints<Domain,RangeField,triangle,1>::getWeight(int i)
{
  return 0.5;
}

template <class Domain, class RangeField >
Domain QuadraturePoints<Domain,RangeField,triangle,1>::getPoint(int i)
{
  // check whether dimension is 2 or not 
  CompileTimeChecker < Domain::dimension == 2 > check;
  Domain tmp;  tmp(0) = (1.0/3.0); tmp(1) = (1.0/3.0);
  return tmp;
}

//*************************************************************************
//! specialization triangles and polOrd = 2
template <class Domain, class RangeField>
struct QuadraturePoints<Domain,RangeField,triangle, 2> 
{
  enum { identifier = 4 };
  enum { numberOfQuadPoints = 3 };
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

//! the sum over the weigths is the volume of the reference element
template <class Domain, class RangeField >
RangeField QuadraturePoints<Domain,RangeField,triangle,2>::getWeight(int i)
{
  return (1.0/6.0);
}
template <class Domain, class RangeField >
Domain QuadraturePoints<Domain,RangeField,triangle,2>::getPoint(int i)
{
  // check whether dimension is 2 or not 
  CompileTimeChecker < Domain::dimension == 2 > check;
  switch (i) 
  {
    case 0: {
              Domain tmp(0.0);  tmp(1) = 0.5;
              //tmp.print(std::cout,1); std::cout << " QuadPoint \n";
              return tmp;
            }
    case 1: {
              Domain tmp(0.0);  tmp(0) = 0.5; //tmp(1) = 0.5;
              //tmp.print(std::cout,1); std::cout << " QuadPoint \n";
              return tmp;
            }
    case 2: {
              Domain tmp(0.0);  tmp(0) = 0.5; tmp(1) = 0.5;
              //tmp.print(std::cout,1); std::cout << " QuadPoint \n";
              return tmp;
            }
    default: {
                std::cerr << "TriangleQuadPoints::getPoint: i out of range! \n";
                abort();
             }
  }
}

//******************************************************************
//
//!  Memorization of the number of quadrature points
//
//******************************************************************

//! specialization tetrahedrons and polOrd = 1
template <class Domain, class RangeField>
struct QuadraturePoints<Domain,RangeField,tetrahedron, 1> 
{
  enum { identifier = 9 };
  enum { numberOfQuadPoints = 1 };
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

template <class Domain, class RangeField >
RangeField QuadraturePoints<Domain,RangeField,tetrahedron,1>::getWeight(int i)
{
  return 1.0;
}

template <class Domain, class RangeField >
Domain QuadraturePoints<Domain,RangeField,tetrahedron,1>::getPoint(int i)
{
  // check whether dimension is 2 or not 
  //CompileTimeChecker < Domain::dimension == 3 > check;
  Domain tmp;  tmp(0) = 0.75; tmp(1) = 0.5; tmp(2) = 0.25;
  return tmp;
}


} // end namespace Dune 

#endif
