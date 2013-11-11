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


#include "ncmtestconsole.h"
#include "ncmtestdefines.hrh"
#include "commandengine.h"
#include "devicewatcher.h"
#include "servicewatcher.h"
#include "ncmwatcher.h"
#include "exitcommand.h"
#include "setncmiapcommand.h"
#include "simpancommand.h"
#include "tcpcommand.h"

#include <commdb.h>
#include <commdbconnpref.h>
#include <nifman.h>
#include <es_enum.h>

//#include <ncm/ncmuiinterfaces.h>
using namespace UsbNcm;

//The app parameter
_LIT(KArgDemo, "DEMO");

//Set the display item length
//every display item has fixed length. It will be displayed screen permanently.
//The details about how to use display item is in CUsbNcmConsole::DrawL()
TInt gDisplayItemLength[] = {
			DISPLAY_USB_SERVICE_STATE_LEN,			//EUsbServiceStateItem
			DISPLAY_USB_DEVICE_STATE_LEN,			//EUsbDeviceStateItem
			DISPLAY_NCM_CONN_STATE_LEN,			//ENcmConnStatusItem
			DISPLAY_NCM_CONN_IP_LEN,				//ENcmConnIpItem
			DISPLAY_UPLINK_CONN_STATE_LEN,			//EUplinkConnStatusItem
			DISPLAY_UPLINK_CONN_IP_LEN,				//EUplinkConnIpItem
			DISPLAY_NCM_BTPAN_LEN,				//ENcmBtPanItem
			DISPLAY_AGENT_STATE_LEN,				//ENcmAgentStateItem
			DISPLAY_AUTH_SETTING_LEN,				//EAuthorisationSettingItem
			DISPLAY_NCM_CONNECTION_STATE
							};

void Panic(TInt aPanic)
	{
	User::Panic(_L("NcmTestConsole"), aPanic);
	}


