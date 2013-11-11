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

#include "exampleusbcontrolapp.h"

#include <e32cons.h>
#include <e32debug.h>
// For Message Watcher
#include <d32otgdi_errors.h>
#include <d32usbdi_errors.h>
#include <usb/usbshared.h>


#define LOG(A,B)	RDebug::Print(_L("UsbControlApp: " L##A),B)
#define PNT(A)		RDebug::Print(_L("UsbControlApp: " L##A))
#define BLN(A)		RDebug::Print(_L("" L##A))
#define PANIC		Panic(__LINE__)


#ifdef __USB_DEBUG__
#define DBG_PANIC	Panic(__LINE__)
#else
#define DBG_PANIC
#endif


void Panic(TInt aLine)
	{
	RDebug::Printf("UsbControlApp: PANIC line=%d", aLine);
	User::Panic(_L("USBCONTROLAPP"), aLine);
	}

void RunAppL()
	{
	CUsbControlAppEngine* engine = CUsbControlAppEngine::NewLC();
	engine->Start();
	CleanupStack::PopAndDestroy(engine);
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
		PNT("*** UsbControlApp E32Main ***\n");
		TRAP(err, RunAppL());
		}
	delete activeScheduler;
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}


CShutdownMonitor* CShutdownMonitor::NewL(MShutdownInterface& aControlAppEngine)
	{
	CShutdownMonitor* self = new(ELeave) CShutdownMonitor(aControlAppEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CShutdownMonitor::CShutdownMonitor(MShutdownInterface& aControlAppEngine)
	: CActive(EPriorityLow) // Low so all notifications that want to be serviced will be done first
	, iParentControlAppEngine(aControlAppEngine)
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
	iParentControlAppEngine.Stop(); // Stopping Active Scheduler will results in the destructor getting called
	}

void CShutdownMonitor::DoCancel()
	{
	iShutdownProp.Cancel();
	}


CUsbControlAppEngine* CUsbControlAppEngine::NewLC()
	{
	CUsbControlAppEngine* self = new(ELeave) CUsbControlAppEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	PNT("\nConstructed Control App\n");
	return self;
	}

CUsbControlAppEngine::~CUsbControlAppEngine()
	{
	PNT("\nClosing Control App\n");
	iViewerMsgQ.Close();
	delete iShutdownMonitor;
	delete iStateMachine;
	delete iMessageWatcher;
	delete iConnIdleWatcher;
	delete iVBusWatcher;
	delete iIdPinWatcher;
	iUsb.Close();
	}

CUsbControlAppEngine::CUsbControlAppEngine()
	{
	}

void CUsbControlAppEngine::ConstructL()
	{
	// Start session to USBMAN
	TInt err = iUsb.Connect();
	LOG("CUsbControlAppEngine::ConstructL()   iUsb.Connect()   err=%d", err);
	User::LeaveIfError(err);
	// Set this as the single controller of USBMAN
	err = iUsb.SetCtlSessionMode(ETrue);
	LOG("CUsbControlAppEngine::ConstructL()   iUsb.SetCtlSessionMode(ETrue)   err=%d", err);
	User::LeaveIfError(err);
	iStateMachine	= CControlAppStateMachine::NewL(*this);
	// Create Watcher Active Objects to monitor events
	iIdPinWatcher	= CIdPinWatcher::NewL(*this);
	iVBusWatcher	= CVBusWatcher::NewL(*this);
	iConnIdleWatcher= CConnectionIdleWatcher::NewL(*this);
	iMessageWatcher	= CMessageWatcher::NewL(*this);

	iShutdownMonitor = CShutdownMonitor::NewL(*this);
	// Create message queue between exampleusbcontrolapp.exe & usbviewer.exe for showing user messages
	err = iViewerMsgQ.CreateGlobal(KControlAppViewerMsgQName, KControlAppViewerMsgQSlots);
	LOG("CUsbControlAppEngine::ConstructL()	 iViewerMsgQ.CreateGlobal		err => %d", err);
	User::LeaveIfError(err);
	// Stop USB services if they are started so system can start from a known state
	err = StopUsbServices();
	LOG("CUsbControlAppEngine::ConstructL()   StopUsbServices()		err=%d", err);
	User::LeaveIfError(err);
	// Last thing - allow State Machine to find a main state by sending it a StartUp event
	iStateMachine->StartUp();
	}

void CUsbControlAppEngine::Start()
	{
	CActiveScheduler::Start(); // Get everything running
	}
	
void CUsbControlAppEngine::Stop() const
	{
	CActiveScheduler::Stop();
	}

// Called by State Machine
TInt CUsbControlAppEngine::StopUsbServices()
	{
	TRequestStatus status;
	iUsb.TryStop(status);
	User::WaitForRequest(status);
	LOG("CUsbControlAppEngine::StopUsbServices()   status.Int()=%d", status.Int());
	return status.Int();
	}

TInt CUsbControlAppEngine::StartUsbServices()
	{
	const TInt KACMPersonality = 1;
	TRequestStatus status;
	iUsb.TryStart(KACMPersonality, status);
	User::WaitForRequest(status);
	LOG("CUsbControlAppEngine::StartUsbServices()   status.Int()=%d", status.Int());
	return status.Int();
	}

TInt CUsbControlAppEngine::EnableFunctionDriverLoading()
	{
	TInt err = iUsb.EnableFunctionDriverLoading();
	LOG("CUsbControlAppEngine::EnableFunctionDriverLoading()   err=%d", err);
	return err;
	}

void CUsbControlAppEngine::DisableFunctionDriverLoading()
	{
	PNT("CUsbControlAppEngine::DisableFunctionDriverLoading()\n");
	iUsb.DisableFunctionDriverLoading();
	}

TInt CUsbControlAppEngine::BusRequest()
	{
	TInt err = iUsb.BusRequest();
	LOG("CUsbControlAppEngine::BusRequest()   err=%d", err);
	return err;
	}

TInt CUsbControlAppEngine::BusDrop()
	{
	TInt err = iUsb.BusDrop();
	LOG("CUsbControlAppEngine::BusDrop()   err=%d", err);
	return err;
	}

TInt CUsbControlAppEngine::BusRespondSrp()
	{
	TInt err = iUsb.BusRespondSrp();
	LOG("CUsbControlAppEngine::BusRespondSrp()   err=%d", err);
	return err;
	}

TInt CUsbControlAppEngine::ClearVBusError()
	{
	TInt err = iUsb.BusClearError();
	LOG("CUsbControlAppEngine::ClearVBusError()   err=%d", err);
	return err;
	}

// Must divide the user message to be displayed into segments that will fit on the usbviewer.exe window.
// Because the viewer.exe displays Events in a downward scrolling list with the latest entry at the top,
// the user message segments must be sent in reverse order.
void CUsbControlAppEngine::DisplayUserMessage(const TDesC& aUserMsg)
	{
	LOG("CUsbControlAppEngine::DisplayUserMessage(\"%S\")\n", &aUserMsg);
	TInt msgLength = aUserMsg.Length();
	TInt lastBitLength = msgLength % KEventLineMsgNumCharacters;
	LOG("CUsbControlAppEngine::DisplayUserMessage	 lastBitLength => %d", lastBitLength);
	if (lastBitLength) // show the 'last bit' segment of the message which is not divisible by KEventLineMsgNumCharacters
		{
		TBuf<KEventLineNumCharacters> buf(KPrefix);
		TPtrC text = aUserMsg.Right(lastBitLength);
		buf.Append(text);
		LOG("CUsbControlAppEngine::DisplayUserMessage	 iViewerMsgQ.Send(buf) buf => %S", &buf);
		TInt err = iViewerMsgQ.Send(buf);
		LOG("CUsbControlAppEngine::DisplayUserMessage	 iViewerMsgQ.Send(buf) err => %d", err);
		}
	msgLength -= lastBitLength;
	while (msgLength > 0) // show the segments of the message which are exactly divisible by KEventLineMsgNumCharacters
		{
		// TBuf of length KEventLineNumCharacters = KPrefix of length 2 + Mid bit of length KEventLineMsgNumCharacters
		TBuf<KEventLineNumCharacters> buf(KPrefix);
		TPtrC text = aUserMsg.Mid(msgLength-KEventLineMsgNumCharacters, KEventLineMsgNumCharacters);
		buf.Append(text);
		LOG("CUsbControlAppEngine::DisplayUserMessage	 iViewerMsgQ.Send(buf) buf => %S", &buf);
		TInt err = iViewerMsgQ.Send(buf);
		LOG("CUsbControlAppEngine::DisplayUserMessage	 iViewerMsgQ.Send(buf) err => %d", err);		
		msgLength -= KEventLineMsgNumCharacters;
		}
	}

// Called by Watchers. Will forward as event to State Machine.
void CUsbControlAppEngine::SetIdPin(TInt aIdPin)
	{
	PNT("CUsbControlAppEngine::SetIdPin()\n");
	iIdPin = aIdPin;
	__ASSERT_ALWAYS(iStateMachine, PANIC);
	aIdPin ? iStateMachine->IdPinPresent() : iStateMachine->IdPinAbsent();
	}

void CUsbControlAppEngine::SetVBus(TInt aVBus)
	{
	PNT("CUsbControlAppEngine::SetVBus()\n");
	iVBus = aVBus;
	__ASSERT_ALWAYS(iStateMachine, PANIC);
	aVBus ? iStateMachine->VBusRise() : iStateMachine->VBusDrop();
	}

void CUsbControlAppEngine::SetConnectionIdle(TInt aConnIdle)
	{
	PNT("CUsbControlAppEngine::SetConnectionIdle()\n");
	iConnIdle = aConnIdle;
	__ASSERT_ALWAYS(iStateMachine, PANIC);
	aConnIdle ? iStateMachine->ConnectionIdle() : iStateMachine->ConnectionActive();
	}

void CUsbControlAppEngine::MessageReceived(CMessageWatcher::TMessageWatcherNotifications aMessageNotification)
	{
	switch(aMessageNotification)
		{
	case CMessageWatcher::EUsbMessageRequestSession :
		PNT("CUsbControlAppEngine::MessageReceived()	EUsbMessageRequestSession\n");
		iStateMachine->RequestSessionCalled();
		break;
	case CMessageWatcher::EUsbMessageSrpReceived :
		PNT("CUsbControlAppEngine::MessageReceived()	EUsbMessageSrpReceived\n");
		iStateMachine->SrpDetected();
		break;
	case CMessageWatcher::EErrUsbOtgSrpTimeout :
		PNT("CUsbControlAppEngine::MessageReceived()	EErrUsbOtgSrpTimeout\n");
		iStateMachine->SrpTimeout();
		break;
	case CMessageWatcher::EErrUsbOtgVbusError :
		PNT("CUsbControlAppEngine::MessageReceived()	EErrUsbOtgVbusError\n");
		iStateMachine->VBusError();
		break;
	default:
		PANIC;
		}
	}

RUsb& CUsbControlAppEngine::Usb()
	{
	return iUsb;
	}

TInt CUsbControlAppEngine::GetIdPin()
	{
	return iIdPin;
	}

TInt CUsbControlAppEngine::GetVBus()
	{
	return iVBus;
	}


CControlAppStateMachine* CControlAppStateMachine::NewL(MControlAppEngineInterface& aControlAppEngine)
	{
	CControlAppStateMachine* self = new(ELeave) CControlAppStateMachine(aControlAppEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	PNT("CControlAppStateMachine::NewL\n");
	CleanupStack::Pop(self);
	return self;
	}

CControlAppStateMachine::~CControlAppStateMachine()
	{
	delete iStateAServicesStopped;
	delete iStateAServicesStarted;
	delete iStateBServicesStarted;
	delete iStateBServicesStopped;
	delete iStateInitial;
	delete iInactivityTimer;
	}

CControlAppStateMachine::CControlAppStateMachine(MControlAppEngineInterface& aControlAppEngine)
	: iParentControlAppEngine(aControlAppEngine)
	, iTriggerSrp(EFalse)
	{
	}

void CControlAppStateMachine::ConstructL()
	{
	iInactivityTimer		= CInactivityTimer::NewL(*this);
	iStateInitial			= new(ELeave) CControlAppStateInitial(*this);
	iStateBServicesStopped	= new(ELeave) CControlAppStateBServicesStopped(*this);
	iStateBServicesStarted	= new(ELeave) CControlAppStateBServicesStarted(*this);
	iStateAServicesStarted	= new(ELeave) CControlAppStateAServicesStarted(*this);
	iStateAServicesStopped	= new(ELeave) CControlAppStateAServicesStopped(*this);
	SetState(EStateInitial);
	}

void CControlAppStateMachine::SetState(TControlAppState aState)
	{
	TPtrC StateString(NULL, 0);
	//               123456789
	_LIT(KInitial,	" Initial ");
	_LIT(KBStopped, "B-Stopped");
	_LIT(KBStarted, "B-Started");
	_LIT(KAStopped, "A-Stopped");
	_LIT(KAStarted, "A-Started");
	switch(aState)
		{
	case EStateInitial :			
		StateString.Set(KInitial);		
		iCurrentState = iStateInitial;		
		break;
	case EStateBServicesStopped :	
		StateString.Set(KBStopped);		
		iCurrentState = iStateBServicesStopped;		
		break;
	case EStateBServicesStarted :	
		StateString.Set(KBStarted);		
		iCurrentState = iStateBServicesStarted;		
		break;
	case EStateAServicesStopped :	
		StateString.Set(KAStopped);		
		iCurrentState = iStateAServicesStopped;		
		break;
	case EStateAServicesStarted :	
		StateString.Set(KAStarted);		
		iCurrentState = iStateAServicesStarted;	
		break;
	default :	PANIC;
		}
	//                                         (123456789)  ***
	BLN("");
	PNT("******************************************************");
	LOG("***  CControlAppStateMachine::SetState(%S)  ***", &StateString);
	PNT("******************************************************");
	BLN("");
	}

// State Machine's publicly visible interface...
//		Initial
void CControlAppStateMachine::StartUp()
	{
	PNT("CControlAppStateMachine::StartUp() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->StartUp();
	}

//		B-Stopped, B-Started
void CControlAppStateMachine::IdPinPresent()
	{
	PNT("CControlAppStateMachine::IdPinPresent() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->IdPinPresent(); 
	}
	
//		B-Stopped, A-Started
void CControlAppStateMachine::VBusRise()
	{
	PNT("CControlAppStateMachine::VBusRise() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state		
	iCurrentState->VBusRise();
	}

//		B-Started, A-Started
void CControlAppStateMachine::VBusDrop()
	{
	PNT("CControlAppStateMachine::VBusDrop() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->VBusDrop();
	}

//		A-Started, A-Stopped
void CControlAppStateMachine::IdPinAbsent()
	{
	PNT("CControlAppStateMachine::IdPinAbsent() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->IdPinAbsent();
	}

//		A-Started
void CControlAppStateMachine::ConnectionIdle()
	{
	PNT("CControlAppStateMachine::ConnectionIdle() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->ConnectionIdle();
	}

//		A-Started
void CControlAppStateMachine::ConnectionActive()
	{
	PNT("CControlAppStateMachine::ConnectionActive() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->ConnectionActive();
	}

//		B-Stopped, A-Stopped, B-Started, A-Started
void CControlAppStateMachine::RequestSessionCalled()
	{
	PNT("CControlAppStateMachine::RequestSessionCalled() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->RequestSessionCalled();
	}

//		B-Started
void CControlAppStateMachine::SrpTimeout()
	{
	PNT("CControlAppStateMachine::SrpTimeout() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->SrpTimeout();
	}

//		A-Stopped
void CControlAppStateMachine::SrpDetected()
	{
	PNT("CControlAppStateMachine::SrpDetected() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->SrpDetected();
	}

//		A-Started, A-Stopped
void CControlAppStateMachine::VBusError()
	{
	PNT("CControlAppStateMachine::VBusError() - event will be forwarded to current state\n");
	__ASSERT_ALWAYS(iCurrentState, PANIC);
	// Could implement global USB enabled/disabled policy here by allowing/preventing event reaching current state
	iCurrentState->VBusError();
	}

void CControlAppStateMachine::ResetInactivityTimer()
	{
	PNT("CControlAppStateMachine::ResetInactivityTimer()\n");
	iInactivityTimer->Reset();
	}

void CControlAppStateMachine::CancelInactivityTimer()
	{
	PNT("CControlAppStateMachine::CancelInactivityTimer()\n");
	iInactivityTimer->Cancel();
	}

void CControlAppStateMachine::InactivityTimerExpired()
	{
	PNT("CControlAppStateMachine::InactivityTimerExpired()\n");
	iCurrentState->InactivityTimerExpired();
	}


CControlAppStateMachine::CInactivityTimer* CControlAppStateMachine::CInactivityTimer::NewL(CControlAppStateMachine& aParentStateMachine)
	{
	PNT("CControlAppStateMachine::CInactivityTimer::NewL()\n");
	CControlAppStateMachine::CInactivityTimer* self = new(ELeave) CControlAppStateMachine::CInactivityTimer(aParentStateMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CControlAppStateMachine::CInactivityTimer::~CInactivityTimer()
	{
	PNT("CControlAppStateMachine::CInactivityTimer::~CInactivityTimer()\n");
	Cancel();
	iTimer.Close();
	}

CControlAppStateMachine::CInactivityTimer::CInactivityTimer(CControlAppStateMachine& aParentStateMachine)
	: CActive(EPriorityStandard)
	, iParentStateMachine(aParentStateMachine)
	{
	CActiveScheduler::Add(this);
	}

void CControlAppStateMachine::CInactivityTimer::ConstructL()
	{
	PNT("CControlAppStateMachine::CInactivityTimer::ConstructL\n");
	TInt err = iTimer.CreateLocal();
	LOG("CInactivityTimer::ConstructL	iTimer.CreateLocal() => %d",err);
	User::LeaveIfError(err);
	}

void CControlAppStateMachine::CInactivityTimer::Reset()
	{
	PNT("CControlAppStateMachine::CInactivityTimer::Reset()\n");
	if (IsActive())
		{
		PNT("CControlAppStateMachine::CInactivityTimer::Reset() - IsActive() so Cancel() before setting RTimer\n");
		Cancel(); // Reset
		}
	iTimer.After(iStatus, KInactivityTimerPeriod); // 10 sec
	SetActive();
	}
	
void CControlAppStateMachine::CInactivityTimer::RunL()
	{
	PNT("CControlAppStateMachine::CInactivityTimer::RunL() - calling iParentState.InactivityTimerExpired()\n");
	iParentStateMachine.InactivityTimerExpired();
	}

void CControlAppStateMachine::CInactivityTimer::DoCancel()
	{
	PNT("CControlAppStateMachine::CInactivityTimer::DoCancel()\n");
	iTimer.Cancel();
	}


// Base state...
CControlAppStateMachine::CControlAppStateBase::CControlAppStateBase(CControlAppStateMachine& aParentStateMachine)
	: iParentStateMachine(aParentStateMachine)
	{
	}

void CControlAppStateMachine::CControlAppStateBase::StartUp()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::StartUp() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::IdPinPresent()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::IdPinPresent() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}
	
void CControlAppStateMachine::CControlAppStateBase::VBusRise()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::VBusRise() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::VBusDrop()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::VBusDrop() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::IdPinAbsent()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::IdPinAbsent() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::ConnectionIdle()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::ConnectionIdle() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::ConnectionActive()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::ConnectionActive() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::InactivityTimerExpired()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::InactivityTimerExpired() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::RequestSessionCalled()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::RequestSessionCalled() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::SrpTriggered()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::SrpTriggered() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::SrpTimeout()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::SrpTimeout() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::SrpDetected()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::SrpDetected() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}

void CControlAppStateMachine::CControlAppStateBase::VBusError()
	{
	PNT("CControlAppStateMachine::CControlAppStateBase::VBusError() - ERROR: unexpected event in current state\n");
	DBG_PANIC;
	}


// Initial state...
CControlAppStateMachine::CControlAppStateInitial::CControlAppStateInitial(CControlAppStateMachine& aParentStateMachine)
	: CControlAppStateBase(aParentStateMachine)
	{
	}

void CControlAppStateMachine::CControlAppStateInitial::VBusRise()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::VBusRise() - no action: wait for StartUp event\n");
	}

void CControlAppStateMachine::CControlAppStateInitial::VBusDrop()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::VBusDrop() - no action: wait for StartUp event\n");
	}

void CControlAppStateMachine::CControlAppStateInitial::IdPinPresent()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::IdPinPresent() - no action: wait for StartUp event\n");
	}

void CControlAppStateMachine::CControlAppStateInitial::IdPinAbsent()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::IdPinAbsent() - no action: wait for StartUp event\n");
	}

void CControlAppStateMachine::CControlAppStateInitial::ConnectionIdle()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::ConnectionIdle() - no action: wait for StartUp event\n");
	}

void CControlAppStateMachine::CControlAppStateInitial::ConnectionActive()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::ConnectionActive() - no action: wait for StartUp event\n");
	}

void CControlAppStateMachine::CControlAppStateInitial::StartUp()
	{
	PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()\n");
	// Determine state to move to from the Initial
	if (iParentStateMachine.iParentControlAppEngine.GetIdPin())
		{
		PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Id Pin detected\n");
		TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
		if (!err) // Started USB services
			{
			PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Started USB services\n");
			iParentStateMachine.SetState(EStateAServicesStarted);
			err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
			if (!err) // Started USB services & enabled FD loading
				{
				PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Enabled FD loading\n");
				}
			else // Started USB services & couldn't enable FD loading
				{
				PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Couldn't enable FD loading\n");
				_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
				iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
				}
			// Regardless of whether enabling FD loading succeeded or not do BusRequest() to power VBus
			err = iParentStateMachine.iParentControlAppEngine.BusRequest(); 
			if (!err)
				{
				PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	BusRequest() successful\n");
				}
			else
				{
				PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	BusRequest() failed\n");
				// This error only indicates whether USBMAN server got the request without any problem.
				// VBus could still fail to go up but this is only likely if there is a Bus Error - this is 
				// handled in a separate event though.
				_LIT(KMsgCouldntPowerUsbDevice,	"Error: Couldn't power USB device - press 'R' to try again");
				iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntPowerUsbDevice);
				// CControlAppStateAServicesStarted::VBusRise() ensures Inactivity Timer is activated when the BusRequest()
				// is successful, but when it fails we must ensure USB services will be eventually stopped:
				iParentStateMachine.ResetInactivityTimer();
				}
			}
		else // Couldn't start USB services
			{
			PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Couldn't start USB services\n");
			_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
			iParentStateMachine.SetState(EStateAServicesStopped); // Since Id Pin is present
			}
		} // if (iParentStateMachine.iParentControlAppEngine.GetIdPin())
	else // i.e. no Id Pin
		{
		if (iParentStateMachine.iParentControlAppEngine.GetVBus())
			{
			PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Id Pin Absent & VBus detected\n");
			TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
			if (!err) // Started USB services
				{
				PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Started USB services\n");
				iParentStateMachine.SetState(EStateBServicesStarted);
				err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
				if (!err) // Started USB services & enabled FD loading
					{
					PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Enabled FD loading\n");
					}
				else // Started USB services & couldn't enable FD loading
					{
					PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	couldn't enable FD loading\n");
					_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
					iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
					}
				}
			else // Couldn't start USB services
				{
				PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Couldn't start USB services\n");
				iParentStateMachine.SetState(EStateBServicesStopped); // even though VBus
				_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
				iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
				}
			} // if (iParentStateMachine.iParentControlAppEngine.GetVBus())
		else  // if (iParentStateMachine.iParentControlAppEngine.GetVBus())
			{
			PNT("CControlAppStateMachine::CControlAppStateInitial::StartUp()	Id Pin Absent & VBus not detected\n");
			iParentStateMachine.SetState(EStateBServicesStopped);
			}
		} // else i.e. no Id Pin
	}


// BServicesStopped state...
CControlAppStateMachine::CControlAppStateBServicesStopped::CControlAppStateBServicesStopped(CControlAppStateMachine& aParentStateMachine)
	: CControlAppStateBase(aParentStateMachine)
	{
	}

// Power detected on VBus
// Try to move to B-Started
void CControlAppStateMachine::CControlAppStateBServicesStopped::VBusRise()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::VBusRise()	VBus detected\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
	if (!err) // Started USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::VBusRise()	Started USB services\n");
		iParentStateMachine.SetState(EStateBServicesStarted);
		err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
		if (!err) // Started USB services & enabled FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::VBusRise()	Enabled FD loading\n");
			}
		else // Started USB services & couldn't enable FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::VBusRise()	couldn't enable FD loading\n");
			_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
			}
		}
	else // Couldn't start USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::VBusRise()	Couldn't start USB services\n");
		_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
		}
	}

