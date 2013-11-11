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

#include "TEcomLeavingRecognizer.h"
#include <eikenv.h>


const TInt KNumDataTypes = 1;
const TUid KUid = {0x10203630};


CApaDataRecognizerType* CEcomLeavingApplicationRec::CreateRecognizerL()
	{
    return new (ELeave) CEcomLeavingApplicationRec;
	}

CEcomLeavingApplicationRec::CEcomLeavingApplicationRec():CApaDataRecognizerType(KUid, CApaDataRecognizerType::EHigh)
	{
    iCountDataTypes = KNumDataTypes;
	}

/*
	Specifies the buffer size required by the data type
*/
TUint CEcomLeavingApplicationRec::PreferredBufSize()
	{
    return 0;
	}

/*
	Should return one of the supported data types based on index sent.
*/
TDataType CEcomLeavingApplicationRec::SupportedDataTypeL(TInt /*lIndex*/) const
	{
	// This function is made to leave, just to reproduce the defect:
	// DEF038928 - App Arc server/ Recogniser needs to be more robust. 
	User::Leave(KErrNoMemory);
    return TDataType(_L8("dodgy/app"));
	}

/*
	Should recognize the data type of the file. 
	Called by the RecognizeL function.
*/
void CEcomLeavingApplicationRec::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& /*aBuffer*/)
	{
	// does nothing
	}
