/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "usbviewer.h"

#include <e32cons.h>
#include <e32debug.h>

#include <d32otgdi_errors.h>
#include <d32usbdi_errors.h>
#include <usb/usbshared.h>
// For moving windows using scancode events
#include "e32event.h"
#include "e32svr.h" 


#define PANIC Panic(__LINE__)
#define LOG(A,B) RDebug::Print(_L("UsbViewer: " L##A),B)
#define PNT(A)	 RDebug::Print(_L("UsbViewer: " L##A))

void Panic(TInt aLine)
	{
	RDebug::Printf("UsbViewer: PANIC line=%d", aLine);
	User::Panic(_L("USBVIEWER"), aLine);
	}

void RunViewerL()
	{
	CUsbViewer* viewer = CUsbViewer::NewLC();
	viewer->Start();
	CleanupStack::PopAndDestroy(viewer);
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
		PNT("*** UsbViewer E32Main ***\n");
		TRAP(err, RunViewerL());
		}
	delete activeScheduler;
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}


XUsbViewerEvent::~XUsbViewerEvent()
	{
	iLink.Deque();
	iEvent.Close();
	}
	
	

CUsbViewer* CUsbViewer::NewLC()
	{
	PNT("CUsbViewer::NewLC");
	CUsbViewer* self = new(ELeave) CUsbViewer;
	CleanupStack::PushL(self);
	self->ConstructL();
	PNT("CUsbViewer::NewLC - Constructed Viewer");
	return self;
	}
	
CUsbViewer::~CUsbViewer()
	{
	PNT("CUsbViewer::~CUsbViewer");

	delete iShutdownMonitor;

	delete iMessageWatcher;
	delete iHostEventWatcher;
	delete iOtgStateWatcher;
	delete iConnIdleWatcher;
	delete iVBusWatcher;
	delete iIdPinWatcher;
	delete iDeviceStateWatcher;
	delete iServiceStateWatcher;

	delete iUserMsgQWatcher;
	iUserMsgQ.Close();

	TDblQueIter<XUsbViewerEvent> iter(iEventList);
	XUsbViewerEvent* event = NULL;
	while((event = iter++) != NULL)
		{
		delete event;
		} 

	Cancel();

	PNT("Closing USB Session");
	iUsb.Close();
	delete iConsole;
	}

CUsbViewer::CUsbViewer()
	: CActive(EPriorityLow) // Low so all notifications that want to be serviced will be done first
	, iAutoSrpResponse(EFalse)
	, iDeviceType(ENoDevice)
	, iEventList(_FOFF(XUsbViewerEvent, iLink))
	{
	CActiveScheduler::Add(this);
	}

void CUsbViewer::ConstructL()
	{
	PNT("CUsbViewer::ConstructL");
	iConsole = Console::NewL(KUsbViewerTitle, TSize(KViewerNumCharactersOnLine, KNumLinesInWindow));
	Move(-3, -3);
	User::LeaveIfError(iUsb.Connect());
	
	for(TInt i=0; i<KNumEventsInWindow; ++i)
		{
        XUsbViewerEvent* nullEvent = new(ELeave) XUsbViewerEvent;
		iEventList.AddFirst(*nullEvent);
		}

	// Open message queue to display user messages
	User::LeaveIfError(iUserMsgQ.OpenGlobal(KControlAppViewerMsgQName));
	iUserMsgQWatcher = CUserMsgQWatcher::NewL(*this);

	iServiceStateWatcher = CServiceStateWatcher::NewL(*this);
	iDeviceStateWatcher = CDeviceStateWatcher::NewL(*this);
	iIdPinWatcher = CIdPinWatcher::NewL(*this);
	iVBusWatcher = CVBusWatcher::NewL(*this);
	iConnIdleWatcher = CConnectionIdleWatcher::NewL(*this);
	iOtgStateWatcher = COtgStateWatcher::NewL(*this);
	iHostEventWatcher = CHostEventWatcher::NewL(*this);
	iMessageWatcher = CMessageWatcher::NewL(*this);
	SetDriverLoading(EUnknown);
	SetDeviceType(ENoDevice);

	iShutdownMonitor = CShutdownMonitor::NewL(*this);
	}

