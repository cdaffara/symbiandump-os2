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
#include <locale>
#include <iostream>
#include<algorithm>  
#include<set>
#include<functional>
#include <vector>
#include <numeric>
#include<list>
#include<exception>
#include <valarray>
#include<memory>
#include<fstream> 
#include<ios>
#include<exception>

#include "tstl_1.h"
 
using namespace std;

#define STDCPP_OOM FALSE// TRUE for OOM testing
  
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctstl_1::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctstl_1::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctstl_1::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "ToUpper", Ctstl_1::ToUpper ),
        ENTRY( "ToLower", Ctstl_1::ToLower ),
        ENTRY( "Islower", Ctstl_1::Islower ),
        ENTRY( "Isupper", Ctstl_1::Isupper ),
        ENTRY( "Maxof", Ctstl_1::Maxof ),
        ENTRY( "Minof", Ctstl_1::Minof ),
        ENTRY( "Maxelement", Ctstl_1::Maxelement ),
        ENTRY( "Minelement", Ctstl_1::Minelement ),
        ENTRY( "Lowerbound", Ctstl_1::Lowerbound ),
        ENTRY( "Upperbound", Ctstl_1::Upperbound ),
        ENTRY( "Swapof", Ctstl_1::Swapof ),
        ENTRY( "Accumulate", Ctstl_1::Accumulate ),
 		ENTRY( "Adjacent_difference", Ctstl_1::Adjacent_difference),
        ENTRY( "Adjacent_find", Ctstl_1::Adjacent_find),
        ENTRY( "Logical_or", Ctstl_1::Logical_or ),
        ENTRY( "Logical_and", Ctstl_1::Logical_and ),
        ENTRY( "Logical_not", Ctstl_1::Logical_not ),
        ENTRY( "count_fun", Ctstl_1::count_fun),
        ENTRY( "countif_fun", Ctstl_1::countif_fun),
        ENTRY( "equal_fun", Ctstl_1::equal_fun),
        ENTRY( "equalto", Ctstl_1::equalto),
        ENTRY( "notequalto", Ctstl_1::notequalto),
 		ENTRY( "sort_fun", Ctstl_1::sort_fun),
 		ENTRY( "less_equal_fun", Ctstl_1::less_equal_fun),
 		ENTRY( "greater_equal_fun", Ctstl_1::greater_equal_fun),
 		ENTRY( "greater_fun", Ctstl_1::greater_fun),
 		ENTRY( "less_fun", Ctstl_1::less_fun),
 		ENTRY( "find_fun", Ctstl_1::find_fun),
 		ENTRY( "findif_fun", Ctstl_1::findif_fun),
 		ENTRY( "findend_fun", Ctstl_1::findend_fun),
 		ENTRY( "findfirstof_fun", Ctstl_1::findfirstof_fun),
 		ENTRY( "Minus", Ctstl_1::Minus),
        ENTRY( "Plus", Ctstl_1::Plus),
        ENTRY( "Divides", Ctstl_1::Divides),
        ENTRY( "Modulus", Ctstl_1::Modulus),
        ENTRY( "Negate", Ctstl_1::Negate),
        ENTRY( "Logicalnot", Ctstl_1::Logicalnot),
        ENTRY( "Logicaland", Ctstl_1::Logicaland),
        ENTRY( "Logicalor", Ctstl_1::Logicalor),
        ENTRY( "Remove", Ctstl_1::Remove),
        ENTRY( "Removeif", Ctstl_1::Removeif),
        ENTRY( "Removecopy", Ctstl_1::Removecopy),
        ENTRY( "Removecopy_if", Ctstl_1::Removecopy_if),
        ENTRY( "Replace_fun", Ctstl_1::Replace_fun),
        ENTRY( "Replacecopy_fun", Ctstl_1::Replacecopy_fun),
        ENTRY( "Replacecopyif_fun", Ctstl_1::Replacecopyif_fun),
        ENTRY( "Replaceif_fun", Ctstl_1::Replaceif_fun),
        ENTRY( "Reverse_fun", Ctstl_1::Reverse_fun),
        ENTRY( "Reversecopy_fun", Ctstl_1::Reversecopy_fun),
        ENTRY( "Rotate_fun", Ctstl_1::Rotate_fun),
        ENTRY( "Rotatecopy_fun", Ctstl_1::Rotatecopy_fun),
 		ENTRY( "equalrange", Ctstl_1::equalrange),
 		ENTRY( "exceptionL", Ctstl_1::exceptionL),
        ENTRY( "foreach", Ctstl_1::foreach),
 		ENTRY( "gsliceL", Ctstl_1::gsliceL),
        ENTRY( "sliceL", Ctstl_1::sliceL),
 		ENTRY( "unaryfunc", Ctstl_1::unaryfunc),
 		ENTRY( "uni_fill", Ctstl_1::uni_fill),
 		ENTRY( "uni_fill_n", Ctstl_1::uni_fill_n),
 		ENTRY( "uni_copy", Ctstl_1::uni_copy),
 		ENTRY( "maskarray", Ctstl_1::maskarray),
	    ENTRY( "autoptr", Ctstl_1::autoptr),
 		ENTRY( "randshuf", Ctstl_1::randshuf),
 		ENTRY( "randstoriter", Ctstl_1::randstoriter),
 		ENTRY( "iterswap", Ctstl_1::iterswap),
 		ENTRY( "ios1", Ctstl_1::ios1),
 		ENTRY( "ios2", Ctstl_1::ios2),
 		ENTRY( "ios3", Ctstl_1::ios3),
 		ENTRY( "nthelement", Ctstl_1::nthelement),
 		ENTRY( "hasfacet", Ctstl_1::hasfacet),


        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctstl_1::ToUpper
 // -----------------------------------------------------------------------------
