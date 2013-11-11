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
* Adheres to the UsbMan USB Class Controller API and talks to C32
* to manage the stub3.CSY that is used to provide a virtual
* serial port service to clients
*
*/

/**
 @file
*/

#include <usb_std.h>
#include <es_ini.h>
#include <d32usbc.h>

#include "usbms_stub.h"
#include "UsbmanInternalConstants.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "usbms_stubTraces.h"
#endif

// Panic category 
_LIT( Kstub3CcPanicCategory, "UsbstubCc" );


/**
 * Panic codes for the USB stub3 Class Controller.
 */
enum Tstub3CcPanic
	{
	/** Class called while in an illegal state */
	EBadApiCall = 0,
	/** Asynchronous function called (not needed, as all requests complete synchronously) */
	EUnusedFunction = 1,
	/** Error reading ini file. */
	EPanicBadIniFile = 2,		
	/** Bad value for the iNumberOfstub3Functions member.*/
	EPanicBadNumberOfstub3Functions = 3,
	
	EPanicUnexpectedStatus,
	EPanicUnexpectedState

	};
	

/**
 * Constructs a CUsbstub3ClassController object
 *
 * @param	aOwner	USB Device that owns and manages the class
 *
 * @return	A new CUsbstub3ClassController object
 */
CUsbstub3ClassController* CUsbstub3ClassController::NewL(
	MUsbClassControllerNotify& aOwner)
	{
	CUsbstub3ClassController* r = new (ELeave) CUsbstub3ClassController(aOwner);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

/**
 * Destructor
 */
CUsbstub3ClassController::~CUsbstub3ClassController()
	{
	Cancel();

	iTimer.Close();
	}



/**
 * Constructor.
 *
 * @param	aOwner	USB Device that owns and manages the class
 */
CUsbstub3ClassController::CUsbstub3ClassController(
	MUsbClassControllerNotify& aOwner)
	: CUsbClassControllerPlugIn(aOwner, Kstub3StartupPriority),
	iStartDelay(Kstub3CCDefaultDelay),
	iStopDelay(Kstub3CCDefaultDelay),
	iFailToStart(EFalse),
	iFailToStop(EFalse)	

	{
	iTimer.CreateLocal();
	}



/**
 * 2nd Phase Construction.
 */
void CUsbstub3ClassController::ConstructL()
	{
	}

/**
 * Called by UsbMan when it wants to start the USB stub3 class. This always
 * completes immediately.
 *
 * @param aStatus The caller's request status, filled in with an error code
 */
void CUsbstub3ClassController::Start(TRequestStatus& aStatus)
	{

	aStatus = KRequestPending;
	iReportStatus = &aStatus;
	//If we are already started then just complete the request.
	if (iState == EUsbServiceStarted)
		{
		User::RequestComplete(iReportStatus, KErrNone);
		return;
		}

	if (iFailToStart)
		{
		User::RequestComplete(iReportStatus, KErrGeneral);
		return;
		}
	
	iState = EUsbServiceStarting;

	iTimer.After(iStatus, iStartDelay*1000);  //convert from usec to msec
	SetActive();
	}

/**
 * Called by UsbMan when it wants to stop the USB stub3 class.
 *
 * @param aStatus The caller's request status: always set to KErrNone
 */
void CUsbstub3ClassController::Stop(TRequestStatus& aStatus)
	{

	aStatus = KRequestPending;
	iReportStatus = &aStatus;
	//If we are already idle then just complete the request.
	if (iState == EUsbServiceIdle)
		{
		User::RequestComplete(iReportStatus, KErrNone);
		return;
		}

	if (iFailToStop)
		{
		User::RequestComplete(iReportStatus, KErrGeneral);
		return;
		}

	iState = EUsbServiceStopping;

	
	iTimer.After(iStatus, iStopDelay*1000);  //convert from usec to msec
	SetActive();
	}

/**
 * Gets information about the descriptor which this class provides.
 *
 * @param aDescriptorInfo Descriptor info structure filled in by this function
 */
void CUsbstub3ClassController::GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const
	{

	aDescriptorInfo.iLength = Kstub3DescriptorLength;
	aDescriptorInfo.iNumInterfaces = Kstub3NumberOfInterfacesPerstub3Function;
	}


/**
 * Standard active object RunL. 
 */
void CUsbstub3ClassController::RunL()
	{
    if(iStatus != KErrNone)
    {
        OstTrace1( TRACE_FATAL, CUSBSTUB3CLASSCONTROLLER_RUNL, 
                        "CUsbstub3ClassController::RunL panic with error %d", 
                        EPanicUnexpectedState);
        
        __ASSERT_DEBUG(EFalse, User::Panic(Kstub3CcPanicCategory, EPanicUnexpectedStatus));
    }
	
	switch (iState)
		{
		case EUsbServiceStarting:
			iState = EUsbServiceStarted;
			break;
		case EUsbServiceStopping:
			iState = EUsbServiceIdle;
			break;
		default:
		    OstTrace1( TRACE_FATAL, CUSBSTUB3CLASSCONTROLLER_RUNL_DUP1, 
		            "CUsbstub3ClassController::RunL panic with error %d", EPanicUnexpectedState);
		    User::Panic(Kstub3CcPanicCategory,EPanicUnexpectedState);
		}
	*iReportStatus = KErrNone;	
	User::RequestComplete(iReportStatus, iStatus.Int());	
	}

/**
 * Standard active object cancellation function. Never called because this
 * class has no asynchronous requests.
 */
void CUsbstub3ClassController::DoCancel()
	{

	if (IsActive())
		{
		iTimer.Cancel();	
		}
	switch (iState)
	{
		case EUsbServiceStarting:
			iState = EUsbServiceIdle;
			break;
		case EUsbServiceStopping:
			iState = EUsbServiceStarted;
			break;
		default:
		    OstTrace1( TRACE_FATAL, CUSBSTUB3CLASSCONTROLLER_DOCANCEL, 
		                                "CUsbstub3ClassController::DoCancel panic with error %d", 
		                                EPanicUnexpectedState);
			User::Panic(Kstub3CcPanicCategory, EPanicUnexpectedState);
	}
	*iReportStatus = KErrNone;		
	User::RequestComplete(iReportStatus, KErrCancel);	
	}

/**
 * Standard active object error function. Never called because this class has
 * no asynchronous requests, and hence its RunL is never called.
 *
 * @param aError The error code (unused)
 * @return Always KErrNone to avoid an active scheduler panic
 */
TInt CUsbstub3ClassController::RunError(TInt /*aError*/)
	{
    OstTrace1( TRACE_FATAL, CUSBSTUB3CLASSCONTROLLER_RUNERROR, 
                            "CUsbstub3ClassController::RunError panic with error %d", 
                            EUnusedFunction);
	__ASSERT_DEBUG( EFalse, User::Panic(Kstub3CcPanicCategory, EUnusedFunction));
	return KErrNone;
	}

TInt CUsbstub3ClassController::SetUpInterface()
/**
 * Set up the interface for use. This involves finding a "Interrupt IN" 
 * endpoint and, if found, configuring the interface.
 */
	{
    return 0;
	}

	
//
// End of file
