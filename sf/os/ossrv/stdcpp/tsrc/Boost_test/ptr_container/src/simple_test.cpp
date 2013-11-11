//
// Boost.Pointer Container
//
//  Copyright Thorsten Ottosen 2003-2005. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/ptr_container/
//

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

using namespace std;
                       
//
// A simple polymorphic class
//
class Poly
{
    int        i_;
    static int cnt_;
    
public:
    Poly() : i_( cnt_++ )  { }
    virtual ~Poly()        { }
    void foo()             { doFoo(); }
    
private:    
    virtual void doFoo()   { ++i_; }
    
public:
    friend inline bool operator>( const Poly& l, const Poly r )
    {
        return l.i_ > r.i_;
    }
};

int Poly::cnt_ = 0;

//
// Normally we need something like this to compare pointers to objects
//
template< typename T >
struct sptr_greater
{
    bool operator()( const boost::shared_ptr<T>& l, const boost::shared_ptr<T>& r ) const
    {
        return *l > *r;
    }
};
                                                         
//
// one doesn't need to introduce new names or live with long ones
//                                                         
typedef boost::shared_ptr<Poly> PolyPtr;


void simple_test()
{
    enum { size = 2000 };
    typedef vector<PolyPtr>          vector_t;
    typedef boost::ptr_vector<Poly>  ptr_vector_t;
    vector_t                         svec;   
    ptr_vector_t                     pvec;
    
    for( int i = 0; i < size; ++i ) 
    {
        svec.push_back( PolyPtr( new Poly ) ); 
        pvec.push_back( new Poly );  // no extra syntax      
    }
                   
    for( int i = 0; i < size; ++i )
    {
        svec[i]->foo();
        pvec[i].foo(); // automatic indirection
        svec[i] = PolyPtr( new Poly );
        pvec.replace( i, new Poly ); // direct pointer assignment not possible, original element is deleted
    }
    
    for( vector_t::iterator i = svec.begin(); i != svec.end(); ++i )
        (*i)->foo();
 
    for( ptr_vector_t::iterator i = pvec.begin(); i != pvec.end(); ++i )
        i->foo(); // automatic indirection
}
 
 
int main()
{
    
    simple_test();
    #ifdef __SYMBIAN32__
		std_log(LOG_FILENAME_LINE,"[Test Case for Simple test]");
		std_log(LOG_FILENAME_LINE,"Result : Passed"); 
   		std_log(LOG_FILENAME_LINE,"[End Test Case ]");
       	testResultXml("Simple_Test");
		close_log_file();
    #endif
	
    return 0;
}




    
