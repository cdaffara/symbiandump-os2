// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include <e32svr.h>
#include <s32mem.h>
#include "featmgrclient.h"
#include "featmgrconfiguration.h"
#include "featmgrdebug.h"
#include "featmgrclientserver.h"

// CONSTANTS
const TInt KRetry( 2 );

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RFeatMgrClient::RFeatMgrClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RFeatMgrClient::RFeatMgrClient()
    : RSessionBase(), iFeaturePckg( NULL, 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::Connect
// Connects to server
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::Connect()
    {
    FUNC_LOG

    // Try this twice
    TInt retry( KRetry );
    TInt err( KErrNone );

    while ( retry > 0 )
        {
        // Try to create a FeatMgr Server session
        err = CreateSession( KServerProcessName, 
                             ServerVersion(), 
                             KDefaultAsyncSlots );

        LOG_IF_ERROR1( err, "RFeatMgrClient::Connect - CreateSession returned: %d", err );
    
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            // KErrNone or unrecoverable error
            retry = 0;
            }
        else
            {
            // Return code was KErrNotFound or KErrServerTerminated.
            // Try to start a new FeatMgr Server
            err = StartServer();

            LOG_IF_ERROR1( err, "RFeatMgrClient::Connect - StartServer returned: %d", err );

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                // Unrecoverable error
                retry = 0;
                }
            }
            
        retry--;
        }
            
    return err;
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::StartServer
// Starts server.
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::StartServer()
    {
    FUNC_LOG
    TIMESTAMP( "StartServer start: " )

    RProcess server;
    const TUidType serverUid( KNullUid, KServerUid2, KNullUid );
    TInt err = server.Create( KServerExeName, // FeatMgrServer.exe
                           KNullDesC, // A descriptor containing data passed as 
                                      // an argument to the thread function of 
                                      // the new process's main thread, when it 
                                      // is first scheduled.
                           serverUid, // FeatMgr server UID
                           EOwnerProcess ); // Ownership of this process handle 

    // Return error code if we couldn't create a process
    if ( err == KErrNone )
        {
        // Rendezvous is used to detect server start
        TRequestStatus stat;
        server.Rendezvous( stat );

        if ( stat != KRequestPending )
            {
            server.Kill( KErrNone ); // Abort startup
            }
        else
            {
            server.Resume();  // Logon OK - start the server
            }

        INFO_LOG( "RFeatMgrClient::StartServer - Waiting server startup" );

        User::WaitForRequest( stat ); // Wait for start or death

        INFO_LOG( "RFeatMgrClient::StartServer - Server startup wait finished" );
        
        // We can't use the 'exit reason' if the server paniced as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        err = (server.ExitType() == EExitPanic)? KErrGeneral : stat.Int();

        // We can close the handle now
        server.Close();        
        }

    TIMESTAMP( "StartServer end: " )

    return err;
    }


// -----------------------------------------------------------------------------
// RFeatMgrClient::ServerVersion
// Return version of server
// -----------------------------------------------------------------------------
//
TVersion RFeatMgrClient::ServerVersion() const 
    {
    return TVersion( KServerVersionMajor, KServerVersionMinor, KServerVersionBuild );
    }


