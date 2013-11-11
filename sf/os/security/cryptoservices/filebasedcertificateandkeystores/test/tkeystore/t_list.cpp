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

/////////////////////////////////////////////////////////////////////////////////
// CListKeys
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CListKeys::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CListKeys::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CListKeys::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CListKeys* self = new (ELeave) CListKeys(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CListKeys::~CListKeys()
	{
	iKeys.Close();
	iKeysToFind.Close();
	delete iListStatus;
	}

CListKeys::CListKeys(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	:	CKeyStoreTestAction(aFs, aConsole, aOut), iResultCount(-1)
	{
	}


void CListKeys::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	
	iFilter.iKeyAlgorithm = iAlgorithm;
	if (iUsage == 0)
		iUsage = EPKCS15UsageAll;
	iFilter.iUsage = iUsage;
	TInt pos = 0;
	TPtrC8 buf(0,0);

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
	
	// set default outcome as success for listing keys
	SetListStatus(KErrListingSuccess);
	buf.Set(Input::ParseElement(aTestActionSpec.iActionBody, KListingStatusStart, KListingStatusEnd, pos, err ));
		if (err == KErrNone)
			SetListStatus(buf);

}



void CListKeys::SetKeySize(const TDesC8& aKeySize)
{
	TLex8 lex(aKeySize);
	lex.Val(iSize);
}


void CListKeys::SetListCount(const TDesC8& aListCount)
	{
	TLex8 lexer(aListCount);
	lexer.Val(iResultCount);
	}

void CListKeys::SetListStatus(const TDesC8& aListStatus)
	{
	if(iListStatus != NULL)
		{
		delete iListStatus;
		iListStatus = NULL;
		}
	iListStatus = aListStatus.AllocL();
	}

void CListKeys::AddFoundKeyL(const TDesC8& aFoundKey)
	{
	HBufC* buf = HBufC::NewLC(aFoundKey.Length());
	buf->Des().Copy(aFoundKey);
	User::LeaveIfError(iKeysToFind.Append(buf));
	CleanupStack::Pop(buf);
	}

void CListKeys::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{	
		case EListKeys:
			{            
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			
			keyStore->List(iKeys, iFilter, aStatus);
			iState = EFinished;
			}
			break;
			
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if( iListStatus->Compare(KErrListingFailure) == 0 )
				{
				iResult = ETrue;
				}
			else
				{
				if (aStatus == iExpectedResult && (iResultCount < 0 ||
												   iResultCount == iKeys.Count()))
					{
					iResult = ETrue;
					}
				else
					{
					iResult = EFalse;
					}
				}
			iActionState = EPostrequisite;				
			}
		}
	}

void CListKeys::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelList();
        }
    }

void CListKeys::Reset()
	{
	iState = EListKeys;
	iKeys.Close();
	}

void CListKeys::DoReportAction()
{
	_LIT(KListing, "Listing...");
	iOut.writeString(KListing);
	iOut.writeNewLine();
}

// Truncate labels after this many characters
// also declared in t_keystore_actions.cpp - create PrintKeyLabel method
static const TInt KMaxLabel = 32;
_LIT(KEllipsis, "...");

void CListKeys::DoCheckResult(TInt aError)
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

				TRAPD(err,PrintKeyInfoL(*iKeys[i])); 
				_LIT(KFailed, "!!!Key listing success!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				iOut.writeNum(err);
				iOut.writeNewLine();
				return;			
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
			_LIT(KFailed, "!!!Key listing failure!!!\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			iOut.writeNewLine();
		}
	}
}


