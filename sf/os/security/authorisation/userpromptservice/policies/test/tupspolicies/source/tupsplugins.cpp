/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "tupsplugins.h"

#include <ecom/ecom.h>
#include <ups/upsconst.h>
#include <ups/fingerprint.h>
#include <ups/upserr.h>
#include "cleanuputils.h"
#include "util.h"

using namespace UserPromptService;

_LIT(KRequestsSection, "requests");

void CTestPlugins::SetupL()
/**
Initialise policy library objects.
 */
	{
	__UHEAP_MARK;
	User::LeaveIfError(iFs.Connect());
	iActiveWaiter = new(ELeave)CActiveWaiter(Logger());

	// Dummy decision record for ForcePromptL
	iDecisionRecord = CDecisionRecord::NewL(TSecureId(0), TUid::Null(),	TUid::Null(),
			TSecureId(0), _L8("foo"), KNullDesC8, KNullDesC, EFalse, 0, 0);		
	}

void CTestPlugins::TearDownL()
/**
Destroy policy library objects.
 */
	{
	iRequests.ResetAndDestroy();
	iExpectedValues.ResetAndDestroy();		
	delete iDecisionRecord;
	iDecisionRecord = 0;
	delete iActiveWaiter;
	iActiveWaiter = 0;
	iFs.Close();
	__UHEAP_MARKEND;
	}

void CTestPlugins::DoCancel() 
	{
	}

void CTestPlugins::TestCacheL()
/**
 */
	{
	TBool passed(ETrue);
			
	TInt numRequests;	
	ASSERT_TRUE(iConfig.GetInt(KRequestsSection,_L("numrequests"), numRequests));
	ASSERT_TRUE(numRequests > 0);
	
	LoadTestDataL(numRequests);
	for (TInt i = 0; i < numRequests; ++i)
		{
		TBuf<16> section(_L("request"));
		section.AppendNum(i);
		
		// Is the test expected to fail ?
		iExpectedError = KErrNone;	
		(void) iConfig.GetInt(section, _L("expectederror"), iExpectedError);				
		
		if (! iInteractive)	GetResponseL(section);
		
		if (iOom) 
			{
			INFO_PRINTF2(_L("Running OOM test for request %d"), i);
			passed &= DoOomRequestL(*iExpectedValues[i], *iRequests[i]);
			}
		else
			{
			TRAPD(err, passed &= DoRequestL(*iExpectedValues[i], *iRequests[i]));
			if (err == KErrNoMemory) User::Leave(err);		
			passed &= CheckExpectedError(Logger(), iExpectedError, err);					
			}				
		}
	if (!passed) 
		{
		User::Leave(KErrTEFUnitFail);
		}
	}

void CTestPlugins::LoadTestDataL(TInt aNumRequests)
	{
	(void) iConfig.GetBool(KRequestsSection,_L("oom"), iOom);
	// Gets the scripted prompt response
	(void) iConfig.GetBool(KRequestsSection,_L("interactive"), iInteractive);

	// Read all the requests up front
	for (TInt i = 0; i < aNumRequests; ++i)
		{		
		TBuf<16> section(_L("request"));
		section.AppendNum(i);
		
		GetExpectedValuesL(section);
		CPromptRequest* req = GetRequestParamsLC(section, iExpectedValues[i]->iClientEntity);			
		iRequests.AppendL(req);
		CleanupStack::Pop(req);				
		}	
	}

void CTestPlugins::GetResponseL(const TDesC& aSection)
	{
	TPtrC response;
	TBool ret = iConfig.GetString(aSection, _L("response"), response);
	ASSERT_TRUE(ret);	
	iResponse = static_cast<CPolicy::TOptions>(StringToOptionL(response));
	
	RFile file;
	TBuf<21> name(_L("!:\\upsrefnotifier.txt"));
	name[0] = iFs.GetSystemDriveChar();	
	User::LeaveIfError(file.Replace(iFs, name, EFileShareExclusive | EFileWrite));
	CleanupClosePushL(file);
	
	HBufC* opt = OptionToString(iResponse).AllocLC();	
	User::LeaveIfError(file.Write(opt->Des().Collapse()));
	
	if (! iOom) INFO_PRINTF2(_L("Scripted prompt response = %S"), &OptionToString(iResponse));
	CleanupStack::PopAndDestroy(2, &file); // file, opt
	}

