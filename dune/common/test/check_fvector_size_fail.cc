#include <config.h>
#include <dune/common/fvector.hh>

int main(int argc, char * argv[])
{

  Dune::FieldVector<double,DIM> one(1);
  Dune::FieldVector<float,2> two(2);

  one=two;

  return 0;
}
