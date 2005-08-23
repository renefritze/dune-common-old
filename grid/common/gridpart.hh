#ifndef DUNE_GRIDPART_HH
#define DUNE_GRIDPART_HH

//- System includes

//- Dune includes

namespace Dune {

  // Forward declarations
  template <class GridImp>
  class LevelGridPartTraits;
  template <class GridImp>
  class LeafGridPartTraits;

  //! \brief Interface for the GridPart classes
  //! A GridPart class allows to access only a specific subset of a grid's
  //! entities. A GridPart implementation provides the corresponding index set
  //! and a begin/end iterator pair for accessing those entities. GridParts
  //! are used to parametrize spaces (see DiscreteFunctionSpaceDefault).
  template <class GridPartTraits>
  class GridPartInterface {
  public:
    //! Type of the implementation
    typedef typename GridPartTraits::GridPartType GridPartType;
    
  public:
    //! Returns first iterator of the subset of the entities of codimension cd
    //! specified by this class
    template <int cd>
    typename GridPartTraits::template Codim<cd>::IteratorType
    begin() const { return asImp().begin(); }

    //! Returns end iterator of the subset of the entities of codimension cd
    //! specified by this class
    template <int cd>
    typename GridPartTraits::template Codim<cd>::IteratorType
    end() const { return asImp().end(); }

  private:
    GridPartType& asImp() { 
      return static_cast<GridPartType&>(*this); 
    }
    
    const GridPartType& asImp() const { 
      return static_cast<const GridPartType&>(*this);
    }
  };

  //! \brief Default implementation for the GridPart classes
  template <class GridPartTraits>
  class GridPartDefault : 
    public GridPartInterface<GridPartTraits> {
  public:
    //! Grid implementation
    typedef GridPartTraits::GridType GridType;
    //! Index set implementation
    typedef GridPartTraits::IndexSetType IndexSetType;

  public:
    //! Constructor
    GridPartDefault(GridType& grid, IndexSetType& iset) :
      GridPartInterface<GridPartTraits>(),
      grid_(grid),
      iset_(iset) {}

    //! Returns reference to the underlying grid
    //! \note Can't be const in order to allow for marking of entities
    GridType& grid() { return grid_; }
    const GridType& grid() const { return grid_; }

    //! Returns reference to index set of the underlying grid
    IndexSetType& indexSet() { return iset_; }
    const IndexSetType& indexSet() const { return iset_; }

  private:
    GridType& grid_; // needs to modifyable, so that one can mark
    IndexSetType& iset_;
  };

  //! \brief Selects a specific level of a grid
  template <class GridImp>
  class LevelGridPart : 
    public GridPartDefault<LevelGridPartTraits<GridImp> > {
  public:
    //- Public typedefs and enums
    //! Corresponding type definitions
    typedef LevelGridPartTraits<GridImp> Traits;
    //! Grid implementation
    typedef typename Traits::GridType GridType;
    //! Level index set that corresponds to the grid
    typedef typename Traits::IndexSetType IndexSetType;
    
    //! Struct defining the iterator types for codimension cd
    template <int cd>
    struct Codim {
      typedef typename Traits::template Codim<cd>::IteratorType IteratorType;
    };
  public:
    //- Public methods
    //! Constructor
    LevelGridPart(GridType& grid, int level) :
      GridPartDefault<Traits>(grid, grid.levelIndexSet(level)),
      level_(level) {}
    
    //! Returns first iterator on a given level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType begin() const {
      return this->grid().template lbegin<cd>(level_);
    }

    //! Returns end iterator on a given level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType end() const {
      return this->grid().template lend<cd>(level_);
    }
    
    //! Level which this GridPart belongs to
    int level() const { return level; }

  private:
    const int level_;
  };

  //! Type definitions for the LevelGridPart class
  template <class GridImp>
  struct LevelGridPartTraits {
    typedef GridImp GridType;
    typedef LevelGridPart<GridImp> GridPartType;
    typedef typename GridImp::LevelIndexSetType IndexSetType;

    template <int cd>
    struct Codim {
      typedef typename GridImp::template Codim<cd>::LevelIterator IteratorType;
    };
  };

  //! \brief Selects the leaf level of a grid
  template <class GridImp>
  class LeafGridPart :
    public GridPartDefault<LeafGridPartTraits<GridImp> > {
  public:
    //- Public typedefs and enums
    //! Type definitions
    typedef LeafGridPartTraits<GridImp> Traits;
    //! Grid implementation type
    typedef typename Traits::GridType GridType;
    //! The leaf index set of the grid implementation
    typedef typename Traits::IndexSetType IndexSetType;
    
    //! Struct providing types of the leaf iterators on codimension cd
    template <int cd>
    struct Codim {
      typedef typename Traits::template Codim<cd>::IteratorType IteratorType;
    };
  public:
    //- Public methods
    //! Constructor
    LeafGridPart(GridType& grid) :
      GridPartDefault<Traits>(grid, grid.leafIndexSet()) {}

    //! Begin iterator on the leaf level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType begin() const {
      return this->grid().template leafbegin<cd>();
    }

    //! End iterator on the leaf level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType end() const {
      return this->grid().template leafend<cd>();
    }
  };

  //! Type definitions for the LeafGridPart class
  template <class GridImp>
  struct LeafGridPartTraits {
    typedef GridImp GridType;
    typedef LeafGridPart<GridImp> GridPartType;
    typedef typename GridImp::LeafIndexSetType IndexSetType;

    template <int cd>
    struct Codim {
      typedef typename GridImp::template Codim<cd>::LeafIterator IteratorType;
    };
  };

} // end namespace Dune

#endif
