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
#include "tstl_4.h"



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
 
#undef test
 using namespace std;
 #define STDCPP_OOM FALSE// TRUE for OOM testing

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctstl_4::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctstl_4::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctstl_4::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctstl_4::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        		 
        	        ENTRY( "Swapof", Ctstl_4::Swapof ),
        	        ENTRY( "Accumulate", Ctstl_4::Accumulate ),
        	 		ENTRY( "Adjacent_difference", Ctstl_4::Adjacent_difference),
        	        ENTRY( "Adjacent_find", Ctstl_4::Adjacent_find),
        	        ENTRY( "Logical_or", Ctstl_4::Logical_or ),
        	        ENTRY( "Logical_and", Ctstl_4::Logical_and ),
        	        ENTRY( "Logical_not", Ctstl_4::Logical_not ),
        	        ENTRY( "count_fun", Ctstl_4::count_fun),
        	        ENTRY( "countif_fun", Ctstl_4::countif_fun),
        	        ENTRY( "equal_fun", Ctstl_4::equal_fun),
        	        ENTRY( "equalto", Ctstl_4::equalto),
        	        ENTRY( "notequalto", Ctstl_4::notequalto),
        	 		ENTRY( "sort_fun", Ctstl_4::sort_fun),
        	 		ENTRY( "less_equal_fun", Ctstl_4::less_equal_fun),
        	 		ENTRY( "greater_equal_fun", Ctstl_4::greater_equal_fun),
        	 		ENTRY( "greater_fun", Ctstl_4::greater_fun),
        	 		ENTRY( "less_fun", Ctstl_4::less_fun),
        	 		ENTRY( "find_fun", Ctstl_4::find_fun),
        	 		ENTRY( "findif_fun", Ctstl_4::findif_fun),
        	 		ENTRY( "findend_fun", Ctstl_4::findend_fun),
        	 		  ENTRY( "Minof", Ctstl_4::Minof ),
        	 	        ENTRY( "Maxelement", Ctstl_4::Maxelement ),
        	 	        ENTRY( "Minelement", Ctstl_4::Minelement ),
        	 	      
        	 	        
        	 	       ENTRY( "ToUpper", Ctstl_4::ToUpper ),
        	 	        ENTRY( "ToLower", Ctstl_4::ToLower ),
        	 	        ENTRY( "Islower", Ctstl_4::Islower ),
        	 	        ENTRY( "Isupper", Ctstl_4::Isupper ),
        	 	        ENTRY( "Maxof", Ctstl_4::Maxof ),
        	 	        
        	 	        
        	 	       ENTRY("Advance", Ctstl_4::Advance ),
        	 	      ENTRY("Backinsert_iterator", Ctstl_4::Backinsert_iterator ),
        	 	      ENTRY("Frontinsert_iterator", Ctstl_4::Frontinsert_iterator ),
        	 	      ENTRY("Insert_iterator", Ctstl_4::Insert_iterator ),
        	 	      ENTRY("Reverse_iterator", Ctstl_4::Reverse_iterator ),
        	 	      ENTRY("Distance", Ctstl_4::Distance ),
        	 	     
        	 	 
        	 	      ENTRY("Isdigit", Ctstl_4::Isdigit ),
        	 	      ENTRY("Isgraph", Ctstl_4::Isgraph ),
        	 	      ENTRY("Isalnum", Ctstl_4::Isalnum ),
        	 	      ENTRY("Isalpha", Ctstl_4::Isalpha ),
        	 	      ENTRY("Iscntrl", Ctstl_4::Iscntrl ),
        	 	      ENTRY("Isprint", Ctstl_4::Isprint ),
        	 	      ENTRY("Ispunct", Ctstl_4::Ispunct ),
        	 	      ENTRY("Isspace", Ctstl_4::Isspace ),
        	 	      ENTRY("Isxdigit", Ctstl_4::Isxdigit ),
        	 	      ENTRY("Fill", Ctstl_4::Fill ),
        	 	      ENTRY("Generate", Ctstl_4::Generate ),
        	 	      ENTRY("Search", Ctstl_4::Search ),
        	 	      ENTRY("Search_n", Ctstl_4::Search_n ),
        	 	      ENTRY("Times", Ctstl_4::Times ),
        	 	      ENTRY("Unique", Ctstl_4::Unique ),
        	 	      ENTRY("Uniquecopy", Ctstl_4::Uniquecopy ),
        	 	      ENTRY("Partialsort", Ctstl_4::Partialsort ),
        	 	      ENTRY("Partialsortcopy", Ctstl_4::Partialsortcopy ),
        	 	      ENTRY("Partition", Ctstl_4::Partition ),
        	 	      ENTRY("Stablepartition", Ctstl_4::Stablepartition ),
        	 	      ENTRY("Setdifference", Ctstl_4::Setdifference ),
       

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }





















//swap

