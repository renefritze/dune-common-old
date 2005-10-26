#ifndef DUNE_SIZECACHE_HH
#define DUNE_SIZECACHE_HH

namespace Dune {


//! oranizes the caching of sizes for one grid and one GeometryType 
template <class GridImp, GeometryType geomType > 
class SingleTypeSizeCache 
{
  typedef SingleTypeSizeCache<GridImp,geomType> ThisType;
  //! our dimension 
  enum { dim    = GridImp::dimension   };

  //! number of codims 
  enum { nCodim = GridImp::dimension+1 };

  typedef GridImp GridType;
    
  // stores all sizes of the levels 
  mutable std::vector<int> levelSizes_[nCodim]; 
  
  // stores all sizes of leafs 
  mutable int leafSizes_[nCodim]; 

  // the grid 
  const GridType & grid_;

  bool notWorry_; 

  // count elements of set by iterating the grid 
  template <class SzCacheType , int codim >
  struct CountLevelEntities
  {
    static inline int count (const SzCacheType & sc, int level, int cd)
    {   
      if( cd == codim )
      { 
        return sc.template countLevelEntities<codim> (level);
      }
      else
        return CountLevelEntities < SzCacheType, codim-1> :: count (sc,level,cd);
    }
  };

  // count elements of set by iterating the grid 
  template <class SzCacheType>
  struct CountLevelEntities<SzCacheType,0>
  {
    static inline int count (const SzCacheType & sc, int level, int cd)
    {   
      enum { codim = 0 };
      assert( cd == codim );
      return sc.template countLevelEntities<codim> (level);
    }
  };

  
  // count elements of set by iterating the grid 
  template <class SzCacheType , int codim >
  struct CountLeafEntities
  {
    static inline int count (const SzCacheType & sc, int cd)
    {   
      if( cd == codim )
      { 
        return sc.template countLeafElements<codim> ();
      }
      else
        return CountLeafEntities < SzCacheType, codim-1> :: count (sc,cd);
    }
  };

  // count elements of set by iterating the grid 
  template <class SzCacheType>
  struct CountLeafEntities<SzCacheType,0>
  {
    static inline int count (const SzCacheType & sc, int cd)
    {   
      enum { codim = 0 };
      assert( cd == codim );
      return sc.template countLeafElements<codim> ();
    }
  };

  
public:
    SingleTypeSizeCache (const GridType & grid, bool notWorry = false )
      : grid_(grid) , notWorry_ ( notWorry )
    {
      for(int i=0; i<nCodim; i++) 
      {
        leafSizes_[i] = -1;
      }
      
      int numMxl = grid_.maxLevel()+1;
      for(int level = 0; level< numMxl; level++)
      {
        for(int i=0; i<nCodim; i++)
        {  
          std::vector<int> & vec = levelSizes_[i]; 
          vec.resize(numMxl);
          for(int level = 0; level<numMxl; level++) vec[level] = -1;
        }
      }
    }

    //********************************************************************
    // level sizes 
    //********************************************************************
    /** \brief Number of grid entities per level and codim
     * because lbegin and lend are none const, and we need this methods 
     * counting the entities on each level, you know.
     */
    int size (int level, int codim) const
    {
      assert( codim >= 0 );
      assert( codim < nCodim );
      assert( level >= 0 );
      if( level >= (int) levelSizes_[codim].size() ) return 0;

      if( levelSizes_[codim][level] < 0)
        levelSizes_[codim][level] = CountLevelEntities<ThisType,dim>::count(*this,level,codim);
      return levelSizes_[codim][level];
    }
    
    //! number of entities per level, codim and geometry type in this process
    int size (int level, int codim, GeometryType type) const
    {
      if(type != geomType) return 0;
      return size(level,codim);
    }
   
