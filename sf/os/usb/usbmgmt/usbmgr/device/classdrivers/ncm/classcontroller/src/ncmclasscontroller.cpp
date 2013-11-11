/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * @file
 * @internalComponent
 */

#include <usb_std.h>
#include <barsc.h>
#include <barsread.h>
#include <cusbclasscontrollerbase.h> 
#include <musbclasscontrollernotify.h>
#include <usb/usbncm.h>
#include <random.h>

#include "ncmclasscontroller.h"
#include "ncmconnectionmanager.h"
#include "ncmclientmanager.h"

// For OST tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmclasscontrollerTraces.h"
#endif


using namespace UsbNcm;
_LIT(KNcmControllerPanic, "UsbNcmCC"); // must be <=16 chars
const TInt KNcmStartupPriority = 3;

// MTU size. 
extern const TUint KEthernetFrameSize = 8192;

// Lengths of the various bits of the NCM descriptor. Taken from the NCM Specification rev 1.0.
const TInt KNcmInterfaceDescriptorLength = 3;
const TInt KNcmCcHeaderDescriptorLength = 5;
const TInt KNcmFunctionalDescriptorLength = 4;
const TInt KNcmCcUfdDescriptorLength = 5;
const TInt KNcmDataClassHeaderDescriptorLength = 5;
const TInt KNcmDeviceDescriptorLength = 18;
const TInt KNcmConfigurationDescriptorLength = 9;
const TInt KNcmCommunicationClassEndpointOutDescriptorLength = 9;
const TInt KNcmNotificationEndpointDescriptorLength = 7;
const TInt KNcmDataClassInterfaceDescriptorLength = 9;
const TInt KNcmDataClassEndpointInDescriptorLength = 7;
const TInt KNcmDataClassEndpointOutDescriptorLength = 7;

const TInt KNcmDescriptorLength = KNcmInterfaceDescriptorLength
        + KNcmCcHeaderDescriptorLength + KNcmFunctionalDescriptorLength
        + KNcmCcUfdDescriptorLength + KNcmDataClassHeaderDescriptorLength
        + KNcmDeviceDescriptorLength + KNcmConfigurationDescriptorLength
        + KNcmCommunicationClassEndpointOutDescriptorLength
        + KNcmNotificationEndpointDescriptorLength
        + KNcmDataClassInterfaceDescriptorLength
        + KNcmDataClassEndpointInDescriptorLength
        + KNcmDataClassEndpointOutDescriptorLength;

// Panic codes
enum TNcmClassControllerPanicCode
    {
    ENcmPanicBadState = 1,
    ENcmPanicOutstandingRequestFromDevice,
    ENcmPanicAlreadyActive,
    ENcmPanicUnhandledError,
    ENcmPanicBadApiCallStart, // Attempt to call Start() when in illegal state
    ENcmPanicBadApiCallStop, // Attempt to call Stop() when in illegal state
    ENcmPanicUnexpectedIapState,
    ENcmPanicUnexpectedError
    };

/**
 * Constructs a CNcmClassController object.
 * @param aOwner USB Device that owns and manages the class
 * @return Ownership of a new CNcmClassController object
 */
CNcmClassController* CNcmClassController::NewL(
        MUsbClassControllerNotify & aOwner)
    {
    OstTraceFunctionEntry0( CNCMCLASSCONTROLLER_NEWL_ENTRY );

    CNcmClassController* self =
            new (ELeave) CNcmClassController(aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_NEWL_EXIT, ( TUint )( self ) );
    return self;
    }

/**
 * Constructor.
 * @param aOwner USB Device that owns and manages the class
 */
CNcmClassController::CNcmClassController(
        MUsbClassControllerNotify& aOwner) :
    CUsbClassControllerPlugIn(aOwner, KNcmStartupPriority)
    {
    OstTraceFunctionEntryExt( CNCMCLASSCONTROLLER_CNCMCLASSCONTROLLER_ENTRY, this );
    
    iState = EUsbServiceIdle; // needs explicit initialisation as non-zero
    
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_CNCMCLASSCONTROLLER_EXIT, this );
    }

