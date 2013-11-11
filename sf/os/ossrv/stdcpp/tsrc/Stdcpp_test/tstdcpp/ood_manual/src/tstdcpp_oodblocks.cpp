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
#include "tstdcpp_ood.h"


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
#include <stl/char_traits.h>
 #include<e32std.h>
 #include<exception>
#include<errno.h>
 using namespace std;

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
// Ctstdcpp_ood::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctstdcpp_ood::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctstdcpp_ood::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctstdcpp_ood::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "istreamL", Ctstdcpp_ood::istreamL ),
                ENTRY( "wistreamL", Ctstdcpp_ood::wistreamL ),


        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctiostreams:: istream
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
  TInt Ctstdcpp_ood::istreamL(CStifItemParser& aItem )
  {
 
int failures =0;
try
{
	

int length;
char * buffer;
char getl[8] ;
 filebuf fb;
 fb.open ("c:\\TestFramework\\docs\\istream.txt",ios::in);
 
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
 
   if(errno!=28)
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
   
  }
 
// -----------------------------------------------------------------------------
// Ctwiostreams::wfilebuf,wistream
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
 TInt Ctstdcpp_ood::wistreamL( CStifItemParser& aItem )
 
 {
 	int failures=0;
 	try
 	{
 		
 	
 int length;
wchar_t * buffer;
wchar_t getl[16] ;
 
 wfilebuf fb;
  fb.open ("c:\\TestFramework\\docs\\wistream.txt",ios::in);
  
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


if(errno!=28)
   failures++;

if(fb.is_open())
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