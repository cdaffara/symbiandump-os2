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

#include "UT_STD.H"

// PFS frame-related utilities

TInt SkipLink(TInt anExtent)
//
// anExtent is the end-of-stream, where is the next link pos?
// : add the size of the next frame link, or skip past anchor link
//
	{return anExtent+Min(KSizeFrameDes16,(-anExtent)&KMaskFrameLength16);}

inline TBool SpaceFor(TInt aSpace,TInt anEnd,TInt aLength)
// Check if there is space for at least aLength between links at aSpace and anEnd
	{return SkipLink(aSpace+aLength)<=anEnd;}

TBool Fits(TInt aSpace,TInt anEnd,TInt aLength)
//
// Check that aLength can be relocated into the space
// either an exact fit, or leaves space for a free frame of at least one byte
//
	{
	TInt end=SkipLink(aSpace+aLength);
	return end==anEnd ? ETrue : SpaceFor(end,anEnd,1);
	}

TInt ExtentL(TStreamExchange& aHost,TStreamMark& aMark,TStreamPos aBase,TInt aStream)
//
// scan a stream to discover it's extent
//
	{
	__ASSERT_DEBUG(aMark.RelatesTo(aHost),User::Invariant());
	__ASSERT_DEBUG(aBase>=KStreamBeginning,User::Invariant());
	__ASSERT_DEBUG(aStream>=0,User::Invariant());
//
	aMark.SeekL(aHost,RFrame16Buf::Position(aBase,aStream)-KSizeFrameDes16);
	TFrameDes16 des;
	aMark.ReadL(aHost,&des,KSizeFrameDes16);
	TInt frame=des;
	if ((frame&KMaskFrameType16)!=EFrameData16)
		if ((frame&KMaskFrameType16)!=EFrameDescriptive16)
		{
		aMark.Withdraw(aHost);
		__LEAVE(KErrCorrupt);
		}
//
	TInt anchor=((aStream>>KShiftFrameLength16)+1)<<KShiftFrameLength16;
	do
		{
		frame&=KMaskFrameLength16;
		TInt lim=anchor-aStream;
		if (frame!=KFrameOpen16)
			{
			if (frame>=lim)
				{
				aMark.Withdraw(aHost);
				__LEAVE(KErrCorrupt);
				}
			return aStream+frame;
			}
		aMark.SeekL(aHost,EStreamMark,lim);
		aStream=anchor;
		anchor+=KFrameFullLength16;
		aMark.ReadL(aHost,&des,KSizeFrameDes16);
		frame=des;
		} while ((frame&KMaskFrameType16)==EFrameContinuation16);
	return aStream;
	}



// Class TRelocatorInput
// Used to transfer a frame-set within the file

const TInt KRelocatorBufferSize=0xc00;

NONSHARABLE_CLASS(TRelocatorInput) : public MStreamInput
	{
public:
	inline TRelocatorInput(TStreamExchange& aHost,TStreamMark& aMark);
	void OpenL(TFrameType16 aType,TStreamPos aBase,TInt aPos,TInt aLength,TInt aTerminator);
	void CommitL();
private:
	TInt PushL(const TAny* aPtr,TInt aMaxLength);
	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer);
	void DesL(TFrameType16 aType);
//
	inline TStreamExchange& Host() const;
	inline TStreamMark& Mark();
private:
	TStreamExchange* iHost;
	TStreamMark* iMark;
	TFrameType16 iType;
	TInt iRemain;
	TInt iAnchor;
	TInt iTerminator;
	};

inline TRelocatorInput::TRelocatorInput(TStreamExchange& aHost,TStreamMark& aMark)
	: iHost(&aHost),iMark(&aMark)
	{}
inline TStreamExchange& TRelocatorInput::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}
inline TStreamMark& TRelocatorInput::Mark()
	{
	__ASSERT_DEBUG(iMark!=NULL,User::Invariant());
	return *iMark;
	}

void TRelocatorInput::OpenL(TFrameType16 aType,TStreamPos aBase,TInt aPos,TInt aLength,TInt aTerminator)
//
// initiate the relocated stream
//
	{
	__ASSERT_DEBUG(aType!=EFrameFree16,User::Invariant());
	__ASSERT_DEBUG(aBase>=KStreamBeginning,User::Invariant());
	__ASSERT_DEBUG(aPos>=0,User::Invariant());
	__ASSERT_DEBUG(aLength>0,User::Invariant());
//
	iType=aType;
	iRemain=aLength;
	iTerminator=aTerminator;
	iAnchor=KFrameFullLength16-(aPos&KMaskFrameLength16);
	Mark().SeekL(Host(),RFrame16Buf::Position(aBase,aPos)-KSizeFrameDes16);
	}


