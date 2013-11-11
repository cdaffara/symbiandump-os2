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

#ifndef MEMSPYDRIVERHEAP_H
#define MEMSPYDRIVERHEAP_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverObjectsInternal.h"
#include "MemSpyDriverOSAdaption.h"

// Constants
// We shouldn't be using any of these any more! -Tomsci
const TUint KRHeapObjectSize = 0x74;
const TUint KRAllocatorAndRHeapMemberDataOffset = 4; // 4 bytes past start of allocator address, i.e. skipping the vtable
const TUint KRHeapMemberDataSize = KRHeapObjectSize - KRAllocatorAndRHeapMemberDataOffset;

// Classes referenced
namespace LtkUtils
	{
	class RAllocatorHelper;
	}

/**
 * Essentially a mirror of RAllocator and RHeap's layout.
 */
NONSHARABLE_CLASS(RMemSpyDriverRHeapBase)
	{
protected:
    RMemSpyDriverRHeapBase();

public: // API
    void PrintInfo();
	LtkUtils::RAllocatorHelper* Helper();
	TMemSpyHeapInfo::THeapImplementationType GetTypeFromHelper() const;

public: // Virtual API
    virtual void Reset();
	virtual void Close();
    virtual DChunk& Chunk() = 0;
    virtual const DChunk& Chunk() const = 0;
    virtual const TLinAddr ChunkBase() const = 0;

protected:
	LtkUtils::RAllocatorHelper* iHelper;
    };

NONSHARABLE_CLASS(RMemSpyDriverRHeapUser) : public RMemSpyDriverRHeapBase
	{
public:
    RMemSpyDriverRHeapUser( DMemSpyDriverOSAdaption& aOSAdaption );
	TInt OpenUserHeap(DThread& aThread, TBool aEuserUdeb);

	DChunk& Chunk() { return *iChunk; }
	const DChunk& Chunk() const { return *iChunk; }
	const TLinAddr ChunkBase() const { return (TLinAddr)OSAdaption().DChunk().GetBase(*iChunk); } 

private:
    inline DMemSpyDriverOSAdaption& OSAdaption() { return iOSAdaption; }
    inline const DMemSpyDriverOSAdaption& OSAdaption() const { return iOSAdaption; } 

private:
    DMemSpyDriverOSAdaption& iOSAdaption;
	DChunk* iChunk;
    };


NONSHARABLE_CLASS(RMemSpyDriverRHeapKernelFromCopy) : public RMemSpyDriverRHeapBase
    {
public:
    RMemSpyDriverRHeapKernelFromCopy( DMemSpyDriverOSAdaption& aOSAdaption );
    
public: // API
    TInt AssociateWithKernelChunk( DChunk* aKernelChunk, DChunk* aCopiedChunk, TLinAddr aCopiedChunkBase, TInt aOffset );
    TBool IsOpen();

public: // From RMemSpyDriverRHeapBase
    void Reset();
    void Close();
    DChunk& Chunk();
    const DChunk& Chunk() const;
    const TLinAddr ChunkBase() const { return iChunkBase; } 

private:
    inline DMemSpyDriverOSAdaption& OSAdaption() { return iOSAdaption; }
    inline const DMemSpyDriverOSAdaption& OSAdaption() const { return iOSAdaption; }
    
private:
    DMemSpyDriverOSAdaption& iOSAdaption;

    // Copy of the client's heap data
    DChunk* iChunk;
    TLinAddr iChunkBase;
    };


NONSHARABLE_CLASS(RMemSpyDriverRHeapKernelInPlace) : public RMemSpyDriverRHeapBase
    {
public:
    RMemSpyDriverRHeapKernelInPlace( DMemSpyDriverOSAdaption& aOSAdaption );
	TInt OpenKernelHeap();
    

public: // From RMemSpyDriverRHeapBase
    void Close();

    DChunk& Chunk();
    const DChunk& Chunk() const;
    const TLinAddr ChunkBase() const { return (TLinAddr)OSAdaption().DChunk().GetBase(*iChunk); } 

private:
    inline DMemSpyDriverOSAdaption& OSAdaption() { return iOSAdaption; }
    inline const DMemSpyDriverOSAdaption& OSAdaption() const { return iOSAdaption; }

private:
    DMemSpyDriverOSAdaption& iOSAdaption;

    // Only important member data is the base class's RAllocatorHelper
    // We do cache the chunk though
	DChunk* iChunk;
    };

	
#endif
