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

#include "MemSpyDriverHeap.h"

// System includes
#include <kern_priv.h>

// User includes
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverUtils.h"
#include "heaputils.h"



RMemSpyDriverRHeapBase::RMemSpyDriverRHeapBase()
	: iHelper(NULL)
    {
    Reset();
    }

LtkUtils::RAllocatorHelper* RMemSpyDriverRHeapBase::Helper()
	{
	return iHelper;
	}

TMemSpyHeapInfo::THeapImplementationType RMemSpyDriverRHeapBase::GetTypeFromHelper() const
	{
	if (iHelper)
		{
		LtkUtils::RAllocatorHelper::TType type = iHelper->GetType();
		switch (type)
			{
			case LtkUtils::RAllocatorHelper::ETypeRHeap:
				return TMemSpyHeapInfo::ETypeRHeap;
			case LtkUtils::RAllocatorHelper::ETypeRHybridHeap:
				return TMemSpyHeapInfo::ETypeRHybridHeap;
			case LtkUtils::RAllocatorHelper::ETypeUnknown:
			default:
				return TMemSpyHeapInfo::ETypeUnknown;
			}
		}
	return TMemSpyHeapInfo::ETypeUnknown;
	}

void RMemSpyDriverRHeapBase::Reset()
    {
	Close();
	}

void RMemSpyDriverRHeapBase::Close()
	{
	if (iHelper)
		{
	    NKern::ThreadEnterCS();
		iHelper->Close();
		delete iHelper;
		iHelper = NULL;
		NKern::ThreadLeaveCS();
		}
    }

void RMemSpyDriverRHeapBase::PrintInfo()
    {
	/* TOMSCI TODO replace with tracing based on latest struct info. See DMemSpyDriverLogChanHeapBase::PrintHeapInfo
	 * Alternatively just call DMemSpyDriverLogChanHeapBase::PrintHeapInfo() somehow?
	 
#if defined(TRACE_TYPE_KERNELHEAP) || defined(TRACE_TYPE_USERHEAP)
    Kern::Printf(" " );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RAllocator - iAccessCount:    0x%08x", iAccessCount );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RAllocator - iHandleCount:    0x%08x", iHandleCount );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RAllocator - iHandles:        0x%08x", iHandles );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RAllocator - iFlags:          0x%08x", iFlags );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RAllocator - iCellCount:      0x%08x", iCellCount );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RAllocator - iTotalAllocSize: 0x%08x", iTotalAllocSize );

    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iMinLength:      0x%08x", iMinLength );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iMaxLength:      0x%08x", iMaxLength );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iOffset:         0x%08x", iOffset);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iGrowBy:         0x%08x", iGrowBy);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iChunkHandle:    0x%08x", iChunkHandle);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iBase:           0x%08x", Base());
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iTop:            0x%08x", iTop );
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iAlign:          0x%08x", iAlign);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iMinCell:        0x%08x", iMinCell);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iPageSize:       0x%08x", iPageSize);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iFree len:       0x%08x", iFree.len);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iFree next:      0x%08x", iFree.next);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iNestingLevel:   0x%08x", iNestingLevel);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      iAllocCount:     0x%08x", iAllocCount);
    Kern::Printf("RMemSpyDriverRHeapBase::PrintInfo - RHeap -      size:              %8d",  Size() );
    Kern::Printf(" " );
    Kern::Printf(" " );
#endif
	*/
    }




RMemSpyDriverRHeapUser::RMemSpyDriverRHeapUser( DMemSpyDriverOSAdaption& aOSAdaption )
    : RMemSpyDriverRHeapBase(), iOSAdaption(aOSAdaption)
    {
    }


TInt RMemSpyDriverRHeapUser::OpenUserHeap(DThread& aThread, TBool aEuserUdeb)
    {
    TLinAddr allocatorAddr = (TLinAddr)OSAdaption().DThread().GetAllocator(aThread);
    NKern::ThreadEnterCS();
    LtkUtils::RUserAllocatorHelper* helper = new LtkUtils::RUserAllocatorHelper;
    if (!helper)
        {
        NKern::ThreadLeaveCS();
        return KErrNoMemory;
        }
    TInt err = helper->OpenUserHeap(OSAdaption().DThread().GetId(aThread), allocatorAddr, aEuserUdeb);
    if (!err)
        {
        iChunk = helper->OpenUnderlyingChunk();
        if (!iChunk) err = KErrNotFound;
        }
    if (err)
        {
        delete helper;
        }
    else
        {
        iHelper = helper;
        }
    NKern::ThreadLeaveCS();
    return err;
    }


