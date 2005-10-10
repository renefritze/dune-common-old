#ifndef DUNE_DISCRETEOPERATOR_HH
#define DUNE_DISCRETEOPERATOR_HH


#include "mapping.hh"


namespace Dune{

template<typename Field, class Domain, class Range, class GridType>
class DiscreteOperator : Operator < Field, Domain, Range > {
  
public:
  virtual Vector operator + (const Vector &) const ;
  virtual Vector operator - (const Vector &) const ;
  virtual Vector operator * (const Field &) const  ;
  virtual Vector operator / (const Field &) const  ;
  virtual Vector& operator  = (const Vector &) ;
  virtual Vector& operator += (const Vector &) ;
  virtual Vector& operator -= (const Vector &) ;
  virtual Vector& operator *= (const Field &)  ;
  virtual Vector& operator /= (const Field &)  ;


  virtual void  operator () ( const Domain & , Range &) const ;


private:

};


}

#endif
