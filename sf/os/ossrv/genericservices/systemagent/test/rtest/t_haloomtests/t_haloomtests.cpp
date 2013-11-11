// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// For the out of memory test of  HAL initialise/ Persist functions :
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32base.h>
#include "halfiles.h"
#include <e32test.h>

LOCAL_D RTest 			TheTest (_L ("OOM Tests"));

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1711
@SYMTestCaseDesc        OOM Test Initialise HAL attributes
@SYMTestPriority        Medium
@SYMTestActions         Tests for the out of memory conditions
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/

LOCAL_C void OOMTestInitialiseHAL()
		{
		TheTest.Next(_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1711 OOM test for InitialiseHAL() "));
		PersistHAL(); // to make sure HAL.DAT is on the system drive
		TInt err;
		TInt tryCount = 0;
		do
			{
			__UHEAP_MARK;

			__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
			err = InitialiseHAL();
			__UHEAP_SETFAIL(RHeap::ENone, 0);

			__UHEAP_MARKEND;
			} while(err == KErrNoMemory);

	 		TheTest(err==KErrNone);
			TheTest.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
		}
/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1710
@SYMTestCaseDesc        OOM Test for Persist HAL attributes
@SYMTestPriority        Medium
@SYMTestActions         Tests for the out of memory conditions
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
LOCAL_C void OOMTestPersistHAL()
		{
		TheTest.Next(_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1710 OOM test for PersistHAL() "));

		TInt err;
		TInt tryCount = 0;
		do
			{
			__UHEAP_MARK;

			__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
			err = PersistHAL();
			__UHEAP_SETFAIL(RHeap::ENone, 0);

			__UHEAP_MARKEND;
			} while(err == KErrNoMemory);

	 		TheTest(err==KErrNone);
			TheTest.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
		}


LOCAL_C void RunTestsL()
		{
		OOMTestInitialiseHAL();
		OOMTestPersistHAL();
		}

GLDEF_C TInt E32Main()
	{

	__UHEAP_MARK;
	TheTest.Title ();
	TheTest.Start (_L ("OOM Tests"));
	CTrapCleanup* cleanup=CTrapCleanup::New();


	TRAPD(err, ::RunTestsL());

	delete cleanup;

	TheTest.End ();
	TheTest.Close ();
	__UHEAP_MARKEND;

	return  err;

	}
