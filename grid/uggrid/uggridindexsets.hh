#ifndef DUNE_UGGRID_INDEXSETS_HH
#define DUNE_UGGRID_INDEXSETS_HH

/** \file
    \brief The index and id sets for the UGGrid class
*/

//#include <dune/grid/common/defaultindexsets.hh>
#include <vector>


namespace Dune {

template <class GridImp> 
struct UGGridLevelIndexSetTypes
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


template<class GridImp>
class UGGridLevelIndexSet : public IndexSet<GridImp,UGGridLevelIndexSet<GridImp>,UGGridLevelIndexSetTypes<GridImp> >
{
  enum {dim = GridImp::dimension};
  typedef IndexSet<GridImp,UGGridLevelIndexSet<GridImp>,UGGridLevelIndexSetTypes<GridImp> > Base;

public:

    /** \brief Default constructor

    Unfortunately we can't force the user to init grid_ and level_, because
    UGGridLevelIndexSets are meant to be stored in an array.

    \todo I want to make this constructor private, but I can't, because
    it is called by UGGrid through a std::vector::resize()
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
  int subIndex (const typename GridImp::Traits::template Codim<0>::Entity& e, int i) const
  {
      return grid_->template getRealEntity<0>(e).template subIndex<cc>(i);
  }


    //! get number of entities of given codim, type and on this level
    int size (int codim) const {
	  if (codim==0)
		return numSimplices_+numPyramids_+numPrisms_+numCubes_;
	  if (codim==dim)
		return numVertices_;
	  if (codim==dim-1)
		return numEdges_;
	  if (codim==1)
		return numTriFaces_+numQuadFaces_;
	  DUNE_THROW(NotImplemented, "wrong codim!");
    }

  //! get number of entities of given codim, type and on this level
  int size (int codim, GeometryType type) const
  {
	if (codim==0) {
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
	}
	if (codim==dim) {
	  return numVertices_;
	} 
	if (codim==dim-1) {
	  return numEdges_;
	}
	if (codim==1) {
	  switch (type) {
	  case simplex:
		return numTriFaces_;
	  case cube:
		return numQuadFaces_;
	  default:
		return 0;
	  }
	}
	DUNE_THROW(NotImplemented, "Wrong codim!");
  }

  /** \brief Deliver all geometry types used in this grid */
  const std::vector<GeometryType>& geomTypes (int codim) const
  {
	return myTypes_[codim];
  }

  //! one past the end on this level
  template<int cd, PartitionIteratorType pitype>
  typename Base::template Codim<cd>::template Partition<pitype>::Iterator begin () const
  {
	return grid_.template lbegin<cd,pitype>(level_);
  }
  
  //! Iterator to one past the last entity of given codim on level for partition type
  template<int cd, PartitionIteratorType pitype>
  typename Base::template Codim<cd>::template Partition<pitype>::Iterator end () const
  {
	return grid_.template lend<cd,pitype>(level_);
  }

  //private:
  int renumberVertex(GeometryType gt, int i) const 
  {
    if (gt==cube) {
	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  // It also works in both directions UG->DUNE, DUNE->UG !
	  const int renumbering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
	  return renumbering[i];
    } else
	  return i;
  }

  int renumberFace(GeometryType gt, int i) const 
  {

    if ( (gt==cube||gt==hexahedron) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[6] = {4, 2, 1, 3, 0, 5};
	  return renumbering[i];
    } 
    if ( (gt==simplex||gt==tetrahedron) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[4] = {1, 2, 0, 3};
	  return renumbering[i];
    } 
    if ( (gt==cube||gt==quadrilateral) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[4] = {3, 1, 0, 2};
	  return renumbering[i];
    } 
    if ( (gt==simplex||gt==triangle) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[3] = {1, 2, 0};
	  return renumbering[i];
    } 
	return i;
  }

