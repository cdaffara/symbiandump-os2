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
// Implements the Key Stream Sink usage tests.
// 
//

#include "tkeystreamsink.h"

using namespace StreamAccess;

//	CScafKeyStreamSink

CScafKeyStreamSink::CScafKeyStreamSink(CScafServer& aParent)
/**
 	Constructor.
 */
	:	CScafStep(aParent)
	{
	SetTestStepName(KScafKeyStreamSinkStep);
	}

CScafKeyStreamSink::~CScafKeyStreamSink()
/**
 	Destructor.
 */
	{
	//empty
	}

TVerdict CScafKeyStreamSink::doTestL()
	{
	TPtrC privatePath;
	
	if(iDecoderConfigurationArray[0]->iPrivateFolderPath.Length())
		{
		privatePath.Set(iDecoderConfigurationArray[0]->iPrivateFolderPath);
		}
	else
		{
		privatePath.Set(KStaPrivateFolder());
		}
	
	//Ensure pre-conditions
	CleanAgentsPrivateFolderL(privatePath);
	
	//First copy the service protected RO into the test agent's private directory
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KServiceProtectedRo(), privatePath);
	
	_LIT8(KPlainString,"aplainstring");
	CSraRightsObject* ro = GetRightsObjectLC(KServiceProtectedRo(),privatePath);
	//Encrypt the string with the encryption key
	//Since encryption is not tested here, the encrypted key is 
	//created by appending the encryption key.
	TBuf8<50> encryptedStr;
	encryptedStr.Append(KPlainString);
	encryptedStr.Append(*ro->Key());
	CleanupStack::PopAndDestroy(ro);
	
	//Create an SDP document object and set the created key stream field object
	CSdpDocument* sdpDoc = CreateSdpDocumentLC();
	//Create an SDP key stream field object with an attribute requiring the service protected RO
	CSdpMediaField* sdp = CreateSdpLC(0);
	AddMediaFieldL(*sdpDoc, sdp);
	CleanupStack::Pop(sdp);

	//Create a test key stream sink
	_LIT(KKeySinkOutputFileName,"testkeysstreamsink.dat");
	CKeyStreamSink* sink = CreateKeyStreamSinkLC(KKeySinkOutputFileName, privatePath);
	
	CTestKeyStreamDecoderBase* decoder = NULL;
	
	if(iDecoderConfigurationArray[0]->iSingleProcessAgent)
		{
		// Single Process Stream Agent
		decoder = CTestSingleProcessKeyStreamDecoder::NewL(*sink, *sdp, *sdpDoc);
		}
	else
		{
		// Client/Server Stream Agent
		decoder = CTestAgentKeyStreamDecoder::NewL(*sink, *sdp, *sdpDoc);
		}
	
	CleanupStack::PushL(decoder);
		
	//Send the encrypted string
	decoder->SendKeyStreamL(encryptedStr);
		
	//Compare the output key with the original one
	HBufC* fOutput = GetFullPathLC(privatePath, KKeySinkOutputFileName);
	
	RFile fOutputKey;
	User::LeaveIfError(fOutputKey.Open(iParent.Fs(), *fOutput, EFileRead));
	CleanupStack::PopAndDestroy(fOutput);
	CleanupClosePushL(fOutputKey);
	TBuf8<50> decryptedKey;
	User::LeaveIfError(fOutputKey.Read(decryptedKey));
	
	if(decryptedKey.Compare(KPlainString))
		{
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(4, sdpDoc); // fOutputKey, decoder, sink, sdpDoc
	return TestStepResult();
	}


//CScafConcurrentKeyStreamSinks

CScafConcurrentKeyStreamSinks::CScafConcurrentKeyStreamSinks(CScafServer& aParent)
/**
 	Constructor.
 */
	:	CScafStep(aParent)
	{
	SetTestStepName(KScafConcurrentKeyStreamSinksStep);
	}

CScafConcurrentKeyStreamSinks::~CScafConcurrentKeyStreamSinks()
/**
 	Destructor.
 */
	{
	//empty
	}

