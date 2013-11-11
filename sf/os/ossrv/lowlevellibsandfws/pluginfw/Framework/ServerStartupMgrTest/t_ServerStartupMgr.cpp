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
//

#include <e32base.h>
#include <e32test.h>
#include <e32debug.h>
#include <f32file.h>
#include <startupdomaindefs.h>
#include <ecom/ecompanics.h>

#include "EComServer.h"
#include "TestHarnessDomainMember.h"
#include "StartupStateObserverObject.h"
#include "ServerStartupManager.h"
#include "EcomTestUtils.h"
#include "EcomTestIniFileUtils.h"
#include "EcomTestCompTestabilityUtils.h"

static RFs TheFs;
static RTest TheTest(_L("T_ServerStartupMgr"));

//
//
//Test macroes and functions
//
//

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}

static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//
//
//TServerStartupManager_StateAccessor class definition and methods
//
//
/**
@internalTechnology

Allows access to the protected and private data and operations
of CServerStartupMgr
*/
class TServerStartupManager_StateAccessor
	{
public:
	static TStartupStateIdentifier GetKnownStartupState(CServerStartupMgr& aServerStartupMgr, TDmDomainState aStartupState);
	static void SetKnownStartupState(CServerStartupMgr& aServerStartupMgr, TStartupStateIdentifier aState);
	static void TestStateAwareObjectsForState(CServerStartupMgr& aServerStartupMgr, TInt32 aState);
	static void SetStateAwareObjectsState(CServerStartupMgr& aServerStartupMgr, TInt32 aState);
	static void SetStateL(CServerStartupMgr& aServerStartupMgr, TInt aState);
	static void RunL(CServerStartupMgr& aServerStartupMgr);
	static TInt RunError(CServerStartupMgr& aServerStartupMgr, TInt aError);
	};

void TServerStartupManager_StateAccessor::TestStateAwareObjectsForState
		(CServerStartupMgr& aServerStartupMgr, TInt32 aState)
	{
	int i;
	const RPointerArray<MStartupStateObserver> observerList =
		aServerStartupMgr.iObserverList;

	for(i=0; i<observerList.Count(); i++)
		{
		CStartupStateObserver* observerObject =
			static_cast<CStartupStateObserver*>(observerList[i]);

		TEST2((TInt32)observerObject->GetState(), aState);
		}
	}

void TServerStartupManager_StateAccessor::SetStateAwareObjectsState
		(CServerStartupMgr& aServerStartupMgr, TInt32 aState)
	{
	int i;
	const RPointerArray<MStartupStateObserver> observerList =
		aServerStartupMgr.iObserverList;

	for(i=0; i<observerList.Count(); i++)
		{
		CStartupStateObserver* observerObject =
			static_cast<CStartupStateObserver*>(observerList[i]);

		observerObject->SetState((CStartupStateObserver::TSsoState)aState);
		}
	}

TStartupStateIdentifier TServerStartupManager_StateAccessor::GetKnownStartupState
		(CServerStartupMgr& aServerStartupMgr, TDmDomainState aStartupState)
	{
	return aServerStartupMgr.GetKnownStartupState(aStartupState);
	}

void TServerStartupManager_StateAccessor::SetKnownStartupState(CServerStartupMgr& aServerStartupMgr, TStartupStateIdentifier aState)
	{
	aServerStartupMgr.iCurrentStartupState = aState;
	}

void TServerStartupManager_StateAccessor::SetStateL(CServerStartupMgr& aServerStartupMgr, TInt aState)
	{
	aServerStartupMgr.ChangeStartupStateL(aState);
	}

void TServerStartupManager_StateAccessor::RunL(CServerStartupMgr& aServerStartupMgr)
	{
	aServerStartupMgr.RunL();
	}

TInt TServerStartupManager_StateAccessor::RunError(CServerStartupMgr& aServerStartupMgr,TInt aError)
	{
	return aServerStartupMgr.RunError(aError);
	}

//
//
//Helper functions
//
//

