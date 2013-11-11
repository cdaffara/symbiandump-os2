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
#include "featmgrsession.h"
#include "featmgrfeatureentry.h"
#include "featmgrdebug.h"

// CONSTANTS
const TInt CFeatMgrPendingRequest::iOffset = _FOFF( CFeatMgrPendingRequest,iLink );

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "FeatMgrSession" );

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatMgrSession::CFeatMgrSession
// C++ constructor
// -----------------------------------------------------------------------------
//
CFeatMgrSession::CFeatMgrSession( CFeatMgrServer& aServer, CFeatMgrFeatureRegistry& aRegistry )
    : iFeatMgrServer( aServer ),
    iRegistry( aRegistry ),
    iList( CFeatMgrPendingRequest::iOffset )
    {
    }

// -----------------------------------------------------------------------------
// CFeatMgrSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeatMgrSession* CFeatMgrSession::NewL( CFeatMgrServer& aServer, 
    CFeatMgrFeatureRegistry& aRegistry )
    {
    FUNC_LOG

    CFeatMgrSession* self = new( ELeave ) CFeatMgrSession( aServer, aRegistry );
    
    return self;
    }

#ifdef EXTENDED_FEATURE_MANAGER_TEST
void CFeatMgrSession::CreateL()
    {
    iFeatMgrServer.AddSession();
    }
#endif

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CFeatMgrSession::~CFeatMgrSession()
    {
    FUNC_LOG

    iNotifyFeatures.Close();

    while ( !iList.IsEmpty() )
        {
        CFeatMgrPendingRequest* pendingReq = iList.First();  
        iList.Remove( *pendingReq );
        delete pendingReq;
        }
    
    iList.Reset();
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    iFeatMgrServer.DropSession();
#endif
    }

// -----------------------------------------------------------------------------
// CFeatMgrSession::PanicClient
// RMessage2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// -----------------------------------------------------------------------------
//
void CFeatMgrSession::PanicClient( const RMessage2& aMessage, TFeatMgrPanic aPanic )
    {
    INFO_LOG1( "CFeatMgrSession::PanicClient(aPanic 0x%x)", aPanic);

	aMessage.Panic( KPanicCategory, aPanic );
    }

TBool CFeatMgrSession::IsWriteOperation( const TInt aFunction ) const
    {
        switch ( aFunction )
            {
            case EFeatMgrEnableFeature:
            case EFeatMgrDisableFeature:
            case EFeatMgrAddFeature:
            case EFeatMgrSetFeatureAndData:
            case EFeatMgrSetFeatureData:
            case EFeatMgrDeleteFeature:
            case EFeatMgrSWIStart:
            case EFeatMgrSWIEnd:
                return ETrue;
            default:
                return EFalse;
            }
    }

