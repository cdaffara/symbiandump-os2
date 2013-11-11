// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "TestManagerObserverStub.h"

// ______________________________________________________________________________
//
CTestManagerObserverStub* CTestManagerObserverStub::NewL()
	{
	CTestManagerObserverStub* self= NewLC();  // calls c'tor and ConstructL
	CleanupStack::Pop();				// removes self
	return self;
	}

CTestManagerObserverStub* CTestManagerObserverStub::NewLC()
	{
	CTestManagerObserverStub* self=new(ELeave) CTestManagerObserverStub();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL();	// Complete the 'construction'.
	return self;
	}

CTestManagerObserverStub::~CTestManagerObserverStub()
	{
	// Do nothing
	}

CTestManagerObserverStub::CTestManagerObserverStub()
: CBase()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CTestManagerObserverStub::ConstructL()
	{
	// Do nothing
	}

void CTestManagerObserverStub::TestsComplete()
	{
	if(iSchedulerStarted)
		{
		CActiveScheduler::Stop();
		}
	}

void CTestManagerObserverStub::SetSchedulerStarted(TBool aSchedulerStarted)
	{
	iSchedulerStarted = aSchedulerStarted;
	}
