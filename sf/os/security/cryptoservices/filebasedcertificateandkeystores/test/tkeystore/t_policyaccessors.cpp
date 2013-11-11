/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"

// CGetPolicy //////////////////////////////////////////////////////////////////

/*static*/ CTestAction* CGetPolicy::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CGetPolicy::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CTestAction* CGetPolicy::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CGetPolicy* self = new (ELeave) CGetPolicy(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CGetPolicy::CGetPolicy(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CKeyStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CGetPolicy::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;

	ASSERT(aTestActionSpec.iActionType == KGetUsePolicy || aTestActionSpec.iActionType == KGetManagementPolicy);
	iTestUsePolicy = (aTestActionSpec.iActionType == KGetUsePolicy);

	TPtrC8 policySpec = Input::ParseElement(aTestActionSpec.iActionResult, KPolicyStart);
	if (policySpec == KNullDesC8)
		{
		User::Leave(KErrArgument);
		}
	Input::ParseSecurityPolicyL(policySpec, iExpectedPolicy);
	
	iState = EListKeys;
	}

CGetPolicy::~CGetPolicy()
	{
	iKeys.Close();
	}

void CGetPolicy::PerformAction(TRequestStatus& aStatus)
	{
    if (aStatus != KErrNone)
        {
        iState = EFinished;
        }
    
	switch (iState)
		{	
		case EListKeys:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);
			
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EFinished;
			}
			break;
		
		case EFinished:
			{
			if (aStatus == KErrNone)
				{
				// find the key				
				CCTKeyInfo* key = NULL;
				for (TInt i = 0 ; i < iKeys.Count() ; i++)
					{
					if (iKeys[i]->Label()==*iLabel)
						{
						key = iKeys[i];
						break;
						}
					}

				if (!key)
					{
					aStatus = KErrNotFound;
					}
				else
					{
					// get the policy
					iPolicy = iTestUsePolicy ? key->UsePolicy() : key->ManagementPolicy();
					}
				}
			
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus.Int() == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			
			iActionState = EPostrequisite;
			}
			break;
			
		default:
			ASSERT(EFalse);	
		}
	}

void CGetPolicy::PerformCancel()
	{
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](0);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelList();
        }
	}

void CGetPolicy::Reset()
	{
	iState = EListKeys;
	iKeys.Close();
	}

void CGetPolicy::DoReportAction()
	{
	_LIT(KGettingUsePolicy, "Getting use policy...");
	_LIT(KGettingManagementPolicy, "Getting management policy...");
	_LIT(KExpect, "Expecting policy: ");

	if (iTestUsePolicy)
		{			
		iOut.writeString(KGettingUsePolicy);
		}
	else
		{			
		iOut.writeString(KGettingManagementPolicy);
		}
	iOut.writeNewLine();
	iOut.writeString(KExpect);
	iOut.writeSecurityPolicyL(iExpectedPolicy);
	iOut.writeNewLine();
	}

void CGetPolicy::DoCheckResult(TInt)
	{
	if (iFinished && iResult)
		{
		_LIT(KGotPolicy, "Retreived policy: ");
		iOut.writeString(KGotPolicy);
		iOut.writeSecurityPolicyL(iPolicy);
		iOut.writeNewLine();

		TPckgC<TSecurityPolicy> policyPckg(iPolicy);
		TPckgC<TSecurityPolicy> expectedPolicyPckg(iExpectedPolicy);
					
		if (policyPckg != expectedPolicyPckg)
			{
			iResult = EFalse;

			_LIT(KBadPolicy, "Policy does not match expected");
			iOut.writeString(KBadPolicy);
			iOut.writeNewLine();
			}
		}
	}

// CSetPolicy //////////////////////////////////////////////////////////////////

/*static*/ CTestAction* CSetPolicy::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSetPolicy::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CTestAction* CSetPolicy::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CSetPolicy* self = new (ELeave) CSetPolicy(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSetPolicy::CSetPolicy(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CKeyStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CSetPolicy::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;

	ASSERT(aTestActionSpec.iActionType == KSetUsePolicy || aTestActionSpec.iActionType == KSetManagementPolicy);
	iTestUsePolicy = (aTestActionSpec.iActionType == KSetUsePolicy);

	TPtrC8 policySpec = Input::ParseElement(aTestActionSpec.iActionBody, KPolicyStart);
	if (policySpec == KNullDesC8)
		{
		User::Leave(KErrArgument);
		}
	Input::ParseSecurityPolicyL(policySpec, iNewPolicy);
	
	iState = EListKeys;
	}

CSetPolicy::~CSetPolicy()
	{
	iKeys.Close();
	}

void CSetPolicy::PerformAction(TRequestStatus& aStatus)
	{
    if (aStatus != KErrNone)
        {
        iState = EFinished;
        }
    
	switch (iState)
		{	
		case EListKeys:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);
			
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = ESetPolicy;
			}
			break;

		case ESetPolicy:
			{
			// find the key				
			CCTKeyInfo* key = NULL;
			for (TInt i = 0 ; i < iKeys.Count() ; i++)
				{
				if (iKeys[i]->Label()==*iLabel)
					{
					key = iKeys[i];
					break;
					}
				}

			iState = EFinished;
			if (!key)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);				
				}
			else
				{
				// set the policy
				CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
				ASSERT(keyStore);

				if (iTestUsePolicy)
					{
					keyStore->SetUsePolicy(*key, iNewPolicy, aStatus);
					}
				else
					{
					keyStore->SetManagementPolicy(*key, iNewPolicy, aStatus);
					}
				}
			}
			break;
		
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus.Int() == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			
			iActionState = EPostrequisite;
			}
			break;
			
		default:
			ASSERT(EFalse);	
		}
	}

void CSetPolicy::PerformCancel()
	{
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](0);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelList();
        }
	}

void CSetPolicy::Reset()
	{
	iState = EListKeys;
	iKeys.Close();
	}

void CSetPolicy::DoReportAction()
	{
	_LIT(KSettingUsePolicy, "Setting use policy...");
	_LIT(KSettingManagementPolicy, "Setting management policy...");
	_LIT(KExpect, "New policy: ");

	if (iTestUsePolicy)
		{			
		iOut.writeString(KSettingUsePolicy);
		}
	else
		{			
		iOut.writeString(KSettingManagementPolicy);
		}
	iOut.writeNewLine();
	iOut.writeString(KExpect);
	iOut.writeSecurityPolicyL(iNewPolicy);
	iOut.writeNewLine();
	}

void CSetPolicy::DoCheckResult(TInt)
	{
	}
