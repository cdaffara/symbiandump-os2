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

#include <e32cons.h>
#include <e32debug.h>

#include "e32svr.h"
#include <e32keys.h>

#include <e32property.h> 

#include "controlappbinder.h"



#define LOG(A,B) RDebug::Print(_L("UsbControlAppBinder: " L##A),B)

#define PNT(A)	 RDebug::Print(_L("UsbControlAppBinder: " L##A))

#define PANIC Panic(__LINE__)

void Panic(TInt aLine)
	{
	RDebug::Printf("UsbControlAppBinder: PANIC line=%d", aLine);
	User::Panic(_L("UsbControlAppBinder"), aLine);
	}


void RunAppL()
	{
	CUsbControlAppBinderConsole* binderapp = CUsbControlAppBinderConsole::NewLC();
	binderapp->Start();
	CleanupStack::PopAndDestroy(binderapp);
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
		PNT("*** UsbControlAppBinder E32Main ***\n");
		TRAP(err, RunAppL());
		}
	delete activeScheduler;
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}



CUsbControlAppBinderConsole* CUsbControlAppBinderConsole::NewLC()
	{
	CUsbControlAppBinderConsole* self = new(ELeave) CUsbControlAppBinderConsole;
	CleanupStack::PushL(self);
	self->ConstructL();
	PNT("Constructed USB Control App Binder\n");
	return self;
	}
	
CUsbControlAppBinderConsole::~CUsbControlAppBinderConsole()
	{
	PNT("Closing USB Control App Binder\n");
	delete iKeys;
	delete iConsole;
	}

CUsbControlAppBinderConsole::CUsbControlAppBinderConsole()
	{
	}

void CUsbControlAppBinderConsole::ConstructL()
	{
	iConsole = Console::NewL(KUsbControlAppBinderTitle, TSize(KBinderWindowWidth, KBinderWindowDepth));

	Move((KScreenWidth-KBinderWindowWidth-2)/2,1);
	
	// After everything else, enable interactivity.
	iKeys = CUsbControlAppBinderKeys::NewL(*this);

	Draw();
	}

void CUsbControlAppBinderConsole::Move(TInt aX, TInt aY)
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

void CUsbControlAppBinderConsole::Start()
	{
	// Get everything running
	CActiveScheduler::Start();
	}
	
void CUsbControlAppBinderConsole::Stop() const
	{
	CActiveScheduler::Stop();
	}

void CUsbControlAppBinderConsole::Draw()
	{
	iConsole->ClearScreen();
	
	iConsole->Printf
	(	_L(	 "\r\n"
			L"'A'\r\n"
			L"\r\n"
			L"\r\n"
			L"\r\n"
			L"'C'\r\n"
			L"\r\n"
			L"\r\n"
			L"\r\n"
			L"'V'\r\n"
			L"\r\n"
			L"\r\n"
			L"\r\n"
			L"'*'\r\n"
			L"\r\n"
			L"\r\n"
			L"\r\n"
			L"'Q'\r\n"
		  )
	);
	}

void CUsbControlAppBinderConsole::StartAwareApp()
	{
	PNT("Start Aware Program\n");
	RProcess iAwareApp;
	TInt err = iAwareApp.Create(_L("testusbawareapp.exe"), KNullDesC);
	
	if (err != KErrNone)
		{
		LOG("Failed to create Aware program, err=%d",err);
		iAwareApp.Close();
		}
	else
		{
		iAwareApp.Resume();
		}
	}

void CUsbControlAppBinderConsole::StartControlApp()
	{
	PNT("Start Control Program\n");
	RProcess iControlApp;
	TInt err = iControlApp.Create(_L("exampleusbcontrolapp.exe"), KNullDesC);
	
	if (err != KErrNone)
		{
		LOG("Failed to create Control program, err=%d",err);
		iControlApp.Close();
		}
	else
		{
		iControlApp.Resume();
		}
	}

void CUsbControlAppBinderConsole::StartViewerApp()
	{
	PNT("Start Viewer Program\n");
	RProcess iViewerApp;
	TInt err = iViewerApp.Create(_L("usbviewer.exe"), KNullDesC);
	
	if (err != KErrNone)
		{
		LOG("Failed to create Viewer program, err=%d",err);
		iViewerApp.Close();
		}
	else
		{
		iViewerApp.Resume();
		}
	}



CUsbControlAppBinderKeys* CUsbControlAppBinderKeys::NewL(CUsbControlAppBinderConsole& aUsbControlAppBinderConsole)
	{
	CUsbControlAppBinderKeys* self = new(ELeave) CUsbControlAppBinderKeys(aUsbControlAppBinderConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUsbControlAppBinderKeys::~CUsbControlAppBinderKeys()
	{
	Cancel();
	}

CUsbControlAppBinderKeys::CUsbControlAppBinderKeys(CUsbControlAppBinderConsole& aUsbControlAppBinderConsole)
	: CActive(EPriorityStandard)
	, iUsbControlAppBinderConsole(aUsbControlAppBinderConsole)
	{
	CActiveScheduler::Add(this);
	}

void CUsbControlAppBinderKeys::ConstructL()
	{
	TInt err = RProperty::Define(KUidUsbControlAppCategory, KUsbControlAppShutdownKey, RProperty::EInt);	
	LOG("CUsbControlAppBinderKeys::ConstructL()   RProperty::Define()   err=%d", err);
	if(err)
		{
		PNT("CUsbControlAppBinderKeys::ConstructL()   RProperty::Define()	Property probably already defined\n");
		}
	err = RProperty::Set(KUidUsbControlAppCategory, KUsbControlAppShutdownKey, 0);
	LOG("CUsbControlAppBinderKeys::ConstructL()   RProperty::Set()   err=%d", err);	
	User::LeaveIfError(err);	
	// Allow keyboard input
	iUsbControlAppBinderConsole.iConsole->Read(iStatus);
	SetActive();
	}

#define SECONDS(t) (t*1000000)

void CUsbControlAppBinderKeys::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	TKeyCode theKey = iUsbControlAppBinderConsole.iConsole->KeyCode();
	
	switch(theKey)
		{
		case 'a': case 'A':
			iUsbControlAppBinderConsole.StartAwareApp();
			break;

		case 'c': case 'C':
			iUsbControlAppBinderConsole.StartControlApp();
			break;

		case 'v': case 'V':
			iUsbControlAppBinderConsole.StartViewerApp();
			break;

		case '*':
			PNT("Start Everything\n");
			iUsbControlAppBinderConsole.StartControlApp();
			User::After(SECONDS(4));
			iUsbControlAppBinderConsole.StartViewerApp();
			User::After(SECONDS(4));
			iUsbControlAppBinderConsole.StartAwareApp();
			break;
			
		case 'q': case 'Q':
			{
			TInt err = RProperty::Set(KUidUsbControlAppCategory, KUsbControlAppShutdownKey, 1);
			LOG("CUsbAwareAppKeys::RunL()   RProperty::Set()   err=%d", err);
			User::LeaveIfError(err);
			iUsbControlAppBinderConsole.Stop();
			return;
			}

		default:
			LOG("Unexpected Key [%c]",theKey);
			break;
		}
	
	iUsbControlAppBinderConsole.iConsole->Read(iStatus);
	SetActive();
	}

void CUsbControlAppBinderKeys::DoCancel()
	{
	iUsbControlAppBinderConsole.iConsole->ReadCancel();
	}




