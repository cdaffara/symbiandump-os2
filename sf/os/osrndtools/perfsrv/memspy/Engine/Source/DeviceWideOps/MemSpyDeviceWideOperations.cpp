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
#include <memspy/engine/memspydevicewideoperations.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperstack.h>
#include <memspy/engine/memspyenginehelperthread.h>
#include <memspy/engine/memspyenginehelperprocess.h>

// Driver includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// Literal constants
_LIT( KMemSpyUiThreadNameKernel, "Kernel" );
_LIT( KMemSpyUiCompactHeap, "Compact Heap" );
_LIT( KMemSpyUiCompactStack, "Compact Stack" );



CMemSpyDeviceWideOperations::CMemSpyDeviceWideOperations( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationsObserver& aObserver, TOperation aOperation )
:   CActive( EPriorityIdle ), iEngine( aEngine ), iObserver( aObserver ), iOperation( aOperation )
    {
    CActiveScheduler::Add( this );
    }


EXPORT_C CMemSpyDeviceWideOperations::~CMemSpyDeviceWideOperations()
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::~CMemSpyDeviceWideOperations() - START - iOperationCancelled: %d", iOperationCancelled );
#endif

    Cancel();

#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::~CMemSpyDeviceWideOperations() - END" );
#endif
    }


void CMemSpyDeviceWideOperations::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::ConstructL() - START" );
#endif

    // Set engine sink time stamp, which will cause the data to be written to a 
    // uniquely named folder (if we're using a file sink)
    TTime now;
    now.HomeTime();
    iEngine.Sink().DataStreamTimeStampBeginL( now );

    const TInt processCountUser = iEngine.Container().Count();
    const TInt threadCountUser = TotalNumberOfThreads();
    const TInt threadCountIncludingKernelSupervisor = threadCountUser + 1; // includes kernel supervisor thread

    // Decide what the maximum progress bar value is...
    // If we're performing a detailed operation, then skip straight to the end because
    // the entire operation is performed in one go.
    iTotalOperationSize = 0;
    switch( iOperation )
        {
    // Only applicable to user-threads
    default:
    case EPerEntityGeneralSummary:
    case EPerEntityGeneralDetailed:
    case EPerEntityGeneralHandles:
    case EPerEntityHeapCellListing:
    case EPerEntityStackInfo:
    case EPerEntityStackDataUser:
    case EPerEntityStackDataKernel:
        iTotalOperationSize = threadCountUser;
        break;
    // Applies to user and kernel threads
    case EPerEntityHeapInfo:
        iTotalOperationSize = threadCountIncludingKernelSupervisor;
        break;
    // Operation is performed in one go
    case EEntireDeviceHeapInfoCompact:
    case EEntireDeviceStackInfoCompact:
        iProcessIndex = processCountUser;
        iTotalOperationSize = 1;
        break;
        }

#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::ConstructL() - op. count: %d", iTotalOperationSize );
#endif

    // Report total amount of work to observer
    iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationSized, iTotalOperationSize, KNullDesC );

    // Start the process rolling...
    CompleteSelf( KErrNone );

    // Indicate that we're now starting the operation
    iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationStarting, 0, KNullDesC );

#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::ConstructL() - END" );
#endif
    }


EXPORT_C CMemSpyDeviceWideOperations* CMemSpyDeviceWideOperations::NewL( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationsObserver& aObserver, TOperation aOperation )
    {
    CMemSpyDeviceWideOperations* self = new(ELeave) CMemSpyDeviceWideOperations( aEngine, aObserver, aOperation );
    CleanupStack::PushL( self );
    self->ConstructL();
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::NewL() - about to pop..." );
#endif
    CleanupStack::Pop( self );
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::NewL() - popped" );
#endif
    return self;
    }


EXPORT_C void CMemSpyDeviceWideOperations::Cancel()
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::Cancel() - START - IsActive: %d", IsActive() );
#endif

    if  ( IsActive() )
        {
        CActive::Cancel();
        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationCancelled, 0, KNullDesC );
        }

