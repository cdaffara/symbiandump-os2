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
* Description:
*
*/

#include "MemSpyDriverDevice.h"

// System includes
#include <memspy/driver/memspydriverconstants.h>

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverEventMonitor.h"
#include "MemSpyDriverLogicalChannel.h"
#include "MemSpyDriverSuspensionManager.h"
#include "MemSpyDriverInspectedProcessManager.h"



DMemSpyDriverDevice::DMemSpyDriverDevice()
	{
	iVersion = KMemSpyDriverVersion();
	}


DMemSpyDriverDevice::~DMemSpyDriverDevice()
    {
	TRACE( Kern::Printf("DMemSpyDriverDevice::~DMemSpyDriverDevice() - START"));

    Cleanup();
    
    TRACE( Kern::Printf("DMemSpyDriverDevice::~DMemSpyDriverDevice() - END"));
    }


TInt DMemSpyDriverDevice::Install()
	{
	TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - START"));
    TInt error = KErrNone;
    //
	TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - creating event monitor..."));
	iEventMonitor = new DMemSpyEventMonitor();
	//
	if	( iEventMonitor != NULL )
    	{
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - constructing event monitor..."));
		error = iEventMonitor->Create( this );
		//
		if	( error == KErrNone )
			{
	        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - starting event monitor..."));
			error = iEventMonitor->Start();
			//
			if	( error == KErrNone )
				{
	            TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - creating process manager..."));
                iProcessManager = new DMemSpyInspectedProcessManager();
                //
                if  ( iProcessManager != NULL )
                    {
	                TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - constructing process manager..."));
                    error = iProcessManager->Create( this );
                    //
                    if  ( error == KErrNone )
                        {
	                    TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - creating process manager..."));
                        iSuspensionManager = new DMemSpySuspensionManager( *this );
                        //
                        if  ( iSuspensionManager != NULL )
                            {
	                        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - constructing process manager..."));
                            error = iSuspensionManager->Construct();
                            //
                            if  ( error == KErrNone )
                                {
	                            TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - creating os adaption..."));
                                iOSAdaption = new DMemSpyDriverOSAdaption( *this );
                                //
                                if  ( iOSAdaption != NULL )
                                    {
	                                TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - constructing os adaption..."));
                                    error = iOSAdaption->Construct();
                                    //
                                    if  ( error == KErrNone )
                                        {
                                        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - setting name..."));
				                        error = SetName( &KMemSpyDriverDeviceName );
                                        }
                                    }
                                }
                            }
                        }
                    }
				}
			}
        }
    
    // Handle errors
    if  ( error != KErrNone )
        {
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - error: %d", error ));
        if  ( iProcessManager )
            {
	        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - closing PM" ));
            iProcessManager->Close( NULL );
            iProcessManager = NULL;
            }
        if  ( iEventMonitor )
            {
	        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - stopping EM" ));
            iEventMonitor->Stop();
	        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - closing EM" ));
            iEventMonitor->Close();
	        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - deleting EM" ));
            iEventMonitor = NULL;
            }
        if  ( iSuspensionManager )
            {
	        TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - deleting SM" ));
            delete iSuspensionManager;
            iSuspensionManager = NULL;
            }
        }
    //
	TRACE( Kern::Printf("DMemSpyDriverDevice::Install() - END - error: %d", error ));
	return error;
	}


void DMemSpyDriverDevice::GetCaps( TDes8& /*aDes*/ ) const
	{
	}


TInt DMemSpyDriverDevice::Create( DLogicalChannelBase*& aChannel )
	{
    TRACE( Kern::Printf("DMemSpyDriverDevice::Create() - START"));
    TInt r = KErrNoMemory;
    //
	aChannel = new DMemSpyDriverLogicalChannel;
    //
    if  ( aChannel )
        {
        if  ( iProcessManager == NULL && iEventMonitor == NULL && iSuspensionManager == NULL )
            {
            TRACE( Kern::Printf("DMemSpyDriverDevice::Create() - need to re-create objects, calling Install()"));
            r = Install();
            TRACE( Kern::Printf("DMemSpyDriverDevice::Create() - called Install()"));
            }
        else
            {
            r = KErrNone;
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverDevice::Create() - END - r: %d", r ));
	return r;
	}


void DMemSpyDriverDevice::Cleanup()
    {

    NKern::ThreadEnterCS();
    if  ( iProcessManager )
        {
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - destroying process manager..."));
        iProcessManager->Close( NULL );
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - process manager destroyed"));
        iProcessManager = NULL;
        }
    
    if  ( iEventMonitor )
        {
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - destroying event monitor..."));
        iEventMonitor->Stop();
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - stopped event monitor..."));
        iEventMonitor->Close();
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - closed event monitor..."));
        iEventMonitor = NULL;
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - event monitor destroyed"));
        }
    
    if  ( iSuspensionManager )
        {
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - destroying suspension manager..."));
        delete iSuspensionManager;
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - suspension manager destroyed"));
        iSuspensionManager = NULL;
        }

    if  ( iOSAdaption )
        {
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - destroying os adaption..."));
        delete iOSAdaption;
	    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - suspension os adaption destroyed"));
        iOSAdaption = NULL;
        }

    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf("DMemSpyDriverDevice::Cleanup() - END"));
    }




