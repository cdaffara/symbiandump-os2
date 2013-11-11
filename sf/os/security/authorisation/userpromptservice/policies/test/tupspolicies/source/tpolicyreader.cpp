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


#include "tpolicyreader.h"

#include <ecom/ecom.h>
#include "policylist.h"
#include "cleanuputils.h"
#include "util.h"

using namespace UserPromptService;

void CTestPolicyReader::SetupL()
/**
Initialise policy reader and related resources.
 */
	{
	__UHEAP_MARK;
	User::LeaveIfError(iFs.Connect());
	}

void CTestPolicyReader::TearDownL()
/**
Free resources
 */
	{
	delete iPolicyReader;
	iSidList.Close();
	iFs.Close();
	__UHEAP_MARKEND;
	}

void CTestPolicyReader::TestPolicyReaderL()
/**
Reads and verifies every entry in a policy file.
 */
	{
	TBool passed(ETrue);
	TBool res;
	TInt err;
	
	TPtrC policyFile;
	res = iConfig.GetString(_L("policies"), _L("policyfile"), policyFile);
	ASSERT_TRUE(res);
	
	// Test policy list error handling
	iExpectedError = KErrNone;	
	res = iConfig.GetInt(_L("policies"), _L("expectedpolicylisterror"), iExpectedError);	
	TRAP(err, TestPolicyListL(policyFile));
	ASSERT_TRUE(CheckExpectedError(Logger(), iExpectedError, err));
	// Skip rest of test if the CPolicyList is expected to fail.
	if (iExpectedError != KErrNone) return;
	
	TRAP(err, CreatePolicyReaderL(policyFile));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Error %d creating policy reader"), err);
		passed = EFalse;
		}
	else
		{				
		TInt numPolicies(-1);
		res = iConfig.GetInt(_L("policies"),_L("numpolicies"), numPolicies);
		ASSERT_TRUE(res);
		ASSERT_TRUE(numPolicies >= 0);

		for (TInt i = 0; i < numPolicies; ++i)
			{
			TBuf<16> section(_L("policy"));
			section.AppendNum(i);
			INFO_PRINTF2(_L("Reading policy %d"), i);			
			TRAP(err, passed &= CheckPolicyL(section));
			passed &= CheckExpectedError(Logger(), iExpectedError, err);			
			}
		}
	if (!passed) User::Leave(KErrTEFUnitFail);
	}

void CTestPolicyReader::TestPolicyListL(const TDesC& aPolicyFile)
	{
	INFO_PRINTF2(_L("Creating a CPolicyList from %S"), &aPolicyFile);
	CPolicyReader* r = CPolicyReader::NewL(iFs, aPolicyFile);
	CleanupStack::PushL(r);
	CPolicyList::TId id;
	CPolicyList* policyList = CPolicyList::NewL(id, *r);
	delete policyList;
	CleanupStack::PopAndDestroy(r);
	}

void CTestPolicyReader::CreatePolicyReaderL(const TDesC& aPolicyFile)
	{
	iPolicyReader = CPolicyReader::NewL(iFs, aPolicyFile);
	}

TBool CTestPolicyReader::CheckPolicyL(const TDesC& aSection)
	{
	TBool passed;
	ReadExpectedValuesL(aSection);
	CPolicy* p = iPolicyReader->NextPolicyL();
	if (! p)
		{
		ERR_PRINTF1(_L("Unexpected end of policy list"));
		User::Leave(KErrEof);
		}
	
	CleanupStack::PushL(p);
	passed = CompareExpectedValues(*p);
	CleanupStack::PopAndDestroy(p);
	return passed;
	}


CTestSuite* CTestPolicyReader::CreateSuiteL(const TDesC& aName)
/**
Creates the test suite for UPS policy reader
@param aName - Suite name
@return - Suite
*/
	{
	SUB_SUITE
	AddTestCase(lTestSuite, _L("TestPolicyReader"), &CTestPolicyReader::TestPolicyReaderL);
	END_SUITE
	}

