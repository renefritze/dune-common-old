#include <amiramesh/AmiraMesh.h>

#include <algorithm>

#include <dune/istl/bvector.hh>

// ///////////////////////////////////////////////////////////
//   Write the leaf level of the grid as an AmiraMesh file
// ///////////////////////////////////////////////////////////
template<class GridType>
void Dune::AmiraMeshWriter<GridType>::writeGrid(const GridType& grid, 
                                                const std::string& filename) 
{
    const int maxlevel = grid.maxlevel();

#ifndef UGGRID_WITH_INDEX_SETS
    writeGrid(grid, filename, maxlevel);
#else
    // Find out whether the grid contains only triangles.  If yes, then
    // it is written as a HxTriangularGrid.  If not, it is written
    // but it cannot (currently) be read by Amira
    bool containsOnlySimplices = true;

    LeafIterator element = grid.leafbegin(maxlevel);
    LeafIterator end     = grid.leafend(maxlevel);

    for (; element!=end; ++element) {
        if (element->geometry().type() != triangle) {
            containsOnlySimplices = false;
            break;
        }
    }

    int maxVerticesPerElement = (containsOnlySimplices) ? 3 : 4;

    // /////////////////////////////////////////////////////
    //   Extract the leaf vertices
    // /////////////////////////////////////////////////////

    LeafIterator lIt    = grid.leafbegin(maxlevel);
    LeafIterator lEndIt = grid.leafend(maxlevel);

    int noOfElem = 0;
    Array<int> leafVertices(grid.hierarchicIndexSet().size(0,dim));
    Array<FieldVector<double,dim> > leafVertexCoords(grid.hierarchicIndexSet().size(0,dim));

    leafVertices.set(-1);

    for (; lIt!=lEndIt; ++lIt) {

        for (int i=0; i<lIt->template count<dim>(); i++) {
            leafVertices[grid.hierarchicIndexSet().template subIndex<dim>(*lIt,i)] = 1;
            leafVertexCoords[grid.hierarchicIndexSet().template subIndex<dim>(*lIt,i)] = lIt->geometry()[i];
        }

        noOfElem++;
    }

    int noOfNodes = 0;
    for (int i=0; i<leafVertices.size(); i++)
        if (leafVertices[i]==1)
            leafVertices[i] = noOfNodes++;

   // create amiramesh object
   AmiraMesh am;

   // Set the appropriate content type
   am.parameters.set("ContentType", "HxTriangularGrid");

   // //////////////////////////////////
   //   Write grid vertex coordinates
   // //////////////////////////////////
   AmiraMesh::Location* geoNodes = new AmiraMesh::Location("Nodes", noOfNodes);
   am.insert(geoNodes);

   AmiraMesh::Data* geo_node_data = new AmiraMesh::Data("Coordinates", geoNodes, 
                                                        McPrimType::mc_float, dim);
   am.insert(geo_node_data);

   int vertexIdx = 0;
   for (int i=0; i<leafVertices.size(); i++)
       if (leafVertices[i] != -1) {

           for (int j=0; j<dim; j++)
               ((float*)geo_node_data->dataPtr())[dim*vertexIdx+j] = leafVertexCoords[i][j];

           vertexIdx++;

       }

   // ///////////////////////////////////////////
   //    Write element section
   // ///////////////////////////////////////////
   AmiraMesh::Location* elementLoc = (containsOnlySimplices) 
       ? new AmiraMesh::Location("Triangles", noOfElem)
       : new AmiraMesh::Location("Quadrangles", noOfElem);
   
   am.insert(elementLoc);

   AmiraMesh::Data* elementData = new AmiraMesh::Data("Nodes", elementLoc, 
                                                       McPrimType::mc_int32, maxVerticesPerElement);
   am.insert(elementData);  

   int *dPtr = (int*)elementData->dataPtr();

   LeafIterator lIt2    = grid.leafbegin(maxlevel);
   //LeafIterator lEndIt = grid.leafend(maxlevel);

   for (int i=0; lIt2!=lEndIt; ++lIt2, i++) {

       switch (lIt2->geometry().type()) {

       default:
          
           for (int j=0; j<lIt2->template count<dim>(); j++) 
               dPtr[i*maxVerticesPerElement+j] = leafVertices[grid.hierarchicIndexSet().template subIndex<dim>(*lIt2,j)]+1;
           
           // If the element has less than four vertices use the last value
           // to fill up the remaining slot
           for (int j=lIt2->template count<dim>(); j<maxVerticesPerElement; j++)
               dPtr[i*maxVerticesPerElement+j] = dPtr[i*maxVerticesPerElement+lIt2->template count<dim>()-1];
       }

   }

   // write material section to AmiraMesh object
   AmiraMesh::Data* elementMaterials = new AmiraMesh::Data("Materials", elementLoc, McPrimType::mc_uint8, 1);
   am.insert(elementMaterials);

   for(int i=0; i<noOfElem; i++)
       ((unsigned char*)elementMaterials->dataPtr())[i] = 0;

   // Actually write the file
   if(!am.write(filename.c_str(), 1))
       DUNE_THROW(IOError, "Writing geometry file failed");
   

   std::cout << "Grid written successfully to: " << filename << std::endl;
#endif
}


