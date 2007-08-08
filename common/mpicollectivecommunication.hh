#ifndef DUNE_MPICOLLECTIVECOMMUNICATION_HH
#define DUNE_MPICOLLECTIVECOMMUNICATION_HH

#include<iostream>
#include<complex>
#include<algorithm>
#include<functional>

#include"exceptions.hh"
#include"collectivecommunication.hh"
#include"binaryfunctions.hh"

#if HAVE_MPI
// MPI header
#include<mpi.h>

namespace Dune
{
  //=======================================================
  // use singleton pattern and template specialization to 
  // generate MPI data types
  //=======================================================

  // any type is interpreted as contiguous piece of memory
  // i.e. only value types are allowed !
  template<typename T>
  class Generic_MPI_Datatype
  {
  public:
	static MPI_Datatype get ()
	{
	  if (type==0)
		{
		  type = new MPI_Datatype;
		  MPI_Type_contiguous(sizeof(T),MPI_BYTE,type);
		  MPI_Type_commit(type);
		}
	  return *type;
	}
  private:
	Generic_MPI_Datatype () {}
	Generic_MPI_Datatype (const Generic_MPI_Datatype& ) {}	
	static MPI_Datatype *type;
  };

  template<typename T>
  MPI_Datatype* Generic_MPI_Datatype<T>::type = 0; 

  // A Macro for defining traits for the primitive data types
#define ComposeMPITraits(p,m) \
  template<> \
  struct Generic_MPI_Datatype<p>{ \
    static inline MPI_Datatype get(){ \
      return m; \
    } \
  };

    
  ComposeMPITraits(char, MPI_CHAR);
  ComposeMPITraits(unsigned char,MPI_UNSIGNED_CHAR);
  ComposeMPITraits(short,MPI_SHORT);
  ComposeMPITraits(unsigned short,MPI_UNSIGNED_SHORT);
  ComposeMPITraits(int,MPI_INT);
  ComposeMPITraits(unsigned int,MPI_UNSIGNED);
  ComposeMPITraits(long,MPI_LONG);
  ComposeMPITraits(unsigned long,MPI_UNSIGNED_LONG);
  ComposeMPITraits(float,MPI_FLOAT);
  ComposeMPITraits(double,MPI_DOUBLE);
  ComposeMPITraits(long double,MPI_LONG_DOUBLE);

#undef ComposeMPITraits


  //=======================================================
  // use singleton pattern and template specialization to 
  // generate MPI operations
  //=======================================================

  template<typename Type, typename BinaryFunction>
  class Generic_MPI_Op
  {
    
  public:
	static MPI_Op get ()
	{
	  if (op==0)
		{
		  op = new MPI_Op;
		  MPI_Op_create((void (*)(void*, void*, int*, MPI_Datatype*))&operation,true,op);
		}
	  return *op;
	}
  private:
	static void operation (Type *in, Type *inout, int *len, MPI_Datatype *dptr)
	{
	  BinaryFunction func;
	  
	  for (int i=0; i< *len; ++i, ++in, ++inout){
	    Type temp;
	    temp = func(*in, *inout);
	    *inout = temp;
	  }
	}
	Generic_MPI_Op () {}
	Generic_MPI_Op (const Generic_MPI_Op& ) {}
	static MPI_Op* op;
  };

  
  template<typename Type, typename BinaryFunction>
  MPI_Op* Generic_MPI_Op<Type,BinaryFunction>::op = 0;
  
#define ComposeMPIOp(type,func,op) \
  template<> \
  class Generic_MPI_Op<type, func<type> >{ \
  public:\
    static MPI_Op get(){ \
      return op; \
    } \
  private:\
    Generic_MPI_Op () {}\
    Generic_MPI_Op (const Generic_MPI_Op& ) {}\
  };

    
  ComposeMPIOp(char, std::plus, MPI_SUM);
  ComposeMPIOp(unsigned char, std::plus, MPI_SUM);
  ComposeMPIOp(short, std::plus, MPI_SUM);
  ComposeMPIOp(unsigned short, std::plus, MPI_SUM);
  ComposeMPIOp(int, std::plus, MPI_SUM);
  ComposeMPIOp(unsigned int, std::plus, MPI_SUM);
  ComposeMPIOp(long, std::plus, MPI_SUM);
  ComposeMPIOp(unsigned long, std::plus, MPI_SUM);
  ComposeMPIOp(float, std::plus, MPI_SUM);
  ComposeMPIOp(double, std::plus, MPI_SUM);
  ComposeMPIOp(long double, std::plus, MPI_SUM);

  ComposeMPIOp(char, std::multiplies, MPI_PROD);
  ComposeMPIOp(unsigned char, std::multiplies, MPI_PROD);
  ComposeMPIOp(short, std::multiplies, MPI_PROD);
  ComposeMPIOp(unsigned short, std::multiplies, MPI_PROD);
  ComposeMPIOp(int, std::multiplies, MPI_PROD);
  ComposeMPIOp(unsigned int, std::multiplies, MPI_PROD);
  ComposeMPIOp(long, std::multiplies, MPI_PROD);
  ComposeMPIOp(unsigned long, std::multiplies, MPI_PROD);
  ComposeMPIOp(float, std::multiplies, MPI_PROD);
  ComposeMPIOp(double, std::multiplies, MPI_PROD);
  ComposeMPIOp(long double, std::multiplies, MPI_PROD);