// Id Pin detected
// Try to move to A-Started
void CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
	if (!err) // Started USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()	Started USB services\n");
		iParentStateMachine.SetState(EStateAServicesStarted);
		err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
		if (!err) // Started USB services & enabled FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()	Enabled FD loading\n");
			}
		else // Started USB services & couldn't enable FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()	Couldn't enable FD loading\n");
			_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
			}
		// Regardless of whether enabling FD loading succeeded or not, attempt to power VBus
		err = iParentStateMachine.iParentControlAppEngine.BusRequest(); 
		if (!err)
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()	BusRequest() successful\n");
			}
		else
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()	BusRequest() failed\n");
			// This error only indicates whether USBMAN server got the request without any problem.
			// VBus could still fail to go up but this is only likely if there is a Bus Error - this is 
			// handled in a separate event though.
			_LIT(KMsgCouldntPowerUsbDevice,	"Error: Couldn't power USB device - press 'R' to try again");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntPowerUsbDevice);
			// CControlAppStateAServicesStarted::VBusRise() ensures Inactivity Timer is activated when the BusRequest()
			// is successful, but when it fails we must ensure USB services will be eventually stopped:
			iParentStateMachine.ResetInactivityTimer();
			}
		}
	else // Couldn't start USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinPresent()	Couldn't start USB services\n");
		iParentStateMachine.SetState(EStateAServicesStopped); // Since Id Pin is present
		_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
		}
	}

