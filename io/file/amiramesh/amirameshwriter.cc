#include <amiramesh/AmiraMesh.h>


#if _3
/** \todo Make sure that the grid is three-dimensional */
template<class GridType, class DiscFuncType>
void Dune::AmiraMeshWriter<GridType, DiscFuncType>::writeGrid(const GridType& grid, 
                                                              const std::string& filename) 
{
    // Temporary:  we write this level
    int level = grid.maxlevel();

    // Find out whether the grid contains only tetrahedra.  If yes, then
    // it is written in TetraGrid format.  If not, it is written in
    // hexagrid format.
    bool containsOnlyTetrahedra = true;

    typename GridType::template Traits<0>::LevelIterator element = grid.template lbegin<0>(level);
    typename GridType::template Traits<0>::LevelIterator end     = grid.template lend<0>(level);

    for (; element!=end; ++element) {
        if (element->geometry().type() != tetrahedron) {
            containsOnlyTetrahedra = false;
            break;
        }
    }

    printf("This is the AmiraMesh writer!\n");
    int maxVerticesPerElement = (containsOnlyTetrahedra) ? 4 : 8;

    const int DIM = 3;

    int noOfNodes = grid.size(level, 3);
    int noOfElem  = grid.size(level, 0);
    
    int i;

   // create amiramesh object
   AmiraMesh am;

   // write grid vertex coordinates
   AmiraMesh::Location* geo_nodes = new AmiraMesh::Location("Nodes", noOfNodes);
   am.insert(geo_nodes);

   AmiraMesh::Data* geo_node_data = new AmiraMesh::Data("Coordinates", geo_nodes, 
                                                        McPrimType::mc_float, DIM);
   am.insert(geo_node_data);

   typedef typename GridType::template Traits<3>::LevelIterator VertexIterator;
   VertexIterator vertex    = grid.template lbegin<DIM>(level);
   VertexIterator endvertex = grid.template lend<DIM>(level);
   
   for (; vertex!=endvertex; ++vertex) 
   {
       int index = vertex->index();
       Vec<3, double> coords = vertex->geometry()[0];
       
       ((float*)geo_node_data->dataPtr())[3*index+0] = coords(0);
       ((float*)geo_node_data->dataPtr())[3*index+1] = coords(1);
       ((float*)geo_node_data->dataPtr())[3*index+2] = coords(2);
     
    }
       
#if 0
   // handle materials
   HxParamBundle* materials = new HxParamBundle("Materials");
   
   for (k=0; k<=maxSubDom; k++) {

       char buffer[100];
       sprintf(buffer, "Material%d", k);
       HxParamBundle* newMaterial = new HxParamBundle(buffer);

       HxParameter* newId = new HxParameter("Id", k);
       newMaterial->insert(newId);

       materials->insert(newMaterial);

   }   

   am_geometry.parameters.insert(materials);

   ncomp = 0;
   for(i=0; i<NVECTYPES; i++)
     ncomp = MAX(ncomp,VD_NCMPS_IN_TYPE(sol, i));
#endif

   /* write element section to file */
   AmiraMesh::Location* element_loc = NULL;
   
   if (containsOnlyTetrahedra)
       element_loc = new AmiraMesh::Location("Tetrahedra", noOfElem);
   else
       element_loc = new AmiraMesh::Location("Hexahedra", noOfElem);

   am.insert(element_loc);

   AmiraMesh::Data* element_data = new AmiraMesh::Data("Nodes", element_loc, 
                                                       McPrimType::mc_int32, maxVerticesPerElement);
   am.insert(element_data);  

   int *dPtr = (int*)element_data->dataPtr();

   typedef typename GridType::template Traits<0>::LevelIterator ElementIterator;
   ElementIterator element2    = grid.template lbegin<0>(level);
   ElementIterator endelement = grid.template lend<0>(level);

   for (i=0; element2!=endelement; ++element2, i++) {
       switch (element2->geometry().type()) {

       case hexahedron:

           dPtr[i*maxVerticesPerElement+0] = element2->template subIndex<3>(0)+1;
           dPtr[i*maxVerticesPerElement+1] = element2->template subIndex<3>(1)+1;
           dPtr[i*maxVerticesPerElement+2] = element2->template subIndex<3>(3)+1;
           dPtr[i*maxVerticesPerElement+3] = element2->template subIndex<3>(2)+1;
           dPtr[i*maxVerticesPerElement+4] = element2->template subIndex<3>(4)+1;
           dPtr[i*maxVerticesPerElement+5] = element2->template subIndex<3>(5)+1;
           dPtr[i*maxVerticesPerElement+6] = element2->template subIndex<3>(7)+1;
           dPtr[i*maxVerticesPerElement+7] = element2->template subIndex<3>(6)+1;
           break;
       default:
          
           for (int j=0; j<element2->geometry().corners(); j++) 
               dPtr[i*maxVerticesPerElement+j] = element2->template subIndex<3>(j)+1;
           
           // If the element has less than 8 vertices use the last value
           // to fill up the remaining slots
           for (int j=element2->geometry().corners(); j<maxVerticesPerElement; j++)
               dPtr[i*maxVerticesPerElement+j] = dPtr[i*maxVerticesPerElement+element2->geometry().corners()-1];
       }

   }

   // write material section to grid file
   AmiraMesh::Data* element_materials = new AmiraMesh::Data("Materials", element_loc, McPrimType::mc_uint8, 1);
   am.insert(element_materials);

//    for(i=0; i<noOfElem; i++)
//        ((unsigned char*)element_materials->dataPtr())[i] = SUBDOMAIN(elemList[i]);

   for(i=0; i<noOfElem; i++)
       ((unsigned char*)element_materials->dataPtr())[i] = 0;

   if(!am.write(filename.c_str(), 1))
       DUNE_THROW(IOError, "Writing geometry file failed!");

   std::cout << "Grid written successfully to: " << filename << std::endl;
}