/*************************************************************************/
/*     Write one level of a 3d-grid into an AmiraMesh file               */
/*************************************************************************/

template<class GridType>
void Dune::AmiraMeshWriter<GridType>::writeGrid(const GridType& grid, 
                                                const std::string& filename, int level) 
{
    if ((dim!=2 && dim!=3) || int(dim) != int(GridType::dimensionworld))
        DUNE_THROW(IOError, "You can only write grids as AmiraMesh if dim==dimworld==2"
                   << " or dim==dimworld==3.");

    // Find out whether the grid contains only tetrahedra.  If yes, then
    // it is written in TetraGrid format.  If not, it is written in
    // hexagrid format.
    bool containsOnlySimplices = true;

    ElementIterator eIt    = grid.template lbegin<0>(level);
    ElementIterator eEndIt = grid.template lend<0>(level);

    for (; eIt!=eEndIt; ++eIt) {
        if (eIt->geometry().type() != tetrahedron &&
            eIt->geometry().type() != triangle &&
            eIt->geometry().type() != simplex) {
            containsOnlySimplices = false;
            break;
        }
    }

    int maxVerticesPerElement = (dim==3) 
        ? ((containsOnlySimplices) ? 4 : 8)
        : ((containsOnlySimplices) ? 3 : 4);

    int noOfNodes = grid.size(level, dim);
    int noOfElem  = grid.size(level, 0);
    
    int i;

   // create amiramesh object
   AmiraMesh am;

   // Set the appropriate content type
   if (dim==2)
       am.parameters.set("ContentType", "HxTriangularGrid");

   // write grid vertex coordinates
   AmiraMesh::Location* geo_nodes = new AmiraMesh::Location("Nodes", noOfNodes);
   am.insert(geo_nodes);

   AmiraMesh::Data* geo_node_data = new AmiraMesh::Data("Coordinates", geo_nodes, 
                                                        McPrimType::mc_float, dim);
   am.insert(geo_node_data);

   typedef typename GridType::template codim<dim>::LevelIterator VertexIterator;
   VertexIterator vertex    = grid.template lbegin<dim>(level);
   VertexIterator endvertex = grid.template lend<dim>(level);
   
   for (; vertex!=endvertex; ++vertex) {

#ifndef UGGRID_WITH_INDEX_SETS
       int index = vertex->index();
#else
       int index = grid.levelIndexSet().index(*vertex);
#endif
       const FieldVector<double, dim>& coords = vertex->geometry()[0];

       // Copy coordinates
       for (int i=0; i<dim; i++)
           ((float*)geo_node_data->dataPtr())[dim*index+i] = coords[i];
     
    }
   
   /* write element section to file */
   AmiraMesh::Location* element_loc = NULL;
   
   if (dim==3) {

       if (containsOnlySimplices)
           element_loc = new AmiraMesh::Location("Tetrahedra", noOfElem);
       else
           element_loc = new AmiraMesh::Location("Hexahedra", noOfElem);

   } else {

       if (containsOnlySimplices)
       element_loc = new AmiraMesh::Location("Triangles", noOfElem);
   else
       element_loc = new AmiraMesh::Location("Quadrangles", noOfElem);

   }

   am.insert(element_loc);

   AmiraMesh::Data* element_data = new AmiraMesh::Data("Nodes", element_loc, 
                                                       McPrimType::mc_int32, maxVerticesPerElement);
   am.insert(element_data);  

   int *dPtr = (int*)element_data->dataPtr();

   eIt    = grid.template lbegin<0>(level);

   if (dim==3) {

       // //////////////////////////////////////////////////
       //   Write elements of a 3D-grid
       // //////////////////////////////////////////////////

       if (containsOnlySimplices) {
           
           for (i=0; eIt!=eEndIt; ++eIt, i++) {
               
               for (int j=0; j<4; j++) 
#ifndef UGGRID_WITH_INDEX_SETS
                   dPtr[i*4+j] = eIt->template subIndex<3>(j)+1;
#else           
               dPtr[i*4+j] = grid.levelIndexSet().template subIndex<3>(*eIt,j)+1;
#endif
           }
           
       } else {
           
           for (i=0; eIt!=eEndIt; ++eIt, i++) {
               switch (eIt->geometry().type()) {
                   
               case hexahedron: {
                   
                   const int hexaReordering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
                   for (int j=0; j<8; j++)
#ifndef UGGRID_WITH_INDEX_SETS
                       dPtr[8*i + j] = eIt->template subIndex<3>(hexaReordering[j])+1;
#else
                   dPtr[8*i + j] = grid.levelIndexSet().template subIndex<3>(*eIt, hexaReordering[j])+1;
#endif
                   break;
               }
                   
               case prism: {
                   const int prismReordering[8] = {0, 1, 1, 2, 3, 4, 4, 5};
                   for (int j=0; j<8; j++)
#ifndef UGGRID_WITH_INDEX_SETS
                       dPtr[8*i + j] = eIt->template subIndex<3>(prismReordering[j])+1;
#else
                   dPtr[8*i + j] = grid.levelIndexSet().template subIndex<3>(*eIt, prismReordering[j])+1;
#endif
                   
                   break;
               }
                   
               case pyramid: {
                   const int pyramidReordering[8] = {0, 1, 2, 3, 4, 4, 4, 4};
                   for (int j=0; j<8; j++)
#ifndef UGGRID_WITH_INDEX_SETS
                       dPtr[8*i + j] = eIt->template subIndex<3>(pyramidReordering[j])+1;
#else
                   dPtr[8*i + j] = grid.levelIndexSet().template subIndex<3>(*eIt, pyramidReordering[j])+1;
#endif
                   
                   break;
               }
                   
               case tetrahedron: {
                   
                   const int tetraReordering[8] = {0, 1, 2, 2, 3, 3, 3, 3};
                   for (int j=0; j<8; j++)
#ifndef UGGRID_WITH_INDEX_SETS
                       dPtr[8*i + j] = eIt->template subIndex<3>(tetraReordering[j])+1;
#else
                   dPtr[8*i + j] = grid.levelIndexSet().template subIndex<3>(*eIt, tetraReordering[j])+1;
#endif
                   
                   break;
               }
                   
               default:
                   DUNE_THROW(NotImplemented, "Unknown element type encountered");
                   
               }
               
           }

       }

   } else {

       typename GridType::template codim<0>::LevelIterator element2   = grid.template lbegin<0>(level);
       typename GridType::template codim<0>::LevelIterator endelement = grid.template lend<0>(level);
       
       for (i=0; element2!=endelement; ++element2, i++) {
           switch (element2->geometry().type()) {
               
           default:
               
               for (int j=0; j<element2->geometry().corners(); j++) 
#ifndef UGGRID_WITH_INDEX_SETS
                   dPtr[i*maxVerticesPerElement+j] = element2->template subIndex<dim>(j)+1;
#else
               dPtr[i*maxVerticesPerElement+j] = grid.levelIndexSet().template subIndex<dim>(*element2, j)+1;
#endif
               
               // If the element has less than 8 vertices use the last value
           // to fill up the remaining slots
               for (int j=element2->geometry().corners(); j<maxVerticesPerElement; j++)
                   dPtr[i*maxVerticesPerElement+j] = dPtr[i*maxVerticesPerElement+element2->geometry().corners()-1];
           }
           
       }

   }

   // write material section to grid file
   AmiraMesh::Data* element_materials = new AmiraMesh::Data("Materials", element_loc, McPrimType::mc_uint8, 1);
   am.insert(element_materials);

   for(i=0; i<noOfElem; i++)
       ((unsigned char*)element_materials->dataPtr())[i] = 0;

   // Actually write the file
   if(!am.write(filename.c_str(), 1))
       DUNE_THROW(IOError, "Writing geometry file failed!");

   std::cout << "Grid written successfully to: " << filename << std::endl;
}