void RunConsoleL()
	{
	TInt cmdLineLength(User::CommandLineLength());
	HBufC* cmdLine = HBufC::NewMaxLC(cmdLineLength);
	TPtr cmdLinePtr = cmdLine->Des();
	User::CommandLine(cmdLinePtr);

	TLex args(*cmdLine);
	// args are separated by spaces
	args.SkipSpace(); 
	
	TPtrC cmdToken = args.NextToken();
	HBufC* tc = HBufC::NewLC(80);
	*tc = cmdToken;
	
	TBool isDemo = EFalse;
	
	//Find if there is 'DEMO' or not.
	while (tc->Length())
		{
		TInt pos = tc->FindF(KArgDemo);
		if ( pos != KErrNotFound )
			{ 
			isDemo = ETrue;
			break;
			}
		
		// next parameter
		*tc = args.NextToken();
		}
	CleanupStack::PopAndDestroy(tc);
	CleanupStack::PopAndDestroy(cmdLine);

	CUsbNcmConsole* console = CUsbNcmConsole::NewLC(isDemo);
	console->StartL();
	CleanupStack::PopAndDestroy(console);
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* activeScheduler = new CActiveScheduler;
	TInt err = KErrNoMemory;
	if(cleanup && activeScheduler)
		{
		CActiveScheduler::Install(activeScheduler);
		TRAP(err,RunConsoleL());
		}
	delete activeScheduler;
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

CUsbNcmConsoleEvent* CUsbNcmConsoleEvent::NewL()
	{	
	CUsbNcmConsoleEvent* self = new(ELeave) CUsbNcmConsoleEvent();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	
	}

CUsbNcmConsoleEvent::CUsbNcmConsoleEvent()
	{	
	}

CUsbNcmConsoleEvent::~CUsbNcmConsoleEvent()
	{
	iLink.Deque();
	iEvent.Close();
	}
	
void CUsbNcmConsoleEvent::ConstructL()
	{
	User::LeaveIfError(iEvent.Create(NUM_CHARACTERS_ON_LINE-1));	
	}


CUsbNcmConsole* CUsbNcmConsole::NewLC(TBool aDemo)
/**
Constructs a CUsbNcmConsole object.
  @param  aDemo 	Indicate start NCM control app and NCM automaticly or manually

When iDemo is ETrue, means ncmtestconsole starts as a demo. It will ask user to set ncm iap and
uplink iap.Then user can experience the NCM function by plug/unplug usb cable. 
*/
	{
	LOG_STATIC_FUNC_ENTRY

	CUsbNcmConsole* self = new(ELeave) CUsbNcmConsole(aDemo);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CUsbNcmConsole::~CUsbNcmConsole()
	{
	LOG_FUNC

	//If Ncm control app is not stopped, stop it.
	//destroy all commands
	delete iKeys;

	//destroy event list
	TDblQueIter<CUsbNcmConsoleEvent> iter(iEventList);
	CUsbNcmConsoleEvent* event = NULL;
	while((event = iter++) != NULL)
		{
		delete event;
		}

	//delete iStartWatcherCallback;

	//destroy all watches
	delete iSharedStateWatcher;
	delete iDeviceWatcher;
	delete iServiceWatcher;
	
	iUsb.Close();
		
	delete iConsole;

	//Destroy all display items
	for(TInt i=0; i<ELastItem; i++)
		{
		iDisplayArray[i].Close();
		}
	iDisplayArray.Close();

	Cancel();
	
	}

CUsbNcmConsole::CUsbNcmConsole(TBool aDemo)
	: CActive(EPriorityLow) // Low so all notifications that want to be serviced will be done first
	, iHelp(EFalse)
	, iDemo(aDemo)
	, iEventList(_FOFF(CUsbNcmConsoleEvent, iLink))
	{
	CActiveScheduler::Add(this);
	}

void CUsbNcmConsole::ConstructL()
	{
	LOG_FUNC
	
	//Init Display Item
	iDisplayArray.ReserveL(ELastItem);
	CreateDisplayItemL();

	iConsole = Console::NewL(KUsbNcmConsoleTitle, TSize(-1,-1));
	User::LeaveIfError(iUsb.Connect());
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("iUsb.Connect() successful"));
	
	//Init event list
	for(TInt i=0; i< NUM_EVENT_ON_SCREEN; ++i)
		{
		CUsbNcmConsoleEvent* nullEvent = CUsbNcmConsoleEvent::NewL();
		iEventList.AddFirst(*nullEvent);
		}
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Create event list successful"));		


	//Start watcher
	//iDeviceWatcher = CDeviceWatcher::NewL(*this);
	//iServiceWatcher = CServiceWatcher::NewL(*this);
	// iUplinkConnectionWatcher = CUplinkWatcher::NewL(*this);
	
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Create watchers successful"));

	//create command engine
	iKeys = CNcmCommandEngine::NewL(*this);
	
	//add commands
	CNcmCommandBase* command = NULL;

	//the command to set ncm iap
	command = CSetNcmIapCommand::NewL(*this, 'n');
	iKeys->RegisterCommand(command);
	//Get the default iap used by ncm
	TInt defaultIap = 0;
	((CSetNcmIapCommand*)command)->GetDefaultIapL(defaultIap);
	SetLocalIapId(defaultIap);	

	//the command to simulate PAN actvie/inactive
	command = CSimPANCommand::NewL(*this, 'p');
	iKeys->RegisterCommand(command);

	//the command to create a connection from PC to H4 and make data transfer
	command = CTcpCommand::NewL(*this, 'v', ETrue, ETrue);
	iKeys->RegisterCommand(command);
	//the command to create a connection from H4 to PC and make data transfer
	command = CTcpCommand::NewL(*this, 'i', ETrue, EFalse);
	iKeys->RegisterCommand(command);

	command = CTcpCommand::NewL(*this , 'r', EFalse, ETrue);
	iKeys->RegisterCommand(command);
	command = CTcpCommand::NewL(*this , 's', EFalse, EFalse);
	iKeys->RegisterCommand(command);

	//here to add new command
	
	//the command to quit 
	command = CExitCommand::NewL(*this, 'q');
	iKeys->RegisterCommand(command);
	
	//Monitor the status published by NCM agent
	iSharedStateWatcher = CSharedStateWatcher::NewL(*this);
	}

void CUsbNcmConsole::StartL()
	{
	//Output the init state to screen
	DrawL();
	//Do StartWatcherCommand if 'demo' is the command parameter
	if(iDemo)
		{
//		iStartWatcherCallback->CallBack();
		}
	CActiveScheduler::Start();
	}
	
void CUsbNcmConsole::Stop() const
	{
	CActiveScheduler::Stop();
	}

void CUsbNcmConsole::DoCancel()
	{
	}

void CUsbNcmConsole::RunL()
	{
	DrawL();
	}

TInt CUsbNcmConsole::RunError(TInt aError)
	{
	User::Panic(_L("CUsbNcmConsole"), aError);
	return aError;
	}

void CUsbNcmConsole::ScheduleDraw(TUint aKey)
/**
Called by commands and monitors.To notify main console there's new infomation to show.
  @param  aKey 	Display type. '1'-'9' - display the command help
  							  'E' - display the events.
  							  '0' - back from command help	
*/
	{
	iHelp = aKey;
	if(!IsActive())
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}
	
RUsb& CUsbNcmConsole::Usb()
	{
	return iUsb;
	}

void CUsbNcmConsole::SetLocalIapId(TInt aId)
/**
Set the Ncm Iap which will be monitored and displayed on screen
  @param  aId 	The Ncm Iap Id
*/
	{
	iLocalIapId = aId;
	ScheduleDraw('E');
	}

void CUsbNcmConsole::NotifyEvent(CUsbNcmConsoleEvent* aEvent)
/**
Called by commands and monitors. To tell main console to display the new event.
  @param  aEvent 	Event reported by commands and monitors
*/
	{
	//Console can't display a NULL event
	__ASSERT_ALWAYS(aEvent, Panic(ENcmEventIsNull));
	//Console can't display a string more than one line
	__ASSERT_ALWAYS(aEvent->iEvent.Length() <= NUM_CHARACTERS_ON_LINE, Panic(ENcmEventLengthValid));

	iEventList.AddFirst(*aEvent);
	CUsbNcmConsoleEvent* theLastEvent = iEventList.Last();
	delete theLastEvent;

	ScheduleDraw('E');
	}

void CUsbNcmConsole::DrawL()
/**
Display all infomation on screen. 
There are two parts on the screen. One is permanent info. It will display all kinds of states such as USB device state,
USB service state, NCM state, and so on. The other is event info show. It will display the events reported by monitors
and commands. It also used to show command help when user need command help. 
*/
	{
	iConsole->ClearScreen();
	// First line is the server version number 
	TVersionName versionName = iVersion.Name();
	iConsole->Printf(_L(
		//          1         2         3         4         5
		// 12345678901234567890123456789012345678901234567890123
		"Server Version: %S             %S\n"
		),
		&versionName, &iDisplayArray[EAuthorisationSettingItem]
		);

	
	iConsole->Printf(_L("Service: %S    Device: %S\n"), 
			&iDisplayArray[EUsbServiceStateItem], &iDisplayArray[EUsbDeviceStateItem]);
	iConsole->Printf(_L("NCM Iap:%2d state:%S IP:%S\n"), 
			iLocalIapId, &iDisplayArray[ENcmConnStatusItem], &iDisplayArray[ENcmConnIpItem]);
	iConsole->Printf(_L("Active:%S NcmConnection:%S\n"), 
			&iDisplayArray[ENcmBtPanItem], &iDisplayArray[ENcmAgentStateItem]);
    iConsole->Printf(_L("Ncm connection:%S\n"), &iDisplayArray[ENcmConnectionStateItem]);
	
	if ((iHelp > 0) && (iHelp <=9))
		{
		iConsole->Printf(_L("Press 0 to quit help\n\n"));
		//Display the command help
		iKeys->PrintHelp(iHelp);
		}
	else
		{
		iConsole->Printf(_L("Press 1 to 9 to get menu help\n\n"));
		//Display the event list
		TDblQueIter<CUsbNcmConsoleEvent> iter(iEventList);
		CUsbNcmConsoleEvent* event = NULL;
		while((event = iter++) != NULL)
			{
			iConsole->Printf(event->iEvent.Left(NUM_CHARACTERS_ON_LINE-1));
			iConsole->Printf(_L("\n"));
			}
		}
	}
	

void CUsbNcmConsole::CreateDisplayItemL()
/**
Init the display item.
*/
	{
	for(TInt i = 0; i < ELastItem; i++)
		{
		RBuf buf;
		iDisplayArray.AppendL(buf);
		iDisplayArray[i].CreateL(gDisplayItemLength[i]);
		}
	}

void CUsbNcmConsole::SetDisplayItem(TInt aIndex, TDes& aInfo)
/**
Called by commands and monitors to show the changed info in screen
*/
	{
	//Prevent array is out of bound
	__ASSERT_ALWAYS(((aIndex < ELastItem) && (aIndex >= 0)), Panic(ENcmDisplayItemVaild));
	
	iDisplayArray[aIndex].SetLength(0);
	iDisplayArray[aIndex].Copy(aInfo);
	
	ScheduleDraw('E');
	}

TInt CUsbNcmConsole::LocalIapId() const
	{
	return iLocalIapId;
	}

const TDes& CUsbNcmConsole::GetDisplayItem(TInt aIndex)
	{
	//Prevent array is out of bound
	__ASSERT_ALWAYS(((aIndex < ELastItem) && (aIndex >= 0)), Panic(ENcmDisplayItemVaild));

	return iDisplayArray[aIndex];
	}


TInt CUsbNcmConsole::DoCommandCallback(TAny *aThisPtr)
	{
	CNcmCommandBase* command = static_cast<CNcmCommandBase*>(aThisPtr);
	TRAPD(err,command->DoCommandL());
	if(err != KErrNone)
		{
		User::Panic(_L("DoCommandCallback"), err);
		}
	return err;
	}


TInt CUsbNcmConsole::DrawAvailableIapsL()
/**
Display commsdb's iaps on console
@return the count of iaps
*/
	{
	LOG_FUNC
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("CNcmTestUtil::DrawAvailableIapsL"));

	iConsole->Printf(_L("CommDB IAPs:\n"));
	iConsole->Printf(_L("ID: Name\n"));
	
	//Get commsdb handle
	CCommsDatabase* db = CCommsDatabase::NewL();
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Get CommsDatabase ok"));	
	CleanupStack::PushL(db);
	
	//Get Iap table infomation
	CCommsDbTableView* iapTableView = db->OpenTableLC(TPtrC(IAP));
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("OpenTableLC ok"));

	
	TUint32 iapId;
	TBuf<128> iapName;
	TInt iapCount = 0;
	
	for (TInt iapTableErr = iapTableView->GotoFirstRecord(); iapTableErr == KErrNone; iapTableErr = iapTableView->GotoNextRecord())
		{
		//Get every IAP
		iapTableView->ReadTextL(TPtrC(COMMDB_NAME), iapName);
		iapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId);
		
		__FLOG_STATIC2(KSubSys, KLogComponent , _L8("Read: %d: %S"), iapId, &iapName);

		iConsole->Printf(_L("%d: %S\n"), iapId, &iapName);		
		iapCount++;
		}

	if (0 == iapCount)
		{
		iConsole->Printf(_L("No available IAP!\n"));
		}
		
	CleanupStack::PopAndDestroy(2); // iapTableView, db
	return iapCount;
	
	}

