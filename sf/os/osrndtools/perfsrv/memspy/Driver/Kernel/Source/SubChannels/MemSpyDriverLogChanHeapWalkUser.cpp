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

#include "MemSpyDriverLogChanHeapWalkUser.h"

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
#include "MemSpyDriverSuspensionManager.h"



DMemSpyDriverLogChanHeapWalkUser::DMemSpyDriverLogChanHeapWalkUser( DMemSpyDriverDevice& aDevice, DThread& aThread )
:	DMemSpyDriverLogChanHeapBase( aDevice, aThread ), iWalkHeap( aDevice.OSAdaption() )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::DMemSpyDriverLogChanHeapWalk() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanHeapWalkUser::~DMemSpyDriverLogChanHeapWalkUser()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::~DMemSpyDriverLogChanHeapWalk() - START - this: 0x%08x", this ));

    NKern::ThreadEnterCS();

    WalkHeapClose();

    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::~DMemSpyDriverLogChanHeapWalk() - END - this: 0x%08x", this ));
	}







TInt DMemSpyDriverLogChanHeapWalkUser::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeHeapUserWalkInit:
            r = WalkHeapInit( (TMemSpyDriverInternalWalkHeapParamsInit*) a1 );
            break;
        case EMemSpyDriverOpCodeHeapUserWalkGetCellInfo:
            r = WalkHeapGetCellInfo( (TAny*) a1, (TMemSpyDriverInternalWalkHeapParamsCell*) a2 );
            break;
	    case EMemSpyDriverOpCodeHeapUserWalkReadCellData:
            r = WalkHeapReadCellData( (TMemSpyDriverInternalWalkHeapCellDataReadParams*) a1 );
            break;
	    case EMemSpyDriverOpCodeHeapUserWalkNextCell:
            r = WalkHeapNextCell( (TUint) a1, (TMemSpyDriverInternalWalkHeapParamsCell*) a2 );
            break;
	    case EMemSpyDriverOpCodeHeapUserWalkClose:
            r = WalkHeapClose();
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanHeapWalkUser::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeHeapUserWalkBase && aFunction < EMemSpyDriverOpCodeHeapUserWalkEnd );
    }






















TInt DMemSpyDriverLogChanHeapWalkUser::WalkHeapInit( TMemSpyDriverInternalWalkHeapParamsInit* aParams )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit() - START"));
    __ASSERT_ALWAYS( !iHeapWalkInitialised && iWalkHeap.Helper() == NULL, MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapWalkPending ) );

    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &iHeapWalkInitialParameters, sizeof(TMemSpyDriverInternalWalkHeapParamsInit) );
    if  ( r == KErrNone )
        {
	    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - thread id: %d, vtable: 0x%08x, debugAllocator: %d", iHeapWalkInitialParameters.iTid, iHeapWalkInitialParameters.iRHeapVTable, iHeapWalkInitialParameters.iDebugAllocator));

	    r = OpenTempObject( iHeapWalkInitialParameters.iTid, EThread );
	    if  ( r == KErrNone )
		    {
            // Find the chunk with the correct handle
            DThread* thread = (DThread*) TempObject();
            if  ( SuspensionManager().IsSuspended( *thread ) )
                {
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - thread: %O", thread));

                // Open client's heap
                r = iWalkHeap.OpenUserHeap(*thread, iHeapWalkInitialParameters.iDebugAllocator);

                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - opening client heap returned: %d", r) );

                if  ( r == KErrNone )
                    {
                    // Indicates that we've initiated a walk - so we can tell whether to close
                    // the chunk later on.
                    iHeapWalkInitialised = ETrue;
                    iWalkHeapCellIndex = 0;

                    // Walk the client's heap
                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - calling heap walker constructor..."));
                    RMemSpyDriverHeapWalker heapWalker(iWalkHeap);
                    
                    TMemSpyDriverLogChanHeapWalkObserver observer( *this );
                    heapWalker.SetObserver( &observer );

                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - starting traversal..."));
                    r = heapWalker.Traverse();
                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - finished traversal - err: %d", r));
                    }

                // If the initialise process didn't complete successfully, then we must be sure
                // to release the associated heap chunk
                if  ( r < KErrNone )
                    {
                    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - error scenario - releasing kernel heap chunk copy" ));
                    iWalkHeap.Close();
                    }
                }
            else
                {
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - parent process not suspended => KErrAccessDenied"));
                r = KErrAccessDenied;
                }
            
	        CloseTempObject();
            }
        else
            {
    	    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - thread not found"));
		    }
        }
    else
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit - params read error: %d", r));
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapInit() - END - ret: %d", r));
    return r;
    }


