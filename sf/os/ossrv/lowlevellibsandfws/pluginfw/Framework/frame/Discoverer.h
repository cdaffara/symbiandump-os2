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
// Definition of the CDiscoverer class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __DISCOVERER_H__
#define __DISCOVERER_H__

#include <e32base.h>
#include <f32file.h>
#include <e32property.h> // for RProperty
#include <domaindefs.h> // for TDmDomainState

#include "DiscovererObserver.h"	// The MDiscovererObserver class
#include "EComEntry.h"
#include "EComEntryBase.h"
#include "callback.h"

// Platsec path to binaries
_LIT(KSysBin,"\\sys\\bin\\");
// Platsec path to resources
_LIT(KResourcePlugins,"\\resource\\plugins\\");
_LIT(KDllExtension,".dll");

_LIT(KRscExtension,".rsc");

// Length of extension of file name.
//The language extension length including the "." is assumed to be 4 
const TInt KExtensionLength=4;

//Max lenth for plugin resource path
const TInt32 KEComPlugRSCPathMaxLen	=	50;

//class forward declare
class CEnvironmentChangeNotifier;
/**
	@internalComponent
	@since 7.0
	Responsible for Interface Implementation Collections,
	installed in the Interface Implementation Collection directories.
 */

class CDiscoverer : public CBase 
{
public:

/** Transition flag for Notification*/
	enum TNotificationFlag
	{
	/** A new plugin added or removed*/
	EPluginsModified,
	
	/**should do a rediscovery */
	EPluginsRediscover
	};

/** State declaration for CDiscoverer */	

	enum TDiscovererState
	{
	/** Undefined state */
	EDisc_Undefined,
	
	/** Initialize state */
	EDisc_NoPluginsDisc,
	
	/** Discover plugins on Ro Internal drive units */
	EDisc_CriticalPluginsDisc,
	
	/** All plugins are discovered.*/
	EDisc_AllPluginsDisc,
	
	/** Plugins added or removed, going to rediscover all plugins */
	EDisc_PluginsDirty
	
	};

/**
	@fn				NewL(MDiscovererObserver& aDiscovererObserver, RFs& aFs)
	Intended Usage	: Standardised safe construction which leaves nothing
					  on the cleanup stack.
	Error Condition	: Not enough memory available.	
	@leave  		KErrNoMemory
	@since			7.0
	@param			aDiscovererObserver A reference to the owning client 
					of the CDiscoverer
	@param			aFs A reference to a connected file server instance.
	@return			CDiscoverer* a pointer to the new class
	@pre 			None
	@post			CDiscoverer is fully constructed, and initialised.
 */
	
	static CDiscoverer* NewL(MDiscovererObserver& aDiscovererObserver, RFs& aFs);

/**
	@fn				~CDiscoverer()
	Intended Usage	: Standard default d'tor	
	Error Condition	: None	
	@since			7.0
	@pre 			CDiscoverer is fully constructed.
	@post			CDiscoverer is totally destroyed.
 */
	
	~CDiscoverer();

/**
	@fn				Resume()
	Intended Usage	: Re-enable the Discoverer to continue checking for new 
					  interface implementations. 
	Error Condition	: Where resume of the discoverer fails. KErrResumeFailed.
	@since			7.0
	@return			TInt The status of the operation. KErrNone or KErrResumeFailed.
	@pre 			CDiscoverer is fully constructed.
	@post			The operation of the discoverer has been resumed. Or where
					resumption failed, no change.
 */
	
	TInt Resume();

/**
	@fn				Suspend()
	Intended Usage	: Temporarily stop looking for new interface implementations.
	Error Condition	: Where suspension of the discoverer fails. KErrSuspendFailed.
	@since			7.0
	@return			TInt The status of the operation. KErrNone or KErrSuspendFailed.
	@pre 			CDiscoverer is fully constructed.
	@post			The operation of the discoverer has been suspended. Or where
					suspend failed, no change.
 */
	
