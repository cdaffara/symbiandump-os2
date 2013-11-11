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
* Implements part of UsbMan USB Class Framework.
*
*/

/**
 @file
*/

#include <usb_std.h>
#include <usb/usblogger.h>
#include "CUsbDummyClassController.h"
#include "inifile.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbDummyClassControllerTraces.h"
#endif



_LIT(KDummyControllerPanic, "UsbDummyCC"); // must be <=16 chars
// Panic codes
enum
	{
	// Bad value for the iSynchronicity member.
	EDummyPanicBadSynchronicity = 0,
	
	// Used for environment errors we do not handle.
	EDummyPanicUnhandledError = 1,
	
	// Used for bad iStatus and other state errors.
	EDummyPanicBadState = 2,
	
	// Error reading ini file.
	EDummyPanicBadIniFile = 3,
	
	// We already have our async request active.
	EDummyPanicAlreadyActive = 4,
	
	// We already have a request outstanding from the device class. 
	EDummyPanicOutstandingRequestFromDevice = 5,
	
	// Attempt to call Start() when in illegal state
	EDummyPanicBadApiCallStart = 6,
	
	// Attempt to call Stop() when in illegal state
	EDummyPanicBadApiCallStop = 7,
	};

const TInt KDummyClassPriority = 1;

// Keys for reading the ini file.
_LIT(KStartup, "Startup");
_LIT(KShutdown, "Shutdown");
_LIT(KType, "Type");
_LIT(KTime, "Time");
_LIT(KError, "Error");
_LIT(KSync, "sync");
_LIT(KAsync, "async");
_LIT(KNever, "never");

CUsbDummyClassController* CUsbDummyClassController::NewL(
		MUsbClassControllerNotify& aOwner,
		TUint aIndex)
/**
 * Constructs a CUsbDummyClassController object.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 * @return Ownership of a new CUsbDummyClassController object
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_NEWL_MUSBCLASSCONTROLLERNOTIFY_TUINT_ENTRY );

	CUsbDummyClassController* self = new(ELeave) CUsbDummyClassController(aOwner, aIndex);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_NEWL_MUSBCLASSCONTROLLERNOTIFY_TUINT_EXIT );
	return self;
	}


CUsbDummyClassController* CUsbDummyClassController::NewL(
	MUsbClassControllerNotify& aOwner, 
	TUint aIndex, TInt aPriority)
/**
 * Constructs a CUsbDummyClassController object.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 * @param aPriority The startup priority of the instance
 * @return Ownership of a new CUsbDummyClassController object
 */
 	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_NEWL_MUSBCLASSCONTROLLERNOTIFY_TUINT_TINT_ENTRY );

 	CUsbDummyClassController* self = new(ELeave) CUsbDummyClassController(aOwner, aIndex, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_NEWL_MUSBCLASSCONTROLLERNOTIFY_TUINT_TINT_EXIT );
	return self;
 	}
 
CUsbDummyClassController::CUsbDummyClassController(
		MUsbClassControllerNotify& aOwner,
		TUint aIndex)
/**
 * Constructor.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 */
 :	CUsbClassControllerBase(aOwner, KDummyClassPriority),
	iIndex(aIndex)
	{
	iState = EUsbServiceIdle; // needs explicit initialisation as non-zero
	}

CUsbDummyClassController::CUsbDummyClassController(
		MUsbClassControllerNotify& aOwner,
		TUint aIndex, TInt aPriority)
		
/**
 * Constructor.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 * @param aPriority a startup priority for the class controller
 */		
 : 	CUsbClassControllerBase(aOwner, aPriority),
 	iIndex(aIndex)
 	{
 	iState = EUsbServiceIdle; // needs explicit initialisation as non-zero
 	}
		
void CUsbDummyClassController::ConstructL()
/**
 * Method to perform second phase construction.
 */
	{
    TInt err = iTimer.CreateLocal();
    if(err < 0)
        {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_CONSTRUCTL, "CUsbDummyClassController::ConstructL; iTimer.CreateLocal() error, Leave error=%d", err );
        User::Leave(err);
        }
	}

CUsbDummyClassController::~CUsbDummyClassController()
/**
 * Destructor.
 */
	{
	Cancel();

	iTimer.Close();
	}
	
