#include "config.h"

#include<dune/common/test/iteratorfacadetest.hh>
#include<dune/common/test/iteratortest.hh>
#include<iostream>
#include<algorithm>
  
void randomize(TestContainer<double>& cont){
  srand(300);
  double size=1000;

  for(int i=0; i < 100; i++){
    cont[i] = (size*(rand()/(RAND_MAX+1.0)));

  }
}
void print(TestContainer<double>& cont){
  for(int i=0; i < 100; i++)
    std::cout<<cont[i]<<" ";
  std::cout<<std::endl;
}
int main(){
  // Test the TestIterator;
  TestContainer<double> container;
  randomize(container);
  //  print(container);
  std::sort(container.begin(), container.end());
  //print(container);
  
  const TestContainer<double> ccontainer(container);
  int ret=0;
  Printer<const double> print;
  ret += testIterator(container, print);
  ret += testIterator(ccontainer, print);
  exit(ret);
}
					 
