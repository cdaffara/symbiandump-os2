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

#include <memspy/driver/memspydriverclient.h>

// System includes
#include <e32svr.h>

// User includes
#include "MemSpyDriverOpCodes.h"
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverobjectsshared.h>
#include "MemSpyDriverStreamReaderImp.h"
#include "MemSpyDriverObjectsInternal.h"
#include "heaputils.h"

// Constants
const TInt KMemSpyClientBufferGrowSize = 0x1000 * 8; // 32kb

// Forward declarations
static void PrintHeapInfo( const TMemSpyHeapInfo& aInfo );




EXPORT_C TInt RMemSpyDriverClient::Open()
	{
    TInt err = iBuffer.Create( KMemSpyClientBufferGrowSize );
    if  ( err == KErrNone )
        {
        err = User::LoadLogicalDevice( KMemSpyDriverDeviceName );
        if  ( err == KErrNone || err == KErrAlreadyExists )
		    {
		    err = DoCreate( KMemSpyDriverDeviceName, KMemSpyDriverVersion(), KNullUnit, NULL, NULL, EOwnerThread );
            if  ( err == KErrNone )
                {
                TUint heapVTable = RHeapVTable();
 	            err = DoControl( EMemSpyDriverOpCodeMiscSetRHeapVTable, (TAny*) heapVTable );
                }
		    }
        }
    //
    if ( err != KErrNone )
        {
        RDebug::Printf( "[MemSpy] RMemSpyDriverClient::Open() - END - err: %d", err );
        }
    //
	return err;
	}



EXPORT_C void RMemSpyDriverClient::Close()
	{
    RBusLogicalChannel::Close();
    const TInt err = User::FreeLogicalDevice( KMemSpyDriverDeviceName );
    //
    if ( err != KErrNone )
        {
        RDebug::Printf( "[MemSpy] RMemSpyDriverClient::Close() - free logical device error: %d", err );
        }
    //
    iBuffer.Close();
    (void) err;
	}


EXPORT_C void RMemSpyDriverClient::GetVersion( TVersion& aVersion )
    {
    TVersion v = KMemSpyDriverVersion();
    Mem::Copy( (TAny*)&aVersion, (TAny*)&v, sizeof( TVersion ) );
    }



















EXPORT_C TInt RMemSpyDriverClient::ReadMemory( TUint aTid, TLinAddr aSrc, TDes8& aDest )
	{
	TMemSpyDriverInternalReadMemParams params;
	params.iTid = aTid;
	params.iAddr = aSrc;
	params.iDes = &aDest;
    aDest.Zero();
    //
	TInt r = DoControl( EMemSpyDriverOpCodeRawMemoryRead, &params, NULL );
    if  ( r >= KErrNone )
        {
        aDest.SetLength( r );
        r = KErrNone;
        }
    //
    return r;
    }














