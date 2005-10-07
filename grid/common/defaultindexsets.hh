#ifndef __DUNE_DEFAULTINDEXSETS_HH__
#define __DUNE_DEFAULTINDEXSETS_HH__

#include <vector>
#include <dune/common/misc.hh>
#include <dune/common/array.hh>
#include <dune/grid/common/grid.hh>

#include <rpc/rpc.h>

namespace Dune {

/*!
  The DefaultGridIndexSet is a wrapper for the grid index which can be 
  index of entity or globalIndex. The DofMapper uses an IndexSet for
  mapping the dofs, so we can hide the real grid index behind the index
  set. Furthermore if a grid doesn't provide the consecutive index set
  then this can be calculated in the IndexSet. These two following index
  sets are just the identiy to the grid indices. 

  The DefaultGridIndexSetBase defines some methods that are needed for
  index sets that cope with adaptation, but aren't needed for the following 
  index set, so most of this methods do notin'.
*/

class DefaultEmptyIndexSet
{
  // dummy value 
  enum { myType = -1 };
public:  
  template <class IndexSetType, class EntityType,int enCodim, int codim>
  struct IndexWrapper 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.index(en);
    }
  };

  // if codim and enCodim are equal, then return index 
  template <class IndexSetType, class EntityType, int codim>
  struct IndexWrapper<IndexSetType,EntityType,codim,codim>
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.index(en);
    }
  };

  // return number of vertex num
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,3> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<3> (en,num);
    }
  };
 
  // return number of vertex num for dim == 2 
  // return number of edge num for dim == 3
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,2> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<2> (en,num);
    }
  };

  // return number of vertex for dim == 1
  // return number of edge num for dim == 2 
  // return number of face num for dim == 3
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,1> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<1> (en,num);
    }
  };

  //! default constructor 
  DefaultEmptyIndexSet () {}

  //! return false mean the no memory has to be allocated 
  bool compress () { return false; }

  //! do nothing here, because fathers index should already exist 
  template <class EntityType> 
  void insertNewIndex(const EntityType & en ) {}

  //! do nothing here, because fathers index should already exist 
  template <class EntityType> 
  void removeOldIndex(const EntityType & en ) {}

  //! nothing to do here 
  void resize () {}

  //! no extra memory for restriction is needed
  int additionalSizeEstimate () const { return 0; }

  //! all indices are old 
  bool indexNew(int num, int codim ) const { return false; }

  //! we have no old size 
  int oldSize ( int codim ) const { return 0; }
  
  int type() const { return myType; }

  //! return old index, for dof manager only 
  int oldIndex (int elNum, int codim ) const { return 0; }
  
  //! return new index, for dof manager only 
  int newIndex (int elNum, int codim ) const 
  { 
    return 0; 
  }
  
  //! write index set to xdr file 
  bool write_xdr(const std::basic_string<char> filename , int timestep) 
  {
    FILE  *file;
    XDR   xdrs;
    const char *path = "";

    std::basic_string<char> fnstr  = genFilename(path,filename, timestep);
    const char * fn = fnstr.c_str();
    file = fopen(fn, "wb");
    if (!file)
    {
        std::cerr << "\aERROR in DefaultGridIndexSet::write_xdr(..): could not open <"
                  << filename << ">!" << std::endl;
      return false;
    }

    xdrstdio_create(&xdrs, file, XDR_ENCODE);
    this->processXdr(&xdrs);

    xdr_destroy(&xdrs);
    fclose(file);

    return true;
  }
  
  //! read index set to xdr file 
  bool read_xdr(const std::basic_string<char> filename , int timestep) 
  {
    FILE   *file;
    XDR     xdrs;
    const char *path = "";

    std::basic_string<char> fnstr = genFilename(path,filename, timestep);
    const char * fn  = fnstr.c_str();
    std::cout << "Reading <" << fn << "> \n";
    file = fopen(fn, "rb");
    if(!file)
    {
      std::cerr << "\aERROR in DefaultGridIndexSet::read_xdr(..): could not open <" 
                << filename << ">!" << std::endl;
      return(false);
    }

    // read xdr 
    xdrstdio_create(&xdrs, file, XDR_DECODE);
    this->processXdr(&xdrs);

    xdr_destroy(&xdrs);
    fclose(file);
    return true;
  }

protected: 
  // read/write from/to xdr stream 
  bool processXdr(XDR *xdrs)
  {
    int type = myType; 
    xdr_int ( xdrs, &type);
    if(type != myType)
    {
      std::cerr << "\nERROR: DefaultGridIndex: wrong type choosen! \n\n";
      assert(type == myType);
    }
    return true;
  }
};

