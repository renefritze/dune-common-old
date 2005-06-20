#ifndef __DUNE_DEFAULTINDEXSETS_HH__
#define __DUNE_DEFAULTINDEXSETS_HH__

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

/*! /brief Interface of IndexSet for DofManager, some functions that only
 * are called once per timestep are virtual ofr simplicity  
 */
class IndexSetInterface 
{
public:
  virtual bool compress () = 0;
  virtual void resize   () = 0;
};
  
template <class GridType>
class DefaultGridIndexSetBase : public IndexSetInterface
{
  // dummy value 
  enum { myType = -1 };
public:  
  enum { ncodim = GridType::dimension + 1 };
  
  DefaultGridIndexSetBase (const GridType & grid ) : grid_ (grid) {}

  //! return false mean the no memory has to be allocated 
  virtual bool compress () { return false; }

  //! do nothing here, because fathers index should already exist 
  void insertNewIndex(const typename GridType::template codim<0>::Entity & en ) {}

  //! do nothing here, because fathers index should already exist 
  void removeOldIndex(const typename GridType::template codim<0>::Entity & en ) {}

  //! nothing to do here 
  virtual void resize () {}

  //! no extra memory for restriction is needed
  int additionalSizeEstimate () const { return 0; }

  //! all indices are old 
  bool indexNew(int num, int codim ) const { return false; }

  //! we have no old size 
  int oldSize ( int level , int codim ) const 
  { 
    return 0; 
  }
  
  //! return old index, for dof manager only 
  int oldIndex (int elNum, int codim ) const 
  { 
    return 0; 
  }
  
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

  // the corresponding grid 
  const GridType & grid_;
};

//! Default is the Identity
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
  template <class EntityType,int enCodim, int codim>
  struct IndexWrapper 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.index();
    }
  };

  // if codim and enCodim are equal, then return index 
  template <class EntityType, int codim>
  struct IndexWrapper<EntityType,codim,codim>
  {
    static inline int index (EntityType & en , int num )
    {
      return en.index();
    }
  };

  // return number of vertex num
  template <class EntityType>
  struct IndexWrapper<EntityType,0,3> 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.template subIndex<3> (num);
    }
  };
 
  // return number of vertex num for dim == 2 
  // return number of edge num for dim == 3
  template <class EntityType>
  struct IndexWrapper<EntityType,0,2> 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.template subIndex<2> (num);
    }
  };

  // return number of vertex for dim == 1
  // return number of edge num for dim == 2 
  // return number of face num for dim == 3
  template <class EntityType>
  struct IndexWrapper<EntityType,0,1> 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.template subIndex<1> (num);
    }
  };

public:
  enum { ncodim = GridType::dimension + 1 };
  DefaultGridIndexSet (const GridType & grid ) : DefaultGridIndexSetBase <GridType> (grid) {}

  //! return size of grid entities per level and codim 
  int size ( int level , int codim ) const   
  {
    return this->grid_.size(level,codim);
  }

  //! return index of entity with codim codim belonging to entity en which
  //! could have a bigger codim (for example return num of vertex num of an
  //! element en 
  template <int codim, class EntityType> 
  int index (EntityType & en, int num) const
  {
    return IndexWrapper<EntityType,EntityType::codimension,codim>::index(en,num);
  }
};

template <class GridType>
class DefaultGridIndexSet<GridType,GlobalIndex> 
: public DefaultGridIndexSetBase <GridType>
{
  // my type, to be revised 
  enum { myType = 0 };
  
  template <class EntityType,int enCodim, int codim>
  struct IndexWrapper 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.globalIndex();
    }
  };

  template <class EntityType, int codim>
  struct IndexWrapper<EntityType,codim,codim>
  {
    static inline int index (EntityType & en , int num )
    {
      return en.globalIndex();
    }
  };

  //! if codim > codim of entity use subIndex 
  template <class EntityType>
  struct IndexWrapper<EntityType,0,2> 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.template subIndex<2> (num);
    }
  };
  
  template <class EntityType>
  struct IndexWrapper<EntityType,0,3> 
  {
    static inline int index (EntityType & en , int num )
    {
      return en.template subIndex<3> (num);
    }
  };

