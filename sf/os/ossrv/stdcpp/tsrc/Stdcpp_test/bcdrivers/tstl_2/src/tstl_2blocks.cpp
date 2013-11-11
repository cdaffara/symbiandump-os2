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
#include<list>
#include<string>
#include<cstring>
#include<vector>
#include<iterator>
#include<bitset>
#include<ios>
#include<algorithm>
#include <numeric>
#include<functional>
#include <locale>
#include<iostream>
#include<memory>
#include<complex>
#include<map>
#include<set>
 #include<exception>
  

#include <stl/char_traits.h> // fpos
#include "tstl_2.h"
#undef test
 using namespace std;
 #define STDCPP_OOM FALSE// TRUE for OOM testing

#ifdef __ARMCC__
#pragma diag_suppress 550
#pragma diag_suppress 177
#pragma diag_suppress 1134
#pragma diag_suppress 1250
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctstl_2::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctstl_2::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctstl_2::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctstl_2::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
              // Second is the actual implementation member function. 
 ENTRY("Advance", Ctstl_2::Advance ),
 ENTRY("Backinsert_iterator", Ctstl_2::Backinsert_iterator ),
 ENTRY("Frontinsert_iterator", Ctstl_2::Frontinsert_iterator ),
 ENTRY("Insert_iterator", Ctstl_2::Insert_iterator ),
 ENTRY("Reverse_iterator", Ctstl_2::Reverse_iterator ),
 ENTRY("Distance", Ctstl_2::Distance ),
//ENTRY("Distancetype", Ctstl_2::Distancetype ),
 ENTRY("Bit_set", Ctstl_2::Bit_set ),
 ENTRY("Isdigit", Ctstl_2::Isdigit ),
 ENTRY("Isgraph", Ctstl_2::Isgraph ),
 ENTRY("Isalnum", Ctstl_2::Isalnum ),
 ENTRY("Isalpha", Ctstl_2::Isalpha ),
 ENTRY("Iscntrl", Ctstl_2::Iscntrl ),
 ENTRY("Isprint", Ctstl_2::Isprint ),
 ENTRY("Ispunct", Ctstl_2::Ispunct ),
 ENTRY("Isspace", Ctstl_2::Isspace ),
 ENTRY("Isxdigit", Ctstl_2::Isxdigit ),
 ENTRY("Fill", Ctstl_2::Fill ),
 ENTRY("Generate", Ctstl_2::Generate ),
 ENTRY("Search", Ctstl_2::Search ),
 ENTRY("Search_n", Ctstl_2::Search_n ),
 ENTRY("Times", Ctstl_2::Times ),
 ENTRY("Unique", Ctstl_2::Unique ),
 ENTRY("Uniquecopy", Ctstl_2::Uniquecopy ),
 ENTRY("Partialsort", Ctstl_2::Partialsort ),
 ENTRY("Partialsortcopy", Ctstl_2::Partialsortcopy ),
 ENTRY("Partition", Ctstl_2::Partition ),
 ENTRY("Stablepartition", Ctstl_2::Stablepartition ),
 ENTRY("Setdifference", Ctstl_2::Setdifference ),
 ENTRY("Setintersection", Ctstl_2::Setintersection ),
 ENTRY("Setsymdifference", Ctstl_2::Setsymdifference ),
 ENTRY("Setunion", Ctstl_2::Setunion ),
 ENTRY("HeapL", Ctstl_2::HeapL ),
 ENTRY("Swapranges", Ctstl_2::Swapranges ),
 ENTRY("Partialsums", Ctstl_2::Partialsums ),
 ENTRY("Unarynegate", Ctstl_2::Unarynegate ),
 ENTRY("Not2", Ctstl_2::Not2),
 ENTRY("GettemporarybufferL", Ctstl_2::GettemporarybufferL),
 ENTRY("Pair", Ctstl_2::Pair),
 ENTRY("Lexicographicalcompare", Ctstl_2::Lexicographicalcompare ),
 ENTRY("Mismatch", Ctstl_2::Mismatch ),
 ENTRY("Compare_fun", Ctstl_2::Compare_fun ),
 ENTRY("Copy_fun", Ctstl_2::Copy_fun ),
 ENTRY("Copybackward_fun", Ctstl_2::Copybackward_fun ),
 ENTRY("mergeL", Ctstl_2::mergeL ),
 ENTRY("allocatoradd", Ctstl_2::allocatoradd ),
 ENTRY("alldeall", Ctstl_2::alldeall ),
 ENTRY("allcons", Ctstl_2::allcons ),
 ENTRY("allrebind", Ctstl_2::allrebind ),
 ENTRY("allmaxsize", Ctstl_2::allmaxsize ),
 ENTRY("complexL",Ctstl_2::complexL),
 ENTRY("limitsL",Ctstl_2::limitsL),
 ENTRY("nextperm",Ctstl_2::nextperm),
 ENTRY("prevperm",Ctstl_2::prevperm),
 ENTRY("ptrfun",Ctstl_2::ptrfun),
 ENTRY("itertraits",Ctstl_2::itertraits), 
 ENTRY("map1",Ctstl_2:: map1), 
 ENTRY("map2",Ctstl_2::map2), 
 ENTRY("map3",Ctstl_2::map3), 
 ENTRY("map4",Ctstl_2::map4), 
 ENTRY("map5",Ctstl_2::map5), 
 ENTRY("map6",Ctstl_2::map6), 
 ENTRY("map7",Ctstl_2::map7), 
 ENTRY("map8",Ctstl_2::map8), 
 ENTRY("map9",Ctstl_2::map9), 
 ENTRY("map10",Ctstl_2::map10), 
 ENTRY("map11",Ctstl_2::map11), 
 ENTRY("map12",Ctstl_2::map12),
 ENTRY("map13",Ctstl_2::map13),
 ENTRY("multimap1",Ctstl_2:: multimap1), 
 ENTRY("multimap2",Ctstl_2::multimap2), 
 ENTRY("multimap3",Ctstl_2::multimap3), 
 ENTRY("multimap4",Ctstl_2::multimap4), 
 ENTRY("multimap5",Ctstl_2::multimap5), 
 ENTRY("multimap6",Ctstl_2::multimap6), 
 ENTRY("multimap7",Ctstl_2::multimap7), 
 ENTRY("multimap8",Ctstl_2::multimap8), 
 ENTRY("multimap9",Ctstl_2::multimap9), 
 ENTRY("multimap10",Ctstl_2::multimap10), 
 ENTRY("multimap11",Ctstl_2::multimap11),
 ENTRY("multimap12",Ctstl_2::multimap12), 
 ENTRY("multimap13",Ctstl_2::multimap13),  
 ENTRY("multi_set",Ctstl_2::multi_set), 
 ENTRY("multi_set2",Ctstl_2::multi_set2), 
 ENTRY("multi_set3",Ctstl_2::multi_set3), 
               
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }
// -----------------------------------------------------------------------------
// Ctstl_2::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctstl_2::Advance( CStifItemParser& aItem )
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }





