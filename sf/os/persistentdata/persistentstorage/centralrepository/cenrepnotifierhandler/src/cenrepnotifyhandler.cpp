// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// INCLUDE FILES
//



#include <e32std.h>
#include <e32svr.h>
#include <centralrepository.h>
#include "cenrepnotifyhandler.h"

// ============================ MEMBER FUNCTIONS ===============================

/**
* Constructor used for single key listening.
*
* @param aCallback Reference to a callback instance.
* @param aSession Reference to an existing repository session.
* @param aKeyType  Type of the key identified by aId parameter.
* @param aId       Id of the key that change notifications are needed for.
* @capability      None
*/
CCenRepNotifyHandler::CCenRepNotifyHandler(
    MCenRepNotifyHandlerCallback& aCallback, 
    CRepository& aSession, 
    TCenRepKeyType aKeyType, 
    TUint32 aId)
    : CActive(EPriorityStandard), iSession(aSession), 
    iCallback(aCallback), iKeyType(aKeyType), iId(aId), iWholeRepository(EFalse)
    {
    CActiveScheduler::Add(this);
    }

/**
* Constructor used for whole repository listening.
*
* @param aCallback Reference to a callback instance.
* @param aSession Reference to an existing repository session.
* @capability None
*/
CCenRepNotifyHandler::CCenRepNotifyHandler(
    MCenRepNotifyHandlerCallback& aCallback, 
    CRepository& aSession)
    : CActive(EPriorityStandard), iSession(aSession), iCallback(aCallback), 
    iKeyType(EIntKey), iId(0), iWholeRepository(ETrue)
    {
    CActiveScheduler::Add(this);
    }

/**
* This is a two-phase constructor method that is used to
* create a new instance for listening to the changes in a single key.
*
* @param aCallback Reference to a callback instance.
* @param aSession  Reference to an existing repository session.
*                  Do not close this session until all CCenRepNotifyHandler
*                  instances referring to it have been deleted.
* @param aKeyType  Type of the key identified by aId parameter. 
* @param aId       Id of the key that change notifications are needed for.
* @return          A pointer to a new instance of the CCenRepNotifyHandler class.
* 
* @leave           KErrArgument if invalid key type is passed as a parameter.
* @capability      None
*/
EXPORT_C CCenRepNotifyHandler* CCenRepNotifyHandler::NewL(
    MCenRepNotifyHandlerCallback& aCallback, 
    CRepository& aSession, 
    TCenRepKeyType aKeyType, 
    TUint32 aId)
    {
    CCenRepNotifyHandler* newInstance = NewLC(aCallback, aSession, aKeyType, aId);
    CleanupStack::Pop();
    return newInstance;
    }

/**
* This is a two-phase constructor method that is used to create a new
* instance for listening to the changes in all keys in the repository. 
*
* Type specific callback methods of MCenRepNotifyHandlerCallback will not
* be used when notifying about changes in this case,
* only HandleNotifyGeneric() is used.
*
* @param aCallback Reference to a callback instance.
* @param aSession  Reference to an existing repository session.
*                  Do not close this session until all CCenRepNotifyHandler
*                  instances referring to it have been deleted.
* @return          A pointer to a new instance of the CCenRepNotifyHandler class.
* @capability      None
*/
EXPORT_C CCenRepNotifyHandler* CCenRepNotifyHandler::NewL(
    MCenRepNotifyHandlerCallback& aCallback, 
    CRepository& aSession)
    {
    CCenRepNotifyHandler* newInstance = NewLC(aCallback, aSession);
    CleanupStack::Pop();
    return newInstance;
    }

/**
* This is a two-phase constructor method that is used to create a new
* instance for listening to the changes in a single key.
* Leaves the constructed instance to cleanup stack.
*
* @param aCallback Reference to a callback instance.
* @param aSession  Reference to an existing repository session.
*                  Do not close this session until all CCenRepNotifyHandler
*                  instances referring to it have been deleted.
* @param aKeyType  Type of the key identified by aId parameter.
* @param aId       Id of the key that change notifications are needed for.
* @return          A pointer to a new instance of the CCenRepNotifyHandler class.
* 
* @leave           KErrArgument if invalid key type is passed as a parameter.
* @capability      None
*/
EXPORT_C CCenRepNotifyHandler* CCenRepNotifyHandler::NewLC(
    MCenRepNotifyHandlerCallback& aCallback, 
    CRepository& aSession, 
    TCenRepKeyType aKeyType, 
    TUint32 aId)
    {
    if ( aKeyType != EIntKey && 
         aKeyType != ERealKey && 
         aKeyType != EStringKey  && 
         aKeyType != EBinaryKey ) 
        { 
        User::Leave( KErrArgument ); 
        } 

    CCenRepNotifyHandler* newInstance = new (ELeave) CCenRepNotifyHandler( aCallback, aSession, 
                                                                           aKeyType, aId);
    CleanupStack::PushL( newInstance );
    return newInstance;
    }

