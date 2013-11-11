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

#ifndef USBTESTCONSOLE_H
#define USBTESTCONSOLE_H

#include <e32cmn.h>
#include <e32base.h>
#include <usbman.h>
#include <e32property.h>

class CConsoleBase;

class CUsbTestConsoleKeys;

class CServiceStateWatcher;
class CDeviceStateWatcher;
class CConnectionIdleWatcher;
class CIdPinWatcher;
class CVBusWatcher;
class CHostEventWatcher;
class CMessageWatcher;
class COtgStateWatcher;

class CUsbManStarter;
class CUsbManStoper;
class CUsbManTryStarter;
class CUsbManTryStoper;
class CUsbTestTimer;   

_LIT(KUsbTestConsoleTitle, "USB Test Console");


NONSHARABLE_STRUCT(XUsbTestConsoleEvent)
	{
	~XUsbTestConsoleEvent();
	TDblQueLink	iLink;
	RBuf		iEvent;
	};


NONSHARABLE_CLASS(CUsbTestConsole) : public CActive
	{
friend class CUsbTestConsoleKeys;

public:
	static CUsbTestConsole* NewLC();
	~CUsbTestConsole();

public:
	void StartL();
	void Stop() const;
	
	RUsb& Usb();
	CUsbTestTimer* Timer() const;  

public:
	enum TFdfDriverLoadingState
		{
		EUnknown,
		EDisabled,
		EEnabled
		};
	
	enum TDeviceType
		{
		ENoDevice,
		EGenericDevice,
		ELogitechHeadset
		};

public: // Callbacks
	void SetServiceState(TUsbServiceState aServiceState);
	void SetDeviceState(TUsbDeviceState aDeviceState);
	void SetIdPin(TInt aIdPin);
    void SetConnectionIdle(TInt aConnectionIdle);
	void SetVBus(TInt aVBus);
	void SetOtgState(TInt aOtgState);
	void SetDriverLoading(TFdfDriverLoadingState aDriverLoading);
	void SetAttachedDevices(TUint aAttachedDevices);
	void SetControlSession(TBool aControl);
	void SetDeviceType(TDeviceType aDeviceType);

	void NotifyEvent(XUsbTestConsoleEvent* aEvent);
	
	TBool GetAutoSrpResponseState();
	void  SetAutoSrpResponseState(TBool aAutoSrpResponse);

private:
	CUsbTestConsole();
	void ConstructL();
	void Draw();

	void DoCancel();
	void RunL();
	void ScheduleDraw(TUint aKey);
	
	TBool 		iHelp;
	TBool		iAutoSrpResponse;
	TDeviceType iDeviceType;
	
private:
	CConsoleBase* iConsole;
	RUsb iUsb;
	
	CUsbTestConsoleKeys* iKeys;
	
private:
	CServiceStateWatcher*	iServiceStateWatcher;
	CDeviceStateWatcher*	iDeviceStateWatcher;
	CIdPinWatcher*			iIdPinWatcher;
    CConnectionIdleWatcher* iConnectionIdleWatcher;
	CVBusWatcher*			iVBusWatcher;
	COtgStateWatcher*		iOtgStateWatcher;
	CHostEventWatcher*		iHostEventWatcher;
	CMessageWatcher*		iMessageWatcher;
	CUsbTestTimer*          iTimer;  

private: // Display variables
	TVersion	iVersion;
	TBuf<9>		iControlSession;
	TBuf<11>	iServStatus; // Needs Trailing Space
	TBuf<11>	iDevStatus; // Needs Trailing Space
	TBuf<5>		iIdPin; // Needs Trailing Space
    TBuf<5>     iConnectionIdle; // Needs Trailing Space
	TBuf<5>		iVBus;  // Needs Trailing Space
	TBuf<3>		iDriverLoading;
	TBuf<3>		iAttachedDevices;
	TBuf<14>	iOtgState;  // Needs Trailing Space
	TBuf<5>		iAttachedDevice;
	TBuf<5>		iSrpState;

public:
	// H4 screen can have 22 lines of text on it at once
	// H4 screen can have 53 characters per line.
	static const TInt KNumCharactersOnLine = 53;

private:
	static const TInt KNumEventsOnScreen = 15;
	
private: // Event list
	TDblQue<XUsbTestConsoleEvent>	iEventList;
	};
	
	
