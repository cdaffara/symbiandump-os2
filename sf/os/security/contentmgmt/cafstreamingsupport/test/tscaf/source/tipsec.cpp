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
// Implements the IpSec Test Cases
// 
//

#include "tipsec.h"

#include <caf/streaming/keystreamdecoder.h>
#include <caf/streaming/protectedstreamdesc.h>
#include <caf/streaming/keystreamsink.h>
#include <caf/streaming/keyassociation.h>

//Networking and IpSec includes
#include <networking/pfkeyv2.h>
#include <networking/ipsecerr.h>
#include <pfkey_send.h>
#include <es_sock.h> 
#include <es_enum.h>

#include <e32base.h>
#include <c32comm.h>

_LIT(KDefaultServerAddr,"192.168.174.5");
_LIT(KClientLocalAddr,"192.168.0.3");
_LIT(KDefaultListenAddr, "0.0.0.0");
const TInt KClientPort = 3002;
const TInt KServerPort = 3003;
const TUint KTestSpiBase = 667;
_LIT8(KTestData, "test\n");
_LIT8(KDefaultEncryptionKey, "1234567890123456");
_LIT8(KDefaultAuthenticationKey, "12345678901234567890");

using namespace StreamAccess;

//--------------------------CScafIpSec--------------------

CScafIpSec::CScafIpSec(CScafServer& aParent): CScafStep(aParent)
	{
	SetTestStepName(KScafIpSec);
	}

TVerdict CScafIpSec::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	ReadTestConfigurationL();
	
	// Create a session to esock server
	User::LeaveIfError(iSocketServ.Connect());	
	// Create a connection
	User::LeaveIfError(iConnection.Open(iSocketServ, KAfInet));
	TRequestStatus status;			
	User::LeaveIfError(iConnection.Start());
	
	User::LeaveIfError(iClientSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolInetUdp, iConnection));
	// By default, we listen on the same port as then we use for the SA - can be different on a negative test
	TInt listenPort(KClientPort); 
	GetIntFromConfig(ConfigSection(), _L("ListenPort"), listenPort);	
	
	// Create and bind the client socket
	TInetAddr listenAddr;
	User::LeaveIfError(listenAddr.Input(KDefaultListenAddr));
	listenAddr.SetPort(listenPort);
	User::LeaveIfError(iClientSocket.Bind(listenAddr));
	
	TPtrC serverAddrFromConfig;
	if (GetStringFromConfig(ConfigSection(), _L("ServerAddress"), serverAddrFromConfig))
		{// If the IP address of the server is specified explicitly in the configuration file, use it as the server address.
		 // This specification is made when the server is a remote host.
		INFO_PRINTF2(_L("Assign server address from the configuration: %S"), &serverAddrFromConfig);
		User::LeaveIfError(iServerAddr.Input(serverAddrFromConfig));
		}
	else
		{// If the server IP address is not specified, try to find out the own IP address of the device
		 // by looking up its ethernet interface. It means that the client and server are running on the same device.
		TBool srvAddrFound = EFalse;
		TSoInetInterfaceInfo networkInfo;
		TPckg<TSoInetInterfaceInfo> opt(networkInfo);
		User::LeaveIfError(iClientSocket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));
		TInt res = KErrNone;
		TName ip;
		do 
			{
			res = iClientSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, opt);
			if(!opt().iAddress.IsUnspecified())
				{
				opt().iAddress.Output(ip);
				INFO_PRINTF3(_L("Interface Name:%S Interface Address:%S"),&(opt().iName), &ip);
				// Skip loopback interfaces and get the address of first extrenal interface
				if(opt().iName.Find(_L("loop"))  == KErrNotFound)
					{
					INFO_PRINTF2(_L("Assign server address as %S"), &ip);
					iServerAddr = opt().iAddress;
					srvAddrFound = ETrue;
					break;
					}
				}
			}while (res == KErrNone);
			// If the device doesn't have got an ethernet interface or its address has not been obtained, try to use a default one.
			if(!srvAddrFound)
				{
				INFO_PRINTF2(_L("Couldn't find a proper interface. Assign server address as %S"), &KDefaultServerAddr);
				User::LeaveIfError(iServerAddr.Input(KDefaultServerAddr));
				}
		}
	iServerAddr.SetPort(KServerPort);

	// Connect the UDP socket - this is needed for the sub-connection interface
	iClientSocket.Connect(iServerAddr, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());		
	
	// The client address is not equal to the listening address, since the PF_KEY association set
	// by key stream sink needs to have a well defined dest. address and not INADDR_ANY used on the listening socket	
	// The section below should be removed if and when we switch to sub-connection interface		
	User::LeaveIfError(iClientAddr.Input(KClientLocalAddr));
	iClientAddr.SetPort(KClientPort);
	
	iAssociationsNumber = 1;
	GetIntFromConfig(ConfigSection(), _L("AssociationsNumber"), iAssociationsNumber);

	iAuthenticationUsed = ETrue;
	GetBoolFromConfig(ConfigSection(), _L("UseAuthentication"), iAuthenticationUsed);	
		
	TPtrC encryptionKeyFromConfig;
	if (GetStringFromConfig(ConfigSection(), _L("EncryptionKey"), encryptionKeyFromConfig))
		{
		iEncryptionKey = HBufC8::NewL(encryptionKeyFromConfig.Length());
		iEncryptionKey->Des().Copy(encryptionKeyFromConfig);
		}
	else
		iEncryptionKey = KDefaultEncryptionKey().AllocL();
	
	iEncryptionAlgorithm = EAES_128_CBC;
	GetIntFromConfig(ConfigSection(), _L("EncryptionAlgorithm"), iEncryptionAlgorithm);
	
	if (iAuthenticationUsed)
		{
		TPtrC authenticationKeyFromConfig;
		if (GetStringFromConfig(ConfigSection(), _L("AuthenticationKey"), authenticationKeyFromConfig))
			{
			iAuthenticationKey = HBufC8::NewL(authenticationKeyFromConfig.Length());
			iAuthenticationKey->Des().Copy(authenticationKeyFromConfig);			
			}
		else
			iAuthenticationKey = KDefaultAuthenticationKey().AllocL();
		iAuthenticationAlgorithm = EHMAC_SHA1;
		GetIntFromConfig(ConfigSection(), _L("AuthenticationAlgorithm"), iAuthenticationAlgorithm);
		}
	else
		iAuthenticationAlgorithm = ENoAuthentication;
	
	return TestStepResult();
	}
		
