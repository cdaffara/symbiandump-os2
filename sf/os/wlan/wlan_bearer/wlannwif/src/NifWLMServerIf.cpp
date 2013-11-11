/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implements interaction with WLMServer for Admission control purpose
*
*/

/*
* %version: 7 %
*/

#include "NifWLMServerIf.h"
#include "WlanProto.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::NewL
// -----------------------------------------------------------------------------
//
CLANNifWLMServerPerAC* CLANNifWLMServerPerAC::NewL(
    TWlmAccessClass aAccessClass,
    TUint aInactivityTime,
    TBool aIsAutomaticMgmt )
    {
    DEBUG( "CLANNifWLMServerPerAC::NewL()" );
    CLANNifWLMServerPerAC* self =
        new (ELeave) CLANNifWLMServerPerAC( aAccessClass, aInactivityTime, aIsAutomaticMgmt );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::CLANNifWLMServerPerAC
// -----------------------------------------------------------------------------
//
CLANNifWLMServerPerAC::CLANNifWLMServerPerAC(
    TWlmAccessClass aAccessClass,
    TUint aInactivityTime,
    TBool aIsAutomaticMgmt ):
    CActive( CActive::EPriorityStandard ),
    iAccessClass( aAccessClass ),
    iTrafficMode( EWlmAcTrafficModeAutomatic ),
    iTrafficStatus( EWlmAcTrafficStatusAdmitted ),
    iContext( ETSCreateRequest ),
    iIsTsCreated( EFalse ),
    iTsId( 0 ),
    iTsParams( KACtoUP[iAccessClass] ),
    iTsStatus( EWlanTrafficStreamStatusActive ),
    iTsInactivityTime( aInactivityTime ),
    iTsDelOrigTime( 0 ),
    iTsDelRemainTime( 0 ),
    iTsDelStartTime( 0 ),
    iIsAutomaticMgmt( aIsAutomaticMgmt )
    {
    DEBUG( "CLANNifWLMServerPerAC::CLANNifWLMServerPerAC()" );

    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::~CLANNifWLMServerPerAC
// -----------------------------------------------------------------------------
//
CLANNifWLMServerPerAC::~CLANNifWLMServerPerAC()
    {
    DEBUG( "CLANNifWLMServerPerAC::~CLANNifWLMServerPerAC()" );

    Cancel();
    iTsDelTimer.Close();
    iWlmServer.Close();
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::ConstructL
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::ConstructL()
    {
    DEBUG( "CLANNifWLMServerPerAC::ConstructL()" );

    User::LeaveIfError( iWlmServer.Connect() );
    User::LeaveIfError( iTsDelTimer.CreateLocal() );
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::IsAdmitted
// -----------------------------------------------------------------------------
//
TBool CLANNifWLMServerPerAC::IsAdmitted()
    {
    return ( iTrafficStatus == EWlmAcTrafficStatusAdmitted );
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::SetTrafficMode
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::SetTrafficMode(
    TWlmAcTrafficMode aMode )
    {
    iTrafficMode = aMode;

    if( aMode == EWlmAcTrafficModeAutomatic )
        {
        DEBUG1( "CLANNifWLMServerPerAC::SetTrafficMode() - traffic mode set to automatic for AC %u",
            iAccessClass );
        }
    else if( aMode == EWlmAcTrafficModeManual )
        {
        DEBUG1( "CLANNifWLMServerPerAC::SetTrafficMode() - traffic mode set to manual for AC %u",
            iAccessClass );

        // If mode is changed from automatic to manual, it means NIF's
        // traffic streams for this AC have been deleted.

        Cancel();
        iIsTsCreated = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::SetTrafficStatus
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::SetTrafficStatus(
    TWlmAcTrafficStatus aStatus )
    {
    iTrafficStatus = aStatus;

    if( aStatus == EWlmAcTrafficStatusAdmitted )
        {
        DEBUG1( "CLANNifWLMServerPerAC::SetTrafficStatus() - traffic admitted on AC %u",
            iAccessClass );
        }
    else if( aStatus == EWlmAcTrafficModeManual )
        {
        DEBUG1( "CLANNifWLMServerPerAC::SetTrafficStatus() - traffic NOT admitted on AC %u",
            iAccessClass );
        }    
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::SuspendInactivityTimer
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::SuspendInactivityTimer()
    {
    if( !iIsTsCreated )
        {
        DEBUG( "CLANNifWLMServerPerAC::SuspendInactivityTimer() - no traffic stream created, nothing to suspend" );

        return;        
        }    

    if( !IsActive() )
        {
        DEBUG( "CLANNifWLMServerPerAC::SuspendInactivityTimer() - timer is already suspended" );

        return;
        }

    DEBUG1( "CLANNifWLMServerPerAC::SuspendInactivityTimer() - AC: %u",
        iAccessClass );

    //First, cancel the running timer
    Cancel();

    TTime time;
    time.UniversalTime();
    TTimeIntervalMicroSeconds difference;
    difference = time.MicroSecondsFrom( iTsDelStartTime );
    iTsDelRemainTime = iTsDelRemainTime.Int() - difference.Int64();
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::ResumeInactivityTimer
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::ResumeInactivityTimer()
    {
    if( !iIsTsCreated )
        {
        DEBUG( "CLANNifWLMServerPerAC::ResumeInactivityTimer() - no traffic stream created, nothing to resume" );

        return;        
        }

    if( IsActive() )
        {
        DEBUG( "CLANNifWLMServerPerAC::ResumeInactivityTimer() - timer is already resumed" );

        return;
        }    

    DEBUG2( "CLANNifWLMServerPerAC::ResumeInactivityTimer() - AC: %u, remaining time(us): %u",
        iAccessClass, iTsDelRemainTime.Int() );

    //Remember the time the timer is started
    iTsDelStartTime.UniversalTime();
    iTsDelTimer.After(iStatus, iTsDelRemainTime);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::OnFrameSend
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::OnFrameSend()
    {
    DEBUG6( "CLanLinkCommon::OnFrameSend() - iAccessClass: %u, iTrafficStatus: %u, iTrafficMode: %u, iIsTsCreated: %u, iIsAutomaticMgmt: %u, IsActive(): %u",
        iAccessClass, iTrafficStatus, iTrafficMode, iIsTsCreated, iIsAutomaticMgmt, IsActive() );

    if( iIsAutomaticMgmt && 
        iTrafficStatus == EWlmAcTrafficStatusNotAdmitted &&
        iTrafficMode == EWlmAcTrafficModeAutomatic &&  
        !iIsTsCreated &&
        !IsActive() )
        {
        DEBUG1( "CLanLinkCommon::OnFrameSend() - requesting a virtual traffic stream for AC %u",
            iAccessClass );

        iWlmServer.CreateTrafficStream(
            iStatus,
            iTsParams,
            ETrue,
            iTsId,
            iTsStatus );
        iContext = ETSCreateRequest;
        SetActive();
        }
    else if( iIsTsCreated )
        {
        // Refresh the inactivity timer.

        Cancel();
        iTsDelRemainTime = iTsDelOrigTime;
        iTsDelStartTime.UniversalTime();
        iTsDelTimer.After( iStatus, iTsDelOrigTime );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::OnFrameReceive
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::OnFrameReceive()
    {
    DEBUG6( "CLanLinkCommon::OnFrameReceive() - iAccessClass: %u, iTrafficStatus: %u, iTrafficMode: %u, iIsTsCreated: %u, iIsAutomaticMgmt: %u, IsActive(): %u",
        iAccessClass, iTrafficStatus, iTrafficMode, iIsTsCreated, iIsAutomaticMgmt, IsActive() );
    
    if( iIsTsCreated &&
        IsActive() )
        {
        // Refresh the inactivity timer.

        Cancel();
        iTsDelRemainTime = iTsDelOrigTime;
        iTsDelStartTime.UniversalTime();
        iTsDelTimer.After( iStatus, iTsDelOrigTime );
        SetActive();
        }    
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::RunL
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::RunL()
    {
    DEBUG1( "CLANNifWLMServerPerAC::RunL() - iStatus: %d",
        iStatus.Int() );

    if( iContext == ETSCreateRequest )
        {
        if( iStatus.Int() == KErrNone )
            {
            DEBUG1("CLANNifWLMServerPerAC::RunL() - virtual traffic stream successfully created for AC %u",
                iAccessClass );
            
            iIsTsCreated = ETrue;

            // Start the inactivity timer.
            iTsDelOrigTime = iTsDelRemainTime = iTsInactivityTime;
            iTsDelStartTime.UniversalTime();
            iContext = ETSDelTimer;
            iTsDelTimer.After( iStatus, iTsDelOrigTime );
            SetActive();
            }
        else
            {
            DEBUG2("CLANNifWLMServerPerAC::RunL() - virtual traffic stream creation for AC %u failed with %d",
                iAccessClass, iStatus.Int() );

            iIsTsCreated = EFalse;
            }
        }
    else if( iContext == ETSDelTimer )
        {
        DEBUG1( "CLANNifWLMServerPerAC::RunL() - inactivity timer expired for AC %u, deleting the virtual traffic stream",
            iAccessClass );

        iWlmServer.DeleteTrafficStream( iStatus, iTsId );
        User::WaitForRequest( iStatus );

        iIsTsCreated = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CLANNifWLMServerPerAC::RunError
// ---------------------------------------------------------------------------
//
TInt CLANNifWLMServerPerAC::RunError(
    TInt /* aError */ )
    {
    DEBUG( "CLANNifWLMServerPerAC::RunError()" );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerPerAC::DoCancel
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerPerAC::DoCancel()    
    {
    if( iContext == ETSCreateRequest )
        {
        DEBUG1( "CLANNifWLMServerPerAC::DoCancel() - cancelling TS create request for AC %u",
            iAccessClass );

        iWlmServer.CancelCreateTrafficStream();
        }
    else
        {
        DEBUG1( "CLANNifWLMServerPerAC::DoCancel() - cancelling TS delete timer for AC %u",
            iAccessClass );

        iTsDelTimer.Cancel();
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLANNifWLMServerCommon::CLANNifWLMServerCommon
// -----------------------------------------------------------------------------
//
CLANNifWLMServerCommon::CLANNifWLMServerCommon(
    CLANLinkCommon* aLinkCommon ) :
    iLinkCommon( aLinkCommon )
    {
    DEBUG( "CLANNifWLMServerCommon::CLANNifWLMServerCommon()" );
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerCommon::~CLANNifWLMServerCommon
// -----------------------------------------------------------------------------
//
CLANNifWLMServerCommon::~CLANNifWLMServerCommon()
    {
    DEBUG( "CLANNifWLMServerCommon::~CLANNifWLMServerCommon" );

    iWlmServer.CancelNotifies();
    iWlmServer.Close();
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerCommon::ConstructL
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerCommon::ConstructL()
    {
    DEBUG( "CLANNifWLMServerCommon::ConstructL()" );

    User::LeaveIfError( iWlmServer.Connect() );
    iWlmServer.ActivateNotifiesL(
        *this,
        EWlmNotifyAcTrafficModeChanged | EWlmNotifyAcTrafficStatusChanged );
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerCommon::GetAcTrafficStatus
// -----------------------------------------------------------------------------
//
TInt CLANNifWLMServerCommon::GetAcTrafficStatus(
    TWlmAcTrafficStatusArray& aArray )    
    {
    DEBUG( "CLANNifWLMServerCommon::GetAcTrafficStatus()" );

    return iWlmServer.GetAcTrafficStatus( aArray );
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerCommon::AccessClassTrafficModeChanged
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerCommon::AccessClassTrafficModeChanged(
    TWlmAccessClass aAccessClass,
    TWlmAcTrafficMode aMode )
    {
    DEBUG( "CLANNifWLMServerCommon::AccessClassTrafficModeChanged()" );

    iLinkCommon->AcTrafficModeChanged(
        aAccessClass, aMode );
    }

// -----------------------------------------------------------------------------
// CLANNifWLMServerCommon::ConstructL
// -----------------------------------------------------------------------------
//
void CLANNifWLMServerCommon::AccessClassTrafficStatusChanged(
    TWlmAccessClass aAccessClass,
    TWlmAcTrafficStatus aStatus )
    {
    DEBUG( "CLANNifWLMServerCommon::AccessClassTrafficStatusChanged()" );

    iLinkCommon->AcTrafficStatusChanged(
        aAccessClass, aStatus );
    }