  ComposeMPIOp(char, Min, MPI_MIN);
  ComposeMPIOp(unsigned char, Min, MPI_MIN);
  ComposeMPIOp(short, Min, MPI_MIN);
  ComposeMPIOp(unsigned short, Min, MPI_MIN);
  ComposeMPIOp(int, Min, MPI_MIN);
  ComposeMPIOp(unsigned int, Min, MPI_MIN);
  ComposeMPIOp(long, Min, MPI_MIN);
  ComposeMPIOp(unsigned long, Min, MPI_MIN);
  ComposeMPIOp(float, Min, MPI_MIN);
  ComposeMPIOp(double, Min, MPI_MIN);
  ComposeMPIOp(long double, Min, MPI_MIN);

  ComposeMPIOp(char, Max, MPI_MAX);
  ComposeMPIOp(unsigned char, Max, MPI_MAX);
  ComposeMPIOp(short, Max, MPI_MAX);
  ComposeMPIOp(unsigned short, Max, MPI_MAX);
  ComposeMPIOp(int, Max, MPI_MAX);
  ComposeMPIOp(unsigned int, Max, MPI_MAX);
  ComposeMPIOp(long, Max, MPI_MAX);
  ComposeMPIOp(unsigned long, Max, MPI_MAX);
  ComposeMPIOp(float, Max, MPI_MAX);
  ComposeMPIOp(double, Max, MPI_MAX);
  ComposeMPIOp(long double, Max, MPI_MAX);

#undef ComposeMPIOp


  //=======================================================
  // use singleton pattern and template specialization to 
  // generate MPI operations
  //=======================================================

  /*! \brief Specialization of CollectiveCommunication for MPI
	\ingroup ParallelCommunication
  */
  template<>
  class CollectiveCommunication<MPI_Comm>
  {
  public:
	//! Instantiation using a MPI communicator
	CollectiveCommunication (const MPI_Comm& c)
	  : communicator(c)
	{
	  MPI_Comm_rank(communicator,&me);
	  MPI_Comm_size(communicator,&procs);
	}

	//! @copydoc CollectiveCommunication::rank()
	int rank () const
	{
	  return me;
	}

	//! @copydoc CollectiveCommunication::size()
	int size () const
	{
	  return procs;
	}

	//! @copydoc CollectiveCommunication::sum(T&)
	template<typename T>
	T sum (T& in) const // MPI does not know about const :-(
	{
	  T out;
	  allreduce<std::plus<T> >(&in,&out,1);
	  return out;
	}

	//! @copydoc CollectiveCommunication::sum(T*,int)
	template<typename T>
	int sum (T* inout, int len) const
	{
	  return allreduce<std::plus<T> >(inout,len);
	}

	//! @copydoc CollectiveCommunication::prod(T&)
	template<typename T>
	T prod (T& in) const // MPI does not know about const :-(
	{
	  T out;
	  allreduce<std::multiplies<T> >(&in,&out,1);
	  return out;
	}

	//! @copydoc CollectiveCommunication::prod(T*,int)
	template<typename T>
	int prod (T* inout, int len) const
	{
	  return allreduce<std::plus<T> >(inout,len);
	}

	//! @copydoc CollectiveCommunication::min(T&)
	template<typename T>
	T min (T& in) const // MPI does not know about const :-(
	{
	  T out;
	  allreduce<Min<T> >(&in,&out,1);
	  return out;
	}

	//! @copydoc CollectiveCommunication::min(T*,int)
	template<typename T>
	int min (T* inout, int len) const
	{
	  return allreduce<Min<T> >(inout,len);
	}

    
	//! @copydoc CollectiveCommunication::max(T&)
	template<typename T>
	T max (T& in) const // MPI does not know about const :-(
	{
	  T out;
	  allreduce<Max<T> >(&in,&out,1);
	  return out;
	}

	//! @copydoc CollectiveCommunication::max(T*,int)
	template<typename T>
	int max (T* inout, int len) const
	{
	  return allreduce<Max<T> >(inout,len);
	}

	//! @copydoc CollectiveCommunication::barrier()
	int barrier () const
	{
	  return MPI_Barrier(communicator);
	}

	//! @copydoc CollectiveCommunication::broadcast()
	template<typename T>
	int broadcast (T* inout, int len, int root) const
	{
	  return MPI_Bcast(inout,len,Generic_MPI_Datatype<T>::get(),root,communicator);
	}
	
	//! @copydoc CollectiveCommunication::gather()
	template<typename T>
	int gather (T* in, T* out, int len, int root) const // note out must have space for P*len elements
	{
	  return MPI_Gather(in,len,Generic_MPI_Datatype<T>::get(),
						out,len,Generic_MPI_Datatype<T>::get(),
						root,communicator);
	}

	operator MPI_Comm () const
	{
	  return communicator;
	}

    template<typename BinaryFunction, typename Type>
    int allreduce(Type* inout, int len) const
    {
      Type* out = new Type[len];
      int ret = allreduce<BinaryFunction>(inout,out,len);
      std::copy(out, out+len, inout);
      delete[] out;
      return ret;
    }
    
    template<typename BinaryFunction, typename Type>
    int allreduce(Type* in, Type* out, int len) const
    {
      return MPI_Allreduce(in, out, len, Generic_MPI_Datatype<Type>::get(),
		    Generic_MPI_Op<Type, BinaryFunction>::get(),communicator);
    }
    
  private:
	MPI_Comm communicator;
	int me;
	int procs;
  };
} // namespace dune

#endif
#endif
