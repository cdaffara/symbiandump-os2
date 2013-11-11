/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include <e32uid.h>
#include <mctauthobject.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"

/////////////////////////////////////////////////////////////////////////////////
// CAuthObjectTest
/////////////////////////////////////////////////////////////////////////////////

template <class TTestImpl>
CTestAction* CAuthObjectTest<TTestImpl>::NewL(RFs& aFs, 
											  CConsoleBase& aConsole, 
											  Output& aOut,
											  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CAuthObjectTest<TTestImpl>::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

template <class TTestImpl>
CTestAction* CAuthObjectTest<TTestImpl>::NewLC(RFs& aFs,
											   CConsoleBase& aConsole, 
											   Output& aOut,
											   const TTestActionSpec& aTestActionSpec)
	{
	CAuthObjectTest<TTestImpl>* self = new(ELeave) CAuthObjectTest<TTestImpl>(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

template <class TTestImpl>
CAuthObjectTest<TTestImpl>::CAuthObjectTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CKeyStoreTestAction(aFs, aConsole, aOut), iState(EListKeys)
	{
	}

template <class TTestImpl>
void CAuthObjectTest<TTestImpl>::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	iImpl.ConstructL(aTestActionSpec);
	}

template <class TTestImpl>
CAuthObjectTest<TTestImpl>::~CAuthObjectTest()
	{
	iKeys.Close();
	}

template <class TTestImpl>
void CAuthObjectTest<TTestImpl>::PerformAction(TRequestStatus& aStatus)
	{
    if (aStatus != KErrNone)
        {
        iState = EFinished;
        }
    
	switch (iState)
		{
		case EListKeys:
			{
			CUnifiedKeyStore& keystore = *CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);

			TCTKeyAttributeFilter filter;
			keystore.List(iKeys, filter, aStatus);			
			iState = EMain;
			break;
			}

		case EMain:
			{
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			
			if (iKeys.Count() == 0)
				{
				User::RequestComplete(status, KErrNotFound);
				return;
				}

			const CCTKeyInfo* key = NULL;
			for (TInt index = 0 ; index < iKeys.Count() ; ++index)
				{
				const CCTKeyInfo* k = iKeys[index];
				if (k->Label() == *iLabel)
					{
					key = k;
					break;
					}
				}

			if (!key)
				{
				iOut.writeString(_L("Key not found: "));
				iOut.writeString(*iLabel);
				iOut.writeNewLine();
				User::RequestComplete(status, KErrNotFound);
				return;
				}

			iAuth = key->Protector();
			if (!iAuth)
				{
				User::RequestComplete(status, KErrNotFound);
				return;
				}

			iImpl.DoTest(*iAuth, aStatus);
			break;
			}

		case EFinished:
			{
			if (aStatus == KErrNone && !iImpl.CheckResult(iOut))
				{
				iOut.writeString(_L("CheckReult() returned EFalse"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}

			if (aStatus == iExpectedResult)
				{
				iOut.writeString(_L("Status: "));
				iOut.writeNum(aStatus.Int());
				iOut.writeString(_L(", expected"));
				iOut.writeNewLine();
				iResult = ETrue;
				}			
			else
				{
				iOut.writeString(_L("Status: "));
				iOut.writeNum(aStatus.Int());
				iOut.writeString(_L(", failure"));
				iOut.writeNewLine();
				iResult = EFalse;
				}
				
			iActionState = EPostrequisite;

			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			break;
			}
		}
	}

template <class TTestImpl>
void CAuthObjectTest<TTestImpl>::PerformCancel()
	{
    switch (iState)
        {
        case EMain:
            {
            CUnifiedKeyStore& keystore = *CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
            keystore.CancelList();
            }
            break;

        case EFinished:
            if (iAuth)
                {
                iImpl.DoCancel(*iAuth);
                }
            break;

        default:
            break;
        }
    }

template <class TTestImpl>
void CAuthObjectTest<TTestImpl>::Reset()
	{
	iState = EListKeys;
	iKeys.Close();
	iImpl.Reset();
    iAuth = NULL;
	}

template <class TTestImpl>
void CAuthObjectTest<TTestImpl>::DoReportAction()
	{
	iOut.writeString(_L("Running auth object test: "));
	iOut.writeString(iImpl.Name());
	iOut.writeNewLine();
	}

template <class TTestImpl>
void CAuthObjectTest<TTestImpl>::DoCheckResult(TInt /*aError*/)
	{
	}

////////////////////////////////////////////////////////////////////////////////
// TAuthObjectTest
////////////////////////////////////////////////////////////////////////////////

void TAuthObjectTest::ConstructL(const TTestActionSpec& /*aTestActionSpec*/)
	{
	}

void TAuthObjectTest::Reset()
	{
	}

TBool TAuthObjectTest::CheckResult(Output& /*aOut*/)
	{
	return ETrue;
	}

/**
 * Test changing the passphrase.
 */
const TDesC& TChangePassphrase::Name()
	{
	_LIT(KName, "Change passphrase");
	return KName;
	}

void TChangePassphrase::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.ChangeReferenceData(aStatus);
	}

void TChangePassphrase::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelChangeReferenceData();
    }

template class CAuthObjectTest<TChangePassphrase>;

/**
 * Test listing protected objects.
 */

TListProtectedObjects::~TListProtectedObjects()
	{
	iExpectedKeys.ResetAndDestroy();
	iObjects.Close();
	}

const TDesC& TListProtectedObjects::Name()
	{
	_LIT(KName, "List protected objects");
	return KName;
	}

void TListProtectedObjects::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	TInt pos = 0;
	while (AddExpectedKeyL(Input::ParseElement(aTestActionSpec.iActionBody, KFoundKeyStart, KFoundKeyEnd, pos)))
		/* do nothing */;
	}