void CUsbDummyClassController::GetBehaviour(CIniFile& aIniFile, 
											const TDesC& aSection, 
											TBehaviour& aBehaviour)
/**
 * Reads information from the ini file, from a given section, to the given 
 * behaviour structure.
 *
 * @param aIniFile The ini file to read from.
 * @param aSection The section to reads from.
 * @param aBehaviour The behaviour struct to read to.
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_ENTRY );
#ifdef _DEBUG
	OstTraceExt1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR,
	        "CUsbDummyClassController::GetBehaviour;aSection=%S", aSection );
#endif//_DEBUG

	TPtrC temp;
	if ( !aIniFile.FindVar(aSection, KType(), temp) )
		{
		OstTraceExt1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP1, 
		        "CUsbDummyClassController::GetBehaviour;PANICKING: can't find Type item in section %S", aSection );
		OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP2, "CUsbDummyClassController::GetBehaviour; panic code=%d", EDummyPanicBadIniFile );
		User::Panic(KDummyControllerPanic, EDummyPanicBadIniFile );
		}
	if ( temp == KSync )
		{
		aBehaviour.iSynchronicity = ESynchronous;
		}
	else if ( temp == KAsync )
		{
		aBehaviour.iSynchronicity = EAsynchronous;
		}
	else if ( temp == KNever )
		{
		aBehaviour.iSynchronicity = ENever;
		}
	else
		{
		OstTraceExt2( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP3, 
		        "CUsbDummyClassController::GetBehaviour;PANICKING: bad Type value (%S) in section %S", temp, aSection );
		OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP4, "CUsbDummyClassController::GetBehaviour; panic code=%d", EDummyPanicBadIniFile );
		User::Panic(KDummyControllerPanic, EDummyPanicBadIniFile);
		}
	TInt delay;
	if ( !aIniFile.FindVar(aSection, KTime(), delay) )
		{
		OstTraceExt1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP5, 
		        "CUsbDummyClassController::GetBehaviour;PANICKING: can't find Time item in section %S", aSection );
		OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP6, "CUsbDummyClassController::GetBehaviour;panic code=%d", EDummyPanicBadIniFile );
		User::Panic(KDummyControllerPanic, EDummyPanicBadIniFile);
		}
	aBehaviour.iDelay = delay;
	if ( !aIniFile.FindVar(aSection, KError(), aBehaviour.iErrorCode) )
		{
		OstTraceExt1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP7, 
		        "CUsbDummyClassController::GetBehaviour;aSection=%S", aSection );
		OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_DUP8, "CUsbDummyClassController::GetBehaviour;panic code=%d", EDummyPanicBadIniFile );
		User::Panic(KDummyControllerPanic, EDummyPanicBadIniFile );
		}
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_GETBEHAVIOUR_EXIT );
	}

void CUsbDummyClassController::DoGetConfigL()
/**
 * Reads the config from the ini file.
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_DOGETCONFIGL_ENTRY );

	CIniFile* iniFile = CIniFile::NewL(_L("dummy.ini"));
	CleanupStack::PushL(iniFile);
	TName section;

	// Read startup behaviour
	section.Format(_L("%S.%d"), &KStartup(), iIndex);
	GetBehaviour(*iniFile, section, iStartupBehaviour);
	// Read shutdown behaviour
	section.Format(_L("%S.%d"), &KShutdown(), iIndex);
	GetBehaviour(*iniFile, section, iShutdownBehaviour);

	CleanupStack::PopAndDestroy(iniFile);
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_DOGETCONFIGL_EXIT );
	}

void CUsbDummyClassController::GetConfig()
/**
 * Reads the config from the ini file.
 * Note that this is assumed to succeed. Any failure will break the test and 
 * it's much cleaner to panic out of the test entirely rather than leave it to 
 * the user to figure out what's gone wrong.
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_GETCONFIG_ENTRY );

	// Always use dummy.ini. The entity setting up the test is responsible for 
	// copying the correct file to c:\\dummy.ini. The first found 
	// by FindByDir will be used.
	TRAPD(err, DoGetConfigL());
	if ( err != KErrNone )
		{
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP7, "CUsbDummyClassController::GetConfig;panic code=%d", EDummyPanicUnhandledError );
		User::Panic(KDummyControllerPanic, EDummyPanicUnhandledError);
		}
	
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG, 
	        "CUsbDummyClassController::GetConfig; Logging dummy class controller behaviour for instance %d", iIndex );
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP1, 
	        "CUsbDummyClassController::GetConfig;iStartupBehaviour.iSynchronicity=%d", iStartupBehaviour.iSynchronicity );
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP2, 
	        "CUsbDummyClassController::GetConfig;iStartupBehaviour.iDelay.Int()=%d", iStartupBehaviour.iDelay.Int() );
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP3, 
	        "CUsbDummyClassController::GetConfig;iStartupBehaviour.iErrorCode=%d", iStartupBehaviour.iErrorCode );
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP4, 
	        "CUsbDummyClassController::GetConfig;iShutdownBehaviour.iSynchronicity=%d", iShutdownBehaviour.iSynchronicity );
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP5, 
	        "CUsbDummyClassController::GetConfig;iShutdownBehaviour.iDelay.Int()=%d", iShutdownBehaviour.iDelay.Int() );
	OstTrace1( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_GETCONFIG_DUP6, 
	        "CUsbDummyClassController::GetConfig;iShutdownBehaviour.iErrorCode=%d", iShutdownBehaviour.iErrorCode );
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_GETCONFIG_EXIT );
	}

void CUsbDummyClassController::Start(TRequestStatus& aStatus)
/**
 * Called by UsbMan to start this class.
 *
 * @param aStatus Will be completed with success or failure.
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_START_ENTRY );
	
	//Start() should only be called if the CC is idle or started	
	
	if(!(iState == EUsbServiceIdle || iState == EUsbServiceStarted))
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_START, "CUsbDummyClassController::Start;panic code=%d", EDummyPanicBadApiCallStart );
        __ASSERT_DEBUG(EFalse, 
                User::Panic(KDummyControllerPanic, EDummyPanicBadApiCallStart) );
	    }

	// Get config from ini file. Note that can't be done once in ConstructL 
	// because then, in the case of a CC which doesn't Stop, we'd never be 
	// able to shut down USBMAN!
	GetConfig();

	// NB We enforce that the device doesn't re-post requests on us.
	if(iReportStatus)
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_START_DUP1, "CUsbDummyClassController::Start;panic code=%d", EDummyPanicOutstandingRequestFromDevice );
        __ASSERT_DEBUG(EFalse, 
                User::Panic(KDummyControllerPanic, EDummyPanicOutstandingRequestFromDevice));
	    }
	aStatus = KRequestPending;
	iReportStatus = &aStatus;
	
	iState = EUsbServiceStarting;

	switch ( iStartupBehaviour.iSynchronicity )
		{
	case ESynchronous:
		User::After(iStartupBehaviour.iDelay);
		iState = EUsbServiceStarted;
		User::RequestComplete(iReportStatus, iStartupBehaviour.iErrorCode);
		iReportStatus = NULL;
		break;

	case EAsynchronous:
		iTimer.After(iStatus, iStartupBehaviour.iDelay);
#ifdef _DEBUG
		if(IsActive())
		    {
            OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_START_DUP2, "CUsbDummyClassController::Start;panic code=%d", EDummyPanicAlreadyActive );
            User::Panic(KDummyControllerPanic, EDummyPanicAlreadyActive);
		    }
#endif
		SetActive();
		break;

	case ENever:
		// Don't do anything and never complete
		break;

	default:
	    OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_START_DUP3, "CUsbDummyClassController::Start;panic code=%d", EDummyPanicBadSynchronicity );
	    User::Panic(KDummyControllerPanic, EDummyPanicBadSynchronicity);
		break;
		}
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_START_EXIT );
	}

void CUsbDummyClassController::Stop(TRequestStatus& aStatus)
/**
 * Called by UsbMan to stop this class.
 *
 * @param aStatus Will be completed with success or failure.
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_STOP_ENTRY );
		
	//Stop() should only be called if the CC is Started or Idle
	if(!(iState == EUsbServiceStarted || iState == EUsbServiceIdle))
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_STOP, "CUsbDummyClassController::Stop;panic code=%d", EDummyPanicBadApiCallStop );
        __ASSERT_DEBUG(EFalse, 
                User::Panic(KDummyControllerPanic, EDummyPanicBadApiCallStop));
	    }
	
	// Get config from ini file. Note that can't be done once in ConstructL 
	// because then, in the case of a CC which doesn't Stop, we'd never be 
	// able to shutdown USBMAN!
	GetConfig();

	// NB We enforce that the device doesn't re-post requests on us.
	if(iReportStatus)
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_STOP_DUP1, "CUsbDummyClassController::Stop;panic code=%d", EDummyPanicOutstandingRequestFromDevice );
        __ASSERT_DEBUG(EFalse, 
                User::Panic(KDummyControllerPanic, EDummyPanicOutstandingRequestFromDevice));
	    }
	aStatus = KRequestPending;
	iReportStatus = &aStatus;

	iState = EUsbServiceStopping;

	switch ( iShutdownBehaviour.iSynchronicity )
		{
	case ESynchronous:
		User::After(iShutdownBehaviour.iDelay);
		iState = EUsbServiceIdle;
		User::RequestComplete(iReportStatus, iShutdownBehaviour.iErrorCode);
		iReportStatus = NULL;
		break;

	case EAsynchronous:
		iTimer.After(iStatus, iShutdownBehaviour.iDelay);
#ifdef _DEBUG
    	if(IsActive())
    	    {
            OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_STOP_DUP2, "CUsbDummyClassController::Stop;panic code=%d", EDummyPanicAlreadyActive );
            User::Panic(KDummyControllerPanic, EDummyPanicAlreadyActive);
    	    }
#endif
		SetActive();
		break;

	case ENever:
		// Don't do anything and never complete
		break;

	default:
	    OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_STOP_DUP3, "CUsbDummyClassController::Stop;panic code=%d", EDummyPanicBadSynchronicity );
	    User::Panic(KDummyControllerPanic, EDummyPanicBadSynchronicity);
		break;
		}
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_STOP_EXIT );
	}

void CUsbDummyClassController::GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const
/**
 * Returns information about the interfaces supported by this class.
 *
 * @param aDescriptorInfo Will be filled in with interface information.
 */
	{
	aDescriptorInfo.iNumInterfaces = 0;
	aDescriptorInfo.iLength = 0;
	}

