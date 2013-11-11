/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#ifndef FEATMGRSERVER_H
#define FEATMGRSERVER_H

//  INCLUDES
#include <e32svr.h>
#include <featmgr/featurecmn.h>
#include <babackup.h>
#include "featmgrclientserver.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrtimer.h"
#include "featmgrfeatureregistry.h"
#include "burstate.h"

#ifdef FEATURE_MANAGER_STARTUP_TEST
    inline void FmgrFatalErrorL(TInt err, const TDesC&, TInt)
        {
        User::Leave(err);
        }
#else
    inline void FmgrFatalErrorL(TInt, const TDesC& aPanicCategory, TInt aPanicCode)
        {
        User::Panic(aPanicCategory, aPanicCode);
        }
#endif

// FORWARD DECLARATIONS
class CFeatMgrPluginHandler;
class CFeatMgrFeatureRegistry;

enum TFeatureSupport
    {
    EFeatureSupportDisable = EFalse,
    EFeatureSupportEnable = ETrue,
    EFeatureSupportUntouch = 2
    };

// CLASS DECLARATIONS

#ifdef EXTENDED_FEATURE_MANAGER_TEST
/**
 Auto shutdown class (standard pattern) for testing purposes 

*/
class CShutdown : public CTimer
	{
	enum TMemScanShutdown
	    {
	    EMemScanServShutdownDelay=0x200000
	    };
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};
#endif // EXTENDED_FEATURE_MANAGER_TEST