#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::Cancel() - END" );
#endif
    }


EXPORT_C TInt CMemSpyDeviceWideOperations::TotalOperationSize() const
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::TotalOperationSize::Cancel() - ret: %d", iTotalOperationSize );
#endif
    return iTotalOperationSize;
    }


void CMemSpyDeviceWideOperations::RunL()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::RunL() - START - iStatus: %d, iOperationCancelled: %d, iThreadIndex: %03d, iOperation: %d", iStatus.Int(), iOperationCancelled, iThreadIndex, iOperation );
#endif

    User::LeaveIfError( iStatus.Int() );
    User::ResetInactivityTime();
 
    if  ( iOperationCancelled )
        {
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyDeviceWideOperations::RunL() - operation was cancelled whilst running... => Finished" );
#endif
        SetFinished();
        }
    else
        {
        // Get the current process
        CMemSpyEngineObjectContainer& container = iEngine.Container();
        const TInt processCount = container.Count();
        //
        if  ( iProcessIndex < processCount ) 
            {
            PerformNextStepL();
            }
        else
            {
            // We're done - RunL will not be called again
            TRAP_IGNORE( PerformFinalOperationL() );
            SetFinished();
            }
        }

#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::RunL() - END" );
#endif
    }


void CMemSpyDeviceWideOperations::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::DoCancel() - START" );
#endif

    // Nothing to do here
    iOperationCancelled = ETrue;

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::DoCancel() - END" );
#endif
    }


TInt CMemSpyDeviceWideOperations::RunError( TInt aError )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::RunError() - START - iOperationCancelled: %d, aError: %d", iOperationCancelled, aError );
#endif

    // KErrNotFound can come when trying to suspend a process or thread that no longer exists.
    if  ( !( aError == KErrDied || aError == KErrNotFound ) )
        {
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyDeviceWideOperations::RunError() - fatal error - cancelling..." );
#endif       
        Cancel();
        }
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::RunError() - END - iOperationCancelled: %d, aError: %d", iOperationCancelled, aError );
#endif
    return KErrNone;
    }


void CMemSpyDeviceWideOperations::CompleteSelf( TInt aError )
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );
    SetActive();
    }


void CMemSpyDeviceWideOperations::PerformFinalOperationL()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::PerformFinalOperationL() - START" );
#endif

    iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationCompleting, 0, KNullDesC );
 
    // Carry out any remaining final one-shot operation
    TPtrC pType( KNullDesC );
    switch( iOperation )
        {
    case EEntireDeviceHeapInfoCompact:
        // Entire operation is performed here
        pType.Set( KMemSpyUiCompactHeap );
        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressStart, 0, pType );
        iEngine.HelperHeap().OutputHeapInfoForDeviceL();
        break;
    case EEntireDeviceStackInfoCompact:
        // Entire operation is performed here
        pType.Set( KMemSpyUiCompactStack );
        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressStart, 0, pType );
        iEngine.HelperStack().OutputStackInfoForDeviceL();
        break;
    case EPerEntityHeapInfo:
        // Complete op by outputting kernel heap summary
        pType.Set( KMemSpyUiThreadNameKernel );
        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressStart, 0, pType );
        iEngine.HelperHeap().OutputHeapInfoKernelL();
        break;
    case EPerEntityHeapData:
        // Complete op by outputting kernel heap data
        // TODO: Uncomment after kernel heap dump is fixed
//        pType.Set( KMemSpyUiThreadNameKernel );
//        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressStart, 0, pType );
//        iEngine.HelperHeap().OutputHeapDataKernelL();
        break;
    default:
        break;
        }

    // Report progress
    iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressEnd, 1, pType );

#ifdef _DEBUG
    RDebug::Print( _L("CMemSpyDeviceWideOperations::PerformFinalOperationL() - END - pType: %S"), &pType );
#endif
    }


