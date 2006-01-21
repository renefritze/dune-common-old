// $Id$

#ifndef DUNE_FUNCTION_ADAPTER_HH
#define DUNE_FUNCTION_ADAPTER_HH

#include"functions.hh"

namespace Dune
{
  
  /**
     Access a Function as a GridFunction
   */
  template<class G, class RT, int m=1>
  class GridFunctionAdapter :
    virtual public GridFunction<G,RT,m>,
    virtual public GridFunctionDefault<G,RT,m>,
    virtual public FunctionDefault<typename G::ctype,RT,G::dimension,m>
  {
	//! get domain field type from the grid
	typedef typename G::ctype DT;

	//! get entity from the grid
	typedef typename G::template Codim<0>::Entity Entity;

	//! get domain dimension from the grid
	enum {n=G::dimension};

	//! get world dimension from the grid
	enum {dimw=G::dimensionworld};

    //! make copy constructor private
  	GridFunctionAdapter (const GridFunctionAdapter&);
  public:
    //! create Adaptor from a Function
    GridFunctionAdapter(const Function<DT,RT,n,m>& f) : _fnkt(f) {};

    // forward the evaluation to _fnkt
	virtual RT eval (int comp, const Dune::FieldVector<DT,n>& x) const
	{
      return _fnkt.eval(comp, x);
	}
  private:
    const Function<DT,RT,n,m>& _fnkt;
  };

}; // end namespace Dune

#endif // DUNE_FUNCTION_ADAPTER_HH
