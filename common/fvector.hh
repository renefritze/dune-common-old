// $Id$
#ifndef DUNE_FVECTOR_HH
#define DUNE_FVECTOR_HH

// this for backwards compatiblity
#define USE_DEPRECATED_K1

#include<math.h>
#include<complex>

#include "exceptions.hh"
#include "genericiterator.hh"

namespace Dune {
   
  /** @defgroup DenseMatVec Dense Matrix and Vector Template Library
	  @{
  */

/*! \file 
 * \brief This file implements a vector constructed from a given type
representing a field and a compile-time given size.
*/

  // forward declaration of template
  template<class K, int n> class FieldVector;

  // template meta program for assignment from scalar
  template<int I>
  struct fvmeta_assignscalar {
	template<class K, int n>
	static K& assignscalar (FieldVector<K,n>& x, const K& k)
	{
	  x[I] = fvmeta_assignscalar<I-1>::assignscalar(x,k);
	  return x[I];
	}
  };
  template<>
  struct fvmeta_assignscalar<0> {
	template<class K, int n>
	static K& assignscalar (FieldVector<K,n>& x, const K& k)
	{
	  x[0] = k;
	  return x[0];
	}
  };

  // template meta program for operator+=
  template<int I>
  struct fvmeta_plusequal {
	template<class K, int n>
	static void plusequal (FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  fvmeta_plusequal<I-1>::plusequal(x,y);
	  x[I] += y[I];
	}
	template<class K, int n>
	static void plusequal (FieldVector<K,n>& x, const K& y)
	{
	  fvmeta_plusequal<I-1>::plusequal(x,y);
	  x[I] += y;
	}
  };
  template<>
  struct fvmeta_plusequal<0> {
	template<class K, int n>
	static void plusequal (FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  x[0] += y[0];
	}
	template<class K, int n>
	static void plusequal (FieldVector<K,n>& x, const K& y)
	{
	  x[0] += y;
	}
  };

  // template meta program for operator-=
  template<int I>
  struct fvmeta_minusequal {
	template<class K, int n>
	static void minusequal (FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  fvmeta_minusequal<I-1>::minusequal(x,y);
	  x[I] -= y[I];
	}
	template<class K, int n>
	static void minusequal (FieldVector<K,n>& x, const K& y)
	{
	  fvmeta_minusequal<I-1>::minusequal(x,y);
	  x[I] -= y;
	}
  };
  template<>
  struct fvmeta_minusequal<0> {
	template<class K, int n>
	static void minusequal (FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  x[0] -= y[0];
	}
	template<class K, int n>
	static void minusequal (FieldVector<K,n>& x, const K& y)
	{
	  x[0] -= y;
	}
  };

  // template meta program for operator*=
  template<int I>
  struct fvmeta_multequal {
	template<class K, int n>
	static void multequal (FieldVector<K,n>& x, const K& k)
	{
	  fvmeta_multequal<I-1>::multequal(x,k);
	  x[I] *= k;
	}
  };
  template<>
  struct fvmeta_multequal<0> {
	template<class K, int n>
	static void multequal (FieldVector<K,n>& x, const K& k)
	{
	  x[0] *= k;
	}
  };

  // template meta program for operator/=
  template<int I>
  struct fvmeta_divequal {
	template<class K, int n>
	static void divequal (FieldVector<K,n>& x, const K& k)
	{
	  fvmeta_divequal<I-1>::divequal(x,k);
	  x[I] /= k;
	}
  };
  template<>
  struct fvmeta_divequal<0> {
	template<class K, int n>
	static void divequal (FieldVector<K,n>& x, const K& k)
	{
	  x[0] /= k;
	}
  };

  // template meta program for operator==
  template<int I>
  struct fvmeta_equality {
	template<class K, int n>
	static bool equality (const FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  return x[I] == y[I] && fvmeta_equality<I-1>::equality(x,y);
	}
  };
  template<>
  struct fvmeta_equality<0> {
	template<class K, int n>
	static bool equality (const FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  return x[0] == y[0];
	}
  };

