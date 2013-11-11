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
* Description:  Definition of MDispatcher interface that used
*        to dispatch incoming messages to servers
*        and outcoming to host client.
*        MDispatcher* should be used by services to send outcoming
*        messages
*
*/


#ifndef DISPATCHERINTERFACE_H__
#define DISPATCHERINTERFACE_H__

#include <e32std.h>

class CHtiMessage;
class CConsoleBase;

enum THtiMessagePriority
    {
    EHtiPriorityDefault = 0,
    EHtiPriorityData = 0,
    EHtiPriorityControl = 2
    };

const TInt KErrorDescriptionMaxLength = 118;

/**
* Observer interface that should be implemented by plugin if it wants
* to resend message failed due to KErrNoMemory.
**/
class MHtiMemoryObserver
    {
public:
    /**
    *
    * This function called by the dispatcher after plug-in request
    * (@see MHtiDispatcher::AddMemoryObserver()) every time
    * when some message was removed from the outgoing queue.
    *
    * @param aAvailableMemory amount of memory currently available for
    *           outgoing messages
    **/
    virtual void NotifyMemoryChange(TInt aAvailableMemory) = 0;
    };

/**
* Interface to the HTI dispatcher used by service plug-ins for message
* sending.
**/
class MHtiDispatcher
    {
public:

    /**
    *
    * Send message from the service back to host client.
    * Transfer ownership of aMessage.
    * The function adds messages to the outgoing queue.
    * Message will be actualy sent later.
    * Function can return KErrNoMemory if there is not enough memory to
    * add it to the queue.
    * Plug-in use AddMemoryObserver to be notified later when memory
    * will be available to continue data transfer.
    *
    * @param aMessage contain server-specific data
    * @param aTargetServiceUid service UID
    * @param aWrappedFlag shoud set to ETrue if message should be
    *       wrapped by security manager
    * @param aPriority message priority
    *
    * @return KErrNone if message was succesfully added to the outgoing queue
    *         KErrNoMemory when there is not enough memory.
    */
    virtual TInt DispatchOutgoingMessage( TDesC8* aMessage,
                    const TUid aTargetServiceUid,
                    TBool aWrappedFlag,
                    THtiMessagePriority aPriority ) = 0;

    virtual TInt DispatchOutgoingMessage(TDesC8* aMessage,
                    const TUid aTargetServiceUid) = 0;

    virtual TInt DispatchOutgoingErrorMessage(TInt aErrorCode,
                    const TDesC8& aErrorDescription,
                    const TUid aTargetServiceUid) = 0;
    /**
    * Adds memory observer.
    *
    * @param anObserver memory observer
    */
    virtual void AddMemoryObserver(MHtiMemoryObserver* anObserver) = 0;

    /**
    * Removes memory observer.
    *
    * @param anObserver memory observer
    */
    virtual void RemoveMemoryObserver(MHtiMemoryObserver* anObserver) = 0;

    /**
    * Returns tha amount of currenly available memory for messages.
    */
    virtual TInt GetFreeMemory() = 0;

    /**
    * Notifis dispatcher that some async request is over.
    * Should be used when plug-in uses async services
    * and enters in a busy state (IsBusy() returns true)
    * Should be called when plug-in is able to process new messages again
    */
    virtual void Notify( TInt anError ) = 0;

    /*
    * Getter for the HTI console
    */
    virtual CConsoleBase* GetConsole() = 0;

    /*
    * Shutdown HTI and reboot the device.
    *
    * Called from service plug-ins or from EHtiReboot HtiSystem command
    */
    virtual void ShutdownAndRebootDeviceL() = 0;
    };

#endif
