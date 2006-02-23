// $Id$

#ifndef DUNE_LOCALSTIFFNESS_HH
#define DUNE_LOCALSTIFFNESS_HH

#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<stdio.h>
#include<stdlib.h>

#include"dune/common/timer.hh"
#include"dune/common/fvector.hh"
#include"dune/common/fmatrix.hh"
#include"dune/common/exceptions.hh"
#include"dune/common/geometrytype.hh"
#include"dune/grid/common/grid.hh"
#include"dune/grid/common/mcmgmapper.hh"
#include"dune/istl/bvector.hh"
#include"dune/istl/operators.hh"
#include"dune/istl/bcrsmatrix.hh"
#include"boundaryconditions.hh"

/**
 * @file dune/disc/operators/p1operator.hh
 * @brief  defines a class for piecewise linear finite element functions
 * @author Peter Bastian
 */

/*! @defgroup DISC_Operators Operators
  @ingroup DISC
  @brief
  
  @section D1 Introduction
  <!--=================-->
  
  To be written
*/

namespace Dune
{
  /** @addtogroup DISC_Operators
   *
   * @{
   */
  /**
   * @brief base class for assembling local stiffness matrices
   *
   */


  /*! @brief Base class for local assemblers

  This class serves as a base class for local assemblers. It provides
  space and access to the local stiffness matrix. The actual assembling is done
  in a derived class via the virtual assemble method.

  The template parameters are:

  - G    A grid type
  - RT   The field type used in the elements of the stiffness matrix
  - m    number of degrees of freedom per node (system size)
   */
  template<class G, class RT, int m>
  class LocalStiffness 
  {
	// grid types
	typedef typename G::ctype DT;
	typedef typename G::Traits::template Codim<0>::Entity Entity;
	enum {n=G::dimension};

  protected:
	enum {SIZE=10};

  public:
	// types for matrics, vectors and boundary conditions
	typedef FieldMatrix<RT,m,m> MBlockType;                      // one entry in the stiffness matrix
	typedef FieldVector<RT,m> VBlockType;                        // one entry in the global vectors
	typedef FixedArray<BoundaryConditions::Flags,m> BCBlockType; // componentwise boundary conditions

	/*! initialize local stiffness matrix
	  @param[in]  maxsize_  maximum ever size of the local stiffness matrix
	*/
	LocalStiffness ()
	{
	  currentsize_ = 0;
	}

	virtual ~LocalStiffness () 
	{
	}

	//! compute local stiffness matrix
	virtual void assemble (const Entity& e, int k=1) = 0;

	//! print contents of local stiffness matrix
	void print (std::ostream& s, int width, int precision)
	{
	  // set the output format
	  s.setf(std::ios_base::scientific, std::ios_base::floatfield);
	  int oldprec = s.precision();
	  s.precision(precision);

	  for (int i=0; i<currentsize_; i++)
		{
		  s << "FEM";    // start a new row
		  s << " ";      // space in front of each entry
		  s.width(4);    // set width for counter
		  s << i;        // number of first entry in a line
		  for (int j=0; j<currentsize_; j++)
			{
			  s << " ";         // space in front of each entry
			  s.width(width);   // set width for each entry anew
			  s << mat(i,j);     // yeah, the number !
			}
		  s << " ";         // space in front of each entry
		  s.width(width);   // set width for each entry anew
		  s << rhs(i);
		  s << " ";         // space in front of each entry
		  s.width(width);   // set width for each entry anew
		  s << bc(i)[0];
		  s << std::endl;// start a new line
		}
	  

	  // reset the output format
	  s.precision(oldprec);
	  s.setf(std::ios_base::fixed, std::ios_base::floatfield);
	}

	//! access local stiffness matrix
	/*! Access elements of the local stiffness matrix. Elements are
	  undefined without prior call to the assemble method.
	 */
	const MBlockType& mat (int i, int j) const
	{
	  return A[i][j];
	}


	//! access right hand side
	/*! Access elements of the right hand side vector. Elements are
	  undefined without prior call to the assemble method.
	 */
	const VBlockType& rhs (int i) const
	{
	  return b[i];
	}

	//! access boundary condition for each dof
	/*! Access boundary condition type for each degree of freedom. Elements are
	  undefined without prior call to the assemble method.
	 */
 	const BCBlockType& bc (int i) const
 	{
 	  return bctype[i];
 	}

	//! set the current size of the local stiffness matrix
	void setcurrentsize (int s)
	{
	  if (s>=SIZE)
 		DUNE_THROW(MathError,"LocalStiffness: increase SIZE");		
	  currentsize_ = s;
	}

	//! get the current size of the local stiffness matrix
	int currentsize ()
	{
	  return currentsize_;
	}

  protected:
	// assembled data
	int currentsize_;
	MBlockType A[SIZE][SIZE];
	VBlockType b[SIZE];
	BCBlockType bctype[SIZE];
  };


  /** @} */

}
#endif
