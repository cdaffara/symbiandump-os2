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

#ifndef MEMSPYDRIVERLOGICALCHANHEAPDATABASE_H
#define MEMSPYDRIVERLOGICALCHANHEAPDATABASE_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>
#ifdef __MARM__
#include <arm.h>
#endif
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverLogChanHeapBase.h"
#include "MemSpyDriverObjectsInternal.h"
#include "MemSpyDriverHeapWalker.h"
#include "MemSpyDriverEnumerationsInternal.h"

// Classes referenced
class DMemSpyDriverDevice;
class RMemSpyMemStreamWriter;

const static TInt KPageSize = 4096;

NONSHARABLE_CLASS(DMemSpyDriverLogChanHeapDataBase) : public DMemSpyDriverLogChanHeapBase
	{
public:
	DMemSpyDriverLogChanHeapDataBase( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanHeapDataBase();

private: // From MHeapWalkerObserver
    void HandleHeapWalkInit();
    TBool HandleHeapCell( TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber );   
    
protected: // For use by the base classes
    TInt PrepareCellListTransferBuffer();
    TInt FetchCellList(TDes8* aBufferSink);
    void ReleaseCellList();
    TInt CalculateCellListBufferSize() const;
    TInt GetFullData( TMemSpyDriverInternalHeapDataParams* aParams );
    TInt DoGetFullData(TMemSpyDriverInternalHeapDataParams& aParams, DThread* aThread, RMemSpyDriverRHeapBase& aHeap);

protected: // To be implemetned by the base classes
    virtual TInt GetFullData( TMemSpyDriverInternalHeapDataParams& aParams ) = 0;

protected: // Data members
    TMemSpyDriverInternalHeapRequestParameters iHeapInfoParams;
	RArray<TMemSpyDriverCell> iCellList;
    RMemSpyMemStreamWriter* iHeapStream;
	};


#endif
