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
// Implements the reference agent test case.
// 
//

#include <caf/streaming/keystreamdecoder.h>
#include <caf/streaming/protectedstreamdesc.h>
#include "treferenceagent.h"
#include "stkm.h"

using namespace StreamAccess;

const TUint32 KTestSpiBase = 667;

CScafRefAgent::CScafRefAgent(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafRefAgent);
	}


TVerdict CScafRefAgent::doTestL()
	{
	SetTestStepResult(EPass);
	CleanAgentsPrivateFolderL(KSraPrivateFolder());
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KProgramProtectedRo(), KSraPrivateFolder());
	
	//Create an SDP document object and set the created key stream field object
	CSdpDocument* sdpDoc = CreateSdpDocumentLC();
	// Generate an SDP object by reading the values from the configuration file
	CSdpMediaField* sdp = CreateSdpLC(0);
	AddMediaFieldL(*sdpDoc, sdp);
	CleanupStack::Pop(sdp);
	
	// Create a trivial source IP address. Since the protected content is not required
	// in this test, this IP address used here is not important, any IP address can be used.
	TInetAddr sourceAddr;
	User::LeaveIfError(sourceAddr.Input(_L("192.168.174.5")));
	sourceAddr.SetPort(3004);
	
	// Destination IP address must be the agent's address
	TInetAddr destinationAddr (iClientAddr);
	
	// Create an IPSec protected key stream description object from the source and destination IP addresses
	CProtectedStreamDesc *protectedStreamDesc = CIpSecProtectedStreamDesc::NewLC(sourceAddr, destinationAddr);
	// Create a key stream decoder by using the IPSec key stream decoder and sdp requiring the reference agent
	CKeyStreamDecoder* keyStreamDecoder = CKeyStreamDecoder::NewLC(*protectedStreamDesc, *sdp, *sdpDoc);
	
	// Create a short term key message (STKM)
	CStkm* stkm = CStkm::NewLC(0, KTestSpiBase, *iEncryptionKey, *iAuthenticationKey);
	CBufFlat* buf = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(buf);
	
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);
	stkm->ExternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	
	HBufC8* des = HBufC8::NewL(buf->Size());
	TPtr8 ptr((des)->Des());
	buf->Read(0, ptr, buf->Size());
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(des);
	
	// Send the STKM to the reference agent
	TRequestStatus status;
	iClientSocket.Send(*des, 0, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	
	// Wait until the agent is finished with the key processing
	RMutex mutex;
	_LIT(KSraMutexName, "MutexBtwTestHarnessAndRefAgent");
	User::LeaveIfError(mutex.OpenGlobal(KSraMutexName()));
	CleanupClosePushL(mutex);
	mutex.Wait();
	
	// Check whether the STKM message has been added to the SADB
	CallValidateSadbL(KTestSpiBase, sourceAddr, destinationAddr, ETrue);
	
	// Release the allocated memories
	CleanupStack::PopAndDestroy(5, protectedStreamDesc);
	CleanupStack::PopAndDestroy(); // sdpDoc
	return TestStepResult();	
	}
