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

#ifndef MEMSPYDRIVERLOGICALCHANHEAPWALKUSER_H
#define MEMSPYDRIVERLOGICALCHANHEAPWALKUSER_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>
#ifdef __MARM__
#include <arm.h>
#endif
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverHeapWalker.h"
#include "MemSpyDriverObjectsInternal.h"
#include "MemSpyDriverLogChanHeapBase.h"
#include "MemSpyDriverEnumerationsInternal.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverLogChanHeapWalkObserver;


NONSHARABLE_CLASS(DMemSpyDriverLogChanHeapWalkUser) : public DMemSpyDriverLogChanHeapBase
	{
public:
	DMemSpyDriverLogChanHeapWalkUser( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanHeapWalkUser();

private: // from DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt WalkHeapInit( TMemSpyDriverInternalWalkHeapParamsInit* aParams );
    TInt WalkHeapNextCell( TUint aTid, TMemSpyDriverInternalWalkHeapParamsCell* aParams );
    TInt WalkHeapClose();
    TInt WalkHeapReadCellData( TMemSpyDriverInternalWalkHeapCellDataReadParams* aParams);
    TInt WalkHeapGetCellInfo( TAny* aCellAddress, TMemSpyDriverInternalWalkHeapParamsCell* aParams );
        
private: // Internal methods
    const TMemSpyDriverInternalWalkHeapParamsCell* CellInfoForAddressWithinCellRange( TAny* aAddress ) const;
    const TMemSpyDriverInternalWalkHeapParamsCell* CellInfoForSpecificAddress( TAny* aAddress ) const;

private: // Heap walker callback
    TBool WalkerHandleHeapCell(TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber );

private:
    TBool iHeapWalkInitialised;
    TInt iWalkHeapCellIndex;
    RMemSpyDriverRHeapUser iWalkHeap;
	RArray< TMemSpyDriverInternalWalkHeapParamsCell > iWalkHeapCells;
    TMemSpyDriverInternalWalkHeapParamsInit iHeapWalkInitialParameters;

private:
    friend class TMemSpyDriverLogChanHeapWalkObserver;
	};


class TMemSpyDriverLogChanHeapWalkObserver : public MMemSpyHeapWalkerObserver
    {
public:
    inline TMemSpyDriverLogChanHeapWalkObserver( DMemSpyDriverLogChanHeapWalkUser& aChannel )
        : iChannel( aChannel )
        {
        }

public: // From MHeapWalkerObserver
    void HandleHeapWalkInit() { }
    TBool HandleHeapCell(TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber )
        {
        return iChannel.WalkerHandleHeapCell( aCellType, aCellAddress, aLength, aNestingLevel, aAllocNumber );
        }

private:
    DMemSpyDriverLogChanHeapWalkUser& iChannel;
    };



#endif
