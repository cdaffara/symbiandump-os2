// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : signalshutdown.cpp
// Part of     : Signal Server
//



#include "signalshutdown.h"

static const TInt KShutdownDelay = 200000;

CSigShutDown::CSigShutDown() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

CSigShutDown::~CSigShutDown()
	{
	}

CSigShutDown* CSigShutDown::NewL()
	{
	CSigShutDown* self = new (ELeave) CSigShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSigShutDown::ConstructL()
	{
	CTimer::ConstructL();
	}

void CSigShutDown::Start()
	{
	After(KShutdownDelay);
	}

void CSigShutDown::RunL()
	{
	CActiveScheduler::Stop();
	}
