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

#ifndef MEMSPYDRIVERLOGICALCHANHEAPBASE_H
#define MEMSPYDRIVERLOGICALCHANHEAPBASE_H

// System includes
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverLogChanBase.h"
#include "MemSpyDriverObjectsInternal.h"
#include "MemSpyDriverHeapWalker.h"
#include "MemSpyDriverEnumerationsInternal.h"

// Classes referenced
class TMemSpyHeapInfo;
class DMemSpyDriverDevice;
class RMemSpyDriverRHeapUser;
class RMemSpyMemStreamWriter;
class RMemSpyDriverRHeapKernelInPlace;
class RMemSpyDriverRHeapKernelFromCopy;


NONSHARABLE_CLASS(DMemSpyDriverLogChanHeapBase) : public DMemSpyDriverLogChanBase, public MMemSpyHeapWalkerObserver
	{
public:
    enum TDrmMatchType
        {
        EMatchTypeNone = 0,
        EMatchTypeName,
        EMatchTypeUid
        };
public:
	~DMemSpyDriverLogChanHeapBase();

protected:
	DMemSpyDriverLogChanHeapBase( DMemSpyDriverDevice& aDevice, DThread& aThread );
    TInt Construct();

protected: // From DMemSpyDriverLogChanBase
    TInt Request( TInt aFunction, TAny* a1, TAny* a2 );

protected: // Capability checks for heap access
    TDrmMatchType IsDrmThread( DThread& aThread );

protected: // From MHeapWalkerObserver
    void HandleHeapWalkInit();
    TBool HandleHeapCell( TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber );

protected: // Heap utility functions
    void PrintHeapInfo( const TMemSpyHeapInfo& aInfo );

private: // Data members

    // Points to stack-based object whilst walking in progress
    RMemSpyMemStreamWriter* iStackStream;
    TInt iFreeCellCount;
	};


#endif