void TRelocatorInput::CommitL()
//
// complete the relocated stream
//
	{
	__ASSERT_DEBUG(iRemain==0,User::Invariant());
	__ASSERT_DEBUG(iAnchor>=0,User::Invariant());
//
	if (iTerminator<0)
		return;
//
	TFrameDes16 des[2];
	des[1]=TFrameDes16(iTerminator);
	TInt l=KSizeFrameDes16;
	if (iAnchor<=KSizeFrameDes16)
		l+=iAnchor;
	Mark().WriteL(Host(),(const TUint8*)&des[2]-l,l);
	}

TInt TRelocatorInput::PushL(const TAny*,TInt)
//
// use a passive write through to the host
//
	{
	return 0;
	}

void TRelocatorInput::DesL(TFrameType16 aType)
//
// Write the next frame descriptor
//
	{
	__ASSERT_DEBUG(aType!=EFrameFree16,User::Invariant());
	TFrameDes16 des=TFrameDes16(iRemain<iAnchor ? aType+iRemain : aType+KFrameOpen16);
	Mark().WriteL(Host(),&des,KSizeFrameDes16);
	}

TStreamTransfer TRelocatorInput::ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer)
//
// bulk of the transfer happens here
//
	{
	__ASSERT_DEBUG(!(aTransfer>iRemain),User::Invariant());
	do
		{
		TUint8 buf[KRelocatorBufferSize];
		TInt len=aSource.ReadL(buf,aTransfer[sizeof(buf)]);
		if (len==0)
			break;
		aTransfer-=len;
		const TUint8* p=buf;
		if (iType!=EFrameFree16)
			{
			DesL(iType);
			iType=EFrameFree16;
			}
		for (;;)
			{
			if (iAnchor==0)
				{	// write next anchor
				iAnchor=KFrameFullLength16;
				DesL(EFrameContinuation16);
				}
			TInt frame=Min(len,iAnchor);
			Mark().WriteL(Host(),p,frame);
			iAnchor-=frame;
			iRemain-=frame;
			len-=frame;
			if (len==0)
				break;
			p+=frame;
			}
		} while (aTransfer>0);
	return aTransfer;
	}



// Class TPermanentStoreRelocator
// used to locate streams for relocation in limited memory

class TPermanentStoreRelocator
	{
#if defined(__SMALL_BUNDLE)
	enum {EBundleSize=8-1};
#else
	enum {EBundleSize=64-1};
#endif
public:
	typedef CPermanentStoreCollector::TEntry TEntry;
public:
	void Reset();
	TBool Reset(TInt aPos);
	TInt FillL(CPermanentStoreToc& aToc);
	void EvaluateLengthsL(TStreamExchange& aHost,TStreamMark& aMark,TStreamPos aBase);
//
	TBool FindFit(TInt aSpace,TInt anEnd);
	inline const TEntry* Current() const;
	void Relocated(const TEntry* anEntry);
//
	TInt Extent(TInt aStream) const;
	inline TInt MinLength() const;
private:
	static void Push(TEntry* aHeap,TEntry* aHole,const TEntry& aValue);
	static TEntry PopPush(TEntry* aHeap,TEntry* anEnd,const TEntry& aValue);
private:
	TEntry* iNext;
	const TEntry* iFinish;
	TInt iMore;
	TInt iPos;
	TInt iCurrentMin,iMinLength;
	TEntry iTable[EBundleSize];
	};

inline const TPermanentStoreRelocator::TEntry* TPermanentStoreRelocator::Current() const
	{__ASSERT_DEBUG(iNext>=iTable&&iNext<iFinish,User::Invariant());return iNext;}
inline TInt TPermanentStoreRelocator::MinLength() const
	{return iMinLength;}

void TPermanentStoreRelocator::Reset()
//
// reset the cached length values
//
	{
	iPos=0;
	iMinLength=1;
	}