/**
 * Method to perform second phase construction.
 */
void CNcmClassController::ConstructL()
    {
    OstTraceFunctionEntry1( CNCMCLASSCONTROLLER_CONSTRUCTL_ENTRY, this );
    
#ifndef OVERDUMMY_NCMCC
    TInt err = KErrNone;
    
    OstTrace0(TRACE_NORMAL, CNCMCLASSCONTROLLER_CONSTRUCTL, "About to load eusbcsc!");
    _LIT(KUsbSCLDDName, "eusbcsc");
    err = User::LoadLogicalDevice(KUsbSCLDDName);
    OstTrace1( TRACE_NORMAL, CNCMCLASSCONTROLLER_CONSTRUCTL_LOAD_CSC_LDD, "LoadLogicalDevice() returns %d!", err );
    if (err != KErrNone && err != KErrAlreadyExists)
        {
        User::Leave(err);
        }
    OstTrace0(TRACE_NORMAL, CNCMCLASSCONTROLLER_CONSTRUCTL_CSC_LDD_LOADED, "Ldd eusbcsc loaded!");    
#endif // OVERDUMMY_NCMCC
    
    RandomMacAddressL(); // Create a random MAC address for NCM host side interface.
    iConnectionMan = CNcmConnectionManager::NewL(*this, iHostMacAddress, iDataBufferSize, iNcmInternalSvr);
    iClientMgr = new  (ELeave) CNcmClientManager(iHostMacAddress);
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_CONSTRUCTL_EXIT, this );
    }

void CNcmClassController::RandomMacAddressL()
    {
    OstTraceFunctionEntry1( CNCMCLASSCONTROLLER_RANDOMMACADDRESSL_ENTRY, this );
    
    //Create the NCM interface MAC address randomly 
    iHostMacAddress.SetLength(KEthernetAddressLength);
    TRandom::RandomL(iHostMacAddress);
  
    //Mark it a locally administered address
    iHostMacAddress[0] = 0x02;
    iHostMacAddress[1] = 0x00;
    iHostMacAddress[2] = 0x00;
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_RANDOMMACADDRESSL_EXIT, this );
    }

/**
 * Destructor.
 */
CNcmClassController::~CNcmClassController()
    {
    OstTraceFunctionEntry1( CNCMCLASSCONTROLLER_CNCMCLASSCONTROLLER_ENTRY_DESTROY, this );
    Cancel();

    // Close internal server to release some resource
    iNcmInternalSvr.Close();
    delete iClientMgr;
    delete iConnectionMan;

#ifndef OVERDUMMY_NCMCC
    OstTrace0(TRACE_NORMAL, CNCMCLASSCONTROLLER_CNCMCLASSCONTROLLER_PRE_UNLOAD_CSC_LDD, "About to unload Usbcsc!");
    _LIT(KUsbSCLDDName, "Usbcsc");
    TInt err = User::FreeLogicalDevice(KUsbSCLDDName);
    OstTrace1(TRACE_NORMAL, CNCMCLASSCONTROLLER_CNCMCLASSCONTROLLER_UNLOAD_LDD_RESULT, "FreeLogicalDevice() returns %d!", err);
#endif // OVERDUMMY_NCMCC
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_CNCMCLASSCONTROLLER_EXIT_DESTROY, this );
    }

/**
 * Called by UsbMan to start this class.
 * @param aStatus Will be completed with success or failure.
 */