    void update(const GridImp& grid, int level) {

        // Commit the index set to a specific level of a specific grid
        grid_ = &grid;
        level_ = level;	

        // ///////////////////////////////////
        //   clear index for codim dim-1 and 1
        // ///////////////////////////////////

        typename GridImp::Traits::template Codim<0>::LevelIterator eIt    = grid_->template lbegin<0>(level_);
        typename GridImp::Traits::template Codim<0>::LevelIterator eEndIt = grid_->template lend<0>(level_);

        for (; eIt!=eEndIt; ++eIt) {
		  typename TargetType<0,dim>::T* target_ = grid_->template getRealEntity<0>(*eIt).target_;
		  // codim dim-1
		  for (int i=0; i<eIt->template count<dim-1>(); i++)
			{
			  GeometryType gt = eIt->geometry().type();
			  int a=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,0,dim);	
			  int b=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,1,dim);
			  int& index = UG_NS<dim>::levelIndex(UG_NS<dim>::GetEdge(UG_NS<dim>::Corner(target_,renumberVertex(gt,a)),UG_NS<dim>::Corner(target_,renumberVertex(gt,b))));
			  index = -1;
			}
		  // codim 1 (faces): todo
		  if (dim==3)
			for (int i=0; i<eIt->template count<1>(); i++)
			  {
				GeometryType gt = eIt->geometry().type();
				int& index = UG_NS<dim>::levelIndex(UG_NS<dim>::SideVector(target_,renumberFace(gt,i)));
				index = -1;
			  }

        }

        // ///////////////////////////////
        //   Init the codim<dim indices
        // ///////////////////////////////
        numSimplices_ = 0;
        numPyramids_  = 0;
        numPrisms_    = 0;
        numCubes_     = 0;
		numEdges_     = 0;
		numTriFaces_  = 0;
		numQuadFaces_ = 0;

        eIt    = grid_->template lbegin<0>(level_);
        eEndIt = grid_->template lend<0>(level_);
        
        for (; eIt!=eEndIt; ++eIt) {
 
		    // codim 0 (elements)
            switch (eIt->geometry().type()) {
            case simplex:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numSimplices_++;
                break;
            case pyramid:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numPyramids_++;
                break;
            case prism:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numPrisms_++;
                break;
            case cube:
                UG_NS<dim>::levelIndex(grid_->template getRealEntity<0>(*eIt).target_) = numCubes_++;
                break;
            default:
                DUNE_THROW(GridError, "Found the GeometryType " << eIt->geometry().type()
                           << ", which should never occur in a UGGrid!");
            }

			typename TargetType<0,dim>::T* target_ = grid_->template getRealEntity<0>(*eIt).target_;

			// codim dim-1 (edges)
			for (int i=0; i<eIt->template count<dim-1>(); i++)
			  {
				GeometryType gt = eIt->geometry().type();
				int a=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,0,dim);	
				int b=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,1,dim);
				int& index = UG_NS<dim>::levelIndex(UG_NS<dim>::GetEdge(UG_NS<dim>::Corner(target_,renumberVertex(gt,a)),UG_NS<dim>::Corner(target_,renumberVertex(gt,b))));
				if (index<0) index = numEdges_++;
			  }

			// codim 1 (faces): todo
			if (dim==3)
			  for (int i=0; i<eIt->template count<1>(); i++)
				{
				  GeometryType gt = eIt->geometry().type();
				  int& index = UG_NS<dim>::levelIndex(UG_NS<dim>::SideVector(target_,renumberFace(gt,i)));
				  if (index<0) // not visited yet 
					switch (ReferenceElements<double,dim>::general(gt).type(i,1))
					  {
					  case simplex:
						index = numTriFaces_++;
						break;
					  case cube:
						index = numQuadFaces_++;
						break;
					  default:
						DUNE_THROW(GridError, "wrong geometry type in face");
					  }
				}
        }

        // Update the list of geometry types present
        myTypes_[0].resize(0);
        if (numSimplices_ > 0)
            myTypes_[0].push_back(simplex);
        if (numPyramids_ > 0)
            myTypes_[0].push_back(pyramid);
        if (numPrisms_ > 0)
            myTypes_[0].push_back(prism);
        if (numCubes_ > 0)
            myTypes_[0].push_back(cube);

		myTypes_[dim-1].resize(0);
		myTypes_[dim-1].push_back(cube);

		if (dim==3)
		  {
			myTypes_[1].resize(0);
			if (numTriFaces_ > 0)
			  myTypes_[1].push_back(simplex);		
			if (numQuadFaces_ > 0)
			  myTypes_[1].push_back(cube);		
		  }

		// //////////////////////////////
        //   Init the vertex indices
        // //////////////////////////////

        typename GridImp::Traits::template Codim<dim>::LevelIterator vIt    = grid_->template lbegin<dim>(level_);
        typename GridImp::Traits::template Codim<dim>::LevelIterator vEndIt = grid_->template lend<dim>(level_);
        
        numVertices_ = 0;
        for (; vIt!=vEndIt; ++vIt)
            UG_NS<dim>::levelIndex(grid_->template getRealEntity<dim>(*vIt).target_) = numVertices_++;

		myTypes_[dim].resize(0);
		myTypes_[dim].push_back(cube);
    }

  const GridImp* grid_;
  int level_;

  int numSimplices_;
  int numPyramids_;
  int numPrisms_;
  int numCubes_;
  int numVertices_;
  int numEdges_;
  int numTriFaces_;
  int numQuadFaces_;

  std::vector<GeometryType> myTypes_[dim+1];
};

