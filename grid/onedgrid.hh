#ifndef DUNE_ONE_D_GRID_HH
#define DUNE_ONE_D_GRID_HH

#include <vector>

#include <dune/common/matvec.hh>
#include <dune/common/capabilities.hh>
#include <dune/grid/common/grid.hh>
#include <dune/common/simplevector.hh>

/** \file
 * \brief The OneDGrid class
 */

namespace Dune 
{
/** @defgroup OneDGrid OneDGrid
    \ingroup GridCommon

 */
  
/** \brief The type used by to store coordinates */
typedef double OneDCType;

// forward declarations 
    template<int codim, int dim, int dimworld> class OneDGridEntity;
    template<int codim, int dim, int dimworld, PartitionIteratorType pitype> class OneDGridLevelIterator;

    template<int dim, int dimworld>            class OneDGridElement;
// template<int dim, int dimworld>            class UGGridBoundaryEntity;
    template<int dim, int dimworld>            class OneDGridHierarchicIterator;
    template<int dim, int dimworld>            class OneDGridIntersectionIterator;
    template<int dim, int dimworld>            class OneDGrid;

    template<int codim>                        class OneDGridHelper;

// singleton holding reference elements
    //template<int dim> struct UGGridReferenceElement;


}  // namespace Dune

#include "onedgrid/onedgridentity.hh"
#include "onedgrid/onedgridelement.hh"
#include "onedgrid/onedgridboundent.hh"
#include "onedgrid/onedintersectionit.hh"
#include "onedgrid/onedgridleveliterator.hh"
#include "onedgrid/onedgridhieriterator.hh"

#include "onedgrid/onedgridentity.cc"

namespace Dune {

    // A simple double linked list
    template<class T>
    class List
    {
        
    public:

        List() : numelements(0), begin(0), rbegin(0) {}

        int size() const {return numelements;}
        
        T* insert_after (T* i, T* t) {

            // Teste Eingabe
            if (i==0 && begin!=0)
                DUNE_THROW(DoubleLinkedListError, "invalid iterator for insert_after");

            // einfuegen
            if (begin==0) {
		// einfuegen in leere Liste
		begin = t; 
                rbegin = t;
            }
            else 
                {
                    // nach Element i.p einsetzen
                    t->pred_ = i;
                    t->succ_ = i->succ_;
                    i->succ_ = t;

                    if (t->succ_!=0) 
                        t->succ_->pred_ = t;

                    // tail neu ?
                    if (rbegin==i) 
                        rbegin = t;
                }

            // Groesse und Rueckgabeiterator
            numelements = numelements+1;

            return t;
        }

        T* insert_before (T* i, T* t) {

            // Teste Eingabe
            if (i==0 && begin!=0) 
                DUNE_THROW(DoubleLinkedListError, 
                           "invalid iterator for insert_before");
            
            // einfuegen
            if (begin==0) 
                {
                    // einfuegen in leere Liste
                    begin=t; 
                    rbegin=t;
                }
            else 
                {
                    // vor Element i.p einsetzen
                    t->succ_ = i;
                    t->pred_ = i->pred_;
                    i->pred_ = t;

                    if (t->pred_!=0) 
                        t->pred_->succ_ = t;
                    // head neu ?
                    if (begin==i) 
                        begin = t;
                }
            
            // Groesse und Rueckgabeiterator
            numelements = numelements+1;
            return t;
        }

        void remove (T* i)
        {
            // Teste Eingabe
            if (i==0)
                return;
            
            // Ausfaedeln
            if (i->succ_!=0) 
                i->succ_->pred_ = i->pred_;
            if (i->pred_!=0) 
                i->pred_->succ_ = i->succ_;
            
            // head & tail
            if (begin==i) 
                begin=i->succ_;
            if (rbegin==i) 
                rbegin = i->pred_;
            
            // Groesse
            numelements = numelements-1;
        }


        int numelements;