TVerdict CScafConcurrentKeyStreamSinks::doTestL()
	{
	TPtrC privatePathA;
	
	if(iDecoderConfigurationArray[0]->iPrivateFolderPath.Length())
		{
		privatePathA.Set(iDecoderConfigurationArray[0]->iPrivateFolderPath);
		}
	else
		{
		privatePathA.Set(KStaPrivateFolder());
		}
	
	TPtrC privatePathB;
	
	if(iDecoderConfigurationArray[1]->iPrivateFolderPath.Length())
		{
		privatePathB.Set(iDecoderConfigurationArray[1]->iPrivateFolderPath);
		}
	else
		{
		privatePathB.Set(KStaPrivateFolder());
		}
	
	//Ensure pre-conditions
	CleanAgentsPrivateFolderL(privatePathA);
	CleanAgentsPrivateFolderL(privatePathB);
	
	//First copy the service protected RO into the private directories of each stream agent
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KServiceProtectedRo(), privatePathA);
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KServiceProtectedRo(), privatePathB);
	
	//Secondly copy the program protected RO into the private directories of each stream agent
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KProgramProtectedRo(), privatePathA);
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KProgramProtectedRo(), privatePathB);

	//Create an SDP document object and set the created key stream field object for each decoder
	CSdpDocument* sdpDoc = CreateSdpDocumentLC();
	
	//Create a SDP object for RO-A
	TInt sdpNum = 0;
	CSdpMediaField* sdpA = CreateSdpLC(sdpNum++);
	AddMediaFieldL(*sdpDoc, sdpA);
	CleanupStack::Pop(sdpA);
	
	//Create a SDP object for RO-B
	CSdpMediaField* sdpB = CreateSdpLC(sdpNum);
	AddMediaFieldL(*sdpDoc, sdpB);
	CleanupStack::Pop(sdpB);

	//Create a test key stream sink for test key stream decoder A
	_LIT(KKeySinkOutputFileNameA,"testkeysstreamsinkA.dat");
	CKeyStreamSink* sinkA = CreateKeyStreamSinkLC(KKeySinkOutputFileNameA,privatePathA);
	
	
	//Create test key stream decoder A
	CTestKeyStreamDecoderBase* decoderA = NULL;
	
	if(iDecoderConfigurationArray[0]->iSingleProcessAgent)
		{
		// Single Process Stream Agent
		decoderA = CTestSingleProcessKeyStreamDecoder::NewL(*sinkA, *sdpA, *sdpDoc);
		}
	else
		{
		// Client/Server Stream Agent
		decoderA = CTestAgentKeyStreamDecoder::NewL(*sinkA, *sdpA, *sdpDoc);
		}

	CleanupStack::PushL(decoderA);
	
	//Create a test key stream sink for test key stream decoder B
	_LIT(KKeySinkOutputFileNameB,"testkeysstreamsinkB.dat");
	CKeyStreamSink* sinkB = CreateKeyStreamSinkLC(KKeySinkOutputFileNameB,privatePathB);
	
	//Create test key stream decoder B
	CTestKeyStreamDecoderBase* decoderB = NULL;
	
	if(iDecoderConfigurationArray[1]->iSingleProcessAgent)
		{
		// Single Process Stream Agent
		decoderB = CTestSingleProcessKeyStreamDecoder::NewL(*sinkB, *sdpB, *sdpDoc);
		}
	else
		{
		// Client/Server Stream Agent
		decoderB = CTestAgentKeyStreamDecoder::NewL(*sinkB, *sdpB, *sdpDoc);
		}
	
	CleanupStack::PushL(decoderB);
	
	_LIT8(KPlainStringA,"plainstringA");
	CSraRightsObject* roA = GetRightsObjectLC(KServiceProtectedRo,privatePathA);
	//Encrypt the plain string A
	TBuf8<50> encryptedStr;
	encryptedStr.Append(KPlainStringA);
	encryptedStr.Append(*roA->Key());
	CleanupStack::PopAndDestroy(roA);
	//Send the encrypted string A
	decoderA->SendKeyStreamL(encryptedStr);
	
	_LIT8(KPlainStringB,"plainstringB");
	CSraRightsObject* roB = GetRightsObjectLC(KProgramProtectedRo,privatePathB);
	//Encrypt the plain string B
	encryptedStr.Zero();
	encryptedStr.Append(KPlainStringB);
	encryptedStr.Append(*roB->Key());
	CleanupStack::PopAndDestroy(roB);
	//Send the encrypted string B
	decoderB->SendKeyStreamL(encryptedStr);

	//Compare the output key file A with the original one
	HBufC* fOutputA = GetFullPathLC(privatePathA, KKeySinkOutputFileNameA);
	RFile fOutputKey;
	User::LeaveIfError(fOutputKey.Open(iParent.Fs(), *fOutputA, EFileRead));
	CleanupStack::PopAndDestroy(fOutputA);
	CleanupClosePushL(fOutputKey);
	TBuf8<50> decryptedKey;
	User::LeaveIfError(fOutputKey.Read(decryptedKey));
	CleanupStack::PopAndDestroy(&fOutputKey);
	
	if(decryptedKey.Compare(KPlainStringA))
		{
		SetTestStepResult(EFail);
		}
	
	//Compare the output key file B with the original one
	HBufC* fOutputB = GetFullPathLC(privatePathB, KKeySinkOutputFileNameB);
	User::LeaveIfError(fOutputKey.Open(iParent.Fs(), *fOutputB, EFileRead));
	CleanupStack::PopAndDestroy(fOutputB);
	CleanupClosePushL(fOutputKey);
	decryptedKey.Zero();
	User::LeaveIfError(fOutputKey.Read(decryptedKey));
	CleanupStack::PopAndDestroy(&fOutputKey);
	
	if(decryptedKey.Compare(KPlainStringB))
		{
		SetTestStepResult(EFail);
		}

	//Release allocated resources
	CleanupStack::PopAndDestroy(2, sinkB);		//< decoderB, sink B
	CleanupStack::PopAndDestroy(2, sinkA);		//< decoderA, sinkA
	CleanupStack::PopAndDestroy(sdpDoc); 		//< sdpDoc
	return TestStepResult();
	}
