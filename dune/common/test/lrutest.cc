#include <assert.h>
#include <iostream>
#include <dune/common/lru.hh>

void lru_test()
{
    std::cout << "testing Dune::lru<int,double>\n";
    
    Dune::lru<int, double> lru;
    lru.insert(10, 1.0);
    assert(lru.front() == lru.back());
    lru.insert(11, 2.0);
    assert(lru.front() == 2.0 && lru.back() == 1.0);
    lru.insert(12, 99);
    lru.insert(13, 1.3);
    lru.insert(14, 12345);
    lru.insert(15, -17);
    assert(lru.front() == -17 && lru.back() == 1.0);
    // update
    lru.insert(10);
    assert(lru.front() == 1.0 && lru.back() == 2.0);
    // update
    lru.touch(13);
    assert(lru.front() == 1.3 && lru.back() == 2.0);
    // remove item
    lru.pop_front();
    assert(lru.front() == 1.0 && lru.back() == 2.0);
    // remove item
    lru.pop_back();
    assert(lru.front() == 1.0 && lru.back() == 99);

    std::cout << "... passed\n";
}

int main ()
{
    lru_test();

    return 0;
}