template<class GridType>
template<class DiscFuncType>
void Dune::AmiraMeshWriter<GridType>::writeFunction(const DiscFuncType& f,
                                                    const std::string& filename)
{
    const int noOfNodes = f.getFunctionSpace().size();

    // temporary hack
    const int ncomp = 1;

    // Create AmiraMesh object
    AmiraMesh am;

    // Set the appropriate content type for 2D grid data
    if (GridType::dimension==2)
        am.parameters.set("ContentType", "HxTriangularData");

    AmiraMesh::Location* sol_nodes = new AmiraMesh::Location("Nodes", noOfNodes);
    am.insert(sol_nodes);
    
    AmiraMesh::Data* nodeData = new AmiraMesh::Data("Data", sol_nodes, McPrimType::mc_double, ncomp);
    am.insert(nodeData);

    AmiraMesh::Field* nodeField = new AmiraMesh::Field(f.name().c_str(), ncomp, McPrimType::mc_double,
                                                       AmiraMesh::t_linear, nodeData);
    am.insert(nodeField);


    // write the data into the AmiraMesh object
    typedef typename DiscFuncType::ConstDofIteratorType DofIterator;
    DofIterator dit = f.dbegin();
    DofIterator ditend = f.dend();

    int i=0;
    for (; dit!=ditend; ++dit, i++) {

        ((double*)nodeData->dataPtr())[i] = *dit;

    }

    // actually save the solution file
    // (the 1 means: ascii)
    if (!am.write(filename.c_str(), 1) )
        DUNE_THROW(IOError, "An error has occured writing file " << filename);

   std::cout << "Solution written successfully to: " << filename << std::endl;
}



