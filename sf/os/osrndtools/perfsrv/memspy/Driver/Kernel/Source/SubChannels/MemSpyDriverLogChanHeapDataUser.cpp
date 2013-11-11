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

#include "MemSpyDriverLogChanHeapDataUser.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverHeapWalker.h"
#include "MemSpyDriverUserEventMonitor.h"
#include "MemSpyDriverSuspensionManager.h"

DMemSpyDriverLogChanHeapDataUser::DMemSpyDriverLogChanHeapDataUser( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanHeapDataBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::DMemSpyDriverLogChanHeapDataUser() - this: 0x%08x", this ));
    }

TInt DMemSpyDriverLogChanHeapDataUser::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanHeapBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
        if  ( aFunction != EMemSpyDriverOpCodeHeapUserDataFetchCellList )
            {
            ReleaseCellList();
            }
        //
        switch( aFunction )
		    {
        case EMemSpyDriverOpCodeHeapUserDataGetInfo:
            r = GetInfoData( (TMemSpyDriverInternalHeapRequestParameters*) a1 );
            break;
        case EMemSpyDriverOpCodeHeapUserDataFetchCellList:
            r = FetchCellList( (TDes8*) a1 );
            break;
        case EMemSpyDriverOpCodeHeapUserDataGetFull:
            r = DMemSpyDriverLogChanHeapDataBase::GetFullData( (TMemSpyDriverInternalHeapDataParams*) a1 );
            break;

        default:
            r = KErrNotSupported;
		    break;
            }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanHeapDataUser::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeHeapUserDataBase && aFunction < EMemSpyDriverOpCodeHeapUserDataEnd );
    }

TInt DMemSpyDriverLogChanHeapDataUser::GetInfoData( TMemSpyDriverInternalHeapRequestParameters* aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData() - START" ) );

    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &iHeapInfoParams, sizeof(TMemSpyDriverInternalHeapRequestParameters) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - params read error: %d", r));
        }
    else
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - thread id: %d, vtable: 0x%08x, debugAllocator: %d", iHeapInfoParams.iTid, iHeapInfoParams.iRHeapVTable, iHeapInfoParams.iDebugAllocator) );

	    r = OpenTempObject( iHeapInfoParams.iTid, EThread );
	    if  ( r != KErrNone )
		    {
    	    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - thread not found") );
            }
        else
            {
            // Check that the process' thread's are suspended
            DThread* thread = (DThread*) TempObject();
            if  ( SuspensionManager().IsSuspended( *thread ) )
                {
                // Open client's heap
                RMemSpyDriverRHeapUser rHeap( OSAdaption() );
				r = rHeap.OpenUserHeap(*thread, iHeapInfoParams.iDebugAllocator);
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - opening client heap returned: %d", r) );

                if  ( r == KErrNone )
    {
                    // This object holds all of the info we will accumulate for the client.
                    TMemSpyHeapInfo masterHeapInfo;
                    masterHeapInfo.SetType(rHeap.GetTypeFromHelper());
                    masterHeapInfo.SetTid( iHeapInfoParams.iTid );
                    masterHeapInfo.SetPid( OSAdaption().DThread().GetOwningProcessId( *thread ) );

                    // This is the RHeap-specific object that contains all RHeap info
                    TMemSpyHeapInfoRHeap& rHeapInfo = masterHeapInfo.AsRHeap();


                    // We must walk the client's heap in order to build statistics
                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - calling heap walker constructor..."));
					RMemSpyDriverHeapWalker heapWalker(rHeap);
                    if  (iHeapInfoParams.iBuildFreeCellList || iHeapInfoParams.iBuildAllocCellList)
                        {
                        heapWalker.SetObserver( this );
                        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - collecting cells"));
                        }
                    else
                        {
                        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - not collecting cells"));
                        }

                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - starting traversal ..." ));

#if ( defined( TRACE_TYPE_USERHEAP ) && defined( TRACE_TYPE_HEAPWALK ) )
                    heapWalker.SetPrintDebug();
#endif
                    if (r == KErrNone) r = heapWalker.Traverse();
                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - finished traversal - err: %d", r ));

                    TMemSpyHeapStatisticsRHeap& rHeapStats = rHeapInfo.Statistics();
                    heapWalker.CopyStatsTo( rHeapStats );

                    // Get remaining meta data that isn't stored elsewhere
                    TMemSpyHeapMetaDataRHeap& rHeapMetaData = rHeapInfo.MetaData();
					DChunk& userHeapChunk = rHeap.Chunk();
					TFullName chunkName;
					userHeapChunk.FullName(chunkName);
					rHeapMetaData.SetChunkName( chunkName );
		            rHeapMetaData.SetChunkSize( (TUint) OSAdaption().DChunk().GetSize( userHeapChunk ) );
					rHeapMetaData.SetChunkHandle( &userHeapChunk );
					rHeapMetaData.SetChunkBaseAddress( OSAdaption().DChunk().GetBase( userHeapChunk ) );
                    rHeapMetaData.SetDebugAllocator(rHeap.Helper()->AllocatorIsUdeb());
                    rHeapMetaData.SetUserThread( ETrue );
					rHeapMetaData.iHeapSize = rHeap.Helper()->CommittedSize();
					rHeapMetaData.iAllocatorAddress = (TAny*)rHeap.Helper()->AllocatorAddress();
					rHeapMetaData.iMinHeapSize = rHeap.Helper()->MinCommittedSize();
					rHeapMetaData.iMaxHeapSize = rHeap.Helper()->MaxCommittedSize();

                    PrintHeapInfo( masterHeapInfo );

                    // Write free cells if requested
                    if  ( r == KErrNone && (iHeapInfoParams.iBuildFreeCellList || iHeapInfoParams.iBuildAllocCellList))
                        {
                        r = PrepareCellListTransferBuffer();
                        }

                    // Update info ready for writing back to the user-side
                    if  ( r >= KErrNone )
                        {
                        // Write results back to user-side
                        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - writing to user-side..."));
                        TMemSpyHeapInfo* userMasterInfo = iHeapInfoParams.iMasterInfo;
                        const TInt error = Kern::ThreadRawWrite( &ClientThread(), userMasterInfo, &masterHeapInfo, sizeof(TMemSpyHeapInfo) );
                        if  ( error < 0 )
                            {
                            r = error;
                            }
                        }

                    // Release resources
					rHeap.Close();
                    }
                }
            else
                {
                r = KErrAccessDenied;
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData - parent process not suspended => KErrAccessDenied"));
                }

	        CloseTempObject();
            }
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetInfoData() - END - ret: %d", r));
    return r;
    }