void ReceiveAndCompareBufL(RSocket &aSocket, const TDesC8 &aCompareTo)
	{
	HBufC8 *testBuf = HBufC8::NewLC(aCompareTo.Length());
	TRequestStatus status;	
	TPtr8 testBufPtr(testBuf->Des());
	aSocket.Recv(testBufPtr, 0, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
		
	if (testBufPtr.Compare(aCompareTo) != 0)
		User::Leave(KErrGeneral);
	
	CleanupStack::PopAndDestroy(testBuf);
	}	

TUint32 ConvertToNetworkOrder(TUint32 aNum)
    {
    const TInt KMaxTUint32CStringLen = 11;
    TUint8 temp[ KMaxTUint32CStringLen ];   
    LittleEndian::Put32( temp, aNum );
    return BigEndian::Get32( temp );
    }
   
// Check whether a particular SA is present in SADB - used for testing the IPsec key stream decoder.
// Two modes supported: positive and negative - in the negative one the SA should not be present
// Some of the code is copy/pasted from IPSec key stream production code, since it cannot be exposed in the
// interfaces there
static void ValidateSadbL(TInt32 aSpi, TInetAddr &aSourceAddr, TInetAddr &aDestAddr, TBool aPositiveTesting)
	{	
	RSocketServ socketServ;
	User::LeaveIfError(socketServ.Connect());
	CleanupClosePushL(socketServ);
	RSADB rsadb;
	User::LeaveIfError(rsadb.Open(socketServ));
	CleanupClosePushL(rsadb);
	// We use the same sequence number as the SPI - since we use different SPI in our tests
	// this provides uniqueness required of sequence id-s 
	TPfkeySendMsg sendMessage(SADB_GET, SADB_SATYPE_ESP, aSpi, RProcess().Id());
	TUint32 bigEndianSpi(ConvertToNetworkOrder(aSpi));
	sendMessage.Add( Int2Type<SADB_EXT_SA>(), bigEndianSpi, 0, 0); 
	sendMessage.Add( Int2Type<SADB_EXT_ADDRESS_SRC>(), aSourceAddr, 0, 0 );
	sendMessage.Add( Int2Type<SADB_EXT_ADDRESS_DST>(), aDestAddr, 0, 0 );
	
	TRequestStatus status;
	rsadb.FinalizeAndSend(sendMessage, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	// Since SADB sends replies to _all_ sockets, we must take care to filter out replies which
	// do not correspond to our request. A similar logic is done in IPSec key stream decoder, but it is private there
	// and cannot be reused
	while (1)
		{
		TPfkeyRecvMsg receivedReply;
		rsadb.ReadRequest(receivedReply, status);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());		
		
		sadb_msg &msgHeader = receivedReply.MsgHdr();
	
		if (msgHeader.sadb_msg_pid != RProcess().Id())
			continue;
		if (msgHeader.sadb_msg_seq != aSpi)
			continue;		
		
		// If the message types does not match, then the problem is internal in IPSec - it should not answer with a different message type	
		if (msgHeader.sadb_msg_type != SADB_GET)
			User::Leave(KErrArgument); 
		if (msgHeader.sadb_msg_errno ^ aPositiveTesting == 0)
			{
			// Mimic the logic in IPSec error handling (see the Update function in key_msg.cpp)		
			TUint16 reservedField = (TUint16)msgHeader.sadb_msg_reserved << 8;
			TUint16 errnoField = msgHeader.sadb_msg_errno;
			User::Leave(-(reservedField + errnoField));
			}
		break;		
		}							
	CleanupStack::PopAndDestroy(2, &socketServ);
	}

void CScafIpSec::CallValidateSadbL(TInt32 aSpi, TInetAddr &aSourceAddr, TInetAddr &aDestAddr, TBool aPositiveTesting)
	{
	ValidateSadbL(aSpi, aSourceAddr, aDestAddr, aPositiveTesting);
	}

void CScafIpSec::InitializeAlgorithmsL(CKeyStreamSink *aKeyStreamSink)
	{
	aKeyStreamSink->SetEncryptionAlgorithmL((TEncryptionAlgorithm)iEncryptionAlgorithm);
	aKeyStreamSink->SetAuthenticationAlgorithmL((TAuthenticationAlgorithm)iAuthenticationAlgorithm);		
	}

CKeyStreamSink *CScafIpSec::CreateDefaultKeyStreamSinkLC()
	{
	// when RSubConnection interface starts working use the below code
	/*
	RSubConnection subconn;
	User::LeaveIfError(subconn.Open(ss, RSubConnection::ECreateNew, conn));
	CleanupClosePushL(subconn);
	
	subconn.Add(clientSocket, status);
	User::WaitForRequest(status);	
	User::LeaveIfError(status.Int());
	
	CProtectedStreamDesc *protectedStreamDesc = CIPSecProtectedStreamDesc::NewLC(subconn);
	*/			
	
	CProtectedStreamDesc *protectedStreamDesc = CIpSecProtectedStreamDesc::NewLC(iServerAddr, iClientAddr);
	CKeyStreamSink *keyStreamSink = protectedStreamDesc->CreateKeyStreamSinkLC();	
	InitializeAlgorithmsL(keyStreamSink);
	
	CleanupStack::Pop(keyStreamSink);
	CleanupStack::PopAndDestroy(protectedStreamDesc);
	CleanupStack::PushL(keyStreamSink);
	return keyStreamSink;
	}
	
void CScafIpSec::SendKeyAssociationToKeySinkL(TInt aSpi, CKeyStreamSink *aKeyStreamSink)
	{
	INFO_PRINTF2(_L("Sending key association with SPI %d"), aSpi);
	CKeyAssociation *ipSecKeyAssociation = CIpSecKeyAssociation::NewL(aSpi, iEncryptionKey, 
							iAuthenticationKey);
	CleanupStack::PushL(ipSecKeyAssociation); // Not using NewLC directly, so that NewL and NewLC will both be covered in tests
	
	aKeyStreamSink->ProcessNewKeyAssociationL(*ipSecKeyAssociation);	
		
	//aKeyStreamSink->ProcessNewKeyAssociationL(*ipSecKeyAssociation);	
	INFO_PRINTF2(_L("Sent key association with SPI %d"), aSpi);
	CleanupStack::PopAndDestroy(ipSecKeyAssociation);	
	}
	
void CScafIpSec::AddAndValidateAssociationsL(CKeyStreamSink *aKeyStreamSink, TInt aSpiBase)
	{
	for (TInt i = 0; i < iAssociationsNumber; ++i)
		{
		SendKeyAssociationToKeySinkL(aSpiBase + i, aKeyStreamSink);
		INFO_PRINTF2(_L("Validating key association with SPI %d"), aSpiBase + i);
		ValidateSadbL(aSpiBase + i, iServerAddr, iClientAddr, ETrue);
		INFO_PRINTF2(_L("Validated key association with SPI %d"), aSpiBase + i);
		}	
	}
	
void CScafIpSec::ValidateNoAssociationsL(TInt aSpiBase)
	{
	// Check that after key stream decoder was removed, all the SA-s had been deleted	
	for (TInt i = 0; i < iAssociationsNumber; ++i)
		ValidateSadbL(aSpiBase + i, iServerAddr, iClientAddr, EFalse);
	INFO_PRINTF3(_L("Validated that no associations exist from SPI %d to SPI %d"), aSpiBase, aSpiBase + iAssociationsNumber - 1);	
	}

TVerdict CScafIpSec::doTestL()
	{
	CKeyStreamSink *keyStreamSink = CreateDefaultKeyStreamSinkLC();
	
	TBool integrationTest(ETrue);
	GetBoolFromConfig(ConfigSection(), _L("IntegrationTest"), integrationTest);	
			
	for (TInt i = 0; i < iAssociationsNumber; ++i)
		{
		SendKeyAssociationToKeySinkL(KTestSpiBase + i, keyStreamSink);
		// Receive the packet and compare the data - disabled on automatic tests
		if (integrationTest)
			{
			ReceiveAndCompareBufL(iClientSocket, KTestData());					
			}
		}		
	CleanupStack::PopAndDestroy(keyStreamSink);
	return TestStepResult();	
	}

TVerdict CScafIpSec::doTestStepPostambleL()
	{
	delete iEncryptionKey;
	delete iAuthenticationKey;
	iClientSocket.Close();	
	iConnection.Close();	
	iSocketServ.Close();
	
	iDecoderConfigurationArray.ResetAndDestroy();
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

//-------------------------CScafIpSecDecoderIntegration---------------------------

CScafIpSecDecoderIntegration::CScafIpSecDecoderIntegration(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecDecoderIntegration);
	}

TVerdict CScafIpSecDecoderIntegration::doTestL()
	{	
	TBool integrationTest(ETrue);
	GetBoolFromConfig(ConfigSection(), _L("IntegrationTest"), integrationTest);
	
	CTestKeyStreamDecoderBase *keyStreamDecoder = NULL;
	CSdpMediaField *sdp = NULL;
	CSdpDocument* sdpDoc = NULL;
	
	TPtrC privatePath;
	
	if(iDecoderConfigurationArray[0]->iPrivateFolderPath.Length())
		{
		privatePath.Set(iDecoderConfigurationArray[0]->iPrivateFolderPath);
		}
	else
		{
		privatePath.Set(KStaPrivateFolder());
		}
	
	//Create an SDP document object and set the created key stream field object
	sdpDoc = CreateSdpDocumentLC();
	
	//Create an SDP object with an attribute requiring the service protected RO
	sdp = CreateSdpLC(0);
	AddMediaFieldL(*sdpDoc, sdp);
	CleanupStack::Pop(sdp);
	
	INFO_PRINTF1(_L("Decoder integration test - created SDP"));	
	CKeyStreamSink *keyStreamSink = CreateDefaultKeyStreamSinkLC();
	
	//Create a test agent key stream decoder
	if(iDecoderConfigurationArray[0]->iSingleProcessAgent)
		{
		// Single Process Stream Agent
		keyStreamDecoder = CTestSingleProcessKeyStreamDecoder::NewL(*keyStreamSink, *sdp, *sdpDoc);
		}
	else
		{
		// Client/Server Stream Agent
		keyStreamDecoder = CTestAgentKeyStreamDecoder::NewL(*keyStreamSink, *sdp, *sdpDoc);
		}
	
	INFO_PRINTF1(_L("Decoder integration test - created key stream decoder"));	
	CleanupStack::PushL(keyStreamDecoder);
	
	for (TInt i = 0; i < iAssociationsNumber; ++i)
		{
		INFO_PRINTF2(_L("Decoder integration test - before sending association %d"), i + 1);	
		keyStreamDecoder->SendIpSecAssociationL(KTestSpiBase + i, iEncryptionKey, iAuthenticationKey);
		if (integrationTest)
			ReceiveAndCompareBufL(iClientSocket, KTestData());					
		else
			ValidateSadbL(KTestSpiBase + i, iServerAddr, iClientAddr, ETrue);
		INFO_PRINTF2(_L("Decoder integration test - after receiving and comparing buffer for association %d"), i + 1);	
		}
	CleanupStack::PopAndDestroy(2, keyStreamSink);
	CleanupStack::PopAndDestroy(); // sdpDoc
	return TestStepResult();
	}

//-------------------------CScafIpSecSadbVerification---------------------------
	
CScafIpSecSadbVerification::CScafIpSecSadbVerification(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecSadbVerification);
	}

