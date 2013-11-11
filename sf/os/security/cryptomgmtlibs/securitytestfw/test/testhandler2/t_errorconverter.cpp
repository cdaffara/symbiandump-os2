/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* T_CErrorConverter.CPP
*
*/


#include <s32file.h>
#include "t_errorconverter.h"
#include <securityerr.h>


class CErrorItem : public CBase
	{
	public:
		static CErrorItem *NewLC(const TDesC &aErrorName, const TInt aError);
		static CErrorItem *NewL(const TDesC &aErrorName, const TInt aError);
		virtual ~CErrorItem(void) { delete iErrorName;};

	public:
		HBufC8* iErrorName;
		const TInt iError;

	private:
		void ConstructL(const TDesC &aErrorName);
		CErrorItem(const TInt aError) : iError(aError){}
	};

CErrorItem *CErrorItem::NewLC(const TDesC &aErrorName, const TInt aError)
	{
	CErrorItem *errorItem = new (ELeave) CErrorItem(aError);
	CleanupStack::PushL(errorItem);
	errorItem->ConstructL(aErrorName);
	return(errorItem);
	}

CErrorItem *CErrorItem::NewL(const TDesC &aErrorName, const TInt aError)
	{
	CErrorItem *errorItem = CErrorItem::NewLC(aErrorName, aError);
	CleanupStack::Pop(errorItem);
	return(errorItem);
	};

void CErrorItem::ConstructL(const TDesC &aErrorName)
	{
	iErrorName = HBufC8::NewL(aErrorName.Length());
	iErrorName->Des().Copy(aErrorName);
	};

EXPORT_C CErrorConverter* CErrorConverter::NewLC(void)
	{
	CErrorConverter *errorConverter = new (ELeave) CErrorConverter();
	CleanupStack::PushL(errorConverter);
	errorConverter->ConstructL();
	return(errorConverter);
	};
EXPORT_C CErrorConverter* CErrorConverter::NewL(void)
	{
	CErrorConverter *errorConverter = CErrorConverter::NewLC();
	CleanupStack::Pop(errorConverter);
	return(errorConverter);
	};

EXPORT_C CErrorConverter::~CErrorConverter()
	{
	iErrorMap.ResetAndDestroy();
	};

void CErrorConverter::ConstructL(void)
	{
	CreateErrorMapL();
	};

EXPORT_C void CErrorConverter::AddErrorItem(const TDesC& aErrorName, const TInt &aErrorCode)
	{
	CErrorItem *errorItem = CErrorItem::NewLC(aErrorName, aErrorCode);

	User::LeaveIfError(iErrorMap.Append(errorItem));
	CleanupStack::Pop(errorItem);
	};

