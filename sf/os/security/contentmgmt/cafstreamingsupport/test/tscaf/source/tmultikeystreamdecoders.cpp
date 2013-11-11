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
// Implements the Multiple Key Stream Decoder Test Case
// 
//

#include "tmultikeystreamdecoders.h"

#include "testprotstrdesc.h"

using namespace StreamAccess;

CScafMultiKeyStreamDecoders::CScafMultiKeyStreamDecoders(CScafServer& aParent): CScafStep(aParent)
/**
 * Constructor
 */
	{
	SetTestStepName(KScafMultiKeyStreamDecoders);
	}

CScafMultiKeyStreamDecoders::~CScafMultiKeyStreamDecoders()
/**
 * Destructor
 */
	{
	}

TVerdict CScafMultiKeyStreamDecoders::doTestStepPreambleL()
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

TVerdict CScafMultiKeyStreamDecoders::doTestL()
	{
	// Assume the test has failed if the test step result hasn't been otherwise set to EPass
	SetTestStepResult(EFail);
	
	// Retrieval of expected values for Key stream Decoder "A"
	TExpectedKeyStreamDecoderAttributes expectedAttributesA = iExpectedKeyStreamDecoderData[0];
	
	// Retrieval of expected values for Key stream Decoder "B"	
	TExpectedKeyStreamDecoderAttributes expectedAttributesB = iExpectedKeyStreamDecoderData[1];
	
	//Create an SDP document object and set the created key stream field object
	CSdpDocument* sdpDoc = CreateSdpDocumentLC();
	
	// Generate the 2 SDP media field objects from the attributes with the INI configuration
	CSdpMediaField* sdpA = CreateSdpLC(0);
	AddMediaFieldL(*sdpDoc, sdpA);
	CleanupStack::Pop(sdpA);
	
	CSdpMediaField* sdpB = CreateSdpLC(1);
	AddMediaFieldL(*sdpDoc, sdpB);
	CleanupStack::Pop(sdpB);
	
	// Generate the 2 protected key stream description objects for each of the key stream decoders. 
	HBufC* filePathA;
	if(iDecoderConfigurationArray[0]->iPrivateFolderPath.Length())
		{
		filePathA = GetFullPathLC(iDecoderConfigurationArray[0]->iPrivateFolderPath, KNullDesC);
		}
	else
		{
		filePathA = GetFullPathLC(KStaPrivateFolder, KNullDesC);
		}
	CTestProtectedStreamDesc* protectStreamDescA = CTestProtectedStreamDesc::NewL(*filePathA);
	CleanupStack::PopAndDestroy(filePathA);
	CleanupStack::PushL(protectStreamDescA);
	
	HBufC* filePathB;
	if(iDecoderConfigurationArray[1]->iPrivateFolderPath.Length())
		{
		filePathB = GetFullPathLC(iDecoderConfigurationArray[1]->iPrivateFolderPath, KNullDesC);
		}
	else
		{
		filePathB = GetFullPathLC(KStaPrivateFolder, KNullDesC);
		}
	CTestProtectedStreamDesc* protectStreamDescB = CTestProtectedStreamDesc::NewL(*filePathB);
	CleanupStack::PopAndDestroy(filePathB);
	CleanupStack::PushL(protectStreamDescB);
	
	/* Create key stream decoder A using the generated protected key stream description
	 * and SDP media description A
	 */
	CKeyStreamDecoder* decoderA = NULL;
	
	TRAPD(err,decoderA = CKeyStreamDecoder::NewL(*protectStreamDescA, *sdpA, *sdpDoc)); 
	
	if(!decoderA || err != KErrNone)
		{
		ERR_PRINTF3(_L("*** Thread %d: FAIL - Unsuccessful Construction of Key Stream Decoder A - %d ***"),iThreadId,err);
		User::Leave(err);
		}
	
	CleanupStack::PushL(decoderA);
	
	
	/* Create key stream decoder B using the generated protected key stream description
	 * and SDP media description B
	 */
	CKeyStreamDecoder* decoderB = NULL;
	
	TRAP(err,decoderB = CKeyStreamDecoder::NewL(*protectStreamDescB, *sdpB, *sdpDoc)); 
	
	if(!decoderB || err != KErrNone)
		{
		ERR_PRINTF3(_L("*** Thread %d: FAIL - Unsuccessful Construction of Key Stream Decoder B - %d ***"),iThreadId,err);
		User::Leave(err);
		}
	
	CleanupStack::PushL(decoderB);
	
		
	/* Check that the retrieved attribute values from either key stream decoder
	 * match the expected results stated within the configuration file
	 */
	TBool attributeCheckResult = ETrue;
	
	
	INFO_PRINTF2(_L("Thread %d: Checking Attributes for Key Stream Decoder A..."),iThreadId);
	if(!CheckKeyStreamDecoderAttributesL(*decoderA, expectedAttributesA))
		{
		ERR_PRINTF2(_L("*** Thread %d: FAIL - Key Stream Decoder A attributes mismatch expected values ***"),iThreadId);
		attributeCheckResult = EFalse;
		}
	
	
	INFO_PRINTF2(_L("Thread %d: Checking Attributes for Key Stream Decoder B..."),iThreadId);
	if(!CheckKeyStreamDecoderAttributesL(*decoderB, expectedAttributesB))
		{
		ERR_PRINTF2(_L("*** Thread %d: FAIL - Key Stream Decoder B attributes mismatch expected values ***"),iThreadId);
		attributeCheckResult = EFalse;
		}
	
	if(attributeCheckResult)
		{
		INFO_PRINTF2(_L("*** Thread %d: SUCCESS - Attributes associated with Key Stream Decoders A & B match expected values ***"),iThreadId);
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(5, sdpDoc); //< decoderB, decoderA, protectStreamDescB, protectStreamDescA, sdpDoc
	return TestStepResult();	
	}

TVerdict CScafMultiKeyStreamDecoders::doTestStepPostambleL()
	{
	iDecoderConfigurationArray.ResetAndDestroy();
	iExpectedKeyStreamDecoderData.Close();
	
	INFO_PRINTF3(_L("Thread %d - HEAP CELLS: %d"),iThreadId,User::CountAllocCells());
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
