#ifndef __DUNE_MAPPING_HH__
#define __DUNE_MAPPING_HH__

#include "vectorspace.hh"

#include <vector>

namespace Dune{


// Note: Range has to have Vector structure as well.
template<typename Field, class Domain, class Range>
class Mapping : public Vector < Field > {
  
public:
  Mapping( ) 
  {
    lincomb_.push_back( term( *this, 1.0 ) );
  }

  virtual ~Mapping( ) {
  }

  typedef Mapping<Field,Domain,Range> MappingType;

  virtual MappingType operator + (const Vector<Field> &) const ;
  virtual MappingType operator - (const Vector<Field> &) const ;
  virtual MappingType operator * (const Field &) const  ;
  virtual MappingType operator / (const Field &) const  ;
  virtual Vector<Field>& operator  = (const Vector<Field> &) ;
  virtual Vector<Field>& operator += (const Vector<Field> &) ;
  virtual Vector<Field>& operator -= (const Vector<Field> &) ;
  virtual Vector<Field>& operator *= (const Field &)  ;
  virtual Vector<Field>& operator /= (const Field &)  ;

  virtual void apply( const Domain &Arg, Range &Dest ) const {
    std::cerr << "ERROR: Mapping::apply called. \n";
  }

  virtual void prepareG ( int level, const Domain & Arg, Range& Dest, Range * tmp , 
      Field & a, Field & b) const 
  {
    std::cerr << "ERROR: Mapping::prepareGlobal called. \n";
  }

  void prepare ( int level, const Domain & Arg, Range& Dest, Range * tmp , 
      Field & a, Field & b) const 
  {
    for ( typename std::vector<term>::const_iterator it = lincomb_.begin(); it != lincomb_.end(); it++ ) 
    {
	    it->v_->prepareG( level, Arg, Dest, tmp , a , b );
    }
  }

  void finalize ( int level, const Domain & Arg, Range& Dest, Range * tmp , 
      Field & a, Field & b) const 
  {
    for ( typename std::vector<term>::const_iterator it = lincomb_.begin(); it != lincomb_.end(); it++ ) 
    {
	    it->v_->finalizeG( level, Arg, Dest, tmp , a , b );
    }
  }

  virtual void finalizeG ( int level, const Domain & Arg, Range& Dest, Range * tmp , 
      Field & a, Field & b) const 
  {
    std::cerr << "ERROR: Mapping::finalizeGlobal called. \n";
  }

  void operator()( const Domain &Arg, Range &Dest ) const {
    //Dest.clear();
    
    int count = 0;   
    for ( typename std::vector<term>::const_iterator it = lincomb_.begin(); it != lincomb_.end(); it++ ) {
      if ( count == 0 ) {
	it->v_->apply( Arg, Dest );
	if ( it->scalar_ != 1. ) {
	  Dest *= it->scalar_;
	} 
      } else {
	Range tmp( Dest );
	it->v_->apply( Arg, tmp );
	if ( it->scalar_ == 1. ) {
	  Dest += tmp;
	} else if ( it->scalar_ == -1. ) {
	  Dest -= tmp;
	} else {
	  tmp *= it->scalar_;
	  Dest += tmp;
	}
      }
      count++;
    }
  }


  virtual void applyAdd( const Domain &Arg, Range &Dest ) const {
  }
  
private:

  struct term {
    term() : v_(NULL), scalar_(1.0), scaleIt_(false) { }

    term(const MappingType &mapping, Field scalar ) : v_(&mapping), scalar_(scalar), scaleIt_( true ) {
      if ( scalar_ == 1. ) {
	scaleIt_ = false;
      }
    }

    const MappingType *v_;
    Field scalar_;
    bool scaleIt_;
  };

  std::vector<term> lincomb_;
};

#include "mapping.cc"

}

#endif
