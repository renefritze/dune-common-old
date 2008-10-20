#include <iostream>
#include <dune/common/typetraits.hh>

int main ()
{
  std :: cout << "Conversion from int to double exists? "
              << Dune::Conversion< int, double > :: exists << std :: endl;

  return 0;
}