  // template meta program for axpy
  template<int I>
  struct fvmeta_axpy {
	template<class K, int n>
	static void axpy (FieldVector<K,n>& x, const K& a, const FieldVector<K,n>& y)
	{
	  fvmeta_axpy<I-1>::axpy(x,a,y);
	  x[I] += a*y[I];
	}
  };
  template<>
  struct fvmeta_axpy<0> {
	template<class K, int n>
	static void axpy (FieldVector<K,n>& x, const K& a, const FieldVector<K,n>& y)
	{
	  x[0] += a*y[0];
	}
  };

  // template meta program for dot
  template<int I>
  struct fvmeta_dot {
	template<class K, int n>
	static K dot (const FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  return x[I]*y[I] + fvmeta_dot<I-1>::dot(x,y);
	}
  };
  template<>
  struct fvmeta_dot<0> {
	template<class K, int n>
	static K dot (const FieldVector<K,n>& x, const FieldVector<K,n>& y)
	{
	  return x[0]*y[0];
	}
  };

  // some abs functions needed for the norms
  template<class K>
  inline double fvmeta_abs (const K& k)
  {
	return (k >= 0) ? k : -k;
  }

  template<class K>
  inline double fvmeta_abs (const std::complex<K>& c)
  {
	return sqrt(c.real()*c.real() + c.imag()*c.imag());
  }

  template<class K>
  inline double fvmeta_absreal (const K& k)
  {
	return fvmeta_abs(k);
  }

  template<class K>
  inline double fvmeta_absreal (const std::complex<K>& c)
  {
	return fvmeta_abs(c.real()) + fvmeta_abs(c.imag());
  }

  template<class K>
  inline double fvmeta_abs2 (const K& k)
  {
	return k*k;
  }

  template<class K>
  inline double fvmeta_abs2 (const std::complex<K>& c)
  {
	return c.real()*c.real() + c.imag()*c.imag();
  }

  // template meta program for one_norm
  template<int I>
  struct fvmeta_one_norm {
	template<class K, int n>
	static double one_norm (const FieldVector<K,n>& x)
	{
	  return fvmeta_abs(x[I]) + fvmeta_one_norm<I-1>::one_norm(x);
	}
  };
  template<>
  struct fvmeta_one_norm<0> {
	template<class K, int n>
	static double one_norm (const FieldVector<K,n>& x)
	{
	  return fvmeta_abs(x[0]);
	}
  };

  // template meta program for one_norm_real
  template<int I>
  struct fvmeta_one_norm_real {
	template<class K, int n>
	static double one_norm_real (const FieldVector<K,n>& x)
	{
	  return fvmeta_absreal(x[I]) + fvmeta_one_norm_real<I-1>::one_norm_real(x);
	}
  };
  template<>
  struct fvmeta_one_norm_real<0> {
	template<class K, int n>
	static double one_norm_real (const FieldVector<K,n>& x)
	{
	  return fvmeta_absreal(x[0]);
	}
  };

  // template meta program for two_norm squared
  template<int I>
  struct fvmeta_two_norm2 {
	template<class K, int n>
	static double two_norm2 (const FieldVector<K,n>& x)
	{
	  return fvmeta_abs2(x[I]) + fvmeta_two_norm2<I-1>::two_norm2(x);
	}
  };
  template<>
  struct fvmeta_two_norm2<0> {
	template<class K, int n>
	static double two_norm2 (const FieldVector<K,n>& x)
	{
	  return fvmeta_abs2(x[0]);
	}
  };

  // template meta program for infinity norm
  template<int I>
  struct fvmeta_infinity_norm {
	template<class K, int n>
	static double infinity_norm (const FieldVector<K,n>& x)
	{
	  return std::max(fvmeta_abs(x[I]),fvmeta_infinity_norm<I-1>::infinity_norm(x));
	}
  };
  template<>
  struct fvmeta_infinity_norm<0> {
	template<class K, int n>
	static double infinity_norm (const FieldVector<K,n>& x)
	{
	  return fvmeta_abs(x[0]);
	}
  };

  // template meta program for simplified infinity norm
  template<int I>
  struct fvmeta_infinity_norm_real {
	template<class K, int n>
	static double infinity_norm_real (const FieldVector<K,n>& x)
	{
	  return std::max(fvmeta_absreal(x[I]),fvmeta_infinity_norm_real<I-1>::infinity_norm_real(x));
	}
  };
  template<>
  struct fvmeta_infinity_norm_real<0> {
	template<class K, int n>
	static double infinity_norm_real (const FieldVector<K,n>& x)
	{
	  return fvmeta_absreal(x[0]);
	}
  };

