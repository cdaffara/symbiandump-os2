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
* Description:  Header file for Stylus Tap indicator eject usb drives
*
*/


#ifndef MSMMDISMOUNTUSBDRIVES_H
#define MSMMDISMOUNTUSBDRIVES_H

#include <e32base.h> 
#ifdef __OVER_DUMMYCOMPONENT__
#include "usb/hostms/dummycomponent/dummyfsclient.h"
#else
#include <f32file.h> 
#endif

#include "msmm_pub_def.h"

class CDismountTimer;
class CMsmmPolicyPluginBase;

/**
 * Timer callback interface to indicate USB drive in use
 *
 * This class specifies the function to be called when a timeout occurs.
 * Used in conjunction with CDismountTimer class
 *  
 */    
class MTimerNotifier
    {
public:
    /**     
     * The function to be called when a timeout occurs.
     *     
     */
    virtual void TimerExpired() = 0;
    };

/**
 *  An Active Object class to request dismount of usb specific drives and notify the result to MSMM plugin 
 *
 */
NONSHARABLE_CLASS( CMsmmDismountUsbDrives ) : public CActive, 
                                            public MTimerNotifier
    {
public:
    virtual ~CMsmmDismountUsbDrives();
    static CMsmmDismountUsbDrives* NewL();
    static CMsmmDismountUsbDrives* NewLC();
    
public:
    /**
     * Send dismount notifications for all usb drives.
     */
    void DismountUsbDrives(CMsmmPolicyPluginBase& aPlugin, TUSBMSDeviceDescription& aDevice);
    
protected:
    CMsmmDismountUsbDrives();
    void ConstructL();
    
private:
    /**
     *  Send dismount notification via RFs
     */
    void DoDismount();
    
    /**
     * Callback to CMsmmPolicyPluginBase
     */
    void CompleteDismountRequest(const TInt aResult);

private: //from CActive    
    void RunL();
    void DoCancel();
    
private: // from MTimerNotifier

    /**
     * Dismount timer callback
     */     
    void TimerExpired();    
        
private:
    /**
     * Drive index
     */
    TInt iDriveIndex;
    /**
     * RFs session
     */
#ifdef __OVER_DUMMYCOMPONENT__
    RDummyFs  iRFs;
#else
    RFs  iRFs;
#endif
    /**
     * List of drives
     */
    TDriveList iDriveList;    
    
    /**
     * Dismount timer
     * Own
     */
    CDismountTimer* iDismountTimer;
    
    /**
     * Callback API for dismount result
     * Do not own
     */
    CMsmmPolicyPluginBase* iPlugin;
    
    /**
     * Device specific details, manufacturer and productid 
     */
    TPckgBuf<TUSBMSDeviceDescription> iDevicePkgInfo;
    };


/**
 * Timer interface for dismount request
 *
 * This class will notify an object after a specified timeout.
 * 
 */        
class CDismountTimer : public CTimer
    {
public:
    static CDismountTimer* NewL( MTimerNotifier* aTimeOutNotify );
    static CDismountTimer* NewLC( MTimerNotifier* aTimeOutNotify);
    virtual ~CDismountTimer();
    void CancelTimer();
    void StartTimer();

protected: 
    /**
     * From CTimer
     * Invoked when a timeout occurs
     *      
     */
    virtual void RunL();

private:
    CDismountTimer( MTimerNotifier* aTimeOutNotify );
    void ConstructL();

private: // Member variables

    /**
     *  The observer for this objects events 
     *  Not own.
     */
    MTimerNotifier* iNotify;
    };


#endif      // MSMMDISMOUNTUSBDRIVES_H
