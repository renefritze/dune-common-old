#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include <dune/common/timer.hh>
#include <dune/common/smallobject.hh>
#include <dune/common/poolallocator.hh>

using namespace Dune;

class A
{
  int a_;

public:
  A( int a )
  : a_( a )
  {}
};


class B
: public SmallObject
{
  int b_;

public:
  B( int b )
  : b_( b )
  {}
};


int main ( int argc, char **argv )
{
  Timer timer;

  const unsigned long iterations = 1 << 7;
  const unsigned long factor = 16;
  std :: cout << "Performing " << (factor*iterations) << " iterations." << std :: endl;

  timer.reset();
  for( unsigned long i = 0; i < iterations; ++i )
  {
    A *a = new A( (int)i );
    delete a;
  }
  double timeA = factor*timer.elapsed();
  std :: cout << "Time without SmallObject: " << timeA << std :: endl;

  timer.reset();
  for( unsigned long i = 0; i < factor*iterations; ++i )
  {
    B *b = new B( (int)i );
    delete b;
  } 
  double timeB = timer.elapsed();
  std :: cout << "Time with SmallObject: " << timeB << std :: endl;
  std :: cout << "Result: SmallObject is " << (timeA / timeB) << " times faster." << std :: endl;

  timer.reset();
  SmallObjectAllocator< A > alloc;
  for( unsigned long i = 0; i < factor*iterations; ++i )
  {
    A *a = alloc.allocate( 1 );
    alloc.construct( a, A( int( i ) ) );
    alloc.destroy( a );
    alloc.deallocate( a, 1 );
  }
  double timeC = timer.elapsed();
  std :: cout << "Time with SmallObjectAllocator: " << timeC << std :: endl;
  std :: cout << "Result: SmallObject is " << (timeA / timeC) << " times faster." << std :: endl;

  timer.reset();
  PoolAllocator< A, 100 > pool;
  for( unsigned long i = 0; i < factor*iterations; ++i )
  {
    A *a = pool.allocate(1);
    pool.construct( a, A( int( i ) ) );
    pool.destroy( a );
    pool.deallocate( a, 1 );
  }
  double timeD = timer.elapsed();
  std :: cout << "Time with pool allocator: " << timeD << std :: endl;
  std :: cout << "Result: pool allocator is " << (timeA / timeD) << " times faster." << std :: endl;
  std :: cout << "Result: pool allocator is " << (timeB / timeD) << " times faster than SmallObject." << std :: endl;

  // we require a speedup due to SmallObject
  //assert((timeA / timeB) > 1.0);

  // we require the speed of the poolallocator
  // assert((timeB2 / timeB) > 1.0);
}
