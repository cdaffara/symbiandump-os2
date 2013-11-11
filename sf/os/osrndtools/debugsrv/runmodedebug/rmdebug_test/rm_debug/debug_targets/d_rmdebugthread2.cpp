// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements a debug thread for testing.
// 
//

#include <e32base.h>
#include <e32base_private.h>
#include <e32cons.h>
#include <e32debug.h>
#include "d_rmdebugthread2.h"

#include "d_rmdebug_step_test.h"
#include "d_rmdebug_bkpt_test.h"
#include "d_demand_paging.h"

TBuf8<SYMBIAN_RMDBG_MEMORYSIZE> gMemoryAccessBytes;
IMPORT_C extern void RMDebug_BranchTst1();
IMPORT_C extern TInt RMDebugDemandPagingTest();

TInt TestData;
TTestFunction FunctionChooser;

const TInt 	KNumberOfTraceCalls = 50;

EXPORT_C TInt TestFunction()
	{
	// Set TestData to an arbitrary value that can be checked by a tester
	TestData = 0xffeeddcc;
	RMDebug_BranchTst1();
	
	// Code here may not be executed because tests can change the PC value
	// at any time, typically once the test passes
	return 0;
	}

/**
  Wrapper around RMDebugDemandPagingTest, need to pause for a short time to
  allow time in t_rmdebug.cpp to issue a User::WaitForRequest to catch the break point
  */
EXPORT_C void TestPagedCode()
	{
	User::After(100000);

	// call the function in paged code
	RMDebugDemandPagingTest();
	}

EXPORT_C void TestMultipleTraceCalls()
	{
	//arbitrary function to set a BP on
	RMDebug_BranchTst1();

	// The tester will change FunctionChooser once it gets what it needs out of the test
	for(TInt cnt = KNumberOfTraceCalls; cnt>0 && (FunctionChooser==EMultipleTraceCalls); cnt--)
		{
		RDebug::Printf("T");
		RDebug::Printf("R");
		RDebug::Printf("A");
		RDebug::Printf("C");
		RDebug::Printf("E");
		}
	
	//another arbitrary function to set a BP on
	RMDebug_StepTest_Non_PC_Modifying();
	}

CDebugServThread::~CDebugServThread()
    {
    }

CDebugServThread::CDebugServThread()
//
// Empty constructor
//
	{
	}


/**
 * Check that the RProperty argument does not change within the given amount of time.
 * If the property does change, the error KErrInUse is returned.
 * 
 */
EXPORT_C TInt TestRunCountSame( RProperty & aProperty, RTimer & aTimer, TTimeIntervalMicroSeconds32 aTimeOut )
    {
    TRequestStatus propertyStatus;
    TRequestStatus timerStatus;
    TInt propertyValueBefore = 0;
    TInt propertyValueAfter = 0;

    aProperty.Subscribe( propertyStatus );
    aProperty.Get( propertyValueBefore );
    aTimer.After( timerStatus, aTimeOut );

    User::WaitForRequest( propertyStatus, timerStatus );
    if (propertyStatus != KRequestPending)
        {
        RDebug::Printf(" CDebugServThread::TestRunCountSame: Property has been set. Returning KErrInUse");
        aTimer.Cancel();
        // Wait for the KErrCancel
        User::WaitForRequest( timerStatus );
        return KErrInUse;
        }

    aProperty.Cancel();
    //This will wait for the KErrCancel to be issued by the property.
    User::WaitForRequest( propertyStatus );
    
    aProperty.Get( propertyValueAfter );
    if( propertyValueAfter != propertyValueBefore )
        {
        RDebug::Printf(" CDebugServThread::TestRunCountSame: Change in property value. Returning KErrInUse");
        return KErrInUse;
        }
        
    return KErrNone;
    }


/**
 * Check that the RProperty argument changes within the given amount of time.
 * If the property does not change, the error KErrTimedOut is returned.
 * If the values before and after are the same, the error KErrNotReady is returned
 */
