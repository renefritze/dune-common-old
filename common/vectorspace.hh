#ifndef DUNE_VECTORSPACE_HH
#define DUNE_VECTORSPACE_HH

namespace Dune{

  /** @defgroup AlgebraCommon Functions and Operators
      The Functions and Operators Module contains objects of 
      arbitrary vector spaces and mappings between them.
  @{
  */

 /** @defgroup Vector Elements of Vector Spaces
     An instance of this class is an element of an vector space. 
     Elements of vector spaces can be added and multiplied by a 
     scalar.
  @{
  */

    /** \brief Vector class
     * This is the base class for all methods and operators.
     */
template <typename Field> 
class Vector 
{
public:

    virtual ~Vector() {};

  //virtual Vector<Field> operator + (const Vector<Field> &) const = 0;
  //virtual Vector<Field> operator - (const Vector<Field> &) const = 0;
  //virtual Vector<Field> operator * (const Field &) const = 0;
  //virtual Vector<Field> operator / (const Field &) const = 0;
  
  /** \brief Assignment operator
      \note Only returns itself...
  */
  virtual Vector<Field>& operator  = (const Vector<Field> &) { return *this;};

  //! Addition
  virtual Vector<Field>& operator += (const Vector<Field> &) = 0;
  //! Subtraction
  virtual Vector<Field>& operator -= (const Vector<Field> &) = 0;
  //! Multiplication
  virtual Vector<Field>& operator *= (const Field &) = 0;
  //! Division
  virtual Vector<Field>& operator /= (const Field &) = 0;
};


/** @} end documentation group */

}


#endif