TInt Ctstl_4::Swapof(CStifItemParser& aItem)
{
int x= 10,y=20;

//__UHEAP_MARK;
int failures =0;
  

 try
 {  //cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
swap(x,y);
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
/*if (x==20 && y==10)


//__UHEAP_MARKEND;

 return KErrNone;
 return KErrGeneral;*/
 if(x!=20)
 failures++;
 if(y!=10)
 failures++;
 
//#if STDCPP_OOM
 // failures++;
 // #endif
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
     
   
  if(failures)
  return KErrGeneral;
  return KErrNone;  
} 



//accumulate 
int mult(int x,int y)
{
	return x*y;
}


 TInt Ctstl_4::Accumulate(CStifItemParser& aItem)
{
    
int failures =0;

   // __UHEAP_MARK;
    
 try
 {
 	
 
  int test_array[10] = {1,2,3,4,5,6,7,8,9,10};
  vector<int> v1(test_array, test_array+10);
 //   cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
  int sum = accumulate(v1.begin(), v1.end(),0);
  
  int prod = accumulate(v1.begin(), v1.end(),1,mult);
  #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
  v1.erase(v1.begin(),v1.end());
   
  /* if(sum == 55  &&  prod == 3628800)
  // __UHEAP_MARKEND;    
   
   return KErrNone;
   return KErrGeneral;*/
   if(sum!=55)
   failures++;
   if(prod!=3628800)
   failures++;
  //#if STDCPP_OOM //failures++; //#endif
   
 }
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
     
   
  if(failures)
  return KErrGeneral;
  return KErrNone;
}  





TInt Ctstl_4::Adjacent_difference(CStifItemParser& aItem)
{
    

     //__UHEAP_MARK;
    
     
  int failures =0;
 

 try
 { 
 vector<int> V1( 10 ), V2( 10 );
 vector<int>::iterator VIter1, VIter2, VIterend, VIterend2;


int array1[10];
int array2[10];
 list <int> L1;
// list <int>::iterator LIter1, LIterend, LIterend2;

 int t;
 for ( t = 1 ; t <= 10 ; t++ )
 {
    L1.push_back( t * t );
 }
//cout<<"";
 #if  STDCPP_OOM
	User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
 // The first member function for the adjacent_differences of
 // elements in a list output to a vector
 VIterend = adjacent_difference ( L1.begin (),L1.end (),V1.begin () );
 #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
  for ( VIter1 = V1.begin( ) ; VIter1 != VIterend ; VIter1++ )
    
    
 {
 	static int i;
 	
 	array1[i] = *VIter1;	
 	i++;
 	}

#if  STDCPP_OOM
	User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
 // The second member function used to compute
 // the adjacent products of the elements in a list
 VIterend2 = adjacent_difference ( L1.begin (),L1.end ( ),V2.begin ( ),multiplies<int>( ) );
 #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
  for ( VIter2 = V2.begin( ) ; VIter2 != VIterend2 ; VIter2++ )
 {
 	
 static int i;
 array2[i] = *VIter2;
 i++;
}
     
/*
  if(array1[0] == 1)
  if(array1[1] == 3)
  if(array1[2] == 5)
  if(array1[3] == 7)
  if(array1[4] == 9)
  if(array1[5] == 11)
  if(array1[6] == 13)
  if(array1[7] == 15)
  if(array1[8] == 17)
  if(array1[9] == 19)
  
  if(array2[0] == 1)
  if(array2[1] == 4)
  if(array2[2] == 36)
  if(array2[3] == 144)
  if(array2[4] == 400)
  if(array2[5] == 900)
  if(array2[6] == 1764)
  if(array2[7] == 3136)
  if(array2[8] == 5184)
  if(array2[9] == 8100)
  
 //__UHEAP_MARKEND;
  return KErrNone;
  return KErrGeneral;*/
  if(array1[0] != 1)
  failures++;
  if(array1[1] != 3)
  failures++;
  if(array1[2] != 5)
  failures++;
  if(array1[3] != 7)
  failures++;
  if(array1[4] != 9)
  failures++;
  if(array1[5] != 11)
  failures++;
  if(array1[6] != 13)
  failures++;
  if(array1[7] != 15)
  failures++;
  if(array1[8] != 17)
  failures++;
  if(array1[9] != 19)
  failures++;
  
  if(array2[0] != 1)
  failures++;
  if(array2[1] != 4)
  failures++;
  if(array2[2] != 36)
  failures++;
  if(array2[3] != 144)
  failures++;
  if(array2[4] != 400)
  failures++;
  if(array2[5] != 900)
  failures++;
  if(array2[6] != 1764)
  failures++;
  if(array2[7] != 3136)
  failures++;
  if(array2[8] != 5184)
  failures++;
  if(array2[9] != 8100)   
  failures++;
      //#if STDCPP_OOM //failures++; //#endif
  
  
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
     
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 
 


}  



TInt Ctstl_4::Adjacent_find(CStifItemParser& aItem)
{
    

     //__UHEAP_MARK;
   
  int failures =0;
  

 try
 {     
 int myints[] = {10,20,30,30,20,10,50,60};
 int myints1[]=
 {
 	10,20,30,40,50,60,70
 };
vector<int> myvector (myints,myints+8) ,myvector1(myints1,myints1+7);
vector<int>::iterator it; 
//cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
 it = adjacent_find (myvector.begin(), myvector.end());
 #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif 
/*
if(*it == 30)


    // __UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;

*/
if(*it!=30)
failures++;

//#if STDCPP_OOM //failures++; //#endif  
}
 catch(bad_alloc&)
 {
 
 }
 catch(...)
 {
 	failures++;
 	
 }
   
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;  
  




}  










// logical or 
TInt Ctstl_4::Logical_or(CStifItemParser& aItem)
{
	

//__UHEAP_MARK;
int failures =0;
 

 try
 {    
int input1 [4] = { 1, 1, 0, 1 };
int input2 [4] = { 0, 1, 0, 0 };
 int output [4];
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, logical_or<bool>());

#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
 /* if(output[0] == 1 && output[1] == 1 && output[2] ==0 && output[3] == 1)
  
 // __UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;  */
if(output[0] != 1)
failures++;
if(output[1] != 1 )
failures++;
if(output[2] !=0)
failures++;
if( output[3] != 1)
failures++;

//#if STDCPP_OOM //failures++; //#endif
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
     
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 

}


//logical and

TInt Ctstl_4::Logical_and(CStifItemParser& aItem)
{
	

//__UHEAP_MARK;
int failures =0;
  

 try
 {    
int input1 [4] = { 1, 1, 0, 1 };
int input2 [4] = { 0, 1, 0, 0 };
 int output [4];
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, logical_and<bool>());
 
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif   

/*  if(output[0] == 0 && output[1] == 1 && output[2] ==0 && output[3] == 0)
 
 //__UHEAP_MARKEND;
  
return KErrNone;
return KErrGeneral;  */
if(output[0] != 0)
failures++;
if(output[1] != 1 )
failures++;
if(output[2] !=0)
failures++;
if( output[3] != 0)
failures++;

//#if STDCPP_OOM //failures++; //#endif
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 }
 
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;


}


//LOGICAL NOT


TInt Ctstl_4::Logical_not(CStifItemParser& aItem)
{
	
//__UHEAP_MARK;
int failures =0;
  

 try
 {    
int input1 [4] = { 1, 1, 0, 1 };

 int output [4];
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif
transform((int*)input1, (int*)input1 + 4,  (int*)output, logical_not<bool>());
 
 
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
/*
  if(output[0] == 0 && output[1] == 0 && output[2] ==1 && output[3] == 0)
  
//__UHEAP_MARKEND;
  
return KErrNone;
return KErrGeneral; */ 
if(output[0] != 0)
failures++;
if(output[1] != 0 )
failures++;
if(output[2] !=1)
failures++;
if( output[3] != 0)
failures++;

//#if STDCPP_OOM //failures++; //#endif
 }
 catch(bad_alloc&)
 {
 }
 catch(...)
 {
 	failures++;
 }
 	
     
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;


}


//COUNT



TInt Ctstl_4::count_fun(CStifItemParser& aItem)
{
	
	
int failures =0;
  

 try
 {    //	__UHEAP_MARK;
	
int sequence[10] = {1,2,3,4,5,5,7,8,9,10};
  int i=0,j=0,k=0;
   vector<int> v(sequence,sequence + 10); 
   //cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
  count(v.begin(),v.end(),5,i);  
  count(v.begin(),v.end(),6,j);
  count(v.begin(),v.end(),10,k);
  
  #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif   
   /* 
   if(i==2 && j==0 && k ==1)
  
    //__UHEAP_MARKEND;
   return KErrNone;
   return KErrGeneral;*/
   if(i!=2)
   failures++;
   if(j!=0)
   failures++;
   if(k!=1)
   failures++;
   //#if STDCPP_OOM //failures++; //#endif
   
 }
 catch(bad_alloc&)
 {
 }
 catch(...)
 {
 	failures++;
 	
 }
  
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 
   
}




//COUNTIF
TInt Ctstl_4::countif_fun(CStifItemParser& aItem)
{

//__UHEAP_MARK;
	int failures =0;

 try
 {   
	int sequence[10] = {1,2,3,4,5,5,7,8,9,10};
  int i=0 ,j=0;
  vector<int> v(sequence,sequence + 10); 
  //cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 

  count_if(v.begin(),v.end(),bind2nd(less<int>(),8),i);  
  count_if(v.begin(),v.end(),bind2nd(less<int>(),1),j);  
   #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
  
    /*
   if(i==7 && j==0)

    //__UHEAP_MARKEND ;  
   return KErrNone;
   return KErrGeneral;*/
   if(i!=7)
   failures++;
   if(j!=0)
   failures++;
 //#if STDCPP_OOM //failures++; //#endif  
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
     
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;
   
}


//equal


TInt Ctstl_4::equal_fun(CStifItemParser& aItem)
{


//__UHEAP_MARK;
	int failures =0;
  

 try
 {   
	int input1 [4] = { 0, 4, 6, -7 };
  int input2 [4] = { -0, 34, 6, -7 };
 
vector<int> v1(input1+0, input1 + 3), v2(input2+0, input2 + 3);
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
   // Check for equality
    if(equal(v1.begin(),v1.end(),v2.begin()))
    failures++;
    //__UHEAP_MARKEND;
    //return KErrNone;
    //return KErrGeneral;
    
 #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif   
//#if STDCPP_OOM //failures++; //#endif   
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
   
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;

 

 
}






//equalto 

TInt Ctstl_4::equalto(CStifItemParser& aItem)
{
	
	
//	__UHEAP_MARK;
		int failures =0;
  
 try
 {   
	int input1 [4] = { 0, 4, 6, -7 };
  int input2 [4] = { -0, 34, 6, -7 };
 int output [4];
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 

transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, equal_to<int>());
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif    
/* 
if(output[0] == 1 && output[1] == 0 && output[2] ==1 && output[3] == 1)

//__UHEAP_MARKEND;  

return KErrNone;
return KErrGeneral;*/
if(output[0] != 1)
failures++;
if(output[1] != 0 )
failures++;
if(output[2] !=1)
failures++;
if( output[3] != 1)
failures++;

//#if STDCPP_OOM //failures++; //#endif
 }
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
 
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 
}


//NOTEQUAL TO

TInt Ctstl_4::notequalto(CStifItemParser& aItem)
{
	int failures =0;
  

 try
 {   

//__UHEAP_MARK;	
	int input1 [4] = { 0, 4, 6, -7 };
  int input2 [4] = { -0, 34, 6, -7 };
 int output [4];
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, not_equal_to<int>());

#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif 
/*
if(output[0] == 0 && output[1] == 1 && output[2] ==0 && output[3] == 0)

//__UHEAP_MARKEND;

return KErrNone;
return KErrGeneral;*/
if(output[0] != 0)
failures++;
if(output[1] != 1)
failures++;
if(output[2] !=0)
failures++;
if( output[3] != 0)
failures++;

//#if STDCPP_OOM //failures++; //#endif


}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
    
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;    
}




//sort

TInt Ctstl_4::sort_fun(CStifItemParser& aItem)
{

int failures =0;
  

 try
 {   
// __UHEAP_MARK;
int sequence[7] = {6262, 6262, 41 ,18467, 6334, 26500, 19169 };
   vector <int>::iterator Iter1;

int output[7];

  vector<int> v1(sequence,sequence + 7); 
  //cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
sort( v1.begin( ), v1.end( ) );
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif  
  for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )

  {
  static int i = 0;
  
  output[i] = *Iter1;
  i++;
 
  }
 
	/*(output[0] == 41 && output[1] == 6262 && output[2] == 6262 && output[3] == 6334 && output[4] == 18467  &&  output[5] == 19169 && output[6] == 26500)
	
	//	__UHEAP_MARKEND;

	return KErrNone ;
	return KErrGeneral;*/
if(output[0] != 41)
failures++;
if(output[1] != 6262)
failures++;
if(output[2] !=6262)
failures++;
if( output[3] != 6334)
failures++;
if(output[4] != 18467)
failures++;
if(output[5] !=19169)
failures++;
if( output[6] != 26500)
failures++;

//#if STDCPP_OOM //failures++; //#endif 	
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
   
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 

}




//less_equal

TInt Ctstl_4::less_equal_fun(CStifItemParser& aItem)
{


// __UHEAP_MARK;
	int failures =0;
   

 try
 {   
int sequence[7] = {6262, 6262, 41 ,18467, 6334, 26500, 19169 };
   vector <int>::iterator Iter1;

int output[7];

  vector<int> v1(sequence,sequence + 7);
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif
sort( v1.begin( ), v1.end( ),less_equal<int>( ) );
 #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif 
  for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )

  {
  static int i = 0;
  
  output[i] = *Iter1;
  i++;
 
  }
 
/*	if(output[0] == 41 && output[1] == 6262 && output[2] == 6262 && output[3] == 6334 && output[4] == 18467  &&  output[5] == 19169 && output[6] == 26500)
	
	//	__UHEAP_MARKEND;

	
	return KErrNone ;
	return KErrGeneral;*/
if(output[0] != 41)
failures++;
if(output[1] != 6262)
failures++;
if(output[2] !=6262)
failures++;
if( output[3] != 6334)
failures++;
if(output[4] != 18467)
failures++;
if(output[5] !=19169)
failures++;
if( output[6] != 26500)
failures++;

//#if STDCPP_OOM //failures++; //#endif 
	
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
   
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;  	
}





//greater_equal

TInt Ctstl_4::greater_equal_fun(CStifItemParser& aItem)
{


// __UHEAP_MARK;
	int failures =0;
 

 try
 {   
int sequence[7] = {6262, 6262, 41 ,18467, 6334, 26500, 19169 };
   vector <int>::iterator Iter1;

int output[7];

  vector<int> v1(sequence,sequence + 7);
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
sort( v1.begin( ), v1.end( ) ,greater_equal<int>( ) );
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
  for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )

  {
  static int i = 0;
  
  output[i] = *Iter1;
  i++;
 
  }
 
/*  	if(output[6] == 41 && output[5] == 6262 && output[4] == 6262 && output[3] == 6334 && output[2] == 18467  &&  output[1] == 19169 && output[0] == 26500)
	
	//	__UHEAP_MARKEND;

	return KErrNone ;
	return KErrGeneral;*/
	 if(output[6] != 41)
failures++;
if(output[5] != 6262)
failures++;
if(output[4] !=6262)
failures++;
if( output[3] != 6334)
failures++;
if(output[2] != 18467)
failures++;
if(output[1] !=19169)
failures++;
if( output[0] != 26500)
failures++;

//#if STDCPP_OOM //failures++; //#endif 
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
     
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 	
}



//greater

TInt Ctstl_4::greater_fun(CStifItemParser& aItem)
{

	int failures =0;
  

 try
 {   
//__UHEAP_MARK;
int input1 [4] = { 0, 4, 6, 8 };
  int input2 [4] = { -0, 34, 6, -7 };
 int output [4];
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, greater<int>());
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif 
	// After this call to transform, output will contain a "1" if input1
//was greater than  input2 or a "0" if input1 was less than or equal to
//input2.
/*
	if(output[0] == 0 && output[1] == 0 && output[2] == 0 && output[3] == 1)
//	__UHEAP_MARKEND;
	return KErrNone;
	return KErrGeneral;*/
	 if(output[0] != 0)
failures++;
if(output[1] != 0)
failures++;
if(output[2] !=0)
failures++;
if( output[3] != 1)
failures++;

//#if STDCPP_OOM //failures++; //#endif
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
    
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 



	
}


//LESS

TInt Ctstl_4::less_fun(CStifItemParser& aItem)
{
// __UHEAP_MARK;	
	int failures =0;
   

 try
 {    int input1 [4] = { 0, 4, 6, 8 };
  int input2 [4] = { -0, 34, 6, -7 };
 int output [4];
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif
transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, less<int>());
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif  
//After this call to transform, output will contain a "1" if intput1
//was less than  input2 or a "0" if intput1 was greater than or equal to
//input2.

/*	if(output[0] == 0 && output[1] == 1 && output[2] == 0 && output[3] == 0)
//	__UHEAP_MARKEND;
	return KErrNone;
	return KErrGeneral;*/
	if(output[0] != 0)
failures++;
if(output[1] != 1)
failures++;
if(output[2] !=0)
failures++;
if( output[3] != 0)
failures++;

//#if STDCPP_OOM //failures++; //#endif

}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
   
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 	
}



