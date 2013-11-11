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

#include "MemSpyDriverLogChanBase.h"

// System includes
#include <u32hal.h>
#include <kernel.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverXferBuffer.h"



DMemSpyDriverLogChanBase::DMemSpyDriverLogChanBase( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   iDevice( aDevice ), iClientThread( aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::DMemSpyDriverLogChanBase() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanBase::~DMemSpyDriverLogChanBase()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::~DMemSpyDriverLogChanBase() - START - this: 0x%08x", this ));

    NKern::ThreadEnterCS();

    if  ( iTempObj != NULL )
        {
    	Kern::SafeClose( iTempObj, NULL );
        }

    delete iXferBuffer;
	
    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::~DMemSpyDriverLogChanBase() - END - this: 0x%08x", this ));
	}


TInt DMemSpyDriverLogChanBase::BaseConstruct( TInt aTransferBufferSize )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::BaseConstruct() - START - this: 0x%08x, aTransferBufferSize: %d", this, aTransferBufferSize ));
    TInt ret = KErrNone;
    //
    if  ( aTransferBufferSize != 0 )
        {
        iXferBuffer = new DMemSpyDriverXferBuffer( iDevice, iClientThread );
        if  ( !iXferBuffer )
            {
            ret = KErrNoMemory;
            }
        else
            {
            ret = iXferBuffer->Construct( aTransferBufferSize );
            if  ( ret != KErrNone )
                {
                delete iXferBuffer;
                iXferBuffer = NULL;
                }
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::BaseConstruct() - END - this: 0x%08x, aTransferBufferSize: %d, size: %d", this, aTransferBufferSize, iXferBuffer ? iXferBuffer->Size() : 0 ));
    return ret;
    }


TInt DMemSpyDriverLogChanBase::Construct()
    {
    const TInt baseErr = BaseConstruct();
    return baseErr;
    }


TInt DMemSpyDriverLogChanBase::Request( TInt /*aFunction*/, TAny* /*a1*/, TAny* /*a2*/ )
    {
    return KErrNone;
    }







DMemSpyDriverDevice& DMemSpyDriverLogChanBase::MemSpyDevice()
    {
    return iDevice;
    }


DMemSpySuspensionManager& DMemSpyDriverLogChanBase::SuspensionManager()
    {
    return MemSpyDevice().SuspensionManager();
    }










TInt DMemSpyDriverLogChanBase::OpenTempObject( TUint aId, TObjectType aType, TBool aAllowDeadObjects )
	{
	__ASSERT_DEBUG( aType == EProcess || aType == EThread, MemSpyDriverUtils::Fault( __LINE__ ) );
	__ASSERT_DEBUG( !iTempObj, MemSpyDriverUtils::Fault( __LINE__ ) );

	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - START - aId: %d, aType: %d, aAllowDeadObjects: %d", aId, aType, aAllowDeadObjects ));
	DObjectCon* pC = Kern::Containers()[ aType ];
	NKern::ThreadEnterCS();
	pC->Wait();
	iTempObj = (aType == EProcess) ? (DObject*) Kern::ProcessFromId( aId ) : (DObject*) Kern::ThreadFromId( aId );
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - kernel obj from id returned: 0x%08x %O", iTempObj, iTempObj ));
    //
    TBool openedOkay = EFalse;
    TInt r = KErrNone;
    //
	if ( iTempObj )
        {
		r = iTempObj->Open();
	    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - open returned: %d", r ));
		//
        if  ( r == KErrNone )
            {
            openedOkay = ETrue;
            if  ( aType == EProcess )
                {
                // Check the process is still alive
                DProcess* process = (DProcess*) iTempObj;
                const TExitType exitType = OSAdaption().DProcess().GetExitType( *process );
                TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - process exitType: %d", exitType ));

                if  ( !aAllowDeadObjects && exitType != EExitPending )
					{
                    r = KErrDied;
                    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - PROCESS IS DEAD!" ));
					}
                }
            else if ( aType == EThread )
                {
                // Check the thread is still alive
                DThread* thread = (DThread*) iTempObj;
                const TExitType exitType = OSAdaption().DThread().GetExitType( *thread );
                TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - thread exitType: %d", exitType ));

                if  ( !aAllowDeadObjects && exitType != EExitPending )
                    {
                    r = KErrDied;
                    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - THREAD IS DEAD!" ));
                    }
                }
            }

        }
    else if ( !iTempObj )
        {
        r = KErrNotFound;
	    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - ENTITY NOT FOUND!" ));
        }
    //
	pC->Signal();
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - signalled container..." ));
	NKern::ThreadLeaveCS();
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - left CS..." ));
    //
    if  ( r != KErrNone && iTempObj )
        {
	    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - ERROR CASE - closing temp object (r: %d, openedOkay: %d)...", r, openedOkay ));
        //
        if ( openedOkay )
            {
            CloseTempObject();
            }
        else
            {
            NKern::SafeSwap( NULL, (TAny*&) iTempObj );
            }
        //
	    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - ERROR CASE - closed temp object" ));
        }
    //
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenTempObject() - END - r: %d", r ));
	return r;
	}


void DMemSpyDriverLogChanBase::CloseTempObject()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanBase::CloseTempObject() - START - iTempObj: 0x%08x %O", iTempObj, iTempObj ));

	__ASSERT_DEBUG( iTempObj, MemSpyDriverUtils::Fault( __LINE__ ) );
    if  ( iTempObj )
        {
	    NKern::ThreadEnterCS();
        TRACE( Kern::Printf("DMemSpyDriverLogChanBase::CloseTempObject() - in CS..." ));
        //
	    Kern::SafeClose( iTempObj, NULL );
        TRACE( Kern::Printf("DMemSpyDriverLogChanBase::CloseTempObject() - done safe close..." ));
        //
	    NKern::ThreadLeaveCS();
        TRACE( Kern::Printf("DMemSpyDriverLogChanBase::CloseTempObject() - left CS" ));
        }

    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::CloseTempObject() - END" ));
	}


DThread& DMemSpyDriverLogChanBase::TempObjectAsThread()
    {
	__ASSERT_DEBUG( iTempObj, MemSpyDriverUtils::Fault( __LINE__ ) );
    DThread* ret = (DThread*) iTempObj;
    return *ret;
    }


DProcess& DMemSpyDriverLogChanBase::TempObjectAsProcess()
    {
	__ASSERT_DEBUG( iTempObj, MemSpyDriverUtils::Fault( __LINE__ ) );
    DProcess* ret = (DProcess*) iTempObj;
    return *ret;
    }
















RMemSpyMemStreamWriter DMemSpyDriverLogChanBase::OpenXferStream()
    {
	__ASSERT_ALWAYS( iXferBuffer, MemSpyDriverUtils::Fault( __LINE__ ) );
    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenXferStream() - this: 0x%08x, xferSize: %d", this, iXferBuffer->Size() ) );
    RMemSpyMemStreamWriter ret( *iXferBuffer );
    return ret;
    }


TInt DMemSpyDriverLogChanBase::OpenXferStream( RMemSpyMemStreamWriter& aWriter, TInt aRequiredSize )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenXferStream() - START - this: 0x%08x, xferSize: %d, aRequiredSize: %d", this, iXferBuffer->Size(), aRequiredSize ) );
    __ASSERT_ALWAYS( iXferBuffer, MemSpyDriverUtils::Fault( __LINE__ ) );
    
    TInt err = KErrNone;
    
    // Never make the buffer smaller - but we will make it bigger if needs be.
    if  ( aRequiredSize > iXferBuffer->Size() )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenXferStream() - Need to grow Xfer buffer..." ));
        err = iXferBuffer->EnsureCapacity( aRequiredSize );
        TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenXferStream() - grow result: %d", err ));
        }
    //
    if  ( err == KErrNone )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenXferStream() - creating writer... with xfer buffer size: %d", iXferBuffer->Size() ));
        aWriter = RMemSpyMemStreamWriter( *iXferBuffer );
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanBase::OpenXferStream() - END - this: 0x%08x, xferSize: %d, aRequiredSize: %d", this, iXferBuffer->Size(), aRequiredSize ) );
    return err;
    }





DMemSpyDriverOSAdaption& DMemSpyDriverLogChanBase::OSAdaption()
    {
    return iDevice.OSAdaption();
    }

