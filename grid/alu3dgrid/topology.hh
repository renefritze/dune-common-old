#ifndef DUNE_ALU3DGRIDTOPOLOGY_HH
#define DUNE_ALU3DGRIDTOPOLOGY_HH

#include "alu3dinclude.hh"

namespace Dune {

  template <ALU3dGridElementType type>
  struct EntityCount {};

  template <>
  struct EntityCount<tetra> {
    enum {numFaces = 4};
    enum {numVertices = 4};
    enum {numVerticesPerFace = 3};
  };

  template <>
  struct EntityCount<hexa> {
    enum {numFaces = 6};
    enum {numVertices = 8};
    enum {numVerticesPerFace = 4};
  };
  

  //! Maps indices of the Dune reference element onto the indices of the
  //! ALU3dGrid reference element and vice-versa.
  template <ALU3dGridElementType type>
  class ElementTopologyMapping 
  {
    enum { numFaces = EntityCount<type>::numFaces };
    enum { numVertices = EntityCount<type>::numVertices };
    enum { numVerticesPerFace = EntityCount<type>::numVerticesPerFace };
   
  public:
    //! Maps face index from Dune onto ALU3dGrid reference element
    static int dune2aluFace(int index);
    //! Maps face index from ALU3dGrid onto Dune reference element
    static int alu2duneFace(int index);

    //! Maps vertex index from Dune onto ALU3dGrid reference element
    static int dune2aluVertex(int index);
    //! Maps vertex index from ALU3dGrid onto Dune reference element
    static int alu2duneVertex(int index);

    //! Maps local vertex index of a face onto a global vertex index 
    //! (Dune->ALU3dGrid)
    //! \param face Face index (Dune reference element)
    //! \param localVertex Local vertex index on face \face (Dune reference 
    //! element)
    //! \return global vertex index in ALU3dGrid reference element
    static int dune2aluFaceVertex(int face, int localVertex);
    //! Maps local vertex index of a face onto a global vertex index 
    //! (ALU3dGrid->Dune)
    //! \param face Face index (ALU3dGrid reference element)
    //! \param localVertex Local vertex index on face \face 
    //! (ALU3dGrid reference element)
    //! \return global vertex index in Dune reference element
    static int alu2duneFaceVertex(int face, int localVertex);

  private:
    const static int dune2aluFace_[numFaces];
    const static int alu2duneFace_[numFaces];

    const static int dune2aluVertex_[numVertices];
    const static int alu2duneVertex_[numVertices];

    const static int dune2aluFaceVertex_[numFaces][numVerticesPerFace];
    const static int alu2duneFaceVertex_[numFaces][numVerticesPerFace];
  };

  //! Maps indices of the Dune reference face onto the indices of the
  //! ALU3dGrid reference face and vice-versa.
  template <ALU3dGridElementType type>
  class FaceTopologyMapping {
  public:
    //! Maps vertex index from Dune onto ALU3dGrid reference face
    static int dune2aluVertex(int index);
    //! Maps vertex index from Dune onto ALU3dGrid reference face, where the
    //! face in the ALU3dGrid has the twist \twist compared to the orientation
    //! of the respective face in the reference element
    //! \param index local Dune vertex index on the particular face (i.e. the 
    //! face which has a twist \twist compared to the reference element's face 
    //! \param twist twist of the face in consideration
    //! \return local ALU3dGrid vertex index on reference element face
    static int dune2aluVertex(int index, int twist);
    //! Maps vertex index from ALU3dGrid onto Dune reference face
    static int alu2duneVertex(int index);
    //! Maps vertex index from ALU3dGrid onto Dune reference face, where the
    //! face in the ALU3dGrid has the twist \twist compared to the orientation
    //! of the respective face in the reference element
    //! \param index local ALU3dGrid vertex index on the particular face (i.e.
    //! the face which has a twist \twist compared to the reference element's
    //! face 
    //! \param twist twist of the face in consideration
    //! \return local Dune vertex index on reference element face
    static int alu2duneVertex(int index, int twist);

    //  private:
    static int twist(int index, int faceTwist);
    static int invTwist(int index, int faceTwist);
  private:
    const static int dune2aluVertex_[EntityCount<type>::numVerticesPerFace];
    const static int alu2duneVertex_[EntityCount<type>::numVerticesPerFace];
  };

  inline const ALU3dImplTraits<tetra>::GEOFaceType*
  getFace(const ALU3DSPACE GEOTetraElementType& elem, int index) {
    assert(index >= 0 && index < 4);
    return elem.myhface3(ElementTopologyMapping<tetra>::dune2aluFace(index));
  }

  inline const ALU3dImplTraits<hexa>::GEOFaceType* 
  getFace(const ALU3DSPACE GEOHexaElementType& elem, int index) {
    assert(index >= 0 && index < 6);
    return elem.myhface4(ElementTopologyMapping<hexa>::dune2aluFace(index));
  }

  //- IMPLEMENTATION
  //- class ElementTopologyMapping
  template <ALU3dGridElementType type>
  int ElementTopologyMapping<type>::dune2aluFace(int index) {
    assert(index >= 0 && index < numFaces);
    return dune2aluFace_[index];
  }
  
  template <ALU3dGridElementType type>
  int ElementTopologyMapping<type>::alu2duneFace(int index) {
    assert(index >= 0 && index < numFaces);
    return alu2duneFace_[index];
  }

  template <ALU3dGridElementType type>
  int ElementTopologyMapping<type>::dune2aluVertex(int index) {
    assert(index >= 0 && index < numVertices);
    return dune2aluVertex_[index];
  }

  template <ALU3dGridElementType type>
  int ElementTopologyMapping<type>::alu2duneVertex(int index) {
    assert(index >= 0 && index < numVertices);
    return alu2duneVertex_[index];
  }

  template <ALU3dGridElementType type>
  int ElementTopologyMapping<type>::
  dune2aluFaceVertex(int face, int localVertex) {
    assert(face >= 0 && face < numFaces);
    assert(localVertex >= 0 && localVertex < numVerticesPerFace);
    return dune2aluFaceVertex_[face][localVertex];
  }

  template <ALU3dGridElementType type>
  int ElementTopologyMapping<type>::
  alu2duneFaceVertex(int face, int localVertex) {
    assert(face >= 0 && face < numFaces);
    assert(localVertex >= 0 && localVertex < numVerticesPerFace);
    return alu2duneFaceVertex_[face][localVertex];
  }

  //- class FaceTopologyMapping
  template <ALU3dGridElementType type>
  int FaceTopologyMapping<type>::dune2aluVertex(int index) {
    assert(index >= 0 && index < EntityCount<type>::numVerticesPerFace);
    return dune2aluVertex_[index];
  }

  template <ALU3dGridElementType type>
  int FaceTopologyMapping<type>::dune2aluVertex(int index, int twist) {
    assert(index >= 0 && index < EntityCount<type>::numVerticesPerFace);
    return invTwist(dune2aluVertex_[index], twist);
  }

  template <ALU3dGridElementType type>
  int FaceTopologyMapping<type>::alu2duneVertex(int index) {
    assert(index >= 0 && index < EntityCount<type>::numVerticesPerFace);
    return alu2duneVertex_[index];
  }

  template <ALU3dGridElementType type>
  int FaceTopologyMapping<type>::alu2duneVertex(int index, int twist) {
    assert(index >= 0 && index < EntityCount<type>::numVerticesPerFace);
    return alu2duneVertex_[invTwist(index, twist)];
  }

} // end namespace Dune
#endif
