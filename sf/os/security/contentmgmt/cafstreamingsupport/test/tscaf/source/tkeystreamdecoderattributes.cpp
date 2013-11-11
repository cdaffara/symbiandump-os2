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
// Implements the Key Stream Decoder Query Attribute Test Case
// 
//

#include "tkeystreamdecoderattributes.h"

#include "testprotstrdesc.h"
#include <caf/streaming/keystreamdecoder.h>

using namespace StreamAccess;

CScafKeyStreamDecoderAttributes::CScafKeyStreamDecoderAttributes(CScafServer& aParent) : CScafStep(aParent)
/**
 * Constructor
 */
	{
	SetTestStepName(KScafKeyStreamDecoderAttributes);
	}

CScafKeyStreamDecoderAttributes::~CScafKeyStreamDecoderAttributes()
/**
 * Destructor
 */
	{
	}

TVerdict CScafKeyStreamDecoderAttributes::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	
	// Reads the Thread ID
	RThread thread;
	iThreadId = thread.Id();
	thread.Close();
	
	INFO_PRINTF3(_L("Thread %d - HEAP CELLS: %d"),iThreadId,User::CountAllocCells());
	
	ReadTestConfigurationL();
	
	return TestStepResult();
	}

TVerdict CScafKeyStreamDecoderAttributes::doTestL()
	{
	// Assume faliure, unless all is successful
	SetTestStepResult(EFail);
	
	// Retrieve the expected values from the array constructed using ini configuration data
	TExpectedKeyStreamDecoderAttributes expectedAttributes = iExpectedKeyStreamDecoderData[0];
	
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
	
	if(!decoder || err != KErrNone)
		{
		ERR_PRINTF3(_L("*** Thread %d: FAIL - Unsuccessful Construction of Key Stream Decoder - %d ***"),iThreadId,err);
		User::Leave(err);
		}
		
	CleanupStack::PushL(decoder);
	
	if(CheckKeyStreamDecoderAttributesL(*decoder, expectedAttributes))
		{
		INFO_PRINTF2(_L("*** Thread %d: SUCCESS - Key Stream Decoder Attributes Match Expected Values ***"),iThreadId);
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(3, sdpDoc); //< decoder, protectStreamDesc, sdpDoc
	return TestStepResult();
	}

TVerdict CScafKeyStreamDecoderAttributes::doTestStepPostambleL()
	{
	iDecoderConfigurationArray.ResetAndDestroy();
	iExpectedKeyStreamDecoderData.Close();
	
	INFO_PRINTF3(_L("Thread %d - HEAP CELLS: %d"),iThreadId,User::CountAllocCells());
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
