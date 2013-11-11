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

#ifndef __TESTSOCKET_H__
#define __TESTSOCKET_H__

#include <test/testexecutestepbase.h>

// INCLUDE FILES
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>

#include <e32svr.h>
#include <e32def.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include <stdapis/arpa/inet.h>
#include <e32std.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <stdapis/net/if.h>
#include <sys/sockio.h>


_LIT(KExampleL, "ExampleL");
_LIT(KUDP, "UDP");
_LIT(KTCP, "TCP");
_LIT(KSocketTest, "SocketTest");
_LIT(KGetSocketNameUsingInvalidSocketDescriptor, "GetSocketNameUsingInvalidSocketDescriptor");
_LIT(KGetSocketNameUsingFileDescriptor, "GetSocketNameUsingFileDescriptor");
_LIT(KGetSocketName, "GetSocketName");
_LIT(KGetSocketNameInvalidLength, "GetSocketNameInvalidLength");
_LIT(KGetSocketNameInvalidSocketBuffer, "GetSocketNameInvalidSocketBuffer");
_LIT(KGetPeerSocketName, "GetPeerSocketName");
_LIT(KGetPeerSocketNameUsingFileDescriptor, "GetPeerSocketNameUsingFileDescriptor");  
_LIT(KGetPeerSocketNameUsingInvalidSocketDescriptor, "GetPeerSocketNameUsingInvalidSocketDescriptor");
_LIT(KGetPeerSocketNameforUnconnectedSocket, "GetPeerSocketNameforUnconnectedSocket");
_LIT(KGetPeerSocketNameInvalidLengthSockAddr, "GetPeerSocketNameInvalidLengthSockAddr");
_LIT(KBindTest, "BindTest");
_LIT(KMultipleBindOnSameSocket, "MultipleBindOnSameSocket");
_LIT(KBindInvalidAddress, "BindInvalidAddress");
_LIT(KBindUsingInvalidSocketDescriptor, "BindUsingInvalidSocketDescriptor");
_LIT(KBindUsingFileDescriptor, "BindUsingFileDescriptor");
_LIT(KBindFailCases, "BindFailCases");		
_LIT(KShutdownTest, "ShutdownTest");
_LIT(KShutDownTestWithInvalidShutdownOption, "ShutDownTestWithInvalidShutdownOption");
_LIT(KShutdownUsingFileDescriptor, "ShutdownUsingFileDescriptor");
_LIT(KShutdownUsingInvalidSocketDescriptor, "ShutdownUsingInvalidSocketDescriptor");
_LIT(KShutdownDisconnectedSocket, "ShutdownDisconnectedSocket");
_LIT(KListenUdp, "ListenUdp");
_LIT(KListenTest, "ListenTest");
_LIT(KListenUsingFileDescriptor, "ListenUsingFileDescriptor");
_LIT(KListenUsingInvalidSocketDescriptor, "ListenUsingInvalidSocketDescriptor");
_LIT(KAcceptTest, "AcceptTest");
_LIT(KUDPAccept, "UDPAccept");
_LIT(KAcceptUsingFileDescriptor, "AcceptUsingFileDescriptor");
_LIT(KAcceptUsingInvalidSocketDescriptor, "AcceptUsingInvalidSocketDescriptor");
_LIT(KAcceptFailCases, "AcceptFailCases");
_LIT(KConnectTestFailCases, "ConnectTestFailCases");
_LIT(KConnectUsingFileDescriptor, "ConnectUsingFileDescriptor");
_LIT(KConnectUsingInvalidSocketDescriptor, "ConnectUsingInvalidSocketDescriptor");		
_LIT(KRecvUsingFileDescriptor, "RecvUsingFileDescriptor");
_LIT(KRecvUsingInvalidSocketDescriptor, "RecvUsingInvalidSocketDescriptor");
_LIT(KRecvTestFailCases, "RecvTestFailCases");
_LIT(KSendTestFailCases, "SendTestFailCases");
_LIT(KSendUsingFileDescriptor, "SendUsingFileDescriptor");
_LIT(KSendUsingInvalidSocketDescriptor, "SendUsingInvalidSocketDescriptor");
_LIT(KSocketOptions, "SocketOptions");
_LIT(KGetSockOptFailCases, "GetSockOptFailCases");
_LIT(KSetSockOptFailCases, "SetSockOptFailCases");		
_LIT(KSendToTestFailCases, "SendToTestFailCases");
_LIT(KSendToUsingFileDescriptor, "SendToUsingFileDescriptor");
_LIT(KSendToUsingInvalidSocketDescriptor, "SendToUsingInvalidSocketDescriptor");
_LIT(KRecvFromTestFailCases, "RecvFromTestFailCases");
_LIT(KSockAtMark, "SockAtMark");
_LIT(KBindResvPort, "BindResvPort");	
_LIT(KBindResvPortFailCases, "BindResvPortFailCases");			
_LIT(KHErrNoLocation, "HErrNoLocation");
_LIT(KSendMsgRecvMsg, "SendMsgRecvMsg");
_LIT(KGetHostName, "GetHostName");
_LIT(KGetHostNameNull, "GetHostNameNull");
_LIT(KGetHostNameZero, "GetHostNameZero");	
_LIT(KTestRecv, "TestRecv");
_LIT(KTestReadStream, "TestReadStream");
_LIT(KTestReadDatagram, "TestReadDatagram");	