TVerdict CScafIpSecSadbVerification::doTestL()
	{
	CKeyStreamSink *keyStreamSink = CreateDefaultKeyStreamSinkLC();	
	AddAndValidateAssociationsL(keyStreamSink, KTestSpiBase);
	CleanupStack::PopAndDestroy(keyStreamSink);
	
	ValidateNoAssociationsL(KTestSpiBase);	
	return TestStepResult();		
	}
		
//-------------------------CScafIpSecSadbVerificationConcurrency---------------------------
	
CScafIpSecSadbVerificationConcurrency::CScafIpSecSadbVerificationConcurrency(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecSadbVerificationConcurrency);	
	}

template <class T> void ResetAndDestroyPointerArray(TAny *pointerArray)
	{
	reinterpret_cast<RPointerArray<T> *>(pointerArray)->ResetAndDestroy();
	}

struct CThreadFuncParam : public CBase
	{
	CThreadFuncParam(TInt aBaseSpi, TInt aAssociationsNumber) 
		: iBaseSpi(aBaseSpi), iAssociationsNumber(aAssociationsNumber) {}
		  
	TInt iBaseSpi;
	TInt iAssociationsNumber;
	};

void TestThreadFuncL(CThreadFuncParam *aThreadParam)
	{
	// Since this function runs in another thread, we cannot use member stack variables
	// of the CScafIpSecSadbVerificationConcurrency class - some of the functionality has to be duplicated here
	TInetAddr clientAddr, serverAddr;
	User::LeaveIfError(clientAddr.Input(KClientLocalAddr));
	clientAddr.SetPort(KClientPort);
	User::LeaveIfError(serverAddr.Input(KDefaultServerAddr));
	serverAddr.SetPort(KServerPort);
	RSocketServ socketServ;
	User::LeaveIfError(socketServ.Connect());
	CleanupClosePushL(socketServ);
	RSADB rsadb;
	User::LeaveIfError(rsadb.Open(socketServ));
	CleanupClosePushL(rsadb);	

	HBufC8 *encryptionKey = KDefaultEncryptionKey().AllocLC();	
	HBufC8 *authenticationKey = KDefaultAuthenticationKey().AllocLC();	

	CProtectedStreamDesc *protectedStreamDesc = CIpSecProtectedStreamDesc::NewLC(serverAddr, clientAddr);
	CKeyStreamSink *keyStreamSink = protectedStreamDesc->CreateKeyStreamSinkLC();	

	keyStreamSink->SetEncryptionAlgorithmL(EAES_128_CBC);
	keyStreamSink->SetAuthenticationAlgorithmL(EHMAC_SHA1);	
		
	for (TInt i = 0; i < aThreadParam->iAssociationsNumber; ++i)
		{
		TInt spi = aThreadParam->iBaseSpi + i;
		CKeyAssociation *ipSecKeyAssociation = CIpSecKeyAssociation::NewLC(spi, encryptionKey, 
																		   authenticationKey);
								   
		keyStreamSink->ProcessNewKeyAssociationL(*ipSecKeyAssociation);	
				
		CleanupStack::PopAndDestroy(ipSecKeyAssociation);			
		ValidateSadbL(spi, serverAddr, clientAddr, ETrue);
		}
	
	CleanupStack::PopAndDestroy(keyStreamSink);
	
	for (TInt i = 0; i < aThreadParam->iAssociationsNumber; ++i)
		ValidateSadbL(aThreadParam->iBaseSpi + i, serverAddr, clientAddr, EFalse);
		
	CleanupStack::PopAndDestroy(5, &socketServ); // socketServ, rsadb, protectedStreamDesc, copiedEncryptionKey, copiedAuthenticationKey
	}

