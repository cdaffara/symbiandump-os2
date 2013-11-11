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

#include "MemSpyDriverLogChanClientServer.h"

// System includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverOSAdaption.h"



DMemSpyDriverLogChanClientServer::DMemSpyDriverLogChanClientServer( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanContainerBase( aDevice, aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanChunks::DMemSpyDriverLogChanChunks() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanClientServer::~DMemSpyDriverLogChanClientServer()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::~DMemSpyDriverLogChanClientServer() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::~DMemSpyDriverLogChanClientServer() - END - this: 0x%08x", this ));
	}



TInt DMemSpyDriverLogChanClientServer::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanContainerBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeClientServerGetServerSessionHandles:
            r = GetServerSessionHandles( (TMemSpyDriverInternalServerSessionHandleParams*) a1 );
            break;
	    case EMemSpyDriverOpCodeClientServerGetServerSessionInfo:
            r = GetServerSessionInfo( a1, (TMemSpyDriverServerSessionInfo*) a2 );
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanClientServer::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeClientServerBase && aFunction < EMemSpyDriverOpCodeClientServerEnd );
    }






TInt DMemSpyDriverLogChanClientServer::GetServerSessionHandles( TMemSpyDriverInternalServerSessionHandleParams* aParams )
	{
    TMemSpyDriverInternalServerSessionHandleParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalServerSessionHandleParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionHandles() - END - params read error: %d", r));
        return r;
        }

    DMemSpyDriverOSAdaptionDServer& serverAdaption = OSAdaption().DServer();

	const TInt maxCount = params.iMaxCount;
    NKern::ThreadEnterCS();

    DObject* serverHandle = (DObject*) params.iServerHandle;
    DServer* server = static_cast<DServer*>(CheckedOpen(EMemSpyDriverContainerTypeServer, serverHandle));
    if (server == NULL)
        {
    	Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionHandles() - END - server not found");
        NKern::ThreadLeaveCS();
        return KErrNotFound;
        }

	ResetTempHandles();

    NKern::LockSystem(); // Iterating session queue requires system lock
    // Iterate through this server's sessions, writing back session pointer (handle)
    // to client
    SDblQue& serverQueue = serverAdaption.GetSessionQueue( *server );
    SDblQueLink* anchor = &serverQueue.iA;
    SDblQueLink* link = serverQueue.First();
    while( link != anchor )
        {
		DSession* session = serverAdaption.GetSession( link );

        // Found a match in the specified container. Write the object's handle (aka the object address)
        // back to the client address space
        if  ( session )
            {
            AddTempHandle( session );
            }

        // Get next item
        link = link->iNext;
        }
    NKern::UnlockSystem();
	server->Close(NULL);
	NKern::ThreadLeaveCS();

    // This variable holds the number of handles that we have already
	// written to the client-side.
	TInt currentWriteIndex = 0;
	const TInt handleCount = TempHandleCount();
    TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionHandles - writing %d handles to client...", handleCount ) );
    for( ; currentWriteIndex<handleCount && r == KErrNone && currentWriteIndex < maxCount; )
        {
        TAny* handle = TempHandleAt( currentWriteIndex );
        r = Kern::ThreadRawWrite( &ClientThread(), params.iSessionHandles + currentWriteIndex, &handle, sizeof(TAny*) );
        if  (r == KErrNone)
            {
            ++currentWriteIndex;
            }
        }

	if  ( r == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }
    else
        {
        const TInt finalWrite = Kern::ThreadRawWrite( &ClientThread(), params.iSessionCountPtr, &currentWriteIndex, sizeof(TInt) );
        if  ( r == KErrNone )
            {
            r = finalWrite;
            }
        }


	TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionHandles() - END - r: %d", r));
	return r;
    }


TInt DMemSpyDriverLogChanClientServer::GetServerSessionInfo( TAny* aSessionHandle, TMemSpyDriverServerSessionInfo* aParams )
    {
    TMemSpyDriverServerSessionInfo params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverServerSessionInfo) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionInfo() - END - params read error: %d", r));
        return r;
        }

    DMemSpyDriverOSAdaptionDSession& sessionAdaption = OSAdaption().DSession();
    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();
    DMemSpyDriverOSAdaptionDProcess& processAdaption = OSAdaption().DProcess();

	NKern::ThreadEnterCS();

    DSession* session = (DSession*)CheckedOpen(EMemSpyDriverContainerTypeSession, (DObject*)aSessionHandle);
    if (session == NULL )
        {
    	Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionInfo() - END - session not found");
        NKern::ThreadLeaveCS();
        return KErrNotFound;
        }

    session->FullName( params.iName );

    // Get owner type and id
    DObject* sessionOwner = sessionAdaption.GetOwner( *session );
    if  ( sessionOwner )
        {
        const TObjectType objectType = sessionAdaption.GetObjectType( *sessionOwner );
        if  ( objectType == EProcess )
            {
            DProcess* sessionProcess = (DProcess*) sessionOwner;
            //
            params.iOwnerId = processAdaption.GetId( *sessionProcess );
            params.iOwnerType = TMemSpyDriverServerSessionInfo::EOwnerProcess;
            }
        else if ( objectType == EThread )
            {
            DThread* sessionThread = (DThread*) sessionOwner;
            //
            params.iOwnerId = threadAdaption.GetId( *sessionThread );
            params.iOwnerType = TMemSpyDriverServerSessionInfo::EOwnerThread;
            }
        }
    else
        {
        params.iOwnerId = -1;
        params.iOwnerType = TMemSpyDriverServerSessionInfo::EOwnerNone;
        }

    // Other attributes
    params.iSessionType = sessionAdaption.GetSessionType( *session );
    params.iAddress = (TUint8*)session;
	session->Close(NULL);
	NKern::ThreadLeaveCS();
    r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverServerSessionInfo) );

	TRACE( Kern::Printf("DMemSpyDriverLogChanClientServer::GetServerSessionInfo() - END - r: %d", r));
	return r;
    }
