// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined(__U32PERM_H__)
#define __U32PERM_H__
#if !defined(__S32STD_H__)
#include <s32std.h>
#endif
#if !defined(__U32FRAME_H__)
#include "U32FRAME.H"
#endif

#if defined(_DEBUG)&&!defined(__SMALL_BUNDLE)
//#define __SMALL_BUNDLE
#endif

//Forward declaratons
class RPermanentFileStoreIter;
class TDriveInfo;

//The offset of the header of a permanent file store.
//Since all permanent file store operations work in their own coordinate system, where physical file offset 32 is
//logical offset 0, KPermanentStoreHeaderOffset is set with -16, which means that the physical file offset is 32 - 16 = 16.
const TInt KPermanentStoreHeaderOffset=-16;

//Permanent file store header length: sizeof(backup TOC ref) + sizeof(handle) + sizeof(TOC ref) + sizeof(crc) = 4 + 4 + 4 + 2 = 14
const TInt KPermanentStoreHeaderLength=14;

//Backup TOC ref length - 4 bytes
const TInt KPermanentStoreBackupLength=4;

//
const TInt KMaskHandleHash=0xff000000;
const TInt KHandleInvalid=0x80000000;
const TInt KHandleTocBase=0x40000000;
const TInt KMaskHandleClear=0x30000000;
const TInt KMaskHandleGen=0x0f000000;
const TInt KIncHandleGen=0x01000000;
//
const TInt KMaxHandleIndex=0x00ffffff;
const TInt KMaskHandleIndex=0x00ffffff;
const TInt KSizeHandleIndex=3;
//
const TInt KTocDeltaCap = 64;		//up to 64 entries in a delta TOC
const TInt KTocDeltaMagic = 2;
const TInt KMaxTocDeltaMagic = KMaxTUint16;
const TInt KTocDelta=KHandleInvalid;
//
const TInt KOffsetTocHeader=-12;
const TInt KSizeTocEntry=5;		//base toc entry size is 5 bytes (when stored in the file, 8 bytes when presented in memory)
const TInt KSizeTocDeltaEntry=8;//delta toc entry size is 8 bytes
const TInt KSizeTocDeltaExtra=7;
const TInt KElementsTocBuf=48;
const TInt KBackTocBuf=20*KSizeTocEntry;
const TInt KSizeTocBuf=KElementsTocBuf*KSizeTocEntry;

//TPermanentStoreHeader class.
//
//Represents the data kept in the permanent store file header.
//Data members:
// - iBackup - "backup TOC reference", 32-bits integer, which keeps the 31-bit file offset of the backup TOC.
//             Plays important role in the "store commit" procedure.
//             The LSB is a "dirty" bit. If during the store opening phase the dirty bit is found to be set,
//             then it means - the previous "store commit" operation has not been completed successfully and
//             the backup TOC shall be used instead of the TOC;
// - iHandle - 32-bit stream handle (MSB - invalid/deleted, 3 bits - unused, 4 bits - generation counter, 24 bits - stream handle).
//             Plays important role in the "stream relocation" procedure during store compaction.
//             iHandle keeps the handle of the stream being relocated, so if the commit phase fails, the original stream entry
//             can be restored at the moment when the store is reopened;
// - iRef    - Current "TOC reference". Represents a file offset, where the current TOC is;
// - iCrc    - 16-bit CRC, protecting iBackup, iHandle, iRef;
class TPermanentStoreHeader
	{
public:
	TPermanentStoreHeader() {}
	inline TUint8* Ptr();
	inline const TUint8* Ptr() const;
	TBool IsValid() const;
//
	inline TPermanentStoreHeader(TInt aToc);
	inline TPermanentStoreHeader(TInt aBackupToc,TInt aHandle,TInt aReference);
//
	inline TBool IsDirty() const;
	inline void MarkDirty();
	inline void SetBackupToc(TInt aBackupToc);
	inline TInt BackupToc() const;
//
	inline TInt Handle() const;
	inline TInt Reference() const;
private:
	void Set(TInt aBackupToc,TInt aHandle,TInt aReference);
private:
	TUint32 iBackup;
	TInt32 iHandle;
	TInt32 iRef;
	TUint16 iCrc;
	};
	