NONSHARABLE_CLASS(CUsbTestConsoleTextEntryBase) : public CActive
	{
public:
	~CUsbTestConsoleTextEntryBase();

protected:
	CUsbTestConsoleTextEntryBase();
	void ConstructL(const TDesC& aEntryField);

private:
	void DoCancel();
	void RunL();

	virtual void ConvertAndSetL() = 0;

private:
	static const TInt KMaxNumOfChars = 255;
	CConsoleBase*	iEntryConsole;
protected:
	RBuf			iChars;
public:
	TDblQueLink		iLink;
	};

NONSHARABLE_CLASS(CUsbTestConsoleKeys) : public CActive
	{
public:
	static CUsbTestConsoleKeys* NewL(CUsbTestConsole& aUsb);
	~CUsbTestConsoleKeys();
	
private:
	CUsbTestConsoleKeys(CUsbTestConsole& aUsb);
	void ConstructL();
	void DoCancel();
	void RunL();
	XUsbTestConsoleEvent* NewConsoleEventL();
	
private:
	CUsbTestConsole&	iTestConsole;
	
private: // Async Utilities
	CUsbManStarter*			iUsbManStarter;
	CUsbManStoper*			iUsbManStoper;
	CUsbManTryStarter*		iUsbManTryStarter;
	CUsbManTryStoper*		iUsbManTryStoper;

private:
	TDblQue<CUsbTestConsoleTextEntryBase>	iEntryConsoles;

private: // configured variables
	TInt	iPersonalityId;
	TBool	iControlSession;
	TBool	iFunctionDriverLoading;
	};

	
template<typename T>
NONSHARABLE_CLASS(CUsbTestConsoleTextEntry) : public CUsbTestConsoleTextEntryBase
	{
public:
	static CUsbTestConsoleTextEntry* NewL(T& aValue, const TDesC& aEntryField);
	~CUsbTestConsoleTextEntry();
	
private:
	CUsbTestConsoleTextEntry(T& aValue);
	void ConvertAndSetL();

private:
    T&						iValue;
	};


	
NONSHARABLE_CLASS(CEventNotifier) : public CActive
	{
protected:
	CEventNotifier(TInt aPriority);
	XUsbTestConsoleEvent* NewConsoleEventL();
	virtual void DoRunL(XUsbTestConsoleEvent* aEvent) = 0;
private:
	void RunL();
	};
	

	
NONSHARABLE_CLASS(CServiceStateWatcher) : public CEventNotifier
	{
public:
	static CServiceStateWatcher* NewL(CUsbTestConsole& aUsb);
	~CServiceStateWatcher();

private:
	CServiceStateWatcher(CUsbTestConsole& aUsb);
	void ConstructL();

	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	TUsbServiceState	iServiceState;
	};


