/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Adheres to the UsbMan USB Class Controller API and talks to C32 or ACM server 
* to manage the ECACM.CSY that is used to provide a virtual serial port service 
* to clients.
*
*/

/**
 @file
*/

#include <usb_std.h>
#include <acminterface.h>
#include <usb/acmserver.h>
#include "CUsbACMClassController.h"
#include "UsbmanInternalConstants.h"
#include "acmserverconsts.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbACMClassControllerTraces.h"
#endif

#ifdef _DEBUG
// Panic category 
_LIT( KAcmCcPanicCategory, "UsbAcmCc" );
#endif


/**
 * Panic codes for the USB ACM Class Controller.
 */
enum TAcmCcPanic
	{
	/** Start called while in an illegal state */
	EBadApiCallStart = 0,
	/** Asynchronous function called (not needed, as all requests complete synchronously) */
	EUnusedFunction = 1,
	/** Value reserved */
	EPanicReserved2 = 2,		
	/** Value reserved */
	EPanicReserved3 = 3,
	/** Stop called while in an illegal state */
	EBadApiCallStop = 4,
	};


/**
 * Constructs a CUsbACMClassController object
 *
 * @param	aOwner	USB Device that owns and manages the class
 *
 * @return	A new CUsbACMClassController object
 */
CUsbACMClassController* CUsbACMClassController::NewL(MUsbClassControllerNotify& aOwner)
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_NEWL_ENTRY );

	CUsbACMClassController* self = new (ELeave) CUsbACMClassController(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_NEWL_EXIT );
	return self;
	}

/**
 * Destructor
 */
CUsbACMClassController::~CUsbACMClassController()
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_CUSBACMCLASSCONTROLLER_DES_ENTRY );
	Cancel();

#ifdef USE_ACM_REGISTRATION_PORT
	iComm.Close();
	iCommServer.Close();
#else
    iAcmServer.Close();
#endif // USE_ACM_REGISTRATION_PORT
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_CUSBACMCLASSCONTROLLER_DES_EXIT );
	}

/**
 * Constructor.
 *
 * @param	aOwner	USB Device that owns and manages the class
 */
CUsbACMClassController::CUsbACMClassController(
    MUsbClassControllerNotify& aOwner) 
    : CUsbClassControllerPlugIn(aOwner, KAcmStartupPriority),
    iNumberOfAcmFunctions(KDefaultNumberOfAcmFunctions)
    {
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_CUSBACMCLASSCONTROLLER_CONS_ENTRY );
    OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_CUSBACMCLASSCONTROLLER_CONS_EXIT );
    }

/**
 * 2nd Phase Construction.
 */
void CUsbACMClassController::ConstructL()
    {
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_CONSTRUCTL_ENTRY );
	
    iNumberOfAcmFunctions = KUsbAcmNumberOfAcmFunctions;

    iAcmProtocolNum[0] = KUsbAcmProtocolNumAcm1;
    iAcmProtocolNum[1] = KUsbAcmProtocolNumAcm2;
    iAcmProtocolNum[2] = KUsbAcmProtocolNumAcm3;
    iAcmProtocolNum[3] = KUsbAcmProtocolNumAcm4;
    iAcmProtocolNum[4] = KUsbAcmProtocolNumAcm5;

	// Prepare to use whichever mechanism is enabled to control bringing ACM 
	// functions up and down.
    TInt	err;
#ifdef USE_ACM_REGISTRATION_PORT
    
    err = iCommServer.Connect();
    if (err < 0)
    	{
		OstTrace1( TRACE_ERROR, CUSBACMCLASSCONTROLLER_CONSTRUCTL, "CUsbACMClassController::ConstructL;leave err=%d", err );
		User::Leave(err);
    	}
	
    err = iCommServer.LoadCommModule(KAcmCsyName);
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CUSBACMCLASSCONTROLLER_CONSTRUCTL_DUP1, "CUsbACMClassController::ConstructL;leave err=%d", err );
		User::Leave(err);
		}

	TName portName(KAcmSerialName);
	portName.AppendFormat(_L("::%d"), 666);
	// Open the registration port in shared mode in case other ACM CCs want to 
	// open it.
	err = iComm.Open(iCommServer, portName, ECommShared);
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CUSBACMCLASSCONTROLLER_CONSTRUCTL_DUP2, "CUsbACMClassController::ConstructL;leave err=%d", err );
		User::Leave(err);
		}