TInt DMemSpyDriverLogChanHeapWalkUser::WalkHeapNextCell( TUint aTid, TMemSpyDriverInternalWalkHeapParamsCell* aParams )
    {
    const TInt walkedHeapCellCount = iWalkHeapCells.Count();
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapNextCell() - START - current cell count: %d", walkedHeapCellCount));
    __ASSERT_ALWAYS( iHeapWalkInitialised && iWalkHeap.Helper(), MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapWalkNotInitialised ) );

    // Open the original thread
	TInt r = OpenTempObject( aTid, EThread );
	if  ( r == KErrNone )
		{
        // Get the thread handle and that we have suspended the process' threads
        DThread* thread = (DThread*) TempObject();
        if  ( SuspensionManager().IsSuspended( *thread ) )
            {
            NKern::ThreadEnterCS();

            if  ( walkedHeapCellCount > 0 && iWalkHeapCellIndex >= 0 && iWalkHeapCellIndex < walkedHeapCellCount )
                {
                // Write back head cell to user-space
                TMemSpyDriverInternalWalkHeapParamsCell cell( iWalkHeapCells[ iWalkHeapCellIndex++ ] );
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapNextCell - returning... cellType: %1d, addr: 0x%08x, len: %8d, nestingLev: %8d, allocNum: %8d", cell.iCellType, cell.iCellAddress, cell.iLength, cell.iNestingLevel, cell.iAllocNumber ));
 
                r = Kern::ThreadRawWrite( &ClientThread(), aParams, &cell, sizeof(TMemSpyDriverInternalWalkHeapParamsCell) );
                if  ( r != KErrNone )
                    {
    	            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapNextCell - params read error: %d", r));
                    }
                }
            else
                {
                r = KErrEof;
                }

            NKern::ThreadLeaveCS();
            }
        else
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapNextCell - parent process not suspended => KErrAccessDenied"));
            r = KErrAccessDenied;
            }
    
        CloseTempObject();
        }
    else
        {
    	Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapNextCell - thread not found");
		}
    //    
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapNextCell() - END - ret: %d", r));
    return r;
    }


TInt DMemSpyDriverLogChanHeapWalkUser::WalkHeapClose()
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapClose() - START"));
    //
    if  ( iHeapWalkInitialised )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapClose - heap walk was still open..."));
      	NKern::ThreadEnterCS();

		iWalkHeap.Close();

        // Discard handled cells
        iWalkHeapCells.Reset();

        iHeapWalkInitialised = EFalse;

    	NKern::ThreadLeaveCS();
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapClose() - END"));
    return KErrNone;
    }


TInt DMemSpyDriverLogChanHeapWalkUser::WalkHeapReadCellData(TMemSpyDriverInternalWalkHeapCellDataReadParams* aParams)
    {
    __ASSERT_ALWAYS( iHeapWalkInitialised && iWalkHeap.Helper(), MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapWalkNotInitialised ) );
    //
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData() - START - thread id: %d, vtable: 0x%08x", iHeapWalkInitialParameters.iTid, iHeapWalkInitialParameters.iRHeapVTable));
    //
	TMemSpyDriverInternalWalkHeapCellDataReadParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalWalkHeapCellDataReadParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData() - END - params read error: %d", r));
        return r;
        }
    
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - cell: 0x%08x, readLen: %8d, writeAddr: 0x%08x", params.iCellAddress, params.iReadLen, params.iDes));

    // Open the original thread
	r = OpenTempObject( iHeapWalkInitialParameters.iTid, EThread );
	if  ( r == KErrNone )
		{
        // Get the thread handle
        DThread* thread = (DThread*) TempObject();

        // Check the threads in the process are suspended
        if  ( SuspensionManager().IsSuspended( *thread ) )
            {
            // Check we can find the cell in the cell list...
            const TMemSpyDriverInternalWalkHeapParamsCell* cell = CellInfoForSpecificAddress( params.iCellAddress );

            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - cell: 0x%08x for address: 0x%08x", cell, params.iCellAddress ));

            if  ( cell )
                {
                const TInt cellLen = cell->iLength;
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - cellLen: %d", cellLen ));

                if  ( params.iReadLen <= cellLen )
                    {

                    // Get user side descriptor length info
         	        TInt destLen = 0;
        	        TInt destMax = 0;
                    TUint8* destPtr = NULL;

                    r = Kern::ThreadGetDesInfo( &ClientThread(), params.iDes, destLen, destMax, destPtr, ETrue );
        	        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - user side descriptor: 0x%08x (0x%08x), len: %8d, maxLen: %8d, r: %d", params.iDes, destPtr, destLen, destMax, r ));

                    // Work out the start offset for the data...
                    if  ( r == KErrNone && destMax >= params.iReadLen )
                        {
                        const TAny* srcPos = ((TUint8*) cell->iCellAddress);
        	            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - srcPos: 0x%08x", srcPos ));

                        // Read some data 
                        r = Kern::ThreadRawRead( thread, srcPos, destPtr, params.iReadLen );
    	                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - read from thread returned: %d", r));

                        if  ( r == KErrNone )
                            {
                            // Client will update descriptor length in this situation.
                            r = params.iReadLen;
                            }
                        }
                    else
                        {
                        if  ( r != KErrBadDescriptor )
                            {
                            r = KErrArgument;                
            	            Kern::Printf( "DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - error - user-descriptor isnt big enough for requested data" );
                            }
                        else
                            {
            	            Kern::Printf( "DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - error - bad or non-writable user-side descriptor" );
                            }
                        }
                    }
                else
                    {
                    r = KErrArgument;
        	        Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - error - read length is bigger than cell length");
                    }
                }
            else
                {
                r = KErrArgument;
                Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - no cell at user supplied address!");
                }
            }
        else
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - parent process not suspended => KErrAccessDenied"));
            r = KErrAccessDenied;
            }

        CloseTempObject();
        }
    else
        {
    	Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData - thread not found");
		}
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapReadCellData() - END result=%d", r));
    return r;
    }


