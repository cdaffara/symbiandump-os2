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

/**
 @file
 @internalComponent
 @prototype
*/

#include "activepropertysubscribercharging.h"
#include"tbatterychargingdefinitions.h"

CActivePropertyWriteRepository::CActivePropertyWriteRepository(MPropertyWriteRepositoryObserver& aObserver)
:	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

CActivePropertyWriteRepository::~CActivePropertyWriteRepository()
	{
	
	Cancel();
	iPropertyWriteToRepository.Close();
	}

CActivePropertyWriteRepository* CActivePropertyWriteRepository::NewL(MPropertyWriteRepositoryObserver& aObserver)
	{
	
	CActivePropertyWriteRepository* self = new(ELeave) CActivePropertyWriteRepository(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CActivePropertyWriteRepository::ConstructL()
	{
	// Assumes the property has already been defined by the driving test code.
	User::LeaveIfError(iPropertyWriteToRepository.Attach(TUid::Uid(KBattChargWriteRepositoryUid), KBattChargWriteRepositoryKey));
	}

void CActivePropertyWriteRepository::Request()
	{
	iPropertyWriteToRepository.Subscribe(iStatus);
	SetActive();
	}

void CActivePropertyWriteRepository::RunL()
	{
	
	if ( iStatus.Int() == KErrNone )
		{
		TInt value;
		TInt err = iPropertyWriteToRepository.Get(value);
		ASSERT(!err);
		iObserver.MpsoPropertyWriteChanged(value);
		}

	Request();
	}

void CActivePropertyWriteRepository::DoCancel()
	{
	
	iPropertyWriteToRepository.Cancel();
	}


// this is to read the property for current charging
CActivePropertyReadChargingCurrent::CActivePropertyReadChargingCurrent(MPropertyReadBatteryChargingCurrentObserver& aObserver)
:	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

CActivePropertyReadChargingCurrent::~CActivePropertyReadChargingCurrent()
	{
	
	Cancel();
	iPropertyReadChargingCurrent.Close();
	}

CActivePropertyReadChargingCurrent* CActivePropertyReadChargingCurrent::NewL(MPropertyReadBatteryChargingCurrentObserver& aObserver)
	{
	
	CActivePropertyReadChargingCurrent* self = new(ELeave) CActivePropertyReadChargingCurrent(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CActivePropertyReadChargingCurrent::ConstructL()
	{
	// Assumes the property has already been defined by the driving test code.
	User::LeaveIfError(iPropertyReadChargingCurrent.Attach(TUid::Uid(KBattChargReadPropertyCurrentUid), KBattChargReadCurrentChargingKey));
	}

void CActivePropertyReadChargingCurrent::Request()
	{
	iPropertyReadChargingCurrent.Subscribe(iStatus);
	SetActive();
	}

void CActivePropertyReadChargingCurrent::RunL()
	{
	
	if ( iStatus.Int() == KErrNone )
		{
		//not interested in the value of the property but just by the fact that it was triggered
		iObserver.MpsoPropertyReadChanged();
		}

	Request();
	}

void CActivePropertyReadChargingCurrent::DoCancel()
	{
	
	iPropertyReadChargingCurrent.Cancel();
	}





