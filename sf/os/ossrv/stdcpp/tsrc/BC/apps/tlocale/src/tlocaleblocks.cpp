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
#include<iostream>
#include<string>
#include <sstream>
#include "tlocale.h"
#define STDCPP_OOM FALSE
#define	testChar	unsigned char

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
				 
					 ENTRY( "ctype_byname1", Ctlocale::ctype_byname1L),
					 ENTRY( "moneypunct_byname1", Ctlocale::moneypunct_byname1L),
					 ENTRY( "moneypunct1", Ctlocale::moneypunct1L),
					 ENTRY( "numpunct1", Ctlocale::numpunct1L),
					 ENTRY( "numpunct_byname1", Ctlocale::numpunct_byname1L),
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
__UHEAP_MARK;
    try
    {
    	
    
 bool result = true;
 // __UHEAP_MARK;
 
   cout<<"";  
 
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 
 
  locale loc("en_GB.UTF-8") ;
 result = has_facet <ctype<char> > ( loc );
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
// Ctlocale::use_facet
// use_facet test method function.
//  
// -----------------------------------------------------------------------------
//


TInt Ctlocale::usefacet( CStifItemParser& aItem )
    {
    int failures=0;
__UHEAP_MARK;
try
{
	

 locale   loc ("en_GB.UTF-8");
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 bool result = use_facet<ctype<char> > ( loc ).is(ctype_base::alpha, 'b');
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
// Ctlocale::num_get
// num_get test method function.
//  
// -----------------------------------------------------------------------------
//


TInt Ctlocale::numget( CStifItemParser& aItem )
    {
    int failures=0;
 __UHEAP_MARK;   
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
 __UHEAP_MARKEND;  
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
__UHEAP_MARK;
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
__UHEAP_MARKEND;   
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
__UHEAP_MARK;
 int failures=0;
 try
 {
 	
 
 locale loc;
cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const numpunct <char> &npunct = use_facet <numpunct <char> >( loc);
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
// Ctlocale::numpunct_byname
// numpunct_byname test method function.
//  
// -----------------------------------------------------------------------------
//
TInt Ctlocale::numpunctbyname( CStifItemParser& aItem )
    {
    int failures=0;
__UHEAP_MARK;    
    try
    {
    	
    
 locale loc;
cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const numpunct_byname <char> &npunct = use_facet <numpunct_byname <char> >( loc);
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
// Ctlocale::money_get
// money_get test method function.
//  
// -----------------------------------------------------------------------------
//

TInt Ctlocale::moneyget( CStifItemParser& aItem )
    {
    int failures=0;
 __UHEAP_MARK;   
    try
    {
    	
    
 typedef istreambuf_iterator<char,char_traits<char> >
 iter_type;

 locale loc;
 string buffer("$100.02");
 string dest;
 long double ldest;
 ios_base::iostate state;
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
 __UHEAP_MARKEND;  
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
 long double ldval = 10002;

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
 __UHEAP_MARKEND;  
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
  const moneypunct<char,false>& mp =
 use_facet<moneypunct<char,false> >(loc);
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif    
 mp.curr_symbol();
 mp.negative_sign();

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
  static struct tm timeb;  
  std::ios_base::iostate state;
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
    	
   
    std::tm t = std::tm ();
   
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
    };
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
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const collate<char>& co =
 use_facet<collate<char> >(loc);
    
 if( co.compare(s1.begin(),s1.end(),s2.begin(),s2.end()-1) != 0)
 failures++;
                         
 if( co.compare(s1.begin(),s1.end(), s2.begin(),s2.end()) != -1)
 failures++;
                         
 // Retrieve hash values for two strings
 if( co.hash(s1.begin(),s1.end())!= 15636)
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
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 const collate_byname<char>& co = use_facet<collate_byname<char> >(loc);
     
 if( co.compare(s1.begin(),s1.end(),s2.begin(),s2.end()-1) != 0)
 failures++;
                         
 if( co.compare(s1.begin(),s1.end(),  s2.begin(),s2.end()) != -1)
 failures++;
                         
 // Retrieve hash values for two strings
 if( co.hash(s1.begin(),s1.end())!= 15636)
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
 __UHEAP_MARK;   

	locale loc ( "de_DE.ISO-8859-1" );
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
  __UHEAP_MARKEND;  
  
		  if(failures  )
		  return KErrGeneral;
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
 __UHEAP_MARK;   

 locale loc ( "de_DE.ISO-8859-1" );
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
 __UHEAP_MARKEND;  
   
		  if(failures  )
		  return KErrGeneral;
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
    
    
    
// helper API
template<class _CharT>
void Convert_string2_string_chart(basic_string<_CharT> &dst, string src)
	{
		int length = src.length();
		const char* str = src.c_str();
		for(int i = 0; i<length;i++)
		{
			dst.append(1, (_CharT)*str++);
			
		}
			
	}
    
 #if 1   

// -----------------------------------------------------------------------------
// Ctlocale::ctype_byname_test1L
// -----------------------------------------------------------------------------
//
struct MyCtype_bybname : public ctype_byname<testChar> {
	MyCtype_bybname(const char *name, size_t refs)
		: ctype_byname<testChar>(name, refs) {}
	testChar my_do_toupper(testChar ch) const
		{return (this->do_toupper(ch)); }
	const testChar *my_do_toupper(testChar *first,
		const testChar *last) const
		{return (this->do_toupper(first, last)); }
	testChar my_do_tolower(testChar ch) const
		{return (this->do_tolower(ch)); }
	const testChar *my_do_tolower(testChar *first,
		const testChar *last) const
		{return (this->do_tolower(first, last)); }
	};
// -----------------------------------------------------------------------------
// Ctlocale::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
	
TInt Ctlocale::ctype_byname1L( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktlocale, "tlocale" );
    _LIT( Ktest1, "In Test1" );
    TestModuleIf().Printf( 0, Ktlocale, Ktest1 );
    // Print to log file
    iLog->Log( Ktest1 );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    
    MyCtype_bybname fac("C", 1);
    
    if (fac.my_do_toupper('a') != 'A')
    	return KErrGeneral;
    
    testChar a[] = {"ABc0D"};
    fac.my_do_tolower(a, a + 2);
    if (strcmp((const char *)a, "abc0D") != 0)
    	return KErrGeneral;
    
    return KErrNone;

    }



// -----------------------------------------------------------------------------
// Ctlocale::moneypunct_byname_test3L
// -----------------------------------------------------------------------------
//
struct Mympunct_byname : public moneypunct_byname<testChar, true> {
	Mympunct_byname(const char *name, size_t refs)
		: moneypunct_byname<testChar, true>(name, refs) {}
	testChar my_do_decimal_point() const
		{return (do_decimal_point()); }
	};
	
TInt Ctlocale::moneypunct_byname1L( CStifItemParser& aItem )
    {

    // Print to UI
    
    _LIT( Ktlocale, "tlocale" );
    _LIT( Ktest1, "In Test3" );
    TestModuleIf().Printf( 0, Ktlocale, Ktest1 );
    // Print to log file
    iLog->Log( Ktest1 );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    
    Mympunct_byname fac("C", 1);
	testChar ch = fac.my_do_decimal_point();
	if (ch != 0)
		return KErrGeneral;
    return KErrNone;

    
    }
// -----------------------------------------------------------------------------
// Ctlocale::moneypunct_test2L
// -----------------------------------------------------------------------------
//
struct Mymoneypunct : public moneypunct<testChar, true> {
	string_type my_do_positive_sign() const
		{return (do_positive_sign()); }
	};

TInt Ctlocale::moneypunct1L( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktlocale, "tlocale" );
    _LIT( Ktest1, "In Test2" );
    TestModuleIf().Printf( 0, Ktlocale, Ktest1 );
    // Print to log file
    iLog->Log( Ktest1 );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    
    Mymoneypunct fac;
	basic_string<testChar> str = fac.my_do_positive_sign();
	if(str.size() != 0)
		return KErrGeneral;
    
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// Ctlocale::numpunct_test4L
// -----------------------------------------------------------------------------
//

struct Mynumpunct : public numpunct<testChar> {
protected:
	basic_string<testChar> do_truename() const
		{	basic_string<testChar> str1;
			Convert_string2_string_chart(str1,"!");
			return str1; }
	basic_string<testChar> do_falsename() const
		{
			basic_string<testChar> str1;
			Convert_string2_string_chart(str1,"!!");
			return str1; }
	};

TInt Ctlocale::numpunct1L( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktlocale, "tlocale" );
    _LIT( Ktest1, "In Test4" );
    TestModuleIf().Printf( 0, Ktlocale, Ktest1 );
    // Print to log file
    iLog->Log( Ktest1 );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    
    Mynumpunct fac;
    
    basic_string<testChar> str = fac.truename();
	if (strcmp((char*)str.c_str(), "!") != 0)
		return KErrGeneral;
	
	str = fac.falsename();
	if (strcmp((char*)str.c_str(), "!!") != 0)
		return KErrGeneral;
		
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Ctlocale::numpunct_byname_test5L
// -----------------------------------------------------------------------------
//

struct Mynumpunct_byname : public numpunct_byname<testChar> {
	Mynumpunct_byname(const char *name, size_t refs)
		: numpunct_byname<testChar>(name, refs) {}
	string_type my_do_truename() const
		{return (do_truename()); }
	};
	
TInt Ctlocale::numpunct_byname1L( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktlocale, "tlocale" );
    _LIT( Ktest1, "In Test5" );
    TestModuleIf().Printf( 0, Ktlocale, Ktest1 );
    // Print to log file
    iLog->Log( Ktest1 );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    
    const Mynumpunct_byname fac("C", 1);    
    basic_string<testChar> str = fac.my_do_truename();
    if(strcmp((char*)str.c_str(),"true") !=0)
    	return KErrGeneral;
    
    return KErrNone;

    }
#endif

#if 0    

// -----------------------------------------------------------------------------
// Ctlocale::num_get_test6L
// -----------------------------------------------------------------------------
//

typedef char_traits<testChar> mycharite;
typedef istreambuf_iterator<testChar, mycharite> myistrite;

struct Mynum_get : public num_get<testChar, myistrite> {
	iter_type my_do_get(iter_type first, iter_type last, ios_base& str,
		ios_base::iostate& st, bool& val) const
		{return (do_get(first, last, str, st, val)); }
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, long& val) const
		{return (do_get(first, last, str, st, val)); }
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, unsigned long& val) const
		{return (do_get(first, last, str, st, val)); }
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, double& val) const
		{return (do_get(first, last, str, st, val)); }
#ifndef __SYMBIAN32__
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, long double& val) const
		{return (do_get(first, last, str, st, val)); }
#endif
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, unsigned short& val) const	// [NOV95]
		{return (do_get(first, last, str, st, val)); }
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, unsigned int& val) const
		{return (do_get(first, last, str, st, val)); }
	iter_type my_do_get(iter_type first, iter_type last,  ios_base& str,
		ios_base::iostate& st, float& val) const
		{return (do_get(first, last, str, st, val)); }
	};
		
TInt Ctlocale::num_get1L( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktlocale, "tlocale" );
    _LIT( Ktest1, "In Test6" );
    TestModuleIf().Printf( 0, Ktlocale, Ktest1 );
    // Print to log file
    iLog->Log( Ktest1 );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    
    Mynum_get fac;
    basic_string<testChar> str1;
    Convert_string2_string_chart(str1,"1V-2W3X4.00Y6A7B-8.C");
  	locale loc;
    basic_istringstream<testChar> istr(str1);
    myistrite first(istr.rdbuf()), last;
    bool bv = false;
    ios_base::iostate st = ios_base::goodbit;
    first = fac.my_do_get(first, last, istr, st, bv);
	if (!bv);
		return KErrGeneral;
	if ((char)*first!= 'V');
		return KErrGeneral;
    	
    
    return KErrNone;

    }
    
#endif    
    
//  End of File