// find code check
//---------------------

TInt Ctstl_4::find_fun(CStifItemParser& aItem)
{
// __UHEAP_MARK;		
	int failures =0;
  
 try
 {     typedef vector<int>::iterator iterator;
  int d1[10] = {0,1,2,2,3,4,2,2,6,7};

   vector<int> v1(d1,d1 + 10); 
   //cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 

   iterator it1 = find(v1.begin(),v1.end(),3);
     #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif      
//last iterator is returned  if unfound

  /* if((it1) && ((*it1) == 3)  ) 
   

	     
//	__UHEAP_MARKEND;
    return KErrNone;
   return  KErrGeneral; */
    
if((it1) && ((*it1) != 3)  )
failures++;
//#if STDCPP_OOM
//  failures++;
 // #endif
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
    
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 

	
}



// find_if   
//----------------------


TInt Ctstl_4::findif_fun(CStifItemParser& aItem)
{

//__UHEAP_MARK;
	
	int failures =0;
  

 try
 {    typedef vector<int>::iterator iterator;
  int d1[10] = {0,1,2,2,3,4,2,2,6,7};

   vector<int> v1(d1,d1 + 10); 
   //cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
   iterator it1 = find_if(v1.begin(),v1.end(),bind1st(equal_to<int>(),7));

      #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif        
//last iterator is returned  if unfound
/*   if((it1) && ((*it1) == 7)  ) 
   
//__UHEAP_MARKEND;
   return KErrNone;
   return KErrGeneral;*/
   if((it1) && ((*it1) != 7) ) 
   failures++;
  //#if STDCPP_OOM //failures++; //#endif
    
  }
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
  
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone;  


	
}



//find_end 

TInt Ctstl_4::findend_fun(CStifItemParser& aItem)
{


//__UHEAP_MARK;
	int failures =0;
   

 try
 {   
 int array1[12] = {0,5,10,15,20,25,0,5,10,15,20,25};
int array2[3]    =  {20,30,40 };
int array3[4]= {5,10,15,20 };
int array4[4] ={30,40,50,60  }; //not found iterator


vector <int> v1(array1,array1+12), v2(array2,array2+3),v3(array3,array3+4);
  vector<int> v4(array4,array4+4);
//  vector <int>::iterator Iter1, Iter2,Iter3 ,Iter4;


vector <int>::iterator result1;

 	
 //cout<<"";
 #if  STDCPP_OOM
	User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

	 #endif
 result1 = find_end( v1.begin( ), v1.end( ), v3.begin( ), v3.end( ) );
 
// result2 = find_end ( v1.begin( ), v1.end( ), v4.begin( ), v4.end( ) );
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

#endif 

/* if((result1 - v1.begin() == 7) && (result1 - v1.begin( ) != 1))

if(result2 == v1.end()) // if not found 
 
 // __UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;*/
if((result1 - v1.begin() != 7) && (result1 - v1.begin( ) == 1))
failures++;
//#if STDCPP_OOM //failures++; //#endif
}
 catch(bad_alloc&)
 {
  }
 catch(...)
 {
 	failures++;
 	
 }
    
 // __UHEAP_MARKEND;
  
  if(failures)
  return KErrGeneral;
  return KErrNone; 


	
}






// Ctstl_4::Minof


TInt Ctstl_4::Minof( CStifItemParser& aItem )
{

 int failures =0;


try
{ 
  int result_int1;
 int result_int2;
 char result_char;
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif  
 result_int1 = min(2,1);
 result_int2 = min(-2,-25);
 result_char = min('a','z');
  
 #if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
 
/* if (result_int1 == 1)
  if(result_int2 == -25) 
 if (result_char == 'a')
if(min(2.23,2.24) == 2.23)  


  return KErrNone;
 return KErrGeneral;
 */
  if (result_int1 != 1)
  failures++;
  
  if(result_int2 != -25) 
  failures++;
 if (result_char != 'a')
 failures++;
if(min(2.23,2.24) != 2.23)    
failures++;

}
catch(bad_alloc&)
{
//do nothing
}
catch(...)
{
	failures++;
	
}
   
 
if(failures)
return KErrGeneral;
return KErrNone; 
 
 
	
}



TInt Ctstl_4::Maxelement(CStifItemParser& aItem)
{
	
	
	

int failures =0;


try
{ 
 
int test_array1[] = { 0,-2,-3333,1,23,3333,234,989};
char test_array2[] =  {'c','d','z','e','w'};

int test_variable1;
char test_variable2;

 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
test_variable1  = *max_element(test_array1,test_array1 + 8);
test_variable2 = *max_element(test_array2,test_array2+5);
int test_variable3 =*max_element(test_array1,test_array1 + 7);

#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif
/*if((test_variable1 == 3333)&&(test_variable2 == 'z'))
if(test_variable3!=3333)
 
return KErrNone;
return KErrGeneral;
*/

if((test_variable1 != 3333)&&(test_variable2 != 'z'))
failures++;
if(test_variable3!=3333)
failures++;
 

}
catch(bad_alloc&)
{
//do nothing
}
catch(...)
{
	failures++;
	
}
  
  

if(failures)
return KErrGeneral;
return KErrNone; 
 

}





TInt Ctstl_4::Minelement(CStifItemParser& aItem)
{


int failures =0;

try
{ 
 
int test_array1[] = { 0,-2,-3333,1,23,3333,234,989};
char test_array2[] =  {'c','d','z','e','w'};

int test_variable1;
char test_variable2;

	  
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);

#endif 
test_variable1  = *min_element(test_array1,test_array1 + 8);
test_variable2 =  *min_element(test_array2,test_array2 + 5);

#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);


#endif

/* if((test_variable1 == -3333)&&(test_variable2 == 'c'))

 
 return KErrNone;
return KErrGeneral;*/

if(test_variable1 != -3333)failures++;
if(test_variable2 != 'c') failures++;
}
catch(bad_alloc&)
{
return KErrNone;
}
catch(...)
{
	failures++;
	
}
   
 

if(failures)
return KErrGeneral;
return KErrNone; 


}




