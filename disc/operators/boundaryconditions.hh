#ifndef DUNE_BOUNDARYCONDITIONS_HH
#define DUNE_BOUNDARYCONDITIONS_HH

/**
 * @file dune/disc/operators/boundaryconditions.hh
 * @brief  Definition of boundary condition types, extend if necessary
 * @author Peter Bastian
 */
namespace Dune
{
  /** @addtogroup DISC_Operators
   *
   * @{
   */
  /**
   * @brief Define a class containing boundary condition flags
   *
   */

  //! base Class that defines boundary condition flags
  struct BoundaryConditions 
  {
      /** \brief These values are ordered according to precedence */
      enum Flags {neumann=1,     //!< Neumann boundary
                  process=2,      //!< Processor boundary 
                  dirichlet=3     //!< Dirichlet boundary
      };   
  };

  /** @} */
}
#endif
