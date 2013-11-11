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

#include "tcptest.h"
#include <in_sock.h>
#include <commdbconnpref.h>
#include <es_enum.h>
#include "tcpcommand.h"
#include "ncmtestconsole.h"

_LIT8(KSendData, "TCP-Packet::HelloWorld\n");

static const TInt KMaxNumOfChars = 255;

//The title of TCP test console
_LIT(KTcpServerMode, "Tcp Server");
_LIT(KTcpClientMode, "Tcp Client");

//The default value of data size
static const TInt KDefaultDataSize = 30;

CTcpProcess* CTcpProcess::NewL(CConsoleBase& aConsole, TInetAddr& aAddr, TInt aPort, TInt aSize, TBool aMode)
	{
	CTcpProcess* self = new(ELeave) CTcpProcess(aConsole, aAddr, aPort, aSize, aMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTcpProcess::~CTcpProcess()
	{
	Cancel();
	iRecvBuf.Close();
    iSendBuf.Close();
    iSocket.Close();
    iListenSocket.Close();
    iSockServ.Close();
	}

CTcpProcess::CTcpProcess(CConsoleBase& aConsole, TInetAddr& aAddr, TInt aPort, TInt aSize, TBool aMode) 
		:  CActive(EPriorityStandard), iConsole(aConsole), iAddr(aAddr), iPort(aPort), iSize(aSize), iMode(aMode)
	{
	CActiveScheduler::Add(this);
	}

void CTcpProcess::ConstructL()
	{
	//Create the data buffer
	User::LeaveIfError(iSendBuf.Create(iSize));
	User::LeaveIfError(iRecvBuf.Create(iSize));
	iRecvSize = iSize;
	
	iProcessState = ECreateConnection;
	
	User::LeaveIfError(iSockServ.Connect());
	if(iMode)
		{
		//Listen at specified port
		TBuf<5>		 protocol = _L("tcp");
		User::LeaveIfError(iListenSocket.Open(iSockServ, protocol));
		User::LeaveIfError(iListenSocket.SetOpt(KSoReuseAddr, KProtocolInetIp, 1));
		User::LeaveIfError(iListenSocket.SetLocalPort(iPort));
		User::LeaveIfError(iListenSocket.Listen(5));
		User::LeaveIfError(iSocket.Open(iSockServ));
		iListenSocket.Accept(iSocket, iStatus);
		iConsole.Printf(_L("\nWait for a connection at port[%d].\n"), iPort);
		}
	else
		{
        RConnection conn;  
        User::LeaveIfError(conn.Open(iSockServ));
        TCommDbConnPref pref;
        pref.SetIapId(13);
        pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
        User::LeaveIfError(conn.Start(pref));
        
		//Connect to specified server
		User::LeaveIfError(iSocket.Open(iSockServ, KAfInet, KSockStream, KProtocolInetTcp, conn));
		iSocket.Connect(iAddr, iStatus);

		iConsole.Printf(_L("\nConnecting....\n"));
		}
	SetActive();
	}


void CTcpProcess::RunL()
	{
	switch(iProcessState)
		{
		case ECreateConnection:
			//Get result of connect
			if(iStatus.Int() != KErrNone)
				{
				iConsole.Printf(_L("Connect err[%d].\nPress any key to quit."), iStatus.Int());
				return;
				}
			else
				{
				//A TCP connection is created.
				if(iMode)
					{
					iConsole.Printf(_L("Get a connection.\n"));
					}
				else
					{
					iConsole.Printf(_L("\nConnected.\n"));					
					}
				iProcessState = EDataTransfer;

				//Ready for read data
				iSocket.RecvOneOrMore(iRecvBuf, 0, iStatus, iRecvSize);
				__FLOG_STATIC1(KSubSys, KLogComponent , _L8("Ready for read data"), iRecvSize);
				SetActive();
				if(!iMode)
					{
					//As a client, send data first.
					iSendBuf.SetLength(iSize);
					iSendBuf.Repeat(KSendData());

					iConsole.Printf(_L("Send data."));
					PrintData(iSendBuf);
					TInt sendLen = SendDataL(iSendBuf, iSize);
					if(sendLen != iSize)
						{
						iConsole.Printf(_L("The length of data sent is not equal to requested! requested[%d], sent[%d]"),
											iSize, sendLen);
						}
					}
				}
			break;

		case EDataTransfer:
			//In data transfer, some data is received
			iConsole.Printf(_L("recv Package, size[%d], status[%d]\n"), iRecvSize(), iStatus.Int());
			if((KErrEof == iStatus.Int()) || (KErrDisconnected == iStatus.Int()))
				{
				iConsole.Printf(_L("Connection closed!"));
				return;
				}
			else if(KErrNone == iStatus.Int())
				{
				iConsole.Printf(_L("Receive data."));
				PrintData(iRecvBuf);

				if(iMode)
					{
					//As a server, send back the data received
					TInt len = SendDataL(iRecvBuf, iRecvSize());
					iConsole.Printf(_L("Send back the data. len[%d]"), len);
					}
				}	
			iRecvBuf.SetLength(0);
			iRecvSize = iSize;
			iSocket.RecvOneOrMore(iRecvBuf, 0, iStatus, iRecvSize);
			SetActive();	
			
			break;
		}
	}

void CTcpProcess::DoCancel()
	{
	switch(iProcessState)
		{
		case ECreateConnection:
			if(iMode)
				{
				iListenSocket.CancelAccept();
				}
			else
				{
				iSocket.CancelConnect();
				}
			break;
		case EDataTransfer:
			iSocket.CancelRecv();
			break;
		}
	}

TInt CTcpProcess::RunError(TInt aError)
	{
	User::Panic(_L("CTcpProcess"), aError);
	return aError;
	}

TInt CTcpProcess::SendDataL(TDes8& aData, TInt aSize)
/**
Send data.
  @return the size of data sent
 */
	{
	TRequestStatus status;
	TSockXfrLength sendSize = aSize;
	iSocket.Send(aData, 0, status, sendSize);
	User::WaitForRequest(status);
	
	switch(status.Int())
		{
		case KErrEof:
			iConsole.Printf(_L("Connection closed!"));
			return 0;		
		case KErrNone:
			iConsole.Printf(_L("Send successfully."));
			break;
		default:
			User::LeaveIfError(status.Int());
			break;
		}
	return sendSize();
	}

void CTcpProcess::PrintData(TDes8& aData)
	{
	iConsole.Printf(_L("The data is: \n"));
	for(TInt i=0; i< aData.Length();i++)
		{
		iConsole.Printf(_L("%c"), aData[i]);
		}
	iConsole.Printf(_L("\n"));
	}

CUdpProcess* CUdpProcess::NewL(CConsoleBase& aConsole, TInetAddr& aAddr,
        TInt aPort, TInt aSize, TBool aMode)
    {
    CUdpProcess* self = new (ELeave) CUdpProcess(aConsole, aAddr, aPort,
            aSize, aMode);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CUdpProcess::~CUdpProcess()
    {
    Cancel();
    iRecvBuf.Close();
    iSendBuf.Close();
    iSocket.Close();
    iListenSocket.Close();
    iSockServ.Close();
    }

CUdpProcess::CUdpProcess(CConsoleBase& aConsole, TInetAddr& aAddr,
        TInt aPort, TInt aSize, TBool aMode) :
    CActive(EPriorityStandard), iConsole(aConsole), iAddr(aAddr),
            iPort(aPort), iSize(aSize), iMode(aMode)
    {
    CActiveScheduler::Add(this);
    }

void CUdpProcess::ConstructL()
    {
    //Create the data buffer
    User::LeaveIfError(iSendBuf.Create(iSize));
    User::LeaveIfError(iRecvBuf.Create(iSize));
    iRecvSize = iSize;

    User::LeaveIfError(iSockServ.Connect());

    // Start NCM IAP
    RConnection conn;
    User::LeaveIfError(conn.Open(iSockServ));
    TCommDbConnPref pref;
    pref.SetIapId(13);
    pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
    User::LeaveIfError(conn.Start(pref));
    
    User::LeaveIfError(iSocket.Open(iSockServ, KAfInet, KSockDatagram,
            KProtocolInetUdp, conn));
    
    iConsole.Printf(_L("In constructL, port=%d"), iAddr.Port());
    
    if (iMode)
        {
        iProcessState = EDataTransfer;
       
        User::LeaveIfError(iSocket.SetLocalPort(iPort));
        iSocket.RecvFrom(iRecvBuf, iPeerAddr, 0, iStatus);
        iConsole.Printf(_L("\nWait for UDP incoming data at port[%d]...\n"),iPort);
        SetActive();
        }
    else 
        {
        iProcessState = EDataSending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        }
    }


void CUdpProcess::RunL()
    {
    __FLOG_STATIC0(KSubSys, KLogComponent , _L8("CUdpProcess::RunL"));
    switch (iProcessState)
        {
        case EDataSending:
            //As a client, send data first.
            iSendBuf.SetLength(iSize);
            iSendBuf.Repeat(KSendData());
            iConsole.Printf(_L("Send data.."));
            PrintData(iSendBuf);
            iConsole.Printf(_L("In RunL, port=%d"), iAddr.Port());
            TInt sendLen = SendDataL(iSendBuf, iAddr, iSize);
            
            if (sendLen != iSize)
                {
                iConsole.Printf(
                        _L("The length of data sent is not equal to requested! requested[%d], sent[%d]"),
                        iSize, sendLen);
                }
            break;

        case EDataTransfer:
            //In data transfer, some data is received
            iConsole.Printf(_L("recv Package, size[%d], status[%d]\n"),
                    iRecvSize(), iStatus.Int());
            if (KErrNone == iStatus.Int())
                {
                iConsole.Printf(_L("Receive data."));
                PrintData(iRecvBuf);

                if (iMode)
                    {
                    //As a server, send back the data received
                    TInt len = SendDataL(iRecvBuf, iPeerAddr, iRecvBuf.Length());
                    iConsole.Printf(_L("Send back the data. len[%d]"), len);
                    }
                }
            else
                {
                iConsole.Printf(_L("Something is wrong..."));
                return;
                }

            iRecvBuf.SetLength(0);
            iRecvSize = iSize;
            //iListenSocket.RecvFrom(iRecvBuf, iPeerAddr, 0, iStatus);
            iSocket.RecvFrom(iRecvBuf, iPeerAddr, 0, iStatus);
            SetActive();
            iConsole.Printf(_L("\nWait for UDP incoming data at port[%d]...\n"),iPort);
            break;
        }
    }

void CUdpProcess::DoCancel()
{
    switch(iProcessState)
    {
    case EDataTransfer:
        //iListenSocket.CancelRecv();
        iSocket.CancelRecv();
        break;
    }
}

TInt CUdpProcess::RunError(TInt aError)
{
    User::Panic(_L("CUdpProcess"), aError);
    return aError;
}

TInt CUdpProcess::SendDataL(TDes8& aData, TInetAddr& aAddr, TInt aSize)
    /**
      Send data.
      @return the size of data sent
      */
{
    TRequestStatus status;
    
    TInt port = aAddr.Port();
    iConsole.Printf(_L("Before sending, size = %d, port=%d\n"), aSize, port);
    TSockXfrLength sendSize = 0; //aSize;
    iSocket.SendTo(aData, aAddr, 0, status, sendSize);
    User::WaitForRequest(status);
    iConsole.Printf(_L("Sending result = %d, and sent=%d\n"), status.Int(), sendSize());

    switch(status.Int())
    {
    case KErrEof:
        iConsole.Printf(_L("Connection closed!"));
        return 0;       
    case KErrNone:
        iConsole.Printf(_L("Send successfully.\n"));
        break;
    default:
        User::LeaveIfError(status.Int());
        break;
    }
    return sendSize();
}

void CUdpProcess::PrintData(TDes8& aData)
{
    iConsole.Printf(_L("The data is: \n"));
    for(TInt i=0; i< aData.Length();i++)
    {
    iConsole.Printf(_L("%c"), aData[i]);
    }
    iConsole.Printf(_L("\n"));
}



CTcpTestConsole* CTcpTestConsole::NewL(TBool aIsTcp, TBool aMode, TDesC& aDefautAddr, TInt aIndex, CTcpCommand& aOwner)
	{
	CTcpTestConsole* self = new(ELeave) CTcpTestConsole(aIsTcp, aMode, aDefautAddr, aIndex, aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	
	}

CTcpTestConsole::CTcpTestConsole(TBool aIsTcp, TBool aMode, TDesC& aDefautAddr, TInt aIndex, CTcpCommand& aOwner) 
	: CActive(EPriorityStandard),iCommandMode(ECommandInit),  iIsTcp(aIsTcp), iMode(aMode), 
	  iDefaultAddr(aDefautAddr), iIndex(aIndex), iOwner(aOwner)
	{	
	CActiveScheduler::Add(this);
	}

CTcpTestConsole::~CTcpTestConsole()
	{
	Cancel();
	delete iTcp;
	delete iUdp;
    delete iConsole;
    iChars.Close();
	}


void CTcpTestConsole::ConstructL()
	{
	if(iMode)
		{
		iConsole = Console::NewL(KTcpServerMode(),TSize(KConsFullScreen,KConsFullScreen));
		}
	else
		{
		iConsole = Console::NewL(KTcpClientMode(),TSize(KConsFullScreen,KConsFullScreen));
		}	
	User::LeaveIfError(iChars.Create(KMaxNumOfChars));
	//Generate the default value and display on screen, user can modify it if they want to use other value.
	iChars.AppendFormat(_L("%S %d"), &iDefaultAddr, KDefaultDataSize);
	Help();
	//wait user input
	iConsole->Read(iStatus);
	SetActive();
	}

void CTcpTestConsole::Help()
	{
	iConsole->Printf(_L("Please change the parameters, then press enter\n"));
	if(iMode)
		{
		iConsole->Printf(_L("   The parameters are port size \n"));
		iConsole->Printf(_L("   port - The port of the Tcp/udp Server listen to \n"));
		iConsole->Printf(_L("   size - the max package size \n"));
		}
	else
		{
		iConsole->Printf(_L("   The parameters are destAddr port size \n"));
		iConsole->Printf(_L("   destAddr - The ip address of Tcp/udp Client connect to.\n"));
		iConsole->Printf(_L("   port - The port of Tcp Client connect to \n"));
		iConsole->Printf(_L("   size - the package size \n"));
		}
	//Display the default value
	iConsole->Printf(_L("%S"), &iChars);
		
	}
TBool CTcpTestConsole::StartL()
	{
	TLex args(iChars);
	// args are separated by spaces
	args.SkipSpace(); 
	
	TInetAddr addr;
	TInt size;

	if(!iMode)
		{
		//Get ip addr
		TPtrC cmdAddr = args.NextToken();
		if(!args.Eos())
			{
			if(KErrNone == addr.Input(cmdAddr))
				{
				args.Inc();
				}
			else
				{
				return EFalse;
				}
			}
		else
			{
			return EFalse;
			}
		}
	
	//Get port
	TInt port;
	if(KErrNone != args.Val(port))
		{
		return EFalse;
		}
	addr.SetPort(port);

	//Get pkg size
	args.Inc();
	if(KErrNone != args.Val(size))
		{
		return EFalse;
		}
	
	iCommandMode = ECommandRunning;
    if (iIsTcp)
        {
        iConsole->Printf(_L("Test for TCP...\n"));
        iTcp = CTcpProcess::NewL(*iConsole, addr, port, size, iMode);
        }
    else
        {
        iConsole->Printf(_L("Test for UDP...\n"));
        iUdp = CUdpProcess::NewL(*iConsole, addr, port, size, iMode);
        }
		
	return ETrue;

	}

void CTcpTestConsole::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CTcpTestConsole::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	switch(iCommandMode)
		{
		case ECommandInit:
			{
			TKeyCode code = iConsole->KeyCode();
			switch(code)
				{
				case EKeyEnter:
					{
					//User input ok
					if(!StartL())
						{
						Help();
						}
					}
					break;
				case EKeyEscape:
					//connection has not been created. User want to cancel and quit
					iOwner.CloseTcpTest(iIndex);
					return;
				case EKeyBackspace:
					if(iChars.Length() > 0)
						{
						iConsole->SetCursorPosRel(TPoint(-1, 0));
						iConsole->ClearToEndOfLine();
						iChars.SetLength(iChars.Length()-1);
						}
					break;
				default:
					iChars.Append(code);
					iConsole->Printf(_L("%c"), code);
					break;
				}
			iConsole->Read(iStatus);
			SetActive();
			}
			break;
		case ECommandRunning:
			//Connection has been created. User quit by press any key.
			iOwner.CloseTcpTest(iIndex);
			break;
		}
	}

TInt CTcpTestConsole::RunError(TInt aError)
	{
	User::Panic(_L("CTcpTestConsole"), aError);
	return aError;
	}