TBool TPermanentStoreRelocator::Reset(TInt aPos)
//
// reset the iterator for a new scan from aPos
//
	{
	__ASSERT_DEBUG(aPos>=0,User::Invariant());
//
	iCurrentMin=KMaxTInt;
	TEntry* e=iTable;
	if (aPos>=iPos || e==iFinish || aPos<e->entry.ref)
		{	// rescan required
		iFinish=iNext=NULL;
		iMore=-1;
		iPos=aPos;
		return EFalse;
		}

// can use current data
	for (;e->entry.ref!=aPos;++e)
		{
		__ASSERT_DEBUG(e->entry.ref<aPos,User::Invariant());
		__ASSERT_DEBUG(e<iFinish,User::Invariant());
		}
	__ASSERT_DEBUG(e->entry.handle>=0,User::Invariant());
	iNext=e;
	return ETrue;
	}

TBool TPermanentStoreRelocator::FillL(CPermanentStoreToc& aToc)
//
// Fill the table with the next bundle of stream offsets
// return if there are more available
//
	{
	__ASSERT_DEBUG(iNext==iFinish,User::Invariant());
	if (!iMore)
		return EFalse;
//
	__ASSERT_DEBUG(iPos>=0,User::Invariant());
//
	RPermanentStoreTocIter iter(aToc);
	CleanupReleasePushL(iter);

	TInt streams=0;
	TInt from=iPos;
	TEntry* table=iTable;
	TEntry* last=table;
	TEntry* end=table+EBundleSize;
	TEntry entry;
	__DEBUG(entry.len=-1);
	for (iter.ResetL();iter.NextL(entry.entry);)
		{
		if (entry.entry.handle<0)
			continue;
		TInt off=entry.entry.ref;
		if (off<from)
			continue;
		++streams;
		if (last<end)
			Push(table,last++,entry);	// push onto the bottom
		else if (off<table->entry.ref)
			PopPush(table,last,entry);	// replace item in heap
		}
	CleanupStack::PopAndDestroy();

	iMore=streams+table-last;
	if (streams)
		iPos=table->entry.ref+1;		// largest offset in table
	iNext=table;
	iFinish=last;
	while (--last>table)
		*last=PopPush(table,last,*last);
	return streams>0;
	}

void TPermanentStoreRelocator::Push(TEntry* aHeap,TEntry* aHole,const TEntry& aValue)
//
// push aValue onto the heap (which will grow)
//
	{
	TInt ix=aHole+1-aHeap;
	while (aHole>aHeap)
		{
		TInt link=ix-(ix>>1);
		ix-=link;
		TEntry* parent=aHole-link;
		if (parent->entry.ref>=aValue.entry.ref)
			break;
		*aHole=*parent;
		aHole=parent;
		}
	*aHole=aValue;
	}

TPermanentStoreRelocator::TEntry TPermanentStoreRelocator::PopPush(TEntry* aHeap,TEntry* anEnd,const TEntry& aValue)
//
// pop the top and push aValue
//
	{
	TEntry top=*aHeap;
	TEntry* hole=aHeap;
	--anEnd;
	TInt ix=1;
	for (;;)
		{
		TEntry* child=hole+ix;
		ix<<=1;
		if (child>anEnd)
			break;
		if (child<anEnd && (child+1)->entry.ref>child->entry.ref)
			{
			++ix;
			++child;
			}
		if (child->entry.ref<=aValue.entry.ref)
			break;
		*hole=*child;
		hole=child;
		}
	*hole=aValue;
	return top;
	}

void TPermanentStoreRelocator::EvaluateLengthsL(TStreamExchange& aHost,TStreamMark& aMark,TStreamPos aBase)
//
// Evaluate the lengths for all the entries in the table
//
	{
	const TEntry* end=iFinish;
	for (TEntry* e=iTable;e<end;++e)
		{
		__ASSERT_DEBUG(e->entry.handle>=0,User::Invariant());
		__ASSERT_DEBUG(e->entry.ref>=0,User::Invariant());
		__ASSERT_DEBUG(e->len==-1,User::Invariant());
		TInt pos=e->entry.ref;
		e->len=ExtentL(aHost,aMark,aBase,pos)-pos;
		}
	}

TBool TPermanentStoreRelocator::FindFit(TInt aSpace,TInt anEnd)
//
// Find a stream which will fit into the space
//
	{
	const TEntry* end=iFinish;
	TEntry* e;
	for (e=iNext;e<end;++e)
		{
		if (e->entry.handle<0)
			continue;
		TInt len=e->len;
		__ASSERT_DEBUG(len>0,User::Invariant());
		if (Fits(aSpace,anEnd,len))
			{
			iNext=e;
			return ETrue;
			}
		// len has been left behind, check for minimum
		if (len<iCurrentMin)
			iCurrentMin=len;
		}
// if no more data, use current min as cached value
	if (iMore==0)
		iMinLength=iCurrentMin;
	iNext=e;
	return EFalse;
	}