// RUsb::RequestSession() called from USB Aware App
// Try to move to B-Started and trigger SRP
void CControlAppStateMachine::CControlAppStateBServicesStopped::RequestSessionCalled()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::RequestSessionCalled()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
	if (!err) // Started USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::RequestSessionCalled()	Started USB services\n");
		iParentStateMachine.SetState(EStateBServicesStarted); // Do this before SrpTriggered()
		err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
		if (!err) // Started USB services & enabled FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::RequestSessionCalled()	Enabled FD loading\n");	
			iParentStateMachine.iCurrentState->SrpTriggered(); // Internally generated event in B-Started state
			}
		else // Started USB services & couldn't enable FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::RequestSessionCalled()	Couldn't enable FD loading\n");
			_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
			// No point doing SRP since wont be able to enumerate peripheral
			}
		}
	else // Couldn't start USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::RequestSessionCalled()	Couldn't start USB services\n");
		_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
		}
	}

void CControlAppStateMachine::CControlAppStateBServicesStopped::VBusDrop()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::VBusDrop()	no action: this A-Device dropped VBus before moving into this state\n");
	DBG_PANIC;
	}


// BServicesStarted state...
CControlAppStateMachine::CControlAppStateBServicesStarted::CControlAppStateBServicesStarted(CControlAppStateMachine& aParentStateMachine)
	: CControlAppStateBase(aParentStateMachine)
	{
	}

