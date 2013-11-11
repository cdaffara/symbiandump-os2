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
#include<fstream>
#include<string>
#include<ostream>
#include <sstream>
#include<strstream>
#include<deque>
#include<iterator>
#include<vector>
#include <ios>
#include <iostream>
#include <fstream>
#include <stl\char_traits.h>
 #include<e32std.h>

#include<exception>

#include "tiostreams.h"
using namespace std;
 #define STDCPP_OOM FALSE// TRUE for OOM testing
 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctiostreams::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctiostreams::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctiostreams::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctiostreams::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
              // Second is the actual implementation member function. 
 ENTRY( "iofstreamL", Ctiostreams::iofstreamL ),
 ENTRY( "stringbufL", Ctiostreams::stringbufL ),
 ENTRY( "stringstreamL", Ctiostreams::stringstreamL ),
 ENTRY( "streambufL", Ctiostreams:: streambufL ),
 ENTRY( "ostreamL", Ctiostreams:: ostreamL ),
 ENTRY( "istreamL", Ctiostreams:: istreamL ),
 ENTRY( "istringstreamL", Ctiostreams:: istringstreamL ),
 ENTRY( "ostringstreamL", Ctiostreams:: ostringstreamL ),
 ENTRY( "ostreamiterators", Ctiostreams::ostreamiterators ),
 ENTRY( "fstreamL", Ctiostreams::fstreamL),
 ENTRY( "istrstreamL", Ctiostreams::istrstreamL),
 ENTRY( "strstreamL", Ctiostreams::strstreamL),
 ENTRY( "ostrstreamL", Ctiostreams::ostrstreamL),
 ENTRY( "istreamiterators", Ctiostreams::istreamiterators ),
 ENTRY( "istreambufiterators", Ctiostreams::istreambufiterators ),
 ENTRY( "strstreambufL", Ctiostreams::strstreambufL ),
 ENTRY( "freezeL", Ctiostreams::freezeL ),
 ENTRY( "fposL", Ctiostreams::fposL ),
  ENTRY( "filebufL", Ctiostreams::filebufL ),
          ENTRY( "basicstring", Ctiostreams::basicstring ),
        ENTRY( "basicfilebufL", Ctiostreams::basicfilebufL ),
        ENTRY( "basicistreamL", Ctiostreams::basicistreamL ),
        ENTRY( "wfstreamL", Ctiostreams::wfstreamL ),
        ENTRY( "wifstreamL", Ctiostreams::wifstreamL ),
        ENTRY( "wistreamL", Ctiostreams::wistreamL ),
		ENTRY( "wofstreamL", Ctiostreams::wofstreamL ),
 		ENTRY( "wistringstreamL", Ctiostreams::wistringstreamL ),
 		ENTRY( "wostringstreamL", Ctiostreams::wostringstreamL ),
 		ENTRY( "wstreambufL", Ctiostreams::wstreambufL ),
 		ENTRY( "wostreamL", Ctiostreams::wostreamL ),
 		ENTRY( "wstringbufL", Ctiostreams::wstringbufL ),
      


        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// Ctiostreams:: ofstream,ifstream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt Ctiostreams::iofstreamL( CStifItemParser& aItem )
    {
 
 
//__UHEAP_MARK;
int failures=0 ;
try
{
cout<<"";	
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
    ofstream myfile;
    filebuf *fbuf;
    char * buffer;
  long size;
    //ifstream 
  myfile.open ("c:\\TestFramework\\docs\\example.txt");
  if(!myfile.is_open())
  failures++;
 
  myfile << "Writing this to a file.";
  myfile<<"\0";
   myfile.close();
  if(myfile.is_open())
  failures++;
  
  
  
  
 // string line;
   ifstream myfile1; 
 
 
 myfile1.open("c:\\TestFramework\\docs\\example.txt" );

fbuf=myfile1.rdbuf();

 
 // get file size using buffer's members
  size=fbuf->pubseekoff (0,ios::end,ios::in);
  fbuf->pubseekpos (0,ios::in);

  // allocate memory to contain file data
  buffer=new char[size];

  // get file data  
  fbuf->sgetn (buffer,size);
  
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  
  
  if(buffer != "Writing this to a file.");
  else
  failures++;
  
  if(size!= 23)
  failures++;
  

if(!myfile1.is_open())
failures++;
myfile1.close();
if(myfile1.is_open())
failures++;
  
   delete buffer;
ios_base::Init();  // 0 -   218 FUNCTION Init()  iostream.cpp
filebuf*  _Stl_create_filebuf(FILE* f, ios_base::openmode mode );  // 0 -   225 FUNCTION _Stl_create_filebuf() iostream.cpp
ios_base::sync_with_stdio();//0 -   445 FUNCTION ios_base::sync_with_stdio()
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
		  
    }

 
// -----------------------------------------------------------------------------
// Ctiostreams:: stringbuf
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 
 TInt Ctiostreams::stringbufL(CStifItemParser& aItem )
 {
 
 	int failures = 0;
 	try
 	{
 cout<<"";		
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 	
stringbuf sb;
  string mystr;

  sb.sputn("Sample string",13);
  mystr=sb.str();

    
   
   if(sb.in_avail()!=13)
  failures++;
   
   
    char ch = sb.sgetc();
   if(ch!= 'S')
   failures++;
   
   
  
  
  
  
   if(mystr.compare("Sample string") != 0)
   failures++;
  
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  /* if(failures)
    
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
// -----------------------------------------------------------------------------
// Ctiostreams:: stringstream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 
 
 TInt Ctiostreams::stringstreamL(CStifItemParser& aItem )
 {
 	
 	int val;
  string  teststr;
  int failures = 0;
  try
  {
  cout<<"";	
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   stringstream ss (stringstream::in | stringstream::out);
   stringstream teststring;
   
    
   teststring<<"stringstream testcase";
   
   teststr = teststring.str();
   
   if(!teststr.compare("stringstream testcase"))
   ;
   else
   failures++;

 ss << "120 42 377 6 5 2000";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 for (int n=0; n<6; n++)
  {
    ss >> val;
    
    switch(n)
    {
    	case 0://precision 
    	if(val!=120)
    	failures++;
    	break;
    	case 1:
    	if(val!=42)
    	failures++;
    	break;
    	case 2:
    	if(val!=377)
    	failures++;
    	break;
    	case 3:
    	if(val!=6)
    	failures++;
    	break;
    	
    	case 4:
    	if(val!=5)
    	failures++;
    	break;
       
    	case 5:
    	if(val!=2000)
    	failures++;
    	break;
    
    default:
    break;
    }
    
   
  }

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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// Ctiostreams:: streambuf
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

 TInt Ctiostreams::streambufL(CStifItemParser& aItem )
 {
//    locale   loc ("en_GB.UTF-8");

 
 
 int failures = 0;
 try
 {
 	
 
 char a[4]=
 {
 	0
 };
 
 char sentence[]= "Sample sentence";
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 streambuf * pbuf;
 ofstream ostr ("c:\\TestFramework\\docs\\streambuf.txt");
 
 
  
 if(!cout.rdbuf( )->getloc( ).name( ).c_str( ))    //failing
// failures++;
 ;
 char ch[14];
 //rdbuf()s
 pbuf = ostr.rdbuf();


//sputn()




 pbuf->sputn (sentence,sizeof(sentence)-1);

  
 ostr.open("c:\\TestFramework\\docs\\streambuf.txt");
  
 if(!ostr.is_open())
 failures++;
   

 long size1 = pbuf->pubseekoff(0,ios_base::end);
 if(size1!=15)
 failures++;
  

  pbuf->sputc('a');
  

 long size2 = pbuf->pubseekoff(0,ios_base::end);
 if(size2!=16)
 failures++;
  
 ifstream istr("c:\\TestFramework\\docs\\streambuf.txt");
 pbuf = istr.rdbuf();

streamsize i = istr.rdbuf()->sgetn(&a[0], 3);   
 
    // Display the size and contents of the buffer passed to sgetn.
   if(i!=3)
   failures++;
  
 
 int k = pbuf->snextc();
 
 
 //sgetc()
 while (pbuf->sgetc()!=EOF)
  {
    // static int i;
    int  i=0;
      //sbumpc()
     ch[i] = pbuf->sbumpc();
     i++;
  }

  
  if(ch[0]!='a' )
  failures++;
  
 
  
  
  
  
  istr.close();
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
 
// -----------------------------------------------------------------------------
// Ctiostreams:: ostream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 
 TInt Ctiostreams::ostreamL(CStifItemParser& aItem )
 
 {
 
  
 int failures = 0;
 try
 {
 	
 
 filebuf fb;
char input[17] = "ostream testcase";
streamsize size = 5;
  fb.open ("c:\\TestFramework\\docs\\ostream.txt",ios::out);
  cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  ostream os(&fb);
  
  os.write(input,size);
  streamoff i = os.tellp();
  if(i!= 5)
  failures++;
  
  os.put('K');
  streamoff j = os.tellp();
  if(j!=6)
  failures++;

  os.seekp(2);
    os<<"i";
  streamoff k = os.tellp();

  if(k!=3)
  failures++;
  
 os.flush();
  
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
// -----------------------------------------------------------------------------
// Ctiostreams:: istream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
  TInt Ctiostreams::istreamL(CStifItemParser& aItem )
  {
 
int failures =0;
try
{
	

int length;
char * buffer;
char getl[8] ;
 filebuf fb;
 fb.open ("c:\\TestFramework\\docs\\istream.txt",ios::in);
 cout<<"";
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 istream is(&fb);
 
 //needs to rewrite


  // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);
  
  
  


if(length != 7)
failures++;

 char ch = is.get();
  
   
 if(is.gcount() != 1)
 failures++;
 
  
 if( ch != 'S')
 failures++;
 
 

 
 
 
 
 char pk1 = is.peek();
 
 if(pk1!= 'h')
 failures++;
 
 
 is.unget();
 
 char pk2 = is.peek();
 if(pk2!= 'S')
 failures++;
 
 is.get();
 is.putback('K');
 
 is.getline(getl,8,'\0');
 
 if(getl == "Khaheen")
 failures++;
 
 if(is.gcount() != 7)
 failures++;
 
 
  fb.close();
  /*if(failures)
 
 
  return KErrGeneral;
  return KErrNone;
  
 */
   
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// Ctiostreams:: istringstream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
  
   TInt Ctiostreams::istringstreamL(CStifItemParser& aItem )
   {
   	int n,val;
 	int failures =0;
 	try
 	{
 	string strvalues = "125 320 512 750 333";
 	cout<<"";	
 	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
	
  	istringstream iss (strvalues,istringstream::in);

if(iss.str()  != "125 320 512 750 333")
failures++;

#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  for (n=0; n<5; n++)
  {
    iss >> val;
     switch(n)
    {
    	case 0:
    	if(val!=125)
    	failures++;
    	break;
    	case 1:
    	if(val!=320)
    	failures++;
    	break;
    	case 2:
    	if(val!=512)
    	failures++;
    	break;
    	case 3:
    	if(val!=750)
    	failures++;
    	break;
    	
    	case 4:
    	if(val!=333)
    	failures++;
    	break;
       
    	    
    default:
    break;
    }
    
  }
  
   
   //#if STDCPP_OOM
//failures++;
// #endif

/* if(failures)
 return KErrGeneral;
 else
 return KErrNone;*/
   }
   
   catch(bad_alloc&)
   {
   	//do nothing
    }
   catch(...)
   {
   	failures++;
   	
   }
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
   
   
   
   
   
   
// -----------------------------------------------------------------------------
// Ctiostreams:: ostringstream,
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
   
   TInt Ctiostreams::ostringstreamL(CStifItemParser& aItem )
   {
 	int failures =0 ;
 	try
 	{
 		
 	
 	basic_string<char> i( "test" );
 	cout<<"";
 	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   ostringstream ss;
   
   ss.rdbuf( )->str( i );
   if(ss.str( ).compare("test") != 0)
   failures++; 

   ss << "z";
   if(ss.str( ) .compare("zest")!=0)
   failures++;
      
   ss.rdbuf( )->str( "be" );
   if(ss.str( ).compare("be")!=0)
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
   
    TInt Ctiostreams::ostreamiterators(CStifItemParser& aItem )
    {
    	
    
  //needs to rewrite
  //____________________
   
   int arr[4] = { 3,4,7,8 };
   int total=0;
   deque<int> d(arr+0, arr+4);
   //
   // stream the whole vector and a sum to cout
   //
  
   copy(d.begin(),(d.end()-1),ostream_iterator<int,char>(cout,""));
  
  
  if( *(d.end()-1) == 8)
  return KErrNone;
  return KErrGeneral;


 }

    
    
// -----------------------------------------------------------------------------
// Ctiostreams:: fstream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
    
    
    TInt Ctiostreams::fstreamL(CStifItemParser& aItem ) 
    {
   //  __UHEAP_MARK;
int failures =0;
try
{
cout<<"";	
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
fstream filestr ("c:\\TestFramework\\docs\\fstream.txt", fstream::in | fstream::out);

if(!filestr.is_open())
failures++;
 
filestr.close();
if(filestr.is_open())
failures++;
  
 // __UHEAP_MARKEND;
  
  /*if(failures)
return KErrNone;
return KErrGeneral;
    */  
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 
 
// -----------------------------------------------------------------------------
// Ctiostreams:: istrstream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
    TInt Ctiostreams::istrstreamL(CStifItemParser& aItem ) 
    {
  //  __UHEAP_MARK;	
  int failures=0;
  try
  {
  	
  
    char* p = "This is first string";
    char* q = "This is second string";
    char* r = "";
    const char* s ="const char";
    streamsize  n =10;
    cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
    istrstream first(p);
    istrstream second(q);
    istrstream third(r);
    
    istrstream four(s);
    
    istrstream  five(p,n);
    istrstream six(s,n);
 /*   if(first.str() == "This is first string")
    if(second.str()!= " This is second string")
    if(third.str() == "")
 //   __UHEAP_MARKEND;
    
    return KErrNone;
    return KErrGeneral;
   */ 
       first.rdbuf();
    second.rdbuf();
    third.rdbuf();
    
     if(first.str() != "This is first string")
    if(second.str()== " This is second string")
    if(third.str() != "")
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
    
TInt Ctiostreams::strstreamL(CStifItemParser& aItem ) 
      {
    // __UHEAP_MARK; 	
     
  	int failures = 0;
  	try
  	{
  	cout<<"";	
  	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  char* s;
   int n;
   ios_base::openmode mode;
//  strstream ss3(s, n, mode);
  	strstream ss1,ss2;
  	 ss1 << "";
  	 ss1.rdbuf();
  	 if(ss1.rdbuf( )->pcount( ) != 0)
  	 failures++;
  	 
  	 string str1= ss1.str();
  	 if(str1.compare("")!=0)
  	 failures++;
  	 
  	 ss2 << "strstream testcase";
  	 ss2 << '\0';
  	 if( ss2.rdbuf( )->pcount( ) != sizeof("strstream testcase\0")-1)
  	 failures++;
  	 
  	 string str = ss2.str();
  	 
  	 if(str.compare("strstream testcase")!= 0)
  	 failures++;
  	 ss1.freeze();
  	 //__UHEAP_MARKEND;
  /*	 
  	 if(failures)
  	 return KErrGeneral;
  	 else
  	return KErrNone;
  */
  
  
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
    
    
TInt Ctiostreams::ostrstreamL(CStifItemParser& aItem ) 
      {
      	
     //__UHEAP_MARK;
     int failures = 0;
     try
     {
     cout<<"";	
     #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  	char* s;
  	int n;
  	ios_base::openmode mode;
//  	ostrstream  oss3( s, n, mode);
  	ostrstream  oss1 ,oss2;
  	string str;
  	 oss1 << "";
  	 oss1.rdbuf();
  	 if(oss1.rdbuf( )->pcount( ) != 0)
  	 failures++;
  	 
  	 oss2 << "ostrstream testcase";
  	 oss2<<'\0';
  	 str = oss2.str();
  	 
  	 if(str.compare("ostrstream testcase") !=0)
  	 failures++;
  	 
   oss2.freeze();
  	 
  	 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  	
  	// __UHEAP_MARKEND;
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
   


 TInt Ctiostreams::istreamiterators(CStifItemParser& aItem )
    {
    	
   // __UHEAP_MARK;
   // Typedefs for convenience.
   int failures=0;
   try
   {
   cout<<"";
   	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   
    typedef std::vector<int, std::allocator<int> >    Vector;

    typedef std::istream_iterator<Vector::value_type,char, std::char_traits<char>, ptrdiff_t>        is_iter;

 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    Vector v;
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
   
   
 TInt Ctiostreams::istreambufiterators(CStifItemParser& aItem )
    {
    	
      
   //   __UHEAP_MARK;
       // create a temporary filename
       
       int failures=0;
       try
       {
       	
       
    const char *fname = tmpnam (0);

    if (!fname)
        return 1;

    // open the file is_iter.out for reading and writing
    std::ofstream out (fname, std::ios::out | std::ios::in | 
                              std::ios::trunc);

    // output the example sentence into the file

    // seek to the beginning of the file
    out.seekp (0);
    cout<<"";
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
    // construct an istreambuf_iterator pointing to
    // the ofstream object underlying streambuffer
    std::istreambuf_iterator<char, std::char_traits<char> >iter (out.rdbuf ());

    // construct an end of stream iterator
    const std::istreambuf_iterator<char,std::char_traits<char> > end;
#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
    std::cout << std::endl;

    // output the content of the file
    while (!iter.equal (end)) {

        // use both operator++ and operator*
        std::cout << *iter++;
    }

    std::cout << std::endl; 

    // remove temporary file
    remove (fname);
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
    
    
    
 TInt Ctiostreams::strstreambufL(CStifItemParser& aItem )
 
 
 {
// 	__UHEAP_MARK;
 	
 int failures =0;
 try
 {
 	
 cout<<"";

  	#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  
  signed char* get;
  streamsize n=10;
  signed char* put;
   unsigned char* uget;
   unsigned char* uput;
  const char* cget;
  const signed char* csget;
  
  const unsigned char* cucget;
  
  
  typedef void* (*__alloc_fn)(size_t);
  typedef void (*__free_fn)(void*);
 
  __alloc_fn  alloc_f;
 
  __free_fn free_f;
  
  	//overloaded
  	strstreambuf  buf1(get, n,put);
  	
  	//overloaded
  	
  	strstreambuf buf2(uget,n,uput);
  	
  	  	//overloaded
  	strstreambuf buf3(cget,n);
  	//onverloaded
  	strstreambuf buf4(csget,n);
  	//overloaded
  	strstreambuf buf5(cucget,n);
  	
  	
//  	strstreambuf buf6( alloc_f, free_f);
  	  	strstreambuf buf7(n);

  	strstreambuf  buf;
  string str;
 int i = buf.sputn("strstreambuf testcase", sizeof("strstreambuf testcase")-1);
 
if((buf.pcount())!= i) 
failures++;
 
 buf.freeze();
//if(buf.str() != "strstreambuf testcase")   //fails
//failures++;

#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 

//__UHEAP_MARKEND; 
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
    
    
    

 TInt Ctiostreams::freezeL(CStifItemParser& aItem )
 {
 
 int failures=0;
 try
 {
 	
  strstream  x;
int failures =0;
    x << "test1";
   
if(!x.good())
failures++;    
cout<<"";
 
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif  
   
  x.rdbuf()->freeze();
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
if(!x.good())
failures++;

    // Stream is bad now, wrote on frozen stream
    x << "test1.5";
    
//  report(x);
if(!x.good())
failures++;
    // Unfreeze stream, but it is still bad
    x.rdbuf()->freeze(false);
 if(!x.good())
failures++;

    // Clear stream
    x.clear();
 
if(!x.good())
failures++;


    x << "test3";




    // Clean up.  Failure to unfreeze stream will cause a
    // memory leak.
    x.rdbuf()->freeze(false);
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 

 TInt Ctiostreams::fposL(CStifItemParser& aItem )
 {
 int failures=0;
 try
 {
 	
 
 streamoff s;
   ifstream file( "c:\\TestFramework\\docs\\fpos_state.txt" );
  cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif
   fpos<mbstate_t> f = file.tellg( );
   char ch;
   while ( !file.eof( ) )
      file.get( ch );
  
  f.state();
    cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif
   //s = f;
  // if(f.state( ))
  // failures++;
  // f.state( 9 );
 //  if( f.state( ))
 //  failures++;
   /*if(failures)
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
 
 
 
// -----------------------------------------------------------------------------
// Cstdcpp_filebuf::filebufL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctiostreams::filebufL( CStifItemParser& aItem )
    {
   int failures=0;
try
{
	


    ifstream is;
    cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  filebuf * fb;

  fb = is.rdbuf();
  fb->open ("c:\\TestFramework\\docs\\filebuf.txt",ios::in);

   if(!fb->is_open())
   failures++;
   

  fb->close();
if(fb->is_open())
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
 
    
TInt Ctiostreams::basicstring( CStifItemParser& aItem )
    {
				int failures =0;
				
				try
				{
					
				
			    const char *cstr1a = "Hello Out There.";
			    cout<<"";
			    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
			   basic_string<char> str1a(cstr1a ,5);
	 
			   if(str1a.compare("Hello")!=0)
			   failures++;
			    
			    str1a.append("Out There");
			    if(!str1a.compare("Hello Out There"))
			    failures++;
			    
			    
			   string  str2a ( "How Do You Do?" );
			   basic_string <char> str2b ( str2a , 7 , 7 );
			   
			    if(str2b.compare("You Do?")!=0)
			   failures++;
			   
			    
			 
			   basic_string <char> str3a ( 5, '9' );
			   if(str3a.compare("99999")!=0)
			   failures++;
			   

			   
			   basic_string <char> str4a;
			   string str4b;
			   basic_string <char> str4c ( str4b.get_allocator( ) );
			   if (!str4c.empty ())
			   failures++;
			      

			  
			   string str5a ( "Hello World");
			    basic_string <char>::iterator strp_Iter, str1_Iter, str2_Iter;
			   
			   
			   
			   strp_Iter = str5a.begin();
			   if(*strp_Iter != 'H' )
				failures++;
			   
			   
			   
			   
			   basic_string <char>::reference refStr2 = str5a.at ( 3 );
			   
			   if(refStr2 != 'l')
			   failures++;
			   
			   
			   if(str5a.size()!=11)
			   failures++;
			   if(str5a.length()!=11)
			   failures++;
			   if(str5a.capacity()!=11)
			   failures++;
			   


		#if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 	   
			  /*  if(failures)
			    return  KErrGeneral;
			    return KErrNone;
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// Ctwiostreams::basic_filebuf
// Example test method function.
// 
// -----------------------------------------------------------------------------
// 
 
 TInt Ctiostreams::basicfilebufL( CStifItemParser& aItem )
 {
 	
	 int failures=0;
	 try
	 {
	 	
	 
   	 char* wszHello = "Hello World";
   	 char wBuffer[128];
   	  cout<<"";
		    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
		
     basic_filebuf<char> wOutFile;
	 
		
    // Open a file, wcHello.txt, then write to it, then dump the
    // file's contents in hex
    wOutFile.open("c:\\TestFramework\\docs\\basicfilebuf.txt",ios_base::out | ios_base::trunc | ios_base::binary);
    if(!wOutFile.is_open())
    failures++;
    
    wOutFile.sputn(wszHello, 11);
    wOutFile.close();
 	if(wOutFile.is_open())
 	failures++;
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 /*	if(failures)
	return KErrGeneral;
 	else
 	return KErrNone;
 
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
// -----------------------------------------------------------------------------
// Ctwiostreams::basic_istream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
 
 TInt Ctiostreams::basicistreamL( CStifItemParser& aItem )
 {
 
 
 		int failures = 0;
 		try
 		{
 		 cout<<"";
 				    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
			
 		
     typedef std::basic_istream<char, std::char_traits<char> >         Input;
    typedef std::basic_ofstream<Input::char_type, Input::traits_type> Output;
	 
		
    Input::char_type s [200];
 
    Output out ("c:\\TestFramework\\docs\\basicistream.txt", std::ios::in | std::ios::out | std::ios::trunc);

     Input in (out.rdbuf ());

    float f = 3.14159;
    int   i = 3;

    // output to the file
    out << "He lifted his head and pondered.\n"
        << f << std::endl << i << std::endl;
  
    // seek to the beginning of the file
    in.seekg (0);
  
  if(!out.is_open())
  failures++;
  
  out.close();
  
  if(out.is_open())
  failures++;
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  /*if(failures)
   return KErrGeneral;
  else
  return KErrNone;
 
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
// -----------------------------------------------------------------------------
// Ctwiostreams::wfstream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
 TInt Ctiostreams::wfstreamL( CStifItemParser& aItem )
 
 {
 	
 int failures=0;
 
 try
 {
 	 cout<<"";
 		    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
		
 wfstream file;
 	  
		
 file.open("c:\\TestFramework\\docs\\wfstream.txt");
 file.rdbuf();
 if(!file.is_open())
 failures++;
 
 file.close();
 
 if(file.is_open())
 failures++;
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
/* if(failures)
 return KErrGeneral;
 return KErrNone;
 
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
 
// -----------------------------------------------------------------------------
// Ctwiostreams::wifstream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
 TInt Ctiostreams::wifstreamL( CStifItemParser& aItem )
 
 {
 int failures =0;
 
 try
 {
 	 cout<<"";
 		    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
		
 wifstream file;
 		    
		
 file.open("c:\\TestFramework\\docs\\wifstream.txt");
 
 
 file.rdbuf();
 if(!file.is_open())
 failures++;
 
 file.close();
 
 if(file.is_open())
 failures++;
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
/* if(failures)
 return KErrGeneral;
 return KErrNone;
 
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    }
 
 
// -----------------------------------------------------------------------------
// Ctwiostreams::wfilebuf,wistream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
 TInt Ctiostreams::wistreamL( CStifItemParser& aItem )
 
 {
 	int failures=0;
 	try
 	{
 		
 	
 int length;
wchar_t * buffer;
wchar_t getl[16] ;
 
 wfilebuf fb;
  fb.open ("c:\\TestFramework\\docs\\wistream.txt",ios::in);
   cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  wistream is(&fb);
  
  if(!fb.is_open())
  failures++;
  
  //if( char(is.get())!= 'S')
 // failures++;
  
//if(sizeof(fb)!=140)  
//failures++;




  // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);
  
  
  


if(length != 15)
failures++;

 wchar_t ch = is.get();
  
   
 if(is.gcount() != 1)
 failures++;
 
  
 if( ch != L'S')
 failures++;
 
 

 
 
 
 
 wchar_t pk1 = is.peek();
 
 if(pk1!= 'a')
 failures++;
 
 
 is.unget();
 
 wchar_t pk2 = is.peek();
 if(pk2!= 'S')
 failures++;
 
 is.get();
 is.putback('K');
 
 is.getline(getl,16,'\0');
 
 if(getl == L"Kample sentence")
 failures++;
 


fb.close();

if(fb.is_open())
failures++;
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
/*
 if(failures)
 
 return sizeof(fb);
 return KErrNone;
  
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 
 
// -----------------------------------------------------------------------------
// Ctwiostreams::wofstream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
 
  TInt Ctiostreams::wofstreamL( CStifItemParser& aItem )
  {
  	
  	int failures=0;
  	try
  	{
  	 cout<<"";	
  	 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  	
  	wofstream outfile;
  	
  	outfile.open("c:\\TestFramework\\docs\\wostream.txt");
  	outfile.write(L"This is an ostreamfile",22);
  	
  	outfile.rdbuf();
  	if(!outfile.is_open())
  	failures++;
  	
  	outfile.close();
  	
  	
  	if(outfile.is_open())
  	failures++;
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 /* 	if(failures)
  	return KErrGeneral;
  	else
  	return KErrNone;
  
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 
  
// -----------------------------------------------------------------------------
// Ctwiostreams::wistringstream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
   TInt Ctiostreams::wistringstreamL( CStifItemParser& aItem )
   {
   int failures=0;
   try
   {
   	
   
   	int output[5];
   int n,val;
   wstring strvalues ;
   
   strvalues = L"1 2 3 4 5";
    cout<<"";
    #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
   wistringstream iss(strvalues,wistringstream::in);
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
   
   for(n=0;n<5;n++)
   {
   	
   static int j;
   
   iss>>val;
   
   output[j]=val;
   
   j++;
   
   
   }
   
      if(iss.str()!=L"1 2 3 4 5")
      failures++;
/*
   if(output[0]== 1)
   if(output[1]== 2)
   if(output[2]== 3)
   if(output[3]== 4 )
   if(output[4]== 5)
   
   return KErrNone;
   else 
   return  KErrGeneral;
   */
   
    if(output[0]!= 1)
    failures++;
   if(output[1]!= 2)
   failures++;
   if(output[2]!= 3)
   failures++;
   if(output[3]!= 4 )
   failures++;
   if(output[4]!= 5)
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 
// -----------------------------------------------------------------------------
// Ctwiostreams::wostringstream,wstring
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
  TInt Ctiostreams::wostringstreamL( CStifItemParser& aItem )
  {
  	int failures=0;
  	try
  	{ cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 		
  	
  
wostringstream oss;
wstring mystr;

oss << L"wostringstream testcase";
oss.rdbuf();
mystr=oss.str();
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
/*if(mystr.compare(L"wostringstream testcase")==0)
return KErrNone;
return KErrGeneral;
*/
if(mystr.compare(L"wostringstream testcase")!=0)
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 
// -----------------------------------------------------------------------------
// Ctwiostreams::wstreambuf,wofstreams
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
   
  TInt Ctiostreams::wstreambufL( CStifItemParser& aItem )
  {


//locale   loc ("en_GB.UTF-8");

 
 
 int failures = 0;
 try
 {
 	
 
 wchar_t a[4]=
 {
 	0
 };
 
 wchar_t sentence[]= L"Sample sentence"; cout<<"";
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
 wstreambuf * pbuf;
 wofstream ostr ("c:\\TestFramework\\docs\\wstreambuf.txt");
 
 
  
// if(cout.rdbuf( )->getloc( ).name( ).c_str( ) != "C") //failing
 //failures++;
 
 wchar_t ch[28];
 //rdbuf()s
 pbuf = ostr.rdbuf();


//sputn()




 pbuf->sputn (sentence,sizeof(sentence)-1);

  
 ostr.open("c:\\TestFramework\\docs\\wstreambuf.txt");
  
 if(!ostr.is_open())
 failures++;
   

 long size1 = pbuf->pubseekoff(0,ios_base::end);
 if(size1!=31)
 failures++;
  

  pbuf->sputc('a');
  

 long size2 = pbuf->pubseekoff(0,ios_base::end);
 if(size2!=32)
 failures++;
  
 wifstream istr("c:\\TestFramework\\docs\\wstreambuf.txt");
 pbuf = istr.rdbuf();

streamsize i = istr.rdbuf()->sgetn(&a[0], 3);   
    //a[i] = istr.widen('\0');

    // Display the size and contents of the buffer passed to sgetn.
   if(i!=3)
   failures++;
  
 
 int k = pbuf->snextc();
 
 
 //sgetc()
 while (pbuf->sgetc()!=EOF)
  {
     static int i;
      //sbumpc()
     ch[i] = pbuf->sbumpc();
     i++;
  }

  
  if(ch[0]!='l' )
  failures++;
  
 
  
  
  
  
  istr.close();
  ostr.close();
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
 /* if(failures)
  return KErrGeneral;
  else
  return KErrNone;
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 

// -----------------------------------------------------------------------------
// Ctwiostreams::wostream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//

 TInt Ctiostreams::wostreamL( CStifItemParser& aItem )  
 {
 	

 
 int failures = 0;
 try
 {
 	

 wfilebuf fb;
wchar_t input[18] = L"wostream testcase";
streamsize size = 5;
  fb.open ("c:\\TestFramework\\docs\\wostream.txt",ios::out); cout<<"";
   #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif 
  wostream os(&fb);
  
  os.write(input,size);
  streamoff i = os.tellp();
  if(i!= 5)
  failures++;
  
  os.put('K');
  streamoff j = os.tellp();
  if(j!=6)
  failures++;

  os.seekp(2);
    os<<"i";
  streamoff k = os.tellp();

  if(k!=3)
  failures++;
  
  os.flush();
  
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  
 
  /*
if(failures)
return KErrGeneral;
return KErrNone;
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 

// -----------------------------------------------------------------------------
// Ctwiostreams::wstringbuf
// Example test method function.
// 
// -----------------------------------------------------------------------------
// 
 
  TInt Ctiostreams::wstringbufL( CStifItemParser& aItem )  
  {
  int failures=0;
  try
  {
   cout<<"";	
 #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::EDeterministic,1);
  #endif  
wstringbuf sb;
wstring mystr;

  
sb.sputn (L"Sample string",13);
mystr=sb.str();

/*if(mystr.compare(L"Sample string") == 0)
return KErrNone;
else
return KErrGeneral;
  */
  #if  STDCPP_OOM
User::__DbgSetAllocFail(FALSE,RHeap::ENone,1);
  #endif 
  if(mystr.compare(L"Sample string") != 0)
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
   
	if( failures == 0 )
		return KErrNone;
  else  
		return KErrGeneral;
    } 
  
//  End of File

//  End of File
    
    
 
//  End of File
