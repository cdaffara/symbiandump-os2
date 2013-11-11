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

#ifndef MEMSPYDRIVERHEAPWALKER_H
#define MEMSPYDRIVERHEAPWALKER_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// Shared includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverHeapStatistics.h"

#include "heaputils.h"
using namespace LtkUtils;

// Heap walker observer interface - can be used to make a record of each cell
class MMemSpyHeapWalkerObserver
    {
public:
    virtual TBool HandleHeapCell(TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber) = 0;
    virtual void HandleHeapWalkInit() = 0;
    };


// Heap walker - allows in-place walking of any heap
class RMemSpyDriverHeapWalker
    {
public:
	RMemSpyDriverHeapWalker(RMemSpyDriverRHeapBase& aHeap, MMemSpyHeapWalkerObserver* aObserver=NULL);
		
public: // API
    TInt Traverse();
    void CopyStatsTo( TMemSpyHeapStatisticsRHeap& aStats );
    void SetObserver( MMemSpyHeapWalkerObserver* aObserver );
    inline void SetPrintDebug() { iPrintDebug = ETrue; }
    inline const TMemSpyHeapWalkStatistics& Stats() const { return iStats; }

private: // Internal methods
	static TBool CellCallback(RAllocatorHelper& aHelper, TAny* aContext, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellAddress, TInt aLength);
	TBool DoCellCallback(RAllocatorHelper& aHelper, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellAddress, TInt aLength);
    TBool NotifyCell( TMemSpyDriverCellType aType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel = -1, TInt aAllocNumber = -1 );
    //
    void UpdateStats( TMemSpyDriverCellType aType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber );
    void InitialiseStats();
    void FinaliseStats();
    void PrintStats();
    //
    inline TBool PrintDebug() const { return iPrintDebug; }

private:
    RMemSpyDriverRHeapBase& iHeap;
    TBool iPrintDebug;
    MMemSpyHeapWalkerObserver* iObserver;
    TMemSpyHeapWalkStatistics iStats;
    };



#endif
