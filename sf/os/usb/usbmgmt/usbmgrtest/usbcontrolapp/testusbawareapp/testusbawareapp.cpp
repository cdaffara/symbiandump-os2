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

#include "testusbawareapp.h"

#include <e32cons.h>
#include <e32debug.h>
#include "e32svr.h"
#include <e32keys.h>

#include "usbcontrolappshared.h"
#include "e32property.h" 

#define LOG(A,B) RDebug::Print(_L("UsbAwareApp: " L##A),B)
#define PNT(A)	 RDebug::Print(_L("UsbAwareApp: " L##A))
#define PANIC Panic(__LINE__)

void Panic(TInt aLine)
	{
	RDebug::Printf("UsbAwareApp: PANIC line=%d", aLine);
	User::Panic(_L("USBAWAREAPP"), aLine);
	}

void RunAppL()
	{
	CUsbAwareAppConsole* awareapp = CUsbAwareAppConsole::NewLC();
	awareapp->Start();
	CleanupStack::PopAndDestroy(awareapp);
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
		PNT("*** UsbAwareApp E32Main ***\n");
		TRAP(err, RunAppL());
		}
	delete activeScheduler;
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}



CUsbAwareAppConsole* CUsbAwareAppConsole::NewLC()
	{
	CUsbAwareAppConsole* self = new(ELeave) CUsbAwareAppConsole;
	CleanupStack::PushL(self);
	self->ConstructL();
	PNT("\nConstructed Aware App\n");
	return self;
	}
	
CUsbAwareAppConsole::~CUsbAwareAppConsole()
	{
	PNT("\nClosing Aware App\n");
	delete iKeys;
	delete iShutdownMonitor;
	delete iConsole;
	}

CUsbAwareAppConsole::CUsbAwareAppConsole()
	{
	}

void CUsbAwareAppConsole::ConstructL()
	{
	iConsole = Console::NewL(KUsbAwareAppTitle, TSize(KViewerNumCharactersOnLine, KNumLinesInWindow));
	Move(-3, 9);
	iShutdownMonitor = CShutdownMonitor::NewL(*this);
	// After everything else, enable interactivity.
	iKeys = CUsbAwareAppKeys::NewL(*this);
	Draw();
	}

void CUsbAwareAppConsole::Move(TInt aX, TInt aY)
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

void CUsbAwareAppConsole::Start()
	{
	// Get everything running
	CActiveScheduler::Start();
	}
	
void CUsbAwareAppConsole::Stop() const
	{
	CActiveScheduler::Stop();
	}

void CUsbAwareAppConsole::Draw()
	{
	iConsole->ClearScreen();
	
	iConsole->Printf(_L(
//          1         2         3         4         5 \r\n
// 12345678901234567890123456789012345678901234567890123
  "Test USB Aware App:           \r\n"
 L"\r\n"
 L"Press 'R' to RequestSession   \r\n"
	)
	);

	}



CUsbAwareAppKeys* CUsbAwareAppKeys::NewL(CUsbAwareAppConsole& aUsbAwareAppConsole)
	{
	CUsbAwareAppKeys* self = new(ELeave) CUsbAwareAppKeys(aUsbAwareAppConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUsbAwareAppKeys::~CUsbAwareAppKeys()
	{
	iUsb.Close();
	Cancel();
	}

CUsbAwareAppKeys::CUsbAwareAppKeys(CUsbAwareAppConsole& aUsbAwareAppConsole)
	: CActive(EPriorityStandard)
	, iUsbAwareAppConsole(aUsbAwareAppConsole)
	{
	CActiveScheduler::Add(this);
	}

void CUsbAwareAppKeys::ConstructL()
	{
	// Connect USB session
	TInt err = iUsb.Connect();
	LOG("CUsbAwareAppKeys::ConstructL()   iUsb.Connect()   err=%d", err);
	User::LeaveIfError(err);
	// Allow keyboard input
	iUsbAwareAppConsole.iConsole->Read(iStatus);
	SetActive();
	}

void CUsbAwareAppKeys::RunL()
	{
	if (!IsActive())
		{
		User::LeaveIfError(iStatus.Int());
		
		switch(iUsbAwareAppConsole.iConsole->KeyCode())
			{
		case 'r': case 'R':
			{
			TInt err = iUsb.RequestSession();
			LOG("CUsbAwareAppKeys::RunL()   RequestSession()	err=%d", err);
			}
			break;
			} // switch
		iUsbAwareAppConsole.iConsole->Read(iStatus);
		SetActive();
		}
	}

void CUsbAwareAppKeys::DoCancel()
	{
	iUsbAwareAppConsole.iConsole->ReadCancel();
	}




CShutdownMonitor* CShutdownMonitor::NewL(MShutdownInterface& aUsbAwareAppConsole)
	{
	CShutdownMonitor* self = new(ELeave) CShutdownMonitor(aUsbAwareAppConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CShutdownMonitor::CShutdownMonitor(MShutdownInterface& aUsbAwareAppConsole)
	: CActive(EPriorityLow) // Low so all notifications that want to be serviced will be done first
	, iUsbAwareAppConsole(aUsbAwareAppConsole)
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
	iUsbAwareAppConsole.Stop(); // Stopping Active Scheduler will results in the destructor getting called
	}

void CShutdownMonitor::DoCancel()
	{
	iShutdownProp.Cancel();
	}
