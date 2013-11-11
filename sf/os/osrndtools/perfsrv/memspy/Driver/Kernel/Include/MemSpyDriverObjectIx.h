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

#ifndef MEMSPYDRIVEROBJECTIX_H
#define MEMSPYDRIVEROBJECTIX_H

// System includes
#include <kernel.h>
#include <kern_priv.h>

#if MCL_ROBJECTIX_DUPLICATION

class RMemSpyObjectIx
	{
public:
	enum {ENoClose=KHandleNoClose,ELocalHandle=0x40000000};
	enum {EReserved=0x80000000u, EAttributeMask=0xfffu};
	enum {EMinSlots=8, EMaxSlots=32768};
public:
	enum {ENumFreeQ=6, EModCount=4, EBitMapSize=128, EMaxLockedIter=8};
private:
	// queue numbers
	enum {EQFree=-6, EQAltFree=-5, EQTempFree=-4, EQRsvd=-3, EQAltRsvd=-2, EQTempRsvd=-1};
	// iObjR, iRsvd fields
	enum {EObjROccupied=4u, EObjRObjMask=0xfffffffcu, EObjRRsvd=1u};
	// states
	enum {ENormal=0u, ETidying=1u, EFindingLast=2u, ECounting=3u, ESearching=4u, ETerminated=5u};
	struct	SSlotQLink
		{
		TInt16		iNext;	// pointer to next free slot, -n if no next
		TInt16		iPrev;	// pointer to previous free slot, -n if no previous
		};
	struct	SFreeSlot : public SSlotQLink
		{
		TUint32		iRsvd;	// 0 for normal, 1 for reserved slot
		};
	struct	SUsedSlot
		{
		TUint32		iAttr;	// bits 0-13 = instance (nonzero), bits 14-19 = object type, bits 20-31 = handle attributes
		TUint32		iObjR;	// pointer to object (nonzero), bit 0=1 if reserved slot
		};
	union SSlot
		{
		SFreeSlot	iFree;
		SUsedSlot	iUsed;
		};
	struct SMonitorObj
		{
		DObject*	iObj;
		TInt		iBoundary;
		TInt		iResult;
		};
	union SModList
		{
		TInt16		iIndex[EModCount];
		TUint32		iBitMap[EBitMapSize/32];
		SMonitorObj	iMonitor;
		};
private:
#ifdef __HANDLES_USE_RW_SPIN_LOCK__
// Beginning of support for spinlock-only protection (i.e. not requiring the system lock)
// for access to handles.  Requires changes everywhere objects returned from handles are
// used, and some additional work in the handle lookup code in cutils.cia.
#error "spinlocks for handle lookup not supported"
	inline void AcquireReadLock()
		{ __SPIN_LOCK_IRQ_R(iRWL); }
	inline void ReleaseReadLock()
		{ __SPIN_UNLOCK_IRQ_R(iRWL); }
	inline void AcquireWriteLock()
		{ __SPIN_LOCK_IRQ_W(iRWL); }
	inline void ReleaseWriteLock()
		{ __SPIN_UNLOCK_IRQ_W(iRWL); }
#else
	/* Places which use a read lock would already have the system lock held */
	inline void AcquireReadLock()
		{ __ASSERT_SYSTEM_LOCK; }
	inline void ReleaseReadLock()
		{  }
	inline void AcquireWriteLock()
		{ NKern::LockSystem(); }
	inline void ReleaseWriteLock()
		{ NKern::UnlockSystem(); }
#endif
private:
	static inline DObject* Occupant(SSlot* aS)
		{ return (DObject*)(aS->iUsed.iObjR & EObjRObjMask); }
	static inline TBool IsReserved(SSlot* aS)
		{ return aS->iUsed.iObjR & EObjRRsvd; }
	static inline TBool IsFreeReserved(SSlot* aS)
		{ return (aS->iUsed.iObjR & EObjRRsvd) && (aS->iUsed.iObjR<EObjROccupied); }
    /*
	void Empty(TInt aQueue);
	SSlot* Dequeue(TInt aSlotIndex);
	void AddHead(TInt aQueue, TInt aSlotIndex);
	void AddTail(TInt aQueue, TInt aSlotIndex);
	void AddBefore(TInt aBase, TInt aSlotIndex);
	void AddAfter(TInt aBase, TInt aSlotIndex);
	void AppendList(TInt aSrcQ, TInt aDestQ);
	void PrependList(TInt aSrcQ, TInt aDestQ);
	TInt DoAdd(DObject* aObj, TUint32 aAttr, SSlot* aSlot);	// add aObj using an existing slot (don't grow)
	TInt DoRemove(TInt aHandle, DObject*& aObject, TUint32& aAttr);	// remove a handle (don't shrink)
	void MarkModified(TInt aSlotIndex);
	static TUint32 GetNextInstanceValue();
	TInt UnReserveSlots(TInt aCount, TBool aAmortize);
	TInt ReserveSlots(TInt aCount);
	TInt Grow(TInt aReserve, SSlot* aSlotData);
	void TidyAndCompact();
	inline SSlotQLink* Link(TInt aIndex)
		{ return (aIndex<0) ? (iFreeQ+ENumFreeQ+aIndex) : &(iSlots+aIndex)->iFree; }
        */
public:
	// common operations
    RMemSpyObjectIx();

    //static void Wait();
	//static void Signal();

    inline TInt Count()
		{ return iCount; }
	inline TInt ActiveCount()
		{ return iActiveCount; }

public:
	// uncommon operations
	DObject* operator[](TInt aIndex);
	TBool Find(DObject* aObject);

private:
	TRWSpinLock		iRWL;
	TInt			iAllocated;			// Max entries before realloc needed
	volatile TInt	iCount;				// Points to at least 1 above the highest occupied slot or unoccupied reserved slot
	volatile TInt	iActiveCount;		// Number of occupied entries in the index (reserved or normal)
	volatile TInt	iReservedFree;		// Number of unoccupied reserved slots
	volatile TInt	iReservedTotal;		// Number of reserved slots (occupied or unoccupied)
	volatile TInt	iReservedFreeHWM;	// Points to at least 1 above the last unoccupied reserved slot
	SSlotQLink		iFreeQ[ENumFreeQ];	// queues of free slots
	SSlot*			iSlots;				// array of handle slots
	TInt			iAmortize;			// Number of handle removals before we see if we can shrink
	TUint8			iState;
	TUint8			iModCount;			// 255=not in use, 0...EModCount->use iModList.iIndex[], EModCount+1->use iModList.iBitMap
	TUint8			iModListShift;
	TUint8			iSpare1;
	SModList		iModList;			// Entries modified while array moving

public:
	static volatile TUint32 NextInstance;
	static DMutex* HandleMutex;
	};

