/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
*/

#include "windowmanager.h"

CWindowManager* CWindowManager::NewL()
	{
	CWindowManager* self = new(ELeave) CWindowManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWindowManager::CWindowManager():
CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CWindowManager::ConstructL()
	{
	User::LeaveIfError(iMsgQueue.CreateLocal(1));
	User::LeaveIfError(iSemaphore.CreateLocal(0));
	
	User::LeaveIfError(iWsSession.Connect());
	// Create the Screen device for the WServ session.
	iWsSd = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iWsSd->Construct());
	// Create a Window Group.
	iWindowGroup = RWindowGroup(iWsSession);
	User::LeaveIfError(iWindowGroup.Construct((TUint32)this, ETrue));
	iWindow = new (ELeave) RWindow(iWsSession);
	User::LeaveIfError(iWindow->Construct(iWindowGroup, ((TUint32)(this)) + 1));

	// Reset the screen mode.
	if(iWsSd->CurrentScreenMode() != 0)
		{
		iWsSd->SetAppScreenMode(0);
		iWsSd->SetScreenMode(0);
		}
	
	iScreenSize = iWsSd->SizeInPixels();
	iWindow->SetExtent(TPoint(0,0), iScreenSize);
	iWindow->SetVisible(ETrue);
	iWindow->Activate();

	User::LeaveIfError(iWindow->SetRequiredDisplayMode(EColor16MA));

	// TODO - this forcing of transparency may not be needed, as there is 
	//		a defect in the graphics component right now
	// force the transparency change to take place
	iWindow->SetBackgroundColor(TRgb(0x000000, 0)); 
	iWindow->BeginRedraw();
	iWindow->EndRedraw();	
	iWsSession.Flush();
	
	iCreatorThreadId = RThread().Id();
	
	SetActive();
	iMsgQueue.NotifyDataAvailable(iStatus);
	}

CWindowManager::~CWindowManager()
	{
	Cancel();
	if(iWindow)
		{
		iWindow->Close();
		delete iWindow;
		}
	iWindowGroup.Close();
	delete iWsSd;
	iWsSession.Close();
	
	iSemaphore.Close();
	iMsgQueue.Close();
	}

void CWindowManager::RunL()
	{
	TMessage msg;
	TInt err = iMsgQueue.Receive(msg);
	if(err != KErrNone && err != KErrUnderflow)
		{
		iReturnCode = err;
		}
	else
		{
		iReturnCode = DoCommand(msg.iCommand, msg.iParam1, msg.iParam2);
		}
	SetActive();
	iMsgQueue.NotifyDataAvailable(iStatus);
	iSemaphore.Signal();
	}

void CWindowManager::DoCancel()
	{
	iMsgQueue.CancelDataAvailable();
	}

TInt CWindowManager::RegisterSurface(TSurfaceId& aSurfaceId)
	{
	return RunCommand(ERegisterSurface, &aSurfaceId);
	}

TInt CWindowManager::SetBackgroundSurface(TSurfaceConfiguration& aSurfaceConfig, TBool aTriggerRedraw)
	{
	return RunCommand(ESetBackgroundSurface, &aSurfaceConfig, aTriggerRedraw);
	}
	
TInt CWindowManager::RunCommand(TCommand aCommand, TAny* aParam1, TInt aParam2)
	{
	if(RThread().Id() == iCreatorThreadId)
		{
		return DoCommand(aCommand, aParam1, aParam2);
		}
	else
		{
		TMessage msg;
		msg.iCommand = aCommand;
		msg.iParam1 = aParam1;
		msg.iParam2 = aParam2;
		iMsgQueue.SendBlocking(msg);
		iSemaphore.Wait();
		return iReturnCode;
		}
	}

TInt CWindowManager::DoCommand(TCommand aCommand, TAny* aParam1, TInt aParam2)
	{
	switch(aCommand)
		{
	case ERegisterSurface:
		{
		TSurfaceId& surfaceId = *reinterpret_cast<TSurfaceId*>(aParam1);
		return iWsSession.RegisterSurface(iWsSd->GetScreenNumber(), surfaceId);
		}
	case ESetBackgroundSurface:
		{
		TSurfaceConfiguration& surfaceConfig = *reinterpret_cast<TSurfaceConfiguration*>(aParam1);
		TBool triggerRedraw = aParam2;
		return iWindow->SetBackgroundSurface(surfaceConfig, triggerRedraw);
		}
	default:
		{
		User::Invariant();
		return KErrNone;
		}
		}
	}

TSize CWindowManager::ScreenSize() const
	{
	return iScreenSize;
	}

void CWindowManager::Flush()
	{
	iWsSession.Flush();
	}
