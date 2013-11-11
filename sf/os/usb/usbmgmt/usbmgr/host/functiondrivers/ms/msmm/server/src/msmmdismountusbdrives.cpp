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
* Description:  Active Object to dismount usb drives
*
*/


#include <usb/usblogger.h>
#include <usb/hostms/srverr.h>
#include <usb/hostms/msmmpolicypluginbase.h>

#include "msmmdismountusbdrives.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmdismountusbdrivesTraces.h"
#endif


const TInt KDismountTimeOut   = 6000000; // 6 seconds


CMsmmDismountUsbDrives::~CMsmmDismountUsbDrives()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_CMSMMDISMOUNTUSBDRIVES_DES_ENTRY );
    
    Cancel(); 
    delete iDismountTimer;    
    iRFs.Close();    
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_CMSMMDISMOUNTUSBDRIVES_DES_EXIT );
    }

/**
 * Symbian two phase constructor
 */
CMsmmDismountUsbDrives* CMsmmDismountUsbDrives::NewL()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_NEWL_ENTRY );
    
    CMsmmDismountUsbDrives* self = CMsmmDismountUsbDrives::NewLC();
    CleanupStack::Pop(self);    
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_NEWL_EXIT );
    return self;
    }

/**
 * Symbian two phase constructor. Object pushed to cleanup stack
 */
CMsmmDismountUsbDrives* CMsmmDismountUsbDrives::NewLC()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_NEWLC_ENTRY );
    
    CMsmmDismountUsbDrives* self = new (ELeave) CMsmmDismountUsbDrives();
    CleanupStack::PushL(self);
    self->ConstructL();
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_NEWLC_EXIT );
    return self;
    }

/**
 * Check the status of current dismount request and continue issuing next if no error
 */
void CMsmmDismountUsbDrives::RunL()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_RUNL_ENTRY );
    
    iDismountTimer->CancelTimer();    
    
    // Indicates there has been an error dismounting a usb drive, report immediately to MSMM plugin and 
    // abort the process
    if ( iStatus != KErrNone )
        {
        CompleteDismountRequest( iStatus.Int() );
        }
    // Indicates we have reached the end of all usb drives dismounting, in other words a success condition
    else if ( iDriveIndex == KMaxDrives )
        {
        CompleteDismountRequest( KErrNone );
        }
    // We still have more drives to traverse
    else if ( iDriveIndex < KMaxDrives )
        {
        DoDismount();
        }
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_RUNL_EXIT );
    }

/**
 * Cancel pending notifier and those in queue 
 */
void CMsmmDismountUsbDrives::DoCancel()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_DOCANCEL_ENTRY );
    
    iRFs.NotifyDismountCancel(iStatus);
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_DOCANCEL_EXIT );
    }

CMsmmDismountUsbDrives::CMsmmDismountUsbDrives()
    : CActive(EPriorityStandard)
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_CMSMMDISMOUNTUSBDRIVES_ENTRY );
    
    CActiveScheduler::Add(this);    
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_CMSMMDISMOUNTUSBDRIVES_EXIT );
    }

void CMsmmDismountUsbDrives::ConstructL()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_CONSTRUCTL_ENTRY );
    
    User::LeaveIfError( iRFs.Connect());
    iDismountTimer = CDismountTimer::NewL(this);
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_CONSTRUCTL_EXIT );
    }

/**
 * Dismount usb drives
 */
void CMsmmDismountUsbDrives::DismountUsbDrives(CMsmmPolicyPluginBase& aPlugin, TUSBMSDeviceDescription& aDevice)
    {    
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_DISMOUNTUSBDRIVES_ENTRY );
    
    Cancel();
    iPlugin = &aPlugin;
    TUSBMSDeviceDescription& device = iDevicePkgInfo();
    device = aDevice;
    iDriveIndex = 0;
    iRFs.DriveList( iDriveList );
    DoDismount();
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_DISMOUNTUSBDRIVES_EXIT );
    }

/**
 * Callback to CMsmmPolicyPluginBase with either success or failure message
 */
void CMsmmDismountUsbDrives::CompleteDismountRequest(const TInt aResult)
    {
    THostMsErrData data;
    if( aResult == KErrNone )
        data.iError = EHostMsErrNone;
    else
        data.iError = EHostMsErrInUse;
    data.iE32Error = aResult;
    data.iManufacturerString = iDevicePkgInfo().iManufacturerString;
    data.iProductString = iDevicePkgInfo().iProductString;
    data.iDriveName = 0x0;
   
    TRAP_IGNORE(iPlugin->SendErrorNotificationL(data));
    }

