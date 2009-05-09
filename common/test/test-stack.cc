// $Id$
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cassert>

#include <dune/common/finitestack.hh>

// stack depth to test
static const int MAX = 100;

template <class SType>
void exercise_stack (SType &S) {
  assert(S.empty());

  // fill stack to maximum
  for (int i = 0; i < MAX; ++i) {
    assert(! S.full());
    S.push(i);
    assert(! S.empty());
  };

  for (int i = MAX - 1; i >= 0; --i) {
    int x = S.top();
    int y = S.pop();
    assert(x == i);
    assert(y == i);    
  };

  assert(S.empty());
}

int main () {  
  // initialize stack, push stuff and check if it comes out again
  Dune::FiniteStack<int, MAX> fixedstack;
  exercise_stack(fixedstack);  

  // check error handling of Stack
  try {
      Dune::FiniteStack<int, MAX> stack1;

    assert(stack1.empty());
    stack1.pop();
    
    // exception has to happen
    return 1;
  } catch (Dune::RangeError &e) {
    // exception was correctly reported
    return 0;
  } catch (...) {
    // wrong type of exception
    return 1;
  }

}