/**
* This is a two-phase constructor method that is used to create a new
* instance for listening to the changes in all keys in the repository.
* Leaves the constructed instance to cleanup stack. 
*
* Type specific callback methods of MCenRepNotifyHandlerCallback will
* not be used when notifying about changes in this case,
* only HandleNotifyGeneric() is used.
*
* @param aCallback Reference to a callback instance.
* @param aSession  Reference to an existing repository session.
*                  Do not close this session until all CCenRepNotifyHandler
*                  instances referring to it have been deleted.
* @return          A pointer to a new instance of the CCenRepNotifyHandler class.
* @capability      None
*/
EXPORT_C CCenRepNotifyHandler* CCenRepNotifyHandler::NewLC(
    MCenRepNotifyHandlerCallback& aCallback, 
    CRepository& aSession)
    {
    CCenRepNotifyHandler* newInstance = new (ELeave) CCenRepNotifyHandler(aCallback, aSession);
    CleanupStack::PushL( newInstance );

    return newInstance;
    }

// Destructor
EXPORT_C CCenRepNotifyHandler::~CCenRepNotifyHandler()
    {
    Cancel();
    }

/**
* When this method is called, the CCenRepNotifyHandler starts
* listening for notifications. If it is already listening, nothing happens.
*
* For single setting handler when there is already an existing notification
* on the same setting and session,the HandleNotifyError will be trigerred with
* KErrAlreadyExists error
* @leave KErrNotFound if setting does not exist
* 		 KErrPermissionDenied if client does not have the necessary capability
* plus other system-wide error codes.
* For whole settings handler, the HandleNotifyError will be trigerred with the 
* error code
* @capability Dependent Capability required depends on platform security of keyspace.
*/
EXPORT_C void CCenRepNotifyHandler::StartListeningL()
    {
    if(IsActive())
        {
        return;
        }
    User::LeaveIfError(OrderNotification());

    SetActive();
    }

/**
* When this method is called, the CCenRepNotifyHandler stops
* listening for notifications. If it is already stopped, nothing happens.
* @capability      None
*/
EXPORT_C void CCenRepNotifyHandler::StopListening()
    {
    if(IsActive())
        {
        Cancel();
        }
    }

/**
* Implements CActive
*/    
void CCenRepNotifyHandler::RunL()
    {
    // Check that notification was for correct ID or it was caused by
    // a repository wide reset (KInvalidNotificationId).
    TUint32 status = static_cast<TUint32>(iStatus.Int());
    if( !iWholeRepository && status != iId && 
        status != NCentralRepositoryConstants::KInvalidNotificationId )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CCenRepNotifyHandler::RunL(): Received notif about unknown key: %d"), 
        status);
        RDebug::Print(_L("CCenRepNotifyHandler::RunL(): Notification not renewed."));
#endif
        // We are not listening to key notified for us. Do not reorder notification 
        // as there is clearly some problems in central repository.
        iCallback.HandleNotifyError(status, KErrArgument, this);
        return;
        }

    if (iWholeRepository && iStatus.Int() == KErrPermissionDenied)
    	{

        RArray<TUint32> allKeyList;
        //check every single settings that we have read permission
        TRAPD(err,iSession.FindL(0x00000000,0x00000000,allKeyList));
        if (err==KErrNone)
        	{
        	TInt arrayCount=allKeyList.Count();
        	for (TInt i=0;i<arrayCount;i++)
        		{
        		TUint32 dummyMeta;
        		err=iSession.GetMeta(allKeyList[i],dummyMeta);
        		//potential error at this stage likely to include only
        		//KErrPermissionDenied plus other resource allocation
        		//error such as KErrNoMemory
        		if (err!=KErrNone)
        			{
        			TInt errorkey=allKeyList[i];
        			allKeyList.Close();
        			iCallback.HandleNotifyError(errorkey, err, this);
					return;
        			}
                }
        	}
        else
            {
            iCallback.HandleNotifyError( NCentralRepositoryConstants::KUnspecifiedKey , err, this );
            return;
            }
    	}

    // Reorder notification
    TInt err = OrderNotification();
    
    // Handle notification
    if ( err == KErrNone )
        {
        SetActive();
        if ( iWholeRepository )
            {
            iCallback.HandleNotifyGeneric(status);
            }
        else
            {
            switch (iKeyType)
                {
                case EIntKey:
                    {
                    TInt newValue;
                    err=iSession.Get(iId, newValue);
                    if (err==KErrNone)
                    	{
                    	iCallback.HandleNotifyInt(iId, newValue);
                    	}
                    }
                    break;
                case ERealKey:            
                    {
                    TReal newValue;
                    err=iSession.Get(iId, newValue);
                    if (err==KErrNone)
                    	{
                    	iCallback.HandleNotifyReal(iId, newValue);
                    	}
                    }
                    break;
                case EStringKey:
                    {
                    TBuf16<NCentralRepositoryConstants::KMaxUnicodeStringLength> newValue;
                    err=iSession.Get(iId, newValue);
                    if (err==KErrNone)
                    	{
                    	iCallback.HandleNotifyString(iId, newValue);
                    	}
                    }
                    break;
                case EBinaryKey:
                    {
                    TBuf8<NCentralRepositoryConstants::KMaxBinaryLength> newValue;
                    err=iSession.Get(iId, newValue);
                    if (err==KErrNone)
                    	{
                    	iCallback.HandleNotifyBinary(iId, newValue);
                    	}
                    }
                    break;
                default:
                    break;
                }
            if (err!=KErrNone)
            	{           	
            	iCallback.HandleNotifyError(iId,err,this);
            	}
            }
        }
    else
        {
        iCallback.HandleNotifyError(status, err, this);
        }
    }
    
