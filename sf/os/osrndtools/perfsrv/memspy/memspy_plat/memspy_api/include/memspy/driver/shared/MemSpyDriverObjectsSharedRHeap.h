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

#ifndef MEMSPYDRIVEROBJECTSSHAREDRHEAP_H
#define MEMSPYDRIVEROBJECTSSHAREDRHEAP_H

// System includes
#include <u32std.h>
#ifndef __KERNEL_MODE__
#include <e32debug.h>
#endif

// User includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// Defines
//#define __CHECK_CHUNK_NAME

// Classes referenced
class CActiveScheduler;
class CTrapCleanup;


/**
 * Base class for MemSpy RHeap statistics
 */
class TMemSpyHeapStatisticsRHeapBase
    {
public: // Type
    enum TType
        {
        ETypeAllocated = 0,
        ETypeFree
        };

public: // Constructors & destructor
    inline TMemSpyHeapStatisticsRHeapBase( TType aType )
        : iType( aType ), 
          iTypeCount( 0 ), 
          iTypeSize( 0 ),
          iLargestCellAddress( NULL ),
          iLargestCellSize( 0 )
        {
        }

public: // API
    inline TType Type() const { return iType; }
    //
    inline TUint TypeCount() const { return iTypeCount; }
    inline void SetTypeCount( TUint aValue ) { iTypeCount = aValue; }
    //
    inline TUint TypeSize() const { return iTypeSize; }
    inline void SetTypeSize( TUint aValue ) { iTypeSize = aValue; }
    //
    inline TAny* LargestCellAddress() const { return iLargestCellAddress; }
    inline void SetLargestCellAddress( TAny* aAddress ) { iLargestCellAddress = aAddress; }
    //
    inline TUint LargestCellSize() const { return iLargestCellSize; }
    inline void SetLargestCellSize( TUint aSize ) { iLargestCellSize = aSize; }

private: // Data members
    TType iType;
    TUint iTypeCount;
    TUint iTypeSize;
    TAny* iLargestCellAddress;
    TUint iLargestCellSize;
    };





/**
 * RHeap statistics for Allocated cell types
 */
class TMemSpyHeapStatisticsRHeapAllocated : public TMemSpyHeapStatisticsRHeapBase
    {
public: // Constructors
    inline TMemSpyHeapStatisticsRHeapAllocated()
        : TMemSpyHeapStatisticsRHeapBase( TMemSpyHeapStatisticsRHeapBase::ETypeAllocated )
        {
        }

public:
    };






/**
 * RHeap statistics for Free cell types
 */
class TMemSpyHeapStatisticsRHeapFree : public TMemSpyHeapStatisticsRHeapBase
    {
public: // Constructors
    inline TMemSpyHeapStatisticsRHeapFree()
        : TMemSpyHeapStatisticsRHeapBase( TMemSpyHeapStatisticsRHeapBase::ETypeFree ),
          iSlackSpace( 0 ), iSlackSpaceCellAddress( NULL ), iChecksum( 0 )
        {
        }

public: // API
    inline TUint SlackSpaceCellSize() const { return iSlackSpace; }
    inline void SetSlackSpaceCellSize( TUint aValue ) { iSlackSpace = aValue; }
    //
    inline TAny* SlackSpaceCellAddress() const { return iSlackSpaceCellAddress; }
    inline void SetSlackSpaceCellAddress( TAny* aAddress ) { iSlackSpaceCellAddress = aAddress; }
    //
    inline TUint32 Checksum() const { return iChecksum; }
    inline void SetChecksum( TUint32 aValue ) { iChecksum = aValue; }

public:
    TUint iSlackSpace;
    TAny* iSlackSpaceCellAddress;
    TUint32 iChecksum;
    };

/**
 * RHeap statistics class
 */
class TMemSpyHeapStatisticsRHeap
    {
public: // Constructors
    inline TMemSpyHeapStatisticsRHeap()
		: iCommittedFreeSpace(0)
        {
        }

public: // API
    inline TMemSpyHeapStatisticsRHeapFree& StatsFree() { return iStatisticsFree; }
    inline const TMemSpyHeapStatisticsRHeapFree& StatsFree() const { return iStatisticsFree; }
    //
    inline TMemSpyHeapStatisticsRHeapAllocated& StatsAllocated() { return iStatisticsAllocated; }
    inline const TMemSpyHeapStatisticsRHeapAllocated& StatsAllocated() const { return iStatisticsAllocated; }


private: // Data members
    TMemSpyHeapStatisticsRHeapFree iStatisticsFree;
    TMemSpyHeapStatisticsRHeapAllocated iStatisticsAllocated;

public: // I am fed up of all these pointless inline accessors...
	TInt iCommittedFreeSpace; // The amount of committed memory that isn't payload data in allocated or free cells
    };







/**
 *
 */
