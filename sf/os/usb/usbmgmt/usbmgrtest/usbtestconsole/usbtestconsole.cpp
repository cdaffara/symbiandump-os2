/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "usbtestconsole.h"

#include <e32cons.h>
#include <e32debug.h>

#include <d32otgdi_errors.h>
#include <d32usbdi_errors.h>
#include <d32usbdi.h>

#include <usb/usbshared.h>

static TBool verbose = EFalse;

const TUint KTimerAWaitBConnect = 30*1000000;  

#define PANIC Panic(__LINE__)

#define LOG(A,B) if (verbose) RDebug::Print(_L("UsbTestConsole: " L##A),B)

#define PNT(A)	 if (verbose) RDebug::Print(_L("UsbTestConsole: " L##A))

void Panic(TInt aLine)
	{
	if (verbose) RDebug::Printf("UsbTestConsole: PANIC line=%d", aLine);
	User::Panic(_L("USBTESTCONSOLE"), aLine);
	}

_LIT(KArgVerbose, "VERBOSE");

void RunConsoleL()
	{
	TInt cmdLineLength(User::CommandLineLength());
	HBufC* cmdLine = HBufC::NewMaxLC(cmdLineLength);
	TPtr cmdLinePtr = cmdLine->Des();
	User::CommandLine(cmdLinePtr);

	TLex args(*cmdLine);
	args.SkipSpace(); // args are separated by spaces
	
	// first arg is the exe name, skip it
	TPtrC cmdToken = args.NextToken();
	HBufC* tc = HBufC::NewLC(80);
	*tc = cmdToken;
	
	while (tc->Length())
		{
		TInt pos = tc->FindF(KArgVerbose);
		
		if ( pos != KErrNotFound )
			{ 
			verbose = ETrue;
			}
		
		// next parameter
		*tc = args.NextToken();
		}
	CleanupStack::PopAndDestroy(tc);
	CleanupStack::PopAndDestroy(cmdLine);

	CUsbTestConsole* console = CUsbTestConsole::NewLC();
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

XUsbTestConsoleEvent::~XUsbTestConsoleEvent()
	{
	iLink.Deque();
	iEvent.Close();
	}
	
	
	
	
CUsbTestConsoleKeys* CUsbTestConsoleKeys::NewL(CUsbTestConsole& aTestConsole)
	{
	CUsbTestConsoleKeys* self = new(ELeave) CUsbTestConsoleKeys(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUsbTestConsoleKeys::~CUsbTestConsoleKeys()
	{
	Cancel();

	TDblQueIter<CUsbTestConsoleTextEntryBase> iter(iEntryConsoles);
	CUsbTestConsoleTextEntryBase* event = NULL;
	while((event = iter++) != NULL)
		{
		delete event;
		}
		
	delete iUsbManStarter;
	delete iUsbManStoper;
	delete iUsbManTryStarter;
	delete iUsbManTryStoper;
	}

CUsbTestConsoleKeys::CUsbTestConsoleKeys(CUsbTestConsole& aTestConsole)
	: CActive(EPriorityStandard)
	, iTestConsole(aTestConsole)
	, iEntryConsoles(_FOFF(CUsbTestConsoleTextEntryBase, iLink))
	{
	CActiveScheduler::Add(this);
	}

void CUsbTestConsoleKeys::ConstructL()
	{
	iTestConsole.iConsole->Read(iStatus);
	SetActive();
	}

void CUsbTestConsoleKeys::DoCancel()
	{
	iTestConsole.iConsole->ReadCancel();
	}

void CUsbTestConsoleKeys::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch(iTestConsole.iConsole->KeyCode())
		{
	case '[':
		{
		RUsb sess;
		TInt err = sess.Connect();
		sess.Close();
		
		User::After(4000000);
		
		err = sess.Connect(); 

		XUsbTestConsoleEvent* event = NewConsoleEventL();
		event->iEvent.AppendFormat(_L("T:USBMAN Test Err[%d]"), err);
		iTestConsole.NotifyEvent(event);
		
		sess.Close();
		}
		break;
	case 'm':
		{
		TInt val = 1;
		TInt err = RProperty::Set(KUidUsbManCategory,KUsbRequestSessionProperty,val);
		XUsbTestConsoleEvent* event = NewConsoleEventL();
		event->iEvent.AppendFormat(_L("T:USBMAN VBus marshalling - err=[%d]"),err);
		iTestConsole.NotifyEvent(event);
		}
		break;
	case 'H': case 'h': case '?': 

		iTestConsole.ScheduleDraw('H');
		break;

	case 'q': case 'Q':
		iTestConsole.Stop();
		return;
		
	case 's':
		// Toggle Start
		if(!iUsbManStarter)
			{
			CUsbManStarter::NewL(iUsbManStarter, iTestConsole);
			}
		else
			{
			iUsbManStarter->DestroyL();
			}
		break;
		
	case 'S':
		// Toggle Stop
		if(!iUsbManStoper)
			{
			// Because we cannot tell query the actual state of Function Driver loading
			// when we shutdown we also automatically disable loading.
			iTestConsole.Usb().DisableFunctionDriverLoading();
			iFunctionDriverLoading = EFalse;
			iTestConsole.SetDriverLoading(CUsbTestConsole::EDisabled);

			CUsbManStoper::NewL(iUsbManStoper, iTestConsole);
			}
		else
			{
			iUsbManStoper->DestroyL();
			}
		break;
		
	case 't':
		// Toggle TryStart
		if(!iUsbManTryStarter)
			{
			CUsbManTryStarter::NewL(iUsbManTryStarter, iTestConsole, iPersonalityId);
			}
		else
			{
			iUsbManTryStarter->DestroyL();
			}
		break;

	case 'T':
		// Toggle TryStop
		if(!iUsbManTryStoper)
			{
			// Because we cannot tell query the actual state of Function Driver loading
			// when we shutdown we also automatically disable loading.
			iTestConsole.Usb().DisableFunctionDriverLoading();
			iFunctionDriverLoading = EFalse;
			iTestConsole.SetDriverLoading(CUsbTestConsole::EDisabled);

			CUsbManTryStoper::NewL(iUsbManTryStoper, iTestConsole);
			}
		else
			{
			iUsbManTryStoper->DestroyL();
			}
		break;
		
	case 'p': case 'P':
		// Enter personality id
			{
			CUsbTestConsoleTextEntryBase* entry = CUsbTestConsoleTextEntry<TInt>::NewL(iPersonalityId, _L("Personality Id"));
			iEntryConsoles.AddLast(*entry);
			}
		break;
		
	case 'c': case 'C':
		// Toggle Control Session
			{
			iControlSession = !iControlSession;
			TInt err = iTestConsole.Usb().SetCtlSessionMode(iControlSession);
			if(err != KErrNone)
				{
				iControlSession = !iControlSession;
				}
			else
				{
				iTestConsole.SetControlSession(iControlSession);
				if(!iControlSession)
					{
					// This is set to unknown because as soon as we relinquish the controlling session
					// someone else could be altering the state.
					iFunctionDriverLoading = EFalse;
                    iTestConsole.SetDriverLoading(CUsbTestConsole::EUnknown);
					}
				}
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("G:SetControlSession:Err[%d]"), err);
			iTestConsole.NotifyEvent(event);
			}
		break;

	case 'b':
		// Bus Request
			{
			TInt err = iTestConsole.Usb().BusRequest();
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("G:BusRequest:Err[%d]"), err);
			iTestConsole.NotifyEvent(event);
			}
		break;

	case 'r':
	    // Respond to SRP
			{
			TInt err = iTestConsole.Usb().BusRespondSrp();
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("G:BusRespondSrp:Err[%d]"), err);
			iTestConsole.NotifyEvent(event);
			}
			break;

	case 'R':
		// Auto-respond to SRP
			{
			iTestConsole.SetAutoSrpResponseState( !iTestConsole.GetAutoSrpResponseState() );
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("G:Auto-Respond to SRP (%d)"), iTestConsole.GetAutoSrpResponseState());
			iTestConsole.NotifyEvent(event);
			}
			break;
			
	case 'x': case 'X':
	    // Clear Bus Error State
			{
			TInt err = iTestConsole.Usb().BusClearError();
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("G:BusClearError:Err[%d]"), err);
			iTestConsole.NotifyEvent(event);
			}
			break;

	case 'B':
		// Bus Drop
			{
			TInt err = iTestConsole.Usb().BusDrop();
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("G:BusDrop:Err[%d]"), err);
			iTestConsole.NotifyEvent(event);
			}
		break;
		
	case 'e': case 'E':
			{
			TBool functionDriverLoading = !iFunctionDriverLoading;
			CUsbTestConsole::TFdfDriverLoadingState state = functionDriverLoading ? CUsbTestConsole::EEnabled : CUsbTestConsole::EDisabled;
			TInt err = KErrNone;
			if(functionDriverLoading)
				{
				err = iTestConsole.Usb().EnableFunctionDriverLoading();
				}
			else
				{
				iTestConsole.Usb().DisableFunctionDriverLoading();
				}

			if(err == KErrNone)
				{
				iFunctionDriverLoading = functionDriverLoading;
				iTestConsole.SetDriverLoading(state);
				}
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			event->iEvent.AppendFormat(_L("H:FunctionDriverLoading[%d] Err[%d]"), functionDriverLoading, err);
			iTestConsole.NotifyEvent(event);
			}
		break;

		case 'A': case 'a': 
			{
			XUsbTestConsoleEvent* event = NewConsoleEventL();
			switch( iTestConsole.iDeviceType )
				{
				case CUsbTestConsole::ELogitechHeadset:
					{
												// 12345678901234567890123456789012345678901234567890123
					event->iEvent.AppendFormat(_L("G:Audio will play OK on this Logitech headset"));
					}
					break;
				case CUsbTestConsole::EGenericDevice:
					{
												// 12345678901234567890123456789012345678901234567890123
					event->iEvent.AppendFormat(_L("G:Audio on this device is not supported"));
					}
					break;
				case CUsbTestConsole::ENoDevice:
					{
												// 12345678901234567890123456789012345678901234567890123
					event->iEvent.AppendFormat(_L("G:There is no device connected!"));
					}
					break;
				}
			iTestConsole.NotifyEvent(event);
			}
		break;
		
	case 'v': case 'V':
		{
		TBool oldVerbose = verbose;
		
		verbose = ETrue;
		
		if (oldVerbose)
			{
			PNT("Switching Logging Off");
			}
		else
			{
			PNT("Switching Logging On");
			}
		
		verbose = !oldVerbose;

		break;
		}
		
	default:
		// Unrecognised key
		XUsbTestConsoleEvent* event = NewConsoleEventL();
		event->iEvent.AppendFormat(_L("G:Unknown:Key[%c]"), iTestConsole.iConsole->KeyCode());
		iTestConsole.NotifyEvent(event);
		break;
		}
	iTestConsole.iConsole->Read(iStatus);
	SetActive();
	}
	