NONSHARABLE_CLASS(CDeviceStateWatcher) : public CEventNotifier
	{
public:
	static CDeviceStateWatcher* NewL(CUsbTestConsole& aUsb);
	~CDeviceStateWatcher();

private:
	CDeviceStateWatcher(CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	TUsbDeviceState		iDeviceState;
	};


NONSHARABLE_CLASS(CIdPinWatcher) : public CEventNotifier
	{
public:
	static CIdPinWatcher* NewL(CUsbTestConsole& aUsb);
	~CIdPinWatcher();

private:
	CIdPinWatcher(CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	RProperty			iIdPinProp;
	};

NONSHARABLE_CLASS(CConnectionIdleWatcher) : public CEventNotifier
	{
public:
	static CConnectionIdleWatcher* NewL(CUsbTestConsole& aUsb);
	~CConnectionIdleWatcher();

private:
	CConnectionIdleWatcher(CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	RProperty			iConnectionIdleProp;
	};


NONSHARABLE_CLASS(CVBusWatcher) : public CEventNotifier
	{
public:
	static CVBusWatcher* NewL(CUsbTestConsole& aUsb);
	~CVBusWatcher();

private:
	CVBusWatcher(CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	RProperty			iVBusProp;
	};
	
NONSHARABLE_CLASS(COtgStateWatcher) : public CEventNotifier
	{
public:
	static COtgStateWatcher* NewL(CUsbTestConsole& aUsb);
	~COtgStateWatcher();

private:
	COtgStateWatcher(CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	RProperty			iOtgStateProp;
	};
	
	
NONSHARABLE_CLASS(CHostEventWatcher) : public CEventNotifier
	{
public:
	static CHostEventWatcher* NewL(CUsbTestConsole& aUsb);
	~CHostEventWatcher();

private:
	CHostEventWatcher(CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&		iTestConsole;
	TDeviceEventInformation	iDeviceInfo;
	RArray<TUint>			iAttachedDevices;
	};
	

NONSHARABLE_CLASS(CMessageWatcher) : public CEventNotifier
	{
public:
	static CMessageWatcher* NewL(CUsbTestConsole& aUsb);
	~CMessageWatcher();

private:
	CMessageWatcher(CUsbTestConsole& aUsb);
	void ConstructL();

	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);

private:
	CUsbTestConsole&	iTestConsole;
	TInt				iMessage;
	};







NONSHARABLE_CLASS(CUsbManStarter) : public CEventNotifier
	{
public:
	static void NewL(CUsbManStarter*& aSelf, CUsbTestConsole& aUsb);
	void DestroyL();
	~CUsbManStarter();
	
private:
	CUsbManStarter(CUsbManStarter*& aSelf, CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);
	
private:
	CUsbManStarter*& 	iSelf;
	CUsbTestConsole&	iTestConsole;
	};
	
	
NONSHARABLE_CLASS(CUsbManStoper) : public CEventNotifier
	{
public:
	static void NewL(CUsbManStoper*& aSelf, CUsbTestConsole& aUsb);
	void DestroyL();
	~CUsbManStoper();
	
private:
	CUsbManStoper(CUsbManStoper*& aSelf, CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);
	
private:
	CUsbManStoper*& 	iSelf;
	CUsbTestConsole&	iTestConsole;
	};
	
	
	
NONSHARABLE_CLASS(CUsbManTryStarter) : public CEventNotifier
	{
public:
	static void NewL(CUsbManTryStarter*& aSelf, CUsbTestConsole& aUsb, TInt aPersonalityId);
	void DestroyL();
	~CUsbManTryStarter();
	
private:
	CUsbManTryStarter(CUsbManTryStarter*& aSelf, CUsbTestConsole& aUsb, TInt aPersonalityId);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);
	
private:
	CUsbManTryStarter*& 	iSelf;
	CUsbTestConsole&	iTestConsole;
	TInt				iPersonalityId;
	};
	
	
NONSHARABLE_CLASS(CUsbManTryStoper) : public CEventNotifier
	{
public:
	static void NewL(CUsbManTryStoper*& aSelf, CUsbTestConsole& aUsb);
	void DestroyL();
	~CUsbManTryStoper();
	
private:
	CUsbManTryStoper(CUsbManTryStoper*& aSelf, CUsbTestConsole& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbTestConsoleEvent* aEvent);
	
private:
	CUsbManTryStoper*& 	iSelf;
	CUsbTestConsole&	iTestConsole;
	};

NONSHARABLE_CLASS(CUsbTestTimer) : public CTimer
	{
public:
	static CUsbTestTimer* NewL(CUsbTestConsole& aUsb);
	~CUsbTestTimer();
	void Start(TTimeIntervalMicroSeconds32 aTime);
		
private:
	CUsbTestTimer(CUsbTestConsole& aUsb);
	void ConstructL();
	void DoCancel();
	void RunL();
	
private:
	TTimeIntervalMicroSeconds32 iTime;
	CUsbTestConsole&	iTestConsole;
	};
#endif // USBTESTCONSOLE_H
