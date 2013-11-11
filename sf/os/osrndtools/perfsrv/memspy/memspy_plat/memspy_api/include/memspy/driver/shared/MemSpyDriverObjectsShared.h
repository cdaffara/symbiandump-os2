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

#ifndef MEMSPYDRIVEROBJECTSSHARED_H
#define MEMSPYDRIVEROBJECTSSHARED_H

// System includes
#include <u32std.h>

// User includes
#include <memspy/driver/memspydriverenumerationsshared.h>
#include <memspy/driver/memspydriverobjectssharedrheap.h>

// Classes referenced
class CActiveScheduler;
class CTrapCleanup;



/**
 * ARM user registers.
 * Size must be multiple of 4 bytes.
 */
class TMemSpyDriverRegSet
	{
public:
	enum { KRegCount = 16 };
	TUint32 iRn[KRegCount];
	TUint32 iCpsr;
	};


/** 
 * ARM-specific exception-related data.
 * Size must be multiple of 4 bytes.
 */ 
class TMemSpyDriverCpuExcInfo
	{
public:
	enum TExcCode 
		{ 
		EPrefetchAbort=0,
		EDataAbort=1,
		EUndefinedOpcode=2,
		};
public:
	TExcCode iExcCode;
	/** Point to instruction which caused exception */
	TUint32 iFaultPc;
	/** 
     Address which caused exception (System Control Coprocessor Fault
	 Address Register)
	*/
	TUint32 iFaultAddress;
	/** System Control Coprocessor Fault Status Register */
	TUint32 iFaultStatus;
	/** R13 supervisor mode banked register */
	TUint32 iR13Svc;
	/** R14 supervisor mode banked register */
	TUint32 iR14Svc;
	/** SPSR supervisor mode banked register */
	TUint32 iSpsrSvc;
	};





/**
 * Stack water mark parameters
 */
struct TMemSpyDriverStackWatermarkParams
    {
    TUint8* iStackDataAddress;
    TInt iStackDataLength;
    TRequestStatus* iStatus;
    TLinAddr iUserStackPointer;
    };




/**
 * Stack information block.
 */
class TMemSpyDriverStackInfo
	{
public:
    TUint iUserStackPointer;
	TUint32 iUserStackBase;
	TInt iUserStackSize;
    TUint32 iUserStackHighWatermark;
    //
    TUint iSupervisorStackPointer;
	TUint32 iSupervisorStackBase;
	TInt iSupervisorStackSize;
    TUint32 iSupervisorStackHighWatermark;
	};


/**
 * Basic thread information block
 */
class TMemSpyDriverThreadInfoBase
	{
public:
    // Registers
    TMemSpyDriverRegSet iCpu;
    
    // Stack
    TMemSpyDriverStackInfo iStackInfo;
    
    // User thread framework
	RAllocator* iAllocator;
    CActiveScheduler* iScheduler;
	};


/**
 * Thread information block.
 */
class TMemSpyDriverThreadInfo : public TMemSpyDriverThreadInfoBase
	{
public:
	TFullName iFullName;
 
    // Basic properties
	TUint iPid;
    TThreadPriority iThreadPriority;

    // Exit information
	TInt iExitReason;
    TExitType iExitType;
    TExitCategoryName iExitCategory;
	};


/**
 * Process information block.
 */
class TMemSpyDriverProcessInfo
    {
public:
    inline TUint32 SID() const { return iSecurityInfo.iSecureId; }
    inline TUint32 VID() const { return iSecurityInfo.iVendorId; }

public:
    TUidType iUids;
    TUint32 iFlags;
    TInt iGeneration;
    TInt iPriority;
    SSecurityInfo iSecurityInfo;
    };


/** 
 * Code Segment Information Block 
 */
class TMemSpyDriverCodeSegInfo
	{
public:
    inline TMemSpyDriverCodeSegInfo()
        : iSize( 0 )
        {
        }

public:
    TUint32 iSize;
    TCodeSegCreateInfo iCreateInfo;
    TProcessMemoryInfo iMemoryInfo;
	};





/**
 * Generic object related
 */
class TMemSpyDriverHandleInfoGeneric
    {
public: // Supported for all types
	TBuf8<KMaxName> iName; // FullName
    TAny* iHandle;
    TMemSpyDriverContainerType iType;

public: // Type-specific members

    // For Process, thread, chunk (owning process id), server (owning thread id)
    TInt iId;

    // For Chunk, Library
    TUint32 iSize;
   
    // For Semaphore, Mutex, Server
    TInt iCount;

    // For Mutex
    TInt iWaitCount;

    // For Server, Session
    TIpcSessionType iSessionType;

    // For Timer
    TMemSpyDriverTimerType iTimerType;
    TMemSpyDriverTimerState iTimerState;

    // For Logical channel
    TInt iOpenChannels;
    
    // For most of the object types
	TBuf8<KMaxName> iNameDetail; // Name
    TInt iAccessCount;
    TInt iUniqueID;
    TUint iProtection;
    TUint8* iAddressOfKernelOwner;
    TInt iPriority;
    TUint8* iAddressOfOwningProcess;
    TUint iCreatorId;
    TUint iSecurityZone;
    TInt iAttributes;
    TUint8* iAddressOfDataBssStackChunk;

    // For Server, Session
    TUint8* iAddressOfOwningThread;
    TUint8* iAddressOfServer;
    TUint16 iTotalAccessCount;
    TUint8 iSvrSessionType;
    TInt iMsgCount;
    TInt iMsgLimit;
    
    // For chunk
    TInt iMaxSize;
    TInt iBottom;
    TInt iTop;
    TInt iStartPos;
    TUint iControllingOwner;
    TUint iRestrictions;
    TUint iMapAttr;
    TUint iChunkType;
    TBuf8<KMaxName> iNameOfOwner; // chunk, server
    
    // For library
    TInt iMapCount;
    TUint8 iState;
    TUint8* iAddressOfCodeSeg;
    
    // Semaphore, mutex, condvar
    TUint8 iResetting;
    TUint8 iOrder;
    
    // For Logical/Physical device
    TVersion iVersion;
    TUint iParseMask;
    TUint iUnitsMask;
    
    // For change notifier
    TUint iChanges;
    };



