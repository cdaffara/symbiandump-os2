/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class implements command queue for BgScan.
*
*/

/*
* %version: 2 %
*/


#ifndef WLANBGSCANCOMMAND_H
#define WLANBGSCANCOMMAND_H

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include <e32msgqueue.h>

// forward declarations
class MWlanBgScanCommandListener;

/**
 *  CWlanBgScanCommand class
 * 
 *  @since S60 v5.2
 */
 
NONSHARABLE_CLASS( CWlanBgScanCommand ) : 
    public CActive
    {
    
public:

    /**
     * WLAN BgScan Commands.
     */
    enum TWlanBgScanCommand
        {
        ESetInterval = 0,
        ECommandMax         // Last command, not used
        };
    
    /**
     * Static constructor.
     * @param aObserver Object that is commanded based on received commands.
     */
    static CWlanBgScanCommand* NewL( MWlanBgScanCommandListener& aListener );
    
    /**
     * Destructor.
     */
    ~CWlanBgScanCommand();
    
    /**
     * Command Queue.
     * Queues command for waiting to be executed. The method completes
     * AWS's AO, which RunL method will then be called in AWS thread
     * context.
     * @param aCommand Command to be queued.
     * @param aParameter Parameter for the command to be queued.
     * @param aStatus Status of the calling active object. On successful
     *                completion contains KErrNone, otherwise one of the
     *                system-wide error codes.
     */
    void CommandQueue( TWlanBgScanCommand aCommand, TUint32 aParameter, TRequestStatus& aStatus );

private: // CWlanBgScanCommand private definitions
        
    /**
     * WLAN BgScan Message.
     * Message format used in BgScan message queue.
     */
    struct TWlanBgScanMsg
        {
        TWlanBgScanCommand iCommand;
        TUint32 iParameter;
        TRequestStatus* iReportStatusPtr;
        RThread iClientThread;
        };
    
private: // CWlanBgScanCommand private methods

    /**
     * C++ constructor.
     * @param aListener Object that is commanded based on received commands.
     */
    CWlanBgScanCommand( MWlanBgScanCommandListener& aListener );
        
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Process Commands.
     * Processes all the commands from BgScan message queue. This method is
     * called when BgScan is indicated that there are messages in the queue.
     * The method is called from RunL.
     */
    void ProcessCommands();
        
private: // From CActive
    
    /**
     * Handles an active object's request completion event.
     */
    void RunL();
    
    /**
     * Cancels asynchronous request.
     */
    void DoCancel();
    
    /**
     * Handles a leave occurring in RunL().
     * @param aError Error that caused RunL to leave.
     */
    TInt RunError( TInt aError );   
    
private: // CAwsWlanScanMonitor private data

    /**
     * BgScan Command Listener.
     * Component to which commands are passed for processing.
     */
    MWlanBgScanCommandListener& iListener;
        
    /**
     * Message Queue.
     * Message queue for commands coming from WLAN Engine.
     */
    RMsgQueue<TWlanBgScanMsg> iMsgQueue;
    
};

#endif // WLANBGSCANCOMMAND_H
