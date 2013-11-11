// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// tkeystreamdecoderconstruct.h
// Implements the Key Stream Decoder Construction Test Case
// 
//

#include "tkeystreamdecoderconstruct.h"

#include "testprotstrdesc.h"
#include <caf/streaming/keystreamdecoder.h>

using namespace StreamAccess;

CScafKeyStreamDecoderConstruct::CScafKeyStreamDecoderConstruct(CScafServer& aParent): CScafStep(aParent)
/**
 * Constructor
 */
	{
	SetTestStepName(KScafKeyStreamDecoderConstruct);
	}

CScafKeyStreamDecoderConstruct::~CScafKeyStreamDecoderConstruct()
/**
 * Destructor
 */
	{
	}

TVerdict CScafKeyStreamDecoderConstruct::doTestL()
	{
	// Assume the test has failed if the test step result hasn't been otherwise set to EPass
	SetTestStepResult(EFail);
	
	//Create an SDP document object and set the created key stream field object
	CSdpDocument* sdpDoc = CreateSdpDocumentLC();
	
	// Create an SDP media field object from the attributes with the INI configuration section
	CSdpMediaField* sdp = CreateSdpLC(0);
	AddMediaFieldL(*sdpDoc, sdp);
	CleanupStack::Pop(sdp);
	
	// Generate a protected key stream description. 
	HBufC* filePath;
	
	if(iDecoderConfigurationArray[0]->iPrivateFolderPath.Length())
		{
		filePath = GetFullPathLC(iDecoderConfigurationArray[0]->iPrivateFolderPath, KNullDesC);
		}
	else
		{
		filePath = GetFullPathLC(KStaPrivateFolder, KNullDesC);
		}
	
	CTestProtectedStreamDesc* protectStreamDesc = CTestProtectedStreamDesc::NewL(*filePath);
	CleanupStack::PopAndDestroy(filePath);
	CleanupStack::PushL(protectStreamDesc);
	
	// Create a key stream decoder
	CKeyStreamDecoder* decoder = NULL;
	
	TRAPD(err,decoder = CKeyStreamDecoder::NewL(*protectStreamDesc, *sdp, *sdpDoc)); 
	
	if(decoder && (err == KErrNone))
		{
		delete decoder;
		INFO_PRINTF1(_L("*** SUCCESS: Key Stream Decoder Successfully Constructed ***"));
		SetTestStepResult(EPass);
		}
	else
		{
		ERR_PRINTF1(_L("*** FAIL: Unsuccessful Construction of Key Stream Decoder ***"));
		User::Leave(err);
		}
	
	// Delete the protected stream descriptor
	CleanupStack::PopAndDestroy(2, sdpDoc);	//< protectStreamDesc, sdpDoc
	return TestStepResult();
	}
