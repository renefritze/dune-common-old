// Test the new (Dune) interface of float_cmp

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include <dune/common/float_cmp.hh>

using std::cout;
using std::endl;
using std::flush;

/////////////////////////
//
//  compile time checks
//

// check that we can access the functions as FloatCmp::function from within the Dune namespace
namespace Dune {
  void checkNamespaceAccess() {
    FloatCmp::eq(0.0, 0.0);
  }
} // namespace Dune
// check that we can access the functions as FloatCmp::function with using namespace Dune
void checkUsingAccess() {
  using namespace Dune;
  FloatCmp::eq(0.0, 0.0);
}

// run time checks
const char* repr(bool b) {
  if(b) return "true ";
  else  return "false";
}

int passed = 0;
int failed = 0;

void count(bool pass) {
  if(pass) { cout << "passed"; ++passed; }
  else     { cout << "failed"; ++failed; }
}

template<typename F>
void tests(F f1, F f2, typename Dune::FloatCmp::EpsilonType<F>::Type eps, bool inside)
{
  bool result;

  cout << "eq(" << f1 << ", " << f2 << ", " << eps << ") = " << flush;
  result = Dune::FloatCmp::eq(f1, f2, eps);
  cout << repr(result) << "\t";
  count(result == inside);
  cout << endl;

  cout << "ge(" << f1 << ", " << f2 << ", " << eps << ") = " << flush;
  result = Dune::FloatCmp::ge(f1, f2, eps);
  cout << repr(result) << "\t";
  count(result == (inside || f1 > f2));
  cout << endl;

  cout << "le(" << f1 << ", " << f2 << ", " << eps << ") = " << flush;
  result = Dune::FloatCmp::le(f1, f2, eps);
  cout << repr(result) << "\t";
  count(result == (inside || f1 < f2));
  cout << endl;

  cout << "ne(" << f1 << ", " << f2 << ", " << eps << ") = " << flush;
  result = Dune::FloatCmp::ne(f1, f2, eps);
  cout << repr(result) << "\t";
  count(result == !inside);
  cout << endl;

  cout << "gt(" << f1 << ", " << f2 << ", " << eps << ") = " << flush;
  result = Dune::FloatCmp::gt(f1, f2, eps);
  cout << repr(result) << "\t";
  count(result == (!inside && f1 > f2));
  cout << endl;

  cout << "lt(" << f1 << ", " << f2 << ", " << eps << ") = " << flush;
  result = Dune::FloatCmp::lt(f1, f2, eps);
  cout << repr(result) << "\t";
  count(result == (!inside && f1 < f2));
  cout << endl;
}

template<typename F>
void tests(F f1, F f2, const typename Dune::FloatCmpOps<F> &ops, bool inside)
{
  bool result;
  cout << "ops = operations(" << ops.epsilon() << ")" << endl;

  cout << "ops.eq(" << f1 << ", " << f2 << ") = " << flush;
  result = ops.eq(f1, f2);
  cout << repr(result) << "\t";
  count(result == inside);
  cout << endl;

  cout << "ops.ge(" << f1 << ", " << f2 << ") = " << flush;
  result = ops.ge(f1, f2);
  cout << repr(result) << "\t";
  count(result == (inside || f1 > f2));
  cout << endl;

  cout << "ops.le(" << f1 << ", " << f2 << ") = " << flush;
  result = ops.le(f1, f2);
  cout << repr(result) << "\t";
  count(result == (inside || f1 < f2));
  cout << endl;

  cout << "ops.ne(" << f1 << ", " << f2 << ") = " << flush;
  result = ops.ne(f1, f2);
  cout << repr(result) << "\t";
  count(result == !inside);
  cout << endl;

  cout << "ops.gt(" << f1 << ", " << f2 << ") = " << flush;
  result = ops.gt(f1, f2);
  cout << repr(result) << "\t";
  count(result == (!inside && f1 > f2));
  cout << endl;

  cout << "ops.lt(" << f1 << ", " << f2 << ") = " << flush;
  result = ops.lt(f1, f2);
  cout << repr(result) << "\t";
  count(result == (!inside && f1 < f2));
  cout << endl;
}

int main() {
  cout.setf(std::ios_base::scientific, std::ios_base::floatfield);
  cout.precision(16);
  Dune::FloatCmpOps<double> ops(1e-7);

  cout << "Tests inside the epsilon environment" << endl;
  tests<double>(1.0, 1.00000001, 1e-7, true);
  tests<double>(1.0, 1.00000001, ops,  true);

  cout << "Tests outside the epsilon environment, f1 < f2" << endl;
  tests<double>(1.0, 1.000001, 1e-7, false);
  tests<double>(1.0, 1.000001, ops,  false);

  cout << "Tests outside the epsilon environment, f1 > f2" << endl;
  tests<double>(1.000001, 1.0, 1e-7, false);
  tests<double>(1.000001, 1.0, ops,  false);

  cout << "Tests with f1 = f2 = 0" << endl;
  tests<double>(0, 0, 1e-7, true);
  tests<double>(0, 0, ops,  true);

  int total = passed + failed;
  cout << passed << "/" << total << " tests passed; " << failed << "/" << total << " tests failed" << endl;
  if(failed > 0) return 1;
  else           return 0;
}
