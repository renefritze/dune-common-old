#include <config.h>

#include <iostream>
#include <dune/common/fvector.hh>
#include <dune/common/fassign.hh>

using namespace Dune;

int main(int argc, char** argv) {
  Dune::FieldVector<double,3> pos;
  
  pos <<= 1, 0, 0;

  Dune::FieldVector<float,FVSIZE> pos2(pos);
}
