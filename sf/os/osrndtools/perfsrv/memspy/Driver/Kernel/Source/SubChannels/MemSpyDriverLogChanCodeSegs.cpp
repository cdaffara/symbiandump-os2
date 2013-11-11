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

#include "MemSpyDriverLogChanCodeSegs.h"

// System includes
#include <plat_priv.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverOSAdaption.h"



DMemSpyDriverLogChanCodeSegs::DMemSpyDriverLogChanCodeSegs( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::DMemSpyDriverLogChanCodeSegs() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanCodeSegs::~DMemSpyDriverLogChanCodeSegs()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::~DMemSpyDriverLogChanCodeSegs() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::~DMemSpyDriverLogChanCodeSegs() - END - this: 0x%08x", this ));
	}



TInt DMemSpyDriverLogChanCodeSegs::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeCodeSegsGetAll:
		    r = GetCodeSegs( (TMemSpyDriverInternalCodeSnapshotParams*) a1 );
		    break;
	    case EMemSpyDriverOpCodeCodeSegsGetCodeSegsForProcess:
		    r = GetCodeSegsForProcess( (TMemSpyDriverInternalCodeSnapshotParams*) a1 );
		    break;
	    case EMemSpyDriverOpCodeCodeSegsGetCodeSegInfo:
		    r = GetCodeSegInfo( (TMemSpyDriverInternalCodeSegParams*) a1 );
		    break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanCodeSegs::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeCodeSegsBase && aFunction < EMemSpyDriverOpCodeCodeSegsEnd );
    }
















TInt DMemSpyDriverLogChanCodeSegs::GetCodeSegs( TMemSpyDriverInternalCodeSnapshotParams* aParams )
	{
    TMemSpyDriverInternalCodeSnapshotParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalCodeSnapshotParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess() - END - params read error: %d", r));
        return r;
        }

    DMemSpyDriverOSAdaptionDCodeSeg& codeSegAdaption = OSAdaption().DCodeSeg();

	const TInt maxCount = params.iMaxCount;
	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess() - START - RAM-only: %d, maxCount: %d", params.iFilter, maxCount));
	
    // This is the number of items we have written to the client
    TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs() - START - maxCount: %d", maxCount));
    TInt clientWriteCount = 0;

    // Need to get the code segs for each process. We'll let the client worry about filtering
    // duplicates.
    DObjectCon* container = Kern::Containers()[ EProcess ];
	Kern::AccessCode();
    container->Wait();
	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 1"));
    //
    TFullName name;
    const TInt count = container->Count();
	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 2, count: %d", count));
    for(TInt i=0; i<count && r == KErrNone; i++)
        {
        DProcess* process = (DProcess*) (*container)[ i ];
        process->Name( name );
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 3, proc: %O", process));

        SDblQue queue;
        //
        const TInt clientSpaceRemaining = maxCount - clientWriteCount;
        const TInt numberOfCodeSegs = codeSegAdaption.GetCodeSegQueue( *process, queue );
        const TInt numberOfCodeSegsToWriteToClient = Min( numberOfCodeSegs, clientSpaceRemaining );
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 4, clientSpaceRemaining: %d", clientSpaceRemaining));
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 5, numberOfCodeSegs: %d", numberOfCodeSegs));
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 6, numberOfCodeSegsToWriteToClient: %d", numberOfCodeSegsToWriteToClient));
        //
	    SDblQueLink* link = queue.iA.iNext;
	    r = KErrNone;
        //
	    for( TInt j=0; j<numberOfCodeSegsToWriteToClient && r == KErrNone; ++j, link = link->iNext )
		    {
		    DCodeSeg* codeSegment = codeSegAdaption.GetCodeSeg( link );

            // If the client only wants RAM-loaded codesegs, then ignore XIP.
            TBool writeEntryToClient = ETrue;
            if  ( params.iFilter )
                {
                const TBool isXIP = codeSegAdaption.GetIsXIP( *codeSegment );
                writeEntryToClient = ( !isXIP );
                }

            // Now write
            if  ( writeEntryToClient )
                {
                r = Kern::ThreadRawWrite( &ClientThread(), params.iHandles + clientWriteCount, &codeSegment, sizeof(TAny*) );
    	        TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 7, codeSegment: 0x%08x to client: %d", codeSegment, r));
                if  ( r == KErrNone )
                    {
                    ++clientWriteCount;
                    }
                }
		    }
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 8"));
        codeSegAdaption.EmptyCodeSegQueue( queue );

    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 9"));
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 10"));
    container->Signal();
	Kern::EndAccessCode();

    TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs - 11 - r: %d", r));
    if  ( r == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }
    else
        {
        const TInt finalWrite = Kern::ThreadRawWrite( &ClientThread(), params.iCountPtr, &clientWriteCount, sizeof(TInt) );
        if  ( r == KErrNone )
            {
            r = finalWrite;
            }
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegs() - END - wrote %d handles to client, r: %d", clientWriteCount, r));
	return r;
	}


TInt DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess( TMemSpyDriverInternalCodeSnapshotParams* aParams )
	{
    TMemSpyDriverInternalCodeSnapshotParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalCodeSnapshotParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess() - END - params read error: %d", r));
        return r;
        }

    DMemSpyDriverOSAdaptionDCodeSeg& codeSegAdaption = OSAdaption().DCodeSeg();

    const TInt maxCount = params.iMaxCount;
	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess() - START - pid: %d, maxCount: %d", params.iFilter, maxCount));
	
	r = OpenTempObject( params.iFilter, EProcess );
	if (r != KErrNone)
		{
		Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess() - END - process not found");
		return r;
		}
		
	DProcess* pP = (DProcess*) TempObject();

	SDblQue q;

    Kern::AccessCode();
    const TInt actcount = codeSegAdaption.GetCodeSegQueue( *pP, q );

    CloseTempObject();

	TInt n = Min(actcount, maxCount);
	SDblQueLink* pL = q.iA.iNext;
	r = KErrNone;
    //
	for (TInt i=0; i<n; ++i, pL = pL->iNext)
		{
	    DCodeSeg* pS = codeSegAdaption.GetCodeSeg( pL );
		
        r = Kern::ThreadRawWrite( &ClientThread(), params.iHandles + i, &pS, sizeof(TAny*) );
		if  ( r != KErrNone )
            {
			break;
            }
		}
    codeSegAdaption.EmptyCodeSegQueue( q );
	Kern::EndAccessCode();

	if  (r == KErrBadDescriptor)
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }
    else
        {
        const TInt finalWrite = Kern::ThreadRawWrite( &ClientThread(), params.iCountPtr, &actcount, sizeof(TInt) );
        if  ( r == KErrNone )
            {
            r = finalWrite;
            }
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegsForProcess() - END - act count: %d, r: %d", actcount, r));
	return r;
	}


