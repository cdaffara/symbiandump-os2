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
#include<locale>
#include<stl/_locale.h>
#include<iostream>
#include<string>
#include "tlocale.h"
#define STDCPP_OOM FALSE
using namespace std;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctlocale::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctlocale::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctlocale::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctlocale::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
 ENTRY( "hasfacet", Ctlocale::hasfacet ),
 ENTRY( "usefacet", Ctlocale::usefacet ),
 ENTRY( "numget", Ctlocale::numget ),
 ENTRY( "numput", Ctlocale::numput ),
 ENTRY( "num_punct", Ctlocale::num_punct ),  
 ENTRY( "numpunctbyname", Ctlocale::numpunctbyname ),
 ENTRY( "moneyget", Ctlocale::moneyget ),
 ENTRY( "moneyput", Ctlocale::moneyput ),
 ENTRY( "money_punct", Ctlocale::money_punct ),  
 ENTRY( "moneypunctbyname", Ctlocale::moneypunctbyname ),
 ENTRY( "timeget", Ctlocale::timeget ),
 ENTRY( "timeput", Ctlocale::timeput ),
 ENTRY( "messagesL", Ctlocale::messagesL ),
 ENTRY( "messagesbyname", Ctlocale::messagesbyname ),
 ENTRY( "collateL", Ctlocale::collateL ),
 ENTRY( "collatebyname", Ctlocale::collatebyname ),
 ENTRY( "codecvt1", Ctlocale::codecvt1 ),
 ENTRY( "codecvt2", Ctlocale::codecvt2 ),
 ENTRY( "codecvtbyname1", Ctlocale::codecvtbyname1 ),
 ENTRY( "codecvtbyname2", Ctlocale::codecvtbyname2 ),
 ENTRY( "catalog", Ctlocale::catalog ),
 ENTRY( "c_type", Ctlocale::c_type ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// Ctlocale::has_facet
// has_facet test method function.
//  
// -----------------------------------------------------------------------------
//


TInt Ctlocale::hasfacet( CStifItemParser& aItem )
    {
    int failures=0;
    try
    {
    	
    
 bool result = true;
  bool result_w = true;
 // __UHEAP_MARK;
 
   cout<<"";  
 
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
// char* p = "c";
//  const char* _Loc_ctype_default(p) ;
 
  locale loc("en_GB.UTF-8") ;
 result = has_facet <ctype<char> > ( loc );
  result_w = has_facet <ctype<wchar_t> > ( loc );
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 

//__UHEAP_MARKEND;
/*
 if(result)
 return KErrNone;
 else
 return KErrGeneral;
 */
 if(!result)
 failures++;
 
  if(!result_w)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ctlocale::use_facet
// use_facet test method function.
//  
// -----------------------------------------------------------------------------
//


TInt Ctlocale::usefacet( CStifItemParser& aItem )
    {
    int failures=0;
//__UHEAP_MARK;
try
{
	

 locale   loc ("en_GB.UTF-8");
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 bool result = use_facet<ctype<char> > ( loc ).is(ctype_base::alpha, 'b');
  bool result_w = use_facet<ctype<wchar_t> > ( loc ).is(ctype_base::alpha, L'b');
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 
// __UHEAP_MARKEND;
/* if (result)
 return KErrNone;
 else
 return KErrGeneral;

*/

if(!result)
 failures++;

if(!result_w)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctlocale::num_get
// num_get test method function.
//  
// -----------------------------------------------------------------------------
//


TInt Ctlocale::numget( CStifItemParser& aItem )
    {
    int failures=0;
    
    try
    {
    	
    
 typedef istreambuf_iterator<char,char_traits<char> >
 iter_type;


 
 locale loc;
 iter_type end;
 cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  const num_get<char,iter_type>& tg = use_facet<num_get<char,iter_type> >(loc);
    
 	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 //return KErrNone;
	
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
// Ctlocale::num_put
// num_put test method function.
//  
// -----------------------------------------------------------------------------
//
TInt Ctlocale::numput( CStifItemParser& aItem )
    {
    int failures=0;
    try
    {
    	
    
 typedef ostreambuf_iterator<char,char_traits<char> >
 iter_type;

 locale loc;
      

 // Construct a ostreambuf_iterator on cout
 iter_type begin(cout);

 // Get a num_put facet reference
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const num_put<char,iter_type>& np = use_facet<num_put<char,iter_type> >(loc);
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif     
  
//return KErrNone;

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
// Ctlocale::num_punct
// num_punct test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::num_punct( CStifItemParser& aItem )
 {
 int failures=0;
 try
 {
 	
 
 locale loc;
cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const numpunct <char> &npunct = use_facet <numpunct <char> >( loc);
  const numpunct <wchar_t> &npunct_w = use_facet <numpunct <wchar_t> >( loc);
   // const numpunct <unsigned> &n1punct = use_facet <numpunct <unsigned> >( loc);
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
/*   
 if(npunct.truename() ==  "true")
 if(npunct.falsename()=="false")  
 if(npunct.decimal_point() =='.')
 if(npunct.thousands_sep() ==',')
   
    
  
 return KErrNone;
 else
 return KErrGeneral;*/
  if(npunct.truename() !=  "true")
  failures++;
 if(npunct.falsename()!="false")  
 failures++;
 if(npunct.decimal_point() !='.')
 failures++;
 if(npunct.thousands_sep() !=',')
 failures++;
 
 
  if(npunct_w.truename() !=  L"true")
  failures++;
 if(npunct_w.falsename()!=L"false")  
 failures++;
 if(npunct_w.decimal_point() != L'.')
 failures++;
 if(npunct_w.thousands_sep() !=L',')
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }
// -----------------------------------------------------------------------------
// Ctlocale::numpunct_byname
// numpunct_byname test method function.
//  
// -----------------------------------------------------------------------------
//
TInt Ctlocale::numpunctbyname( CStifItemParser& aItem )
    {
    int failures=0;
    
    try
    {
    	
    
 locale loc;
cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const numpunct_byname <char> &npunct = use_facet <numpunct_byname <char> >( loc);
  const numpunct_byname <wchar_t> &npunct_w = use_facet <numpunct_byname <wchar_t> >( loc);
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    
 /*if(npunct.truename() ==  "true")
 if(npunct.falsename()=="false") 
 if(npunct.decimal_point( ) =='.')
 if(npunct.thousands_sep( ) ==',')
   
 
  
return KErrNone;
else
return KErrGeneral;*/

if(npunct.truename() !=  "true")
failures++;
 if(npunct.falsename()!="false") 
 failures++;
 if(npunct.decimal_point( ) !='.')
 failures++;
 if(npunct.thousands_sep( ) !=',')
 failures++;
 
 
 
 if(npunct_w.truename() !=  L"true")
failures++;
 if(npunct_w.falsename()!= L"false") 
 failures++;
 if(npunct_w.decimal_point( ) != L'.')
 failures++;
 if(npunct_w.thousands_sep( ) != L',')
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctlocale::money_get
// money_get test method function.
//  
// -----------------------------------------------------------------------------
//

TInt Ctlocale::moneyget( CStifItemParser& aItem )
    {
    int failures=0;
    
    try
    {
    	
    
 typedef istreambuf_iterator<char,char_traits<char> >
 iter_type;

 locale loc;
 string buffer("$100.02");
 string dest;
// long double ldest;
// ios_base::iostate state;
 iter_type end;
 cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 // Get a money_get facet
 const money_get<char,iter_type>& mgf =  use_facet<money_get<char,iter_type> >(loc);
     
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
	
// return KErrNone;
	
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
// Ctlocale::money_put
// money_put test method function.
//  
// -----------------------------------------------------------------------------
//
TInt Ctlocale::moneyput( CStifItemParser& aItem )
    {
    int failures=0;
    
    try
    {
    	
    
 typedef ostreambuf_iterator<char,char_traits<char> >
 iter_type;

 locale loc;
 string buffer("10002");
// long double ldval = 10002;

  iter_type begin(cout);
  cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  const money_put<char,iter_type>& mp =   use_facet<money_put<char,iter_type> >(loc);

    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif  

  
//return KErrNone;

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
// Ctlocale::moneypunct
// moneypunct test method function.
//  
// -----------------------------------------------------------------------------
//
TInt Ctlocale::money_punct( CStifItemParser& aItem )
    {
     
     int failures=0;
     try
     {
     	
     
 
 locale loc;
 cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  
  
  
  const moneypunct<char,false>& mp = use_facet<moneypunct<char,false> >(loc);
    const moneypunct<char,true>& mp_true = use_facet<moneypunct<char,true> >(loc);

  
   const moneypunct<wchar_t,false>& mp_w = use_facet<moneypunct<wchar_t,false> >(loc);
   
      const moneypunct<wchar_t,true>& mp_w_true = use_facet<moneypunct<wchar_t,true> >(loc);
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif    
 mp.curr_symbol();
 mp.negative_sign();
mp.grouping();
mp.curr_symbol();
mp.frac_digits();
mp.pos_format() ;
mp.neg_format(); 


mp_true.curr_symbol();
 mp_true.negative_sign();
mp_true.grouping();
mp_true.curr_symbol();
mp_true.frac_digits();
mp_true.pos_format() ;
mp_true.neg_format(); 

 mp_w.curr_symbol();
 mp_w.negative_sign();
mp_w.grouping();
mp_w.curr_symbol();
mp_w.frac_digits();
mp_w.pos_format() ;
mp_w.neg_format(); 


mp_w_true.curr_symbol();
 mp_w_true.negative_sign();
mp_w_true.grouping();
mp_w_true.curr_symbol();
mp_w_true.frac_digits();
mp_w_true.pos_format() ;
mp_w_true.neg_format(); 

/* if(mp.decimal_point() == '.')
 if(mp.thousands_sep() == ',')
 if( mp.frac_digits() == 0)
 

    
  
return KErrNone;
else
return KErrGeneral;*/

if(mp.decimal_point() != '.')
failures++;
 if(mp.thousands_sep() != ',')
 failures++;
 if( mp.frac_digits() != 0)
 failures++;

 
 if(mp_true.decimal_point() != '.')
failures++;
 if(mp_true.thousands_sep() != ',')
 failures++;
 if( mp_true.frac_digits() != 0)
 failures++;
 
 
 
 
 if(mp_w.decimal_point() != L'.')
failures++;
 if(mp_w.thousands_sep() != L',')
 failures++;
 if( mp_w.frac_digits() != 0)
 failures++;
 
 
 if(mp_w_true.decimal_point() != L'.')
failures++;
 if(mp_w_true.thousands_sep() != L',')
 failures++;
 if( mp_w_true.frac_digits() != 0)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctlocale::moneypunct_byname
// moneypunct_byname test method function.
//  
// -----------------------------------------------------------------------------
// 
 
TInt Ctlocale::moneypunctbyname( CStifItemParser& aItem )
    {
    
    int failures=0;
    try
    {
    	
    
                 
 locale loc;
  
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  const moneypunct_byname<char,false>& mp = 
     
 use_facet<moneypunct_byname<char,false> >(loc);
 
   const moneypunct_byname<char,true>& mp_true = 
     
 use_facet<moneypunct_byname<char,true> >(loc);
 
 
   const moneypunct_byname<wchar_t,false>& mp_w = 
     
 use_facet<moneypunct_byname<wchar_t,false> >(loc);
 
 
 
    const moneypunct_byname<wchar_t,true>& mp_w_true = 
     
 use_facet<moneypunct_byname<wchar_t,true> >(loc);
   
   
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
   
 mp.curr_symbol();
 mp.negative_sign();
 if(mp.decimal_point() != '.')
 failures++;
 if(mp.thousands_sep() != ',')
 failures++;
 if( mp.frac_digits() != 0)
 failures++;
 
 
 mp_true.curr_symbol();
 mp_true.negative_sign();
 if(mp_true.decimal_point() != '.')
 failures++;
 if(mp_true.thousands_sep() != ',')
 failures++;
 if( mp_true.frac_digits() != 0)
 failures++;
 
  
 mp_w.curr_symbol();
 mp_w.negative_sign();
 if(mp_w.decimal_point() != L'.')
 failures++;
 if(mp_w.thousands_sep() != L',')
 failures++;
 if( mp_w.frac_digits() != 0)
 failures++;
 
 
 mp_w_true.curr_symbol();
 mp_w_true.negative_sign();
 if(mp_w_true.decimal_point() != L'.')
 failures++;
 if(mp_w_true.thousands_sep() != L',')
 failures++;
 if( mp_w_true.frac_digits() != 0)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctlocale::time_get
// time_get  test method function.
//  
// -----------------------------------------------------------------------------
// 


TInt Ctlocale::timeget( CStifItemParser& aItem )
    {
    int failures=0;
    
    try
    {
    	
    
  typedef std::istreambuf_iterator<char,
  std::char_traits<char> > Iter;
//  static struct tm timeb;  
//  std::ios_base::iostate state;
cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const std::time_get<char, Iter> &tg =
 std::use_facet<std::time_get<char, Iter> >(std::locale ("C"));
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 //return KErrNone;
	
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
// Ctlocale::time_put
// time_put  test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::timeput( CStifItemParser& aItem )
    { 
    
    int failures=0; 
    try
    {
    	
   
/*    std::tm t = std::tm ();
   
    t.tm_sec   = 56;    
    t.tm_min   = 34;    
    t.tm_hour  = 14;    
    t.tm_mday  = 29;   
    t.tm_mon   =  1;    
    t.tm_year  = 84;    
                       
    t.tm_yday  = 59;    
    t.tm_wday  =  3;   
    t.tm_isdst =  0;   

    const char* const fmt[] = {
      "%a", "%A", "%b", "%B", "%c", "%C", "%d", "%D",
      "%e", "%F", "%g", "%G", "%h", "%H", "%I", "%j",
      "%k", "%l", "%m", "%M", "%n", "%p", "%r", "%R",
      "%S", "%t", "%T", "%u", "%U", "%V", "%w", "%W", "%x",
      "%X", "%y", "%Y", "%z", "%Z", "%%", "%Ec", "%EC", "%Ex",
      "%EX", "%Ey", "%EY", "%Od", "%Oe", "%OH", "%OI", "%Om",
      "%OM", "%OS", "%Ou", "%OU", "%OV", "%Ow", "%OW", "%Oy"         
    };*/
    cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
     const std::time_put<char> &tput =
        std::use_facet<std::time_put<char> >(std::cout.getloc ());

#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  
//return KErrNone;

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
// Ctlocale::messages
// messages  test method function.
//  
// -----------------------------------------------------------------------------
// 
 
TInt Ctlocale::messagesL( CStifItemParser& aItem )
{
int failures=0;

try
{
	
 
      locale loc;
      cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
       const messages<char>& mess =use_facet<messages<char> >(loc);
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  // no support to .cat files

      /*  
      string def("Message Not Found");
      messages<char>::catalog cat =
              mess.open("./rwstdmessages.cat",loc);
      if (cat != -1)
       {
        string msg0 = mess.get(cat,1,1,def);
        string msg1 = mess.get(cat,1,2,def);
        string msg2 = mess.get(cat,1,6,def); // invalid msg #
        string msg3 = mess.get(cat,2,1,def);

        mess.close(cat);
        cout << msg0 << endl << msg1 << endl
              << msg2 << endl << msg3 << endl;
       }
      else
        cout << "Unable to open message catalog" << endl;*/

      //return KErrNone;
	
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
// Ctlocale::messages_byname
// messages_byname  test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::messagesbyname( CStifItemParser& aItem )
{
int failures=0;

try
{
	
 
 locale loc;
 cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const messages_byname<char>& mess =
      
        use_facet<messages_byname<char> >(loc);
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    
// no support to .cat files
    /*    
      string def("Message Not Found");
      messages_byname<char>::catalog cat =
              mess.open("./rwstdmessages.cat",loc);
      if (cat != -1)
       {
        string msg0 = mess.get(cat,1,1,def);
        string msg1 = mess.get(cat,1,2,def);
        string msg2 = mess.get(cat,1,6,def); // invalid msg #
        string msg3 = mess.get(cat,2,1,def);

        mess.close(cat);
        cout << msg0 << endl << msg1 << endl
              << msg2 << endl << msg3 << endl;
       }
      else
        cout << "Unable to open message catalog" << endl;*/

      //return KErrNone;	
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
// Ctlocale::collate
// collate  test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::collateL( CStifItemParser& aItem )
{
 int failures =0;
 try
 {
 	
 
 locale loc;
 string s1("blue");
 string s2("blues");
  wstring ws1(L"blue");
 wstring ws2(L"blues");
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const collate<char>& co =
 use_facet<collate<char> >(loc);
 
 
  const collate<wchar_t>& w_co =
 use_facet<collate<wchar_t> >(loc);  //wchar
    
 if( co.compare(s1.begin(),s1.end(),s2.begin(),s2.end()-1) != 0)
 failures++;
                         
 if( co.compare(s1.begin(),s1.end(), s2.begin(),s2.end()) != -1)
 failures++;
                         
 // Retrieve hash values for two strings
 if( co.hash(s1.begin(),s1.end())!= 15636)
 failures++;
 
 //wchar
    
 if( w_co.compare(ws1.begin(),ws1.end(),ws2.begin(),ws2.end()-1) != 0)
 failures++;
                         
 if( w_co.compare(ws1.begin(),ws1.end(), ws2.begin(),ws2.end()) != -1)
 failures++;
                         
 // Retrieve hash values for two strings
 if( w_co.hash(ws1.begin(),ws1.end())!= 15636)
 failures++;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif    
  
      
      
       
	
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
// Ctlocale::collatebyname
// collatebyname  test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::collatebyname( CStifItemParser& aItem )
{
 int failures =0;
 try
 {
 	
 
 locale loc;
 string s1("blue");
 string s2("blues");
 
  wstring ws1(L"blue");
 wstring ws2(L"blues");
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const collate_byname<char>& co = use_facet<collate_byname<char> >(loc);
 const collate_byname<wchar_t>& w_co = use_facet<collate_byname<wchar_t> >(loc);
     
 if( co.compare(s1.begin(),s1.end(),s2.begin(),s2.end()-1) != 0)
 failures++;
                         
 if( co.compare(s1.begin(),s1.end(),  s2.begin(),s2.end()) != -1)
 failures++;
                         
 // Retrieve hash values for two strings
 if( co.hash(s1.begin(),s1.end())!= 15636)
 failures++;
 
 //wchar
    
 if( w_co.compare(ws1.begin(),ws1.end(),ws2.begin(),ws2.end()-1) != 0)
 failures++;
                         
 if( w_co.compare(ws1.begin(),ws1.end(), ws2.begin(),ws2.end()) != -1)
 failures++;
                         
 // Retrieve hash values for two strings
 if( w_co.hash(ws1.begin(),ws1.end())!= 15636)
 failures++;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif    
  
      
      
       
	
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
// Ctlocale::codecvt1
// codecvt1  test method function.
//  
// -----------------------------------------------------------------------------
// 

TInt Ctlocale::codecvt1( CStifItemParser& aItem )
{
/*	locale loc ( "de_DE.ISO-8859-15" );
	int failures=0;
	try
	{
	cout<<"";	
	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
    int result = use_facet<codecvt<char, char, mbstate_t> > ( loc ).encoding ();
    if(result != 1)
    failures++;
   
     
  
  
    char* str = "This is the string whose length is to be measured!";
    mbstate_t state = {0};
    locale loc1("C");//English_Britain");//German_Germany
   //int res = use_facet<codecvt<wchar_t, char, mbstate_t>>( loc ).length( state,str, &str[strlen(str)], 90 );

  
    int res = use_facet<codecvt<wchar_t, char, mbstate_t> >
     ( loc1 ).length( state,str, &str[strlen(str)], 90 );

  
  
    if(res!=50)
    failures++;
  
  
 
     locale loc2( "C");//English_Britain" );//German_Germany
     int res2 = use_facet<codecvt<char, char, mbstate_t> >
     ( loc ).max_length( );
     if(res2!=1)
     failures++;

  
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    
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
		  return KErrGeneral;*/
		  return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctlocale::codecvt2
// codecvt2 test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::codecvt2( CStifItemParser& aItem )
{
int failures=0;
try
{
	
 
 char strout[91];
 wchar_t *strin = L"This is the wchar_t string to be converted.";
 memset( &strout[0], 0, ( sizeof( char ) )*( 91 ) );
 char* strnext;
 const wchar_t* pwszNext;
 mbstate_t state;
 locale loc("C"); 
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 int res = use_facet<codecvt<wchar_t, char, mbstate_t> >
      ( loc ).out( state,strin, &strin[wcslen( strin )], pwszNext ,
 strout, &strout[wcslen( strin )], strnext );
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 strout[wcslen( strin )] = 0;
    
  string str = "This is the wchar_t string to be converted.";
 
/*if(str.compare(&strout[0]) == 0)
return KErrNone;
else
return KErrGeneral;

*/

if(str.compare(&strout[0]) != 0)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ctlocale::codecvtbyname1
// codecvtbyname1 test method function.
//  
// -----------------------------------------------------------------------------
// 

TInt Ctlocale::codecvtbyname1( CStifItemParser& aItem )
{
/* locale loc ( "de_DE.ISO-8859-15" );
 int failures=0;
 try
 {
 cout<<"";	
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 int result = use_facet<codecvt_byname<char, char, mbstate_t> > ( loc ).encoding ();
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 if(result != 1)
 failures++;
 
  
 char* str = "This is the string whose length is to be measured!";
 mbstate_t state = {0};
 locale loc1("C"); 
 
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  int res = use_facet<codecvt_byname<wchar_t, char, mbstate_t> >
     ( loc1 ).length( state,str, &str[strlen(str)], 90 );

  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  
  if(res!=50)
  failures++;
  
   
  
 locale loc2( "C"); 
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 int res2 = use_facet<codecvt_byname<char, char, mbstate_t> >( loc ).max_length( );
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 if(res2!=1)
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
   
		  if(failures  )
		  return KErrGeneral;*/
		  return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ctlocale::codecvtbyname2
// codecvtbyname2 test method function.
//  
// -----------------------------------------------------------------------------
// 
TInt Ctlocale::codecvtbyname2( CStifItemParser& aItem )
{
int failures=0;
try
{
	

 char* strout = "This is the string to be converted!";
 wchar_t strin [91];
 memset(&strin[0], 0, (sizeof(wchar_t))*(91));
 const char* pszNext;
 wchar_t* pwszNext;
 mbstate_t state = {0};
 locale loc("C"); 
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 int res = use_facet<codecvt<wchar_t, char, mbstate_t> >
     ( loc ).in( state, strout, &strout[strlen(strout)], pszNext,
          strin, &strin[strlen(strout)], pwszNext );
      #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif     
 strin[strlen(strout)] = 0;
 
 wstring str = L"This is the string to be converted!";

/*
if(str.compare(&strin[0]) == 0)
return KErrNone;
else
return KErrGeneral;

   */
 if(str.compare(&strin[0]) != 0)
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
   
		  if(failures  )
		  return KErrGeneral;
		  return KErrNone;
    }

//cataloge

TInt Ctlocale::catalog(CStifItemParser& aItem )
    {
    int failures=0;
    try
    {
    	
    
  // __UHEAP_MARK;
 
   cout<<"";  
 
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
// char* p = "c";
/*  const char* _Loc_ctype_default(p);
   const char* _Loc_numeric_default(p) ;
	const char* _Loc_time_default(p) ;
   const char* _Loc_collate_default(p) ;
   const char* _Loc_monetary_default(p) ;
   const char* _Loc_messages_default(p) ;*/
   
/*    locale::category c;
   //_Locale* L = "k";
   const char* name1 = "catalog";
    const char* name2="test";*/
    
            
   const locale& L1= 0;
   const locale& L2= 0;
//   const char* name = "testing";
  // void _Stl_loc_combine_names(L,name1,name2,c);
   //locale::locale(const locale& L1, const locale& L2, category c)
   //locale::category c;
  
   
  
  //locale loc1(L1,name,c);
  //locale loc(L1,L2,c);
    

  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 

//__UHEAP_MARKEND;
/*
 if(result)
 return KErrNone;
 else
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
    
    
    
    //cataloge

TInt Ctlocale::c_type(CStifItemParser& aItem )
    {
    int failures=0;
    try
    {
    	
    
  // __UHEAP_MARK;
 
   cout<<"";  
 
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  
  
  
  locale loc1 ( "C" );
   
   char *string = "Hello, my name is John!";
//    wchar_t *wstring = L"Hello, my name is John!";

   const char* i = use_facet<ctype<char> > ( loc1 ).scan_is ( ctype_base::punct, &string[0], &string[strlen(&string[0])-1] );
    // const wchar_t* wi = use_facet<ctype<wchar_t> > ( loc1 ).scan_is ( ctype_base::punct, &wstring[0], &wstring[strlen(&wstring[0])-1] );
   const char* j = use_facet<ctype<char> > ( loc1 ).scan_not( ctype_base::alpha, &string[0], &string[strlen(&string[0])-1] );
   if((*i)!= ',')
   failures++;
   
    if((*j)!= ',')
   failures++;
    
    
    
    
    
    //lower
    
    char string2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    wchar_t wstring2[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char to_lower = 'a';
wchar_t wto_lower = L'a';

   use_facet<ctype<char> > ( loc1 ).tolower( &string2[0], &string2[strlen(&string2[0])] );
     use_facet<ctype<wchar_t> > ( loc1 ).tolower( &wstring2[0], &wstring2[27] );

   use_facet<ctype<char> > ( loc1 ).tolower(to_lower);
     use_facet<ctype<wchar_t> > ( loc1 ).tolower(wto_lower);
//upper

  
    char string3[] = "abcdefghijklmnopqrstuvwxyz";
    wchar_t wstring3[] = L"abcdefghijklmnopqrstuvwxyz";
char to_upper = 'A';

wchar_t wto_upper = L'A';

char str2[16];

//char test;
   use_facet<ctype<char> > ( loc1 ).toupper( &string3[0], &string3[strlen(&string3[0])] );
     use_facet<ctype<wchar_t> > ( loc1 ).toupper( &wstring3[0], &wstring3[27] );

   use_facet<ctype<char> > ( loc1 ).toupper(to_upper);
     use_facet<ctype<wchar_t> > ( loc1 ).toupper(wto_upper);



 bool result1 = (use_facet<ctype<wchar_t> > ( loc1 ).narrow
      (wstring3, wstring3 + wcslen(wstring3), 'X', &str2[0] ) != 0);
      
       bool result2 = (use_facet<ctype<char> > ( loc1 ).narrow
      (string3, string3 + strlen(string3), 'X', &str2[0] ) != 0);
      
      	const ctype_base::mask* _get_classic_table();   
      		const unsigned char*  _get_S_upper_table();
      			const unsigned char*  _get_S_lower_table();
      			void Dummy_API(void); 
     //  ctype::narrow(to_upper,test);
      
      //char ctype<char>::do_narrow(char __c, char /* dfault */ ) 
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 

//__UHEAP_MARKEND;
/*
 if(result)
 return KErrNone;
 else
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