	TInt Suspend();
	
	
/**
	@fn 			ProcessSSAEventL()
	Intended Usage	: It discovers the plugins on different Drive Units according to 
					the passing state. It will be used by CRegistrar to notify the 
					object of the system start-up state changes.
	@param 			aKnownState the start-up state passing in to CDiscoverer
	@pre			CDiscoverer is fully constructed
	@post			CDiscoverer is notified of the start-up state change
*/
	void ProcessSSAEventL(TStartupStateIdentifier aKnownState);
	
/** 
	@fn 			ProcessDNEventL()
	Intended Usage	:It will be used by one iRscDirNotifier in iRscDirNotifierList 
					to indicate that plugins have been removed or added(state is 
					EDisc_PluginsDirty) and to start a 
					re-discovery(when done, state is EDisc_AllPluginsDisc).
	@leave			KErrNoMemory
	@param			:aFlag the notification transition flag.
	@param			:aDriveUnit a drive which on the plugins has been changed and need to re-discovery
	@pre 			:CDiscoverer is fully constructed.
	@post			: state will be changed, and a re-discovery will happen 
					if the current state is EDisc_PluginsDirty. 
*/
	void ProcessDNEventL(TNotificationFlag aFlag, const TDriveUnit& aDriveUnit);

/**
	@fn 			State()
	Intended Usage	:Retrieve the object's current state.
	@return 		TDiscovererState the CDiscoverer's current state.
	@pre			CDiscoverer is fully constructed.
	@post 			the CDiscoverer's current state has been returned.
*/
	TDiscovererState State() const;
	
/** Set the iSwiChangeCallBack member
@param aCallBack new value for iSwiChangeCallBack
*/
	void SetSwiChangeCallBack(const TCallBackWithArg& aCallBack);

/** Set the iBurChangeCallBack member
@param aCallBack new value for iBurChangeCallBack
*/
	void SetBurChangeCallBack(const TCallBackWithArg& aCallBack);

private:
/**
	@internalComponent
	@fn				CDiscoverer(MDiscovererObserver& aDiscovererObserver, RFs& aFs)
	Intended Usage	: Standardised default c'tor	
	Error Condition	: None	
	@since			7.0
	@param			aDiscovererObserver A reference to the owning client 
					of the CDiscoverer
	@param			aFs A reference to an connected file server instance.
	@pre 			None
	@post			CDiscoverer is fully constructed
 */
	
	CDiscoverer(MDiscovererObserver& aDiscovererObserver, RFs& aFs);

/**
	@fn				ConstructL()
	Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
	Error Condition	: Leaves with error code : usually KErrNoMemory.
	@leave  		KErrNoMemory.
	@since			7.0
	@return			void 
	@pre 			CDiscoverer is fully constructed.
	@post			CDiscoverer is fully initialised.
 */
	
	void ConstructL();

/**
	@fn				NotificationL(TInt aStatus)
	Intended Usage	: Called by the one object of iRscDirNotifierList when the 
					contents of the Interface Implementation directories
					has changed on a specified drive.
	Error Condition	: None	
	@leave			KErrNoMemory
	@since			7.0
	@param			aDriveUnit a drive which has changes.
	@return			ETrue to indicate the notifier should re-activate.
	@pre 			CDiscoverer is fully constructed.
	@post			CDiscoverer has been notified that a re-scan of the 
					plugin directories is required.
 */
	
	TBool NotificationL(TInt aStatus, const TDriveUnit& aDriveUnit);

/**
	@fn				SwiChangeNotificationL(TInt aSwiOperation)
	Intended Usage	: Called by iSwiChangeNotifier when it is notified of a
					SWI changed.
	Error Condition	: None	
	@since			9.2
	@param			aSwiOperation the SWI operation that is currently taking place
	@pre 			CDiscoverer is fully constructed.
	@post			CDiscoverer has been notified that a re-scan of the
					removable plugin directories is required and has stored current
					SWI operation.
 */
	
	void SwiChangeNotificationL(TInt aSwiOperation);

/**
	@fn				LanguageChangeNotificationL()
	Intended Usage	: Called by iLanguageChangeNotifier when it is notified of language
					downgrade path changed.
	Error Condition	: None	
	@since			9.3
	@pre 			CDiscoverer is fully constructed.
	@post			CDiscoverer has been notified that a rescan may be required
 */
 	
	void LanguageChangeNotificationL();

	// Methods used by the Incremental Directory Scanning mechanism

/**
	@fn				void RediscoveryScanDirectoryL()
	Intended Usage	: Perform a scan on the Interface Implementation 
					Collection directories on a specified drive.
	Error Condition	: Not enough memory available	
	@leave  		KErrNoMemory
	@since			7.0
	@param			:A drive on which the directory is scanned.
	@return			void
	@pre 			CDiscoverer is fully constructed.
	@post			The scan of the Interface Implementation Collection 
					directories has processed on a specified drive.
 */
	
