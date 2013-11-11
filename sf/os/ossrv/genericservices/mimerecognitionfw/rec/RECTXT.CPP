// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <apmrec.h>
#include <apmstd.h>
#include "RECTXT.H"
#include <ecom/implementationproxy.h>

const TUid KUidMimeTxtRecognizer={0x100012FB};
const TInt KMinBufferLength=42;  // minimum amount of file needed to determine a text file IF it's not called .TXT
const TInt KMaxBufferLength=1024; // maximum amount of buffer space we will ever use
_LIT8(KDataTypeTextPlain,"text/plain");
_LIT(KTextFileExt,".txt");

CApaTextRecognizer::CApaTextRecognizer()
	:CApaDataRecognizerType(KUidMimeTxtRecognizer,CApaDataRecognizerType::ELow)
	// Text files are low recognition - they don't have a clear signature
	{
	iCountDataTypes=1;
	}

TUint CApaTextRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

#if defined(_DEBUG)
TDataType CApaTextRecognizer::SupportedDataTypeL(TInt aIndex) const
#else
TDataType CApaTextRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
#endif
	{
	__ASSERT_DEBUG(aIndex==0,User::Invariant());
	return TDataType(KDataTypeTextPlain);
	}

void CApaTextRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	TBool nameRecognized=EFalse;
    
	// check if the file has valid UIDs 
	if (aBuffer.Length() >= 16)
		{
		// if the first 3 bytes are valid UIDs,then this file is not a plain/text. 
		// Set iConfidence appropriately and exit.
		const TCheckedUid checkUid(aBuffer.Left(16));    
		if (checkUid.UidType().IsValid())
			{
			iConfidence=ENotRecognized;
			return;
			}
		}

	if (aName.Length()>4)
		{
		nameRecognized=(aName.Right(4).CompareF(KTextFileExt)==0);
		}
	const TInt length=Min(aBuffer.Length(), KMaxBufferLength);
	if (length<KMinBufferLength && !nameRecognized)
		return;
	TInt ii;
	for (ii=0; ii<length; ii++)
		{
		const TUint chr=aBuffer[ii];
		// these are guesses of what WON'T be in a text file
		if (chr<9 || chr==11 || chr==12 || (chr>13 && chr<32))
			{
			break;
			}
		if (chr>148)
			{
			break;
			}
		}
	const TBool validChars=(ii==length);
	
	if (nameRecognized)
		{
		iConfidence=validChars? EProbable : EUnlikely;
		}
	else
		{
		if (!validChars)
			{
			return;
			}
		iConfidence=EPossible;
		}
	iDataType=TDataType(KDataTypeTextPlain);
	}

CApaDataRecognizerType* CApaTextRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CApaTextRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(0x101F7DA0,CApaTextRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }


