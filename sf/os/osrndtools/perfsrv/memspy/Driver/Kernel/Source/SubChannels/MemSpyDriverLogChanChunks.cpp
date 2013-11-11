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

#include "MemSpyDriverLogChanChunks.h"

// System includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"

// Constants
_LIT8( KMemSpyLitDllDollarData, "DLL$DATA" );
_LIT8( KMemSpyLitDollarDat, "$DAT" );
_LIT8( KMemSpyLitDollarCode, "$CODE" );
_LIT8( KMemSpyLitDollarGlobalCode, "GLOBAL$CODE" );
_LIT8( KMemSpyLitLocalObject, "Local-" );


DMemSpyDriverLogChanChunks::DMemSpyDriverLogChanChunks( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::DMemSpyDriverLogChanChunks() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanChunks::~DMemSpyDriverLogChanChunks()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::~DMemSpyDriverLogChanChunks() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::~DMemSpyDriverLogChanChunks() - END - this: 0x%08x", this ));
	}



TInt DMemSpyDriverLogChanChunks::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
        case EMemSpyDriverOpCodeChunkGetHandles:
            r = GetChunkHandles( (TMemSpyDriverInternalChunkHandleParams*) a1 );
            break;
        case EMemSpyDriverOpCodeChunkGetInfo:
            r = GetChunkInfo( (TMemSpyDriverInternalChunkInfoParams*) a1 );
            break;

        default:
            r = KErrNotSupported;
		    break;
    		}
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanChunks::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeChunkBase && aFunction < EMemSpyDriverOpCodeChunkEnd );
    }




















TInt DMemSpyDriverLogChanChunks::GetChunkHandles( TMemSpyDriverInternalChunkHandleParams* aParams )
    {
	TMemSpyDriverInternalChunkHandleParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalChunkHandleParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkHandles() - END - params read error: %d", r));
        return r;
        }

	const TInt maxCount = params.iMaxCount;
	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkHandles() - START - id: %d,  maxCount: %d, type: %d", params.iId, maxCount, params.iType));

    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = OSAdaption().DChunk();
    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();
    DMemSpyDriverOSAdaptionDProcess& processAdaption = OSAdaption().DProcess();


    // This variable holds the number of handles that we have already
    // written to the client-side.
    TInt currentWriteIndex = 0;
	
    if  ( params.iType == EMemSpyDriverPrivateObjectTypeProcess || params.iType == EMemSpyDriverPrivateObjectTypeThread )
        {
        if  ( params.iType == EMemSpyDriverPrivateObjectTypeThread )
    	    {
	        r = OpenTempObject( params.iId, EThread );
            if  ( r == KErrNone )
                {
                // Open the owning process instead, so that we can see which chunks are mapped
                // into the thread.
                DThread* thread = (DThread*) TempObject();
                DProcess* process = threadAdaption.GetOwningProcess( *thread );
                if  ( process )
                    {
                    const TUint parentProcessId = processAdaption.GetId( *process );
                    CloseTempObject();
                    r = OpenTempObject( parentProcessId, EProcess );
                    }
                else
                    {
                    CloseTempObject();
                    r = KErrNotFound;
                    }
                }
       	    }
        else
            {
	        r = OpenTempObject( params.iId, EProcess );
            }

        // Handle error opening correct process
        if (r != KErrNone)
    	    {
    	    Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkHandles() - END - parent process not found");
    	    return r;
    	    }
   
        DProcess* process = (DProcess*) TempObject();
	    NKern::ThreadEnterCS();

	    // Iterate through each handle in the process
	    MemSpyObjectIx* processHandles = processAdaption.GetHandles( *process );
		MemSpyObjectIx_HandleLookupLock();
        const TInt processHandleCount = processHandles->Count();
		MemSpyObjectIx_HandleLookupUnlock();

	    for( TInt processHandleIndex = 0; processHandleIndex<processHandleCount && r == KErrNone && currentWriteIndex < maxCount; processHandleIndex++ )
    	    {
    	    // Get a handle from the process container...
            MemSpyObjectIx_HandleLookupLock();
			if (processHandleIndex >= processHandles->Count()) break; // Count may have changed in the meantime
    	    DObject* object = (*processHandles)[ processHandleIndex ];
			if (object && object->Open() != KErrNone) object = NULL;
			MemSpyObjectIx_HandleLookupUnlock();
            
            if  ( object )
                {
                const TObjectType objectType = processAdaption.GetObjectType( *object );
                if ( objectType == EChunk )
                    {
                    DChunk* chunk = (DChunk*) object;
                    TAny* handle = (TAny*) chunk;
                    r = Kern::ThreadRawWrite( &ClientThread(), params.iHandles + currentWriteIndex, &handle, sizeof(TAny*) );
                    if  ( r == KErrNone )
                        {
                        ++currentWriteIndex;
                        }
                    }
				object->Close(NULL);
                }
    	    }

        // If we were asked for process-related chunks, also check the chunk container
        // for entries which we don't have handles to, but do refer to our process
        // Need a listing of all chunks in the system. Let client filter duplicates.
        DObjectCon* container = Kern::Containers()[ EChunk ];
        container->Wait();
        //
        const TInt count = container->Count();
        for( TInt i=0; i<count && r == KErrNone && currentWriteIndex < maxCount; i++ )
            {
            DChunk* chunk= (DChunk*) (*container)[ i ];
            //
            const TBool isRelated = DoesChunkRelateToProcess( *chunk, TempObjectAsProcess() );
            if  ( isRelated )
                {
                r = Kern::ThreadRawWrite( &ClientThread(), params.iHandles + currentWriteIndex, &chunk, sizeof(TAny*) );
                if  ( r == KErrNone )
                    {
                    ++currentWriteIndex;
                    }
                }
            }
        //
        container->Signal();
        NKern::ThreadLeaveCS();

        CloseTempObject();
        }
    else
        {
        // Need a listing of all chunks in the system. Let client filter duplicates.
        DObjectCon* container = Kern::Containers()[ EChunk ];
        NKern::ThreadEnterCS();
        container->Wait();
        //
        const TInt count = container->Count();
        for( TInt i=0; i<count && r == KErrNone && currentWriteIndex < maxCount; i++ )
            {
            DChunk* chunk= (DChunk*) (*container)[ i ];
            //
            r = Kern::ThreadRawWrite( &ClientThread(), params.iHandles + currentWriteIndex, &chunk, sizeof(TAny*) );
            if  (r == KErrNone)
                {
                ++currentWriteIndex;
                }
            }
        //
        container->Signal();
        NKern::ThreadLeaveCS();
        }
	
	if  ( r == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }
    else
        {
        const TInt finalWrite = Kern::ThreadRawWrite( &ClientThread(), params.iCountPtr, &currentWriteIndex, sizeof(TInt) );
        if  ( r == KErrNone )
            {
            r = finalWrite;
            }
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkHandles() - END - number of handles written to client: %d, ret: %d", currentWriteIndex, r));
	return r;
    }