TInt TestThreadFunc(TAny *aThreadParam)
	{
	CThreadFuncParam *param = reinterpret_cast<CThreadFuncParam *>(aThreadParam);
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	TRAPD(err, TestThreadFuncL(param));
	delete cleanup;
	return err;
	}

void ThreadCleanup(TAny *aThread)
	{	
	((RThread *)aThread)->Terminate(0);
	((RThread *)aThread)->Close();
	}

void CleanupThreadArray(TAny *aThreadArray)
	{
	RArray<RThread> *threadArrayPtr = reinterpret_cast<RArray<RThread> *>(aThreadArray);
	TInt threadsNum = threadArrayPtr->Count();
	for (TInt i = 0; i < threadsNum; ++i)
		{
		ThreadCleanup(&(*threadArrayPtr)[i]);
		}
	threadArrayPtr->Close();
	}
		
TVerdict CScafIpSecSadbVerificationConcurrency::doTestL()
	{
	const TInt KDefaultThreadsNumber = 10;
	TInt threadsNumber(KDefaultThreadsNumber);
	GetIntFromConfig(ConfigSection(), _L("ThreadsNumber"), threadsNumber);	
	
	RArray<RThread> spawnedThreads;
	CleanupStack::PushL(TCleanupItem(CleanupThreadArray, &spawnedThreads));	
	RPointerArray<CThreadFuncParam> threadParams;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyPointerArray<CKeyStreamSink>, &threadParams));
	
	TInt i = 0;
	for (; i < threadsNumber; ++i)
		{
		const TInt KHeapSize = 0x600;
		RThread thread;
		TInt spiBase = KTestSpiBase + iAssociationsNumber * i;
				
		CThreadFuncParam *threadParam = new (ELeave) CThreadFuncParam(spiBase, iAssociationsNumber);
		CleanupStack::PushL(threadParam);
		threadParams.AppendL(threadParam);
		CleanupStack::Pop(threadParam);
		
		TBuf<25> threadName;
		threadName.Format(_L("CAF IPSec Thread %d"), i);
		User::LeaveIfError(thread.Create(threadName, TestThreadFunc, KDefaultStackSize, KHeapSize, KHeapSize, threadParam));
		CleanupStack::PushL(TCleanupItem(ThreadCleanup, &thread));		
		spawnedThreads.AppendL(thread);
		// The thread itself is owned by the array		
		CleanupStack::Pop(&thread); 		
		}
		
	for (i = 0; i < threadsNumber; ++i)
		spawnedThreads[i].Resume();
		
	for (i = 0; i < threadsNumber; ++i)
		{
		TRequestStatus status;	
		// Wait for all threads to finish	
		spawnedThreads[i].Logon(status);
		User::WaitForRequest(status);
		if (status.Int() != KErrNone)
			{
			ERR_PRINTF3(_L("In IpSec concurrency tests, thread %d has returned with status %d"), i, status.Int());
			SetTestStepResult(EFail);
			}
		else
			INFO_PRINTF2(_L("IPSec concurrency test - thread %d finished successfully"), i);
		}
		
	CleanupStack::PopAndDestroy(2); // threadParams, spawnedThreads - cleanup item
	return TestStepResult();
	}