void TPermanentStoreRelocator::Relocated(const TEntry* anEntry)
//
// Relocation has been successful
//
	{
	__ASSERT_DEBUG(anEntry==iNext,User::Invariant());
	TEntry* e=CONST_CAST(TEntry*,anEntry);
	e->entry.handle=-1;
	iNext=e+1;
	}

TInt TPermanentStoreRelocator::Extent(TInt aStream) const
//
// Return this stream extent if we know it
//
	{
	const TEntry* e=iTable;
	if (aStream>=iPos || e==iFinish || aStream<e->entry.ref)
		return -1;	// we don't know it
	for (;e->entry.ref!=aStream;++e)
		{
		__ASSERT_DEBUG(e->entry.ref<aStream,User::Invariant());
		__ASSERT_DEBUG(e<iFinish,User::Invariant());
		}
	__ASSERT_DEBUG(e->entry.handle>=0,User::Invariant());
	__ASSERT_DEBUG(e->len>=0,User::Invariant());
	return aStream+e->len;
	}



// class TPermanentStoreStreamIter

void TPermanentStoreStreamIter::Reset()
//
// reset the iterator for a new scan
//
	{
	iNext=NULL;
	iFinish=NULL;
	iMore=-1;
	iPos=0;
	}

TInt TPermanentStoreStreamIter::FillL(CPermanentStoreToc& aToc)
//
// Fill the table with the next bundle of stream offsets
// return the total streams left to iterate
//
	{
	__ASSERT_DEBUG(iNext==iFinish,User::Invariant());
	if (!iMore)
		return 0;
//
	__ASSERT_DEBUG(iPos>=0,User::Invariant());
//
	RPermanentStoreTocIter iter(aToc);
	CleanupReleasePushL(iter);

	TInt streams=0;
	TInt from=iPos;
	TInt* heap=iTable;
	TInt* last=heap;
	TInt* end=heap+EBundleSize;
	RPermanentStoreTocIter::TEntry entry;
	for (iter.ResetL();iter.NextL(entry);)
		{
		if (entry.handle<0)
			continue;
		TInt off=entry.ref;
		if (off<from)
			continue;
		++streams;
		if (last<end)
			Push(heap,last++,off);	// push onto the bottom
		else if (off<*heap)
			PopPush(heap,last,off);	// replace item in heap
		}
	CleanupStack::PopAndDestroy();

	iMore=streams+(heap-last);
	iPos=*heap+1;		// largest offset in table
	iNext=heap;
	iFinish=last;
	while (--last>heap)
		*last=PopPush(heap,last,*last);
	return streams;
	}

TInt TPermanentStoreStreamIter::Next()
//
// return the next offset, or <0 if the table is empty
//
	{
	__ASSERT_DEBUG(iMore>=0,User::Invariant());
//
	while (iNext<iFinish)
		{
		TInt off=*iNext++;
		if (off>=0)
			return off;
		}
	return -1;
	}

void TPermanentStoreStreamIter::Relocated(TInt aStream)
//
// aStream has been relocated, mark the table
//
	{
	__ASSERT_DEBUG(iMore>=0,User::Invariant());
//
	if (aStream>=iPos)
		return;
	TInt* p=iNext;
	for (;;)
		{
		if (p==iFinish)
			return;
		if (*p>=0)
			break;
		++p;
		}
	if (aStream<*p)
		return;
//
	for (;*p!=aStream;++p)
		{
		__ASSERT_DEBUG(p<iFinish,User::Invariant());
		__ASSERT_DEBUG(*p<aStream,User::Invariant());
		}
	if (p==iNext)
		iNext=p+1;
	else
		*p=-1;
	}

void TPermanentStoreStreamIter::Push(TInt* aHeap,TInt* aHole,TInt aValue)
//
// push aValue onto the heap (which will grow)
//
	{
	TInt ix=aHole+1-aHeap;
	while (aHole>aHeap)
		{
		TInt link=ix-(ix>>1);
		ix-=link;
		TInt* parent=aHole-link;
		if (*parent>=aValue)
			break;
		*aHole=*parent;
		aHole=parent;
		}
	*aHole=aValue;
	}

