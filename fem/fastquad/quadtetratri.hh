#ifndef __DUNE_QUADTETRATRI_HH__
#define __DUNE_QUADTETRATRI_HH__

// the UG Quadratures 
#include "ugquadratures.hh"

namespace Dune{
  
//******************************************************************
//
//!  Memorization of the number of quadrature points
//
//******************************************************************

static const double referenceVol_triangle    = 0.5;
static const double referenceVol_tetrahedron = 1.0/6.0;
  
//! specialization triangles 
template <class Domain, class RangeField, int polOrd>
struct QuadraturePoints<Domain,RangeField,triangle,polOrd> 
{
  enum { dim = 2 };
  enum { numberOfCorners = dim+1 };
  typedef UG_Quadratures::QUADRATURE QUADRATURE;
  static int numberOfQuadPoints (); 
  static int order (); 
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
int QuadraturePoints<Domain,RangeField,triangle,polOrd>:: 
numberOfQuadPoints()
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  return quad->nip;
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
int QuadraturePoints<Domain,RangeField,triangle,polOrd>:: 
order()
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  return quad->order;
}
//! the weight is the volume of the reference element
template <class Domain, class RangeField,int polOrd>
RangeField QuadraturePoints<Domain,RangeField,triangle,polOrd>:: 
getWeight(int i)
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  return (referenceVol_triangle * static_cast<RangeField> (quad->weight[i]));
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField,int polOrd>
Domain QuadraturePoints<Domain,RangeField,triangle,polOrd>::
getPoint(int i)
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  Domain tmp;
  for(int j=0; j<dim; j++)
    tmp(j) = quad->local[i][j];
  return tmp;
}

//**********************************************************************
//! specialization triangles , polOrd 3 
//**********************************************************************
#if 0 
HAVE_ALBERT 
template <class Domain, class RangeField, int polOrd>
struct QuadraturePoints<Domain,RangeField,triangle,polOrd> 
{
  enum { dim = 2 };
  enum { numberOfCorners = dim+1 };
  typedef UG_Quadratures::QUADRATURE QUADRATURE;
  static int numberOfQuadPoints (); 
  static int order (); 
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
int QuadraturePoints<Domain,RangeField,triangle,polOrd>:: 
numberOfQuadPoints()
{
  const ALBERT QUAD * quad = ALBERT get_quadrature(2,3);
  return quad->n_points;
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
int QuadraturePoints<Domain,RangeField,triangle,polOrd>:: 
order()
{
  const ALBERT QUAD * quad = ALBERT get_quadrature(2,3);
  return quad->degree;
}
//! the weight is the volume of the reference element
template <class Domain, class RangeField,int polOrd>
RangeField QuadraturePoints<Domain,RangeField,triangle,polOrd>:: 
getWeight(int i)
{
  const ALBERT QUAD * quad = ALBERT get_quadrature(2,polOrd);
  //assert((i>=0) && (i<quad->n_points));
  return  referenceVol_triangle * quad->w[i];    
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField,int polOrd>
Domain QuadraturePoints<Domain,RangeField,triangle,polOrd>::
getPoint(int i)
{
  const ALBERT QUAD * quad = ALBERT get_quadrature(2,3);
  assert((i>=0) && (i<quad->n_points));

  // summ of coordinates of Dune reference element 
  Domain tmp = 0.0;
  tmp(0) += quad->lambda[i][0];
  tmp(1) += quad->lambda[i][1];

  return tmp;
}
#endif
//*************************************************************
//! specialization tetrahedrons  
template <class Domain, class RangeField, int polOrd>
struct QuadraturePoints<Domain,RangeField,tetrahedron,polOrd> 
{
  enum { dim = 3 }; // tetrahedrons  
  enum { numberOfCorners = dim+1 };
  typedef UG_Quadratures::QUADRATURE QUADRATURE;
  static int numberOfQuadPoints (); 
  static int order (); 
  static Domain getPoint (int i);
  static RangeField getWeight (int i);
};

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
int QuadraturePoints<Domain,RangeField,tetrahedron,polOrd>:: 
numberOfQuadPoints()
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  return quad->nip;
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
int QuadraturePoints<Domain,RangeField,tetrahedron,polOrd>:: 
order()
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  return quad->order;
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField, int polOrd>
RangeField QuadraturePoints<Domain,RangeField,tetrahedron,polOrd>:: 
getWeight(int i)
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  return (referenceVol_tetrahedron * static_cast<RangeField> (quad->weight[i]));
}

//! the weight is the volume of the reference element
template <class Domain, class RangeField,int polOrd>
Domain QuadraturePoints<Domain,RangeField,tetrahedron,polOrd>::
getPoint(int i)
{
  QUADRATURE * quad = UG_Quadratures::GetQuadrature(dim,numberOfCorners,polOrd);
  Domain tmp;
  for(int j=0; j<dim; j++)
    tmp(j) = quad->local[i][j];
  return tmp;
}

} // end namespace Dune 

#endif
