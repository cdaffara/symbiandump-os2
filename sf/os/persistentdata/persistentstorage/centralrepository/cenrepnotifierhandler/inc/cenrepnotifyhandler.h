/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CENREPNOTIFYHANDLER_H
#define CENREPNOTIFYHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MCenRepNotifyHandlerCallback;
class CRepository;

/**
* Active object wrapper for Central Repository one-shot notification handling.
* Central Repository Notification Handler API provides an easy-to-use 
* implementation of a CActive-based wrapper for Central Repository single-shot
* notifications. In most cases Central Repository Notification Handler can 
* automatically resubscribe to notifications and fetch the modified value from
* Central Repository. 
* The API consists of the classes CCenRepNotifyHandler and 
* MCenRepNotifyHandlerCallback. The user of this class needs to implement relevant
* MCenRepNotifyHandlerCallback interface methods to receive notifications. 
* The user of Central Repository Notification Handler API needs access to 
* Central Repository (centralrepository.h).
*
* Usage:
*  
* Initialization example (from a class that implements MCenRepNotifyHandlerCallback interface):
* @code
* iSession = CRepository::NewL(KTestUid);
* iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EStringKey, KKey1);
* iNotifyHandler->StartListeningL();
* @endcode
*
* Uninitialization example:
* @code
* iNotifyHandler->StopListening(); 
* delete iNotifyHandler;
* @endcode
*
* Handler method implementation example:
* @code
* void CMyCenRepNotifyTest::HandleNotifyString(TUint32 aId, const TDesC16&  aNewValue)
*    {
*    // Print out the notified value
*    RDebug::Print(_L("Key %d changed, new value: %S"), aId, &aNewValue);
*    }
* @endcode
*
* @publishedAll
* @released
*/
class CCenRepNotifyHandler : public CActive
    {
    public:
        /**
        * Defines different key types. Enumeration is used to indicate the 
        * key type that is listened to. 
        */
        enum TCenRepKeyType
        {
            EIntKey,    ///< Key holds a TInt value.
            ERealKey,   ///< Key holds a TReal value.
            EStringKey, ///< Key holds a TDesC16 value.
            EBinaryKey  ///< Key holds a TDesC8 value.
        };

        IMPORT_C static CCenRepNotifyHandler* NewL( MCenRepNotifyHandlerCallback& aCallback, 
                                                    CRepository& aSession, 
                                                    TCenRepKeyType aKeyType, 
                                                    TUint32 aId );

        IMPORT_C static CCenRepNotifyHandler* NewL( MCenRepNotifyHandlerCallback& aCallback, 
                                                    CRepository& aSession );

        IMPORT_C static CCenRepNotifyHandler* NewLC( MCenRepNotifyHandlerCallback& aCallback, 
                                                     CRepository& aSession, 
                                                     TCenRepKeyType aKeyType, 
                                                     TUint32 aId );

        IMPORT_C static CCenRepNotifyHandler* NewLC( MCenRepNotifyHandlerCallback& aCallback, 
                                                     CRepository& aSession );
        IMPORT_C void StartListeningL();

        IMPORT_C void StopListening();


        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCenRepNotifyHandler();

    protected:

        void RunL();
    
        TInt RunError( TInt aError );

        void DoCancel();

    private:

        CCenRepNotifyHandler( MCenRepNotifyHandlerCallback& aCallback, 
                              CRepository& aSession, 
                              TCenRepKeyType aKeyType, 
                              TUint32 aId );
                                                   
        CCenRepNotifyHandler( MCenRepNotifyHandlerCallback& aCallback, 
                              CRepository& aSession );
        TInt OrderNotification();
       
    private:
        
        CRepository& iSession;  // not owned by this class
        MCenRepNotifyHandlerCallback& iCallback; // not owned by this class
        TCenRepKeyType iKeyType;
        TUint32 iId;        
        TBool iWholeRepository;  // The flag to indicate if listening is for whole repository
    };


/**
* Class provides a callback interface for handling the notifification
* events from the Central Repository. The Client derives a class 
* from this interface and implements the HandleNotify-methods that 
* interest it.
* An empty default implementation is provided for all of the methods.
* In debug build the default implementations print out a debug trace.
*
* @publishedAll
* @released
*/
class MCenRepNotifyHandlerCallback
    {

    public:

    /** 
    * This callback method is used to notify the client about
    * changes for integer value keys, i.e. key type is EIntKey.
    *
    * @param aId Id of the key that has changed.
    * @param aNewValue The new value of the key.
    * @capability Dependent Capability required depends on implementation of override.
    */
    IMPORT_C virtual void HandleNotifyInt( TUint32 aId, TInt aNewValue );

    /** 
    * This callback method is used to notify the client about
    * changes for real value keys, i.e. key type is ERealKey.
    *
    * @param aId Id of the key that has changed.
    * @param aNewValue The new value of the key.
     * @capability Dependent Capability required depends on implementation of override.
   */
    IMPORT_C virtual void HandleNotifyReal( TUint32 aId, TReal aNewValue );

    /** 
    * This callback method is used to notify the client about
    * changes for string value keys, i.e. key type is EStringKey.
    *
    * @param aId Id of the key that has changed.
    * @param aNewValue The new value of the key.
    * @capability Dependent Capability required depends on implementation of override.
    */
    IMPORT_C virtual void HandleNotifyString( TUint32 aId, const TDesC16& aNewValue );

    /** 
    * This callback method is used to notify the client about
    * changes for binary value keys, i.e. key type is EBinaryKey.
    *
    * @param aId Id of the key that has changed.
    * @param aNewValue The new value of the key.
    * @capability Dependent Capability required depends on implementation of override.
    */
    IMPORT_C virtual void HandleNotifyBinary( TUint32 aId, const TDesC8& aNewValue );

    /** 
    * This callback method is used to notify the client about
    * changes in keys when the whole repository is listened for.
    *
    * Note: It is not guaranteed that a notification will be received
    *       for all keys, if multiple keys are changed in rapid succession
    *       by multiple threads or when the whole repository is reset,
    *       therefore only listen for whole repository if this is not an issue.
    *
    * @param aId Id of the key that has changed. If multiple keys were changed by
    *            whole repository reset, value will be KInvalidNotificationId.
    * @capability Dependent Capability required depends on implementation of override.
    */
    IMPORT_C virtual void HandleNotifyGeneric( TUint32 aId );

    /** 
    * This callback method is used to notify the client about errors
    * in the handler. Any error in handling causes the handler to stop
    * handling any more notifications. Handling can be restarted with
    * a call to aHandler->StartListeningL(), if the error is non-fatal.
    * However, be careful to trap any errors from this call if this is done.
    *
    * @param aId Id of the key this instance listens to or if notifications for
    *            whole repository are listened, could also be KInvalidNotificationId.
    * @param aError Error code.
    * @param aHandler Pointer to the handler instance. 
    *                 This pointer can be used to identify the handler or restart the listening.
    * @capability Dependent Capability required depends on implementation of override.
    */
    IMPORT_C virtual void HandleNotifyError( TUint32 aId, TInt aError, 
                                            CCenRepNotifyHandler* aHandler );
    };

#endif      // CENREPNOTIFYHANDLER_H

// End of File
