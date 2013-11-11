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


#include <unifiedkeystore.h>
#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include "t_output.h"

_LIT(KEllipsis, "...");

/////////////////////////////////////////////////////////////////////////////////
// CAddKey
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CAddKey::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CAddKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CAddKey::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CAddKey* self = new (ELeave) CAddKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CAddKey::~CAddKey()
{
	if (iKeyInfo != NULL)
		{
		iKeyInfo->Release();
		}
}

CAddKey::CAddKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EAddKey)
	{
	}

void CAddKey::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	SetKeySize(Input::ParseElement(aTestActionSpec.iActionBody, KKeySizeStart));
	SetStartDateL(Input::ParseElement(aTestActionSpec.iActionBody, KKeyStartDateStart));
	SetEndDateL(Input::ParseElement(aTestActionSpec.iActionBody, KKeyEndDateStart));
}

void CAddKey::SetKeySize(const TDesC8& aKeySize)
	{
	TLex8 lex(aKeySize);
	lex.Val(iSize);
	}

void CAddKey::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EAddKey:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			if(iKeyStoreImplLabel.Length() != 0)
					{
					SetKeyStoreIndex(keyStore);
					}
					
			#ifdef SYMBIAN_AUTH_SERVER		
			if(iUseNewApi)
				{
				keyStore->CreateKey(iKeyStoreImplIndex, iUsage, iSize, *iLabel, iAlgorithm,
												iAccessType, iStartDate, iEndDate, *iAuthExpression, iFreshness, iKeyInfo, aStatus);
				}
			else
			#endif // SYMBIAN_AUTH_SERVER
				{
				keyStore->CreateKey(iKeyStoreImplIndex, iUsage, iSize, *iLabel, iAlgorithm,
								iAccessType, iStartDate, iEndDate, iKeyInfo, aStatus);

				}			
			iState = EFinished;
			}
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
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
			break;	//	Nothing to do, for the compiler
		}
	}

void CAddKey::PerformCancel()
    {
    if (iState == EFinished)
        {
        CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
        keyStore->CancelCreateKey();
        }
    }

void CAddKey::Reset()
	{
	iState = EAddKey;
	if (iKeyInfo != NULL)
		{
		iKeyInfo->Release();
		iKeyInfo = NULL;
		}
	}

void CAddKey::DoReportAction()
{
	_LIT(KAdding, "Adding key...");
	iOut.writeString(KAdding);

	iOut.writeString(iLabel->Left(32));
	if (iLabel->Length() > 32)
		{
		iOut.writeString(KEllipsis);
		}
	
	iOut.writeNewLine();
}


void CAddKey::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key added successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Key adding failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}
			else
			{
				_LIT(KFailed, "Key adding failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}


void CAddKey::SetStartDateL(const TDesC8& aData)
	{
    if (aData.Length() == 0)
		return;

	HBufC* buf = HBufC::NewLC(aData.Length());
	TPtr ptr = buf->Des();
	ptr.Copy(aData);	
	User::LeaveIfError(iStartDate.Parse(*buf));
	CleanupStack::PopAndDestroy(buf);
	}

void CAddKey::SetEndDateL(const TDesC8& aData)
	{
    if (aData.Length() == 0)
		return;

	HBufC* buf = HBufC::NewLC(aData.Length());
	TPtr ptr = buf->Des();
	ptr.Copy(aData);	
	User::LeaveIfError(iEndDate.Parse(*buf));
	CleanupStack::PopAndDestroy(buf);
	}
