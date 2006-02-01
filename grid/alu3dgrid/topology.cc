#include "topology.hh"

namespace Dune {

  //- class ElementTopologyMapping
  template <>
  const int ElementTopologyMapping<tetra>::
  dune2aluFace_[EntityCount<tetra>::numFaces] = {0, 1, 2, 3};

  // which face in the ALUGrid hexa is the face in the dune reference hexa 
  template <>
  const int ElementTopologyMapping<hexa>::
  dune2aluFace_[EntityCount<hexa>::numFaces] = {5, 3, 2, 4, 0, 1};

  // see gitter_geo.cc in the ALUGrid code for this mapping
  template <>
  const int ElementTopologyMapping<tetra>::
  dune2aluEdge_[EntityCount<tetra>::numEdges] = {0, 3 , 1 , 2 , 4, 5};

  // maps edges in the ALUGrid reference hexa to edges in the Dune Hexa
  template <>
  const int ElementTopologyMapping<hexa>::
  dune2aluEdge_[EntityCount<hexa>::numEdges] = {2, 4, 7, 6, 1, 3,
                                                9, 10, 0, 5, 8, 11};

  template <>
  const int ElementTopologyMapping<tetra>::
  alu2duneEdge_[EntityCount<tetra>::numEdges] = {0, 2, 3, 1 , 4, 5} ;

  // inverse mapping of the above dune2aluEdge for hexas 
  template <>
  const int ElementTopologyMapping<hexa>::
  alu2duneEdge_[EntityCount<hexa>::numEdges] = {8, 4, 0, 5, 1, 9, 
                                                3, 2, 10, 6, 7, 11};

  template <>
  const int ElementTopologyMapping<tetra>::
  alu2duneFace_[EntityCount<tetra>::numFaces] = {0, 1, 2, 3};

  // inverse mapping  of the dune2aluFace mapping 
  template <>
  const int ElementTopologyMapping<hexa>::
  alu2duneFace_[EntityCount<hexa>::numFaces] = {4, 5, 2, 1, 3, 0};

  template <>
  const int ElementTopologyMapping<tetra>::
  dune2aluVertex_[EntityCount<tetra>::numVertices] = {0, 1, 2, 3};

  // map the hexa vertices to the dune hexahedron vertices 
  template <>
  const int ElementTopologyMapping<hexa>::
  dune2aluVertex_[EntityCount<hexa>::numVertices] = {0, 1, 3, 2, 4, 5, 7, 6};

  template <>
  const int ElementTopologyMapping<tetra>::
  alu2duneVertex_[EntityCount<tetra>::numVertices] = {0, 1, 2, 3};

  // the same as dune2aluVertex for hexas 
  template <>
  const int ElementTopologyMapping<hexa>::
  alu2duneVertex_[EntityCount<hexa>::numVertices] = {0, 1, 3, 2, 4, 5, 7, 6};

  // actually, the orientation changes on each face, but this is compensated
  // by the change in orientation of the reference face
  template <>
  const int ElementTopologyMapping<tetra>::
  faceOrientation_[EntityCount<tetra>::numFaces] = {-1, -1, -1, -1};

  template <>
  const int ElementTopologyMapping<hexa>::
  faceOrientation_[EntityCount<hexa>::numFaces] = {-1, 1, 1, -1, -1, 1};

  template <>
  const int ElementTopologyMapping<tetra>::
  dune2aluFaceVertex_[numFaces][numVerticesPerFace] = {{0, 2, 1},
                                                       {0, 2, 1},
                                                       {0, 2, 1},
                                                       {0, 2, 1}};

  //********************************************************************
  // 
  //  local(inside) and global(outside) numbering of the face of a hexa 
  // 
  //           aluFace 0       ==         duneFace 4 
  //
  //    z   3           2                2         3 
  //    |    ----------                  ----------            
  //    |   /1       2/ alu --> dune    /2       3/  dune --> alu 
  //    |  /         / {0,2,3,1}       /         /   {0,3,1,2} 
  //    | /         /                 /         /
  //    |/0       3/                 /0       1/
  //     ----------                  ----------  
  //    0         1                 0         1 
  //         
  ////////////////////////////////////////////////////////////////////
  //
  //           aluFace 1       ==         duneFace 5 
  //
  //        7           6                6         7 
  //         ----------                  ----------            
  //        /3       2/ alu --> dune    /2       3/  dune --> alu 
  //       /         / {0,1,3,2}       /         /   {0,1,3,2} 
  //      /         /                 /         /
  //     /0       1/                 /0       1/
  //   4 ---------- 5              4 ---------- 5 
  //    |                                    
  //    |
  //    |
  //    |z 
  //
  ////////////////////////////////////////////////////////////////////
  //
  //           aluFace 2       ==         duneFace 2 
  //
  //     4          5                    4          5
  //      ----------                      ----------
  //      |3      2|  alu --> dune        |2      3|  dune --> alu 
  //      |        |  {0,1,3,2}           |        |  {0,1,3,2} 
  //      |        |                      |        |
  //      |0      1|                      |0      1|
  //      ----------                      ----------
  //     0          1                    0          1
  //      
  ////////////////////////////////////////////////////////////////////
  //
  //           aluFace 4       ==         duneFace 3 
  //
  //     7          6                    4          5
  //      ----------                      ----------
  //      |2      3|  alu --> dune        |2      3|  dune --> alu 
  //      |        |  {1,0,2,3}           |        |  {1,0,2,3} 
  //      |        |                      |        |
  //      |1      0|                      |0      1|
  //      ----------                      ----------
  //     3          2                    0          1
  //      
  ///////////////////////////////////////////////////////////////////
  //
  //           aluFace 3       ==         duneFace 1 
  //
  //           6                                 7 
  //          /|                                /|
  //         /2|   alu --> dune                /3|    dune --> alu 
  //        /  |   {0,1,3,2}                  /  |    {0,1,3,2}
  //       /   |                             /   |
  //     5/3  1|2                          5/2  1|3
  //      |   /                             |   /
  //      |  /                              |  /
  //      |0/                               |0/
  //      |/                                |/ 
  //      /--------x                        /      
  //     1                                 1   
  //      
  ///////////////////////////////////////////////////////////////////
  //
  //           aluFace 5       ==         duneFace 0 
  //
  //           7                                 6 
  //          /|                                /|
  //         /2|   alu --> dune                /3|    dune --> alu 
  //        /  |   {0,2,3,1}                  /  |    {0,3,1,2}
  //       /   |                             /   |
  //     4/1  3|3                          4/2  1|2
  //      |   /                             |   /
  //      |  /                              |  /
  //      |0/                               |0/
  //      |/                                |/ 
  // x----/                                 /      
  //     0                                 0   
  //      
  // maps for each face the local number so that the right dune face is
  // coming up 
  // parameter is local dune face number and vertex number 
  template <>
  const int ElementTopologyMapping<hexa>::
  dune2aluFaceVertex_[numFaces][numVerticesPerFace] = {{0, 3, 1, 2}, // ok
                                                       {0, 1, 3, 2}, // ok
                                                       {0, 1, 3, 2}, // ok
                                                       {1, 0, 2, 3}, // ok 
                                                       {0, 3, 1, 2}, // ok 
                                                       {0, 1, 3, 2}};// ok 

