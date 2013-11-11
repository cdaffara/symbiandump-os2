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

#ifndef MEMSPYEVENTMONITOR_H
#define MEMSPYEVENTMONITOR_H

// System includes
#include <kernel.h>
#include <kern_priv.h>

// Classes referenced
class DMemSpyDriverDevice;

// For EMTypeMask
enum TMemSpyEventMonitorEvent
    {
    EMemSpyEventNull                = 0x0000,
    //
    EMemSpyEventThreadAdd           = 0x0001,
    EMemSpyEventThreadRemove        = 0x0002,
    EMemSpyEventThreadKill          = 0x0004,
    //
    EMemSpyEventProcessAdd          = 0x0008,
    EMemSpyEventProcessUpdate       = 0x0010,
    EMemSpyEventProcessRemove       = 0x0020,
    //
    EMemSpyEventChunkAdd            = 0x0040,
    EMemSpyEventChunkUpdate         = 0x0080,
    EMemSpyEventChunkDelete         = 0x0100,
    };


class MMemSpyEventMonitorObserver
	{
public: // From MMemSpyEventMonitorObserver
    virtual TUint EMTypeMask() const = 0;
    //
    virtual void EMHandleProcessAdd( DProcess& /*aProcess*/ ) { }
    virtual void EMHandleProcessUpdated( DProcess& /*aProcess*/ ) { }
    virtual void EMHandleProcessRemoved( DProcess& /*aProcess*/ ) { }
    //
    virtual void EMHandleThreadAdd( DThread& /*aThread*/ ) { }
    virtual void EMHandleThreadRemoved( DThread& /*aThread*/ ) { }
    virtual void EMHandleThreadKilled( DThread& /*aThread*/ ) { }
    //
    virtual void EMHandleChunkAdd( DChunk& /*aChunk*/ ) { }
    virtual void EMHandleChunkUpdated( DChunk& /*aChunk*/ ) { }
    virtual void EMHandleChunkDeleted( DChunk& /*aChunk*/ ) { }

public: // Nasty, but I don't care...
    SDblQueLink __iEMLink;
    };



NONSHARABLE_CLASS(DMemSpyEventMonitor) : public DKernelEventHandler
	{
public:
	DMemSpyEventMonitor();
	~DMemSpyEventMonitor();

public: // Device API
    TInt Create( DMemSpyDriverDevice* aDevice );
	TInt Start();
	TInt Stop();
	
public: // API
    TInt RequestEvents( MMemSpyEventMonitorObserver& aObserver );
    TInt RequestEventsCancel( MMemSpyEventMonitorObserver& aObserver );

private: // Handle kernel event callbacks
	static TUint EventHandler( TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aThis );
	TUint HandleEvent( TKernelEvent aType, TAny* a1, TAny* a2 );
    //
	void EventProcessAdd( DProcess* aProcess );
	void EventProcessUpdate( DProcess* aProcess );
	void EventProcessRemoved( DProcess* aProcess );
	//
	void EventThreadAdd( DThread* aThread );
	void EventThreadRemoved( DThread* aThread );
	void EventThreadKilled( DThread* aThread );
    //
	void EventChunkAdd( DChunk* aChunk );
	void EventChunkUpdate( DChunk* aChunk );
	void EventChunkDelete( DChunk* aChunk );

private: // Internal methods
    TBool IsObserving( MMemSpyEventMonitorObserver& aObserver ); 

private:
	/** Lock serialising calls to event handler */
	DMutex* iLock;

    /** open reference to LDD for avoiding lifetime issues */
	DLogicalDevice* iDevice;

    // List of observers
	SDblQue iObservers;
	
	// Whether we are tracking or not
	TBool iTracking;
	};

#endif