	void RediscoveryScanDirectoryL(const TDriveUnit& aDriveUnit);

/**
	@fn				void ScanDirectoryCancel()
	Intended Usage	: Cancel any scan of the Interface Implementation Collection 
					directories in progress.
	Error Condition	: None	
	@since			7.0
	@return			void 
	@pre 			CDiscoverer is fully constructed.
	@post			The re-scan of the Interface Implementation 
					Collection directories has beed aborted.
 */
	
	void ScanDirectoryCancel();
	
/**
	@fn				CompleteNotificationProcessing()
	Intended Usage	: Signal that the directory change notification has
					 been fully processed on all drives.
	Error Condition	: None	
	@since			7.0
	@pre 			CDiscoverer is fully constructed.
	@post			An asynchronous re-scan of the Interface Implementation 
					Collection directories is complete.
 */
	
	void CompleteNotificationProcessing();

/**
	@fn				ValidateEntryL(const TEntry& aEntry, const TDriveName& aDriveName, CPluginBase*& aEntryToFill,TBool aIsRO)
	Intended Usage	: Verify that a Interface Implementation Collection directories entry is valid
					discovery.
	Error Condition	: No memory
	@leave  		KErrNoMemory
	@since			9.1
	@param			aEntry	A directory entry for an Interface Implementation Collection.
	@param			aDriveName	The drive containing this entry
	@param			aEntryToFill A newly created directory entry with the 
					filename updated to include the full path.
	@param			aIsRO	Indicates whether current drive is Read Only
	@pre 			CDiscoverer is fully constructed, 
					aEntry, aPath and aEntryToFill are all valid references.
	@post			aEntryToFill is created and valid, with the full path and filename as its name entry.
 */
	
	void ValidateEntryL(const TEntry& aEntry, const TDriveName& aDriveName, CPluginBase*& aEntryToFill, TBool aIsRO);

/**
	@fn				ValidateEntryL(RResourceArchive& aRscArchive,CPluginBase*& aEntryToFill);
	Intended Usage	: Verify that a Interface Implementation Collection directories entry is valid
					discovery.
	Error Condition	: No memory
	@leave  		KErrNoMemory
	@since			9.1
	@param			aRscArchive A referernce to the resource archive
	@param			aEntryToFill A newly created directory entry with the 
					filename updated to include the full path.
	@pre 			CDiscoverer is fully constructed, 
	@post			aEntryToFill is created and valid, with the full path and filename as its name entry.
 */
	
	void ValidateEntryL(RResourceArchive& aRscArchive, CPluginBase*& aEntryToFill);
/**
	@fn				ProcessEntryL(const TDriveName& aDrive,CPluginBase*& aEntry, TBool aAnyDllDiscovered)
	Intended Usage	: Verify that a Interface Implementation Collection directories entry is valid
					discovery.
	Error Condition	: Depends on the observer implementation.	
	@leave  		Depends on the observer implementation. (@see CRegistrar).
	@since			9.1
	@param			aDrive the drive name containing this entry
	@param			aEntry	A directory entry for an Interface Implementation Collection.
	@param			aAnyDllDiscovered a flag to tell if any Dll is discovered in the drive.
	@pre 			CDiscoverer is fully constructed.
	@post			The directory entry has been processed by the observer.
 */
	
	void ProcessEntryL(const TDriveName& aDrive, CPluginBase*& aEntry, TBool aAnyDllDiscovered);

/**
	@fn				DriveMountedL(TDriveUnit aDrive)
	Intended Usage	: Signal a drive is available.
	Error Condition	: Depends on the observer implementation.	
	@leave  		Depends on the observer implementation. @see CRegistrar.
	@since			7.0
	@param			aDrive	The drive to be noted as available.
	@return			void. 
	@pre 			CDiscoverer is fully constructed.
	@post			The drive mount has been signalled to the observer.
 */
	
	void DriveMountedL(TDriveUnit aDrive);

/**
	@fn				DriveUnmountedL(TDriveUnit aDrive)
	Intended Usage	: Signal a drive is unavailable.
	Error Condition	: Leaves with an error code
	@leave			KErrNoMemory
	@since			7.0
	@param			aDrive	The drive to be noted as unavailable.
	@pre 			CDiscoverer is fully constructed.
	@post			The drive dismount has been signalled to the observer.
 */
	