#elif MCL_DOBJECTIX_DUPLICATION

class DMemSpyObjectIx : public DBase
	{
public:
    inline DMemSpyObjectIx() { }

public:
	DObject* At(TInt aHandle,TInt aUniqueID);
	DObject* At(TInt aHandle);
	TBool Find(DObject* aObject);
	TInt Count(DObject* aObject);
	DObject* operator[](TInt aIndex);
	//static void Wait( DMemSpyObjectIx* aObjectIndex );
	//static void Signal( DMemSpyObjectIx* aObjectIndex );
	inline TInt Count();
	inline TInt ActiveCount();

private:
	TInt iNextInstance;
	TInt iAllocated;		// Max entries before realloc needed
	TInt iCount;			// Points to at least 1 above the highest active index
	TInt iActiveCount;		// Number of actual entries in the index
	SDObjectIxRec* iObjects;
	TAny* iPtr;
	TInt iFree;				// The index of the first free slot or -1.
	TInt iUpdateDisabled;   // If >0, disables: iCount update, reorder of the free list and memory shrinking.

public:
	static DMutex* HandleMutex;
	};

inline TInt DMemSpyObjectIx::Count()
	{return iCount;}

inline TInt DMemSpyObjectIx::ActiveCount()
	{return iActiveCount;}

#endif



#if MCL_ROBJECTIX_DUPLICATION

    #define MemSpyObjectIx                                          RMemSpyObjectIx
    //#define MemSpyObjectIx_Wait( IX )                               RMemSpyObjectIx::Wait()
    //#define MemSpyObjectIx_Signal( IX )                             RMemSpyObjectIx::Signal()
    #define MemSpyObjectIx_GetHandlePointer_Thread( DTHREAD )       reinterpret_cast< MemSpyObjectIx* >( &DTHREAD.iHandles )
    #define MemSpyObjectIx_GetHandlePointer_Process( DPROCESS )     reinterpret_cast< MemSpyObjectIx* >( &DPROCESS.iHandles )

#elif MCL_DOBJECTIX_DUPLICATION

    #define MemSpyObjectIx                                          DMemSpyObjectIx
    //#define MemSpyObjectIx_Wait( IX )                               DMemSpyObjectIx::Wait( IX )
    //#define MemSpyObjectIx_Signal( IX )                             DMemSpyObjectIx::Signal( IX )
    #define MemSpyObjectIx_GetHandlePointer_Thread( DTHREAD )       reinterpret_cast< MemSpyObjectIx* >( DTHREAD.iHandles )
    #define MemSpyObjectIx_GetHandlePointer_Process( DPROCESS )     reinterpret_cast< MemSpyObjectIx* >( DPROCESS.iHandles )

#else

    #define MemSpyObjectIx                  DObjectIx
    //#define MemSpyObjectIx_Wait( IX )       
    //#define MemSpyObjectIx_Signal( IX )     
    #define MemSpyObjectIx_IsValid_Thread( DTHREAD )    ( DTHREAD.iHandles != NULL )
    #define MemSpyObjectIx_IsValid_Process( DPROCESS )  ( DPROCESS.iHandles != NULL )
    #define MemSpyObjectIx_GetHandlePointer_Thread( DTHREAD )       reinterpret_cast< MemSpyObjectIx* >( DTHREAD.iHandles )
    #define MemSpyObjectIx_GetHandlePointer_Process( DPROCESS )     reinterpret_cast< MemSpyObjectIx* >( DPROCESS.iHandles )

#endif

#define MemSpyObjectIx_HandleLookupLock()							NKern::LockSystem()
#define MemSpyObjectIx_HandleLookupUnlock()							NKern::UnlockSystem()

#endif
