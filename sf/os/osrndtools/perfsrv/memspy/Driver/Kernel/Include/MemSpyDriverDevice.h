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

#ifndef MEMSPYDRIVERDEVICE_H
#define MEMSPYDRIVERDEVICE_H

// System includes
#include <kernel.h>

// Classes referenced
class DMemSpyEventMonitor;
class DMemSpyDriverOSAdaption;
class DMemSpySuspensionManager;
class DMemSpyInspectedProcessManager;

NONSHARABLE_CLASS(DMemSpyDriverDevice) : public DLogicalDevice
	{
public:
	DMemSpyDriverDevice();
    ~DMemSpyDriverDevice();

public: // from DLogicalDevice
	TInt Install();
	void GetCaps( TDes8& aDes ) const;
	TInt Create( DLogicalChannelBase*& aChannel );

public: // API
    void Cleanup();
    //
    inline TUint32 RHeapVTable() const { return iRHeapVTable; }
    inline void SetRHeapVTable( TUint32 aVTable ) { iRHeapVTable = aVTable; }
    //
    inline DMemSpyEventMonitor& EventMonitor() { return *iEventMonitor; }
    inline DMemSpyDriverOSAdaption& OSAdaption() { return *iOSAdaption; }
    inline DMemSpySuspensionManager& SuspensionManager() const { return *iSuspensionManager; }
    inline DMemSpyInspectedProcessManager& ProcessInspectionManager() { return *iProcessManager; }

private: // Data members
    TUint32 iRHeapVTable;
    DMemSpyEventMonitor* iEventMonitor;
    DMemSpyDriverOSAdaption* iOSAdaption;
    DMemSpySuspensionManager* iSuspensionManager;
    DMemSpyInspectedProcessManager* iProcessManager;
	};



#endif
