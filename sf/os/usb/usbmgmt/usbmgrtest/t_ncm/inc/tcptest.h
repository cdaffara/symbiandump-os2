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

/** @file
 @internalComponent
 @test
 */

#ifndef TCPTEST_H
#define TCPTEST_H

#include <e32cons.h>
#include <es_sock.h>
#include <in_sock.h>

class CTcpProcess : public CActive
/**
Run TCP test: Create a tcp connection and make data transfer on it.
*/
	{
public:
	static CTcpProcess* NewL(CConsoleBase& aConsole, TInetAddr& aAddr, TInt aPort, TInt aSize, TBool aMode);
	~CTcpProcess();

private:
	CTcpProcess(CConsoleBase& aConsole, TInetAddr& aAddr, TInt aPort, TInt aSize, TBool aMode);
	void ConstructL();

	void RecvDataL();
    TInt SendDataL(TDes8& aData, TInt aSize);
    void PrintData(TDes8& aData);

private:
	//From CActive
    virtual void RunL();
    virtual void DoCancel();
	TInt RunError(TInt aError);

private:
	enum TProcessState
		{
		ECreateConnection,	//Create tcp connection
		EDataTransfer		//Data transfer
		};

private:
	CConsoleBase& iConsole;

	RSocketServ iSockServ;
	RSocket iListenSocket;
	RSocket iSocket;
	
	TInetAddr& iAddr; 
	TInt iPort;
	
	//The size of send buf and receive buf
	TInt iSize;

	RBuf8 iRecvBuf;
	RBuf8 iSendBuf;

	TSockXfrLength iRecvSize;
	
	TBool iMode; //ETrue: as a Server, and data transfer is receive first, then send back
				 //EFalse: as a client, and data transfer is send first, then receive.
	
	TProcessState iProcessState;
	
	};


class CUdpProcess : public CActive
            /**
              Run TCP test: Create a udp connection and make data transfer on it.
              */
{
    public:
    static CUdpProcess* NewL(CConsoleBase& aConsole, TInetAddr& aAddr, TInt aPort, TInt aSize, TBool aMode);
    ~CUdpProcess();

    private:
    CUdpProcess(CConsoleBase& aConsole, TInetAddr& aAddr, TInt aPort, TInt aSize, TBool aMode);
    void ConstructL();

    void RecvDataL();
    TInt SendDataL(TDes8& aData, TInetAddr& aAddr, TInt aSize);
    void PrintData(TDes8& aData);

    private:
    //From CActive
    virtual void RunL();
    virtual void DoCancel();
    TInt RunError(TInt aError);

    private:
    enum TProcessState
    {
        EDataSending,   //Data Sending
        EDataTransfer   //Data Recving
    };

    private:
    CConsoleBase& iConsole;

    RSocketServ iSockServ;
    RSocket iListenSocket;
    RSocket iSocket;

    TInetAddr iAddr; 
    TInetAddr iPeerAddr;
    TInt iPort;

    //The size of send buf and receive buf
    TInt iSize;

    RBuf8 iRecvBuf;
    RBuf8 iSendBuf;

    TSockXfrLength iRecvSize;

    TBool iMode; //ETrue: as a Server, and data transfer is receive first, then send back
    //EFalse: as a client, and data transfer is send first, then receive.

    TProcessState iProcessState;

};


class CTcpCommand;
class CTcpTestConsole : public CActive 
/**
The console of tcp test.
*/
	{
public:
	static CTcpTestConsole* NewL(TBool aIsTcp, TBool aMode, TDesC& aDefautAddr, TInt aIndex, CTcpCommand& aOwner);
	~CTcpTestConsole();

	TBool StartL();
	void Help();
	
private:
	CTcpTestConsole(TBool aIsTcp, TBool aMode, TDesC& aDefautAddr, TInt aIndex, CTcpCommand& aOwner);
	void ConstructL();

private:
	//From CActive
    virtual void RunL();
    virtual void DoCancel();
	TInt RunError(TInt aError);

private:
	enum TCommandMode
		{
		ECommandInit,	//Get parameter from user input
		ECommandRunning //tcp test run
		};
	
private:
	CConsoleBase* iConsole;
	RBuf		 iChars;
	TCommandMode iCommandMode;
	
	CTcpProcess* iTcp;
	CUdpProcess* iUdp;
	TBool iMode; //ETrue server EFalse client
	TBool iIsTcp;
	
	//The defaut addr, format like 'addr port' (192.168.2.101 5000)
	const TDesC& iDefaultAddr;
	TInt iIndex;
	CTcpCommand& iOwner;
	
	};

#endif //TCPTEST_H