//-------------------------CScafIpSecNegative---------------------------

CScafIpSecNegative::CScafIpSecNegative(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecNegative);	
	}

TVerdict CScafIpSecNegative::doTestL()
	{	
	TInt encryptionKeyLength(iEncryptionKey->Length());
	ASSERT(encryptionKeyLength);
	// Save encryption and authentication key to temp. variable
	HBufC8 *malformedEncryptionKey = HBufC8::NewLC(encryptionKeyLength);
	TPtr8 malformedEncryptionKeyPtr(malformedEncryptionKey->Des());
	malformedEncryptionKeyPtr.Copy(iEncryptionKey->Des());
	
	TInt authenticationKeyLength(iAuthenticationKey->Length());
	ASSERT(authenticationKeyLength);
	HBufC8 *malformedAuthenticationKey = HBufC8::NewLC(authenticationKeyLength);
	TPtr8 malformedAuthenticationKeyPtr(malformedAuthenticationKey->Des());	
	malformedAuthenticationKeyPtr.Copy(iAuthenticationKey->Des());
	// Delete the last characters from keys - this makes them invalid	
	malformedEncryptionKeyPtr.SetLength(encryptionKeyLength - 1);	
	malformedAuthenticationKeyPtr.SetLength(authenticationKeyLength - 1);	
	
	CKeyStreamSink *keyStreamSink = CreateDefaultKeyStreamSinkLC();
	// Send new assoc. - should fail	
	CKeyAssociation *malformedKeyAssociation = CIpSecKeyAssociation::NewLC(KTestSpiBase, malformedEncryptionKey, 
							malformedAuthenticationKey);	
	TRAPD(err, keyStreamSink->ProcessNewKeyAssociationL(*malformedKeyAssociation));							
	if (err != EIpsec_BadCipherKey)
		{
		if (err != KErrNoMemory)
			{
			ERR_PRINTF2(_L("In IpSec negative tests, an incorrect error was returned when setting invalid keys. The error is %d"), err);
			}			
		if (err == KErrNone)
			SetTestStepResult(EFail);
		else
			User::Leave(err);
		}			
	INFO_PRINTF1(_L("IPSec negative test - received correct error on malformed association"));				
	// Here, we should have had a test which verifies that IPSec key stream sink rejects non-IPSec associations
	// Due to lack of dynamic_cast support, it is omitted.
		
	// Send twice a new assoc. - should fail. KTestSpiBase + 1 is used 
	// since IPSec implementation adds the malformed assoc. from step A to DB despite returning an error
	SendKeyAssociationToKeySinkL(KTestSpiBase + 1, keyStreamSink);
	INFO_PRINTF1(_L("IPSec negative test - sent correct association"));			
	TRAP(err, SendKeyAssociationToKeySinkL(KTestSpiBase + 1, keyStreamSink));
	if (err != KErrAlreadyExists)
		{
		if (err != KErrNoMemory)
			{
			ERR_PRINTF2(_L("In IpSec negative tests, an incorrect error was returned when setting duplicate SA. The error is %d"), err);
			}			
		if (err == KErrNone)
			SetTestStepResult(EFail);
		else
			User::Leave(err);
		}	
	INFO_PRINTF1(_L("IPSec negative test - received correct error on duplicate association"));			
	CleanupStack::PopAndDestroy(4, malformedEncryptionKey); // malformedEncryptionKey, malformedAuthenticationKey, keyStreamSink, malformedKeyAssociation
	return TestStepResult();
	}
	