/**
 *
 */
class TMemSpyDriverServerSessionInfo
    {
public: // Enumerations
    enum TOwnerType
        {
        EOwnerNone = -1,
        EOwnerThread = 0,
        EOwnerProcess
        };

public:
    TInt iOwnerId;
    TOwnerType iOwnerType;
    TBuf8<KMaxName> iName;
    TIpcSessionType iSessionType;
    TUint8* iAddress;
    };



/**
 *
 */
class TMemSpyDriverProcessInspectionInfo
    {
public:
    inline TMemSpyDriverProcessInspectionInfo()
        : iProcessId( 0 ),
          iMemoryHeap( 0 ),
          iMemoryStack( 0 ),
          iMemoryGlobalData( 0 ),
          iMemoryChunkLocal( 0 ),
          iMemoryChunkShared( 0 )
        {
        }

public: // API
    inline TUint32 TotalIncShared() const
        {
        const TUint32 ret = TotalExcShared() + iMemoryChunkShared;
        return ret;
        }

    inline TUint32 TotalExcShared() const
        {
        const TUint32 ret = iMemoryHeap + iMemoryStack + iMemoryChunkLocal + iMemoryGlobalData;
        return ret;
        }

public:
    TUint32 iProcessId;
    //
    TUint32 iMemoryHeap;
    TUint32 iMemoryStack;
    TUint32 iMemoryGlobalData;
    TUint32 iMemoryChunkLocal;
    TUint32 iMemoryChunkShared;
    //
    TInt64 iTime;
    };






class TMemSpyHeapInfo
    {
public: // Enumerations
    enum THeapImplementationType
        {
        ETypeUnknown = 0,
        ETypeRHeap = 1,
		ETypeRHybridHeap = 2,
        };

public: // Constructor & destructor
    inline TMemSpyHeapInfo()
        {
        }

public: // API
    inline void SetType( THeapImplementationType aType ) { iType = aType; }
    inline THeapImplementationType Type() const { return iType; }
    //
    inline TMemSpyHeapInfoRHeap& AsRHeap() { return iRHeap; }
    inline const TMemSpyHeapInfoRHeap& AsRHeap() const { return iRHeap; }
    //
    inline void SetTid( TUint32 aId ) { iTid = aId; }
    inline TUint32 Tid() const { return iTid; }
    //
    inline void SetPid( TUint32 aId ) { iPid = aId; }
    inline TUint32 Pid() const { return iPid; }

private: // Data members
    THeapImplementationType iType;
    TMemSpyHeapInfoRHeap iRHeap;
    TUint32 iPid;
    TUint32 iTid;
    };






/**
 * Chunk info base class
 */
class TMemSpyDriverChunkInfoWithoutName
    {
public:
    inline TMemSpyDriverChunkInfoWithoutName()
        : iHandle( NULL ),
          iBaseAddress( NULL ),
          iSize( 0 ),
          iMaxSize( 0 ),
          iOwnerId( 0 ),
          iType( EMemSpyDriverChunkTypeUnknown )
        {
        }

public:
    TAny* iHandle;
	TAny* iBaseAddress;
	TUint iSize;
	TUint iMaxSize;
	TUint iOwnerId;
    TMemSpyDriverChunkType iType;
    TInt iAttributes;
    };


/**
 * Chunk info 
 */
class TMemSpyDriverChunkInfo : public TMemSpyDriverChunkInfoWithoutName
    {
public:
    inline TMemSpyDriverChunkInfo()
        : TMemSpyDriverChunkInfoWithoutName(),
          iName( KNullDesC )
        {
        }

public:
	TFullName iName;
    };


/**
 * cell information
 */
class TMemSpyDriverFreeCell
	{
public:
	TMemSpyDriverCellType iType;
	TAny* iAddress;
	TInt iLength;
	};

// For compatibility I can't change TMemSpyDriverCell to be the class and typdef/derive TMemSpyDriverFreeCell. Sigh...
typedef TMemSpyDriverFreeCell TMemSpyDriverCell;


/** 
 * P&S information
 */
class TMemSpyDriverPAndSInfo
    {
public:
    RProperty::TType iType;
    TUint iCategory;
    TUint iKey;
    TInt iRefCount;
    TUint32 iTid;
    TUint32 iCreatorSID;
    };


/**
 * CondVar suspended thread information
 */
class TMemSpyDriverCondVarSuspendedThreadInfo
    {
public:
    TBuf8<KMaxName> iName;
    TUint8* iAddress;
    };

#endif