XUsbTestConsoleEvent* CUsbTestConsoleKeys::NewConsoleEventL()
	{
	XUsbTestConsoleEvent* event = new(ELeave) XUsbTestConsoleEvent;
	CleanupStack::PushL(event);
	User::LeaveIfError(event->iEvent.Create(CUsbTestConsole::KNumCharactersOnLine-1));
	CleanupStack::Pop();
	return event;
	}
	


CUsbTestConsole* CUsbTestConsole::NewLC()
	{
	CUsbTestConsole* self = new(ELeave) CUsbTestConsole;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CUsbTestConsole::~CUsbTestConsole()
	{
	PNT("Closing Console");

	delete iKeys;

	TDblQueIter<XUsbTestConsoleEvent> iter(iEventList);
	XUsbTestConsoleEvent* event = NULL;
	while((event = iter++) != NULL)
		{
		delete event;
		}

	delete iMessageWatcher;
	delete iHostEventWatcher;
	delete iOtgStateWatcher;
	delete iVBusWatcher;
	delete iIdPinWatcher;
	delete iDeviceStateWatcher;
	delete iServiceStateWatcher;
	delete iTimer;  

	Cancel();

	PNT("Closing USB Session");

	iUsb.Close();
	
	delete iConsole;
	}

CUsbTestConsole::CUsbTestConsole()
	: CActive(EPriorityLow) // Low so all notifications that want to be serviced will be done first
	, iHelp(EFalse)
	, iAutoSrpResponse(EFalse)
	, iDeviceType(ENoDevice)
	, iEventList(_FOFF(XUsbTestConsoleEvent, iLink))
	{
	CActiveScheduler::Add(this);
	}

void CUsbTestConsole::ConstructL()
	{
	iConsole = Console::NewL(KUsbTestConsoleTitle, TSize(-1,-1));
	User::LeaveIfError(iUsb.Connect());
	
	for(TInt i=0; i<KNumEventsOnScreen; ++i)
		{
        XUsbTestConsoleEvent* nullEvent = new(ELeave) XUsbTestConsoleEvent;
		iEventList.AddFirst(*nullEvent);
		}

	iServiceStateWatcher = CServiceStateWatcher::NewL(*this);
	iDeviceStateWatcher = CDeviceStateWatcher::NewL(*this);
	iIdPinWatcher = CIdPinWatcher::NewL(*this);
	iVBusWatcher = CVBusWatcher::NewL(*this);
    iConnectionIdleWatcher = CConnectionIdleWatcher::NewL(*this);
	iOtgStateWatcher = COtgStateWatcher::NewL(*this);
	iHostEventWatcher = CHostEventWatcher::NewL(*this);
	iMessageWatcher = CMessageWatcher::NewL(*this);
	iTimer = CUsbTestTimer::NewL(*this);  
	SetDriverLoading(EUnknown);
	SetDeviceType(ENoDevice);
	SetAutoSrpResponseState(EFalse);

	// After everything else, enable interactivity.
	iKeys = CUsbTestConsoleKeys::NewL(*this);
	}
CUsbTestTimer* CUsbTestConsole::Timer() const
	{
	return iTimer;
	}


void CUsbTestConsole::StartL()
	{
	// Get everything running
	CActiveScheduler::Start();
	}
	
void CUsbTestConsole::Stop() const
	{
	CActiveScheduler::Stop();
	}

void CUsbTestConsole::DoCancel()
	{
	// Don't need to do anything as the AO is completed straight away.
	}

void CUsbTestConsole::RunL()
	{
	__ASSERT_ALWAYS(iStatus.Int() == KErrNone, PANIC);
	Draw();
	}

void CUsbTestConsole::ScheduleDraw(TUint aKey)
	{
	if(!IsActive())
		{
		iHelp = (aKey=='H') ? !iHelp : EFalse;
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);

		TSize size = iConsole->ScreenSize();
		iConsole->SetCursorPosAbs(TPoint(size.iWidth-1, 0));
		iConsole->Write(_L("*"));
		}
	}
	
RUsb& CUsbTestConsole::Usb()
	{
	return iUsb;
	}
	
void CUsbTestConsole::SetServiceState(TUsbServiceState aServiceState)
	{
	switch(aServiceState)
		{
    case EUsbServiceIdle:
    	iServStatus =
    		//  12345678901
			_L("Idle       ");
    	break;

	case EUsbServiceStarting:
    	iServStatus =
    		//  12345678901
			_L("Starting   ");
		break;

	case EUsbServiceStarted:
	    iServStatus =
    		//  12345678901
			_L("Started    ");
		break;

	case EUsbServiceStopping:
    	iServStatus =
    		//  12345678901
			_L("Stopping   ");
		break;

	case EUsbServiceFatalError:
    	iServStatus =
    		//  12345678901
			_L("Error      ");
		break;

	default:
		PANIC;
		break;
		}
	LOG("Service State => %S", &iServStatus);
	ScheduleDraw('E');
	}
	
