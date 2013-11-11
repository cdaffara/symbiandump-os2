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
* Description: This file contains SUEvent module implementation.
*
*/

// INCLUDE FILES
#include <e32math.h>
#include "SUEvent.h"
#include <StifTestEventInterface.h>

#include <e32svr.h>

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
// CSUEvent::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CSUEvent::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    SUEvent.cpp file and to SUEvent.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array

        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g.
        // CSUEvent::PrintTest. Otherwise the compiler
        // gives errors.

		//sets event as state event
        ENTRY("SetEventState",			CSUEvent::SetEventState),
        //sets event as indication event
        ENTRY("SetEventIndication",		CSUEvent::SetEventIndication),
        //unsets event
        ENTRY("UnsetEvent",				CSUEvent::UnsetEvent),
        //waits for evenr
        ENTRY("WaitEvent",				CSUEvent::WaitEvent),
        //sets event as indication in hard mode
        ENTRY("SetEventIndicationHard",	CSUEvent::SetEventIndicationHard),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CSUEvent::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CSUEvent::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) /
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

//#0
TInt CSUEvent::SetEventState(TTestResult& aResult, const TFileName& aEventName)
    {
//RDebug::Print(_L(" CSUEvent::SetEventState start event_name=%S (%d)"), &aEventName, RThread().Id().Id());
	iLog->Log(_L("CSUEvent::SetEventState event=%S"), &aEventName);

	//Set state event
	TEventIf setEvent(TEventIf::ESetEvent, aEventName, TEventIf::EState);
//RDebug::Print(_L(" CSUEvent::SetEventState call Event (%d)"), RThread().Id().Id());
	TestModuleIf().Event(setEvent);
	
//RDebug::Print(_L(" CSUEvent::SetEventState call SetResult (%d)"), RThread().Id().Id());
	aResult.SetResult(KErrNone, _L(""));

//RDebug::Print(_L(" CSUEvent::SetEventState end (%d)"), RThread().Id().Id());
    return KErrNone;
    }

//#1
TInt CSUEvent::SetEventIndication(TTestResult& aResult, const TFileName& aEventName)
    {
//RDebug::Print(_L(" CSUEvent::SetEventIndication start event_name=%S (%d)"), &aEventName, RThread().Id().Id());
	iLog->Log(_L("CSUEvent::SetEventIndication event=%S"), &aEventName);

	//Set indication event
	TEventIf setEvent(TEventIf::ESetEvent, aEventName, TEventIf::EIndication);
//RDebug::Print(_L(" CSUEvent::SetEventIndication call Event (%d)"), RThread().Id().Id());
	TestModuleIf().Event(setEvent);

//RDebug::Print(_L(" CSUEvent::SetEventIndication call SetResult (%d)"), RThread().Id().Id());
	aResult.SetResult(KErrNone, _L(""));
//RDebug::Print(_L(" CSUEvent::SetEventIndication end (%d)"), RThread().Id().Id());
    return KErrNone;
    }

//#2
TInt CSUEvent::UnsetEvent(TTestResult& aResult, const TFileName& aEventName)
    {
RDebug::Print(_L("CSUEvent::UnsetEvent start event_name=%S"), &aEventName);
	iLog->Log(_L("CSUEvent::UnsetEvent event=%S"), &aEventName);

	//Unset event
	TEventIf event(TEventIf::EUnsetEvent, aEventName, TEventIf::EState);
RDebug::Print(_L("CSUEvent::UnsetEvent call Event"));
	TestModuleIf().Event(event);

RDebug::Print(_L("CSUEvent::UnsetEvent call SetResult"));
	aResult.SetResult(KErrNone, _L(""));
RDebug::Print(_L("CSUEvent::UnsetEvent end"));
    return KErrNone;
    }


//#3
TInt CSUEvent::WaitEvent(TTestResult& aResult, const TFileName& aEventName)
    {
//RDebug::Print(_L(" CSUEvent::WaitEvent start event name=%S (%d)"), &aEventName, RThread().Id().Id());
	iLog->Log(_L("CSUEvent::WaitEvent event=%S"), &aEventName);

	//Request, wait for, and release event
//RDebug::Print(_L(" CSUEvent::WaitEvent set event request (%d)"), RThread().Id().Id());
	TEventIf event(TEventIf::EReqEvent, aEventName, TEventIf::EState);
//RDebug::Print(_L(" CSUEvent::WaitEvent call Event (%d)"), RThread().Id().Id());
	TestModuleIf().Event(event);
//RDebug::Print(_L(" CSUEvent::WaitEvent set event (%d)"), RThread().Id().Id());
	event.SetType(TEventIf::EWaitEvent);
//RDebug::Print(_L(" CSUEvent::WaitEvent call Event (%d)"), RThread().Id().Id());
	TestModuleIf().Event(event);
//RDebug::Print(_L(" CSUEvent::WaitEvent set event release (%d)"), RThread().Id().Id());
	event.SetType(TEventIf::ERelEvent);
//RDebug::Print(_L(" CSUEvent::WaitEvent call Event (%d)"), RThread().Id().Id());
	TestModuleIf().Event(event);

//RDebug::Print(_L(" CSUEvent::WaitEvent call SetResult (%d)"), RThread().Id().Id());
	aResult.SetResult(KErrNone, _L(""));
//RDebug::Print(_L(" CSUEvent::WaitEvent end (%d)"), RThread().Id().Id());
    return KErrNone;
    }


//#4
TInt CSUEvent::SetEventIndicationHard(TTestResult& aResult, const TFileName& /*aEventName*/)
    {
//RDebug::Print(_L(" CSUEvent::SetEventIndicationHard start (%d)"), RThread().Id().Id());
	iLog->Log(_L("CSUEvent::SetEventIndicationHard"));

	TBuf<20> eventName;
	eventName.Copy(_L("event1"));
	//Set indication event
	TEventIf setEvent(TEventIf::ESetEvent, eventName, TEventIf::EIndication);
//RDebug::Print(_L(" CSUEvent::SetEventIndicationHard call Event (%d)"), RThread().Id().Id());
	TestModuleIf().Event(setEvent);

//RDebug::Print(_L(" CSUEvent::SetEventIndicationHard call SetResult (%d)"), RThread().Id().Id());
	aResult.SetResult(KErrNone, _L(""));
//RDebug::Print(_L(" CSUEvent::SetEventIndicationHard end (%d)"), RThread().Id().Id());
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
?type ?classname::?member_function(
   ?arg_type arg,
   ?arg_type arg )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/

// End of File
