#ifndef __DUNE_UGGRID_BOUNDENT_HH__
#define __DUNE_UGGRID_BOUNDENT_HH__


namespace Dune {

/** \todo Please doc me! */
template<int dim, int dimworld>  
class UGGridBoundaryEntity 
: public BoundaryEntityDefault <dim,dimworld, UGCtype,
          UGGridElement,UGGridBoundaryEntity>
{ 
  friend class UGGridIntersectionIterator<dim,dimworld>;
public:
    UGGridBoundaryEntity () : _geom (false) , /* _elInfo ( NULL ), */
  _neigh (-1) {};

  //! return type of boundary , i.e. Neumann, Dirichlet ... 
  BoundaryType type () 
  {
      std::cerr << "UGGridBoundaryEntity::type(): Preliminary implementation,"
          "always returns 'Dirichlet'\n";
      return Dirichlet;
  }

  //! return identifier of boundary segment, number 
  int id ()
  {
      std::cerr << "UGGridBoundaryEntity::id() Preliminary implementation,"
          "always returns -1\n";
    return -1; 
  }

  //! return true if geometry of ghost cells was filled  
  bool hasGeometry () { 
      std::cerr << "UGGridBoundaryEntity::hasGeometry() Preliminary implementation,"
          "always returns 'false'\n";
      return false; 
  }

  //! return geometry of the ghost cell 
  UGGridElement<dim,dimworld> geometry () 
  {
    return _geom;
  }

private: 

  int _neigh;
  
  UGGridElement<dim,dimworld> _geom;

};

}  // namespace Dune

#endif
