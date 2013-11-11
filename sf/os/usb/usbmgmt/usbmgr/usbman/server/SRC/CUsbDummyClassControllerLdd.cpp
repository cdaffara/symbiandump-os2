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
#include <cusbclasscontrolleriterator.h>
#include <musbclasscontrollernotify.h>
#include "CUsbDummyClassControllerLdd.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbDummyClassControllerLddTraces.h"
#endif



const TInt32 KDummyLDDTestServUid = 0x20031D88;
const TUint KCCErrorTestStepPublishedDeviceEventKey = 0x20031D6F;
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



CUsbDummyClassControllerLdd* CUsbDummyClassControllerLdd::NewL(
		MUsbClassControllerNotify& aOwner,
		TUint aIndex)

/**
 * Constructs a CUsbDummyClassControllerLdd object.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 * @return Ownership of a new CUsbDummyClassControllerLdd object
 */

	{
	CUsbDummyClassControllerLdd* self = new(ELeave) CUsbDummyClassControllerLdd(aOwner, aIndex);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	

CUsbDummyClassControllerLdd::CUsbDummyClassControllerLdd(
		MUsbClassControllerNotify& aOwner,
		TUint aIndex)
/**
 * Constructor.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 */
 :	CUsbClassControllerPlugIn(aOwner, KDummyClassPriority),
	iIndex(aIndex)
	{
	iState = EUsbServiceIdle; // needs explicit initialisation as non-zero
    iPropertyExist = EFalse;
	}

CUsbDummyClassControllerLdd::CUsbDummyClassControllerLdd(
		MUsbClassControllerNotify& aOwner,
		TUint aIndex, TInt aPriority)
		
/**
 * Constructor.
 *
 * @param aOwner USB Device that owns and manages the class
 * @param aIndex The index number of the instance
 * @param aPriority a startup priority for the class controller
 */		
 : 	CUsbClassControllerPlugIn(aOwner, aPriority),
 	iIndex(aIndex)
 	{
 	iState = EUsbServiceIdle; // needs explicit initialisation as non-zero
    iPropertyExist = EFalse;
 	}
		
void CUsbDummyClassControllerLdd::ConstructL()
/**
 * Method to perform second phase construction.
 */
	{
    OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLERLDD_CONSTRUCTL_ENTRY );

	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLERLDD_CONSTRUCTL_EXIT );
	}

CUsbDummyClassControllerLdd::~CUsbDummyClassControllerLdd()
/**
 * Destructor.
 */
	{
    OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLERLDD_CUSBDUMMYCLASSCONTROLLERLDD_ENTRY );
    
	Cancel();
    iPropertyExist = EFalse;
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLERLDD_CUSBDUMMYCLASSCONTROLLERLDD_EXIT );
	}

void CUsbDummyClassControllerLdd::DoGetConfigL()
/**
 * Reads the config from the ini file.
 */
	{
	

	}

void CUsbDummyClassControllerLdd::GetConfig()
/**
 * Reads the config from the ini file.
 * Note that this is assumed to succeed. Any failure will break the test and 
 * it's much cleaner to panic out of the test entirely rather than leave it to 
 * the user to figure out what's gone wrong.
 */
	{
	


	}

void CUsbDummyClassControllerLdd::Start(TRequestStatus& aStatus)
/**
 * Called by UsbMan to start this class.
 *
 * @param aStatus Will be completed with success or failure.
 */
	{
    OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLERLDD_START_ENTRY );
    
    CUsbClassControllerIterator *iterator = NULL;
    TRAPD(ret,iterator = Owner().UccnGetClassControllerIteratorL());
    if(ret == KErrNone)
    	{
    	delete iterator;
    	}
    TRequestStatus* tmp = &aStatus;
	User::RequestComplete(tmp, KErrNone);
	if(!iPropertyExist)
        {
          TInt err = iProperty.Attach(TUid::Uid(KDummyLDDTestServUid),
                            KCCErrorTestStepPublishedDeviceEventKey);
     

        iProperty.Subscribe(iStatus);
        SetActive();
        iPropertyExist = ETrue;
        }
	
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLERLDD_START_EXIT );
	}

void CUsbDummyClassControllerLdd::Stop(TRequestStatus& aStatus)
/**
 * Called by UsbMan to stop this class.
 *
 * @param aStatus Will be completed with success or failure.
 */
	{
    OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLERLDD_STOP_ENTRY );
    
    TRequestStatus* tmp = &aStatus;
	User::RequestComplete(tmp, KErrNone);
			
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLERLDD_STOP_EXIT );
	}

void CUsbDummyClassControllerLdd::GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const
/**
 * Returns information about the interfaces supported by this class.
 *
 * @param aDescriptorInfo Will be filled in with interface information.
 */
	{
	aDescriptorInfo.iNumInterfaces = 0;
	aDescriptorInfo.iLength = 0;
	}

void CUsbDummyClassControllerLdd::RunL()
/**
 * Standard active object RunL. 
 */
	{
         OstTraceFunctionEntry0( CUSBDUMMYCLASSCONTROLLERLDD_RUNL_ENTRY );
    TInt num;
    iProperty.Get(num);
    if(num < 0)
        User::Leave(num);
    iProperty.Subscribe(iStatus);
    SetActive();
	OstTraceFunctionExit0( CUSBDUMMYCLASSCONTROLLERLDD_RUNL_EXIT );
	}

void CUsbDummyClassControllerLdd::DoCancel()
/**
 * Standard active object cancellation function. 
 */
	{
    iProperty.Cancel();
	}

TInt CUsbDummyClassControllerLdd::RunError(TInt aError)
/**
 * Standard active object error-handling function. 
 *
 * Should return KErrNone to avoid an active scheduler panic. This function
 * should never be called as there is another mechanism for catching errors.
 */
	{
    Owner().UccnError(KErrNone);
    return KErrNone;
	}

//
// End of file
