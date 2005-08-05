#ifndef DUNE_UGGRID_INDEXSETS_HH
#define DUNE_UGGRID_INDEXSETS_HH

/** \file
    \brief The index and id sets for the UGGrid class
*/

#include <vector>

namespace Dune {

template<int dim>
class UGGridLevelIndexSet
{
    typedef UGGrid<dim,dim> GridImp;
public:
    /** \brief Default constructor

    Unfortunately we can't force the user to init grid_ and level_, because
    UGGridLevelIndexSets are meant to be stored in an array.
    */
    UGGridLevelIndexSet () {}

  //! get index of an entity
  template<int cd>
  int index (const typename GridImp::Traits::template Codim<cd>::Entity& e) const 
  {
      return grid_->template getRealEntity<cd>(e).levelIndex(); 
  }

  //! get index of subentity of a codim 0 entity
  template<int cc>
  int subindex (const typename GridImp::Traits::template Codim<0>::Entity& e, int i) const
  {
      return grid_->template getRealEntity<0>(e).template subIndex<cc>(i);
  }

  //! get number of entities of given codim, type and on this level
  int size (int codim, GeometryType type) const
  {
      return grid_->size(level_,codim, type);
  }

    /** \brief Deliver all geometry types used in this grid */
  const std::vector<GeometryType>& geomtypes () const
  {
	return myTypes_;
  }

    /** \todo Should be private */
    void update(const GridImp& grid, int level) {

        // Commit the index set to a specific level of a specific grid
        grid_ = &grid;
        level_ = level;	

        // ///////////////////////////////
        //   Init the element indices
        // ///////////////////////////////
        int numSimplices = 0;
        int numPyramids  = 0;
        int numPrisms    = 0;
        int numCubes     = 0;

        typename GridImp::Traits::template Codim<0>::LevelIterator eIt    = grid_->template lbegin<0>(level_);
        typename GridImp::Traits::template Codim<0>::LevelIterator eEndIt = grid_->template lend<0>(level_);
        
        for (; eIt!=eEndIt; ++eIt) {
         
            switch (eIt->geometry().type()) {
            case simplex:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numSimplices++;
                break;
            case pyramid:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numPyramids++;
                break;
            case prism:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numPrisms++;
                break;
            case cube:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numCubes++;
                break;
            default:
                DUNE_THROW(GridError, "Found the GeometryType " << eIt->geometry().type()
                           << ", which should never occur in a UGGrid!");
            }

        }

        // Update the list of geometry types present
        myTypes_.resize(0);
        if (numSimplices > 0)
            myTypes_.push_back(simplex);
        if (numPyramids > 0)
            myTypes_.push_back(pyramid);
        if (numPrisms > 0)
            myTypes_.push_back(prism);
        if (numCubes > 0)
            myTypes_.push_back(cube);

        // //////////////////////////////
        //   Init the vertex indices
        // //////////////////////////////

        typename GridImp::Traits::template Codim<dim>::LevelIterator vIt    = grid_->template lbegin<dim>(level_);
        typename GridImp::Traits::template Codim<dim>::LevelIterator vEndIt = grid_->template lend<dim>(level_);
        
        int id = 0;
        for (; vIt!=vEndIt; ++vIt)
            UG_NS<dim>::levelIndex(grid_->template getRealEntity<dim>(*vIt).target_) = id++;

    }

private:
  const GridImp* grid_;
  int level_;
  std::vector<GeometryType> myTypes_;
};

template<int dim>
class UGGridLeafIndexSet
{
    typedef UGGrid<dim,dim> GridImp;

public:
  //! constructor stores reference to a grid and level
  UGGridLeafIndexSet (const GridImp& g) : grid_(g)
  {
  }

  //! get index of an entity
  template<int cd>
  int index (const typename GridImp::Traits::template Codim<cd>::Entity& e) const 
  {
      return grid_.template getRealEntity<cd>(e).leafIndex(); 
  }

  //! get index of subentity of a codim 0 entity
  template<int cc>
  int subindex (const typename GridImp::Traits::template Codim<0>::Entity& e, int i) const
  {
      return grid_.template getRealEntity<0>(e).template subLeafIndex<cc>(i);
  }

  //! get number of entities of given codim, type and level (the level is known to the object)
  int size (int codim, GeometryType type) const
  {
      if (codim==dim) {

          return numVertices_;

      } else if (codim==0) {

          switch (type) {
          case simplex:
              return numSimplices_;
          case pyramid:
              return numPyramids_;
          case prism:
              return numPrisms_;
          case cube:
              return numCubes_;
          default:
              return 0;
          }

      } else {
          DUNE_THROW(NotImplemented, "UGGridLeafIndexSet::size(codim,type) for codim neither 0 nor dim");
      }
  }