/**
*
* Function is used for each state change of InitialiseL() testing.
* It checks whether:
* 	    - For each state RequestTransitionNotification() is called
*		- For EStartupStateUndefined or EStartupStateNonCritical state
*		  Requestion of notification is cancelled .
* 	    - Each MStartupServerStateObserver object is set to their
* 	      corresponding state
* @param aServerStartupMgr the server startup manager being tested
* @param aNewState the domain state to be tested
* @param aObserverObjCurrentState the current state of all the observer objects
* registered to aServerStartupMgr
* @param aExpectedRequestTransitionNotification expected result from
* IsTransitionNotificationRequestedL() call to check whether RequestTransitionNotification()
* was called
* @param aExpectedCancelRequestNotification expected result from
* IsCancelRequestNotification() call to check whether Request of Notification
* was cancelled.
* @param aObserverObjExpectedState the expected new state for all of the MStartupServerStateObserver
* @param aExpectedStartupState the expected new state for the CServerStartupManager
* after RunL is called.
*/
void CheckForInitialiseStateChangeL(CServerStartupMgr& aServerStartupMgr,
							  TInt aNewState,
							  CStartupStateObserver::TSsoState aObserverObjCurrentState,
							  TBool aExpectedRequestTransitionNotification,
							  TBool aExpectedCancelRequestNotification,
  							  CStartupStateObserver::TSsoState aObserverObjExpectedState,
  							  TStartupStateIdentifier aExpectedStartupState,
  							  TBool aSsaEnabled)
	{
	//reset the server startup manager flags
	aServerStartupMgr.ResetRequestTransitionNotificationL();

	//set the new/tested state for server startup manager
	TServerStartupManager_StateAccessor::SetKnownStartupState
		(aServerStartupMgr, EStartupStateUndefined);

	//set the new/tested state for the domain
	TServerStartupManager_StateAccessor::SetStateL
		(aServerStartupMgr, aNewState);

	//set the current state for all of the MStartupServerStateObserver objects
	TServerStartupManager_StateAccessor::SetStateAwareObjectsState
		(aServerStartupMgr, aObserverObjCurrentState);

	//call InitialiseL()
	aServerStartupMgr.InitialiseL(aSsaEnabled);

	//test if RequestTransitionNotification() is called
	TEST2(aServerStartupMgr.IsTransitionNotificationRequestedL(), aExpectedRequestTransitionNotification);

	//test if Request of Notification is cancelled
	TEST2(aServerStartupMgr.IsRequestNotificationCancelledL(), aExpectedCancelRequestNotification);

	//test if MStartupStateObserver objects are set to expected state
	TServerStartupManager_StateAccessor::TestStateAwareObjectsForState
		(aServerStartupMgr, aObserverObjExpectedState);

	//test if CServerStartupManager is set to expected state
	TEST2(aServerStartupMgr.CurrentStartupState(), aExpectedStartupState);
	}


/**
*
* Function is used for each state change of RunL() testing.
* It checks whether:
* 	    - AcknowledgeLastStateL() is called for each state.
* 	    - RequestTransitionNotification() is called for all states.
*		- For EStartupStateUndefined or EStartupStateNonCritical state
*		  Requestion of notification is cancelled.
* 	    - Each MStartupServerStateObserver object is set to their
* 	      corresponding state
* @param aServerStartupMgr the server startup manager being tested
* @param aNewState the domain state to be tested
* @param aObserverObjCurrentState the current state of all the observer objects
* registered to aServerStartupMgr
* @param aExpectedRequestTransitionNotification expected result from
* IsTransitionNotificationRequestedL() call to check whether RequestTransitionNotification()
* was called
* @param aExpectedCancelRequestNotification expected result from
* IsCancelRequestNotification() call to check whether Request of Notification
* was cancelled.
* @param aExpectedAcknowledgeLastState expected result from IsLastStateAcknowledged()
* call to check whether AcknowledgeLastStateL() was called
* @param aObserverObjExpectedState the expected new state for all of the MStartupServerStateObserver
* after RunL is called.
* @param aExpectedStartupState the expected new state for the CServerStartupManager
* after RunL is called.
*/
void CheckForRunLStateChangeL(CServerStartupMgr& aServerStartupMgr,
							  TInt aNewState,
							  CStartupStateObserver::TSsoState aObserverObjCurrentState,
							  TBool aExpectedRequestTransitionNotification,
							  TBool aExpectedCancelRequestNotification,
							  TBool aExpectedAcknowledgeLastState,
  							  CStartupStateObserver::TSsoState aObserverObjExpectedState,
  							  TStartupStateIdentifier aExpectedStartupState)
	{
	//reset the server startup manager flags
	aServerStartupMgr.ResetLastStateAcknowledgedL();
	aServerStartupMgr.ResetRequestTransitionNotificationL();

	//set the current state for CDmDomain
	TServerStartupManager_StateAccessor::SetStateL
		(aServerStartupMgr, aNewState);

	//set the current state for all of the MStartupServerStateObserver objects
	TServerStartupManager_StateAccessor::SetStateAwareObjectsState
		(aServerStartupMgr, aObserverObjCurrentState);

	//call RunL()
	TRAPD(err, TServerStartupManager_StateAccessor::
				RunL(aServerStartupMgr));
	if(aNewState == EStartupStateUndefined)
		{
		TEST2(err, KErrBadHandle);
		}
	else
		{
		TEST2(err, KErrNone);
		}

	//test if RequestTransitionNotification() is called
	TEST2(aServerStartupMgr.IsTransitionNotificationRequestedL(), aExpectedRequestTransitionNotification);

	//test if Request of Notification is cancelled
	TEST2(aServerStartupMgr.IsRequestNotificationCancelledL(), aExpectedCancelRequestNotification);

	//test if AcknowledgeLastStateL() is called
	TEST2(aServerStartupMgr.IsLastStateAcknowledgedL(), aExpectedAcknowledgeLastState);

	//test if MStartupStateObserver objects are set to expected state
	TServerStartupManager_StateAccessor::TestStateAwareObjectsForState
		(aServerStartupMgr, aObserverObjExpectedState);

	//test if CServerStartupManager is set to expected state
	TEST2(aServerStartupMgr.CurrentStartupState(), aExpectedStartupState);
	}