#else
	
	err = iAcmServer.Connect();
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CUSBACMCLASSCONTROLLER_CONSTRUCTL_DUP3, "CUsbACMClassController::ConstructL;leave err=%d", err );
		User::Leave(err);
		}

#endif // USE_ACM_REGISTRATION_PORT
    OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_CONSTRUCTL_EXIT );
    }

/**
 * Called by UsbMan when it wants to start the USB ACM class. This always
 * completes immediately.
 *
 * @param aStatus The caller's request status, filled in with an error code
 */
void CUsbACMClassController::Start(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_START_ENTRY );	

	// We should always be idle when this function is called (guaranteed by
	// CUsbSession).
	if (iState != EUsbServiceIdle)
		{
		OstTrace1( TRACE_FATAL, CUSBACMCLASSCONTROLLER_START, "CUsbACMClassController::Start;iState=%d", (TInt)iState );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmCcPanicCategory, EBadApiCallStart) );
		}

	TRequestStatus* reportStatus = &aStatus;
	TRAPD(err, DoStartL());
	iState = (err == KErrNone) ? EUsbServiceStarted : EUsbServiceIdle;
	User::RequestComplete(reportStatus, err);
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_START_EXIT );
	}

void CUsbACMClassController::DoStartL()
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_DOSTARTL_ENTRY );
	

	iState = EUsbServiceStarting;
	OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL, "CUsbACMClassController::DoStartL;iNumberOfAcmFunctions=%d", iNumberOfAcmFunctions );

#ifdef USE_ACM_REGISTRATION_PORT

    // Create ACM functions.
    TUint acmSetting;
    for (TUint i = 0; i < iNumberOfAcmFunctions; i++)
        {
		OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP1, "CUsbACMClassController::DoStartL;iAcmProtocolNum[i]=%u", iAcmProtocolNum[i] );

        // indicate the number of ACMs to create, and its protocol number (in the 3rd-lowest byte)
        acmSetting = 1 | (static_cast<TUint> (iAcmProtocolNum[i]) << 16);
        TInt err = iComm.SetSignalsToMark(acmSetting);
        if (err != KErrNone)
            {
			OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP2, "CUsbACMClassController::DoStartL;SetSignalsToMark error = %d", err );
            if (i != 0)
                {
                // Must clear any ACMs that have completed.
                // only other than KErrNone if C32 Server fails
                (void) iComm.SetSignalsToSpace(i);
                }
            OstTrace1( TRACE_ERROR, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP6, "CUsbACMClassController::DoStartL;leave err=%d", err );
            User::Leave(err);
            }
        }

#else // use ACM server
    // Create ACM functions
    for ( TInt i = 0; i < iNumberOfAcmFunctions; i++ )
        {
        TInt err;
        //Use default control interface name and data interface name
        //For improving performance, control interface name and data interface name configurable 
        //is not supported now.
        err = iAcmServer.CreateFunctions(1, iAcmProtocolNum[i], KControlIfcName, KDataIfcName);

        if ( err != KErrNone )
            {
			OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP3, "CUsbACMClassController::DoStartL;\tFailed to create ACM function. Error: %d", err);
			
            if (i != 0)
                {
                //Must clear any ACMs that have been completed
                iAcmServer.DestroyFunctions(i);
                OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP4, "CUsbACMClassController::DoStartL;\tDestroyed %d Interfaces", i );
                }
            OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP7, "CUsbACMClassController::DoStartL; leave Error: %d", err);
            User::Leave(err);
            }
        }

#endif // USE_ACM_REGISTRATION_PORT
	
	OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTARTL_DUP5, "CUsbACMClassController::DoStartL;\tCreated %d ACM Interfaces", iNumberOfAcmFunctions );
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_DOSTARTL_EXIT );
	}

/**
 * Called by UsbMan when it wants to stop the USB ACM class.
 *
 * @param aStatus The caller's request status: always set to KErrNone
 */