EXPORT_C TInt WaitForRunCountChange( RProperty & aProperty, RTimer & aTimer, TTimeIntervalMicroSeconds32 aTimeOut )
    {
    TRequestStatus propertyStatus;
    TRequestStatus timerStatus;
    TInt propertyValueBefore = 0;
    TInt propertyValueAfter = 0;
    
    aProperty.Get( propertyValueBefore );
    aProperty.Subscribe( propertyStatus );

    aTimer.After( timerStatus, aTimeOut );

    User::WaitForRequest( propertyStatus, timerStatus );
    if (timerStatus != KRequestPending)
        {
        RDebug::Printf(" CDebugServThread::WaitForRunCountChange: timeout. Returning KErrTimedOut");
        aProperty.Cancel();
        // Wait for the KErrCancel
        User::WaitForRequest( propertyStatus );
        return KErrTimedOut;
        }
    
    aTimer.Cancel();
    //This will wait for the KErrCancel to be issued by the timer.
    User::WaitForRequest( timerStatus );
    
    aProperty.Get( propertyValueAfter );
    if( propertyValueAfter == propertyValueBefore )
        {
        RDebug::Printf(" CDebugServThread::WaitForRunCountChange: No change in property value. Returning KErrNotReady");
        return KErrNotReady;
        }
        
    return KErrNone;
    }

GLDEF_C TInt CDebugServThread::ThreadFunction(TAny*)
//
// Generic thread function for testing
//
	{
	// set FunctionChooser to run the default function
	FunctionChooser = EDefaultFunction;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup == NULL)
		{
		User::Leave(KErrNoMemory);
		}
    
	TInt err = RProperty::Define( RProcess().SecureId(), ERMDBGRunCountProperty, RProperty::EInt );
    if( (err != KErrAlreadyExists) && (err != KErrNone) )
        {
        RDebug::Printf("CDebugServThread::ThreadFunction - unable to create 'ERunCount' property. err:%d", err);
        }
    
	RThread::Rendezvous(KErrNone);

	TestData = 1;

	/* Beware of adding printf or other debug-generating events in this loop because
	* they interfere with the tests
	*/
	while(TestData != 0xFFFFFFFF)
		{
        //iRunCountPublish.Set( TestData );
        RProperty::Set( RProcess().SecureId(), ERMDBGRunCountProperty, TestData );
        
		switch(FunctionChooser)
			{
			case EDemandPagingFunction:
				TestPagedCode();
				break;
			case EDefaultFunction:
				// the default function is the stepping test functions
			case EStepFunction:
				{
				RMDebug_BranchTst1();

				// Single stepping test support code

				// ARM tests
				RMDebug_StepTest_Non_PC_Modifying();

				RMDebug_StepTest_Branch();

				RMDebug_StepTest_Branch_And_Link();

				RMDebug_StepTest_MOV_PC();

				RMDebug_StepTest_LDR_PC();
 
// thumb/interworking tests not supported on armv4
#ifdef __MARM_ARMV5__

				// Thumb tests
				RMDebug_StepTest_Thumb_Non_PC_Modifying();

				RMDebug_StepTest_Thumb_Branch();

				RMDebug_StepTest_Thumb_Branch_And_Link();

				RMDebug_StepTest_Thumb_Back_Branch_And_Link();

				// ARM <-> Thumb interworking tests
				RMDebug_StepTest_Interwork();

				RMDebug_StepTest_Thumb_AddPC();

#endif	// __MARM_ARMV5__
				
				// Single-stepping performance
				RMDebug_StepTest_Count();

				// multiple step test
				RMDebug_StepTest_ARM_Step_Multiple();

				// Breakpoints in loop test
				RMDebug_Bkpt_Test_Entry();

				TestData++;
				
				RDebug::Printf("** TestData=%d", TestData) ;

				// Wait 50mSecs. // (suspends this thread)
				User::After(50000);

				break;
				}
			case EMultipleTraceCalls:
				TestMultipleTraceCalls();
				break;
			default:
				//do nothing
				break;
			}
		}

	RProperty::Delete( RProcess().SecureId(), ERMDBGRunCountProperty );
	        
	delete cleanup;

	return (KErrNone);
	}

EXPORT_C TInt StartDebugThread(RThread& aDebugThread, const TDesC& aDebugThreadName)
//
// Starts a test thread
//
{
	TInt res=KErrNone;

	// Create the thread
	res = aDebugThread.Create(	aDebugThreadName,
								CDebugServThread::ThreadFunction,
								KDefaultStackSize,
								KDebugThreadDefaultHeapSize,
								KDebugThreadDefaultHeapSize,
								NULL
								);

	// Check that the creation worked
	if (res == KErrNone)
		{
		TRequestStatus rendezvousStatus;

		aDebugThread.SetPriority(EPriorityNormal);
		// Make a request for a rendezvous
		aDebugThread.Rendezvous(rendezvousStatus);
		// Set the thread as ready for execution
		aDebugThread.Resume();
		// Wait for the resumption
		User::WaitForRequest(rendezvousStatus);
		}                                 
	else
		{
		// Close the handle.
		aDebugThread.Close();
		}
	        
	return res;
	}