	void DriveUnmountedL(TDriveUnit aDrive);
	
	
	
/**
	@fn 			StartNotifiers()
	Intended Usage	: start the iRscDirNotifierList and iSwiChangeNotifier when EDisc_AllPlugins
					state is reached.
	@pre 			: CDiscoverer is fully constructed. and iRscDirNotifierList and iSwiChangeNotifier
					are fully constructed.
	@post			: iRscDirNotifierList Activate and iSwiChangeNotifier Subscribe methods are called.
*/
	void StartNotifiers();
	
	
/** 
	@fn 			InitialiseEvent()
	Intended Usage	: called during the CDiscoverer's construction 
					to change the state.
	@pre			: non
	@post			: State is changed from EDisc_Undefined to EDisc_NoPluginsDisc.
*/		
	void InitialiseEvent();
	
/**
	@fn				IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive)
	Intended Usage	: Find if any Dll is registered in the drive.
	@param 			: aDrive the identifier of the drive to find if any Dll is registered.
	@return			: ETrue if any Dll is registered in the drive unit, otherwise EFalse.
	@pre 			: CDiscoverer is fully constructed.
	@post 			: If any Dll registered info will be returned.
*/	
	TBool IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive)const ;	

/**
	@fn				SwiOperationInProgress()
	Intended Usage	: Determine if any SWI operation is currently in progress
	@return			: ETrue if any SWI operation is in progress otherwise EFalse.
	@pre 			: CDiscoverer is fully constructed.
	@post 			: None
*/	
	TBool SwiOperationInProgress() ;

/**
	@fn				LocaleChangedL(TAny*)
	Intended Usage	: Callback function to be attached to an instance of CEnvironmentChangeNotifier to 
					  rebuild registry data. Note that this has to be a static member function.
	@param 			: A parameter to be passed usually with a "this" pointer so that while the function
					  remaining a static function it can access all the member data anyway.
	@return			: ETrue if language downgrade path is changed, otherwise EFalse.
	@pre 			: CDiscoverer is fully constructed.
	@post 			: Call LanguageChangeNotificationL method to rescan resource files.
*/		
	static TInt LocaleChangedL(TAny*) ;

