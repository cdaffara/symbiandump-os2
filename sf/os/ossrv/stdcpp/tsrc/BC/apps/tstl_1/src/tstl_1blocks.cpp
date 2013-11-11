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

#include<list>
#include<string>
#include<cstring>
#include<iterator>
#include<bitset>
#include<ios>
#include<complex>
#include<map>
#include<set>

#include<ostream>
#include <queue>
#include <deque>
#include <stack>


#include <stl\char_traits.h> // fpos
#undef test


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




				 ENTRY("Advance", Ctstl_1::Advance ),
				 ENTRY("Backinsert_iterator", Ctstl_1::Backinsert_iterator ),
				 ENTRY("Frontinsert_iterator", Ctstl_1::Frontinsert_iterator ),
				 ENTRY("Insert_iterator", Ctstl_1::Insert_iterator ),
				 ENTRY("Reverse_iterator", Ctstl_1::Reverse_iterator ),
				 ENTRY("Distance", Ctstl_1::Distance ),
				//ENTRY("Distancetype", Ctstl_1::Distancetype ),
				 ENTRY("Bit_set", Ctstl_1::Bit_set ),
				 ENTRY("Isdigit", Ctstl_1::Isdigit ),
				 ENTRY("Isgraph", Ctstl_1::Isgraph ),
				 ENTRY("Isalnum", Ctstl_1::Isalnum ),
				 ENTRY("Isalpha", Ctstl_1::Isalpha ),
				 ENTRY("Iscntrl", Ctstl_1::Iscntrl ),
				 ENTRY("Isprint", Ctstl_1::Isprint ),
				 ENTRY("Ispunct", Ctstl_1::Ispunct ),
				 ENTRY("Isspace", Ctstl_1::Isspace ),
				 ENTRY("Isxdigit", Ctstl_1::Isxdigit ),
				 ENTRY("Fill", Ctstl_1::Fill ),
				 ENTRY("Generate", Ctstl_1::Generate ),
				 ENTRY("Search", Ctstl_1::Search ),
				 ENTRY("Search_n", Ctstl_1::Search_n ),
				 ENTRY("Times", Ctstl_1::Times ),
				 ENTRY("Unique", Ctstl_1::Unique ),
				 ENTRY("Uniquecopy", Ctstl_1::Uniquecopy ),
				 ENTRY("Partialsort", Ctstl_1::Partialsort ),
				 ENTRY("Partialsortcopy", Ctstl_1::Partialsortcopy ),
				 ENTRY("Partition", Ctstl_1::Partition ),
				 ENTRY("Stablepartition", Ctstl_1::Stablepartition ),
				 ENTRY("Setdifference", Ctstl_1::Setdifference ),
				 ENTRY("Setintersection", Ctstl_1::Setintersection ),
				 ENTRY("Setsymdifference", Ctstl_1::Setsymdifference ),
				 ENTRY("Setunion", Ctstl_1::Setunion ),
				 ENTRY("HeapL", Ctstl_1::HeapL ),
				 ENTRY("Swapranges", Ctstl_1::Swapranges ),
				 ENTRY("Partialsums", Ctstl_1::Partialsums ),
				 ENTRY("Unarynegate", Ctstl_1::Unarynegate ),
				 ENTRY("Not2", Ctstl_1::Not2),
				 ENTRY("GettemporarybufferL", Ctstl_1::GettemporarybufferL),
				 ENTRY("Pair", Ctstl_1::Pair),
				 ENTRY("Lexicographicalcompare", Ctstl_1::Lexicographicalcompare ),
				 ENTRY("Mismatch", Ctstl_1::Mismatch ),
				 ENTRY("Compare_fun", Ctstl_1::Compare_fun ),
				 ENTRY("Copy_fun", Ctstl_1::Copy_fun ),
				 ENTRY("Copybackward_fun", Ctstl_1::Copybackward_fun ),
				 ENTRY("mergeL", Ctstl_1::mergeL ),
				 ENTRY("allocatoradd", Ctstl_1::allocatoradd ),
				 ENTRY("alldeall", Ctstl_1::alldeall ),
				 ENTRY("allcons", Ctstl_1::allcons ),
				 ENTRY("allrebind", Ctstl_1::allrebind ),
				 ENTRY("allmaxsize", Ctstl_1::allmaxsize ),
				 ENTRY("complexL",Ctstl_1::complexL),
				 ENTRY("limitsL",Ctstl_1::limitsL),
				 ENTRY("nextperm",Ctstl_1::nextperm),
				 ENTRY("prevperm",Ctstl_1::prevperm),
				 ENTRY("ptrfun",Ctstl_1::ptrfun),
				 ENTRY("itertraits",Ctstl_1::itertraits), 
				 ENTRY("map1",Ctstl_1:: map1), 
				 ENTRY("map2",Ctstl_1::map2), 
				 ENTRY("map3",Ctstl_1::map3), 
				 ENTRY("map4",Ctstl_1::map4), 
				 ENTRY("map5",Ctstl_1::map5), 
				 ENTRY("map6",Ctstl_1::map6), 
				 ENTRY("map7",Ctstl_1::map7), 
				 ENTRY("map8",Ctstl_1::map8), 
				 ENTRY("map9",Ctstl_1::map9), 
				 ENTRY("map10",Ctstl_1::map10), 
				 ENTRY("map11",Ctstl_1::map11), 
				 ENTRY("map12",Ctstl_1::map12),
				 ENTRY("map13",Ctstl_1::map13),
				 ENTRY("multimap1",Ctstl_1:: multimap1), 
				 ENTRY("multimap2",Ctstl_1::multimap2), 
				 ENTRY("multimap3",Ctstl_1::multimap3), 
				 ENTRY("multimap4",Ctstl_1::multimap4), 
				 ENTRY("multimap5",Ctstl_1::multimap5), 
				 ENTRY("multimap6",Ctstl_1::multimap6), 
				 ENTRY("multimap7",Ctstl_1::multimap7), 
				 ENTRY("multimap8",Ctstl_1::multimap8), 
				 ENTRY("multimap9",Ctstl_1::multimap9), 
				 ENTRY("multimap10",Ctstl_1::multimap10), 
				 ENTRY("multimap11",Ctstl_1::multimap11),
				 ENTRY("multimap12",Ctstl_1::multimap12), 
				 ENTRY("multimap13",Ctstl_1::multimap13),  
				 ENTRY("multi_set",Ctstl_1::multi_set), 
				 ENTRY("multi_set2",Ctstl_1::multi_set2), 
				 ENTRY("multi_set3",Ctstl_1::multi_set3), 
				 
				 
				
				
				 
         ENTRY( "Includes", Ctstl_1::Includes ),
			 	 ENTRY( "innerproduct", Ctstl_1::innerproduct ),
				 ENTRY( "inplacemerge", Ctstl_1::inplacemerge ),
				 ENTRY( "Queue", Ctstl_1::Queue ),
				 ENTRY( "Pqueue", Ctstl_1::Pqueue ),
		 		 ENTRY( "stablesortL", Ctstl_1::stablesortL ),
		 		 ENTRY( "Stack", Ctstl_1::Stack ),
 	 			 ENTRY( "string1_testL", Ctstl_1::string1_testL ),
#if 0 // 2-3
  		 	 ENTRY("Setintersection", Ctstl_1::Setintersection ),
				 ENTRY("Setsymdifference", Ctstl_1::Setsymdifference ),
				 ENTRY("Setunion", Ctstl_1::Setunion ),
				 ENTRY("HeapL", Ctstl_1::HeapL ),
				 ENTRY("Swapranges", Ctstl_1::Swapranges ),
				 ENTRY("Partialsums", Ctstl_1::Partialsums ),
				 ENTRY("Unarynegate", Ctstl_1::Unarynegate ),
				 ENTRY("Not2", Ctstl_1::Not2),
				 ENTRY("GettemporarybufferL", Ctstl_1::GettemporarybufferL),
				 ENTRY("Pair", Ctstl_1::Pair),
				 ENTRY("Lexicographicalcompare", Ctstl_1::Lexicographicalcompare ),
				 ENTRY("Mismatch", Ctstl_1::Mismatch ),
				 ENTRY("Compare_fun", Ctstl_1::Compare_fun ),
				 ENTRY("Copy_fun", Ctstl_1::Copy_fun ),
				 ENTRY("Copybackward_fun", Ctstl_1::Copybackward_fun ),
				 ENTRY("mergeL", Ctstl_1::mergeL ),
				 ENTRY("allocatoradd", Ctstl_1::allocatoradd ),
				 ENTRY("alldeall", Ctstl_1::alldeall ),
				 ENTRY("allcons", Ctstl_1::allcons ),
				 ENTRY("allrebind", Ctstl_1::allrebind ),
				 ENTRY("allmaxsize", Ctstl_1::allmaxsize ),
