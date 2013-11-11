/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Interfaces for WLAN Background Scan component and AWS component.
*
*/

/*
* %version: 5 %
*/

#ifndef AWSINTERFACE_H
#define AWSINTERFACE_H


#include <e32std.h>

/**
 * The possible statuses of an AWS Power Save Mode.
 */
enum TAwsPsMode
    {
    /**
     * No power save.
     */
    EAwsPsModeOff = 0,
    /**
     * Deep power save.
     */      
    EAwsPsModeDeep,
    /**
     * Light power save.
     */
    EAwsPsModeLight
    };

/**
 *  @brief Interface definition for Adaptive WLAN Scanning component.
 *
 *  This class defines the methods for starting and stopping AWS.
 *  
 *  @since S60 v5.2
 */
class MAws
    {

public:

    /**
     * Start AWS.
     * 
     * @param aStatus Status of the calling active object. On successful
     *                completion contains KErrNone, otherwise one of the
     *                system-wide error codes.
     *
     * @since S60 v5.2
     */
    virtual void Start( TRequestStatus& aStatus ) = 0;

    /**
     * Stop AWS.
     *
     * @param aStatus Status of the calling active object. On successful
     *                completion contains KErrNone, otherwise one of the
     *                system-wide error codes.
     *                
     * @since S60 v5.2
     */
    virtual void Stop( TRequestStatus& aStatus ) = 0;
    
    /**
     * Set Power save mode.
     *
     * @param aMode new mode to be taken into use
     * @param aStatus Status of the calling active object. On successful
     *                completion contains KErrNone, otherwise one of the
     *                system-wide error codes.
     *                
     * @since S60 v5.2
     */
    virtual void SetPowerSaveMode( TAwsPsMode aMode, TRequestStatus& aStatus ) = 0;

    };

/**
 *  @brief Interface definition for AWS background scan provider.
 *
 *  This class defines the methods AWS uses to command background scan provider.
 *  
 *  @since S60 v5.2
 */
class MAwsBgScanProvider
    {

public:

    /**
     * Set new background scan interval.
     *
     * @since S60 v5.2
     * @param aNewInterval new interval in seconds to be taken into use
     * @param aStatus Status of the calling active object. On successful
     *                completion contains KErrNone, otherwise one of the
     *                system-wide error codes.
     */
    virtual void SetInterval( TUint32 aNewInterval, TRequestStatus& aStatus ) = 0;

    };


#endif // AWSINTERFACE_H
