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

#include "MemSpyDriverLogicalChannel.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include <memspy/driver/memspydriverconstants.h>
#include "MemSpyDriverLogChanChunks.h"
#include "MemSpyDriverLogChanClientServer.h"
#include "MemSpyDriverLogChanCodeSegs.h"
#include "MemSpyDriverLogChanContainers.h"
#include "MemSpyDriverLogChanHeapDataUser.h"
#include "MemSpyDriverLogChanHeapDataKernel.h"
#include "MemSpyDriverLogChanHeapWalkUser.h"
#include "MemSpyDriverLogChanMisc.h"
#include "MemSpyDriverLogChanProcessInspection.h"
#include "MemSpyDriverLogChanStack.h"
#include "MemSpyDriverLogChanRawMemory.h"
#include "MemSpyDriverLogChanUserEventMonitor.h"
#include "MemSpyDriverLogChanThreadAndProcess.h"


DMemSpyDriverLogicalChannel::DMemSpyDriverLogicalChannel()
    {
    }


DMemSpyDriverLogicalChannel::~DMemSpyDriverLogicalChannel()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::~DMemSpyDriverLogicalChannel() - START"));

    NKern::ThreadEnterCS();
    SubChannelsDestroy();

    TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::~DMemSpyDriverLogicalChannel() - closing client thread..."));
    Kern::SafeClose( (DObject*&) iClientThread, NULL );
    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::~DMemSpyDriverLogicalChannel() - calling device to cleanup..."));
    MemSpyDevice().Cleanup();

    TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::~DMemSpyDriverLogicalChannel() - END"));
	}


TInt DMemSpyDriverLogicalChannel::DoCreate( TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVer )
	{
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::DoCreate() - START - heldFM: %d, device: 0x%08x", Kern::CurrentThread().iNThread.iHeldFastMutex != NULL, &MemSpyDevice() ) );

    TInt error = KErrNone;
    //
    if  ( !Kern::QueryVersionSupported( KMemSpyDriverVersion(), aVer ) )
        {
        error = KErrNotSupported;
        }
    else
        {
        // Try to get the memory model type
        
    	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::DoCreate - opening client thread..."));

        iClientThread = &Kern::CurrentThread();
	    error = iClientThread->Open();
        TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::DoCreate - client thread open error: %d", error ));

        if  ( error == KErrNone )
            {
            TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::DoCreate - creating sub channels error: %d", error ));
            error = SubChannelsRegister();
            }
        }
    //
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::DoCreate() - END - heldFM: %d", Kern::CurrentThread().iNThread.iHeldFastMutex != NULL ) );
    return error;
	}












TInt DMemSpyDriverLogicalChannel::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TRACE( Kern::Printf(" " ) );
	TRACE( Kern::Printf(" " ) );
	TRACE( Kern::Printf("--------------------------------------------------------------------------------------------------------------------- " ) );
	TRACE_OP( Kern::Printf("DMemSpyDriverLogicalChannel::Request() - START - fn: %3d, a1: 0x%08x, a2: 0x%08x, heldFM: %d", aFunction, a1, a2, iClientThread->iNThread.iHeldFastMutex != NULL ) );
	TRACE( Kern::Printf("--------------------------------------------------------------------------------------------------------------------- " ) );
	//
    TInt r = KErrNotSupported;
    //
    DMemSpyDriverLogChanBase* handler = SubChannelForFunction( aFunction );
    if  ( handler )
        {
        r = handler->Request( aFunction, a1, a2 );
        }
    //
#ifdef _DEBUG
    if  ( r < 0 && r != KErrEof )
        {
	    Kern::Printf( "DMemSpyDriverLogicalChannel::Request() - END - fn: %3d, a1: 0x%08x, a2: 0x%08x, heldFM: %d, r: %d", aFunction, a1, a2, iClientThread->iNThread.iHeldFastMutex != NULL, r );
        }
#endif
	TRACE( Kern::Printf(" " ) );
	TRACE( Kern::Printf(" " ) );
    //
    return r;
	}













TInt DMemSpyDriverLogicalChannel::SubChannelsRegister()
    {
    TInt r = KErrNone;
    DMemSpyDriverDevice& device = MemSpyDevice();
    DMemSpyDriverLogChanBase* subChan = NULL;
    //
    subChan = new DMemSpyDriverLogChanChunks( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanClientServer( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanCodeSegs( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanContainers( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanHeapDataUser( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanHeapDataKernel( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanHeapWalkUser( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanMisc( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanProcessInspection( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanRawMemory( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanStack( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanThreadAndProcess( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    subChan = new DMemSpyDriverLogChanUserEventMonitor( device, *iClientThread );
    r = SubChannelConstructAndSave( subChan );
    if ( r != KErrNone )
        {
        return r;
        }
    //
    return r;
    }


TInt DMemSpyDriverLogicalChannel::SubChannelConstructAndSave( DMemSpyDriverLogChanBase*& aSubChannel )
    {
    TInt r = KErrNoMemory;
    //
    if ( aSubChannel )
        {
        NKern::ThreadEnterCS();
        r = aSubChannel->Construct();
        //
        if  ( r == KErrNone )
            {
            r = iSubChannels.Append( aSubChannel );
            if  ( r != KErrNone )
                {
                delete aSubChannel;
                }
            }
        else
            {
            delete aSubChannel;
            }
        //
        NKern::ThreadLeaveCS();
        }
    //
    aSubChannel = NULL;
    return r;
    }


void DMemSpyDriverLogicalChannel::SubChannelsDestroy()
    {
    const TInt count = iSubChannels.Count();
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::SubChannelsDestroy() - START - count: %d", count ) );
    //
    NKern::ThreadEnterCS();
    for( TInt i=0; i<count; i++ )
        {
        DMemSpyDriverLogChanBase* subChan = iSubChannels[ i ];
	    TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::SubChannelsDestroy() - deleting subChannel: 0x%08x", subChan ) );
        delete subChan;
        }
    //
    iSubChannels.Reset();
    NKern::ThreadLeaveCS();
    //
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::SubChannelsDestroy() - END" ) );
    }


DMemSpyDriverLogChanBase* DMemSpyDriverLogicalChannel::SubChannelForFunction( TInt aFunction )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::SubChannelForFunction() - START - aFunction: %d", aFunction ) );
    //
    DMemSpyDriverLogChanBase* ret = NULL;
    const TInt count = iSubChannels.Count();
    for( TInt i=0; i<count; i++ )
        {
        DMemSpyDriverLogChanBase* subChan = iSubChannels[ i ];
        if  ( subChan->IsHandler( aFunction ) )
            {
            ret = subChan;
            break;
            }
        }
    //
	TRACE( Kern::Printf("DMemSpyDriverLogicalChannel::SubChannelForFunction() - END - aFunction: %d, subChannel: 0x%08x", aFunction, ret ) );
    return ret;
    }


DMemSpyDriverDevice& DMemSpyDriverLogicalChannel::MemSpyDevice()
    {
    DMemSpyDriverDevice& device = *((DMemSpyDriverDevice*) iDevice);
    return device;
    }

