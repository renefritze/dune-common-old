// $Id$

#ifndef __DUNE_ITERATORTEST_HH__
#define __DUNE_ITERATORTEST_HH__
#include<iostream>
#include<algorithm>

/**
 * @brief Tests the capabilities of a forward iterator.
 * @param begin Iterator positioned at the stsrt.
 * @param end Iterator positioned at the end.
 * @param opt Functor for doing whatever one wants.
 */
template<class Iter, class Opt>
int testForwardIterator(Iter begin, Iter end, Opt& opt){
  //std::cout<< "forward: ";
  for(;begin!=end; ++begin)
    opt(*begin);
  //std::cout<< " OK "<<std::endl;
  return 0;
}

/**
 * @brief Tests the capabilities of a bidirectional iterator.
 *
 * Namely it test wether random positions can be reached from
 * each directions.
 *
 * @param begin Iterator positioned at the stsrt.
 * @param end Iterator positioned at the end.
 * @param opt Functor for doing whatever one wants.
 */
template<class Iter, class Opt>
int testBidirectionalIterator(Iter begin, Iter end, Opt opt){
  testForwardIterator(begin, end, opt);
  Iter tbegin=--begin;
  Iter tend=--end;
  for(;tbegin!=tend; --tend)
    opt(*tend);

  typename Iter::difference_type size = std::distance(begin, end);
  srand((unsigned)time(0));

  int no= (size>10)?10:size;

  for(int i=0; i < no; i++){
    int index = static_cast<int>(size*(rand()/(RAND_MAX+1.0)));
    int backwards=size-index;
    tbegin=begin;
    tend=end;
    for(int j=0; j < index; j++) ++tbegin;
    for(int j=0; j < backwards; j++) --tend;

    if(tbegin != tend){
      std::cerr<<"Did not reach same index by starting forward from "
	       <<"begin and backwards from end."<<std::endl;
      return 1;
    }
  }
  return 0;
}

template<class Iter, class Opt>
int testRandomAccessIterator(Iter begin, Iter end, Opt opt){
  int ret=testBidirectionalIterator(begin, end, opt);

  typename Iter::difference_type size = end-begin;

  srand((unsigned)time(0));

  int no= (size>10)?10:size;

  for(int i=0; i < no; i++){
    int index = static_cast<int>(size*(rand()/(RAND_MAX+1.0)));
    opt(begin[index]);
  }

  // Test the less than operator
  if(begin != end &&!( begin<end)){
    std::cerr<<"! (begin()<end())"<<std::endl;
    ret++;
  }
  
  for(int i=0; i < no; i++){
    int index = static_cast<int>(size*(rand()/(RAND_MAX+1.0)));
    Iter rand(begin), test(begin), res;
    rand+=index;
    
    if((res=begin+index) != rand){
      std::cerr << " i+n should have the result i+=n, where i is the "
		<<"iterator and n is the difference type!" <<std::endl;
      ret++;
    }
    for(int i=0; i< index; i++) ++test;

    if(test != rand){
      std::cerr << "i+=n should have the same result as applying the"
		<< "increment ooperator n times!"<< std::cerr;
      ret++;
    } 
    
    rand=end, test=end;
    rand-=index;

    
    if((end-index) != rand){
      std::cerr << " i-n should have the result i-=n, where i is the "
		<<"iterator and n is the difference type!" <<std::endl;
      ret++;
    }
    for(int i=0; i< index; i++) --test;

    if(test != rand){
      std::cerr << "i+=n should have the same result as applying the"
		<< "increment ooperator n times!"<< std::cerr;
      ret++;
    } 
  }

  for(int i=0; i < no; i++){
    Iter iter1 = begin+static_cast<int>(size*(rand()/(RAND_MAX+1.0)));
    Iter iter2 = begin+static_cast<int>(size*(rand()/(RAND_MAX+1.0)));
    typename Iter::difference_type diff = iter2 -iter1;
    if((iter1+diff)!=iter2){
      std::cerr<< "i+(j-i) = j should hold, where i,j are iterators!"<<std::endl;
      ret++;
    }
  }
  
  return ret;
}

template<class Iter, class Opt, typename iterator_category>
int testIterator(Iter& begin, Iter& end, Opt& opt, iterator_category cat);

template<class Iter, class Opt>
int testIterator(Iter& begin, Iter& end, Opt& opt, std::forward_iterator_tag){
  return testForwardIterator(begin, end, opt);
}

template<class Iter, class Opt>
int testIterator(Iter& begin, Iter& end, Opt& opt, std::bidirectional_iterator_tag){
  return testBidirectionalIterator(begin, end, opt);
}

template<class Iter, class Opt>
int testIterator(Iter& begin, Iter& end, Opt& opt, std::random_access_iterator_tag){
  //  std::cout << "Testing iterator ";
  int ret = testRandomAccessIterator(begin, end, opt);
  //std::cout<<std::endl;
  return ret;
}

template<class Iter, class Opt>
int testConstIterator(Iter& begin, Iter& end, Opt& opt){
  //std::cout << "Testing constant iterator: ";
  int ret=testIterator(begin, end, opt, typename std::iterator_traits<Iter>::iterator_category());
  //std::cout<<std::endl;
  return ret;
}

template<class Container, class Opt>
int testIterator(Container& c, Opt& opt){
  typename Container::iterator begin=c.begin(), end=c.end();
  typename Container::const_iterator cbegin(begin);
  typename Container::const_iterator cbegin1=begin;
  typename Container::const_iterator cend=c.end();
  int ret=0;

  if(end!=cend || cend!=end){
    std::cerr<<"constant and mutable iterators should be equal!"<<std::endl;
    ret=1;
  }
  return ret + testConstIterator(cbegin, cend, opt) +
    testIterator(begin,end,opt);
}

template<class Container, class Opt>
int testIterator(const Container& c, Opt& opt){
  typename Container::const_iterator begin=c.begin(), end=c.end();
  return testConstIterator(begin,end, opt);
}

template<class Iter, class Opt>
void testAssignment(Iter begin, Iter end, Opt& opt){
  //std::cout << "Assignment: ";
  for(;begin!=end;begin++)
    *begin=typename std::iterator_traits<Iter>::value_type();
  //std::cout<<" Done."<< std::endl;
}

template<class Iter, class Opt>
int testIterator(Iter& begin, Iter& end, Opt& opt){
  testAssignment(begin, end, opt);
  return testConstIterator(begin, end, opt);
}

#endif