/**
* Implements CActive
* @param aError the error returned
* @return error
*/
TInt CCenRepNotifyHandler::RunError(TInt aError)
    {
    if ( iWholeRepository )
        {
        iCallback.HandleNotifyError(NCentralRepositoryConstants::KInvalidNotificationId, 
                                    aError, this);    
        }
    else
        {
        iCallback.HandleNotifyError(iId, aError, this);    
        }
    
    return KErrNone;
    }

/**
* Implements CActive
*/    
void CCenRepNotifyHandler::DoCancel()
    {
    if ( iWholeRepository )
        {
        iSession.NotifyCancelAll();
        }
    else
        {
        iSession.NotifyCancel(iId);
        }
    }

/* 
* Order notification
* @return error code from CenRep
*/
TInt CCenRepNotifyHandler::OrderNotification()
    {
    if ( iWholeRepository )
        {
        // order notification for all keys in repository
        return iSession.NotifyRequest(0x00000000, 0x00000000, iStatus);
        }
    else
        {
        return iSession.NotifyRequest(iId, iStatus);
        }
    }

// -----------------------------------------------------------------------------
// MCenRepNotifyHandlerCallback::HandleNotifyXXX
// Default implementations for callback interface. 
// In debug build these methods print trace. 
// In release build they do nothing.
//
// These methods are documented in cenrepnotifierhandler.h. Don't redocument
// here.
// -----------------------------------------------------------------------------
// 
#ifdef _DEBUG
EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    RDebug::Print(_L("MCenRepNotifyHandlerCallback: Integer key %d changed, new value: %d"), 
    aId, aNewValue);
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyReal(TUint32 aId, TReal aNewValue)
    {
    RDebug::Print(_L("MCenRepNotifyHandlerCallback: Real key %d changed, new value: %e"), 
    aId, aNewValue);
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyString(TUint32 aId, 
                                                               const TDesC16&  aNewValue)
    {
    RDebug::Print(_L("MCenRepNotifyHandlerCallback: String key %d changed, new value: %S"), 
    aId, &aNewValue);
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyBinary(TUint32 aId, 
                                                               const TDesC8&  aNewValue)
    {
    RDebug::Print(_L("MCenRepNotifyHandlerCallback: Binary key %d changed, new value: %s"), 
    aId, aNewValue.Ptr());
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyGeneric(TUint32 aId)
    {
    if ( aId == NCentralRepositoryConstants::KInvalidNotificationId )
        {
        RDebug::Print(_L("MCenRepNotifyHandlerCallback: "));
        RDebug::Print(_L("Repository wide reset caused generic notification"));
        }
    else
        {
        RDebug::Print(_L("MCenRepNotifyHandlerCallback: Generic key %d changed"), aId);
        }
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyError(TUint32 aId, TInt aError, 
                                                              CCenRepNotifyHandler* aHandler)
    {
    RDebug::Print(_L("MCenRepNotifyHandlerCallback %d notifies error for id: %d, error: %d"), 
    aHandler, aId, aError);
    }

#else

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyInt(TUint32 /*aId*/, TInt /*aNewValue*/)
    {
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyReal(TUint32 /*aId*/, TReal /*aNewValue*/)
    {
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyString(TUint32 /*aId*/, 
    const TDesC16&  /*aNewValue*/)
    {
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyBinary(TUint32 /*aId*/, 
    const TDesC8&  /*aNewValue*/)
    {
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyGeneric(TUint32 /*aId*/)
    {
    }

EXPORT_C void MCenRepNotifyHandlerCallback::HandleNotifyError(TUint32 /*aId*/, TInt /*aError*/, 
    CCenRepNotifyHandler* /*aHandler*/)
    {
    }
#endif // _DEBUG

// End of File