void CUsbTestConsole::SetDeviceState(TUsbDeviceState aDeviceState)
	{
	switch(aDeviceState)
		{
	case EUsbDeviceStateUndefined:
		iDevStatus =
    		//  12345678901
			_L("Undefined  ");
		break;

	case EUsbDeviceStateDefault:
		iDevStatus =
    		//  12345678901
			_L("Default    ");
		break;

	case EUsbDeviceStateAttached:
		iDevStatus =
    		//  12345678901
			_L("Attached   ");
		break;

	case EUsbDeviceStatePowered:
		iDevStatus =
    		//  12345678901
			_L("Powered    ");
		break;

	case EUsbDeviceStateConfigured:
		iDevStatus =
    		//  12345678901
			_L("Configured ");
		break;

	case EUsbDeviceStateAddress:
		iDevStatus =
    		//  12345678901
			_L("Address    ");
		break;

	case EUsbDeviceStateSuspended:
		iDevStatus =
    		//  12345678901
			_L("Suspended  ");
		break;

	default:
		PANIC;
		break;
		}
	LOG("Device State => %S", &iDevStatus);
	ScheduleDraw('E');
	}
	
void CUsbTestConsole::SetConnectionIdle(TInt aConnectionIdle)
	{
	switch(aConnectionIdle)
		{
	case 0:
		iConnectionIdle =
    		//  12345
			_L("Busy ");
		break;

	case 1:
		iConnectionIdle =
    		//  12345
			_L("Idle ");
		break;

	default:
		PANIC;
		break;
		}
	LOG("ConnectionIdle => %S", &iConnectionIdle);
	ScheduleDraw('E');
	}

void CUsbTestConsole::SetIdPin(TInt aIdPin)
	{
	switch(aIdPin)
		{
    case 0:
        iIdPin =
    		//  12345
			_L("-    ");
		break;

	case 1:
		iIdPin =
    		//  12345
			_L("+    ");
		break;

	default:
		PANIC;
		break;
		}
	LOG("Id Pin => %S", &iIdPin);
	ScheduleDraw('E');
	}
	
void CUsbTestConsole::SetVBus(TInt aVBus)
	{
	switch(aVBus)
		{
	case 0:
		iVBus =
    		//  12345
			_L("-    ");
		break;

	case 1:
		iVBus =
    		//  12345
			_L("+    ");
		break;

	default:
		PANIC;
		break;
		}
	LOG("VBus => %S", &iVBus);
	ScheduleDraw('E');
	}

void CUsbTestConsole::SetOtgState(TInt aOtgState)
	{
	switch(aOtgState)
		{
	case 0x01:
		iOtgState =
    		//  123456789012
			_L("Reset       ");
		break;

	case 0x02:
		iOtgState =
    		//  123456789012
			_L("A-Idle      ");
		break;

	case 0x04:
		iOtgState =
    		//  123456789012
			_L("A-Host      ");
		break;

	case 0x08:
		iOtgState =
    		//  1234567890123
			_L("A-Peripheral");
		break;

	case 0x10:
		iOtgState =
    		//  123456789012
			_L("A-Vbus Error");
		break;

	case 0x20:
		iOtgState =
    		//  123456789012
			_L("B-Idle      ");
		break;

	case 0x40:
		iOtgState =
    		//  1234567890123
			_L("B-Peripheral");
		break;

	case 0x80:
		iOtgState =
    		//  1234567890123
			_L("B-Host      ");
		break;
		
	default:
		iOtgState =
    		//  1234567890123
			_L("Don't Panic!");
		break;
		}
	LOG("OTG State => %S", &iOtgState);
	ScheduleDraw('E');
	}

void CUsbTestConsole::SetDriverLoading(TFdfDriverLoadingState aDriverLoading)
	{
	switch(aDriverLoading)
		{
	case EUnknown:
		iDriverLoading =
			//  123
			_L("???");
		break;
	case EDisabled:
		iDriverLoading =
			//  123
			_L("Off");
		break;
	case EEnabled:
		iDriverLoading =
			//  123
			_L("On ");
		break;
	default:
		PANIC;
		break;
		}
	LOG("Driver Loading => %S", &iDriverLoading);
	ScheduleDraw('E');
	}
	
void CUsbTestConsole::SetAttachedDevices(TUint aAttachedDevices)
	{
	if(aAttachedDevices > 999)
		{
		iAttachedDevices =
			//  123
			_L("xxx");
		}
	else
		{
		iAttachedDevices.NumFixedWidthUC(aAttachedDevices, EDecimal, 3);
		}
	LOG("Attached Devices => %S", &iAttachedDevices);
	ScheduleDraw('E');
	}
	
void CUsbTestConsole::SetControlSession(TBool aControl)
	{
	if(aControl)
		{
		iControlSession =
			//  1234567
			_L("Control");
		}
	else
		{
		iControlSession =
			//  1234567
			_L("       ");
		}
	LOG("Control Session => [%d]", aControl);
	ScheduleDraw('E');
	}

void CUsbTestConsole::SetDeviceType(TDeviceType aDeviceType)
	{
	iDeviceType = aDeviceType;

	switch ( aDeviceType )
		{
	case ENoDevice:
		iAttachedDevice =
			//  12345
			_L("     ");
		break;
	case EGenericDevice:
		iAttachedDevice =
			//  12345
			_L("<   >");
		break;
	case ELogitechHeadset:
		iAttachedDevice =
			//  12345
			_L("<<A>>");
		break;
		}
	LOG("Audio Device => [%S]", &iAttachedDevice);
	ScheduleDraw('E');
	}
	
TBool CUsbTestConsole::GetAutoSrpResponseState()
	{
	return iAutoSrpResponse;
	}
	
void CUsbTestConsole::SetAutoSrpResponseState(TBool aAutoSrpResponse)
	{
	iAutoSrpResponse = aAutoSrpResponse;
	
	if ( aAutoSrpResponse )
		{
		iSrpState = _L("Auto");
		}
	else
		{
		iSrpState = _L("    ");
		}
	}

void CUsbTestConsole::NotifyEvent(XUsbTestConsoleEvent* aEvent)
	{
	__ASSERT_ALWAYS(aEvent, PANIC);
	__ASSERT_ALWAYS(aEvent->iEvent.Length() <= KNumCharactersOnLine, PANIC);

	iEventList.AddFirst(*aEvent);
	delete iEventList.Last();
	LOG("Event => %S", &(aEvent->iEvent));
	ScheduleDraw('E');
	}