  /** \brief Construct a vector space out of a tensor product of fields.

	 K is the field type (use float, double, complex, etc) and n 
	 is the number of components.

	 It is generally assumed that K is a numerical type compatible with double
	 (E.g. norms are always computed in double precision).

	 Implementation of all members uses template meta programs where appropriate
  */
  template<class K, int SIZE>
  class FieldVector
  {
	//! The actual number of elements that gets allocated.
	//! It's always at least 1.
	/** \todo Wie genau gehen wir mit dem Fall N==0 um?  So? */
	enum { n = (SIZE > 0) ? SIZE : 1 };

  public:
	// remember size of vector 
	enum { dimension = SIZE };

	// standard constructor and everything is sufficient ...

	//===== type definitions and constants

	//! export the type representing the field
	typedef K field_type;

	//! export the type representing the components
	typedef K block_type;

	//! We are at the leaf of the block recursion
	enum {
	  //! The number of block levels we contain
	  blocklevel = 1
	};

	//! export size
	enum {
	  //! The size of this vector.
	  size = n
	};

	//! Constructor making uninitialized vector
	FieldVector() {}

	//! Constructor making vector with identical coordinates
	explicit FieldVector (const K& t)
	{
	  for (int i=0; i<n; i++) p[i] = t;
	}

	//===== assignment from scalar
        //! Assignment operator for scalar
	FieldVector& operator= (const K& k)
	{
	  fvmeta_assignscalar<n-1>::assignscalar(*this,k);
	  return *this;   
	}


	//===== access to components

	//! random access 
	K& operator[] (int i)
	{
#ifdef DUNE_ISTL_WITH_CHECKING
	  if (i<0 || i>=n) DUNE_THROW(MathError,"index out of range");
#endif
	  return p[i];
	}

	//! same for read only access
	const K& operator[] (int i) const
	{
#ifdef DUNE_ISTL_WITH_CHECKING
	  if (i<0 || i>=n) DUNE_THROW(MathError,"index out of range");
#endif
	  return p[i];
	}

	//! Iterator class for sequential access
    typedef Dune::GenericIterator<FieldVector<K,n>,K> Iterator;
	//! typedef for stl compliant access
    typedef Iterator iterator;
    
	//! begin iterator
	Iterator begin ()
	{
	  return Iterator(*this,0);
	}

	//! end iterator
	Iterator end ()
	{
	  return Iterator(*this,n);
	}

	//! begin iterator
	Iterator rbegin ()
	{
	  return Iterator(*this,n-1);
	}

	//! end iterator
	Iterator rend ()
	{
	  return Iterator(*this,-1);
	}

	//! return iterator to given element or end()
	Iterator find (int i)
	{
	  if (i>=0 && i<n)
		return Iterator(*this,i);
	  else
		return Iterator(*this,n);
	}

	//! ConstIterator class for sequential access
    typedef Dune::GenericIterator<const FieldVector<K,n>,const K> ConstIterator;
	//! typedef for stl compliant access
    typedef ConstIterator const_iterator;

	//! begin ConstIterator
	ConstIterator begin () const
	{
	  return ConstIterator(*this,0);
	}

	//! end ConstIterator
	ConstIterator end () const
	{
	  return ConstIterator(*this,n);
	}

	//! begin ConstIterator
	ConstIterator rbegin () const
	{
	  return ConstIterator(*this,n-1);
	}

	//! end ConstIterator
	ConstIterator rend () const
	{
	  return ConstIterator(*this,-1);
	}

	//! return iterator to given element or end()
	ConstIterator find (int i) const
	{
	  if (i>=0 && i<n)
		return ConstIterator(*this,i);
	  else
		return ConstIterator(*this,n);
	}
    
	//===== vector space arithmetic

	//! vector space addition
	FieldVector& operator+= (const FieldVector& y)
	{
	  fvmeta_plusequal<n-1>::plusequal(*this,y);
	  return *this;
	}