//! Wraps the interface methods of indexsets and adds the addiotnal needed
//! functions 
template <class IndexSetImp> 
class IndexSetWrapper : public DefaultEmptyIndexSet 
{
public:
  //! store const reference to set 
  IndexSetWrapper(const IndexSetImp & set) : set_(set) {}
  
  //! store const reference to set 
  IndexSetWrapper(const IndexSetWrapper<IndexSetImp> & s) : set_(s.set_) {}
  
  //! return size of set for codim  
  int size ( int codim , GeometryType type ) const   
  {
    return set_.size(codim,type);
  }

  //! return size of grid entities per level and codim 
  int size ( int codim ) const   
  {
    int s = 0; 
    const std::vector< GeometryType > & types = set_.geomTypes(codim);
    for(unsigned int i=0; i<types.size(); i++)
      s += set_.size(codim,types[i]);
    return s;
  }

  //! return index of en 
  template <class EntityType> 
  int index (const EntityType & en) const
  {
    return set_.index(en); 
  }

  //! return sub index of given entities sub entity with codim and number 
  template <int codim,class EntityType> 
  int subIndex (const EntityType & en, int num) const
  {
    return set_.template subIndex<codim> (en,num);
  }

  //! wrap geomTypes method of set 
  const std::vector< GeometryType > & geomTypes (int codim) const 
  {
    return set_.geomTypes(codim); 
  }

  //! return index 
  template <int codim, class EntityType> 
  int index (const EntityType & en, int num) const
  {
    enum { enCodim = EntityType::codimension };
    return IndexWrapper<IndexSetImp,EntityType,enCodim,codim>::index(set_,en,num);
  }

private: 
  const IndexSetImp & set_; 
};

template <class GridType>
class DefaultGridIndexSetBase : public DefaultEmptyIndexSet
{
  // dummy value 
  enum { myType = -1 };
public:  
  enum { ncodim = GridType::dimension + 1 };

  //! Conschdrugdor 
  DefaultGridIndexSetBase (const GridType & grid ) : grid_ (grid) {}
protected:
  // the corresponding grid 
  const GridType & grid_;
};

//! Wraps LevelIndexSet for use with LagrangeFunctionSpace 
template <class GridType, GridIndexType GridIndex = LevelIndex>
class DefaultGridIndexSet : public DefaultGridIndexSetBase <GridType> 
                            
{
  // my type, to be revised 
  enum { myType = 1 };

  //! return global number of sub entity with local number 'num' 
  //! and codim of the sub entity = codim
  //! the enCodim is needed because the method index and subIndex have
  //! different names, ;)
  //! the wrapper classes allow partial specialisation, 
  //! which is not allowed, but needed badly, :) the bob-trick
  //
  //! default implementation returns index of given entity
  template <class IndexSetType, class EntityType,int enCodim, int codim>
  struct IndexWrapper 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.index(en);
    }
  };

  // if codim and enCodim are equal, then return index 
  template <class IndexSetType, class EntityType, int codim>
  struct IndexWrapper<IndexSetType,EntityType,codim,codim>
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.index(en);
    }
  };

  // return number of vertex num
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,3> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<3> (en,num);
    }
  };
 
  // return number of vertex num for dim == 2 
  // return number of edge num for dim == 3
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,2> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<2> (en,num);
    }
  };

  // return number of vertex for dim == 1
  // return number of edge num for dim == 2 
  // return number of face num for dim == 3
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,1> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<1> (en,num);
    }
  };

  typedef typename GridType :: Traits :: LevelIndexSet LevelIndexSetType;
  const LevelIndexSetType & set_; 
  
  typedef typename GridType :: template Codim<0>:: Entity EntityCodim0Type; 
public:
  enum { ncodim = GridType::dimension + 1 };

  //! Constructor getting grid and level for Index Set 
  DefaultGridIndexSet (const GridType & grid , const int level ) 
    : DefaultGridIndexSetBase <GridType> (grid) , set_(grid.levelIndexSet(level)) {}
 
  //! Constructor choosing the grid's maxlevel as given level 
  DefaultGridIndexSet (const GridType & grid ) 
    : DefaultGridIndexSetBase <GridType> (grid) , set_(grid.levelIndexSet(grid.maxLevel())) {}

  //! return size of grid entities per level and codim 
  int size ( int codim , GeometryType type ) const   
  {
    return set_.size(codim,type);
  }

  //! return size of grid entities per level and codim 
  int size ( int codim ) const   
  {
    int s = 0; 
    const std::vector< GeometryType > & types = set_.geomTypes(codim);
    for(unsigned int i=0; i<types.size(); i++)
      s += set_.size(codim,types[i]);
    return s;
  }

  //! return index of entity with codim codim belonging to entity en which
  //! could have a bigger codim (for example return num of vertex num of an
  //! element en 
  template <int codim, class EntityType> 
  int index (const EntityType & en, int num) const
  {
    return IndexWrapper<LevelIndexSetType,EntityType,EntityType::codimension,codim>::index(set_,en,num);
  }

  //! wrap index method of set
  template <class EntityType> 
  int index (const EntityType & en) const
  {
    return set_.index(en); 
  }

  //! wrap subIndex method of set
  template <int codim> 
  int subIndex (const EntityCodim0Type & en, int num) const
  {
    return set_.template subIndex<codim> (en,num); 
  }

  //! wrap geomTypes method of set 
  const std::vector< GeometryType > & geomTypes (int codim) const 
  {
    return set_.geomTypes(codim); 
  }

  //! return type of index set (for input/output)
  int type() const { return myType; }
};

