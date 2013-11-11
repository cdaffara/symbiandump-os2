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

#ifndef MEMSPYDRIVERINSPECTEDPROCESSMANAGER_H
#define MEMSPYDRIVERINSPECTEDPROCESSMANAGER_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverEventMonitor.h"

// Classes referenced
class DMemSpyDriverDevice;
class DMemSpyInspectedProcess;


NONSHARABLE_CLASS(DMemSpyInspectedProcessManager) : public DObject, public MMemSpyEventMonitorObserver
	{
public:
	DMemSpyInspectedProcessManager();
	~DMemSpyInspectedProcessManager();
    TInt Create( DMemSpyDriverDevice* aDevice );

public: // API
    TInt ProcessOpen( DProcess* aProcess );
    TInt ProcessClose( DProcess* aProcess );
    TInt ProcessCount() const;
    //
    DMemSpyInspectedProcess* InspectedProcessByProcessId( TUint aProcessId );
    void AutoStartListReset();
    TInt AutoStartListAdd( TUint aSID );

private: // Event handler queue methods
    void FreeAllInspectedProcesses();
    DMemSpyEventMonitor& EventMonitor();

private: // From MMemSpyEventMonitorObserver
    TUint EMTypeMask() const;
    void EMHandleProcessAdd( DProcess& aProcess );

private:
    DMemSpyDriverDevice* iDevice;
	SDblQue iMonitoredProcesses;
	
	// So that MemSpy can be informed when one of the observed processes changed
	DThread* iObserverThread;
	TRequestStatus* iObserverRS;
    RArray<TUint> iAutoStartSIDs;
	};


#endif
