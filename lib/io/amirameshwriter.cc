#include <amiramesh/AmiraMesh.h>



template<class GRID, class T>
void Dune::AmiraMeshWriter<GRID, T>::write(GRID& grid, 
                                  const Array<T>& sol,
                                  const std::string& filename) 
{
    // Temporary:  we write this level
    int level = grid.maxlevel();

    // Find out whether the grid contains only tetrahedra.  If yes, then
    // it is written in TetraGrid format.  If not, it is written in
    // hexagrid format.
    bool containsOnlyTetrahedra = true;

    GRID::Traits<0>::LevelIterator element = grid.template lbegin<0>(level);
    GRID::Traits<0>::LevelIterator end     = grid.template lend<0>(level);

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

    printf("noOfNodes %d,  nodeOfElem: %d\n", noOfNodes, noOfElem);
    int tl, k, i, noOfBndTri, MarkKey, ncomp, maxSubDom;

    std::string solFilename;

   
    // Construct the name for the geometry file
    std::string geoFilename(filename);
    geoFilename += ".am";

   // create amiramesh object
   AmiraMesh am_geometry;

   // write grid vertex coordinates
   AmiraMesh::Location* geo_nodes = new AmiraMesh::Location("Nodes", noOfNodes);
   am_geometry.insert(geo_nodes);

   AmiraMesh::Data* geo_node_data = new AmiraMesh::Data("Coordinates", geo_nodes, 
                                                        McPrimType::mc_float, DIM);
   am_geometry.insert(geo_node_data);


   GRID::Traits<3>::LevelIterator vertex    = grid.template lbegin<3>(level);
   GRID::Traits<3>::LevelIterator endvertex = grid.template lend<3>(level);
   i=0;
   for (; vertex!=endvertex; ++vertex) 
   {
       Vec<3, double> coords = vertex->geometry()[0];
       
       ((float*)geo_node_data->dataPtr())[i++] = coords(0);
       ((float*)geo_node_data->dataPtr())[i++] = coords(1);
       ((float*)geo_node_data->dataPtr())[i++] = coords(2);
     
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
       element_loc = new AmiraMesh::Location("Tetrahedra", noOfElem);
   else
       element_loc = new AmiraMesh::Location("Hexahedra", noOfElem);

   am_geometry.insert(element_loc);

   AmiraMesh::Data* element_data = new AmiraMesh::Data("Nodes", element_loc, 
                                                       McPrimType::mc_int32, maxVerticesPerElement);
   am_geometry.insert(element_data);  

   int (*dPtr)[maxVerticesPerElement] = (int(*)[maxVerticesPerElement]) element_data->dataPtr();

   GRID::Traits<0>::LevelIterator element2    = grid.template lbegin<0>(level);
   GRID::Traits<0>::LevelIterator endelement = grid.template lend<0>(level);

   for (i=0; element2!=endelement; ++element2, i++) {
       switch (element2->geometry().type()) {

       case hexahedron:
          
           dPtr[i][0] = element2->subIndex<3>(0)+1;
           dPtr[i][1] = element2->subIndex<3>(1)+1;
           dPtr[i][2] = element2->subIndex<3>(3)+1;
           dPtr[i][3] = element2->subIndex<3>(2)+1;
           dPtr[i][4] = element2->subIndex<3>(4)+1;
           dPtr[i][5] = element2->subIndex<3>(5)+1;
           dPtr[i][6] = element2->subIndex<3>(7)+1;
           dPtr[i][7] = element2->subIndex<3>(6)+1;
           break;
       default:
          
           for (int j=0; j<element2->geometry().corners(); j++) 
               dPtr[i][j] = element2->subIndex<3>(j)+1;
           
           // If the element has less than 8 vertices use the last value
           // to fill up the remaining slots
           for (int j=element2->geometry().corners(); j<maxVerticesPerElement; j++)
               dPtr[i][j] = dPtr[i][element2->geometry().corners()-1];
       }

   }

   // write material section to geo-file
   AmiraMesh::Data* element_materials = new AmiraMesh::Data("Materials", element_loc, McPrimType::mc_uint8, 1);
   am_geometry.insert(element_materials);

//    for(i=0; i<noOfElem; i++)
//        ((unsigned char*)element_materials->dataPtr())[i] = SUBDOMAIN(elemList[i]);

   for(i=0; i<noOfElem; i++)
       ((unsigned char*)element_materials->dataPtr())[i] = 0;


   //////////////////////////////////////////////////////
   
   // Now save the solution
#if 0
   AmiraMesh::Data* nodeData = new AmiraMesh::Data("Data", geo_nodes, McPrimType::mc_float, ncomp);
   am_geometry.insert(nodeData);

   AmiraMesh::Field* nodeField = new AmiraMesh::Field("f", ncomp, McPrimType::mc_float,
                                                      AmiraMesh::t_linear, nodeData);
   am_geometry.insert(nodeField);

   i=0;
   for (k=fl; k<=tl; k++)
     for (vec=FIRSTVECTOR(GRID_ON_LEVEL(theMG,k)); vec!= NULL; vec=SUCCVC(vec))
       if ((VOTYPE(vec) == NODEVEC) && ((k==tl) || (VNCLASS(vec)<1) ))
         {
             SHORT vtype   = VTYPE(vec);
             /** \bug This definition of ncomp shadows another one! */
             SHORT ncomp   = VD_NCMPS_IN_TYPE(sol, vtype);
             SHORT *cmpptr = VD_CMPPTR_OF_TYPE(sol, vtype);
             SHORT j;
             
             for (j=0; j<ncomp; j++)
                 ((float*)nodeData->dataPtr())[ncomp*i+j] = VVALUE(vec, cmpptr[j]);
           
             i++;
         }

   // actually save the solution file
//    if (!am_solution.write(solFilename, 1) ) {
//        printf("An error has occured writing file %s.\n", solFilename);
//        return PARAMERRORCODE;
//    }

#endif
   if(!am_geometry.write(geoFilename.c_str(), 1)) {
       printf("writing geometry file failed in amira : \n");
       /** \todo Do a decent error handling */
       return;
   }

   printf("Grid written successfully to:\n %s \n", geoFilename.c_str());
}



