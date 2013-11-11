/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Adheres to the UsbMan USB Class Controller API and talks to mass storage 
* file server
*
*/

/**
 @file
 @internalTechnology
*/

#include <barsc.h> 
#include <barsread.h>
#include <usb_std.h>
#include <cusbclasscontrollerplugin.h>
#include <usbms.rsg>
#include "CUsbMsClassController.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbMsClassControllerTraces.h"
#endif

#ifdef _DEBUG
// Panic category 
_LIT( KMsCcPanicCategory, "UsbMsCc" );
#endif

/**
 Panic codes for the USB mass storage Class Controller.
 */
enum TMsCcPanic
	{
	//Class called while in an illegal state
	EBadApiCall = 0,
    EUnusedFunction = 1,
	};

/**
 Constructs a CUsbMsClassController object
 
 @param	aOwner	USB Device that owns and manages the class
 @return	A new CUsbMsClassController object
 */
CUsbMsClassController* CUsbMsClassController::NewL(
	MUsbClassControllerNotify& aOwner)
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_NEWL_ENTRY );
	CUsbMsClassController* r = new (ELeave) CUsbMsClassController(aOwner);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_NEWL_EXIT );
	return r;
	}

/**
 Destructor
 */
CUsbMsClassController::~CUsbMsClassController()
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_CUSBMSCLASSCONTROLLER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_CUSBMSCLASSCONTROLLER_DES_EXIT );
	}

/**
 Constructor.
 
 @param	aOwner	USB Device that owns and manages the class
 */
CUsbMsClassController::CUsbMsClassController(
	MUsbClassControllerNotify& aOwner)
	: CUsbClassControllerPlugIn(aOwner, KMsStartupPriority)	
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_CUSBMSCLASSCONTROLLER_CONS_ENTRY );
	// Intentionally left blank
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_CUSBMSCLASSCONTROLLER_CONS_EXIT );
	}

/**
 2nd Phase Construction.
 */
void CUsbMsClassController::ConstructL()
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_CONSTRUCT_ENTRY );
	ReadMassStorageConfigL();
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_CONSTRUCT_EXIT );
	}

/**
 Called by UsbMan when it wants to start the mass storage class. 
 
 @param aStatus The caller's request status, filled in with an error code
 */
void CUsbMsClassController::Start(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_START_ENTRY );
	
	// The service state should always be idle when this function is called 
	// (guaranteed by CUsbSession).
	
	if (iState != EUsbServiceIdle)
		{
		OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_START, "CUsbMsClassController::Star;iState=%d", (TInt)iState );
		__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EBadApiCall) );
		}

	TRequestStatus* reportStatus = &aStatus;

	iState = EUsbServiceStarting;

#ifndef __OVER_DUMMYUSBLDD__
	// Connect to USB Mass Storage server
	TInt err = iUsbMs.Connect();
#else
	TInt err = KErrNone;
#endif
	if (err != KErrNone)
		{
		iState = EUsbServiceIdle;
		User::RequestComplete(reportStatus, err);
		OstTrace0( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_START_DUP1, 
				"CUsbMsClassController::Start;Failed to connect to mass storage file server" );
		OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_START_EXIT );
		return;
		}

#ifndef __OVER_DUMMYUSBLDD__
	// Start mass storage device
	err = iUsbMs.Start(iMsConfig);
#else
	err = KErrNone;
#endif

	if (err != KErrNone)
		{
		iState = EUsbServiceIdle;
		// Connection was created successfully in last step
		// Get it closed since failed to start device.
		iUsbMs.Close();
		User::RequestComplete(reportStatus, err);
		OstTrace0( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_START_DUP2, 
						"CUsbMsClassController::Start;Failed to start mass storage device" );
		OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_START_EXIT_DUP1 );
		return;
		}

	iState = EUsbServiceStarted;
	User::RequestComplete(reportStatus, KErrNone);
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_START_EXIT_DUP2 );
	}

/**
 Called by UsbMan when it wants to stop the USB ACM class.
 
 @param aStatus KErrNone on success or a system wide error code
 */
void CUsbMsClassController::Stop(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_STOP_ENTRY );
	
	// The service state should always be started when this function is called
	// (guaranteed by CUsbSession)
	if (iState != EUsbServiceStarted)
		{
		OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_STOP, "CUsbMsClassController::Stop;iState=%d", (TInt)iState );
		__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EBadApiCall) );
		}

	TRequestStatus* reportStatus = &aStatus;
#ifndef __OVER_DUMMYUSBLDD__
	TInt err = iUsbMs.Stop();
	
	if (err != KErrNone)
		{
		iState = EUsbServiceStarted;
		User::RequestComplete(reportStatus, err);
		OstTrace0( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_STOP_DUP1, 
						"CUsbMsClassController::Start;Failed to stop mass storage device" );
		OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_STOP_EXIT );
		return;
		}	

	iUsbMs.Close();
#else
	iState = EUsbServiceIdle;
#endif
	User::RequestComplete(reportStatus, KErrNone);
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_START_STOP_DUP1 );
	}

/**
 Gets information about the descriptor which this class provides. Never called
 by usbMan.
 
 @param aDescriptorInfo Descriptor info structure filled in by this function
 */
