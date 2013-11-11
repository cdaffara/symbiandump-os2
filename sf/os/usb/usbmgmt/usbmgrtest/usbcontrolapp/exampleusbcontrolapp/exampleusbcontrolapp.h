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

#ifndef EXAMPLEUSBCONTROLAPP_H
#define EXAMPLEUSBCONTROLAPP_H

#include <e32cmn.h>
#include <e32base.h>
#include <usbman.h>
#include <e32property.h>
#include "e32msgqueue.h"

#include "usbcontrolappshared.h"


class MControlAppEngineWatcherInterface;
static const TInt KInactivityTimerPeriod = 10000000; // 10 sec


NONSHARABLE_CLASS(CIdPinWatcher) : public CActive
	{
public:
	static CIdPinWatcher* NewL(MControlAppEngineWatcherInterface& aControlAppEngine);
	~CIdPinWatcher();

private:
	CIdPinWatcher(MControlAppEngineWatcherInterface& aControlAppEngine);
	void ConstructL();
	void SubscribeForNotification();
	void DoCancel();
	void RunL();

private:
	MControlAppEngineWatcherInterface&	iParentControlAppEngine;
	RProperty							iIdPinProp;
	};


NONSHARABLE_CLASS(CVBusWatcher) : public CActive
	{
public:
	static CVBusWatcher* NewL(MControlAppEngineWatcherInterface& aControlAppEngine);
	~CVBusWatcher();

private:
	CVBusWatcher(MControlAppEngineWatcherInterface& aControlAppEngine);
	void ConstructL();
	void SubscribeForNotification();
	void DoCancel();
	void RunL();

private:
	MControlAppEngineWatcherInterface&	iParentControlAppEngine;
	RProperty							iVBusProp;
	};
	

NONSHARABLE_CLASS(CConnectionIdleWatcher) : public CActive
	{
public:
	static CConnectionIdleWatcher* NewL(MControlAppEngineWatcherInterface& aControlAppEngine);
	~CConnectionIdleWatcher();

private:
	CConnectionIdleWatcher(MControlAppEngineWatcherInterface& aControlAppEngine);
	void ConstructL();
	void SubscribeForNotification();
	void DoCancel();
	void RunL();

private:
	MControlAppEngineWatcherInterface&	iParentControlAppEngine;
	RProperty							iConnIdleProp;
	};


NONSHARABLE_CLASS(CMessageWatcher) : public CActive
	{
public:
	static CMessageWatcher* NewL(MControlAppEngineWatcherInterface& aControlAppEngine);
	~CMessageWatcher();

private:
	CMessageWatcher(MControlAppEngineWatcherInterface& aControlAppEngine);
	void ConstructL();

	void DoCancel();
	void RunL();

public:
	enum TMessageWatcherNotifications
		{
		EUsbMessageRequestSession,
		EUsbMessageSrpReceived,
		EErrUsbOtgSrpTimeout,
		EErrUsbOtgVbusError
		};

private:
	MControlAppEngineWatcherInterface&	iParentControlAppEngine;
	TInt								iMessage;
	};


NONSHARABLE_CLASS(MControlAppStateMachineInterface)
	{
public: // All the events that the state machine can receive from outside:
	//		Initial
	virtual void StartUp()					= 0;
	//		B-Stopped, B-Started
	virtual void IdPinPresent()				= 0;
	//		B-Stopped, A-Started
	virtual void VBusRise()					= 0;
	//		B-Started, A-Started
	virtual void VBusDrop()					= 0;
	//		A-Started, A-Stopped
	virtual void IdPinAbsent()				= 0;
	//		A-Started
	virtual void ConnectionIdle()			= 0;
	//		A-Started
	virtual void ConnectionActive()			= 0;
	//		B-Stopped, A-Stopped, B-Started, A-Started
	virtual void RequestSessionCalled()		= 0;
	//		B-Started
	virtual void SrpTimeout()				= 0;
	//		A-Stopped, A-Started
	virtual void SrpDetected()				= 0;
	//		A-Started, A-Stopped
	virtual void VBusError()				= 0;
	};

class MControlAppEngineInterface;

