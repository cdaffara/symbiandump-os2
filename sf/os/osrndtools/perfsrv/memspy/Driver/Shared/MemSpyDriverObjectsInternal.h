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

#ifndef MEMSPYDRIVEROBJECTSINTERNAL_H
#define MEMSPYDRIVEROBJECTSINTERNAL_H

// System includes
#include <e32cmn.h>

// User includes
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>
#include "MemSpyDriverEnumerationsInternal.h"




class TMemSpyDriverInternalHeapRequestParameters
    {
public:
    inline TMemSpyDriverInternalHeapRequestParameters()
        : iTid(0),
          iRHeapVTable(0),
          iBuildFreeCellList(EFalse),
          iBuildAllocCellList(EFalse),
          iUseKernelHeapCopy(EFalse),
          iDebugAllocator(EFalse),
          iMasterInfo(NULL)
        {
        }

public: // Params IN
    TUint iTid;
    TUint32 iRHeapVTable;
    TBool iBuildFreeCellList;
	TBool iBuildAllocCellList;
	TBool iUseKernelHeapCopy;

public: // Params IN or OUT (IN in User heap requests, OUT in Kernel heap requests)
    TBool iDebugAllocator;

public: // Params IN and OUT
    TMemSpyHeapInfo* iMasterInfo;
    };





	



/**
 * Parameters for User & Kernel heap data fetch
 */
struct TMemSpyDriverInternalHeapDataParams
    {
    TUint iTid;
    TUint iRHeapVTable;
    TDes8* iDes;
    TInt iRemaining;
    TUint iReadAddress;
    TUint32 iChecksum;
    TBool iDebugAllocator;
    };



/**
 *
 */
struct TMemSpyDriverInternalStackDataParams
    {
    TUint iTid;
    TDes8* iDes;
    TMemSpyDriverDomainType iDomain;
    TBool iEntireStack;
    TInt iRemaining;
    };


/**
 *
 */
struct TMemSpyDriverInternalChunkHandleParams
	{
	TUint iId;
	TMemSpyDriverPrivateObjectType iType;
	TAny** iHandles;
	TInt* iCountPtr;
    TInt iMaxCount;
	};


/**
 *
 */
struct TMemSpyDriverInternalChunkInfoParams
	{
	TAny* iHandle;
	TAny* iBaseAddress;
	TInt iSize;
	TInt iMaxSize;
	TUint iOwnerId;
	TBuf8<KMaxFullName> iName;
    TMemSpyDriverChunkType iType;
    TInt iAttributes;
	};


/**
 *
 */
struct TMemSpyDriverInternalCodeSnapshotParams
	{
	TUint iFilter;
	TAny** iHandles;
	TInt* iCountPtr;
    TInt iMaxCount;
	};



/**
 *
 */
struct TMemSpyDriverInternalCodeSegParams
	{
	TUint iPid;
	TAny* iHandle;
	TMemSpyDriverCodeSegInfo* iInfoPointer;
	};




/**
 *
 */
struct TMemSpyDriverInternalContainerHandleParams
	{
    TInt iTidOrPid;
    TMemSpyDriverThreadOrProcess iHandleSource;
    TMemSpyDriverContainerType iContainer;
	TAny** iHandles;
	TInt* iCountPtr;
    TInt iMaxCount;
	};




/**
 *
 */
struct TMemSpyDriverInternalServerSessionHandleParams
	{
    TAny* iServerHandle;
	TAny** iSessionHandles;
	TInt* iSessionCountPtr;
    TInt iMaxCount;
	};



/**
 *
 */
struct TMemSpyDriverInternalContainerObjectParams
	{
    TUint iTid;
	TAny* iHandle;
	TBuf8<KMaxFullName> iFullName;
	};





/**
 *
 */
struct TMemSpyDriverInternalReadMemParams
	{
	TUint iTid;
	TLinAddr iAddr;
	TDes8* iDes;
	};


/**
 *
 */
struct TMemSpyDriverInternalThreadInfoParams : public TMemSpyDriverThreadInfoBase
	{
    // In:
    TUint iRHeapVTable;
	TBool iDebugAllocator;

    // Out:
	TBuf8<KMaxFullName> iFullName;

    /*
	TUint iPid;
	TLinAddr iStackBase;
	TInt iStackSize;
	TMemSpyDriverRegSet iCpu;
	TLinAddr iSupervisorStackBase;
	TInt iSupervisorStackSize;
    CActiveScheduler* iScheduler;
	RAllocator* iAllocator;

	TBuf8<KMaxExitCategoryName> iExitCategory;
	TInt iExitReason;
    TThreadPriority iThreadPriority;
    TInt iDefaultPriority;
    TExitType iExitType;
    TInt iIpcCount;
	RAllocator* iCreatedAllocator;
    TInt iThreadType;
    TInt iLeaveDepth;
    TUint32 iFlags;
    */
	};


/**
 *
 */
struct TMemSpyDriverInternalWalkHeapParamsInit
	{
	TUint iTid;
	TUint iRHeapVTable;
	TBool iDebugAllocator;
	};


/**
 *
 */
struct TMemSpyDriverInternalWalkHeapParamsCell
	{
	TInt iCellType;
	TAny* iCellAddress;
	TInt iLength;
	TInt iNestingLevel;
	TInt iAllocNumber;
	};


/**
 *
 */
struct TMemSpyDriverInternalWalkHeapCellDataReadParams
    {
    TAny* iCellAddress;
    TInt iReadLen;
    TDes8* iDes;
    };






/**
 *
 */
struct TMemSpyDriverInternalEventMonitorParams
	{
	TUint iHandle;
	TRequestStatus* iStatus;
	TAny* iContext;
	};




/**
 *
 */
struct TMemSpyDriverInternalCondVarSuspendedThreadParams
    {
    TAny* iCondVarHandle;
    TAny** iThrHandles;
    TInt* iThrCountPtr;
    TInt iMaxCount;
    };

#endif
