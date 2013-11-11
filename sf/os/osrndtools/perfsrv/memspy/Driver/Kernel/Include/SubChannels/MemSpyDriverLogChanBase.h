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

#ifndef MEMSPYDRIVERLOGCHANBASE_H
#define MEMSPYDRIVERLOGCHANBASE_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>
#ifdef __MARM__
#include <arm.h>
#endif

// User includes
#include "MemSpyDriverStreamWriter.h"

// Classes referenced
class DMemSpyDriverDevice;
class DMemSpyDriverXferBuffer;
class DMemSpyDriverOSAdaption;
class DMemSpySuspensionManager;

NONSHARABLE_CLASS(DMemSpyDriverLogChanBase) : public DBase
	{
public:
	~DMemSpyDriverLogChanBase();

protected: // Internal construction
	DMemSpyDriverLogChanBase( DMemSpyDriverDevice& aDevice, DThread& aThread );
    TInt BaseConstruct( TInt aTransferBufferSize = 0 );

public: // API
    /**
     * Finish construction of the channel object
     */
    virtual TInt Construct();

    /**
     * Request a channel function - this function implementation (in this class)
     * is able to perform general policing, e.g. SID, capability etc.
     *
     * Currently this always returns KErrNone, but none the less, derived classes
     * should call this implementation and handle return value appropriately.
     */
    virtual TInt Request( TInt aFunction, TAny* a1, TAny* a2 );

    /**
     * Sub channel should return ETrue if it can handle the specified function
     * request.
     */
    virtual TBool IsHandler( TInt aFunction ) const = 0;

protected: // Internal access
    DMemSpyDriverDevice& MemSpyDevice();
    DMemSpySuspensionManager& SuspensionManager();

protected: // Internal methods
	TInt OpenTempObject( TUint aId, TObjectType aType, TBool aAllowDeadObjects = EFalse );
	void CloseTempObject();
    DThread& TempObjectAsThread();
    DProcess& TempObjectAsProcess();

protected: // Transfer buffer
    RMemSpyMemStreamWriter OpenXferStream();
    TInt OpenXferStream( RMemSpyMemStreamWriter& aWriter, TInt aRequiredSize );

protected: // Internal access
    DMemSpyDriverOSAdaption& OSAdaption();
    inline DObject* TempObject() { return iTempObj; }
    inline const DObject* TempObject() const { return iTempObj; }
    //
    inline DThread& ClientThread() { return iClientThread; }

private: // Data members
    DMemSpyDriverDevice& iDevice;
	DThread& iClientThread;
	DObject* iTempObj;
    DMemSpyDriverXferBuffer* iXferBuffer;
	};



#endif
