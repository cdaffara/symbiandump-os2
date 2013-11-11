/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include<iostream>
#include<algorithm>
#include<vector>
#include<iostream>
 #include<ostream>
#include<iterator>
#include<numeric>
#include <queue>
#include <deque>
#include <stack>
#include<list>
#include <string>

#include<functional>

#include <locale>
 #include <numeric>
 #include <valarray>
 #include<fstream> 
 
#include<list>
#include<string>
#include<cstring>
#include<vector>
#include<iterator>

#include<ios>
#include<algorithm>
#include <numeric>
#include<functional>
#include <locale>
#include<iostream>
#include<memory>
#include<complex>
#include<map>
//#include<set>
 #include<exception>
 // #include<bitset>

#include <stl/char_traits.h> // fpos
#include "tstl_3.h"

 
using namespace std;
 #define STDCPP_OOM FALSE// TRUE for OOM testing

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctstl_3::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctstl_3::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctstl_3::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctstl_3::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Includes", Ctstl_3::Includes ),
		ENTRY( "innerproduct", Ctstl_3::innerproduct ),
		ENTRY( "inplacemerge", Ctstl_3::inplacemerge ),
		ENTRY( "Queue", Ctstl_3::Queue ),
		ENTRY( "Pqueue", Ctstl_3::Pqueue ),
 		ENTRY( "stablesortL", Ctstl_3::stablesortL ),
 		ENTRY( "Stack", Ctstl_3::Stack ),
  		ENTRY( "string1_testL", Ctstl_3::string1_testL ),
  		 ENTRY("Setintersection", Ctstl_3::Setintersection ),
 ENTRY("Setsymdifference", Ctstl_3::Setsymdifference ),
 ENTRY("Setunion", Ctstl_3::Setunion ),
 ENTRY("HeapL", Ctstl_3::HeapL ),
 ENTRY("Swapranges", Ctstl_3::Swapranges ),
 ENTRY("Partialsums", Ctstl_3::Partialsums ),
 ENTRY("Unarynegate", Ctstl_3::Unarynegate ),
 ENTRY("Not2", Ctstl_3::Not2),
 ENTRY("GettemporarybufferL", Ctstl_3::GettemporarybufferL),
 ENTRY("Pair", Ctstl_3::Pair),
 ENTRY("Lexicographicalcompare", Ctstl_3::Lexicographicalcompare ),
 ENTRY("Mismatch", Ctstl_3::Mismatch ),
 ENTRY("Compare_fun", Ctstl_3::Compare_fun ),
 ENTRY("Copy_fun", Ctstl_3::Copy_fun ),
 ENTRY("Copybackward_fun", Ctstl_3::Copybackward_fun ),
 ENTRY("mergeL", Ctstl_3::mergeL ),
 ENTRY("allocatoradd", Ctstl_3::allocatoradd ),
 ENTRY("alldeall", Ctstl_3::alldeall ),
 ENTRY("allcons", Ctstl_3::allcons ),
 ENTRY("allrebind", Ctstl_3::allrebind ),
 ENTRY("allmaxsize", Ctstl_3::allmaxsize ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// Ctstl_3::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

bool myfunction (int i, int j) { return i<j; }


TInt Ctstl_3::Includes( CStifItemParser& aItem )
    {
int failures =0;
try
{
	

   int container1[] = {5,10,15,20,25,30,35,40,45,50};
  int container2[] = {40,30,20,10};
int container3[]={100,200,300,400,10,20,30,40};
  sort (container1,container1+10);
  sort (container2,container2+4);
sort(container3,container3+8);
/*
if(!includes(container1,container1+10,container3,container3+8))


   if ( includes(container1,container1+10,container2,container2+4) )
 
   if ( includes(container1,container1+10,container2,container2+4, myfunction) )
 
  return KErrNone;
  return KErrGeneral;
*/
 // cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 

if(includes(container1,container1+10,container3,container3+8))
failures++;


   if ( !includes(container1,container1+10,container2,container2+4) )
   failures++;
 
   if ( !includes(container1,container1+10,container2,container2+4, myfunction) )
   failures++;
 
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
// #if STDCPP_OOM
//failures++;
// #endif
    }
    
   catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    } 
   // vector<int> myvector (8,10); 

// -----------------------------------------------------------------------------
// Ctstl_3::innerproduct
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_3::innerproduct( CStifItemParser& aItem )
    {

    int failures=0;
    try
    {
    	
    
typedef vector < int > intArray;
typedef ostream_iterator < int, char, char_traits<char> >
FloatOstreamIt;

 
 
    FloatOstreamIt itOstream(cout," ");

    // Initialize the arrays
    intArray rgF1, rgF2;
    for (int i=1; i<=5; i++) {
        rgF1.push_back(i);
        rgF2.push_back(i*i);
    };

    // Print the arrays
 
    copy(rgF1.begin(),rgF1.end(),itOstream);
  
    copy(rgF2.begin(),rgF2.end(),itOstream);
   // cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
    // Compute the inner_product of the arrays.  This is the
    // sum of the products (S.O.P) of the corresponding elements

    int ip1 = inner_product(rgF1.begin(),rgF1.end(),rgF2.begin(),0);

     
 
 
 int ip2 = inner_product(rgF1.begin(),rgF1.end(),rgF2.begin(),1,multiplies<int>(),plus<int>());
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
/* if(ip1==225)
if(ip2==86400)
return KErrNone;
return KErrGeneral;*/

    // Compute the inner_product of the arrays.  This is the
    // product of the sums (P.O.S.) of the corresponding elements
   
   if(ip1!=225)
   failures++;
if(ip2!=86400)
failures++;
    
 //#if STDCPP_OOM
//failures++;
// #endif
    }


catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


 // -----------------------------------------------------------------------------
// Ctstl_3::inplacemerge
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_3::inplacemerge( CStifItemParser& aItem )
    {
int failures=0;
try
{
	

    int first[] = {5,10,15,20,25};
  int second[] = {50,40,30,20,10};
  vector<int> v(10);
  vector<int>::iterator it;
int a[10];
  sort (first,first+5);
  sort (second,second+5);

  copy (first,first+5,v.begin());
  copy (second,second+5,v.begin()+5); 
  // cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  inplace_merge(v.begin(),v.begin()+5,v.end());
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
   for (it=v.begin(); it!=v.end(); ++it)
   {
   	static int i;
   	a[i]=*it;
   	i++;
   }
 /*
   if(a[0]==5)
   if(a[1]==10)
   if(a[2]==10)
   if(a[3]==15)
   if(a[4]==20)
   if(a[5]==20)
   if(a[6]==25)
   if(a[7]==30)
   if(a[8]==40)
   if(a[9]==50)
   return KErrNone;
   return KErrGeneral;
 */
 
  if(a[0]!=5)
   if(a[1]!=10)
   if(a[2]!=10)
   if(a[3]!=15)
   if(a[4]!=20)
   if(a[5]!=20)
   if(a[6]!=25)
   if(a[7]!=30)
   if(a[8]!=40)
   if(a[9]!=50)
   failures++;
 //#if STDCPP_OOM
//failures++;
// #endif
    }

catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
// -----------------------------------------------------------------------------
// Ctstl_3::Queue
// Queue test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_3::Queue( CStifItemParser& aItem )
    {

    int failures=0;
    try
    {
    	
    
  int arr[3];
   // cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   queue<int,list<int> >  q;
   
  
   q.push(1);
   arr[0]=q.back();
   if(q.size()!=1)
   failures++;
   if(q.back()!=1)
   failures++;
   if(arr[0]!=1)
   failures++;
   
   
   
   q.push(2);
   arr[1]=q.back();
   if(q.size()!=2)
   failures++;
   if(q.back()!=2)
   failures++;
   if(arr[1]!=2)
   failures++;
   
   
   
   q.push(3);
   arr[2]=q.back();
   if(q.size()!=3)
   failures++;
   if(q.back()!=3)
   failures++;
   if(arr[2]!=3)
   failures++;
   
   
   q.pop();
   if(q.size()!=2)
   failures++;
   if(q.back()!=3)
   failures++;
   if(q.front()!=2)
   failures++;
   
   q.pop();
   q.pop();
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
   if(!q.empty())
   failures++;
   
  /* 
   if(failures)
   return KErrGeneral;
   return KErrNone;
   */
   
   
    //#if STDCPP_OOM
//failures++;
// #endif
   

    }
    
    catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
// -----------------------------------------------------------------------------
// Ctstl_3::PQueue
// PQueue test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctstl_3::Pqueue( CStifItemParser& aItem )
    {

    int failures=0;
    
    try
    {
    	
    
  int arr[3];
  
  typedef deque<int> d; 
  // cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 priority_queue<int, d, less<int> > q;

  
    
  q.push(1);
  arr[0]=q.top();
   if(q.size()!=1)
   failures++;
   if(q.top()!=1)
   failures++;
   if(arr[0]!=1)
   failures++;
   
   
   
   q.push(2);
   arr[1]=q.top();
   if(q.size()!=2)
   failures++;
   if(q.top()!=2)
   failures++;
   if(arr[1]!=2)
   failures++;
   
   
   
   q.push(3);
   arr[2]=q.top();
   if(q.size()!=3)
   failures++;
   if(q.top()!=3)
   failures++;
   if(arr[2]!=3)
   failures++;
   
   
   q.pop();
     if(q.size()!=2)
   failures++;
   
   if(q.top()!=2)
   failures++;
   
   q.pop();
   q.pop();
   
   if(!q.empty())
   failures++;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif  
  /*
   if(failures)
   return KErrGeneral;
   return KErrNone;
   
  */ 
    //#if STDCPP_OOM
//failures++;
// #endif
   
   

    }
    
  catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }  
    
    // -----------------------------------------------------------------------------
// Ctstl_3::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


bool UDgreater (int elem1, int elem2 )
{
   return elem1 > elem2;
}


TInt Ctstl_3::stablesortL( CStifItemParser& aItem )
    {
int failures=0;
try
{
	

    vector <int> v1;
   vector <int>::iterator Iter1;

   int i,a1[12],a2[12],a3[12];
   for ( i = 0 ; i <= 5 ; i++ )
   {
      v1.push_back( 2 * i );
   }

   for ( i = 0 ; i <= 5 ; i++ )
   {
      v1.push_back( 2 * i  );
   }

    // cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   stable_sort(v1.begin( ), v1.end( ) );
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
   
    for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
      
    {
    static int i;
    a1[i] = *Iter1;
    i++;	
    }
    
    
    
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   // To sort in descending order, specify binary predicate
   stable_sort(v1.begin( ), v1.end( ), greater<int>( ) );
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
    {
    	static int i;
    	a2[i]=*Iter1;
    	i++;
    }
    
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   // A user-defined (UD) binary predicate can also be used
   stable_sort(v1.begin( ), v1.end( ), UDgreater );
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
      
      
    {
    	static int i;
    	a3[i]=*Iter1;
    i++;
    }


  /* 
   if(a1[0]==0)
   if(a1[1]==0)
   if(a1[2]==2)
   if(a1[3]==2)
   if(a1[4]==4)
   if(a1[5]==4)
   if(a1[6]==6)
   if(a1[7]==6)
   if(a1[8]==8)
   if(a1[9]==8)
   if(a1[10]==10)
   if(a1[11]==10)
   if(a2[0]==a3[0]==10)   
   if(a2[1]==a3[1]==10)   
   if(a2[2]==a3[2]==8)   
   if(a2[3]==a3[3]==8)   
   if(a2[4]==a3[4]==6)   
   if(a2[5]==a3[5]==6)   
   if(a2[6]==a3[6]==4)   
   if(a2[7]==a3[7]==4)   
   if(a2[8]==a3[8]==2)   
   if(a2[9]==a3[9]==2)   
   if(a2[10]==0)
   if(a3[10]==0)   
   if(a2[11]==0 )
   if(a3[11]==0)   
   return KErrNone;
   return KErrGeneral;
   */
   
   
   if(a1[0]!=0)
   if(a1[1]!=0)
   if(a1[2]!=2)
   if(a1[3]!=2)
   if(a1[4]!=4)
   if(a1[5]!=4)
   if(a1[6]!=6)
   if(a1[7]!=6)
   if(a1[8]!=8)
   if(a1[9]!=8)
   if(a1[10]!=10)
   if(a1[11]!=10)
   if(a2[0]!=a3[0]!=10)   
   if(a2[1]!=a3[1]!=10)   
   failures++;
   
     //#if STDCPP_OOM
//failures++;
// #endif
   
    }
    
  catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }  
    
// -----------------------------------------------------------------------------
// Ctstl_3::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_3::Stack( CStifItemParser& aItem )
    {


//__UHEAP_MARK;
   int failures=0;
   try
   {
   	
     int arr[3]; 
     // cout<<"";
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   stack<int, deque<int> > s;
 
 
  s.push(1);
  if(s.size()!=1)
  failures++;
  arr[0] = s.top();
  
  s.push(2);
  if(s.size()!=2)
  failures++;
  arr[1] = s.top();
  
  s.push(3);
  if(s.size()!=3)
  failures++;
  arr[2] = s.top();
  
  
  s.pop();
  
  if(s.size()!=2)
  failures++;
  if(s.top()!=2)
  failures++;
  
  
  
  
  
   
  
  if(1!=arr[0])
    failures++;
   if(2!=arr[1])
    failures++;
   
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
  /*
  if(failures)

    return KErrGeneral;
    return KErrNone;
 */   
    //__UHEAP_MARKEND;
    
 //#if STDCPP_OOM
//failures++;
// #endif
    }


catch(std::bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



// -----------------------------------------------------------------------------
// Ctstl_3::string1_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_3::string1_testL( CStifItemParser& /*aItem*/ )
    {


//__UHEAP_MARK;
      int failures=0;
      try
      {
      	
      
   char* array = "Hello, World!";
    // cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  std::string v(array);
  int i;
   
  if(v.compare("Hello, World!"))
     failures++;
     
  v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.
  for(i = 0; i < v.size(); i++)
   
   if('H'!=v[0])
     failures++;
   else if('!'!=v[1])
     failures++;
      
  
  v.insert(1, (char*)array);
  v.erase(v.begin()); // Erase first element.
  v.erase(v.end() - 1); // Erase last element.
   if(v.compare("Hello, World!"))
    failures++;
  v.clear(); // Erase all.
  
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  /*
  if(failures)
    return KErrGeneral;
  else
    return KErrNone;
  */
   //#if STDCPP_OOM
//failures++;
// #endif
  
 // __UHEAP_MARKEND;
  
    }


catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

string func( const string& par )
{
  string tmp( par );

  return tmp;
}


//#if defined (_STLP_PTHREADS)
void *f( void * )
//#elif defined (_STLP_WIN32THREADS)
//DWORD __stdcall f (void *)
//#endif
{
  string s( "qyweyuewunfkHBUKGYUGL,wehbYGUW^(@T@H!BALWD:h^&@#*@(#:JKHWJ:CND" );

  for ( int i = 0; i < 2000000; ++i ) {
    string sx = func( s );
  }

  return KErrNone;
}





  TInt Ctstl_3::Setintersection(CStifItemParser& aItem)
  {
  	int failures=0;
  try{
 //__UHEAP_MARK;

 int first[] = {5,10,15,20,25};
   int second[] = {50,40,30,20,10};
   int output[10];
   vector<int> v(10);                           // 0  0  0  0  0  0  0  0  0  0
   vector<int>::iterator it;

   sort (first,first+5);     //  5 10 15 20 25
   sort (second,second+5);   // 10 20 30 40 50
   // cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   it=set_intersection (first, first+5, second, second+5, v.begin());
          #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif                                        // 10 20 0  0  0  0  0  0  0  0

  for(it=v.begin();it!=v.end();++it)
 {
 	
 static int i;
 output[i] = *it;
 i++;

 }
  /*
  if(output[0]==10)
   if(output[1]==20)
   if(output[2]==0)
   return KErrNone;
   return KErrGeneral;
  
  */
  
  if(output[0]!=10)
     failures++;
   if(output[1]!=20)
   failures++;
   if(output[2]!=0)
   failures++;
    
    //#if STDCPP_OOM
 //failures++;
 // #endif
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  
  
  TInt Ctstl_3::Setsymdifference(CStifItemParser& aItem)
  {int failures=0;
  	try{
  
 //__UHEAP_MARK;

 int first[] = {5,10,15,20,25};
   int second[] = {50,40,30,20,10};
   int output[10];
   vector<int> v(10);                           // 0  0  0  0  0  0  0  0  0  0
   vector<int>::iterator it;

   sort (first,first+5);     //  5 10 15 20 25
   sort (second,second+5);   // 10 20 30 40 50
   // cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   it=set_symmetric_difference(first, first+5, second, second+5, v.begin());
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  for(it=v.begin();it!=v.end();++it)
 {
 	
 static int i;
 output[i] = *it;
 i++;

 }


 /*
   if(output[0]==5)
   if(output[1]==15)
   if(output[2]==25)
   if(output[3]==30)
   if(output[4]==40)
   if(output[5]==50)
   return KErrNone;
   return KErrGeneral;
   */// __UHEAP_MARKEND;
   
   
     if(output[0]!=5)
     failures++;
   if(output[1]!=15)
   failures++;
   if(output[2]!=25)
   failures++;
   if(output[3]!=30)
   failures++;
   if(output[4]!=40)
   failures++;
   if(output[5]!=50)
   failures++;
   
   //#if STDCPP_OOM
 //failures++;
 // #endif
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
     
  TInt Ctstl_3::Setunion(CStifItemParser& aItem)
  {
  	int failures=0;
  
 //__UHEAP_MARK;
 try{
 int first[] = {5,10,15,20,25};
   int second[] = {50,40,30,20,10};
   int output[10];
   vector<int> v(10);                           // 0  0  0  0  0  0  0  0  0  0
   vector<int>::iterator it;

   sort (first,first+5);     //  5 10 15 20 25
   sort (second,second+5);   // 10 20 30 40 50
  // cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   it=set_union(first, first+5, second, second+5, v.begin());
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  for(it=v.begin();it!=v.end();++it)
 {
 	
 static int i;
 output[i] = *it;
 i++;

 }/*
   if(output[0]==5)
   if(output[1]==10)
   if(output[2]==15)
   if(output[3]==20)
   if(output[4]==25)
   if(output[5]==30)
   if(output[6]==40)
   if(output[7]==50)
   return KErrNone;
   return KErrGeneral;*/
    //__UHEAP_MARKEND;

 	if(output[0]!=5)
 	failures++;
 	
   if(output[1]!=10)
   failures++;
   
   if(output[2]!=15)
   failures++;
   if(output[3]!=20)
   failures++;
   if(output[4]!=25)
   failures++;
   if(output[5]!=30)
   failures++;
   if(output[6]!=40)
   failures++;
   if(output[7]!=50)
   failures++;


    //#if STDCPP_OOM
 //failures++;
 // #endif
  }
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  
  TInt Ctstl_3::HeapL(CStifItemParser& aItem)
  {
  	
  
 //__UHEAP_MARK;


 int failures=0;
 try{
 int myints[] = {10,20,30,5,15};

   vector<int> v(myints,myints+5);
   
  // vector<int>::iterator it;
  // cout<<""; 
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   make_heap(v.begin(),v.end());  //make_heap
   
   if(v.front()!=30)
   failures++;
  
   pop_heap(v.begin(),v.end());  //pop_heap
   v.pop_back();                   //pop_back
  
  
  if(v.front()!=20)
  failures++;
  
   v.push_back(99);  //push_back
   
   
   push_heap(v.begin(),v.end()); //push heap
   
   if(v.front()!=99)
   failures++;
   
  
   sort_heap(v.begin(),v.end()); //sort_heap

      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
    if(v[0]!=5)
    failures++;
    if(v[1]!=10)
    failures++;
    if(v[2]!=15)
    failures++;
    if(v[3]!=20)
    failures++;
    if(v[4]!=99)
    failures++;
    
     
 /*
  if(failures)
  return KErrGeneral;
  return KErrNone;
  */
     //#if STDCPP_OOM
 //failures++;
 // #endif
  
    //__UHEAP_MARKEND;

  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  TInt Ctstl_3::Swapranges(CStifItemParser& aItem)
  {
  	int failures=0;
 // __UHEAP_MARK;
 try{
 	
  vector<int> first (5,10);        //  first: 10 10 10 10 10
   vector<int> second (5,33);       // second: 33 33 33 33 33
   vector<int>::iterator it;
 int output[5];
  // cout<<""; 
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   swap_ranges(first.begin()+1, first.end()-1, second.begin());
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif


 // print out results of swap:
    for (it=first.begin(); it!=first.end(); ++it)
     
     
    {
    	static int i;
    	 output[i] = *it;
    	i++;
    }
    
    
   /* 
    if(output[0]==10)
    if(output[1]==33)
    if(output[2]==33)
    if(output[3]==33)
    if(output[4]==10)
    return KErrNone;
    return KErrGeneral;
    */
     if(output[0]!=10)
     failures++;
    if(output[1]!=33)
    failures++;
    if(output[2]!=33)
    failures++;
    
    if(output[3]!=33)
    failures++;
    if(output[4]!=10)
    failures++;
    
      //#if STDCPP_OOM
 //failures++;
 // #endif
    
 //__UHEAP_MARKEND;
  
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }

  
  TInt Ctstl_3::Partialsums(CStifItemParser& aItem)
  {
  	int failures=0;
  //__UHEAP_MARK;
  try{
  	
  vector<int> V1( 10 ), V2( 10 );
    vector<int>::iterator VIter1,   VIterend; 
  
 int sum =1;
  int i=2 ;
    list <int> L1;
//    list <int>::iterator LIter1, LIterend;

    int t;
    for ( t = 1 ; t <= 10 ; t++ )
    {
       L1.push_back( t );
    }

  // cout<<"";    
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    // The first member function for the partial sums of
    // elements in a list output to a vector
    VIterend = partial_sum ( L1.begin ( ) , L1.end ( ) , 
       V1.begin ( ) );
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     for ( VIter1 = V1.begin( ) ; VIter1 != VIterend ; VIter1++ )
       
     {
     
       if(sum!=*VIter1)
     failures++;
     sum = sum+i;
     i++;
     	
     }
   
   /*
    if(failures)
    return KErrGeneral;
    return KErrNone;
    */
    
     //#if STDCPP_OOM
 //failures++;
 // #endif 
     
 //__UHEAP_MARKEND;
  
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }


 TInt Ctstl_3::Unarynegate(CStifItemParser& aItem)
  {
  	int failures=0;
 // __UHEAP_MARK;
  try{
  vector <int> v1;
  
    //vector <int>::iterator Iter;
    
    int i;
    for ( i = 0 ; i <= 7 ; i++ )
    {
       v1.push_back( 5 * i );
    }
  

    
    int result2;
    int result3;
    // Use the negator to count the elements less than or equal to 10
    // cout<<"";
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   result2 = count_if( v1.begin( ), v1.end( ),unary_negate<binder2nd <greater<int> > >( bind2nd( greater<int>( ),10 ) ) );
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     result3 = count_if( v1.begin( ), v1.end( ),not1(bind2nd( greater<int>( ), 10) ) );
  

    
    /*
    if(result2 == 3)
    if(result3 == 3)
    return KErrNone;
    return KErrGeneral;
    */
      if(result2 != 3)
      failures++;
    if(result3 != 3)
    failures++;
    
    
 //__UHEAP_MARKEND;
    //#if STDCPP_OOM
 //failures++;
 // #endif
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  TInt Ctstl_3::Not2(CStifItemParser& aItem)
  {
  	int failures=0;
  //__UHEAP_MARK;
  try{
     vector <int> v1;
    vector <int>::iterator Iter1;
 int output[7];
    //int i;
    v1.push_back( 6262 );
    v1.push_back( 6262 );
    v1.push_back( 18467 );
    v1.push_back( 6334 );
    v1.push_back( 26500 );
    v1.push_back( 19169);
    v1.push_back(41);
     

  
   // cout<<"";
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif

    // To sort in descending order, 
    // use the binary_negate helper function not2
    sort( v1.begin( ), v1.end( ), not2(less<int>( ) ) );
    
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
    
    for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
    {
    	static int i;
    	output[i] = *Iter1;
    	i++;
    }
    
    /*
    if(output[0]==26500) 
   if(output[1]==19169)
    if(output[2]== 18467)
    if(output[3]== 6334)
    if(output[4]== 6262)
    if(output[5]== 6262)
    if(output[6]== 41)
    
    return KErrNone;
    return KErrGeneral; 
    */
    
    
      if(output[0]!=26500) 
      failures++;
    if(output[1]!=19169)
    failures++;
    if(output[2]!= 18467)
    failures++;
    if(output[3]!= 6334)
    failures++;
    if(output[4]!= 6262)
    failures++;
    if(output[5]!= 6262)
    failures++;
    if(output[6]!= 41)
    failures++;
    
     //#if STDCPP_OOM
 //failures++;
 // #endif
    
    //__UHEAP_MARKEND;
  
  }

 catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }



 TInt Ctstl_3::GettemporarybufferL(CStifItemParser& aItem)
  {
  	int failures=0;
  //__UHEAP_MARK;
  try{
  	
     // Create an array of ints
     // int intArray [ ] = { 10, 20, 30, 40, 100, 200, 300, 1000, 2000 };

    
    int count = 9; //sizeof ( intArray ) / sizeof ( int );
		
    pair<int *, ptrdiff_t> resultPair;
    // cout<<""; 
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    resultPair = get_temporary_buffer<int>( count );
   #if  STDCPP_M
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif

 int* tempBuffer = resultPair.first;

 return_temporary_buffer(tempBuffer);
 /*
    if(count==9)
    if(resultPair.second == 9)
    return KErrNone;
    return KErrGeneral;
   */
   if(count!=9)
   failures++;
     #if  STDCPP_OOM
   if(resultPair.second!=0)
    #else
     if(resultPair.second!=9)
     #endif
   failures++;
    
   
    //#if STDCPP_OOM
 //failures++;
 // #endif 
   // __UHEAP_MARKEND;
  
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  
  
  TInt Ctstl_3::Pair(CStifItemParser& aItem)
  {
  	
  //__UHEAP_MARK;
  
  int failures=0; 
  try{  
  	
   // cout<<"";	
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
 typedef struct pair<int, float> PAIR_IF;

   PAIR_IF A(10,3.14f);
   PAIR_IF B(18,3.14f);
   PAIR_IF C(10,6.28f);
   PAIR_IF D(10,3.14f);

   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  
   if (A==D)
    
   if (B!=C)
    
   if (A>C)
      failures++;
  

   if (A>=C)
    failures++;

  
 /*
   if(failures)
   return KErrGeneral;
   return KErrNone;
  */
   //#if STDCPP_OOM
 //failures++;
 // #endif
  
    
    //__UHEAP_MARKEND;
  
  }


 catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }


  
  bool twice ( int elem1, int elem2 )
 {
    return 2 * elem1 < elem2;
 }


  TInt Ctstl_3::Lexicographicalcompare(CStifItemParser& aItem)
  {
  	
  //__UHEAP_MARK;
  
  


  int failures=0;
  
  try
  {
  	
  vector <int> v1, v2;
    list <int> L1;
//    vector <int>::iterator Iter1, Iter2;
//    list <int>::iterator L1_Iter, L1_inIter;

    int i;
    for ( i = 0 ; i <= 5 ; i++ )
    {
       v1.push_back( 5 * i );
    }
    int ii;
    for ( ii = 0 ; ii <= 6 ; ii++ )
    {
       L1.push_back( 5 * ii );
    }

    int iii;
    for ( iii = 0 ; iii <= 5 ; iii++ )
    {
       v2.push_back( 10 * iii );
    }

    
    // Self lexicographical_comparison of v1 under identity
    bool result1;
    // cout<<"";
       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    
    result1 = lexicographical_compare (v1.begin( ), v1.end( ), v1.begin( ), v1.end( ) );
    
       

    // lexicographical_comparison of v1 and L2 under identity
    bool result2;
    result2 = lexicographical_compare (v1.begin( ), v1.end( ), L1.begin( ), L1.end( ) );
   
        

    bool result3;
    result3 = lexicographical_compare (v1.begin( ), v1.end( ), v2.begin( ), v2.end( ), twice );
       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     /*
     if (!result1)
     if ( result2 )
    if (!result3 )
     
       return KErrNone;
       return KErrGeneral;
       
       */
       
       if(result1)
       failures++;
       if(!result2)
       failures++;
       if(result3)
       failures++;
       
  //#if STDCPP_OOM
 //failures++;
 // #endif
    
   // __UHEAP_MARKEND;
  
  }
 catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }

 //Mismatch

 bool mypredicate (int i, int j) {
   return (i==j);
 }



 TInt Ctstl_3::Mismatch(CStifItemParser& aItem)
  {
  	
 // __UHEAP_MARK;
  
  
 int failures=0;
 try{

  vector<int> myvector;
   for (int i=1; i<6; i++) myvector.push_back (i*10); // myvector: 10 20 30 40 50

   int myints[] = {10,20,80,320,1024};                //   myints: 10 20 80 320 1024

   pair<vector<int>::iterator,int*> mypair;
   // cout<<"";  
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   // using default comparison:
   mypair = mismatch (myvector.begin(), myvector.end(), myints);
   // cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  if(*mypair.first != 30)
  failures++;
  
  if(*mypair.second != 80)
  failures++;
    

   mypair.first++; 
   mypair.second++;
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   // using predicate comparison:
   mypair = mismatch (mypair.first, myvector.end(), mypair.second, mypredicate);
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   if(*mypair.first != 40)
   failures++;
  if(*mypair.second != 320)
  failures++;
    /*
    if(failures)
    return KErrGeneral;
    return KErrNone;
    */
  //#if STDCPP_OOM
 //failures++;
 // #endif
    
   // __UHEAP_MARKEND;
  
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  TInt Ctstl_3::Compare_fun(CStifItemParser& aItem)
  
  {
  int failures=0;	
  //__UHEAP_MARK;
  try
  {
  
   
  string str1 ("green apple");
   string str2 ("red apple");
   // cout<<"";  
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if (str1.compare("str2") == 0)
  failures++;
   
   if (str1.compare(6,5,"apple") != 0)
   failures++;
  
   if (str2.compare(str2.size()-5,5,"apple") != 0)
   
   failures++;
   
  
   if (str1.compare(6,5,str2,4,5) != 0)
   failures++;
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  /*
   if(failures)
   
   return KErrGeneral;
   return KErrNone;
  */
  //__UHEAP_MARKEND;
   //#if STDCPP_OOM
 //failures++;
 // #endif
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
  
  TInt Ctstl_3::Copy_fun(CStifItemParser& aItem)
  
  {
  	int failures=0;
 // __UHEAP_MARK;
  try
  {
  int myints[]={10,20,30,40,50,60,70};
   vector<int> myvector;
   vector<int>::iterator it;
 int a[7];

   myvector.resize(7);  
     // cout<<""; 
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   copy ( myints, myints+7, myvector.begin() );
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
    for (it=myvector.begin(); it!=myvector.end(); ++it)
    {
    	static int i;
    	a[i] = *it;
    	i++;
    }
  /*   
 if(a[0]==10)
 if(a[1]==20)
 if(a[2]==30)
 if(a[3]==40)
 if(a[4]==50)
 if(a[5]==60)
 if(a[6]==70)
 return KErrNone;
 return KErrGeneral;*/

 if(a[0]!=10)
 failures++;
 if(a[1]!=20)
 failures++;

 if(a[2]!=30)
 failures++;
 if(a[3]!=40)
 failures++;
 if(a[4]!=50)
 failures++;
 if(a[5]!=60)
 failures++;
 if(a[6]!=70)
 failures++;

  
 //__UHEAP_MARKEND;
  //#if STDCPP_OOM
 //failures++;
 // #endif 
  }
  
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
   TInt Ctstl_3::Copybackward_fun(CStifItemParser& aItem)
  
  {
  	
  	int failures=0;
  	
  //__UHEAP_MARK;
  try{
  
  vector<int> myvector;
   vector<int>::iterator it;
 int a[11];
   // set some values:
   for (int i=1; i<=5; i++)
     myvector.push_back(i*10);          // myvector: 10 20 30 40 50

   myvector.resize(myvector.size()+6);  // allocate space for 6 more elements
   // cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   copy_backward ( myvector.begin(), myvector.begin()+5, myvector.end() );
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
  for (it=myvector.begin(); it!=myvector.end(); ++it)
    {
    	static int i;
    	a[i] = *it;
    	i++;
    }
  /*   
 if(a[0]==10)
 if(a[1]==20)
 if(a[2]==30)
 if(a[3]==40)
 if(a[4]==50)

 if(a[6]==10)
 if(a[7]==20)
 if(a[8]==30)
 if(a[9]==40)
 if(a[10]==50)

  return KErrNone;
  else
  
 return KErrGeneral;
   */
 // __UHEAP_MARKEND;
  
  if(a[0]!=10)
  failures++;
  
 if(a[1]!=20)
 failures++;
 if(a[2]!=30)
 failures++;
 if(a[3]!=40)
 failures++;
 if(a[4]!=50)
 failures++;

 if(a[6]!=10)
 failures++;
 if(a[7]!=20)
 failures++;
 if(a[8]!=30)
 failures++;
 if(a[9]!=40)
 failures++;
 if(a[10]!=50)
 failures++;
     //#if STDCPP_OOM
 //failures++;
 // #endif
  }
  
  catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
  
  
   TInt Ctstl_3::mergeL(CStifItemParser& aItem)
   
   {
   	int failures=0;
   	try
   	{
    int a[4];
    
     list <int> c1, c2;
    list <int>::iterator   c2_Iter ;
   
 //  __UHEAP_MARK;
  
    
  
   	
    
    c1.push_back( 3 );
    c1.push_back( 6 );
    c2.push_back( 2 );
    c2.push_back( 4 );
    
  // cout<<"";
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif  
  
   c2.merge( c1 );  // Merge c1 into c2 in (default) ascending order
   
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   c2.sort( greater<int>( ) );
     for ( c2_Iter = c2.begin( ); c2_Iter != c2.end( ); c2_Iter++ )
           
     {
     static int i;
     a[i]= *c2_Iter;
     i++;
     }
   
 // __UHEAP_MARKEND;
   
  /* if(a[0]==6)
   if(a[1]==4)
   if(a[2]==3)
   if(a[3]==2)
   
   
   
   return KErrNone;
  
   return KErrGeneral;
   */
   
   
   if(a[0]!=6)
   
   failures++;
   
   if(a[1]!=4)
   failures++;
   if(a[2]!=3)
   failures++;
   if(a[3]!=2)
   failures++;
   
     //#if STDCPP_OOM
 //failures++;
 // #endif
   
   
   }

 catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }

 TInt Ctstl_3::allocatoradd(CStifItemParser& aItem)
   
   {
   	int failures = 0;
   
   //  __UHEAP_MARK;
 try{
      vector <int> v1;
//    vector <int>::iterator v1Iter;
    vector <int>:: allocator_type v1Alloc;
     
    int i;
    for ( i = 1 ; i <= 7 ; i++ )
    {
       v1.push_back( 2 * i );
    }

      // cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif

    allocator<int>::const_pointer v1Ptr;
    
    
    const int k = 8;
      v1Ptr = v1Alloc.address( k );
        #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
      
  /*     
    if( *v1Ptr == 8)
 // __UHEAP_MARKEND;

   return KErrNone;
  
   return KErrGeneral;
   */
   
   
   if(*v1Ptr!=8)
   failures++;
   
   
     //#if STDCPP_OOM
 //failures++;
 // #endif
   
   
   }

 catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }


   
   TInt Ctstl_3::alldeall(CStifItemParser& aItem)
   
   
   {
   
     //__UHEAP_MARK;

   	int failures =0;
   	
   	try{
   		
   		
   allocator<int> v1Alloc;

    allocator<int>::pointer aPtr;

    aPtr = v1Alloc.allocate ( 5 );

    int i;
    for ( i = 0 ; i < 5 ; i++ )
    {
       aPtr[ i ] = i;
    }

    if(aPtr[0] != 0)
    failures++;
  // cout<<""; 
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    v1Alloc.deallocate( aPtr, 5 );
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
  if(aPtr[0] == 0)
  failures++;
  
  
    //__UHEAP_MARKEND;
 /*
  if (failures)
  return KErrGeneral;
  else
  return KErrNone;
  
  */

    //#if STDCPP_OOM
 //failures++;
 // #endif
   }

   catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
   
   
   TInt Ctstl_3::allcons(CStifItemParser& aItem)
   {
   
   
   // __UHEAP_MARK;

   int output[7],failures = 0;
   try{
  vector <int> v1;
    vector <int>::iterator v1Iter;
    vector <int>:: allocator_type v1Alloc;

    int i;
    for ( i = 1 ; i <= 7 ; i++ )
    {
       v1.push_back( 3 * i );
    }

     for ( v1Iter = v1.begin( ) ; v1Iter != v1.end( ) ; v1Iter++ )
      
     {
     static int i;
     output[i] = *v1Iter;
     i++;	
     
     } 
  
  
  if(output[1] != 6)
  failures++;
  
  
    allocator<int>::pointer v1PtrA;
    int kA = 6, kB = 7;
    v1PtrA = v1Alloc.address( *find( v1.begin( ), v1.end( ), kA ) );
    v1Alloc.destroy ( v1PtrA );
    // cout<<""; 
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    v1Alloc.construct ( v1PtrA , kB );
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     for ( v1Iter = v1.begin( ) ; v1Iter != v1.end( ) ; v1Iter++ )
   
   {
     static int i;
     output[i] = *v1Iter;
     i++;	
     
     } 
     
     if(output[1] != 7)
     failures++;
     
     
    //  __UHEAP_MARKEND;

    /* if(failures)
     return KErrGeneral;
     return KErrNone;
   */
   
     //#if STDCPP_OOM
 //failures++;
 // #endif
   
     
   
   
   	
   }
   catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
   
     TInt Ctstl_3::allrebind(CStifItemParser& aItem)
     {
     
     
     
     
   // __UHEAP_MARK;	
    int failures =0;
    try{
    
    typedef vector<int>::allocator_type IntAlloc;

    IntAlloc v1Iter;
    vector<int> v1;
   // cout<<""; 
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    IntAlloc::rebind<char>::other::pointer pszC = IntAlloc::rebind<char>::other(v1.get_allocator()).allocate(1, (void *)0);
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
    int * pInt = v1Iter.allocate(10);

    
     
  //  __UHEAP_MARKEND;
    
   /* if(failures)
    return KErrGeneral;
    return KErrNone;
    */
    
      //#if STDCPP_OOM
 //failures++;
 // #endif
    
     }

 catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
     
     
     TInt Ctstl_3::allmaxsize(CStifItemParser& aItem)

     {int failures=0;
     	
     	
     	try{
     vector <int> v1;
//    vector <int>::iterator v1Iter;
    vector <int>:: allocator_type v1Alloc;

     

  
    allocator<int>::size_type v1size;
   // cout<<"";   
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    v1size = v1Alloc.max_size( );	
       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     /*if(v1size == 1073741823)
     return KErrNone;
     else
     return KErrGeneral;*/
     
     
     if(v1size != 1073741823)
     failures++;
     
     
     //#if STDCPP_OOM
 //failures++;
 // #endif
     
     }
   
   
   catch(bad_alloc&)
    {
    	//do nothing
     }
    catch(...)
    {
    	failures++;
    	
    }
    
 		  if(failures  )
 		  return KErrGeneral;
 		  return KErrNone;
     }
   
   
   
   
// -----------------------------------------------------------------------------
// Ctstl_3::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Ctstl_3::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File

//  End of File