void CUsbViewer::Move(TInt aX, TInt aY)
	{
	TRawEvent event;

	event.Set(TRawEvent::EKeyDown, EStdKeyLeftShift);
	UserSvr::AddEvent(event);

	if (aX)
		{
		if ( aX > 0 )
			{
			// Move to the right...
			for(TInt i=aX; i; i--)
				{
				event.Set(TRawEvent::EKeyDown, EStdKeyRightArrow);
				UserSvr::AddEvent(event);
				event.Set(TRawEvent::EKeyUp, EStdKeyRightArrow);
				UserSvr::AddEvent(event);
				}
			}
		else
			{
			// Move to the Left...
			for(TInt i=aX; i; i++)
				{
				event.Set(TRawEvent::EKeyDown, EStdKeyLeftArrow);
				UserSvr::AddEvent(event);
				event.Set(TRawEvent::EKeyUp, EStdKeyLeftArrow);
				UserSvr::AddEvent(event);
				}
			}
		}
	
	if (aY)
		{
		if ( aY > 0 )
			{
			// Move downwards...
			for(TInt i=aY; i; i--)
				{
				event.Set(TRawEvent::EKeyDown, EStdKeyDownArrow);
				UserSvr::AddEvent(event);
				event.Set(TRawEvent::EKeyUp, EStdKeyDownArrow);
				UserSvr::AddEvent(event);
				}
			}
		else
			{
			// Move upwards...
			for(TInt i=aY; i; i++)
				{
				event.Set(TRawEvent::EKeyDown, EStdKeyUpArrow);
				UserSvr::AddEvent(event);
				event.Set(TRawEvent::EKeyUp, EStdKeyUpArrow);
				UserSvr::AddEvent(event);
				}
			}
		}

	event.Set(TRawEvent::EKeyUp, EStdKeyLeftShift);
	UserSvr::AddEvent(event);
	}

void CUsbViewer::Start()
	{
	// Get everything running
	CActiveScheduler::Start();
	}
	
void CUsbViewer::Stop() const
	{
	CActiveScheduler::Stop();
	}

void CUsbViewer::DoCancel()
	{
	// Don't need to do anything as the AO is completed straight away.
	}

void CUsbViewer::RunL()
	{
	__ASSERT_ALWAYS(iStatus.Int() == KErrNone, PANIC);
	Draw();
	}

void CUsbViewer::ScheduleDraw()
	{
	PNT("CUsbViewer::ScheduleDraw");
	if(!IsActive())
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);

		TSize size = iConsole->ScreenSize();
		iConsole->SetCursorPosAbs(TPoint(size.iWidth-1, 0));
		iConsole->Write(_L("*"));
		}
	}
	
RUsb& CUsbViewer::Usb()
	{
	return iUsb;
	}

RMsgQueue<TBuf<KViewerNumCharactersOnLine> >& CUsbViewer::UserMsgQ()
	{
	return iUserMsgQ;
	}
	
void CUsbViewer::SetServiceState(TUsbServiceState aServiceState)
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
	ScheduleDraw();
	}
	
void CUsbViewer::SetDeviceState(TUsbDeviceState aDeviceState)
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
	ScheduleDraw();
	}
	
void CUsbViewer::SetIdPin(TInt aIdPin)
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
	ScheduleDraw();
	}
	
void CUsbViewer::SetVBus(TInt aVBus)
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
	ScheduleDraw();
	}

void CUsbViewer::SetConnectionIdle(TInt aConnIdle)
	{
	switch(aConnIdle)
		{
	case 0:
		iConnIdle =
    		//  12345
			_L("-    ");
		break;

	case 1:
		iConnIdle =
    		//  12345
			_L("+    ");
		break;

	default:
		PANIC;
		break;
		}
	LOG("Connection Idle => %S", &iConnIdle);
	ScheduleDraw();
	}