// -----------------------------------------------------------------------------
// Ctstl_4::ToUpper
 // -----------------------------------------------------------------------------
//
TInt Ctstl_4::ToUpper( CStifItemParser& aItem )
    {
 
     int failures=0 ; 

//__UHEAP_MARK;


   try
   {
  	//testcase starts
   char lower_char ;
            
      	
		     for(lower_char='a';lower_char<='z';lower_char++)		      
 		      
		     {
		     
 			#if  STDCPP_OOM
			User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
			#endif 		     
		     
		     char upper_char = toupper(lower_char);
		     
		     #if  STDCPP_OOM 	
 			 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  		   	#endif 
 		     		     
		          
		     if ((lower_char - 32) != upper_char )      
		 	 
		 	 failures++;
		     }
		     
		     
		     
		     for(char lower_char=0x00;lower_char<=0x60;lower_char++)
		     {
		     	#if  STDCPP_OOM
				User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
				#endif 
		     	char upper_char = toupper(lower_char);
		     	#if  STDCPP_OOM 	
 				 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 
				 #endif 
 
		     	if(lower_char!=upper_char)
		     	failures++;
		     	
		     }
		     
		     for(char lower_char=0x7B;lower_char<=0x7E;lower_char++)
		     {
		     	#if  STDCPP_OOM
				User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
				#endif 
		     	char upper_char = toupper(lower_char);
		     	#if  STDCPP_OOM 	
  				User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 
			 #endif 
 
		     	if(lower_char!=upper_char)
		     	failures++;
		     	


	 		  }
	 		  

	 		  //testcase ends

	 		  
 //#if STDCPP_OOM
 //failures++; 
 //#endif

}

//catching bad alloc exception
   catch(bad_alloc&)
   {
    //do nothing
   }
   
   //catching any other exception
   
   catch(...)
   {
   	failures++;
   	
   }
   
   
  
 
		  //__UHEAP_MARKEND;
		  if(failures)
		  return KErrGeneral;
		  return KErrNone;
    
    
    }
    
 // -----------------------------------------------------------------------------
