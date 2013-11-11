// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include "uloggerwatcher.h"

namespace Ulogger
{

/*
	NewL method
*/
CULoggerWatcher* CULoggerWatcher::NewL()
	{
	CULoggerWatcher* self = CULoggerWatcher::NewLC();
	CleanupStack::Pop(); //self
	return self;
	}

/*
	NewLC method
*/
CULoggerWatcher* CULoggerWatcher::NewLC()
	{
	CULoggerWatcher* self = new (ELeave) CULoggerWatcher();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*
	default constructor method
*/	
CULoggerWatcher::CULoggerWatcher()
: CActive(CActive::EPriorityUserInput)
	{
	}

/*
	default destructor method
*/

CULoggerWatcher::~CULoggerWatcher()
	{
	Cancel();
	}

/*
	ConstructL method
*/
void CULoggerWatcher::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/*
	DoCancel method
*/	

void CULoggerWatcher::DoCancel()
	{		
	}
	
/*
	RunL method
*/
	
void CULoggerWatcher::RunL()
	{
	if(iStatus.Int() == KErrNone)
		iObserver->DataNotification();	
	}
	
/*
	StartWatching method
	@aObserver
*/
	
void CULoggerWatcher::StartWatching(MDataWatcherObserver* aObserver)
	{
	if(!IsActive())
	{
		iObserver = aObserver;
		SetActive();
		
	}	
	}
	
}//Namespace
