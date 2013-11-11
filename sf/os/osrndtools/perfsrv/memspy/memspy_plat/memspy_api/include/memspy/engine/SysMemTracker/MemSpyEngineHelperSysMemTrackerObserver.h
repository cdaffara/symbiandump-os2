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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKER_OBSERVER_H
#define MEMSPYENGINEHELPERSYSMEMTRACKER_OBSERVER_H

// System includes
#include <e32base.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Classes referenced
class CMemSpyEngineHelperSysMemTrackerCycle;



class MMemSpyEngineHelperSysMemTrackerObserver
    {
public: // From MMemSpyEngineHelperSysMemTrackerObserver
    virtual void HandleCyclesResetL() = 0;
    virtual void HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle ) = 0;
    virtual void HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle ) = 0;
    };



#endif