// Ctstl_4::ToLower
// -----------------------------------------------------------------------------
//
    
    
    
   
    TInt Ctstl_4::ToLower( CStifItemParser& aItem )
    {

     //__UHEAP_MARK;
   char upper_char ;
     int failures = 0;       
     

   try
   {
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
		     for(upper_char='A';upper_char<='Z';upper_char++)
		     {
		     	
		     
		     char lower_char = tolower(upper_char);
		          
		     if (lower_char  != (upper_char + 32 ))       
		 	 
		 failures++;
		     }
    
    
		     for(char upper_char=0x00;upper_char<=0x40;upper_char++)
		     {
		     	
		     	char lower_char = tolower(upper_char);
		     	if(lower_char!=upper_char)
		     	failures++;
		     	
		     }
		     for(char upper_char=0x5B;upper_char<=0x7E;upper_char++)
		     {
		     	
		     	char lower_char = tolower(upper_char);
		     	if(lower_char!=upper_char)
		     	failures++;
		     	


		     }
		     
 #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  
 #endif		     
		     	 		    
 //#if STDCPP_OOM 
 //failures++; 
 //#endif
		     
}

   catch(bad_alloc&)
   {
   //do nothing
   }
   
   catch(...)
   {
   	failures++;
   	
   }
  		  
	//	  __UHEAP_MARKEND;
		  if(failures)
		  return KErrGeneral;
		  return KErrNone;
    
    }

 
 // -----------------------------------------------------------------------------
// Ctstl_4::Islower
// -----------------------------------------------------------------------------
//
 
  TInt Ctstl_4::Islower( CStifItemParser& aItem )
    {

    // __UHEAP_MARK;
   
    char lower_char;
    int failures =0;
  

   try
   { 
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
    for(lower_char = 'a' ;lower_char<='z';lower_char++)
    {
         
    if(!islower(lower_char))
   failures++;
    	
    }
   		     	 		    
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif		     	 		    
 //#if STDCPP_OOM
  //failures++; 
  //#endif
	
}
   catch(bad_alloc&)
   {
  
   }
   catch(...)
   {
   	failures++;
   	
   }
      
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
    }

 // -----------------------------------------------------------------------------
