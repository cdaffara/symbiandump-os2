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

#ifndef USBVIEWER_H
#define USBVIEWER_H

#include <e32cmn.h>
#include <e32base.h>
#include <usbman.h>
#include <e32property.h>
#include "e32msgqueue.h"

#include "usbcontrolappshared.h"

class CConsoleBase;

class CUserMsgQWatcher;
class CServiceStateWatcher;
class CDeviceStateWatcher;
class CIdPinWatcher;
class CVBusWatcher;
class CConnectionIdleWatcher;
class CHostEventWatcher;
class CMessageWatcher;
class COtgStateWatcher;

class CShutdownMonitor;

_LIT(KUsbViewerTitle, "USB Viewer");


NONSHARABLE_STRUCT(XUsbViewerEvent)
	{
	~XUsbViewerEvent();
	TDblQueLink	iLink;
	RBuf		iEvent;
	};


NONSHARABLE_CLASS(MShutdownInterface)
	{
public:
	virtual void Stop() const = 0;
	};


NONSHARABLE_CLASS(CUsbViewer) : public CActive, public MShutdownInterface
	{
public:
	static CUsbViewer* NewLC();
	~CUsbViewer();

public:
	void Start();
	void Stop() const;
	
	RUsb& Usb();
	RMsgQueue<TBuf<KViewerNumCharactersOnLine> >& UserMsgQ();	

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
	void SetVBus(TInt aVBus);
	void SetConnectionIdle(TInt aConnIdle);
	void SetOtgState(TInt aOtgState);
	void SetDriverLoading(TFdfDriverLoadingState aDriverLoading);
	void SetAttachedDevices(TUint aAttachedDevices);
	void SetDeviceType(TDeviceType aDeviceType);

	void NotifyEvent(XUsbViewerEvent* aEvent);

private:
	CUsbViewer();
	void ConstructL();
	void Move(TInt aX, TInt aY);
	void Draw();

	void DoCancel();
	void RunL();
	void ScheduleDraw();
	
	TBool		iAutoSrpResponse;
	TDeviceType iDeviceType;
	
private:
	CConsoleBase*		iConsole;
	RUsb				iUsb;
	CShutdownMonitor*	iShutdownMonitor;
	
private:
	CUserMsgQWatcher*		iUserMsgQWatcher;
	CServiceStateWatcher*	iServiceStateWatcher;
	CDeviceStateWatcher*	iDeviceStateWatcher;
	CIdPinWatcher*			iIdPinWatcher;
	CVBusWatcher*			iVBusWatcher;
	CConnectionIdleWatcher*	iConnIdleWatcher;
	COtgStateWatcher*		iOtgStateWatcher;
	CHostEventWatcher*		iHostEventWatcher;
	CMessageWatcher*		iMessageWatcher;

private: // Display variables
	TVersion	iVersion;
	TBuf<11>	iServStatus; // Needs Trailing Space
	TBuf<11>	iDevStatus; // Needs Trailing Space
	TBuf<5>		iIdPin; // Needs Trailing Space
	TBuf<5>		iVBus;  // Needs Trailing Space
	TBuf<5>		iConnIdle; // Needs Trailing Space
	TBuf<3>		iDriverLoading;
	TBuf<3>		iAttachedDevices;
	TBuf<14>	iOtgState;  // Needs Trailing Space
	TBuf<5>		iAttachedDevice;

private:
	static const TInt KNumEventsInWindow = 7;
	// H4 screen can have 22 lines of text on it at once
	static const TInt KNumLinesInWindow = 14;
	
private: // Event list
	TDblQue<XUsbViewerEvent>						iEventList;
	// RMsgQueue between exampleusbcontrolapp.exe & usbviewer.exe for displaying user messages
	RMsgQueue<TBuf<KViewerNumCharactersOnLine> >	iUserMsgQ;
	};
	