void TListProtectedObjects::Reset()
	{
	iObjects.Close();
	}

TBool TListProtectedObjects::AddExpectedKeyL(const TDesC8& aKeyLabel)
	{
	if (aKeyLabel.Length() == 0)
		return EFalse;

	HBufC* label = HBufC::NewMaxLC(aKeyLabel.Length());
	TPtr ptr = label->Des();
	ptr.Copy(aKeyLabel);
	User::LeaveIfError(iExpectedKeys.Append(label));
	CleanupStack::Pop(label);
	return ETrue;
	}

void TListProtectedObjects::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.ListProtectedObjects(iObjects, aStatus);
	}

TBool TListProtectedObjects::CheckResult(Output& aOut)
	{
	aOut.writeString(_L("Expected "));
	aOut.writeNum(iExpectedKeys.Count());
	aOut.writeString(_L(" keys, found "));
	aOut.writeNum(iObjects.Count());
	aOut.writeNewLine();
	
	if (iObjects.Count() != iExpectedKeys.Count())
		{
		return EFalse;
		}

	for (TInt index = 0 ; index < iObjects.Count() ; ++index)
		{
		MCTTokenObject* key = iObjects[index];
		const TDesC& keyLabel = key->Label();
		HBufC* expectedLabel = iExpectedKeys[index];
		
		if (keyLabel != *expectedLabel)
			{
			aOut.writeString(_L("Expected "));
			aOut.writeString(*expectedLabel);
			aOut.writeString(_L(" but found "));
			aOut.writeString(keyLabel);
			aOut.writeNewLine();
			return EFalse;
			}
		}

	return ETrue;
	}
	
void TListProtectedObjects::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelListProtectedObjects();
    }

template class CAuthObjectTest<TListProtectedObjects>;

/**
 * Test unblocking the passphrase.
 */
const TDesC& TUnblockPassphrase::Name()
	{
	_LIT(KName, "Unblock passphrase");
	return KName;
	}