/*static*/ CTestAction* CGetKeyInfo::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CGetKeyInfo::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CGetKeyInfo::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CGetKeyInfo* self = new (ELeave) CGetKeyInfo(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CGetKeyInfo::~CGetKeyInfo()
	{
	if (iKey)
		iKey->Release();
	iKeys.Close();
	}

void CGetKeyInfo::PerformAction(TRequestStatus& aStatus)
	{
    if (aStatus != KErrNone)
        {
        iState = EFinished;
        }
    
	switch (iState)
		{	
	case EListKeysPreGet:
		{
		CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
		ASSERT(keyStore);	//	Flag it up
		if (keyStore)
			keyStore->List(iKeys, iFilter, aStatus);
			
		iState = EGet;
		}
		break;
		
	case EGet:
		{
		CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
		ASSERT(keyStore);	//	Flag it up
			
		TInt count = iKeys.Count();
		for (TInt i = 0; i < count; i++)
			{
			if (iKeys[i]->Label()==*iLabel)
				iGetHandle = *iKeys[i];
			}
		keyStore->GetKeyInfo(iGetHandle, iKey, aStatus); 
		iState = EFinished;
		}
		break;
			
	case EFinished:
		{
		iKeys.Close();
		// Check a few things
		if (!iKey)
			{
			if (aStatus == KErrNone)
				aStatus = KErrGeneral;
			}
		else
			{
			if (iKey->Label() != *iLabel || iKey->Handle() != iGetHandle)
				{
				iOut.writeString(_L("Error: wrong handle"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iUsage != EPKCS15UsageAll && iKey->Usage() != iUsage)
				{
				iOut.writeString(_L("Error: wrong usage"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iSize && iKey->Size() != iSize)
				{
				iOut.writeString(_L("Error: wrong size"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iAlgorithm != 0 && iKey->Algorithm() != iAlgorithm)
				{
				iOut.writeString(_L("Error: wrong algorithm"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iAccessType && iKey->AccessType() != iAccessType)
				{
				iOut.writeString(_L("Error: wrong access type"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iNativeSet && iNative != iKey->Native())
				{
				iOut.writeString(_L("Error: wrong native flag"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iStartDate.Int64() != -1 && iKey->StartDate() != iStartDate)
				{
				iOut.writeString(_L("Error: wrong start date"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			if (iEndDate.Int64() != -1 && iKey->EndDate() != iEndDate)
				{
				iOut.writeString(_L("Error: wrong end date"));
				iOut.writeNewLine();
				aStatus = KErrGeneral;
				}
			}

		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, aStatus.Int());
		if (aStatus.Int()==iExpectedResult)
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

void CGetKeyInfo::PerformCancel()
	{
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](0);
    ASSERT(keystore);
    if (iState == EGet)
        {
        keystore->CancelList();
        }
	else if (iState == EFinished)
		{
		keystore->CancelGetKeyInfo();
		}
	}

void CGetKeyInfo::Reset()
	{
	iState = EListKeysPreGet;
	iKeys.Close();
	if (iKey)
		{
		iKey->Release();
		iKey = NULL;
		}
	}

void CGetKeyInfo::DoReportAction()
{
	_LIT(KGetting, "Getting...");
	iOut.writeString(KGetting);
	iOut.writeNewLine();
}

void CGetKeyInfo::DoCheckResult(TInt)
	{
    if (iFinished && iKey)
		{
		TRAPD(err,PrintKeyInfoL(*iKey)); 
		_LIT(KFailed, "!!!Key listing failure!!!\n");
		iConsole.Write(KFailed);
		iOut.writeString(KFailed);
		iOut.writeNum(err);
		iOut.writeNewLine();
		}
	}
 
CGetKeyInfo::CGetKeyInfo(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CKeyStoreTestAction(aFs, aConsole, aOut), iStartDate(-1), iEndDate(-1)
	{
	iUsage = EPKCS15UsageAll; // Use this value to flag "not set" - this is not a usage you would ever expect 
	}

void CGetKeyInfo::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	
	SetKeySize(Input::ParseElement(aTestActionSpec.iActionBody, KKeySizeStart));
	SetNativeL(Input::ParseElement(aTestActionSpec.iActionBody, KKeyNativeStart));
	SetStartDateL(Input::ParseElement(aTestActionSpec.iActionBody, KKeyStartDateStart));
	SetEndDateL(Input::ParseElement(aTestActionSpec.iActionBody, KKeyEndDateStart));
	
	iState = EListKeysPreGet;
}

void CGetKeyInfo::SetKeySize(const TDesC8& aKeySize)
{
	TLex8 lex(aKeySize);
	lex.Val(iSize);
}

void CGetKeyInfo::SetNativeL(const TDesC8& aData)
{
    if (aData.Length() == 0)
		return;
 
	TLex8 lex(aData);
	TInt isNative;
	lex.Val(isNative);
	iNativeSet = ETrue;
	if (isNative == 0)
		iNative = EFalse;
	else if (isNative == 1)
		iNative = ETrue;
	else
		User::Leave(KErrCorrupt);
}

void CGetKeyInfo::SetStartDateL(const TDesC8& aData)
	{
    if (aData.Length() == 0)
		return;

	if (aData == _L8("unset"))
		{
		iStartDate = TTime(0);
		return;
		}

	HBufC* buf = HBufC::NewLC(aData.Length());
	TPtr ptr = buf->Des();
	ptr.Copy(aData);	
	User::LeaveIfError(iStartDate.Parse(*buf));
	CleanupStack::PopAndDestroy(buf);
	}

void CGetKeyInfo::SetEndDateL(const TDesC8& aData)
	{
    if (aData.Length() == 0)
		return;

	if (aData == _L8("unset"))
		{
		iEndDate = TTime(0);
		return;
		}
	
	HBufC* buf = HBufC::NewLC(aData.Length());
	TPtr ptr = buf->Des();
	ptr.Copy(aData);	
	User::LeaveIfError(iEndDate.Parse(*buf));
	CleanupStack::PopAndDestroy(buf);
	}