// -----------------------------------------------------------------------------
// CFeatMgrSession::ServiceL
// Calls request handling functions. Also traps any leaves and signals client if
// error occurs.
// -----------------------------------------------------------------------------
//
void CFeatMgrSession::ServiceL( const RMessage2& aMessage )
    {
    FUNC_LOG
    // If plugins are not ready all request will be queued. 
    // During backup & restore operation, all write request 
    //  e.g. EnableFeature will return with KErrServerBusy
    TInt msgCmd = aMessage.Function();
    if ( !iFeatMgrServer.PluginsReady() || ( iFeatMgrServer.BURIsInProgress()  && IsWriteOperation( msgCmd ) ) )
        {
        if ( iFeatMgrServer.BURIsInProgress() )
            {
            INFO_LOG( "CFeatMgrSession::ServiceL() - backup/restore is in progress - no write operation allowed" );
            aMessage.Complete( KErrServerBusy );
            }
        else
            {
            INFO_LOG( "CFeatMgrSession::ServiceL() - plugins not ready" );
            CFeatMgrPendingRequest* request=NULL;
            TRAPD(error,request=CFeatMgrPendingRequest::NewL( aMessage ));
            if (error!=KErrNone)
              {
              LOG_IF_ERROR1( error, "CFeatMgrSession::ServiceL(): Error in Adding Pending Request: %d", error );
              //cannot create pending request so need to indicate to the client rather than letting the cient wait forever.
              aMessage.Complete(error);              
              }
            else
              {
              iList.AddLast(*request);
              }
            }
        }
    else
        {
        #if defined(FEATMGR_INFO_LOG_ENABLED)
            // check memory usage
            TInt biggestBlock;
            INFO_LOG1( "CFeatMgrSession::ServiceL() - #### Memory Available in Heap: %6d ####", 
                                    User::Heap().Available(biggestBlock) );
            INFO_LOG1( "CFeatMgrSession::ServiceL() - #### Biggest block:            %6d ####", 
                                    biggestBlock );
        #endif

        TRAPD( error, DispatchMessageL( aMessage ) );

        LOG_IF_ERROR1( error, "CFeatMgrSession::ServiceL(): Error in DispatchMessageL: %d", 
                                     error );

        if( aMessage.Function() != EFeatMgrReqNotify )
            {
            aMessage.Complete( error );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrSession::ServicePendingRequestsL
// Calls request handling functions. Also traps any leaves and signals client if
// error occurs.
// -----------------------------------------------------------------------------
//
void CFeatMgrSession::ServicePendingRequestsL()
    {       
    FUNC_LOG
    
    while ( !iList.IsEmpty() )
        {
        CFeatMgrPendingRequest* pendingReq = iList.First();  
        
        TRAPD( error, DispatchMessageL( pendingReq->iMessage ) );

        LOG_IF_ERROR1( error, "CFeatMgrSession::ServicePendingRequestsL(): Error in DispatchMessageL: %d", 
            error );

        if( pendingReq->iMessage.Function() != EFeatMgrReqNotify )
            {
            pendingReq->iMessage.Complete( error );
            }
        
        iList.Remove( *pendingReq );
        delete pendingReq;
        }
    
    iList.Reset();
    }

// -----------------------------------------------------------------------------
// CFeatMgrSession::DispatchMessageL
// Calls matching function of CFeatMgrServer for handling the request.
// -----------------------------------------------------------------------------
//
void CFeatMgrSession::DispatchMessageL( const RMessage2& aMessage )
    {
    FUNC_LOG
    
    INFO_LOG1( "CFeatMgrSession::DispatchMessageL(0x%x)", aMessage.Function() );

    TInt msgCmd = aMessage.Function();
    
    // Getting the ID of the process making the calls on feature manager. This ID is used
    // for the addition, deletion, setting, enabling, and disabling functions.
    TUint processId = 0;	// default value for a process id
    TInt getIdErr = KErrGeneral;
    if( msgCmd >= EFeatMgrEnableFeature && msgCmd <= EFeatMgrSWIEnd )
    	{
	    RThread thread;
	    getIdErr = aMessage.Client(thread, EOwnerProcess);
	    if( getIdErr == KErrNone)
	    	{
		    RProcess process;
		    getIdErr = thread.Process(process);
		    if( getIdErr == KErrNone)
		    	{
		    	TProcessId prcId = process.Id();
		    	processId = TUint(prcId.Id());
		    	}
	    	}
    	}
	    	
    // Check command code and call appropriate function
    switch ( msgCmd )
        {
    	case EFeatMgrFeatureSupported:
    	    {
    	    TFeatureEntry feature;
    	    TPckg<TFeatureEntry> pckg( feature );
    	    aMessage.ReadL( 0, pckg );
    	    TFeatureServerEntry feat( feature );
    	    TInt supported = iRegistry.IsFeatureSupported( feat );
    	    // Construct entry for passing back to client.
    	    TFeatureEntry featBack( feat.FeatureUid(), feat.FeatureFlags(), feat.FeatureData() );
    	    TPckgC<TFeatureEntry> pckgBack( featBack );
            TPckgC<TInt> resPckg( supported );
            aMessage.WriteL( 0, pckgBack );
            aMessage.WriteL( 1, resPckg );
    	    
    	    break;
    	    }
    	    
        case EFeatMgrFeaturesSupported:
    	    {
        	TInt count( aMessage.Int0() );
        	TInt responseCount( 0 );
        	RFeatureArray temp;
        	CleanupClosePushL( temp );
        	temp.ReserveL( count );
        	
            for ( TInt i = 0; i < count; i++ )
                {
                TFeatureEntry feature;
                TPckg<TFeatureEntry> pckg( feature );
                TInt offset = i * sizeof(TFeatureEntry);
                // Read feature entry and fetch entry status/data
                aMessage.ReadL( 1, pckg, offset );
                TFeatureServerEntry feat( feature );
                TInt supported = iRegistry.IsFeatureSupported( feat );
                
                // Non-existing and uninitialised feature entries are not returned.
                if( supported != KErrNotFound && supported != KErrNotReady )
                    {
                    responseCount++;
                    TFeatureEntry featBack( feat.FeatureUid(), feat.FeatureFlags(), 
                        feat.FeatureData() );
                    // Can't write to same slot before reading entries first
                    temp.AppendL( featBack );
                    }
                }
                
            // Write found entries back to client
            for ( TInt i = 0; i < responseCount; i++ )
                {
                TInt offset = i * sizeof(TFeatureEntry);
                TPckgC<TFeatureEntry> pckgBack( temp[i] );
                aMessage.WriteL( 1, pckgBack, offset );
                }
            CleanupStack::PopAndDestroy( &temp );

            // Write number of found entries back to client
            TPckgC<TInt> resPckg( responseCount );
            aMessage.WriteL( 2, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrNumberOfSupportedFeatures:
    	    {
    	    TInt count = iRegistry.NumberOfSupportedFeatures();
            TPckgC<TInt> resPckg( count );
            aMessage.WriteL( 0, resPckg );
    	    
    	    break;
    	    }
    	    
    	case EFeatMgrListSupportedFeatures:
    	    {   
            RFeatureUidArray supportedFeatures;
            CleanupClosePushL( supportedFeatures );
            
            iRegistry.SupportedFeaturesL( supportedFeatures );
            TInt count( supportedFeatures.Count() );
           
            if ( aMessage.Int0() == count )
                {
                for ( TInt i = 0; i < count; i++ )
                    {
                    TPckg<TUid> pckg( supportedFeatures[i] );
                    TInt offset = i * sizeof(TUid);
                    aMessage.WriteL( 1, pckg, offset );
                    }
                
                CleanupStack::PopAndDestroy( &supportedFeatures );
                }
            else
                {
                CleanupStack::PopAndDestroy( &supportedFeatures );
                User::Leave( KErrServerBusy );
                }
         	break;
    	    }

    	case EFeatMgrEnableFeature:
    	    {
    	    if( getIdErr == KErrNone )
    	    	{
	    	    TUid feature = TUid::Uid(aMessage.Int0());
	    	    getIdErr = iRegistry.SetFeature( feature, EFeatureSupportEnable, NULL, processId );
    	    	}
    	    TPckgC<TInt> resPckg( getIdErr );
	        aMessage.WriteL( 1, resPckg );

	        break;
    	    }
  		
  		case EFeatMgrDisableFeature:
    	    {
    	    if( getIdErr == KErrNone )
    	    	{
	    	    TUid feature = TUid::Uid(aMessage.Int0());
	    	    getIdErr = iRegistry.SetFeature( feature, EFeatureSupportDisable, NULL, processId );
    	    	}    	    
            TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 1, resPckg );
    	    
    	    break;
    	    }

        case EFeatMgrAddFeature:
    	    {
    	    if( getIdErr == KErrNone)
    	    	{
    	    	TFeatureEntry feature;
        	    TPckg<TFeatureEntry> pckg( feature );
        	    aMessage.ReadL( 0, pckg );
        	    TFeatureServerEntry feat( feature );
        	    getIdErr = iRegistry.AddFeature( feat, processId );
    	    	}    	    
            TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 1, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrDeleteFeature:
    	    {
    	    if( getIdErr == KErrNone)
    	    	{
    	    	TUid feature = TUid::Uid(aMessage.Int0());
    	    	getIdErr = iRegistry.DeleteFeature( feature, processId );
    	    	}
    	    TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 1, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrSetFeatureAndData:
    	    {
    	    if( getIdErr == KErrNone)
    	    	{
    	    	TUid feature = TUid::Uid(aMessage.Int0());
	    	    TBool enable = aMessage.Int1();
	    	    TUint32 data = aMessage.Int2();
	    	    getIdErr = iRegistry.SetFeature( feature, enable, &data, processId );
    	    	}
    	    TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 3, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrSetFeatureData:
    	    {
    	    if( getIdErr == KErrNone)
    	    	{
    	    	TUid feature = TUid::Uid(aMessage.Int0());
	    	    TUint32 data = aMessage.Int1();
	    	    getIdErr = iRegistry.SetFeature( feature, EFeatureSupportUntouch, &data, processId );
    	    	}
    	    TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 2, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrReqNotify:
    	    {
    	    // When client requests notification for feature, it could be checked
    	    // whether feature exists at all and even whether feature modifiable,
    	    // i.e. could request ever cause notification. If this will be done, 
    	    // remember document error codes in API documentation.
    	    
    	    // Message is needed for later signaling of client upon feature change.
    	    if( iNotifyMessage.IsNull() ) 
    	        {
    	        iNotifyMessage = aMessage;
    	        }
    	    else
    	        {
    	        PanicClient( aMessage, EPanicNotifyRequest );
    	        }
    	        
    	    break;
    	    }
        
        case EFeatMgrReqNotifyUids:
    	    {
    	    // When client requests notification for features, it could be checked
    	    // whether features exists at all and even whether features modifiable,
    	    // i.e. could request ever cause notification. If this will be done, 
    	    // remember document error codes in API documentation.
    	    
    	    TInt err( KErrNone );
    	    
        	// Fetch transfer buffer from client
        	TInt count( aMessage.Int0() );
        	err = iNotifyFeatures.Reserve( count );
        	
        	if( err == KErrNone )
        	    {
                for ( TInt i = 0; i < count; i++ )
                    {
                    TUid feature;
                    TPckg<TUid> pckg( feature );
                    TInt offset = i * sizeof(TUid);
                    aMessage.ReadL( 1, pckg, offset );
                    err = iNotifyFeatures.Append( feature );
                    if( err != KErrNone )
                        {
                        break;
                        }
                    }
        	    }
    	    
            TPckgC<TInt> resPckg( err );
            aMessage.WriteL( 2, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrReqNotifyCancel:
    	    {
    	    TUid feature = TUid::Uid(aMessage.Int0());
    	    TInt index( iNotifyFeatures.Find( feature ) );
    	    TInt err( KErrNotFound );
    	    
    	    if( index != KErrNotFound )
    	        {
    	        err = KErrNone;
    	        iNotifyFeatures.Remove( index );
    	        }
    	    
    	    // If no more features to be notified, complete request.
    	    if( !iNotifyFeatures.Count() 
    	        && iNotifyMessage.IsNull() == EFalse)
    	        {
    	        iNotifyMessage.Complete( KErrCancel );
    	        }
    	    
            TPckgC<TInt> resPckg( err );
            aMessage.WriteL( 1, resPckg );
    	    
    	    break;
    	    }
        
        case EFeatMgrReqNotifyCancelAll:
    	    {
    	    iNotifyFeatures.Reset();
    	    if( iNotifyMessage.IsNull() == EFalse )
    	        {
    	        iNotifyMessage.Complete( KErrCancel );
    	        }
            TPckgC<TInt> resPckg( KErrNone );
            aMessage.WriteL( 0, resPckg );
    	    
    	    break;
    	    }
    	    
        case EFeatMgrSWIStart:
    	    {
    	    if( getIdErr == KErrNone)  
    	    	{
    	    	getIdErr = iRegistry.SWIStart(processId);
	    	    }
    	    TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 0, resPckg );
            
    	    break;
    	    }
    	    
        case EFeatMgrSWIEnd:
    	    {
    	    if( getIdErr == KErrNone)
    	    	{
    	    	getIdErr = iRegistry.SWIEnd(processId);
    	    	}
    	    TPckgC<TInt> resPckg( getIdErr );
            aMessage.WriteL( 0, resPckg );
            
    		break;
    	    }

#ifdef EXTENDED_FEATURE_MANAGER_TEST
    	    
#pragma BullseyeCoverage off
    	    
        case EFeatMgrResourceMark:
            ResourceCountMarkStart();
            break;
            
        case EFeatMgrResourceCheck:
            ResourceCountMarkEnd(aMessage);
            break;
        
        case EFeatMgrResourceCount:
            {
            TInt retCode = CountResources();
            User::Leave(retCode);
            }
            break;
        
        case EFeatMgrSetHeapFailure:
            {
            RAllocator::TAllocFail mode = static_cast <RAllocator::TAllocFail> (aMessage.Int0());
            TInt failAllocNum = aMessage.Int1();
            if(mode == RHeap::EBurstFailNext || mode == RHeap::EBurstRandom || mode == RHeap::EBurstTrueRandom || mode == RHeap::EBurstDeterministic)
                {
                User::__DbgSetBurstAllocFail(RHeap::EUser, mode, failAllocNum, 20);
                }
            else
                {
                User::__DbgSetAllocFail(RHeap::EUser, mode, failAllocNum);
                }
            }
            break;

    	    // debug only API 
    	    // returns the size of the iNotifyFeatures array
        case EFeatMgrNumberOfNotifyFeatures:
    	    {
    	    TInt count = iNotifyFeatures.Count();
            TPckgC<TInt> resPckg( count );
            aMessage.WriteL( 0, resPckg );
    	    
    	    break;
    	    }

    	    // returns the number of allocated heap cells
        case EFeatMgrCountAllocCells:
    	    {
    	    TInt count = User::CountAllocCells();
            TPckgC<TInt> resPckg( count );
            aMessage.WriteL( 0, resPckg );
    	    
    	    break;
    	    }
    	    
#pragma BullseyeCoverage on
    	    
#endif

        // Cannot identify the message.
        default:
            {
            PanicClient( aMessage, EPanicIllegalArgument );
            break;
           }
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrSession::ServiceNotifications
// -----------------------------------------------------------------------------
//
void CFeatMgrSession::ServiceNotifications( TFeatureServerEntry& aFeature, 
    TFeatureChangeType aType )
    {       
    FUNC_LOG
    
    if( !iNotifyMessage.IsNull() ) 
        {
        TInt index( iNotifyFeatures.Find( aFeature.FeatureUid() ) );
        
        if( index != KErrNotFound )
            {
            if( aType == EFeatureFeatureDeleted )
            	{
            	// The feature was deleted - won't have any more notifications
            	iNotifyFeatures.Remove(index);
            	}

            // Write changed feature back to client and complete request
            TPckgC<TInt> feature( aFeature.FeatureUid().iUid );
            TInt err( iNotifyMessage.Write( 0, feature ) );
            if( err == KErrNone )
                {
                iNotifyMessage.Complete( aType );
                }
            }
        }
    }

TInt CFeatMgrSession::CountResources()
    {
    return User::CountAllocCells();
    }

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatMgrPendingRequest::CFeatMgrPendingRequest
// C++ constructor
// -----------------------------------------------------------------------------
//
CFeatMgrPendingRequest::CFeatMgrPendingRequest()
    {
    }

// -----------------------------------------------------------------------------
// CFeatMgrPendingRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeatMgrPendingRequest* CFeatMgrPendingRequest::NewL(const RMessage2& aMessage)
    {
    FUNC_LOG
    CFeatMgrPendingRequest* self = new (ELeave) CFeatMgrPendingRequest;
    
    CleanupStack::PushL( self );
    self->ConstructL( aMessage );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CFeatMgrPendingRequest::ConstructL
// ---------------------------------------------------------
//
void CFeatMgrPendingRequest::ConstructL(const RMessage2& aMessage)
    {
    iMessage = aMessage;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CFeatMgrPendingRequest::~CFeatMgrPendingRequest()
    {
    FUNC_LOG
    }


//  End of File  
