#ifndef __DUNE_LEAFINDEXSET_HH__
#define __DUNE_LEAFINDEXSET_HH__

#include <dune/common/array.hh>
#include <dune/grid/common/defaultindexsets.hh>

namespace Dune { 

/*! 
 This class provides an Array which as an aditional resize and copy the old
 values functionality. 
*/
template <class T> 
class IndexArray : public Array<T> 
{
public:
  IndexArray() : Array<T> () 
  {    
    this->n = 0; this->p = 0;
  }

  //! reallocate array with size m
  void resizeAndCopy (int m, int factor)
  {
    if(m <= this->n) return;
    int newSize = m*factor;
    T * newMem = 0;
    
    try 
    {
      newMem = new T[newSize];
    }
    catch (std::bad_alloc) 
    {
      std::cerr << "Not enough memory!" << std::endl;
      throw;
    }

    if(this->n > 0)
      std::memcpy(newMem , this->p , this->n*sizeof(T));
    
    this->n = newSize;
    if(this->p) delete [] this->p;   
    this->p = newMem;
  }

  //! write Array to xdr stream
  bool processXdr(XDR *xdrs)
  {
    if(xdrs != NULL)
    {
      int len = this->n;
      xdr_int( xdrs, &len );
      if(len != this->n) this->resize(len);
        xdr_vector(xdrs,(char *) this->p,this->n,sizeof(T),(xdrproc_t)xdr_double);
      return true;
    }
    else
      return false;
  }

};

//******************************************************************
//
// Indexset that provides consecutive indicies for the leaf level
// this index set uses the grid global_index 
//
//******************************************************************
/*! 
  This index set generates and consecutive leaf index out of the unique
  global index of each entity. This index set can be used instead of the
  default grid index sets an can be generated for each grid implementation.

  Note that only codim = 0 is working at the moment. 
*/
template <class GridType>
class AdaptiveLeafIndexSet : public DefaultGridIndexSetBase <GridType>
{
  enum INDEXSTATE { NEW, USED, UNUSED };
  
  // the mapping of the global to leaf index 
  IndexArray<int> leafIndex_;

  // old indices 
  IndexArray<int> oldLeafIndex_;

  // stack for holes 
  IndexArray<int> holes_; 

  // the state of each index 
  IndexArray<INDEXSTATE> state_;
 
  // next index to give away 
  int nextFreeIndex_;

  // actual size of index set  
  int actSize_;
  
  // number of hole we have 
  int actHole_;
  
  // size of old index set 
  int oldSize_;

public:
  enum { ncodim = GridType::dimension + 1 };
  AdaptiveLeafIndexSet ( GridType & grid ) : DefaultGridIndexSetBase <GridType> (grid) , 
    nextFreeIndex_ (0), actSize_(0), actHole_(0), oldSize_(0)
  {
    // create index set 
    resize();
    markAllUsed();
  }

  //! create index for father entity, needed for restriction  
  template <class EntityType>
  void createFatherIndex (EntityType &en ) 
  {
    this->insert( en );
  }

  //! if grid has changed, resize index vectors, and create 
  //! indices for new entities, new entities are entities that 
  //! lie below the old entities 
  void resize () 
  {
    int oldSize = leafIndex_.size();
   
    //std::cout << leafIndex_.size() << " " << this->grid_.global_size(0) << " Leaf Grid Size \n";
    if(leafIndex_.size() < this->grid_.global_size(0))
    {
      leafIndex_.resizeAndCopy(this->grid_.global_size(0),1); 
      state_.resizeAndCopy(this->grid_.global_size(0),1); 

      // here we dont need to copy 
      oldLeafIndex_.resizeAndCopy(this->grid_.global_size(0),1); 
      holes_.resizeAndCopy(this->grid_.global_size(0),1); 
    }

    for(int i=oldSize; i<leafIndex_.size(); i++)
    {
      leafIndex_[i] = -1;
    }

    // give all entities that lie below the old entities new numbers 
    markAllBelowOld ();
  }


  //! for dof manager, to check whether it has to copy dof or not 
  bool indexNew (int num) 
  {
    assert(num < state_.size());
    return state_[num] == NEW;
  }

  //! make to index numbers consecutive 
  //! return true, if at least one hole was closed 
  bool compress ()
  {
    bool haveToCopy = false;

    // mark which indices are still used 
    markAllUsed();

    // mark holes 
    actHole_ = 0;
    for(int i=0; i<state_.size(); i++)
    {
      if((state_[i] == UNUSED) && (leafIndex_[i] >= 0)) 
      { 
        holes_[actHole_] = leafIndex_[i];
        actHole_++;
      }
    }

    // copy index, for copying in dof manager 
    oldLeafIndex_ = leafIndex_;
   
    // close holes 
    for(int i=0; i<leafIndex_.size(); i++)
    { 
      if(state_[i] != UNUSED) 
      {
        if(leafIndex_[i] >= actSize_)
        {
          // serach next hole that is smaler than actual size 
          actHole_--;
          while ( holes_[actHole_] >= actSize_)
          {
            actHole_--;
          }
          assert(actHole_ >= 0);
          leafIndex_[i] = holes_[actHole_];

          // means that dof manager has to copy the mem
          state_[i] = NEW;
          haveToCopy = true;
        }
      }
      else 
      {
        leafIndex_[i] = -1;
      }
    }

    // the next index that can be given away is equal to size
    nextFreeIndex_ = actSize_;

    //print(true);

    return haveToCopy;
  }

  // memorise index 
  template <class EntityType>
  void insert (EntityType & en)
  {
    this->insert ( en.global_index() );
  }