/**
 * Dismount next usb drive
 */
void CMsmmDismountUsbDrives::DoDismount()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_DODISMOUNT_ENTRY );
       
    TDriveInfo info;
    TInt err = KErrNone;
    for ( ; iDriveIndex < KMaxDrives; iDriveIndex++ )
        {
        if ( iDriveList[iDriveIndex] )
            {
            err = iRFs.Drive( info , iDriveIndex );            
            if ( info.iConnectionBusType == EConnectionBusUsb &&                 
                 info.iDriveAtt & KDriveAttExternal && 
                 err == KErrNone  )
                {
                OstTrace0( TRACE_NORMAL, CMSMMDISMOUNTUSBDRIVES_DODISMOUNT, 
                        "CMsmmDismountUsbDrives::DoDismount Dismount notify request" );
                iRFs.NotifyDismount( iDriveIndex, iStatus, EFsDismountNotifyClients );                
                iDismountTimer->StartTimer();
                SetActive();
                OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_DODISMOUNT_EXIT );
                return;
                }                     
            }
        }
    // Indicates we have gone through all the drives and no more usb drives left to request dismount
    CompleteDismountRequest( KErrNone );
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_DODISMOUNT_EXIT_DUP1 );
    }


/**
 * Callback function from CDismountTimer after 6 seconds indicating a usb drive is not released by another process, report it as an error
 */
void CMsmmDismountUsbDrives::TimerExpired()
    {
    OstTraceFunctionEntry0( CMSMMDISMOUNTUSBDRIVES_TIMEREXPIRED_ENTRY );
    
    Cancel();
    iDismountTimer->CancelTimer();    
    CompleteDismountRequest( KErrInUse );    
    OstTraceFunctionExit0( CMSMMDISMOUNTUSBDRIVES_TIMEREXPIRED_EXIT );
    }    

//CDismountTimer

CDismountTimer* CDismountTimer::NewL( MTimerNotifier* aTimeOutNotify)
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_NEWL_ENTRY );
        
    CDismountTimer* self = CDismountTimer::NewLC( aTimeOutNotify );
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CDISMOUNTTIMER_NEWL_EXIT );
    return self;
    }

CDismountTimer* CDismountTimer::NewLC( MTimerNotifier* aTimeOutNotify )
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_NEWLC_ENTRY );
        
    CDismountTimer* self = new (ELeave) CDismountTimer( aTimeOutNotify );
    CleanupStack::PushL(self);
    self->ConstructL();
    OstTraceFunctionExit0( CDISMOUNTTIMER_NEWLC_EXIT );
    return self;
    }

CDismountTimer::CDismountTimer( MTimerNotifier* aTimeOutNotify):
    CTimer(EPriorityStandard), 
    iNotify(aTimeOutNotify)    
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_CDISMOUNTTIMER_CONS_ENTRY );
    }    

CDismountTimer::~CDismountTimer()
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_CDISMOUNTTIMER_DES_ENTRY );
    
    Cancel();
    OstTraceFunctionExit0( CDISMOUNTTIMER_CDISMOUNTTIMER_DES_EXIT );
    }

void CDismountTimer::ConstructL()
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_CONSTRUCTL_ENTRY );
        
    if ( !iNotify )    
        {
        User::Leave(KErrArgument);    
        }
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( CDISMOUNTTIMER_CONSTRUCTL_EXIT );
    }

void CDismountTimer::RunL()
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_RUNL_ENTRY );
    
    // Timer request has completed, so notify the timer's owner
    iNotify->TimerExpired();
    OstTraceFunctionExit0( CDISMOUNTTIMER_RUNL_EXIT );
    }
void CDismountTimer::CancelTimer()
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_CANCELTIMER_ENTRY );
     
    Cancel();    
    OstTraceFunctionExit0( CDISMOUNTTIMER_CANCELTIMER_EXIT );
    }

void CDismountTimer::StartTimer()
    {
    OstTraceFunctionEntry0( CDISMOUNTTIMER_STARTTIMER_ENTRY );
    
    After( KDismountTimeOut );  
    OstTraceFunctionExit0( CDISMOUNTTIMER_STARTTIMER_EXIT );
    }
// End of File