  template <>
  const int ElementTopologyMapping<tetra>::
  alu2duneFaceVertex_[numFaces][numVerticesPerFace] = {{0, 2, 1},
                                                       {0, 2, 1},
                                                       {0, 2, 1},
                                                       {0, 2, 1}};

  // the inverse mapping to the above dune2aluFaceVertex 
  // for hexa (see docu above)
  template <>
  const int ElementTopologyMapping<hexa>::
  alu2duneFaceVertex_[numFaces][numVerticesPerFace] = {{0, 2, 3, 1},  // ok 
                                                       {0, 1, 3, 2},  // ok
                                                       {0, 1, 3, 2},  // ok 
                                                       {0, 1, 3, 2},  // ok
                                                       {1, 0, 2, 3},  // ok 
                                                       {0, 2, 3, 1}}; // ok 

  //- class FaceTopologyMapping
  template <>
  int FaceTopologyMapping<tetra>::
  twist(int index, int faceTwist) {
    return (faceTwist < 0) ? 
      (7 - index + faceTwist)%3 : (faceTwist + index)%3 ;
  }

  template <>
  int FaceTopologyMapping<hexa>::
  twist(int index, int faceTwist) {
    return (faceTwist < 0) ? 
      (9 - index + faceTwist)%4 : (faceTwist + index)%4 ;
  }

  template <>
  int FaceTopologyMapping<tetra>::
  invTwist(int index, int faceTwist) {
    return (faceTwist < 0) ? 
      (7 - index + faceTwist)%3 : (3 + index - faceTwist)%3;
  }

  template <>
  int FaceTopologyMapping<hexa>::
  invTwist(int index, int faceTwist) {
    return (faceTwist < 0) ? 
      (9 - index + faceTwist)%4 : (4 + index - faceTwist)%4;
  }

  // alu traingle face are oriented just the other way then dune faces 
  // therefore vertex 1 and 2 are swapped because 
  // ALUGrid tetra face are oriented just the other way compared to Dune
  // tetra faces, see also gitter_geo.cc of the ALUGrid code
  template <>
  const int FaceTopologyMapping<tetra>::
  dune2aluVertex_[EntityCount<tetra>::numVerticesPerFace] = {0, 2, 1};


  // the mapping of vertices in the reference quad 
  // this is used for hexa face during intersection iterator build 
  // and to calculate the intersectionSelfLocal and
  // intersectionSelfNeighbor geometries. 
  template <>
  const int FaceTopologyMapping<hexa>::
  dune2aluVertex_[EntityCount<hexa>::numVerticesPerFace] = {0, 3, 1, 2};

  // alu triangle face are oriented just the other way then dune faces 
  // therefore vertex 1 and 2 are swaped 
  template <>
  const int FaceTopologyMapping<tetra>::
  alu2duneVertex_[EntityCount<tetra>::numVerticesPerFace] = {0, 2, 1};

  template <>
  const int FaceTopologyMapping<hexa>::
  alu2duneVertex_[EntityCount<hexa>::numVerticesPerFace] = {0, 2, 3, 1};

  template <>
  const int FaceTopologyMapping<tetra>::
  dune2aluEdge_[EntityCount<tetra>::numEdgesPerFace] = {1, 2, 0};

  template <>
  const int FaceTopologyMapping<hexa>::
  dune2aluEdge_[EntityCount<hexa>::numEdgesPerFace] = {0, 2, 3, 1};

  template <>
  const int FaceTopologyMapping<tetra>::
  alu2duneEdge_[EntityCount<tetra>::numEdgesPerFace] = {2, 0, 1};

  template <>
  const int FaceTopologyMapping<hexa>::
  alu2duneEdge_[EntityCount<hexa>::numEdgesPerFace] = {0, 3, 1, 2};
  
} // end namespace Dune