TInt DMemSpyDriverLogChanHeapDataUser::GetFullData( TMemSpyDriverInternalHeapDataParams& aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData() - START") );

	TInt r = OpenTempObject( aParams.iTid, EThread );
	if  ( r != KErrNone )
		{
    	Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData() - END - thread not found");
		return r;
		}
    else 
        {
        const DMemSpyDriverLogChanHeapBase::TDrmMatchType drmMatchType = IsDrmThread( TempObjectAsThread() );

        if  ( drmMatchType != DMemSpyDriverLogChanHeapBase::EMatchTypeNone )
            {
            // Check whether it's a DRM thread...
            DThread* thread = (DThread*) TempObject();
    	    Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData() - END - Not allowing dump of DRM heap - matchType: %d, thread: %O", drmMatchType, thread );
            CloseTempObject();
		    return KErrAccessDenied;
            }
        }

    // Check that the process' thread's are suspended
    DThread* thread = (DThread*) TempObject();
    if  ( SuspensionManager().IsSuspended( *thread ) )
        {
        // Open the heap
	    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData - thread: %O", thread) );
        RMemSpyDriverRHeapUser heap( OSAdaption() );
		r = heap.OpenUserHeap(*thread, aParams.iDebugAllocator);
		TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData - opening heap returned: %d", r) );
        if  (r == KErrNone)
            {
            r = DoGetFullData(aParams, thread, heap);
			}
        else
            {
    	    Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData - couldnt open heap for thread %O, err=%d", thread, r);
            r = KErrNotSupported;
            }
		heap.Close();
        }
    else
        {
        Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData - parent process not suspended => KErrAccessDenied - thread: %O", thread );
        r = KErrAccessDenied;
        }

	CloseTempObject();

	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataUser::GetFullData() - END - ret: %d", r) );
    return r;
    }
