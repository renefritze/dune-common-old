#ifndef DUNE_GRIDPART_HH
#define DUNE_GRIDPART_HH

//- System includes

//- Dune includes
#include <dune/grid/common/grid.hh>

namespace Dune {

  // Forward declarations
  template <class GridImp, PartitionIteratorType pitype>
  class LevelGridPartTraits;
  template <class GridImp, PartitionIteratorType pitype>
  class LeafGridPartTraits;
  template <class GridImp,class IndexSetImp, PartitionIteratorType pitype>
  struct DefaultGridPartTraits;

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

    //! Level of the grid part
    int level() const { return asImp().end(); }

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
    typedef typename GridPartTraits::GridType GridType;
    //! Index set implementation
    typedef typename GridPartTraits::IndexSetType IndexSetType;

  public:
    //! Constructor
    GridPartDefault(const GridType& grid, const IndexSetType& iset) :
      GridPartInterface<GridPartTraits>(),
      grid_(grid),
      iset_(iset) {}

    //! Returns reference to the underlying grid
    const GridType& grid() const { return grid_; }

    //! Returns reference to index set of the underlying grid
    const IndexSetType& indexSet() const { return iset_; }

  private:
    const GridType& grid_; 
    const IndexSetType& iset_;
  };

  //! \brief Selects a specific level of a grid
  template <class GridImp, PartitionIteratorType pitype = Interior_Partition>
  class LevelGridPart : 
    public GridPartDefault<LevelGridPartTraits<GridImp,pitype> > {
  public:
    //- Public typedefs and enums
    //! Corresponding type definitions
    typedef LevelGridPartTraits<GridImp,pitype> Traits;
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
    LevelGridPart(const GridType& grid, int level) :
      GridPartDefault<Traits>(grid, grid.levelIndexSet(level)),
      level_(level) {}
    
    //! Returns first iterator on a given level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType begin() const {
      return this->grid().template lbegin<cd,pitype>(level_);
    }

    //! Returns end iterator on a given level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType end() const {
      return this->grid().template lend<cd,pitype>(level_);
    }
    
    //! Level which this GridPart belongs to
    int level() const { return level_; }

  private:
    const int level_;
  };

  //! Type definitions for the LevelGridPart class
  template <class GridImp, PartitionIteratorType pitype>
  struct LevelGridPartTraits {
    typedef GridImp GridType;
    typedef LevelGridPart<GridImp,pitype> GridPartType;
    typedef typename GridImp::Traits::LevelIndexSet IndexSetType;

    template <int cd>
    struct Codim {
      typedef typename GridImp::template Codim<cd>::template Partition<pitype>::LevelIterator IteratorType;
    };
  };

  //! \brief Selects the leaf level of a grid
  template <class GridImp, PartitionIteratorType pitype = Interior_Partition>
  class LeafGridPart :
    public GridPartDefault<LeafGridPartTraits<GridImp,pitype> > {
  public:
    //- Public typedefs and enums
    //! Type definitions
    typedef LeafGridPartTraits<GridImp,pitype> Traits;
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
    LeafGridPart(const GridType& grid) :
      GridPartDefault<Traits>(grid, grid.leafIndexSet()) {}

    //! Begin iterator on the leaf level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType begin() const {
      return this->grid().template leafbegin<cd,pitype>();
    }

    //! End iterator on the leaf level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType end() const {
      return this->grid().template leafend<cd,pitype>();
    }

    //! Returns maxlevel of the grid
    int level() const { return this->grid().maxlevel(); }
  };

  //! Type definitions for the LeafGridPart class
  template <class GridImp,PartitionIteratorType pitype>
  struct LeafGridPartTraits {
    typedef GridImp GridType;
    typedef LeafGridPart<GridImp,pitype> GridPartType;
    typedef typename GridImp::Traits :: LeafIndexSet IndexSetType;

    template <int cd>
    struct Codim {
      typedef typename GridImp::template Codim<cd>::template Partition<pitype>::LeafIterator IteratorType;
    };
  };

  //! quick hack, to be revised by me 
  //! \brief Selects the leaf level of a grid
  template <class GridImp, class IndexSetImp , PartitionIteratorType pitype = Interior_Partition>
  class DefaultGridPart :
    public GridPartDefault<DefaultGridPartTraits<GridImp,IndexSetImp,pitype> > {
  public:
    //- Public typedefs and enums
    //! Type definitions
    typedef DefaultGridPartTraits<GridImp,IndexSetImp,pitype> Traits;
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
    DefaultGridPart(const GridType& grid, const IndexSetType & iset ) :
      GridPartDefault<Traits>(grid, iset) {}

    //! Begin iterator on the leaf level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType begin() const {
      return this->grid().template leafbegin<cd,pitype>();
    }

    //! End iterator on the leaf level
    template <int cd>
    typename Traits::template Codim<cd>::IteratorType end() const {
      return this->grid().template leafend<cd,pitype>();
    }

    //! Level of the grid part
    int level() const { return this->grid().maxlevel(); }
  };

  //! Type definitions for the LeafGridPart class
  template <class GridImp,class IndexSetImp, PartitionIteratorType pitype>
  struct DefaultGridPartTraits {
    typedef GridImp GridType;
    typedef DefaultGridPart<GridImp,IndexSetImp,pitype> GridPartType;
    typedef IndexSetImp IndexSetType;

    template <int cd>
    struct Codim {
      typedef typename GridImp::template Codim<cd>::template Partition<pitype>::LeafIterator IteratorType;
    };
  };


} // end namespace Dune

#endif
