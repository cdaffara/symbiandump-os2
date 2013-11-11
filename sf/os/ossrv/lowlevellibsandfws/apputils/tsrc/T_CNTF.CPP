// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by BLB, November 1996
// test code for CEnvironmentChangeNotifier
// 
//

#include <bacntf.h>
#include <e32test.h>

LOCAL_D RTest test(_L("T_CNTF"));

TInt TheStatus=0;
TBool WasCalled=EFalse;
CEnvironmentChangeNotifier* TheNotifier=NULL;

GLDEF_C TInt MyCallback(TAny*)
	{
	TInt stat=TheNotifier->Change()&~(EChangesPowerStatus|EChangesThreadDeath);
	if (stat)
		{	// ignore just changes in power status etc
		WasCalled=ETrue;
		TheStatus=stat;
		CActiveScheduler::Stop();
		}
	return(KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0406
@SYMTestCaseDesc        TLocale class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Tests for environment setting changes.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testLocale()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0406 Changing locale "));
	TLocale locale;
	test(!WasCalled);
	locale.SetCountryCode(locale.CountryCode()+1);
	locale.Set();
	CActiveScheduler::Start();
	test(WasCalled);
	test(TheStatus&EChangesLocale);
	test((TheStatus&EChangesMidnightCrossover)==0);
	WasCalled=EFalse;
	TheStatus=0;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0407
@SYMTestCaseDesc        Tests for Locale changes
@SYMTestPriority        Medium
@SYMTestActions         Tests for change in systems time.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testMidnight()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0407 Midnight crossover "));
	TTime timeNow;
	timeNow.HomeTime();
	TDateTime dateTime=timeNow.DateTime();
	dateTime.SetHour(23);
	dateTime.SetSecond(58);
	dateTime.SetMinute(59);
	timeNow=dateTime;
	User::SetHomeTime(timeNow);
	test(!WasCalled);
	CActiveScheduler::Start();
	test(WasCalled);
	test(TheStatus&EChangesSystemTime);
	test((TheStatus&EChangesLocale)==0);
	if ((TheStatus&EChangesMidnightCrossover)==0)
		{
		WasCalled=EFalse;
		TheStatus=0;
		CActiveScheduler::Start();
		test(WasCalled);
		test(TheStatus&EChangesMidnightCrossover);
		test((TheStatus&EChangesLocale)==0);
		}
	WasCalled=EFalse;
	TheStatus=0;
	}

// Following testLocale re-set the country code value back to the default
// so that any more tests added in future will be starting from the OS default state
LOCAL_C void resetLocale()
	{
	test.Next(_L("Resetting locale before next test"));
	TLocale locale;
	test(!WasCalled);
	locale.SetCountryCode(locale.CountryCode()-1);
	locale.Set();
	CActiveScheduler::Start();
	test(WasCalled);
	test(TheStatus&EChangesLocale);
	WasCalled=EFalse;
	TheStatus=0;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	CActiveScheduler* shed=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(shed);
	test.Title();
	test.Start(_L("Testing change notifier "));
	TCallBack callback(MyCallback);
	TheNotifier=CEnvironmentChangeNotifier::NewL(0,callback);
	TheNotifier->Start();

	// Check notifier is working
	CActiveScheduler::Start();
	test(WasCalled);
	test(TheStatus!=0);
	WasCalled=EFalse;
	TheStatus=0;

	// Test - change country code
	testLocale();

	// Test - check midnight rollover
	testMidnight();

	// Reset country code to default state
	resetLocale();

	TheNotifier->Cancel();
	delete(CActiveScheduler::Current());
	delete(TheNotifier);
	test.End();
	test.Close();
	delete cleanup;
	__UHEAP_MARKEND;
	return(KErrNone);
    }