//
//
//Tests
//
//

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0178
@SYMTestCaseDesc 	Check that the CServerStartupMgr::GetKnownStartupState() works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Set knownStartupState to EStartupStateUndefined, EStartupStateCriticalStatic,
EStartupStateCriticalDynamic, EStartupStateNonCritical. Call GetKnownStartupState() to see if
correct known startup state is returned.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
void GetKnownStartupState_TestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0178 "));
	TUint32 i;
	static const TUint32 KTestCount = 11;
	TInt32 testStates[] = { EStartupStateUndefined, EStartupStateUndefined+1,
							EStartupStateCriticalStatic-1, EStartupStateCriticalStatic, EStartupStateCriticalStatic+1,
							EStartupStateCriticalDynamic-1, EStartupStateCriticalDynamic, EStartupStateCriticalDynamic+1,
							EStartupStateNonCritical-1, EStartupStateNonCritical, EStartupStateNonCritical+1};
	//Test results
	TInt32 resultStates[] = {EStartupStateUndefined, EStartupStateUndefined,
							 EStartupStateUndefined, EStartupStateCriticalStatic, EStartupStateCriticalStatic,
							 EStartupStateCriticalStatic, EStartupStateCriticalDynamic, EStartupStateCriticalDynamic,
							 EStartupStateCriticalDynamic, EStartupStateNonCritical, EStartupStateNonCritical};

	EnableSsa(TheTest, TheFs);

	CServerStartupMgr* serverStartupMgr =
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KDmIdRoot, TheFs);
	CleanupStack::PushL(serverStartupMgr);

	//check for all the test states
	for(i=0; i<KTestCount; i++)
		{
		TEST2(TServerStartupManager_StateAccessor::GetKnownStartupState(*serverStartupMgr, testStates[i]),
		      resultStates[i]);
		}

	CleanupStack::Pop(serverStartupMgr);
	delete serverStartupMgr;

	ResetSsa(TheTest, TheFs);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0179
@SYMTestCaseDesc 	Check that the CServerStartupMgr::InitialiseL() works for the non SSA case correctly.
@SYMTestPriority 	High
@SYMTestActions  	Set SSA to false. Then check that all the MStartupStateObserver
objects registered to server startup manager has switched to the correct known start-up state
as a result of InitialiseL()
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
void InitialiseL_NonSsa_TestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0179 "));
	TestEnableDisableSsaL(TheTest, TheFs);

	DisableSsa(TheTest, TheFs);

	//Create some MStartupServerStateObserver objects
	CStartupStateObserver* dummyObject1 = CStartupStateObserver::NewLC();
	CStartupStateObserver* dummyObject2 = CStartupStateObserver::NewLC();
	CStartupStateObserver* dummyObject3 = CStartupStateObserver::NewLC();

	CServerStartupMgr* serverStartupMgr =
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KDmIdRoot, TheFs);
	CleanupStack::PushL(serverStartupMgr);

	//Register MStartupServerStateObserver objects
	serverStartupMgr->RegisterObserverL(dummyObject1);
	serverStartupMgr->RegisterObserverL(dummyObject2);
	serverStartupMgr->RegisterObserverL(dummyObject3);

	TServerStartupManager_StateAccessor::SetKnownStartupState
		(*serverStartupMgr, EStartupStateUndefined);

	TServerStartupManager_StateAccessor::SetStateL
		(*serverStartupMgr, EStartupStateUndefined);

	serverStartupMgr->InitialiseL(EFalse);

	//Now that CDmDomain::ConstructL() is called check that each
	//MStartupServerStateObserver object is set to
	//TSsoState::SsoAllPluginsInternalized
	TServerStartupManager_StateAccessor::TestStateAwareObjectsForState
		(*serverStartupMgr, CStartupStateObserver::SsoAllPluginsInternalized);

	TEST2(serverStartupMgr->CurrentStartupState(), EStartupStateNonCritical);

	CleanupStack::Pop(serverStartupMgr);
	delete serverStartupMgr;

	CleanupStack::PopAndDestroy(dummyObject3);
	CleanupStack::PopAndDestroy(dummyObject2);
	CleanupStack::PopAndDestroy(dummyObject1);

	ResetSsa(TheTest, TheFs);
	}