TInt TPermanentStoreStreamIter::PopPush(TInt* aHeap,TInt* anEnd,TInt aValue)
//
// pop the top and push aValue
//
	{
	TInt top=*aHeap;
	TInt* hole=aHeap;
	--anEnd;
	TInt ix=1;
	for (;;)
		{
		TInt* child=hole+ix;
		ix<<=1;
		if (child>anEnd)
			break;
		if (child<anEnd && *(child+1)>*child)
			{
			++ix;
			++child;
			}
		if (*child<=aValue)
			break;
		*hole=*child;
		hole=child;
		}
	*hole=aValue;
	return top;
	}



// Class CPermanentStoreCollector

CPermanentStoreCollector* CPermanentStoreCollector::CompactorL(CPermanentStoreCoord& aCoord)
	{
	CPermanentStoreCollector* self=ReclaimerL(aCoord);
	CleanupStack::PushL(self);
	self->iReloc=new(ELeave) TPermanentStoreRelocator;
	CleanupStack::Pop();
	return self;
	}

CPermanentStoreCollector* CPermanentStoreCollector::ReclaimerL(CPermanentStoreCoord& aCoord)
	{
	return new(ELeave) CPermanentStoreCollector(aCoord);
	}

CPermanentStoreCollector::CPermanentStoreCollector(CPermanentStoreCoord& aCoord)
	: iCoord(&aCoord),iHost(&aCoord.Host()),iStreams(EGranularity,_FOFF(TEntry,entry.ref))
	{
	Coord().Inc();
	}

CPermanentStoreCollector::~CPermanentStoreCollector()
	{
	delete iReloc;
	iStreams.Close();
	iMark.Withdraw(Host());
	Coord().Dec();
	}

void CPermanentStoreCollector::DoRelease()
	{
	delete this;
	}

void CPermanentStoreCollector::DoResetL(TInt& aCount)
	{
	iMark.Withdraw(Host());
	iMark=KStreamBeginning;
	iCoordGen=Coord().Generation();
	iFree=0;
	TRAPD(r, aCount = FastResetL());
	if (r == KErrNone)
		return;
	if (r != KErrNoMemory)
		User::Leave(r);
// fall back to fixed memory solution
	iState=EGetFree;
	if (Compacting())
		iReloc->Reset();
	iIter.Reset();
	TInt streams=iIter.FillL(Coord().ConsolidateL());
	aCount=streams+1;
	}

const TInt KMaxStepEffort=9;

void CPermanentStoreCollector::DoNextL(TInt& aStep,TInt& aTotal)
//
// Dispatch the next set of operations
//
	{
	if (aStep<=0)
		{
		__DEBUG(Panic(TStorePanic()));
		return;
		}
//
	if (iCoordGen!=Coord().Generation() || Coord().TableL().IsVirtual())
		__LEAVE(KErrNotReady);
//
	TInt effort=0;
	do
		{
		switch (iState)
			{
		default:
			__DEBUG(User::Invariant());
		case EGetFree:
			effort+=GetFreeL();
			break;
		case ESkip:
			effort+=SkipL(aTotal);
			--aStep;
			break;
		case EInitRelocator:
			effort+=InitRelocator();
			break;
		case EFillRelocator:
			effort+=FillRelocatorL();
			break;
		case EEvalRelocator:
			effort+=EvalRelocatorL();
			break;
		case EScanRelocator:
			effort+=ScanRelocator();
			break;
		case ERelocateStream:
			effort+=RelocateStreamL();
			--aStep;
			break;
		case ERelocateToc:
			RelocateTocL(aTotal);
			iStreams.Close();
			__ASSERT_DEBUG(aStep==1,User::Invariant());
			aStep=0;
			return;
		case EFastSort:
			FastSort();
			--aStep;
			return;
		case EFastExtent:
			FastExtentL(aTotal);
			--aStep;
			return;
		case EFastRelocate:
			FastRelocateL(aTotal);
			--aStep;
			return;
			}
		} while(effort<KMaxStepEffort);
	__ASSERT_DEBUG(aStep>=1,User::Invariant());
	}

TInt CPermanentStoreCollector::GetFreeL()
//
// find the end of the free space
//
	{
	__ASSERT_DEBUG(iState==EGetFree,User::Invariant());
	__ASSERT_DEBUG(iFree>=0,User::Invariant());
//
	TInt effort=0;
	iEnd=iIter.Next();
	if (iEnd<0)
		{
		if (iIter.FillL(Coord().ConsolidateL())==0)
			{
			iState=ERelocateToc;	// no more streams
			return effort;
			}
		effort=KMaxStepEffort;
		iEnd=iIter.Next();
		__ASSERT_DEBUG(iEnd>=0,User::Invariant());
		}
	iState=Compacting() && HaveEnoughSpace() ? EInitRelocator : ESkip;
	return effort;
	}

