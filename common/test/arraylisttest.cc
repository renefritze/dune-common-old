#include<dune/common/arraylist.hh>
#include<iostream>
#include<cstdlib>
#include<algorithm>

class Double{
public:
    double val;
    Double(): val(0){}
    Double(double d): val(d){}
    Double& operator=(double d){
	val=d;
	return *this;
    }
};

bool operator<(Double a, Double b){
    return a.val<b.val;
}

template<class T, int size>
void randomizeList(Dune::ArrayList<T,size>& alist){
    using namespace Dune;
    
    srand((unsigned)time(0));

    int lowest=0, highest=1000, range=(highest-lowest)+1;
    
    for(int i=0; i < 250; i++)
	alist.push_back(T(static_cast<int>(range*(rand()/(RAND_MAX+1.0)))));
}

int testSorting(){
    using namespace Dune;
    ArrayList<double,10> alist;

    randomizeList(alist);
    std::sort(alist.begin(), alist.end());
    double last=-1;
    
    for(ArrayList<double,10>::iterator iter=alist.begin(), end=alist.end();
	iter != end; ++iter){
	if((*iter)>=last){
	    last=*iter;
	}else{
	    std::cerr << last<<">"<<(*iter)<<" List is not sorted! "<<__FILE__ <<":"<<__LINE__<<std::endl;
	    return 1;
	}
    }
    return 0;
}

template<int size>
void initConsecutive(Dune::ArrayList<double,size>& alist){
    using namespace Dune;

    for(int i=0; i < 100; i++)
	alist.push_back(i);
}

int testRandomAccess(){
    using namespace Dune;
    ArrayList<double,10> alist;
    initConsecutive(alist);

    ArrayList<double,10>::iterator iter=alist.begin();


    for(int i=0; i < 100; i++){
	if(iter[i]!=i){
	    std::cerr << "Random Access failed: "<<iter[i]<<"!="<<i<<" "<< __FILE__ <<":"<<__LINE__<<std::endl;
	    return 1;
	}
	
	if(*(iter+i)!=i){
	    std::cerr << "Random Access failed "<< __FILE__ <<":"<<__LINE__<<std::endl;
	    return 1;
	}
    }
    return 0;
}

int testComparison(){
    using namespace Dune;
    ArrayList<double,10> alist;
    initConsecutive(alist);

    ArrayList<double,10>::iterator iter=alist.begin(), iter1=alist.begin();
    iter1=5+iter;
    iter1=iter-5;
    iter1=5-iter;
    iter1=iter+5;
    

    if(!(iter<iter1)){
	std::cerr<<*iter<<">="<<*iter1<<" Operator< seems to be wrong! "<< __FILE__ <<__LINE__<<std::endl;
	return 1;
    }

    if(!(iter1>iter)){
	std::cerr<<"operator> seems to be wrong! "<< __FILE__ <<__LINE__<<std::endl;
	return 1;
    }
    
    if(!(iter<=iter1)){
	std::cerr<<"operator<= seems to be wrong! "<< __FILE__ <<__LINE__<<std::endl;
	return 1;
    }

    if(!(iter1>=iter)){
	std::cerr<<"operator>= seems to be wrong! "<< __FILE__ <<__LINE__<<std::endl;
	return 1;
    }

    if(!(iter1 != iter)){
	std::cerr<<"operator!= seems to be wrong! "<< __FILE__ <<__LINE__<<std::endl;
	return 1;
    }
    
    if(!(iter1 == iter+5)){
	std::cerr<<"operator== seems to be wrong! "<< __FILE__ <<__LINE__<<std::endl;
	return 1;
    }
    return 0;
}

	   
int main(){
    using namespace Dune;
    using namespace std;

    int ret=0;

    if(0==testComparison())
	cout << "Comparison is OK."<<endl;
    else{
	ret++;
	cerr<< "Comparison failed!"<<endl;
    }

    if(0==testRandomAccess())
	cout << "Radom Access is OK."<<endl;
    else{
	ret++;
	cerr<< "Ransom Access failed!"<<endl;
    }

    if(0==testSorting())
	cout << "Sorting is OK."<<endl;
    else{
	ret++;
	cerr<< "Sorting failed!"<<endl;
    }

    exit(ret);

}
