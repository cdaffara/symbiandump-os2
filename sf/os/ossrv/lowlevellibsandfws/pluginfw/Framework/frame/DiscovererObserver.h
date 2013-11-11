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
// The interface definition for the MDiscovererObserver class
// 
//

#ifndef __DISCOVEREROBSERVER_H__
#define __DISCOVEREROBSERVER_H__

#include <e32base.h>
#include <f32file.h>

class CPluginBase;

/**
@internalComponent
Type for distinguishing between the processing stages of plugins
*/
enum TPluginProcessingTypeIdentifier
	{
	EPluginProcessingTypeNonCriticalOnly, //plugins discovered non-critical stage of start-up
	EPluginProcessingTypeCriticalOnly,	  //plugins discovered at critical stage of start-up
	EPluginProcessingTypeAll,			  //all plugins
	};

/**
	@internalComponent
	@since 7.0
	Responsible for managing the registry (adding and removing 
	entries).
 */

class MDiscovererObserver
	{
public:
/**
	@fn				DiscoveriesBegin()
	Intended Usage	: Pure virtual method used by the CDiscoverer class
					to signal that a discovery session is starting.
	Error Condition	: None.
	@since			7.0
	@return			None.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			Discovery start is noted, and any appropriate action
					for the observer is completed. 
 */
	
	virtual void DiscoveriesBegin() = 0;

/**
	@fn				RegisterDiscoveryL(const TDriveName& aDrive,CPluginBase*& aDirEntry, TBool aAnyDllDiscovered)
	Intended Usage	: Pure virtual method used by the CDiscoverer class
					to register a discovery.
	Error Condition	: Depends upon the implementation class.
	@leave  		Implementation class dependent. @see CRegistrar.
	@since			9.1
	@param			aDrive the drive containing this entry
	@param			aDirEntry A reference to the Discovered directory entry.
	@param 			aAnyDllDiscovered A flag to tell if any Dll is discovered in the drive.
	@return			None.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			Discovery is registered.
 */
	
	virtual void RegisterDiscoveryL(const TDriveName& aDrive,CPluginBase*& aDirEntry, TBool aAnyDllDiscovered) = 0;
	
/**
	@fn				DiscoveriesComplete(TBool aSuccessful,TPluginProcessingTypeIdentifier aProcessingType)
	Intended Usage	: Pure virtual method used by the CDiscoverer class
					to signal that a discovery session is complete.
	Error Condition	: None.
	@since			7.0
	@param			aSuccessful ETrue indicates the scan completed successfully,
					EFalse, that the scan was only partial.
	@param			aProcessingType indicates the type of processing for plugins
					for ensuring that plugins are not processed multiple times
					during start-up phase
	@return			None.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			Discovery completion is noted, and any appropriate action
					for the observer is completed. 
 */
	
	virtual void DiscoveriesComplete(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType) = 0;

/**
	@fn				DriveRemovedL(TDriveUnit aDrive)
	Intended Usage	:Pure virtual method used by the CDiscoverer class
					to notify the observer that a drive letter
					is no longer available for use.
	Error Condition	: Leaves with an error code.
	@leave  		Depends on implementation
	@since			7.0	:
	@param			aDrive	The drive identifier.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			Drive removal is noted, and any appropriate action
					for the observer is completed. 
					E.g. The registered Interface Implementation Collectionss
					stored upon the specified drive are no 
					longer available for use.
 */
	
	virtual void DriveRemovedL(TDriveUnit aDrive) = 0;

/**
	@fn				DriveReinstatedL(TDriveUnit aDrive)
	Intended Usage	:Pure virtual method used by the CDiscoverer class
					to notify the observer that a drive letter
					has become available for use.
	Error Condition	: None.	
	@since			7.0	:
	@param			aDrive	The drive identifier.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			Drive removal is noted, and any appropriate action
					for the observer is completed. 
					E.g. The registered Interface Implementation Collections
					stored upon the specified drive are again 
					made available for use.
 */
	
	virtual void DriveReinstatedL(TDriveUnit aDrive) = 0;

/**
	@fn				NotifiedWithErrorCode(TInt aError)
	Intended Usage	:Pure virtual method used by the CDiscoverer class
					to notify the observer that the notifier has been activated
					with an error status code. The observer should instruct
					the notification object how to proceed.
	Error Condition	: None.	
	@since			7.0	:
	@param			aError	The error value recieved by the notifier.
	@return			ETrue for ignore error, EFalse otherwise. 
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			The status code should be used by the observer to instruct 
					the notifier to cease recieving notifications or not.
 */
	
	virtual TBool NotifiedWithErrorCode(TInt aError) = 0;
	
/**
	@fn 			IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive)
	Intended Usage	:Pure virtual method used by the CDiscoverer class
					to notify the observer to find if any Dll is registered
					in the drive.
	@param 			aDrive The identifier of the drive for any Dll registered info.
	@return 		return if any Dll is registered in the drive.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			If any Dll registered will be returned.				 
*/
	virtual TBool IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive) const = 0;

/**
	@fn 			SetDiscoveryFlagL(const TDriveUnit aDrive)
	Intended Usage	:Pure virtual method used by the CDiscoverer class
					to notify the observer to find the drive has changed.
	@param 			aDrive The identifier of the drive changed.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			The flag is set.				 
*/	
	virtual void SetDiscoveryFlagL(const TDriveUnit &aDrive) = 0;
	
/**
	@fn				LanguageChangedL(TBool& aLanguageChanged)
	Intended Usage	:Pure virtual method used by the CDiscoverer class
					to notify the observer that the language switch notifier has been activated.
					The observer should instruct the notification object how to proceed.
	Error Condition	: Leaves with an error code.
	@leave  		Depends on implementation
	@since			9.3	:
	@param			aLanguageChanged the flag returned to indicate whether language downgrade path is changed.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			The CRegistryData will have its iLanguageChanged 
					flag set to True when language switch happens.
 */	
	virtual void LanguageChangedL(TBool& aLanguageChanged) = 0;	
	 	
	};

#endif // __DISCOVEREROBSERVER_H__