/**
  Feature Manager server implementation 

*/
NONSHARABLE_CLASS(CFeatMgrServer) : 
    public CPolicyServer,
    public MFeatMgrTimerCallback,
    public MFeatMgrRegistryObserver,
    private MBackupOperationObserver, 
    private MBackupObserver    
    {
    public:  // Constructors and destructor
        
        /**
         Creates a new server. Newly created instance is left in the cleanup stack.
         @param aPriority Priority of the server.
         @return A pointer to the created object.
        */
        static CFeatMgrServer* NewLC( const TInt aPriority );

        /**
         Destructor.
        */
        virtual ~CFeatMgrServer();

    public: // New functions      
           
        /**
         Called by plugin handler to inform feature info to server.
        */ 
        void FeatureInfoL( RArray<FeatureInfoCommand::TFeature>& aFeatureList, 
                           CFeatMgrPluginHandler* aPluginHandler );

        /**
         Called by plugin handler to inform feature info to server.
        */ 
        void FeatureInfoL( RFeatureArray& aFeatureList, CFeatMgrPluginHandler* aPluginHandler );

        /**
         Returns ETrue if all plugins are ready and 
         Feature info received. 
         Else returns EFalse.
        
         @return plugins loading status
        */
        TBool PluginsReady() const;
        
        /**
         Returns ETrue if backup is in progress 
         Else returns EFalse.
         @return backup status
        */
        TBool BURIsInProgress() const;

#ifdef EXTENDED_FEATURE_MANAGER_TEST
        /**
         Called by session to indicate addition of session.
        */ 
        void AddSession();
        
        /**
         Called by session to indicate end of session.
        */ 
        void DropSession();
#endif // EXTENDED_FEATURE_MANAGER_TEST

    public: // Functions from base classes
        
    	// From MFeatMgrTimerCallback    	
    	virtual void TimerFired();

        // From MFeatMgrRegistryObserver    	
    	virtual void HandleFeatureChange( TFeatureServerEntry& aFeature, TFeatureChangeType aType );
        
       	/**
		Function from MBackupOperationObserver in response to a change in the state of the backup and
		restore operation(s). This function is called to notify the Feature Manager that a backup and 
		restore operation is in progress. However, this is not the whole story, and notification using 
		the callback ChangeLockFileL() is also used.
		@param			aBackupOperationAttributes The current state of the backup operation. Made up of 
		                MBackupObserver::TFileLockFlags and TOperationType types.
		@see			MBackupObserver::TFileLockFlags 
		@see			TOperationType.
	    */
        void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);

       	/**
		 * Function from MBackupObserver in response to a change in the state of the file to be subject to
		 * the BUR operation. This function is called to notify the Feature Manager that a filelock has 
		 * changed.
		 * @param aFileName name of the file whose status has changed.
		 * @param	aFlags the lock status information for the file. 
	     */
        void ChangeFileLockL( const TDesC& aFileName, TFileLockFlags aFlags );
        
    public:

    /**
     * Feature manager state machine function.
     * This returns the feature manager back to a normal state.
     * @param BURStatus current state
     * @return BURStatus the next state
     */ 
    BURStatus Goto_NormalState( BURStatus aCurrent );
    /**
     * Feature manager state machine function.
     * This will put the Feature Manager into the "start backup" state. Where the Feature Manager
     * is expecting the BUR backup to start.
     * @param BURStatus current state
     * @return BURStatus the next state
     */ 
    BURStatus Goto_StartBackupState( BURStatus aCurrent );
    /**
     * Feature manager state machine function.
     * This will put the Feature Manager into the "end backup" state. Where the Feature Manager
     * is expecting the BUR backup has just ended, and it is required to return itself to a 
     * normal operating mode.
     * @param BURStatus current state
     * @return BURStatus the next state
     */ 
    BURStatus Goto_EndBackupState( BURStatus aCurrent );
    /**
     * Feature manager state machine function.
     * This will put the Feature Manager into the "start restore" state. Where the Feature Manager
     * is expecting the BUR restore operation to start.
     * @param BURStatus current state
     * @return BURStatus the next state
     */ 
    BURStatus Goto_StartRestoreState( BURStatus aCurrent );
    /**
     * Feature manager state machine function.
     * This will put the Feature Manager into the "end restore" state. Where the Feature Manager
     * is expecting the BUR restore operation has just ended, and it is required to return itself 
     * to a normal operating mode.
     * @param BURStatus current state
     * @return BURStatus the next state
     */ 
    BURStatus Goto_EndRestoreState( BURStatus aCurrent );
    /**
     * Feature manager state machine function.
     * This will return the Feature Manager from its current state, and into a "safe" state whereby
     * a normal operating mode is possible again. This is an error recovery function.
     * @param BURStatus current state
     * @return BURStatus the next state
     */ 
    BURStatus Goto_ErrorState( BURStatus aCurrent );
    
    private:

    /* Clears the feature array ready to re-populate it during a restore operation */
    void ClearFeaturesL( void );
    /* Reloads the feature array and merges features during a restore operation */
    void LoadFeaturesL( void );
    /* Handles any outstanding notifications after a restore operation */
    void HandleRestoredNotificationsL( void );
    
    private:

        /**
         C++ constructor.
        */
        CFeatMgrServer( const TInt aPriority, 
                        const TServerType aType = EUnsharableSessions );
        /**
         By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
         Creates a new session when client connects.
         @param aVersion Version
         @param aMessage 'connect' message from the client
         @return Pointer to created session or leaves with codes
                 KErrNotSupported if versions does not match
                 KErrNoMemory if creation of new session fails.
        */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage ) const;
        /**
         Panics the server thread
         @param aCategory Panicer's id 
         @param aPanic    Reason of panic 
         @return void
        */
        void Panic( const TDesC& aCategory, const TInt aReason );
        
        /**
         Lists and loads Feature Manager plugins.
        */
        TBool LoadPluginsL();
        
        /**
         Deletes plugin handler if feature info is ready.
        */
        void DeletePlugins();
        
        /**
         Check whether plugins have finished processing.
        */
        void CheckPluginsReadyL();
        
        /**
         Calls ServicePendingRequestsL of sessions  
        */
        void ServicePendingRequests();
        
    private:    // Data
        
        // Timer pointer.
        CFeatMgrTimer* iTimer;     

        CBurState iBurState;
        
        // ETrue when backup in progress
        TBool iBURInProgress;
        
        // ETrue when feature info received from all plugins
        TBool iPluginsReady;
                
        // Struct to contain information of found Feature Manager plugins
        struct SFeatMgrPluginInfo
            {
            // Plugin handler of the plugin
            CFeatMgrPluginHandler*  iPluginHandler;
            // ETrue if feature info received from plugin handler
            TBool iPluginReady;
            };
	    
	    // Array of found Feature Manager plugins
	    RArray<SFeatMgrPluginInfo>  iPluginList;
	    	    
        // ETrue if all plugin handlers are deleted
        TBool  iPluginsDeleted;
        
        // ETrue if ServicePendingRequest not called yet.
        TBool iPendingRequests;
        
        // File server session
        RFs iFs;         

        // ETrue when iPluginsReady is ETrue and all feature files have been read. 
        TBool iFeaturesReady;

        CFeatMgrFeatureRegistry* iRegistry;

        // Backup Notification
        TUint32                   iBackupFlag;
        CBaBackupSessionWrapper * iBackupNotification;

#ifdef EXTENDED_FEATURE_MANAGER_TEST
        TInt iSessionCount;
        CShutdown iShutdown;
#endif
    };

#endif      // FEATMGRSERVER_H 
            
// End of File