    //********************************************************************
    // leaf sizes 
    //********************************************************************
    //! number of leaf entities per codim in this process
    int size (int codim) const
    {
      assert( codim >= 0 );
      assert( codim < nCodim );
      if( leafSizes_[codim] < 0)
        leafSizes_[codim] = CountLeafEntities<ThisType,dim>::count(*this,codim);
      return leafSizes_[codim];
    }; 
    
    //! number of leaf entities per codim and geometry type in this process
    int size (int codim, GeometryType type) const
    {
      if(type != geomType) return 0;
      return size(codim);
    }

private:
    template <int codim> 
    int countLevelEntities(int level) const 
    {
      typedef typename GridType::template Codim<codim> :: LevelIterator LevelIterator;
      LevelIterator it  = grid_.template lbegin<codim> (level);
      LevelIterator end = grid_.template lend<codim>   (level);
      if( notWorry_ )  return countElements(it,end);
      return countElements(it,end,geomType);
    }
    
    template <int codim> 
    int countLeafEntities() const 
    {
      typedef typename GridType::template Codim<codim> :: LeafIterator LeafIterator;
      LeafIterator it  = grid_.template leafbegin<codim> ();
      LeafIterator end = grid_.template leafend<codim>   ();
      if( notWorry_ )  return countElements(it,end);
      return countElements(it,end,geomType);
    }
    
    // counts entities with given type for given iterator 
    template <class IteratorType> 
    int countElements(IteratorType & it, const IteratorType & end ,
                      GeometryType type) const 
    {
      int count = 0;
      if((type == simplex) || (type == cube))
      {
        for( ; it != end; ++it )
        {
          if(it->geometry().type() == type) 
            count ++ ;
        }
      }
      return count;
    }
   
    // counts entities for given iterator 
    template <class IteratorType> 
    int countElements(IteratorType & it, const IteratorType & end) const 
    {
      int count = 0;
      for( ; it != end; ++it )
      {
        count ++ ;
      }
      return count;
    }
};
  
//! oranizes the caching of sizes for one grid   
template <class GridImp> 
class SizeCache 
{
  typedef SizeCache<GridImp> ThisType;
  typedef GridImp GridType;
  
  SingleTypeSizeCache<GridType,simplex> simplexSize_;
  SingleTypeSizeCache<GridType,cube>    cubeSize_;
  
public:
    SizeCache (const GridType & grid) : simplexSize_(grid), cubeSize_(grid) 
    {
      // check that used grid only has simplex and/or cube as geomTypes 
      const std::vector<GeometryType> & geomTypes = grid.geomTypes();
      int found  = 0;
      int others = 0;
      for(unsigned int i=0; i<geomTypes.size(); i++)
      {
        if( (geomTypes[i] == simplex) || 
            (geomTypes[i] == cube)    )
          found++; 
        else 
          others++;
      }
      // only use with simplex or cube types
      // if others are found assert
      assert( !others );

      // assert that at least one of our tpyes is found 
      assert( found > 0 );
    }

    //********************************************************************
    // level sizes 
    //********************************************************************
    /** \brief Number of grid entities per level and codim
     * because lbegin and lend are none const, and we need this methods 
     * counting the entities on each level, you know.
     */
    int size (int level, int codim) const
    {
      return size(level,codim,simplex) + size(level,codim,cube);
    }
    
    //! number of entities per level, codim and geometry type in this process
    int size (int level, int codim, GeometryType type) const
    {
      if( type == simplex ) return simplexSize_.size(level,codim);
      if( type == cube ) return cubeSize_(level,codim);
      return 0; 
    }
   
    //********************************************************************
    // leaf sizes 
    //********************************************************************
    //! number of leaf entities per codim in this process
    int size (int codim) const
    {
      return size(codim,simplex) + size(codim,cube);
    }; 
    
    //! number of leaf entities per codim and geometry type in this process
    int size (int codim, GeometryType type) const
    {
      if( type == simplex ) return simplexSize_.size(codim);
      if( type == cube ) return cubeSize_(codim);
      return 0; 
    }
};
  

} // end namespace Dune 

#endif
