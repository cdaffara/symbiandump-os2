// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of the CDiscoverer State Accessor class
// 
//

#ifndef __DISCOVERERSATEACCESSOR_H__
#define __DISCOVERERSATEACCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <s32stor.h>
#include <s32file.h>

#include "TestUtilities.h"
#include <test_bed/testbeddefinitions.h>
#include "StateAccessor.h"
#include "Discoverer.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : State accessor for the CDiscoverer object under test.
 */
class TDiscoverer_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	:	
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TDiscoverer_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
	inline virtual TInt InvariantTest(TAny* aTestObject);

	/**
		@fn				Internalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		@since			7.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason 
						for the failure	to load the object state.
		@pre 			TDiscoverer_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			The class under test is set to the state specified
	 */
	inline virtual TInt Internalize(TAny* aTestObject);

	/**
		@fn				Externalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		@since			7.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason 
						for the failure	to externalize the object.
		@pre 			TDiscoverer_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				InternalizeL(CDiscoverer* aDiscoverer)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.	
		@since			7.0
		@param			CDiscoverer* A pointer to the object under test.
		@return			void.
		@pre 			TDiscoverer_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline virtual void InternalizeL(CDiscoverer* aDiscoverer);

	/**
		@fn				ExternalizeL(CDiscoverer* aDiscoverer)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.	
		@since			7.0
		@param			CDiscoverer* A pointer to the object under test.
		@return			None.
		@pre 			TDiscoverer_StateAccessor is fully constructed.
		@post			No change to the class under test.
	*/
	inline virtual void ExternalizeL(CDiscoverer* aDiscoverer);

	// Now CDiscoverer private API accessors
	/**
		@fn				void Notification(TAny* aTestObject)
		Intended Usage	: Caller by the CDirChangeNotifier object when the ECom
						directory has changed.
		Error Condition	: None	
		@since			7.0
		@return			void 
		@pre 			CDiscoverer is fully constructed.
		@post			CDiscoverer has been notified that a re-scan of the 
						ECom directories is required.
	 */
	void Notification(TAny* aTestObject);

	// Methods used by the Incremental Directory Scanning mechanism
	/**
		@fn				void ScanDirectoryL(TAny* aTestObject)
		Intended Usage	: Perform a non-incremental scan of the ECom directory.
		Error Condition	: Not enough memory available.	
		@leave	 		KErrNoMemory
		@since			7.0
		@return			void 
		@pre 			CDiscoverer is fully constructed.
		@post			CDiscoverer a re-scan of the 
						ECom directories is complete.
	 */
	void ScanDirectoryL(TAny* aTestObject);

	/**
		@fn				void ScanDirectoryCancel(TAny* aTestObject)
		Intended Usage	: Cancel any scan of the ECom directory in progress.
		Error Condition	: None	
		@since			7.0
		@return			void 
		@pre 			CDiscoverer is fully constructed.
		@post			CDiscoverer a re-scan of the 
						ECom directories is complete.
	 */
	void ScanDirectoryCancel(TAny* aTestObject);
	
	/**
		@fn				void CompleteNotificationProcessing(TAny* aTestObject)
		Intended Usage	: Signal the processing of a Directory Change notification
						is complete.
		Error Condition	: None	
		@since			7.0
		@return			void 
		@pre 			CDiscoverer is fully constructed.
		@post			CDiscoverer a re-scan of the 
						ECom directories is complete.
	 */
	void CompleteNotificationProcessing(TAny* aTestObject);

	/**
		@fn				ValidateEntryL(TAny* aTestObject, const TEntry& anEntry, const TParse& aPath, TEntry*& anEntryToFill)
		Intended Usage	: Verify that a ECom directory entry is valid
						discovery.
		Error Condition	: None	
		@since			7.0
		@param			aTestObject
		@param			anEntry
		@param			aPath
		@param			anEntryToFill
		@return			TBool ETrue indicates it is valid. 
		@pre 			CDiscoverer is fully constructed.
		@post			.
	 */
	void ValidateEntryL(TAny* aTestObject, const TEntry& anEntry, const TParse& aPath, TEntry*& anEntryToFill);

	/**
		@fn				ProcessEntryL(TAny* aTestObject, const TEntry& anEntry)
		Intended Usage	: Verify that a ECom directory entry is valid
						discovery.
		Error Condition	: Depends on the observer implementation.	
		@leave			Depends on the observer implementation. (@see CRegistrar).
		@since			7.0
		@param			anEntry
		@return			void. 
		@pre 			CDiscoverer is fully constructed.
		@post			The directory entry has been processed by the observer.
	 */
	void ProcessEntryL(TAny* aTestObject, const TEntry& anEntry);

	/**
		@fn				DriveMountedL(TAny* aTestObject, TDriveUnit aDrive)
		Intended Usage	: Signal a drive is available.
		Error Condition	: Depends on the observer implementation.	
		@leave			Depends on the observer implementation. @see CRegistrar.
		@since			7.0
		@param			aDrive	The drive to noted as available.
		@return			void. 
		@pre 			CDiscoverer is fully constructed.
		@post			The drive mount is recorded.
	 */
	void DriveMountedL(TAny* aTestObject, TDriveUnit aDrive);

	/**
		@fn				DriveUnmountedL(TAny* aTestObject, TDriveUnit aDrive)
		Intended Usage	: Signal a drive is unavailable.
		Error Condition	: Leaves with an error code.
		@leave			@see CDiscoverer::DriveUnmountedL
		@since			7.0
		@param			aDrive	The drive to noted as unavailable.
		@return			void. 
		@pre 			CDiscoverer is fully constructed.
		@post			The drive dismount is recorded.
	 */
	void DriveUnmountedL(TAny* aTestObject, TDriveUnit aDrive);

// Access to private members of CRegistrar

	};	// TDiscoverer_StateAccessor

#include "DiscovererStateAccessor.inl"

#endif // __DISCOVERERSATEACCESSOR_H__