TBool CTestPlugins::DoRequestL(const TExpectedValue& aExpected, const CPromptRequest& aRequest)
	{
	TBool passed(ETrue);
	
	CPluginManager* pluginManager = CPluginManager::NewL();
	CleanupStack::PushL(pluginManager);	
	CPolicyCache* policyCache = CPolicyCache::NewL(iFs, _L("\\tups\\tpolicies\\policies\\"));	
	CleanupStack::PushL(policyCache);
	
	TBool protectedSid = ((aRequest.ClientSid().iId & 0x80000000) == 0);
	if (protectedSid != aRequest.IsClientSidProtected())
		{
		ERR_PRINTF3(_L("Expected value for CPromptRequest::IsProtectedSid() = %d, actual value = %d"),
			protectedSid, aRequest.IsClientSidProtected());
		passed = EFalse;
		}
	
	const CPolicy* policy = policyCache->MatchL(aRequest);	
	ASSERT_NOT_NULL(policy);
	// Attempt to load the policy a second time, should get the same 
	// policy object and policy cache should not leak.
	__UHEAP_MARK;
	ASSERT_EQUALS(policy, policyCache->MatchL(aRequest));
	__UHEAP_MARKEND;
	
	// It should also be safe to load the service configuration if we have loaded a single
	// policy file.
	RArray<TServiceConfig> configs;
	CleanupClosePushL(configs);
	policyCache->ServiceConfigL(aRequest.ServerSid().iId, configs);
	ASSERT_TRUE(configs.Count() > 0);	// If policy was loaded there must be at least one service config	
	CleanupStack::PopAndDestroy(&configs);
	
	passed &= CheckPolicyL(aExpected, policy);
	
	if (policy->PromptRequired())
		{	
		// Just verify that GetExtension doesn't crash
		TAny* extensionPtr(0);		
		CPlugin<CPolicyEvaluator>* policyEvaluator = 
			pluginManager->CreateEvaluatorL(policy->PolicyEvaluator());
		CleanupStack::PushL(policyEvaluator);
		policyEvaluator->Imp().GetExtension(0, extensionPtr, NULL);		

		RPointerArray<CFingerprint> fingerprints;
		CleanupResetAndDestroyPushL(fingerprints);
		const CClientEntity* clientEntity(0);
		const TAny* dialogCreatorParams(0);
		
		policyEvaluator->Imp().GenerateFingerprints(aRequest, *policy, 
			fingerprints, clientEntity, dialogCreatorParams, iActiveWaiter->iStatus);		
		iActiveWaiter->WaitActive(KErrNone);
					
		if (! iOom) INFO_PRINTF3(_L("Policy Evaluator 0x%08x: num fingerprints = %d"), 
			policy->PolicyEvaluator(), fingerprints.Count());
			
		if (aExpected.iClientEntity.Length())
			{
			if (! clientEntity)
				{	
				ERR_PRINTF1(_L("Client entity expected"));
				passed = EFalse;
				}
			else if (clientEntity->Name() != aExpected.iClientEntity)
				{
				ERR_PRINTF1(_L("Cliententity name mismatch"));
				passed = EFalse;
				}
			}
		else if (aExpected.iClientEntity.Length() == 0 && clientEntity)
			{
			ERR_PRINTF1(_L("Unexpected client entity"));
			passed = EFalse;
			}
		
		// Make sure base class implementation for ForcePromptL returns EFalse
		TUint evaluatorInfo(0);
		ASSERT_FALSE(policyEvaluator->Imp().CPolicyEvaluator::ForcePromptL(*iDecisionRecord, evaluatorInfo));
		
		CPlugin<CDialogCreator>* dialogCreator
			= pluginManager->CreateDialogCreatorL(policy->DialogCreator());
		CleanupStack::PushL(dialogCreator);
		dialogCreator->Imp().GetExtension(0, extensionPtr, NULL);
				
		dialogCreator->Imp().PrepareDialog(
			aRequest, *policy, fingerprints, clientEntity, dialogCreatorParams, iActiveWaiter->iStatus);
		iActiveWaiter->WaitActive(KErrNone);
		
		const CFingerprint* fingerprint(0);
		CPolicy::TOptions selected;
		dialogCreator->Imp().DisplayDialog(
			selected, fingerprint, evaluatorInfo, iActiveWaiter->iStatus);
		iActiveWaiter->WaitActive(KErrNone);
		
		if (! iOom) INFO_PRINTF2(_L("User response: %S"), &OptionToString(selected));
		if (! iInteractive && selected != iResponse) // don't check response in interactive mode
			{
			ERR_PRINTF3(_L("Expected user response = %S, actual user response = %S"), 
				&OptionToString(iResponse), &OptionToString(selected));
				passed = EFalse;
			}				
		
		if (fingerprint) 
			{
			if (! iOom) INFO_PRINTF2(_L("Description for selected fingerprint: %S"), &fingerprint->Description());
			}
		CleanupStack::PopAndDestroy(3, policyEvaluator); // fingerprint, dialogcreator
		}
	else 
		{
		if (! iOom) INFO_PRINTF2(_L("Policy %d does not require a prompt to be displayed"), policy->Flags());
		}	
	CleanupStack::PopAndDestroy(2, pluginManager); // policyCache
	return passed;
	}