void CErrorConverter::CreateErrorMapL()
	{
	AddErrorItem(_L("KErrNone"), KErrNone);
	AddErrorItem(_L("KErrNotFound"), KErrNotFound);
	AddErrorItem(_L("KErrGeneral"), KErrGeneral);
	AddErrorItem(_L("KErrCancel"), KErrCancel);
	AddErrorItem(_L("KErrNoMemory"), KErrNoMemory);
	AddErrorItem(_L("KErrNotSupported"), KErrNotSupported);
	AddErrorItem(_L("KErrArgument"), KErrArgument);
	AddErrorItem(_L("KErrTotalLossOfPrecision"), KErrTotalLossOfPrecision);
	AddErrorItem(_L("KErrBadHandle"), KErrBadHandle);
	AddErrorItem(_L("KErrOverflow"), KErrOverflow);
	AddErrorItem(_L("KErrUnderflow"), KErrUnderflow);
	AddErrorItem(_L("KErrAlreadyExists"), KErrAlreadyExists);
	AddErrorItem(_L("KErrPathNotFound"), KErrPathNotFound);
	AddErrorItem(_L("KErrDied"), KErrDied);
	AddErrorItem(_L("KErrInUse"), KErrInUse);
	AddErrorItem(_L("KErrServerTerminated"), KErrServerTerminated);
	AddErrorItem(_L("KErrServerBusy"), KErrServerBusy);
	AddErrorItem(_L("KErrCompletion"), KErrCompletion);
	AddErrorItem(_L("KErrNotReady"), KErrNotReady);
	AddErrorItem(_L("KErrUnknown"), KErrUnknown);
	AddErrorItem(_L("KErrCorrupt"), KErrCorrupt);
	AddErrorItem(_L("KErrAccessDenied"), KErrAccessDenied);
	AddErrorItem(_L("KErrLocked"), KErrLocked);
	AddErrorItem(_L("KErrWrite"), KErrWrite);
	AddErrorItem(_L("KErrDisMounted"), KErrDisMounted);
	AddErrorItem(_L("KErrEof"), KErrEof);
	AddErrorItem(_L("KErrDiskFull"), KErrDiskFull);
	AddErrorItem(_L("KErrBadDriver"), KErrBadDriver);
	AddErrorItem(_L("KErrBadName"), KErrBadName);
	AddErrorItem(_L("KErrCommsLineFail"), KErrCommsLineFail);
	AddErrorItem(_L("KErrCommsFrame"), KErrCommsFrame);
	AddErrorItem(_L("KErrCommsOverrun"), KErrCommsOverrun);
	AddErrorItem(_L("KErrCommsParity"), KErrCommsParity);
	AddErrorItem(_L("KErrTimedOut"), KErrTimedOut);
	AddErrorItem(_L("KErrCouldNotConnect"), KErrCouldNotConnect);
	AddErrorItem(_L("KErrCouldNotDisconnect"), KErrCouldNotDisconnect);
	AddErrorItem(_L("KErrDisconnected"), KErrDisconnected);
	AddErrorItem(_L("KErrBadLibraryEntryPoint"), KErrBadLibraryEntryPoint);
	AddErrorItem(_L("KErrBadDescriptor"), KErrBadDescriptor);
	AddErrorItem(_L("KErrAbort"), KErrAbort);
	AddErrorItem(_L("KErrTooBig"), KErrTooBig);
	AddErrorItem(_L("KErrDivideByZero"), KErrDivideByZero);
	AddErrorItem(_L("KErrBadPower"), KErrBadPower);
	AddErrorItem(_L("KErrDirFull"), KErrDirFull);
	AddErrorItem(_L("KErrHardwareNotAvailable"), KErrHardwareNotAvailable);
	AddErrorItem(_L("KErrPrivateKeyNotFound"), KErrPrivateKeyNotFound);
	AddErrorItem(_L("KErrPermissionDenied"), KErrPermissionDenied);
	}

EXPORT_C TBool CErrorConverter::GetExpectedResultL(const TDesC& aResult /* in */, TInt &aError /* out */)
	{
	HBufC8 *resultBuf = HBufC8::NewLC(aResult.Length());
	resultBuf->Des().Copy(aResult);
	TBool result = EFalse;
	CErrorItem *pError;
	TInt errorPos=0;

	aError = KErrUnknown;

	for(errorPos = 0; errorPos < iErrorMap.Count() && !result; errorPos++)
		{
		pError = iErrorMap[errorPos];
		if(pError->iErrorName->CompareF(*resultBuf)==0)
			{
			aError = pError->iError;
			result = ETrue;
			};
		}
	CleanupStack::PopAndDestroy(resultBuf);
	
	return(result);
	}


EXPORT_C TBool CErrorConverter::GetExpectedResultL(const TInt &aError /* in */, HBufC*& aResult /* out */)
	{
	TBool result = EFalse;
	TInt errorPos=0;
	CErrorItem *pError;

	__ASSERT_DEBUG(!result, User::Panic(_L("CErrorConverter::GetExpectedResultL"), 1));
	for(errorPos = 0; errorPos < iErrorMap.Count() && !result; errorPos++)
		{
		pError = iErrorMap[errorPos];
		if(pError->iError == aError)
			{
			aResult = HBufC::NewLC(pError->iErrorName->Length());
			aResult->Des().Copy(*pError->iErrorName);
			result = ETrue;
			};
		}
	// just so it always has something to pop even if it didnt find error
	if(!result)
		{
		aResult = HBufC::NewLC(0);
		};

	return(result);
	}