RMemSpyDriverRHeapKernelFromCopy::RMemSpyDriverRHeapKernelFromCopy( DMemSpyDriverOSAdaption& aOSAdaption )
:   iOSAdaption( aOSAdaption ), iChunk( NULL )
    {
    }


void RMemSpyDriverRHeapKernelFromCopy::Reset()
    {
    RMemSpyDriverRHeapBase::Reset();
	//
    iChunk = NULL;
    }


TInt RMemSpyDriverRHeapKernelFromCopy::AssociateWithKernelChunk( DChunk* aKernelChunk, DChunk* aCopiedChunk, TLinAddr aCopiedChunkBase, TInt aOffset )
    {
    TRACE_KH( Kern::Printf("RMemSpyDriverRHeapKernelFromCopy::AssociateWithKernelChunk() - START - aChunk: 0x%08x, aOffset: %d", 
                            aCopiedChunk, aOffset) );

    iChunk = aCopiedChunk;
    iChunkBase = aCopiedChunkBase;

    NKern::ThreadEnterCS();
    TInt ret = KErrNone;
    LtkUtils::RKernelCopyAllocatorHelper* helper = new LtkUtils::RKernelCopyAllocatorHelper();
    if (helper)
        {
        helper->OpenCopiedHeap(aKernelChunk, aCopiedChunk, aOffset);
        iHelper = helper;        
        }
    else
        {
        ret = KErrNoMemory;
        }
    NKern::ThreadLeaveCS();
    
    TRACE_KH( Kern::Printf("RMemSpyDriverRHeapKernelFromCopy::AssociateWithKernelChunk() - END") );
    return ret;
    }

DChunk& RMemSpyDriverRHeapKernelFromCopy::Chunk()
    {
    return *iChunk;
    }


const DChunk& RMemSpyDriverRHeapKernelFromCopy::Chunk() const
    {
    return *iChunk;
    }

void RMemSpyDriverRHeapKernelFromCopy::Close()
	{
    if  ( iChunk != NULL )
        {
        NKern::ThreadEnterCS();
        Kern::ChunkClose( iChunk );
        iChunk = NULL;
        NKern::ThreadLeaveCS();
        }
    RMemSpyDriverRHeapBase::Close();
	}

TBool RMemSpyDriverRHeapKernelFromCopy::IsOpen()
    {
    return (iChunk != NULL);
    }

RMemSpyDriverRHeapKernelInPlace::RMemSpyDriverRHeapKernelInPlace( DMemSpyDriverOSAdaption& aOSAdaption )
:   iOSAdaption( aOSAdaption ), iChunk( NULL )
    {
    }

TInt RMemSpyDriverRHeapKernelInPlace::OpenKernelHeap()
	{
	NKern::ThreadEnterCS();
	LtkUtils::RAllocatorHelper* helper = new LtkUtils::RAllocatorHelper;
	if (!helper)
		{
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}
	TInt err = helper->OpenKernelHeap();
	if (!err)
		{
		iChunk = helper->OpenUnderlyingChunk();
		if (!iChunk) err = KErrNotFound;
		}

	if (err)
		{
		delete helper;
		}
	else
		{
		iHelper = helper;
		}
	NKern::ThreadLeaveCS();
	return err;
	}

void RMemSpyDriverRHeapKernelInPlace::Close()
    {
	NKern::ThreadEnterCS();
	iChunk->Close(NULL);
	iChunk = NULL;
	RMemSpyDriverRHeapBase::Close();
	NKern::ThreadLeaveCS();
    }

DChunk& RMemSpyDriverRHeapKernelInPlace::Chunk()
    {
    return *iChunk;
    }


const DChunk& RMemSpyDriverRHeapKernelInPlace::Chunk() const
    {
    return *iChunk;
    }

