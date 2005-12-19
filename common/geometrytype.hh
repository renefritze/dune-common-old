#ifndef DUNE_GEOMETRY_TYPE_HH
#define DUNE_GEOMETRY_TYPE_HH

/** \file
    \brief A unique label for each type of element that can occur in a grid
*/

namespace Dune {

    /** \brief Enum that declares identifiers for different geometry types. 
        
    This list can be extended in the future. Not all meshes need to implement
    all geometry types.

    \deprecated Only here during transition to the new ElementType class
    */
    
    enum GeometryType {vertex,line, triangle, quadrilateral, tetrahedron, 
                       pyramid, prism, hexahedron,
                       simplex, cube};
    
    /** \brief Associates a string to a GeometryType
    \deprecated Only here during transition to the new ElementType class
    */
    inline std::string GeometryName(GeometryType type){
        switch(type) {
        case vertex:
            return "vertex";
        case line:
            return "line";
        case triangle:
            return "triangle";
        case quadrilateral:
            return "quadrilateral";
        case tetrahedron:
            return "tetrahedron";
        case pyramid:
            return "pyramid";
        case prism:
            return "prism";
        case hexahedron:
            return "hexahedron";
        case simplex:
            return "simplex";
        case cube:
            return "cube";
            
        default:
            DUNE_THROW(NotImplemented, "name of unknown geometry requested");
        }
    }
    
    /** \brief Stream output of GeometryType 
    \deprecated Only here during transition to the new ElementType class
    */
    inline std::ostream& operator<< (std::ostream& s, Dune::GeometryType t)
    {
        s << Dune::GeometryName(t);
        return s;
    }


    /** \brief Unique label for each type of entities that can occur in DUNE grids

    This class has to be extended if a grid implementation with new entity types
    is added to DUNE.
     */
    class NewGeometryType
    {
    public:
        /** \brief Each entity can be tagged by one of these basic types 
            plus its space dimension */
        enum BasicType {simplex, cube, pyramid, prism};

    private:

        /** \brief Basic type of the element */
        BasicType basicType_;

        /** \brief Dimension of the element */
        short dim_;
        
    public:

        /** \brief Constructor */
        NewGeometryType(BasicType basicType, unsigned int dim)
            : basicType_(basicType), dim_(dim)
        {}

        /** @name Setup Methods */
        /*@{*/

        /** \brief Make a vertex */
        void makeVertex() {dim_ = 0;}
        
        /** \brief Make a line segment */
        void makeLine() {dim_ = 1;}
        
        /** \brief Make a triangle */
        void makeTriangle() {basicType_ = simplex; dim_ = 2;}
        
        /** \brief Make a quadrilateral */
        void makeQuadrilateral() {basicType_ = cube; dim_ = 2;}
        
        /** \brief Make a tetrahedron */
        void makeTetrahedron() {basicType_ = simplex; dim_ = 3;}
        
        /** \brief Make a pyramid */
        void makePyramid() {basicType_ = pyramid;}
        
        /** \brief Make a prism */
        void makePrism() {basicType_ = prism;}
        
        /** \brief Make a hexahedron */
        void makeHexahedron() {basicType_ = cube; dim_ = 3;}
        
        /** \brief Make a simplex of given dimension */
        void makeSimplex(unsigned int dim) {basicType_ = simplex; dim_ = dim;}
        
        /** \brief Make a hypercube of given dimension */
        void makeCube(unsigned int dim) {basicType_ = cube; dim_ = dim;}
        
        /*@}*/

        
        /** @name Query Methods */
        /*@{*/
        /** \brief Return true if entity is a vertex */
        bool isVertex() const {return dim_==0;}
        
        /** \brief Return true if entity is a line segment */
        bool isLine() const {return dim_==1;}
        
        /** \brief Return true if entity is a triangle */
        bool isTriangle() const {return basicType_==simplex && dim_==2;}
        
        /** \brief Return true if entity is a quadrilateral */
        bool isQuadrilateral() const {return basicType_==cube && dim_==2;}
        
        /** \brief Return true if entity is a tetrahedron */
        bool isTetrahedron() const {return basicType_==simplex && dim_==3;}
        
        /** \brief Return true if entity is a pyramid */
        bool isPyramid() const {return basicType_==pyramid;}
        
        /** \brief Return true if entity is a prism */
        bool isPrism() const {return basicType_==prism;}
        
        /** \brief Return true if entity is a hexahedron */
        bool isHexahedron() const {return basicType_==cube && dim_==3;}
        
        /** \brief Return true if entity is a simplex of any dimension */
        bool isSimplex() const {return basicType_==simplex || dim_ < 2;}
        
        /** \brief Return true if entity is a cube of any dimension */
        bool isCube() const {return basicType_==cube || dim_ < 2;}
        
        /** \brief Return dimension of the entity */
        unsigned int dim() const {return dim_;}

        /*@}*/
        
    };
    
}

#endif