TInt CPermanentStoreCollector::SkipL(TInt& aTotal)
//
// Find some free space, iEnd was the last stream extracted from concat
//
	{
	__ASSERT_DEBUG(iState==ESkip,User::Invariant());
	__ASSERT_DEBUG(iFree>=0&&iFree<=iEnd,User::Invariant());
//
	aTotal+=iEnd-iFree;
	iFree=SkipLink(ExtentL(iEnd));
	iState=EGetFree;
	return 1;		// effort
	}

TInt CPermanentStoreCollector::InitRelocator()
//
// initialise the relocator for the free space
//
	{
	__ASSERT_DEBUG(iState==EInitRelocator,User::Invariant());
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	iState=iReloc->Reset(iEnd) ? EScanRelocator : EFillRelocator;
	return 0;		// no real work at all
	}

TInt CPermanentStoreCollector::FillRelocatorL()
//
// Fill the relocator table
//
	{
	__ASSERT_DEBUG(iState==EFillRelocator,User::Invariant());
	__ASSERT_DEBUG(iFree>=0&&iFree<iEnd,User::Invariant());
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	TBool more=iReloc->FillL(Coord().ConsolidateL());
	iState=more ? EEvalRelocator : ESkip;
	return more ? KMaxStepEffort : 0;
	}

TInt CPermanentStoreCollector::EvalRelocatorL()
//
// evaluate the extents for the relocator
//
	{
	__ASSERT_DEBUG(iState==EEvalRelocator,User::Invariant());
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	iReloc->EvaluateLengthsL(Host(),iMark,Coord().Base());
	iState=EScanRelocator;
	return KMaxStepEffort;
	}

TInt CPermanentStoreCollector::ScanRelocator()
//
// find a stream that will fit
//
	{
	__ASSERT_DEBUG(iState==EScanRelocator,User::Invariant());
	__ASSERT_DEBUG(iFree>=0&&iFree<iEnd,User::Invariant());
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	iState=iReloc->FindFit(iFree,iEnd) ? ERelocateStream : EFillRelocator;
	return 0;
	}

TInt CPermanentStoreCollector::RelocateStreamL()
//
// find and relocate a stream
//
	{
	__ASSERT_DEBUG(iState==ERelocateStream,User::Invariant());
	__ASSERT_DEBUG(iFree>=0&&iFree<iEnd,User::Invariant());
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	const TEntry& e = *iReloc->Current();
	RelocateStreamL(e, iEnd);
	iState=e.entry.ref==iEnd ? EGetFree : HaveEnoughSpace() ? EScanRelocator : ESkip;
	iIter.Relocated(e.entry.ref);
	iReloc->Relocated(&e);
	return KMaxStepEffort;
	}

void CPermanentStoreCollector::RelocateTocL(TInt& aTotal)
//
// relocate the toc - return any wasted bytes
//
	{
	__ASSERT_DEBUG(iState==ERelocateToc,User::Invariant());
	__ASSERT_DEBUG(iFree>=0,User::Invariant());
//
	TInt toc=Coord().iToc+KOffsetTocHeader;
	if (toc<0)
		return;
//
	if (Compacting())
		{
		TInt len=Coord().TableL().Extent()-toc;
		if (Fits(iFree,toc,len))
			{
			RelocateL(toc, len, EFrameDescriptive16, toc);
			Coord().MoveL(iFree-KOffsetTocHeader,iFree + len);
			return;
			}
		}
	// don't move it
	aTotal += toc-iFree;
	}

TBool CPermanentStoreCollector::HaveEnoughSpace() const
	{
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	return SpaceFor(iFree,iEnd,iReloc->MinLength());
	}

TInt CPermanentStoreCollector::ExtentL(TInt aStream)
//
// Check if the relocator knows before scanning the file
//
	{
	if (Compacting())
		{
		TInt ext=iReloc->Extent(aStream);
		if (ext>=0)
			return ext;
		}
	return ::ExtentL(Host(),iMark,Coord().Base(),aStream);
	}