//CPermanentStoreToc class.
//
//Represents the data kept in the permanent file store TOC (Table Of Content).
//Each TOC consists of:
// - TOC header         - CPermanentStoreToc::STocHead structure;
// - set of TOC entries - CPermanentStoreToc::TEntry structure;
//
//Each TOC entry consists of:
// - A stream handle (32 bits: MSB - invalid/deleted, 3 bits - unused, 4 bits - generation counter, 24 bits - stream handle);
// - A stream ref - the offset of the stream data in the permannet file store;
NONSHARABLE_CLASS(CPermanentStoreToc) : public CBase
	{
public:
	struct TEntry
		{
		TInt handle;
		TInt ref;
		static TInt Compare(const TEntry&, const TEntry&);
		};
	struct STocHead
		{
		TInt32 primary;
		TInt32 avail;
		TUint32 count;
		};
	enum TPut {EWrite,ETestBeforeWrite};
public:
	static CPermanentStoreToc* NewL(TStreamPos aBase,TStreamExchange& aHost,TInt aToc,TInt aBaseReloc);
	~CPermanentStoreToc();
//
	inline TInt Extent() const;
	void Move(TInt aToc,TInt anExtent);
//
	inline TBool IsVirtual() const;
	TInt RealizeL(TInt aPrimary,TInt anExtent) const;
	void Adopt(TInt aToc,TInt aPrimary);
//
	inline TInt Primary() const;
	inline void Changed();
//
	TInt AllocL(TInt anOffset);
	TInt AllocL();
	void Cancel(TInt aHandle);
	void FreeL(TInt aHandle);
	TInt AtL(TInt aHandle) const;
	void PutL(TInt aHandle,TInt anOffset,TPut aCheck);
	TInt GetL(TInt aHandle);
	TInt Set(TInt aHandle,TInt anOffset);
//
	CPermanentStoreToc(TStreamPos aBase,TStreamExchange& aHost);
	void ConstructL(TInt aToc,TInt aBaseReloc);
	
	TInt RefSpan(TInt aHandle,TInt& aLength);
	
private:
	inline TStreamPos Base() const;
	inline TStreamExchange& Host() const;
	inline const TEntry* Entry(TInt aHandle) const;
	TEntry* Entry(TInt aHandle);
	TEntry& FetchL(TInt aHandle);
	TEntry& DoAllocL();
	TInt DoAtL(TInt aHandle) const;
	void PutBaseL(TInt aHandle, TInt aReference);
	void PutDeltaL(TInt aPos, TInt aHandle, TInt aReference);
	void PutTocL(TInt aTocBase, TPut aCheck);
	inline TBool HasDelta() const;
	TBool IsDelta() const;
	TInt InternalizeL(RReadStream& aIn, TInt aBaseReloc);
	TInt DeltaL(RFrame16Buf& aBuf,TInt aExtent,const STocHead& aHead) const;
	TInt RewriteL(RFrame16Buf& aBuf,TInt aExtent,const STocHead& aHead) const;
	TInt SmallTocL(RFrame16Buf& aBuf,TInt aExtent,const STocHead& aHead) const;
private:
	TInt iPrimary;
	TInt iAvail;
	TInt iCount;
	RArray<TEntry> iEntries;
//
	TStreamPos iBase;
	TStreamExchange* iHost;
	TInt iMagic;
	TInt iOff;
	TInt iExt;
	TInt iTocOff;
	TInt iTocExt;
	__MUTABLE TInt iWindow;
	__MUTABLE TUint8 iBuf[KSizeTocBuf];
private:
	friend class RPermanentStoreTocIter;
	};

