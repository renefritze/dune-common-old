#ifndef __DUNE_OPERATOR_HH__
#define __DUNE_OPERATOR_HH__

#include "mapping.hh"

namespace Dune
{

template<typename Field, class Domain, class Range>
class Operator : Mapping < Field, Domain, Range > 
{
  
public:
  virtual Vector<Field> operator + (const Vector<Field> &) const ;
  virtual Vector<Field> operator - (const Vector<Field> &) const ;
  virtual Vector<Field> operator * (const Field &) const  ;
  virtual Vector<Field> operator / (const Field &) const  ;
  virtual Vector<Field>& operator  = (const Vector<Field> &) ;
  virtual Vector<Field>& operator += (const Vector<Field> &) ;
  virtual Vector<Field>& operator -= (const Vector<Field> &) ;
  virtual Vector<Field>& operator *= (const Field &)  ;
  virtual Vector<Field>& operator /= (const Field &)  ;

  virtual void  operator () ( const Domain & , Range &) const ;

private:

};

}

#endif
