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
* TTCKeywords class member functions.
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

     Class: TTCKeywords

     Method: Keyword

     Description: Returns a string desrciptor corresponding to keyword number. 

     Parameters:    TKeywords aKeyword: in: keyword index.
     
     Return Values: TPtrC8: keyword descriptor

     Errors/Exceptions: None
     
     Status: Approved 
    
-------------------------------------------------------------------------------
*/      
TPtrC TTCKeywords::Keyword( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"title",
        (TText*)L"run",
        (TText*)L"cancel",
        (TText*)L"pause",
        (TText*)L"resume",
        (TText*)L"complete",
        (TText*)L"request",
        (TText*)L"wait",
        (TText*)L"release",
        (TText*)L"set",
        (TText*)L"unset",
        (TText*)L"timeout",
        (TText*)L"priority",
        (TText*)L"print",
        (TText*)L"allocate",
        (TText*)L"free",
        (TText*)L"remote",
        (TText*)L"sendreceive",
        (TText*)L"canceliferror",
        (TText*)L"measurement",
        (TText*)L"loop",
        (TText*)L"endloop",
        (TText*)L"pausecombiner",        
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

     Class: TTCKeywords

     Method: RunOptArg

     Description: Returns a string desrciptor corresponding to run keyword 
                  optional argument number. 

     Parameters:    TRunOptArgs aArg: in: run keyword optional argument index
     
     Return Values: TPtrC8: argument descriptor

     Errors/Exceptions: None
     
     Status: Approved 
    
-------------------------------------------------------------------------------
*/
TPtrC TTCKeywords::RunOptArg( TInt aArg )
    {
    static TText* const runOptArgs[] =
        {
        (TText*)L"expect",
        (TText*)L"testid",
        (TText*)L"ini",
        (TText*)L"category",
        (TText*)L"timeout",
        (TText*)L"title",
        (TText*)L"args",
        };

    if( aArg >= (TInt)(sizeof( runOptArgs )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          }

    TPtrC arg( runOptArgs[ aArg ] );
    return arg;
    
    }

/*
-------------------------------------------------------------------------------

     Class: TTCKeywords

     Method: PauseOptArg

     Description: Returns a string desrciptor corresponding to pause keyword 
                      optional argument number. 

     Parameters:    TPauseOptArgs aArg: in: argument index.
     
     Return Values: TPtrC8: argument descriptor

     Errors/Exceptions: None
     
     Status: Approved 
    
-------------------------------------------------------------------------------
*/
TPtrC TTCKeywords::PauseOptArg( TInt aArg )
    {
    static TText* const pauseOptArgs[] =
        {
        (TText*)L"time",
        };
    
    if( aArg >= (TInt)(sizeof( pauseOptArgs )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC arg( pauseOptArgs[ aArg ] ); 
    return arg;
    
    }
      
/*
-------------------------------------------------------------------------------

     Class: TTCKeywords

     Method: EventOptArg

     Description: Returns a string desrciptor corresponding to event keywords 
                      optional argument number. 

     Parameters:    TEventOptArgs aArg: in: argument index.
     
     Return Values: TPtrC8: keyword descriptor

     Errors/Exceptions: None
     
     Status: Approved 
    
-------------------------------------------------------------------------------
*/              
TPtrC TTCKeywords::EventOptArg( TInt aArg )
    {
    static TText* const optArgs[] =
        {
        (TText*)L"state",
        };
    
    if( aArg >= (TInt)(sizeof( optArgs )/sizeof(TText*)) )
        {
        TPtrC null;
          return null;
          } 
    
    TPtrC arg( optArgs[ aArg ] ); 
    return arg;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: TTCKeywords

     Method: ResultCategory

     Description: Returns a string desrciptor corresponding to event keywords 
                      optional argument number. 

     Parameters:    TEventOptArgs aArg: in: argument index.
     
     Return Values: TPtrC8: keyword descriptor

     Errors/Exceptions: None
     
     Status: Approved 
    
-------------------------------------------------------------------------------
*/              
TPtrC TTCKeywords::Priority( TInt aArg )
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

     Class: TTCKeywords

     Method: Parse

     Description: Returns a keyword enum corresponding to keyword 
                      string descriptor.

     Parameters:    TPtrC aKeyword: in: keyword descriptor.
                    KeywordFunc aFunc: in: Function pointer to keyword parser 
     
     Return Values: TInt: keyword index
                    KErrNotFound: keyword not found

     Errors/Exceptions: None
     
     Status: Proposal 
    
-------------------------------------------------------------------------------
*/
TInt TTCKeywords::Parse( TDesC& aKeyword, KeywordFunc aFunc )
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
    
/*
-------------------------------------------------------------------------------

     Class: TTCKeywords

     Method: ResultCategory

     Description: Returns a string desrciptor corresponding to result 
        category number. 

     Parameters:    TInt aArg: in: argument index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Proposal
    
-------------------------------------------------------------------------------
*/              
TPtrC TTCKeywords::ResultCategory( TInt aArg )
    {
    static TText* const category[] =
        {
        (TText*)L"ongoing",
        (TText*)L"normal",
        (TText*)L"cancelled",
        (TText*)L"errorfrommodule",
        (TText*)L"leave",
        (TText*)L"panic",
        (TText*)L"exception",
        (TText*)L"timeout",
        };
        
    if( aArg >= (TInt)(sizeof( category )/sizeof(TText*)) )
        {
        TPtrC null;
        return null;
        } 
    
    TPtrC arg( category[ aArg ] ); 
    return arg;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: TTCKeywords

     Method: ResultCategory

     Description: Returns a string desrciptor corresponding to event keywords 
                      optional argument number. 

     Parameters:    TEventOptArgs aArg: in: argument index.
     
     Return Values: TPtrC8: keyword descriptor

     Errors/Exceptions: None
     
     Status: Proposal 
    
-------------------------------------------------------------------------------
*/              
TFullTestResult::TCaseExecutionResult 
    TTCKeywords::GetResultCategory( TDesC& aCategory )
    {
    
    TInt ind;
    for( ind = 0; ResultCategory( ind ).Length() > 0; ind++ )
        {
        if( ResultCategory( ind ) == aCategory )
            {
            switch( ind )
                {
                case TFullTestResult::ECaseExecuted:
                case TFullTestResult::ECaseLeave:
                case TFullTestResult::ECasePanic:
                case TFullTestResult::ECaseException:
                case TFullTestResult::ECaseTimeout:
                    return (TFullTestResult::TCaseExecutionResult)ind;
                default:
                    break;
                }        
            }
        }      
    return TFullTestResult::ECaseOngoing;    
    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
