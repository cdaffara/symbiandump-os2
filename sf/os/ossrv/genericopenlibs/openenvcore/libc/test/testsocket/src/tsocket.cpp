/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#include "tsocket.h"

CTestSocket::~CTestSocket() 
	{ 
	}  

CTestSocket::CTestSocket(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSocket::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSocket::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTestSocket::doTestStepL()
	{
	int err;

   	if(TestStepName() == KExampleL)
   		{
		INFO_PRINTF1(_L("ExampleL():"));
		err = ExampleL();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else
		if(TestStepName() == KUDP)
   			{
   	   		INFO_PRINTF1(_L("UDP():"));
   	   		err = UDP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTCP)
   			{
   	   		INFO_PRINTF1(_L("TCP():"));
   	   		err = TCP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetSocketName)
   			{
   	   		INFO_PRINTF1(_L("GetSocketName():"));
   	   		err = GetSocketName();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetSocketNameUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("GetSocketNameUsingFileDescriptor():"));
   	   		err = GetSocketNameUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetSocketNameUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("GetSocketNameUsingInvalidSocketDescriptor():"));
   	   		err = GetSocketNameUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetSocketNameInvalidLength)
   			{
   	   		INFO_PRINTF1(_L("GetSocketNameInvalidLength():"));
   	   		err = GetSocketNameInvalidLength();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetSocketNameInvalidSocketBuffer)
   			{
   	   		INFO_PRINTF1(_L("GetSocketNameInvalidSocketBuffer():"));
   	   		err = GetSocketNameInvalidSocketBuffer();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetPeerSocketName)
   			{
   	   		INFO_PRINTF1(_L("GetPeerSocketName():"));
   	   		err = GetPeerSocketName();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetPeerSocketNameUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("GetPeerSocketNameUsingFileDescriptor():"));
   	   		err = GetPeerSocketNameUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetPeerSocketNameUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("GetPeerSocketNameUsingInvalidSocketDescriptor():"));
   	   		err = GetPeerSocketNameUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetPeerSocketNameforUnconnectedSocket)
   			{
   	   		INFO_PRINTF1(_L("GetPeerSocketNameforUnconnectedSocket():"));
   	   		err = GetPeerSocketNameforUnconnectedSocket();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetPeerSocketNameInvalidLengthSockAddr)
   			{
   	   		INFO_PRINTF1(_L("GetPeerSocketNameInvalidLengthSockAddr():"));
   	   		err = GetPeerSocketNameInvalidLengthSockAddr();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindTest)
   			{
   	   		INFO_PRINTF1(_L("BindTest():"));
   	   		err = BindTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KMultipleBindOnSameSocket)
   			{
   	   		INFO_PRINTF1(_L("MultipleBindOnSameSocket():"));
   	   		err = MultipleBindOnSameSocket();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindInvalidAddress)
   			{
   	   		INFO_PRINTF1(_L("BindInvalidAddress():"));
   	   		err = BindInvalidAddress();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("BindUsingInvalidSocketDescriptor():"));
   	   		err = BindUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("BindUsingFileDescriptor():"));
   	   		err = BindUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShutdownTest)
   			{
   	   		INFO_PRINTF1(_L("ShutdownTest():"));
   	   		err = ShutdownTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShutDownTestWithInvalidShutdownOption)
   			{
   	   		INFO_PRINTF1(_L("ShutDownTestWithInvalidShutdownOption():"));
   	   		err = ShutDownTestWithInvalidShutdownOption();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShutdownUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("ShutdownUsingFileDescriptor():"));
   	   		err = ShutdownUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShutdownUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("ShutdownUsingInvalidSocketDescriptor():"));
   	   		err = ShutdownUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShutdownDisconnectedSocket)
   			{
   	   		INFO_PRINTF1(_L("ShutdownDisconnectedSocket():"));
   	   		err = ShutdownDisconnectedSocket();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSocketTest)
   			{
   	   		INFO_PRINTF1(_L("SocketTest():"));
   	   		err = SocketTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KListenTest)
   			{
   	   		INFO_PRINTF1(_L("ListenTest():"));
   	   		err = ListenTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KListenUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("ListenUsingFileDescriptor():"));
   	   		err = ListenUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KListenUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("ListenUsingInvalidSocketDescriptor():"));
   	   		err = ListenUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KAcceptTest)
   			{
   	   		INFO_PRINTF1(_L("AcceptTest():"));
   	   		err = AcceptTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KUDPAccept)
   			{
   	   		INFO_PRINTF1(_L("UDPAccept():"));
   	   		err = UDPAccept();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KAcceptUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("AcceptUsingFileDescriptor():"));
   	   		err = AcceptUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KAcceptUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("AcceptUsingInvalidSocketDescriptor():"));
   	   		err = AcceptUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KConnectTestFailCases)
   			{
   	   		INFO_PRINTF1(_L("ConnectTestFailCases():"));
   	   		err = ConnectTestFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KConnectUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("ConnectUsingFileDescriptor():"));
   	   		err = ConnectUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KConnectUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("ConnectUsingInvalidSocketDescriptor():"));
   	   		err = ConnectUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KRecvTestFailCases)
   			{
   	   		INFO_PRINTF1(_L("RecvTestFailCases():"));
   	   		err = RecvTestFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KRecvUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("RecvUsingInvalidSocketDescriptor():"));
   	   		err = RecvUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KRecvUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("RecvUsingFileDescriptor():"));
   	   		err = RecvUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendTestFailCases)
   			{
   	   		INFO_PRINTF1(_L("SendTestFailCases():"));
   	   		err = SendTestFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("SendUsingFileDescriptor():"));
   	   		err = SendUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("SendUsingInvalidSocketDescriptor():"));
   	   		err = SendUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSocketOptions)
   			{
   	   		INFO_PRINTF1(_L("SocketOptions():"));
   	   		err = SocketOptions();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetSockOptFailCases)
   			{
   	   		INFO_PRINTF1(_L("GetSockOptFailCases():"));
   	   		err = GetSockOptFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSetSockOptFailCases)
   			{
   	   		INFO_PRINTF1(_L("SetSockOptFailCases():"));
   	   		err = SetSockOptFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendToTestFailCases)
   			{
   	   		INFO_PRINTF1(_L("SendToTestFailCases():"));
   	   		err = SendToTestFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendToUsingFileDescriptor)
   			{
   	   		INFO_PRINTF1(_L("SendToUsingFileDescriptor():"));
   	   		err = SendToUsingFileDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendToUsingInvalidSocketDescriptor)
   			{
   	   		INFO_PRINTF1(_L("SendToUsingInvalidSocketDescriptor():"));
   	   		err = SendToUsingInvalidSocketDescriptor();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else 
		if(TestStepName() == KTestSendReturnValue)
			{
			INFO_PRINTF1(_L("TestSendReturnValue():"));
			err = TestSendReturnValue();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}                  
	else
		if(TestStepName() == KRecvFromTestFailCases)
   			{
   	   		INFO_PRINTF1(_L("RecvFromTestFailCases():"));
   	   		err = RecvFromTestFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSockAtMark)
   			{
   	   		INFO_PRINTF1(_L("SockAtMark():"));
   	   		err = SockAtMark();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindResvPort)
   			{
   	   		INFO_PRINTF1(_L("BindResvPort():"));
   	   		err = BindResvPort();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindResvPortFailCases)
   			{
   	   		INFO_PRINTF1(_L("BindResvPortFailCases():"));
   	   		err = BindResvPortFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KHErrNoLocation)
   			{
   	   		INFO_PRINTF1(_L("HErrNoLocation():"));
   	   		err = HErrNoLocation();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSendMsgRecvMsg)
   			{
   	   		INFO_PRINTF1(_L("SendMsgRecvMsg():"));
   	   		err = SendMsgRecvMsg();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTestRecv)
   			{
   	   		INFO_PRINTF1(_L("TestRecv():"));
   	   		err = TestRecv();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTestReadStream)
   			{
   	   		INFO_PRINTF1(_L("TestReadStream():"));
   	   		err = TestReadStream();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTestReadDatagram)
   			{
   	   		INFO_PRINTF1(_L("TestReadDatagram():"));
   	   		err = TestReadDatagram();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KBindFailCases)
   			{
   	   		INFO_PRINTF1(_L("BindFailCases():"));
   	   		err = BindFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KAcceptFailCases)
   			{
   	   		INFO_PRINTF1(_L("AcceptFailCases():"));
   	   		err = AcceptFailCases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   if(TestStepName() == KAcceptTestZeroAddrLen)
   	    	{
   	      		INFO_PRINTF1(_L("AcceptTestZeroAddrLen():"));
   	       		err = AcceptTestZeroAddrLen();
   	       		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	    	}
	else
	   if(TestStepName() == KAcceptTestNullAddr)
	    	{
	      		INFO_PRINTF1(_L("AcceptTestNullAddr():"));
	       		err = AcceptTestNullAddr();
	       		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	    	}
	 else
		if(TestStepName() == KListenUdp)
   			{
   	   		INFO_PRINTF1(_L("ListenUdp():"));
   	   		err = ListenUdp();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetHostName)
   			{
   	   		INFO_PRINTF1(_L("GetHostName():"));
   	   		err = GetHostName();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetHostNameNull)
   			{
   	   		INFO_PRINTF1(_L("GetHostNameNull():"));
   	   		err = GetHostNameNull();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KGetHostNameZero)
   			{
   	   		INFO_PRINTF1(_L("GetHostNameZero():"));
   	   		err = GetHostNameZero();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == KTestSktlseek)
   			{
   	   		INFO_PRINTF1(_L("TestSktlseek():"));
   	   		err = TestSktlseek();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == KTestSockAfLocal)
   			{
   	   		INFO_PRINTF1(_L("TestSockAfLocal():"));
   	   		err = TestSockAfLocal();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTestSktfstat)
   			{
   	   		INFO_PRINTF1(_L("TestSktfstat():"));
   	   		err = TestSktfstat();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == KTestSktfsync)
   			{
   	   		INFO_PRINTF1(_L("TestSktfsync():"));
   	   		err = TestSktfsync();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == KTestGreaterThan16kReadWrite)
   			{
   	   		INFO_PRINTF1(_L("TestGreaterThan16kReadWrite():"));
   	   		err = TestGreaterThan16kReadWrite();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else
			if(TestStepName() == KTestNotify)
		   		{
		   		INFO_PRINTF1(_L("TestNotify():"));
		   		err = TestNotify();
		   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		   		} 
	else
		if(TestStepName() == KTestRecvMsgPeekTCP)
	   		{
	   		INFO_PRINTF1(_L("TestRecvMsgPeekTCP():"));
	   		err = TestRecvMsgPeekTCP();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		} 
	else
		if(TestStepName() == KTestRecvMsgPeekUDP)
	   		{
	   		INFO_PRINTF1(_L("TestRecvMsgPeekUDP():"));
	   		err = TestRecvMsgPeekUDP();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		} 
	else
		if(TestStepName() == KTestLargeUDP)
		{
		INFO_PRINTF1(_L("TestLargeUDP:"));
		err = TestLargeUDP();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else
		if(TestStepName() == KTestFionread)
   			{
   	   		INFO_PRINTF1(_L("TestFionread():"));
   	   		err = TestFionread();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
	else
		if(TestStepName() == KTestBind1)
	   		{
	   		INFO_PRINTF1(_L("TestBind1():"));
	   		err = TestBind1();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		}		
	else
		if(TestStepName() == KTestConnectNonBlocking)
	   		{
	   		INFO_PRINTF1(_L("TestConnectNonBlocking():"));
	   		err = TestConnectNonBlocking();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		}		
	else
		if(TestStepName() == KTCP1)
   			{
   	   		INFO_PRINTF1(_L("TCP1():"));
   	   		err = TCP1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTestV4MappedAddress)
   			{
   	   		INFO_PRINTF1(_L("TestV4MappedAddress():"));
   	   		err = TestV4MappedAddress();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
   	else
		if(TestStepName() == KReadTestFailCase)
   			{
   	   		INFO_PRINTF1(_L("ReadTestFailCase():"));
   	   		err = ReadTestFailCase();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KTestNotify1)
			{
			INFO_PRINTF1(_L("TestNotify1():"));
			err = TestNotify1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
	else
		if(TestStepName() == KTestSockFcntl)
			{
			INFO_PRINTF1(_L("TestSockFcntl():"));
			err = TestSockFcntl();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
	else
		if(TestStepName() == KTestSockIoctl)
			{
			INFO_PRINTF1(_L("TestSockIoctl():"));
			err = TestSockIoctl();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
	else
		if(TestStepName() == KSockErrnoTest)
			{
			INFO_PRINTF1(_L("SockErrnoTest():"));
			err = SockErrnoTest();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
    else
        if(TestStepName() == KSockSendOnClosedConn )
            {
            INFO_PRINTF1(_L("SockSendOnClosedConn():"));
            err = SockSendOnClosedConn();
            SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
            }
   	if(TestStepName() == KSockWriteOnClosedConn )
   	            {
   	            INFO_PRINTF1(_L("SockWriteOnClosedConn():"));
   	            err = SockWriteOnClosedConn();
   	            SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	            }
   	
return TestStepResult(); 

	}
 