	//! vector space subtraction
	FieldVector& operator-= (const FieldVector& y)
	{
	  fvmeta_minusequal<n-1>::minusequal(*this,y);
	  return *this;
	}

	//! Binary vector addition
	FieldVector<K, size> operator+ (const FieldVector<K, size>& b) const
	{
	  FieldVector<K, size> z = *this;
	  return (z+=b);
	}

	//! Binary vector subtraction
	FieldVector<K, size> operator- (const FieldVector<K, size>& b) const
	{
	  FieldVector<K, size> z = *this;
	  return (z-=b);
	}

	//! vector space add scalar to all comps
	FieldVector& operator+= (const K& k)
	{
	  fvmeta_plusequal<n-1>::plusequal(*this,k);
	  return *this;
	}

	//! vector space subtract scalar from all comps
	FieldVector& operator-= (const K& k)
	{
	  fvmeta_minusequal<n-1>::minusequal(*this,k);
	  return *this;
	}

	//! vector space multiplication with scalar 
	FieldVector& operator*= (const K& k)
	{
	  fvmeta_multequal<n-1>::multequal(*this,k);
	  return *this;
	}

	//! vector space division by scalar
	FieldVector& operator/= (const K& k)
	{
	  fvmeta_divequal<n-1>::divequal(*this,k);
	  return *this;
	}

	//! Binary vector comparison
	bool operator== (const FieldVector& y) const
	{
	  return fvmeta_equality<n-1>::equality(*this,y);
	}

	//! vector space axpy operation
	FieldVector& axpy (const K& a, const FieldVector& y)
	{
	  fvmeta_axpy<n-1>::axpy(*this,a,y);
	  return *this;
	}


	//===== Euclidean scalar product

	//! scalar product
    K operator* (const FieldVector& y) const
	{
	  return fvmeta_dot<n-1>::dot(*this,y);
	}


	//===== norms

	//! one norm (sum over absolute values of entries)
    double one_norm () const
	{
	  return fvmeta_one_norm<n-1>::one_norm(*this);
	}

	//! simplified one norm (uses Manhattan norm for complex values)
    double one_norm_real () const
	{
	  return fvmeta_one_norm_real<n-1>::one_norm_real(*this);
	}

	//! two norm sqrt(sum over squared values of entries)
    double two_norm () const
	{
	  return sqrt(fvmeta_two_norm2<n-1>::two_norm2(*this));
	}

	//! sqare of two norm (sum over squared values of entries), need for block recursion
    double two_norm2 () const
	{
	  return fvmeta_two_norm2<n-1>::two_norm2(*this);
	}

	//! infinity norm (maximum of absolute values of entries)
    double infinity_norm () const
	{
	  return fvmeta_infinity_norm<n-1>::infinity_norm(*this);
	}

	//! simplified infinity norm (uses Manhattan norm for complex values)
	double infinity_norm_real () const
	{
	  return fvmeta_infinity_norm_real<n-1>::infinity_norm_real(*this);
	}


	//===== sizes

	//! number of blocks in the vector (are of size 1 here)
	int N () const
	{
	  return n;
	}

	//! dimension of the vector space
	int dim () const
	{
	  return n;
	}

      //! Send vector to output stream
	void print (std::ostream& s) const
	{
	  for (int i=0; i<n; i++)
		if (i>0)
		  s << " " << p[i];
		else
		  s << p[i];
	}

  private:
	// the data, very simply a built in array
	K p[n]; 
  };

  //! Send vector to output stream
  template<typename K, int n>
  std::ostream& operator<< (std::ostream& s, const FieldVector<K,n>& v)
  {
    v.print(s);
    return s;
  }

#ifdef USE_DEPRECATED_K1
  // forward declarations
  template<class K> class K1Vector;

  /**! \brief Vectors containing only one component
   */
  template<class K>
  class K1Vector
  {
  public:
	friend class K11Matrix<K>;

	//===== type definitions and constants

	//! export the type representing the field
	typedef K field_type;

	//! export the type representing the components
	typedef K block_type;

	//! We are at the leaf of the block recursion
	enum {
	  //! The number of block levels we contain.
	  // This is always one for this type.
	  blocklevel = 1
	};

	//! export size
	enum {
	  //! The size of this vector.
	  // This is always one for this type.
	  size = 1};

