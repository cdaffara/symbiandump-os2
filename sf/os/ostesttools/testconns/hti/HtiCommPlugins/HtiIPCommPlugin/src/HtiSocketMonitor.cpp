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
* Description:  ECOM plugin for communication over IP port
*
*/


// INCLUDE FILES
#include "HtiSocketMonitor.h"
#include "HtiIPCommLogging.h"


CHtiSocketMonitor* CHtiSocketMonitor::NewL( MHtiSocketObserver::TRequestType aType, MHtiSocketObserver* aObserver )
    {
    CHtiSocketMonitor* self = new (ELeave) CHtiSocketMonitor( aType, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CHtiSocketMonitor::CHtiSocketMonitor( MHtiSocketObserver::TRequestType aType, MHtiSocketObserver* aObserver ):
    CActive( EPriorityStandard ),
    iType( aType ),
    iObserver( aObserver )
    {
    }

CHtiSocketMonitor::~CHtiSocketMonitor()
    {
    Cancel();
    }

void CHtiSocketMonitor::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

void CHtiSocketMonitor::RunL()
    {
    iObserver->ReportComplete( iType, iStatus.Int() );
    }

void CHtiSocketMonitor::DoCancel()
    {
    HTI_LOG_TEXT( "CHtiSocketMonitor::DoCancel called" );
    }

TInt CHtiSocketMonitor::RunError( TInt aError )
    {
    HTI_LOG_FORMAT( "CHtiSocketMonitor::RunError %d", aError );
    aError = aError;
    return KErrNone;
    }

void CHtiSocketMonitor::Activate()
    {
    HTI_LOG_FUNC_IN( "CHtiSocketMonitor::Activate" );

    if ( !IsAdded() )
        {
        HTI_LOG_TEXT( "Panic! HtiSocketMonitor not added" );
        User::Panic( _L("HtiSocketMonitor not added"), 1 );
        }

    if ( IsActive() )
        {
        HTI_LOG_TEXT( "Panic! HtiSocketMonitor cannot handle more than one request" );
        User::Panic( _L("HtiSocketMonitor cannot handle more than one request"), 1 );
        }

    SetActive();
    HTI_LOG_FUNC_OUT( "CHtiSocketMonitor::Activate" );
    }