// VBus not being powered any more by remote device
// Try to stop USB services and move to B-Stopped
void CControlAppStateMachine::CControlAppStateBServicesStarted::VBusDrop()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::VBusDrop()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StopUsbServices();
	if (!err) // Stopped USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::VBusDrop()	Stopped USB services\n");
		iParentStateMachine.SetState(EStateBServicesStopped);
		}
	else // Couldn't stop USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::VBusDrop()	Couldn't stop USB services\n");
		// Stay in current state B-Started.
		// VBus probably dropped as a result of B-Plug being withdrawn (or remote A-Device powering down VBus).
		// If A-Plug inserted, will move to A-Started from this state. (If VBus is re-powered, in the correct state too).
		_LIT(KMsgCouldntStopUsb,	"Error: Couldn't stop USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStopUsb);
		}
	}

// We've come from B-Stopped to trigger an SRP
// If SRP times out (notification from USBMAN will be another separate event received) we'll return to B-Stopped.
// If SRP successful, VBus will be powered and we can stay in the current state.
void CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTriggered()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTriggered()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.BusRequest(); // SRP
	// If no err, we don't have to do anything
	if (!err)
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTriggered()	BusRequest for SRP successful\n");
		}
	else // Was problem doing SRP - won't get SRP timeout event - go back to B-Stopped now
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTriggered()	BusRequest for SRP failed\n");
		_LIT(KMsgFailedBecomingHost,	"Error: Failed to become Host");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgFailedBecomingHost);
		err = iParentStateMachine.iParentControlAppEngine.StopUsbServices();
		if (!err) // Cleaned up by stopping USB services
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTriggered()	Stopped USB services\n");
			iParentStateMachine.SetState(EStateBServicesStopped);
			}
		else
			{
			PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTriggered()	Couldn't stop USB services\n");
			_LIT(KMsgCouldntStopUsb,	"Error: Couldn't stop USB services");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStopUsb);
			// Stay in current state B-Started.
			// Can re-try SRP by doing RequestSession() -> BusRequest() in this state.
			// No event received if B-Plug is withdrawn.
			// If A-Plug inserted, will move to A-Started from this state.
			}
		}
	}

