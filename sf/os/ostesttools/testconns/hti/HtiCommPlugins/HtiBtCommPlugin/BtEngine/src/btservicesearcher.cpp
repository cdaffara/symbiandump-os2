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
* Description:  Bluetooth service searcher.
*
*/


// INCLUDE FILES
#include "btservicesearcher.h"
#include "btservicesearcher.pan"

#include "HtiBtEngineLogging.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBTServiceSearcher::CBTServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CBTServiceSearcher::CBTServiceSearcher()
:  iIsDeviceSelectorConnected( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::~CBTServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CBTServiceSearcher::~CBTServiceSearcher()
    {
    if ( iIsDeviceSelectorConnected )
        {
        iDeviceSelector.CancelNotifier( KDeviceSelectionNotifierUid );
        iDeviceSelector.Close();
        }
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::SelectDeviceByDiscoveryL()
// Select a device.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::SelectDeviceByDiscoveryL(
        TRequestStatus& aObserverRequestStatus )
    {
    if ( ! iIsDeviceSelectorConnected )
        {
        User::LeaveIfError( iDeviceSelector.Connect() );
        iIsDeviceSelectorConnected = ETrue;
        }

    iSelectionFilter().SetUUID( ServiceClass() );

    iDeviceSelector.StartNotifierAndGetResponse(
        aObserverRequestStatus,
        KDeviceSelectionNotifierUid,
        iSelectionFilter,
        iResponse );
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::SelectDeviceByNameL()
// Select a device by BT device name.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::SelectDeviceByNameL( const TDesC& aDeviceName,
        TRequestStatus& aObserverRequestStatus )
    {
    LOG_D( "CBTServiceSearcher::SelectDeviceByNameL" );

    RSocketServ socketServ;
    User::LeaveIfError( socketServ.Connect() );
    CleanupClosePushL( socketServ );

    TProtocolDesc pInfo;
    _LIT( KL2Cap, "BTLinkManager" );
    User::LeaveIfError(
        socketServ.FindProtocol( TProtocolName( KL2Cap ), pInfo ) );

    LOG_D( "CBTServiceSearcher: Found protocol" );

    RHostResolver hr;
    User::LeaveIfError( hr.Open( socketServ,
                            pInfo.iAddrFamily, pInfo.iProtocol ) );
    CleanupClosePushL( hr );

    LOG_D( "CBTServiceSearcher: HostResolver open" );

    TInquirySockAddr addr;
    TNameEntry entry;
    addr.SetIAC( KGIAC );
    addr.SetAction( KHostResName | KHostResInquiry );

    User::LeaveIfError( hr.GetByAddress( addr, entry ) );
    LOG_D( "CBTServiceSearcher: GetByAddress ok" );
    TBool isFound = EFalse;
    TInt err = KErrNone;
    while ( !isFound && !err )
        {
        if ( !aDeviceName.CompareF( entry().iName ) )
            {
            LOG_D( "CBTServiceSearcher: Name match" );
            isFound = ETrue;
            }
        else
            {
            LOG_D( "CBTServiceSearcher: Not match - getting next" );
            err = hr.Next( entry );
            }
        }
    CleanupStack::PopAndDestroy( 2 ); // hr, socketServ

    iStatusObserver = &aObserverRequestStatus;

    if ( isFound )
        {
        TInquirySockAddr& sa = TInquirySockAddr::Cast( entry().iAddr );
        iResponse().SetDeviceAddress( sa.BTAddr() );
        iResponse().SetDeviceName( entry().iName );
        TBTDeviceClass deviceClass( sa.MajorServiceClass(),
                                    sa.MajorClassOfDevice(),
                                    sa.MinorClassOfDevice() );
        iResponse().SetDeviceClass( deviceClass );
        User::RequestComplete( iStatusObserver, KErrNone );
        }

    else
        {
        LOG_E( "CBTServiceSearcher: Device not found! Can't connect!" );
        User::RequestComplete( iStatusObserver, err );
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::FindServiceL()
// Find a service on the specified device.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::FindServiceL( const TBTDevAddr& aDeviceAddress,
    TRequestStatus& aObserverRequestStatus )
    {
    iResponse().SetDeviceAddress( aDeviceAddress );
    if ( !iResponse().IsValidBDAddr() )
        {
        User::Leave( KErrArgument );
        }
    iHasFoundService = EFalse;

    // delete any existing agent and search pattern
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;

    iAgent = CSdpAgent::NewL(*this, BTDevAddr());

    iSdpSearchPattern = CSdpSearchPattern::NewL();

    iSdpSearchPattern->AddL( ServiceClass() );
    // return code is the position in the list that the UUID is inserted at
    // and is intentionally ignored

    iAgent->SetRecordFilterL( *iSdpSearchPattern );

    iStatusObserver = &aObserverRequestStatus;

    iAgent->NextRecordRequestL();
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::NextRecordRequestComplete()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::NextRecordRequestComplete(
    TInt aError,
    TSdpServRecordHandle aHandle,
    TInt aTotalRecordsCount)
    {
    TRAPD( error,
           NextRecordRequestCompleteL( aError, aHandle, aTotalRecordsCount );
    );

    if ( error != KErrNone )
        {
        Panic( EBTServiceSearcherNextRecordRequestComplete );
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::NextRecordRequestCompleteL()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::NextRecordRequestCompleteL(
    TInt aError,
    TSdpServRecordHandle aHandle,
    TInt aTotalRecordsCount )
    {
    if ( aError == KErrEof )
        {
        Finished();
        return;
        }

    if ( aError != KErrNone )
        {
        LOGFMT_E("CBTServiceSearcher: NextRecordRequestCompleteL: %d", aError );
        Finished( aError );
        return;
        }

    if ( aTotalRecordsCount == 0 )
        {
        LOG_I("CBTServiceSearcher: NextRecordRequestCompleteL: No records found");
        Finished( KErrNotFound );
        return;
        }

    //  Request its attributes
    iAgent->AttributeRequestL( aHandle, KSdpAttrIdProtocolDescriptorList );
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestResult()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestResult(
    TSdpServRecordHandle aHandle,
    TSdpAttributeID aAttrID,
    CSdpAttrValue* aAttrValue )
    {
    TRAPD( error,
         AttributeRequestResultL( aHandle, aAttrID, aAttrValue );
         );
    if ( error != KErrNone )
        {
        Panic( EBTServiceSearcherAttributeRequestResult );
        }

    // Delete obsolete local atribute pointer.
    delete aAttrValue;
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestResultL()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestResultL(
    TSdpServRecordHandle /*aHandle*/,
    TSdpAttributeID aAttrID,
    CSdpAttrValue* aAttrValue )
    {
    __ASSERT_ALWAYS( aAttrID == KSdpAttrIdProtocolDescriptorList,
                     User::Leave( KErrNotFound ) );

    TSdpAttributeParser parser( ProtocolList(), *this );

    // Validate the attribute value, and extract the RFCOMM channel
    aAttrValue->AcceptVisitorL( parser );

    if ( parser.HasFinished() )
        {
        // Found a suitable record so change state
        iHasFoundService = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestComplete()
// Process the attribute request completion.
// -----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestComplete( TSdpServRecordHandle aHandle,
                                                   TInt aError )
    {
    TRAPD( error,
        AttributeRequestCompleteL( aHandle, aError );
    );
    if ( error != KErrNone )
        {
        Panic( EBTServiceSearcherAttributeRequestComplete );
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestCompleteL()
// Process the attribute request completion.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestCompleteL( TSdpServRecordHandle
                                                    /*aHandle*/,
                                                    TInt aError )
    {
    if ( aError != KErrNone )
        {
        LOGFMT_W("CBTServiceSearcher::AttributeRequestCompleteL: %d", aError);
        }
    else
        {
        // done with attributes of this service record, request next
        iAgent->NextRecordRequestL();
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::Finished()
// The search has finished and notify the observer
// that the process is complete.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::Finished( TInt aError /* default = KErrNone */ )
    {
    if ( aError == KErrNone && !HasFoundService() )
        {
        aError = KErrNotFound;
        }
    User::RequestComplete( iStatusObserver, aError );
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::HasFinishedSearching()
// Is the instance still wanting to search.
// ----------------------------------------------------------------------------
//
TBool CBTServiceSearcher::HasFinishedSearching() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::BTDevAddr()
// Returns the bluetooth device address.
// ----------------------------------------------------------------------------
//
const TBTDevAddr& CBTServiceSearcher::BTDevAddr()
    {
    return iResponse().BDAddr();
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::ResponseParams()
// Returns information about the device selected by the user.
// ----------------------------------------------------------------------------
//
const TBTDeviceResponseParams& CBTServiceSearcher::ResponseParams()
    {
    return iResponse();
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::HasFoundService()
// True if a service has been found.
// ----------------------------------------------------------------------------
//
TBool CBTServiceSearcher::HasFoundService() const
    {
    return iHasFoundService;
    }

// End of File
