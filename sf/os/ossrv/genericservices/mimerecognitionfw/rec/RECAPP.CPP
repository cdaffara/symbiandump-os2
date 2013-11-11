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
#include "RECAPP.H"
#include <ecom/implementationproxy.h> 

//
// This is a very odd recognizer - it doesn't know how many things it recognizes
// It assumes there are none.
//

const TInt KMimeAppRecognizerValue=0x1000415F;
const TUid KUidMimeAppRecognizer={KMimeAppRecognizerValue};
const TInt KAppNumMimeTypes=0;

CApaAppRecognizer::CApaAppRecognizer()
	:CApaDataRecognizerType(KUidMimeAppRecognizer,CApaDataRecognizerType::EHigh)
	// All these mime types have reasonable recognition
	{
	iCountDataTypes=KAppNumMimeTypes;
	}

TUint CApaAppRecognizer::PreferredBufSize()
	{
	return 16;
	}

TDataType CApaAppRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType();
	}

void CApaAppRecognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& aBuffer)
	{
	if (aBuffer.Length()>=16)
		{
		const TUidType uids(TCheckedUid(aBuffer.Left(16)).UidType());
#if defined(_UNICODE)
		if (uids[1].iUid==0x10003a12)
#else
		if (uids[1].iUid==0x1000006d)
#endif
 			{
	 		iDataType=TDataType(uids[2]);
	 		iConfidence=ECertain;
	 		}
		}
	}

CApaDataRecognizerType* CApaAppRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CApaAppRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(0x101F7D9F,CApaAppRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }




GLDEF_C TInt E32Dll(
					)
//
// DLL entry point
//
	{
	return KErrNone;
	}
