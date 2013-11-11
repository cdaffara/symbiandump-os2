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

#ifndef MEMSPYDRIVERPANICS_H
#define MEMSPYDRIVERPANICS_H

// System includes
#include <e32cmn.h>


/**
 * Mem spy driver client panic category
 */
_LIT( KMemSpyClientPanic, "MemSpyDriver" );


/**
 * Panic codes
 */
enum TMemSpyDriverPanic
	{
	EPanicBadDescriptor = 0,
	EPanicHeapWalkPending,
	EPanicHeapWalkNotInitialised,
    EPanicThreadsInProcessNotSuspended,
    EPanicWrongProcessSuspended,
    EPanicAttemptingToSuspendMultipleProcesses,
    EPanicAttemptingToResumeNonSuspendedProcess,
    EPanicHeapInfoNotYetSeeded,
    EPanicHeapChunkAlreadyCloned,
    EPanicForcedKill,
    EPanicForcedTerminate,
    EPanicForcedPanic,
    EPanicKernelHeapDataInitError,
    EPanicKernelHeapDataFetchError,
    EPanicHeapFreeCellStreamNotClosed,
    EPanicHeapFreeCellStreamNotOpen,
    EPanicHeapKernelCopyExpected,
	};


	
#endif