void CUsbViewer::SetOtgState(TInt aOtgState)
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
	ScheduleDraw();
	}

void CUsbViewer::SetDriverLoading(TFdfDriverLoadingState aDriverLoading)
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
	ScheduleDraw();
	}
	
void CUsbViewer::SetAttachedDevices(TUint aAttachedDevices)
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
	ScheduleDraw();
	}

void CUsbViewer::SetDeviceType(TDeviceType aDeviceType)
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
	ScheduleDraw();
	}

void CUsbViewer::NotifyEvent(XUsbViewerEvent* aEvent)
	{
	LOG("CUsbViewer::NotifyEvent	event length = %d", aEvent->iEvent.Length());
	__ASSERT_ALWAYS(aEvent, PANIC);
	__ASSERT_ALWAYS(aEvent->iEvent.Length() <= KViewerNumCharactersOnLine, PANIC);

	iEventList.AddFirst(*aEvent);
	delete iEventList.Last();

	LOG("Event => %S", &(aEvent->iEvent));
	ScheduleDraw();
	}

void CUsbViewer::Draw()
	{
	PNT("CUsbViewer::Draw");
	iConsole->ClearScreen();

	iConsole->Printf(_L(
//          1         2         3  
// 12345678901234567890123456789012
//               
//        12345         12345678901
  "IdPin: %S" L"SrvSt : %S\n"      
//        12345         12345678901
 L"VBus : %S" L"DevSt : %S\n"
//        123           123
 L"DLoad: %S  " L"# Devs: %S\n"   
//        12345         1234
 L"Dev  : %S" L"        \n"
//        12345        1234567890123
 L"C.Idl: %S" L"OTG  : %S \n"   
	),
	&iIdPin,			&iServStatus, 
	&iVBus,				&iDevStatus, 
	&iDriverLoading,	&iAttachedDevices,
	&iAttachedDevice,	
	&iConnIdle,			&iOtgState
	);

	// Events...
	TDblQueIter<XUsbViewerEvent> iter(iEventList);
	XUsbViewerEvent* event = NULL;
	while((event = iter++) != NULL)
		{
		iConsole->Printf(_L("\n"));
		iConsole->Printf(event->iEvent.Left(KViewerNumCharactersOnLine));
		}
	}
	