void CUsbTestConsole::Draw()
	{
	iConsole->ClearScreen();
	
	// First line is the server version number (and session state)
	TVersionName versionName = iVersion.Name();
	iConsole->Printf(_L(
//          1         2         3         4         5
// 12345678901234567890123456789012345678901234567890123
  "Server Version: %S   %S\n"
	),
	&versionName, &iControlSession
	);

	// Print "dash board"
	iConsole->Printf(_L(
//          1         2         3         4         5
// 12345678901234567890123456789012345678901234567890123
  "Device              OTG          Host          "L"%S\n"
//          12345678901        12345                123
 L"Service: %S"      L"Id Pin: %S"L"Driver Loading: %S\n"
 L"Device:  %S"      L"V. Bus: %S"L"Attached Devs:  %S\n"
 //           1234567890123           12345
 L"OTG State: %S "        L"Device: %S " L"SRP: %S\n"
	),
	&iConnectionIdle,
    &iServStatus, &iIdPin, &iDriverLoading,
	&iDevStatus, &iVBus, &iAttachedDevices,
	&iOtgState, &iAttachedDevice, &iSrpState
	);

	if (iHelp)
		{
		//                     12345678901234567890123456789012345678901234567890123
		iConsole->Printf(_L("\nHelp:Available Command Selections"));
		iConsole->Printf(_L("\n"));
		iConsole->Printf(_L("\n '?'/'h'/'H' = Help (show these menu commands)"));
		iConsole->Printf(_L("\n 'v'/'V' = Toggle 'verbose' logging"));
		iConsole->Printf(_L("\n 'q'/'Q' = Quit application"));
		iConsole->Printf(_L("\n 'c'/'C' = Toggle control session"));
		iConsole->Printf(_L("\n 's' = Start()           'S' = Stop()"));
		iConsole->Printf(_L("\n 't' = TryStart()        'T' = TryStop()"));
		iConsole->Printf(_L("\n 'p'/'P' = Enter personality number (for TryStart())"));
		iConsole->Printf(_L("\n 'e'/'E' = Toggle 'enable driver loading'"));
		iConsole->Printf(_L("\n 'b' = BusRequest()      'B' = BusDrop()"));
		iConsole->Printf(_L("\n 'r' = Respond (positively) to SRP"));
		iConsole->Printf(_L("\n 'R' = Toggle SRP Auto-Response"));
		iConsole->Printf(_L("\n 'x'/'X' = Clear Bus Error (A_VBUS_ERR)"));
		iConsole->Printf(_L("\n 'a'/'A' = Check audio capability"));
		iConsole->Printf(_L("\n '[' = Perform USBMAN test"));
		}
	else
		{
		// Events...
		TDblQueIter<XUsbTestConsoleEvent> iter(iEventList);
		XUsbTestConsoleEvent* event = NULL;
		while((event = iter++) != NULL)
			{
			iConsole->Printf(_L("\n"));
			iConsole->Printf(event->iEvent.Left(KNumCharactersOnLine-1));
			}
		}
	}
	
	

CUsbTestConsoleTextEntryBase::CUsbTestConsoleTextEntryBase()
	: CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

CUsbTestConsoleTextEntryBase::~CUsbTestConsoleTextEntryBase()
	{
	Cancel();
	iLink.Deque();
	delete iEntryConsole;
	iChars.Close();
	}

void CUsbTestConsoleTextEntryBase::ConstructL(const TDesC& aEntryField)
	{
	User::LeaveIfError(iChars.Create(KMaxNumOfChars));
	iEntryConsole = Console::NewL(_L("UsbTestConsole Entry Console"), TSize(-1,-1));
	iEntryConsole->Printf(aEntryField);
	iEntryConsole->Printf(_L(": "));
	iEntryConsole->Read(iStatus);
	SetActive();
	}

void CUsbTestConsoleTextEntryBase::DoCancel()
	{
	iEntryConsole->ReadCancel();
	}

void CUsbTestConsoleTextEntryBase::RunL()
	{
	TKeyCode code = iEntryConsole->KeyCode();
	switch(code)
		{
		case EKeyEnter:
				{
				TRAPD(err, ConvertAndSetL());
				if(err == KErrNone)
					{
					delete this;
					return;
					}
				}
			break;
		case EKeyEscape:
			delete this;
			return;
		case EKeyBackspace:
			if(iChars.Length() > 0)
				{
				iEntryConsole->SetCursorPosRel(TPoint(-1, 0));
				iEntryConsole->ClearToEndOfLine();
				iChars.SetLength(iChars.Length()-1);
				}
			break;
		default:
			iChars.Append(code);
			iEntryConsole->Printf(_L("%c"), code);
			break;
		}
	iEntryConsole->Read(iStatus);
	SetActive();
	}


template<typename T>
CUsbTestConsoleTextEntry<T>* CUsbTestConsoleTextEntry<T>::NewL(T& aValue, const TDesC& aEntryField)
	{
	CUsbTestConsoleTextEntry<T>* self = new(ELeave) CUsbTestConsoleTextEntry<T>(aValue);
	CleanupStack::PushL(self);
	self->ConstructL(aEntryField);
	CleanupStack::Pop(self);
	return self;
	}

template<typename T>
CUsbTestConsoleTextEntry<T>::~CUsbTestConsoleTextEntry()
	{
	}

template<typename T>
CUsbTestConsoleTextEntry<T>::CUsbTestConsoleTextEntry(T& aValue)
	: CUsbTestConsoleTextEntryBase()
	, iValue(aValue)
	{
	}

template<>
void CUsbTestConsoleTextEntry<TUint>::ConvertAndSetL()
	{
	TLex lex(iChars);
	
	lex.SkipSpaceAndMark();

	TBool hex = ETrue;
	if (lex.Get() != '0' || lex.Get() != 'x')
		{
		lex.UnGetToMark();
		hex = EFalse;
		}
		
	TUint val;
	TRadix rad = hex ? EHex : EDecimal;
	User::LeaveIfError(lex.Val(val, rad));
	
	iValue = val;
	}
	
template<>
void CUsbTestConsoleTextEntry<TInt>::ConvertAndSetL()
	{
	TLex lex(iChars);
	
	lex.SkipSpaceAndMark();

	TUint val;
	User::LeaveIfError(lex.Val(val, EDecimal));
	
	iValue = val;
	}








	


CEventNotifier::CEventNotifier(TInt aPriority)
	: CActive(aPriority)
	{
	}

XUsbTestConsoleEvent* CEventNotifier::NewConsoleEventL()
	{
	XUsbTestConsoleEvent* event = new(ELeave) XUsbTestConsoleEvent;
	CleanupStack::PushL(event);
	User::LeaveIfError(event->iEvent.Create(CUsbTestConsole::KNumCharactersOnLine-1));
	CleanupStack::Pop();
	return event;
	}

void CEventNotifier::RunL()
	{
	DoRunL(NewConsoleEventL());
	}


CServiceStateWatcher* CServiceStateWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CServiceStateWatcher* self = new(ELeave) CServiceStateWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CServiceStateWatcher::~CServiceStateWatcher()
	{
	Cancel();
	}

CServiceStateWatcher::CServiceStateWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	, iServiceState(EUsbServiceIdle)
	{
	CActiveScheduler::Add(this);
	}

void CServiceStateWatcher::ConstructL()
	{
	iTestConsole.Usb().ServiceStateNotification(iServiceState, iStatus);
	SetActive();

	TUsbServiceState serviceState;
	User::LeaveIfError(iTestConsole.Usb().GetServiceState(serviceState));
	iTestConsole.SetServiceState(serviceState);
	}

void CServiceStateWatcher::DoCancel()
	{
	iTestConsole.Usb().ServiceStateNotificationCancel();
	}

void CServiceStateWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	iTestConsole.SetServiceState(iServiceState);
	TPtrC res(NULL, 0);
	_LIT(KIdle, "Idle");
	_LIT(KStarting, "Starting");
	_LIT(KStarted, "Started");
	_LIT(KStopping, "Stopping");
	_LIT(KError, "Error");
	switch(iServiceState)
		{
	case EUsbServiceIdle:
		res.Set(KIdle);
		break;
	case EUsbServiceStarting:
		res.Set(KStarting);
		break;
	case EUsbServiceStarted:
		res.Set(KStarted);
		break;
	case EUsbServiceStopping:
		res.Set(KStopping);
		break;
	case EUsbServiceFatalError:
		res.Set(KError);
		break;
	default:
		PANIC;
		break;
		}
		
	iTestConsole.Usb().ServiceStateNotification(iServiceState, iStatus);
	SetActive();

	aEvent->iEvent.AppendFormat(_L("D:ServiceState [%S]"), &res);
	iTestConsole.NotifyEvent(aEvent);

	// Seems like there can be race conditions
	TUsbServiceState serviceState;
	User::LeaveIfError(iTestConsole.Usb().GetServiceState(serviceState));

	iTestConsole.SetServiceState(serviceState);
	}

	