TInt DMemSpyDriverLogChanChunks::GetChunkInfo( TMemSpyDriverInternalChunkInfoParams* aParams )
    {
	TMemSpyDriverInternalChunkInfoParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalChunkInfoParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkInfo() - END - params read error: %d", r));
        return r;
        }

    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkInfo() - START - handle: 0x%08x", params.iHandle));
		
	DObjectCon* container = Kern::Containers()[EChunk];
	NKern::ThreadEnterCS();

    container->Wait();
    const TInt count = container->Count();

    DChunk* foundChunk = NULL;
    
    for(TInt i=0; i<count; i++)
        {
        DChunk* chunk = (DChunk*) (*container)[i];
        if  ( chunk == params.iHandle )
            {
            foundChunk = chunk;
            TRACE( PrintChunkInfo( *chunk ) );
			r = foundChunk->Open();
            break;
            }
        }

    container->Signal();

    if  ( foundChunk == NULL )
        {
    	Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkInfo() - END - KErrNotFound - couldnt find chunk");
		NKern::ThreadLeaveCS();
        return KErrNotFound;
        }
	if (r)
		{
		Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkInfo() - END - %d - Failed to open chunk", r);
		NKern::ThreadLeaveCS();
		return r;
		}

    // Prepare return data
    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = OSAdaption().DChunk();
    //
    params.iBaseAddress = chunkAdaption.GetBase( *foundChunk );
    params.iSize = chunkAdaption.GetSize( *foundChunk );
    params.iMaxSize = chunkAdaption.GetMaxSize( *foundChunk );
    foundChunk->FullName( params.iName );

    // Mirror the process memory tracker
    DProcess* owner = chunkAdaption.GetOwningProcess( *foundChunk );
    if  ( owner )
        {
        params.iOwnerId = OSAdaption().DProcess().GetId( *owner );
        }
    else
        {
        owner = static_cast< DProcess* >( chunkAdaption.GetOwner( *foundChunk, EProcess ) );
        if  ( owner )
            {
            params.iOwnerId = OSAdaption().DProcess().GetId( *owner );
            }
        else
            {
            params.iOwnerId = chunkAdaption.GetControllingOwnerId( *foundChunk );
            }
        }

    // Get type & attribs
    params.iType = IdentifyChunkType( *foundChunk );
    params.iAttributes = chunkAdaption.GetAttributes( *foundChunk );

	// Finished with foundChunk
	foundChunk->Close(NULL);
	NKern::ThreadLeaveCS();
    
    // Write back to client
    r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalChunkInfoParams) );
	if  ( r == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::GetChunkInfo() - END - handle: 0x%08x, params.iOwnerId: %d, r: %d", params.iHandle, params.iOwnerId, r ));
    return r;
    }


