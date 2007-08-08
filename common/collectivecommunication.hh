#ifndef DUNE_COLLECTIVECOMMUNICATION_HH
#define DUNE_COLLECTIVECOMMUNICATION_HH

#include<iostream>
#include<complex>
#include<algorithm>

#include"exceptions.hh"

/*! \defgroup ParallelCommunication Parallel Communication
  \ingroup Common

  Dune offers an abstraction to the basic methods of parallel
  communication. It allows to switch parallel features on and of,
  without changing the code.

*/

namespace Dune
{
  /*! @brief Collective communication interface and sequential default implementation

  CollectiveCommunication offers an abstraction to the basic methods
  of parallel communication, following the message-passing
  paradigm. It allows to switch parallel features on and of, without
  changing the code. Currently only MPI and sequential code are
  supported.

  A CollectiveCommunication object is returned by all grids (also
  the sequential ones) in order to allow code to be written in
  a transparent way for sequential and parallel grids.

  This class provides a default implementation for sequential grids.
  The number of processes involved is 1, any sum, maximum, etc. returns
  just its input argument and so on.

  In specializations one can implement the real thing using appropriate
  communication functions, e.g. there exists an implementation using
  the Message Passing %Interface (MPI), see Dune::CollectiveCommunication<MPI_Comm>.

  Moreover, the communication subsystem used by an implementation
  is not visible in the interface, i.e. Dune grid implementations
  are not restricted to MPI.

  \ingroup ParallelCommunication
  */
  template<typename C>
  class CollectiveCommunication
  {
  public:
    //! Construct default object
    CollectiveCommunication()
    {}
	CollectiveCommunication (const C&)
	{}

	//! Return rank, is between 0 and size()-1
	int rank () const
	{
	  return 0;
	}

	//! Number of processes in set, is greater than 0
	int size () const
	{
	  return 1;
	}

	/** @brief  Compute the sum of the argument over all processes and 
		return the result in every process. Assumes that T has an operator+
	*/
	template<typename T>
	T sum (T& in) const // MPI does not know about const :-(
	{
	  return in;
	}

	/** @brief Compute the sum over all processes for each component of an array and return the result
		in every process. Assumes that T has an operator+
	 */
	template<typename T>
	int sum (T* inout, int len) const
	{
	  return 0;
	}

	/** @brief  Compute the product of the argument over all processes and 
		return the result in every process. Assumes that T has an operator*
	*/
	template<typename T>
	T prod (T& in) const // MPI does not know about const :-(
	{
	  return in;
	}

	/** @brief Compute the product over all processes 
		for each component of an array and return the result
		in every process. Assumes that T has an operator*
	 */
	template<typename T>
	int prod (T* inout, int len) const
	{
	  return 0;
	}

	/** @brief  Compute the minimum of the argument over all processes and 
		return the result in every process. Assumes that T has an operator<
	*/
	template<typename T>
	T min (T& in) const // MPI does not know about const :-(
	{
	  return in;
	}

	/** @brief Compute the minimum over all processes 
		for each component of an array and return the result
		in every process. Assumes that T has an operator<
	 */
	template<typename T>
	int min (T* inout, int len) const
	{
	  return 0;
	}

	/** @brief  Compute the maximum of the argument over all processes and 
		return the result in every process. Assumes that T has an operator<
	*/
	template<typename T>
	T max (T& in) const // MPI does not know about const :-(
	{
	  return in;
	}

	/** @brief Compute the maximum over all processes 
		for each component of an array and return the result
		in every process. Assumes that T has an operator<
	 */
	template<typename T>
	int max (T* inout, int len) const
	{
	  return 0;
	}

	/** @brief Wait until all processes have arrived at this point in the program.
	*/
	int barrier () const
	{
	  return 0;
	}

	/** @brief Distribute an array from the process with rank root to all other processes
	*/
	template<typename T>
	int broadcast (T* inout, int len, int root) const
	{
	  return 0;
	}

	/** @brief 	Each process sends its in array of length len to the root process
		(including the root itself). In the root process these arrays are stored in rank
		order in the out array which must have size len * number of processes.
	*/
	template<typename T>
	int gather (T* in, T* out, int len, int root) const // note out must have same size as in
	{
	  for (int i=0; i<len; i++)
		out[i] = in[i];
	  return 0;
	}

    /** 
     * @brief Compute something over all processes 
     * for each component of an array and return the result
     * in every process. 
     *
     * The template parameter BinaryFunction is the type of
     * the binary function to use for the computation
     *
     * @param inout The array to compute on.
     * @param len The number of components in the array
     */
    template<typename BinaryFunction, typename Type>
    int allreduce(Type* inout, int len) const
    {
      return 0;
    }
    
    /** 
     * @brief Compute something over all processes 
     * for each component of an array and return the result
     * in every process. 
     *
     * The template parameter BinaryFunction is the type of
     * the binary function to use for the computation
     *
     * @param in The array to compute on.
     * @param out The array to store the results in.
     * @param len The number of components in the array
     */
    template<typename BinaryFunction, typename Type>
    void allreduce(Type* in, Type* out, int len) const
    {
      std::copy(in, in+len, out);
      return 0;
    }
    
  };
}

#endif