CDeviceStateWatcher* CDeviceStateWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CDeviceStateWatcher* self = new(ELeave) CDeviceStateWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CDeviceStateWatcher::~CDeviceStateWatcher()
	{
	Cancel();
	}

CDeviceStateWatcher::CDeviceStateWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CDeviceStateWatcher::ConstructL()
	{
	iTestConsole.Usb().DeviceStateNotification(0xffffffff, iDeviceState, iStatus);
	SetActive();

	TUsbDeviceState deviceState;
	User::LeaveIfError(iTestConsole.Usb().GetDeviceState(deviceState));
	iTestConsole.SetDeviceState(deviceState);
	}

void CDeviceStateWatcher::DoCancel()
	{
	iTestConsole.Usb().DeviceStateNotificationCancel();
	}

void CDeviceStateWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	iTestConsole.SetDeviceState(iDeviceState);
	TPtrC res(NULL, 0);
	_LIT(KUndefined, "Undefined");
    _LIT(KDefault, "Default");
    _LIT(KAttached, "Attached");
    _LIT(KPowered, "Powered");
    _LIT(KConfigured, "Configured");
    _LIT(KAddress, "Address");
    _LIT(KSuspended, "Suspended");
	switch(iDeviceState)
		{
	case EUsbDeviceStateUndefined:
		res.Set(KUndefined);
		break;
	case EUsbDeviceStateDefault:
		res.Set(KDefault);
		break;
	case EUsbDeviceStateAttached:
		res.Set(KAttached);
		break;
	case EUsbDeviceStatePowered:
		res.Set(KPowered);
		break;
	case EUsbDeviceStateConfigured:
		res.Set(KConfigured);
		break;
	case EUsbDeviceStateAddress:
		res.Set(KAddress);
		break;
	case EUsbDeviceStateSuspended:
		res.Set(KSuspended);
		break;
	default:
		PANIC;
		break;
		}

    iTestConsole.Usb().DeviceStateNotification(0xffffffff, iDeviceState, iStatus);
	SetActive();

	aEvent->iEvent.AppendFormat(_L("D:DeviceState [%S]"), &res);
	iTestConsole.NotifyEvent(aEvent);

	// Seems like there can be race conditions
	TUsbDeviceState deviceState;
	User::LeaveIfError(iTestConsole.Usb().GetDeviceState(deviceState));

	iTestConsole.SetDeviceState(deviceState);
	}
	
CConnectionIdleWatcher* CConnectionIdleWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CConnectionIdleWatcher * self = new(ELeave) CConnectionIdleWatcher (aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CConnectionIdleWatcher::~CConnectionIdleWatcher()
	{
	Cancel();
	iConnectionIdleProp.Close();
	}

CConnectionIdleWatcher::CConnectionIdleWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CConnectionIdleWatcher::ConstructL()
	{
	User::LeaveIfError(iConnectionIdleProp.Attach(KUidUsbManCategory, KUsbOtgConnectionIdleProperty));
	iConnectionIdleProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	TInt err = iConnectionIdleProp.Get(val);
	LOG("CConnectionIdleWatcher::ConstructL iConnectionIdleProp.Get(val) => val=%d", val);
	User::LeaveIfError(err);
	iTestConsole.SetConnectionIdle(val);
	}

void CConnectionIdleWatcher::DoCancel()
	{
	iConnectionIdleProp.Cancel();
	}

void CConnectionIdleWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iConnectionIdleProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iConnectionIdleProp.Get(val));

	TPtrC res(NULL, 0);
	_LIT(KConnectionIdle, "Idle");
	_LIT(KConnectionBusy, "Busy");
	switch(val)
		{
	case 0:
		res.Set(KConnectionBusy);
		break;

	case 1:
		res.Set(KConnectionIdle);
		break;

	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();

	aEvent->iEvent.AppendFormat(_L("O:ConnectionIdle [%S]"), &res);
	iTestConsole.SetConnectionIdle(val);
	iTestConsole.NotifyEvent(aEvent);
	}
	
	
CIdPinWatcher* CIdPinWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CIdPinWatcher* self = new(ELeave) CIdPinWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CIdPinWatcher::~CIdPinWatcher()
	{
	Cancel();
	iIdPinProp.Close();
	}

CIdPinWatcher::CIdPinWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CIdPinWatcher::ConstructL()
	{
	User::LeaveIfError(iIdPinProp.Attach(KUidUsbManCategory, KUsbOtgIdPinPresentProperty));
	iIdPinProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	TInt err = iIdPinProp.Get(val);
	LOG("CIdPinWatcher::ConstructL iIdPinProp.Get(val) => %d",err);
	User::LeaveIfError(err);
	iTestConsole.SetIdPin(val);
	}

void CIdPinWatcher::DoCancel()
	{
	iIdPinProp.Cancel();
	}

void CIdPinWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iIdPinProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iIdPinProp.Get(val));

	TPtrC res(NULL, 0);
	_LIT(KIdPinInserted, "Inserted");
	_LIT(KIdPinRemoved, "Removed");
	switch(val)
		{
	case 0:
		res.Set(KIdPinRemoved);
		break;

	case 1:
		res.Set(KIdPinInserted);
		break;

	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();

	aEvent->iEvent.AppendFormat(_L("O:IdPin [%S]"), &res);
	iTestConsole.SetIdPin(val);
	iTestConsole.NotifyEvent(aEvent);
	}
	
	
CVBusWatcher* CVBusWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CVBusWatcher* self = new(ELeave) CVBusWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CVBusWatcher::~CVBusWatcher()
	{
	Cancel();
	iVBusProp.Close();
	}

CVBusWatcher::CVBusWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CVBusWatcher::ConstructL()
	{
	User::LeaveIfError(iVBusProp.Attach(KUidUsbManCategory, KUsbOtgVBusPoweredProperty));
	iVBusProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iVBusProp.Get(val));
	iTestConsole.SetVBus(val);
	}

void CVBusWatcher::DoCancel()
	{
	iVBusProp.Cancel();
	}

void CVBusWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iVBusProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iVBusProp.Get(val));

	TPtrC res(NULL, 0);
	_LIT(KVBusRaised, "Raised");
	_LIT(KVBusDropped, "Dropped");
	switch(val)
		{
	case 0:
		{
		if ( iTestConsole.GetAutoSrpResponseState() ) 
			{
			(iTestConsole.Timer())->Cancel();
			}
		res.Set(KVBusDropped);
		break;
		}
	case 1:
		res.Set(KVBusRaised);
		break;

	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();

	aEvent->iEvent.AppendFormat(_L("O:VBus [%S]"), &res);
	iTestConsole.SetVBus(val);
	iTestConsole.NotifyEvent(aEvent);
	}
	
COtgStateWatcher* COtgStateWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	COtgStateWatcher* self = new(ELeave) COtgStateWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

COtgStateWatcher::~COtgStateWatcher()
	{
	Cancel();
	iOtgStateProp.Close();
	}

COtgStateWatcher::COtgStateWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void COtgStateWatcher::ConstructL()
	{
	User::LeaveIfError(iOtgStateProp.Attach(KUidUsbManCategory, KUsbOtgStateProperty));
	iOtgStateProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iOtgStateProp.Get(val));
	iTestConsole.SetOtgState(val);
	}

void COtgStateWatcher::DoCancel()
	{
	iOtgStateProp.Cancel();
	}

void COtgStateWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iOtgStateProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iOtgStateProp.Get(val));

	TPtrC res(NULL, 0);
	
	_LIT(KReset, 		"Reset"			);

	_LIT(KAIdle, 		"A-Idle"		);
	_LIT(KAHost, 		"A-Host"		);
	_LIT(KAPeripheral, 	"A-Peripheral"	);
	_LIT(KABusError, 	"A-Bus Error"	);
	
	_LIT(KBIdle, 		"B-Idle"		);
	_LIT(KBPeripheral, 	"B-Peripheral"	);
	_LIT(KBHost, 		"B-Host"		);
	
	_LIT(KUnknown, 		"Unknown"		);

	switch(val)
		{
	case EUsbOtgStateReset:
		res.Set(KReset);
		break;

	case EUsbOtgStateAIdle:
		res.Set(KAIdle);
		break;

	case EUsbOtgStateAHost:
		if ( iTestConsole.GetAutoSrpResponseState() ) 
			{
			(iTestConsole.Timer())->Cancel();
			}
		res.Set(KAHost);
		break;

	case EUsbOtgStateAPeripheral:
		res.Set(KAPeripheral);
		break;

	case EUsbOtgStateAVbusError:
		res.Set(KABusError);
		break;

	case EUsbOtgStateBIdle:
		res.Set(KBIdle);
		break;

	case EUsbOtgStateBPeripheral:
		res.Set(KBPeripheral);
		break;
		
	case EUsbOtgStateBHost:
		res.Set(KBHost);
		break;

	default:
		res.Set(KUnknown);
		break;
		}
	CleanupStack::Pop();

	aEvent->iEvent.AppendFormat(_L("O:OtgState [%S]"), &res);
	iTestConsole.SetOtgState(val);
	iTestConsole.NotifyEvent(aEvent);
	}

CHostEventWatcher* CHostEventWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CHostEventWatcher* self = new(ELeave) CHostEventWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHostEventWatcher::~CHostEventWatcher()
	{
	Cancel();
	}

CHostEventWatcher::CHostEventWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CHostEventWatcher::ConstructL()
	{
	iTestConsole.Usb().HostEventNotification(iStatus, iDeviceInfo);
	SetActive();
	iTestConsole.SetAttachedDevices(0);
	}

void CHostEventWatcher::DoCancel()
	{
	iTestConsole.Usb().HostEventNotificationCancel();
	}

void CHostEventWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	switch(iDeviceInfo.iEventType)
		{
	case EDeviceAttachment:
			{
			if(iDeviceInfo.iError == KErrNone)
				{
				User::LeaveIfError(iAttachedDevices.Append(iDeviceInfo.iDeviceId));
				}
			aEvent->iEvent.AppendFormat(_L("H:Attach[%08x] Err[%d] VidPid[%04x,%04x]"), iDeviceInfo.iDeviceId, iDeviceInfo.iError, iDeviceInfo.iVid, iDeviceInfo.iPid);
			
			if (  (iDeviceInfo.iVid == 0x046D)
			    &&(iDeviceInfo.iPid == 0x0A02)
			   )
				{
				iTestConsole.SetDeviceType(CUsbTestConsole::ELogitechHeadset);
				}
			else
				{
				iTestConsole.SetDeviceType(CUsbTestConsole::EGenericDevice);
				}
			}
		break;

	case EDriverLoad:
			{
			TPtrC res(NULL, 0);
			_LIT(KDriverLoadSuccess, "Success");
			_LIT(KDriverLoadPartialSuccess, "Warning");
			_LIT(KDriverLoadFailure, "Failure");
			switch(iDeviceInfo.iDriverLoadStatus)
				{
			case EDriverLoadSuccess:
				res.Set(KDriverLoadSuccess);
				break;
			case EDriverLoadPartialSuccess:
				res.Set(KDriverLoadPartialSuccess);
				break;
			case EDriverLoadFailure:
				res.Set(KDriverLoadFailure);
				break;
			default:
				PANIC;
				break;
				}
			aEvent->iEvent.AppendFormat(_L("H:Load[%08x] Err[%d] Status[%S]"), iDeviceInfo.iDeviceId, iDeviceInfo.iError, &res);
			}
		break;

	case EDeviceDetachment:
			{
			aEvent->iEvent.AppendFormat(_L("H:Detach [%08x]"), iDeviceInfo.iDeviceId);
			TInt ix = iAttachedDevices.Find(iDeviceInfo.iDeviceId);
			if(ix == KErrNotFound)
				{
				// This is probably caused by starting a new instance of the test console.
				break;
				}
			iAttachedDevices.Remove(ix);
			iTestConsole.SetDeviceType(CUsbTestConsole::ENoDevice);
			}
		break;
	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();
	iTestConsole.SetAttachedDevices(iAttachedDevices.Count());
	iTestConsole.NotifyEvent(aEvent);
	iTestConsole.Usb().HostEventNotification(iStatus, iDeviceInfo);
	SetActive();
	}
	
	
