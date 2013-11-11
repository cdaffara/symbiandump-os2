/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements part of the UsbMan USB Class Framework
*
*/

/**
 @file
*/
 
#include <e32debug.h>
#include <e32std.h>
#include <usb_std.h>
#include "../../public/clientServerShared.h"
#include "../../ClassControllerClientSession/inc/classControllerClientSession.h"
#include "CUsbObexClassController.h"


// Panic category only used in debug builds
#ifdef _DEBUG
_LIT( KObexCcPanicCategory, "UsbObexCc" );
#endif

/**
 * Panic codes for the USB OBEX Class Controller.
 */
enum TObexCCPanic
	{
	/** Asynchronous function called (not needed, as all requests complete synchronously) */
	EUnusedFunction = 0,
	/** Start() called while in an illegal state */
	EBadApiCallStart = 1,
	/** Stop() called while in an illegal state */
	EBadApiCallStop = 2,
	};


/**
 * Constructs a CUsbObexClassController object
 *
 * @param	aOwner	USB Device that owns and manages the class
 *
 * @return	A new CUsbObexClassController object
 */
 
CUsbObexClassController* CUsbObexClassController::NewL(
	MUsbClassControllerNotify& aOwner)
	{
	CUsbObexClassController* r = new (ELeave) CUsbObexClassController(aOwner);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

/**
 * Destructor
 */
CUsbObexClassController::~CUsbObexClassController()
	{
	iClassContClient.Close();
	}



/**
 * Constructor.
 *
 * @param	aOwner	USB Device that owns and manages the class
 */
CUsbObexClassController::CUsbObexClassController(
	MUsbClassControllerNotify& aOwner)
	: CUsbClassControllerPlugIn(aOwner, KObexStartupPriority)
	{
	}



/**
 * 2nd Phase Construction.
 */
void CUsbObexClassController::ConstructL()
	{
	}



/**
 * Called by UsbMan when it wants to start the USB Obex class. 
 * Data member from RSessionBase implements a Start call on the
 * Obex Server process.
 * 
 * @param aStatus The caller's request status, filled in with an error code
 */
void CUsbObexClassController::Start(TRequestStatus& aStatus)
	{
	//Start() should never be called if started, starting or stopping (or in state EUsbServiceFatalError)
	__ASSERT_DEBUG( iState == EUsbServiceIdle, User::Panic(KObexCcPanicCategory, EBadApiCallStart) );

	TRequestStatus* status = &aStatus;
	TInt err;
	iState = EUsbServiceStarting;

	err = iClassContClient.Connect(); // Connect to the Server Session
	if (err != KErrNone)
		{
		iState = EUsbServiceFatalError;
		User::RequestComplete(status, err);
		}
	else
		{
		err = iClassContClient.StartService(); // Call function to implement Obex Server
		if (err != KErrNone)
			{
			iState = EUsbServiceFatalError;
			User::RequestComplete(status, err);
			}
		else
			{
			iState = EUsbServiceStarted;
			User::RequestComplete(status,KErrNone);
			}
		}

	}

/**
 * Called by UsbMan when it wants to stop the USB Obex class.
 * Data member from RSessionBase implements a Stop call on
 * the Obex Server process.
 *
 * @param aStatus The caller's request status: always set to KErrNone
 */
void CUsbObexClassController::Stop(TRequestStatus& aStatus)
	{
	
	//Stop() should never be called if stopping, idle or starting (or in state EUsbServiceFatalError)
	__ASSERT_DEBUG( iState == EUsbServiceStarted, User::Panic(KObexCcPanicCategory, EBadApiCallStop) );
	
	TRequestStatus* status = &aStatus;
	TInt err;
	iState = EUsbServiceStopping;

	err = iClassContClient.StopService();
	if (err != KErrNone)
		{
		iState = EUsbServiceFatalError;
		User::RequestComplete(status, err);
		}
	else
		{	
		iState = EUsbServiceIdle;
		User::RequestComplete(status, KErrNone);
		}
			
	}

/**
 * Gets information about the descriptor which this class provides.
 *
 * @param aDescriptorInfo Descriptor info structure filled in by this function
 */
void CUsbObexClassController::GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const
	{
	aDescriptorInfo.iNumInterfaces = KNumObexInterfaces;
	aDescriptorInfo.iLength = KObexDescriptorLength;
	}



/**
 * Standard active object RunL. Never called because this class has no
 * asynchronous requests. As it is never called, function causes a panic.
 */
void CUsbObexClassController::RunL()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KObexCcPanicCategory, EUnusedFunction));
	}

/**
 * Standard active object cancellation function. Never called because this
 * class has no asynchronous requests. As it is never called, function causes a panic.
 */
void CUsbObexClassController::DoCancel()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KObexCcPanicCategory, EUnusedFunction));
	}


/**
 * Standard active object error-handling function. 
 *
 * Should return KErrNone to avoid an active scheduler panic. This function
 * should never be called as there is another mechanism for catching errors.
 */
TInt CUsbObexClassController::RunError(TInt /*aError*/)
	{	
	__ASSERT_DEBUG(EFalse, User::Panic(KObexCcPanicCategory, EUnusedFunction));
	return KErrNone;
	}
	
	
