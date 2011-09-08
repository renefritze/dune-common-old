#include <config.h>
#include <dune/common/dynvector.hh>

int main(int argc, char * argv[])
{

  Dune::DynamicVector<double> one(1);
  Dune::DynamicVector<double> two(2);

  two = one;
  
  return 0;
}