//	
class RPermanentStoreTocIter
	{
public:
	typedef CPermanentStoreToc::TEntry TEntry;
	typedef CPermanentStoreToc::STocHead STocHead;
public:
	RPermanentStoreTocIter(const CPermanentStoreToc& aTable);
	inline void Close();
	void Release();
//
	void ResetL();
	TBool NextL(TEntry& anEntry);
private:
	const CPermanentStoreToc& iTable;
	RFrame16Buf iBuf;
	TInt iIndex;
	TInt iCount;
	TInt iNext;
	const TEntry* iDelta;
	const TEntry* iDeltaEnd;
	};
	
//
class TPermanentStoreCache
	{
public:
	struct TItem {TInt handle;TInt offset;TInt extent;};
public:
	inline TPermanentStoreCache();
	const TItem* At(TInt aHandle) const;
	void Relocated(TInt aHandle,TInt anOffset);
	void Put(const TItem* anItem,TInt anOffset,TInt anExtent);
	void Add(TInt aHandle,TInt anOffset,TInt anExtent);
	void Remove(TInt aHandle);
	void Invalidate();
private:
	enum {EItems=2*16-1};
private:
	TItem iItems[EItems];
	};
	
//
NONSHARABLE_CLASS(CPermanentStoreCoord) : public CBase
	{
private:
	enum {EReady,EBackup=0x1,EClip=0x2};
	enum TFileQoS 
		{
		EUnknown,		//
		ESimple,		//File, "write byte" is an atomic operation
		EBlockAtomic,	//File, "block write" is an atomic operation
		ETransactional	//Transactional file system.
		};
	typedef TPermanentStoreCache::TItem TItem;
public:
	inline TBool IsTrim() const;
	TStreamPos LimitL();
	inline void Clipped();
//
	TStreamId PrimaryL();
	void ChangedL();
	TBool CommitL(TStreamId aPrimary);
	TBool RevertL(TStreamId& aPrimary);
//
	TStreamId ExtendL();
	void DeleteL(TStreamId anId);
//
 	CPermanentStoreCoord(TStreamPos aBase,TStreamExchange& aHost);
	void InternalizeL(RReadStream& aStream);
	~CPermanentStoreCoord();
private:
	void CanExtendL();
	TInt DoCreateL();
	void DoReplaceL(TInt aHandle);
	TInt DoOpenL(TInt& anOffset,TInt aHandle);
	void DoRelease(TInt aHandle,TInt anOffset,TInt anExtent);
	TInt DoCommit(TInt aHandle,TInt anOffset,TInt anExtent);
//
	inline TStreamPos Base() const;
	inline TStreamExchange& Host() const;
	inline TInt Toc() const;
	inline CPermanentStoreToc& Table() const;
	CPermanentStoreToc& TableL();
	CPermanentStoreToc& ConsolidateL();
	void RelocateL(TInt aHandle,TInt anOffset);
	void MoveL(TInt aToc,TInt anExtent);
	inline TUint Generation() const;
	inline void Inc();
	inline void Dec();
	inline TBool Accessed() const;
	TFileQoS FileQoSL();
	TBool IsBlockAtomicL(TInt aDriveNo) const;
//
	MStreamBuf* BeginL(TPermanentStoreHeader& aHeader);
private:
	TStreamPos iBase;
	TStreamExchange *iHost;
	TInt iToc;
	CPermanentStoreToc* iTable;
	TInt iReloc;
	TInt iTarget;
	TPermanentStoreCache iCache;
//
	TUint iGen;
	TInt iRefs;
	TInt iAccess;
	TInt iExtend;
	TInt iState;
	TInt iExt;
	TFileQoS iFileQos;
private:
	friend class HPermanentStoreBuf;
	friend class CPermanentStoreCollector;
	friend class RPermanentFileStoreIter;
	};