// -----------------------------------------------------------------------------
// RFeatMgrClient::FeatureSupported()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::FeatureSupported( TFeatureEntry& aFeature ) const
    {  
    TPckg<TFeatureEntry> pckg( aFeature );
    TPckgBuf<TInt> pckgRet;
    
    TInt retval = SendReceive(EFeatMgrFeatureSupported, TIpcArgs( &pckg, &pckgRet ));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::FeatureSupported - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG2( "RFeatMgrClient::FeatureSupported - uid %d, supported %d",
                            aFeature.FeatureUid().iUid, pckgRet() );   
    return pckgRet();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::FeaturesSupported()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::FeaturesSupported( RFeatureArray& aFeatures )
    {  
    TPckgBuf<TInt> pckg;
    
    TInt retval( KErrNone );
    
    TRAP( retval, SendRcvFeatureArrayL( aFeatures, pckg() ) );

    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::FeaturesSupported - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::FeaturesSupported - return %d", pckg() );
    return pckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::EnableFeature()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::EnableFeature( TUid aFeature ) const
    {  
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrEnableFeature, TIpcArgs(aFeature.iUid, &pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::EnableFeature - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::EnableFeature - return %d", pckg() );
    return pckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::DisableFeature()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::DisableFeature( TUid aFeature ) const
    {  
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrDisableFeature, TIpcArgs(aFeature.iUid, &pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::DisableFeature - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::DisableFeature - return %d", pckg() );
    return pckg();
    }
    
// -----------------------------------------------------------------------------
// RFeatMgrClient::SetFeature()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::SetFeature( TUid aFeature, TBool aEnable, TInt aData ) const
    {  
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrSetFeatureAndData, TIpcArgs(
        aFeature.iUid, aEnable, aData, &pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::SetFeature - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::SetFeature - return %d", pckg() );
    return pckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::SetFeature()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::SetFeature( TUid aFeature, TInt aData ) const
    {  
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrSetFeatureData, TIpcArgs(
        aFeature.iUid, aData, &pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::SetFeature - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::SetFeature - return %d", pckg() );
    return pckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::AddFeature()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::AddFeature( TFeatureEntry aFeature ) const
    {  
    TPckg<TFeatureEntry> pckg( aFeature );
    TPckgBuf<TInt> pckgRet;
    
    TInt retval = SendReceive(EFeatMgrAddFeature, TIpcArgs(&pckg, &pckgRet));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::AddFeature - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::AddFeature - return %d", pckgRet() );
    return pckgRet();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::DeleteFeature()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::DeleteFeature( TUid aFeature ) const
    {  
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrDeleteFeature, TIpcArgs(aFeature.iUid, &pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::DeleteFeature - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::DeleteFeature - return %d", pckg() );
    return pckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::ListSupportedFeaturesL()
// -----------------------------------------------------------------------------
//
void RFeatMgrClient::ListSupportedFeaturesL( RFeatureUidArray& aSupportedFeatures )
    {  
    // Reset as array might contain old data.
    aSupportedFeatures.Reset();
    TInt retry( 5 );
    TInt err( KErrNone );
    TInt count;
    TPckg<TInt> sizePckg( count );
    
    while ( retry > 0 )
        {
        User::LeaveIfError( SendReceive( EFeatMgrNumberOfSupportedFeatures, 
            TIpcArgs( &sizePckg ) ) );
        
        HBufC8* buf = HBufC8::NewLC( count * sizeof( TInt ) );
        TPtr8 ptr = buf->Des();
        
        err = SendReceive( EFeatMgrListSupportedFeatures, TIpcArgs( count, &ptr ) );
        LOG_IF_ERROR1( err, "RFeatMgrClient::ListSupportedFeaturesL - SendReceive error %d", err );
        INFO_LOG1( "RFeatMgrClient::ListSupportedFeaturesL - count %d", count );

        if ( err == KErrNone )
            {
            aSupportedFeatures.ReserveL( count );
                
            for ( TInt i = 0; i < count; i++ )
                {
                TPtrC8 featurePtr = ptr.Mid( i * sizeof( TUid ), sizeof( TUid ) );
                TUid featureId = TUid::Uid( 0 );                    
                TPckg<TUid> feature( featureId );
                feature.Copy( featurePtr );
                aSupportedFeatures.AppendL( featureId );
                }

            retry = 0;
            }
        else if ( err == KErrServerBusy )
            {
            retry--;       
            }
        else
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy( buf );
        }   
        
    User::LeaveIfError( err );
    }


// -----------------------------------------------------------------------------
// RFeatMgrClient::ReRequestNotification(TUid&, TRequestStatus&)
// -----------------------------------------------------------------------------
//
void RFeatMgrClient::ReRequestNotification( TUid& aFeatUid, TRequestStatus& aStatus )
    {
    TPckgBuf<TInt> pckg;
    iFeaturePckg.Set( (TUint8*) &aFeatUid.iUid, sizeof(TUid), sizeof(TUid) );
	  TIpcArgs args( &iFeaturePckg );
  	SendReceive( EFeatMgrReqNotify, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::RequestNotification(RFeatureUidArray&, TUid&, TRequestStatus&)
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::RequestNotification( RFeatureUidArray& aFeatures, 
        TUid& aFeatUid, TRequestStatus& aStatus )
    {
    TPckgBuf<TInt> pckg;
    
    TInt retval( KErrNone );
    
    TRAP( retval, SendUidArrayL( aFeatures, pckg() ) );

    if ( retval == KErrNone )
        {
    	iFeaturePckg.Set( (TUint8*) &aFeatUid.iUid, sizeof(TUid), sizeof(TUid) );
    	TIpcArgs args( &iFeaturePckg );
        SendReceive( EFeatMgrReqNotify, args, aStatus );
        }
    else
        {
        ERROR_LOG1( "RFeatMgrClient::RequestNotification - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::RequestNotification - return %d", pckg() );
    return pckg();
    }
        
// -----------------------------------------------------------------------------
// RFeatMgrClient::RequestNotifyCancel(TUid)
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::RequestNotifyCancel( TUid aFeature ) const
    {
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrReqNotifyCancel, TIpcArgs(aFeature.iUid, &pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::RequestNotifyCancel - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::RequestNotifyCancel - return %d", pckg() );
    return pckg();
    }
        
// -----------------------------------------------------------------------------
// RFeatMgrClient::RequestNotifyCancelAll()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::RequestNotifyCancelAll() const
    {
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrReqNotifyCancelAll, TIpcArgs(&pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::RequestNotifyCancelAll - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::RequestNotifyCancelAll - return %d", pckg() );
    return pckg();
    }
        
// -----------------------------------------------------------------------------
// RFeatMgrClient::SendUidArrayL()
// -----------------------------------------------------------------------------
//
void RFeatMgrClient::SendUidArrayL( RFeatureUidArray& aFeatures, TInt &retval)
    {
    FUNC_LOG
    
    INFO_LOG1( "RFeatMgrClient::SendUidArrayL - Send %d features", aFeatures.Count() );

    TInt size = aFeatures.Count() * sizeof(TUid);
	CBufBase* buffer = CBufFlat::NewL( size );
	CleanupStack::PushL( buffer );
	buffer->ResizeL( size );

    RBufWriteStream stream( *buffer );
    CleanupClosePushL( stream );
    
    TInt count = aFeatures.Count();
    // Write each field in array to stream
    for(TInt i = 0; i < count; i++)
        {
        stream.WriteUint32L( aFeatures[i].iUid );
        }
    
    stream.CommitL();
    
    // Write transfer buffer to server
	TPtr8 pBuffer(buffer->Ptr(0));
	TPckgBuf<TInt> pckg;
	TIpcArgs args( count, &pBuffer, &pckg );
	TInt sendErr = SendReceive( EFeatMgrReqNotifyUids, args );
	
	retval = pckg();

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );
	User::LeaveIfError( sendErr );
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::SendRcvFeatureArrayL()
// -----------------------------------------------------------------------------
//
void RFeatMgrClient::SendRcvFeatureArrayL( RFeatureArray& aFeatures, TInt &retval)
    {
    FUNC_LOG
    
    INFO_LOG1( "RFeatMgrClient::SendRcvFeatureArrayL - Send %d features", aFeatures.Count() );

    TInt size = aFeatures.Count() * sizeof(TFeatureEntry);
	CBufBase* buffer = CBufFlat::NewL( size );
	CleanupStack::PushL( buffer );
	buffer->ResizeL( size );

    RBufWriteStream stream( *buffer );
    CleanupClosePushL( stream );
    
    TInt count = aFeatures.Count();
    // Write each field in array to stream
    for(TInt i = 0; i < count; i++)
        {
        TFeatureEntry entry( aFeatures[i].FeatureUid(), aFeatures[i].FeatureFlags(), 
            aFeatures[i].FeatureData() );
        stream.WriteUint32L( aFeatures[i].FeatureUid().iUid );
        stream.WriteUint32L( aFeatures[i].FeatureFlags().iFlags );
        stream.WriteUint32L( aFeatures[i].FeatureData() );
        stream.WriteUint32L( 0 ); // reserved
        }
    
    stream.CommitL();
    
    // Write transfer buffer to server
	TPtr8 pBuffer(buffer->Ptr(0));
	TPckgBuf<TInt> pckg;
	TIpcArgs args( count, &pBuffer, &pckg );
	TInt sendErr = SendReceive( EFeatMgrFeaturesSupported, args );

    if ( sendErr == KErrNone )
        {
    	TInt responseCount = pckg();
        // Read modified feature entries back to array
        RBufReadStream readStream( *buffer );
        CleanupClosePushL( readStream );
        aFeatures.Reset();
        aFeatures.ReserveL( responseCount );
            
        for ( TInt i = 0; i < responseCount; i++ )
            {
            TUid uid = TUid::Uid( readStream.ReadUint32L() );
            TBitFlags32 flags = readStream.ReadUint32L();
            TUint32 data = readStream.ReadUint32L();
            readStream.ReadUint32L(); // reserved
            TFeatureEntry entry( uid, flags, data );
            aFeatures.AppendL( entry );
            }
            
        CleanupStack::PopAndDestroy( &readStream );
        }

	retval = KErrNone; // Currently no error response exist.

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );
	User::LeaveIfError( sendErr );
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::SWIStart()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::SWIStart() const
    {
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrSWIStart, TIpcArgs(&pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::SWIStart - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::SWIStart - return %d", pckg() );
    return pckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::SWIEnd()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::SWIEnd() const
    {
    TPckgBuf<TInt> pckg;
    
    TInt retval = SendReceive(EFeatMgrSWIEnd, TIpcArgs(&pckg));
    
    if ( retval != KErrNone )
        {
        ERROR_LOG1( "RFeatMgrClient::SWIEnd - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::SWIEnd - return %d", pckg() ); 
    return pckg();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// DEBUG only API functions

#ifdef EXTENDED_FEATURE_MANAGER_TEST

#pragma BullseyeCoverage off

/** 
*/
void RFeatMgrClient::ResourceMark()
    {
    (void)SendReceive(EFeatMgrResourceMark);
    }

/** 
*/
void RFeatMgrClient::ResourceCheck()
    {
    (void)SendReceive(EFeatMgrResourceCheck);
    }

/** 
*/
TInt RFeatMgrClient::ResourceCount()
    {
    return SendReceive(EFeatMgrResourceCount);
    }

/** 
*/
void RFeatMgrClient::SetHeapFailure(TInt aAllocFailType, TInt aRate)
    {
    (void)SendReceive(EFeatMgrSetHeapFailure, TIpcArgs(aAllocFailType, aRate));
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::NumberOfNotifyFeatures()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::NumberOfNotifyFeatures( void ) const
    {  
    TInt count;
    TPckg<TInt> sizePckg( count );
    
    TInt retval = SendReceive( EFeatMgrNumberOfNotifyFeatures, TIpcArgs( &sizePckg ) );
    if ( KErrNone != retval )
        {
        ERROR_LOG1( "RFeatMgrClient::NumberOfNotifyFeatures - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::NumberOfNotifyFeatures - return %d", sizePckg() );
    return sizePckg();
    }

// -----------------------------------------------------------------------------
// RFeatMgrClient::CountAllocCells()
// -----------------------------------------------------------------------------
//
TInt RFeatMgrClient::CountAllocCells( void ) const
    {  
    TInt count;
    TPckg<TInt> sizePckg( count );
    
    TInt retval = SendReceive( EFeatMgrCountAllocCells, TIpcArgs( &sizePckg ) );
    if ( KErrNone != retval )
        {
        ERROR_LOG1( "RFeatMgrClient::CountAllocCells - SendReceive error %d", retval );
        return retval;
        }

    INFO_LOG1( "RFeatMgrClient::CountAllocCells - return %d", sizePckg() );
    return sizePckg();
    }

#pragma BullseyeCoverage on

#endif

//  End of File  
