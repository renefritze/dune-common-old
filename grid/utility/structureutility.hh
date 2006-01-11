#ifndef DUNE_STRUCTUREUTILITY_HH
#define DUNE_STRUCTUREUTILITY_HH

#include <dune/grid/yaspgrid.hh>
#include <dune/grid/sgrid.hh>

namespace Dune {

  //! defines true for all grids, specialisation for structured grids 
  template <int dim, int dimworld, class GridImp>
  struct IsUnstructured {
    enum { value = true };
  };
  
  //! specialisation for SGrid  
  template <int dim, int dimworld>
  struct IsUnstructured<dim,dimworld,SGrid<dim, dimworld> > {
    enum { value = false }; 
  };

  //! specialisation for YaspGrid  
  template <int dim, int dimworld>
  struct IsUnstructured<dim,dimworld,YaspGrid<dim, dimworld> > {
    enum { value = false }; 
  };

} // end namespace Dune

#endif
