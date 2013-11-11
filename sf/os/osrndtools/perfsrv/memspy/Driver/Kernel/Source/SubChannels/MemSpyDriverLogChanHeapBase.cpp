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

#include "MemSpyDriverLogChanHeapBase.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"

// Constants
const TInt KMemSpyDriverLogChanHeapBaseXferBufferSize = 1024 * 16;


DMemSpyDriverLogChanHeapBase::DMemSpyDriverLogChanHeapBase( DMemSpyDriverDevice& aDevice, DThread& aThread )
:	DMemSpyDriverLogChanBase( aDevice, aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::DMemSpyDriverLogChanHeapBase() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanHeapBase::~DMemSpyDriverLogChanHeapBase()
	{
	}


TInt DMemSpyDriverLogChanHeapBase::Construct()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::Construct() - START - this: 0x%08x", this ));
    
    const TInt ret = BaseConstruct( KMemSpyDriverLogChanHeapBaseXferBufferSize );

	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::Construct() - END - this: 0x%08x, err: %d", this, ret ));
    return ret;
	}











TInt DMemSpyDriverLogChanHeapBase::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	const TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    return r;
	}













DMemSpyDriverLogChanHeapBase::TDrmMatchType DMemSpyDriverLogChanHeapBase::IsDrmThread( DThread& aThread )
    {
    TDrmMatchType ret = EMatchTypeNone;
    //
    const TUid procUid = aThread.iOwningProcess->iUids.iUid[ 2 ];
    TRACE( Kern::Printf( "DMemSpyDriverLogChanHeapBase::IsDrmThread() - START - aThread: %O, process uid: 0x%08x", &aThread, procUid.iUid ));

    // Some more rudimentary checks based upon process name and
    // known uids.
    TFullName fullName;
    aThread.FullName( fullName );

    // Exclude threads containing "DRM"
    _LIT( KDrmThreadMatchText, "*DRM*" );
    const TInt matchPos = fullName.MatchF( KDrmThreadMatchText );
    if ( matchPos >= 0 )
        {
        TRACE( Kern::Printf( "DMemSpyDriverLogChanHeapBase::IsDrmThread() - found \'DRM\' at pos: %d (%S)", matchPos, &fullName ));
        ret = EMatchTypeName;
        }
    else
        {
        // Some known DRM related process UIDs
        switch( procUid.iUid )
            {
        case 0x10005A22: // DRMEncryptor.exe
        case 0x01105901: // DRMEncryptor.exe
        case 0x101F85C7: // DRMRightsManager.exe
        case 0x10205CA8: // DcfRepSrv.exe
        case 0x101F51F2: // RightsServer.exe
        case 0x101F6DC5: // DRMHelperServer.exe
        case 0x10282F1B: // wmdrmserver.exe
            ret = EMatchTypeUid;
            break;
        default:
            ret = EMatchTypeNone;
            break;
            }
        }

    TRACE( Kern::Printf( "DMemSpyDriverLogChanHeapBase::IsDrmThread() - END - procUid: 0x%08x, matchType: %d", procUid.iUid, ret ));
    return ret;
    }





































void DMemSpyDriverLogChanHeapBase::PrintHeapInfo( const TMemSpyHeapInfo& aInfo )
    {
    const TMemSpyHeapInfoRHeap& rHeapInfo = aInfo.AsRHeap();
    //const TMemSpyHeapObjectDataRHeap& rHeapObjectData = rHeapInfo.ObjectData();
    const TMemSpyHeapStatisticsRHeap& rHeapStats = rHeapInfo.Statistics();
    const TMemSpyHeapMetaDataRHeap& rHeapMetaData = rHeapInfo.MetaData();

    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() ---------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - Stats (Free)                                    -" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() ---------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell count:                     %d", rHeapStats.StatsFree().TypeCount() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell size:                      %d", rHeapStats.StatsFree().TypeSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell largest:                   0x%08x", rHeapStats.StatsFree().LargestCellAddress() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell largest size:              %d", rHeapStats.StatsFree().LargestCellSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - slack:                          0x%08x", rHeapStats.StatsFree().SlackSpaceCellAddress() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - slack size:                     %d", rHeapStats.StatsFree().SlackSpaceCellSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - checksum:                       0x%08x", rHeapStats.StatsFree().Checksum() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - " ) );

    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() ---------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - Stats (Alloc)                                   -" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() ---------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell count:                     %d", rHeapStats.StatsAllocated().TypeCount() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell size:                      %d", rHeapStats.StatsAllocated().TypeSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell largest:                   0x%08x", rHeapStats.StatsAllocated().LargestCellAddress() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell largest size:              %d", rHeapStats.StatsAllocated().LargestCellSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - " ) );

    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() ---------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - Misc. Info                                      -" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() ---------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - chunk size:                     %d", rHeapMetaData.ChunkSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - chunk handle:                   0x%08x", rHeapMetaData.ChunkHandle() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - chunk base address:             0x%08x", rHeapMetaData.ChunkBaseAddress() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - debug allocator:                %d", rHeapMetaData.IsDebugAllocator() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - shared heap:                    %d", rHeapMetaData.IsSharedHeap() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - user thread:                    %d", rHeapMetaData.IsUserThread() ) );
    //TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell header size (free):        %d", rHeapMetaData.HeaderSizeFree() ) );
    //TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - cell header size (alloc):       %d", rHeapMetaData.HeaderSizeAllocated() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - heap vTable:                    0x%08x", rHeapMetaData.VTable() ) );
    //TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - heap object size:               %d", rHeapMetaData.ClassSize() ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - heap size:                      %d", rHeapMetaData.iHeapSize ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - allocator address:              0x%08x", rHeapMetaData.iAllocatorAddress ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - min heap size:                  %d", rHeapMetaData.iMinHeapSize ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapBase::PrintHeapInfo() - max heap size:                  %d", rHeapMetaData.iMaxHeapSize ) );
    }

// TODO remove this? how is it actually being used?
TBool DMemSpyDriverLogChanHeapBase::HandleHeapCell(TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt /*aNestingLevel*/, TInt /*aAllocNumber*/)
    {
    TInt error = KErrNone;
    //
    if  (aCellType & EMemSpyDriverFreeCellMask)
        {
        if  ( iStackStream )
            {
            if  ( !iStackStream->IsFull() )
                {
                ++iFreeCellCount;
                TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapBase::HandleHeapCell - writing free cell %d @ 0x%08x, space left: %u", iFreeCellCount, aCellAddress, iStackStream->Remaining() ));
                //
                iStackStream->WriteInt32( aCellType );
                iStackStream->WriteUint32( reinterpret_cast<TUint32>( aCellAddress ) );
                iStackStream->WriteInt32( aLength );
                }
            else
                {
                Kern::Printf( "DMemSpyDriverLogChanHeapBase::HandleHeapCell - Kernel Free Cell stack stream IS FULL!" );
                error = KErrAbort;
                }
            }
       }
    //
    return ( error == KErrNone );
    }


void DMemSpyDriverLogChanHeapBase::HandleHeapWalkInit()
	{
	iFreeCellCount = 0;
	}