public:
  enum { ncodim = GridType::dimension + 1 };
  DefaultGridIndexSet ( GridType & grid ) : DefaultGridIndexSetBase <GridType> (grid) {}
      
  int size ( int level , int codim ) const   
  {
    return this->grid_.global_size(codim);
  }

  template <int codim, class EntityType> 
  int index (EntityType & en, int num) const
  {
    return IndexWrapper<EntityType,EntityType::codimension,codim>::index(en,num);
  }
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

/*! \brief 
  DefaultLevelIndexSet creates a LevelIndexSet for a Grid by using its
  HierarchicIndexSet 
 */ 
template <class GridType>
class DefaultLevelIndexSet
{
  enum { dim = GridType :: dimension };
  enum { numCodim = dim + 1 };
  
  typedef typename GridType :: HierarchicIndexSetType HierarchicIndexSetType;

  typedef DefaultLevelIndexSet<GridType> MyType;
public:
  //! create LevelIndex by using the HierarchicIndexSet of a grid 
  DefaultLevelIndexSet(const GridType & grid ) :
    grid_(grid) , hIndexSet_ ( grid.hierarchicIndexSet() ) 
  {
    calcNewIndex ();
  }

  //! return LevelIndex of given entity 
  template <class EntityType>
  int index (const EntityType & en) const
  {
    enum { cd = EntityType :: codimension };
    return indexOnLevel< cd > (hIndexSet_.index(en),en.level());
  }
 
  //! return subIndex (LevelIndex) for a given Entity of codim = 0 and a
  //! given SubEntity codim and number of SubEntity
  template <int cd>
  int subIndex (const typename GridType::template codim<0>::Entity & en, int i) const
  {
    return indexOnLevel< cd > (hIndexSet_.template subIndex<cd>(en,i),en.level());
  }

  //! return size of IndexSet for a given level and codim 
  int size ( int level, int codim ) const
  {
    return size_[level*numCodim + codim];
  }

private:
  // grid this level set belongs to 
  const GridType & grid_;
  // the grids HierarchicIndexSet
  const HierarchicIndexSetType & hIndexSet_;

  //! map the hierarchicIndex from the grid to the index on Level
  template <int cd>
  int indexOnLevel(int hIndex, int level ) const
  {
    return levelIndex_[cd][level][hIndex];
  };

public:
  template <int cd>
  void calcLevelIndexForCodim ()
  {
    int nEntities = hIndexSet_.size(grid_.maxlevel(),cd);
    for(int level=0; level <= grid_.maxlevel(); level++)
    {
      Array<int> & levIndex = levelIndex_[cd][level];
      // resize memory if necessary 
      if(nEntities > levIndex.size())
        makeNewSize(levIndex, nEntities);

      // walk grid and store index 
      typedef typename GridType::Traits::template codim<cd>::LevelIterator LevelIterator;
      int num = 0;
      LevelIterator endit  = grid_.template lend< cd >   (level);
      for(LevelIterator it = grid_.template lbegin< cd > (level); it != endit; ++it)
      {
        int no = hIndexSet_.index(*it);
        levIndex[no] = num;
        num++;
      }
      // remember the number of entity on level and cd = 0
      size_[level*numCodim + cd ] = num;
    }
  }

  void calcNewIndex ()
  {
    int numLevel = grid_.maxlevel() + 1;
    // make new size and set all levels to -1 ==> new calc
    if( (numLevel) * (numCodim) > size_.size() )
      makeNewSize(size_, 2*((numLevel) * numCodim) );

    CalcLevelForCodim<MyType,dim>::recursive(*this);
  }

private:
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
  enum { MAXL = 128 }; // to be revised 
  Array<int> levelIndex_[dim+1][MAXL];
};

} // end namespace Dune 

#endif
