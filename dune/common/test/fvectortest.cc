#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <dune/common/fvector.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/static_assert.hh>
#include <dune/common/classname.hh>
#include <iostream>
#include <complex>
#include <typeinfo>


using Dune::FieldVector;
using std::complex;

template<class ft, class rt, int d>
struct FieldVectorMainTest
{
  FieldVectorMainTest() {
    ft a = 1;
    FieldVector<ft,d> v(1);
    FieldVector<ft,d> w(2);
    FieldVector<ft,d> z(2);
    const FieldVector<ft,d> x(z);
    a = x[0];
    bool b DUNE_UNUSED;
    rt n DUNE_UNUSED;

    std::cout << __func__ << "\t ( " << className(v) << " )" << std::endl;

    // test traits
    dune_static_assert(
      ( Dune::is_same< typename Dune::FieldTraits<
        FieldVector<ft,d> >::field_type, ft >::value ),
      "FieldTraits<FieldVector> yields wrong field_type"
      );
    dune_static_assert(
      ( Dune::is_same< typename Dune::FieldTraits<ft>::real_type, rt >::value ),
      "FieldTraits<field_type> yields wrong real_type"
      );
    dune_static_assert(
      ( Dune::is_same< typename Dune::FieldTraits<
        FieldVector<ft,d> >::real_type, rt >::value ),
      "FieldTraits<FieldVector> yields wrong real_type"
      );
    
    // Test whether the norm methods compile
    n = (w+v).two_norm();
    n = (w+v).two_norm2();
    n = (w+v).one_norm();
    n = (w+v).one_norm_real();
    n = (w+v).infinity_norm();
    n = (w+v).infinity_norm_real();

    // test op(vec,vec)
    z = v + w;
    z = v - w;
    FieldVector<ft,d> z2 DUNE_UNUSED = v + w;
    w -= v;
    w += v;

    // test op(vec,scalar)
    w +=a;
    w -= a;
    w *= a;
    w /= a;

    // test scalar product, axpy
    a = v * w;
    a = v.dot(w);
    z = v.axpy(a,w);
    
    // test comparison
    b = (w != v);
    b = (w == v);

    // assignment to vector of complex
    FieldVector< std::complex<rt> ,d> cv = v;
    cv = a;
    const FieldVector< std::complex<rt> ,d> ccv DUNE_UNUSED = x;
    
    // test istream operator
    std::stringstream s;
    for (int i=0; i<d; i++)
    {
      s << i << " ";
      v[i] = i;
    }
    s >> w;
    assert(v == w);
    
    // test container methods
    typename FieldVector<ft,d>::size_type size = FieldVector<ft,d>::dimension;
    assert(size == w.size());
  }
};


template<class ft>
struct ScalarOperatorTest
{
  ScalarOperatorTest()
  {
    ft a = 1;
    ft c = 2;
    FieldVector<ft,1> v(2);
    FieldVector<ft,1> w(2);
    bool b DUNE_UNUSED;

    std::cout << __func__ << "\t ( " << className(v) << " )" << std::endl;

    a = a * c;
    a = a + c;
    a = a / c;
    a = a - c;
      
    v = a;
    v = w = v;
    a = v;

    a = v + a;
    a = v - a;
    a = v * a;
    a += (ft)1;// make sure a!=0
    a = v / a;

    v = v + a;
    v = v - a;
    v = v * a;
    a += (ft)1;// make sure a!=0
    v = v / a;
      
    a = a + v;
    a = a - v;
    a = a * v;
    v += (ft)1;// make sure v!=0
    a = a / v;

    v = a + v;
    v = a - v;
    v = a * v;
    v += (ft)1;// make sure v!=0
    v = a / v;

    v -= v;
    v -= a;
    v += v;
    v += a;
    v *= a;
    a += (ft)1;// make sure a!=0
    v /= a;

    b = (v == a);
    b = (v != a);
    b = (a == v);
    b = (a != v);

  }
};