/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0180
@SYMTestCaseDesc 	Check that the CServerStartupMgr::InitialiseL() works for the SSA case correctly.
@SYMTestPriority 	High
@SYMTestActions  	Set SSA to true. Then check that all the MStartupStateObserver
objects registered to server startup manager has switched to the correct known start-up
state as a result of InitialiseL(). Next check that correct actions have been taken
when known startup state for server startup manager is set to all possible values
and after InitialiseL() has been called.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
void InitialiseL_Ssa_TestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0180 "));
	TestEnableDisableSsaL(TheTest, TheFs);

	EnableSsa(TheTest, TheFs);

	//Create some MStartupServerStateObserver objects
	CStartupStateObserver* dummyObject1 = CStartupStateObserver::NewLC();
	CStartupStateObserver* dummyObject2 = CStartupStateObserver::NewLC();
	CStartupStateObserver* dummyObject3 = CStartupStateObserver::NewLC();

	CServerStartupMgr* serverStartupMgr =
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KDmIdRoot, TheFs);
	CleanupStack::PushL(serverStartupMgr);

	//Register MStartupServerStateObserver objects
	serverStartupMgr->RegisterObserverL(dummyObject1);
	serverStartupMgr->RegisterObserverL(dummyObject2);
	serverStartupMgr->RegisterObserverL(dummyObject3);

	//initialise the domain
	//set the state for CDmDomain
	TServerStartupManager_StateAccessor::SetStateL
		(*serverStartupMgr, EStartupStateCriticalStatic);

	serverStartupMgr->InitialiseL(ETrue);

	//Now that CDmDomain::ConstructL() is called check that each
	//MStartupServerStateObserver object is set to
	//TSsoState::SsoCriticalPlugingsInternalized
	TServerStartupManager_StateAccessor::TestStateAwareObjectsForState
		(*serverStartupMgr, CStartupStateObserver::SsoCriticalPlugingsInternalized);

	// Following tests ensure that
	//    - For each state RequestTransitionNotification() is called
	//	  - For EStartupStateUndefined or EStartupStateNonCritical
	//	  the request of notification is cancelled.
	//    - Each MStartupServerStateObserver object is set to their
	//      corresponding state

	//Test for EStartupStateUndefined
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateUndefined,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							ETrue,
							CStartupStateObserver::SsoAllPluginsInternalized,
							EStartupStateNonCritical,ETrue);

	//Test for a user defined state
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateCriticalStatic-1,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							EFalse,
							CStartupStateObserver::SsoNoPluginsInternalized,
							EStartupStateUndefined,ETrue);

	//Test for EStartupStateCriticalStatic
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateCriticalStatic,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							EFalse,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalStatic,ETrue);

	//Test for a user defined state
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateCriticalDynamic-1,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							EFalse,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalStatic,ETrue);

	//Test for EStartupStateCriticalDynamic
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateCriticalDynamic,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							EFalse,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalDynamic,ETrue);

	//Test for a user defined state
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateNonCritical-1,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							EFalse,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalDynamic,ETrue);

	//Test for EStartupStateNonCritical
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateNonCritical,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							ETrue,
							CStartupStateObserver::SsoAllPluginsInternalized,
							EStartupStateNonCritical,ETrue);

	//Test for a user defined state
	CheckForInitialiseStateChangeL(*serverStartupMgr, EStartupStateNonCritical+1,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue,
							ETrue,
							CStartupStateObserver::SsoAllPluginsInternalized,
							EStartupStateNonCritical,ETrue);

	CleanupStack::Pop(serverStartupMgr);
	delete serverStartupMgr;

	CleanupStack::PopAndDestroy(dummyObject3);
	CleanupStack::PopAndDestroy(dummyObject2);
	CleanupStack::PopAndDestroy(dummyObject1);

	ResetSsa(TheTest, TheFs);
	}