void TUnblockPassphrase::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.Unblock(aStatus);
	}
	
void TUnblockPassphrase::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelUnblock();
    }

template class CAuthObjectTest<TUnblockPassphrase>;

/**
 * Test auth object open method.
 */
const TDesC& TAuthOpen::Name()
	{
	_LIT(KName, "Auth object open");
	return KName;
	}

void TAuthOpen::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.Open(aStatus);
	}
	
void TAuthOpen::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelOpen();
    }

template class CAuthObjectTest<TAuthOpen>;

/**
 * Test auth object close method.
 */
const TDesC& TAuthClose::Name()
	{
	_LIT(KName, "Auth object close");
	return KName;
	}

void TAuthClose::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.Close(aStatus);
	}	

void TAuthClose::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelClose();
    }

template class CAuthObjectTest<TAuthClose>;

/**
 * Test get time remaining.
 */
const TDesC& TGetTimeRemaining::Name()
	{
	_LIT(KName, "Get time remaining");
	return KName;
	}

void TGetTimeRemaining::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	TPtrC8 ptr = Input::ParseElement(aTestActionSpec.iActionBody, KTimeoutStart);
	if (ptr.Length() == 0)
		User::Leave(KErrNotFound);
	TLex8 lex(ptr);
	lex.Val(iExpectedTime);
	}

void TGetTimeRemaining::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.TimeRemaining(iTimeRemaining, aStatus);	
	}

TBool TGetTimeRemaining::CheckResult(Output& aOut)
	{
	aOut.writeString(_L("Expected "));
	aOut.writeNum(iExpectedTime);
	aOut.writeString(_L(", got "));
	aOut.writeNum(iTimeRemaining);
	aOut.writeNewLine();

	// Check time remaining is within 5 seconds of expected
	TInt difference = iTimeRemaining - iExpectedTime;
	if (difference < 0)
		difference = -difference;
	return difference < 6;
	}
	
void TGetTimeRemaining::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelTimeRemaining();
    }

template class CAuthObjectTest<TGetTimeRemaining>;

/**
 * Test get timeout.
 */
const TDesC& TAuthSetTimeout::Name()
	{
	_LIT(KName, "Set timeout");
	return KName;
	}

void TAuthSetTimeout::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	TPtrC8 ptr = Input::ParseElement(aTestActionSpec.iActionBody, KTimeoutStart);
	if (ptr.Length() == 0)
		User::Leave(KErrNotFound);
	TLex8 lex(ptr);
	lex.Val(iNewTimeout);
	}

void TAuthSetTimeout::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.SetTimeout(iNewTimeout, aStatus);
	}	

void TAuthSetTimeout::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelSetTimeout();
    }

template class CAuthObjectTest<TAuthSetTimeout>;

/**
 * Test set timeout.
 */
const TDesC& TGetTimeout::Name()
	{
	_LIT(KName, "Get timeout");
	return KName;
	}

void TGetTimeout::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	TPtrC8 ptr = Input::ParseElement(aTestActionSpec.iActionBody, KTimeoutStart);
	if (ptr.Length() == 0)
		User::Leave(KErrNotFound);
	TLex8 lex(ptr);
	lex.Val(iExpectedTimeout);
	}

void TGetTimeout::DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus)
	{
	aAuth.Timeout(iTimeout, aStatus);
	}	

TBool TGetTimeout::CheckResult(Output& aOut)
	{
	aOut.writeString(_L("Expected "));
	aOut.writeNum(iExpectedTimeout);
	aOut.writeString(_L(", got "));
	aOut.writeNum(iTimeout);
	aOut.writeNewLine();

	return iExpectedTimeout == iTimeout;
	}

void TGetTimeout::DoCancel(MCTAuthenticationObject& aAuth)
    {
    aAuth.CancelTimeout();
    }

template class CAuthObjectTest<TGetTimeout>;