// Attributes
private:

	// Define the helper classes which handle the 
	// notification processing.
	class CDirChangeNotifier;
	
	
	typedef RPointerArray<CDirChangeNotifier> RDirChangeNotifierPointerArray;
	// Handles notifications of changes to plugin resources directory on list of drives.
	RDirChangeNotifierPointerArray iRscDirNotifierList;

	class CSwiChangeNotifier;

	// Handles notifications of changes to SWI
	CSwiChangeNotifier* iSwiChangeNotifier;

	// Indicates whether a discovery due to SWI changes is pending
	TBool iSwiChangeDiscoveryPending;

	//Handle notifications of changes to environment (including language switch)
	CEnvironmentChangeNotifier* iLanguageChangeNotifier	;

	// Stores the current SWI operation
	TInt iSwiOperation;

	// Indicates whether a discovery due to language changes is pending
	TBool iLanguageChangeDiscoveryPending;
	
	// the current state of CDiscoverer
	TDiscovererState iState;
	
	class CIdleScanningTimer;

	// A single timer, processes all pending notifications.
	CIdleScanningTimer* iScanningTimer;
	
	class CDirScanner;

	/** 
		The internal Interface Implementation Collection 
		identifier and registration handler  
	 */
	
	CDirScanner* iDirScanner;

	/** A reference to the owning client of the CDiscoverer */
	
	MDiscovererObserver& iDiscovererObserver;

	/** A reference to the connected file server instance */
	
	RFs& iFs;

	/** The list of existing drives */
	
	RArray<TUint> iDrivesDiscovered;

	/** cached info of drives A to Z */
	CEComCachedDriveInfo* iCachedDriveInfo;
	
	TCallBackWithArg iSwiChangeCallBack;
	TCallBackWithArg iBurChangeCallBack;

	// List the internal classes as friends
	friend class CSwiChangeNotifier;
	friend class CDirChangeNotifier;
	friend class CIdleScanningTimer;
	friend class CDirScanner;
	// And the Test bed state accessor
	friend class TDiscoverer_StateAccessor;


};
// __________________________________________________________________________
//
/**
The notification object which uses P&S to monitor SWI KSAUidSoftwareInstallKey 
for changes. When SWI is present in the device/ROM this attribute publishes the
current state of a software install or uninstall.
When its RunL method is called and depending on the install/uninstall state it 
notifies its owning CDiscoverer class object to re-scan removable drives.

@internalComponent
*/
class CDiscoverer::CSwiChangeNotifier : public CActive
	{
public:

	/**
		@fn				NewL(CDiscoverer& aDiscoverer)
		Intended Usage	: Standardised safe construction which leaves nothing
						  on the cleanup stack.
		Error Condition	: Not enough memory available.	
		@leave  			KErrNoMemory
		@since			9.2
		@param			aDiscoverer A reference to the owning CDiscoverer
		@return			CSwiChangeNotifier* a pointer to the new class
		@pre 			None
		@post			CSwiChangeNotifier is fully constructed, and initialised.
	 */	
	static CSwiChangeNotifier* NewL(CDiscoverer& aDiscoverer);

	/**
		@fn				virtual ~CSwiChangeNotifier()
		Intended Usage	: Standard default d'tor	
		Error Condition	: None	
		@since			9.2
		@pre 			CSwiChangeNotifier is fully constructed.
		@post			CSwiChangeNotifier is totally destroyed
	 */
	virtual ~CSwiChangeNotifier();
	
	/**
		@fn				Subscribe()
		Intended Usage	: Subscribes with P&S for change notifications for the 
                          SWI KSAUidSoftwareInstallKey property
		Error Condition	: None	
		@since			9.2
		@pre 			CSwiChangeNotifier is fully constructed.
		@post			CSwiChangeNotifier is active on the current scheduler.
	 */
	void Subscribe();

private:
	/**
		@fn				CSwiChangeNotifier(CDiscoverer& aDiscoverer)
		Intended Usage	: Standardised default c'tor	
		Error Condition	: None	
		@since			9.2
		@param			aDiscoverer A reference to its owning class instance.
		@pre 			None
		@post			CSwiChangeNotifier is fully constructed
	 */
	CSwiChangeNotifier(CDiscoverer& aDiscoverer);

	/**
		@fn				RunL()
		Intended Usage	: When the object activates, this is method is called
						and notifies the CDiscoverer to re-scan the 
						removable drives.. 
		Error Condition	: Not enough memory available to complete the scan.
		@leave  			KErrNoMemory
		@since			9.2
		@pre 			CSwiChangeNotifier is fully constructed.
		@post			CSwiChangeNotifier notifications are subscribed and plugins on
						removable drives have been rediscovered..
	 */
	void RunL();

	/**
		@fn				void ConstructL()
		Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
		Error Condition	: Leaves with error code : usually KErrNoMemory.
		@leave  			KErrNoMemory.
		@since			9.2
		@return			void 
		@pre 			CSwiChangeNotifier is fully constructed.
		@post			CSwiChangeNotifier is fully initialised.
	 */
	void ConstructL();

	/**
		@fn				DoCancel()
		Intended Usage	: The cancel action called by CActive::Cancel(). 
		Error Condition	: None	
		@since			9.2
		@pre 			CSwiChangeNotifier is fully constructed.
		@post			CSwiChangeNotifier notifications are cancelled and
						it is no longer active on the current scheduler.
	 */
	void DoCancel();

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Called by the notifier RunL leaving.
		@since			9.2
		@param			aError The error code that the RunL left with.
		@return			TInt KErrNone. 
		@pre 			CSwiChangeNotifier is fully constructed.
		@post			CSwiChangeNotifier is returned to a
						sensible active state.
	 */
	TInt RunError(TInt aError);

private:
	/** A reference to its owning class instance */
	CDiscoverer& iDiscoverer;	
	/** Handle allowing access to P&S properties */
	RProperty iProperty;
	/** Friend class to enable access to private members for testing. */
	friend class TDiscoverer_StateAccessor;
	};

// __________________________________________________________________________
//
/**
	@internalComponent
	@since 7.0
	The notification object which watches the Interface Implementation Collection directories for
	any changes.
	When its RunL method is called, it notifies its owning CDiscoverer class
	object to re-scan the Interface Implementation Collection directories.
 */