// scalar ordering doesn't work for complex numbers
template<class ft>
struct ScalarOrderingTest
{
  ScalarOrderingTest()
  {
    ft a = 1;
    ft c = 2;
    FieldVector<ft,1> v(2);
    FieldVector<ft,1> w(2);
    bool b DUNE_UNUSED;

    std::cout << __func__ << "\t ( " << className(v) << " )" << std::endl;

    b = (a <  c);
    b = (a <= c);
    b = (a >= c);
    b = (a >  c);
      
    b = (v == a);
    b = (v != a);
    b = (a == v);
    b = (a != v);

    b = (v <  a);
    b = (v <= a);
    b = (v >= a);
    b = (v >  a);

    b = (v <  w);
    b = (v <= w);
    b = (v >= w);
    b = (v >  w);

    b = (a <  w);
    b = (a <= w);
    b = (a >= w);
    b = (a >  w);
  }
};

// scalar ordering doesn't work for complex numbers
template <class rt, int d>
struct DotProductTest
{
  DotProductTest() {
    typedef std::complex<rt> ct;
    const rt myEps(1e-6);

    dune_static_assert(
        ( Dune::is_same< typename Dune::FieldTraits<rt>::real_type, rt>::value ),
        "DotProductTest requires real data type as template parameter!"
    );

    const ct I(0.,1.); // imaginary unit
    const FieldVector<rt,d> one(1.); // vector filled with 1
    const FieldVector<ct,d> iVec(ct(0.,1.)); // vector filled with I

    std::cout << __func__ << "\t \t ( " << Dune::className(one) << " and " << Dune::className(iVec) << ")" << std::endl;

    const bool isRealOne = Dune::is_same<typename Dune::FieldTraits<rt>::field_type,typename Dune::FieldTraits<rt>::real_type>::value;
    const bool isRealIVec = Dune::is_same<typename Dune::FieldTraits<ct>::field_type,typename Dune::FieldTraits<ct>::real_type> ::value;
    dune_static_assert(isRealOne,"1-vector expected to be real");
    dune_static_assert(!isRealIVec,"i-vector expected to be complex");

    ct result = ct();
    ct length = ct(d);


    // one^H*one should equal d
    result = dot(one,one);
    assert(std::abs(result-length)<= myEps);
    result = one.dot(one);
    assert(std::abs(result-length)<= myEps);


    // iVec^H*iVec should equal d
    result = dot(iVec,iVec);
    assert(std::abs(result-length)<= myEps);
    result = iVec.dot(iVec);
    assert(std::abs(result-length)<= myEps);


    // test that we do conjugate first argument
    result = dot(one,iVec);
    assert(std::abs(result-length*I)<= myEps);
    result = dot(one,iVec);
    assert(std::abs(result-length*I)<= myEps);


    // test that we do not conjugate second argument
    result = dot(iVec,one);
    assert(std::abs(result+length*I)<= myEps);
    result = iVec.dot(one);
    assert(std::abs(result+length*I)<= myEps);


    // test that dotT does not conjugate at all
    result = dotT(one,one) + one*one;
    assert(std::abs(result-ct(2)*length)<= myEps);
    result = dotT(iVec,iVec) + iVec*iVec;
    assert(std::abs(result+ct(2)*length)<= myEps);
    result = dotT(one,iVec) + one*iVec;
    assert(std::abs(result-ct(2)*length*I)<= myEps);
    result = dotT(iVec,one) + iVec*one;
    assert(std::abs(result-ct(2)*length*I)<= myEps);

  }

};

template<class ft, int d>
struct FieldVectorTest
{
  FieldVectorTest()
  {
    // --- test complex and real valued vectors
    FieldVectorMainTest<ft,ft,d>();
    FieldVectorMainTest<complex<ft>,ft,d>();
    DotProductTest<ft,d>();
    // --- test next lower dimension
    FieldVectorTest<ft,d-1>();
  }
};

// specialization for 1d vector
template<class ft>
class FieldVectorTest<ft,1>
{
public:
  FieldVectorTest()
  {
    // --- real valued
    FieldVectorMainTest<ft,ft,1>();
    ScalarOperatorTest<ft>();
    ScalarOrderingTest<ft>();
    DotProductTest<ft,1>();
    // --- complex valued
    FieldVectorMainTest<complex<ft>,ft,1>();
    ScalarOperatorTest< complex<ft> >();
    // ordering doesn't work for complex numbers

    // --- test next lower dimension
    FieldVectorMainTest<ft,ft,0>();
  }
};

int main()
{
  try {
    FieldVectorTest<int, 3>();
    FieldVectorTest<float, 3>();
    FieldVectorTest<double, 3>();
  } catch (Dune::Exception& e) {
    std::cerr << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }
}