// Message notification from USBMAN that SRP requested earlier has timed out.
// Try to move to B-Stopped
void CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTimeout()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTimeout()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StopUsbServices();
	if (!err) // Stopped USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTimeout()	Stopped USB services\n");
		iParentStateMachine.SetState(EStateBServicesStopped);
		}
	else // Couldn't stop USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::SrpTimeout()	Couldn't stop USB services\n");
		_LIT(KMsgCouldntStopUsb,	"Error: Couldn't stop USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStopUsb);
		// Stay in current state B-Started. 
		// Can re-try SRP by doing RequestSession() -> BusRequest() in this state.
		// No event received if B-Plug is withdrawn.
		// If A-Plug inserted, will move to A-Started from this state.
		}
	}

// Will see when SRP successful
void CControlAppStateMachine::CControlAppStateBServicesStarted::VBusRise()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::VBusRise()\n");
	}

// Time taken to notice the drop in VBus by which time the Id Pin has been inserted.
// Cater for the IdPinPresent event in this state therefore.
// Try to power VBus and move to A-Started state
void CControlAppStateMachine::CControlAppStateBServicesStarted::IdPinPresent()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::IdPinPresent()\n");
	iParentStateMachine.SetState(EStateAServicesStarted); // Since Id Pin present and USB services still started.
	// USB services already started but may have failed to enable FD loading previously so try again (calling twice does no harm)
	TInt err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
	if (!err)
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::IdPinPresent()	Enabled FD loading\n");	
		}
	else 
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::IdPinPresent()	Couldn't enable FD loading\n");
		_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
		}
	err = iParentStateMachine.iParentControlAppEngine.BusRequest();
	if (!err) // Powered VBus
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::IdPinPresent()	Powered VBus\n");
		}
	else // Couldn't power VBus
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::IdPinPresent()	Couldn't power VBus\n");
		// RequestSession() in A-Started will have the effect of powering VBus
		_LIT(KMsgCouldntPowerUsbDevice,	"Error: Couldn't power USB device - press 'R' to try again");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntPowerUsbDevice);
		// CControlAppStateAServicesStarted::VBusRise() ensures Inactivity Timer is activated when the BusRequest()
		// is successful, but when it fails we must ensure USB services will be eventually stopped:
		iParentStateMachine.ResetInactivityTimer();
		}
	}

