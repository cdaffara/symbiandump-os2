/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to get notification of system time change.
*
*/


#ifndef WLANSYSTEMTIMEHANDLER_H
#define WLANSYSTEMTIMEHANDLER_H

#include <e32base.h>
#include <e32std.h>


/**
 * Callback interface
 */
class MWlanSystemTimeCallback
    {
    public:
    
        /**
         * User has changed the time and therefore WLAN region cache is not valid anymore.
         */
        virtual void OnSystemTimeChange() = 0;
        
        /**
         * WLAN region cache needs to be always cleared after defined time.
         */
        virtual void OnCacheClearTimerExpiration() = 0;
    };

/**
 * System time change handler.
 * Active object that waits for system time change
 * notification from Symbian side.
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CWlanSystemTimeHandler ) : public CActive
    {
public: // Constructors and destructor

    /**
     * Static constructor.
     * @param aClient          Interfce to client.
     */
    static CWlanSystemTimeHandler* NewL(
        MWlanSystemTimeCallback& aClient);

    /**
     * Destructor.
     */
    virtual ~CWlanSystemTimeHandler();

public: // New functions

    /**
     * Start the timer
     * Handler starts to wait for system time change notifications
     * @since S60 v.5.0
     * @return error code
     */
    TInt StartTimer();

    /**
     * Stop the timer
     * Handler stops to wait for system time change notifications
     * @since S60 v.5.0
     * @return error code
     */
    void StopTimer();


public: // Functions from base classes

    /**
    * From CActive Is executed when synchronous request is ready.
    */
    void RunL();

    /**
    * From CActive Cancel synchronous request.
    */
    void DoCancel();

private: // Functions

    /**
     * C++ default constructor.
     * @param aClient Interfce to client.
     */
    CWlanSystemTimeHandler(
        MWlanSystemTimeCallback& aClient );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
   
private: // Data

    /**
     * Asynchronous timer used to receive notification for system time change.
     */
    RTimer iTimer;
       
    /**
     * Interface to forward packets.
     */
    MWlanSystemTimeCallback& iClient;

    };

/**
 * Region cache clearing handler.
 * Active object that ensures that region cache is cleared after defined time.
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CWlanPeriodicCacheClearingHandler ) : public CActive
    {
public: // Constructors and destructor

    /**
     * Static constructor.
     * @param aClient          Interfce to client.
     */
    static CWlanPeriodicCacheClearingHandler* NewL(
        MWlanSystemTimeCallback& aClient);

    /**
     * Destructor.
     */
    virtual ~CWlanPeriodicCacheClearingHandler();

public: // New functions

    /**
     * Start the timer
     * Handler starts to wait for region cache clearing timer expiration.
     * @since S60 v.5.0
     * @return error code
     */
    TInt StartTimer();

    /**
     * Stop the timer
     * Handler stops to wait for region cache clearing timer expiration.
     * @since S60 v.5.0
     * @return error code
     */
    void StopTimer();


public: // Functions from base classes

    /**
    * From CActive Is executed when synchronous request is ready.
    */
    void RunL();

    /**
    * From CActive Cancel synchronous request.
    */
    void DoCancel();

private: // Functions

    /**
     * C++ default constructor.
     * @param aClient Interfce to client.
     */
    CWlanPeriodicCacheClearingHandler(
        MWlanSystemTimeCallback& aClient );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
   
private: // Data
   
    /**
     * Asynchronous timer used to ensure that WLAN region cache is cleared after
     * defined time.
     */
    RTimer iClearTimer;
    
    /**
     * The amount of times that iClearTimer needs to be started before the
     * whole region cache expiration time has exceeded.
     */
    TInt iClearTimerCounter;
    
    /**
     * Interface to forward the information that timer has expired.
     */
    MWlanSystemTimeCallback& iClient;

    };

#endif // WLANSYSTEMTIMEHANDLER_H