template <class GridImp> 
struct UGGridLeafIndexSetTypes
{
  //! The types
  template<int cd>
  struct Codim
  {
	template<PartitionIteratorType pitype>
	struct Partition
	{
	  typedef typename GridImp::Traits::template Codim<cd>::template Partition<pitype>::LeafIterator Iterator;
	};
  };
};


template<class GridImp>
class UGGridLeafIndexSet: public IndexSet<GridImp,UGGridLeafIndexSet<GridImp>,UGGridLeafIndexSetTypes<GridImp> >
{
  typedef IndexSet<GridImp,UGGridLeafIndexSet<GridImp>,UGGridLeafIndexSetTypes<GridImp> > Base;
public:
    //friend class UGGrid<dim,dim>;

    enum {dim = GridImp::dimension};

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
  int subIndex (const typename GridImp::Traits::template Codim<0>::Entity& e, int i) const
  {
      return grid_.template getRealEntity<0>(e).template subLeafIndex<cc>(i);
  }

  //! get number of entities of given codim and type 
  int size (int codim, GeometryType type) const
  {
	if (codim==0) {
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
	}
	if (codim==dim) {
	  return numVertices_;
	} 
	if (codim==dim-1) {
	  return numEdges_;
	}
	if (codim==1) {
	  switch (type) {
	  case simplex:
		return numTriFaces_;
	  case cube:
		return numQuadFaces_;
	  default:
		return 0;
	  }
	}
	DUNE_THROW(NotImplemented, "Wrong codim!");
  }

    /** deliver all geometry types used in this grid */
  const std::vector<GeometryType>& geomTypes (int codim) const
  {
	return myTypes_[codim];
  }

  //! one past the end on this level
  template<int cd, PartitionIteratorType pitype>
  typename Base::template Codim<cd>::template Partition<pitype>::Iterator begin () const
  {
	return grid_.template leafbegin<cd,pitype>();
  }
  
  //! Iterator to one past the last entity of given codim on level for partition type
  template<int cd, PartitionIteratorType pitype>
  typename Base::template Codim<cd>::template Partition<pitype>::Iterator end () const
  {
	return grid_.template leafend<cd,pitype>();
  }

    //private:
  int renumberVertex(GeometryType gt, int i) const 
  {
    if (gt==cube) {
	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  // It also works in both directions UG->DUNE, DUNE->UG !
	  const int renumbering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
	  return renumbering[i];
    } else
	  return i;
  }
    
