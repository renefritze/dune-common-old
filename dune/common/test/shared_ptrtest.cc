// $Id$
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include<dune/common/shared_ptr.hh>

#include<cassert>
#include<iostream>
#include<vector>
#include<cstdlib>

template<class T>
class Deleter
{
public:
    Deleter(bool& deleted) :
        deleted_(deleted)
    {}

    void operator() (T* p) const
    {
        delete p;
        deleted_ = true;
    }

private:
    bool& deleted_;
};

int main(){
    using namespace Dune;
    int ret=0;
    {
        // test default constructor
        shared_ptr<int> foo;

        // test cast-to-bool
        if (foo) {
            std::cout << "Default constructor doesn't create a NULL pointer!" << std::endl;
            ret=1;
        }

        // test custom deleter
        bool deleted = false;
        {
            shared_ptr<int> foo(new int(1), Deleter<int>(deleted));

            //test if deleter is called
            deleted = false;
            foo.reset(new int(2));  // this should call the deleter in the constructor
            if (not(deleted))
            {
                std::cout << "Custom deleter not called!" << std::endl;
                ret=1;
            }

            //test if old deleter is not called
            deleted = false;
            foo.reset();  // this should call no deleter
            if (deleted)
            {
                std::cout << "Old deleter was called!" << std::endl;
                ret=1;
            }

            //test if old deleter is not called
            deleted = false;
            foo.reset(new int(3), Deleter<int>(deleted));  // this should call no deleter
            if (deleted)
            {
                std::cout << "Old deleter was called!" << std::endl;
                ret=1;
            }
            // going out of scope should call the deleter
        }
        if (not(deleted))
        {
            std::cout << "Custom deleter not called!" << std::endl;
            ret=1;
        }
        {
            shared_ptr<int> foo(new int(1), Deleter<int>(deleted));

            foo.reset(new int(4));  // this should call the deleter...

            deleted = false;
            // ... but going out of scope should call no deleter
        }
        if (deleted)
        {
            std::cout << "1Old deleter was called!" << std::endl;
            ret=1;
        }

	// test constructor from a given pointer
        shared_ptr<double> bar(new double(43.0));
        assert(bar);

        // test reset()
        bar.reset();
        assert(!bar);

        // test get() for empty shared_ptr
        assert(!bar.get());

        // test reset(T*)
        double* p = new double(44.0);
        bar.reset(p);
        assert(bar);
        assert(bar.use_count()==1);
        
        // test get()
        double* barPtr = bar.get();
        assert(barPtr==p);

	// test constructor from a given pointer
        shared_ptr<double> b(new double(42.0));
	{
	    shared_ptr<double> d(b);
	    *b = 7;
	}

	if(b.use_count()!=1){
	    std::cout << "Reference count is wrong! "<<__LINE__<<":"<<
		__FILE__<<std::endl;
	    ret=1;
	}
	{
	    shared_ptr<double> c(b);
	
	    if(*b!=*c){
		std::cerr<<"References do not match! "<<__LINE__<<":"<<
		    __FILE__<<std::endl;
		ret=1;
	    }
	    if(b.use_count()!=2 || c.use_count()!=2){
		std::cout << "Reference count is wrong! "<<__LINE__<<":"<<
		    __FILE__<<std::endl;
		ret=1;
	    }
	    *b=8;
	    if(*b!=8 || *c!=8){
		std::cout<<"Assigning new value failed! "<<__LINE__<<":"<<
		    __FILE__<<std::endl;
		ret=1;
	    }
	}

	if(b.use_count()!=1){
	    std::cout << "Reference count is wrong! "<<__LINE__<<":"<<
		__FILE__<<std::endl;
	    ret=1;
	}
    }
    return (ret);
}