void CTestPlugins::GetExpectedValuesL(const TDesC& aSection)
	{
	TExpectedValue* expected = new(ELeave) TExpectedValue();
	CleanupStack::PushL(expected);
	iExpectedValues.AppendL(expected);
	CleanupStack::Pop(expected);
	
	iConfig.GetHex(aSection, _L("expectedflags"), expected->iFlags);
	iConfig.GetHex(aSection, _L("expectedmajorversion"), expected->iMajorVersion);
	iConfig.GetHex(aSection, _L("expectedminorversion"), expected->iMinorVersion);
	iConfig.GetBool(aSection, _L("expectedsilent"), expected->iSilent);		
	iConfig.GetHex(aSection, _L("expectedpolicyevaluator"), expected->iPolicyEvaluator);

	TPtrC clientEntity;
	if (iConfig.GetString(aSection, _L("cliententity"), clientEntity))
		{
		ASSERT_TRUE(clientEntity.Length() < expected->iClientEntity.MaxLength());		
		expected->iClientEntity.Copy(clientEntity);
		}	
	}

TBool CTestPlugins::CheckPolicyL(const TExpectedValue& aExpected, const CPolicy* aPolicy)
/**
Checks the correct policy was found for the request.
@param	aPolicy	The policy object returned by the policy cache.
@return			ETrue if successful; otherwise, EFalse is returned.
*/
	{
	TBool passed = ETrue;
		
	if (aExpected.iFlags != aPolicy->Flags())
		{
		ERR_PRINTF3(_L("Expected flags = 0x%04x, actual flags 0x%04x"),
			aExpected.iFlags, aPolicy->Flags());
			passed = EFalse;
		}
	
	if (aExpected.iMajorVersion != aPolicy->MajorVersion())
		{
		ERR_PRINTF3(_L("Expected major version = %d, actual major version = %d"),
			aExpected.iMajorVersion, aPolicy->MajorVersion());
		passed = EFalse;
		}
	
	if (aExpected.iMinorVersion != aPolicy->MinorVersion())
		{
		ERR_PRINTF3(_L("Expected minor version = %d, actual minor version = %d"),
			aExpected.iMinorVersion, aPolicy->MinorVersion());
		passed = EFalse;
		}					

	if ((TUint)aExpected.iPolicyEvaluator != aPolicy->PolicyEvaluator().iUid) 
		{
		ERR_PRINTF3(_L("Expected policy evaluator = %08x, actual policy evaluator = %08x"),
			aExpected.iPolicyEvaluator, aPolicy->PolicyEvaluator().iUid)
		passed = EFalse;
		}

	TBool silent = ! aPolicy->PromptRequired();	
	if (silent) 
		{
		if (! iOom) INFO_PRINTF1(_L("Policy never requires a prompt."));
		}
	
	if (silent != aExpected.iSilent)
		{
		ERR_PRINTF3(_L("Expected value for silent policy = %d, actual = %d"),
			aExpected.iSilent, silent);
			passed = EFalse;
		}
					
	return passed;
	}

CTestSuite* CTestPlugins::CreateSuiteL(const TDesC& aName)
/**
Creates the test suite for User Prompt Service ECOM plug-ins
@param aName - Suite name
@return - Suite
*/
	{
	SUB_SUITE
	AddAsyncTestCase(lTestSuite, _L("TestCacheL"), &CTestPlugins::TestCacheL);
	AddAsyncTestCase(lTestSuite, _L("TestPluginManagerL"), &CTestPlugins::TestPluginManagerL);
	AddAsyncTestCase(lTestSuite, _L("TestLengthsL"), &CTestPlugins::TestLengthsL);
	END_SUITE
	}