//! Wraps HierarchicIndex Sets of AlbertaGrid and ALUGrid 
template <class GridType>
class DefaultGridIndexSet<GridType,GlobalIndex> 
: public DefaultGridIndexSetBase <GridType>
{
  // my type, to be revised 
  enum { myType = 0 };

  template <class IndexSetType, class EntityType,int enCodim, int codim>
  struct IndexWrapper 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.index(en);
    }
  };

  // if codim and enCodim are equal, then return index 
  template <class IndexSetType, class EntityType, int codim>
  struct IndexWrapper<IndexSetType,EntityType,codim,codim>
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.index(en);
    }
  };

  // return number of vertex num
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,3> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<3> (en,num);
    }
  };
 
  // return number of vertex num for dim == 2 
  // return number of edge num for dim == 3
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,2> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<2> (en,num);
    }
  };

  // return number of vertex for dim == 1
  // return number of edge num for dim == 2 
  // return number of face num for dim == 3
  template <class IndexSetType, class EntityType>
  struct IndexWrapper<IndexSetType,EntityType,0,1> 
  {
    static inline int index (const IndexSetType & set, const EntityType & en , int num )
    {
      return set.template subIndex<1> (en,num);
    }
  };
  
  typedef typename GridType :: HierarchicIndexSet HSetType;

  // reference to hierarchical index set 
  const HSetType & set_;

  typedef typename GridType :: template Codim<0> :: Entity EntityCodim0Type; 
public:
  enum { ncodim = GridType::dimension + 1 };
  DefaultGridIndexSet ( const GridType & grid , const int level =-1 ) 
    : DefaultGridIndexSetBase <GridType> (grid) 
    , set_(grid.hierarchicIndexSet()) {}
     
  //! wrap method size of set 
  int size ( int codim , GeometryType type ) const   
  {
    return set_.size(codim,type);
  }

  //! return size of grid entities per level and codim 
  int size ( int codim ) const   
  {
    int s = 0; 
    const std::vector< GeometryType > & types = set_.geomTypes(codim);
    for(unsigned int i=0; i<types.size(); i++)
      s += set_.size(codim,types[i]);
    return s;
  }

  //! index method for Lagrange Mapper 
  template <int codim, class EntityType> 
  int index (const EntityType & en, int num) const
  {
    enum { enCodim = EntityType::codimension };
    return IndexWrapper<HSetType,EntityType,enCodim,codim>::index(set_,en,num);
  }

  //! wrap index method of set
  template <class EntityType> 
  int index (const EntityType & en) const
  {
    return set_.index(en); 
  }

  //! wrap subIndex method of set
  template <int codim> 
  int subIndex (const EntityCodim0Type & en, int num) const
  {
    return set_.template subIndex<codim> (en,num); 
  }

  //! wrap geomTypes method of set 
  const std::vector< GeometryType > & geomTypes (int codim) const 
  {
    return set_.geomTypes(codim); 
  }

  //! return type (for Grape In/Output)
  int type() const { return myType; }
};

//*********************************************************************
/*! \brief 
 * DefaultLevelIndexSet generates a level index set for a grid out of a
 * grids hierarchic index set by storing for each entity in the grid 
 * a number in an array. 
 */
//*********************************************************************

template <class DefaultLevelIndexSetType, int codim>
struct CalcLevelForCodim 
{
  static void recursive(DefaultLevelIndexSetType & d)
  {
    d.template calcLevelIndexForCodim<codim> (); 
    CalcLevelForCodim<DefaultLevelIndexSetType,codim-1>::recursive(d);
  }
};

template <class DefaultLevelIndexSetType>
struct CalcLevelForCodim<DefaultLevelIndexSetType,0>
{
  static void recursive(DefaultLevelIndexSetType & d)
  {
    d.template calcLevelIndexForCodim<0> (); 
  }
};


template <class GridImp>
struct DefaultLevelIteratorTypes
{
  //! The types
  template<int cd>
  struct Codim
  {
    template<PartitionIteratorType pitype>
    struct Partition
    {
      typedef typename GridImp::Traits::template Codim<cd>::template Partition<pitype>::LevelIterator Iterator;
    };
  };
};

