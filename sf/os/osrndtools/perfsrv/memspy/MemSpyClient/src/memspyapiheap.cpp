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

#include <memspy/api/memspyapiheap.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyheapdata.h>


CMemSpyApiHeap::CMemSpyApiHeap() 
	: iHeapData(0)
	{
	}

EXPORT_C CMemSpyApiHeap::~CMemSpyApiHeap()
	{
	delete iHeapData;
	}

void CMemSpyApiHeap::ConstructL(const TMemSpyHeapData& aData)
	{
	iHeapData = new (ELeave) TMemSpyHeapData(aData);
	}

CMemSpyApiHeap* CMemSpyApiHeap::NewL(const TMemSpyHeapData& aData)
	{
	CMemSpyApiHeap* self = CMemSpyApiHeap::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiHeap* CMemSpyApiHeap::NewLC(const TMemSpyHeapData& aData)
	{
	CMemSpyApiHeap* self = new (ELeave) CMemSpyApiHeap;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}

//api methods
EXPORT_C TDesC& CMemSpyApiHeap::Type()
	{
	return iHeapData->iType;
	}

EXPORT_C TInt CMemSpyApiHeap::Size()
	{
	return iHeapData->iSize;
	}

EXPORT_C TInt CMemSpyApiHeap::BaseAddress()
	{
	return iHeapData->iBaseAddress;
	}

EXPORT_C TBool CMemSpyApiHeap::Shared()
	{
	return iHeapData->iShared;
	}

EXPORT_C TInt CMemSpyApiHeap::ChunkSize()
	{
	return iHeapData->iChunkSize;
	}

EXPORT_C TInt CMemSpyApiHeap::AllocationsCount()
	{
	return iHeapData->iAllocationsCount;
	}

EXPORT_C TInt CMemSpyApiHeap::FreeCount()
	{
	return iHeapData->iFreeCount;
	}

EXPORT_C TInt CMemSpyApiHeap::BiggestAllocation()
	{
	return iHeapData->iBiggestAllocation;
	}

EXPORT_C TInt CMemSpyApiHeap::BiggestFree()
	{
	return iHeapData->iBiggestFree;
	}

EXPORT_C TInt CMemSpyApiHeap::TotalAllocations()
	{
	return iHeapData->iTotalAllocations;
	}

EXPORT_C TInt CMemSpyApiHeap::TotalFree()
	{
	return iHeapData->iTotalFree;
	}

EXPORT_C TInt CMemSpyApiHeap::SlackFreeSpace()
	{
	return iHeapData->iSlackFreeSpace;
	}

EXPORT_C TInt CMemSpyApiHeap::Fragmentation()
	{
	return iHeapData->iFragmentation;
	}

EXPORT_C TInt CMemSpyApiHeap::HeaderSizeA()
	{
	return iHeapData->iHeaderSizeA;
	}

EXPORT_C TInt CMemSpyApiHeap::HeaderSizeF()
	{
	return iHeapData->iHeaderSizeF;
	}

EXPORT_C TInt CMemSpyApiHeap::AllocationOverhead()
	{
	return iHeapData->iAllocationOverhead;
	}

EXPORT_C TInt CMemSpyApiHeap::FreeOverhead()
	{
	return iHeapData->iFreeOverhead;
	}

EXPORT_C TInt CMemSpyApiHeap::TotalOverhead()
	{
	return iHeapData->iTotalOverhead;
	}

EXPORT_C TInt CMemSpyApiHeap::Overhead()
	{
	return iHeapData->iOverhead;
	}

EXPORT_C TInt CMemSpyApiHeap::MinLength()
	{
	return iHeapData->iMinLength;
	}

EXPORT_C TInt CMemSpyApiHeap::MaxLength()
	{
	return iHeapData->iMaxLength;
	}

EXPORT_C TBool CMemSpyApiHeap::DebugAllocatorLibrary()
	{
	return iHeapData->iDebugAllocatorLibrary;
	}