CPromptRequest* CTestPlugins::GetRequestParamsLC(const TDesC& aSection, const TDesC8& aClientEntity)
/**
Gets the data from the CPromptRequest from a section in the config file.
@param aSection The name of the section containing the prompt request data.
@return			The new CPromptRequest object.
*/
{
	TInt clientSid;		
	ASSERT_TRUE(iConfig.GetHex(aSection,_L("clientsid"), clientSid));
	
	TInt clientVid;
	ASSERT_TRUE(iConfig.GetHex(aSection,_L("clientvid"), clientVid));
	
	TInt serverSid;
	ASSERT_TRUE(iConfig.GetHex(aSection,_L("serversid"), serverSid));

	TInt serviceId;
	ASSERT_TRUE(iConfig.GetHex(aSection,_L("serviceid"), serviceId));

	TPtrC destination;
	ASSERT_TRUE(iConfig.GetString(aSection,_L("destination"), destination));

	TBool securityResult;	
	ASSERT_TRUE(iConfig.GetBool(aSection,_L("securityresult"), securityResult));
	
	TPtrC opaqueData;
	ASSERT_TRUE(iConfig.GetString(aSection,_L("opaquedata"), opaqueData));
	
	TThreadId myThreadId;
	RThread me;
	myThreadId = me.Id();
	me.Close();

	TProcessId myProcessId;
	RProcess mep;
	myProcessId = mep.Id();
	mep.Close();
	
	RBuf destinationRb;
	destinationRb.CreateL(destination);
	CleanupClosePushL(destinationRb);
	
	RBuf8 opaqueDataRb;
	opaqueDataRb.CreateL(opaqueData.Length() + aClientEntity.Length() + 64);	
	CleanupClosePushL(opaqueDataRb);
	opaqueDataRb.Copy(opaqueData);
	if (aClientEntity.Length())
		{
		opaqueDataRb.Append(_L8("<ce>"));
		opaqueDataRb.Append(aClientEntity);
		opaqueDataRb.Append(_L8("</ce>"));		
		}


	CPromptRequest* req = 
		CPromptRequest::NewL(TUid::Uid(clientSid), TUid::Uid(clientVid), myThreadId, myProcessId,
				TUid::Uid(serverSid), TUid::Uid(serviceId), 
				destinationRb, opaqueDataRb, securityResult);
				
	CleanupStack::PopAndDestroy(&opaqueDataRb);
	CleanupStack::PopAndDestroy(&destinationRb);

	CleanupStack::PushL(req);

	return req;
}

void CTestPlugins::TestLengthsL()
/**
Tests length constrains in the CFingerprint and CClientEntity classes
*/
	{
	TInt maxLen = KUpsMaxFingerprintLength + 1;
	HBufC8* bigBuffer = HBufC8::NewMaxLC(maxLen);
	TPtrC8 emptyBuf;
	
	_LIT8(K32Bytes, "01234567890123456789012345678901");
	_LIT(KDescription, "description");
	
	CFingerprint* fp(0);
	CClientEntity* ce(0);
	
	TInt err(KErrNone);
		
	if (! iOom) INFO_PRINTF1(_L("Create a fingerprint with an empty value - should pass"));
	fp = CFingerprint::NewL(emptyBuf, KDescription);
	delete fp;
	
	if (! iOom) INFO_PRINTF1(_L("Create a fingerprint with value that's too long  - should fail"));
	TRAP(err, fp = CFingerprint::NewL(*bigBuffer, KDescription));
	ASSERT_EQUALS(err, KErrUpsBadFingerprintLength);
	
	if (! iOom) INFO_PRINTF1(_L("Create a fingerprint with 32 bytes of data - should pass"));
	fp = CFingerprint::NewLC(K32Bytes, KDescription);
	ASSERT_TRUE(fp->Fingerprint() == K32Bytes);
	ASSERT_TRUE(fp->Description() == KDescription);
	CleanupStack::PopAndDestroy(fp);
	
	if (! iOom) INFO_PRINTF1(_L("Create a client entity with an empty value - should pass"));
	ce = CClientEntity::NewLC(emptyBuf);
	ASSERT_TRUE(ce->Name() == emptyBuf);
	CleanupStack::PopAndDestroy(ce);
	
	if (! iOom) INFO_PRINTF1(_L("Create a client entity with a name that's too long - should fail"));
	TRAP(err, ce = CClientEntity::NewL(*bigBuffer));
	ASSERT_EQUALS(err, KErrUpsBadClientEntityLength);
	
	if (! iOom) INFO_PRINTF1(_L("Create a client entity with a 32 byte name - should pass"));
	ce = CClientEntity::NewLC(K32Bytes);
	ASSERT_TRUE(ce->Name() == K32Bytes);
	CleanupStack::PopAndDestroy(ce);
	
	CleanupStack::PopAndDestroy(bigBuffer);
	}

