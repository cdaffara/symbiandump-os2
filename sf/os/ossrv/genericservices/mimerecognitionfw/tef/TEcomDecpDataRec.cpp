// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <eikenv.h>
#include <bautils.h>
#include <f32file.h>
#include <ecom/implementationproxy.h> 

#include "TEcomDecpDataRec.h"

const TInt KNumDataTypes = 1;
const TUid KUid = {0x102032A2};


const TInt KTestDataRecognizerError=-420;  // some random number for test error code
_LIT(KDeceptiveRecognizerToken, "c:\\test\\appfwk\\emime\\deceptive_recognizer.token");


CApaDataRecognizerType* CTEcomDecpDataRec::CreateRecognizerL()
	{
    return new (ELeave) CTEcomDecpDataRec;
	}

CTEcomDecpDataRec::CTEcomDecpDataRec():CApaDataRecognizerType(KUid, CApaDataRecognizerType::EHigh)
	{
    iCountDataTypes = KNumDataTypes;
	}

TUint CTEcomDecpDataRec::PreferredBufSize()
	{
    return 0;
	}

TDataType CTEcomDecpDataRec::SupportedDataTypeL(TInt /*lIndex*/) const
	{
	return TDataType(_L8("deceptivedata/app"));
	}

void CTEcomDecpDataRec::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& /*aBuffer*/)
	{
	// KTestDataRecognizerError - Random error code chosen to simulate a leave in a recognizer which can be used for testing.
	//
	// NOTE: To avoid leaving with KTestDataRecognizerError whenever a file is not recognized, a token has been used,
	//		 so that only when running emime tests this leaving will occur (i.e, the token is created/destroyed during such tests).
	//		 Otherwise, the DoRecognizeL does nothing.
	
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	if (BaflUtils::FileExists(fs, KDeceptiveRecognizerToken))
		{
		User::Leave(KTestDataRecognizerError);
		}

 	CleanupStack::PopAndDestroy(&fs);
 	}