EXPORT_C TInt RMemSpyDriverClient::GetCodeSegs( TAny** aHandleArray, TInt& aHandleCount, TBool aOnlyRamLoaded )
    {
	TMemSpyDriverInternalCodeSnapshotParams params;
	params.iFilter = aOnlyRamLoaded;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
	params.iMaxCount = aHandleCount;
    //
    aHandleCount = 0;
    //
	return DoControl( EMemSpyDriverOpCodeCodeSegsGetAll, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetCodeSegs( TUint aPid, TAny** aHandleArray, TInt& aHandleCount )
	{
	TMemSpyDriverInternalCodeSnapshotParams params;
	params.iFilter = aPid;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
	params.iMaxCount = aHandleCount;
    //
    aHandleCount = 0;
    //
	return DoControl( EMemSpyDriverOpCodeCodeSegsGetCodeSegsForProcess, &params, NULL );
	}


EXPORT_C TInt RMemSpyDriverClient::GetCodeSegInfo( TAny* aHandle, TUint aPid, TMemSpyDriverCodeSegInfo& aInfo )
	{
	TMemSpyDriverInternalCodeSegParams params;
	params.iPid = aPid;
	params.iHandle = aHandle;
	params.iInfoPointer = &aInfo;
    //
	const TInt r = DoControl( EMemSpyDriverOpCodeCodeSegsGetCodeSegInfo, &params, NULL );
	return r;
	}



























EXPORT_C TInt RMemSpyDriverClient::GetChunkHandles( TAny** aHandleArray, TInt& aHandleCount )
    {
	TMemSpyDriverInternalChunkHandleParams params;
	params.iId = 0;
	params.iType = EMemSpyDriverPrivateObjectTypeAll;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
    params.iMaxCount = aHandleCount;
    //
    aHandleCount = 0;
	//
	const TInt err = DoControl( EMemSpyDriverOpCodeChunkGetHandles, &params, NULL );
    return err;
    }


EXPORT_C TInt RMemSpyDriverClient::GetChunkInfo( TAny* aHandle, TMemSpyDriverChunkInfo& aInfo )
    {
	TMemSpyDriverInternalChunkInfoParams params;
	TBuf8<KMaxFullName> name;
	params.iHandle = aHandle;
	const TInt r = DoControl( EMemSpyDriverOpCodeChunkGetInfo, &params, NULL );
	//
	if  ( r == KErrNone )
		{
        aInfo.iHandle = aHandle;
		aInfo.iBaseAddress = params.iBaseAddress;
		aInfo.iSize = params.iSize;
		aInfo.iMaxSize = params.iMaxSize;
		aInfo.iOwnerId = params.iOwnerId;
        aInfo.iType = params.iType;
        aInfo.iAttributes = params.iAttributes;
		aInfo.iName.Copy( params.iName );
		}
	//
	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetChunkInfo( TAny* aHandle, TMemSpyDriverChunkInfoWithoutName& aInfo )
    {
    TMemSpyDriverChunkInfo info;
    //
    const TInt r = GetChunkInfo( aHandle, info );
    if  ( r == KErrNone )
        {
        aInfo = info;
        }
    //
    return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetChunkHandlesForProcess( TUint aPid, TAny** aHandleArray, TInt& aHandleCount )
    {
	TMemSpyDriverInternalChunkHandleParams params;
	params.iId = aPid;
	params.iType = EMemSpyDriverPrivateObjectTypeProcess;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
	params.iMaxCount = aHandleCount;
	//
    aHandleCount = 0;
    //
	return DoControl( EMemSpyDriverOpCodeChunkGetHandles, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetChunkHandlesForThread( TUint aTid, TAny** aHandleArray, TInt& aHandleCount )
    {
	TMemSpyDriverInternalChunkHandleParams params;
	params.iId = aTid;
	params.iType = EMemSpyDriverPrivateObjectTypeThread;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
	params.iMaxCount = aHandleCount;
	//
    aHandleCount = 0;
    //
	return DoControl( EMemSpyDriverOpCodeChunkGetHandles, &params, NULL );
    }





















EXPORT_C TInt RMemSpyDriverClient::GetThreadInfo( TUint aTid, TMemSpyDriverThreadInfo& aInfo )
	{
	TMemSpyDriverInternalThreadInfoParams params;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    //
	TInt r = DoControl( EMemSpyDriverOpCodeThreadAndProcessGetInfoThread, (TAny*)aTid, &params);
	//
	if  ( r == KErrNone )
		{
        // Copy common properties
        Mem::Copy( &aInfo, &params, sizeof( TMemSpyDriverThreadInfoBase ) );

        // Copy name
        aInfo.iFullName.Copy( params.iFullName );

        // Get other basic properties via RThread
        RThread thread;
        r = OpenThread( aTid, thread );
        if ( r == KErrNone )
            {
            RProcess process;
            r = thread.Process( process );
            if ( r == KErrNone )
                {
	            aInfo.iPid = process.Id();
                process.Close();
                }

            aInfo.iThreadPriority = thread.Priority();
            aInfo.iExitReason = thread.ExitReason();
            aInfo.iExitType = thread.ExitType();
            aInfo.iExitCategory = thread.ExitCategory();

            thread.Close();
            }
        }

    return r;
	}

EXPORT_C TInt RMemSpyDriverClient::GetProcessInfo( TUint aPid, TMemSpyDriverProcessInfo& aInfo )
    {
	TInt r = DoControl( EMemSpyDriverOpCodeThreadAndProcessGetInfoProcess, (TAny*)aPid, &aInfo);
    
    // Get other properties via RProcess.
    if  ( r == KErrNone )
        {
        RProcess process;
        r = OpenProcess( aPid, process );
        if  ( r == KErrNone )
            {
            aInfo.iUids = process.Type();
            aInfo.iPriority = process.Priority();
            //
            process.Close();
            }
        }

    return r;
    }


EXPORT_C TInt RMemSpyDriverClient::ProcessThreadsSuspend( TUint aPid )
    {
	return DoControl( EMemSpyDriverOpCodeThreadAndProcessSuspendAllThreads, (TAny*) aPid, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::ProcessThreadsResume( TUint aPid )
    {
	return DoControl( EMemSpyDriverOpCodeThreadAndProcessResumeAllThreads, (TAny*) aPid, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::ThreadEnd( TUint aId, TExitType aType )
    {
    TInt err = KErrNone;
    //
    if  ( aType == EExitPending )
        {
        err = KErrArgument;
        }
    else
        {
        err = DoControl( EMemSpyDriverOpCodeThreadAndProcessEndThread, (TAny*) aId, (TAny*) aType );
        }
    //
    return err;
    }


EXPORT_C TInt RMemSpyDriverClient::OpenThread( TUint aId, RThread& aThread )
    {
    TInt ret = KErrNone;
    aThread.Close();
    //
    const TInt err = DoControl( EMemSpyDriverOpCodeThreadAndProcessOpenThread, (TAny*) aId );
    if  ( err > 0 )
        {
        aThread.SetHandle( err );
        ret = KErrNone;
        }
    else
        {
        ret = err;
        }
    //
    return ret;
    }


EXPORT_C TInt RMemSpyDriverClient::OpenProcess( TUint aId, RProcess& aProcess )
    {
    TInt ret = KErrNone;
    aProcess.Close();
    //
    const TInt err = DoControl( EMemSpyDriverOpCodeThreadAndProcessOpenProcess, (TAny*) aId );
    if  ( err > 0 )
        {
        aProcess.SetHandle( err );
        ret = KErrNone;
        }
    else
        {
        ret = err;
        }
    //
    return ret;
    }


EXPORT_C void RMemSpyDriverClient::GetThreadsL( const TProcessId& aId, RArray<TThreadId>& aThreads )
    {
	aThreads.Reset();
    //
    ResetStreamBuffer();
    const TInt err = DoControl( EMemSpyDriverOpCodeThreadAndProcessGetThreads, (TAny*) (TUint) aId, (TAny*) &iBuffer );
    User::LeaveIfError( err );

    // Open stream
    RMemSpyMemStreamReader stream = StreamOpenL();
    CleanupClosePushL( stream );
        
    // Extract thread ids
    const TInt threadCount = stream.ReadInt32L();
    for( TInt i=0; i<threadCount; i++ )
        {
        const TThreadId id( stream.ReadUint32L() );
        aThreads.AppendL( id );
        }
        
    // Tidy up
    CleanupStack::PopAndDestroy( &stream );
    }


EXPORT_C TInt RMemSpyDriverClient::SetPriority( TUint aId, TThreadPriority aPriority )
    {
    // The kernel side API to set thread priority expects a value in the range of 0-63, i.e. an absolute
    // NThread priority. In order to support process-relative thread priorities, we'd need to duplicate
    // the kernel priority mapping tables - not a good idea - therefore we'll only support absolute values.
    TInt err = KErrNone;
    if  ( aPriority < EPriorityAbsoluteVeryLow )
        {
        err = KErrArgument;
        }
    else
        {
        err = DoControl( EMemSpyDriverOpCodeThreadAndProcessSetPriorityThread, (TAny*) aId, (TAny*) aPriority );
        }
    //
    return err;
    }















EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoUser( TMemSpyHeapInfo& aInfo, TUint aTid )
    {
    TMemSpyDriverInternalHeapRequestParameters params;
    //
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    params.iMasterInfo = &aInfo;
    //
	TInt r = DoControl( EMemSpyDriverOpCodeHeapUserDataGetInfo, &params, NULL );
	//
	if  ( r == KErrNone )
    	{
        PrintHeapInfo( aInfo );
        
        // Need to do this on the user-side
        if  ( aInfo.Type() == TMemSpyHeapInfo::ETypeRHeap )
            {
            TMemSpyHeapInfoRHeap& rHeapInfo = aInfo.AsRHeap();
            TMemSpyHeapMetaDataRHeap& metaData = rHeapInfo.MetaData();
            metaData.SetVTable( RHeapVTable() );
            //metaData.SetClassSize( sizeof( RHeap ) );
            }
        }
    else if ( r == KErrNotSupported )
        {
        aInfo.SetType( TMemSpyHeapInfo::ETypeUnknown );
        r = KErrNone;
        }
	//
	return r;
    }

EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoUser(TMemSpyHeapInfo& aInfo, 
                                                   TUint aTid, 
                                                   RArray< TMemSpyDriverFreeCell >& aFreeCells)
	{
	return GetHeapInfoUser(aInfo, aTid, aFreeCells, EFalse);
	}

// For the record I don't think this function should be exported, but since the one above was I'm going with the flow. -TomS
EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoUser(TMemSpyHeapInfo& aInfo, 
                                                   TUint aTid, 
                                                   RArray<TMemSpyDriverCell>& aCells, 
                                                   TBool aCollectAllocatedCellsAsWellAsFree)
	{
    TMemSpyDriverInternalHeapRequestParameters params;
    //
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    params.iMasterInfo = &aInfo;
    params.iBuildFreeCellList = ETrue;
	params.iBuildAllocCellList = aCollectAllocatedCellsAsWellAsFree;
    //
    aCells.Reset();
    ResetStreamBuffer();
	TInt r = DoControl( EMemSpyDriverOpCodeHeapUserDataGetInfo, &params );
	//
	if  ( r >= KErrNone )
    	{
        PrintHeapInfo( aInfo );
        
        // Need to do this on the user-side
        if  ( aInfo.Type() == TMemSpyHeapInfo::ETypeRHeap )
            {
            TMemSpyHeapInfoRHeap& rHeapInfo = aInfo.AsRHeap();
            TMemSpyHeapMetaDataRHeap& metaData = rHeapInfo.MetaData();
            metaData.SetVTable( RHeapVTable() );
            //metaData.SetClassSize( sizeof( RHeap ) );
            }

        // Resize transfer buffer to make room for free cells. We only make the buffer
        // bigger, not smaller.
        if  ( iBuffer.Size() < r )
            {
            r = iBuffer.ReAlloc( r );
            }
    
        // Now fetch the heap data
        if  ( r == KErrNone )
            {
            r = DoControl( EMemSpyDriverOpCodeHeapUserDataFetchCellList, &iBuffer );
            if  ( r == KErrNone )
                {
                TRAP( r, ReadHeapInfoFreeCellsFromXferBufferL( aCells ) );
                }
            }
        }
    else if ( r == KErrNotSupported )
        {
        aInfo.SetType( TMemSpyHeapInfo::ETypeUnknown );
        r = KErrNone;
        }
	//
	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoKernel( TMemSpyHeapInfo& aInfo )
    {
    TMemSpyDriverInternalHeapRequestParameters params;
    //
    params.iTid = KMemSpyDriverGetKernelHeapDataPseudoThreadId;
    params.iRHeapVTable = NULL;
    params.iMasterInfo = &aInfo;
    //
	TInt r = DoControl( EMemSpyDriverOpCodeHeapKernelDataGetInfo, &params, NULL );
	//
	if  ( r == KErrNone )
    	{
        PrintHeapInfo( aInfo );
        }
    else if ( r == KErrNotSupported )
        {
        aInfo.SetType( TMemSpyHeapInfo::ETypeUnknown );
        r = KErrNone;
        }
	//
	return r;
    }

EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoKernel(TMemSpyHeapInfo& aInfo, 
                                                     RArray< TMemSpyDriverFreeCell >& aFreeCells)
    {
    return GetHeapInfoKernel(aInfo, aFreeCells, EFalse, EFalse);
    }

EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoKernel(TMemSpyHeapInfo& aInfo, 
                                                     RArray<TMemSpyDriverCell>& aCells, 
                                                     TBool aCollectAllocatedCellsAsWellAsFree)
    {
    return GetHeapInfoKernel(aInfo, aCells, aCollectAllocatedCellsAsWellAsFree, EFalse);    
    }

EXPORT_C TInt RMemSpyDriverClient::GetHeapInfoKernel(TMemSpyHeapInfo& aInfo, 
                                                     RArray<TMemSpyDriverCell>& aCells, 
                                                     TBool aCollectAllocatedCellsAsWellAsFree, 
                                                     TBool aUseKernelCopy)
    {
    TMemSpyDriverInternalHeapRequestParameters params;
    //
	params.iTid = KMemSpyDriverGetKernelHeapDataPseudoThreadId;
    params.iRHeapVTable = NULL;
    params.iMasterInfo = &aInfo;
    params.iBuildFreeCellList = ETrue;
	params.iBuildAllocCellList = aCollectAllocatedCellsAsWellAsFree;
    params.iUseKernelHeapCopy = aUseKernelCopy;
    //
    aCells.Reset();
    ResetStreamBuffer();
	TInt r = DoControl( EMemSpyDriverOpCodeHeapKernelDataGetInfo, &params );
	//
	if  ( r >= KErrNone )
    	{
        PrintHeapInfo( aInfo );
        
        // Resize transfer buffer to make room for free cells. We only make the buffer
        // bigger, not smaller.
        if  ( iBuffer.Size() < r )
            {
            r = iBuffer.ReAlloc( r );
            }
    
        // Now fetch the heap data
        if  ( r == KErrNone )
            {
            r = DoControl( EMemSpyDriverOpCodeHeapKernelDataFetchCellList, &iBuffer );
            if  ( r == KErrNone )
                {
                TRAP( r, ReadHeapInfoFreeCellsFromXferBufferL( aCells ) );
                }
            }
        }
    else if ( r == KErrNotSupported )
        {
        aInfo.SetType( TMemSpyHeapInfo::ETypeUnknown );
        r = KErrNone;
        }
	//
	return r;
    }
       

EXPORT_C TBool RMemSpyDriverClient::IsDebugKernel()
    {
    TBool isDebugKernel = EFalse;
    DoControl( EMemSpyDriverOpCodeHeapKernelDataGetIsDebugKernel, (TAny*) &isDebugKernel );
    return isDebugKernel;
    }






















EXPORT_C TInt RMemSpyDriverClient::GetHeapData(TUint aTid, 
                                               TUint32 aFreeCellChecksum, 
                                               TDes8& aDest, 
                                               TUint& aReadAddress, 
                                               TUint& aAmountRemaining )
    {
    TMemSpyDriverInternalHeapDataParams params;
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    params.iDes = &aDest;
    params.iChecksum = aFreeCellChecksum;
    params.iRemaining = -1;
	params.iReadAddress = 0;
    aDest.Zero();
    //
	TInt r = DoControl( EMemSpyDriverOpCodeHeapUserDataGetFull, &params, NULL );
	//
	if  ( r >= KErrNone )
    	{
        aDest.SetLength( r );
        aReadAddress = params.iReadAddress;
    	aAmountRemaining = params.iRemaining;
        r = KErrNone;
    	}
	//
	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetHeapDataNext(TUint aTid, 
                                                   TDes8& aDest, 
                                                   TUint& aReadAddress, 
                                                   TUint& aAmountRemaining)
    {
    TMemSpyDriverInternalHeapDataParams params;
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    params.iDes = &aDest;
    params.iChecksum = 0;
    params.iRemaining = aAmountRemaining;
	params.iReadAddress = aReadAddress;
    aDest.Zero();
    //
	TInt r = DoControl( EMemSpyDriverOpCodeHeapUserDataGetFull, &params, NULL );
	//
	if  ( r >= KErrNone )
    	{
        aDest.SetLength( r );
        aReadAddress = params.iReadAddress;
    	aAmountRemaining = params.iRemaining;
        r = KErrNone;
    	}
	//
    return r;
    }


EXPORT_C TInt RMemSpyDriverClient::CopyHeapDataKernel()
    {
    return DoControl( EMemSpyDriverOpCodeHeapKernelDataCopyHeap, NULL, NULL );    
    }

EXPORT_C TInt RMemSpyDriverClient::FreeHeapDataKernel()
    {
    return DoControl( EMemSpyDriverOpCodeHeapKernelDataFreeHeapCopy, NULL, NULL );    
    }

EXPORT_C TInt RMemSpyDriverClient::GetHeapDataKernel(TUint aTid, 
                                                     TDes8& aDest, 
                                                     TUint& aReadAddress, 
                                                     TUint& aAmountRemaining)
    {
    TMemSpyDriverInternalHeapDataParams params;
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    params.iDes = &aDest;
    params.iChecksum = 0;
    params.iRemaining = -1;
    params.iReadAddress = 0;
    aDest.Zero();
    //
    TInt r = DoControl( EMemSpyDriverOpCodeHeapKernelDataGetFull, &params, NULL );
    //
    if  ( r >= KErrNone )
        {
        aDest.SetLength( r );
        aReadAddress = params.iReadAddress;
        aAmountRemaining = params.iRemaining;
        r = KErrNone;
        }
    return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetHeapDataKernelNext(TUint aTid, 
                                                         TDes8& aDest, 
                                                         TUint& aReadAddress, 
                                                         TUint& aAmountRemaining)
    {
    TMemSpyDriverInternalHeapDataParams params;
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    params.iDes = &aDest;
    params.iChecksum = 0;
    params.iRemaining = aAmountRemaining;
    params.iReadAddress = aReadAddress;
    aDest.Zero();
    //
    TInt r = DoControl( EMemSpyDriverOpCodeHeapKernelDataGetFull, &params, NULL );
    //
    if  ( r >= KErrNone )
        {
        aDest.SetLength( r );
        aReadAddress = params.iReadAddress;
        aAmountRemaining = params.iRemaining;
        r = KErrNone;
        }
    //
    return r;
    }


































EXPORT_C TInt RMemSpyDriverClient::WalkHeapInit( TUint aTid )
    {
    TMemSpyDriverInternalWalkHeapParamsInit params;
    params.iTid = aTid;
    params.iRHeapVTable = RHeapVTable();
    params.iDebugAllocator = DebugEUser();
    //
	const TInt r = DoControl( EMemSpyDriverOpCodeHeapUserWalkInit, &params, NULL );
	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::WalkHeapNextCell(TUint aTid, 
                                                    TMemSpyDriverCellType& aCellType, 
                                                    TAny*& aCellAddress, 
                                                    TInt& aLength, 
                                                    TInt& aNestingLevel, 
                                                    TInt& aAllocNumber, 
                                                    TInt& aCellHeaderSize, 
                                                    TAny*& aCellPayloadAddress)
    {
    aCellType = EMemSpyDriverBadCellMask;
    aCellAddress = NULL;
    aLength = 0;
    aNestingLevel = 0;
    aAllocNumber = 0;
    aCellHeaderSize = 0;
    aCellPayloadAddress = NULL;
    //
    TMemSpyDriverInternalWalkHeapParamsCell params;
	const TInt r = DoControl( EMemSpyDriverOpCodeHeapUserWalkNextCell, (TAny*) aTid, &params );
    //
	if  ( r == KErrNone )
	    {
        aCellType = (TMemSpyDriverCellType) params.iCellType;
        aCellAddress = params.iCellAddress;
        aLength = params.iLength;
        aNestingLevel = params.iNestingLevel;
        aAllocNumber = params.iAllocNumber;
        aCellPayloadAddress = ((TUint8*) aCellAddress) + aCellHeaderSize;
        }
    //
   	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::WalkHeapReadCellData( TAny* aCellAddress, TDes8& aDest, TInt aReadLen )
    {
    TMemSpyDriverInternalWalkHeapCellDataReadParams params;
    params.iCellAddress = aCellAddress;
    params.iReadLen = aReadLen;
    params.iDes = &aDest;
    aDest.Zero();
    //
	TInt r = DoControl( EMemSpyDriverOpCodeHeapUserWalkReadCellData, &params, NULL );
    if  ( r >= KErrNone )
        {
        aDest.SetLength( r );
        r = KErrNone;
        }
    //
	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::WalkHeapGetCellInfo(TAny*& aCellAddress, 
                                                       TMemSpyDriverCellType& aCellType, 
                                                       TInt& aLength, 
                                                       TInt& aNestingLevel, 
                                                       TInt& aAllocNumber, 
                                                       TInt& aCellHeaderSize, 
                                                       TAny*& aCellPayloadAddress)
    {
    aCellType = EMemSpyDriverBadCellMask;
    aLength = 0;
    aNestingLevel = 0;
    aAllocNumber = 0;
    aCellHeaderSize = 0;
    aCellPayloadAddress = NULL;
    //
    TMemSpyDriverInternalWalkHeapParamsCell params;
	const TInt r = DoControl( EMemSpyDriverOpCodeHeapUserWalkGetCellInfo, aCellAddress, &params );
    //
	if  ( r == KErrNone )
	    {
        aCellAddress = params.iCellAddress;
        aCellType = (TMemSpyDriverCellType) params.iCellType;
        aLength = params.iLength;
        aNestingLevel = params.iNestingLevel;
        aAllocNumber = params.iAllocNumber;
        aCellPayloadAddress = ((TUint8*) aCellAddress) + aCellHeaderSize;
        }
    //
   	return r;
    }


EXPORT_C void RMemSpyDriverClient::WalkHeapClose()
    {
	DoControl( EMemSpyDriverOpCodeHeapUserWalkClose, NULL, NULL );
    }



























EXPORT_C TInt RMemSpyDriverClient::GetStackInfo( TUint aTid, TMemSpyDriverStackInfo& aInfo )
    {
    TMemSpyDriverStackInfo params;
	const TInt r = DoControl( EMemSpyDriverOpCodeStackGetInfo, (TAny*)aTid, &params );
	if  (r==KErrNone)
		{
		aInfo.iUserStackPointer = params.iUserStackPointer;
		aInfo.iUserStackBase = params.iUserStackBase;
		aInfo.iUserStackSize = params.iUserStackSize;
		aInfo.iUserStackHighWatermark = params.iUserStackHighWatermark;
		aInfo.iSupervisorStackPointer = params.iSupervisorStackPointer;
		aInfo.iSupervisorStackBase = params.iSupervisorStackBase;
		aInfo.iSupervisorStackSize = params.iSupervisorStackSize;
		aInfo.iSupervisorStackHighWatermark = params.iSupervisorStackHighWatermark;
		}
	//
	return r;
    }    


EXPORT_C TInt RMemSpyDriverClient::GetStackData(TUint aTid, 
                                                TDes8& aDest, 
                                                TUint& aAmountRemaining, 
                                                TMemSpyDriverDomainType aDomain, 
                                                TBool aEntireStack)
    {
    TMemSpyDriverInternalStackDataParams params;
    params.iTid = aTid;
    params.iDes = &aDest;
    params.iDomain = aDomain;
    params.iEntireStack = aEntireStack;
    params.iRemaining = -1;
    aDest.Zero();
    //
	TInt r = DoControl( EMemSpyDriverOpCodeStackGetData, &params, NULL );
	//
	if  ( r >= KErrNone )
    	{
        aDest.SetLength( r );
    	aAmountRemaining = params.iRemaining;
        r = KErrNone;
    	}
	//
	return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetStackDataNext(TUint aTid, 
                                                    TDes8& aDest, 
                                                    TUint& aAmountRemaining, 
                                                    TMemSpyDriverDomainType aDomain, 
                                                    TBool aEntireStack )
    {
    TMemSpyDriverInternalStackDataParams params;
    params.iTid = aTid;
    params.iDes = &aDest;
    params.iDomain = aDomain;
    params.iEntireStack = aEntireStack;
    params.iRemaining = aAmountRemaining;
    //
	TInt r = DoControl( EMemSpyDriverOpCodeStackGetData, &params, NULL );
	//
	if  ( r >= KErrNone )
    	{
        aDest.SetLength( r );
    	aAmountRemaining = params.iRemaining;
        r = KErrNone;
    	}
	//
    return r;
    }































































EXPORT_C TInt RMemSpyDriverClient::EventMonitorOpen( TUint& aHandle )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeEventMonitorOpen, (TAny*) &aHandle );
    return error;
    }


EXPORT_C TInt RMemSpyDriverClient::EventMonitorClose( TUint aHandle )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeEventMonitorClose, (TAny*) aHandle );
    return error;
    }


EXPORT_C void RMemSpyDriverClient::EventMonitorNotify( TUint aHandle, TRequestStatus& aStatus, TUint& aContext )
    {
    aStatus = KRequestPending;
    //
    TMemSpyDriverInternalEventMonitorParams params;
    params.iHandle = aHandle;
    params.iStatus = &aStatus;
    params.iContext = (TAny*) &aContext;
    //
    const TInt err = DoControl( EMemSpyDriverOpCodeEventMonitorNotify, (TAny*) &params );
    if  ( err != KErrNone )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, err );
        }
    }


EXPORT_C void RMemSpyDriverClient::EventMonitorNotifyCancel( TUint aHandle )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeEventMonitorNotifyCancel, (TAny*) aHandle );
    (void) error;
    }



















EXPORT_C TInt RMemSpyDriverClient::ProcessInspectionOpen( TUint aPid )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeProcessInspectOpen, (TAny*) aPid );
    return error;
    }


EXPORT_C TInt RMemSpyDriverClient::ProcessInspectionClose( TUint aPid )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeProcessInspectClose, (TAny*) aPid );
    return error;
    }


EXPORT_C void RMemSpyDriverClient::ProcessInspectionRequestChanges(TUint aPid, 
                                                                   TRequestStatus& aStatus, 
                                                                   TMemSpyDriverProcessInspectionInfo& aInfo )
    {
    aInfo.iProcessId = aPid;
    aStatus = KRequestPending;
    const TInt err = DoControl( EMemSpyDriverOpCodeProcessInspectRequestChanges, (TAny*) &aStatus, (TAny*) &aInfo );
    if  ( err != KErrNone )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, err );
        }
    }


EXPORT_C void RMemSpyDriverClient::ProcessInspectionRequestChangesCancel( TUint aPid )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeProcessInspectRequestChangesCancel, (TAny*) aPid );
    (void) error;
    }


EXPORT_C void RMemSpyDriverClient::ProcessInspectionAutoStartItemsReset()
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeProcessInspectAutoStartListReset );
    (void) error;
    }