/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0181
@SYMTestCaseDesc 	Check that the CServerStartupMgr::RunL() works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Set SSA to true. Then check that correct actions have
been taken when known startup state for server startup manager is set to all
possible values and after InitialiseL() has been called.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
void RunL_TestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0181 "));
	EnableSsa(TheTest, TheFs);

	//Create some MStartupServerStateObserver objects
	CStartupStateObserver* dummyObject1 = CStartupStateObserver::NewLC();
	CStartupStateObserver* dummyObject2 = CStartupStateObserver::NewLC();
	CStartupStateObserver* dummyObject3 = CStartupStateObserver::NewLC();

	CServerStartupMgr* serverStartupMgr =
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KDmIdRoot, TheFs);
	CleanupStack::PushL(serverStartupMgr);

	//Register MStartupServerStateObserver objects
	serverStartupMgr->RegisterObserverL(dummyObject1);
	serverStartupMgr->RegisterObserverL(dummyObject2);
	serverStartupMgr->RegisterObserverL(dummyObject3);

	serverStartupMgr->InitialiseL(ETrue);

	//Following tests ensure that:
	//    - AcknowledgeLastStateL() is called for each state.
	//    - RequestTransitionNotification() is called for all states
	//	  - For EStartupStateUndefined or EStartupStateNonCritical
	//	  the request of notification is cancelled.
	//    - Each MStartupServerStateObserver object is set to their
	//      corresponding state
	//Test for EStartupStateUndefined
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateUndefined,
							CStartupStateObserver::SsoUndefined,
							ETrue, ETrue, ETrue,
							CStartupStateObserver::SsoUndefined,
							EStartupStateUndefined);

	//Test for a user defined state
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateCriticalStatic-1,
							CStartupStateObserver::SsoUndefined,
							ETrue, EFalse, ETrue,
							CStartupStateObserver::SsoUndefined,
							EStartupStateUndefined);

	//Test for EStartupStateCriticalStatic
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateCriticalStatic,
							CStartupStateObserver::SsoNoPluginsInternalized,
							ETrue, EFalse, ETrue,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalStatic);

	//Test for EStartupStateCriticalStatic
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateCriticalDynamic-1,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							ETrue, EFalse,ETrue,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalStatic);

	//Test for EStartupStateCriticalDynamic
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateCriticalDynamic,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							ETrue, EFalse, ETrue,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalDynamic);

	//Test for EStartupStateCriticalDynamic
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateNonCritical-1,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							ETrue, EFalse, ETrue,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							EStartupStateCriticalDynamic);

	//Test for EStartupStateNonCritical
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateNonCritical,
							CStartupStateObserver::SsoCriticalPlugingsInternalized,
							ETrue,ETrue, ETrue,
							CStartupStateObserver::SsoAllPluginsInternalized,
							EStartupStateNonCritical);

	//Test for EStartupStateNonCritical
	CheckForRunLStateChangeL(*serverStartupMgr, EStartupStateNonCritical+1,
							CStartupStateObserver::SsoAllPluginsInternalized,
							ETrue, ETrue, ETrue,
							CStartupStateObserver::SsoAllPluginsInternalized,
							EStartupStateNonCritical);

	CleanupStack::Pop(serverStartupMgr);
	delete serverStartupMgr;

	CleanupStack::PopAndDestroy(dummyObject3);
	CleanupStack::PopAndDestroy(dummyObject2);
	CleanupStack::PopAndDestroy(dummyObject1);

	ResetSsa(TheTest, TheFs);
	}

/**
Creates and installs active scheduler for this thread,
creates a CServerStartupMgr instance and calls
TServerStartupManager_StateAccessor::RunError

*/
TInt DoRunError_TestL()
	{
	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);

	CActiveScheduler::Install(scheduler);

	//create a new ServerStartupMgr
	CServerStartupMgr* serverStartupMgr =
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KDmIdRoot, TheFs);

	CleanupStack::PushL(serverStartupMgr);

	//call the RunError method which should panic
	TInt result = TServerStartupManager_StateAccessor::RunError(*serverStartupMgr,KErrNoMemory);

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, scheduler);

	return result;

	}