void CNcmClassController::Start(TRequestStatus& aStatus)
    {
    OstTraceFunctionEntryExt( CNCMCLASSCONTROLLER_START_ENTRY, this );
    OstTrace1(TRACE_NORMAL, CNCMCLASSCONTROLLER_START_PRINT_STATE, "iState=%d", iState);
    
    //Start() should only be called if the CC is idle state		
    __ASSERT_DEBUG((iState == EUsbServiceIdle),
            User::Panic(KNcmControllerPanic, ENcmPanicBadApiCallStart));

    // NB We enforce that the device doesn't re-post requests on us.
    __ASSERT_DEBUG(!iReportStatus,
            User::Panic(KNcmControllerPanic, ENcmPanicOutstandingRequestFromDevice));

    // We should not be active before start
    __ASSERT_DEBUG(!IsActive(), User::Panic(KNcmControllerPanic, ENcmPanicAlreadyActive));

    aStatus = KRequestPending;
    iReportStatus = &aStatus;

    iState = EUsbServiceStarting;

    // According to the latest discussion of 100ms limitation to 
    // each personality starting, the calling to iConnectionMan->Start
    // will happen later. A controller will set sevice state to started
    // immediately after calling to RDevUsbcScClient::SetInterface()

    TInt err = KErrNone;
    
    // Can not leave. Trap any error from SetNcmInterfacesL()
    TRAP(err, iClientMgr->SetNcmInterfacesL(iDataBufferSize));
    User::RequestComplete(iReportStatus, err);
    
    if (KErrNone == err)
        {
        // Set service state to started
        iState = EUsbServiceStarted;
        
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_START_EXIT, this );
    }

/**
 * Called by UsbMan to stop this class.
 * @param aStatus Will be completed with success or failure.
 */
void CNcmClassController::Stop(TRequestStatus& aStatus)
    {
    OstTraceFunctionEntryExt( CNCMCLASSCONTROLLER_STOP_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMCLASSCONTROLLER_STOP_PRINT_STATE, "iState=%d", iState );

    __ASSERT_DEBUG(((iState == EUsbServiceStarted)||(iState == EUsbServiceStarting)),
            User::Panic(KNcmControllerPanic, ENcmPanicBadApiCallStop));

    // NB We enforce that the device doesn't re-post requests on us.
    __ASSERT_DEBUG(!iReportStatus,
            User::Panic(KNcmControllerPanic, ENcmPanicOutstandingRequestFromDevice));
        
    if (IsActive()) // Networking connetion building is ongoing!
        {
        Cancel();
        }
    else
        {
        iConnectionMan->Stop();
        }

    aStatus = KRequestPending;
    iReportStatus = &aStatus;
    User::RequestComplete(iReportStatus, KErrNone);
    
    iConnectingToNcmPktDrv = EFalse;
    iState = EUsbServiceIdle;
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_STOP_EXIT, this );
    }

/**
 * Returns information about the interfaces supported by this class.
 * @param aDescriptorInfo Will be filled in with interface information.
 */
void CNcmClassController::GetDescriptorInfo(
        TUsbDescriptor& aDescriptorInfo) const
    {
    OstTraceFunctionEntryExt( CNCMCLASSCONTROLLER_GETDESCRIPTORINFO_ENTRY, this );

    aDescriptorInfo.iNumInterfaces = 2;
    aDescriptorInfo.iLength = KNcmDescriptorLength;
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_GETDESCRIPTORINFO_EXIT, this );
    }

void CNcmClassController::McmoErrorIndication(TInt aError)
    {
    OstTraceFunctionEntryExt( CNCMCLASSCONTROLLER_MCMOERRORINDICATION_ENTRY, this );

    Owner().UccnError(aError);
    
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_MCMOERRORINDICATION_EXIT, this );
    }

/**
 * Called when connection manager completes.
 */
