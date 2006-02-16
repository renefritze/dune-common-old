#ifndef DUNE_ONE_D_GEOMETRY_HH
#define DUNE_ONE_D_GEOMETRY_HH

/** \file
 * \brief The OneDGridElement class and its specializations
 */

namespace Dune {

    template<int mydim, int coorddim, class GridImp>
    class OneDMakeableGeometry : public Geometry<mydim, coorddim, GridImp, OneDGridGeometry>
    {
    public:

        OneDMakeableGeometry() :
            Geometry<mydim, coorddim, GridImp, OneDGridGeometry>(OneDGridGeometry<mydim, coorddim, GridImp>())
        {};

        void setToTarget(OneDEntityImp<mydim>* target) {
            this->realGeometry.target_ = target;
        }

    };

    template<class GridImp>
    class OneDMakeableGeometry<1,1,GridImp> : public Geometry<1, 1, GridImp, OneDGridGeometry>
    {
    public:

        OneDMakeableGeometry() :
            Geometry<1, 1, GridImp, OneDGridGeometry>(OneDGridGeometry<1, 1, GridImp>())
        {};

        void setToTarget(OneDEntityImp<1>* target) {
            this->realGeometry.target_ = target;
        }

        void setPositions(double p1, double p2) {
            this->realGeometry.storeCoordsLocally_ = true;
            this->realGeometry.pos_[0][0] = p1;
            this->realGeometry.pos_[1][0] = p2;
        }
    };

    // forward declaration
    template <int codim, int dim, class GridImp>
    class OneDGridEntity;

    /****************************************************************/
    /*       Specialization for faces in a 1d grid (i.e. vertices)  */
    /****************************************************************/

