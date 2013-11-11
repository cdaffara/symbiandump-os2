// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalComponent - Internal Symbian test code 
*/

#include "TTestEcomDataRec.h"
#include "TEcomDecpDataRec.h"
#include "TEcomLeavingRecognizer.h"
#include <eikenv.h>
#include <ecom/implementationproxy.h> 


const TInt KNumDataTypes = 1;
const TUid KUid = {0x1d1f75ed};


GLDEF_C TInt E32Dll(
					)
	{
    return KErrNone;
	}


CTestEcomRecognizer::CTestEcomRecognizer():CApaDataRecognizerType(KUid, CApaDataRecognizerType::EHigh)
	{
    iCountDataTypes = KNumDataTypes;
	}

/*
	Specifies the buffer size required by the data type
*/
TUint CTestEcomRecognizer::PreferredBufSize()
	{
    return 0;
	}

TDataType CTestEcomRecognizer::SupportedDataTypeL(TInt /*lIndex*/) const
	{
	return TDataType(_L8("dodgy/app"));
	}

/*
	Should recognize the data type of the file. 
	Called by the RecognizeL function.
*/
void CTestEcomRecognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& /*aBuffer*/)
	{
	// does nothing
	}



CApaDataRecognizerType* CTestEcomRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CTestEcomRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(0x101F7DA1,CTestEcomRecognizer::CreateRecognizerL),
	IMPLEMENTATION_PROXY_ENTRY(0x102032A2,CTEcomDecpDataRec::CreateRecognizerL),
	IMPLEMENTATION_PROXY_ENTRY(0x10203630,CEcomLeavingApplicationRec::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