void CNcmClassController::RunL()
    {
    OstTraceFunctionEntry1( CNCMCLASSCONTROLLER_RUNL_ENTRY, this );
    TInt completionCode = iStatus.Int();
    OstTraceExt2( TRACE_NORMAL, CNCMCLASSCONTROLLER_RUNL_PRINT_INFO, "iState=%d; completionCode=%d", iState, completionCode );
    
    // Goto RunError() to handle any exception
    User::LeaveIfError(completionCode);
    
    // We should only be in starting state when connection manager completes. Stopping is a synchronous call.
    __ASSERT_DEBUG(EUsbServiceStarted == iState, User::Panic(KNcmControllerPanic, 
            ENcmPanicBadState));

    switch (iState)
        {
        case EUsbServiceStarted:
            {            
            OstTrace0( TRACE_NORMAL, CNCMCLASSCONTROLLER_RUNL_STARTED, "EUsbServiceStarted" );
            if (iConnectingToNcmPktDrv)
                {
                OstTrace0( TRACE_FLOW, CNCMCLASSCONTROLLER_RUNL_TRANSFER_HANDLES, "An Ethernet connection over NCM built. Transfer interface handlers to NCM internal server." );
                // NCM Packet Driver Loaded in C32, now transfer LDD handles to packet driver side through NCM internal server
                iClientMgr->TransferInterfacesL(iNcmInternalSvr);
                            
                delete iClientMgr;
                iClientMgr = NULL;
                }
            else
                {
                OstTrace0( TRACE_NORMAL, CNCMCLASSCONTROLLER_RUNL_BUILT_NCM_CONNECTION, "Going to build ethernet connection over NCM!" );
                iConnectingToNcmPktDrv = ETrue;
                iStatus = KRequestPending;
                SetActive();
                iConnectionMan->Start(iStatus);
                }
            
            break;
            }
            
        default:
            {
            OstTrace0( TRACE_ERROR, CNCMCLASSCONTROLLER_RUNL_DEFAULT, "Default::SHOULD NOT BE HERE!!!" );
            User::Panic(KNcmControllerPanic, ENcmPanicBadState);
            break;
            }
        }
    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_RUNL_EXIT, this );
    }

/**
 * Implements cancellation of an outstanding request.
 */
void CNcmClassController::DoCancel()
    {
    OstTraceFunctionEntry1( CNCMCLASSCONTROLLER_DOCANCEL_ENTRY, this );

    OstTraceExt2( TRACE_NORMAL, CNCMCLASSCONTROLLER_DOCANCEL_INFO_PRINT, "iState=%d; iReportStatus=%p", iState, iReportStatus );

    // Update our iState. If we're starting, then roll back to idle. If we're 
    // stopping, role back to started. Nothing else is legal.
    switch (iState)
        {
        case EUsbServiceStarted:
            OstTrace0( TRACE_NORMAL, CNCMCLASSCONTROLLER_DOCANCEL_IDLE, "EUsbServiceIdle" );
            iState = EUsbServiceIdle;
            if (iConnectingToNcmPktDrv)
                {
                iConnectionMan->StartCancel();
                }
            __ASSERT_DEBUG(!iReportStatus, User::Panic(KNcmControllerPanic, ENcmPanicUnhandledError));
            
            OstTraceFunctionExit1( CNCMCLASSCONTROLLER_DOCANCEL_EXIT, this );
            return;
            
        default:
            OstTrace1(TRACE_FATAL, CNCMCLASSCONTROLLER_DOCANCEL_DEFAULT, "Default::Should not be here:iState=%d", iState);
            User::Panic(KNcmControllerPanic, ENcmPanicBadState);
            break;
        }

    // Complete the client's request.	
    __ASSERT_DEBUG(iReportStatus, User::Panic(KNcmControllerPanic, ENcmPanicUnhandledError));
    User::RequestComplete(iReportStatus, KErrCancel);

    OstTraceFunctionExit1( CNCMCLASSCONTROLLER_DOCANCEL_EXIT_DUP1, this );
    }

/**
 * Called when RunL leaves. Does nothing, just return KErrNone.
 * @return Error code. Simply return KErrNone.
 */
TInt CNcmClassController::RunError(TInt aError)
    {
    OstTraceFunctionEntryExt( CNCMCLASSCONTROLLER_RUNERROR_ENTRY, this );
    
    if (KErrCancel != aError)
        {
        // Report this failure to the observer
        // Finally this will report to usbman and NCM class will be stopped.    
        McmoErrorIndication(aError);
        }

    OstTraceFunctionExitExt( CNCMCLASSCONTROLLER_RUNERROR_EXIT, this, KErrNone );
    return KErrNone;
    }
