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


#include "tserviceconfig.h"
#include <test/testexecutelog.h>
#include <test/tefunit.h>

#include "policycache.h"

using namespace UserPromptService;

CServiceConfigTestStep::CServiceConfigTestStep()
/**
Constructor
*/
	{
	// Set human readable name for logging
	SetTestStepName(KServiceConfigStep);		
	}
	
CServiceConfigTestStep::~CServiceConfigTestStep()
/**
Destructor
*/
	{
	delete iPolicyCache;
	iPolicyCache = 0;
	iFs.Close();
	}

TVerdict CServiceConfigTestStep::doTestStepPreambleL()
/**
Initialise
@return Whether the test initialization passed or failed.
 */
	{
	User::LeaveIfError(iFs.Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CServiceConfigTestStep::doTestStepL()
/**
Do the test
@return Whether the test passed or failed.
 */
	{
	TPtrC policyDir;
	ASSERT_TRUE(GetStringFromConfig(ConfigSection(), _L("policydir"), policyDir));
	
	iPolicyCache = CPolicyCache::NewL(iFs, policyDir);	

	TInt serverSid;
	ASSERT_TRUE(GetHexFromConfig(ConfigSection(), _L("serversid"), serverSid));
	
	INFO_PRINTF2(_L("Loading service configuration for system server 0x%08x"), serverSid);
	RArray<TServiceConfig> serviceConfigs;
	iPolicyCache->ServiceConfigL(TSecureId(serverSid), serviceConfigs);
	CleanupClosePushL(serviceConfigs);
	
	TInt numServices;
	ASSERT_TRUE(GetIntFromConfig(ConfigSection(), _L("numservices"), numServices));
	
	if (numServices != serviceConfigs.Count())
		{
		ERR_PRINTF3(_L("Number of services = %d, expected number of services = %d"),
			serviceConfigs.Count(), numServices);
			SetTestStepResult(EFail);			
		}
	
	for (TInt i = 0; i < Min(serviceConfigs.Count(), numServices); ++i)
		{		
		TBuf<32> key;
		
		INFO_PRINTF3(_L("service %08x, policy %d"), 
			serviceConfigs[i].iServiceId, serviceConfigs[i].iPolicy);
		
		TInt serviceId;
		key.AppendFormat(_L("service%d"), i);
		ASSERT_TRUE(GetHexFromConfig(ConfigSection(), key, serviceId));		
		if (serviceConfigs[i].iServiceId != serviceId)
			{
			ERR_PRINTF4(_L("Config %d, expected service id = %08x, actual service id %08x"),
				i, serviceId, serviceConfigs[i].iServiceId);
			SetTestStepResult(EFail);			
			}
				
		TPtrC policyString;
		key.Zero();
		key.AppendFormat(_L("policy%d"), i);
		ASSERT_TRUE(GetStringFromConfig(ConfigSection(), key, policyString));
		TAuthorisationPolicy policy = PolicyEnumFromString(policyString);		
		if (serviceConfigs[i].iPolicy != policy)
			{
			ERR_PRINTF4(_L("Config %d, expected authorisation policy = %d, actual authorisation policy = %d"),
				i, policy, serviceConfigs[i].iPolicy);
			SetTestStepResult(EFail);
			}
		}	
	
	CleanupStack::PopAndDestroy(&serviceConfigs);
	return TestStepResult();
	}

TAuthorisationPolicy CServiceConfigTestStep::PolicyEnumFromString(const TDesC& aEnumString)
	{
	TAuthorisationPolicy policy(EAlwaysCheck);
	
	if (aEnumString.CompareF(_L("EAlwaysCheck")) == 0)
		{
		policy =  EAlwaysCheck;		
		}
	else if (aEnumString.CompareF(_L("ECheckPostManufacture")) == 0)
		{
		policy =  ECheckPostManufacture;
		}
	else if (aEnumString.CompareF(_L("ECheckUnprotectedSids")) == 0)
		{
		policy = ECheckUnprotectedSids;
		}
	else if (aEnumString.CompareF(_L("ECheckIfFailed")) == 0)
		{
		policy = ECheckIfFailed;
		}
	else if (aEnumString.CompareF(_L("ENeverCheck")) == 0)
		{
		policy = ENeverCheck;
		}
	else 
		{
		ERR_PRINTF2(_L("TAuthorisationPolicy enum %S not recognised"), &aEnumString);
		ASSERT_TRUE(EFalse);	
		}	
	return policy;
	}

TVerdict CServiceConfigTestStep::doTestStepPostambleL()
/**
Cleanup
@return Whether cleanup passed or failed.
 */
	{
	return TestStepResult();
	}