CShutdownMonitor* CShutdownMonitor::NewL(MShutdownInterface& aParentUsbViewer)
	{
	CShutdownMonitor* self = new(ELeave) CShutdownMonitor(aParentUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CShutdownMonitor::CShutdownMonitor(MShutdownInterface& aParentUsbViewer)
	: CActive(EPriorityLow) // Low so all notifications that want to be serviced will be done first
	, iParentUsbViewer(aParentUsbViewer)
	{
	CActiveScheduler::Add(this);
	}

void CShutdownMonitor::ConstructL()
	{
	// Monitor the KUsbControlAppShutdownKey property to tell us when to shut down
	TInt err = iShutdownProp.Attach(KUidUsbControlAppCategory, KUsbControlAppShutdownKey);
	LOG("CShutdownMonitor::ConstructL	 iShutdownProp.Attach() => %d", err);
	User::LeaveIfError(err);
	iShutdownProp.Subscribe(iStatus);
	SetActive();
	TInt val;
	// Make sure the cuurent value is 0 - shut down when this changes to 1
	err = iShutdownProp.Get(val);
	LOG("CShutdownMonitor::ConstructL()	 iShutdownProp.Get(val)		val => %d", val);
	LOG("CShutdownMonitor::ConstructL()	 iShutdownProp.Get(val)		err => %d", err);
	User::LeaveIfError(err);
	__ASSERT_ALWAYS(val==0, PANIC);
	}

CShutdownMonitor::~CShutdownMonitor()
	{
	iShutdownProp.Close();
	}

void CShutdownMonitor::RunL()
	{
	// Request to shut everything down made in USB Aware App
	TInt val;
	TInt err = iShutdownProp.Get(val);
	LOG("CShutdownMonitor::RunL	 iShutdownProp.Get(val) err => %d", err);
	LOG("CShutdownMonitor::RunL	 iShutdownProp.Get(val) val => %d", val);
	Cancel(); // Not interested in any more notifications
	iParentUsbViewer.Stop(); // Stopping Active Scheduler will results in the destructor getting called
	}

void CShutdownMonitor::DoCancel()
	{
	iShutdownProp.Cancel();
	}
	


CEventNotifier::CEventNotifier(TInt aPriority)
	: CActive(aPriority)
	{
	}

XUsbViewerEvent* CEventNotifier::NewViewerEventL()
	{
	XUsbViewerEvent* event = new(ELeave) XUsbViewerEvent;
	CleanupStack::PushL(event);
	User::LeaveIfError(event->iEvent.Create(KViewerNumCharactersOnLine));
	CleanupStack::Pop();
	return event;
	}

void CEventNotifier::RunL()
	{
	DoRunL(NewViewerEventL());
	}




CServiceStateWatcher* CServiceStateWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CServiceStateWatcher* self = new(ELeave) CServiceStateWatcher(aUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CServiceStateWatcher::~CServiceStateWatcher()
	{
	Cancel();
	}

CServiceStateWatcher::CServiceStateWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
	, iServiceState(EUsbServiceIdle)
	{
	CActiveScheduler::Add(this);
	}

void CServiceStateWatcher::ConstructL()
	{
	iUsbViewer.Usb().ServiceStateNotification(iServiceState, iStatus);
	SetActive();

	TUsbServiceState serviceState;
	User::LeaveIfError(iUsbViewer.Usb().GetServiceState(serviceState));
	iUsbViewer.SetServiceState(serviceState);
	}

void CServiceStateWatcher::DoCancel()
	{
	iUsbViewer.Usb().ServiceStateNotificationCancel();
	}

void CServiceStateWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	iUsbViewer.SetServiceState(iServiceState);
	TPtrC res(NULL, 0);
	_LIT(KIdle,     "Idle");
	_LIT(KStarting, "Starting");
	_LIT(KStarted,  "Started");
	_LIT(KStopping, "Stopping");
	_LIT(KError,    "Error");
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
		
	iUsbViewer.Usb().ServiceStateNotification(iServiceState, iStatus);
	SetActive();
//                                  16              8 1 -> 25
	aEvent->iEvent.AppendFormat(_L("D:ServiceState [%S]"), &res);
	iUsbViewer.NotifyEvent(aEvent);

	// RunL call may have been delayed: ensure viewer has the very latest information
	TUsbServiceState serviceState;
	User::LeaveIfError(iUsbViewer.Usb().GetServiceState(serviceState));

	iUsbViewer.SetServiceState(serviceState);
	}

	
CDeviceStateWatcher* CDeviceStateWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CDeviceStateWatcher* self = new(ELeave) CDeviceStateWatcher(aUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDeviceStateWatcher::~CDeviceStateWatcher()
	{
	Cancel();
	}

CDeviceStateWatcher::CDeviceStateWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
	{
	CActiveScheduler::Add(this);
	}

void CDeviceStateWatcher::ConstructL()
	{
	iUsbViewer.Usb().DeviceStateNotification(0xffffffff, iDeviceState, iStatus);
	SetActive();

	TUsbDeviceState deviceState;
	User::LeaveIfError(iUsbViewer.Usb().GetDeviceState(deviceState));
	iUsbViewer.SetDeviceState(deviceState);
	}

void CDeviceStateWatcher::DoCancel()
	{
	iUsbViewer.Usb().DeviceStateNotificationCancel();
	}

void CDeviceStateWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	iUsbViewer.SetDeviceState(iDeviceState);
	TPtrC res(NULL, 0);
	_LIT(KUndefined,  "Undefined");
    _LIT(KDefault,    "Default");
    _LIT(KAttached,   "Attached");
    _LIT(KPowered,    "Powered");
    _LIT(KConfigured, "Configured");
    _LIT(KAddress,    "Address");
    _LIT(KSuspended,  "Suspended");
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

    iUsbViewer.Usb().DeviceStateNotification(0xffffffff, iDeviceState, iStatus);
	SetActive();
//                                  15            10 1 -> 26
	aEvent->iEvent.AppendFormat(_L("D:DeviceState [%S]"), &res);
	iUsbViewer.NotifyEvent(aEvent);

	// RunL call may have been delayed: ensure viewer has the very latest information
	TUsbDeviceState deviceState;
	User::LeaveIfError(iUsbViewer.Usb().GetDeviceState(deviceState));

	iUsbViewer.SetDeviceState(deviceState);
	}
	
	
CIdPinWatcher* CIdPinWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CIdPinWatcher* self = new(ELeave) CIdPinWatcher(aUsbViewer);
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

CIdPinWatcher::CIdPinWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
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
	iUsbViewer.SetIdPin(val);
	}

void CIdPinWatcher::DoCancel()
	{
	iIdPinProp.Cancel();
	}

void CIdPinWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iIdPinProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iIdPinProp.Get(val));

	TPtrC res(NULL, 0);
	_LIT(KIdPinInserted, "Inserted");
	_LIT(KIdPinRemoved,  "Removed");
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
//                                  9        8 1 -> 18
	aEvent->iEvent.AppendFormat(_L("O:IdPin [%S]"), &res);
	iUsbViewer.SetIdPin(val);
	iUsbViewer.NotifyEvent(aEvent);
	}
	
	
