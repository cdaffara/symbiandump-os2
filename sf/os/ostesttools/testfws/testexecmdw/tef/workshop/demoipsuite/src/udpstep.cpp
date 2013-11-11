/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Example CTestStep derived implementation
*
*/



/**
 @file UDPStep.cpp
*/
#include "udpstep.h"
#include "ipsuitedefs.h"

CUDPStep::~CUDPStep()
/**
 * Destructor
 */
	{
	}

CUDPStep::CUDPStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KDemoUDPStep);
	}

TVerdict CUDPStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{

	RSocket socket;
	User::LeaveIfError(socket.Open(iServer, KAfInet, KSockDatagram, KProtocolInetUdp));
	CleanupClosePushL(socket);

	// Get the IP Address into a 16 bit descriptor to print it
	TBuf<50> IPAddrBuf;
	iDestAddr.Output(IPAddrBuf);
	_LIT(KSendDataMessage,"Send Data to %S Port = %d Length = %d");
	TPtr8 writeDataDes(iWriteData->Des());
	INFO_PRINTF4(KSendDataMessage,&IPAddrBuf,iDestAddr.Port(),writeDataDes.Length());

	TRequestStatus status;
	socket.SendTo(writeDataDes,iDestAddr, 0,status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	_LIT(KSentDataMessage,"Data Sent");
	INFO_PRINTF1(KSentDataMessage);

	// Use RecvFrom in this case
	// The status is not completed until the descriptor is full
	// or there is a socket error/timeout -22
	TPtr8 readDataDes(iReadData->Des());
	socket.RecvFrom(readDataDes,iDestAddr, 0, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	_LIT(KRecvDataMessage,"Data Received Length = %d");
	INFO_PRINTF2(KRecvDataMessage,readDataDes.Length());

	// The receive data should always match the sent
	// Try deliberately corrupting the send or receive descriptor and
	// the panic will be picked up in the script engine
	__ASSERT_ALWAYS(readDataDes == writeDataDes,User::Panic(KDemoIPSuitePanic,EUDPDataCorrupt));

	socket.Close();

	CleanupStack::Pop(1);			// Server and Socket Handles

	return TestStepResult();
	}
