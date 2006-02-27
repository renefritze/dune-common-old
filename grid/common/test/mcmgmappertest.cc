// $Id$

/** \file
    \brief A unit test for the MultipleCodimMultipleGeometryMapper
*/

#include <config.h>

#include <iostream>
#include <set>

#include <dune/grid/uggrid.hh>
#include <dune/io/file/amirameshreader.hh>
#include <dune/grid/common/mcmgmapper.hh>

using namespace Dune;

// Parameter for mapper class
template<int dim>
struct ElementDataLayout
{
    bool contains (int codim, Dune::GeometryType gt)
    {
        return codim==0;
    }
};

template<int dim>
struct FaceDataLayout
{
    bool contains (int codim, Dune::GeometryType gt)
    {
        return codim==1;
    }
};


// /////////////////////////////////////////////////////////////////////////////////
//   Check whether the index created for element data is unique, consecutive 
//   and starting from zero.
// /////////////////////////////////////////////////////////////////////////////////
template <class Mapper, class IndexSet>
void checkElementDataMapper(const Mapper& mapper, const IndexSet& indexSet)
{
    typedef typename IndexSet::template Codim<0>::template Partition<All_Partition>::Iterator Iterator;

    Iterator eIt    = indexSet.template begin<0,All_Partition>();
    Iterator eEndIt = indexSet.template end<0,All_Partition>();

    int min = 1;
    int max = 0;
    std::set<int> indices;

    for (; eIt!=eEndIt; ++eIt) {
    
        int index = mapper.map(*eIt);
        
        min = std::min(min, index);
        max = std::max(max, index);

        std::pair<std::set<int>::iterator, bool> status = indices.insert(index);

        if (!status.second)   // not inserted because already existing
            DUNE_THROW(GridError, "Mapper element index is not unique!");
    }

    if (min!=0)
        DUNE_THROW(GridError, "Mapper element index is not starting from zero!");

    if (max!=indexSet.size(0)-1)
        DUNE_THROW(GridError, "Mapper element index is not consecutive!");
        
}

// /////////////////////////////////////////////////////////////////////////////////
//   Check whether the index created for face data is unique, consecutive 
//   and starting from zero.
// /////////////////////////////////////////////////////////////////////////////////
template <class Mapper, class IndexSet>
void checkFaceDataMapper(const Mapper& mapper, const IndexSet& indexSet)
{
#if 0
    typedef typename IndexSet::template Codim<0>::template Partition<All_Partition>::Iterator Iterator;

    Iterator eIt    = indexSet.template begin<0,All_Partition>();
    Iterator eEndIt = indexSet.template end<0,All_Partition>();

    // Reset the counting variables
    int min = 1;
    int max = 0;
    indices.clear();

    for (; eIt!=eEndIt; ++eIt) {

        typedef typename GridType::template Codim<0>::Entity::IntersectionIterator IntersectionIterator;

        IntersectionIterator iIt    = eIt->ibegin();
        IntersectionIterator iEndIt = eIt->iend();

        for (; iIt!=iEndIt; ++iIt) {

#if 0
            // The correctness of the pan-type index for codim 0 has been checked above.  
            // So now we can use it.
            if (iIt.neighbor() 
                && panTypeIndexSet.template index<0>(*eIt) > panTypeIndexSet.template index<0>(*iIt.outside()))
                continue;
#endif

            int index = mapper.map(*eIt, iIt.numberInSelf());

//             std::cout << hostIndexSet.template subIndex<1>(*eIt, iIt.numberInSelf())
//                 << "  Index: " << index << "   type: " << eIt->geometry().type() << std::endl;
            min = std::min(min, index);
            max = std::max(max, index);

            std::pair<std::set<int>::iterator, bool> status = indices.insert(index);

            if (!status.second)   // not inserted because already existing
                DUNE_THROW(GridError, "Mapper index is not unique!");

        }

    }
    
    if (min!=0)
        DUNE_THROW(GridError, "Mapper index for codim 1 is not starting from zero!");
    
    if (max!=indexSet.size(1))
        DUNE_THROW(GridError, "Mapper index for codim 1 is not consecutive!");
        
#endif
}

/*
  The MultipleGeometryMultipleCodimMapper only does something helpful on grids with more 
  than one element type.  So far only UGGrids do this, so we use them to test the mapper.
*/

int main () try
{

    // ////////////////////////////////////////////////////////////////////////
    //  Do the test for a 2d UGGrid
    // ////////////////////////////////////////////////////////////////////////
    {
        typedef UGGrid<2,2> GridType;
        typedef GridType::Traits::LeafIndexSet LeafIndexSetType;
        typedef GridType::Traits::LevelIndexSet LevelIndexSetType;
        
        GridType grid;
        AmiraMeshReader<GridType>::read(grid, "../../test/ug-testgrid-2.am");
        
        // create hybrid grid 
        grid.mark(1,grid.leafbegin<0>());
        grid.adapt();
        grid.globalRefine(1);
        
        LeafMultipleCodimMultipleGeomTypeMapper<GridType, ElementDataLayout> leafMCMGMapper(grid);
        checkElementDataMapper(leafMCMGMapper, grid.leafIndexSet());
        
        for (int i=2; i<=grid.maxLevel(); i++) {
            LevelMultipleCodimMultipleGeomTypeMapper<GridType, ElementDataLayout> levelMCMGMapper(grid, i);
            checkElementDataMapper(levelMCMGMapper, grid.levelIndexSet(i));
        }
    }

    // ////////////////////////////////////////////////////////////////////////
    //  Do the  test for a 3d UGGrid
    // ////////////////////////////////////////////////////////////////////////
    {
        typedef UGGrid<3,3> GridType;
        typedef GridType::Traits::LeafIndexSet LeafIndexSetType;
        typedef GridType::Traits::LevelIndexSet LevelIndexSetType;
        
        GridType grid;
        AmiraMeshReader<GridType>::read(grid, "../../test/ug-testgrid-3.am");
        
        // create hybrid grid 
        grid.mark(1,grid.leafbegin<0>());
        grid.adapt();
        grid.globalRefine(1);
        
        LeafMultipleCodimMultipleGeomTypeMapper<GridType, ElementDataLayout> leafMCMGMapper(grid);
        checkElementDataMapper(leafMCMGMapper, grid.leafIndexSet());
        
        for (int i=2; i<=grid.maxLevel(); i++) {
            LevelMultipleCodimMultipleGeomTypeMapper<GridType, ElementDataLayout> levelMCMGMapper(grid, i);
            checkElementDataMapper(levelMCMGMapper, grid.levelIndexSet(i));
        }        
    }

    return 0;

 } catch (Exception &e) {
    std::cerr << e << std::endl;
    return 1;
 } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
 }