TInt DMemSpyDriverLogChanHeapWalkUser::WalkHeapGetCellInfo( TAny* aCellAddress, TMemSpyDriverInternalWalkHeapParamsCell* aParams )
    {
    __ASSERT_ALWAYS( iHeapWalkInitialised && iWalkHeap.Helper(), MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapWalkNotInitialised ) );
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo() - START - thread id: %d, vtable: 0x%08x", iHeapWalkInitialParameters.iTid, iHeapWalkInitialParameters.iRHeapVTable));
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo - cell: 0x%08x", aCellAddress));

    // Open the original thread
	TInt r = OpenTempObject( iHeapWalkInitialParameters.iTid, EThread );
	if (r != KErrNone)
		{
    	Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo() - END - thread not found");
		return r;
		}

    // Get the thread handle
    DThread* thread = (DThread*) TempObject();

    // Check the threads in the process are suspended
    if  ( !SuspensionManager().IsSuspended( *thread ) )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo - END - parent process not suspended => KErrAccessDenied"));
        CloseTempObject();
        return KErrAccessDenied;
        }

    // Check we can find the cell in the cell list...
    const TMemSpyDriverInternalWalkHeapParamsCell* cell = CellInfoForSpecificAddress( aCellAddress );
    if  ( cell == NULL )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo - no exact match for address: 0x%08x...", aCellAddress));
        
        // If the cell still wasn't found, then let's look for any heap cell that contains
        // the client-specified address (i.e. find the heap cell that contains the specified
        // address).
        if  ( cell == NULL )
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo - still couldnt find cell by exact address. Searching for the cell that contains the specified address..."));
            cell = CellInfoForAddressWithinCellRange( aCellAddress );
            }
        }

    if  ( cell )
        {
        // Have enough info to write back to client now
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo - returning... cellType: %1d, addr: 0x%08x, len: %8d, nestingLev: %8d, allocNum: %8d", cell->iCellType, cell->iCellAddress, cell->iLength, cell->iNestingLevel, cell->iAllocNumber ));
        r = Kern::ThreadRawWrite( &ClientThread(), aParams, cell, sizeof(TMemSpyDriverInternalWalkHeapParamsCell) );
        }
    else
        {
        r = KErrArgument;
        Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo - no cell at user supplied address!");
        }
    
    CloseTempObject();
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapWalk::WalkHeapGetCellInfo() - END result=%d", r));
    return r;
    }



























const TMemSpyDriverInternalWalkHeapParamsCell* DMemSpyDriverLogChanHeapWalkUser::CellInfoForAddressWithinCellRange( TAny* aAddress ) const
    {
    const TMemSpyDriverInternalWalkHeapParamsCell* ret = NULL;
    //
    const TInt count = iWalkHeapCells.Count();
    for(TInt i=0; i<count; i++)
        {
        const TMemSpyDriverInternalWalkHeapParamsCell& item = iWalkHeapCells[i];
        const TAny* cellExtent = (TAny*) (TUint32( item.iCellAddress ) + item.iLength);
        //
        if  ( aAddress >= item.iCellAddress && aAddress < cellExtent )
            {
            ret = &item;
            }
        }
    //
    return ret;
    }


const TMemSpyDriverInternalWalkHeapParamsCell* DMemSpyDriverLogChanHeapWalkUser::CellInfoForSpecificAddress( TAny* aAddress ) const
    {
    const TMemSpyDriverInternalWalkHeapParamsCell* ret = NULL;
    //
    const TInt count = iWalkHeapCells.Count();
    for(TInt i=0; i<count; i++)
        {
        const TMemSpyDriverInternalWalkHeapParamsCell& item = iWalkHeapCells[i];
        if  ( item.iCellAddress == aAddress )
            {
            ret = &item;
            }
        }
    //
    return ret;
    }
















TBool DMemSpyDriverLogChanHeapWalkUser::WalkerHandleHeapCell(TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber )
    {
    TInt error = KErrNone;
    //
    if  ( iHeapWalkInitialised )
        {
        TMemSpyDriverInternalWalkHeapParamsCell cell;
        cell.iCellType = aCellType;
	    cell.iCellAddress = aCellAddress;
	    cell.iLength = aLength;
	    cell.iNestingLevel = aNestingLevel;
	    cell.iAllocNumber = aAllocNumber;
        //
  	    NKern::ThreadEnterCS();
        error = iWalkHeapCells.Append( cell );
  	    NKern::ThreadLeaveCS();
        }
    //
    return ( error == KErrNone );
    }