#endif  // #if _3


#ifdef _2
template<class GridType, class DiscFuncType>
void Dune::AmiraMeshWriter<GridType, DiscFuncType>::writeGrid(const GridType& grid, 
                                                              const std::string& filename) 
{
 
    // Temporary:  we write this level
    int level = grid.maxlevel();

    // Find out whether the grid contains only triangles.  If yes, then
    // it is written as a HxTriangularGrid.  If not, it cannot be
    // written (so far).
    bool containsOnlyTetrahedra = true;

    typename GridType::template Traits<0>::LevelIterator element = grid.template lbegin<0>(level);
    typename GridType::template Traits<0>::LevelIterator end     = grid.template lend<0>(level);

    for (; element!=end; ++element) {
        if (element->geometry().type() != triangle) {
            containsOnlyTetrahedra = false;
            break;
        }
    }

    printf("This is the 2D AmiraMesh writer!\n");
    int maxVerticesPerElement = (containsOnlyTetrahedra) ? 3 : 4;

    const int DIM = 2;

    int noOfNodes = grid.size(level, DIM);
    int noOfElem  = grid.size(level, 0);

    printf("noOfNodes %d,  nodeOfElem: %d\n", noOfNodes, noOfElem);
    int i;
    //int tl, k, noOfBndTri, MarkKey, ncomp, maxSubDom;

    std::string solFilename;

   
    // Construct the name for the geometry file
    std::string geoFilename(filename);
    geoFilename += ".am";

   // create amiramesh object
   AmiraMesh am_geometry;

   // Set the appropriate content type
   am_geometry.parameters.set("ContentType", "HxTriangularGrid");

   // write grid vertex coordinates
   AmiraMesh::Location* geo_nodes = new AmiraMesh::Location("Nodes", noOfNodes);
   am_geometry.insert(geo_nodes);

   AmiraMesh::Data* geo_node_data = new AmiraMesh::Data("Coordinates", geo_nodes, 
                                                        McPrimType::mc_float, DIM);
   am_geometry.insert(geo_node_data);


   typename GridType::template Traits<2>::LevelIterator vertex    = grid.template lbegin<DIM>(level);
   typename GridType::template Traits<2>::LevelIterator endvertex = grid.template lend<DIM>(level);
   
   for (; vertex!=endvertex; ++vertex) 
   {
       int index = vertex->index();
       Vec<DIM, double> coords = vertex->geometry()[0];

       ((float*)geo_node_data->dataPtr())[2*index+0] = coords(0);
       ((float*)geo_node_data->dataPtr())[2*index+1] = coords(1);
     
    }
       
#if 0
   // handle materials
   HxParamBundle* materials = new HxParamBundle("Materials");
   
   for (k=0; k<=maxSubDom; k++) {

       char buffer[100];
       sprintf(buffer, "Material%d", k);
       HxParamBundle* newMaterial = new HxParamBundle(buffer);

       HxParameter* newId = new HxParameter("Id", k);
       newMaterial->insert(newId);

       materials->insert(newMaterial);

   }   

   am_geometry.parameters.insert(materials);

   ncomp = 0;
   for(i=0; i<NVECTYPES; i++)
     ncomp = MAX(ncomp,VD_NCMPS_IN_TYPE(sol, i));
#endif

   /* write element section to geo - file */
   AmiraMesh::Location* element_loc = NULL;
   
   if (containsOnlyTetrahedra)
       element_loc = new AmiraMesh::Location("Triangles", noOfElem);
   else
       element_loc = new AmiraMesh::Location("Quadrangles", noOfElem);

   am_geometry.insert(element_loc);

   AmiraMesh::Data* element_data = new AmiraMesh::Data("Nodes", element_loc, 
                                                       McPrimType::mc_int32, maxVerticesPerElement);
   am_geometry.insert(element_data);  

   //int *(dPtr[maxVerticesPerElement]) = (int*)element_data->dataPtr();
   int *dPtr = (int*)element_data->dataPtr();

   typename GridType::template Traits<0>::LevelIterator element2   = grid.template lbegin<0>(level);
   typename GridType::template Traits<0>::LevelIterator endelement = grid.template lend<0>(level);

   for (i=0; element2!=endelement; ++element2, i++) {
       switch (element2->geometry().type()) {

       default:
          
           for (int j=0; j<element2->geometry().corners(); j++) 
               dPtr[i*maxVerticesPerElement+j] = element2->template subIndex<DIM>(j)+1;
           
           // If the element has less than 8 vertices use the last value
           // to fill up the remaining slots
           for (int j=element2->geometry().corners(); j<maxVerticesPerElement; j++)
               dPtr[i*maxVerticesPerElement+j] = dPtr[i*maxVerticesPerElement+element2->geometry().corners()-1];
       }

   }

   // write material section to geo-file
   AmiraMesh::Data* element_materials = new AmiraMesh::Data("Materials", element_loc, McPrimType::mc_uint8, 1);
   am_geometry.insert(element_materials);

//    for(i=0; i<noOfElem; i++)
//        ((unsigned char*)element_materials->dataPtr())[i] = SUBDOMAIN(elemList[i]);

   for(i=0; i<noOfElem; i++)
       ((unsigned char*)element_materials->dataPtr())[i] = 0;


   if(!am_geometry.write(geoFilename.c_str(), 1))
       DUNE_THROW(IOError, "Writing geometry file failed");
   

   std::cout << "Grid written successfully to: " << geoFilename << std::endl;
}
#endif // #ifdef _2

template<class GridType, class DiscFuncType>
void Dune::AmiraMeshWriter<GridType, DiscFuncType>::writeFunction(const DiscFuncType& f,
                                                              const std::string& filename)
{
    // Get grid type associated with DiscFuncType
    typedef typename DiscFuncType::FunctionSpaceType FunctionSpaceType;

    const GridType& grid = f.getFunctionSpace().getGrid();

    const int level = grid.maxlevel();
    const int noOfNodes = grid.size(level, GridType::dimension);

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
    typedef typename DiscFuncType::DofIteratorType DofIterator;
    DofIterator dit = f.dbegin(level);
    DofIterator ditend = f.dend(level);

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