EXPORT_C TInt RMemSpyDriverClient::ProcessInspectionAutoStartItemsAdd( TUint aSID )
    {
    const TInt error = DoControl( EMemSpyDriverOpCodeProcessInspectAutoStartListAdd, (TAny*) aSID );
    return error;
    }






















EXPORT_C TInt RMemSpyDriverClient::GetContainerHandles(TMemSpyDriverContainerType aContainer, 
                                                       TAny** aHandleArray, 
                                                       TInt& aHandleCount)
    {
	TMemSpyDriverInternalContainerHandleParams params;
	params.iTidOrPid = KMemSpyDriverEnumerateContainerHandles;
	params.iContainer = aContainer;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
    params.iMaxCount = aHandleCount;
    params.iHandleSource = EMemSpyDriverThreadOrProcessTypeThread; // Not used
    //
    aHandleCount = 0;
	//
	return DoControl( EMemSpyDriverOpCodeContainersGetHandles, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetThreadHandlesByType(TInt aTid, 
                                                          TMemSpyDriverContainerType aType, 
                                                          TAny** aHandleArray, 
                                                          TInt& aHandleCount)
    {
	TMemSpyDriverInternalContainerHandleParams params;
	params.iTidOrPid = aTid;
	params.iContainer = aType;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
    params.iMaxCount = aHandleCount;
    params.iHandleSource = EMemSpyDriverThreadOrProcessTypeThread;
	//
    aHandleCount = 0;
    //
	return DoControl( EMemSpyDriverOpCodeContainersGetHandles, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetProcessHandlesByType(TInt aPid, 
                                                           TMemSpyDriverContainerType aType, 
                                                           TAny** aHandleArray, 
                                                           TInt& aHandleCount)
    {
	TMemSpyDriverInternalContainerHandleParams params;
	params.iTidOrPid = aPid;
	params.iContainer = aType;
	params.iHandles = aHandleArray;
	params.iCountPtr = &aHandleCount;
    params.iMaxCount = aHandleCount;
    params.iHandleSource = EMemSpyDriverThreadOrProcessTypeProcess;
	//
    aHandleCount = 0;
    //
	return DoControl( EMemSpyDriverOpCodeContainersGetHandles, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetGenericHandleInfo(TInt aTid, 
                                                        TMemSpyDriverContainerType aType, 
                                                        TAny* aHandle, 
                                                        TMemSpyDriverHandleInfoGeneric& aParams)
    {
    aParams.iType = aType;
    aParams.iHandle = aHandle;
    //
    const TInt r = DoControl( EMemSpyDriverOpCodeContainersGetHandleInfo, (TAny*) aTid, &aParams );
    return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetApproximateKernelObjectSize( TMemSpyDriverContainerType aType )
    {
    TInt size = 0;
    const TInt error = DoControl( EMemSpyDriverOpCodeContainersGetApproxSize, (TAny*) aType, (TAny*) &size );
    (void) error;
    //
    return size;
    }


EXPORT_C TInt RMemSpyDriverClient::GetReferencesToMyThread( TUint aTid )
    {
    ResetStreamBuffer();
	return DoControl( EMemSpyDriverOpCodeContainersGetReferencesToMyThread, (TAny*) aTid, (TAny*) &iBuffer );
    }


EXPORT_C TInt RMemSpyDriverClient::GetReferencesToMyProcess( TUint aPid )
    {
    ResetStreamBuffer();
	return DoControl( EMemSpyDriverOpCodeContainersGetReferencesToMyProcess, (TAny*) aPid, (TAny*) &iBuffer );
    }


EXPORT_C TInt RMemSpyDriverClient::GetPAndSInfo( TAny* aHandle, TMemSpyDriverPAndSInfo& aInfo )
    {
    const TInt r = DoControl( EMemSpyDriverOpCodeContainersGetPAndSInfo, (TAny*) aHandle, &aInfo );
    return r;
    }


EXPORT_C TInt RMemSpyDriverClient::GetCondVarSuspendedThreads(TAny* aCondVarHandle, 
                                                              TAny** aThreadHandleArray, 
                                                              TInt& aThreadCount)
    {
    TMemSpyDriverInternalCondVarSuspendedThreadParams params;
    params.iCondVarHandle = aCondVarHandle;
    params.iThrHandles = aThreadHandleArray;
    params.iThrCountPtr = &aThreadCount;
    params.iMaxCount = aThreadCount;
    //
    aThreadCount = 0;
    //
    return DoControl( EMemSpyDriverOpCodeContainersGetCondVarSuspendedThreads, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetCondVarSuspendedThreadInfo(TAny* aHandle, 
                                                                 TMemSpyDriverCondVarSuspendedThreadInfo& aParams)
    {
    return DoControl( EMemSpyDriverOpCodeContainersGetCondVarSuspendedThreadInfo, aHandle, &aParams );
    }

















EXPORT_C TInt RMemSpyDriverClient::GetServerSessionHandles(TAny* aServerHandle, 
                                                           TAny** aSessionHandleArray, 
                                                           TInt& aSessionHandleCount)
    {
	TMemSpyDriverInternalServerSessionHandleParams params;
	params.iServerHandle = aServerHandle;
	params.iSessionHandles = aSessionHandleArray;
	params.iSessionCountPtr = &aSessionHandleCount;
    params.iMaxCount = aSessionHandleCount;
    //
    aSessionHandleCount = 0;
	//
	return DoControl( EMemSpyDriverOpCodeClientServerGetServerSessionHandles, &params, NULL );
    }


EXPORT_C TInt RMemSpyDriverClient::GetServerSessionInfo(TAny* aSessionHandle, 
                                                        TMemSpyDriverServerSessionInfo& aParams)
    {
    return DoControl( EMemSpyDriverOpCodeClientServerGetServerSessionInfo, aSessionHandle, &aParams );
    }











EXPORT_C RMemSpyMemStreamReader RMemSpyDriverClient::StreamOpenL()
    {
    CMemSpyMemStreamReader* imp = new(ELeave) CMemSpyMemStreamReader( iBuffer );
    RMemSpyMemStreamReader ret( imp );
    CleanupClosePushL( ret ); 
    imp->ConstructL();
    CleanupStack::Pop( &ret );
    return ret;
    }













EXPORT_C TMemSpyMemoryModelType RMemSpyDriverClient::MemoryModelType()
    {
    TMemSpyMemoryModelType ret = EMemSpyMemoryModelTypeUnknown;
    //
    const TInt err = DoControl( EMemSpyDriverOpCodeMiscGetMemoryModelType );
    switch( err )
        {
    default:
    case EMemSpyMemoryModelTypeUnknown:
        ret = EMemSpyMemoryModelTypeUnknown;
        break;
    case EMemSpyMemoryModelTypeMoving:
        ret = EMemSpyMemoryModelTypeMoving;
        break;
    case EMemSpyMemoryModelTypeMultiple:
        ret = EMemSpyMemoryModelTypeMultiple;
        break;
    case EMemSpyMemoryModelTypeEmulator:
        ret = EMemSpyMemoryModelTypeEmulator;
        break;
        }
    //
    return ret;
    }


EXPORT_C TUint32 RMemSpyDriverClient::RoundToPageSize( TUint32 aValue )
    {
    TUint32 temp = aValue;
    TAny* pValue = (TAny*) &temp;
    DoControl( EMemSpyDriverOpCodeMiscGetRoundToPageSize, pValue );
    return temp;
    }


EXPORT_C TInt RMemSpyDriverClient::Impersonate( TUint32 aValue )
    {
    return DoControl( EMemSpyDriverOpCodeMiscImpersonate, (TAny*) aValue );
    }


























TUint RMemSpyDriverClient::RHeapVTable()
    {
    RHeap* heap = (RHeap*) &User::Allocator();
    //
    TUint* pHeap = (TUint*) heap;
    const TUint heapVTable = *pHeap;
    //
    ////RDebug::Printf( "[MemSpy] RMemSpyDriverClient::RHeapVTable() - ret: 0x%08x", heapVTable );
    return heapVTable;
    }


TBool RMemSpyDriverClient::DebugEUser()
    {
	LtkUtils::RAllocatorHelper allocHelper;
	TBool result = EFalse;
	TInt err = allocHelper.Open(&User::Allocator());
	if (!err)
		{
		result = allocHelper.AllocatorIsUdeb();
		allocHelper.Close();
		}
	return result;
    }


void RMemSpyDriverClient::ResetStreamBuffer()
    {
    iBuffer.Zero();
    }


void RMemSpyDriverClient::ReadHeapInfoFreeCellsFromXferBufferL( RArray<TMemSpyDriverFreeCell>& aFreeCells )
    {
    aFreeCells.Reset();

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] RMemSpyDriverClient::ReadHeapInfoFreeCellsFromXferBufferL() - buf len: %d", iBuffer.Length() );
#endif

    if  ( iBuffer.Length() )
        {
        RMemSpyMemStreamReader stream = StreamOpenL();
        CleanupClosePushL( stream );

        const TInt count = stream.ReadInt32L();
#ifdef _DEBUG
        RDebug::Printf( "[MemSpy] RMemSpyDriverClient::ReadHeapInfoFreeCellsFromXferBufferL() - count: %d", count );
#endif

        for( TInt i=0; i<count; i++ )
            {
            TMemSpyDriverFreeCell entry;
            entry.iType = (TMemSpyDriverCellType)stream.ReadInt32L();
            entry.iAddress = reinterpret_cast< TAny* >( stream.ReadUint32L() );
            entry.iLength = stream.ReadInt32L();
            aFreeCells.AppendL( entry );
            }

        CleanupStack::PopAndDestroy( &stream );
        }

#ifdef _DEBUG
        RDebug::Printf( "[MemSpy] RMemSpyDriverClient::ReadHeapInfoFreeCellsFromXferBufferL() - END" );
#endif

    ResetStreamBuffer();
    }












static void PrintHeapInfo( const TMemSpyHeapInfo& aInfo )
    {
#if defined( _DEBUG ) && !defined( __WINS__ )
    const TMemSpyHeapInfoRHeap& rHeapInfo = aInfo.AsRHeap();
    //const TMemSpyHeapObjectDataRHeap& rHeapObjectData = rHeapInfo.ObjectData();
    const TMemSpyHeapStatisticsRHeap& rHeapStats = rHeapInfo.Statistics();
    const TMemSpyHeapMetaDataRHeap& rHeapMetaData = rHeapInfo.MetaData();

	/*
	 * TODO update to reflect new memory allocator structs etc
	 * 
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator                                      -");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator::iAccessCount:       %d", rHeapObjectData.iAccessCount);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator::iHandleCount:       %d", rHeapObjectData.iHandleCount);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator::iHandles:           0x%08x", rHeapObjectData.iHandles);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator::iFlags:             0x%08x", rHeapObjectData.iFlags);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator::iCellCount:         %d", rHeapObjectData.iCellCount);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RAllocator::iTotalAllocSize:    %d", rHeapObjectData.iTotalAllocSize);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - ");

    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap                                           -");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iMinLength:              %d", rHeapObjectData.iMinLength);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iMaxLength:              %d", rHeapObjectData.iMaxLength);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iOffset:                 %d", rHeapObjectData.iOffset);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iGrowBy:                 %d", rHeapObjectData.iGrowBy);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iChunkHandle:            0x%08x", rHeapObjectData.iChunkHandle);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iBase:                   0x%08x", rHeapObjectData.iBase);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iTop:                    0x%08x", rHeapObjectData.iTop);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iAlign:                  %d", rHeapObjectData.iAlign);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iMinCell:                %d", rHeapObjectData.iAlign);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iPageSize:               %d", rHeapObjectData.iAlign);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iFree.next:              0x%08x", rHeapObjectData.iFree.next);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iFree.len:               %d", rHeapObjectData.iFree.len);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iNestingLevel:           %d", rHeapObjectData.iNestingLevel);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iAllocCount:             %d", rHeapObjectData.iAllocCount);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iFailType:               %d", rHeapObjectData.iFailType);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iFailRate:               %d", rHeapObjectData.iFailRate);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iFailed:                 %d", rHeapObjectData.iFailed);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iFailAllocCount:         %d", rHeapObjectData.iFailAllocCount);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iRand:                   %d", rHeapObjectData.iRand);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - RHeap::iTestData:               0x%08x", rHeapObjectData.iTestData);
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - ");
	*/

    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - Stats (Free)                                    -");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell count:                     %d", rHeapStats.StatsFree().TypeCount());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell size:                      %d", rHeapStats.StatsFree().TypeSize());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell largest:                   0x%08x", rHeapStats.StatsFree().LargestCellAddress());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell largest size:              %d", rHeapStats.StatsFree().LargestCellSize());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - slack:                          0x%08x", rHeapStats.StatsFree().SlackSpaceCellAddress());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - slack size:                     %d", rHeapStats.StatsFree().SlackSpaceCellSize());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - checksum:                       0x%08x", rHeapStats.StatsFree().Checksum());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - ");

    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - Stats (Alloc)                                   -");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell count:                     %d", rHeapStats.StatsAllocated().TypeCount());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell size:                      %d", rHeapStats.StatsAllocated().TypeSize());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell largest:                   0x%08x", rHeapStats.StatsAllocated().LargestCellAddress());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell largest size:              %d", rHeapStats.StatsAllocated().LargestCellSize());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - ");

    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - Misc. Info                                      -");
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() ---------------------------------------------------");
    const TPtrC chunkName( rHeapMetaData.ChunkName() );
    RDebug::Print(_L("RMemSpyDriverClient::PrintHeapInfo() - chunk name:                     [%S]"), &chunkName );
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - chunk size:                     %d", rHeapMetaData.ChunkSize());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - chunk handle:                   0x%08x", rHeapMetaData.ChunkHandle());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - chunk base address:             0x%08x", rHeapMetaData.ChunkBaseAddress());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - debug allocator:                %d", rHeapMetaData.IsDebugAllocator());
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - shared heap:                    %d", rHeapMetaData.IsSharedHeap() );
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - user thread:                    %d", rHeapMetaData.IsUserThread() );
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - thread id:                      %d", aInfo.Tid() );
    RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - process id:                     %d", aInfo.Pid() );
    //RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell header size (free):        %d", rHeapMetaData.HeaderSizeFree());
    //RDebug::Printf("RMemSpyDriverClient::PrintHeapInfo() - cell header size (alloc):       %d", rHeapMetaData.HeaderSizeAllocated());
#else
    (void) aInfo;
#endif
    }