void CUsbMsClassController::GetDescriptorInfo(TUsbDescriptor& /*aDescriptorInfo*/) const
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_GETDESCRIPTORINFO_ENTRY );
	OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_GETDESCRIPTORINFO, 
			"CUsbMsClassController::GetDescriptorInfo;panic line=%d", (TInt)__LINE__ );
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_GETDESCRIPTORINFO_EXIT );
	}

/**
 Standard active object RunL. Never called because this class has no
 asynchronous requests.
 */
void CUsbMsClassController::RunL()
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_RUNL_ENTRY );
	OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_RUNL, 
			"CUsbMsClassController::RunL;panic line=%d", (TInt)__LINE__ );
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_RUNL_EXIT );
	}

/**
 Standard active object cancellation function. Never called because this
 class has no asynchronous requests.
 */
void CUsbMsClassController::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_DOCANCEL_ENTRY );
	OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_DOCANCEL, 
			"CUsbMsClassController::DoCancel;panic line=%d", (TInt)__LINE__ );
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_DOCANCEL_EXIT );
	}

/**
 Standard active object error function. Never called because this class has
 no asynchronous requests, and hence its RunL is never called.
 
 @param aError The error code (unused)
 @return Always KErrNone to avoid an active scheduler panic
 */
TInt CUsbMsClassController::RunError(TInt /*aError*/)
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_RUNERROR_ENTRY );
	OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_RUNERROR, 
			"CUsbMsClassController::RunError;panic line=%d", (TInt)__LINE__ );
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_RUNERROR_EXIT );
	return KErrNone;
	}

/**
 Read mass storage configuration info from the resource file
 */
void CUsbMsClassController::ReadMassStorageConfigL()
	{
	OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_ENTRY );

	// Try to connect to file server
	RFs		fs;
	TInt	fserr = fs.Connect();
	
	if (fserr < 0)
		{
		OstTrace1( TRACE_FATAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL, 
				"CUsbMsClassController::ReadMassStorageConfigL;leave err = %d", fserr );
		User::Leave(fserr);
		}
	CleanupClosePushL(fs);

	RResourceFile resource;
	TRAPD(err, resource.OpenL(fs, KUsbMsResource));
	OstTrace1( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_DUP1, 
				"CUsbMsClassController::ReadMassStorageConfigL;Opened resource file with error %d", err );

	if (err != KErrNone)
		{
		OstTrace0( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_DUP2, 
					"CUsbMsClassController::ReadMassStorageConfigL;Unable to open resource file" );
		CleanupStack::PopAndDestroy(&fs);
		OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_EXIT );
		return;
		}

	CleanupClosePushL(resource);

	resource.ConfirmSignatureL(KUsbMsResourceVersion);

	HBufC8* msConfigBuf = 0;
	TRAPD(ret, msConfigBuf = resource.AllocReadL(USBMS_CONFIG));
	if (ret != KErrNone)
		{
		OstTrace0( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_DUP3, 
					"CUsbMsClassController::ReadMassStorageConfigL;Failed to open mass storage configuration file" );
		CleanupStack::PopAndDestroy(2, &fs); 
		OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_EXIT_DUP1 );
		return;
		}
	CleanupStack::PushL(msConfigBuf);
	

	// The format of the USB resource structure is:
	
	/* 	
	 * 	STRUCT USBMASSSTORAGE_CONFIG
	 *	{
	 *	LTEXT	vendorId;           // no more than 8 characters
	 *	LTEXT	productId;          // no more than 16 characters
	 *	LTEXT	productRev;        	// no more than 4 characters
	 *	};
	 */
	 
	// Note that the resource must be read in this order!
	
	TResourceReader reader;
	reader.SetBuffer(msConfigBuf);

	TPtrC	vendorId		= reader.ReadTPtrC();
	TPtrC	productId		= reader.ReadTPtrC();
	TPtrC	productRev		= reader.ReadTPtrC();
	
	// populate iMsConfig, truncate if exceeding limit
	ConfigItem(vendorId, iMsConfig.iVendorId, 8);
	ConfigItem(productId, iMsConfig.iProductId, 16);
	ConfigItem(productRev, iMsConfig.iProductRev, 4);
	
	// Debugging
	OstTraceExt1( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_DUP4, 
					"CUsbMsClassController::ReadMassStorageConfigL;vendorId = %S\n", vendorId );

	OstTraceExt1( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_DUP5, 
					"CUsbMsClassController::ReadMassStorageConfigL;productId = %S\n", productId );

	OstTraceExt1( TRACE_NORMAL, CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_DUP6, 
					"CUsbMsClassController::ReadMassStorageConfigL;productRev = %S\n", productRev );
		
	CleanupStack::PopAndDestroy(3, &fs); // msConfigBuf, resource, fs		
	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_READMASSSTORAGECONFIGL_EXIT_DUP2 );
	}
	
/**
 Utility. Copies the data from TPtr to TBuf and checks data length 
 to make sure the source does not exceed the capacity of the target
 */
 void CUsbMsClassController::ConfigItem(const TPtrC& source, TDes& target, TInt maxLength)
 	{
	 OstTraceFunctionEntry0( CUSBMSCLASSCONTROLLER_CONFIGITEM_ENTRY );
 	if (source.Length() < maxLength)
 		{
 		maxLength = source.Length();
 		}
 		
 	target.Copy(source.Ptr(), maxLength);
 	OstTraceFunctionExit0( CUSBMSCLASSCONTROLLER_CONFIGITEM_EXIT );
 	}