	//===== construction

      /** \brief Default constructor */
	K1Vector ()
	{       }

      /** \brief Constructor with a given scalar */
	K1Vector (const K& k)
	{
	  p = k;
	}

      /** \brief Assignment from the base type */
	K1Vector& operator= (const K& k)
	{
	  p = k;
	  return *this;   
	}

	//===== vector space arithmetic

	//! vector space addition
	K1Vector& operator+= (const K1Vector& y)
	{
	  p += y.p;
	  return *this;
	}

	//! vector space subtraction
	K1Vector& operator-= (const K1Vector& y)
	{
	  p -= y.p;
	  return *this;
	}

	//! vector space add scalar to each comp
	K1Vector& operator+= (const K& k)
	{
	  p += k;
	  return *this;
	}

	//! vector space subtract scalar from each comp
	K1Vector& operator-= (const K& k)
	{
	  p -= k;
	  return *this;
	}

	//! vector space multiplication with scalar 
	K1Vector& operator*= (const K& k)
	{
	  p *= k;
	  return *this;
	}

	//! vector space division by scalar
	K1Vector& operator/= (const K& k)
	{
	  p /= k;
	  return *this;
	}

	//! vector space axpy operation
	K1Vector& axpy (const K& a, const K1Vector& y)
	{
	  p += a*y.p;
	  return *this;
	}


	//===== Euclidean scalar product

	//! scalar product
    const K operator* (const K1Vector& y) const
	{
	  return p*y.p;
	}


	//===== norms

	//! one norm (sum over absolute values of entries)
    double one_norm () const
	{
	  return fvmeta_abs(p);
	}

	//! simplified one norm (uses Manhattan norm for complex values)
    double one_norm_real () const
	{
	  return fvmeta_abs_real(p);
	}

	//! two norm sqrt(sum over squared values of entries)
    double two_norm () const
	{
	  return sqrt(fvmeta_abs2(p));
	}

	//! square of two norm (sum over squared values of entries), need for block recursion
    double two_norm2 () const
	{
	  return fvmeta_abs2(p);
	}

	//! infinity norm (maximum of absolute values of entries)
    double infinity_norm () const
	{
	  return fvmeta_abs(p);
	}

	//! simplified infinity norm (uses Manhattan norm for complex values)
	double infinity_norm_real () const
	{
	  return fvmeta_abs_real(p);
	}


	//===== sizes

	//! number of blocks in the vector (are of size 1 here)
	int N () const
	{
	  return 1;
	}

	//! dimension of the vector space (==1)
	int dim () const
	{
	  return 1;
	}

	//===== conversion operator

      /** \brief Conversion operator */
	operator K () {return p;}

      /** \brief Const conversion operator */
	operator K () const {return p;}

  private:
	// the data
	K p; 
  };
#endif // USE_DEPRECATED_K1
  
  // forward declarations
  template<class K, int n, int m> class FieldMatrix;

  /**! Vectors containing only one component
   */
  template<class K>
  class FieldVector<K,1>
  {
    enum { n = 1 };
  public:
	friend class FieldMatrix<K,1,1>;

	//===== type definitions and constants

	//! export the type representing the field
	typedef K field_type;

	//! export the type representing the components
	typedef K block_type;

	//! We are at the leaf of the block recursion
	enum {blocklevel = 1};

	//! export size
	enum {size = 1};

	//===== construction

      /** \brief Default constructor */
	FieldVector ()
	{       }

      /** \brief Constructor with a given scalar */
	FieldVector (const K& k)
	{
	  p = k;
	}

      /** \brief Assignment from the base type */
	FieldVector& operator= (const K& k)
	{
	  p = k;
	  return *this;   
	}

	//===== access to components

	//! random access 
	K& operator[] (int i)
	{
#ifdef DUNE_ISTL_WITH_CHECKING
	  if (i != 0) DUNE_THROW(MathError,"index out of range");
#endif
	  return p;
	}

	//! same for read only access
	const K& operator[] (int i) const
	{
#ifdef DUNE_ISTL_WITH_CHECKING
	  if (i != 0) DUNE_THROW(MathError,"index out of range");
#endif
	  return p;
	}

