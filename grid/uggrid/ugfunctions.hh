#ifndef __DUNE_UG_FUNCTIONS_HH__
#define __DUNE_UG_FUNCTIONS_HH__

#include "ugtypes.hh"

namespace Dune {

template<int dim>
class UG {
public:

    /** \brief Returns pointers to the coordinate arrays of an UG element */
    static void Corner_Coordinates(typename TargetType<0,dim>::T* theElement, double* x[]);

    static int Sides_Of_Elem(typename TargetType<0,dim>::T* theElement) {
#ifdef _3
        using UG3d::nb_offset;
        using UG3d::element_descriptors;
#endif
        return SIDES_OF_ELEM(theElement);
    }

    //! Encapsulates the NBELEM macro
    static typename TargetType<0,dim>::T* NbElem(typename TargetType<0,dim>::T* theElement, int nb) {
#ifdef _3
        using UG3d::ELEMENT;
        using UG3d::nb_offset;
#else
        using UG2d::ELEMENT;
#endif
        return NBELEM(theElement, nb);
    }

    //! Encapsulates the TAG macro
    static unsigned int Tag(const typename TargetType<0,dim>::T* theElement) {
        return TAG(theElement);
    }

    static int Corners_Of_Elem(const typename TargetType<0,dim>::T* theElement) {
#ifdef _2
        using UG2d::element_descriptors;
#else
        using UG3d::element_descriptors;
#endif
        return CORNERS_OF_ELEM(theElement);
    }

    static void Local_To_Global(int n, DOUBLE** y, 
                                const Dune::Vec<dim, double>& local,  
                                Dune::Vec<dim, double>& global) {
        LOCAL_TO_GLOBAL(n,y,local,global);
    }

    /** 
     * \param n Number of corners of the element
     * \param x Coordinates of the corners of the element
     * \param local Local evaluation point
     *
     */
    static void Transformation(int n, double** x, 
                        const Vec<dim, double>& local, Mat<dim,dim,double>& mat) {
        TRANSFORMATION(n, x, local, mat);
    }

    static typename TargetType<dim,dim>::T* Corner(typename TargetType<0,dim>::T* theElement, int i) {
#ifdef _2
        using UG2d::NODE;
        using UG2d::n_offset;
#else
        using UG3d::NODE;
        using UG3d::n_offset;
#endif
        return CORNER(theElement, i);
    }

    static void InitUg(int* argcp, char*** argvp) {
#ifdef _3
        UG3d::InitUg(argcp, argvp);
#else
        UG2d::InitUg(argcp, argvp);
#endif
    }

#ifdef _3
    static void* CreateBoundaryValueProblem(char* BVPname,
                                          int numOfCoeffFunc, 
                                          UG3d::CoeffProcPtr coeffs[],
                                          int numOfUserFct,
                                          UG3d::UserProcPtr userfct[]) {
        return UG3d::CreateBoundaryValueProblem(BVPname, 0, numOfCoeffFunc, coeffs,
                                                numOfUserFct, userfct);
    }
#else
    static void* CreateBoundaryValueProblem(char* BVPname,
                                          int numOfCoeffFunc, 
                                          UG2d::CoeffProcPtr coeffs[],
                                          int numOfUserFct,
                                          UG2d::UserProcPtr userfct[]) {
        return UG2d::CreateBoundaryValueProblem(BVPname, 0, numOfCoeffFunc, coeffs,
                                                numOfUserFct, userfct);
    }
#endif

};



#ifdef _3
/** \brief Returns pointers to the coordinate arrays of an UG element */
void Corner_Coordinates(UG3d::ELEMENT* theElement, double* x[])
{
    using UG3d::TETRAHEDRON;
    using UG3d::NODE;
    using UG3d::PYRAMID;
    using UG3d::PRISM;
    using UG3d::HEXAHEDRON;
    using UG3d::n_offset;
    int n;  // Dummy variable just to please the macro
    CORNER_COORDINATES(theElement, n, x);
}
#else
/** \brief Returns pointers to the coordinate arrays of an UG element */
void Corner_Coordinates(TargetType<0,2>::T* theElement, double* x[])
{
    using UG2d::NODE;
    using UG2d::TRIANGLE;
    using UG2d::QUADRILATERAL;
    using UG2d::n_offset;
    int n;  // Dummy variable just to please the macro
    CORNER_COORDINATES(theElement, n, x);
}
#endif

} // namespace Dune

#endif