void DMemSpyDriverLogChanChunks::PrintChunkInfo( DChunk& aChunk )
    {
    MemSpyDriverUtils::PrintChunkInfo( aChunk, OSAdaption() );
    }


TMemSpyDriverChunkType DMemSpyDriverLogChanChunks::IdentifyChunkType( DChunk& aChunk )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - START" ) );

    TMemSpyDriverChunkType ret = EMemSpyDriverChunkTypeUnknown;

    TName name;
    aChunk.Name( name );
    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - name: %S", &name ) );

    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = OSAdaption().DChunk();
    const TChunkType type = chunkAdaption.GetType( aChunk );

    if  ( name == KMemSpyLitDllDollarData )
        {
        // This chunk contains Dll Global Data for the process
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeGlobalData" ) );
        ret = EMemSpyDriverChunkTypeGlobalData;
        }
    else if ( type == ERamDrive )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeRamDrive" ) );
        ret = EMemSpyDriverChunkTypeRamDrive;
        }
    else if ( type == EKernelStack )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeStackKernel" ) );
        ret = EMemSpyDriverChunkTypeStackKernel;
        }
    else if ( name == KMemSpyLitDollarDat )
        {
        // This chunk contains process global data as well as user-side stacks for
        // the process. 
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeStackAndProcessGlobalData" ) );
        ret = EMemSpyDriverChunkTypeStackAndProcessGlobalData;
        }
    else if ( name == KMemSpyLitDollarGlobalCode && type == EDll )
        {
        // GLOBAL$CODE is used for RAM loaded code which is globally visible. This
        // basically means locale DLLs - these must be visible to every process, even
        // those which haven't loaded them.        
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeCodeGlobal" ) );
        ret = EMemSpyDriverChunkTypeCodeGlobal;
        }
    else if ( name == KMemSpyLitDollarCode || type == EKernelCode || type == EDll || type == EUserCode )
        {
        // RAM-loaded code, which on the multiple memory model at least means that the code chunk is eseentially just a mapping
        // artifact. The RAM itself is owned by the code segment, therefore counting the size of these CODE elements may result
        // in inaccurate results if the code is shared amongst multiple processes.
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeCode" ) );
        ret = EMemSpyDriverChunkTypeCode;
        }
    else if ( type == EUserSelfModCode )
        {
        // Dynamically create code chunk
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeCodeSelfModifiable" ) );
        ret = EMemSpyDriverChunkTypeCodeSelfModifiable;
        }
    else if ( IsHeapChunk( aChunk, name ) )
        {
        // Catch kernel heap too
        if  ( type == EKernelData )
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeHeapKernel" ) );
            ret = EMemSpyDriverChunkTypeHeapKernel;
            }
        else
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeHeap" ) );
            ret = EMemSpyDriverChunkTypeHeap;
            }
        }
    else if ( type == EUserData && chunkAdaption.GetOwningProcess( aChunk ) == NULL )
        {
        // Global shared chunks match this pattern. Of course, we could check the memory model mapping attributes
        // as that would give us the info in a heartbeat, but it's too specific.
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeGlobal" ) );
        ret = EMemSpyDriverChunkTypeGlobal;
        }
    else if ( type == EUserData && chunkAdaption.GetOwner( aChunk ) != NULL && name.Length() > KMemSpyLitLocalObject().Length() && name.Left( KMemSpyLitLocalObject().Length() ) == KMemSpyLitLocalObject )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeLocal" ) );
        ret = EMemSpyDriverChunkTypeLocal;
        }
    else
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IdentifyChunkType() - EMemSpyDriverChunkTypeUnknown" ) );
        TRACE( PrintChunkInfo( aChunk ) );
        ret = EMemSpyDriverChunkTypeUnknown;
        }

    return ret;
    }