// Let underlying OTG deal with what this means in this state
void CControlAppStateMachine::CControlAppStateBServicesStarted::RequestSessionCalled()
	{
	PNT("CControlAppStateMachine::CControlAppStateBServicesStarted::RequestSessionCalled()\n");
	(void) iParentStateMachine.iParentControlAppEngine.BusRequest(); // Not interested in error code
	}


// AServicesStarted state...
CControlAppStateMachine::CControlAppStateAServicesStarted::CControlAppStateAServicesStarted(CControlAppStateMachine& aParentStateMachine)
	: CControlAppStateBase(aParentStateMachine)
	{
	}

// A-Plug has been removed
// Try to move to B-Stopped state
void CControlAppStateMachine::CControlAppStateAServicesStarted::IdPinAbsent()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::IdPinAbsent()\n");
	// BusDrop() not needed as VBus unpowered at a lower level if Id Pin not present
	TInt err = iParentStateMachine.iParentControlAppEngine.StopUsbServices();
	if (!err) // Stopped USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::IdPinAbsent()	Stopped USB services	(BusDrop unnecessary here)\n");
		iParentStateMachine.SetState(EStateBServicesStopped);
		}
	else // Couldn't stop USB services started & Id Pin absent - go to B-Started
		{
		PNT("CControlAppStateMachine::CControlAppStateBServicesStopped::IdPinAbsent()	Couldn't stop USB services\n");
		// Move to B-Started since no Id Pin but USB services still started.
		iParentStateMachine.SetState(EStateBServicesStarted);
		_LIT(KMsgCouldntStopUsb,	"Error: Couldn't stop USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStopUsb);
		}
	}

// Remote B-Device sent SRP
// It is possible for VBus to be down in A-Started state so SRP could arrive
void CControlAppStateMachine::CControlAppStateAServicesStarted::SrpDetected()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::SrpDetected()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.BusRespondSrp();
	if (!err)
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::SrpDetected()	BusRespondSrp() succeeded\n");
		}
	else
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::SrpDetected()	BusRespondSrp() error\n");
		_LIT(KMsgCouldntPowerUsbDevice,	"Error: Couldn't power USB device");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntPowerUsbDevice);
		}
	}

void CControlAppStateMachine::CControlAppStateAServicesStarted::VBusRise()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::VBusRise() - start Inactivity Timer...\n");
	iParentStateMachine.ResetInactivityTimer();
	}

void CControlAppStateMachine::CControlAppStateAServicesStarted::ConnectionIdle()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::ConnectionIdle()\n");
	iParentStateMachine.ResetInactivityTimer();
	}

void CControlAppStateMachine::CControlAppStateAServicesStarted::ConnectionActive()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::ConnectionActive()\n");
	iParentStateMachine.CancelInactivityTimer();
	}

void CControlAppStateMachine::CControlAppStateAServicesStarted::InactivityTimerExpired()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::InactivityTimerExpired()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.BusDrop();
	if (!err)
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::InactivityTimerExpired()	Dropped VBus\n");
		err = iParentStateMachine.iParentControlAppEngine.StopUsbServices();
		if (!err) // Stopped USB services
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::InactivityTimerExpired()	Stopped USB services\n");
			iParentStateMachine.SetState(EStateAServicesStopped);
			}
		else // Couldn't stop USB services started & Id Pin present - stay in A-Started
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::InactivityTimerExpired()	Couldn't stop USB services\n");
			_LIT(KMsgCouldntStopUsb,	"Error: Couldn't stop USB services");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStopUsb);
			}
		}
	else
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::InactivityTimerExpired()	Can't drop VBus\n");
		_LIT(KMsgCouldntDropVBus,	"Error: Couldn't drop VBus to save power. Unplug device.");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntDropVBus);
		}
	}

