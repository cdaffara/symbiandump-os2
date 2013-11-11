/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "msmmterminator.h"
#include "eventqueue.h"

#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmterminatorTraces.h"
#endif


const TInt KShutdownDelay = 2000000; // approx 2 seconds
const TInt KMsmmTerminatorPriority = CActive::EPriorityStandard;

CMsmmTerminator* CMsmmTerminator::NewL(const CDeviceEventQueue& anEventQueue)
    {
    OstTraceFunctionEntry0( CMSMMTERMINATOR_NEWL_ENTRY );
    
    CMsmmTerminator* self = new (ELeave) CMsmmTerminator(anEventQueue);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CMSMMTERMINATOR_NEWL_EXIT );
    return self;
    }

void CMsmmTerminator::Start()
    {
    OstTraceFunctionEntry0( CMSMMTERMINATOR_START_ENTRY );
    
    After(KShutdownDelay);
    OstTraceFunctionExit0( CMSMMTERMINATOR_START_EXIT );
    }

void CMsmmTerminator::RunL()
    {
    OstTraceFunctionEntry0( CMSMMTERMINATOR_RUNL_ENTRY );
    
    if (iEventQueue.Count())
        {
        // There are some events still in the event queue to 
        // wait to be handled. Restart the shutdown timer.
        Start();
        }
    else
        {
        CActiveScheduler::Stop();
        }
    OstTraceFunctionExit0( CMSMMTERMINATOR_RUNL_EXIT );
    }

CMsmmTerminator::CMsmmTerminator(const CDeviceEventQueue& anEventQueue):
CTimer(KMsmmTerminatorPriority),
iEventQueue(anEventQueue)
    {
    OstTraceFunctionEntry0( CMSMMTERMINATOR_CMSMMTERMINATOR_CONS_ENTRY );
    
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( CMSMMTERMINATOR_CMSMMTERMINATOR_CONS_EXIT );
    }

void CMsmmTerminator::ConstructL()
    {
    OstTraceFunctionEntry0( CMSMMTERMINATOR_CONSTRUCTL_ENTRY );
    
    CTimer::ConstructL();
    OstTraceFunctionExit0( CMSMMTERMINATOR_CONSTRUCTL_EXIT );
    }

// End of file