//-------------------------CScafIpSecMultipleSinks---------------------------

CScafIpSecMultipleSinks::CScafIpSecMultipleSinks(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecMultipleSinks);	
	}

TInt KDefaultNumberOfSinks = 10;

TVerdict CScafIpSecMultipleSinks::doTestL()
	{
	// Read from configuration the number of sinks
	TInt sinksNumber(KDefaultNumberOfSinks);
	GetIntFromConfig(ConfigSection(), _L("SinksNumber"), sinksNumber);
	// Instantiate decoders using the same protected stream desc.
	CProtectedStreamDesc *protectedStreamDesc = CIpSecProtectedStreamDesc::NewLC(iServerAddr, iClientAddr);
	RPointerArray<CKeyStreamSink> sinkArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyPointerArray<CKeyStreamSink>, &sinkArray));
	TInt i = 0;
	for (; i < sinksNumber; ++i)
		{
		CKeyStreamSink *sink = protectedStreamDesc->CreateKeyStreamSinkLC();
		User::LeaveIfError(sinkArray.Append(sink));
		InitializeAlgorithmsL(sink);
		CleanupStack::Pop(sink);
		INFO_PRINTF2(_L("IPSec multiple sinks test - instantiated decoder %d"), i);
		}
	// Loop over decoders and number of associations, submit associations and validate them
	for (i = 0; i < iAssociationsNumber; ++i)
		for (TInt j = 0; j < sinksNumber; ++j)
			{
			TInt spi = KTestSpiBase + j * iAssociationsNumber + i;
			SendKeyAssociationToKeySinkL(spi, sinkArray[j]);
			ValidateSadbL(spi, iServerAddr, iClientAddr, ETrue);
			}
		
	// Delete decoders
	CleanupStack::PopAndDestroy(&sinkArray);
	// Validate that there are no associations
	for (; i < sinksNumber; ++i)
		{
		ValidateNoAssociationsL(KTestSpiBase + i * iAssociationsNumber);
		}	
	CleanupStack::PopAndDestroy(protectedStreamDesc);
	return TestStepResult();
	}
	