TInt DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo( TMemSpyDriverInternalCodeSegParams* aParams )
	{
    TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - START"));
	TMemSpyDriverInternalCodeSegParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalCodeSegParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - END - params read error: %d", r));
        return r;
        }

    TMemSpyDriverCodeSegInfo codeSegInfo;
    codeSegInfo.iSize = 0;
    
    // Create info
    codeSegInfo.iCreateInfo.iFileName.Zero();
    codeSegInfo.iCreateInfo.iCodeSize = 0;
    codeSegInfo.iCreateInfo.iTextSize = 0;
    codeSegInfo.iCreateInfo.iDataSize = 0;
    codeSegInfo.iCreateInfo.iBssSize = 0;
    codeSegInfo.iCreateInfo.iTotalDataSize = 0;
  
    // Memory Info
    codeSegInfo.iMemoryInfo.iCodeBase = 0;
    codeSegInfo.iMemoryInfo.iCodeSize = 0;
    codeSegInfo.iMemoryInfo.iConstDataBase = 0;
    codeSegInfo.iMemoryInfo.iConstDataSize = 0;
    codeSegInfo.iMemoryInfo.iInitialisedDataBase = 0;
    codeSegInfo.iMemoryInfo.iInitialisedDataSize = 0;
    codeSegInfo.iMemoryInfo.iUninitialisedDataBase = 0;
    codeSegInfo.iMemoryInfo.iUninitialisedDataSize = 0;

	DProcess* process = NULL;
    DMemSpyDriverOSAdaptionDCodeSeg& codeSegAdaption = OSAdaption().DCodeSeg();

	r = KErrNotFound;
	Kern::AccessCode();
	DCodeSeg* codeSeg = codeSegAdaption.GetCodeSegFromHandle( params.iHandle );
    TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - codeSeg: 0x%08x, handle: 0x%08x", codeSeg, params.iHandle ));

    if  ( codeSeg != NULL )
		{
#ifdef __EPOC32__
        TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - calling Kern::CodeSegGetMemoryInfo for code seg: %C...", codeSeg ) );
        Kern::CodeSegGetMemoryInfo( *codeSeg, codeSegInfo.iMemoryInfo, process );
        r = KErrNone;
        TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - called Kern::CodeSegGetMemoryInfo, r: %d", r ) );
#else
        r = KErrNone;
#endif
        //
		if  ( r == KErrNone )
			{
            TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - calling Info...") );
            codeSegAdaption.GetCreateInfo( *codeSeg, codeSegInfo.iCreateInfo );
            TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - called info" ) );
            
            TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - codeSegInfo.iMemoryInfo.iCodeBase: 0x%08x, codeSegInfo.iMemoryInfo.iCodeSize: %8d", codeSegInfo.iMemoryInfo.iCodeBase, codeSegInfo.iMemoryInfo.iCodeSize ) );
            codeSegInfo.iSize = codeSegAdaption.GetSize( *codeSeg );

            TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - doing write to user land..." ) );
            r = Kern::ThreadRawWrite( &ClientThread(), params.iInfoPointer, &codeSegInfo, sizeof(TMemSpyDriverCodeSegInfo) );
            TRACE( Kern::Printf("DMemSpyDriverLogChanCodeSegs::GetCodeSegInfo() - done write to user-land, r: %d", r ) );
			}
		}
	Kern::EndAccessCode();
    //
	if  ( r == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }
    //
	return r;
	}