CVBusWatcher* CVBusWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CVBusWatcher* self = new(ELeave) CVBusWatcher(aUsbViewer);
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

CVBusWatcher::CVBusWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
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
	iUsbViewer.SetVBus(val);
	}

void CVBusWatcher::DoCancel()
	{
	iVBusProp.Cancel();
	}

void CVBusWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iVBusProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iVBusProp.Get(val));

	TPtrC res(NULL, 0);
	_LIT(KVBusRaised,  "Raised");
	_LIT(KVBusDropped, "Dropped");
	switch(val)
		{
	case 0:
		res.Set(KVBusDropped);
		break;

	case 1:
		res.Set(KVBusRaised);
		break;

	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();
//									8       7 1 -> 16
	aEvent->iEvent.AppendFormat(_L("O:VBus [%S]"), &res);
	iUsbViewer.SetVBus(val);
	iUsbViewer.NotifyEvent(aEvent);
	}


CConnectionIdleWatcher* CConnectionIdleWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CConnectionIdleWatcher* self = new(ELeave) CConnectionIdleWatcher(aUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CConnectionIdleWatcher::~CConnectionIdleWatcher()
	{
	Cancel();
	iConnIdleProp.Close();
	}

CConnectionIdleWatcher::CConnectionIdleWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
	{
	CActiveScheduler::Add(this);
	}

void CConnectionIdleWatcher::ConstructL()
	{
	User::LeaveIfError(iConnIdleProp.Attach(KUidUsbManCategory, KUsbOtgConnectionIdleProperty));
	iConnIdleProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	TInt err = iConnIdleProp.Get(val);
	LOG("CConnectionIdleWatcher::ConstructL iConnIdleProp.Get(val) => %d",err);
	User::LeaveIfError(err);
	iUsbViewer.SetConnectionIdle(val);
	}

void CConnectionIdleWatcher::DoCancel()
	{
	iConnIdleProp.Cancel();
	}

void CConnectionIdleWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	iConnIdleProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iConnIdleProp.Get(val));

	TPtrC res(NULL, 0);
	_LIT(KConnIdle,		"Idle");
	_LIT(KConnActive,	"Active");
	switch(val)
		{
	case 0:
		res.Set(KConnActive);
		break;

	case 1:
		res.Set(KConnIdle);
		break;

	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();
//                                  12          6 1 -> 19
	aEvent->iEvent.AppendFormat(_L("O:ConnIdle [%S]"), &res);
	iUsbViewer.SetConnectionIdle(val);
	iUsbViewer.NotifyEvent(aEvent);
	}

	
