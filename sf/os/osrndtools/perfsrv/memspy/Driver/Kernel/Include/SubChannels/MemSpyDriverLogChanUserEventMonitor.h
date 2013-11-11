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

#ifndef MEMSPYDRIVERLOGICALCHANUSEREVENTMONITOR_H
#define MEMSPYDRIVERLOGICALCHANUSEREVENTMONITOR_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class DMemSpyDriverClientEMManager;
class TMemSpyDriverInternalEventMonitorParams;


NONSHARABLE_CLASS(DMemSpyDriverLogChanUserEventMonitor) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanUserEventMonitor( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanUserEventMonitor();

public: // From DMemSpyDriverLogChanBase
    TInt Construct();
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt EventMonitorOpen( TAny* aHandle );
    TInt EventMonitorClose( TUint aHandle );
    TInt EventMonitorNotify( TMemSpyDriverInternalEventMonitorParams* aParams );
    TInt EventMonitorNotifyCancel( TUint aHandle );

private: // Data members
    DMemSpyDriverClientEMManager* iEventMonitorManager;
	};


#endif