/* relocate a stream into [iFree, aExtent)

During compaction, for each string which is to be moved from position A1 to B1, the sequence of operations is:

1.  Copy stream S1 content from position A1 to position B1 . The copy never overlaps so the old stream content is still good at this point.
2.  Optionally rewrite the file header to state that stream S1 is being relocated to B1 (more about the ‘optional below’)
3.  Overwrite the TOC entry for S1 to state that the content is now at B1

This function completes 3 steps above and will be called again and again for every string to be moved.

In terms of data consistency, first consider the impact of a mid-write failure in any of these steps (when write caching is disabled):
1.  If step #1 only partially completes the file is good as the original content is intact and the new content was being written to otherwise free space
2.  If step #2 only partially completes the header CRC fails and only the TOC reference is considered valid (so the corrupt stream relocation record is ignored).
		 The TOC will be good because it is being overwritten with the same content.
3.  If step #3 only partially completes the entry for S1 in the TOC is corrupt, BUT the relocation record for S1 in the file header is good and will
 override the entry in the TOC.

In all cases the file is never broken by a crash in mid-compaction.

Step #2 is optional – there are many cases when step #3 cannot fail ‘halfway through’ because the underlying media makes atomic block/page based
updates and the write does not cross any block boundaries. In STORE we assume that blocks cannot be smaller than 512 bytes and any flash based
media provides the required behavior. Thus 99% of the step #2 writes are eliminated.

Note that sequencing MATTERS even for just one stream. If the TOC update hits the disk before the content is moved, and then the device fails
we will have a broken file: S1 points to B1 which contains garbage.  Equally in the case where step #2 is required (i.e. when step #3 straddles
a block boundary and could fail) step 2 has to go before the step 3. Otherwise write #3 could go to disk and fail part way through before write #2 
and leave the TOC corrupt with no recovery in the file header.

Consider the case that step 2 was omitted, so the Store relies on step 3 being completed in order to know that S1 is in location B1;
and that no flush is done after step 3. In step 4 the stream S2 is moved – at this point the old space for stream S1 at A1 is considered empty
– and suppose it gets moved from A2 to B2 where B2 overlaps/overwrites A1. If the writes in step 3 and step 4 are re-ordered and the step 3
write does not happen – then the TOC will claim that S1 is still at A1 but this location in the file has been overwritten with data from S2.
A corrupted file.

Based on the knowledge above, it is strongly recommended to set EFileWriteDirectIO bit when opening the file so that the order is maintained
when writing to the file.
*/
void CPermanentStoreCollector::RelocateStreamL(const CPermanentStoreCollector::TEntry& aReloc, TInt aExtent)
	{
	if (Coord().Accessed())	// must have exclusive access to relocate the stream
		__LEAVE(KErrInUse);
//
	TInt end=RelocateL(aReloc.entry.ref,aReloc.len,aReloc.entry.handle == KHandleTocBase ? EFrameDescriptive16 : EFrameData16, aExtent);
	//Step 1, 4,....
	Coord().RelocateL(aReloc.entry.handle, iFree);
	// Step 2 & 3, 5 & 6,...
	iCoordGen=Coord().Generation();	// changed by relocation
	iFree = end;
	}

TInt CPermanentStoreCollector::RelocateL(TInt aStream, TInt aLength, TFrameType16 aType, TInt aExtent)
//
// Relocate a data stream into [iFree, aExtent)
//
	{
	__ASSERT_DEBUG(Fits(iFree,aExtent,aLength),User::Invariant());
	__ASSERT_DEBUG(Compacting(),User::Invariant());
//
	TInt end=SkipLink(iFree+aLength);
	TInt terminator;
	if (end==aExtent)
		terminator=-1;
	else
		{
		TInt anchor=((end>>KShiftFrameLength16)+1)<<KShiftFrameLength16;
		if (aExtent<anchor)
			{
			__ASSERT_DEBUG(aExtent-KSizeFrameDes16-end>0,User::Invariant());
			terminator=EFrameFree16+(aExtent-KSizeFrameDes16-end);
			}
		else
			terminator=EFrameFree16+KFrameOpen16;
		}
//
	RFrame16Buf from(Coord().Base());
	from.Set(Host(),aStream,aStream+aLength,MStreamBuf::ERead);
	from.PushL();
	TRelocatorInput input(Host(),iMark);
	input.OpenL(aType,Coord().Base(),iFree,aLength,terminator);
	from.ReadL(input,aLength);
	input.CommitL();
	CleanupStack::PopAndDestroy();
//
	return end;
	}


