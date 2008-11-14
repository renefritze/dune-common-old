#include <config.h>

#include <dune/common/bitsetvector.hh>
#ifdef __GNUC__
#include <ext/malloc_allocator.h>
#endif

#include<dune/common/test/iteratortest.hh>

template<class BBF>
struct ConstReferenceOp
{
    typedef typename BBF::value_type bitset;
    typedef typename BBF::const_reference const_reference;
    
    void operator()(const_reference t){
        bitset x = t[0];
    }
};

template<class BBF>
void testContainer(BBF & bbf)
{
    typedef typename BBF::value_type bitset;
    typedef typename BBF::reference reference;
    typedef typename BBF::const_reference const_reference;

    const BBF & cbbf = bbf;

    bitset x = bbf[3];
    reference y = bbf[4];
    const_reference z = bbf[4];
    const reference v = bbf[4];

    // assignement
    y = false;
    y[2] = true;
    y = x;
    y = z;
    y = v;
    x = y;
    x = z;
    x = v;
    y = cbbf[1];
    x = cbbf[1];
    bbf[4] = x;
    bbf[4] = v;
    bbf[4] = y;
    bbf[4] = true;

    // invoke methods
    x.size();
    y.size();
    z.size();
    v.size();
    
    // equality
    y == cbbf[2];
    y == bbf[3];
    y == x;
    x == y;
    x == z;
    z == x;
    z == y;
    y == z;

    // inequality
    y != cbbf[2];
    y != bbf[3];
    y != x;
    x != y;
    x != z;
    z != x;
    z != y;
    y != z;

    // flip
    y.flip();
    y.flip(2);
    y[3].flip();
}

template<class BBF>
void testConstContainer(const BBF& bbf){
    typedef typename BBF::value_type bitset;
    typedef typename BBF::iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;
    typedef typename BBF::const_reference reference;

    const BBF & cbbf = bbf;

    bitset x = bbf[3];
    value_type z;
    reference y = bbf[4];

    // assignement
    x = y;
    x = cbbf[1];

    // equality
    y == cbbf[2];
    y == bbf[3];
    y == x;
    x == y;

    // inequality
    y != cbbf[2];
    y != bbf[3];
    y != x;
    x != y;
}

template<int block_size, class Alloc>
void doTest() {
    typedef Dune::BitSetVector<block_size, Alloc> BBF;

    BBF bbf(10,true);
    const BBF & cbbf = bbf;

    // test containers and some basic bitset operations
    testContainer(bbf);
    testConstContainer(bbf);
    testConstContainer(cbbf);
    
    // iterator interface
    ConstReferenceOp<BBF> cop;
    assert(testIterator(bbf, cop) == 0);
    assert(testIterator(cbbf, cop) == 0);
}

int main()
{
    doTest<4, std::allocator<bool> >();
#ifdef __GNUC__
    doTest<4, __gnu_cxx::malloc_allocator<bool> >();
#endif
    return 0;
}