class CDiscoverer::CDirChangeNotifier : public CActive
	{
public:
	/**
		@fn				CDirChangeNotifier(CDiscoverer& aDiscoverer, RFs& aFs, const TDesC& aNotificationFilePath)
		Intended Usage	: Standardised default c'tor	
		Error Condition	: None	
		@since			7.0
		@param			aDiscoverer A reference to its owning class instance.
		@param			aFs A reference to a connected file server instance.
		@param			aNotificationFilePath A reference to the file path that is to be watched.
		@param			aDriveUnit A drive which the notification object watches for.
		@pre 			None
		@post			CDirChangeNotifier is fully constructed
	 */
	CDirChangeNotifier(CDiscoverer& aDiscoverer, RFs& aFs,const TDriveUnit& aDriveUnit);
	/**
		@fn				virtual ~CDirChangeNotifier()
		Intended Usage	: Standard default d'tor	
		Error Condition	: None	
		@since			7.0
		@pre 			CDirChangeNotifier is fully constructed.
		@post			CDirChangeNotifier is totally destroyed
	 */
	virtual ~CDirChangeNotifier();
	/**
		@fn				Activate()
		Intended Usage	: Activate the notifier and attach it to the File 
						session to watch the appropriate Interface Implementation Collection directories. 
		Error Condition	: None	
		@since			7.0
		@pre 			CDirChangeNotifier is fully constructed.
		@post			CDirChangeNotifier is active on the current scheduler.
	 */
	void Activate();

private:
	/**
		@fn				RunL()
		Intended Usage	: When the object activates, this is method is called
						and notifies the CDiscoverer to re-scan the 
						Interface Implementation Collection	directories. 
		Error Condition	: Not enough memory available to complete the scan.
		@leave  		KErrNoMemory
		@since			7.0
		@pre 			CDirChangeNotifier is fully constructed.
		@post			CDirChangeNotifier notifications are cancelled and
						it is no longer active on the current scheduler.
	 */
	void RunL();
	/**
		@fn				DoCancel()
		Intended Usage	: The cancel action called by CActive::Cancel(). 
		Error Condition	: None	
		@since			7.0
		@pre 			CDirChangeNotifier is fully constructed.
		@post			CDirChangeNotifier notifications are cancelled and
						it is no longer active on the current scheduler.
	 */
	void DoCancel();

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Called by the notifier RunL leaving.
		@since			7.0
		@param			aError The error code that the RunL left with.
		@return			TInt KErrNone. 
		@pre 			CDirChangeNotifier is fully constructed.
		@post			CDirChangeNotifier is returned to a
						sensible active state.
	 */
	TInt RunError(TInt aError);

private:
	/** A reference to its owning class instance */
	CDiscoverer& iDiscoverer;	
	/** A reference to a connected file server instance */
	RFs&	iFs;
	/** The file path for a specific drivethat we are interested in.*/
	//const TDesC& iNotificationFilePath;
	typedef TBuf<KEComPlugRSCPathMaxLen> TResourcePathName;
	TResourcePathName iNotificationFilePath;

	/** A drive which the notification object watches for.*/
	TDriveUnit iDriveUnit;
	/** Friend class to enable access to private members for testing. */
	friend class TDiscoverer_StateAccessor;
	};

/**
	@since 7.0	
	The timer Active object for providing incremental plugin directory scanning. 
	Note that task execution is dependant upon the task priority
	The default priority is idle time execution only.
	This object controls the yielding of time to other processes by having a low priority
	but requesting immediate reactivation, after each incremental part of the directory scan.
	It is launched by the CDirChangeNotifier's notification call.
 */