CMessageWatcher* CMessageWatcher::NewL(CUsbTestConsole& aTestConsole)
	{
	CMessageWatcher* self = new(ELeave) CMessageWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CMessageWatcher::~CMessageWatcher()
	{
	Cancel();
	}

CMessageWatcher::CMessageWatcher(CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CMessageWatcher::ConstructL()
	{
	iTestConsole.Usb().MessageNotification(iStatus, iMessage);
	SetActive();
	}

void CMessageWatcher::DoCancel()
	{
	iTestConsole.Usb().MessageNotificationCancel();
	}

void CMessageWatcher::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	TInt err = iStatus.Int();
	
	if (err)
		{
		aEvent->iEvent.AppendFormat(_L("O:Message[%d] Err[%d]"), iMessage, err);
		}
	else
		{
		TPtrC text(NULL, 0);

		// OTGDI
		_LIT(KMessOtgdiEventQueueOverflow,			"O:Event Queue Overflow"			);
		_LIT(KMessOtgdiStateQueueOverflow,			"O:State Queue Overflow"			);
		_LIT(KMessOtgdiMessageQueueOverflow,		"O:Message Queue Overflow"			);
		_LIT(KMessOtgdiBadState,					"O:Bad State"						);
		_LIT(KMessOtgdiStackNotStarted,				"O:Stack Not Started"				);
		_LIT(KMessOtgdiVbusAlreadyRaised,			"O:VBUS Already Raised"				);
		_LIT(KMessOtgdiSrpForbidden,				"O:SRP Forbidden"					);
		_LIT(KMessOtgdiBusControlProblem,			"O:Bus Control Problem"				);
		_LIT(KMessOtgdiVbusPowerUpError,			"O:VBUS Power Up Error"				);		
		_LIT(KMessOtgdiHnpEnableProblem,			"O:HNP Enable Problem"				);		
		_LIT(KMessOtgdiPeriphNotSupported,			"O:Peripheral Not Supported"		);		
		_LIT(KMessOtgdiVbusError,					"O:VBUS Error"						);
		_LIT(KMessOtgdiSrpTimeout,					"O:SRP Timeout"						);
		_LIT(KMessOtgdiSrpActive,					"O:SRP Already Active"				);
		_LIT(KMessOtgdiSrpNotPermitted,				"O:SRP Not Permitted"				);
		_LIT(KMessOtgdiHnpNotPermitted,				"O:HNP Not Permitted"				);
		_LIT(KMessOtgdiHnpNotEnabled,				"O:HNP Not Enabled"					);
		_LIT(KMessOtgdiHnpNotSuspended,				"O:HNP Not Suspended"				);
		_LIT(KMessOtgdiVbusPowerUpNotPermitted,		"O:VBUS Power Up Not Permitted"		);
		_LIT(KMessOtgdiVbusPowerDownNotPermitted,	"O:VBUS Power Down Not Permitted"	);
		_LIT(KMessOtgdiVbusClearErrorNotPermitted,	"O:VBUS Clear Error Not Permitted"	);
		_LIT(KMessOtgdiHnpNotResponding,			"O:HNP Not Responding"				);
		_LIT(KMessOtgdiHnpBusDrop,					"O:VBUS Drop During HNP!"			);

		// USBDI - Main

		_LIT(KMessUsbdiRequestsPending,				"U:Requests Pending"				);		
		_LIT(KMessUsbdiBadAddress,					"U:Bad Address"						);
		_LIT(KMessUsbdiNoAddress,					"U:No Address"						);
		_LIT(KMessUsbdiSetAddrFailed,				"U:Set Address Failed"				);
		_LIT(KMessUsbdiNoPower,						"U:No Power"						);
		_LIT(KMessUsbdiTooDeep,						"U:Too Deep"						);
		_LIT(KMessUsbdiIOError,						"U:IO Error"						);
		_LIT(KMessUsbdiNotConfigured,				"U:Not Configured"					);
		_LIT(KMessUsbdiTimeout,						"U:Timeout"							);
		_LIT(KMessUsbdiStalled,						"U:Stalled"							);
		_LIT(KMessUsbdiTestFailure,					"U:Test Failure"					);
		_LIT(KMessUsbdiBadState,					"U:Bad State"						);
		_LIT(KMessUsbdiDeviceSuspended,				"U:Device Suspended"				);

		// USBDI - Descriptors

		_LIT(KMessUsbdiBadDescriptorTopology,		"U:Bad Descriptor Topology"			);

		// USBDI - DevMon

		_LIT(KMessUsbdiDeviceRejected,				"U:Device Rejected"					);
		_LIT(KMessUsbdiDeviceFailed,				"U:Device failed"					);
		_LIT(KMessUsbdiBadDevice,					"U:Bad Device"						);
		_LIT(KMessUsbdiBadHubPosition,				"U:Bad Hub Position"				);
		_LIT(KMessUsbdiBadHub,						"U:Bad Hub"							);
		_LIT(KMessUsbdiEventOverflow,				"U:Event Overflow"					);
		
		// USBMAN
		
		_LIT(KMessUsbmanSrpInitiated,				"M:SRP Initiated"					);
		_LIT(KMessUsbmanSrpReceived,				"M:SRP Received"					);
		_LIT(KMessUsbmanHnpDisabled,				"M:HNP Disabled"					);
		_LIT(KMessUsbmanHnpEnabled,					"M:HNP Enabled"						);
		_LIT(KMessUsbmanVbusRaised,					"M:VBUS Raised"						);
		_LIT(KMessUsbmanVbusDropped,				"M:VBUS Dropped"					);
		_LIT(KMessUsbmanRequestSession,				"M:Request Session"					);

		_LIT(KMessUnknown, 							"*:Unknown"							);

		switch(iMessage)
			{
			// OTGDI

			case KErrUsbOtgEventQueueOverflow:			text.Set(KMessOtgdiEventQueueOverflow);			break;
			case KErrUsbOtgStateQueueOverflow:	       	text.Set(KMessOtgdiStateQueueOverflow);			break;
			case KErrUsbOtgMessageQueueOverflow:		text.Set(KMessOtgdiMessageQueueOverflow);		break;
			case KErrUsbOtgBadState:					text.Set(KMessOtgdiBadState);					break;		
			case KErrUsbOtgStackNotStarted:				text.Set(KMessOtgdiStackNotStarted);			break;	
			case KErrUsbOtgVbusAlreadyRaised:	       	text.Set(KMessOtgdiVbusAlreadyRaised);			break;
			case KErrUsbOtgSrpForbidden:				text.Set(KMessOtgdiSrpForbidden);				break;	
			case KErrUsbOtgBusControlProblem:	       	text.Set(KMessOtgdiBusControlProblem);			break;
			case KErrUsbOtgVbusPowerUpError:			text.Set(KMessOtgdiVbusPowerUpError);			break;		
			case KErrUsbOtgHnpEnableProblem:			text.Set(KMessOtgdiHnpEnableProblem);			break;		
			case KErrUsbOtgPeriphNotSupported:			text.Set(KMessOtgdiPeriphNotSupported);			break;		
			case KErrUsbOtgVbusError:			       	text.Set(KMessOtgdiVbusError);					break;		
			case KErrUsbOtgSrpTimeout:			       	text.Set(KMessOtgdiSrpTimeout);					break;		
			case KErrUsbOtgSrpActive:			       	text.Set(KMessOtgdiSrpActive);					break;		
			case KErrUsbOtgSrpNotPermitted:				text.Set(KMessOtgdiSrpNotPermitted);			break;
			case KErrUsbOtgHnpNotPermitted:				text.Set(KMessOtgdiHnpNotPermitted);			break;
			case KErrUsbOtgHnpNotEnabled:				text.Set(KMessOtgdiHnpNotEnabled);				break;
			case KErrUsbOtgHnpNotSuspended:				text.Set(KMessOtgdiHnpNotSuspended);			break;
			case KErrUsbOtgVbusPowerUpNotPermitted:  	text.Set(KMessOtgdiVbusPowerUpNotPermitted);	break;
			case KErrUsbOtgVbusPowerDownNotPermitted:	text.Set(KMessOtgdiVbusPowerDownNotPermitted);	break;
			case KErrUsbOtgVbusClearErrorNotPermitted:	text.Set(KMessOtgdiVbusClearErrorNotPermitted);	break;
			case KErrUsbOtgHnpNotResponding:			text.Set(KMessOtgdiHnpNotResponding);			break;
			case KErrUsbOtgHnpBusDrop:					text.Set(KMessOtgdiHnpBusDrop);					break;

			// USBDI - Main

			case KErrUsbRequestsPending:				text.Set(KMessUsbdiRequestsPending);			break;		
			case KErrUsbBadAddress:						text.Set(KMessUsbdiBadAddress); 				break;			
			case KErrUsbNoAddress:						text.Set(KMessUsbdiNoAddress); 					break;			
			case KErrUsbSetAddrFailed:					text.Set(KMessUsbdiSetAddrFailed); 				break;		
			case KErrUsbNoPower:						text.Set(KMessUsbdiNoPower); 					break;				
			case KErrUsbTooDeep:						text.Set(KMessUsbdiTooDeep); 					break;				
			case KErrUsbIOError:						text.Set(KMessUsbdiIOError); 					break;				
			case KErrUsbNotConfigured:					text.Set(KMessUsbdiNotConfigured); 				break;		
			case KErrUsbTimeout:						text.Set(KMessUsbdiTimeout); 					break;				
			case KErrUsbStalled:						text.Set(KMessUsbdiStalled); 					break;				
			case KErrUsbTestFailure:					text.Set(KMessUsbdiTestFailure); 				break;			
			case KErrUsbBadState:						text.Set(KMessUsbdiBadState); 					break;			
			case KErrUsbDeviceSuspended:				text.Set(KMessUsbdiDeviceSuspended); 			break;		
																									
			// USBDI - Descriptors																	
			
			case KErrUsbBadDescriptorTopology:			text.Set(KMessUsbdiBadDescriptorTopology); 		break;

			// USBDI - DevMon
			
			case KErrUsbDeviceRejected:					text.Set(KMessUsbdiDeviceRejected); 			break;		
			case KErrUsbDeviceFailed:					text.Set(KMessUsbdiDeviceFailed); 				break;		
			case KErrUsbBadDevice:						text.Set(KMessUsbdiBadDevice); 					break;			
			case KErrUsbBadHubPosition:					text.Set(KMessUsbdiBadHubPosition); 			break;		
			case KErrUsbBadHub:							text.Set(KMessUsbdiBadHub); 					break;				
			case KErrUsbEventOverflow:					text.Set(KMessUsbdiEventOverflow); 				break;		
																										
			// USBMAN																					
																										
			case KUsbMessageSrpInitiated:				text.Set(KMessUsbmanSrpInitiated);				break;
			case KUsbMessageSrpReceived:				text.Set(KMessUsbmanSrpReceived);				break;
			case KUsbMessageHnpDisabled:				text.Set(KMessUsbmanHnpDisabled);				break;
			case KUsbMessageHnpEnabled:					text.Set(KMessUsbmanHnpEnabled);				break;
			case KUsbMessageVbusRaised:					text.Set(KMessUsbmanVbusRaised);				break;
			case KUsbMessageVbusDropped:				text.Set(KMessUsbmanVbusDropped);				break;
			case KUsbMessageRequestSession:				text.Set(KMessUsbmanRequestSession);			break;
																										
			default:									text.Set(KMessUnknown);							break;
			}																							

		aEvent->iEvent.AppendFormat(_L("O:Message[%d] [%S]"), iMessage, &text);
		
		if (   ( iMessage == KUsbMessageSrpReceived )
		    && ( iTestConsole.GetAutoSrpResponseState() )
		   )
			{
			iTestConsole.Usb().BusRespondSrp();
			iTestConsole.Timer()->Start(KTimerAWaitBConnect);
			}
		else if ( iMessage == KUsbMessageRequestSession )
			{
			iTestConsole.Usb().BusRequest();
			} 
		}

	iTestConsole.Usb().MessageNotification(iStatus, iMessage);
	SetActive();
	
	iTestConsole.NotifyEvent(aEvent);
	}


	
void CUsbManStarter::NewL(CUsbManStarter*& aSelf, CUsbTestConsole& aTestConsole)
	{
	aSelf = NULL;
	CUsbManStarter* self = new(ELeave) CUsbManStarter(aSelf, aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	aSelf = self;
	}

CUsbManStarter::~CUsbManStarter()
	{
	Cancel();
	iSelf = NULL;
	}

CUsbManStarter::CUsbManStarter(CUsbManStarter*& aSelf, CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iSelf(aSelf)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CUsbManStarter::ConstructL()
	{
	iTestConsole.Usb().Start(iStatus);
	SetActive();
	}

void CUsbManStarter::DoCancel()
	{
	iTestConsole.Usb().StartCancel();
	}

void CUsbManStarter::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	aEvent->iEvent.AppendFormat(_L("G:Start:Err[%d]"), iStatus.Int());
	iTestConsole.NotifyEvent(aEvent);
	delete this;
	}

void CUsbManStarter::DestroyL()
	{
	XUsbTestConsoleEvent* event = NewConsoleEventL();
	event->iEvent.Append(_L("G:StartCancel"));
	iTestConsole.NotifyEvent(event);
	delete this;
	}
	
	
void CUsbManStoper::NewL(CUsbManStoper*& aSelf, CUsbTestConsole& aTestConsole)
	{
	aSelf = NULL;
	CUsbManStoper* self = new(ELeave) CUsbManStoper(aSelf, aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	aSelf = self;
	}

CUsbManStoper::~CUsbManStoper()
	{
	Cancel();
	iSelf = NULL;
	}

CUsbManStoper::CUsbManStoper(CUsbManStoper*& aSelf, CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iSelf(aSelf)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CUsbManStoper::ConstructL()
	{
	iTestConsole.Usb().Stop(iStatus);
	SetActive();
	}

void CUsbManStoper::DoCancel()
	{
	iTestConsole.Usb().StopCancel();
	}

void CUsbManStoper::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	aEvent->iEvent.AppendFormat(_L("G:Stop:Err[%d]"), iStatus.Int());
	iTestConsole.NotifyEvent(aEvent);
	delete this;
	}

void CUsbManStoper::DestroyL()
	{
	XUsbTestConsoleEvent* event = NewConsoleEventL();
	event->iEvent.Append(_L("G:StopCancel"));
	iTestConsole.NotifyEvent(event);
	delete this;
	}
	
	
	
void CUsbManTryStarter::NewL(CUsbManTryStarter*& aSelf, CUsbTestConsole& aTestConsole, TInt aPersonalityId)
	{
	aSelf = NULL;
	CUsbManTryStarter* self = new(ELeave) CUsbManTryStarter(aSelf, aTestConsole, aPersonalityId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	aSelf = self;
	}

CUsbManTryStarter::~CUsbManTryStarter()
	{
	Cancel();
	iSelf = NULL;
	}

CUsbManTryStarter::CUsbManTryStarter(CUsbManTryStarter*& aSelf, CUsbTestConsole& aTestConsole, TInt aPersonalityId)
	: CEventNotifier(EPriorityStandard)
	, iSelf(aSelf)
	, iTestConsole(aTestConsole)
	, iPersonalityId(aPersonalityId)
	{
	CActiveScheduler::Add(this);
	}

void CUsbManTryStarter::ConstructL()
	{
	iTestConsole.Usb().TryStart(iPersonalityId, iStatus);
	SetActive();
	}

void CUsbManTryStarter::DoCancel()
	{
	iTestConsole.Usb().StartCancel();
	}

void CUsbManTryStarter::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	aEvent->iEvent.AppendFormat(_L("G:TryStart:Err[%d]"), iStatus.Int());
	iTestConsole.NotifyEvent(aEvent);
	delete this;
	}

void CUsbManTryStarter::DestroyL()
	{
	XUsbTestConsoleEvent* event = NewConsoleEventL();
	event->iEvent.Append(_L("G:StartCancel"));
	iTestConsole.NotifyEvent(event);
	delete this;
	}
	
	
void CUsbManTryStoper::NewL(CUsbManTryStoper*& aSelf, CUsbTestConsole& aTestConsole)
	{
	aSelf = NULL;
	CUsbManTryStoper* self = new(ELeave) CUsbManTryStoper(aSelf, aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	aSelf = self;
	}

CUsbManTryStoper::~CUsbManTryStoper()
	{
	Cancel();
	iSelf = NULL;
	}

CUsbManTryStoper::CUsbManTryStoper(CUsbManTryStoper*& aSelf, CUsbTestConsole& aTestConsole)
	: CEventNotifier(EPriorityStandard)
	, iSelf(aSelf)
	, iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CUsbManTryStoper::ConstructL()
	{
	iTestConsole.Usb().TryStop(iStatus);
	SetActive();
	}

void CUsbManTryStoper::DoCancel()
	{
	iTestConsole.Usb().StopCancel();
	}

void CUsbManTryStoper::DoRunL(XUsbTestConsoleEvent* aEvent)
	{
	aEvent->iEvent.AppendFormat(_L("G:TryStop:Err[%d]"), iStatus.Int());
	iTestConsole.NotifyEvent(aEvent);
	delete this;
	}

void CUsbManTryStoper::DestroyL()
	{
	XUsbTestConsoleEvent* event = NewConsoleEventL();
	event->iEvent.Append(_L("G:StopCancel"));
	iTestConsole.NotifyEvent(event);
	delete this;
	}

CUsbTestTimer* CUsbTestTimer::NewL(CUsbTestConsole& aUsb)
	{
	CUsbTestTimer* self = new(ELeave) CUsbTestTimer(aUsb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUsbTestTimer::~CUsbTestTimer()
	{
	Cancel();
	}

void CUsbTestTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

CUsbTestTimer::CUsbTestTimer(CUsbTestConsole& aUsb)
: CTimer(EPriorityStandard)
, iTestConsole(aUsb)
	{	
	}

void CUsbTestTimer::Start(TTimeIntervalMicroSeconds32 aTime)
	{
	After(aTime);
	}

void CUsbTestTimer::RunL()
	{
		XUsbTestConsoleEvent* event = new(ELeave) XUsbTestConsoleEvent;
		CleanupStack::PushL(event);
		User::LeaveIfError(event->iEvent.Create(CUsbTestConsole::KNumCharactersOnLine-1));
		CleanupStack::Pop();
		
		TPtrC res(NULL, 0);
		_LIT(KErrMsg, "Device Not Responding");
		
		res.Set(KErrMsg);
		event->iEvent.AppendFormat(_L("T:Timer Message [%S]"), &res);
		iTestConsole.NotifyEvent(event);
	}

void CUsbTestTimer::DoCancel()
	{
	CTimer::DoCancel();
	}