//
NONSHARABLE_CLASS(HPermanentStoreBuf) : public RFrame16Buf
	{
public:
	static HPermanentStoreBuf* CreateL(CPermanentStoreCoord& aCoord,TStreamId& anId,TInt aMode=ERead|EWrite);
	static HPermanentStoreBuf* ReplaceL(CPermanentStoreCoord& aCoord,TStreamId anId,TInt aMode=ERead|EWrite);
	static HPermanentStoreBuf* OpenL(CPermanentStoreCoord& aCoord,TStreamId anId,TInt aMode=ERead|EWrite);
//
	virtual ~HPermanentStoreBuf();
private:
	static HPermanentStoreBuf* NewLC(CPermanentStoreCoord& aCoord);
	static HPermanentStoreBuf* ExtendLC(CPermanentStoreCoord& aCoord,TInt aMode);
	inline HPermanentStoreBuf(CPermanentStoreCoord& aCoord);
	inline CPermanentStoreCoord& Coord() const;
//
	void DoRelease();
	void DoSynchL();
private:
	CPermanentStoreCoord* iCoord;
	TInt iHandle;
	};
//
class TPermanentStoreStreamIter
	{
#if defined(__SMALL_BUNDLE)
	enum {EBundleSize=8-1};
#else
	enum {EBundleSize=64-1};
#endif
public:
	void Reset();
	TInt FillL(CPermanentStoreToc& aToc);
	TInt Next();
//
	void Relocated(TInt aStream);
private:
	static void Push(TInt* aHeap,TInt* aHole,TInt aValue);
	static TInt PopPush(TInt* aHeap,TInt* anEnd,TInt aValue);
private:
	TInt* iNext;
	const TInt* iFinish;
	TInt iMore;
	TInt iPos;
	TInt iTable[EBundleSize];
	};

class TPermanentStoreRelocator;
NONSHARABLE_CLASS(CPermanentStoreCollector) : public CBase,public MIncrementalCollector
	{
	enum TState
		{
		EGetFree,ESkip,EInitRelocator,EFillRelocator,EEvalRelocator,EScanRelocator,ERelocateStream,ERelocateToc,
		EFastSort,EFastExtent,EFastRelocate
		};
	enum {EGranularity = 64};
	enum {EExtentStep = 64};
public:
	struct TEntry
		{
		TInt len;
		RPermanentStoreTocIter::TEntry entry;
		};
public:
	static CPermanentStoreCollector* CompactorL(CPermanentStoreCoord& aCoord);
	static CPermanentStoreCollector* ReclaimerL(CPermanentStoreCoord& aCoord);
protected:
	CPermanentStoreCollector(CPermanentStoreCoord& aCoord);
	~CPermanentStoreCollector();
private:
	void DoRelease();
	void DoResetL(TInt& aCount);
	void DoNextL(TInt& aStep,TInt& aTotal);
//
	TInt GetFreeL();
	TInt SkipL(TInt& aTotal);
	TInt InitRelocator();
	TInt FillRelocatorL();
	TInt EvalRelocatorL();
	TInt ScanRelocator();
	TInt RelocateStreamL();
	TBool HaveEnoughSpace() const;
	TInt ExtentL(TInt aStream);
//
// fast compaction
	TInt FastResetL();
	void FastSort();
	void FastExtentL(TInt& aTotal);
	void FastRelocateL(TInt& aTotal);
	TEntry* BestFit(TInt aPos, TInt aExt, TEntry* aFirst, TEntry* aLast);
//
// common utilities
	void RelocateTocL(TInt& aTotal);
	void RelocateStreamL(const TEntry& aReloc, TInt aExtent);
	TInt RelocateL(TInt aStream, TInt aLength, TFrameType16 aType, TInt aExtent);
//
	inline TBool Compacting() const;
	inline CPermanentStoreCoord& Coord() const;
	inline TStreamExchange& Host() const;
private:
	CPermanentStoreCoord* iCoord;
	TUint iCoordGen;
	TStreamExchange* iHost;
	TStreamMark iMark;
	TState iState;
	TInt iFree;
	TInt iEnd;
	TEntry* iNext;
	TEntry* iLast;
	RArray<TEntry> iStreams;
	TPermanentStoreRelocator* iReloc;
	TPermanentStoreStreamIter iIter;
	};

#include "U32PERM.INL"
#endif
