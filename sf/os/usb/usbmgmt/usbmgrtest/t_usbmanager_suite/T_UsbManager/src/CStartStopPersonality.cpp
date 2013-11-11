/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/
#include <test/testexecutelog.h>
#include "CUsbTestStepBase.h"
#include "CStartStopPersonality.h"
	
//////////////////////////////////
//    CStartStopPersonality1    //
//////////////////////////////////

CStartStopPersonality1::CStartStopPersonality1()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KStartStopPersonality1);
	iDoNotConnect = ETrue;
	}

/*
 This test case uses 3 sessions to the USB manager to call TryStart and TryStop in
 different states of the server.
 For TryStart the first 2 sessions use the same personality id and the third one uses
 a different personality id.
*/
TVerdict CStartStopPersonality1::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	//Read personality from ini file
	TInt personality1 = 0;
	TInt personality2 = 0;
	_LIT(KPersonality1, "personality1"); 
	if (!GetIntFromConfig(ConfigSection(), KPersonality1, personality1))
		{
		INFO_PRINTF1(_L("Can't get personality1 id from config file"));
		return TestStepResult();
		}
	_LIT(KPersonality2, "personality2"); 
	if (!GetIntFromConfig(ConfigSection(), KPersonality2, personality1))
		{
		INFO_PRINTF1(_L("Can't get personality2 id from config file"));
		return TestStepResult();
		}
	
	//Open sessions;
	RUsb sess1, sess2, sess3;

	TInt res;
	res = sess1.Connect();
	if (res != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting session 1 to USB service: %d"), res);
		return TestStepResult();
		}
	res = sess2.Connect();
	if (res != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting session 2 to USB service: %d"), res);
		return TestStepResult();
		}
	res = sess3.Connect();
	if (res != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting session 3 to USB service: %d"), res);
		return TestStepResult();
		}

	//Initial State: Idle
	//Function: TryStart
	INFO_PRINTF1(_L("Initial State: Idle, Function: TryStart"));
	SetIdle(sess1);
	if (CheckState(sess1, EUsbServiceIdle) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetIdle(sess1);
		if (CheckState(sess1, EUsbServiceIdle) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStart(sess1, sess2, sess3, KErrNone, KErrNone, KErrAbort, personality1, personality2, EUsbServiceStarted) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Function: TryStop
	INFO_PRINTF1(_L("Initial State: Idle, Function: TryStop"));
	SetIdle(sess1);
	if (CheckState(sess1, EUsbServiceIdle) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetIdle(sess1);
		if (CheckState(sess1, EUsbServiceIdle) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStop(sess1, sess2, sess3, KErrNone, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Function: TryStart
	INFO_PRINTF1(_L("Initial State: Starting, Function: TryStart"));
	SetStarting(sess1);
	if (CheckState(sess1, EUsbServiceStarting) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetStarting(sess1);
		if (CheckState(sess1, EUsbServiceStarting) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStart(sess1, sess2, sess3, KErrNone, KErrNone, KErrAbort, personality1, personality2, EUsbServiceStarted) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Function: TryStop
	INFO_PRINTF1(_L("Initial State: Starting, Function: TryStop"));
	SetStarting(sess1);
	if (CheckState(sess1, EUsbServiceStarting) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetStarting(sess1);
		if (CheckState(sess1, EUsbServiceStarting) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStop(sess1, sess2, sess3, KErrServerBusy, KErrServerBusy, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}

	//Initial State: Stopping
	//Function: TryStart
	INFO_PRINTF1(_L("Initial State: Stopping, Function: TryStart"));
	SetStopping(sess1);
	if (CheckState(sess1, EUsbServiceStopping) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetStopping(sess1);
		if (CheckState(sess1, EUsbServiceStopping) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStart(sess1, sess2, sess3, KErrServerBusy, KErrServerBusy, KErrServerBusy, personality1, personality2, EUsbServiceStopping) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Function: TryStop
	INFO_PRINTF1(_L("Initial State: Stopping, Function: TryStop"));
	SetStopping(sess1);
	if (CheckState(sess1, EUsbServiceStopping) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetStopping(sess1);
		if (CheckState(sess1, EUsbServiceStopping) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStop(sess1, sess2, sess3, KErrNone, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Function: TryStart
	//Assumes that the personality we are trying to start in sessions 1 and 2
	//is the same as the current personality.
	INFO_PRINTF1(_L("Initial State: Started, Function: TryStart"));
	SetStarted(sess1);
	if (CheckState(sess1, EUsbServiceStarted) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetStarted(sess1);
		if (CheckState(sess1, EUsbServiceStarted) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStart(sess1, sess2, sess3, KErrNone, KErrNone, KErrAbort, personality1, personality2, EUsbServiceStarted) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Function: TryStop
	INFO_PRINTF1(_L("Initial State: Started, Function: TryStop"));
	SetStarted(sess1);
	if (CheckState(sess1, EUsbServiceStarted) != KErrNone)
		{
		INFO_PRINTF1(_L("Trying again"));
		SetStarted(sess1);
		if (CheckState(sess1, EUsbServiceStarted) != KErrNone)
			{
			return TestStepResult();
			}
		}
	if (TryStop(sess1, sess2, sess3, KErrNone, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	
	sess1.Close();
	sess2.Close();
	sess3.Close();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CStartStopPersonality1::TryStart(RUsb aSess1, RUsb aSess2, RUsb aSess3, TInt aRet1, TInt aRet2, TInt aRet3, TInt aPersonality1, TInt aPersonality2, TUsbServiceState aState)
	{
	TRequestStatus stat1, stat2, stat3;
	aSess1.TryStart(aPersonality1, stat1);
	aSess2.TryStart(aPersonality1, stat2);
	aSess3.TryStart(aPersonality2, stat3);
	User::WaitForRequest(stat1);
	User::WaitForRequest(stat2);
	User::WaitForRequest(stat3);
	if (stat1 != aRet1)
		{
		INFO_PRINTF2(_L("Session 1 TryStart returned unexpected error code: %d"), stat1.Int());
		return KErrGeneral;
		}
	if (stat2 != aRet2)
		{
		INFO_PRINTF2(_L("Session 2 TryStart returned unexpected error code: %d"), stat2.Int());
		return KErrGeneral;
		}
	if (stat3 != aRet3)
		{
		INFO_PRINTF2(_L("Session 3 TryStart returned unexpected error code: %d"), stat3.Int());
		return KErrGeneral;
		}
	
	//Make sure the correct personality is loaded and the server is in the expected state
	TInt err;
	TInt currPersonality;
	err = aSess1.GetCurrentPersonalityId(currPersonality);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to get current personality id: %d"), err);
		return KErrGeneral;
		}
	else if (currPersonality != aPersonality1)
		{
		INFO_PRINTF3(_L("Current personality id does not match expected value: %d != %d"), currPersonality, aPersonality1);
		return KErrGeneral;
		}
	
	return CheckState(aSess1, aState);
	}

TInt CStartStopPersonality1::TryStop(RUsb aSess1, RUsb aSess2, RUsb aSess3, TInt aRet1, TInt aRet2, TInt aRet3)
	{
	TRequestStatus stat1, stat2, stat3;
	aSess1.TryStop(stat1);
	aSess2.TryStop(stat2);
	aSess3.TryStop(stat3);
	User::WaitForRequest(stat1);
	User::WaitForRequest(stat2);
	User::WaitForRequest(stat3);
	if (stat1 != aRet1)
		{
		INFO_PRINTF2(_L("Session 1 TryStop returned unexpected error code: %d"), stat1.Int());
		return KErrGeneral;
		}
	if (stat2 != aRet2)
		{
		INFO_PRINTF2(_L("Session 2 TryStop returned unexpected error code: %d"), stat2.Int());
		return KErrGeneral;
		}
	if (stat3 != aRet3)
		{
		INFO_PRINTF2(_L("Session 3 TryStop returned unexpected error code: %d"), stat3.Int());
		return KErrGeneral;
		}
	return KErrNone;
	}

//////////////////////////////////
//    CStartStopPersonality2    //
//////////////////////////////////

CStartStopPersonality2::CStartStopPersonality2()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KStartStopPersonality2);
	iDoNotConnect = ETrue;
	}

TVerdict CStartStopPersonality2::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	//Read personality from ini file
	TInt personality;
	_LIT(KPersonality, "personality1"); 
	if (!GetIntFromConfig(ConfigSection(), KPersonality, personality))
		{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		return TestStepResult();
		}
	
	//Open sessions;
	RUsb sess1, sess2;

	TInt res;
	res = sess1.Connect();
	if (res != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting session 1 to USB service: %d"), res);
		return TestStepResult();
		}
	res = sess2.Connect();
	if (res != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting session 2 to USB service: %d"), res);
		return TestStepResult();
		}
	
	TRequestStatus stat1, stat2;
	
	//Initial State: Idle
	//Functions: TryStart/Start
	INFO_PRINTF1(_L("Initial State: Idle, Functions: TryStart/Start"));
	SetIdle(sess1);
	if (CheckState(sess1, EUsbServiceIdle) != KErrNone)
		{
		return TestStepResult();
		}
	sess1.TryStart(personality, stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: Start/TryStart
	INFO_PRINTF1(_L("Initial State: Idle, Functions: Start/TryStart"));
	SetIdle(sess1);
	sess1.Start(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: TryStart/Stop
	INFO_PRINTF1(_L("Initial State: Idle, Functions: TryStart/Stop"));
	SetIdle(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrUsbServiceStopped, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: Stop/TryStart
	INFO_PRINTF1(_L("Initial State: Idle, Functions: Stop/TryStart"));
	SetIdle(sess1);
	sess1.Stop(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: TryStop/Start
	INFO_PRINTF1(_L("Initial State: Idle, Functions: TryStop/Start"));
	SetIdle(sess1);
	sess1.TryStop(stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: Start/TryStop
	INFO_PRINTF1(_L("Initial State: Idle, Functions: Start/Stop"));
	SetIdle(sess1);
	sess1.Start(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: TryStop/Stop
	INFO_PRINTF1(_L("Initial State: Idle, Functions: TryStop/Stop"));
	SetIdle(sess1);
	sess1.TryStop(stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Idle
	//Functions: Stop/TryStop
	INFO_PRINTF1(_L("Initial State: Idle, Functions: Stop/TryStop"));
	SetIdle(sess1);
	sess1.Stop(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: TryStart/Start
	INFO_PRINTF1(_L("Initial State: Starting, Functions: TryStart/Start"));
	SetIdle(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: Start/TryStart
	INFO_PRINTF1(_L("Initial State: Starting, Functions: Start/TryStart"));
	SetStarting(sess1);
	sess1.Start(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: TryStart/Stop
	INFO_PRINTF1(_L("Initial State: Starting, Functions: TryStart/Stop"));
	SetStarting(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrUsbServiceStopped, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: Stop/TryStart
	INFO_PRINTF1(_L("Initial State: Starting, Functions: Stop/TryStart"));
	SetStarting(sess1);
	sess1.Stop(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: TryStop/Start
	INFO_PRINTF1(_L("Initial State: Starting, Functions: TryStop/Start"));
	SetStarting(sess1);
	sess1.TryStop(stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrServerBusy, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: Start/TryStop
	INFO_PRINTF1(_L("Initial State: Starting, Functions: Start/TryStop"));
	SetStarting(sess1);
	sess1.Start(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: TryStop/Stop
	INFO_PRINTF1(_L("Initial State: Starting, Functions: TryStop/Stop"));
	SetStarting(sess1);
	sess1.TryStop(stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrServerBusy, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Starting
	//Functions: Stop/TryStop
	INFO_PRINTF1(_L("Initial State: Starting, Functions: Stop/TryStop"));
	SetStarting(sess1);
	sess1.Stop(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: TryStart/Start
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: TryStart/Start"));
	SetStopping(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrServerBusy, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: Start/TryStart
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: Start/TryStart"));
	SetStopping(sess1);
	sess1.Start(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: TryStart/Stop
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: TryStart/Stop"));
	SetStopping(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrServerBusy, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: Stop/TryStart
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: Stop/TryStart"));
	SetStopping(sess1);
	sess1.Stop(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: TryStop/Start
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: TryStop/Start"));
	SetStopping(sess1);
	sess1.TryStop(stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrUsbServiceStarted, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: Start/TryStop
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: Start/TryStop"));
	SetStopping(sess1);
	sess1.Start(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: TryStop/Stop
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: TryStop/Stop"));
	SetStopping(sess1);
	sess1.TryStop(stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Stopping
	//Functions: Stop/TryStop
	INFO_PRINTF1(_L("Initial State: Stopping, Functions: Stop/TryStop"));
	SetStopping(sess1);
	sess1.Stop(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: TryStart/Start
	INFO_PRINTF1(_L("Initial State: Started, Functions: TryStart/Start"));
	SetIdle(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: Start/TryStart
	//Assumes that the personality we are trying to start in sessions 1 and 2
	//is the same as the current personality.
	INFO_PRINTF1(_L("Initial State: Started, Functions: Start/TryStart"));
	SetStarted(sess1);
	sess1.Start(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: TryStart/Stop
	//Assumes that the personality we are trying to start in session 1
	//is the same as the current personality.
	INFO_PRINTF1(_L("Initial State: Started, Functions: TryStart/Stop"));
	SetStarted(sess1);
	sess1.TryStart(personality, stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: Stop/TryStart
	INFO_PRINTF1(_L("Initial State: Started, Functions: Stop/TryStart"));
	SetStarted(sess1);
	sess1.Stop(stat1);
	sess2.TryStart(personality, stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrServerBusy) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: TryStop/Start
	INFO_PRINTF1(_L("Initial State: Started, Functions: TryStop/Start"));
	SetStarted(sess1);
	sess1.TryStop(stat1);
	sess2.Start(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrUsbServiceStarted, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: Start/TryStop
	INFO_PRINTF1(_L("Initial State: Started, Functions: Start/TryStop"));
	SetStarted(sess1);
	sess1.Start(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: TryStop/Stop
	INFO_PRINTF1(_L("Initial State: Started, Functions: TryStop/Stop"));
	SetStarted(sess1);
	sess1.TryStop(stat1);
	sess2.Stop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	//Initial State: Started
	//Functions: Stop/TryStop
	INFO_PRINTF1(_L("Initial State: Started, Functions: Stop/TryStop"));
	SetStarted(sess1);
	sess1.Stop(stat1);
	sess2.TryStop(stat2);
	if (CheckReturnCodes(stat1, stat2, KErrNone, KErrNone) != KErrNone)
		{
		return TestStepResult();
		}
	
	sess1.Close();
	sess2.Close();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CStartStopPersonality2::CheckReturnCodes(TRequestStatus& aStat1, TRequestStatus& aStat2, TInt aRet1, TInt aRet2)
	{
	User::WaitForRequest(aStat1);
	User::WaitForRequest(aStat2);
	if (aStat1 != aRet1)
		{
		INFO_PRINTF2(_L("Session 1 returned unexpected error code: %d"), aStat1.Int());
		return KErrGeneral;
		}
	if (aStat2 != aRet2)
		{
		INFO_PRINTF2(_L("Session 2 returned unexpected error code: %d"), aStat2.Int());
		return KErrGeneral;
		}
	return KErrNone;
	}