class TMemSpyHeapMetaDataRHeap
    {
public: // Constructors
    inline TMemSpyHeapMetaDataRHeap()
        : iChunkSize( 0 ),
          iChunkHandle( NULL ),
          iChunkBaseAddress( NULL ),
          iDebugAllocator( EFalse ),
          //iHeaderSizeFree( 0 ),
          //iHeaderSizeAllocated( 0 ),
          iIsUserThread( ETrue ),
		  iVTable(0),
          iSharedHeap( EFalse ),
          iHeapSize(0),
          iAllocatorAddress(NULL),
		  iMinHeapSize(0),
		  iMaxHeapSize(0)
        {
        }

public: // API
#ifndef __KERNEL_MODE__
    inline TPtrC ChunkName() const
        {
        TBool alreadyExpanded = ETrue;

#ifdef __CHECK_CHUNK_NAME
        {
        RDebug::Printf( "TMemSpyHeapMetaDataRHeap::ChunkName() - BEF - length: %d", iChunkName.Length() );
        const TUint8* data2 = iChunkName.Ptr();
        const TInt count = iChunkName.Length();
        for( TInt i=0;i<count;i++)
            {
            RDebug::Printf( "TMemSpyHeapMetaDataRHeap::ChunkName() - BEF - data[%03d] = 0x%08x = %02x (%c)", i, data2, *data2, *data2 );
            ++data2;
            }
        }
#endif

        // Check if it already has been converted to unicode
        const TUint8* data = iChunkName.Ptr();
        if ( iChunkName.Length() >= 1 )
            {
            const TUint8 highByte = *(data + 1);
            alreadyExpanded = ( highByte == 0 ); 
            }

        // Once expanded, the descriptor is twice the length
        TPtrC ret( (TUint16*) iChunkName.Ptr(), iChunkName.Length() / 2 );
        //
        if ( !alreadyExpanded )
            {
            const TInt halfLength = iChunkName.Length() / 2;
            TMemSpyHeapMetaDataRHeap* self = const_cast< TMemSpyHeapMetaDataRHeap* >( this );
            ret.Set( self->iChunkName.Expand() );
            }

#ifdef __CHECK_CHUNK_NAME
        {
        RDebug::Printf( "TMemSpyHeapMetaDataRHeap::ChunkName() - AFT - length: %d", iChunkName.Length() );
        const TUint8* data2 = iChunkName.Ptr();
        const TInt count = iChunkName.Length();
        for( TInt i=0;i<count;i++)
            {
            RDebug::Printf( "TMemSpyHeapMetaDataRHeap::ChunkName() - AFT - data[%03d] = 0x%08x = %02x (%c)", i, data2, *data2, *data2 );
            ++data2;
            }
        const TPtrC pNextRet( (TUint16*) iChunkName.Ptr(), iChunkName.Length() / 2 );
        RDebug::Print( _L("TMemSpyHeapMetaDataRHeap::ChunkName() - ret: [%S], nextRet: [%S]"), &ret, &pNextRet );
        }
#endif
        //
        return ret;
        }

    inline void SetChunkName( const TDesC16& aValue )
        {
        iChunkName.Copy( aValue );
        iChunkName.Collapse();
        }
#else
    inline void SetChunkName( const TDesC8& aValue )
        {
        iChunkName.Copy( aValue );
        }
#endif
    //
    inline TUint ChunkSize() const { return iChunkSize; }
    inline void SetChunkSize( TUint aValue ) { iChunkSize = aValue; }
    //
    inline TAny* ChunkHandle() const { return iChunkHandle; }
    inline void SetChunkHandle( TAny* aValue ) { iChunkHandle = aValue; }
    //
    inline TAny* ChunkBaseAddress() const { return iChunkBaseAddress; }
    inline void SetChunkBaseAddress( TAny* aValue ) { iChunkBaseAddress = aValue; }
    //
    inline TBool IsDebugAllocator() const { return iDebugAllocator; }
    inline void SetDebugAllocator( TBool aValue ) { iDebugAllocator = aValue; }
    //
    inline TBool IsUserThread() const { return iIsUserThread; }
    inline void SetUserThread( TBool aValue ) { iIsUserThread = aValue; }
    //
    inline TBool IsSharedHeap() const { return iSharedHeap; }
    inline void SetSharedHeap( TBool aValue ) { iSharedHeap = aValue; }
    //
    inline TUint VTable() const { return iVTable; }
    inline void SetVTable( TUint aValue ) { iVTable = aValue; }

private: // Data members
    TBuf8< KMaxFullName * 2 > iChunkName;
public:
    TUint iChunkSize;
    TAny* iChunkHandle;
    TAny* iChunkBaseAddress;
    TBool iDebugAllocator;
    TBool iSharedHeap;
    TBool iIsUserThread;
    TUint iVTable;
	TUint iHeapSize; // Committed size - generally the same as iChunkSize (except maybe for kernel heap)
	TAny* iAllocatorAddress; // replacement for things using the RHeap base address
	TUint iMinHeapSize; // Minimum committed size
	TUint iMaxHeapSize; // Max committed size
    };


/**
 *
 */
class TMemSpyHeapInfoRHeap
    {
public: // Constructors
    inline TMemSpyHeapInfoRHeap()
        {
        }

public: // API
    inline TMemSpyHeapMetaDataRHeap& MetaData() { return iMetaData; }
    inline const TMemSpyHeapMetaDataRHeap& MetaData() const { return iMetaData; }
    //
    //inline TMemSpyHeapObjectDataRHeap& ObjectData() { return iObjectData; }
    //inline const TMemSpyHeapObjectDataRHeap& ObjectData() const { return iObjectData; }
    //
    inline TMemSpyHeapStatisticsRHeap& Statistics() { return iStatistics; }
    inline const TMemSpyHeapStatisticsRHeap& Statistics() const { return iStatistics; }

	inline TInt Overhead() const { return iMetaData.iHeapSize - iStatistics.StatsAllocated().TypeSize() - iStatistics.iCommittedFreeSpace; }

private: // Data members
    TMemSpyHeapMetaDataRHeap iMetaData;
    //TMemSpyHeapObjectDataRHeap iObjectData;
    TMemSpyHeapStatisticsRHeap iStatistics;
    };





#endif
