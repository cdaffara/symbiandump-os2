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


#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"

/////////////////////////////////////////////////////////////////////////////////
// CListAllKeys
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CListAllKeys::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CListAllKeys::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CListAllKeys::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CListAllKeys* self = new (ELeave) CListAllKeys(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CListAllKeys::~CListAllKeys()
	{
	iKeys.Close();
	iKeysToFind.Close();
	}

CListAllKeys::CListAllKeys(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	:	CKeyStoreTestAction(aFs, aConsole, aOut), iResultCount(-1)
	{
	}


void CListAllKeys::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	
	iFilter.iKeyAlgorithm = iAlgorithm;
	if (iUsage == 0)
		iUsage = EPKCS15UsageAll;
	iFilter.iUsage = iUsage;
	TInt pos = 0;
	TPtrC8 buf(0,0);

	iStore = 0;

	TInt err;
	buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, KKeySizeStart, KKeySizeEnd, pos, err));
	if (err == KErrNone)
		SetKeySize(buf);

	buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, KListCountStart, KListCountEnd, pos, err));
	if (err == KErrNone)
		SetListCount(buf);

	do
		{
		buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, 
									KFoundKeyStart, KFoundKeyEnd,pos,err));
		if (err == KErrNone)
			AddFoundKeyL(buf);
		} 
	while (err == KErrNone);
}



void CListAllKeys::SetKeySize(const TDesC8& aKeySize)
{
	TLex8 lex(aKeySize);
	lex.Val(iSize);
}


void CListAllKeys::SetListCount(const TDesC8& aListCount)
	{
	TLex8 lexer(aListCount);
	lexer.Val(iResultCount);
	}

void CListAllKeys::AddFoundKeyL(const TDesC8& aFoundKey)
	{
	HBufC* buf = HBufC::NewLC(aFoundKey.Length());
	buf->Des().Copy(aFoundKey);
	User::LeaveIfError(iKeysToFind.Append(buf));
	CleanupStack::Pop(buf);
	}

void CListAllKeys::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{	
		case EListKeys:
			{      
			// get keys from the next KeyStore			      
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iStore);
			keyStore->List(iKeys, iFilter, aStatus);
			
			iStore++;
			if ( iStore < CSharedKeyStores::TheUnifiedKeyStores().Count() )
				{
				iState = EListKeys;
				}
			else
				{
				iState = EFinished;
				}
			}
			break;
			
		case EFinished:
			{
			iStore = 0;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult && (iResultCount < 0 ||
											   iResultCount == iKeys.Count()))
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			
			iActionState = EPostrequisite;				
			}
		}
	}

void CListAllKeys::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelList();
        }
    }

void CListAllKeys::Reset()
	{
	iState = EListKeys;
	iKeys.Close();
	}

void CListAllKeys::DoReportAction()
{
	_LIT(KListing, "Listing...");
	iOut.writeString(KListing);
	iOut.writeNewLine();
}

// Truncate labels after this many characters
// also declared in t_keystore_actions.cpp - create PrintKeyLabel method
static const TInt KMaxLabel = 32;
_LIT(KEllipsis, "...");

void CListAllKeys::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Keys listed successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			_LIT(KKeyCount, "%d Keys found");
			TBuf<256> buf;
			buf.Format(KKeyCount, iKeys.Count());
			iOut.writeString(buf);
			iOut.writeNewLine();

			if (iResultCount != -1)
				{
				_LIT(KKeyCount, "Expected to find %d");
				TBuf<256> buf;
				buf.Format(KKeyCount, iResultCount);
				iOut.writeString(buf);
				iOut.writeNewLine();
				}
			
			TInt i=0;
			for (; i < iKeys.Count(); i++)
				{
				_LIT(KKeyNum, "Key number: %d ");
				buf.Format(KKeyNum, i);
				iOut.writeString(buf);
				iOut.writeNewLine();

				PrintKeyInfoL(*iKeys[i]); 
				}
			
			TInt j;
			for (i = 0; i < iKeysToFind.Count(); i++)
				{
				for (j=0; j < iKeys.Count(); j++)
					{
					if (iKeys[j]->Label()== *iKeysToFind[i])
						break;
					}
				if (j == iKeys.Count())
					{
					// We didn't find that label
					_LIT(KFailedToFind, "Failed to find key with label ");
					iOut.writeString(KFailedToFind);
					iOut.writeString(iKeysToFind[i]->Left(KMaxLabel));
					if (iKeysToFind[i]->Length() > KMaxLabel)
						{
						iOut.writeString(KEllipsis);
						}
					iOut.writeNewLine();
					iResult = EFalse;
					}
				}
		}
		else
		{
		if (aError!=iExpectedResult)
			{
			_LIT(KFailed, "!!!Key listing failure!!!\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			iOut.writeNewLine();
			}
		else
			{
			iResult = ETrue;
			_LIT(KFailed, "Operation failed, but expected\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			}
		}
	}
}