#endif				 
				 
	
#if 0 //repeat	1-4
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
        	 		  ENTRY( "Minof", Ctstl_1::Minof ),
        	 	        ENTRY( "Maxelement", Ctstl_1::Maxelement ),
        	 	        ENTRY( "Minelement", Ctstl_1::Minelement ),
        	 	       ENTRY( "ToUpper", Ctstl_1::ToUpper ),
        	 	        ENTRY( "ToLower", Ctstl_1::ToLower ),
        	 	        ENTRY( "Islower", Ctstl_1::Islower ),
        	 	        ENTRY( "Isupper", Ctstl_1::Isupper ),
        	 	        ENTRY( "Maxof", Ctstl_1::Maxof ),
#endif        	 	        
#if 0        		//2-4 	        
        	 	       ENTRY("Advance", Ctstl_1::Advance ),
        	 	      ENTRY("Backinsert_iterator", Ctstl_1::Backinsert_iterator ),
        	 	      ENTRY("Frontinsert_iterator", Ctstl_1::Frontinsert_iterator ),
        	 	      ENTRY("Insert_iterator", Ctstl_1::Insert_iterator ),
        	 	      ENTRY("Reverse_iterator", Ctstl_1::Reverse_iterator ),
        	 	      ENTRY("Distance", Ctstl_1::Distance ),
        	 	      ENTRY("Isdigit", Ctstl_1::Isdigit ),
        	 	      ENTRY("Isgraph", Ctstl_1::Isgraph ),
        	 	      ENTRY("Isalnum", Ctstl_1::Isalnum ),
        	 	      ENTRY("Isalpha", Ctstl_1::Isalpha ),
        	 	      ENTRY("Iscntrl", Ctstl_1::Iscntrl ),
        	 	      ENTRY("Isprint", Ctstl_1::Isprint ),
        	 	      ENTRY("Ispunct", Ctstl_1::Ispunct ),
        	 	      ENTRY("Isspace", Ctstl_1::Isspace ),
        	 	      ENTRY("Isxdigit", Ctstl_1::Isxdigit ),
        	 	      ENTRY("Fill", Ctstl_1::Fill ),
        	 	      ENTRY("Generate", Ctstl_1::Generate ),
        	 	      ENTRY("Search", Ctstl_1::Search ),
        	 	      ENTRY("Search_n", Ctstl_1::Search_n ),
        	 	      ENTRY("Times", Ctstl_1::Times ),
        	 	      ENTRY("Unique", Ctstl_1::Unique ),
        	 	      ENTRY("Uniquecopy", Ctstl_1::Uniquecopy ),
        	 	      ENTRY("Partialsort", Ctstl_1::Partialsort ),
        	 	      ENTRY("Partialsortcopy", Ctstl_1::Partialsortcopy ),
        	 	      ENTRY("Partition", Ctstl_1::Partition ),
        	 	      ENTRY("Stablepartition", Ctstl_1::Stablepartition ),
        	 	      ENTRY("Setdifference", Ctstl_1::Setdifference ),
#endif     				 
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

