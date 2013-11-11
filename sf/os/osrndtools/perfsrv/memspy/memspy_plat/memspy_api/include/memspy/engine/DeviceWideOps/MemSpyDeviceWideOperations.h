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

#ifndef MEMSPYDEVICEWIDEOPERATIONS_H
#define MEMSPYDEVICEWIDEOPERATIONS_H

// System includes
#include <e32base.h>

// Classes referenced
class CMemSpyEngine;

class MMemSpyDeviceWideOperationsObserver
    {
public: // Enumerations
    enum TEvent
        {
        // Indicates we've worked out how much work is required
        EOperationSized = 0,

        // Indicates that we are starting the requested operation
        EOperationStarting,

        // Indicates some work has been done
        EOperationProgressStart,
        EOperationProgressEnd,

        // Indicates that the cancellation request has been acknowledged
        EOperationCancelled,

        // Indicates that we are entering the completion phase
        EOperationCompleting,

        // Indicates that the operation is complete and this object can be destroyed now
        EOperationCompleted
        };

public: // From MMemSpyDeviceWideOperationsObserver
    virtual void HandleDeviceWideOperationEvent( TEvent aEvent, TInt aParam1, const TDesC& aParam2 ) = 0;
    };


NONSHARABLE_CLASS( CMemSpyDeviceWideOperations ) : public CActive
    {
public:
    enum TOperation
        {
        // Brief overview of user thread
        EPerEntityGeneralSummary = 0,

        // Detailed information about user thread
        EPerEntityGeneralDetailed,

        // Thread handles for user thread (dumps thread info container)
        EPerEntityGeneralHandles,
        
        // Heap information for user or kernel thread
        EPerEntityHeapInfo,

        // Compact heap information for entire device (kernel & user threads)
        EEntireDeviceHeapInfoCompact,

        // Heap cell listings for specific user thread
        EPerEntityHeapCellListing,
        
        // Heap data for kernel or user threads
        EPerEntityHeapData,
        
        // Summary stack info
        EPerEntityStackInfo,

        // Compact stack information for the entire device (user threads only)
        EEntireDeviceStackInfoCompact,
        
        // User-side stack for user thread
        EPerEntityStackDataUser,

        // Supervisor-side stack for user thread
        EPerEntityStackDataKernel,
        };

public:
    IMPORT_C static CMemSpyDeviceWideOperations* NewL( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationsObserver& aObserver, TOperation aOperation );
    IMPORT_C ~CMemSpyDeviceWideOperations();

private:
    CMemSpyDeviceWideOperations( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationsObserver& aObserver, TOperation aOperation );
    void ConstructL();

public: // API
    IMPORT_C void Cancel();
    IMPORT_C TInt TotalOperationSize() const;

private: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private: // Internal methods
    void CompleteSelf( TInt aError );
    void PerformNextStepL();
    void PerformFinalOperationL();
    void SetFinished();
    TInt TotalNumberOfThreads() const;

private: // Member data
    CMemSpyEngine& iEngine;
    MMemSpyDeviceWideOperationsObserver& iObserver;
    TOperation iOperation;
    TInt iProcessIndex;
    TInt iThreadIndex;
    TBool iOperationCancelled;
    TInt iTotalOperationSize;
    };


#endif