// Ctstl_4::Isupper
// -----------------------------------------------------------------------------
//
 
 
 TInt Ctstl_4::Isupper( CStifItemParser& aItem )
    {

      //__UHEAP_MARK;
   
    char upper_char;
    
    int failures =0;
     

   try
   { 
  
    for(upper_char = 'A' ;upper_char<='Z';upper_char++)
    {
    
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif
    if(!isupper(upper_char))
   failures++;
    
    #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 	
    }
    		     	 		    
 //#if STDCPP_OOM 
 //failures++;
  //#endif
   }
   catch(bad_alloc&)
   {
   
   }
   catch(...)
   {
   	failures++;
   	
   }
      
     
    if(failures)
    return KErrGeneral;
    return KErrNone;
  }
 
 
 //  Ctstl_4::Maxof
 
 
 TInt Ctstl_4::Maxof( CStifItemParser& aItem )
    {
      int failures =0;
     

   try
   { 
     int result_int1;
     int result_int2;
     char result_char;
//     float result_float;
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif
     result_int1 = max(2,1);
     result_int2 = max(-2,-25);
     result_char = max('a','z');
       #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
     
  /*   
     if (result_int1 == 2)
     if(result_int2 == -2)
      if (result_char == 'z')
      if(max(2.23,2.24) == 2.24)  
      // __UHEAP_MARKEND;
     return KErrNone;
     return KErrGeneral;*/
     if (result_int1 != 2)
     failures++;
     if(result_int2 != -2)
     failures++;
      if (result_char != 'z')
      failures++;
      if(max(2.23,2.24) != 2.24)
      failures++;
    
 //#if STDCPP_OOM //failures++; //#endif
    
    
    }
   catch(bad_alloc&)
   {
   //do nothing
   }
   catch(...)
   {
   	failures++;
   	
   }
     
     
    if(failures)
    return KErrGeneral;
    return KErrNone; 
    
    	
    }
    
 
 // -----------------------------------------------------------------------------
 // Ctstl_4::ExampleL
 // Example test method function.
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 //
 TInt Ctstl_4::Advance( CStifItemParser& aItem )
     {
    
  	
  int failures=0 ; 

  


    try
    {
    
     list<string>::iterator iList1,iList2;
   list<string> List;
     List.push_back("A1");
     List.push_back("B2");
     List.push_back("C3");
     List.push_back("D4");
     List.push_back("E5");
     List.push_back("F6");
     List.push_back("G7");
 //    cout<<"";
  #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
   #endif 

     iList1=List.begin();
    	advance(iList1,2);
  	iList2 = List.begin();
  	advance(iList2,0);
  	 #if  STDCPP_OOM 	
   User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

   
  #endif 
 if(*iList1 != "C3")
 failures++;

 if(*iList2 != "A1")	
 failures++;

  //#if STDCPP_OOM
 //failures++;
 // #endif
 /* 
 if(*iList1 == "C3")
 if(*iList2 == "A1")
 return KErrNone;
 return KErrGeneral;
 */
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





 TInt Ctstl_4::Backinsert_iterator( CStifItemParser& aItem )
     {
     int failures=0;
     try
     {
     	
     

     int i;
 int output[4];
    vector<int> vec;
   for (i = 1 ; i < 4 ; ++i )  
    {
       vec.push_back (  i );
    }
    
   
    vector <int>::iterator vIter;



    back_insert_iterator<vector<int> >::container_type vec1 = vec; 
 //   cout<<"";
  #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    back_inserter ( vec1 ) = 40;

    #if  STDCPP_OOM 	
   User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

   
  #endif
    for ( vIter = vec1.begin ( ) ; vIter != vec1.end ( ); vIter++)
      
      
    {
    	
    static int i;
    output[i] = *vIter;
    i++;
    
    } 
    
    
    if(output[0] != 1)
    failures++;
  if(output[1] != 2)
  failures++;
  if(output[2] != 3)
  failures++;
  if(output[3] != 40)
  failures++;
    
 /*   
  if(output[0] == 1)
  if(output[1] == 2)
  if(output[2] == 3)
  if(output[3] == 40)
 return KErrNone;
 return KErrGeneral;
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



 TInt Ctstl_4::Frontinsert_iterator( CStifItemParser& aItem )
     {
 int failures=0;
 try{
     int i;
    list <int>::iterator L_Iter;
 int output[12];
    list<int> L;
    for (i = -1 ; i < 9 ; ++i )  
    {
       L.push_back ( 2 * i );
    }
 // cout<<"";
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif

    // Using the member function to insert an element
    front_inserter ( L ) = 20;

    // Alternatively, one may use the template function
    front_insert_iterator< list < int> > Iter(L);
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
    *Iter = 30;

     for ( L_Iter = L.begin( ) ; L_Iter != L.end( ); L_Iter++)
       
       
     {
     	static int i;
     	output[i] = *L_Iter;
     	i++;
     }
  if(output[0] != 30)
  failures++;
 if(output[1] != 20)
 failures++;
 if(output[2] != -2)
 failures++;
 if(output[3] != 0)      
 failures++;

     //#if STDCPP_OOM
 //failures++;
 // #endif 
    
  /*  
 if(output[0] == 30)
 if(output[1] == 20)
 if(output[2] == -2)
 if(output[3] == 0)
 return KErrNone;
 return KErrGeneral;*/

    
    
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




 TInt Ctstl_4::Insert_iterator( CStifItemParser& aItem )
     {
 int failures=0;
 try{
     int i;
    list <int>::iterator L_Iter;
 int output[5];
    list<int> L;
    for (i = 2 ; i < 5 ; ++i )  
    {
       L.push_back ( 10 * i );
    }
 // cout<<"";
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif

    // Using the template version to insert an element
    insert_iterator<list <int> > Iter( L, L.begin ( ) );
    *Iter = 1;

    // Alternatively, using the member function to insert an element
    inserter ( L, L.end ( ) ) = 500;
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     for ( L_Iter = L.begin( ) ; L_Iter != L.end( ); L_Iter++)
         
    {
    	static int i;
    output[i] = *L_Iter;
    i++;
    } 
    
   if(output[0] != 1)
   failures++;
  if(output[4] != 500)  
  failures++;
    /*
  if(output[0] == 1)
  if(output[4] == 500)

 return KErrNone;
 return KErrGeneral;
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

 TInt  Ctstl_4::Reverse_iterator( CStifItemParser& aItem )

 {
 int failures=0;
 try{
 int i;
  

    vector<int> vec;
    
    for ( i = 1 ; i < 6 ; ++i )
    {
       vec.push_back ( i );
    }
    
   // vector <int>::iterator vIter;
     

//    vector <int>::reverse_iterator rvIter;
     
    vector <int>::iterator pos;
    
    pos = find ( vec.begin ( ), vec.end ( ), 4 );
   
   
   if(*pos != 4)
   failures++;
 //   cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
      vector <int>::reverse_iterator rpos ( pos );
       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   if(*rpos != 3)
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

 TInt  Ctstl_4::Distance( CStifItemParser& aItem )

 {
 int failures =0;
 try{
 int i;
  

    list<int> L;
    for ( i = -1 ; i < 9 ; ++i ) 
    {
       L.push_back ( 2 * i );
    }
    
    
    list <int>::iterator  LPOS = L.begin ( );

   
  

    advance ( LPOS , 7 );
    
     list<int>::difference_type Ldiff ;
  //   cout<<"";
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    Ldiff = distance ( L.begin ( ) , LPOS );
    
       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     /*if(*LPOS ==12)
     if(Ldiff==7)
     return KErrNone;
     return KErrGeneral;*/
     
  if(*LPOS !=12)
  failures++;
     if(Ldiff!=7)
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
  
   
  
  
  
  
   
    //#if STDCPP_OOM
 //failures++;
 // #endif 

  
 	

 TInt  Ctstl_4::Isdigit( CStifItemParser& aItem )

{

char str[]="1776ad";
  
  int failures =0;
  try{ 
 // 	cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
  if (!isdigit(str[0]))
  failures++;
  if (!isdigit(str[1]))
  failures++;
  if (!isdigit(str[2]))
  failures++;
  if (!isdigit(str[3]))
  failures++;
  if (isdigit(str[4]))
  failures++;
  if (isdigit(str[5]))
  failures++;
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
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


 TInt  Ctstl_4::Isgraph( CStifItemParser& aItem )

{

 int failures =0;
 // char array[42] = "!"#$%&'()*+,-./0123456789:;<=>?@[\]^_`{|}~";
 
 
  try{
  
  for(char i=0x21;i<=0x7E;i++)
  {
//  	 cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
  
  if(!isgraph(i))
  
  failures++;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
  
  
  }
 
   
  
 
  
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
     
     

  TInt  Ctstl_4::Isalnum( CStifItemParser& aItem )

 {

  int failures =0;
   
  
   try{
   
   for(char text='a';text<='z';text++)
   {
   
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!isalnum(text))
   failures++;
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
   
   
   }
   
   
   for(char text='A';text<='Z';text++)
   {
   
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!isalnum(text))
   failures++;
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
   
   
   }
   
    
    
    for(int text='0';text<='9';text++)
   {
   
   
   if(!isalnum(text))
   failures++;
   
   
   }
   
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
 TInt  Ctstl_4::Isalpha( CStifItemParser& aItem )

 {

  int failures =0;
   
  try{
   
   
   for(char text='a';text<='z';text++)
   {
   
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!isalpha(text))
   failures++;
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
   
   
   }
   
   
   for(char text='A';text<='Z';text++)
   {
   
   
   if(!isalpha(text))
   failures++;
   
   
   }
   
    
    
    for(int text='0';text<='9';text++)
   {
   
   
   if(isalpha(text))
   failures++;
   
   
   }
   
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



 TInt  Ctstl_4::Iscntrl( CStifItemParser& aItem )

 {
 	 int failures =0;

 try{
   
  
   
   
   for(char text=0x00;text<=0x1F;text++)
   {
   
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!iscntrl(text))
   failures++;
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
     
   }
   
   if(!iscntrl(0x7F))
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


 TInt  Ctstl_4::Isprint( CStifItemParser& aItem )

 {

  int failures =0;
   
  try{
   
   
   for(char text=0x20;text<=0x7E;text++)
   {
   
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!isprint(text))
   failures++;
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     
   }
   
     
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


 TInt  Ctstl_4::Ispunct( CStifItemParser& aItem )

 {

  int failures =0;
     
   try{
   for(char text=0x21;text<=0x2F;text++)
   {
       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!ispunct(text))
   failures++; 
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
     
   }
   
   for(char text=0x3A;text<=0x40;text++)
   {
     
   if(!ispunct(text))
   failures++;
     
   }
   
   for(char text=0x5B;text<=0x60;text++)
   {
     
   if(!ispunct(text))
   failures++;
     
   }
   
   for(char text=0x7B;text<=0x7E;text++)
   {
   
   
   if(!ispunct(text))
   failures++;
     
   }
   
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
   TInt  Ctstl_4::Isspace( CStifItemParser& aItem )

 {

  int failures =0;
   
  try{
   
   
   for(char text=0x09;text<=0x0D;text++)
   {
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   if(!isspace(text))
   failures++;
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
     
   }
   
  if(!isspace(0x20))
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

  TInt  Ctstl_4::Isxdigit( CStifItemParser& aItem )

 {

  int failures =0;
 try{
   for(char text=0x30;text<=0x39;text++)
   {
   
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   if(!isxdigit(text))
   failures++;
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
     
   }
   
  
   for(char text=0x41;text<=0x46;text++)
   {
   
   
   if(!isxdigit(text))
   failures++;
     
   }
   
   
   for(char text=0x61;text<=0x66;text++)
   {
   
   
   if(!isxdigit(text))
   failures++;
     
   }
   
   
   
     /*
   
   if(failures)
 return KErrGeneral;
   return KErrNone;*/
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


 TInt  Ctstl_4::Fill( CStifItemParser& aItem )

 {

  int failures =0;
   
  try
  {
  
  	 vector<int> myvector (8,10);        // myvector: 10 10 10 10 10 10 10 10
  	 vector<int>::iterator it ; 
 // 	 cout<<"";
  	   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
    	 fill_n(myvector.begin(),4,20);     // myvector: 20 20 20 20 10 10 10 10
    	   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
   		for(  it = myvector.begin();it!=myvector.begin()+4;++it)
   		{
   	
   			if(*it!=20)
   			failures++;
   	
   		}
   
     
   
   
   	vector<int> myvector1 (8);
 //  	cout<<"";  
   	  #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif                     // myvector: 0 0 0 0 0 0 0 0
    	fill (myvector1.begin(),myvector1.begin()+4,5);
    	  #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  

    		for (vector<int>::iterator it=myvector1.begin(); it!=myvector1.begin()+3; ++it)
    		{
    	
    			if(*it!=5)
   		 	failures++;
    
    		}

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





 static int UniqueNumber()
  {
  
  static int current;
  return current++;	
  }


 TInt  Ctstl_4::Generate( CStifItemParser& aItem )

 {
 	int failures = 0;

 try{
   	vector<int> myvector (8);
   	vector<int>::iterator it;
 	int output[8];
 //  cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   generate(myvector.begin(), myvector.end(), UniqueNumber);
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif

    for (it=myvector.begin(); it!=myvector.end(); ++it)
    
    {
    	static int i;
    output[i] = *it;
    i++;
    
    
    }
  
  
  for(int i=0;i<=7;i++)
  {
  if(!output[i] == i)
  failures++;	
  }
 //   cout<<"";
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   generate_n(myvector.begin()+5,3,UniqueNumber);
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   
   
   for(int i=5;i<=7;i++)
  {
  if(!output[0] == 0)
  if(!output[1]==0)
  if(!output[2]==0)
  if(!output[3]==0)
  if(!output[4]==0)
  if(!output[i] == i)
  failures++;	
  }
   
   
    
   /*
   if(!failures)
     return KErrNone;
    return KErrGeneral;
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
    
 		  if(failures )
 		  return KErrGeneral;
 		  return KErrNone;
     }




 bool mypredicate1 (int i, int j) {
   return (i==j);
 }



 TInt  Ctstl_4::Search( CStifItemParser& aItem )

 {
 	int failures=0;
 try{

 vector<int> myvector;
   vector<int>::iterator it;

   // set some values:        myvector: 10 20 30 40 50 60 70 80 90
   for (int i=1; i<10; i++) 
   myvector.push_back(i*10);


   // using default comparison:
   int match1[] = {40,50,60,70};
 //   cout<<"";
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   it = search (myvector.begin(), myvector.end(), match1, match1+4);

      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
  if(int(it-myvector.begin())!=3)

 failures++;


 // using predicate comparison:
   int match2[] = {20,30,50};
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   it = search (myvector.begin(), myvector.end(), match2, match2+3, mypredicate1);

      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     
      if(int(it-myvector.begin()) == 1)
      failures++;
      
  //#if STDCPP_OOM
 //failures++;
 // #endif
 //return KErrNone;
 //return KErrGeneral;
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




 TInt  Ctstl_4::Search_n(CStifItemParser& aItem )

 {
 	
 	int failures=0;
 try{
 int myints[]={10,20,30,30,20,10,10,20};
   vector<int> myvector (myints,myints+8);

   vector<int>::iterator it; 
 //  cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   // using default comparison:
   it = search_n (myvector.begin(), myvector.end(), 2, 30);

       #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif


 /*
 if(int(it-myvector.begin())==2)
 return KErrNone;
 return KErrGeneral;
 */

 if(int(it-myvector.begin())!=2)
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
 //code to be verified
 //--------------------------

 TInt Ctstl_4::Times(CStifItemParser& aItem)
  {
  int failures=0;
  
 //__UHEAP_MARK;
 try
 {
 	

 int input1 [4] = { -10, 20, 30, 40 };
 int input2 [4] = { 40, 30, 20, -10 };
    int output [4];
     #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, multiplies<int>());
    #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   if( output[1] != 600)
   failures++;
   if( output[2] != 600 )
   failures++;
   
   if( output[3] != -400)
   failures++;
   
     
    
   
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
  
  TInt Ctstl_4::Unique(CStifItemParser& aItem)
  {
  	
  int failures=0;
 //__UHEAP_MARK;
 try{
 int myints[] = {10,20,20,20,30,30,20,20,10};    // 10 20 20 20 30 30 20 20 10
   vector<int> myvector (myints,myints+9);
   vector<int>::iterator it;
 int output[5];
 // cout<<"";
  #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif

   // using default comparison:
   it = unique (myvector.begin(), myvector.end()); // 10 20 30 20 10 30 20 20 10
                                                   //                ^
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   myvector.resize( it - myvector.begin() );       // 10 20 30 20 10

   
   // print out content:
    for (it=myvector.begin(); it!=myvector.end(); ++it)
     
   {
   	
   static int i;
    output[i] = *it;
   i++;
   
   }
     
     
    /*
     
     if(output[0]== 10)
     if(output[1]== 20)
     if(output[2]== 30)
     if(output[3]== 20)
     if(output[4]== 10)
     
     return KErrNone;
     return KErrGeneral;*/
  //  __UHEAP_MARKEND;
  
  if(output[0]!=10)
 	failures++;
 	
   if(output[1]!=20)
   failures++;
   
   if(output[2]!=30)
   failures++;
   if(output[3]!=20)
   failures++;
   if(output[4]!=10)
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
  
  TInt Ctstl_4::Uniquecopy(CStifItemParser& aItem)
  {
  	int failures=0;
  	
  try{
 //__UHEAP_MARK;
 int myints[] = {10,20,20,20,30,30,20,20,10};
   vector<int> myvector (9);                            // 0  0  0  0  0  0  0  0  0
   vector<int>::iterator it;
 int output[9]; 
 //cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   // using default comparison:
   it=unique_copy(myints,myints+9,myvector.begin());   // 10 20 30 20 10 0  0  0  0
          #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif                                                //                ^
 for (it=myvector.begin(); it!=myvector.end(); ++it)
 {
 static int i;
 output[i] = *it;
 i++;	
 }
                                                           // 10 20 30 20 30 0  0  0  0
    /*                                                    //          ^
  if(output[0]==10)
  if(output[1]==20)
  if(output[2]==30)
  if(output[3]==20)
  if(output[4]==10)
  if(output[5]==0)
  if(output[6]==0)
  if(output[7]==0)
  if(output[8]==0)
  return KErrNone;
  return KErrGeneral;
  */
  if(output[0]!=10)
 	failures++;
 	
   if(output[1]!=20)
   failures++;
   
   if(output[2]!=30)
   failures++;
   if(output[3]!=20)
   failures++;
   if(output[4]!=10)
   failures++;
   if(output[5]!=0)
   failures++;
   if(output[6]!=0)
   failures++;
   if(output[7]!=0)
   failures++;
    if(output[8]!=0)
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
  
   
  TInt Ctstl_4::Partialsort(CStifItemParser& aItem)
  {
  	int failures=0;
  	try
  	{
  
 //__UHEAP_MARK;
 int myints[] = {9,8,7,6,5,4,3,2,1};
   vector<int> myvector (myints, myints+9);
   vector<int>::iterator it;
 int output[9]; 
 //cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   // using default comparison (operator <):
   partial_sort (myvector.begin(), myvector.begin()+5, myvector.end());

      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
     for (it=myvector.begin(); it!=myvector.end(); ++it)
     {
     	static int i;
     	output[i] = *it;
     	i++;
     }
     
 /*
   if(output[0]==1)
   if(output[1]==2)
   if(output[2]==3)
   if(output[3]==4)
   if(output[5]==9)
   if(output[6]==8)
   if(output[7]==7)
   if(output[8]==6)
   if(output[4]==5)
   
   return KErrNone;
   return KErrGeneral;
 */
  
   // __UHEAP_MARKEND;
 if(output[0]!=1)
 	failures++;
 	
   if(output[1]!=2)
   failures++;
   
   if(output[2]!=3)
   failures++;
   if(output[3]!=4)
   failures++;
   if(output[4]!=5)
   failures++;
   if(output[5]!=9)
   failures++;
   if(output[6]!=8)
   failures++;
   if(output[7]!=7)
   failures++;
    if(output[8]!=6)
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
  

 TInt Ctstl_4::Partialsortcopy(CStifItemParser& aItem)
  {
  	int failures=0;
  try{
 //__UHEAP_MARK;
 int myints[] = {9,8,7,6,5,4,3,2,1};
   vector<int> myvector (5);
   vector<int>::iterator it;
 int output[5]; 
 //cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   // using default comparison (operator <):
   partial_sort_copy (myints, myints+9, myvector.begin(), myvector.end());

   
      #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif


 for (it=myvector.begin(); it!=myvector.end(); ++it)
     {
     	static int i;
     	output[i] = *it;
     	i++;
     }
     
 /*
   if(output[0]==1)
   if(output[1]==2)
   if(output[2]==3)
   if(output[3]==4)
   if(output[4]==5)
    
  return KErrNone;
  return KErrGeneral;*/
  
    //__UHEAP_MARKEND;
    
    if(output[0]!=1)
    failures++;
   if(output[1]!=2)
   failures++;
   if(output[2]!=3)
   failures++;
   if(output[3]!=4)
   failures++;
   if(output[4]!=5)
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
     
     
  
  bool IsOdd(int i)
  {
  	return (i%2)==1;
  }
  
  
  TInt Ctstl_4::Partition(CStifItemParser& aItem)
  {
  	int failures=0;
  try{
 //__UHEAP_MARK;
 //int myints[] = {9,8,7,6,5,4,3,2,1};
 vector<int> myvector;
   vector<int>::iterator it, bound;
 int Odd[5];
 int Even[4];
   // set some values:
   for (int i=1; i<10; ++i) myvector.push_back(i); // 1 2 3 4 5 6 7 8 9
 //   cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   bound = partition (myvector.begin(), myvector.end(), IsOdd);
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   // print out content:
    for (it=myvector.begin(); it!=bound; ++it)
     
    {
    	static int i;
    	Odd[i] = *it;
    	i++;
    } 

   
   for (it=bound; it!=myvector.end(); ++it)
     
   {
   	static int i;
   	Even[i]=*it;
   	i++;
   }

 /*   
 if(Odd[0] ==1)
 if(Odd[1] ==9)
 if(Odd[2] ==3)
 if(Odd[3] ==7)
 if(Odd[4] ==5)
 if(Even[0]==6)
  if(Even[1]==4)
  if(Even[2]==8)
  if(Even[3]==2)
    
  return KErrNone;
  return KErrGeneral;*/
  if(Odd[0] !=1)
    failures++;
    
 if(Odd[1] !=9)
 failures++;
 if(Odd[2] !=3)
 failures++;
 if(Odd[3] !=7)
 failures++;
 if(Odd[4] !=5)
 failures++;
 if(Even[0]!=6)
 failures++;
 if(Even[1]!=4)
 failures++;
  if(Even[2]!=8)
  failures++;
  if(Even[3]!=2)
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
  
  
  
   TInt Ctstl_4::Stablepartition(CStifItemParser& aItem)
  {
  	int failures=0;
  try{
 //__UHEAP_MARK;
 //int myints[] = {9,8,7,6,5,4,3,2,1};
 vector<int> myvector;
   vector<int>::iterator it, bound;
 int Odd[5];
 int Even[4];
   // set some values:
   for (int i=1; i<10; ++i) myvector.push_back(i); // 1 2 3 4 5 6 7 8 9
 //   cout<<"";
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   bound = stable_partition (myvector.begin(), myvector.end(), IsOdd);
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   // print out content:
    for (it=myvector.begin(); it!=bound; ++it)
     
    {
    	static int i;
    	Odd[i] = *it;
    	i++;
    } 

   
   for (it=bound; it!=myvector.end(); ++it)
     
   {
   	static int i;
   	Even[i]=*it;
   	i++;
   }

    /*
 if(Odd[0] ==1)
 if(Odd[1] ==3)
 if(Odd[2] ==5)
 if(Odd[3] ==7)
 if(Odd[4] ==9)
 if(Even[0]==2)
 if(Even[1]==4)
  if(Even[2]==6)
  if(Even[3]==8)
    
  return KErrNone;
  return KErrGeneral;*/
  
    //__UHEAP_MARKEND;
    
    if(Odd[0] !=1)
    failures++;
    
 if(Odd[1] !=3)
 failures++;
 if(Odd[2] !=5)
 failures++;
 if(Odd[3] !=7)
 failures++;
 if(Odd[4] !=9)
 failures++;
 if(Even[0]!=2)
 failures++;
 if(Even[1]!=4)
 failures++;
  if(Even[2]!=6)
  failures++;
  if(Even[3]!=8)
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


 TInt Ctstl_4::Setdifference(CStifItemParser& aItem)
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
 //  cout<<""; 
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   it=set_difference (first, first+5, second, second+5, v.begin());
                                                // 5 15 25  0  0  0  0  0  0  0
   #if  STDCPP_OOM
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
 for(it=v.begin();it!=v.end();++it)
 {
 	
 static int i;
 output[i] = *it;
 i++;

 }
  
   /*if(output[0]==5)
   if(output[1]==15)
   if(output[2]==25)
   return KErrNone;
   return KErrGeneral;*/
    if(output[0]!=5)
     failures++;
   if(output[1]!=15)
   failures++;
   if(output[2]!=25)
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
  
  



 
//  End of File