    /** deliver all geometry types used in this grid */
  const std::vector<GeometryType>& geomtypes () const
  {
	return myTypes_;
  }

    /** \todo Should be private */
    void update() {

        // ///////////////////////////////
        //   Init the element indices
        // ///////////////////////////////
        numSimplices_ = 0;
        numPyramids_  = 0;
        numPrisms_    = 0;
        numCubes_     = 0;

        typename GridImp::Traits::template Codim<0>::LeafIterator eIt    = grid_.template leafbegin<0>();
        typename GridImp::Traits::template Codim<0>::LeafIterator eEndIt = grid_.template leafend<0>();
        
        for (; eIt!=eEndIt; ++eIt) {
         
            switch (eIt->geometry().type()) {
            case simplex:
                UG_NS<dim>::leafIndex(grid_.template getRealEntity<0>(*eIt).target_) = numSimplices_++;
                break;
            case pyramid:
                UG_NS<dim>::leafIndex(grid_.template getRealEntity<0>(*eIt).target_) = numPyramids_++;
                break;
            case prism:
                UG_NS<dim>::leafIndex(grid_.template getRealEntity<0>(*eIt).target_) = numPrisms_++;
                break;
            case cube:
                UG_NS<dim>::leafIndex(grid_.template getRealEntity<0>(*eIt).target_) = numCubes_++;
                break;
            default:
                DUNE_THROW(GridError, "Found the GeometryType " << eIt->geometry().type()
                           << ", which should never occur in a UGGrid!");
            }

        }

        // Update the list of geometry types present
        myTypes_.resize(0);
        if (numSimplices_ > 0)
            myTypes_.push_back(simplex);
        if (numPyramids_ > 0)
            myTypes_.push_back(pyramid);
        if (numPrisms_ > 0)
            myTypes_.push_back(prism);
        if (numCubes_ > 0)
            myTypes_.push_back(cube);

        // //////////////////////////////
        //   Init the vertex indices
        // //////////////////////////////
        typename GridImp::Traits::template Codim<dim>::LeafIterator vIt    = grid_.template leafbegin<dim>();
        typename GridImp::Traits::template Codim<dim>::LeafIterator vEndIt = grid_.template leafend<dim>();
        
        numVertices_ = 0;
        for (; vIt!=vEndIt; ++vIt)
            UG_NS<dim>::leafIndex(grid_.template getRealEntity<dim>(*vIt).target_) = numVertices_++;

    }
        
private:

    const GridImp& grid_;

    int numSimplices_;
    int numPyramids_;
    int numPrisms_;
    int numCubes_;
    int numVertices_;

  std::vector<GeometryType> myTypes_;
};


template<int dim>
class UGGridGlobalIdSet
{

    typedef UGGrid<dim,dim> GridImp;

public:
  //! define the type used for persistent indices
  typedef unsigned int GlobalIdType;

  //! constructor stores reference to a grid
  UGGridGlobalIdSet (const GridImp& g) : grid_(g) {}

  //! get id of an entity
  template<int cd>
  GlobalIdType id (const typename GridImp::Traits::template Codim<cd>::Entity& e) const 
  {
      return grid_.template getRealEntity<cd>(e).globalId();
  }

  //! get id of subentity
  template<int cc>
  GlobalIdType subid (const typename GridImp::Traits::template Codim<0>::Entity& e, int i) const
  {
      return grid_.template getRealEntity<0>(e).template subGlobalId<cc>(i);
  }

    /** \todo Should be private */
    void update() {}

private:

  const GridImp& grid_;
};


template<int dim>
class UGGridLocalIdSet
{

    typedef UGGrid<dim,dim> GridImp;

public:
  //! define the type used for persistent local ids
  typedef uint LocalIdType;

  //! constructor stores reference to a grid
  UGGridLocalIdSet (const GridImp& g) : grid_(g) {}

  //! get id of an entity
  template<int cd>
  LocalIdType id (const typename GridImp::Traits::template Codim<cd>::Entity& e) const 
  {
      return grid_.template getRealEntity<cd>(e).localId();
  }

  //! get id of subentity
  template<int cc>
  LocalIdType subid (const typename GridImp::Traits::template Codim<0>::Entity& e, int i) const
  {
      return grid_.template getRealEntity<0>(e).template subLocalId<cc>(i);
  }

    /** \todo Should be private */
    void update() {}

private:

  const GridImp& grid_;
};


}  // namespace Dune


#endif
