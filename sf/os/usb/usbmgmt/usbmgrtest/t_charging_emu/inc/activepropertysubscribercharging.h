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

#ifndef ACTIVEPROPERTYSUBSCRIBERCHARGING_H
#define ACTIVEPROPERTYSUBSCRIBERCHARGING_H

#include <e32base.h>
#include <e32property.h>


NONSHARABLE_CLASS(MPropertyWriteRepositoryObserver)
	{
public:
	virtual void MpsoPropertyWriteChanged(const TInt aValue) = 0;
	};


NONSHARABLE_CLASS(CActivePropertyWriteRepository) : public CActive
	{
public:
	static CActivePropertyWriteRepository* NewL(MPropertyWriteRepositoryObserver& aObserver);
	~CActivePropertyWriteRepository();

public:
	void Request();

private:
	CActivePropertyWriteRepository(MPropertyWriteRepositoryObserver& aObserver);
	void ConstructL();

private: // from CActive
	void RunL();
	void DoCancel();

private: // utility
	void HandleCompletionL(TInt aError);

private: // unowned
	MPropertyWriteRepositoryObserver& iObserver;

private: // owned
	RProperty iPropertyWriteToRepository;	
	
	};
	
NONSHARABLE_CLASS(MPropertyReadBatteryChargingCurrentObserver)
	{
public:
	virtual void MpsoPropertyReadChanged() = 0;
	};


NONSHARABLE_CLASS(CActivePropertyReadChargingCurrent) : public CActive
	{
public:
	static CActivePropertyReadChargingCurrent* NewL(MPropertyReadBatteryChargingCurrentObserver& aObserver);
	~CActivePropertyReadChargingCurrent();

public:
	void Request();

private:
	CActivePropertyReadChargingCurrent(MPropertyReadBatteryChargingCurrentObserver& aObserver);
	void ConstructL();

private: // from CActive
	void RunL();
	void DoCancel();

private: // utility
	void HandleCompletionL(TInt aError);

private: // unowned
	MPropertyReadBatteryChargingCurrentObserver& iObserver;

private: // owned
	RProperty iPropertyReadChargingCurrent;	
	
	};

#endif // ACTIVEPROPERTYSUBSCRIBERCHARGING_H