NONSHARABLE_CLASS(CControlAppStateMachine) : public CBase, public MControlAppStateMachineInterface
	{
// States are friend classes so that they can call private methods of the state machine.
// We don't want those private methods visible outside the state machine.
friend class CControlAppStateInitial;
friend class CControlAppStateBServicesStopped;
friend class CControlAppStateBServicesStarted;
friend class CControlAppStateAServicesStarted;
friend class CControlAppStateAServicesStopped;
friend class CInactivityTimer;

public:
	static CControlAppStateMachine* NewL(MControlAppEngineInterface& aControlAppEngine);
	~CControlAppStateMachine();
private:
	CControlAppStateMachine(MControlAppEngineInterface& aControlAppEngine);
	void ConstructL();

public: // From MControlAppStateMachineInterface, all the events that the state machine can receive from outside:
	void StartUp();
	void IdPinPresent();
	void VBusRise();
	void VBusDrop();
	void IdPinAbsent();
	void ConnectionIdle();
	void ConnectionActive();
	void RequestSessionCalled();
	void SrpTimeout();
	void SrpDetected();
	void VBusError();

private:
	enum TControlAppState
		{
		EStateInitial,
		EStateBServicesStopped,
		EStateBServicesStarted,
		EStateAServicesStopped,
		EStateAServicesStarted
		};
	void SetState(TControlAppState aState);

	void ResetInactivityTimer();
	void CancelInactivityTimer();
	void InactivityTimerExpired();


	NONSHARABLE_CLASS(CInactivityTimer) : public CActive
		{
	public:
		static CInactivityTimer* NewL(CControlAppStateMachine& aParentStateMachine);
		~CInactivityTimer();
		void Reset(); // Sets and resets timer. 
		// Timer cancelled by calling CActive::Cancel()
	private:
		CInactivityTimer(CControlAppStateMachine& aParentStateMachine);
		void ConstructL();		
		void DoCancel();
		void RunL();
	private:
		CControlAppStateMachine&	iParentStateMachine;
		RTimer						iTimer;
		};

// Base State
	NONSHARABLE_CLASS(CControlAppStateBase) : public CBase, public MControlAppStateMachineInterface
		{
	public:
		CControlAppStateBase(CControlAppStateMachine& aParentStateMachine);
		virtual void StartUp();
		virtual void IdPinPresent();	
		virtual void VBusRise();
		virtual void VBusDrop();
		virtual void IdPinAbsent();
		virtual void ConnectionIdle();
		virtual void ConnectionActive();
		virtual void InactivityTimerExpired();	// Internally generated event
		virtual void RequestSessionCalled();
		virtual void SrpTriggered();			// Internally generated event
		virtual void SrpTimeout();
		virtual void SrpDetected();
		virtual void VBusError();
	protected:
		CControlAppStateMachine& iParentStateMachine;
		};

// Initial State
	NONSHARABLE_CLASS(CControlAppStateInitial) : public CControlAppStateBase
		{
	public:
		CControlAppStateInitial(CControlAppStateMachine& aParentStateMachine);
		// No action on these events which will come from Watchers when they are created.
		// The Watchers will update the Engine with these values however and the 
		// State Machine can act on those values on receiving the StartUp event.
		void VBusRise();
		void VBusDrop();
		void IdPinPresent();
		void IdPinAbsent();
		void ConnectionIdle();
		void ConnectionActive();
		// Act on this events only:
		void StartUp();
		};

// B-ServicesStopped State
	NONSHARABLE_CLASS(CControlAppStateBServicesStopped) : public CControlAppStateBase
		{
	public:
		CControlAppStateBServicesStopped(CControlAppStateMachine& aParentStateMachine);
		void VBusRise();
		void IdPinPresent();
		void RequestSessionCalled();
		void VBusDrop();
		};

// B-ServicesStarted State
	NONSHARABLE_CLASS(CControlAppStateBServicesStarted) : public CControlAppStateBase
		{
	public:
		CControlAppStateBServicesStarted(CControlAppStateMachine& aParentStateMachine);
		void VBusDrop();	
		void SrpTriggered(); // Internally generated event
		void SrpTimeout();
		void VBusRise(); // called when SRP has been successful
		void IdPinPresent();
		void RequestSessionCalled();
		};

// A-ServicesStarted State
	NONSHARABLE_CLASS(CControlAppStateAServicesStarted) : public CControlAppStateBase
		{
	public:
		CControlAppStateAServicesStarted(CControlAppStateMachine& aParentStateMachine);
		void IdPinAbsent();
		void SrpDetected();
		void VBusRise();
		void ConnectionIdle();
		void ConnectionActive();
		void InactivityTimerExpired(); // Internally generated event
		void RequestSessionCalled();
		void VBusError();
		};

// A-ServicesStopped State
	NONSHARABLE_CLASS(CControlAppStateAServicesStopped) : public CControlAppStateBase
		{
	public:
		CControlAppStateAServicesStopped(CControlAppStateMachine& aParentStateMachine);
		void IdPinAbsent();
		void SrpDetected();
		void RequestSessionCalled();
		void VBusError();
		};

private:
	CControlAppStateBase*				iCurrentState;
	CControlAppStateInitial*			iStateInitial;
	CInactivityTimer*					iInactivityTimer;
	CControlAppStateBServicesStopped*	iStateBServicesStopped;
	CControlAppStateBServicesStarted*	iStateBServicesStarted;
	CControlAppStateAServicesStarted*	iStateAServicesStarted;
	CControlAppStateAServicesStopped*	iStateAServicesStopped;

private:
	MControlAppEngineInterface&			iParentControlAppEngine;
	TBool								iTriggerSrp;
	};


