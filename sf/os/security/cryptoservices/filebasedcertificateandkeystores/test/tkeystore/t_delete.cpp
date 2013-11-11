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

/*static*/ CTestAction* CDeleteKeys::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CDeleteKeys::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CDeleteKeys::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CDeleteKeys* self = new (ELeave) CDeleteKeys(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CDeleteKeys::~CDeleteKeys()
{
	iKeys.Close();
}

void CDeleteKeys::PerformAction(TRequestStatus& aStatus)
{
    if (aStatus != KErrNone)
        {
        iState = EFinished;
        }

	CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
	ASSERT(keyStore);

    switch (iState)
		{		
		case EListKeys:
			keyStore->List(iKeys, iFilter, aStatus);

			if (*iLabel != KNullDesC)				
				iState = EDeleteKeyByLabel;
			else if (iNonExistentKey)
				iState = EDeleteNonExistentKey;
			else
				iState = EDeleteAllKeys;
				
			break;

		case EDeleteKeyByLabel:
			for ( ; iDeleteIndex < iKeys.Count() ; ++iDeleteIndex)
				{
				CCTKeyInfo* key = iKeys[iDeleteIndex];
				if (key->Label() == *iLabel)
					{
					keyStore->DeleteKey(*key, aStatus);
					++iDeletedCount;
					break;
					}
				}

			if (iDeleteIndex == iKeys.Count())
				{
				TRequestStatus* status = &aStatus;				
				User::RequestComplete(status, KErrNotFound);
				}
			
			iState = EFinished;
			break;

		case EDeleteNonExistentKey:
			{
			// Skip test if we want to delete a non-existant key, but there are
			// no keys present to get token handle from - this happens if
			// previous tests fail
			if (iKeys.Count() == 0)
				{
				_LIT(KSkippingTest, "!!!No keys present, skipping delete non-existant key test!!!\n");
				iConsole.Write(KSkippingTest);
				iOut.writeString(KSkippingTest);
			    iActionState = EPostrequisite;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNone);
				break;
				}

			CCTKeyInfo* keyInfo = iKeys[0];
			iDeleteHandle = keyInfo->Handle();
			if (iNonExistentKey > 0)
				{
				//	Modify the handle to a key that will not exist
				iDeleteHandle.iObjectId = 0xfbadcafe;
				}

			iState = EFinished;			
			keyStore->DeleteKey(iDeleteHandle, aStatus); 				
			}
			break;

		case EDeleteAllKeys:
			if (iDeleteIndex < iKeys.Count())
				{
				iState = EDeleteAllKeys;
				keyStore->DeleteKey(*iKeys[iDeleteIndex], aStatus);
				++iDeleteIndex;
				++iDeletedCount;
				}
			else
				{
				iState = EFinished;
				TRequestStatus* status = &aStatus;				
				User::RequestComplete(status, KErrNone);
				}
			break;			

		case EFinished:
			{
            iOut.writeString(_L("Delete: initial key count == "));
            iOut.writeNum(iKeys.Count());
            iOut.writeNewLine();
            iOut.writeString(_L("Delete: delete count == "));
            iOut.writeNum(iDeletedCount);
            iOut.writeNewLine();			
            iOut.writeString(_L("Delete: status == "));
            iOut.writeNum(aStatus.Int());
            iOut.writeNewLine();

			if (aStatus != KErrNone && iDeletedCount > 0)
				{
				// Attempt to delete key failed, adjust count
				--iDeletedCount;
				}
            
			iResult = (aStatus == iExpectedResult) &&
				(iExpectedDeleteCount == iDeletedCount || iExpectedDeleteCount == -1);
			
			iActionState = EPostrequisite;
            
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;
        
		default:
			User::Invariant();	
	}
}

void CDeleteKeys::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
	keystore->Cancel();
    }

void CDeleteKeys::Reset()
	{
	iState = EListKeys;
	iDeleteIndex = 0;
	iDeletedCount = 0;
	iKeys.Close();
	}

void CDeleteKeys::DoReportAction()
{
	_LIT(KDeleting, "Deleting...");
	iOut.writeString(KDeleting);
	iOut.writeNewLine();
}

void CDeleteKeys::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			if (iExpectedDeleteCount!=-1)
			{
				_LIT(KSuccessful, "%d Key(s) deleted successfully (expected to delete %d)\n");			
				buf.Format(KSuccessful, iDeletedCount, iExpectedDeleteCount);
				iConsole.Write(buf);
				iOut.writeString(buf);
				iOut.writeNewLine();
			}
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				if (iExpectedDeleteCount!=-1)
				{
					_LIT(KFailed, "!!!Key delete failure %d!!!\n");
					buf.Format(KFailed, aError);
					iConsole.Write(buf);
					iOut.writeString(buf);
				}
			}
			else
			{
				_LIT(KFailed, "Key delete failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CDeleteKeys::CDeleteKeys(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
 :	CKeyStoreTestAction(aFs, aConsole, aOut), 
	iInitialKeyCount(-1), 
	iFinalKeyCount(0),
	iDeletedCount(0),
	iDeleteIndex(0),
	iNonExistentKey(0),
	iExpectedDeleteCount(-1)
{}

void CDeleteKeys::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	iFilter.iKeyAlgorithm = iAlgorithm;

	TPtrC8 buf(0,0);
	buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, KNonExistentKeyStart));
	if (buf.Length() != 0)
		SetNonExistent(buf);

	buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, KDeleteCountStart));
	if (buf.Length() != 0)
		SetDeleteCount(buf);
	
	buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, KOwnerStart));
	if (buf.Length() != 0)
		{
		User::Leave(KErrNotSupported);
		}

	if (*iLabel != KNullDesC || iNonExistentKey)
		{
		iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
		}
	else
		{		
		iFilter.iPolicyFilter = TCTKeyAttributeFilter::EManageableKeys;
		}

	iState = EListKeys;
}


void CDeleteKeys::SetNonExistent(const TDesC8& aNonExistent)
{
	TLex8 lexer(aNonExistent);
	lexer.Val(iNonExistentKey);
}

void CDeleteKeys::SetDeleteCount(const TDesC8& aDeleteCount)
{
	TLex8 lexer(aDeleteCount);
	lexer.Val(iExpectedDeleteCount);
}