COtgStateWatcher* COtgStateWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	COtgStateWatcher* self = new(ELeave) COtgStateWatcher(aUsbViewer);
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

COtgStateWatcher::COtgStateWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
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
	iUsbViewer.SetOtgState(val);
	}

void COtgStateWatcher::DoCancel()
	{
	iOtgStateProp.Cancel();
	}

void COtgStateWatcher::DoRunL(XUsbViewerEvent* aEvent)
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

//                                  12         12 1 -> 25
	aEvent->iEvent.AppendFormat(_L("O:OtgState [%S]"), &res);
	iUsbViewer.SetOtgState(val);
	iUsbViewer.NotifyEvent(aEvent);
	}

CHostEventWatcher* CHostEventWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CHostEventWatcher* self = new(ELeave) CHostEventWatcher(aUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHostEventWatcher::~CHostEventWatcher()
	{
	Cancel();
	}

CHostEventWatcher::CHostEventWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
	{
	CActiveScheduler::Add(this);
	}

void CHostEventWatcher::ConstructL()
	{
	iUsbViewer.Usb().HostEventNotification(iStatus, iDeviceInfo);
	SetActive();
	iUsbViewer.SetAttachedDevices(0);
	}

void CHostEventWatcher::DoCancel()
	{
	iUsbViewer.Usb().HostEventNotificationCancel();
	}

void CHostEventWatcher::DoRunL(XUsbViewerEvent* aEvent)
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
//										    5    8    6     4    1 4   1 -> 29
			aEvent->iEvent.AppendFormat(_L("H:At[%.08x] V,P[%.04x,%.04x]"), iDeviceInfo.iDeviceId, iDeviceInfo.iVid, iDeviceInfo.iPid);
			
			if (  (iDeviceInfo.iVid == 0x046D)
			    &&(iDeviceInfo.iPid == 0x0A02)
			   )
				{
				iUsbViewer.SetDeviceType(CUsbViewer::ELogitechHeadset);
				}
			else
				{
				iUsbViewer.SetDeviceType(CUsbViewer::EGenericDevice);
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
//                                          7      8    6     3   5    1  1 -> 31
			aEvent->iEvent.AppendFormat(_L("H:Load[%.08x] Err[%.3d] St[%1S]"), iDeviceInfo.iDeviceId, iDeviceInfo.iError, &res);
			}
		break;

	case EDeviceDetachment:
			{
//											10        8	   1 -> 19
			aEvent->iEvent.AppendFormat(_L("H:Detach [%.08x]"), iDeviceInfo.iDeviceId);
			TInt ix = iAttachedDevices.Find(iDeviceInfo.iDeviceId);
			if(ix == KErrNotFound)
				{
				// This is probably caused by starting a new instance of the test console.
				break;
				}
			iAttachedDevices.Remove(ix);
			iUsbViewer.SetDeviceType(CUsbViewer::ENoDevice);
			}
		break;
	default:
		PANIC;
		break;
		}
	CleanupStack::Pop();
	iUsbViewer.SetAttachedDevices(iAttachedDevices.Count());
	iUsbViewer.NotifyEvent(aEvent);
	iUsbViewer.Usb().HostEventNotification(iStatus, iDeviceInfo);
	SetActive();
	}
	
	
