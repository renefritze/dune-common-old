// $Id$

#include <config.h>

#include <iostream>

#include <dune/grid/sgrid.hh>

#include "femcheck.cc"

int main () {
  try {

    int n[] = { 10, 10, 10 };
    double h[] = { 1.0, 2.0, 3.0 };
    
    // extra-environment to check destruction
    {
      std::cout << std::endl << "SGrid<2,2>" << std::endl << std::endl;
      Dune::SGrid<2,2> g1(n, h);
      femCheck(g1);
    };

    {
      std::cout << std::endl << "SGrid<3,3>" << std::endl << std::endl;
      Dune::SGrid<3,3> g2(n, h);
      femCheck(g2);
    };

  } catch (Dune::Exception &e) {
    std::cerr << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }

  return 0;
};