// Let underlying OTG deal with what this means in this state
void CControlAppStateMachine::CControlAppStateAServicesStarted::RequestSessionCalled()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::RequestSessionCalled()\n");
	(void) iParentStateMachine.iParentControlAppEngine.BusRequest(); // Not interested in error code
	}

void CControlAppStateMachine::CControlAppStateAServicesStarted::VBusError()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStarted::VBusError()\n");
	iParentStateMachine.CancelInactivityTimer(); // To prevent a BusDrop being done - VBus is already down.
	(void) iParentStateMachine.iParentControlAppEngine.ClearVBusError(); // Not interested in error code
	// A call to RequestSession() will be necessary to raise VBus again
	_LIT(KMsgVBusError,	"Error: Couldn't power USB device - please unplug");
	iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgVBusError);	
	}


// AServicesStopped state...
CControlAppStateMachine::CControlAppStateAServicesStopped::CControlAppStateAServicesStopped(CControlAppStateMachine& aParentStateMachine)
	: CControlAppStateBase(aParentStateMachine)
	{
	}

// A-Plug been removed
// Try to move to B-Stopped
void CControlAppStateMachine::CControlAppStateAServicesStopped::IdPinAbsent()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::IdPinAbsent()\n");
	iParentStateMachine.SetState(EStateBServicesStopped);
	}

// Remote B-Device sent SRP
// Try to move to A-Started
void CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
	if (!err) // Started USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()	Started USB services\n");
		iParentStateMachine.SetState(EStateAServicesStarted);
		// Need to enable FD loading for when the roles revert back to their defaults
		// for after the role swap that follows the BusRespondSRP()
		err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
		if (!err) // Started USB services & enabled FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()	Enabled FD loading\n");
			}
		else // Started USB services & couldn't enable FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()	Couldn't enable FD loading\n");
			_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
			}
		// Regardless of whether FD loading succeeded
		err = iParentStateMachine.iParentControlAppEngine.BusRespondSrp();
		if (!err)
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()	BusRespondSrp() succeeded\n");
			}
		else
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()	BusRespondSrp() error\n");
			// In A-Started, if no activity for a while (which will be the case if BusRespondSRP() failed = VBus unpowered)
			// Inactivity Timer will expire and send the system back to A-Stopped.
			_LIT(KMsgCouldntPowerUsbDevice,	"Error: Couldn't power USB device");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntPowerUsbDevice);
			}
		}
	else // Couldn't start USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::SrpDetected()	Couldn't start USB services\n");
		_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
		}
	}

// Local USB Aware App would like to use USB services
// Try to move to A-Started
void CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()\n");
	TInt err = iParentStateMachine.iParentControlAppEngine.StartUsbServices();
	if (!err) // Started USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()	Started USB services\n");
		iParentStateMachine.SetState(EStateAServicesStarted);
		// In A-Started, if no activity for a while, which will be the case if 
		// EnableFunctionDriverLoading() fails (= no host activity) or BusRequest() fails (= no VBus)
		// Inactivity Timer will expire soon and send the system back to A-Stopped.
		err = iParentStateMachine.iParentControlAppEngine.EnableFunctionDriverLoading();
		if (!err) // Started USB services & enabled FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()	Enabled FD loading\n");
			}
		else // Started USB services & couldn't enable FD loading
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()	Couldn't enable FD loading\n");
			_LIT(KMsgCouldntEnableFdLoading,	"Error: Couldn't enable driver loading - can't function as host");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntEnableFdLoading);
			}
		// Regardless of whether enabling FD loading succeeded
		err = iParentStateMachine.iParentControlAppEngine.BusRequest();
		if (!err)
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()	BusRequest() succeeded\n");
			}
		else
			{
			PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()	BusRequest() error\n");
			_LIT(KMsgCouldntPowerUsbDevice,	"Error: Couldn't power USB device");
			iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntPowerUsbDevice);
			// CControlAppStateAServicesStarted::VBusRise() ensures Inactivity Timer is activated when the BusRequest()
			// is successful, but when it fails we must ensure USB services will be eventually stopped:
			iParentStateMachine.ResetInactivityTimer();
			}
		}
	else // Couldn't start USB services
		{
		PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::RequestSessionCalled()	Couldn't start USB services\n");
		_LIT(KMsgCouldntStartUsb,	"Error: Couldn't start USB services");
		iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgCouldntStartUsb);
		}
	}

void CControlAppStateMachine::CControlAppStateAServicesStopped::VBusError()
	{
	PNT("CControlAppStateMachine::CControlAppStateAServicesStopped::VBusError()\n");
	(void) iParentStateMachine.iParentControlAppEngine.ClearVBusError(); // Not interested in error code
	// A call to RequestSession() will be necessary to raise VBus again
	_LIT(KMsgVBusError,	"Error: Couldn't power USB device - please unplug");
	iParentStateMachine.iParentControlAppEngine.DisplayUserMessage(KMsgVBusError);
	}


