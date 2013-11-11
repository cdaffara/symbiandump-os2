/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MDispatcher interface that used
*        to dispatch incoming messages to servers
*        and outcoming to host client.
*        MDispatcher should be used by services to send outcoming
*        messages and by communication modules to dispatch messages.
*        This is the main class in HtiFramework.
*
*/


#ifndef DISPATCHER_H__
#define DISPATCHER_H__

#include <e32base.h>
#include <e32cons.h>
#include "HtiDispatcherInterface.h"
#include "HtiMessage.h"
#include "HtiLogging.h"

const static TUid KHtiSystemServiceUid = { 0x1020DEB6 };

const static TInt KServiceArrayGranularity = 4;

const static TInt KHandbrakeTimerIntervalMS = 100 * 1000; //in microseconds
const static TInt KMaxFailedDispatchCalls = 4;

//forward declarations
class CHtiCommAdapter;
//class CHtiMessage;
class CHtiMessageQueue;
class CHTIServicePluginInterface;
class CHTICommPluginInterface;
class CHtiSecurityManager;


// HTI Framework error codes
enum THtiError
    {
    EHtiErrBigMessage = 1,
    EHtiErrNoMemory,
    EHtiErrServiceNotFound,
    EHtiErrServiceError,
    EHtiErrNotAuthorized,
    EHtiErrUnwrap
    };

// Types for restore factory settings
enum TRfsType
    {
    ERfsUnknown = -1,
    ERfsNormal  = 0,
    ERfsDeep    = 1
    };

/**
* Helper class that constructs messages
* used by the HTI system service
*
**/
class THtiSystemProtocolHelper
    {
public:
    static TDesC8* ErrorMessageL( TInt aHtiErrorCode,
                                  const TUid aTargetServiceUid );
    static TDesC8* ErrorMessageL( TInt aHtiErrorCode,
                                  const TUid aTargetServiceUid,
                                  TInt aErrorCode,
                                  const TDesC8& aErrorDescription );
    static TDesC8* AuthMessageL( const TDesC8& aToken );
    };