/**
Thread entry point for the test thread.  Creates a CTrapCleanup and
calls  DoRunError_TestL to carry out the test

*/
TInt RunErrorThreadEntry(TAny* /*a*/)
	{

	CTrapCleanup* tc = CTrapCleanup::New();

	TRAPD(err,DoRunError_TestL());

	delete tc;

	return err;

	}


/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3164
@SYMTestCaseDesc 	Check that the CServerStartupMgr::RunError() works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Create a new thread which will call RunError.  Wait for the
					thread to exit and check the thread exit type and reason to
					verify behaviour
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF094675
*/
void RunError_Test()
	{
	__UHEAP_MARK;
	_LIT(KStartThreadName,"CServerStartupMgr RunError Thread");

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3164 CServerStartupMgr RunError test "));

	//Disable JIT so that the Panic doesn't bring up a dialog
	//and stop the test
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	//Create a new thread to run the test
	RThread testThread;
	testThread.Create(KStartThreadName, RunErrorThreadEntry,
					KDefaultStackSize,KMinHeapSize,KMinHeapSize,NULL);
	TRequestStatus status;
	testThread.Logon(status);
	testThread.Resume();

	//Wait for the thread to exit
	User::WaitForRequest(status);

	//Obtain exit type and reason for test thread
	TExitType exitType = testThread.ExitType();
	TInt exitReason = testThread.ExitReason();

	//close the thread handle
	testThread.Close();

	//Set JIT back to original state
	User::SetJustInTime(jitEnabled);

	//Verify the exit reason and exit code
	TEST(exitType == EExitPanic);
	TEST(exitReason == EEComPanic_CServerStartupMgr_RunError);

	__UHEAP_MARKEND;

	}

LOCAL_C void CreateDelete_TestL()
	{
	CServerStartupMgr* serverStartupMgr =
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KSM2OSServicesDomain3, TheFs);
#else
		new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KBaseServicesDomain3, TheFs);
#endif //SYMBIAN_SYSTEM_STATE_MANAGEMENT

	TEST(serverStartupMgr != NULL);

	delete serverStartupMgr;
	}


// Type definition for pointer to member function.
// Used in calling t_ServerStartupMgr test functions.
typedef void (*ClassFuncPtrL) (void);

/**
Wrapper function to call all test functions
@param		testFunc pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	testFuncL();

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, testFuncL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TEST2(err, KErrNone);
	TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

LOCAL_C void DoTestsL()
	{
	DoBasicTestL(&CreateDelete_TestL, _L("CreateDelete_TestL"));
	DoBasicTestL(&GetKnownStartupState_TestL, _L("GetKnownStartupState_TestL"));
	DoBasicTestL(&InitialiseL_NonSsa_TestL, _L("InitialiseL_NonSsa_TestL"));
	DoBasicTestL(&InitialiseL_Ssa_TestL, _L("InitialiseL_Ssa_TestL"));
	DoBasicTestL(&RunL_TestL, _L("RunL_TestL"));

	DoOOMTestL(&CreateDelete_TestL, _L("OOM CreateDelete_TestL"));
	DoOOMTestL(&GetKnownStartupState_TestL, _L("OOM GetKnownStartupState_TestL"));
	DoOOMTestL(&InitialiseL_NonSsa_TestL, _L("OOM InitialiseL_NonSsa_TestL"));
	DoOOMTestL(&InitialiseL_Ssa_TestL, _L("OOM InitialiseL_Ssa_TestL"));
	DoOOMTestL(&RunL_TestL, _L("OOM RunL_TestL"));

	RunError_Test();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Printf(_L("\n"));
	TheTest.Title();
	TheTest.Start(_L("ServerStartupMgr Tests."));

	TEST2(TheFs.Connect(), KErrNone);

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	// Construct and install the Active Scheduler. The Active Scheduler is needed
	// by components used by this test as they are ActiveObjects.
	CActiveScheduler*	activeScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	EnableEcomTestBehaviour(TheTest, TheFs);

	TRAPD(err,DoTestsL());
	ResetSsa(TheTest, TheFs);
	TheTest(err==KErrNone);

	DisableEcomTestBehaviour(TheTest, TheFs);

	TheTest.End();
	TheTest.Close();

	//delete scheduler;
	delete activeScheduler;
	delete cleanup;

	TheFs.Close();

	__UHEAP_MARKEND;

	User::Heap().Check();

	return KErrNone;
	}