void CUsbDummyClassController::RunL()
/**
 * Standard active object RunL. 
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_RUNL_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CUSBDUMMYCLASSCONTROLLER_RUNL, "CUsbDummyClassController::RunL;iStatus.Int()=%d;iState=%d", iStatus.Int(), iState );


	if ( iStatus != KErrNone )
		{
		// Panic runtime errors from the timer. We can't ignore them, and 
		// there's no point trying to code round them. This is part of the 
		// test framework and if it's failing we want to alert the user 
		// without faffing around. (It invalidates the test.)
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_RUNL_DUP1, "CUsbDummyClassController::RunL;panic code=%d", EDummyPanicUnhandledError );
        User::Panic(KDummyControllerPanic, EDummyPanicUnhandledError);
		}								  

	if(!iReportStatus)
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_RUNL_DUP2, "CUsbDummyClassController::RunL;panic code=%d", EDummyPanicBadState );
        __ASSERT_DEBUG(EFalse, 
                User::Panic(KDummyControllerPanic, EDummyPanicBadState));
	    }

	switch ( iState )
		{
	case EUsbServiceStarting:
		// Completion of asynchronous startup...
		if(iStartupBehaviour.iSynchronicity != EAsynchronous)
		    {
            OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_RUNL_DUP3, "CUsbDummyClassController::RunL;panic code=%d", EDummyPanicBadSynchronicity );
            __ASSERT_DEBUG(EFalse, 
                    User::Panic(KDummyControllerPanic, EDummyPanicBadSynchronicity));
		    }
		iState = EUsbServiceStarted;
		User::RequestComplete(iReportStatus, iStartupBehaviour.iErrorCode);
		iReportStatus = NULL;
		break;

	case EUsbServiceStopping:
		// Completion of asynchronous shutdown...
		if(iShutdownBehaviour.iSynchronicity != EAsynchronous)
		    {
            OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_RUNL_DUP4, "CUsbDummyClassController::RunL;panic code=%d", EDummyPanicBadSynchronicity );
            __ASSERT_DEBUG(EFalse, 
                    User::Panic(KDummyControllerPanic, EDummyPanicBadSynchronicity));
		    }
		iState = EUsbServiceIdle;
		User::RequestComplete(iReportStatus, iShutdownBehaviour.iErrorCode);
		iReportStatus = NULL;
		break;

	case EUsbServiceIdle:
	case EUsbServiceStarted:
	default:
	    OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_RUNL_DUP5, "CUsbDummyClassController::RunL;panic code=%d", EDummyPanicBadState );
	    User::Panic(KDummyControllerPanic, EDummyPanicBadState);
		break;
		}

	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_RUNL_EXIT );
	}

void CUsbDummyClassController::DoCancel()
/**
 * Standard active object cancellation function. 
 */
	{
	OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLER_DOCANCEL_ENTRY );

	// Note that CActive::Cancel does not call DoCancel unless we are active. 
	// Therefore we are at this point active. Therefore, we should have 
	// iReportStatus pointing to something. This is true whether we are in 
	// our destructor or being cancelled by the device. It is also true that 
	// (for us to be active at all) our synchronicity should be set to async 
	// and the timer must have been CreateLocal'd successfully.

	// Cancel may be called in one of the following situations: 
	// 1/ In our destructor. 
	// 2/ The device wants to cancel us in the middle of a Start before 
	// issuing a Stop (or vice versa). Note that the device may cancel us in 
	// the middle of a Start, then immediately issue another Start.
	
	// Cancel our own asynchronous operation.