class CHtiDispatcher:
    public CBase,
    public MHtiDispatcher
    {
public:
    /**
    * Creates dispatcher. Leaves on failure.
    *
    * @param aCommPlugin plug-in name to use for communication
    * @param aMaxMsgSize maximum size for an incoming message
    * @param aMaxQueueMemorySize maximum size of all messages in the
    *        incoming queue
    * @param aReconnectDelay seconds of delay for reconnecting when connection
    *        lost. Value 0 means that reconnect would not happen.
    * @param aShowConsole whether to open a console window for HTI
    * @param aShowErrorDialogs whether to show a dialog in case of critical
    *        error or just silently exit
    * @return pointer to the created CHtiDispatcher instance
    */
    static CHtiDispatcher* NewL( const TDesC8& aCommPlugin,
                                 TInt aMaxMsgSize,
                                 TInt aMaxQueueMemorySize,
                                 TInt aReconnectDelay,
                                 TBool aShowConsole,
                                 TBool aShowErrorDialogs );

    /**
    * Creates dispatcher and puts the created isntance to the cleanup stack.
    * Leaves on failure.
    *
    * @param aCommPlugin plug-in name to use for communication
    * @param aMaxMsgSize maximum size for an incoming message
    * @param aMaxQueueMemorySize maximum size of all messages in the
    *        incoming queue
    * @param aReconnectDelay seconds of delay for reconnecting when connection
    *        lost. Value 0 means that reconnect would not happen.
    * @param aShowConsole whether to open a console window for HTI
    * @param aShowErrorDialogs whether to show a dialog in case of critical
    *        error or just silently exit
    * @return pointer to the created CHtiDispatcher instance
    */
    static CHtiDispatcher* NewLC( const TDesC8& aCommPlugin,
                                  TInt aMaxMsgSize,
                                  TInt aMaxQueueMemorySize,
                                  TInt aReconnectDelay,
                                  TBool aShowConsole,
                                  TBool aShowErrorDialogs );

    /**
    * Destructor. Frees the allocated resources.
    */
    ~CHtiDispatcher();

    /**
    *
    * Parse message to a corresponding service
    * The function returns immediately.
    * Transfer ownership of aMessage.
    *
    *  @param aMessage contains incoming message
    */
    void DispatchIncomingMessage( CHtiMessage* aMessage );

    /**
    * Construct and dispath HTI error message. For internal use.
    *
    * @param aHtiErrorCode error code
    * @param aTargetService UID of the service plug-in that caused the error
    * @return standard Symbian error code
    */
    TInt DispatchOutgoingErrorMessage( THtiError aHtiErrorCode,
                   const TUid aTargetService = KHtiSystemServiceUid );

public: // Inherited methods from MHtiDispatcher
    TInt DispatchOutgoingMessage( TDesC8* aMessage,
                    const TUid aTargetServiceUid,
                    TBool aWrappedFlag,
                    THtiMessagePriority aPriority );

    TInt DispatchOutgoingMessage( TDesC8* aMessage,
                    const TUid aTargetServiceUid );

    TInt DispatchOutgoingErrorMessage( TInt aErrorCode,
                    const TDesC8& aErrorDescription,
                    const TUid aTargetServiceUid );

    /**
    * Adds memory observer.
    *
    * @param anObserver memory observer
    */
    void AddMemoryObserver( MHtiMemoryObserver* anObserver );

    /**
    * Removes memory observer.
    *
    * @param anObserver memory observer
    */
    void RemoveMemoryObserver( MHtiMemoryObserver* anObserver );

    /**
    * Returns the amount of memory available for message in the incoming queue
    *
    */
    TInt GetFreeMemory();

    void Notify( TInt aError );

    /*
    * Returns a pointer to the HTI console
    */
    CConsoleBase* GetConsole();

    /**
    * Unload all service plugins and clear queues
    */
    void Reset();

    /*
    * Shutdown HTI and reboot the device.
    *
    * Called from service plug-ins or from EHtiReboot HtiSystem command
    */
    void ShutdownAndRebootDeviceL();

    /*
    * Query the ShowErrorDialogs configuration value.
    * @return iShowErrorDialogs value
    */
    TBool GetShowErrorDialogs();

    
    /*
     * Delay a period and reconnect when connection lost.
     * If the period is 0, reconnect would not happen.
     * @return whether reconnect
     */
     TBool CommReconnect();
     
protected:
    /**
    * Constructors
    *
    */
    CHtiDispatcher( TInt aMaxQueueMemorySize, TInt aReconnectDelay, TBool aShowErrorDialogs );

    void ConstructL( const TDesC8& aCommPlugin,
                     TInt aMaxMsgSize,
                     TBool aShowConsole );

    /**
    * Construct and dispath HTI error message. For internal use
    *
    * @param aHtiErrorCode error code
    * @param aLeaveCode leave code that caused error
    * @param aTargetServiceUid UID of the service plug-in that caused the error
    *
    */
    TInt DispatchOutgoingErrorMessage( THtiError aHtiErrorCode,
                                    TInt aLeaveCode,
                                    const TUid aTargetServiceUid );

    /**
    * Ensures that iIdle is started
    * when a new message come
    *
    */
    void Start();

    /**
    * Does message dispatching one at a time
    * This method is called by the static callback function
    * DispatchCallback().
    *
    */
    TInt DoDispatch();

    void DoMemoryNotification();

    /**
    * Callback functions called by CIdle in idel time to
    * process messages in the queues
    *
    * @param aObj the reference to a CHTIDispatcher is passed
    *             to call its non-static DoDispatch() method
    */
    static TInt DispatchCallback( TAny* aObj );

    /**
    * Finds and if needed loads specified service
    * Returns NULL if service in question was not found
    *
    */
    CHTIServicePluginInterface* GetService( const TUid aServiceUid );

    /**
    * Destructs all loaded services from iLoadedServices array.
    * Note this function does not delete iLoadedServices object itself.
    *
    */
    void UnloadAllServices();

    /**
    * Used instead of panic when framework cannot work further
    * Reason code and description used for logging
    *
    * @param aReason code for rebooting
    * @param aReasonDescr description
    */
    void UrgentReboot( TInt aReason, const TDesC8& aReasonDescr );

    /**
    * Handle HTI message addressed to HTI system service.
    * It's processed right in the dispatcher.
    * The method is trapped and no special actions if it leaves.
    * @param aMessage the message data
    */
    void HandleSystemMessageL( const TDesC8& aMessage );

    // functions to handle HTI system commands

    /**
    * Phone reboot
    *
    */
    void Reboot();

    /**
    * Activate restore factory settings. Causes also a reboot.
    * The mode of restore factory settings (normal or deep) is
    * defined by iRfsMode.
    */
    void RestoreFactorySettings();

    /**
    * Checks whether given file is in a ROM drive.
    * @param aFileName full path to the file to check
    */
    TBool IsFileInRom( const TDesC& aFileName );

    TDesC8* ServicePluginsListL();

    /**
    * Checks and readjust priorities of comm adapters and idle object to
    * prevent overflow of message queues.
    *
    */
    void CheckPriorities();

    /**
    * Kills HTI watchdog process if it exists.
    *
    */
    void KillHtiWatchDogL();

#ifdef __ENABLE_LOGGING__
    /**
    * List in log files all found comm and service plugins
    */
    void DebugListPlugins();
#endif

    /**
    * Generate unique ID for HTI instance.
    */
    void CreateInstanceId();

    /**
    * Maps the service plugin protocol UID to implementation UID and vice versa.
    */
    TUid MapServicePluginUid( const TUid aUid );

private:
    /**
    * used to dispatch messages when thread is free
    */
    CIdle* iIdle;

    /**
    * Indicates that iIdle was started
    */
    TBool iIdleActive;

    /**
    * Incoming message queue
    * holds pointers to messages located in heap
    */
    CHtiMessageQueue* iIncomingQueue;

    /**
    * Outgoing message queue
    * holds pointers to messages located in heap
    */
    CHtiMessageQueue* iOutgoingQueue;

    /**
    * loaded service plugins description
    */
    struct TServiceItem
        {
        TUid iServiceUid;
        CHTIServicePluginInterface* iService; /** service instance */
        };

    /**
    * An array that keeps all loaded service plugins
    */
    RArray<TServiceItem>* iLoadedServices;

    /**
    * Memory observers
    */
    RPointerArray<MHtiMemoryObserver>* iMemoryObservers;

    /**
    * Instance of comm. plug-in used by iListener and iSender.
    * It's not used by dispatcher.
    */
    CHTICommPluginInterface* iCommPlugin;

    /**
    * CCommAdapter instance for constant receiving of incoming HTI messages
    */
    CHtiCommAdapter* iListener;

    /**
    * CCommAdapter instance for sending outgoing HTI messages
    */
    CHtiCommAdapter* iSender;

    /**
    * Security manager
    */
    CHtiSecurityManager* iSecurityManager;

    /**
    * maximum size of all messages in the incoming queue
    */
    const TInt iMaxQueueMemorySize;

    /**
    * Flag to indicate reboot after framework stoped
    */
    TBool iToReboot;

    /**
    * Flag indicating the requested factory settings restore mode.
    */
    TRfsType iRfsMode;

    /**
    * Queue size thresold for priorities readjusting
    */
    TInt iQueueSizeLowThresold;
    TInt iQueueSizeHighThresold;

    /**
    * Console for HTI
    */
    CConsoleBase* iConsole;

    /**
    * By default the idle object and comm adapters AO will have the same priority
    * iIdle will be added later to the AS that gives comm. adapter AO priority
    * In case of very fast incoming data there is possibility that idle will not be
    * called at all, because comm API will comlete async request at once. That will
    * lead to incoming queue overflow. The solution is to change AOs priorities.
    * Using AO priorities directly is complicated that active AO's priority cannot be changed.
    * So the order of AOs with equal priorities used to change the way they will be
    * picked up by AS.
    * When new incoming message is added, in comm adapter AO, the incoming queue is checked
    * in case it reaches predefined high-watermark, comm adapter AO is removed from AS and
    * added again, so idle AO will be picked next time.
    * when idle AO checks that incoming queue is less than predefined low-watermark, it will
    * remove itself and add to AS, to give a way to comm adapter's AO.
    * The following flag iIdleOverCommAdapter is used when idle is first in AS.
    *
    */
    TBool iIdleOverCommAdapter;

    /**
    * Unique ID for HTI instance.
    * The ID is generated only when it is queried for the first time (to add
    * randomness). Until then it's zero.
    */
    TUint32 iHtiInstanceId;

    /**
    * Indicates whether to show error dialogs (notifiers) in case of critical
    * errors or just silently exit.
    */
    TBool iShowErrorDialogs;
    
    /**
     * Delay a period and reconnect when connection lost.
     * If the period is 0, reconnect would not happen.
     */
    TInt iReconnectDelay;
    
    /**
     * Indicates reboot reason.
     * Reason type is defined in enum RStarterSession::TResetReason value in starterclient.h
     */
    TInt iRebootReason;
    };


#endif