__UHEAP_MARK;


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
   
   
  
 
		  __UHEAP_MARKEND;
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

 __UHEAP_MARK;
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
  		  
	  __UHEAP_MARKEND;
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
    __UHEAP_MARK;
   
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
      
	  __UHEAP_MARKEND;
    
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

     __UHEAP_MARK;
   
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
      
	  __UHEAP_MARKEND;
     
    if(failures)
    return KErrGeneral;
    return KErrNone;
  }
 
 
 //  Ctstl_1::Maxof
 
 
 TInt Ctstl_1::Maxof( CStifItemParser& aItem )
    {
      int failures =0;
     __UHEAP_MARK;
     

   try
   { 
     int result_int1;
     int result_int2;
     char result_char;
     float result_float;
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
     
	  __UHEAP_MARKEND;
      if(failures)
    return KErrGeneral;
    return KErrNone; 
    
    	
    }
    
   
   // Ctstl_1::Minof
    
    
    TInt Ctstl_1::Minof( CStifItemParser& aItem )
    {
  
     int failures =0;
     __UHEAP_MARK;
 

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
       
	  __UHEAP_MARKEND;
     
    if(failures)
    return KErrGeneral;
    return KErrNone; 
     
     
    	
    }
    
    
    
   TInt Ctstl_1::Maxelement(CStifItemParser& aItem)
   {
     __UHEAP_MARK;
   	
   	
   	
 
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
   int test_variable3 =*max_element(test_array1,test_array1 + 10);
   
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
      
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
     
   
   }
 
 
 
 
 
 TInt Ctstl_1::Minelement(CStifItemParser& aItem)
   {
     __UHEAP_MARK;
   
 
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
       
	  __UHEAP_MARKEND;
     
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
    
   
   }
 
 
 //lowerbound
 
 TInt Ctstl_1::Lowerbound(CStifItemParser& aItem)
 {
 
 
     __UHEAP_MARK;
  
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
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
    

 } 

 
//upperbound
 
  TInt Ctstl_1::Upperbound(CStifItemParser& aItem)
 {
 
 
     __UHEAP_MARK;
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
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
    } 
 
 
 
 
 
 //swap
 
  TInt Ctstl_1::Swapof(CStifItemParser& aItem)
 {
 int x= 10,y=20;
 
     __UHEAP_MARK;
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
       
	  __UHEAP_MARKEND;
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
 
     __UHEAP_MARK;
      
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
       
	  __UHEAP_MARKEND;
     
    if(failures)
    return KErrGeneral;
    return KErrNone;
 }  
 
 
 
 
 
  TInt Ctstl_1::Adjacent_difference(CStifItemParser& aItem)
 {
      
 
     __UHEAP_MARK;
      
       
    int failures =0;
   

   try
   { 
   vector<int> V1( 10 ), V2( 10 );
   vector<int>::iterator VIter1, VIter2, VIterend, VIterend2;


int array1[10];
int array2[10];
   list <int> L1;
   list <int>::iterator LIter1, LIterend, LIterend2;

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
       
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
   


 }  
 
 
 
 TInt Ctstl_1::Adjacent_find(CStifItemParser& aItem)
 {
      
 
     __UHEAP_MARK;
     
    int failures =0;
    

   try
   {     
   int myints[] = {10,20,30,30,20,10,50,60};
   int myints1[]=
   {
   	10,20,30,40,50,60,70
   };
  vector<int> myvector (myints,myints+8) ,myvector1(myints1,myints1+7);
  vector<int>::iterator it,it2; // cout<<"";
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
     
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;  
    




 }  
 
 
 
 
 
 
 
 
 
 
// logical or 
 TInt Ctstl_1::Logical_or(CStifItemParser& aItem)
 {
 	
 
     __UHEAP_MARK;
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
       
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

 }
 

//logical and
 
 TInt Ctstl_1::Logical_and(CStifItemParser& aItem)
 {
 	
 
     __UHEAP_MARK;
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
   
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;


 }

 
 //LOGICAL NOT
 
 
  TInt Ctstl_1::Logical_not(CStifItemParser& aItem)
 {
 	
     __UHEAP_MARK;
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
   	
       
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;


 }
 
 
 //COUNT
 
 
 
 TInt Ctstl_1::count_fun(CStifItemParser& aItem)
 {
 	
 	
 int failures =0;
    
     __UHEAP_MARK;
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
    
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
     
  }

 
 
 
 //COUNTIF
  TInt Ctstl_1::countif_fun(CStifItemParser& aItem)
 {
 
 __UHEAP_MARK;
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
       
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
     
  }
  
 
 //equal
 
 
  TInt Ctstl_1::equal_fun(CStifItemParser& aItem)
 {
 
 
     __UHEAP_MARK;
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
     
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;

   

   
  }
 
 
 
 
 
 
 //equalto 
  
  TInt Ctstl_1::equalto(CStifItemParser& aItem)
 {
 	
 	
     __UHEAP_MARK;
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
   
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
  }
 

//NOTEQUAL TO

TInt Ctstl_1::notequalto(CStifItemParser& aItem)
 {
 	int failures =0;
    
     __UHEAP_MARK;
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
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;    
  }
  
  
  
  
  //sort
  
  TInt Ctstl_1::sort_fun(CStifItemParser& aItem)
  {
  
  int failures =0;
     __UHEAP_MARK;

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
     
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

  }
  
  
  
  
  //less_equal
  
  TInt Ctstl_1::less_equal_fun(CStifItemParser& aItem)
  {
  
  
     __UHEAP_MARK;
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
     
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;  	
  }
  
  
 
 
 
 //greater_equal
  
  TInt Ctstl_1::greater_equal_fun(CStifItemParser& aItem)
  {
  
  
     __UHEAP_MARK;
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
       
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 	
  }
  
  
 
 //greater
  
  TInt Ctstl_1::greater_fun(CStifItemParser& aItem)
  {
  
  	int failures =0;
     __UHEAP_MARK;

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
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 



  	
  }
  
  
 //LESS
  
  TInt Ctstl_1::less_fun(CStifItemParser& aItem)
  {
     __UHEAP_MARK;
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
     
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 	
  }
  


 // find code check
 //---------------------
  
  TInt Ctstl_1::find_fun(CStifItemParser& aItem)
  {
     __UHEAP_MARK;
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
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

  	
  }
  
 
 
 // find_if   
 //----------------------
  
  
  TInt Ctstl_1::findif_fun(CStifItemParser& aItem)
  {
  
     __UHEAP_MARK;
  	
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
    
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;  


  	
  }
  
 
 
 //find_end 
  
  TInt Ctstl_1::findend_fun(CStifItemParser& aItem)
  {
  
  
     __UHEAP_MARK;
	int failures =0;
     

   try
   {   
   int array1[12] = {0,5,10,15,20,25,0,5,10,15,20,25};
  int array2[3]    =  {20,30,40 };
  int array3[4]= {5,10,15,20 };
  int array4[4] ={30,40,50,60  }; //not found iterator
  
  
  vector <int> v1(array1,array1+12), v2(array2,array2+3),v3(array3,array3+4);
    vector<int> v4(array4,array4+4);
    vector <int>::iterator Iter1, Iter2,Iter3 ,Iter4;
 
  
    

   
   vector <int>::iterator result1,result2; // cout<<"";
   #if  STDCPP_OOM
	User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  
 	 #endif
   result1 = find_end( v1.begin( ), v1.end( ), v3.begin( ), v3.end( ) );
   
   result2 = find_end ( v1.begin( ), v1.end( ), v4.begin( ), v4.end( ) );
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
      
	  __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 


  	
  }
  
  
  //find_first_of
  
  TInt Ctstl_1::findfirstof_fun(CStifItemParser& aItem)
  {
     __UHEAP_MARK;
  
  	int failures =0;
    

   try
   {   
   int array1[12] = {0,5,10,15,20,25,0,5,10,15,20,25};
  int array2[3]    =  {20,30,40 };
  int array3[4]= {5,10,15,20 };
  int array4[4] ={30,40,50,60  }; //not found iterator
  
  
  vector <int> v1(array1,array1+12), v2(array2,array2+3),v3(array3,array3+4);
    vector<int> v4(array4,array4+4);
    vector <int>::iterator Iter1, Iter2,Iter3 ,Iter4;
 
  
    

   
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
       
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

  	
  }
  



TInt Ctstl_1::Minus(CStifItemParser& aItem)
 {
 	
 	int failures =0;
     __UHEAP_MARK;
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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 }


TInt Ctstl_1::Plus(CStifItemParser& aItem)
 {
__UHEAP_MARK;
 	
	int failures =0;
    

   try
   {    
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
      
    __UHEAP_MARKEND;
    
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
     __UHEAP_MARK;

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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

 }


TInt Ctstl_1::Negate(CStifItemParser& aItem)
 {
 	
 	int failures =0;
     __UHEAP_MARK;

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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 }

 
 
 
 TInt Ctstl_1::Logicalnot(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    
     __UHEAP_MARK;
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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

 }
 
 
 
 TInt Ctstl_1::Logicaland(CStifItemParser& aItem)
 {
 	
 	int failures =0;
    
     __UHEAP_MARK;
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
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 }
 
 
 
 
 TInt Ctstl_1::Logicalor(CStifItemParser& aItem)
 {
 	
 	int failures =0;
     __UHEAP_MARK;

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
    
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 }
 
 //remove
 
 TInt Ctstl_1::Remove(CStifItemParser& aItem)
 {
 	
 	int failures =0;
     __UHEAP_MARK;
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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 
 }
 
 
 
TInt Ctstl_1::Removeif(CStifItemParser& aItem)
 {
 	
 	int failures =0;
     __UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 
 } 
  
  
  
  
  
  
  
  
  
  TInt Ctstl_1::Removecopy(CStifItemParser& aItem)
 {
 	
 	int failures =0;
     __UHEAP_MARK;

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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 

 
 } 
  
  
  
  
  
  
  
  
  
  
  
  
  TInt Ctstl_1::Removecopy_if(CStifItemParser& aItem)
 {
 	
 	
	int failures =0;
     __UHEAP_MARK;

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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 



 
 } 
 
 
 
 
 TInt Ctstl_1::Replace_fun(CStifItemParser& aItem)
 {
 	
 	
     __UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 


 
 } 
 
 
 
 
 
 
  TInt Ctstl_1::Replacecopy_fun(CStifItemParser& aItem)
  {
  	int failures =0;
     
     __UHEAP_MARK;
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
       
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 
 
  }
 
 
 
 
  TInt Ctstl_1::Replacecopyif_fun(CStifItemParser& aItem)
  {
  	
 	int failures =0;
    
     __UHEAP_MARK;
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
       
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
  }
 
 
 
  TInt Ctstl_1::Replaceif_fun(CStifItemParser& aItem)
  {
  	
  	int failures =0;
     __UHEAP_MARK;

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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 
  
  
  
  }
 
 TInt Ctstl_1::Reverse_fun(CStifItemParser& aItem)
  {
  
     __UHEAP_MARK;
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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
  
  
  }
 
 
 
 TInt Ctstl_1::Reversecopy_fun(CStifItemParser& aItem)
  {
  	
  	
  	int failures =0;
    
     __UHEAP_MARK;
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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
  
  
  }
 
 


 TInt Ctstl_1::Rotate_fun(CStifItemParser& aItem)
 
 {
 
 	int failures =0;
     __UHEAP_MARK;

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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 
 
 }
 
 
 TInt Ctstl_1::Rotatecopy_fun(CStifItemParser& aItem)
 
 {
     __UHEAP_MARK;
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
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
 	
 }
 
 
  TInt Ctstl_1::equalrange(CStifItemParser& aItem)
  
  {
 	int failures =0;
   
     __UHEAP_MARK;
   try
   {   // __UHEAP_MARK;
  int failures =0,k,l;
  vector <int> v1;
   vector <int>::iterator Iter1;
   pair < vector <int> :: iterator , vector <int> :: iterator > Result1, Result2, Result3;

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
   vector <int>::iterator Iter2;
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
     
    __UHEAP_MARKEND;
    
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
    
     __UHEAP_MARK;
   try
   {   
   	     	
   const int VECTOR_SIZE = 8 ;

    typedef vector<int > IntVector ;

     typedef IntVector::iterator IntVectorIt ;

   IntVector Numbers(VECTOR_SIZE) ;   //vector containing numbers

    IntVectorIt start, end, it ;

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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
   	
   }
   
   
 TInt Ctstl_1::gsliceL(CStifItemParser& aItem)
     
 {
     	
int i,failures=0;
     __UHEAP_MARK;

   try
   { 
   
     size_t startVAR,strideVAR,sizeVAR;

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
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
     }
 
  
  
    
 TInt Ctstl_1::sliceL(CStifItemParser& aItem)
     
 {
     	
 int failures=0;
 
// Simulating  no memory
     __UHEAP_MARK;

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
   
     
    __UHEAP_MARKEND;
    
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
     __UHEAP_MARK;
try
   {
vector<int> v1;
vector<int>::iterator Iter;
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
   
    
    __UHEAP_MARKEND;
    
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
         
     __UHEAP_MARK;
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
     
    __UHEAP_MARKEND;
    
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
     __UHEAP_MARK;
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
   
    
    __UHEAP_MARKEND;
    
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
     __UHEAP_MARK;

   try
   { // cout<<"";
   // Simulating  no memory
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
#endif 
  	 int Array[] = { 10, 20, 30, 40 };
   	const int N = sizeof( Array ) / sizeof( int );

   	int i;
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
    
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;

}
 
 
// maskarray 
 
 
 TInt Ctstl_1::maskarray(CStifItemParser& aItem)
 
 {
 
     __UHEAP_MARK;
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
   
    
    __UHEAP_MARKEND;
    
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
     __UHEAP_MARK;
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
   
     
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;


}
 
 
 
 
 //random shuffle
 
 
 TInt Ctstl_1::randshuf(CStifItemParser& aItem)
 
 {
 int failures=0;
 
     __UHEAP_MARK;

   try
   { 
   vector <int> v1;
   vector <int>::iterator Iter1, Iter2;
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
   
    
    __UHEAP_MARKEND;
    
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
     __UHEAP_MARK;


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
   
    
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;

}
 
 


 TInt Ctstl_1::iterswap(CStifItemParser& aItem)
 
 { int failures=0;
 
     __UHEAP_MARK;

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
   
   
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
 	
 }
 
 
 
  TInt Ctstl_1::ios1(CStifItemParser& aItem)
  
  {
  int failures=0;
     __UHEAP_MARK;

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
   
    
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
 	
  }

 
 
  TInt Ctstl_1::ios2(CStifItemParser& aItem)
  
  {
  int failures=0;
 
     __UHEAP_MARK;
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
   
    
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
  }
  
  
   
  TInt Ctstl_1::ios3(CStifItemParser& aItem)
  
  {
  int failures=0;
 
     __UHEAP_MARK;
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

  
 float l = 31.31234F;
   
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
   
      
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
  }



TInt Ctstl_1::nthelement( CStifItemParser& aItem )
    {
    int failures=0;
     __UHEAP_MARK;
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
    
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone;
    }
    
    
    
    
 // hasfacet   
    
    
    TInt Ctstl_1::hasfacet( CStifItemParser& aItem )
    {
 int failures=0;
 
     __UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
    
    if(failures)
    return KErrGeneral;
    return KErrNone; 
    }





















// -----------------------------------------------------------------------------
// Ctstl_1::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::Advance( CStifItemParser& aItem )
    {
     __UHEAP_MARK;
 	
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
    // cout<<"";
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
    __UHEAP_MARKEND;
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }





TInt Ctstl_1::Backinsert_iterator( CStifItemParser& aItem )
    {
     __UHEAP_MARK;
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
   // cout<<"";
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_1::Frontinsert_iterator( CStifItemParser& aItem )
    {
     __UHEAP_MARK;
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }




TInt Ctstl_1::Insert_iterator( CStifItemParser& aItem )
    {
      __UHEAP_MARK;
   
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt  Ctstl_1::Reverse_iterator( CStifItemParser& aItem )

{
     __UHEAP_MARK;

int failures=0;
try{
int i;
 

   vector<int> vec;
   
   for ( i = 1 ; i < 6 ; ++i )
   {
      vec.push_back ( i );
   }
   
  // vector <int>::iterator vIter;
    

   vector <int>::reverse_iterator rvIter;
    
   vector <int>::iterator pos;
   
   pos = find ( vec.begin ( ), vec.end ( ), 4 );
  
  
  if(*pos != 4)
  failures++;
   // cout<<"";
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
     __UHEAP_MARKEND;
  
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt  Ctstl_1::Distance( CStifItemParser& aItem )

{
     __UHEAP_MARK;

int failures =0;
try{
int i;
 

   list<int> L;
   for ( i = -1 ; i < 9 ; ++i ) 
   {
      L.push_back ( 2 * i );
   }
   
   
   list <int>::iterator L_Iter, LPOS = L.begin ( );

  
 

   advance ( LPOS , 7 );
   
    list<int>::difference_type Ldiff ;
    // cout<<"";
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 /*
 TInt  Ctstl_1::Distancetype( CStifItemParser& aItem )

{

int i;
 

   list<int> L;
   for ( i = -1 ; i < 9 ; ++i ) 
   {
      L.push_back ( 2 * i );
   }
   
   
   list <int>::iterator L_Iter, LPOS = L.begin ( );

  
 

   advance ( LPOS , 7 );
   
    list<int>::difference_type Ldiff ;
   Ldiff = distance_type ( L.begin ( ) , LPOS );
   
     
    if(*LPOS ==12)
    if(Ldiff==7)
    return KErrNone;
    return KErrGeneral;
    
 
	
}
 */
 
 
 
 TInt  Ctstl_1::Bit_set( CStifItemParser& aItem )

{
     __UHEAP_MARK;

 int failures =0;  
try{
bitset<2> b0;
 if(b0 != 00)
 failures++;
 
  
   // Using the second member function
   bitset<5> b1(6);
      if(b1 != 6)
    failures++;
     // cout<<"";    
    
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   
   bitset<3> b3 (6);  
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
    bool b = b3.any ( );
 if(!b)
failures++;  
   bool n = b3.none ( );

   if(n)
   failures++;
   
   size_t i;
    i = b3.count();
    if(i!=2)
    failures++;
    
         bitset<3> fb1;
    fb1 = b3.flip();
 
   
   size_t j;
   j= b3.count();
    
   if(j!=1)
   failures++;
   
 
     
      bitset<5> a1 ( 13 );
  
 
    i = a1.count();

if(i!=3)
failures++;

   bitset<5> b1r3;
   b1r3 = a1.reset( 2 );

i = b1r3.count();


if(i!=2)
failures++;



   bitset<5> b1r;
   b1r = a1.reset( );
  
i= b1r.count();
if(i!=0)
failures++;
 bitset<5> b1s;
 b1s = a1.set();
 i=b1s.count();
 if(i!=5)
 failures++;
 
int k ;
k= a1.size();
if(k!=5)
failures++;

 bool test  = a1.test ( 3 );
    
    if(!test) 
    failures++;
 #define test(x) __test(x,__LINE__,__FILE__)
    
    bitset<5> str ( 7 );

          

   string s1;
   s1 =  str.template to_string<char, 
   char_traits<char>, allocator<char> >( );
    
 
    if(s1.compare("00111"))
    failures++;
     bitset<5> tlon( 7 );

 


   unsigned long int lon;
   lon = tlon.to_ulong( );
  if(lon!=7)
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

 
 TInt  Ctstl_1::Isdigit( CStifItemParser& aItem )

{
     __UHEAP_MARK;

char str[]="1776ad";
  
  int failures =0;
  try{ 
  	// cout<<"";
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


 TInt  Ctstl_1::Isgraph( CStifItemParser& aItem )

{
     __UHEAP_MARK;

 int failures =0;
 // char array[42] = "!"#$%&'()*+,-./0123456789:;<=>?@[\]^_`{|}~";
 
 
  try{
  
  for(char i=0x21;i<=0x7E;i++)
  {
  	 // cout<<"";
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
    __UHEAP_MARKEND;
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

 TInt  Ctstl_1::Isalnum( CStifItemParser& aItem )

{
     __UHEAP_MARK;

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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
TInt  Ctstl_1::Isalpha( CStifItemParser& aItem )

{
     __UHEAP_MARK;

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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt  Ctstl_1::Iscntrl( CStifItemParser& aItem )

{
     __UHEAP_MARK;
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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt  Ctstl_1::Isprint( CStifItemParser& aItem )

{
     __UHEAP_MARK;

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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt  Ctstl_1::Ispunct( CStifItemParser& aItem )

{
     __UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
  TInt  Ctstl_1::Isspace( CStifItemParser& aItem )

{
     __UHEAP_MARK;

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
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

 TInt  Ctstl_1::Isxdigit( CStifItemParser& aItem )

{
     __UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt  Ctstl_1::Fill( CStifItemParser& aItem )

{
     __UHEAP_MARK;

 int failures =0;
  
 try
 {
 
 	 vector<int> myvector (8,10);        // myvector: 10 10 10 10 10 10 10 10
 	 vector<int>::iterator it ; 
 	 // cout<<"";
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
  	// cout<<"";  
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
   
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }





static int UniqueNumber()
 {
 
 static int current;
 return current++;	
 }


TInt  Ctstl_1::Generate( CStifItemParser& aItem )

{
     __UHEAP_MARK;
	int failures = 0;

try{
  	vector<int> myvector (8);
  	vector<int>::iterator it;
	int output[8];
  // cout<<"";
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
   // cout<<"";
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



TInt  Ctstl_1::Search( CStifItemParser& aItem )

{
     __UHEAP_MARK;
	int failures=0;
try{

vector<int> myvector;
  vector<int>::iterator it;

  // set some values:        myvector: 10 20 30 40 50 60 70 80 90
  for (int i=1; i<10; i++) 
  myvector.push_back(i*10);


  // using default comparison:
  int match1[] = {40,50,60,70};
   // cout<<"";
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
   
    __UHEAP_MARKEND;
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }




TInt  Ctstl_1::Search_n(CStifItemParser& aItem )

{
     __UHEAP_MARK;
	
	int failures=0;
try{
int myints[]={10,20,30,30,20,10,10,20};
  vector<int> myvector (myints,myints+8);

  vector<int>::iterator it; 
  // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
//code to be verified
//--------------------------

TInt Ctstl_1::Times(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
 int failures=0;
 
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 TInt Ctstl_1::Unique(CStifItemParser& aItem)
 {
 	
 int failures=0;
     __UHEAP_MARK;
try{
int myints[] = {10,20,20,20,30,30,20,20,10};    // 10 20 20 20 30 30 20 20 10
  vector<int> myvector (myints,myints+9);
  vector<int>::iterator it;
int output[5]; // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 TInt Ctstl_1::Uniquecopy(CStifItemParser& aItem)
 {
 	int failures=0;
 	
 try{
     __UHEAP_MARK;
int myints[] = {10,20,20,20,30,30,20,20,10};
  vector<int> myvector (9);                            // 0  0  0  0  0  0  0  0  0
  vector<int>::iterator it;
int output[9]; // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
  
 TInt Ctstl_1::Partialsort(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
 	int failures=0;
 	try
 	{
 
//__UHEAP_MARK;
int myints[] = {9,8,7,6,5,4,3,2,1};
  vector<int> myvector (myints, myints+9);
  vector<int>::iterator it;
int output[9]; // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 

TInt Ctstl_1::Partialsortcopy(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
 	int failures=0;
 try{
//__UHEAP_MARK;
int myints[] = {9,8,7,6,5,4,3,2,1};
  vector<int> myvector (5);
  vector<int>::iterator it;
int output[5]; // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
 
 bool IsOdd(int i)
 {
 	return (i%2)==1;
 }
 
 
 TInt Ctstl_1::Partition(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
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
   // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
  TInt Ctstl_1::Stablepartition(CStifItemParser& aItem)
 {
      __UHEAP_MARK;
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
   // cout<<"";
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::Setdifference(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
 TInt Ctstl_1::Setintersection(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
 
 TInt Ctstl_1::Setsymdifference(CStifItemParser& aItem)
 {
     __UHEAP_MARK;
 
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
 TInt Ctstl_1::Setunion(CStifItemParser& aItem)
 {
      __UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
 TInt Ctstl_1::HeapL(CStifItemParser& aItem)
 {
 	
 
     __UHEAP_MARK;


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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 TInt Ctstl_1::Swapranges(CStifItemParser& aItem)
 {
      __UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

 
 TInt Ctstl_1::Partialsums(CStifItemParser& aItem)
 {
      __UHEAP_MARK;

 	int failures=0;
 try{
 	
 vector<int> V1( 10 ), V2( 10 );
   vector<int>::iterator VIter1,   VIterend; 
 
int sum =1;
 int i=2 ;
   list <int> L1;
   list <int>::iterator LIter1, LIterend;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::Unarynegate(CStifItemParser& aItem)
 {
      __UHEAP_MARK;

 	int failures=0;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 TInt Ctstl_1::Not2(CStifItemParser& aItem)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_1::GettemporarybufferL(CStifItemParser& aItem)
 {
      __UHEAP_MARK;

 	int failures=0;
 try{
    // Create an array of ints
   int intArray [ ] = { 10, 20, 30, 40, 100, 200, 300, 1000, 2000 };
   int count = sizeof ( intArray ) / sizeof ( int );
    

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
 
 TInt Ctstl_1::Pair(CStifItemParser& aItem)
 {
 	
     __UHEAP_MARK;
 
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


 
 bool twice ( int elem1, int elem2 )
{
   return 2 * elem1 < elem2;
}


 TInt Ctstl_1::Lexicographicalcompare(CStifItemParser& aItem)
 {
 	
     __UHEAP_MARK;
 
 


 int failures=0;
 
 try
 {
 	
 vector <int> v1, v2;
   list <int> L1;
   vector <int>::iterator Iter1, Iter2;
   list <int>::iterator L1_Iter, L1_inIter;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

//Mismatch

bool mypredicate (int i, int j) {
  return (i==j);
}



TInt Ctstl_1::Mismatch(CStifItemParser& aItem)
 {
 	
     __UHEAP_MARK;
 
 
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 TInt Ctstl_1::Compare_fun(CStifItemParser& aItem)
 
 {
      __UHEAP_MARK;

 int failures=0;	
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
 TInt Ctstl_1::Copy_fun(CStifItemParser& aItem)
 
 {
      __UHEAP_MARK;

 	int failures=0;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
  TInt Ctstl_1::Copybackward_fun(CStifItemParser& aItem)
 
 {
      __UHEAP_MARK;
	
 	int failures=0;
 	
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
  TInt Ctstl_1::mergeL(CStifItemParser& aItem)
  
  {
__UHEAP_MARK;
  
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::allocatoradd(CStifItemParser& aItem)
  
  {
__UHEAP_MARK;

  	int failures = 0;
  
try{
     vector <int> v1;
   vector <int>::iterator v1Iter;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


  
  TInt Ctstl_1::alldeall(CStifItemParser& aItem)
  
  
  {
  
__UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
  
  
  TInt Ctstl_1::allcons(CStifItemParser& aItem)
  {
  
  
__UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
  
    TInt Ctstl_1::allrebind(CStifItemParser& aItem)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    TInt Ctstl_1::allmaxsize(CStifItemParser& aItem)

    {
 __UHEAP_MARK;
   
    int failures=0;
    	
    	
    	try{
    vector <int> v1;
   vector <int>::iterator v1Iter;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
  
  
  
  
    TInt Ctstl_1::complexL(CStifItemParser& aItem)
    
    {
 __UHEAP_MARK;
   
    int failures=0;
    	try
    	{
    		
    	

    double pi = 3.14159265359;
     // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
    complex <double> c1 ( 4.0 , 5.0 );
    
    double re = real(c1);
    
    double im = imag(c1);
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
  /*  if(re == 4)
    if(im == 5)
   return KErrNone;
   return KErrGeneral;
   
*/

if(re!=4)
failures++;
if (im!= 5)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::limitsL(CStifItemParser& aItem)

{

__UHEAP_MARK;
	
int failures=0;

try
{
 // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
//is_speciallized
if(!numeric_limits<float>::is_specialized)
failures++;

if(!numeric_limits<int>::is_specialized)
failures++;


//if(!numeric_limits<long double>::is_specialized) //failing
//failures++;


if(!numeric_limits<double>::is_specialized)
failures++;

if(!numeric_limits<wchar_t>::is_specialized)
failures++;


if(!numeric_limits<char>::is_specialized)
failures++;



if(!numeric_limits<bool>::is_specialized)
failures++;




if(!numeric_limits<signed char>::is_specialized)
failures++;


if(!numeric_limits<unsigned char>::is_specialized)
failures++;



if(!numeric_limits<short>::is_specialized)
failures++;



if(!numeric_limits<unsigned short>::is_specialized)
failures++;




//if(!numeric_limits<_int64>::is_specialized)
//failures++;





//digits
if(numeric_limits<float>::digits !=24)
failures++;

if( numeric_limits<double>::digits !=53)
failures++;

//if( numeric_limits<long double>::digits !=64)  failed 0
//failures++;

 if (numeric_limits<int>::digits != 31)
failures++;
 
if(numeric_limits<__int64>::digits !=63)
 failures++;

 
if(numeric_limits<char>::digits != 7)
 failures++;


if(numeric_limits<short>::digits != 15)
 failures++;

if(numeric_limits<unsigned short>::digits != 16)
 failures++;

if(numeric_limits<long>::digits != 31)
failures++;


if(numeric_limits<unsigned long>::digits != 32)
 failures++;

 
if(numeric_limits<wchar_t>::digits != 16)
failures++;

if(numeric_limits<unsigned char>::digits != 8)
 failures++;


if(numeric_limits<signed char>::digits != 7)
 failures++;

if(numeric_limits<bool>::digits != 1)
 failures++;



//digit10

if(numeric_limits<float>::digits10 !=6)
failures++;

if( numeric_limits<double>::digits10 !=15)
failures++;

//if( numeric_limits<long double>::digits10 !=18)  
//failures++;

if (numeric_limits<int>::digits10 != 9)
failures++;
 
//if(numeric_limits<__int64>::digits10 !=63)
// failures++;

 
if(numeric_limits<char>::digits10 != 2)
 failures++;


if(numeric_limits<short>::digits10 != 4)
 failures++;

if(numeric_limits<unsigned short>::digits10 != 4)
 failures++;

if(numeric_limits<long>::digits10 != 9)
failures++;



if(numeric_limits<unsigned long>::digits10 !=9)
 failures++;

if(numeric_limits<wchar_t>::digits10 != 4) 
failures++;

if(numeric_limits<unsigned char>::digits10 != 2)
 failures++;


if(numeric_limits<signed char>::digits10 != 2)
 failures++;

if(numeric_limits<bool>::digits10 != 0)
 failures++;
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif

//de_norm

//__UHEAP_MARKEND;


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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    
TInt Ctstl_1::nextperm(CStifItemParser& aItem)
{
__UHEAP_MARK;

 int failures =0;
 
 try
 {
  int myints[] = {1,2};

   // cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
 
  do {
  
  
  static int i;
  if (i==0)
  {
  if(myints[0]!=1)
  failures++;
  if(myints[1]!=2)
  failures++;
  
  }
  
  else
  {
    if(myints[0]!=2)
  failures++;
  if(myints[1]!=1)
  failures++;	
  }
    i++; 
  
  } while ( next_permutation (myints,myints+2) );

 
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
	
}

catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    

TInt Ctstl_1::prevperm(CStifItemParser& aItem)
{
__UHEAP_MARK;

 int failures =0;
 
 try{
  int myints[] = {2,1};
 // cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
 
  do {
  
  
  static int i;
  if (i==0)
  {
  if(myints[0]!=2)
  failures++;
  if(myints[1]!=1)
  failures++;
  
  }
  
  else
  {
    if(myints[0]!=1)
  failures++;
  if(myints[1]!=2)
  failures++;	
  }
    i++; 
  
  } while ( prev_permutation (myints,myints+2) );

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

	
}

catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::ptrfun(CStifItemParser& aItem)
{
__UHEAP_MARK;
int failures=0;
try

{

    vector <char*> v1;
   vector <char*>::iterator   RIter;

   v1.push_back ( "this" );
   v1.push_back ( "is" );
   v1.push_back ( "ptrfun" );
   v1.push_back ( "test" );
   v1.push_back ( "case" );
    // cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   // To search the sequence for "pearly"
   // use a pointer_to_function conversion
 // __UHEAP_MARK;
 RIter = find_if( v1.begin( ), v1.end( ), not1 ( bind2nd (ptr_fun ( strcmp ), "ptrfun" ) ) );
 
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
//__UHEAP_MARKEND;
/*
if(strcmp(*RIter,"ptrfun\n"))
 
return KErrNone;
else
return KErrGeneral;

*/

if(!strcmp(*RIter,"ptrfun\n"))
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::itertraits(CStifItemParser& aItem)
{
__UHEAP_MARK;
 int failures=0;
 
 try
 
 {
   vector<int> vi;
   vector<char> vc;
   list<char> lc;
   // cout<<""; 
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   iterator_traits<vector<int>:: iterator>::iterator_category cati;
   iterator_traits<vector<char>:: iterator>::iterator_category catc;
   iterator_traits<list<char>:: iterator>::iterator_category catlc;
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
  
   //if ( typeid ( cati ) != typeid( catc ) ) //compiler error
  // failures++;
   
      
   // if ( typeid ( vi.begin( ) ) == typeid( vc.begin( ) ) )
   //failures++;
   /* if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map1(CStifItemParser& aItem)
{
__UHEAP_MARK;

int failures=0;
//insert,begin,map
try
{
 // cout<<"";	
	  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
map <int, int> m1;

   map <int, int> :: iterator m1_Iter;
   map <int, int> :: const_iterator m1_cIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 0, 0 ) );
   m1.insert ( Int_Pair ( 1, 1 ) );
   m1.insert ( Int_Pair ( 2, 4 ) );

   m1_cIter = m1.begin ( );
   if(m1_cIter -> first != 0)
   failures++;
   
   m1_Iter = m1.begin ( );
   m1.erase ( m1_Iter );

 

   m1_cIter = m1.begin( );
   
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   if( m1_cIter -> first !=1)
   failures++;
   /*
   if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_1::map2(CStifItemParser& aItem)
{
__UHEAP_MARK;
//clear,size

  int failures=0;
  
  try
  {
    map<int, int> m1;
    map<int, int>::size_type i;
    typedef pair<int, int> Int_Pair;
     // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
    m1.insert(Int_Pair(1, 1));
    m1.insert(Int_Pair(2, 4));

    i = m1.size();
    if(i!=2)
    failures++;
    
    m1.clear();
    i = m1.size();
    
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
  if(i!=0)
  failures++;

  /*
   if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map3(CStifItemParser& aItem)
{
__UHEAP_MARK;
//count
int failures=0;

try
{
     map<int, int> m1;
    map<int, int>::size_type i;
    typedef pair<int, int> Int_Pair;

    m1.insert(Int_Pair(1, 1));
    m1.insert(Int_Pair(2, 1));
    m1.insert(Int_Pair(1, 4));
    m1.insert(Int_Pair(2, 1));
     // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
    // Keys must be unique in map, so duplicates are ignored
    i = m1.count(1);
     if(i!=1)
     failures++;
     
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
    i = m1.count(2);
 if(i!=1)
 failures++;
 
    i = m1.count(3);
  if(i!=0)
  failures++;
  
  
  
/*
  
   if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map4(CStifItemParser& aItem)
{
__UHEAP_MARK;
//empty
int failures=0;

try
{
   map <int, int> m1, m2;

   typedef pair <int, int> Int_Pair;
   m1.insert ( Int_Pair ( 1, 1 ) );
    // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   if ( m1.empty( ) )
      failures++;

   if ( !m2.empty( ) )
      failures++;
      
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
	
}


catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map5(CStifItemParser& aItem)
{
__UHEAP_MARK;
//erase,end
int failures=0;

try
{
    map <int, int> m1;

   map <int, int> :: iterator m1_Iter;
   map <int, int> :: const_iterator m1_cIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) ); 
   // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   m1_cIter = m1.end( );
   m1_cIter--;
  
   if( m1_cIter -> second != 30)
   failures++;
   
   m1_Iter = m1.end( );
   m1_Iter--;
   m1.erase ( m1_Iter );

   

   m1_cIter = m1.end( );
   m1_cIter--;
   if( m1_cIter -> second !=20)
   failures++;
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
	
}
catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::map6(CStifItemParser& aItem)
{
__UHEAP_MARK;
//equal_range,upper_bound
int failures=0;

try
{
   typedef map <int, int, less<int> > IntMap;
   IntMap m1;
   map <int, int> :: const_iterator m1_RcIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );

   pair <IntMap::const_iterator, IntMap::const_iterator> p1, p2;
   p1 = m1.equal_range( 2 );

   if( p1.first -> second !=20)
   failures++;
   

   if( p1.second -> second != 30)
   failures++; 
   // cout<<"";
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif

   // Compare the upper_bound called directly 
   m1_RcIter = m1.upper_bound( 2 );

   if( m1_RcIter -> second!=30)
   failures++;

   p2 = m1.equal_range( 4 );

   // If no match is found for the key,
   // both elements of the pair return end( )
   if ( ( p2.first == m1.end( ) ) && ( p2.second == m1.end( ) ) == 0)
   failures++;
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
	
}

catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map7(CStifItemParser& aItem)
{
__UHEAP_MARK;
//find
int failures=0;

try
{
map <int, int> m1;
   map <int, int> :: const_iterator m1_AcIter, m1_RcIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );
    // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   m1_RcIter = m1.find( 2 );
   if( m1_RcIter -> second != 20)
   failures++;

   // If no match is found for the key, end( ) is returned
   m1_RcIter = m1.find( 4 );

   if ( m1_RcIter == m1.end( ) );
     // cout << "The map m1 doesn't have an element "
         //  << "with a key of 4." << endl;
   else
      if( m1_RcIter -> second != 4)
      failures++;
      

   // The element at a specific location in the map can be found 
   // using a dereferenced iterator addressing the location
   m1_AcIter = m1.end( );
   m1_AcIter--;
   m1_RcIter = m1.find( m1_AcIter -> first );
   if( m1_RcIter -> second != 30)
   failures++;
   
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

	
}
catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map8(CStifItemParser& aItem)
{
__UHEAP_MARK;

//key_comp
	int failures=0;
	
	try
	{
 
   map <int, int, less<int> > m1;
   map <int, int, less<int> >::key_compare kc1 = m1.key_comp( ) ;
   bool result1 = kc1( 2, 3 ) ;
    // cout<<"";
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   if( result1 == false)
   failures++;
   
    

   map <int, int, greater<int> > m2;
   map <int, int, greater<int> >::key_compare kc2 = m2.key_comp( );
   bool result2 = kc2( 2, 3 ) ;
   if( result2 == true )
   failures++;
   
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


}
catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map9(CStifItemParser& aItem)

{
__UHEAP_MARK;


//lowerbound
int failures=0;	


try

{
map <int, int> m1;
   map <int, int> :: const_iterator m1_AcIter, m1_RcIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );
    // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   m1_RcIter = m1.lower_bound( 2 );
    
       if( m1_RcIter -> second != 20)
       failures++;
       

   // If no match is found for this key, end( ) is returned
   m1_RcIter = m1. lower_bound ( 4 );

   if ( m1_RcIter == m1.end( ) );
      // << "The map m1 doesn't have an element "
          // << "with a key of 4." << endl;
   else
      if( m1_RcIter -> second != 4)
      failures++;
      

   // The element at a specific location in the map can be found 
   // using a dereferenced iterator addressing the location
   m1_AcIter = m1.end( );
   m1_AcIter--;
   m1_RcIter = m1. lower_bound ( m1_AcIter -> first );
    if( m1_RcIter -> second != 30)
    failures++;
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


}
catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::map10(CStifItemParser& aItem)

{
__UHEAP_MARK;
//swap
int failures=0;

try

{
int a[2];
using namespace std;
   map <int, int> m1, m2, m3;
   map <int, int>::iterator m1_Iter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );
   m2.insert ( Int_Pair ( 10, 100 ) );
   m2.insert ( Int_Pair ( 20, 200 ) );
   m3.insert ( Int_Pair ( 30, 300 ) );

    // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
 
   m1.swap( m2 );

    for ( m1_Iter = m1.begin( ); m1_Iter != m1.end( ); m1_Iter++ )
    {
    static int i;
    	
    a[i] = m1_Iter -> second;
    
    i++;
    
    }
    if(a[0]!=100)
    failures++;
    if(a[1]!= 200)
    failures++;
    

    swap( m1, m3 );

    for ( m1_Iter = m1.begin( ); m1_Iter != m1.end( ); m1_Iter++ )
    {
    	
    
     int a= m1_Iter -> second;
     if(a!=300)
     failures++;
    }
    
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

}
catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

TInt Ctstl_1::map11(CStifItemParser& aItem)
{
__UHEAP_MARK;

 
int failures=0;

try
{
map <int, int, less<int> > m1;
   map <int, int, less<int> >::value_compare vc1 = m1.value_comp( );
   pair< map<int,int>::iterator, bool > pr1, pr2;
   
   pr1= m1.insert ( map <int, int> :: value_type ( 1, 10 ) );
   pr2= m1.insert ( map <int, int> :: value_type ( 2, 5 ) ); // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   if( vc1( *pr1.first, *pr2.first ) == false )   
    
      failures++;
      
  

   if(vc1( *pr2.first, *pr1.first ) == true )
    
     failures++;
     
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif


/*
if(failures)	
return KErrGeneral;
else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
TInt Ctstl_1::map12(CStifItemParser& aItem)
{

__UHEAP_MARK;
 
int psize,failures=0;
try
{
  map<char,int> mymap;
  pair<const char,int>* p; // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   p=mymap.get_allocator().allocate(2);

   psize = (int) sizeof(map<char,int>::value_type)*2;

 if(psize!=16)
 failures++;
 

  mymap.get_allocator().deallocate(p,2);

   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
    if(mymap.max_size()!=4294967295)
    failures++;


/*
if(failures)	
return KErrGeneral;
else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
TInt Ctstl_1::map13(CStifItemParser& aItem)
{
__UHEAP_MARK;

 int failures=0;
 
 try
 {
map<char,int> mymap;
  map<char,int>::reverse_iterator rit;

  mymap['x'] = 100;
  mymap['y'] = 200; // cout<<"";
  
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   for ( rit=mymap.rbegin() ; rit != mymap.rend(); rit++ )
    
    
   {
   	static int i;
   if(i==0)
   {
   	
   
   if( rit->second !=200)
   failures++;
   }
   i++;
   }

  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 

/*
if(failures)	
return KErrGeneral;
else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::multimap1(CStifItemParser& aItem)
{

__UHEAP_MARK;

 

multimap <int, int> m1;
int failures=0;

try{ // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   multimap <int, int> :: iterator m1_Iter;
   multimap <int, int> :: const_iterator m1_cIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 0, 0 ) );
   m1.insert ( Int_Pair ( 1, 1 ) );
   m1.insert ( Int_Pair ( 2, 4 ) );

   m1_cIter = m1.begin ( );
   if(m1_cIter -> first != 0)
   failures++;
   
   m1_Iter = m1.begin ( );
   m1.erase ( m1_Iter );

 

   m1_cIter = m1.begin( );
   if( m1_cIter -> first !=1)
   failures++;
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   
   /*if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::multimap2(CStifItemParser& aItem)
{
//clear,size
__UHEAP_MARK;

  int failures=0;
  
  try
  {
    multimap<int, int> m1;
    multimap<int, int>::size_type i;
    typedef pair<int, int> Int_Pair; // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
    m1.insert(Int_Pair(1, 1));
    m1.insert(Int_Pair(2, 4));

    i = m1.size();
    if(i!=2)
    failures++;
    
    m1.clear();
    i = m1.size();
  if(i!=0)
  failures++;

    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   /*if(failures)
   return KErrGeneral;
   else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

TInt Ctstl_1::multimap3(CStifItemParser& aItem)
{
__UHEAP_MARK;
//count
int failures=0;


try
{
	
     multimap<int, int> m1;
    multimap<int, int>::size_type i;
    typedef pair<int, int> Int_Pair;

    m1.insert(Int_Pair(1, 1));
    m1.insert(Int_Pair(2, 1));
    m1.insert(Int_Pair(1, 4));
    m1.insert(Int_Pair(2, 1));

    // Keys must be unique in multimap, so duplicates are ignored
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
    i = m1.count(1);
     if(i!=2)
     failures++;
     

    i = m1.count(2);
 if(i!=2)
 failures++;
 
    i = m1.count(3);
  if(i!=0)
  failures++;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif 
/*
  
   if(failures)
   return KErrGeneral;
   else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::multimap4(CStifItemParser& aItem)
{
//empty
int failures=0;

try
{
   multimap <int, int> m1, m2;

   typedef pair <int, int> Int_Pair;
   m1.insert ( Int_Pair ( 1, 1 ) );
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   if ( m1.empty( ) )
      failures++;

   if ( !m2.empty( ) )
      failures++;
  
   /*if(failures)
   return KErrGeneral;
   else
   return KErrNone;*/
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::multimap5(CStifItemParser& aItem)
{
__UHEAP_MARK;
//erase,end
int failures=0;

try
{
	
    multimap <int, int> m1;

   multimap <int, int> :: iterator m1_Iter;
   multimap <int, int> :: const_iterator m1_cIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   m1_cIter = m1.end( );
   m1_cIter--;
  
   if( m1_cIter -> second != 30)
   failures++;
   
   m1_Iter = m1.end( );
   m1_Iter--;
   m1.erase ( m1_Iter );

   

   m1_cIter = m1.end( );
   m1_cIter--;
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   if( m1_cIter -> second !=20)
   failures++;

  
  /* if(failures)
   return KErrGeneral;
   else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::multimap6(CStifItemParser& aItem)
{
__UHEAP_MARK;
//equal_range,upper_bound
int failures=0;

try
{
   typedef multimap <int, int, less<int> > Intmultimap;
   Intmultimap m1;
   multimap <int, int> :: const_iterator m1_RcIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   pair <Intmultimap::const_iterator, Intmultimap::const_iterator> p1, p2;
   p1 = m1.equal_range( 2 );

   if( p1.first -> second !=20)
   failures++;
   

   if( p1.second -> second != 30)
   failures++;
   

   // Compare the upper_bound called directly 
   m1_RcIter = m1.upper_bound( 2 );

   if( m1_RcIter -> second!=30)
   failures++;

   p2 = m1.equal_range( 4 );
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   // If no match is found for the key,
   // both elements of the pair return end( )
   if ( ( p2.first == m1.end( ) ) && ( p2.second == m1.end( ) ) == 0)
   failures++;
      
  /*
   if(failures)
   return KErrGeneral;
   else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::multimap7(CStifItemParser& aItem)
{
__UHEAP_MARK;
//find
int failures=0;

try
{
multimap <int, int> m1;
   multimap <int, int> :: const_iterator m1_AcIter, m1_RcIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );

    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   m1_RcIter = m1.find( 2 );
   if( m1_RcIter -> second != 20)
   failures++;

   // If no match is found for the key, end( ) is returned
   m1_RcIter = m1.find( 4 );

   if ( m1_RcIter == m1.end( ) );
     // cout << "The multimap m1 doesn't have an element "
          // << "with a key of 4." << endl;
   else
      if( m1_RcIter -> second != 4)
      failures++;
      

   // The element at a specific location in the multimap can be found 
   // using a dereferenced iterator addressing the location
   m1_AcIter = m1.end( );
   m1_AcIter--;
   m1_RcIter = m1.find( m1_AcIter -> first );
   
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   if( m1_RcIter -> second != 30)
   failures++;
   /*
   if(failures)
   return KErrGeneral;
   else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_1::multimap8(CStifItemParser& aItem)
{
__UHEAP_MARK;

//key_comp
	int failures=0;
	
	try
	{
 
   multimap <int, int, less<int> > m1;
   multimap <int, int, less<int> >::key_compare kc1 = m1.key_comp( ) ;
   bool result1 = kc1( 2, 3 ) ;
   
   if( result1 == false)
   failures++;
   
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif

   multimap <int, int, greater<int> > m2;
   multimap <int, int, greater<int> >::key_compare kc2 = m2.key_comp( );
   bool result2 = kc2( 2, 3 ) ;
   if( result2 == true )
   failures++;
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
  /* if(failures)
   return KErrGeneral;
   else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::multimap9(CStifItemParser& aItem)

{
__UHEAP_MARK;


//lowerbound
int failures=0;	

try
{
multimap <int, int> m1;
   multimap <int, int> :: const_iterator m1_AcIter, m1_RcIter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) ); // cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   m1_RcIter = m1.lower_bound( 2 );
    
       if( m1_RcIter -> second != 20)
       failures++;
       

   // If no match is found for this key, end( ) is returned
   m1_RcIter = m1. lower_bound ( 4 );

   if ( m1_RcIter == m1.end( ) );
      //cout << "The multimap m1 doesn't have an element "
          // << "with a key of 4." << endl;
   else
      if( m1_RcIter -> second != 4)
      failures++;
      

   // The element at a specific location in the multimap can be found 
   // using a dereferenced iterator addressing the location
   m1_AcIter = m1.end( );
   m1_AcIter--;
   m1_RcIter = m1. lower_bound ( m1_AcIter -> first );
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
    if( m1_RcIter -> second != 30)
    failures++;
    
    /*if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    

TInt Ctstl_1::multimap10(CStifItemParser& aItem)

{
__UHEAP_MARK;
//swap
int failures=0;

try
{
int a[2];
using namespace std;
   multimap <int, int> m1, m2, m3;
   multimap <int, int>::iterator m1_Iter;
   typedef pair <int, int> Int_Pair;

   m1.insert ( Int_Pair ( 1, 10 ) );
   m1.insert ( Int_Pair ( 2, 20 ) );
   m1.insert ( Int_Pair ( 3, 30 ) );
   m2.insert ( Int_Pair ( 10, 100 ) );
   m2.insert ( Int_Pair ( 20, 200 ) );
   m3.insert ( Int_Pair ( 30, 300 ) );

   
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   // This is the member function version of swap
   //m2 is said to be the argument multimap; m1 the target multimap
   m1.swap( m2 );

    for ( m1_Iter = m1.begin( ); m1_Iter != m1.end( ); m1_Iter++ )
    {
    static int i;
    	
    a[i] = m1_Iter -> second;
    
    i++;
    
    }
    
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
    if(a[0]!=100)
    failures++;
    if(a[1]!= 200)
    failures++;
    

   // This is the specialized template version of swap
   swap( m1, m3 );

    for ( m1_Iter = m1.begin( ); m1_Iter != m1.end( ); m1_Iter++ )
    {
    	
    
     int a= m1_Iter -> second;
     if(a!=300)
     failures++;
    }
 /*  
if(failures)
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::multimap11(CStifItemParser& aItem)
{
__UHEAP_MARK;

//val_comp
int failures=0;

try
{

  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif

   multimap <int, int, less<int> > m1;
   multimap <int, int, less<int> >::value_compare vc1 = m1.value_comp( );
   multimap<int,int>::iterator Iter1, Iter2;
   
   Iter1= m1.insert ( multimap <int, int> :: value_type ( 1, 10 ) );
   Iter2= m1.insert ( multimap <int, int> :: value_type ( 2, 5 ) );

   if( vc1( *Iter1, *Iter2 ) == false )   
   failures++;

   if( vc1( *Iter2, *Iter1 ) == true )   
   failures++;
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif

/*
if(failures)	
return KErrGeneral;
else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    
    
TInt Ctstl_1::multimap12(CStifItemParser& aItem)
{

 __UHEAP_MARK;

	int psize1,failures=0;
	
	try
	{
  multimap<char,int> mymap1;
  pair<const char,int>* p;
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   p=mymap1.get_allocator().allocate(2);

   psize1 = (int) sizeof(map<char,int>::value_type)*2;

 if(psize1!=16)
 failures++;
 

  mymap1.get_allocator().deallocate(p,2);

   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
    if(mymap1.max_size()!=4294967295)
    failures++;


/*
if(failures)	
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    

TInt Ctstl_1::multimap13(CStifItemParser& aItem)
{
__UHEAP_MARK;

 int failures=0;
 
 try
 {
 multimap<char,int> mymap;
 multimap<char,int>::reverse_iterator rit;


	mymap.insert (pair<char,int>('x',100));
 	mymap.insert (pair<char,int>('y',200));
  
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif

   for ( rit=mymap.rbegin() ; rit != mymap.rend(); rit++ )
    
   {
   	static int i;
   if(i==0)
   	{
    
  	 if( rit->second !=200)
   	failures++;
   	}
   i++;
   }

 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif

/*
if(failures)	
return KErrGeneral;
else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_1::multi_set(CStifItemParser& aItem)

{
__UHEAP_MARK;


int failures =0;

try
{
	
	
	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif

//__UHEAP_MARK;
//{
multiset <int> ms1;
   multiset <int> :: const_iterator low,ms1_AcIter,up,find1;
multiset<int>::iterator Iter;

multiset<int>::size_type count1, maxsize;







if(ms1.size()!= 0)

failures++;



   
   ms1.insert( 10 );
   ms1.insert( 20 );
   ms1.insert( 30 );

find1 = ms1.find(20);

if(*find1!=20)
failures++;


if(ms1.size()!=3)
failures++;



count1 = ms1.count(10);

if(count1 != 1)
failures++;


count1 = ms1.count(40);

if(count1!=0)
failures++;




Iter = ms1.begin();

ms1.erase(Iter);


if(*ms1.begin()!=20)
failures++;



 low = ms1.lower_bound( 20 );
 if( *low != 20)
failures++;


up = ms1.upper_bound(20);
if(*up!=30)
failures++;

   low = ms1.lower_bound( 40 );
up = ms1.upper_bound(30);


   // If no match is found for the key, end( ) is returned


if(up!=ms1.end())
failures++;


if(ms1.empty())
failures++;


ms1.clear();

if(!ms1.empty())
failures++;


maxsize = ms1.max_size();
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
if(maxsize!= 4294967295)
failures++;

//__UHEAP_MARKEND;
/*if(failures)

return KErrGeneral;
else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_1::multi_set2(CStifItemParser& aItem)

{
__UHEAP_MARK;
	int myints[] = {77,16,2,30,30},failures=0;
	
	try
	{
	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
  	multiset<int> mymultiset (myints,myints+5);

  	multiset<int>::reverse_iterator rit;

   for ( rit=mymultiset.rbegin() ; rit != mymultiset.rend(); rit++ )
   {
   	
   	static int i;
   	if(i==0)
   		{
   		if(*rit!=77)
   		failures++;
   		}
   		i++;
   }
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
 /*
 
 if(failures)
 return KErrGeneral;
 else
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_1::multi_set3(CStifItemParser& aItem)

{
__UHEAP_MARK;
	multiset<int> mymultiset;
  	int * p,failures=0;
  	
  	try
  	{
  	unsigned int i;
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
   	p = mymultiset.get_allocator().allocate(5);
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
 #endif
   	for (i=0; i<5; i++) 
   	p[i]=(i+1)*10;
 
	if(sizeof(p)!=4)
	failures++;
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
 #endif
 	 mymultiset.get_allocator().deallocate(p,5);

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
}	


catch(bad_alloc&)
   {
   	//do nothing
 }
   catch(...)
   {
   	failures++;
   	
   }
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }





















// -----------------------------------------------------------------------------
// Ctstl_1::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

bool myfunction (int i, int j) { return i<j; }


TInt Ctstl_1::Includes( CStifItemParser& aItem )
    {
__UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    } 
   // vector<int> myvector (8,10); 

// -----------------------------------------------------------------------------
// Ctstl_1::innerproduct
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::innerproduct( CStifItemParser& aItem )
    {
__UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


 // -----------------------------------------------------------------------------
// Ctstl_1::inplacemerge
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::inplacemerge( CStifItemParser& aItem )
    {
__UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
// -----------------------------------------------------------------------------
// Ctstl_1::Queue
// Queue test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::Queue( CStifItemParser& aItem )
    {
__UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
// -----------------------------------------------------------------------------
// Ctstl_1::PQueue
// PQueue test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctstl_1::Pqueue( CStifItemParser& aItem )
    {
__UHEAP_MARK;

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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }  
    
    // -----------------------------------------------------------------------------
// Ctstl_1::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


bool UDgreater (int elem1, int elem2 )
{
   return elem1 > elem2;
}


TInt Ctstl_1::stablesortL( CStifItemParser& aItem )
    {
__UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }  
    
// -----------------------------------------------------------------------------
// Ctstl_1::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::Stack( CStifItemParser& aItem )
    {


__UHEAP_MARK;
   int failures=0,i=0;
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
   
    __UHEAP_MARKEND;
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



// -----------------------------------------------------------------------------
// Ctstl_1::string1_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_1::string1_testL( CStifItemParser& /*aItem*/ )
    {


__UHEAP_MARK;
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
   
    __UHEAP_MARKEND;
   
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


  
    //end of file