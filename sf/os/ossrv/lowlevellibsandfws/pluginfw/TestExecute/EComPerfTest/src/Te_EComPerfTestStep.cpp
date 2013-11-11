// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Base test step for ECom performance tests
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include "Te_EComPerfTestStep.h"
#include "EcomTestUtils.h"

#ifdef __ECOM_SERVER_PERFORMANCE__
const TInt KOneSecond = 1000000;
#endif

CEComPerfTestBase::CEComPerfTestBase(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

CEComPerfTestBase::~CEComPerfTestBase()
	{
	}

/**
 Set-up ECom performance tests by starting the ECom server and then waiting to allow start-up to complete
*/
TVerdict CEComPerfTestBase::doTestStepPreambleL()
	{
#ifdef __ECOM_SERVER_PERFORMANCE__
	//Open the ecom session which in turn should start the ecom server.
	//Next, wait to make sure that startup is complete before requesting timing data.
	//Finally, get rid of the ecom session.
	REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);
	User::After(KOneSecond*15); //delay for 15 sec to ensure ecom start up has completed
	CleanupStack::PopAndDestroy(&ecomSession);

#endif
	return TestStepResult() ;
	}

TVerdict CEComPerfTestBase::doTestStepPostambleL()
	{
 	return TestStepResult() ;
	}

/**
 Fails the test if the measured time is less than the maximum time
 This test is only enabled on hardware non-DP release builds
 (testing for DP builds was disabled due to inconsistency of the results)
 @param aActualTime The measured time
 @param aMaxTime The maximum time
 */
#if defined(__EPOC32__) && !defined(_DEBUG)
void CEComPerfTestBase::CheckPerformance(TReal aActualTime, TReal aMaxTime, TPtrC aTimingMessage)
	{
	THardwareConfiguration hardware_configuration = EComTestUtils::GetHardwareConfiguration();
    if ( hardware_configuration != EPlatformH4NANDDP )
		{	
		if (aActualTime < aMaxTime)
			{
			INFO_PRINTF4(_L("Performance test for [%S] passed (Measured time [%f] mSecs, Time limit [%f] mSecs\n"), &aTimingMessage, aActualTime, aMaxTime);
			}
		else
			{
			INFO_PRINTF4(_L("Performance test for [%S] FAILED (Measured time [%f] mSecs, Time limit [%f] mSecs\n"), &aTimingMessage, aActualTime, aMaxTime);
			}
		TEST(aActualTime < aMaxTime);		
		}
	else
#else
void CEComPerfTestBase::CheckPerformance(TReal aActualTime, TReal /*aMaxTime*/, TPtrC aTimingMessage)
	{
#endif
		/*Stub implementation used when performance testing is disabled
		Performance testing is not enabled on debug, emulator and DP builds*/
		INFO_PRINTF3(_L("Performance check disabled. Measured time for [%S] is [%f] mSecs\n"), &aTimingMessage, aActualTime);
	}


#ifdef __ECOM_SERVER_PERFORMANCE__

_LIT(KUnknownDesC, "Unknown");
_LIT(KCreateDesC, "Create");
_LIT(KListDesC, "List");
_LIT(KNotifyOnChangeDesC, "Notify On Change");
_LIT(KCancelNotifyOnChangeDesC, "Cancel Notify On Change");
_LIT(KCollectImplementationsDesC, "Collect Implementations");
_LIT(KListExtendedInterfacesDesC, "List extended interfaces");
/**
 Converts a client request type into a string description of the enumeration
 @param aClientRequest The client request to retrieve a string description of
 @return The string description of aClientRequest
 */
TPtrC CEComPerfTestBase::ClientRequestTypeName(TEComClientRequestType aClientRequestType)
	{
	switch (aClientRequestType)
		{
		case EEComCreateRequestType:
			return (TDesC&)KCreateDesC;
		case EEComListRequestType:
			return (TDesC&)KListDesC;
		case EEComNotifyOnChangeRequestType:
			return (TDesC&)KNotifyOnChangeDesC;
		case EEComCancelNotifyOnChangeRequestType:
			return (TDesC&)KCancelNotifyOnChangeDesC;
		case EEComCollectImplementationsRequestType:
			return (TDesC&)KCollectImplementationsDesC;
		case EEComListExtendedInterfacesRequestType:
			return (TDesC&)KListExtendedInterfacesDesC;
		default:
			return (TDesC&)KUnknownDesC;
		}
	}

_LIT(KCriticalStaticDesC, "CriticalStatic");
_LIT(KCriticalDynamicDesC, "CriticalDynamic");
_LIT(KNonCriticalDesC, "NonCritical");
/**
 Converts a startup state into a string description of the enumeration
 @param aStartupStateIdentifier The startup state to retrieve a string description of
 @return The string description of aStartupStateIdentifier
 */
TPtrC CEComPerfTestBase::StartupStateName(TStartupStateIdentifier aStartupStateIdentifier)
	{
	switch (aStartupStateIdentifier)
		{
		case EStartupStateCriticalStatic:
			return (TDesC&)KCriticalStaticDesC;
		case EStartupStateCriticalDynamic:
			return (TDesC&)KCriticalDynamicDesC;
		case EStartupStateNonCritical:
			return (TDesC&)KNonCriticalDesC;
		default:
			return (TDesC&)KUnknownDesC;
		}
	}
#endif // #ifdef __ECOM_SERVER_PERFORMANCE__

#ifndef __ECOM_SERVER_PERFORMANCE__
void CEComPerfTestBase::MacroNotDefinedError()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("***Please define __ECOM_SERVER_PERFORMANCE__ macro to run this test ***"));
	}
#endif // #ifndef __ECOM_SERVER_PERFORMANCE__