void CMemSpyDeviceWideOperations::PerformNextStepL()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::PerformNextStepL() - START - iProcessIndex: %d, iThreadIndex: %d", iProcessIndex, iThreadIndex );
#endif

    // Get the current process
    CMemSpyEngineObjectContainer& container = iEngine.Container();
    const TInt processCount = container.Count();
    CMemSpyProcess& process = container.At( iProcessIndex );
    const TInt threadCount = process.Count();
    
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::PerformNextStepL() - threadCount: %d, processCount: %d", threadCount, processCount );
#endif

    // Get current thread
    if  ( iThreadIndex < threadCount )
        {
        CMemSpyThread& thread = process.At( iThreadIndex++ );
        const TPtrC pName( thread.Name() );
    
#ifdef _DEBUG
        RDebug::Print( _L("CMemSpyDeviceWideOperations::PerformNextStepL() - thread: %S"), &pName );
#endif

        // Report progress
        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressStart, 0, pName );

        TInt progressAmount = 1;
        TRAP_IGNORE(

            switch(iOperation)
                {
            case EPerEntityGeneralSummary:
            case EPerEntityGeneralDetailed:
                if  ( iOperation == EPerEntityGeneralSummary )
                    {
                    iEngine.HelperProcess().OutputProcessInfoL( process );
                    }
                else
                    {
                    iEngine.HelperProcess().OutputProcessInfoDetailedL( process );
                    }

                // Process specific item, so do this only once per perocess
                progressAmount = threadCount; 
                iThreadIndex = threadCount;
                break;
            case EPerEntityGeneralHandles:
                thread.InfoContainerForceSyncronousConstructionL().PrintL();
                break;
            case EPerEntityHeapInfo:
                // Output user thread summary here, kernel thread summary will be
                // handled when dialog dismissed
                iEngine.HelperHeap().OutputHeapInfoUserL( thread );
                break;
            case EPerEntityHeapCellListing:
                iEngine.HelperHeap().OutputCellListingUserL( thread );
                break;
            case EPerEntityHeapData:
                iEngine.HelperHeap().OutputHeapDataUserL( thread );
                break;
            case EPerEntityStackInfo:
                iEngine.HelperStack().OutputStackInfoL( thread );
                break;
            case EPerEntityStackDataUser:
                iEngine.HelperStack().OutputStackDataL( thread, EMemSpyDriverDomainUser );
                break;
            case EPerEntityStackDataKernel:
                iEngine.HelperStack().OutputStackDataL( thread, EMemSpyDriverDomainKernel );
                break;
            default:
                break;
                }
            );

        // Report progress
        iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationProgressEnd, progressAmount, pName );
        }
    else
        {
        // This process is exhausted - move on to the next
        ++iProcessIndex;
        iThreadIndex = 0;
 
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyDeviceWideOperations::PerformNextStepL() - move to next process..." );
#endif
        }

    // Request RunL be called again...
    CompleteSelf( KErrNone );

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyDeviceWideOperations::PerformNextStepL() - END" );
#endif
    }


void CMemSpyDeviceWideOperations::SetFinished()
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::SetFinished() - START" );
#endif

    // Cancel sink time stamp
    iEngine.Sink().DataStreamTimeStampEnd();

    iObserver.HandleDeviceWideOperationEvent( MMemSpyDeviceWideOperationsObserver::EOperationCompleted, 0, KNullDesC );

#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::SetFinished() - END" );
#endif
    }


TInt CMemSpyDeviceWideOperations::TotalNumberOfThreads() const
    {
    TInt count = 0;
    //
    const CMemSpyEngineObjectContainer& container = iEngine.Container();
    const TInt processCount = container.Count();
    //
    for(TInt i=0; i<processCount; i++)
        {
        const CMemSpyProcess& process = container.At( i );
        count += process.Count();
        }
    //
#ifdef _DEBUG
    RDebug::Printf("CMemSpyDeviceWideOperations::TotalNumberOfThreads() - count: %d", count );
#endif
    return count;
    }








