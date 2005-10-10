#ifndef DUNE_ITERATORFACADETEST_HH
#define DUNE_ITERATORFACADETEST_HH
#include<dune/common/iteratorfacades.hh>
#include<dune/common/genericiterator.hh>
#include<dune/common/typetraits.hh>

template<class T> 
class TestContainer{
public:
  typedef Dune::GenericIterator<TestContainer<T>,T> iterator;
  
  typedef Dune::GenericIterator<const TestContainer<T>,const T> const_iterator;

  TestContainer(){
    for(int i=0; i < 100; i++)
      values_[i]=i;
  }

  iterator begin(){
    return iterator(*this, 0);
  }

  const_iterator begin() const{    
    return const_iterator(*this, 0);
  }

  iterator end(){
    return iterator(*this, 100);
  }

  const_iterator end() const{    
    return const_iterator(*this, 100);
  }
  
  T& operator[](int i){
    return values_[i];
  }

  
  const T& operator[](int i) const{
    return values_[i];
  }
private:
  T values_[100];
};

#endif
