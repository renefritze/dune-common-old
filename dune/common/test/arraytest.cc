// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set ts=8 sw=2 et sts=2:

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstddef>
#include <iostream>
#include <ostream>

#include <dune/common/array.hh>
#include <dune/common/classname.hh>
#include <dune/common/fvector.hh>

template<class T, std::size_t n>
void f(const Dune::array<T, n> &a) {
  std::cout << "Got a " << Dune::className(a) << " with elements";
  for(std::size_t i = 0; i < n; ++i)
    std::cout << " (" << a[i] << ")";
  std::cout << std::endl;
}

int main() {
  // check that make_array works
  f(Dune::make_array(1, 2));

  Dune::FieldVector<double, 2> x(0);
  f(Dune::make_array(x, x));
}
