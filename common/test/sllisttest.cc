#include<dune/common/sllist.hh>
#include<dune/common/test/iteratortest.hh>
#include<dune/common/poolallocator.hh>
#include<iostream>

class DoubleWrapper
{
public:
  DoubleWrapper(double b) 
    : d(b)
  {}
  
  DoubleWrapper()
    : d()
  {}
  
  operator double()
  {
    return d;  
  }
  
private:
  double d;
};


template<typename T,class A>
void randomizeListBack(Dune::SLList<T,A>& alist){
    using namespace Dune;
    
    srand((unsigned)time(0));

    int lowest=0, highest=1000, range=(highest-lowest)+1;
    
    for(int i=0; i < 10; i++)
	alist.push_back(T(range*(rand()/(RAND_MAX+1.0))));
}

template<typename T,class A>
void randomizeListFront(Dune::SLList<T,A>& alist){
    using namespace Dune;
    
    srand((unsigned)time(0));

    int lowest=0, highest=1000, range=(highest-lowest)+1;
    
    for(int i=0; i < 10; i++)
	alist.push_back((range*(rand()/(RAND_MAX+1.0))));
}

int testDeleteNext()
{
  typedef Dune::SLList<int,Dune::PoolAllocator<int,8*1024-16> > List;
  List alist;
  
  alist.push_back(3);
  alist.push_back(4);
  alist.push_back(5);
  
  List::iterator iter=alist.oneBeforeBegin();
  iter.deleteNext();
  List::iterator iter1=iter;
  ++iter1;
  if(*(alist.begin())!=4){
    std::cerr<<"delete next on position before head failed!"<<std::endl;
    return 1;
  }
  if(*iter1!=4){
    std::cerr<<"delete next failed"<<std::endl;
    return 1;
  }
  ++iter;
  iter.deleteNext();
  ++iter;
  if(iter!=alist.end()){
    std::cerr<<"delete next faild"<<std::endl;
    return 1;
  }
  if(*(alist.tail())!=4){
    std::cerr<<"delete before tail did not change tail!"<<std::endl;
  }
  
  return 0;
}

int testInsertAfter()
{
  typedef Dune::SLList<int,Dune::PoolAllocator<int,8*1024-16> > List;
  List alist;
  
  alist.push_back(3);
  List::iterator iter=alist.begin();
  iter.insertAfter(5);
  int ret=0;
  
  if(*iter!=3){
    std::cerr<<"Value at current position changed due to insertAfter"<<std::endl;
    ret++;
  }
  ++iter;
  if(iter==alist.end() || *iter!=5){
    std::cerr<<"Insertion failed!"<<std::endl;
    ++ret;
  }

  iter=alist.oneBeforeBegin();
  iter.insertAfter(5);
  ++iter;
  if(iter==alist.end() || *iter!=5){
    std::cerr<<"Insertion failed!"<<std::endl;
    ++ret;
  }
  if(*(alist.begin())!=5){
    std::cerr<<"Insert after at onebeforeBegin did not change head!"<<std::endl;
    ++ret;
  }
  iter = alist.tail();
  iter.insertAfter(20);
  ++iter;
  if(iter == alist.end() || *iter != 20){
    std::cerr<<"Insertion failed!"<<std::endl;
    ++ret;
  }

  if(*(alist.tail())!=20){
    std::cerr<<"tail was not changed!!"<<std::endl;
    ++ret;
  }
    
  alist.clear();
  iter=alist.oneBeforeBegin();
  iter.insertAfter(5);
  ++iter;
  if(iter==alist.end() || *iter!=5){
    std::cerr<<"Insertion failed!"<<std::endl;
    ++ret;
  }
  return ret;
}

template<typename T>
int testOneBeforeBegin(T& alist)
{
  typename T::iterator iterBefore = alist.oneBeforeBegin(),
    iter = alist.begin();
  typename T::const_iterator citerBefore = alist.oneBeforeBegin();
  
  int ret=0;
  ++iterBefore;
  ++citerBefore;
  
  if(iterBefore!=iter || &(*iterBefore) != &(*iter)){
    std::cerr<<"one before iterator incremented once should point to begin()"<<std::endl;
    ret++;
  }
  if(citerBefore!=iter || &(*citerBefore) != &(*iter)){
    std::cerr<<"one before iterator incremented once should point to begin()"<<std::endl;
    ret++;
  }
  return ret;
}

  
int testPushPop(){
    using namespace Dune;
    int ret=0;
    
    Dune::SLList<int,PoolAllocator<int,8*1024-16> > alist;
    
    if(alist.begin() != alist.end()){
      ret++;
      std::cout<<"For empty list begin and end iterator do not match! "<<__FILE__<<":"<<__LINE__<<std::endl;
      ret++;
    }
      
    alist.push_back(1);
    if(*(alist.begin())!=1) {
      std::cout<<"Entry should be 1! Push back failed! "<<__FILE__<<":"<<__LINE__<<std::endl;
      ret++;
    }
    
    alist.push_back(2);
    if(*(alist.begin())!=1){
      ret++;
      std::cout<<"Entry should be 2! Push back failed! "<<__FILE__<<":"<<__LINE__<<std::endl;
    }
    
    alist.push_front(3);
    if(*(alist.begin())!=3){
      ret++;
      std::cout<<"Entry should be 3! Push front failed! "<<__FILE__<<":"<<__LINE__<<std::endl;
    }
    
    alist.pop_front();
    if(*(alist.begin())!=1){
      ret++;
      std::cout<<"Entry should be 1! Push back failed! "<<__FILE__<<":"<<__LINE__<<std::endl;
    }
    return ret;
}

int main()
{
  int ret=0;
  
  Dune::SLList<double> list;
  Dune::SLList<double,Dune::PoolAllocator<double, 8*1024-20> > list1;
  Dune::SLList<DoubleWrapper, Dune::PoolAllocator<DoubleWrapper, 8*1024-20> > list2;
  randomizeListBack(list1);
  randomizeListFront(list);
  randomizeListFront(list2);

  
  ret+=testIterator(list);
  ret+=testIterator(list1);
  ret+=testPushPop();
  ret+=testOneBeforeBegin(list1);
  ret+=testInsertAfter();
  ret+=testDeleteNext();
  
  list.clear();
  list1.clear();
  list2.clear();
  randomizeListBack(list);
  randomizeListFront(list1);
  exit(ret);
  
}