        T* begin;
        T* rbegin;

    };

//**********************************************************************
//
// --OneDGrid
//
//**********************************************************************

template <int codim>
struct OneDGridHelper {};

template <>
struct OneDGridHelper<1>
{
  static OneDGridLevelIterator<1,1,1, All_Partition> lbegin(const OneDGrid<1,1> * g, int level);
};

template <>
struct OneDGridHelper<0>
{
  static OneDGridLevelIterator<0,1,1, All_Partition> lbegin(const OneDGrid<1,1> * g, int level);
};

/** \brief The 1D-Grid class
 * \ingroup OneDGrid
 *
 * This implementation of the grid interface provides one-dimensional
 grids only.  No matter what the values of dim and dimworld may be,
 you'll always get a 1D-grid in a 1D-world.  Unlike SGrid, however,
 which can also be instantiated in 1D, the OneDGrid is nonuniform
 and provides local mesh refinement and coarsening.
 */
template <int dim, int dimworld>
class OneDGrid : public GridDefault  < dim, dimworld,
                                     OneDCType, OneDGrid,
                                     OneDGridLevelIterator,OneDGridEntity>
{

    friend class OneDGridHelper <0>;
    friend class OneDGridHelper <1>;
    friend class OneDGridEntity <0,dim,dimworld>;
    friend class OneDGridEntity <dim,dim,dimworld>;
    friend class OneDGridHierarchicIterator<dim,dimworld>;
    friend class OneDGridIntersectionIterator<dim,dimworld>;
    
    /** \brief OneDGrid is only implemented for 1d */
    CompileTimeChecker< (dim==1 && dimworld==1) >   Use_OneDGrid_only_for_1d;   

    
    // **********************************************************
    // The Interface Methods
    // **********************************************************

public:  
      
    //typedef OneDGridReferenceElement<dim> ReferenceElement;
    
    /** \brief The leaf iterator type  (currently only a level iterator)
     * \todo Replace this by a true leaf iterator */
    typedef OneDGridLevelIterator<0,dim,dimworld, All_Partition> LeafIterator;

    /** \brief Constructor with an explicit set of coordinates */
    OneDGrid(const SimpleVector<OneDCType>& coords);

    /** \brief Constructor for a uniform grid */
    OneDGrid(int numElements, double leftBoundary, double rightBoundary);

    OneDGrid();

  //! Desctructor 
    ~OneDGrid();
   
  //! Return maximum level defined in this grid. Levels are numbered
  //! 0 ... maxlevel with 0 the coarsest level.  
    int maxlevel() const {return vertices.size()-1;}

  //! Iterator to first entity of given codim on level
  template<int codim>
  OneDGridLevelIterator<codim,dim,dimworld, All_Partition> lbegin (int level) const
    {
      return OneDGridHelper<codim>::lbegin(this, level);
    }

  //! one past the end on this level
  template<int codim>
  OneDGridLevelIterator<codim,dim,dimworld, All_Partition> lend (int level) const
    {
      if (level<0 || level>maxlevel())
        DUNE_THROW(GridError, "LevelIterator in nonexisting level " << level << " requested!");
      
      OneDGridLevelIterator<codim,dim,dimworld,All_Partition> it(0);
      return it;
    }

#if 0
    //! Iterator to first entity of given codim on level
    template<int codim, PartitionIteratorType PiType>
    OneDGridLevelIterator<codim,dim,dimworld, PiType> lbegin (int level) const;

    //! one past the end on this level
    template<int codim, PartitionIteratorType PiType>
    OneDGridLevelIterator<codim,dim,dimworld, PiType> lend (int level) const;
#endif

    /** \brief Create leaf iterator  (currently only a level iterator)
     * \todo Replace this by a true leaf iterator */
    LeafIterator leafbegin (int level) const {return lbegin<0>(level);}

    /** \brief Create one past last on leaf level  (currently only a level iterator)
     * \todo Replace this by a true leaf iterator */
    LeafIterator leafend (int level) const {return lend<0>(level);}

    /** \brief Number of grid entities per level and codim
     */
    int size (int level, int codim) const {
        if (codim<0 || codim>1)
            DUNE_THROW(GridError, "There are no codim " << codim << " entities in a OneDGrid!");

        if (codim==0)
            return elements[level].size();
        
        return vertices[level].size();
    }

    //! Triggers the grid refinement process
    bool adapt();

    /** \brief Dynamic type identification */
    GridIdentifier type () { return OneDGrid_Id; }
    
    // **********************************************************
    // End of Interface Methods
    // **********************************************************
    
       /** \brief The different forms of grid refinement supported by OneDGrid */
    enum RefinementType {
        /** \brief New level consists only of the refined elements */
        LOCAL,
        /** \brief New level consists of the refined elements and the unrefined ones, too */
        COPY};

   /** \brief Sets the type of grid refinement */
    void setRefinementType(RefinementType type) {
        refinementType_ = type;
    }

    /** \brief Does one uniform refinement step
     *
     * \param refCount I don't know what this is good for.  It doesn't
     *        actually do anything.
     */
    void globalRefine(int refCount);

    
private:

   //! The type of grid refinement currently in use
    RefinementType refinementType_;

    OneDGridEntity<1,1,1>* getLeftUpperVertex(const OneDGridEntity<0,1,1>* eIt);

    OneDGridEntity<1,1,1>* getRightUpperVertex(const OneDGridEntity<0,1,1>* eIt);

    /** \brief Returns an iterator the the first element on the left of
        the input element which has sons.
    */
    OneDGridEntity<0,1,1>* getLeftNeighborWithSon(OneDGridEntity<0,1,1>* eIt);

    // The vertices of the grid hierarchy
    std::vector<List<OneDGridEntity<1,1,1> > > vertices;

    // The elements of the grid hierarchy
    std::vector<List<OneDGridEntity<0,1,1> > > elements;

  
}; // end Class OneDGrid

#if 0  // to be implemented
namespace Capabilities
{

  template<int dim,int dimw>
  struct hasLeafIterator< UGGrid<dim,dimw> >
  {
    static const bool v = true;
  };
  
  template<int dim, int dimw, int cdim>
  struct hasEntity< UGGrid<dim,dimw>, UGGridEntity<cdim,dim,dimw> >
  {
    static const bool v = true;
  };
  
  template<int dim,int dimw>
  struct isParallel< UGGrid<dim,dimw> >
  {
    static const bool v = true;
  };
  
}
#endif

} // namespace Dune

#endif