class CDiscoverer::CIdleScanningTimer : public CTimer
	{
public:
	/**
		@fn				NewL(CDiscoverer& aDiscoverer)
		Intended Usage	: Standardised safe construction which leaves nothing
						  on the cleanup stack.
		Error Condition	: Not enough memory available.	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDiscoverer A reference to the owning CDiscoverer
		@return			CIdleScanningTimer* a pointer to the new class
		@pre 			None
		@post			CIdleScanningTimer is fully constructed, and initialised.
	 */	
	static CIdleScanningTimer* NewL(CDiscoverer& aDiscoverer);
	/**
		@fn				virtual ~CIdleScanningTimer()
		Intended Usage	: Standard default d'tor	
		Error Condition	: None	
		@since			7.0
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is totally destroyed
	 */
	virtual ~CIdleScanningTimer();
	
	/** 
		@fn				AddDriveL(TDriveUnit aDrive)
		Intended Usage	: Add a drive for processing notification
		Error Condition	: None	
		@leave  		System wide error codes.
		@param			aDrive The drive to be added.
		@post			The drive number added into pending list only if it has not been added before
	 */
	inline void AddDriveL(const TDriveUnit aDrive)
		{
		if(iPendingDriveList.Find(aDrive) == KErrNotFound)
			{
			iPendingDriveList.AppendL(aDrive);
			}
		}
	
	/**
		@fn				IsAnyNotificationProcessingPending()
		Intended Usage	: To check if there is any notification waiting to be processed.
		@return			TBool A flag indicating if there is any pending notification to be processed
	 */	
	inline TBool IsAnyNotificationProcessingPending()
		{
		if(iPendingDriveList.Count())
			return ETrue;
		return EFalse;
		}
	
	/**
		@fn				RestartScanPeriod()
		Intended Usage	: Called to restart the scanning period.
		Error Condition	: None	
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is activated and will process pending drives list, if the timer has not been suspended
	 */
	void RestartScanPeriod();
	
	/**
		@fn				Suspend()
		Intended Usage	: Called to suspend the scanning timer, disabling the scanning from being restarted.
		Error Condition	: None	
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is set as suspended. No re-discovery will take place.
	 */
	void Suspend();
	
	/**
		@fn				Resume()
		Intended Usage	: Called to resume the scanning timer, and enable scanning to be restarted.
		Error Condition	: None	
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is set as resumed. Re-discovery will take place.
	 */
	void Resume();

private:
	/**
		@fn				CIdleScanningTimer(CDiscoverer& aDiscoverer)
		Intended Usage	: Standardised default c'tor made explicit to avoid unintentional
						conversion construction by the compiler.	
		Error Condition	: None	
		@since			7.0
		@param			aDiscoverer A reference to its owning class instance.
		@pre 			None
		@post			CIdleScanningTimer is fully constructed
	 */
	explicit CIdleScanningTimer(CDiscoverer& aDiscoverer);
	/**
		@fn				void ConstructL()
		Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
		Error Condition	: Leaves with error code : usually KErrNoMemory.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			void 
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is fully initialised.
	 */
	void ConstructL();

	/**
		@fn				DoCancel()
		Intended Usage	: The cancel action called by CActive::Cancel(). 
		Error Condition	: None	
		@since			7.0
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is no longer active on the current scheduler.
	 */
	void DoCancel();

	/**
		@fn				RunL()
		Intended Usage	: When the object activates, this method calls
						the CDiscoverer to continue the incremental scan 
						of the Interface Implementation Collection directories. 
		Error Condition	: Not enough memory available to add to complete the scan.
		@leave  		KErrNoMemory
		@since			7.0
		@pre 			CIdleScanningTimer is fully constructed.
		@post			The Interface Implementation Collection directories Scan has completed another step and the
						CIdleScanningTimer has been re-activated for the next step
						OR The Interface Implementation Collection directories Scan is complete and the 
						CDirChangeNotifier has been re-activated.
	 */
	void RunL();

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Called when the directory change
						notification processing failed by leaving.
		@since			7.0
		@param			aError The error code that the RunL left with.
		@return			TInt KErrNone. 
		@pre 			CIdleScanningTimer is fully constructed.
		@post			CIdleScanningTimer is non-active.
	 */
	TInt RunError(TInt aError);

private:
	/** A reference to its owning class instance */
	CDiscoverer& iDiscoverer;	

	/** 
		An array of drives to be processed.
	*/
	RArray<TUint> iPendingDriveList;
	
	/**
		A flag to indicate if the timer is suspended or not.
	*/
	TBool iSuspended;

	// List the internal classes as friends
	// the Test bed state accessor
	friend class TDiscoverer_StateAccessor;
	};



/**
	CDirScanner implements incremental scanning of the Interface Implementation 
	Collection directories on behalf of the CDiscoverer.
	Its methods are called in response to the timer task execution,
	thereby requiring the incremental scheduling.
 */ 