/*! \brief 
  DefaultLevelIndexSet creates a LevelIndexSet for a Grid by using its
  HierarchicIndexSet 
 */ 
template <class GridImp>
class DefaultLevelIndexSet :
  public IndexSet< GridImp, 
                   DefaultLevelIndexSet <GridImp>,
                   DefaultLevelIteratorTypes<GridImp> > 

{
  typedef GridImp GridType;
  enum { dim = GridType :: dimension };
  enum { numCodim = dim + 1 };
  typedef typename GridType :: HierarchicIndexSet HierarchicIndexSetType;

  typedef DefaultLevelIndexSet<GridType> MyType;

public:
  enum { ncodim = GridType::dimension + 1 };
  //! create LevelIndex by using the HierarchicIndexSet of a grid 
  //! for the given level 
  DefaultLevelIndexSet(const GridType & grid , int level ) :
    grid_(grid) , level_(level) , hIndexSet_ ( grid.hierarchicIndexSet() )
    , size_ ( numCodim )
  {
    calcNewIndex ();
  }

  //! return LevelIndex of given entity 
  template<int cd>
  int index (const typename GridImp::template Codim<cd>::Entity& en) const 
  {
    assert( level_ == en.level() );
    return levelIndex_[cd][ hIndexSet_.index(en) ];
  }
 
  //! return subIndex (LevelIndex) for a given Entity of codim = 0 and a
  //! given SubEntity codim and number of SubEntity
  template <int cd>
  int subIndex (const typename GridType::template Codim<0>::Entity & en, int i) const
  {
    assert( level_ == en.level() );
    return levelIndex_[cd][ hIndexSet_.template subIndex<cd>(en,i) ];
  }

  //! return size of IndexSet for a given level and codim 
  int size ( int codim ) const
  {
    return size_[codim];
  }

  //! return size of IndexSet for a given level and codim 
  //! this method is to be revised 
  int size ( int codim , GeometryType type ) const
  {
    return size_[codim];
  }

  //! do calculation of the index set, has to be called when grid was
  //! changed or if index set is created 
  void calcNewIndex ()
  {
    // make new size and set all levels to -1 ==> new calc
    CalcLevelForCodim<MyType,dim>::recursive(*this);
  }

  // calculate index for the codim 
  template <int cd>
  void calcLevelIndexForCodim ()
  {
    int nEntities = hIndexSet_.size(cd);
    Array<int> & levIndex = levelIndex_[cd];
    // resize memory if necessary 
    if(nEntities > levIndex.size())
      makeNewSize(levIndex, nEntities);

    // walk grid and store index 
    typedef typename GridType::Traits::template Codim<cd>::LevelIterator LevelIterator;
    int num = 0;
    LevelIterator endit  = grid_.template lend< cd >   (level_);
    for(LevelIterator it = grid_.template lbegin< cd > (level_); it != endit; ++it)
    {
      int no = hIndexSet_.index(*it);
      levIndex[no] = num;
      num++;
    }
    // remember the number of entity on level and cd = 0
    size_[cd] = num;
  }

  //! deliver all geometry types used in this grid
  const std::vector<GeometryType>& geomTypes (int codim) const
  {
    return grid_.geomTypes(codim);
  }


  /** @brief Iterator to first entity of given codimension and partition type.
   */
  template<int cd, PartitionIteratorType pitype>
  typename DefaultLevelIteratorTypes<GridImp>::template Codim<cd>::
    template Partition<pitype>::Iterator begin (int level) const
  { 
    return this->grid_.template lbegin<cd,pitype> (level);
  }

  /** @brief Iterator to one past the last entity of given codim for partition type
   */
  template<int cd, PartitionIteratorType pitype>
  typename DefaultLevelIteratorTypes<GridImp>::template Codim<cd>::
    template Partition<pitype>::Iterator end (int level) const
  { 
    return this->grid_.template lend<cd,pitype> (level);
  }

private:
  // grid this level set belongs to 
  const GridType & grid_;

  // the level for which this index set is created 
  const int level_;
  
  // the grids HierarchicIndexSet
  const HierarchicIndexSetType & hIndexSet_;

  void makeNewSize(Array<int> &a, int newNumberOfEntries)
  {
    if(newNumberOfEntries > a.size())
    {
      a.resize(newNumberOfEntries);
    }
    for(int i=0; i<a.size(); i++) a[i] = -1;
  }

  // number of entitys of each level an codim 
  Array<int> size_;

  //*********************************************************
  // Methods for mapping the hierarchic Index to index on Level
  Array<int> levelIndex_[numCodim];
};

} // end namespace Dune 

#endif
