#ifndef __DUNE_MAPPING_HH__
#define __DUNE_MAPPING_HH__

#include "vectorspace.hh"

namespace Dune{

template<typename Field, class Domain, class Range>
class Mapping : Vector < Field > {
  
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

private:

  enum operation {add, subtract};

  struct term {
    Vector<Field> *v;
    operation op;
		
    Field scalar;
    bool scaleIt;
  };

};

#include "mapping.cc"

}

#endif
