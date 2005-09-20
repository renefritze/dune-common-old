#include <iostream>
#include <fstream>
#include <dune/common/fvector.hh>
#include <dune/common/timer.hh>
#include <dune/istl/bvector.hh>
#include <dune/istl/io.hh>
#include <dune/common/iteratorfacades.hh>

#ifdef DUNE_EXPRESSIONTEMPLATES
Indent INDENT;
#endif

template<int bs, int sz>
void timing_vector()
{
  std::cout << "timing_vector<" << bs << ", " << sz << ">\n";
  typedef Dune::FieldVector<double,bs> VB;
  typedef Dune::BlockVector<VB> BV;
  typedef Dune::BlockVector<BV> BBV;
  BV bv1(sz), bv2(sz);
  bv1 = 1;
  bv2 = 0;
  bv2[1][0]=1;
  bv2[1][1]=2;

  BBV bbv(2);
  bbv[0].resize(bv1.N());
  bbv[1].resize(bv2.N());

  Dune::Timer stopwatch;
  stopwatch.reset();
  for (int i=0; i<100; i++) bbv *= 2;
  std::cout << "Time [bbv*=2] " << stopwatch.elapsed() << std::endl;
}

//template<int BlockSize, int N, int M>
template<int BN, int BM, int N, int M>
void timing_matrix()
{
  std::cout << "timing_matrix<" << BN << ", " << BM << ", "
            << N << ", " << M << ">\n";

  typedef double matvec_t;
  typedef Dune::FieldVector<matvec_t,BN> LVB;
  typedef Dune::FieldVector<matvec_t,BM> VB;
  typedef Dune::FieldMatrix<matvec_t,BN,BM> MB;
  typedef Dune::BlockVector<LVB> LeftVector;
  typedef Dune::BlockVector<VB> Vector;
  typedef Dune::BCRSMatrix<MB> Matrix;

  Matrix A(N,M,Matrix::row_wise);
  typename Matrix::CreateIterator i=A.createbegin();
  typename Matrix::CreateIterator end=A.createend();
  std::cout << "Building matrix structure\n";
  // build up the matrix structure
  int c=0;
  for (; i!=end; ++i)
  {
    // insert a non zero entry for myself
    i.insert(c);
    // insert index M-1
    i.insert(M-1);
    c++;
  }
  std::cout << "...done\n";

  LeftVector v(N);
  v = 0;
  Vector x(M);
  x = 1;
  
  Dune::Timer stopwatch;
  stopwatch.reset();
#ifdef DUNE_EXPRESSIONTEMPLATES
  v += A * x;
#else
  A.umv(x,v);
#endif
  std::cout << "Time [v+=A*x] " << stopwatch.elapsed() << std::endl;

  std::cout << std::endl;
}

int main ()
{
#ifdef DUNE_EXPRESSIONTEMPLATES
  std::cout << "Expression Templates\n";
#else
  std::cout << "Old Version\n";
#endif

  timing_vector<1,1000000>();
  timing_vector<10,100000>();
  timing_vector<100,10000>();

//   timing_matrix<150,150,500,4000>();
//   timing_matrix<150,150,1000,2000>();
  timing_matrix<1,18,400000,500000>();
//   timing_matrix<6,3,400000,500000>();
//   timing_matrix<3,6,400000,500000>();
//   timing_matrix<18,1,400000,500000>();
//   timing_matrix<50,50,9000,10000>();
  
  std::cout << std::endl;
}