	//! Iterator class for sequential access
    typedef Dune::GenericIterator<FieldVector<K,n>,K> Iterator;
	//! typedef for stl compliant access
    typedef Iterator iterator;
    
	//! begin iterator
	Iterator begin ()
	{
	  return Iterator(*this,0);
	}

	//! end iterator
	Iterator end ()
	{
	  return Iterator(*this,n);
	}

	//! begin iterator
	Iterator rbegin ()
	{
	  return Iterator(*this,n-1);
	}

	//! end iterator
	Iterator rend ()
	{
	  return Iterator(*this,-1);
	}

	//! return iterator to given element or end()
	Iterator find (int i)
	{
	  if (i>=0 && i<n)
		return Iterator(*this,i);
	  else
		return Iterator(*this,n);
	}

	//! ConstIterator class for sequential access
    typedef Dune::GenericIterator<const FieldVector<K,n>,const K> ConstIterator;
	//! typedef for stl compliant access
    typedef ConstIterator const_iterator;

	//! begin ConstIterator
	ConstIterator begin () const
	{
	  return ConstIterator(*this,0);
	}

	//! end ConstIterator
	ConstIterator end () const
	{
	  return ConstIterator(*this,n);
	}

	//! begin ConstIterator
	ConstIterator rbegin () const
	{
	  return ConstIterator(*this,n-1);
	}

	//! end ConstIterator
	ConstIterator rend () const
	{
	  return ConstIterator(*this,-1);
	}

	//! return iterator to given element or end()
	ConstIterator find (int i) const
	{
	  if (i>=0 && i<n)
		return ConstIterator(*this,i);
	  else
		return ConstIterator(*this,n);
	}
	//===== vector space arithmetic

	//! vector space addition
	FieldVector& operator+= (const FieldVector& y)
	{
	  p += y.p;
	  return *this;
	}

	//! vector space subtraction
	FieldVector& operator-= (const FieldVector& y)
	{
	  p -= y.p;
	  return *this;
	}

	//! vector space add scalar to each comp
	FieldVector& operator+= (const K& k)
	{
	  p += k;
	  return *this;
	}

	//! vector space subtract scalar from each comp
	FieldVector& operator-= (const K& k)
	{
	  p -= k;
	  return *this;
	}

	//! vector space multiplication with scalar 
	FieldVector& operator*= (const K& k)
	{
	  p *= k;
	  return *this;
	}

	//! vector space division by scalar
	FieldVector& operator/= (const K& k)
	{
	  p /= k;
	  return *this;
	}

	//! vector space axpy operation
	FieldVector& axpy (const K& a, const FieldVector& y)
	{
	  p += a*y.p;
	  return *this;
	}


	//===== Euclidean scalar product

	//! scalar product
    const K operator* (const FieldVector& y) const
	{
	  return p*y.p;
	}


	//===== norms

	//! one norm (sum over absolute values of entries)
    double one_norm () const
	{
	  return fvmeta_abs(p);
	}

	//! simplified one norm (uses Manhattan norm for complex values)
    double one_norm_real () const
	{
	  return fvmeta_abs_real(p);
	}

	//! two norm sqrt(sum over squared values of entries)
    double two_norm () const
	{
	  return sqrt(fvmeta_abs2(p));
	}

	//! square of two norm (sum over squared values of entries), need for block recursion
    double two_norm2 () const
	{
	  return fvmeta_abs2(p);
	}

	//! infinity norm (maximum of absolute values of entries)
    double infinity_norm () const
	{
	  return fvmeta_abs(p);
	}

	//! simplified infinity norm (uses Manhattan norm for complex values)
	double infinity_norm_real () const
	{
	  return fvmeta_abs_real(p);
	}


	//===== sizes

	//! number of blocks in the vector (are of size 1 here)
	int N () const
	{
	  return 1;
	}

	//! dimension of the vector space (==1)
	int dim () const
	{
	  return 1;
	}

    //! Send vector to output stream
	void print (std::ostream& s) const
	{
		  s << p;
	}
	//===== conversion operator

      /** \brief Conversion operator */
	operator K () {return p;}

      /** \brief Const conversion operator */
	operator K () const {return p;}

  private:
	// the data
	K p; 
  };

  /** @} end documentation */

} // end namespace

#endif