class CDiscoverer::CDirScanner : public CBase
	{
// Methods
public:
	/**
		@fn				CDirScanner(CDiscoverer& aDiscoverer, RFs& aFs)
		Intended Usage	: Standardised safe construction which leaves nothing
						  on the cleanup stack.
		Error Condition	: Not enough memory available.	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDiscoverer A reference to its owning class instance.
		@param			aFs A reference to a connected file server instance.
		@return			CDirScanner* a pointer to the new class
		@pre 			None
		@post			CDirScanner is fully constructed, and initialised.
	 */
	static CDirScanner* NewL(CDiscoverer& aDiscoverer, RFs& aFs);

	/**
		@fn				virtual ~CDirScanner()
		Intended Usage	: Standard default d'tor	
		Error Condition	: None	
		@since			7.0
		@pre 			CDirScanner is fully constructed.
		@post			CDirScanner is totally destroyed
	 */
	virtual ~CDirScanner();

	
	/**
		@fn				DiscoverPluginsL()
		Intended Usage	: Discover the plugins the specific drive units.
		@param			: aIsRO a flag to show if the Drive Units list is RO Internal or not.
		@pre 			: CDiscoverer is fully constructed.iDirScanner is fully constructed.
		@pre			: CDirScanner is fully constructed.
		@post			: the plugins on the specific Drive Units have been discovered.
	 */
	 void DiscoverPluginsL(TBool aDiscoverReadOnlyDrives);

	/**
		@fn				ScanDriveL(TDriveUnit aDrive)
		Intended Usage	: Helper method for StartScanL.
						Scan a drive for the Interface Implementation Collection directories
						and identify any Interface Implementation 
						Collections within that directory
						for addition to the registry. 
		Error Condition	: None	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDrive The drive to scan.
		@param			aIsRO A flag to tell if the drive unit is RO or Non-RO
		@pre 			CDirScanner is fully constructed.
		@post			Another drive's Interface Implementation Collection directories has been scanned,
	 */
	void ScanDriveL(const TDriveUnit& aDrive,  TBool aIsRO);

private:
	/**
		@fn				CDirScanner(CDiscoverer& aDiscoverer, RFs& aFs)
		Intended Usage	: Standardised default c'tor. 
		Error Condition	: None	
		@since			7.0
		@param			aDiscoverer A reference to its owning class instance.
		@param			aFs A reference to a connected file server instance.
		@pre 			None
		@post			CDirScanner is fully constructed
	 */
	CDirScanner(CDiscoverer& aDiscoverer, RFs& aFs);

	/**
		@fn				void ConstructL()
		Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
		Error Condition	: Leaves with error code : usually KErrNoMemory.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			void 
		@pre 			CDirScanner is fully constructed.
		@post			CDirScanner is fully initialised.
	 */
	void ConstructL();



	/**
		@fn				DoScanDriveL(const TDriveUnit aDrive, const TUidType aUidType)
		Intended Usage	: Helper method for StartScanL.
						Scan a drive for the Interface Implementation Collection directories
						and identify any Interface Implementation 
						Collections within that directory
						for addition to the registry. 
		Error Condition	: None	
		@leave  		KErrNoMemory
		@since			9.0
		@param			aDrive The drive to scan.
		@param			aUidType The uid type to be searched for.
		@param			aIsRO Indicates whether the current drive is read Only.
		@pre 			CDirScanner is fully constructed.
		@post			Another drive's Interface Implementation Collection directories has been scanned,
	 */
	TBool DoScanDriveL(const TDriveUnit& aDrive, const TUidType& aUidType, TBool aIsRO);

	/**
		@fn				DoScanSpiFileL(const TParse aSpiPath)
		Intended Usage	: Helper method for StartScanL.
						Scan a SPI file for the Interface Implementation Collection directories
						and identify any Interface Implementation Collections within that directory
						for addition to the registry. 
		Error Condition	: None	
		@leave  		KErrNoMemory
		@since			9.0
		@param			aSpiPath The SPI file to scan.
		@pre 			CDirScanner is fully constructed.
		@post			Another drive's Interface Implementation Collection has been scanned from an SPI file.
	 */
	TBool DoScanSpiFileL(const TParse& aSpiPath);

	
// Attributes / Properties
private:
	/** A reference to its owning class instance */
	CDiscoverer& iDiscoverer;	
	/** A reference to a connected file server instance */
	RFs&	iFs;
	/** The Full Interface Implementation Collection Directory path */
	
	// List the internal classes as friends
	// the Test bed state accessor
	friend class TDiscoverer_StateAccessor;


};  // CDirScanner

#endif // __DISCOVERER_H__