TBool CUsbNcmConsole::AttachConnectionL(TInt aIap, RConnection& aConnection)
/**
Try to attach the connection specified by iIapId. If attach failed or no specified connection,return false
*/
	{
	LOG_FUNC
	__FLOG_STATIC1(KSubSys, KLogComponent , _L8("AttachConnectionL aIap=%d"), aIap);

	TBool result = ETrue;
	TUint count = 0;
	//Get active connections
	User::LeaveIfError(aConnection.EnumerateConnections(count));
	__FLOG_STATIC2(KSubSys, KLogComponent , _L8("active connection count = %d, watch iap = %d"), count, aIap);

	TBool hasConnection = EFalse;
	for(TInt i = 1; i <= count; i++)
		{
		TPckgBuf<TConnectionInfo> info;
		User::LeaveIfError(aConnection.GetConnectionInfo(i,info));
		TUint32 id = info().iIapId;
		__FLOG_STATIC2(KSubSys, KLogComponent , _L8("aIap=%d, id=%d"), aIap, id);
		
		if(id == aIap)
			{
			hasConnection = ETrue;
			//The connection is the connection specified by aIap
			__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Got it!"));			

			//Attach the connection
			TInt err = aConnection.Attach(info, RConnection::EAttachTypeMonitor);	
			if(err != KErrNone)
				{
				result = EFalse;
				__FLOG_STATIC1(KSubSys, KLogComponent , _L8("Attach error[%d]"), err);
				}
			else
				{
				__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Attach it!"));	
				}
			break;
			}
		}

	if(!hasConnection)
		{
		//connection not found;
		__FLOG_STATIC0(KSubSys, KLogComponent , _L8("No connection found!"));			
		result = EFalse;		
		}

	return result;

	}
