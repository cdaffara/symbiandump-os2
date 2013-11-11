// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of test active object for log engine testing 
// Identical implementation exists in common\syslibs\logeng\ongoing\test\src\test.cpp
// 
//

/**
 @file
 @internalComponent
*/

#include "testactive.h"

CTestActive::CTestActive(TInt aPriority)
:	CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	iDelayTime=0;
	}

CTestActive::~CTestActive()
	{
	Cancel();
	}

void CTestActive::DoCancel()
	{
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);
	}

void CTestActive::Start()
	{
	iDelayCompletion=EFalse;
	iDelayTime=0;
	iStatus = KRequestPending;
	SetActive();
	}

void CTestActive::Start(TInt aDelay)
	{
	iDelayCompletion=ETrue;
	iDelayTime=aDelay;
	iStatus = KRequestPending;
	SetActive();
	}

void CTestActive::RunL() 
	{
	if(iDelayCompletion && iDelayTime)
		{
		// Wait for events in other threads to have a go....
		User::After(iDelayTime);
		iDelayTime=0;
		iStoredStatus=iStatus;
		SetActive();
		TRequestStatus* s=&iStatus;
		User::RequestComplete(s, KErrNone);
		}
	else
		{
		if(iDelayCompletion)
			iStatus=iStoredStatus;

		CActiveScheduler::Stop();
		}
	}
