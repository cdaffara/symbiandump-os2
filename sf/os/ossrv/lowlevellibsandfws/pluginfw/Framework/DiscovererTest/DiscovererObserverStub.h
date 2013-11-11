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
// Implementation of the stub class for the Discoverer tests
// 
//

#ifndef __DISCOVEREROBSERVERSTUB_H__
#define __DISCOVEREROBSERVERSTUB_H__

#include "DiscovererObserver.h"
#include "DiscovererStateAccessor.h"
#include "DiscovererTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	@since 7.0
	This class is intended to provide the stub functionality of the
	CDiscoverers observer class. The methods will be used to log the
	calls that CDiscoverer is making to its observer.
	Note that the calls do not return any error codes, and so there
	is no requirement to repeat a transition, exercising the various
	control paths of the stub caller.
	RegisterDiscoveryL, and DriveReinstatedL can leave however, 
	and there will be a repeat call on the transition that call
	these stub methods to exercise the alternative
	control paths of the caller.
	In the actual ECom implementation MDiscovererObserver is inherited
	privately but this causes problems with casting the stub so it is 
	public here.
 */
class CObserverStub : public CBase, public MDiscovererObserver
{
public:
	/**
		@fn				CObserverStub* NewL(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext)
		Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
		Error Condition	: Leaves with the error code.	
		@leave			KErrNoMemory.	
		@since			7.0
		@param			CUnitTest& aOwner" "
		@return			CObserverStub*
		@pre 			None
		@post			CObserverStub is fully constructed and initialised.
	 */
	static CObserverStub* NewL(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext);

	/**
		@fn				CObserverStub* NewLC(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext)
		Intended Usage	: Standardised safe construction which leaves CObserverStub* upon the cleanup stack.
		Error Condition	: Leaves with the error code.	
		@leave 			KErrNoMemory.	
		@since			7.0
		@param			CUnitTest& aOwner" "
		@return			CObserverStub*
		@pre 			None
		@post			CObserverStub is fully constructed and initialised, and remains on the cleanup stack.
	 */
	static CObserverStub* NewLC(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext);

	/**
		@fn				 CObserverStub::~CObserverStub()
		Intended Usage	: Default Destructor	
		@since			7.0
		@pre 			CObserverStub is fully constructed
		@post			CObserverStub is fully destroyed
	 */
	virtual ~CObserverStub();

	// Observer methods requiring overload
	/**
		@fn				DiscoveriesBegin()
		Intended Usage	: Overload of the MDiscovererObserver callback method.
						to signal that a discovery session is starting.
		Error Condition	: None.
		@since			7.0
		@return			None.
		@pre 			The MDiscovererObserver must be fully constructed, and
						capable of accepting discoveries for registration.
		@post			Discovery start is noted, and any appropriate action
						for the observer is completed. 
	 */
	virtual void DiscoveriesBegin();

	/**
		@fn				RegisterDiscoveryL(const TEntry& aDirEntry)
		Intended Usage	: Overload of the MDiscovererObserver callback method.
						to register a discovery.
		Error Condition	: Depends upon the implementation class.
		@leave			Implementation class dependent. @see CRegistrar.
		@since			7.0
		@param			aDirEntry A reference to the Discovered directory entry.
		@return			None.
		@pre 			The MDiscovererObserver must be fully constructed, and
						capable of accepting discoveries for registration.
		@post			Discovery is registered.
	 */
	virtual void RegisterDiscoveryL(const TEntry& aDirEntry);

	/**
		@fn				DiscoveriesComplete(TBool aSuccessful)
		Intended Usage	: Overload of the MDiscovererObserver callback method.
						to signal that a discovery session is complete.
		Error Condition	: None.
		@since			7.0
		@param			aSuccessful ETrue indicates the scan completed successfully,
						EFalse, that the scan was only partial.
		@return			None.
		@pre 			The MDiscovererObserver must be fully constructed, and
						capable of accepting discoveries for registration.
		@post			Discovery completion is noted, and any appropriate action
						for the observer is completed. 
	 */
	virtual void DiscoveriesComplete(TBool aSuccessful);

	/**
		@fn				DriveRemovedL(TDriveUnit aDrive)
		Intended Usage	:Overload of the MDiscovererObserver callback method.
						to notify the observer that a drive letter
						is no longer available for use.
		Error Condition	: None.	
		@since			7.0	:
		@param			aDrive	The drive identifier.
		@pre 			The MDiscovererObserver must be fully constructed, and
						capable of accepting discoveries for registration.
		@post			Drive removal is noted, and any appropriate action
						for the observer is completed. 
						E.g. The registered implementation groupings
						stored upon the specified drive are no 
						longer available for use.
	 */
	virtual void DriveRemovedL(TDriveUnit aDrive);

	/**
		@fn				DriveReinstatedL(TDriveUnit aDrive)
		Intended Usage	:Overload of the MDiscovererObserver callback method.
						to notify the observer that a drive letter
						has become available for use.
		Error Condition	: None.	
		@since			7.0	:
		@param			aDrive	The drive identifier.
		@pre 			The MDiscovererObserver must be fully constructed, and
						capable of accepting discoveries for registration.
		@post			Drive removal is noted, and any appropriate action
						for the observer is completed. 
						E.g. The registered implementation groupings
						stored upon the specified drive are again 
						made available for use.
	 */
	virtual void DriveReinstatedL(TDriveUnit aDrive);

	/**
		@fn				NotifiedWithErrorCode(TInt aError)
		Intended Usage	:Overload of the MDiscovererObserver callback method.
						The notification that the notifier has been activated
						with an error status code. The observer should instruct
						the notification object how to proceed.
		Error Condition	: None.	
		@since			7.0	:
		@param			aError	The error value recieved by the notifier.
		@return			ETrue for ignore error, EFalse otherwise. 
		@pre 			CRegistrar is fully constructed.
		@post			The status code should be used by the observer to instruct 
						the notifier to cease recieving notifications or not.
	 */
	virtual TBool NotifiedWithErrorCode(TInt aError);

private:
	/**
		@fn				 CObserverStub::CObserverStub(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext)
		Intended Usage	: Default Constructor : usable only by derived classes	
		@since			7.0
		@param			CUnitTest& A reference to the owning CUnitTest
		@return			None
		@pre 			None
		@post			CObserverStub is fully constructed
	 */
	CObserverStub(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext);

	/**
		@fn				void CObserverStub::ConstructL()
		Intended Usage	: Completes the safe construction of the CObserverStub object
		Error Condition	: Leaves with the error code.	
		@leave 			KErrNoMemory.	
		@since			7.0
		@return			None
		@pre 			CObserverStub is fully constructed
		@post			CObserverStub is fully initialised
	 */
	void ConstructL();

// Attributes / Properties
private:
	/** A reference to the owning CUnitTest */
	CUnitTest& iOwner;
	/** A reference to the Unit Test context object */
	CDiscoverer_UnitTestContext& iContext;
};  // End of CObserverStub definition


#endif	// __DISCOVEREROBSERVERSTUB_H__
