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

#include "ConsoleDWOperation.h"

// System includes
#include <e32debug.h>


CMemSpyDeviceWideOperationWaiter::CMemSpyDeviceWideOperationWaiter( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }


CMemSpyDeviceWideOperationWaiter::~CMemSpyDeviceWideOperationWaiter()
    {
#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::~CMemSpyDeviceWideOperationWaiter() - START" );
#endif

#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::~CMemSpyDeviceWideOperationWaiter() - deleting operation...: 0x%08x", iOperation );
#endif
    delete iOperation;

#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::~CMemSpyDeviceWideOperationWaiter() - deleting waiter...: 0x%08x", iWaiter );
#endif
    delete iWaiter;

#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::~CMemSpyDeviceWideOperationWaiter() - END" );
#endif
    }


void CMemSpyDeviceWideOperationWaiter::ExecuteL( CMemSpyDeviceWideOperations::TOperation aOperation )
    {
#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::ExecuteL() - START" );
#endif

    iWaiter = new(ELeave) CActiveSchedulerWait();
    //iOperation = CMemSpyDeviceWideOperations::NewL( iEngine, *this, aOperation );

#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::ExecuteL() - starting scheduler wait..." );
#endif

    iWaiter->Start();

#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::ExecuteL() - END" );
#endif
    }

void CMemSpyDeviceWideOperationWaiter::ExecuteLD( CMemSpyEngine& aEngine, CMemSpyDeviceWideOperations::TOperation aOperation )
    {
    CMemSpyDeviceWideOperationWaiter* self = new(ELeave) CMemSpyDeviceWideOperationWaiter( aEngine );
    CleanupStack::PushL( self );
    self->ExecuteL( aOperation );
    CleanupStack::PopAndDestroy( self );
    }


void CMemSpyDeviceWideOperationWaiter::StopWait()
    {
#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::StopWait() - START - iOperation: 0x%08x", iOperation );
#endif

    if  ( iWaiter->IsStarted() )
        {
        iWaiter->AsyncStop();
        }

#ifdef _DEBUG
    RDebug::Printf("[MCon] CMemSpyDeviceWideOperationWaiter::StopWait() - END" );
#endif
    }


void CMemSpyDeviceWideOperationWaiter::HandleDeviceWideOperationEvent( TEvent aEvent, TInt aParam1, const TDesC& aParam2 )
    {
#ifdef _DEBUG
    RDebug::Print( _L("[MCon] CMemSpyDeviceWideOperationWaiter::HandleDeviceWideOperationEvent() - START - aEvent: %d, aParam1: %d, aParam2: %S"), aEvent, aParam1, &aParam2 );
#else
    (void) aParam1;
    (void) aParam2;
#endif

    switch( aEvent )
        {
    case MMemSpyDeviceWideOperationsObserver::EOperationCompleted:
        StopWait();
        break;
    default:
    case MMemSpyDeviceWideOperationsObserver::EOperationSized:
    case MMemSpyDeviceWideOperationsObserver::EOperationStarting:
    case MMemSpyDeviceWideOperationsObserver::EOperationProgressStart:
    case MMemSpyDeviceWideOperationsObserver::EOperationProgressEnd:
    case MMemSpyDeviceWideOperationsObserver::EOperationCancelled:
    case MMemSpyDeviceWideOperationsObserver::EOperationCompleting:
        break;
        }

#ifdef _DEBUG
    RDebug::Print( _L("[MCon] CMemSpyDeviceWideOperationWaiter::HandleDeviceWideOperationEvent() - END - aEvent: %d, aParam1: %d, aParam2: %S"), aEvent, aParam1, &aParam2 );
#endif
    }




