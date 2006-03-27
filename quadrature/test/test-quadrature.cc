#include <limits>
#include <iostream>

#include <config.h>

#include <dune/quadrature/quadraturerules.hh>
#include <dune/grid/common/referenceelements.hh>

template<class ctype, int dim>
bool checkQuadrature(Dune::GeometryType t, int p)
{
  double success = true;
  double volume = 0;
  // Quadratures
  typedef Dune::QuadratureRule<ctype, dim> Quad;
  typedef typename Quad::const_iterator QuadIterator;
  const Quad & quad =
    Dune::QuadratureRules<ctype,dim>::rule(t, p);
  QuadIterator qp = quad.begin();
  QuadIterator qend = quad.end();
  for (; qp!=qend; ++qp)
  {
    volume += qp->weight();
  }
  if (std::abs(volume -
               Dune::ReferenceElements<ctype, dim>::general(t).volume())
      > 10*std::numeric_limits<double>::epsilon())
  {
    std::cerr << "Error: Quadrature for " << t << " and order=" << p
              << " does not sum to volume of RefElem" << std::endl;
    std::cerr << "\tSums to " << volume << "( RefElem.volume() = "
              << Dune::ReferenceElements<ctype, dim>::general(t).volume()
              << ")" << std::endl;
    success = false;
  }
  success = success && checkQuadrature<ctype,dim>(t, p+1);
  return success;
}

template<class ctype, int dim>
bool checkQuadrature(Dune::GeometryType t)
{
  try {
    return checkQuadrature<ctype,dim>(t, 1);
  }
  catch (Dune::NotImplemented & e) {
    std::cout << e.what() << std::endl;
  }
}

int main ()
{
  bool success = true;
  
  try {
    Dune::GeometryType cube1d(Dune::GeometryType::cube,1);
    Dune::GeometryType cube2d(Dune::GeometryType::cube,2);
    Dune::GeometryType cube3d(Dune::GeometryType::cube,3);
    
    Dune::GeometryType simplex2d(Dune::GeometryType::simplex,2);
    Dune::GeometryType simplex3d(Dune::GeometryType::simplex,3);
    
    Dune::GeometryType prism3d(Dune::GeometryType::prism,3);
    Dune::GeometryType pyramid3d(Dune::GeometryType::pyramid,3);
    
    success = success &&
      checkQuadrature<double, 1>(cube1d);
    success = success &&
      checkQuadrature<double, 2>(cube2d);
    success = success &&
      checkQuadrature<double, 3>(cube3d);

    success = success &&
      checkQuadrature<double, 2>(simplex2d);
    success = success &&
      checkQuadrature<double, 3>(simplex3d);

    success = success &&
      checkQuadrature<double, 3>(prism3d);
    success = success &&
      checkQuadrature<double, 3>(pyramid3d);
  }
  catch (Dune::Exception &e) {
    std::cerr << e << std::endl;
    return 1;
  }
  catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }
  
  return success ? 0:1;
}