TInt Ctstl_2::Backinsert_iterator( CStifItemParser& aItem )
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_2::Frontinsert_iterator( CStifItemParser& aItem )
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




TInt Ctstl_2::Insert_iterator( CStifItemParser& aItem )
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

TInt  Ctstl_2::Reverse_iterator( CStifItemParser& aItem )

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
    

//   vector <int>::reverse_iterator rvIter;
    
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt  Ctstl_2::Distance( CStifItemParser& aItem )

{
int failures =0;
try{
int i;
 

   list<int> L;
   for ( i = -1 ; i < 9 ; ++i ) 
   {
      L.push_back ( 2 * i );
   }
   
   
   list <int>::iterator LPOS = L.begin ( );

  
 

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 /*
 TInt  Ctstl_2::Distancetype( CStifItemParser& aItem )

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
 
 
 
 TInt  Ctstl_2::Bit_set( CStifItemParser& aItem )

{

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
   s1 =  str.template to_string<char, char_traits<char>, allocator<char> >( );
    
 
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

 
 TInt  Ctstl_2::Isdigit( CStifItemParser& aItem )

{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


 TInt  Ctstl_2::Isgraph( CStifItemParser& aItem )

{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

 TInt  Ctstl_2::Isalnum( CStifItemParser& aItem )

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
TInt  Ctstl_2::Isalpha( CStifItemParser& aItem )

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



TInt  Ctstl_2::Iscntrl( CStifItemParser& aItem )

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


TInt  Ctstl_2::Isprint( CStifItemParser& aItem )

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


TInt  Ctstl_2::Ispunct( CStifItemParser& aItem )

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
  TInt  Ctstl_2::Isspace( CStifItemParser& aItem )

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

 TInt  Ctstl_2::Isxdigit( CStifItemParser& aItem )

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


TInt  Ctstl_2::Fill( CStifItemParser& aItem )

{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }





static int UniqueNumber()
 {
 
 static int current;
 return current++;	
 }


TInt  Ctstl_2::Generate( CStifItemParser& aItem )

{
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



TInt  Ctstl_2::Search( CStifItemParser& aItem )

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }




TInt  Ctstl_2::Search_n(CStifItemParser& aItem )

{
	
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
//code to be verified
//--------------------------

TInt Ctstl_2::Times(CStifItemParser& aItem)
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
 
 TInt Ctstl_2::Unique(CStifItemParser& aItem)
 {
 	
 int failures=0;
//__UHEAP_MARK;
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 TInt Ctstl_2::Uniquecopy(CStifItemParser& aItem)
 {
 	int failures=0;
 	
 try{
//__UHEAP_MARK;
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
  
 TInt Ctstl_2::Partialsort(CStifItemParser& aItem)
 {
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 

TInt Ctstl_2::Partialsortcopy(CStifItemParser& aItem)
 {
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
 
 bool IsOdd(int i)
 {
 	return (i%2)==1;
 }
 
 
 TInt Ctstl_2::Partition(CStifItemParser& aItem)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
  TInt Ctstl_2::Stablepartition(CStifItemParser& aItem)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::Setdifference(CStifItemParser& aItem)
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
 
 
 
 TInt Ctstl_2::Setintersection(CStifItemParser& aItem)
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
 
 
 
 
 TInt Ctstl_2::Setsymdifference(CStifItemParser& aItem)
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
    
 TInt Ctstl_2::Setunion(CStifItemParser& aItem)
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
 
 
 
 TInt Ctstl_2::HeapL(CStifItemParser& aItem)
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
 
 
 TInt Ctstl_2::Swapranges(CStifItemParser& aItem)
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

 
 TInt Ctstl_2::Partialsums(CStifItemParser& aItem)
 {
 	int failures=0;
 //__UHEAP_MARK;
 try{
 	
 vector<int> V1( 10 ), V2( 10 );
   vector<int>::iterator VIter1,   VIterend; 
 
int sum =1;
 int i=2 ;
   list <int> L1;
//   list <int>::iterator LIter1, LIterend;

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


TInt Ctstl_2::Unarynegate(CStifItemParser& aItem)
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
 
 
 TInt Ctstl_2::Not2(CStifItemParser& aItem)
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



TInt Ctstl_2::GettemporarybufferL(CStifItemParser& aItem)
 {
 	int failures=0;
 //__UHEAP_MARK;
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
 
 
 
 
 TInt Ctstl_2::Pair(CStifItemParser& aItem)
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


 TInt Ctstl_2::Lexicographicalcompare(CStifItemParser& aItem)
 {
 	
 //__UHEAP_MARK;
 
 


 int failures=0;
 
 try
 {
 	
 vector <int> v1, v2;
   list <int> L1;
//   vector <int>::iterator Iter1, Iter2;
//   list <int>::iterator L1_Iter, L1_inIter;

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



TInt Ctstl_2::Mismatch(CStifItemParser& aItem)
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
 
 
 TInt Ctstl_2::Compare_fun(CStifItemParser& aItem)
 
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
 
 
 
 TInt Ctstl_2::Copy_fun(CStifItemParser& aItem)
 
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
 
  TInt Ctstl_2::Copybackward_fun(CStifItemParser& aItem)
 
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
 
 
  TInt Ctstl_2::mergeL(CStifItemParser& aItem)
  
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

TInt Ctstl_2::allocatoradd(CStifItemParser& aItem)
  
  {
  	int failures = 0;
  
  //  __UHEAP_MARK;
try{
     vector <int> v1;
//   vector <int>::iterator v1Iter;
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


  
  TInt Ctstl_2::alldeall(CStifItemParser& aItem)
  
  
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
  
  
  TInt Ctstl_2::allcons(CStifItemParser& aItem)
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
  
    TInt Ctstl_2::allrebind(CStifItemParser& aItem)
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
    
    
    TInt Ctstl_2::allmaxsize(CStifItemParser& aItem)

    {int failures=0;
    	
    	
    	try{
    vector <int> v1;
//   vector <int>::iterator v1Iter;
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
  
  
  
  
    TInt Ctstl_2::complexL(CStifItemParser& aItem)
    
    {int failures=0;
    	try
    	{
    		
    	

//    double pi = 3.14159265359;
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::limitsL(CStifItemParser& aItem)

{

//__UHEAP_MARK;
	
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    
TInt Ctstl_2::nextperm(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    

TInt Ctstl_2::prevperm(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::ptrfun(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::itertraits(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map1(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_2::map2(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map3(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map4(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map5(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::map6(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map7(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map8(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map9(CStifItemParser& aItem)

{


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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::map10(CStifItemParser& aItem)

{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

TInt Ctstl_2::map11(CStifItemParser& aItem)
{

 
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
TInt Ctstl_2::map12(CStifItemParser& aItem)
{

 
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
TInt Ctstl_2::map13(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::multimap1(CStifItemParser& aItem)
{


 

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::multimap2(CStifItemParser& aItem)
{
//clear,size

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    

TInt Ctstl_2::multimap3(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::multimap4(CStifItemParser& aItem)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::multimap5(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::multimap6(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::multimap7(CStifItemParser& aItem)
{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


TInt Ctstl_2::multimap8(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::multimap9(CStifItemParser& aItem)

{


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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    

TInt Ctstl_2::multimap10(CStifItemParser& aItem)

{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::multimap11(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    
    
    
    
TInt Ctstl_2::multimap12(CStifItemParser& aItem)
{

 
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
    

TInt Ctstl_2::multimap13(CStifItemParser& aItem)
{

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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

TInt Ctstl_2::multi_set(CStifItemParser& aItem)

{


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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_2::multi_set2(CStifItemParser& aItem)

{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }



TInt Ctstl_2::multi_set3(CStifItemParser& aItem)

{
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }











 









//  End of File
