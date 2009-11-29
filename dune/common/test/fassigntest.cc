#include <config.h>

#include <iostream>
#include <dune/common/fvector.hh>
#include <dune/common/fassign.hh>

using namespace Dune;

int main(int argc, char** argv) try
{
Dune::FieldVector<double,3> pos;

pos <<= 1, 0, 0;

} catch (Exception e) {

std::cout << e << std::endl;

}