//
TInt Ctstl_1::ToUpper( CStifItemParser& aItem )
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
// Ctstl_1::ToLower
// -----------------------------------------------------------------------------
//
    
    
    
   
    TInt Ctstl_1::ToLower( CStifItemParser& aItem )
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
// Ctstl_1::Islower
// -----------------------------------------------------------------------------
//
 
  TInt Ctstl_1::Islower( CStifItemParser& aItem )
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
// Ctstl_1::Isupper
// -----------------------------------------------------------------------------
//
 
 
 TInt Ctstl_1::Isupper( CStifItemParser& aItem )
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
 
 
 //  Ctstl_1::Maxof
 
 
 TInt Ctstl_1::Maxof( CStifItemParser& aItem )
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
    
   
   // Ctstl_1::Minof
    
    
    TInt Ctstl_1::Minof( CStifItemParser& aItem )
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
    
    
    
   TInt Ctstl_1::Maxelement(CStifItemParser& aItem)
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
 
 
 
 
 
 TInt Ctstl_1::Minelement(CStifItemParser& aItem)
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
 
 
 //lowerbound
 
 TInt Ctstl_1::Lowerbound(CStifItemParser& aItem)
 {
 
 
 //__UHEAP_MARK;
  
     int failures =0;
    

   try
   { 
     
 	
 
  set<int> myset;
 
  set<int>::iterator it,itlow;

int array[2];

  for (int i=1; i<10; i++) 
  myset.insert(i*10); // 10 20 30 40 50 60 70 80 90
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  itlow=myset.lower_bound(30);  
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif               
  
   
   for (it=myset.begin(); it!=itlow; it++)
  {
  	
 static int j =0;
 
  array[j] = (*it);
   j++;
 }
 
 
  
 
 myset.erase(myset.begin(),myset.end());
 
 /*
   if(array[0] == 10 && array[1] == 20  )
 
   // __UHEAP_MARKEND;
   return KErrNone;
   return KErrGeneral;*/

 if(array[0] != 10)
 failures++;
 if( array[1] != 20)
 failures++;
 
 
 
 
 //  #if STDCPP_OOM
  // failures++;
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

 
//upperbound
 
  TInt Ctstl_1::Upperbound(CStifItemParser& aItem)
 {
 
 
// __UHEAP_MARK;
    int failures =0;
    

   try
   { 
     
  set<int> myset;
  set<int>::iterator it,itlow,itup;




int array[6];

  for (int i=1; i<10; i++) 
 
  myset.insert(i*10); 
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  itup=myset.upper_bound (30);               
 
 #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   for (it=itup; it!=myset.end(); it++)
  {
  	
  
  static  int j =0;
   array[j] = (*it);
   j++;
 }
 
 
 /*
   if(array[0] == 40 )
   if(array[1] == 50 )
   if(array[2] == 60 )
   if(array[3] ==70 )
   if(array[4] == 80 )
   if(array[5] == 90 )
   
 
    // __UHEAP_MARKEND; 

   return KErrNone;
   return KErrGeneral;*/
   
    if(array[0] != 40 )
    failures++;
   if(array[1] != 50 )
   failures++;
    if(array[2] != 60 )
    failures++;
   if(array[3] !=70 )
   failures++;
   if(array[4] != 80 )
   failures++;
   if(array[5] != 90 )
   failures++;
    
    
   //#if STDCPP_OOM //failures++; //#endif
    }
    
   catch(exception&)
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
 
 
 
 
 
 //swap
 
  TInt Ctstl_1::Swapof(CStifItemParser& aItem)
 {
 int x= 10,y=20;
 
 //__UHEAP_MARK;
 int failures =0;
    

   try
   {  // cout<<"";
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
 
  
  
   TInt Ctstl_1::Accumulate(CStifItemParser& aItem)
 {
      
 int failures =0;
 
     // __UHEAP_MARK;
      
   try
   {
   	
   
    int test_array[10] = {1,2,3,4,5,6,7,8,9,10};
    vector<int> v1(test_array, test_array+10);
      // cout<<"";
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
 
 
 
 
 
  TInt Ctstl_1::Adjacent_difference(CStifItemParser& aItem)
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
//   list <int>::iterator LIter1, LIterend, LIterend2;

   int t;
   for ( t = 1 ; t <= 10 ; t++ )
   {
      L1.push_back( t * t );
   }
 // cout<<"";
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
 
 
 
 TInt Ctstl_1::Adjacent_find(CStifItemParser& aItem)
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
  vector<int>::iterator it; // cout<<"";
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
 TInt Ctstl_1::Logical_or(CStifItemParser& aItem)
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
 
 TInt Ctstl_1::Logical_and(CStifItemParser& aItem)
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
 
 
  TInt Ctstl_1::Logical_not(CStifItemParser& aItem)
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
 
 
 
 TInt Ctstl_1::count_fun(CStifItemParser& aItem)
 {
 	
 	
 int failures =0;
    

   try
   {    //	__UHEAP_MARK;
 	
 int sequence[10] = {1,2,3,4,5,5,7,8,9,10};
    int i=0,j=0,k=0;
     vector<int> v(sequence,sequence + 10); // cout<<"";
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
  TInt Ctstl_1::countif_fun(CStifItemParser& aItem)
 {
 
 //__UHEAP_MARK;
 	int failures =0;
 
   try
   {   
	int sequence[10] = {1,2,3,4,5,5,7,8,9,10};
    int i=0 ,j=0;
    vector<int> v(sequence,sequence + 10); // cout<<"";
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
 
 
  TInt Ctstl_1::equal_fun(CStifItemParser& aItem)
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
  
  TInt Ctstl_1::equalto(CStifItemParser& aItem)
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

TInt Ctstl_1::notequalto(CStifItemParser& aItem)
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
  
  TInt Ctstl_1::sort_fun(CStifItemParser& aItem)
  {
  
  int failures =0;
    

   try
   {   
 // __UHEAP_MARK;
  int sequence[7] = {6262, 6262, 41 ,18467, 6334, 26500, 19169 };
     vector <int>::iterator Iter1;

int output[7];

    vector<int> v1(sequence,sequence + 7); // cout<<"";
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
  
  TInt Ctstl_1::less_equal_fun(CStifItemParser& aItem)
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
  
  TInt Ctstl_1::greater_equal_fun(CStifItemParser& aItem)
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
  
  TInt Ctstl_1::greater_fun(CStifItemParser& aItem)
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
  
  TInt Ctstl_1::less_fun(CStifItemParser& aItem)
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
  
  TInt Ctstl_1::find_fun(CStifItemParser& aItem)
  {
 // __UHEAP_MARK;		
	int failures =0;
    
   try
   {     typedef vector<int>::iterator iterator;
    int d1[10] = {0,1,2,2,3,4,2,2,6,7};

     vector<int> v1(d1,d1 + 10); // cout<<"";
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
  
  
  TInt Ctstl_1::findif_fun(CStifItemParser& aItem)
  {
  
//__UHEAP_MARK;
  	
 	int failures =0;
    

   try
   {    typedef vector<int>::iterator iterator;
    int d1[10] = {0,1,2,2,3,4,2,2,6,7};

     vector<int> v1(d1,d1 + 10); // cout<<"";
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
  
  TInt Ctstl_1::findend_fun(CStifItemParser& aItem)
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
//    vector <int>::iterator Iter1, Iter2,Iter3 ,Iter4;
 
  
    

   vector <int>::iterator result1;


   #if  STDCPP_OOM
	User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
 	 #endif
   result1 = find_end( v1.begin( ), v1.end( ), v3.begin( ), v3.end( ) );
   
//   result2 = find_end ( v1.begin( ), v1.end( ), v4.begin( ), v4.end( ) );
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
  
  
  //find_first_of
  
  TInt Ctstl_1::findfirstof_fun(CStifItemParser& aItem)
  {
  
  	int failures =0;
    

   try
   {   
 // __UHEAP_MARK;
   int array1[12] = {0,5,10,15,20,25,0,5,10,15,20,25};
  int array2[3]    =  {20,30,40 };
  int array3[4]= {5,10,15,20 };
  int array4[4] ={30,40,50,60  }; //not found iterator
  
  
  vector <int> v1(array1,array1+12), v2(array2,array2+3),v3(array3,array3+4);
    vector<int> v4(array4,array4+4);
//    vector <int>::iterator Iter1, Iter2,Iter3 ,Iter4;
 
  
    

   
   vector <int>::iterator result1,result2;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
   result1 = find_first_of( v1.begin( ), v1.end( ), v3.begin( ), v3.end( ) );
   
   result2 = find_first_of( v1.begin( ), v1.end( ), v4.begin( ), v4.end( )   );
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif
  if((result1 - v1.begin() == 1) && (result1 - v1.begin( ) != 7))
  
  
  
 /* if(result2 == v1.end()) // if not found 
     // __UHEAP_MARKEND;
  
 return KErrNone;
 return KErrGeneral;*/
 
  if(result2 != v1.end())
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
  



TInt Ctstl_1::Minus(CStifItemParser& aItem)
 {
 	
 	int failures =0;
   
   try
   {   
//__UHEAP_MARK;
int input1 [4] = { 10, 20, 30, 40 };
int input2 [4] = { 40, 30, 20, 10 };
   int output [4];
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 

  transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, minus<int>());
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
  
   /* if(output[0] == -30 && output[1] == -10 && output[2] ==10 && output[3] == 30)
    
    // __UHEAP_MARKEND;
 return KErrNone;
 return KErrGeneral;  */
	if(output[0] != -30)
 failures++;
 if(output[1] != -10)
 failures++;
 if(output[2] != 10)
 failures++;
 if( output[3] != 30)
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


TInt Ctstl_1::Plus(CStifItemParser& aItem)
 {
 	
	int failures =0;
    

   try
   {    
//__UHEAP_MARK;
int input1 [4] = { -10, 20, 30, 40 };
int input2 [4] = { 40, 30, 20, -10 };
   int output [4];
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif  
  transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, plus<int>());
  
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   /* if(output[0] == 30 && output[1] == 50 && output[2] ==50 && output[3] == 30)
    
   //  __UHEAP_MARKEND;
 return KErrNone;
 return KErrGeneral;  
 */
	if(output[0] != 30)
 failures++;
 if(output[1] != 50)
 failures++;
 if(output[2] != 50)
 failures++;
 if( output[3] != 30)
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



  TInt Ctstl_1::Divides(CStifItemParser& aItem)
 {
  
 int input1 [4] = { -10, 20, 30,40 };
int input2 [4] = { 40, 30, 20,4 };
   int output [4];
     
  	
   
 transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, divides<int>());
  
  
    if(output[0] == (-10/40) && output[1] == (20/30) && output[2] ==(30/20))
    return KErrNone;
  return KErrGeneral;
 
    
    
    
   
  
 }



TInt Ctstl_1::Modulus(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    

   try
   {   
//__UHEAP_MARK;
int input1 [4] = { -10, 20, 30, 40 };
int input2 [4] = { 40, 30, 20, -10 };
   int output [4];
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, modulus<int>());
  
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
    /*if(output[0] == (-10%40) && output[1] == (20%30) && output[2] ==(30%20) && output[3] == (40%-10))
    
     //__UHEAP_MARKEND;
 return KErrNone;
 return KErrGeneral;  */
 
 	if(output[0] != (-10%40))
 failures++;
 if(output[1] != (20%30))
 failures++;
 if(output[2] != (30%20))
 failures++;
 if( output[3] != (40%-10))
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


TInt Ctstl_1::Negate(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    

   try
   {   
//__UHEAP_MARK;
int input1 [4] = { 10, 20, 30, 40 };
    int output[4];
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  transform((int*)input1, (int*)input1 + 4 , (int*)output, negate<int>());
  
   #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   /* if(output[0] == -10 && output[1] == (-20) && output[2] ==(-30) && output[3] == (-40))
    
   
    //__UHEAP_MARKEND;
   
 return KErrNone;
 return KErrGeneral;  */
 if(output[0] != -10)
 failures++;
 if(output[1] != -20)
 failures++;
 if(output[2] != -30)
 failures++;
 if( output[3] != -40)
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

 
 
 
 TInt Ctstl_1::Logicalnot(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    

   try
   {   
//__UHEAP_MARK;
bool input1 [2] = { 0,1 };
 
   bool output [2];
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  transform((bool*)input1, (bool*)input1 + 2,   (bool*)output, logical_not<bool>());
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
  
  
    /*if(output[0] == 1 && output[1] == 0  )
    // __UHEAP_MARKEND;
   
 return KErrNone;
 return KErrGeneral; */
  if(output[0] != 1)
  failures++;
  if(output[1] != 0)
  failures++;
  
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
 
 
 
 TInt Ctstl_1::Logicaland(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    

   try
   {   
//__UHEAP_MARK;
bool input1 [4] = { 1, 0, 0, 1 };
bool input2 [4] = { 0, 1, 0, 1 };
   bool output [4];
   
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  transform((bool*)input1, (bool*)input1 + 4, (bool*)input2, (bool*)output, logical_and<bool>());
  
   
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   /* if(output[0] ==  0 && output[1] == 0 && output[2] ==0 && output[3] == 1)
    
   //  __UHEAP_MARKEND;
 return KErrNone;
 return KErrGeneral;*/
 if(output[0] !=  0)
 failures++;
 if( output[1] != 0)
 failures++;
 if(output[2] !=0)
 failures++;
 if(output[3] != 1)   
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
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 }
 
 
 
 
 TInt Ctstl_1::Logicalor(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    

   try
   {   
//__UHEAP_MARK;
bool input1 [4] = { 1, 0, 0, 1 };
bool input2 [4] = { 0, 1, 0, 1};
  bool output [4];
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
  transform((bool*)input1, (bool*)input1 + 4, (bool*)input2, (bool*)output, logical_or<bool>());
   #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif  
  
  //  if(output[0] == 1 && output[1] == 1 && output[2] == 0 && output[3] == 1)
    
    //__UHEAP_MARKEND;
// return KErrNone;
 //return KErrGeneral;  

if(output[0] !=  1)
 failures++;
 if( output[1] != 1)
 failures++;
 if(output[2] !=0)
 failures++;
 if(output[3] != 1)   
 failures++;
 //#if STDCPP_OOM
// failures++;
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
 
 //remove
 
 TInt Ctstl_1::Remove(CStifItemParser& aItem)
 {
 	
 	int failures =0;
   
   try
   {   	
 //	__UHEAP_MARK;
 int arr[10] = {1,2,3,4,5,6,7,8,7,9};
    vector<int> v(arr, arr+10);
     
     int size1 =  sizeof(arr);
     
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
 
     // remove the 7
    vector<int>::iterator result =   
             remove(v.begin(), v.end(), 7);
      #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 

     for ( result = v.begin( ) ; result != v.end( ) ; result++ )
  
    {
    static int i = 0;
    
    arr[i] = *result;
    i++;
   
    }
      
     int size2 = sizeof(arr);

   /*   if(arr[6] != 7 && arr[6]==8  )
      if(arr[0]==1)
      if(arr[1]==2)
      if(arr[2]==3)
      if(arr[3]==4)
      if(arr[4]==5)
      if(arr[5]==6)
      if(arr[6]==8)
      if(arr[7]==9)
      if(size1==size2)
      // __UHEAP_MARKEND;
      return KErrNone;
      return KErrGeneral;*/
      if(arr[0]!=1)
      failures++;
      if(arr[1]!=2)
      failures++;
      if(arr[2]!=3)
      failures++;
      if(arr[3]!=4)
      failures++;
      if(arr[4]!=5)
      failures++;
      if(arr[5]!=6)
      failures++;
      if(arr[6]!=8)
      failures++;
      if(arr[7]!=9)
      failures++;
      if(size1!=size2)
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
 
 
 
TInt Ctstl_1::Removeif(CStifItemParser& aItem)
 {
 	
 	int failures =0;
   

   try
   {   	
 //	__UHEAP_MARK;
 int arr[10] = {1,2,3,4,5,6,7,8,7,9};
    vector<int> v(arr, arr+10);
     
     int size1 =  sizeof(arr);
     
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
     // remove the 7
    vector<int>::iterator result =   
             remove_if(v.begin(), v.end(),bind2nd(less_equal<int>(), 7));
     
    #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif     
     for ( result = v.begin( ) ; result != v.end( ) ; result++ )
  
    {
    static int i = 0;
    
    arr[i] = *result;
    i++;
   
    }
      
     int size2 = sizeof(arr);

      /*if(arr[0]==8)
      if(arr[1]==9)
      if(arr[2]==3)
      if(arr[3]==4)
      if(arr[4]==5)
      if(arr[5]==6)
      if(arr[6]==7)
      if(arr[7]==8)
      if(size1==size2)
      // __UHEAP_MARKEND;
      return KErrNone;
      return KErrGeneral;*/
      if(arr[0]!=8)
      failures++;
      if(arr[1]!=9)
      failures++;
      if(arr[2]!=3)
      failures++;
      if(arr[3]!=4)
      failures++;
      if(arr[4]!=5)
      failures++;
      if(arr[5]!=6)
      failures++;
      if(arr[6]!=7)
      failures++;
      if(arr[7]!=8)
      failures++;
      if(size1!=size2)
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
  
  
  
  
  
  
  
  
  
  TInt Ctstl_1::Removecopy(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    

   try
   {   	
 //	__UHEAP_MARK;
 	
 const int MAX_ELEMENTS = 8 ;

   // Define a template class vector of integers
   typedef vector<int > IntVector ;

   //Define an iterator for template class vector of integer
   typedef IntVector::iterator IntVectorIt ;

   //vector containing numbers
   IntVector Numbers(MAX_ELEMENTS), Result(MAX_ELEMENTS) ;
   
#ifdef __ARMCC__
#pragma diag_suppress 550
#endif
   IntVectorIt start, end, it, last, resultIt ;


   //Initialize vector Numbers
   Numbers[0] = 10 ;
   Numbers[1] = 20 ;
   Numbers[2] = 10 ;
   Numbers[3] = 15 ;
   Numbers[4] = 12 ;
   Numbers[5] = 7 ;
   Numbers[6] = 9 ;
   Numbers[7] = 10 ;

   start = Numbers.begin() ;   // location of first
                               // element of Numbers

   end = Numbers.end() ;       // one past the location
                               // last element of Numbers

   resultIt = Result.begin() ; // location of first
                               // element of Result

   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif  

   // copy all elements from Numbers to Result
   // skipping any item that equals 10
   last = remove_copy(start, end, resultIt, 10) ;

     #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   start = Result.begin() ;   // location of first
                               // element of Result

   end = Result.end() ;       // one past the location
                              // last element of Result

   // print content of Result
   
   for(it = start; it != end; it++)
     
   {
   static int i;
   Numbers[i] = *it;
   i++;
   	
   }
 /*      
if(Numbers[0] == 20)
if(Numbers[1] == 15)
if(Numbers[2] == 12)
if(Numbers[3] == 7)
if(Numbers[4] == 9)
if(Numbers[5] == 0)
if(Numbers[6] == 0)
if(Numbers[7] == 0)

// __UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;
*/
if(Numbers[0] != 20)
failures++;
if(Numbers[1] != 15)
failures++;
if(Numbers[2] != 12)
failures++;
if(Numbers[3] != 7)
failures++;
if(Numbers[4] != 9)
failures++;
if(Numbers[5] != 0)
failures++;
if(Numbers[6] != 0)
failures++;
if(Numbers[7] != 0)
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
  
  
  
  
  
  
  
  
  
  
  
  
  TInt Ctstl_1::Removecopy_if(CStifItemParser& aItem)
 {
 	
 	
	int failures =0;
    

   try
   {    //	__UHEAP_MARK;
 	
 const int MAX_ELEMENTS = 8 ;

   // Define a template class vector of integers
   typedef vector<int > IntVector ;

   //Define an iterator for template class vector of integer
   typedef IntVector::iterator IntVectorIt ;

   //vector containing numbers
   IntVector Numbers(MAX_ELEMENTS), Result(MAX_ELEMENTS) ;

   IntVectorIt start, end, it, last, resultIt ;

   //Initialize vector Numbers
   Numbers[0] = 10 ;
   Numbers[1] = 20 ;
   Numbers[2] = 10 ;
   Numbers[3] = 25 ;
   Numbers[4] = 12 ;
   Numbers[5] = 7 ;
   Numbers[6] = 9 ;
   Numbers[7] = 30 ;

   start = Numbers.begin() ;   // location of first
                               // element of Numbers

   end = Numbers.end() ;       // one past the location
                               // last element of Numbers

   resultIt = Result.begin() ; // location of first
                               // element of Result

   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif  

   // copy all elements from Numbers to Result
   // skipping any item that equals 10
   last = remove_copy_if(start, end, resultIt, bind2nd(greater_equal<int>(), 25)) ;

     #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   start = Result.begin() ;   // location of first
                               // element of Result

   end = Result.end() ;       // one past the location
                              // last element of Result

   // print content of Result
   
   for(it = start; it != end; it++)
     
   {
   static int i;
   Numbers[i] = *it;
   i++;
   	
   }
/*       
if(Numbers[0] == 10)
if(Numbers[1] == 20)
if(Numbers[2] == 10)
if(Numbers[3] == 12)
if(Numbers[4] == 7)
if(Numbers[5] == 9)
if(Numbers[6] == 0)
if(Numbers[7] == 0)

 //__UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;*/
if(Numbers[0] != 10)
failures++;
if(Numbers[1] != 20)
failures++;
if(Numbers[2] != 10)
failures++;
if(Numbers[3] != 12)
failures++;
if(Numbers[4] != 7)
failures++;
if(Numbers[5] != 9)
failures++;
if(Numbers[6] != 0)
failures++;
if(Numbers[7] != 0)
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
 
 
 
 
 TInt Ctstl_1::Replace_fun(CStifItemParser& aItem)
 {
 	
 	
 //	__UHEAP_MARK;
 	int failures =0;
    

   try
   {   	
 const int MAX_ELEMENTS = 8 ;

   // Define a template class vector of integers
   typedef vector<int > IntVector ;

   //Define an iterator for template class vector of integer
   typedef IntVector::iterator IntVectorIt ;

   //vector containing numbers
   IntVector Numbers(MAX_ELEMENTS)  ;

   IntVectorIt start, end, it ;

   //Initialize vector Numbers
   Numbers[0] = 10 ;
   Numbers[1] = 20 ;
   Numbers[2] = 10 ;
   Numbers[3] = 25 ;
   Numbers[4] = 12 ;
   Numbers[5] = 7 ;
   Numbers[6] = 9 ;
   Numbers[7] = 30 ;

   start = Numbers.begin() ;   // location of first
                               // element of Numbers

   end = Numbers.end() ;       // one past the location
                               // last element of Numbers

    // location of first
                               // element of Result

    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 

   // copy all elements from Numbers to Result
   // skipping any item that equals 10
    replace(start, end, 10, 35) ;
  
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif   

    
        // one past the location
                              // last element of Result

   // print content of Result
   
   for(it = start; it != end; it++)
     
   {
   static int i;
   Numbers[i] = *it;
   i++;
   	
   }
/*       
if(Numbers[0] == 35)
if(Numbers[1] == 20)
if(Numbers[2] == 35)
if(Numbers[3] == 25)
if(Numbers[4] == 12)
if(Numbers[5] == 7)
if(Numbers[6] == 9)
if(Numbers[7] == 30)

// __UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;

*/
if(Numbers[0] != 35)
failures++;
if(Numbers[1] != 20)
failures++;
if(Numbers[2] != 35)
failures++;
if(Numbers[3] != 25)
failures++;
if(Numbers[4] != 12)
failures++;
if(Numbers[5] != 7)
failures++;
if(Numbers[6] != 9)
failures++;
if(Numbers[7] != 30)
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
 
 
 
 
 
 
  TInt Ctstl_1::Replacecopy_fun(CStifItemParser& aItem)
  {
  	int failures =0;
     

   try
   {   	//	__UHEAP_MARK;
  const int MAX_ELEMENTS = 8 ;

    // Define a template class vector of integers
    typedef vector<int > IntVector ;

    //Define an iterator for template class vector of integer
    typedef IntVector::iterator IntVectorIt ;

    //vector containing numbers
    IntVector Numbers(MAX_ELEMENTS), Result(MAX_ELEMENTS) ;

    IntVectorIt start, end, it, last, resultIt ;

    //Initialize vector Numbers
    Numbers[0] = 10 ;
    Numbers[1] = 20 ;
    Numbers[2] = 10 ;
    Numbers[3] = 15 ;
    Numbers[4] = 12 ;
    Numbers[5] = 7 ;
    Numbers[6] = 9 ;
    Numbers[7] = 10 ;

    start = Numbers.begin() ;   // location of first
                                // element of Numbers

    end = Numbers.end() ;       // one past the location
                                // last element of Numbers

    resultIt = Result.begin() ; // location of first
                                // element of Result

 
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif
    // copy all elements from Numbers to Result
    // replacing any item that equals 10 with 30
    last = replace_copy(start, end, resultIt, 10, 30) ;

  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif
    start = Result.begin() ;   // location of first
                                // element of Result

    end = Result.end() ;       // one past the location
                               // last element of Result

 
    for(it = start; it != end; it++)
       
    {
    	static int i;
    	Result[i] = *it;
    	i++;
    }
         

/*  
  
  if (Result[0] == 30)
  if (Result[1] == 20)
  if (Result[2] == 30)
  if (Result[3] == 15)
  if (Result[4] == 12)
  if (Result[5] == 7)
  if (Result[6] == 9)
  if (Result[7] == 30)
  //	__UHEAP_MARKEND;
  return KErrNone;
  return KErrGeneral;
  */
    if (Result[0] != 30)
    failures++;
  if (Result[1] != 20)
  failures++;
  
  if (Result[2] != 30)
  failures++;
  if (Result[3] != 15)
  failures++;
  if (Result[4] != 12)
  failures++;
  if (Result[5] != 7)
  failures++;
  if (Result[6] != 9)
  failures++;
  if (Result[7] != 30)
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
 
 
 
 
  TInt Ctstl_1::Replacecopyif_fun(CStifItemParser& aItem)
  {
  	
 	int failures =0;
    

   try
   {    	
  	//	__UHEAP_MARK;
  const int MAX_ELEMENTS = 8 ;

    // Define a template class vector of integers
    typedef vector<int > IntVector ;

    //Define an iterator for template class vector of integer
    typedef IntVector::iterator IntVectorIt ;

    //vector containing numbers
    IntVector Numbers(MAX_ELEMENTS), Result(MAX_ELEMENTS) ;

    IntVectorIt start, end, it, last, resultIt ;

    //Initialize vector Numbers
    Numbers[0] = 10 ;
    Numbers[1] = 20 ;
    Numbers[2] = 10 ;
    Numbers[3] = 15 ;
    Numbers[4] = 12 ;
    Numbers[5] = 7 ;
    Numbers[6] = 9 ;
    Numbers[7] = 10 ;

    start = Numbers.begin() ;   // location of first
                                // element of Numbers

    end = Numbers.end() ;       // one past the location
                                // last element of Numbers

    resultIt = Result.begin() ; // location of first
                                // element of Result

#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
#endif 

    // copy all elements from Numbers to Result
    // replacing any item that >= 10 with 30
    last = replace_copy_if(start, end, resultIt,
                           bind2nd(greater_equal<int>(), 10), 30) ;

 #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif

    start = Result.begin() ;   // location of first
                                // element of Result

    end = Result.end() ;       // one past the location
                               // last element of Result
 for(it = start; it != end; it++)
       
    {
    	static int i;
    	Result[i] = *it;
    	i++;
    }
     
/*
   if (Result[0] == 30)
  if (Result[1] == 30)
  if (Result[2] == 30)
  if (Result[3] == 30)
  if (Result[4] == 30)
  if (Result[5] == 7)
  if (Result[6] == 9)
  if (Result[7] == 30)
  //	__UHEAP_MARKEND;
  return KErrNone;
  return KErrGeneral;*/
  
  if (Result[0] != 30)
  failures++;
    if (Result[1] != 30)
    failures++;
  if (Result[2] != 30)
  failures++;
  if (Result[3] != 30)
  failures++;
  
  if (Result[4] != 30)
  failures++;
  
  if (Result[5] != 7)
  failures++;
  
  if (Result[6]!= 9)
  failures++;
  
  if (Result[7] != 30)
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
 
 
 
  TInt Ctstl_1::Replaceif_fun(CStifItemParser& aItem)
  {
  	
  	int failures =0;
   

   try
   {   	
  	//	__UHEAP_MARK;
  const int VECTOR_SIZE = 8 ;

    // Define a template class vector of integers
    typedef vector<int > IntVector ;

    //Define an iterator for template class vector of integer
    typedef IntVector::iterator IntVectorIt ;

    IntVector Numbers(VECTOR_SIZE) ;   //vector containing numbers

    IntVectorIt start, end, it ;

    start = Numbers.begin() ;   // location of first
                                // element of Numbers

    end = Numbers.end() ;       // one past the location
                                // last element of Numbers

    //Initialize vector Numbers
    Numbers[0] = 10 ;
    Numbers[1] = 20 ;
    Numbers[2] = 10 ;
    Numbers[3] = 15 ;
    Numbers[4] = 12 ;
    Numbers[5] = 7 ;
    Numbers[6] = 9 ;
    Numbers[7] = 10 ;

     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 

    // replace all elements from Numbers that are <= 10 with 4
     replace_if(start, end, bind2nd(less_equal<int>(), 10), 4 ) ;
#if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   for(it = start; it != end; it++)
     
   {
   static int i;
   Numbers[i] = *it;
   i++;
   	
   }
 /*      
if(Numbers[0] == 4)
if(Numbers[1] == 20)
if(Numbers[2] == 4)
if(Numbers[3] == 15)
if(Numbers[4] == 12)
if(Numbers[5] == 4)
if(Numbers[6] == 4)
if(Numbers[7] == 4)

 //	__UHEAP_MARKEND;
return KErrNone;
return KErrGeneral;*/


if(Numbers[0] != 4)
failures++;
if(Numbers[1] != 20)
failures++;
if(Numbers[2] != 4)
failures++;
if(Numbers[3] != 15)
failures++;
if(Numbers[4] != 12)
failures++;
if(Numbers[5] != 4)
failures++;
if(Numbers[6] != 4)
failures++;
if(Numbers[7] != 4)
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
 
 TInt Ctstl_1::Reverse_fun(CStifItemParser& aItem)
  {
  
  //	__UHEAP_MARK;
 	int failures =0;
    

   try
   {    	
  const int VECTOR_SIZE = 8;

   // Define a template class vector of strings
   typedef vector<string > StrVector;

   //Define an iterator for template class vector of strings
   typedef StrVector::iterator StrVectorIt;

   StrVector Tongue_Twister(VECTOR_SIZE);
   StrVectorIt start, end, it;

   // location of first element of Tongue_Twister
   start = Tongue_Twister.begin();

   // one past the location last element of Tongue_Twister
   end = Tongue_Twister.end();

   //Initialize vector Tongue_Twister
   Tongue_Twister[0] = "she";
   Tongue_Twister[1] = "sells";
   Tongue_Twister[2] = "sea";
   Tongue_Twister[3] = "shells";
   Tongue_Twister[4] = "by";
   Tongue_Twister[5] = "the";
   Tongue_Twister[6] = "sea";
   Tongue_Twister[7] = "shore";

    // cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
   // reverse the items in the vector Tongue_Twister
   reverse(start, end);

  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 

   // print content of Tongue_Twister
   cout << "Now try the reversed Tongue Twister:";
   for (it = start; it != end; it++)
     
   {
   static int i;
   Tongue_Twister[i] = *it;
   i++;	
   
   }
      
      
 /*     
      
   if( Tongue_Twister[0] == "shore")
   if(Tongue_Twister[1] == "sea")
   if(Tongue_Twister[2] == "the")
   if(Tongue_Twister[3] == "by")
   if(Tongue_Twister[4] == "shells")
   if(Tongue_Twister[5] == "sea")
   if(Tongue_Twister[6] == "sells")
   if(Tongue_Twister[7] == "she")
   //	__UHEAP_MARKEND;
   return KErrNone;
   return KErrGeneral;
*/

  //#if STDCPP_OOM //failures++; //#endif
  
  if( Tongue_Twister[0] != "shore")
  failures++;
  
   if(Tongue_Twister[1] != "sea")
   failures++;
   
   if(Tongue_Twister[2] != "the")
   failures++;
   
   if(Tongue_Twister[3] != "by")
   failures++;
   
   if(Tongue_Twister[4] != "shells")
   failures++;
   if(Tongue_Twister[5] != "sea")
   failures++;
   if(Tongue_Twister[6] != "sells")
   failures++;
   if(Tongue_Twister[7] != "she")
   failures++;
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
 
 
 
 TInt Ctstl_1::Reversecopy_fun(CStifItemParser& aItem)
  {
  	
  	
  	int failures =0;
    

   try
   {   	
  	//	__UHEAP_MARK;
  const int VECTOR_SIZE = 8 ;

    // Define a template class vector of strings
    typedef vector<string> StrVector ;

    //Define an iterator for template class vector of strings
    typedef StrVector::iterator StrVectorIt ;

    StrVector Tongue_Twister(VECTOR_SIZE) ;
    StrVector Reversed_Twister(VECTOR_SIZE) ;

    StrVectorIt start, end, it, RTstart, RTend ;

    start = Tongue_Twister.begin() ; // location of first
                                     // element of Tongue_Twister

    end = Tongue_Twister.end() ;     // one past the location last
                                     // element of Tongue_Twister

    RTstart = Reversed_Twister.begin() ; // location of first
                                         // element of Reversed_Twister

    RTend = Reversed_Twister.end() ; // one past the location last
                                     // element of Reversed_Twister

    //Initialize vector Tongue_Twister
    Tongue_Twister[0] = "she";
    Tongue_Twister[1] = "sells";
    Tongue_Twister[2] = "sea";
    Tongue_Twister[3] = "shells";
    Tongue_Twister[4] = "by";
    Tongue_Twister[5] = "the";
    Tongue_Twister[6] = "sea";
    Tongue_Twister[7] = "shore";
 // cout<<"";
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 

    // reverse the items in the vector Tongue_Twister
    // and copy the results to Reversed_Twister
    reverse_copy(start, end, RTstart);

      #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
    for(it = start; it != end; it++)
       
       
    {
    	static int i;
    	Tongue_Twister[i] = *it;
    	i++;
    }

     
     
      for(it = RTstart; it != RTend; it++)
        
        
      {
      	
      static int i;
      Reversed_Twister[i] = *it;
      i++;
      }
      
      
      
      
      
     /* if( Tongue_Twister[7] == "shore")
   if(Tongue_Twister[6] == "sea")
   if(Tongue_Twister[5] == "the")
   if(Tongue_Twister[4] == "by")
   if(Tongue_Twister[3] == "shells")
   if(Tongue_Twister[2] == "sea")
   if(Tongue_Twister[1] == "sells")
   if(Tongue_Twister[0] == "she")
   
   */
    if( Tongue_Twister[7] != "shore")
    failures++;
    if(Tongue_Twister[6] != "sea")
   failures++;
   if(Tongue_Twister[5] != "the")
   failures++;
   if(Tongue_Twister[4] != "by")
   failures++;
   if(Tongue_Twister[3] != "shells")
   failures++;
   if(Tongue_Twister[2] != "sea")
   failures++;
   if(Tongue_Twister[1] != "sells")
   failures++;
   if(Tongue_Twister[0] != "she")
   failures++;
  /* 
   if( Reversed_Twister[0] == "shore")
   if(Reversed_Twister[1] == "sea")
   if(Reversed_Twister[2] == "the")
   if(Reversed_Twister[3] == "by")
   if(Reversed_Twister[4] == "shells")
   if(Reversed_Twister[5] == "sea")
   if(Reversed_Twister[6] == "sells")
   if(Reversed_Twister[7] == "she")
   
   
   //	__UHEAP_MARKEND;

   return KErrNone;
   return KErrGeneral;*/
   
    if( Reversed_Twister[0] != "shore")
    failures++;
    if(Reversed_Twister[1] != "sea")
   failures++;
   if(Reversed_Twister[2] != "the")
   failures++;
   if(Reversed_Twister[3] != "by")
   failures++;
   if(Reversed_Twister[4] != "shells")
   failures++;
   if(Reversed_Twister[5] != "sea")
   failures++;
   if(Reversed_Twister[6] != "sells")
   failures++;
   if(Reversed_Twister[7] != "she")
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
 
 


 TInt Ctstl_1::Rotate_fun(CStifItemParser& aItem)
 
 {
 
 	int failures =0;
    

   try
   {   
 //	__UHEAP_MARK;	
 const int VECTOR_SIZE = 8;

   // Define a template class vector of strings
   typedef vector<string> StrVector;

   //Define an iterator for template class vector of strings
   typedef StrVector::iterator StrVectorIt;

   StrVector Tongue_Twister(VECTOR_SIZE);

   StrVectorIt start, end, middle, it;

   // location of first element of Tongue_Twister
   start = Tongue_Twister.begin();   

   // one past the location last element of Tongue_Twister
   end = Tongue_Twister.end();       


   // Initialize vector Tongue_Twister
   Tongue_Twister[0] = "she";
   Tongue_Twister[1] = "sells";
   Tongue_Twister[2] = "sea";
   Tongue_Twister[3] = "shells";
   Tongue_Twister[4] = "by";
   Tongue_Twister[5] = "the";
   Tongue_Twister[6] = "sea";
   Tongue_Twister[7] = "shore";

   middle = start + 3;  // start position for rotating elements

   // cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
 

   // rotate the items in the vector Tongue_Twister by 3 positions
   rotate(start, middle, end);

   #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif  

     for (it = start; it != end; it++)
     {
     	
     static int i;
     Tongue_Twister[i] = *it;
     i++;
     
     }
      
      
 
  /* if( Tongue_Twister[4] == "shore")
   if(Tongue_Twister[5] == "she")
   if(Tongue_Twister[6] == "sells")
   if(Tongue_Twister[7] == "sea")
   if(Tongue_Twister[0] == "shells")
   if(Tongue_Twister[1] == "by")
   if(Tongue_Twister[2] == "the")
   if(Tongue_Twister[3] == "sea")
   //	__UHEAP_MARKEND; 
   return KErrNone;
   return KErrGeneral;*/
   
   if( Tongue_Twister[4] != "shore")
    failures++;
    if(Tongue_Twister[5] != "she")
   failures++;
   if(Tongue_Twister[6] != "sells")
   failures++;
   if(Tongue_Twister[7] != "sea")
   failures++;
   if(Tongue_Twister[0] != "shells")
   failures++;
   if(Tongue_Twister[1] != "by")
   failures++;
   if(Tongue_Twister[2] != "the")
   failures++;
   if(Tongue_Twister[3] != "sea")
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
 
 
 TInt Ctstl_1::Rotatecopy_fun(CStifItemParser& aItem)
 
 {
 //	__UHEAP_MARK;
 	int failures =0;
    

   try
   {   
  const int VECTOR_SIZE = 8 ;

    typedef vector<string> StrVector ;

    typedef StrVector::iterator StrVectorIt ;

   StrVector Tongue_Twister(VECTOR_SIZE) ;
   StrVector Rotated_Twister(VECTOR_SIZE) ;

   StrVectorIt start, middle, end, it, RTstart, RTend ;

    start = Tongue_Twister.begin() ;    

    end = Tongue_Twister.end() ;

    middle = start + 3 ;             

   // location of first element of Rotated_Twister
   RTstart = Rotated_Twister.begin() ;

   // one past the location last element of Rotated_Twister
   RTend = Rotated_Twister.end() ;  

   // Initialize vector Tongue_Twister
   Tongue_Twister[0] = "she" ;
   Tongue_Twister[1] = "sells" ;
   Tongue_Twister[2] = "sea" ;
   Tongue_Twister[3] = "shells" ;
   Tongue_Twister[4] = "by";
   Tongue_Twister[5] = "the";
   Tongue_Twister[6] = "sea" ;
   Tongue_Twister[7] = "shore" ;
 // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
    
   // rotate the items in the vector Tongue_Twist to the right by
   // 3 positions and copy the results to Rotated_Twister
   rotate_copy(start, middle, end, RTstart) ;

    
#if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif  
  
   
   for (it = start; it != end; it++)
   {
   	
   static int i;
   Tongue_Twister[i] = *it;
   i++;
   
   }
   

 
   for (it = RTstart; it != RTend; it++)
     
     
   {
   	
   static int i;
   
   
   Rotated_Twister[i] = *it;
   i++;
   
   }
     
 
 /*
      if( Tongue_Twister[7] == "shore")
   if(Tongue_Twister[6] == "sea")
   if(Tongue_Twister[5] == "the")
   if(Tongue_Twister[4] == "by")
   if(Tongue_Twister[3] == "shells")
   if(Tongue_Twister[2] == "sea")
   if(Tongue_Twister[1] == "sells")
   if(Tongue_Twister[0] == "she")
   
   
   if( Rotated_Twister[0] == "shells")
   if(Rotated_Twister[1] == "by")
   if(Rotated_Twister[2] == "the")
   if(Rotated_Twister[3] == "sea")
   if(Rotated_Twister[4] == "shore")
   if(Rotated_Twister[5] == "she")
   if(Rotated_Twister[6] == "sells")
   if(Rotated_Twister[7] == "sea")
 
 //	__UHEAP_MARKEND;
 return KErrNone;
 return KErrGeneral;*/
 if( Tongue_Twister[7] != "shore")
    failures++;
    if(Tongue_Twister[6] != "sea")
   failures++;
   if(Tongue_Twister[5] != "the")
   failures++;
   if(Tongue_Twister[4] != "by")
   failures++;
   if(Tongue_Twister[3] != "shells")
   failures++;
   if(Tongue_Twister[2] != "sea")
   failures++;
   if(Tongue_Twister[1] != "sells")
   failures++;
   if(Tongue_Twister[0] != "she")
   failures++;
   
 if( Rotated_Twister[4] != "shore")
    failures++;
    if(Rotated_Twister[3] != "sea")
   failures++;
   if(Rotated_Twister[2] != "the")
   failures++;
   if(Rotated_Twister[1] != "by")
   failures++;
   if(Rotated_Twister[0] != "shells")
   failures++;
   if(Rotated_Twister[7] != "sea")
   failures++;
   if(Rotated_Twister[6] != "sells")
   failures++;
   if(Rotated_Twister[5] != "she")
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
 
 
  TInt Ctstl_1::equalrange(CStifItemParser& aItem)
  
  {
 	int failures =0;
   

   try
   {   // __UHEAP_MARK;
  int failures =0;
  vector <int> v1;
//   vector <int>::iterator Iter1;
   pair < vector <int> :: iterator , vector <int> :: iterator > Result1, Result2;

   // Constructing vectors v1a & v1b with default less than ordering
   int i;
   for ( i = -1 ; i <= 4 ; i++ )
   {
      v1.push_back(  i );
   }

   int ii;
   for ( ii =-3 ; ii <= 0 ; ii++ )
   {
      v1.push_back( ii );
   }

   sort ( v1.begin ( ) , v1.end ( ) );
  

   // Constructing vectors v2 with range sorted by greater
   vector <int> v2 ( v1 );
//   vector <int>::iterator Iter2;
   sort ( v2.begin ( ) , v2.end ( ) , greater<int> ( ) );

    // cout<<"";

 
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 

   // equal_range of 3 in v1 with default binary predicate less <int> ( )
   Result1 = equal_range ( v1.begin ( ) , v1.end ( ) , 3 );
    #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
   if( *Result1.first != 3)
   failures++;
   if(*Result1.second !=  4)
   failures++;
   
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif
   
   // equal_range of 3 in v2 with the binary predicate greater <int> ( )
   Result2 = equal_range ( v2.begin ( ) , v2.end ( ) , 3 , greater <int> ( ) );
   
   #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif
   if( *Result2.first != 3)
   failures++;
   if( *Result2.second != 2)
   failures++;
   
  // __UHEAP_MARKEND;
   
 /*  if(failures)
   return KErrGeneral;
   return KErrNone;*/
   
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

 
 
 
   static void f() { throw runtime_error("a runtime error"); }

   TInt Ctstl_1::exceptionL(CStifItemParser& aItem)
   {
   	
  
    
         try
          {
             f();
          }
         catch (const exception& e)
          {
           //  cout << "Got an exception: " << e.what() << endl;
                  
          return KErrNone;
         
         
          }
        
         
       
        
return KErrGeneral;
   
   
   
   }




// prints the cube of integer n




	
int PrintCube(int n)
{
 
 return(n*n*n);
}


   TInt Ctstl_1::foreach(CStifItemParser& aItem)
   
   {
 
   		int failures =0;
    

   try
   {   
   	     	
   const int VECTOR_SIZE = 8 ;

    typedef vector<int > IntVector ;

     typedef IntVector::iterator IntVectorIt ;

   IntVector Numbers(VECTOR_SIZE) ;   //vector containing numbers

//    IntVectorIt start, end, it ;

    int i ;

    // Initialize vector Numbers
    for (i = 0; i < VECTOR_SIZE; i++)
        Numbers[i] = i + 1 ;

  
 
 	  // cout<<""; 
    // for each element in the range [first, last)
    // print the cube of the element
  
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif
   for_each(Numbers.begin() , Numbers.end(), PrintCube) ;
      
  #if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif
     
  // if(output[0] ==1 )
   //if(output[1] == 8)
   // return KErrNone;
  //return KErrGeneral;
    	
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
   
   
 TInt Ctstl_1::gsliceL(CStifItemParser& aItem)
     
 {
     	
int i,failures=0;
    

   try
   { 
   
     size_t startVAR;

   valarray<int> va ( 20 ), vaResult;
  


 valarray<size_t> Len ( 2 ), Stride ( 2 );
   Len [0] = 4;
   Len [1] = 4;
   Stride [0] = 7;
   Stride [1] = 4; // cout<<"";

  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
  #endif 
gslice vaGSlice(0,Len,Stride);
 
 

 startVAR = vaGSlice.start ( );
if(startVAR!=0)
failures++;
 

    const valarray <size_t> strideGS = vaGSlice.stride ( );


   

   
for ( i = 0 ; i < 2 ; i++ )
   

{  

if( strideGS[ 0 ]!=7)
failures++;
if(strideGS[1]!=4)
failures++;
}

const valarray <size_t> sizeGS = vaGSlice.size ( ); // cout<<"";
#if  STDCPP_OOM 	
  User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);

  
 #endif 
for ( i = 0 ; i < 2 ; i++ )
 {

 if (sizeGS[0] !=4 )
failures++;

if(sizeGS[1]!=4)
failures++;
}
/*

if(failures)
return KErrGeneral;
else
return KErrNone;
    
   */ 
      	
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
 
  
  
    
 TInt Ctstl_1::sliceL(CStifItemParser& aItem)
     
 {
     	
 int failures=0;
 
// Simulating  no memory


   try
   { // cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
          
   size_t startVAR,strideVAR,sizeVAR;

 

  
  slice vaSlice ( 3 , 6 , 3 );

 
 

   startVAR = vaSlice.start ( );
if(startVAR!=3)
failures++;


strideVAR = vaSlice.stride();

if(strideVAR !=3)
failures++;



sizeVAR = vaSlice.size();
#if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
if(sizeVAR!=6)
failures++;


/*
     if(failures)
     return KErrGeneral;
     return KErrNone;
     */
//#if STDCPP_OOM
//failures++;
//#endif
 }


// catch's bad_alloc  exception

 catch(bad_alloc&)
 {
 }
   // catch's exception
 catch(...)
 {
 failures++;
 	
 }
   
     
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;   
 }
 
 // unary function 
   
class greaterthan10: unary_function<int, bool>
{

public:
    result_type operator()(argument_type i)
    {
        return (result_type)(i > 10);
    }
};


    
    
TInt Ctstl_1::unaryfunc(CStifItemParser& aItem)
     
{ 
int failures=0;

try
   {
vector<int> v1;
//vector<int>::iterator Iter;
int i ;
for (i = 0; i <= 5; i++)
{
v1.push_back(5 * i);
}

     
int result1; // cout<<"";
// Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif
    //vector<int>::iterator::difference_type result1;
    result1 = count_if(v1.begin(), v1.end(), greaterthan10());
#if  STDCPP_OOM 	
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
#endif 
    /*if(result1 == 3)
    return KErrNone;
    else
    return KErrGeneral;*/
    if(result1!=3)
    failures++;
    
     
//#if STDCPP_OOM //failures++; //#endif
 }


// catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;   
}
 
 
 
 
 // unintiallized fill
 
 
 
 class Integer
  {         // No default constructor
   public:
      Integer( int x ) : val( x ) {}
      int get( ) { return val; }
   private:
      int val;
   };

TInt Ctstl_1::uni_fill(CStifItemParser& aItem)
     
     {
         
 //  __UHEAP_MARK;
     int failures=0;
 


   try
   {
   const int N = 3;
   int failures = 0;
   Integer val (1);
   Integer* Array = ( Integer* ) malloc( N * sizeof( int ) ); // cout<<"";
  // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
   uninitialized_fill( Array, Array + N, val );
     
 #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
   int i;
       for ( i = 0 ; i < N; i++ )
      {
         if( Array [ 0 ].get( ) !=1)
         failures++;
         if( Array [ 1 ].get( ) !=1)
         failures++;
         if( Array [ 2 ].get( ) !=1)
         failures++;
   
   
  
         }
         
          delete Array;
          
        //  __UHEAP_MARKEND;
  /* if(failures)
   return KErrGeneral;
   else
   return KErrNone;*/
   
   
 //#if STDCPP_OOM //failures++; //#endif  
}
// catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
     
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;   
 
 
     }
     
     
     
     
     
     
     
     //unintiallzed fill n
 
 class Integer1 {   // No default constructor
public:
   Integer1( int x ) : val( x ) {}
   int get( ) { return val; }
private:
   int val;
};

TInt Ctstl_1::uni_fill_n(CStifItemParser& aItem)
 {
  //  __UHEAP_MARK;
     int failures=0;
 


   try
   {
 //__UHEAP_MARK;
   int failures =0;
   const int N = 3;
   Integer1 val ( 6);
   Integer1* Array = ( Integer1* ) malloc( N * sizeof( int ) ); // cout<<"";
   // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
   uninitialized_fill_n( Array, N, val );  // C4996
   
   #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif  
   int i;
    for ( i = 0 ; i < N; i++ )
    {
    if( Array [ 0 ].get( ) !=6)
         failures++;
         if( Array [ 1 ].get( ) !=6)
         failures++;
         if( Array [ 2 ].get( ) !=6)
         failures++;	
    }
      
  delete Array;
/*  
  //__UHEAP_MARKEND;
  if(failures)
   return KErrGeneral;
   else
   return KErrNone; */
   //#if STDCPP_OOM //failures++; //#endif
 }
   // catch's bad_alloc  exception
catch(bad_alloc&)
    {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;   
 
}
 
 
 
 // unintiallized  copy
 
  class Integer2
   {
   public:
      Integer2( int x ) : val( x ) {}
   
   private:
    int val; 
   };

	TInt Ctstl_1::uni_copy(CStifItemParser& aItem)
 	{
  int failures=0;


   try
   { // cout<<"";
   // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
  	 int Array[] = { 10, 20, 30, 40 };
   	const int N = sizeof( Array ) / sizeof( int );

//   	int i;
    Integer2* ArrayPtr = ( Integer2* ) malloc( N * sizeof( int ) );
     

   	Integer2* LArrayPtr = uninitialized_copy(Array, Array + N, ArrayPtr);  
   #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif  	   
 /*
  	if ( ( void* )LArrayPtr == ( void* )( ArrayPtr + N ) )
     return KErrNone;
     else
     return KErrGeneral;*/
     if ( ( void* )LArrayPtr != ( void* )( ArrayPtr + N ) )
     failures++;
     
  
   
 //#if STDCPP_OOM //failures++; //#endif
 }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
   
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;

}
 
 
// maskarray 
 
 
 TInt Ctstl_1::maskarray(CStifItemParser& aItem)
 
 {
 
// __UHEAP_MARK;
int failures=0;
 
 

   try
   { 
 int ibuf[10] = {0,1,2,3,4,5,6,7,8,9};
 bool mbuf[10] = {1,0,1,1,1,0,0,1,1,0};
 valarray<int>         vi(ibuf,10);
 valarray<bool> mask(mbuf,10);
  // cout<<"";
 // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif
mask_array<int> msk = vi[mask];
#if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif  
 valarray<int> vi3 = msk;
      //__UHEAP_MARKEND;  
 // return KErrNone;
 //#if STDCPP_OOM //failures++; //#endif     
}
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
	
 }
 
 
 
 
 //auto pointer
 
 
 
 
 
 
 
class ptr
{
public:
  ptr(int i) 
   {
 	x = i;
}  ;
   
   ~ptr( ) 
   {
    
    };

   int x;

}  ;

TInt Ctstl_1::autoptr(CStifItemParser& aItem)
 
 {

int failures=0;
  // cout<<"";
// Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 

   try
   { 
   // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
 auto_ptr<ptr> pi ( new ptr( 5 ) );
 
 pi.reset( new ptr( 6 ) );
 ptr* pi2 = pi.get ( );
 ptr* pi3 = pi.release ( );
  #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
 
 
 /*if (pi2 == pi3)
  
return KErrNone;
else
return KErrGeneral;*/

if(pi2!=pi3)
failures++;

//#if STDCPP_OOM //failures++; //#endif
 }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
     
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;


}
 
 
 
 
 //random shuffle
 
 
 TInt Ctstl_1::randshuf(CStifItemParser& aItem)
 
 {
 int failures=0;
 


   try
   { 
   vector <int> v1;
   vector <int>::iterator Iter1;
   int i,failures = 0 ;
   static int j ,k;
   for ( i = 1 ; i <= 3 ; i++ )
   v1.push_back( i ); // cout<<"";
   // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
   random_shuffle( v1.begin( ), v1.end( ) );
   random_shuffle( v1.begin( ), v1.end( ));
    #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
   push_heap( v1.begin( ), v1.end( ) );

 for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
 {
  
		if(j == 0)
		{     
		if(*Iter1 != 2)
		failures++;
 		} 


		j++;
 }
  
   random_shuffle( v1.begin( ), v1.end( ));
   push_heap( v1.begin( ), v1.end( ) );
  
   for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
     
		{
 
		if(k == 0)
			{

			if(*Iter1 != 3)
			failures++;
  			}
			k++;

		}
/*
if(failures)
return KErrGeneral;
else
return KErrNone;*/

//#if STDCPP_OOM //failures++; //#endif
 }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;

 }

 
 
 
 
 
 
class RInt
{
public:
   RInt(int i)
   {
 
      x = i;
      bIsConstructed = true;
   };
   
   
   
   
   RInt &operator=( int i )
   {
      if (!bIsConstructed)
 	  x = i; return *this;
   };
   int x;
   bool bIsConstructed;
};

 
 TInt Ctstl_1::randstoriter(CStifItemParser& aItem)
 
 {
  int failures=0;
 


   try
   { 
   int failures =0;
   std::list<int> l;
   l.push_back( 1 );
   l.push_back( 2 );
   l.push_back( 3 );
   l.push_back( 4 );

   
   RInt *pInt = (RInt*)malloc(sizeof(RInt)*l.size( ));
  // cout<<"";
// Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
   std::copy( l.begin( ), l.end( ), std::raw_storage_iterator<RInt*,RInt>(pInt)); 
    #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
   for (unsigned int i = 0; i < l.size( ); i++ )
   {
   	  	static int j;
    	if(j==0)
   		{
   		
   		 if(pInt[j].x != 1)
   			failures++;
   
   		}
   }
   free(pInt);


/*
if(failures)
return KErrGeneral;
else
return KErrNone;

*/
//#if STDCPP_OOM //failures++; //#endif
}
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;

}
 
 


 TInt Ctstl_1::iterswap(CStifItemParser& aItem)
 
 { int failures=0;
 


   try
   { 
 	
  int myints[]={10,20,30,40,50 };           
  vector<int> myvector (4,99);             
  int failures = 0; // cout<<"";
// Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
  iter_swap(myints,myvector.begin());      

  iter_swap(myints+3,myvector.begin()+2);   
 #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif  
   	for (vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
    
   	{
  	 static int i;
   		if(i==0) 
  		{
  	
  			if(*it != 10)
 			 failures++;
    
   		}
	i++;
    }
    /*
   
   if(failures)
   return KErrGeneral;
   else
   return KErrNone;*/
   
//#if STDCPP_OOM //failures++; //#endif
  }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
   
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
 	
 }
 
 
 
  TInt Ctstl_1::ios1(CStifItemParser& aItem)
  
  {
  int failures=0;
 

   try
   { // cout<<""; // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 

  fstream file;
   file.exceptions(ios::failbit);
    #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
   try 
   {
      file.open( "nofile.txt", ios_base::in );
    }
   catch( ios_base::failure f ) 
   {
      //do nothing
     
   }	
  catch(...)
  {
  
failures++;
   }
  //#if STDCPP_OOM //failures++; //#endif
   }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
 	
  }

 
 
  TInt Ctstl_1::ios2(CStifItemParser& aItem)
  
  {
  int failures=0;
 

   try
   {  // cout<<"";
  // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 

   int k = cout.flags();
  if(cout.flags( ) != 4104)
  failures++;
  
   cout.flags( ios::dec | ios::boolalpha );
 int j = cout.flags();
  #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
  if( cout.flags( )!= 264)
  failures++;
  
  /*
   
  if(failures)
  return KErrGeneral;
  else
  return  KErrNone;*/
  
  //#if STDCPP_OOM //failures++; //#endif
  }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
  }
  
  
   
  TInt Ctstl_1::ios3(CStifItemParser& aItem)
  
  {
  int failures=0;
 

   try
   { 
  int failures =0;
    // cout<<"";
// Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif
     static const int i = ios_base::xalloc();
   static const int j = ios_base::xalloc();
   cout.iword( i ) = 11;
    cin.pword( j ) = (char *)"testing";
  if( cout.iword( i ) != 11)
  failures++;
  
  cout.width(2);
  
  if( ( char * )cin.pword( j ) != "testing")
  failures++;
  
  
  int k = 10;
    if(k!=10)
    failures++;
    

 cout.unsetf( ios_base::dec );
 cout.setf( ios_base::hex );
if(k!= 0xa)
failures++;

  
// float l = 31.31234F;
   
  cout.precision( 3 );
 
  cout.getloc( ).name( ).c_str( );
 
  #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
  
  /*if(failures)
  return KErrGeneral;
  else
  return KErrNone;*/
  
  //#if STDCPP_OOM //failures++; //#endif
  }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
      
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
  }



TInt Ctstl_1::nthelement( CStifItemParser& aItem )
    {
    int failures=0;
 
   try
   { 
int myints[] = {4,10,70,30,10,69,96,100 };    // 10 20 20 20 30 30 20 20 10
  vector<int> Numbers (myints,myints+8);
 int output[8];
     typedef vector<int> IntVector ;
 
     typedef IntVector::iterator IntVectorIt ;

 
    IntVectorIt start, end, it ;
 

    start = Numbers.begin() ;   // location of first
                                // element of Numbers

    end = Numbers.end() ;       // one past the location
                                // last element of Numbers

     // cout<<"";
// Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 


    // partition the elements by the 4th element
    nth_element(start, start+4, end) ;

    
 #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
     for(it = start; it != end; it++)
       
       
    {
    static int i;
    output[i] = *it;
    i++;	
    }
    
    
   /* if(output[0]==4)
    if(output[1]==10)
    if(output[2]==10)
    if(output[3]==30)
    if(output[4]==69)
    if(output[5]==70)
    if(output[6]==96)
    if(output[7]==100)
    

 return KErrNone;
 return KErrGeneral;*/
 
 
   if(output[0] != 4)
 failures++;
 if(output[1] != 10)
 failures++;
 if(output[2] !=10)
 failures++;
 if( output[3] != 30)
 failures++;
 if(output[4] != 69)
 failures++;
 if(output[5] !=70)
 failures++;
 if( output[6] != 96)
 failures++;
  if( output[7] != 100)
 failures++;
 
 //#if STDCPP_OOM //failures++; //#endif
    }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
    }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
    
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
    }
    
    
    
    
 // hasfacet   
    
    
    TInt Ctstl_1::hasfacet( CStifItemParser& aItem )
    {
 int failures=0;
 


   try
   { 
 bool result ;
 // __UHEAP_MARK;
 
    
 locale loc("en_GB.UTF-8") ;
  
  // cout<<"";
 // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
 result = has_facet <ctype<char> > ( loc );
  
 #if  STDCPP_OOM 	
 User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif   
 
  

//__UHEAP_MARKEND;
/*
 if(result)
 return KErrNone;
 else
 return KErrGeneral;*/
 if(!result)
 failures++;
 
 //#if STDCPP_OOM //failures++; //#endif
   }
   // catch's bad_alloc  exception

   catch(bad_alloc&)
   {
   }
   // catch's exception
   catch(...)
   {
   	failures++;
   	
   }
   
   // __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
    }


  
    //end of file