CMessageWatcher* CMessageWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CMessageWatcher* self = new(ELeave) CMessageWatcher(aUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMessageWatcher::~CMessageWatcher()
	{
	Cancel();
	}

CMessageWatcher::CMessageWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
	{
	CActiveScheduler::Add(this);
	}

void CMessageWatcher::ConstructL()
	{
	iUsbViewer.Usb().MessageNotification(iStatus, iMessage);
	SetActive();
	}

void CMessageWatcher::DoCancel()
	{
	iUsbViewer.Usb().MessageNotificationCancel();
	}

void CMessageWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	TInt err = iStatus.Int();
	
	if (err)
		{
		aEvent->iEvent.AppendFormat(_L("O:Err! [%d] [%d]"), iMessage, err); // 32 chars max
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
			case KErrUsbOtgMessageQueueOverflow:		text.Set(KMessOtgdiMessageQueueOverflow)	;	break;
			case KErrUsbOtgBadState:					text.Set(KMessOtgdiBadState);					break;		
			case KErrUsbOtgStackNotStarted:				text.Set(KMessOtgdiStackNotStarted);			break;	
			case KErrUsbOtgVbusAlreadyRaised:	       	text.Set(KMessOtgdiVbusAlreadyRaised);			break;
			case KErrUsbOtgSrpForbidden:				text.Set(KMessOtgdiSrpForbidden);				break;	
			case KErrUsbOtgBusControlProblem:	       	text.Set(KMessOtgdiBusControlProblem);			break;
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
			} // switch(iMessage)																							

		// The message number can be positive (USBMAN) or negative (OTGDI/USBDI)
		
		LOG("CMessageWatcher::DoRunL iMessage = [%d]",iMessage);
		LOG("CMessageWatcher::DoRunL text     = [%S]",&text);

		if ( iMessage > 0 )
			{
			//								4   1 4   3   19  1 -> 32
			aEvent->iEvent.AppendFormat(_L("Msg[+%4.4d] [%.19S]"), iMessage, &text); // 32 chars max
			}
		else
			{
			//								4   1 4   3   19  1 -> 32
			aEvent->iEvent.AppendFormat(_L("Msg[-%4.4d] [%.19S]"), -iMessage, &text); // 32 chars max
			}
		} // else

	iUsbViewer.Usb().MessageNotification(iStatus, iMessage);
	SetActive();
	iUsbViewer.NotifyEvent(aEvent);
	}




CUserMsgQWatcher* CUserMsgQWatcher::NewL(CUsbViewer& aUsbViewer)
	{
	CUserMsgQWatcher* self = new(ELeave) CUserMsgQWatcher(aUsbViewer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUserMsgQWatcher::~CUserMsgQWatcher()
	{
	Cancel();
	}

CUserMsgQWatcher::CUserMsgQWatcher(CUsbViewer& aUsbViewer)
	: CEventNotifier(EPriorityStandard)
	, iUsbViewer(aUsbViewer)
	{
	CActiveScheduler::Add(this);
	}

void CUserMsgQWatcher::ConstructL()
	{
	iUsbViewer.UserMsgQ().NotifyDataAvailable(iStatus);
	SetActive();
	}

void CUserMsgQWatcher::DoCancel()
	{
	iUsbViewer.UserMsgQ().CancelDataAvailable();
	}

void CUserMsgQWatcher::DoRunL(XUsbViewerEvent* aEvent)
	{
	CleanupStack::PushL(aEvent);

	User::LeaveIfError(iUsbViewer.UserMsgQ().Receive(iUserMsgLine));
	LOG("CUserMsgQWatcher::DoRunL		iUsbViewer.UserMsgQ().Receive => %S", &iUserMsgLine);
	CleanupStack::Pop();

	aEvent->iEvent.AppendFormat(_L("%S"), &iUserMsgLine);
	iUsbViewer.NotifyEvent(aEvent);

	iUsbViewer.UserMsgQ().NotifyDataAvailable(iStatus); // Re-subscribe
	SetActive();
	}

