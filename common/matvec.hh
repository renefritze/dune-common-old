#ifndef __MATVEC_HH__
#define __MATVEC_HH__

#include<iostream>
#include<math.h>

namespace Dune {
/** @defgroup Common Dune Common Module

	This module contains classes for general usage in dune, such as e.g.
	(small) dense matrices and vectors or containers.

	@{
 */


//************************************************************************
/*!
  Generic vector class for short vectors in d dimensions. Used e.g. for global or local coordinates.
 */
template<int n, class T = double>
class Vec {
public:
	//! Constructor making uninizialized vector
	Vec() {}

	//! Constructor making vector from built-in array
	Vec (T* y) {for(int i=0; i<n; i++) x[i]=y[i];}

	//! Constructor making vector with one coordinate set, others zeroed
	Vec (int k, T t)
	{
		for (int i=0; i<n; i++) x[i] = 0, x[k] = t;
	}
 
	//! Constructor making unit vector in direction k
	Vec (int k)
	{
		for (int i=0; i<n; i++) x[i] = 0, x[k] = 1;
	}
 
	//! Constructor making vector with identical coordinates
	Vec (T t)
	{
		for (int i=0; i<n; i++) x[i] = t;
	}
 
	//! assign component type to all components
	Vec<n,T>& operator= (T t)
	{
		for (int i=0; i<n; i++) x[i]=t;
		return *this;
	}

	//! operator () for read/write access to element of the vector
	T& operator() (int i) {return x[i];}

	//! operator+ adds two vectors
	Vec<n,T> operator+ (const Vec<n,T>& b)
	{
		Vec<n,T> z; for (int i=0; i<n; i++) z.x[i] = x[i]+b.x[i]; return z;
	}

	//! operator- binary minus
	Vec<n,T> operator- (const Vec<n,T>& b)
	{
		Vec<n,T> z; for (int i=0; i<n; i++) z.x[i] = x[i]-b.x[i]; return z;
	}

	//! scalar product of two vectors with operator* 
	T operator* (const Vec<n,T>& b)
	{
		T s=0; for (int i=0; i<n; i++) s += x[i]*b.x[i]; return s;
	}

	//! multiplication of vector with scalar
	T operator* (T k)
	{
		Vec<n,T> z; for (int i=0; i<n; i++) z.x[i] =  k*x[i]; return s;
	}

	//! 1 norm
	T norm1 ()
	{
		T s=0.0;
		for (int i=0; i<n; i++) s += ABS(x[i]);
		return s;
	}

	//! 2 norm
	T norm2 ()
	{
		T s=0.0;
		for (int i=0; i<n; i++) s += x[i]*x[i];
		return sqrt(s);
	}

	//! \infty norm
	T norminfty ()
	{
		T s=0.0;
		for (int i=0; i<n; i++)
			if (ABS(x[i])>s) s = ABS(x[i]);
		return s;
	}

	void print (std::ostream& s, int indent)
	{
		for (int k=0; k<indent; k++) s << " ";
		s << "Vec [ ";
		for (int i=0; i<n; i++) s << v(i) << " ";
		s << "]" << endl;
	}

private:
	//! built-in array to hold the data.
	T x[n];
};

//! multiplication of scalar with vector
template<int n, class T> 
inline Vec<n,T> operator* (T k, Vec<n,T> b)
{
	Vec<n,T> z; for (int i=0; i<n; i++) z(i) = k*b(i); return z;
}

//! unary minus
template<int n, class T> 
inline Vec<n,T> operator- (Vec<n,T> b)
{
	Vec<n,T> z; for (int i=0; i<n; i++) z(i) = -b(i); return z;
}

template <int n, class T>
inline std::ostream& operator<< (std::ostream& s, Vec<n,T>& v)
{
	s << "Vec [ ";
    for (int i=0; i<n; i++) s << v(i) << " ";
    s << "]" << endl;
    return s;
}

//************************************************************************
/*!
  Generic vector class for short vectors in d dimensions. Used e.g. for global or local coordinates.
 */
template<int n, int m, class T = double>
class Mat {
public:
	//! Constructor making uninizialized matrix
	Mat() {}

	//! operator () for read/write access to element in matrix
	T& operator() (int i, int j) {return a[j](i);}

	//! operator () for read/write access to column vector
	Vec<n,T>& operator() (int j) {return a[j];}

	//! matrix/vector multiplication
	Vec<n,T> operator* (const Vec<m,T>& x)
	{
		Vec<n,T> z(0.0);
		for (j=0; j<m; j++)
			for (i=0; i<n; i++) z(i) += a[j](i)*x(j);
		return z;
	}

	void print (std::ostream& s, int indent)
	{
		for (int k=0; k<indent; k++) " ";
		s << "Mat [n=" << n << ",m=" << m << "]" << endl;
		for (int i=0; i<n; i++)
		{
			for (int k=0; k<indent+2; k++) s << " ";
			s << "row i=" << i << " [ ";
			for (int j=0; j<n; j++) s << A(i,j) << " ";
			s << "]" << endl;
		}
	}

private:
	//! built-in array to hold the data
	Vec<n,T> a[m];
};

template <int n, int m, class T>
inline std::ostream& operator<< (std::ostream& s, Mat<n,m,T>& A)
{
    for (int i=0; i<n; i++)
	{
		s << "Mat i=" << i << " [ ";
		for (int j=0; j<n; j++) s << A(i,j) << " ";
		s << "]" << endl;
	}
    return s;
}

/** @} */

}


#endif