TBool DMemSpyDriverLogChanChunks::IsHeapChunk( DChunk& aChunk, const TName& aName )
    {
    (void) aName; // UREL warning
    const TUint rHeapVTable = MemSpyDevice().RHeapVTable();
    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - START - this: 0x%08x, aChunk: 0x%08x, RHeapVTable: 0x%08x, aName: %S, [%O]", this, &aChunk, rHeapVTable, &aName, &aChunk ) );

    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = OSAdaption().DChunk();
    DMemSpyDriverOSAdaptionDProcess& processAdaption = OSAdaption().DProcess();
    
    // The first 4 bytes of every chunk correspond to the allocator VTable (For heap chunks).
    // If it matches RHeap's vtable, we'll treat it as a heap.
    TBool isHeap = EFalse;

    // There must be an owning process or else it's definitely not a heap chunk.
    DProcess* process = chunkAdaption.GetOwningProcess( aChunk );
    TUint8* base = chunkAdaption.GetBase( aChunk );
    const TInt size = chunkAdaption.GetSize( aChunk );
    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - base: 0x%08x, size: %d, process: 0x%08x (%O)", base, size, process, process ) );

    if  ( process && size >= 4 )
        {
		NKern::ThreadEnterCS();
        // Chunks are mapped into entire process so any thread within the process is enough...
        DThread* firstThread = processAdaption.OpenFirstThread( *process );
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - firstThread: 0x%08x (%O)", firstThread, firstThread ) );
        if  ( firstThread != NULL )
            {
            TBuf8<4> allocatorVTableBuffer;
            TInt err = Kern::ThreadRawRead( firstThread, base, (TUint8*) allocatorVTableBuffer.Ptr(), allocatorVTableBuffer.MaxLength() );
            TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk - read result of vtable data from requested thread is: %d", err ));
            //
            if  ( err == KErrNone )
                {
                TRACE( MemSpyDriverUtils::DataDump("possible chunk vtable data - %lS", allocatorVTableBuffer.Ptr(), allocatorVTableBuffer.MaxLength(), allocatorVTableBuffer.MaxLength() ) );
                allocatorVTableBuffer.SetLength( allocatorVTableBuffer.MaxLength() );
                
                const TUint32 vtable =   allocatorVTableBuffer[0] +
                                        (allocatorVTableBuffer[1] << 8) + 
                                        (allocatorVTableBuffer[2] << 16) + 
                                        (allocatorVTableBuffer[3] << 24);
                TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk - [possible] vTable within chunk is: 0x%08x", vtable) );

                // Check the v-table to work out if it really is an RHeap
                isHeap = ( vtable == rHeapVTable );
                TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - isHeap: %d", isHeap ) );
                }

            TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - closing first thread..." ) );
            Kern::SafeClose( (DObject*&) firstThread, NULL );
            }
		NKern::ThreadLeaveCS();
        }

    /* We only want RHeap's at the moment
    if  ( !isHeap && aName == KMemSpyLitDollarHeap )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - its called $HEAP, but its not an RHeap... we\'ll let it through though..." ) );
        isHeap = ETrue;
        }
    */

    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::IsHeapChunk() - END - this: 0x%08x, isHeap: %d", this, isHeap ) );
    return isHeap;
    }


TBool DMemSpyDriverLogChanChunks::DoesChunkRelateToProcess( DChunk& aChunk, DProcess& aProcess )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::DoesChunkRelateToProcess() - START - this: 0x%08x, chunk: 0x%08x (%O), process: 0x%08x (%O)", this, &aChunk, &aChunk, &aProcess, &aProcess ) );
    TBool ret = EFalse;
    //
    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = OSAdaption().DChunk();
    DMemSpyDriverOSAdaptionDProcess& processAdaption = OSAdaption().DProcess();
    //
    const TUint pid = processAdaption.GetId( aProcess );
    DProcess* process = chunkAdaption.GetOwningProcess( aChunk );
    if  ( process )
        {
        ret = ( pid == processAdaption.GetId( *process ) );
        }
    else
        {
        DObject* owner = chunkAdaption.GetOwner( aChunk, EProcess );
        if  ( owner )
            {
            process = (DProcess*) owner;
            ret = ( pid == processAdaption.GetId( *process ) );
            }
        else
            {
            ret = ( pid == chunkAdaption.GetControllingOwnerId( aChunk ) );
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::DoesChunkRelateToProcess() - END - this: 0x%08x, chunk: 0x%08x (%O), process: 0x%08x (%O), ret: %d", this, &aChunk, &aChunk, &aProcess, &aProcess, ret ) );
    return ret;
    }

