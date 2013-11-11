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
* Description: This module contains the implementation of 
* TTestKeywords class member functions.
*
*/


// INCLUDE FILES
#include "TestKeywords.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: TTestKeywords

     Method: Keyword

     Description: Returns a string desrciptor corresponding to keyword number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC8: keyword descriptor

     Errors/Exceptions: None
     
     Status: Approved 
    
-------------------------------------------------------------------------------
*/
TPtrC TTestKeywords::Keyword( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"title",
        (TText*)L"timeout",
        (TText*)L"priority",
        (TText*)L"create",
        (TText*)L"delete",
        (TText*)L"request",
        (TText*)L"wait",
        (TText*)L"release",
        (TText*)L"set",
        (TText*)L"unset",
        (TText*)L"print",
        (TText*)L"allownextresult",
        (TText*)L"waittestclass",
        (TText*)L"createkernel",
        (TText*)L"pause",
        (TText*)L"loop",
        (TText*)L"endloop",
        (TText*)L"oomignorefailure",
        (TText*)L"oomheapfailnext",
        (TText*)L"oomheapsetfail",
        (TText*)L"oomheaptonormal",
        (TText*)L"testinterference",
        (TText*)L"measurement",
        (TText*)L"allowerrorcodes",
        (TText*)L"bringtoforeground",
        (TText*)L"sendtobackground",
        (TText*)L"presskey",
        (TText*)L"typetext",        
        (TText*)L"var",
        (TText*)L"callsub",
        (TText*)L"createx",
        (TText*)L"setresultdescription",
        (TText*)L"sendpointerevent",
        };

    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 

    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;

    }

/*
-------------------------------------------------------------------------------

     Class: TTestKeywords

     Method: Priority

     Description: Returns a string desrciptor corresponding to priority 
                    keywords optional argument number. 

     Parameters:   TInt aArg: in: argument index.
     
     Return Values: TPtrC8: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/
TPtrC TTestKeywords::Priority( TInt aArg )
    {
    static TText* const priority[] =
        {
        (TText*)L"high",
        (TText*)L"normal",
        (TText*)L"low",
        };

    if( aArg >= (TInt)(sizeof( priority )/sizeof(TText*)) )
        {
        TPtrC null;
        return null;
        } 

    TPtrC arg( priority[ aArg ] ); 
    return arg;
    
    }
      
/*
-------------------------------------------------------------------------------

     Class: TTestKeywords

     Method: Parse

     Description: Returns a keyword enum corresponding to keyword 
                      string descriptor.

     Parameters:    TPtrC aKeyword: in: keyword descriptor.
                    KeywordFunc aFunc: in: Function pointer to keyword parser 
     
     Return Values: TInt: keyword index

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/
TInt TTestKeywords::Parse( TDesC& aKeyword, KeywordFunc aFunc )
    {
    TInt ind;
    for( ind = 0; aFunc( ind ).Length() > 0; ind++ )
        {
        if( aFunc( ind ) == aKeyword )
            {
            return ind;
            }
        }
    return KErrNotFound;
    };
    
// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