#ifdef _DEBUG
	if(!iTimer.Handle())
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_DOCANCEL, "CUsbDummyClassController::DoCancel;panic code=%d", EDummyPanicBadState ); 
                User::Panic(KDummyControllerPanic, EDummyPanicBadState);
	    }
#endif
	iTimer.Cancel();

	// Update our iState. If we're starting, then roll back to idle. If we're 
	// stopping, role back to started. Nothing else is legal.
	switch ( iState )
		{
	case EUsbServiceStarting:
		if(iStartupBehaviour.iSynchronicity != EAsynchronous)
		    {
            OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_DOCANCEL_DUP1, "CUsbDummyClassController::DoCancel;panic code=%d", EDummyPanicBadSynchronicity );
            __ASSERT_DEBUG(EFalse, 
                    User::Panic(KDummyControllerPanic, EDummyPanicBadSynchronicity));
		    }
		iState = EUsbServiceIdle;
		break;

	case EUsbServiceStopping:
		if(iShutdownBehaviour.iSynchronicity != EAsynchronous)
		    {
            OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_DOCANCEL_DUP2, "CUsbDummyClassController::DoCancel;panic code=%d", EDummyPanicBadSynchronicity );
            __ASSERT_DEBUG(EFalse, 
                    User::Panic(KDummyControllerPanic, EDummyPanicBadSynchronicity));
		    }
		iState = EUsbServiceStarted;
		break;

	case EUsbServiceIdle:
	case EUsbServiceStarted:
	default:
	    OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_DOCANCEL_DUP3, "CUsbDummyClassController::DoCancel;panic code=%d", EDummyPanicBadState );
	    User::Panic(KDummyControllerPanic, EDummyPanicBadState);
		break;
		}

	// Complete the client's request.
	if(!iReportStatus)
	    {
        OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_DOCANCEL_DUP4, "CUsbDummyClassController::DoCancel;panic code=%d", EDummyPanicBadState );
        __ASSERT_DEBUG(EFalse, 
                User::Panic(KDummyControllerPanic, EDummyPanicBadState));
	    }
	User::RequestComplete(iReportStatus, KErrCancel); 
	iReportStatus = NULL;
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLER_DOCANCEL_EXIT );
	}

TInt CUsbDummyClassController::RunError(TInt /*aError*/)
/**
 * Standard active object error-handling function. 
 *
 * Should return KErrNone to avoid an active scheduler panic. This function
 * should never be called as there is another mechanism for catching errors.
 */
	{
    OstTrace1( TRACE_FATAL, CUSBDUMMYCLASSCONTROLLER_RUNERROR, "CUsbDummyClassController::RunError;panic code=%d", EDummyPanicBadState );
    __ASSERT_DEBUG(EFalse,
            User::Panic(KDummyControllerPanic, EDummyPanicBadState));
	return KErrNone;
	}

//
// End of file
