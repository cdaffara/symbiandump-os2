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
#include<string>
#include <vector>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<istream>

#include<locale>
#include "twiostreams.h"
#define STDCPP_OOM FALSE
using namespace std;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCtwiostreams::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctwiostreams::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CCtwiostreams::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctwiostreams::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "basicstring", Ctwiostreams::basicstring ),
        ENTRY( "basicfilebufL", Ctwiostreams::basicfilebufL ),
        ENTRY( "basicistreamL", Ctwiostreams::basicistreamL ),
        ENTRY( "wfstreamL", Ctwiostreams::wfstreamL ),
        ENTRY( "wifstreamL", Ctwiostreams::wifstreamL ),
        ENTRY( "wistreamL", Ctwiostreams::wistreamL ),
		ENTRY( "wofstreamL", Ctwiostreams::wofstreamL ),
 		ENTRY( "wistringstreamL", Ctwiostreams::wistringstreamL ),
 		ENTRY( "wostringstreamL", Ctwiostreams::wostringstreamL ),
 		ENTRY( "wstreambufL", Ctwiostreams::wstreambufL ),
 		ENTRY( "wostreamL", Ctwiostreams::wostreamL ),
 		ENTRY( "wstringbufL", Ctwiostreams::wstringbufL ),
         

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctwiostreams::basic_string
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt Ctwiostreams::basicstring( CStifItemParser& aItem )
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
			    basic_string <char>::iterator strp_Iter;
			   
			   
			   
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
 
 TInt Ctwiostreams::basicfilebufL( CStifItemParser& aItem )
 {
 	
	 int failures=0;
	 try
	 {
	 	
	 
   	 char* wszHello = "Hello World";
//   	 char wBuffer[128];
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
 
 TInt Ctwiostreams::basicistreamL( CStifItemParser& aItem )
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
	 
		
//    Input::char_type s [200];
 
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
 TInt Ctwiostreams::wfstreamL( CStifItemParser& aItem )
 
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
 TInt Ctwiostreams::wifstreamL( CStifItemParser& aItem )
 
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
 TInt Ctwiostreams::wistreamL( CStifItemParser& aItem )
 
 {
 	int failures=0;
 	try
 	{
 		
 	
 int length;
//wchar_t * buffer;
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
 
  TInt Ctwiostreams::wofstreamL( CStifItemParser& aItem )
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
   TInt Ctwiostreams::wistringstreamL( CStifItemParser& aItem )
   {
   int failures=0;
   try
   {
   	
   
   	int output[5];
   int n = 0,val = 0;
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
  TInt Ctwiostreams::wostringstreamL( CStifItemParser& aItem )
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
   
  TInt Ctwiostreams::wstreambufL( CStifItemParser& aItem )
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

 TInt Ctwiostreams::wostreamL( CStifItemParser& aItem )  
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
 
  TInt Ctwiostreams::wstringbufL( CStifItemParser& aItem )  
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