//-------------------------CScafIpSecAlgorithmChange---------------------------

CScafIpSecAlgorithmChange::CScafIpSecAlgorithmChange(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecAlgorithmChange);	
	}

TVerdict CScafIpSecAlgorithmChange::doTestL()
	{
	CKeyStreamSink *keyStreamSink = CreateDefaultKeyStreamSinkLC();
	SendKeyAssociationToKeySinkL(KTestSpiBase, keyStreamSink);
	TRAPD(err, keyStreamSink->SetEncryptionAlgorithmL(ENoEncryption));
	if (err != KErrNotSupported)
		{
		ERR_PRINTF2(_L("In IpSec algorithm change, an incorrect error was returned when setting NULL encryption. The error is %d"), err);
		SetTestStepResult(EFail);	
		}
	INFO_PRINTF1(_L("IPSec algorithms change test - received correct error after modifying encryption algorithm"));
	TRAP(err, keyStreamSink->SetAuthenticationAlgorithmL(ENoAuthentication));	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF2(_L("In IpSec algorithm change, an incorrect error was returned when setting NULL authentication. The error is %d"), err);
		SetTestStepResult(EFail);	
		}	
	INFO_PRINTF1(_L("IPSec algorithms change test - received correct error after modifying authentication algorithm"));		
	CleanupStack::PopAndDestroy(keyStreamSink);
	return TestStepResult();
	}