TInt CPermanentStoreCollector::FastResetL()
//
// Fill the table with the streams with data in the store
//
	{
	iStreams.Reset();
//
	CleanupClosePushL(iStreams);
	RPermanentStoreTocIter iter(Coord().ConsolidateL());
	CleanupReleasePushL(iter);
	TEntry entry;
	for (iter.ResetL();iter.NextL(entry.entry);)
		{
		if (entry.entry.handle<0)
			continue;
		if (entry.entry.ref<0)
			continue;
		User::LeaveIfError(iStreams.Append(entry));
		}
	CleanupStack::PopAndDestroy(&iter);
	CleanupStack::Pop(&iStreams);
//
	// always have final (toc) step
	iState = ERelocateToc;
	TInt streams = iStreams.Count();
	if (streams == 0)
		return 1;
	iState = EFastSort;
	// ordering is 1 step and evaluating the extents is several more
	TInt count = 2 + (streams + EExtentStep - 1)/EExtentStep;
	if (Compacting())
		count += streams;
	return count;
	}

void CPermanentStoreCollector::FastSort()
	{
	__ASSERT_DEBUG(iState == EFastSort, User::Invariant());
//
	iStreams.SortSigned();
	iNext = &iStreams[0];
	iLast = iNext + iStreams.Count();
	iState = EFastExtent;
	}

void CPermanentStoreCollector::FastExtentL(TInt& aTotal)
//
// Evaluate the lengths for all the streams
// if reclaiming, update aTotal with free space skipped
// return false until we've done the last one
//
	{
	__ASSERT_DEBUG(iState == EFastExtent, User::Invariant());
	__ASSERT_DEBUG(iNext != iLast, User::Invariant());

	TEntry* e = iNext;
	const TEntry* end = Min(iLast, e + EExtentStep);
	do
		{
		TInt ref = e->entry.ref;
		__ASSERT_DEBUG(TUint(iFree)<=TUint(ref),User::Invariant());
		TInt ext = ::ExtentL(Host(), iMark, Coord().Base(), ref);
		e->len = ext - ref;
		if (!Compacting())
			aTotal += ref - iFree;
		iFree = SkipLink(ext);
		} while (++e < end);
	iNext = e;
//
	if (e == iLast)
		{
		if (!Compacting())
			iState = ERelocateToc;
		else
			{
			iNext = &iStreams[0];
			iFree = 0;
			iState = EFastRelocate;
			}
		}
	}

CPermanentStoreCollector::TEntry* CPermanentStoreCollector::BestFit(TInt aPos, TInt aExt, TEntry* aFirst, TEntry* aLast)
//
// [aPos, aExt) is a hole - find the best fit to fill it in [aFirst, aLast)
//
	{
	__ASSERT_DEBUG(aPos <= aExt, User::Invariant());
	TEntry* r = 0;
	if (aPos == aExt)
		return r;
//
	if ((aExt & KMaskFrameLength16) != 0)
		aExt -= KSizeFrameDes16;
	const TInt mingap = Min(KSizeFrameDes16 + 1, (aExt & KMaskFrameLength16));
	TInt rlen = 0;
	TInt avail = aExt - aPos;
	do
		{
		TInt len;
		for (;;)
			{
			len = (--aLast)->len;
			if (len > rlen)
				break;
			if (aFirst == aLast)
				return r;
			}
		TInt d = avail - len;
		if (d < 0)
			continue;
		if (d == 0)			// exact fit
			return aLast;
		if (d < mingap)
			continue;
		r = aLast;
		rlen = len;
		} while (aFirst != aLast);
	return r;
	}

void CPermanentStoreCollector::FastRelocateL(TInt& aTotal)
//
// Look for a stream to move. Either move a stream or fail to find a fit before returning
// fill holes from the front of the file with the biggest block that will fit (inverted current algorithm)
// return true when the last hole has been looked at
//
	{
	__ASSERT_DEBUG(iState == EFastRelocate, User::Invariant());
	__ASSERT_DEBUG(iNext != iLast, User::Invariant());

	TEntry* e = iNext;
	TInt ext = e->entry.ref;
	__ASSERT_DEBUG(ext >= 0, User::Invariant());

	TEntry* r = BestFit(iFree, ext, e, iLast);
	if (!r)
		{
		// Nothing fits, accumulate free space
		aTotal += ext - iFree;
		iFree = SkipLink(ext + e->len);
		}
	else
		{
		// lets move it
		RelocateStreamL(*r, ext);
		if (r != e)
			{
			// relocated a stream other than the one terminating the current hole
			// mark it at moved
			r->entry.ref = -1;
			r->len = -1;
			return;
			}
		}
	// skip through any relocated streams
	while (++e < iLast && e->entry.ref < 0)
		;
	iNext = e;
	if (e == iLast)
		iState = ERelocateToc;
	}