NONSHARABLE_CLASS(CShutdownMonitor) : public CActive
	{
public:
	static CShutdownMonitor* NewL(MShutdownInterface& aParentUsbViewer);
	~CShutdownMonitor();
private:
	CShutdownMonitor(MShutdownInterface& aParentUsbViewer);
	void ConstructL();
	// From CActive
	void DoCancel();
	void RunL();
private:
	RProperty				iShutdownProp;
	MShutdownInterface&		iParentUsbViewer;
	};

	
NONSHARABLE_CLASS(CEventNotifier) : public CActive
	{
protected:
	CEventNotifier(TInt aPriority);
	XUsbViewerEvent* NewViewerEventL();
	virtual void DoRunL(XUsbViewerEvent* aEvent) = 0;
private:
	void RunL();
	};
	
	
NONSHARABLE_CLASS(CServiceStateWatcher) : public CEventNotifier
	{
public:
	static CServiceStateWatcher* NewL(CUsbViewer& aUsb);
	~CServiceStateWatcher();

private:
	CServiceStateWatcher(CUsbViewer& aUsb);
	void ConstructL();

	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&	iUsbViewer;
	TUsbServiceState	iServiceState;
	};


NONSHARABLE_CLASS(CDeviceStateWatcher) : public CEventNotifier
	{
public:
	static CDeviceStateWatcher* NewL(CUsbViewer& aUsb);
	~CDeviceStateWatcher();

private:
	CDeviceStateWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&	iUsbViewer;
	TUsbDeviceState		iDeviceState;
	};


NONSHARABLE_CLASS(CIdPinWatcher) : public CEventNotifier
	{
public:
	static CIdPinWatcher* NewL(CUsbViewer& aUsb);
	~CIdPinWatcher();

private:
	CIdPinWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&	iUsbViewer;
	RProperty			iIdPinProp;
	};


NONSHARABLE_CLASS(CVBusWatcher) : public CEventNotifier
	{
public:
	static CVBusWatcher* NewL(CUsbViewer& aUsb);
	~CVBusWatcher();

private:
	CVBusWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&		iUsbViewer;
	RProperty		iVBusProp;
	};


NONSHARABLE_CLASS(CConnectionIdleWatcher) : public CEventNotifier
	{
public:
	static CConnectionIdleWatcher* NewL(CUsbViewer& aUsb);
	~CConnectionIdleWatcher();

private:
	CConnectionIdleWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&		iUsbViewer;
	RProperty		iConnIdleProp;
	};

	
NONSHARABLE_CLASS(COtgStateWatcher) : public CEventNotifier
	{
public:
	static COtgStateWatcher* NewL(CUsbViewer& aUsb);
	~COtgStateWatcher();

private:
	COtgStateWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&		iUsbViewer;
	RProperty		iOtgStateProp;
	};
	
	
NONSHARABLE_CLASS(CHostEventWatcher) : public CEventNotifier
	{
public:
	static CHostEventWatcher* NewL(CUsbViewer& aUsb);
	~CHostEventWatcher();

private:
	CHostEventWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&				iUsbViewer;
	TDeviceEventInformation	iDeviceInfo;
	RArray<TUint>			iAttachedDevices;
	};
	

NONSHARABLE_CLASS(CMessageWatcher) : public CEventNotifier
	{
public:
	static CMessageWatcher* NewL(CUsbViewer& aUsb);
	~CMessageWatcher();

private:
	CMessageWatcher(CUsbViewer& aUsb);
	void ConstructL();

	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&	iUsbViewer;
	TInt		iMessage;
	};


NONSHARABLE_CLASS(CUserMsgQWatcher) : public CEventNotifier
	{
public:
	static CUserMsgQWatcher* NewL(CUsbViewer& aUsb);
	~CUserMsgQWatcher();

private:
	CUserMsgQWatcher(CUsbViewer& aUsb);
	void ConstructL();
	
	void DoCancel();
	void DoRunL(XUsbViewerEvent* aEvent);

private:
	CUsbViewer&							iUsbViewer;
	TBuf<KViewerNumCharactersOnLine>	iUserMsgLine;
	};
	

#endif // USBVIEWER_H