    /** \brief Special implementation of the vertex geometry class directly holding the vertex position

    This vertex geometry class directly holds the vertex position instead of a pointer to an 
    implementation class.  Therefore it can store vertices that don't actually exist in the
    current grid.  This is necessary for methods like 
    OneDGridIntersectionIterator::intersectionSelfLocal()

    \todo This class can maybe completely replace the other one
    */
template<class GridImp>  
class OneDGridVertex : 
        public Geometry<0, 1, GridImp, OneDGridGeometry>
{ 
    
public:

    OneDGridVertex() :
        Geometry<0, 1, GridImp, OneDGridGeometry>(OneDGridGeometry<0, 1, GridImp>())
    {};

    //! return the element type identifier (vertex)
    GeometryType type () const {return GeometryType(GeometryType::cube,0);}

    //! return the number of corners of this element (==1)
    int corners () const {return 1;}

    //! access to coordinates of corners. Index is the number of the corner 
    const FieldVector<OneDCType, 1>& operator[] (int i) const {
        return pos_;
    }

    //private:
    FieldVector<OneDCType, 1> pos_;

};

template<class GridImp>  
class OneDGridGeometry <0, 1, GridImp> : 
        public GeometryDefaultImplementation <0, 1, GridImp,OneDGridGeometry>
{ 
    
    template <int codim_, int dim_, class GridImp_>
    friend class OneDGridEntity;
    template <int mydim_, int coorddim_, class GridImp_>
    friend class OneDGridGeometry;

public:

    //! return the element type identifier (vertex)
    GeometryType type () const {return GeometryType(0);}

    //! return the number of corners of this element (==1)
    int corners () const {return 1;}

    //! access to coordinates of corners. Index is the number of the corner 
    const FieldVector<OneDCType, 1>& operator[] (int i) const {
        return target_->pos_;
    }

    /** \brief Maps a local coordinate within reference element to 
     * global coordinate in element  */
    FieldVector<OneDCType, 1> global (const FieldVector<OneDCType, 0>& local) const {
        return target_->pos_;
    }

    /** \brief Maps a global coordinate within the element to a 
     * local coordinate in its reference element */
    FieldVector<OneDCType, 0> local (const FieldVector<OneDCType, 1>& global) const {
        FieldVector<OneDCType, 0> l;
        return l;
    }

    //private:
    OneDEntityImp<0>* target_;
    
};

//**********************************************************************
//
// --OneDGridGeometry
  /** \brief Defines the geometry part of a mesh entity. 
   * \ingroup OneDGrid
*/
template<int mydim, int coorddim, class GridImp>  
class OneDGridGeometry : 
public GeometryDefaultImplementation <mydim, coorddim, GridImp, OneDGridGeometry>
{ 
    template <int codim_, int dim_, class GridImp_>
    friend class OneDGridEntity;

    friend class OneDGrid<GridImp::dimension, GridImp::dimensionworld>;

    template <int cc_, int dim_, class GridImp_>
    friend class OneDGridSubEntityFactory;

    friend class OneDGridIntersectionIterator<GridImp>;

public:

    OneDGridGeometry() : storeCoordsLocally_(false) {}

    /** \brief Return the element type identifier 
     *
     * OneDGrid obviously supports only lines
     */
    GeometryType type () const {return GeometryType(1);}

    //! return the number of corners of this element. Corners are numbered 0...n-1
    int corners () const {return 2;}

    //! access to coordinates of corners. Index is the number of the corner 
    const FieldVector<OneDCType, coorddim>& operator[](int i) const {
        assert(i==0 || i==1);
        return (storeCoordsLocally_) ? pos_[i] : target_->vertex_[i]->pos_;
    }

    /** \brief Maps a local coordinate within reference element to 
     * global coordinate in element  */
    FieldVector<OneDCType, coorddim> global (const FieldVector<OneDCType, mydim>& local) const {
        FieldVector<OneDCType, coorddim> g;
        g[0] = (storeCoordsLocally_)
            ? pos_[0][0] * (1-local[0]) + pos_[1][0] * local[0]
            : target_->vertex_[0]->pos_[0] * (1-local[0]) + target_->vertex_[1]->pos_[0] * local[0];
        return g;
    }

    /** \brief Maps a global coordinate within the element to a 
     * local coordinate in its reference element */
    FieldVector<OneDCType, mydim> local (const FieldVector<OneDCType, coorddim>& global) const {
        FieldVector<OneDCType, mydim> l;
        if (storeCoordsLocally_) {
            l[0] = (global[0] - pos_[0][0]) / (pos_[1][0] - pos_[0][0]);
        } else {
            const double& v0 = target_->vertex_[0]->pos_[0];
            const double& v1 = target_->vertex_[1]->pos_[0];
            l[0] = (global[0] - v0) / (v1 - v0);
        }
        return l;
    }
    
    //! Returns true if the point is in the current element
    bool checkInside(const FieldVector<OneDCType, coorddim> &global) const {
        return (storeCoordsLocally_)
            ? pos_[0][0] <= global[0] && global[0] <= pos_[1][0]
            : target_->vertex_[0]->pos_[0] <= global[0] && global[0] <= target_->vertex_[1]->pos_[0];
    }

    /** ???
   */
    OneDCType integrationElement (const FieldVector<OneDCType, mydim>& local) const {
        return (storeCoordsLocally_)
            ? pos_[1][0] - pos_[0][0]
            : target_->vertex_[1]->pos_[0] - target_->vertex_[0]->pos_[0];
    }

    //! The Jacobian matrix of the mapping from the reference element to this element
    const FieldMatrix<OneDCType,mydim,mydim>& jacobianInverseTransposed (const FieldVector<OneDCType, mydim>& local) const {
        if (storeCoordsLocally_)
            jacInverse_[0][0] = 1 / (pos_[1][0] - pos_[0][0]);
        else
            jacInverse_[0][0] = 1 / (target_->vertex_[1]->pos_[0] - target_->vertex_[0]->pos_[0]);

        return jacInverse_;
    }


    //private:
    OneDEntityImp<1>* target_;

    bool storeCoordsLocally_;

    // Stores the element corner positions if it is returned as geometryInFather
    FieldVector<OneDCType,coorddim> pos_[2];

    //! The jacobian inverse
    mutable FieldMatrix<OneDCType,coorddim,coorddim> jacInverse_;

};

}  // namespace Dune

#endif
