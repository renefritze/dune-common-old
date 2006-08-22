#include "config.h"

#include<dune/common/bigunsignedint.hh>
#include<limits>
#include<iostream>

int main()
{
  
  std::cout<<"unsigned short: max="<<std::numeric_limits<unsigned short>::max()
	   <<" min="<<std::numeric_limits<unsigned short>::min()
	   <<" digits="<<std::numeric_limits<unsigned short>::digits<<std::endl;
  std::cout<<"int: max="<<std::numeric_limits<int>::max()<<" min="
	   <<std::numeric_limits<int>::min()<<" digits="
	   <<std::numeric_limits<int>::digits<<std::endl;
  std::cout<<"unsigned int: max="<<std::numeric_limits<unsigned int>::max()
	   <<" min="<<std::numeric_limits<unsigned int>::min()<<" digits="
	   <<std::numeric_limits<unsigned int>::digits<<" digits10="
	   <<std::numeric_limits<unsigned int>::digits10<<" radix="
	   <<std::numeric_limits<unsigned int>::radix<<" eps="
	   <<std::numeric_limits<unsigned int>::epsilon()
	   <<" round_error="
	   <<std::numeric_limits<unsigned int>::round_error()
	   <<" min_exponent="
	   <<std::numeric_limits<unsigned int>::min_exponent
	   <<" float_denorm_style="
	   <<std::numeric_limits<unsigned int>::has_denorm
	   <<" traps="<<std::numeric_limits<unsigned int>::traps
	   <<std::endl;
  std::cout<<"bigunsignedint: max="<<std::numeric_limits<Dune::bigunsignedint<32> >::max()
	   <<" min="<<std::numeric_limits<Dune::bigunsignedint<32> >::min()<<" digits="<<std::numeric_limits<Dune::bigunsignedint<32> >::digits<<std::endl;
  std::cout<<"bigunsignedint: max="<<std::numeric_limits<Dune::bigunsignedint<100> >::max()
	   <<" min="<<std::numeric_limits<Dune::bigunsignedint<100> >::min()<<" digits="<<std::numeric_limits<Dune::bigunsignedint<100> >::digits
	   <<" traps="<<std::numeric_limits<Dune::bigunsignedint<100> >::traps
	   <<std::endl;

  int a1, b1, c1;
  a1=100;
  b1=3;
  c1=a1/b1;
  std::cout<<a1<<"/"<<b1<<"="<<c1<<std::endl;

 
  Dune::bigunsignedint<100> a, b, c;
  a=100;
  b=3;
  c=a/b;
  std::cout<<a<<"/"<<b<<"="<<c<<std::endl;
  
  try{
    
  a=100;
  b=0;
  c=a/1;
  std::cout<<a1<<"/"<<b1<<"="<<c1<<std::endl;
  }
  catch(Dune::MathError e){
    std::cout<<e<<std::endl;
  }
}