template<class GridType>
template<class VectorType>
void Dune::AmiraMeshWriter<GridType>::writeBlockVector(const GridType& grid,
                                                       const VectorType& f,
                                                       const std::string& filename)
{
    int level = grid.maxlevel();

    // Find out whether the grid contains only tetrahedra.  If yes, then
    // it is written in TetraGrid format.  If not, it is written in
    // hexagrid format.
    bool containsOnlyTetrahedra = true;

    ElementIterator element = grid.template lbegin<0>(level);
    ElementIterator end     = grid.template lend<0>(level);

    for (; element!=end; ++element) {
        if (element->geometry().type() != tetrahedron) {
            containsOnlyTetrahedra = false;
            break;
        }
    }



    // Get number of components
    const int ncomp = VectorType::block_type::size;

    // Create AmiraMesh object
    AmiraMesh am;

    // Set the appropriate content type for 2D grid data
    if (GridType::dimension==2)
        am.parameters.set("ContentType", "HxTriangularData");

    if (!containsOnlyTetrahedra && GridType::dimension==3) {

        int numElements = grid.size(level, 0);
        AmiraMesh::Location* hexa_loc = new AmiraMesh::Location("Hexahedra", numElements);
        am.insert(hexa_loc);

    }

    AmiraMesh::Location* sol_nodes = new AmiraMesh::Location("Nodes", f.size());
    am.insert(sol_nodes);
    
    AmiraMesh::Data* nodeData = new AmiraMesh::Data("Data", sol_nodes, McPrimType::mc_double, ncomp);
    am.insert(nodeData);

    AmiraMesh::Field* nodeField;

    if (containsOnlyTetrahedra || GridType::dimension==2) {
        nodeField = new AmiraMesh::Field("sol", ncomp, McPrimType::mc_double,
                                         AmiraMesh::t_linear, nodeData);
    } else {

        nodeField = new AmiraMesh::Field("sol", ncomp, McPrimType::mc_double,
                                         AmiraMesh::t_trilinear, nodeData);
    }

    am.insert(nodeField);


    // write the data into the AmiraMesh object
    typedef typename VectorType::ConstIterator Iterator;
    Iterator dit    = f.begin();
    Iterator ditend = f.end();

    int i=0;
    for (; dit!=ditend; ++dit) {

        for (int j=0; j<ncomp; j++)
            ((double*)nodeData->dataPtr())[i++] = (*dit)[j];

    }

    // actually save the solution file
    // (the 1 means: ascii)
    if (!am.write(filename.c_str(), 1) )
        DUNE_THROW(IOError, "An error has occured writing file " << filename);

   std::cout << "Solution written successfully to: " << filename << std::endl;
}