void CTestPlugins::TestPluginManagerL()
/**
 Creates and destroy UPS plug-ins to test CPluginManager.
 */
	{
	TInt uidVal;
	CPluginManager* pluginManager = CPluginManager::NewL();
	CleanupStack::PushL(pluginManager);
	
	CPlugin<CPolicyEvaluator>* policyEvaluator(0);
	CPlugin<CDialogCreator>* dialogCreator(0);	
	
	ASSERT_TRUE(iConfig.GetHex(_L("pluginmanager"),_L("policyevaluatoruid"), uidVal));
	TUid policyEvaluatorUid = TUid::Uid(uidVal);
	
	ASSERT_TRUE(iConfig.GetHex(_L("pluginmanager"),_L("dialogcreatoruid"), uidVal));
	TUid dialogCreatorUid = TUid::Uid(uidVal);
	
	if (! iOom) INFO_PRINTF1(_L("Calling unload with no active plug-ins"));
	pluginManager->Unload();
	ASSERT_EQUALS(pluginManager->iPluginCount, 0);

	if (! iOom) INFO_PRINTF1(_L("Calling unload after creating and destroying a plug-in"));
	policyEvaluator = pluginManager->CreateEvaluatorL(policyEvaluatorUid);	
	ASSERT_EQUALS(pluginManager->iPluginCount, 1);
	delete policyEvaluator;
	ASSERT_EQUALS(pluginManager->iPluginCount, 0);
	pluginManager->Unload();
	ASSERT_EQUALS(pluginManager->iPluginCount, 0);
	
	if (! iOom) INFO_PRINTF1(_L("Calling unload after deleting last plug-in"));
	policyEvaluator = pluginManager->CreateEvaluatorL(policyEvaluatorUid);	
	ASSERT_EQUALS(pluginManager->iPluginCount, 1);
	delete policyEvaluator;
	ASSERT_EQUALS(pluginManager->iPluginCount, 0);
	policyEvaluator = pluginManager->CreateEvaluatorL(policyEvaluatorUid);	
	ASSERT_EQUALS(pluginManager->iPluginCount, 1);
	pluginManager->Unload();
	ASSERT_EQUALS(pluginManager->iPluginCount, 1);	
	delete policyEvaluator;
	ASSERT_EQUALS(pluginManager->iPluginCount, 0);
	
	INFO_PRINTF1(_L("Policy evaluators and dialog creators"));
	policyEvaluator = pluginManager->CreateEvaluatorL(policyEvaluatorUid);	
	ASSERT_EQUALS(pluginManager->iPluginCount, 1);
	CleanupStack::PushL(policyEvaluator);
	pluginManager->Unload();
	ASSERT_EQUALS(pluginManager->iPluginCount, 1);
	dialogCreator = pluginManager->CreateDialogCreatorL(dialogCreatorUid);	
	ASSERT_EQUALS(pluginManager->iPluginCount, 2);
	CleanupStack::PushL(dialogCreator);
	CleanupStack::PopAndDestroy(2, policyEvaluator); // dialogCreator	
	ASSERT_EQUALS(pluginManager->iPluginCount, 0);
	
	CleanupStack::PopAndDestroy(pluginManager);
	}

TBool CTestPlugins::DoOomRequestL(const TExpectedValue& aExpected, const CPromptRequest& aRequest)
/**
	Performs OOM test
 */
	{
	TBool passed = EFalse;
	
	TInt countAfter = 0;
	TInt countBefore = 0;
	for (TInt oomCount = 0; ; oomCount++)
		{
		INFO_PRINTF2(_L("\n ==== Number of memory allocations %d ===="), oomCount);
		passed = EFalse;
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
		countBefore = User::CountAllocCells();
		TRAPD(error, DoRequestL(aExpected, aRequest));// ----> This is the actual test that runs under OOM conditions.
		countAfter = User::CountAllocCells();
		__UHEAP_RESET;
		
		ASSERT_TRUE(error == KErrNone || error == KErrNoMemory);
		
		if (error == KErrNone)
			{
			passed = ETrue;
			INFO_PRINTF2(_L("OOM Status %d"),error);
			INFO_PRINTF1(_L("Test outcome : Passed"));
			break;
			}
		else
			{
			if (countBefore != countAfter)
				{
				INFO_PRINTF2(_L("OOM Status %d"),error);
				INFO_PRINTF2(_L("OOM Failed at %d"), oomCount);
				break;
				}
			}
		INFO_PRINTF2(_L("OOM Failed Point status %d"), error);
		}
	INFO_PRINTF3(_L("Heap alloc count ok: %d final vs %d initial"), countAfter,countBefore);
	return passed;
	}
