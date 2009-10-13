#ifndef DUNE_FINITE_STACK_HH
#define DUNE_FINITE_STACK_HH

#include <dune/common/exceptions.hh>

namespace Dune {
  
  /*! \addtogroup Common
    @{
  */

  /*! \file 
    
  This file implements a stack classes FiniteStack. It is
  mainly used by the grid iterators where exact knowledge of the stack
  implementation is needed to guarantee efficient execution.
  */

  /** \brief A stack with static memory allocation
   *
    This class implements a very efficient stack where the maximum
    depth is known in advance. Note that no error checking is
    performed!
 
    \param n Maximum number of stack entries
     */
  template<class T, int n>
  class FiniteStack {
  public :

      //! Returns true if the stack is empty
        bool empty () const
        {
          return f==0;
        }

      //! Returns true if the stack is full
        bool full () const
        {
          return f>=n;
        }

      //! Puts a new object onto the stack
        void push (const T& t)
        {
          s[f++] = t;
        }

      //! Removes and returns the uppermost object from the stack
        T pop ()
        {
          return s[--f];
        }
       
      //! Returns the uppermost object on the stack
        T top () const
        {
          return s[f-1];
        }
  
      //! Dynamic stacksize
        int size () const
        {
          return f;
        }

      //! Makes empty stack
        FiniteStack ()
        {
          f = 0;
        }

  private:
        T s[n];
        int f;
  };

}

//! }@

#endif