NONSHARABLE_CLASS(MShutdownInterface)
	{
public:
	virtual void Stop() const = 0;
	};


class CUsbControlAppEngine;

NONSHARABLE_CLASS(CShutdownMonitor) : public CActive
	{
public:
	static CShutdownMonitor* NewL(MShutdownInterface& aControlAppEngine);
	~CShutdownMonitor();
private:
	CShutdownMonitor(MShutdownInterface& aControlAppEngine);
	void ConstructL();
	// From CActive
	void DoCancel();
	void RunL();
private:
	RProperty				iShutdownProp;
	MShutdownInterface&		iParentControlAppEngine;
	};


NONSHARABLE_CLASS(MControlAppEngineInterface)
	{
public: // All the services the Engine provides that the State Machine calls on:
	virtual TInt GetIdPin()									= 0;
	virtual TInt GetVBus()									= 0;
	virtual TInt StopUsbServices()							= 0;
	virtual TInt StartUsbServices()							= 0;
	virtual TInt EnableFunctionDriverLoading()				= 0;
	virtual void DisableFunctionDriverLoading()				= 0;
	virtual TInt BusRequest()								= 0;
	virtual TInt BusDrop()									= 0;
	virtual TInt BusRespondSrp()							= 0;
	virtual TInt ClearVBusError()							= 0;
	virtual void DisplayUserMessage(const TDesC& aUserMsg)	= 0;
	};

NONSHARABLE_CLASS(MControlAppEngineWatcherInterface)
	{
public: // All the methods the Engine provides that the Watcher classes can call:
	virtual void SetIdPin(TInt aIdPin)				= 0;
	virtual void SetVBus(TInt aVBus)				= 0;
	virtual void SetConnectionIdle(TInt aConnIdle)	= 0;
	virtual void MessageReceived(CMessageWatcher::TMessageWatcherNotifications aMessageNotification) = 0;
	virtual RUsb& Usb()								= 0;
	};

NONSHARABLE_CLASS(CUsbControlAppEngine) : public CBase, public MControlAppEngineInterface, public MControlAppEngineWatcherInterface, public MShutdownInterface
	{
public:
	static CUsbControlAppEngine* NewLC();
	~CUsbControlAppEngine();
	// Start/stop Active Scheduler
	void Start();     
	void Stop() const;
	// From MControlAppEngineWatcherInterface, Watcher callbacks
	void SetIdPin(TInt aIdPin);
	void SetVBus(TInt aVBus);
	void SetConnectionIdle(TInt aConnIdle);
	void MessageReceived(CMessageWatcher::TMessageWatcherNotifications aMessageNotification);
	RUsb& Usb();
	// Getters
	TInt GetIdPin();
	TInt GetVBus();
	// From MControlAppEngineInterface, called by State Machine
	TInt StopUsbServices();
	TInt StartUsbServices();
	TInt EnableFunctionDriverLoading();
	void DisableFunctionDriverLoading();
	TInt BusRequest();
	TInt BusDrop();
	TInt BusRespondSrp();
	TInt ClearVBusError();
	void DisplayUserMessage(const TDesC& aUserMsg);

private:
	CUsbControlAppEngine();
	void ConstructL();

private: 
	// Watcher related
	CIdPinWatcher* 			iIdPinWatcher;
	CVBusWatcher* 			iVBusWatcher;
	CConnectionIdleWatcher* iConnIdleWatcher;
	CMessageWatcher*		iMessageWatcher;
	TInt					iIdPin;
	TInt					iVBus;
	TInt					iConnIdle;
	// Resources
	CControlAppStateMachine*	iStateMachine;
	CShutdownMonitor*			iShutdownMonitor;
	RUsb						iUsb;
	//    RMsgQueue between exampleusbcontrolapp.exe & usbviewer.exe for displaying user messages
	RMsgQueue<TBuf<KViewerNumCharactersOnLine> >	iViewerMsgQ; 
	}; // CUsbControlAppEngine

#endif // EXAMPLEUSBCONTROLAPP_H