//-------------------------CScafIpSecSARemoval---------------------------

CScafIpSecSARemoval::CScafIpSecSARemoval(CScafServer& aParent): CScafIpSec(aParent)
	{
	SetTestStepName(KScafIpSecSARemoval);	
	}

const TUint KDefaultMaxSpiNumber = 3;  // The constant is copied from IPSec's sink production code - it is not exposed in its interface

TVerdict CScafIpSecSARemoval::doTestL()
	{
	CKeyStreamSink *keyStreamSink = CreateDefaultKeyStreamSinkLC();	
	if (iAssociationsNumber <= KDefaultMaxSpiNumber)
		{
		ERR_PRINTF3(_L("Incorrect number of associations specified in SA removal test - should be at least %d, and it is %d"), KDefaultMaxSpiNumber + 1, iAssociationsNumber);
		User::Leave(KErrArgument);
		}
	TInt i = 0;
	for (; i < iAssociationsNumber; ++i)
		{
		SendKeyAssociationToKeySinkL(KTestSpiBase + i, keyStreamSink);	
		}
	INFO_PRINTF2(_L("IPSec SA removal test - sent %d associations successfully"), iAssociationsNumber);
	for (i = 0; i < iAssociationsNumber - KDefaultMaxSpiNumber; ++i)
		{
		ValidateSadbL(KTestSpiBase + i, iServerAddr, iClientAddr, EFalse);		
		}
	INFO_PRINTF3(_L("IPSec SA removal test - verified that associations %d to %d do not exist"), KTestSpiBase, KTestSpiBase + iAssociationsNumber - KDefaultMaxSpiNumber - 1);
	for (i = iAssociationsNumber - KDefaultMaxSpiNumber; i < iAssociationsNumber; ++i)
		{
		ValidateSadbL(KTestSpiBase + i, iServerAddr, iClientAddr, ETrue);		
		}
	INFO_PRINTF3(_L("IPSec SA removal test - verified that associations %d to %d exist"), KTestSpiBase + iAssociationsNumber - KDefaultMaxSpiNumber, KTestSpiBase + iAssociationsNumber - 1);

	CleanupStack::PopAndDestroy(keyStreamSink);
	return TestStepResult();
	}
