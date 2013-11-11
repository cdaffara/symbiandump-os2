/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <memspy/api/memspyapimemorytrackingcycle.h>
#include <memspy/engine/memspymemorytrackingcycledata.h>

EXPORT_C TInt CMemSpyApiMemoryTrackingCycle::CycleNumber() const
	{
	return iData->iCycleNumber;	
	}

EXPORT_C const TTime& CMemSpyApiMemoryTrackingCycle::Time() const
	{
	return iData->iTime;	
	}

EXPORT_C const TDesC& CMemSpyApiMemoryTrackingCycle::Caption() const
	{
	return iData->iCaption;	
	}

EXPORT_C const TInt64& CMemSpyApiMemoryTrackingCycle::FreeMemory() const
	{
	return iData->iFreeMemory;
	}

EXPORT_C TInt64 CMemSpyApiMemoryTrackingCycle::MemoryDelta() const
	{
	return iData->iMemoryDelta;
	}

EXPORT_C TInt64 CMemSpyApiMemoryTrackingCycle::PreviousCycleDiff() const
	{
	return iData->iPreviousCycleDiff;
	}

EXPORT_C TInt CMemSpyApiMemoryTrackingCycle::ChangeCount() const
	{
	return iData->iChangeCount;
	}

EXPORT_C CMemSpyApiMemoryTrackingCycle::~CMemSpyApiMemoryTrackingCycle()
	{
	delete iData;
	}

CMemSpyApiMemoryTrackingCycle* CMemSpyApiMemoryTrackingCycle::NewL(const TMemSpyMemoryTrackingCycleData& aData)
	{
	CMemSpyApiMemoryTrackingCycle* self = CMemSpyApiMemoryTrackingCycle::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiMemoryTrackingCycle* CMemSpyApiMemoryTrackingCycle::NewLC(const TMemSpyMemoryTrackingCycleData& aData)
	{
	CMemSpyApiMemoryTrackingCycle* self = new (ELeave) CMemSpyApiMemoryTrackingCycle;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return self;
	}

CMemSpyApiMemoryTrackingCycle::CMemSpyApiMemoryTrackingCycle()
	{
	}

void CMemSpyApiMemoryTrackingCycle::ConstructL(const TMemSpyMemoryTrackingCycleData& aData)
	{
	iData = new (ELeave) TMemSpyMemoryTrackingCycleData(aData);
	}