void CTestPolicyReader::ReadExpectedValuesL(const TDesC& aSection)
/**
 Reads the expected values of the policy from section in the .ini file.
 @param aPolicySection The name of the section containing the expected policy files.
 */
	{
	TBool ret;
	
	iExpectedError = KErrNone;
	ret = iConfig.GetInt(aSection, _L("expectederror"), iExpectedError);
	if (ret) return; // If an error is expected then other config settings aren't required.

	ret = iConfig.GetHex(aSection, _L("sidclasses"), iSidClasses);
	if (! ret) iSidClasses = 0x0000FFFF; 

	iSidList.Reset();
	TInt numSids(0);
	ret = iConfig.GetInt(aSection, _L("numsids"), numSids);
	for (TInt i = 0; i < numSids; ++i)
		{
		TBuf<8> key;
		key.AppendFormat(_L("sid%d"), i);

		TInt sid;
		ret = iConfig.GetHex(aSection, key, sid);
		ASSERT_TRUE(ret);
		iSidList.AppendL(sid);
		}

	ASSERT_TRUE(iConfig.GetString(aSection, _L("destination"), iDestination));
	ASSERT_TRUE(iConfig.GetHex(aSection, _L("options"), iOptions));
	ASSERT_TRUE(iConfig.GetHex(aSection, _L("systemserversecurity"), iSystemServerSecurity));

	TInt policyEvaluator;
	ASSERT_TRUE(iConfig.GetHex(aSection, _L("policyevaluator"), policyEvaluator));
	iPolicyEvaluator = TUid::Uid(policyEvaluator);
	
	TInt dialogCreator;
	ASSERT_TRUE(iConfig.GetHex(aSection, _L("dialogcreator"), dialogCreator));
	iDialogCreator = TUid::Uid(dialogCreator);
	ASSERT_TRUE(iConfig.GetHex(aSection, _L("flags"), iFlags));

	ASSERT_TRUE(iConfig.GetInt(aSection, _L("majorversion"), iMajorVersion));
	ASSERT_TRUE(iConfig.GetInt(aSection, _L("minorversion"), iMinorVersion));
	ASSERT_TRUE(iConfig.GetBool(aSection, _L("default"), iDefault));
	}

TBool CTestPolicyReader::CompareExpectedValues(const CPolicy& aPolicy)
/**
 Compares the expected policy values from the ini file to the fields
 in the policy object read from the resource file.
 @param aPolicy The policy object to compare the ini value to.
 @return ETrue, if the values match; otherwise, EFalse is returned.
 */
	{
	TBool ret(ETrue);

	if (static_cast<TUint16>(iSidClasses) != aPolicy.SidClasses().iSidClasses)
		{
		ERR_PRINTF3(_L("Expected SID classes 0x%04x, actual SID classes 0x%04x"),
				   iSidClasses, aPolicy.SidClasses().iSidClasses);
		ret = EFalse;
		} 

	if (iSidList.Count() != aPolicy.SidList().Count())
		{
		ERR_PRINTF3(_L("Expected number of SIDs = %d, actual number of SIDs = %d"),
					iSidList.Count(), aPolicy.SidList().Count());
		ret = EFalse;
		}
		
	if (iSystemServerSecurity != aPolicy.SystemServerSecurity())
		{
		ERR_PRINTF3(_L("Expected systemserversecurity %d, actual systemserversecurity %d"),
			iSystemServerSecurity, aPolicy.SystemServerSecurity());
		ret = EFalse;
		}

	for (TInt i = 0; i < Min(iSidList.Count(), aPolicy.SidList().Count()); ++i)
		{
		if (iSidList[i].iId != aPolicy.SidList()[i].iId)
			{
			ERR_PRINTF4(_L("Expected SID%d = 0x%08x, actual SID 0x%08x"),
						i, iSidList[i].iId, aPolicy.SidList()[i].iId);
			ret = EFalse;			
			}
		}
	
	if (iDestination != aPolicy.Destination())
		{
		ERR_PRINTF3(_L("Expected destination %S, actual destination %S"),
					&iDestination, &aPolicy.Destination());
		ret = EFalse;
		}

	if (iOptions != aPolicy.Options())
		{
		ERR_PRINTF3(_L("Expected options = 0x%08x, actual options = 0x%08x"),
					&iOptions, aPolicy.Options());
		ret = EFalse;
		}

	if (iPolicyEvaluator != aPolicy.PolicyEvaluator())
		{
		ERR_PRINTF3(_L("Expected policy evaluator 0x%08x, actual policy evaluator 0x%08x"),
					iPolicyEvaluator, aPolicy.PolicyEvaluator());
		ret = EFalse;
		}

	if (iDialogCreator != aPolicy.DialogCreator())
		{
		ERR_PRINTF3(_L("Expected dialog creator 0x%08x, actualy dialog creator 0x%08x"),
					iDialogCreator, aPolicy.DialogCreator());
		ret = EFalse;
		}

	if (iFlags != aPolicy.Flags())
		{
		ERR_PRINTF3(_L("Expected flags = 0x%04x, actual flags = 0x%04x"),
					iFlags, aPolicy.Flags());
		ret = EFalse;
		}
		
	if (iMajorVersion != aPolicy.MajorVersion())
		{
		ERR_PRINTF3(_L("Expected majorversion = %d, actual major version = %d"),
					iMajorVersion, aPolicy.MajorVersion());
		ret = EFalse;		
		}

	if (iMinorVersion != aPolicy.MinorVersion())
		{
		ERR_PRINTF3(_L("Expected majorversion = %d, actual major version = %d"),
					iMinorVersion, aPolicy.MinorVersion());
		ret = EFalse;		
		}		
		
	if (iDefault != aPolicy.Default())
		{
		ERR_PRINTF3(_L("Expected default policy = %d, actual default policy= %d"),
					iDefault, aPolicy.Default());
		ret = EFalse;		
		}				
	return ret;
	}