  //! return how much extra memory is needed for restriction 
  int tmpSize () const { return 2 * nextFreeIndex_; }

  //! return size of grid entities per level and codim 
  int size ( int level , int codim ) const
  {
    return nextFreeIndex_;
  }

  //! return global index 
  //! for dof mapper 
  template <int codim, class EntityType>
  int index (EntityType & en, int num) const
  {
    assert(codim == 0);
    return leafIndex_[en.global_index()];
  }
  
  //! return size of grid entities per level and codim 
  //! for dof mapper 
  int oldSize ( int level , int codim ) const
  {
    return state_.size();
  }

  //! return old index, for dof manager only 
  int oldIndex (int elNum) const
  {
    return oldLeafIndex_[elNum];
  }

  //! return new index, for dof manager only 
  int newIndex (int elNum) const
  {
    return leafIndex_[elNum];
  }

private:
  // memorise index 
  template <class EntityType>
  bool insertNewIndex (EntityType & en, bool canInsert )
  {
    // which is the case if we havent reached a entity which has 
    // already a number 
    if(!canInsert) 
    {
      // from now on, indices can be inserted 
      if(leafIndex_[en.global_index()] >= 0)
        return true; 

      // we have to go deeper 
      return false;
    }
    else 
      this->insert ( en );
   
    return true;
  }

  // insert element and create index for element number  
  void insert (int num )
  {
    assert(num < leafIndex_.size() );

    if(leafIndex_[num] < 0)
    {
      leafIndex_[num] = nextFreeIndex_;
      nextFreeIndex_++;
    }
    state_[num] = USED;
  }
  
  //! mark indices that are still used and give new indices to 
  //! elements that need one 
  void markAllUsed () 
  {
    typedef typename GridType::LeafIterator LeafIterator; 

    for(int i=0; i<state_.size(); i++) state_[i] = UNUSED;
    
    LeafIterator it    = this->grid_.leafbegin ( this->grid_.maxlevel());
    LeafIterator endit = this->grid_.leafend   ( this->grid_.maxlevel());
   
    oldSize_ = nextFreeIndex_;
    
    actSize_ = 0;

    // walk over leaf level on locate all needed entities  
    for( ; it != endit ; ++it )
    {
      this->insert( *it );
      actSize_++;
    }
  }

  //! give all entities that lie below the old entities new numbers 
  void markAllBelowOld () 
  {
    typedef typename GridType::template Traits<0>::LevelIterator LevelIterator; 

    LevelIterator macroit  = this->grid_.template lbegin<0> (0);
    LevelIterator macroend = this->grid_.template lend  <0> (0);

    int maxlevel = this->grid_.maxlevel();
    
    for( ; macroit != macroend; ++macroit )
    {
      typedef typename GridType::template Traits<0>::
            Entity::Traits::HierarchicIterator HierarchicIterator; 
     
      bool areNew = false;
      
      HierarchicIterator it    = macroit->hbegin ( maxlevel );
      HierarchicIterator endit = macroit->hend   ( maxlevel );
      
      for( ; it != endit ; ++it )
      {
        // areNEw == true, then index is inserted 
        areNew = insertNewIndex  ( *it , areNew ); 
      }
    }
  }
 
  // print interal data, for debugging only 
  void print (bool oldtoo = false ) const
  {
    std::cout << "Size " << leafIndex_.size() << "\n";
    std::cout << "i    |   val    | state  \n";
    int actSize =0;
    for(int i=0; i<leafIndex_.size(); i++)
    {
      if(state_[i] != UNUSED) actSize++;
      std::cout << i << " | " << leafIndex_[i] << " | " << state_[i];
      if(oldtoo) std::cout << " | " << oldLeafIndex_[i];
      std::cout << "\n";
    }
    std::cout << "Real Size " << nextFreeIndex_ << "\n";
    std::cout << "ActSize   " << actSize << "\n";
  }

public:

// write indexset to xdr file 
bool write_xdr(const char * filename, int timestep)
{
  FILE  *file;
  XDR   xdrs;
  const char *path = NULL;

  const char * fn  = genFilename(path,filename, timestep);
  file = fopen(fn, "wb");
  if (!file)
  {
    fprintf(stderr,"\aERROR in AdaptiveLeafIndexSet::write_xdr(..): couldnot open <%s>!\n", filename);
    fflush(stderr);
    return false;
  }

  xdrstdio_create(&xdrs, file, XDR_ENCODE);
  this->processXdr(&xdrs);

  xdr_destroy(&xdrs);
  fclose(file);
}

//! read index set from given xdr file 
bool read_xdr(const char * filename , int timestep)
{
  FILE   *file;
  XDR     xdrs;
  const char *path = NULL;

  const char * fn  = genFilename(path,filename, timestep);
  std::cout << "Reading <" << fn << "> \n";
  file = fopen(fn, "rb");
  if(!file)
  {
    fprintf(stderr,"\aERROR in AdaptiveLeafIndexSet::read_xdr(..): couldnot open <%s>!\n", filename);
    fflush(stderr);
    return(false);
  }

  // read xdr 
  xdrstdio_create(&xdrs, file, XDR_DECODE);
  this->processXdr(&xdrs);

  xdr_destroy(&xdrs);
  fclose(file);
  return true;
}

private:
  // read/write from/to xdr stream 
  bool processXdr(XDR *xdrs)
  {
    xdr_int ( xdrs, &nextFreeIndex_ );
    xdr_int ( xdrs, &actSize_ );
    leafIndex_.processXdr(xdrs);
    state_.processXdr(xdrs);
    return true;
  }
}; // end of class AdaptiveLeafIndexSet 


} // end namespace Dune 

#endif