  int renumberFace(GeometryType gt, int i) const 
  {

    if ( (gt==cube||gt==hexahedron) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[6] = {4, 2, 1, 3, 0, 5};
	  return renumbering[i];
    } 
    if ( (gt==simplex||gt==tetrahedron) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[4] = {1, 2, 0, 3};
	  return renumbering[i];
    } 
    if ( (gt==cube||gt==quadrilateral) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[4] = {3, 1, 0, 2};
	  return renumbering[i];
    } 
    if ( (gt==simplex||gt==triangle) && dim==3) {

	  // Dune numbers the vertices of a hexahedron and quadrilaterals differently than UG.
	  // The following two lines do the transformation
	  // The renumbering scheme is {0,1,3,2} for quadrilaterals, therefore, the
	  // following code works for 2d and 3d.
	  const int renumbering[3] = {1, 2, 0};
	  return renumbering[i];
    } 
	return i;
  }

    void update() {

        // //////////////////////////////////////////////////////
        // Handle codim 1 and dim-1: levelwise from top to bottom
        // //////////////////////////////////////////////////////

		// first loop : clear indices
		for (int level_=grid_.maxlevel(); level_>=0; level_--)
		  {
			typename GridImp::Traits::template Codim<0>::LevelIterator eIt    = grid_.template lbegin<0>(level_);
			typename GridImp::Traits::template Codim<0>::LevelIterator eEndIt = grid_.template lend<0>(level_);

			for (; eIt!=eEndIt; ++eIt) 
			  {
				// get pointer to UG object
				typename TargetType<0,dim>::T* target_ = grid_.template getRealEntity<0>(*eIt).target_;

				// codim dim-1
				for (int i=0; i<eIt->template count<dim-1>(); i++)
				  {
					GeometryType gt = eIt->geometry().type();
					int a=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,0,dim);	
					int b=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,1,dim);
					int& index = UG_NS<dim>::leafIndex(UG_NS<dim>::GetEdge(UG_NS<dim>::Corner(target_,renumberVertex(gt,a)),UG_NS<dim>::Corner(target_,renumberVertex(gt,b))));
					index = -1;
				  }

				// codim 1 (faces): todo
				if (dim==3)
				  for (int i=0; i<eIt->template count<1>(); i++)
					{
					  GeometryType gt = eIt->geometry().type();
					  int& index = UG_NS<dim>::leafIndex(UG_NS<dim>::SideVector(target_,renumberFace(gt,i)));
					  index = -1;
					}
			  }
		  }

		// init counters
		numEdges_     = 0;
		numTriFaces_  = 0;
		numQuadFaces_ = 0;

		// second loop : set indices
		for (int level_=grid_.maxlevel(); level_>=0; level_--)
		  {
			typename GridImp::Traits::template Codim<0>::LevelIterator eIt    = grid_.template lbegin<0>(level_);
			typename GridImp::Traits::template Codim<0>::LevelIterator eEndIt = grid_.template lend<0>(level_);

			for (; eIt!=eEndIt; ++eIt) 
			  {
				// we need only look at leaf elements
				if (!eIt->isLeaf()) continue;

				// get pointer to UG object
				typename TargetType<0,dim>::T* target_ = grid_.template getRealEntity<0>(*eIt).target_;

				// codim dim-1 (edges)
				for (int i=0; i<eIt->template count<dim-1>(); i++)
				  {
					GeometryType gt = eIt->geometry().type();
					int a=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,0,dim);	
					int b=ReferenceElements<double,dim>::general(gt).subentity(i,dim-1,1,dim);
					int& index = UG_NS<dim>::leafIndex(UG_NS<dim>::GetEdge(UG_NS<dim>::Corner(target_,renumberVertex(gt,a)),UG_NS<dim>::Corner(target_,renumberVertex(gt,b))));
					if (index<0)
					  { 
						// get new index and assign
						index = numEdges_++;
						// write index through to coarser grids
						typename TargetType<0,dim>::T* father_ = UG_NS<dim>::EFather(target_);
						while (father_!=0)
						  {
							if (!UG_NS<dim>::hasCopy(father_)) break; // handle only copies
							UG_NS<dim>::leafIndex(UG_NS<dim>::GetEdge(UG_NS<dim>::Corner(father_,renumberVertex(gt,a)),UG_NS<dim>::Corner(father_,renumberVertex(gt,b)))) = index;
							father_ = UG_NS<dim>::EFather(father_);
						  }
					  }
				  }

				// codim 1 (faces): todo
				if (dim==3)
				  for (int i=0; i<eIt->template count<1>(); i++)
					{
					  GeometryType gt = eIt->geometry().type();
					  int& index = UG_NS<dim>::leafIndex(UG_NS<dim>::SideVector(target_,renumberFace(gt,i)));
					  if (index<0) // not visited yet 
						{
						  // get new index and assign
						  switch (ReferenceElements<double,dim>::general(gt).type(i,1))
							{
							case simplex:
							  index = numTriFaces_++;
							  break;
							case cube:
							  index = numQuadFaces_++;
							  break;
							default:
							  DUNE_THROW(GridError, "wrong geometry type in face");
							}
						  // write index through to coarser grid
						  typename TargetType<0,dim>::T* father_ = UG_NS<dim>::EFather(target_);
						  while (father_!=0)
							{
							  if (!UG_NS<dim>::hasCopy(father_)) break; // handle only copies
							  UG_NS<dim>::leafIndex(UG_NS<dim>::SideVector(father_,renumberFace(gt,i))) = index;
							  father_ = UG_NS<dim>::EFather(father_);
							}
						}
					}
			  }
		  }

        // Update the list of geometry types present
		myTypes_[dim-1].resize(0);
		myTypes_[dim-1].push_back(cube);

		if (dim==3)
		  {
			myTypes_[1].resize(0);
			if (numTriFaces_ > 0)
			  myTypes_[1].push_back(simplex);		
			if (numQuadFaces_ > 0)
			  myTypes_[1].push_back(cube);		
		  }

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
        myTypes_[0].resize(0);
        if (numSimplices_ > 0)
            myTypes_[0].push_back(simplex);
        if (numPyramids_ > 0)
            myTypes_[0].push_back(pyramid);
        if (numPrisms_ > 0)
            myTypes_[0].push_back(prism);
        if (numCubes_ > 0)
            myTypes_[0].push_back(cube);


        // //////////////////////////////
        //   Init the vertex indices
        // //////////////////////////////
        typename GridImp::Traits::template Codim<dim>::LeafIterator vIt    = grid_.template leafbegin<dim>();
        typename GridImp::Traits::template Codim<dim>::LeafIterator vEndIt = grid_.template leafend<dim>();
        
		// leaf index in node writes through to vertex !
        numVertices_ = 0;
        for (; vIt!=vEndIt; ++vIt)
            UG_NS<dim>::leafIndex(grid_.template getRealEntity<dim>(*vIt).target_) = numVertices_++;

		myTypes_[dim].resize(0);
		myTypes_[dim].push_back(cube);
    }
        
    const GridImp& grid_;

  int numSimplices_;
  int numPyramids_;
  int numPrisms_;
  int numCubes_;
  int numVertices_;
  int numEdges_;
  int numTriFaces_;
  int numQuadFaces_;

  std::vector<GeometryType> myTypes_[dim+1];
};


//template<int dim>
template <class GridImp>
class UGGridGlobalIdSet : public IdSet<GridImp,UGGridGlobalIdSet<GridImp>,unsigned int>
{

public:
  //! constructor stores reference to a grid
  UGGridGlobalIdSet (const GridImp& g) : grid_(g) {}

  //! define the type used for persistent indices
  typedef unsigned int GlobalIdType;

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

    //private:

    /** \todo Should be private */
    void update() {}

  const GridImp& grid_;
};


template<class GridImp>
class UGGridLocalIdSet : public IdSet<GridImp,UGGridLocalIdSet<GridImp>,unsigned int>
{
public:
    //friend class UGGrid<dim,dim>;

    //! constructor stores reference to a grid
    UGGridLocalIdSet (const GridImp& g) : grid_(g) {}

public:
  //! define the type used for persistent local ids
  typedef unsigned int LocalIdType;

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

    //private:

    /** \todo Should be private */
    void update() {}

  const GridImp& grid_;
};


}  // namespace Dune


#endif