void CUsbACMClassController::Stop(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_STOP_ENTRY );

	// We should always be started when this function is called (guaranteed by
	// CUsbSession).
	//User::Panic(KAcmCcPanicCategory, EBadApiCallStop);
	if (iState != EUsbServiceStarted)
		{
		OstTrace1( TRACE_FATAL, CUSBACMCLASSCONTROLLER_STOP, "CUsbACMClassController::Stop;iState=%d", (TInt)iState );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmCcPanicCategory, EBadApiCallStop) );
		}

	TRequestStatus* reportStatus = &aStatus;
	DoStop();
	User::RequestComplete(reportStatus, KErrNone);
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_STOP_EXIT );
	}

/**
 * Gets information about the descriptor which this class provides.
 *
 * @param aDescriptorInfo Descriptor info structure filled in by this function
 */
void CUsbACMClassController::GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_GETDESCRIPTORINFO_ENTRY );
	aDescriptorInfo.iLength = KAcmDescriptorLength;
	aDescriptorInfo.iNumInterfaces = KAcmNumberOfInterfacesPerAcmFunction*(iNumberOfAcmFunctions);
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_GETDESCRIPTORINFO_EXIT );
	}

/**
Destroys ACM functions we've already brought up.
 */
void CUsbACMClassController::DoStop()
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_DOSTOP_ENTRY );
	if (iState == EUsbServiceStarted)
		{
#ifdef USE_ACM_REGISTRATION_PORT
		TInt err = iComm.SetSignalsToSpace(iNumberOfAcmFunctions);
		if (err != KErrNone)
			{
			OstTrace1( TRACE_FATAL, CUSBACMCLASSCONTROLLER_DOSTOP_DUP1, "CUsbACMClassController::DoStop;err=%d", err );
			User::Invariant();
			}
		//the implementation in CRegistrationPort always return KErrNone
		(void)err;
		// If there is an error here, USBSVR will just ignore it, but 
		// it indicates that our interfaces are still up. We know the CSY 
		// doesn't raise an error, but an IPC error may have occurred. This is 
		// a problem with USBSVR in general- Stops are more or less assumed to 
		// work.
#else
		// Destroy interfaces. Can't do anything with an error here.
		static_cast<void>(iAcmServer.DestroyFunctions(iNumberOfAcmFunctions));
#endif // USE_ACM_REGISTRATION_PORT
		OstTrace1( TRACE_NORMAL, CUSBACMCLASSCONTROLLER_DOSTOP, "CUsbACMClassController::DoStop;\tDestroyed %d Interfaces", iNumberOfAcmFunctions );
		iState = EUsbServiceIdle;
		}
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_DOSTOP_EXIT );
	}

/**
 * Standard active object RunL. Never called because this class has no
 * asynchronous requests.
 */
void CUsbACMClassController::RunL()
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_RUNL_ENTRY );
	OstTrace0( TRACE_FATAL, CUSBACMCLASSCONTROLLER_RUNL, "CUsbACMClassController::RunL;EUnusedFunction" );
	__ASSERT_DEBUG( EFalse, User::Panic(KAcmCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_RUNL_EXIT );
	}

/**
 * Standard active object cancellation function. Never called because this
 * class has no asynchronous requests.
 */
void CUsbACMClassController::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_DOCANCEL_ENTRY );
	OstTrace0( TRACE_FATAL, CUSBACMCLASSCONTROLLER_DOCANCEL, "CUsbACMClassController::DoCancel;EUnusedFunction" );
	__ASSERT_DEBUG( EFalse, User::Panic(KAcmCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_DOCANCEL_EXIT );
	}

/**
 * Standard active object error function. Never called because this class has
 * no asynchronous requests, and hence its RunL is never called.
 *
 * @param aError The error code (unused)
 * @return Always KErrNone to avoid an active scheduler panic
 */
TInt CUsbACMClassController::RunError(TInt /*aError*/)
	{
	OstTraceFunctionEntry0( CUSBACMCLASSCONTROLLER_RUNERROR_ENTRY );
	OstTrace0( TRACE_FATAL, CUSBACMCLASSCONTROLLER_RUNERROR, "CUsbACMClassController::RunError;EUnusedFunction" );
	__ASSERT_DEBUG( EFalse, User::Panic(KAcmCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBACMCLASSCONTROLLER_RUNERROR_EXIT );
	return KErrNone;
	}
