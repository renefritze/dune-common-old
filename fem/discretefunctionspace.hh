#ifndef __DUNE_DISCRETEFUNCTIONSPACE_HH__
#define __DUNE_DISCRETEFUNCTIONSPACE_HH__

#include "../common/functionspace.hh"
#include "basefunctions.hh"

namespace Dune{

template< class FunctionSpaceType , class GridTemp, 
          class DiscreteFunctionSpaceImp, class BaseFunctionSetInter >
class DiscreteFunctionSpaceInterface 
  : public FunctionSpaceType  
{
  
public:
  typedef BaseFunctionSetInter BaseFunctionSetType;
  typedef GridTemp GridType;

  //! Constructor 
  DiscreteFunctionSpaceInterface ( GridType & g, int ident ) : 
   FunctionSpaceType (ident),  
   grid_ (g) {};

  //! get base function set from derived implementation 
  template <class EntityType>
  const BaseFunctionSetType & 
  getBaseFunctionSet ( EntityType &en ) const 
  {
    return asImp().getBaseFunctionSet( en );
  }
 
  //! return the corresponding Grid 
  const GridType & getGrid () const { return grid_; }

  //! return number of degrees of freedom for spezified grid and level 
  //! depends also on the base function set
  int size ( int level ) const { asImp().size(level); };
  
protected:
  //! the corresponding Grid
  const GridType & grid_ ;
};

} // end namespace Dune 

#endif
