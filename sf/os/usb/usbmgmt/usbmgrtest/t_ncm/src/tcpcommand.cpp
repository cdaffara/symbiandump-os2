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

#include "tcpcommand.h"
#include "tcptest.h"
#include "ncmtestconsole.h"

_LIT(KTcpClientCommandDescription, "Create tcp connection from H4 to PC");
_LIT(KTcpServerCommandDescription, "Create tcp connection from PC to H4");
_LIT(KUdpClientCommandDescription, "Create udp connection from H4 to PC");
_LIT(KUdpServerCommandDescription, "Create udp connection from PC to H4");


const TInt KDefaultPortFrom = 5000;

CTcpCommand* CTcpCommand::NewL(CUsbNcmConsole& aTestConsole, TUint aKey, TBool aIsTcp, TBool aIsServer)
	{
	LOG_STATIC_FUNC_ENTRY
	
	CTcpCommand* self = new(ELeave) CTcpCommand(aTestConsole, aKey, aIsTcp, aIsServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CTcpCommand::~CTcpCommand()
	{
	for(TInt i = 0; i < KMaxTcpTestCount; i++)
		{
		delete iTestArray[i];
		}	
	}

CTcpCommand::CTcpCommand(CUsbNcmConsole& aTestConsole, TUint aKey, TBool aIsTcp, TBool aIsServer)
	: CNcmCommandBase(EPriorityStandard, aTestConsole, aKey), iIsTcp(aIsTcp), iIsServer(aIsServer)
	{
	}

void CTcpCommand::ConstructL()
    {
    if (iIsServer)
        {
        if (iIsTcp)
            {
            SetDescription(KTcpServerCommandDescription());
            }
        else
            {
            SetDescription(KUdpServerCommandDescription());
            }
        }
    else
        {
        if (iIsTcp)
            {
            SetDescription(KTcpClientCommandDescription());
            }
        else
            {
            SetDescription(KUdpClientCommandDescription());
            }
        }
    }

void CTcpCommand::DoCommandL()
	{
	LOG_FUNC
	
	//Create an CTcpTestConsole object to run the tcp test.
	TBool noRun = ETrue;
	for(TInt i = 0; i < KMaxTcpTestCount; i++)
		{
		if(!iTestArray[i])
			{
			//create the default parameter for tcp test
			//  as a server : the parameter is port (Listen at which port)
			//  as a client : the parameter is ip and port. IP is got from NCM's IP + 1
			//                   (The DHCP server will assign "NCM's IP+1" to PC.)
			RBuf addr;
			addr.CreateL(30);
			
			if(!iIsServer)
				{
				TInetAddr serverIP;
				serverIP.Input(_L("192.168.3.100"));
				//serverIP.Input(iTestConsole.GetDisplayItem(ENcmConnIpItem));
				TUint32 serverAddr = serverIP.Address();
				TUint32 hostId = (serverAddr & ~KInetAddrNetMaskC) + 1;
				if (hostId >= 255)
					 {
					 hostId = 1;
					 }
				TUint32 clientAddr = (serverAddr & KInetAddrNetMaskC) | hostId;
				TInetAddr clientIP(clientAddr, 0);
				clientIP.Output(addr);			
				}
			
			addr.AppendFormat(_L(" %d"), KDefaultPortFrom+i);
			//Create a tcp test console
			iTestArray[i] = CTcpTestConsole::NewL(iIsTcp, iIsServer, addr, i, *this);
			noRun = EFalse;
			addr.Close();
			break;
			}
		}
	if(noRun)
		{
		//The count of tcp test is up to the max value. 
		CUsbNcmConsoleEvent* event = CUsbNcmConsoleEvent::NewL();
		TInt count = KMaxTcpTestCount;
		event->iEvent.AppendFormat(_L("The test connection is up to %d.Please close some."), count);
		iTestConsole.NotifyEvent(event);			
		}
	}

void CTcpCommand::CloseTcpTest(TInt aIndex)
	{
	__ASSERT_ALWAYS(((aIndex < KMaxTcpTestCount) && (aIndex >= 0)), Panic(ENcmArrayBound));

	delete iTestArray[aIndex];
	iTestArray[aIndex] = NULL;
	}