class CTestSocket : public CTestStep
	{
public:
	~CTestSocket(); 
	CTestSocket(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	void GetParameters(char aParamets[10][256]);
	TInt ExampleL();
	TInt UDP(  );
    TInt TCP(  );
    TInt GetSocketName(  );
    TInt GetSocketNameUsingFileDescriptor(  );
    TInt GetSocketNameUsingInvalidSocketDescriptor(  );		
    TInt GetSocketNameInvalidLength(  );
    TInt GetSocketNameInvalidSocketBuffer(  );
    TInt GetPeerSocketName( );
    TInt GetPeerSocketNameUsingFileDescriptor( );
    TInt GetPeerSocketNameUsingInvalidSocketDescriptor( );
    TInt GetPeerSocketNameforUnconnectedSocket( );
    TInt GetPeerSocketNameInvalidLengthSockAddr( );
    TInt BindTest( );
    TInt MultipleBindOnSameSocket( );        
    TInt BindInvalidAddress( );
    TInt BindUsingInvalidSocketDescriptor( );
    TInt BindUsingFileDescriptor( );
    TInt ShutdownTest( );
    TInt ShutDownTestWithInvalidShutdownOption( );        
    TInt ShutdownUsingFileDescriptor( );
    TInt ShutdownUsingInvalidSocketDescriptor( );
    TInt ShutdownDisconnectedSocket( );
    TInt SocketTest();
    TInt ListenTest();
    TInt ListenUsingFileDescriptor();
    TInt ListenUsingInvalidSocketDescriptor();
	TInt AcceptTest();
	TInt UDPAccept();
	TInt AcceptUsingFileDescriptor();
	TInt AcceptUsingInvalidSocketDescriptor();
	TInt ConnectTestFailCases();
	TInt ConnectUsingFileDescriptor();
	TInt ConnectUsingInvalidSocketDescriptor();		
	TInt RecvTestFailCases();
	TInt RecvUsingInvalidSocketDescriptor();
	TInt RecvUsingFileDescriptor();
	TInt SendTestFailCases();
	TInt SendUsingFileDescriptor();
	TInt SendUsingInvalidSocketDescriptor();
	TInt SocketOptions();
	TInt GetSockOptFailCases();
	TInt SetSockOptFailCases();
	TInt SendToTestFailCases();
	TInt SendToUsingFileDescriptor();
	TInt SendToUsingInvalidSocketDescriptor();
	TInt RecvFromTestFailCases();
	TInt SockAtMark();
	TInt BindResvPort();
	TInt BindResvPortFailCases();
	TInt HErrNoLocation();
	TInt SendMsgRecvMsg();		
	TInt TestRecv();
	TInt TestReadStream();
	TInt TestReadDatagram();	
	TInt BindFailCases();
	TInt AcceptFailCases();
	TInt ListenUdp();
	TInt GetHostName();
	TInt GetHostNameNull();
	TInt GetHostNameZero();		
	};
#endif 