CIdPinWatcher* CIdPinWatcher::NewL(MControlAppEngineWatcherInterface& aControlAppEngine)
	{
	CIdPinWatcher* self = new(ELeave) CIdPinWatcher(aControlAppEngine);
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

CIdPinWatcher::CIdPinWatcher(MControlAppEngineWatcherInterface& aControlAppEngine)
	: CActive(EPriorityStandard)
	, iParentControlAppEngine(aControlAppEngine)
	{
	CActiveScheduler::Add(this);
	}

void CIdPinWatcher::ConstructL()
	{
	TInt err = iIdPinProp.Attach(KUidUsbManCategory, KUsbOtgIdPinPresentProperty);
	LOG("CIdPinWatcher::ConstructL iIdPinProp.Attach(KUidUsbManCategory, KUsbOtgIdPinPresentProperty) => %d",err);
	User::LeaveIfError(err);

	SubscribeForNotification();

	// Get the current value and update the Engine
	TInt val;
	err = iIdPinProp.Get(val);
	LOG("CIdPinWatcher::ConstructL iIdPinProp.Get(val) => %d",err);
	User::LeaveIfError(err);
	iParentControlAppEngine.SetIdPin(val);
	}

void CIdPinWatcher::SubscribeForNotification()
	{
	iIdPinProp.Subscribe(iStatus);
	SetActive();
	}
	
void CIdPinWatcher::RunL()
	{
	SubscribeForNotification();
	// Get newly changed value
	TInt val;
	User::LeaveIfError(iIdPinProp.Get(val));
	// Update value in Engine
	iParentControlAppEngine.SetIdPin(val);
	}

void CIdPinWatcher::DoCancel()
	{
	iIdPinProp.Cancel();
	}


	
CVBusWatcher* CVBusWatcher::NewL(MControlAppEngineWatcherInterface& aControlAppEngine)
	{
	CVBusWatcher* self = new(ELeave) CVBusWatcher(aControlAppEngine);
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

CVBusWatcher::CVBusWatcher(MControlAppEngineWatcherInterface& aControlAppEngine)
	: CActive(EPriorityStandard)
	, iParentControlAppEngine(aControlAppEngine)
	{
	CActiveScheduler::Add(this);
	}

void CVBusWatcher::ConstructL()
	{
	TInt err = iVBusProp.Attach(KUidUsbManCategory, KUsbOtgVBusPoweredProperty);
	LOG("CVBusWatcher::ConstructL iVBusProp.Attach(KUidUsbManCategory, KUsbOtgVBusPoweredProperty) => %d",err);
	User::LeaveIfError(err);

	SubscribeForNotification();

	// Get the current value and update the Engine
	TInt val;
	err = iVBusProp.Get(val);
	LOG("CVBusWatcher::ConstructL iVBusProp.Get(val) => %d",err);
	User::LeaveIfError(err);
	iParentControlAppEngine.SetVBus(val);
	}

void CVBusWatcher::SubscribeForNotification()
	{
	iVBusProp.Subscribe(iStatus);
	SetActive();
	}

void CVBusWatcher::RunL()
	{
	SubscribeForNotification();
	// Get newly changed value
	TInt val;
	User::LeaveIfError(iVBusProp.Get(val));
	// Update value in Engine
	iParentControlAppEngine.SetVBus(val);
	}

void CVBusWatcher::DoCancel()
	{
	iVBusProp.Cancel();
	}



CConnectionIdleWatcher* CConnectionIdleWatcher::NewL(MControlAppEngineWatcherInterface& aControlAppEngine)
	{
	CConnectionIdleWatcher* self = new(ELeave) CConnectionIdleWatcher(aControlAppEngine);
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

CConnectionIdleWatcher::CConnectionIdleWatcher(MControlAppEngineWatcherInterface& aControlAppEngine)
	: CActive(EPriorityStandard)
	, iParentControlAppEngine(aControlAppEngine)
	{
	CActiveScheduler::Add(this);
	}

void CConnectionIdleWatcher::ConstructL()
	{
	TInt err = iConnIdleProp.Attach(KUidUsbManCategory, KUsbOtgConnectionIdleProperty);
	LOG("CConnectionIdleWatcher::ConstructL iIdPinProp.Attach(KUidUsbManCategory, KUsbOtgConnectionIdleProperty) => %d",err);
	User::LeaveIfError(err);

	SubscribeForNotification();

	// Get the current value and update the Engine
	TInt val;
	err = iConnIdleProp.Get(val);
	LOG("CConnectionIdleWatcher::ConstructL iConnIdleProp.Get(val) => %d",err);
	User::LeaveIfError(err);
	iParentControlAppEngine.SetConnectionIdle(val);
	}

void CConnectionIdleWatcher::SubscribeForNotification()
	{
	iConnIdleProp.Subscribe(iStatus);
	SetActive();
	}
	
void CConnectionIdleWatcher::RunL()
	{
	SubscribeForNotification();
	// Get newly changed value
	TInt val;
	User::LeaveIfError(iConnIdleProp.Get(val));
	// Update value in Engine
	iParentControlAppEngine.SetConnectionIdle(val);
	}

void CConnectionIdleWatcher::DoCancel()
	{
	iConnIdleProp.Cancel();
	}


	
CMessageWatcher* CMessageWatcher::NewL(MControlAppEngineWatcherInterface& aControlAppEngine)
	{
	CMessageWatcher* self = new(ELeave) CMessageWatcher(aControlAppEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMessageWatcher::~CMessageWatcher()
	{
	Cancel();
	}

CMessageWatcher::CMessageWatcher(MControlAppEngineWatcherInterface& aControlAppEngine)
	: CActive(EPriorityStandard)
	, iParentControlAppEngine(aControlAppEngine)
	{
	CActiveScheduler::Add(this);
	}

void CMessageWatcher::ConstructL()
	{
	iParentControlAppEngine.Usb().MessageNotification(iStatus, iMessage);
	SetActive();
	}

void CMessageWatcher::DoCancel()
	{
	iParentControlAppEngine.Usb().MessageNotificationCancel();
	}

void CMessageWatcher::RunL()
	{
	TInt err = iStatus.Int();
	if (err)
		{
		LOG("CMessageWatcher::RunL()	iStatus.Int()=%d", err);
		}
	else
		{
		switch(iMessage)
			{
			case KErrUsbOtgVbusError:			       	
				iParentControlAppEngine.MessageReceived(EErrUsbOtgVbusError);
				break;	
			case KErrUsbOtgSrpTimeout:			       	
				iParentControlAppEngine.MessageReceived(EErrUsbOtgSrpTimeout);
				break;
			case KUsbMessageSrpReceived:				
				iParentControlAppEngine.MessageReceived(EUsbMessageSrpReceived);
				break;
			case KUsbMessageRequestSession:				
				iParentControlAppEngine.MessageReceived(EUsbMessageRequestSession);
				break;
			default: // The State Machine is not interested in any other messages
				break;
			}

		LOG("CMessageWatcher::RunL()	msg = %d", iMessage);
		} // else

	iParentControlAppEngine.Usb().MessageNotification(iStatus, iMessage);
	SetActive();
	}


