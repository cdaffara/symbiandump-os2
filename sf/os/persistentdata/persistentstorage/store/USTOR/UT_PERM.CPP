// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include "UT_STD.H"

//The default media block size, used in the computations, if the file system guarantees atomic "block write" file operations.
const TInt KDefaultMediaBlockSize = 512;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////              TPermanentStoreHeader               /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Returns false, if:
// - the dirty bit is set (indicates commit phase #2 or #3 is not complete);
// - the CRC check fails ("backup TOC ref", "handle" and "TOC ref" are protected by 16-bit CRC in the permanent store file header);
TBool TPermanentStoreHeader::IsValid() const
	{
	if (IsDirty())
		return EFalse;
//
	TUint16 crc=0;
	Mem::Crc(crc,Ptr(),_FOFF(TPermanentStoreHeader,iCrc));
	return crc==iCrc;
	}

//Sets the "backup TOC ref", "handle" and "TOC ref" in current TPermanentStoreHeader object.
//16-bit CRC is calculated, based on the values of the input parameters, and stored together with them in the 
//TPermanentStoreHeader object.
void TPermanentStoreHeader::Set(TInt aBackupToc,TInt aHandle,TInt aReference)
	{
	iBackup=TUint32(aBackupToc)<<1;
	iHandle=aHandle;
	iRef=aReference;
	iCrc=0;
	Mem::Crc(iCrc,Ptr(),_FOFF(TPermanentStoreHeader,iCrc));
	__ASSERT_DEBUG(IsValid(),User::Invariant());
	}

const TInt KTocGranularity=12;

struct STocEntry
	{
	TInt32 handle;
	TInt32 ref;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////              CPermanentStoreToc               ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TInt CPermanentStoreToc::TEntry::Compare(const TEntry& aLeft, const TEntry& aRight)
//
// Ordering for Stream handles, return <0,0,>0 depending on comparison
// The index part of the handle is only 24 bits, so just subtract the handles for result
//
	{
	return (aLeft.handle&KMaskHandleIndex) - (aRight.handle&KMaskHandleIndex);
	}

CPermanentStoreToc* CPermanentStoreToc::NewL(TStreamPos aBase,TStreamExchange& aHost,TInt aToc,TInt aBaseReloc)
	{
	CPermanentStoreToc* table=new(ELeave) CPermanentStoreToc(aBase,aHost);
	CleanupStack::PushL(table);
	table->ConstructL(aToc,aBaseReloc);
	CleanupStack::Pop();
	return table;
	}

TBool CPermanentStoreToc::IsDelta() const
//
// Report whether the current TOC should be written as a delta, or a base TOC
//
	{
	TInt delta = iEntries.Count();
	if (delta >= KTocDeltaCap)
		return EFalse;
	TInt magic = iMagic + delta;
	if (magic > KMaxTocDeltaMagic)
		return EFalse;
	return magic*KSizeTocDeltaEntry <= iCount*KSizeTocEntry - KSizeTocDeltaExtra;
	}

void CPermanentStoreToc::Move(TInt aToc,TInt anExtent)
//
// Final stage of compaction, update to address new TOC
//
	{
	__ASSERT_DEBUG(anExtent<=iOff&&anExtent>=aToc&&anExtent-(aToc+KOffsetTocHeader)==iExt-iOff,User::Invariant());
	if (!HasDelta())
		{
		// Base-toc only, update the base toc position as well
		TInt window=iWindow-iTocExt;
		iTocOff=aToc+KOffsetTocHeader;
		iTocExt=anExtent;
		iWindow=anExtent+window;
		}
	iOff = aToc+KOffsetTocHeader;
	iExt = anExtent;
	}

TInt CPermanentStoreToc::RealizeL(TInt aPrimary,TInt anExtent) const
	{
	__ASSERT_DEBUG(IsVirtual(),User::Invariant());
	RFrame16Buf buf(Base());
	buf.PushL();
	const TBool delta = IsDelta();
	aPrimary &= KMaskStreamIdValue;
	if (delta)
		aPrimary |= KTocDelta;
	STocHead h;
	h.primary=TInt32(aPrimary);
	h.avail=TInt32(iAvail);
	h.count=TUint32(iCount);
//
	TInt off;
	if (delta)
		off=DeltaL(buf,anExtent,h);
	else
		off=RewriteL(buf,anExtent,h);
//
	buf.SynchL();
	CleanupStack::PopAndDestroy(&buf);
	return off-KOffsetTocHeader;
	}

void CPermanentStoreToc::Adopt(TInt aToc,TInt aPrimary)
//
// Final stage of Commit - all file changes have been successful
// Record the new Toc location and reset the changed flag
//
	{
	__ASSERT_DEBUG(aToc+KOffsetTocHeader>=iExt,User::Invariant());

	if (IsDelta())
		{
		// Adjust the TOC-delta location
		TInt c = iEntries.Count();
		iOff=aToc+KOffsetTocHeader;
		iExt=aToc + KSizeTocDeltaExtra + KSizeTocDeltaEntry*c;
		if (c > KTocDeltaMagic)
			iMagic += c - KTocDeltaMagic;
		}
	else
		{
		// Adjust all the TOC data and reset the delta-set
		TInt window = iTocOff>=0 ? iWindow-iTocOff : -KOffsetTocHeader;
		iTocOff=iOff=aToc+KOffsetTocHeader;
		iTocExt=iExt=aToc+KSizeTocEntry*iCount;
		iWindow = iTocOff + window;
		iEntries.Reset();
		iMagic = 0;
		}
	iPrimary=aPrimary;
	__ASSERT_DEBUG(!IsVirtual(),User::Invariant());
	}

TInt CPermanentStoreToc::AllocL(TInt anOffset)
	{
	TEntry& entry=DoAllocL();
	entry.ref=anOffset;
	return entry.handle;
	}

TInt CPermanentStoreToc::AllocL()
	{
	TEntry& entry=DoAllocL();
	entry.ref=KFrameNonexistent16;
	return entry.handle|=KHandleInvalid;
	}

void CPermanentStoreToc::Cancel(TInt aHandle)
	{
	__ASSERT_DEBUG(aHandle<0&&IsVirtual(),User::Invariant());
	TEntry* entry=Entry(aHandle);
	__ASSERT_DEBUG((entry!=NULL) && (entry->handle==aHandle),User::Invariant());
	entry->ref=iAvail;
	iAvail=aHandle;
	}

void CPermanentStoreToc::FreeL(TInt aHandle)
	{
	__ASSERT_DEBUG(aHandle>0,User::Invariant());
	TEntry& entry=FetchL(aHandle);
	aHandle|=KHandleInvalid;
	entry.handle=aHandle;
	entry.ref=iAvail;
	iAvail=aHandle;
	Changed();
	}

TInt CPermanentStoreToc::AtL(TInt aHandle) const
	{
	__ASSERT_DEBUG(aHandle>0,User::Invariant());

	// check the delta table
	const TEntry* entry=Entry(aHandle);
	if (entry!=NULL )
		{
		// if there is an entry in the delta table, but its
		// not an exact match leave with KErrNotFound
		// This happens when the delta indicates it's been deleted
		if (entry->handle!=aHandle)
			User::Leave(KErrNotFound);
		
		return entry->ref;
		}
//
	// if it's not in the delta table check the base TOC
	return DoAtL(aHandle);
	}

void CPermanentStoreToc::PutL(TInt aHandle, TInt anOffset, TPut aCheck)
//
// Used by compaction to update a single stream reference IN-PLACE in the TOC
// We need to check in which TOC to make the update
//
	{
	__ASSERT_DEBUG(!IsVirtual(),User::Invariant());
	__ASSERT_DEBUG(aHandle>0,User::Invariant());
	__ASSERT_DEBUG(aHandle!=KHandleTocBase || HasDelta(),User::Invariant());

	if (aHandle == KHandleTocBase)
		{
		// update the TOC-base link
		PutTocL(anOffset - KOffsetTocHeader, aCheck);
		if (iTocOff != anOffset)
			{
			TInt size = iTocExt - iTocOff;
			TInt window = iWindow - iTocOff;
			iTocOff = anOffset;
			iTocExt = anOffset + size;
			iWindow = anOffset + window;
			}
		return;
		}

	TEntry e;
	e.handle=aHandle;
	TInt i;
	if (iEntries.FindInOrder(e,i,&TEntry::Compare)==0)
		{
		// update TOC-delta entry
		TEntry& entry=iEntries[i];
		if (entry.handle==aHandle && entry.ref != anOffset)
			{
			PutDeltaL(i,aHandle,anOffset);
			entry.ref=anOffset;
			}
		return;
		}

	// update TOC-base entry
	if (aCheck==EWrite || DoAtL(aHandle)!=anOffset)
		PutBaseL(aHandle,anOffset);
	}

TInt CPermanentStoreToc::GetL(TInt aHandle)
	{
	__ASSERT_DEBUG(aHandle>0,User::Invariant());
	TEntry& entry=FetchL(aHandle);
	return entry.ref;
	}

TInt CPermanentStoreToc::Set(TInt aHandle,TInt anOffset)
	{
	__ASSERT_DEBUG(aHandle!=0,User::Invariant());
	TEntry* entry=Entry(aHandle);
	__ASSERT_DEBUG((entry!=NULL) && (entry->handle==aHandle),User::Invariant());
	aHandle&=KMaskStreamIdValue;
	entry->handle=aHandle;
	entry->ref=anOffset;
	Changed();
	return aHandle;
	}

CPermanentStoreToc::CPermanentStoreToc(TStreamPos aBase,TStreamExchange& aHost)
	: /*iPrimary(0),iAvail(0),iCount(0),*/iEntries(KTocGranularity),
		iBase(aBase),iHost(&aHost),iOff(KFrameNonexistent16)/*,iExt(0)*/,
		iTocOff(KFrameNonexistent16)/*,iTocExt(0),iWindow(0)*/
	{}

CPermanentStoreToc::~CPermanentStoreToc()
	{
	iEntries.Close();
	}

void CPermanentStoreToc::ConstructL(TInt aToc,TInt aBaseReloc)
//
// Read and validate the TOC header at aToc.
// aBaseReloc may contain a relocated TOC-base offset, which should override the one in a TOC-delta
//
	{
	if (aToc==0)
		return;
	__ASSERT_DEBUG(aToc>0&&iEntries.Count()==0,User::Invariant());
	RFrame16Buf buf(Base());
	aToc+=KOffsetTocHeader;
	buf.OpenL(Host(),aToc,EFrameDescriptive16|buf.ERead);
	buf.PushL();
	RReadStream stream(&buf);
	STocHead h;
	stream.ReadL((TUint8*)&h,sizeof(STocHead)); // platform dependency
	if ((h.primary&~(KMaskStreamIdValue|static_cast<TUint>(KTocDelta)))!=0||
		h.avail>0||(h.avail&KMaskHandleClear)!=0||(h.count&~KMaskHandleIndex)!=0)
		__LEAVE(KErrCorrupt);
//
	iPrimary=TInt(h.primary) & ~KTocDelta;
	iAvail=TInt(h.avail);
	iCount=TInt(h.count);
	iOff = aToc;
//
	if (h.primary & KTocDelta)
		{
		// This is a TOC-delta
		aToc = InternalizeL(stream, aBaseReloc) + KOffsetTocHeader;

		// Now locate and validate the base TOC
		buf.Release();
		buf.OpenL(Host(),aToc,EFrameDescriptive16|buf.ERead);
		stream.ReadL((TUint8*)&h,sizeof(STocHead)); // platform dependency
		if ((h.primary&~KMaskStreamIdValue)!=0||(h.count&~KMaskHandleIndex)!=0||
			h.avail>0||(h.avail&KMaskHandleClear)!=0||TInt(h.count)>iCount)
			__LEAVE(KErrCorrupt);
		}
	TInt size = KSizeTocEntry*TInt(h.count);
// Eagerly load the first few TOC entries into the cache
// This is good as it almost certainly lies in the file buffer already
	if (size>0)
		stream.ReadL(&iBuf[0],Min(size,TInt(KSizeTocBuf)));
	size-=KOffsetTocHeader;
	if (buf.SizeL()!=size)
		__LEAVE(KErrCorrupt);
	iTocOff=aToc;
	iWindow=aToc-KOffsetTocHeader;
	iTocExt=aToc+size;
	if (iExt == 0)
		iExt = iTocExt;		// set extent for non-delta TOC
//
	CleanupStack::PopAndDestroy(&buf);
	}

TInt CPermanentStoreToc::InternalizeL(RReadStream& aIn, TInt aBaseReloc)
//
// Load and validate the delta-toc table
//
	{
	TInt tocoff = aIn.ReadInt32L();
	if (aBaseReloc != KFrameNonexistent16)
		tocoff = aBaseReloc - KOffsetTocHeader;
	if (tocoff<0||tocoff>=iOff)
		__LEAVE(KErrCorrupt);
	iMagic = aIn.ReadUint16L();
	if (!IsDelta())
		__LEAVE(KErrCorrupt);
	TInt n = aIn.ReadUint8L();
	TInt size = -KOffsetTocHeader + KSizeTocDeltaExtra + KSizeTocDeltaEntry * n;
	if (aIn.Source()->SizeL()!=size)
		__LEAVE(KErrCorrupt);
	iExt = iOff + size;
	TInt last = 0;
	while (--n >= 0)
		{
		STocEntry e;
		aIn.ReadL((TUint8*)&e,KSizeTocDeltaEntry);	// platform dependency
		if ((e.handle&KMaskHandleClear)!=0||e.handle>=0&&(e.ref<KFrameNonexistent16)||
			e.handle<0&&(e.ref>0||(e.ref&KMaskHandleClear)!=0))
			__LEAVE(KErrCorrupt);
		TInt i = e.handle&KMaskHandleIndex;
		if (i <= last || i > iCount)
			__LEAVE(KErrCorrupt);
		last = i;
		TEntry entry;
		entry.handle = TInt(e.handle);
		entry.ref = TInt(e.ref);
		User::LeaveIfError(iEntries.Append(entry));
		}

	return tocoff;
	}

TInt CPermanentStoreToc::DeltaL(RFrame16Buf& aBuf,TInt aExtent,const STocHead& aHead) const
//
// Write the TOC-delta
//
	{
	TInt off=aBuf.ExtendL(Host(),aExtent,EFrameDescriptive16|aBuf.EWrite);
	RWriteStream out(&aBuf);
	out.WriteL((TUint8*)&aHead,sizeof(STocHead)); // platform dependency
	out.WriteInt32L(iTocOff - KOffsetTocHeader);
	TInt n = iEntries.Count();
	TInt magic = iMagic;
	if (n > KTocDeltaMagic)
		magic += n - KTocDeltaMagic;
	__ASSERT_DEBUG(magic <= KMaxTocDeltaMagic,User::Invariant());
	__ASSERT_DEBUG(n <= (TInt)KMaxTUint8, User::Invariant());
	out.WriteUint16L(magic);
	out.WriteUint8L(n);
	for (int i = 0; i < n; ++i)
		{
		const TEntry& entry = iEntries[i];
		STocEntry e;
		e.handle=TInt32(entry.handle);
		e.ref=TInt32(entry.ref);
		out.WriteL((TUint8*)&e,KSizeTocDeltaEntry); // platform dependency
		}
	return off;
	}

TInt CPermanentStoreToc::RewriteL(RFrame16Buf& aBuf,TInt aExtent,const STocHead& aHead) const
//
// Write the TOC-base
//
	{
	const TInt KElementsRewrite=304;
	const TInt KSizeRewriteBuf=KElementsRewrite*KSizeTocEntry;
	TUint8 toc[KSizeRewriteBuf];

	RFrame16Buf buf(Base());
	buf.PushL();
	TInt oldsize = 0;
	TInt window = 0;
	if (iTocOff>=0)
		{
		oldsize = iTocExt-iTocOff+KOffsetTocHeader;
		window = iWindow + KOffsetTocHeader - iTocOff;
		if (oldsize <= Min(KSizeTocBuf,KSizeRewriteBuf) && window == 0)
			{
			// the old TOC is already in the toc-base cache, no need to read it
			Mem::Copy(&toc[0],&iBuf[0],oldsize);
			oldsize = 0;	// this prevents the read request
			}
		else
			{
			buf.Set(Host(),iTocOff,iTocExt,EFrameDescriptive16|buf.ERead);
			buf.SeekL(buf.ERead,TStreamPos(-KOffsetTocHeader));
			}
		}
	RReadStream in(&buf);
	// defer the initialisation of the write in roder to improve file buffering performance
	RWriteStream out(&aBuf);
	TInt off=-1;
	TInt size=iCount*KSizeTocEntry;

	for (TInt base=0,delta=0;base<size;base+=KSizeRewriteBuf)
		{
		// fill buffer with old TOC data
		if (base < oldsize)
			in.ReadL(&toc[0],Min(KSizeRewriteBuf,oldsize-base));
		// apply changes to this block
		for (TInt n=iEntries.Count();delta<n;++delta)
			{
			const TEntry& entry=iEntries[delta];
			TInt pos = (entry.handle&KMaskHandleIndex)*KSizeTocEntry - KSizeTocEntry - base;
			__ASSERT_DEBUG(pos>=0,User::Invariant());
			if (pos>=KSizeRewriteBuf)
				break;
			STocEntry e;
			e.handle=TInt32(entry.handle);
			e.ref=TInt32(entry.ref);
			Mem::Copy(&toc[pos],(const TUint8*)&e+KSizeHandleIndex,KSizeTocEntry);
			pos += base - window;
			if (TUint(pos) < TUint(KSizeTocBuf))
				Mem::Copy(MUTABLE_CAST(TUint8*,&iBuf[pos]),(const TUint8*)&e+KSizeHandleIndex,KSizeTocEntry);
			}
		// write buffer to file
		if (off == -1)
			{
			// initialise writing
			off=aBuf.ExtendL(Host(),aExtent,EFrameDescriptive16|aBuf.EWrite);
			out.WriteL((TUint8*)&aHead,sizeof(STocHead)); // platform dependency
			}
		out.WriteL(&toc[0],Min(KSizeRewriteBuf,size-base));
		}
//
	CleanupStack::PopAndDestroy(&buf);
	return off;
	}

CPermanentStoreToc::TEntry* CPermanentStoreToc::Entry(TInt aHandle)
	{
	TEntry e;
	e.handle=aHandle;
	TInt i;
	if (iEntries.FindInOrder(e,i,&TEntry::Compare)!=0)
		return NULL;
//
	TEntry& entry=iEntries[i];
	
	// allow entry to return a pointer even if not an exact match
	return &entry;
	}

CPermanentStoreToc::TEntry& CPermanentStoreToc::FetchL(TInt aHandle)
	{
	TEntry e;
	e.handle=aHandle;
	TInt i;
	if (iEntries.FindInOrder(e,i,&TEntry::Compare)!=0)
		{
		e.ref=DoAtL(aHandle);
		User::LeaveIfError(iEntries.Insert(e,i));
		}
	TEntry& entry=iEntries[i];
	if (entry.handle!=aHandle)
		__LEAVE(KErrNotFound);
//
	return entry;
	}

CPermanentStoreToc::TEntry& CPermanentStoreToc::DoAllocL()
	{
	TInt handle=iAvail;
	TEntry* entry;
	if (handle==0)
		{
		__ASSERT_DEBUG(iEntries.Count()==0||(iEntries[iEntries.Count()-1].handle&KMaskHandleIndex)<=iCount,User::Invariant());
		User::LeaveIfError(iEntries.Append(TEntry()));
		entry=&iEntries[iEntries.Count()-1];
		handle=++iCount;
		}
	else
		{
		entry=&FetchL(handle);
		handle=(handle+KIncHandleGen)&KMaskStreamIdValue;
//
		TInt avail=entry->ref;
		if (avail>0||(avail&KMaskHandleClear)!=0)
			__LEAVE(KErrCorrupt);
//
		iAvail=avail;
		}
	entry->handle=handle;
	Changed();
	return *entry;
	}

TInt CPermanentStoreToc::DoAtL(TInt aHandle) const
	{
	TInt off=iTocOff;
	if (off<0)
		__LEAVE(KErrNotFound);
//
	off+=-KOffsetTocHeader-KSizeTocEntry+KSizeTocEntry*(aHandle&KMaskHandleIndex);
	__ASSERT_DEBUG(off>=iTocOff-KOffsetTocHeader,User::Invariant());
	if (off>=iTocExt)
		__LEAVE(KErrNotFound);
//
	TInt window=iWindow;
	if (off-window<0||off-window>=KSizeTocBuf)
		{
		TInt len=iTocOff-KOffsetTocHeader;
		window=Max(len,Min(off-KBackTocBuf,iTocExt-KSizeTocBuf));
		len=Min(iTocExt-len,TInt(KSizeTocBuf));
//
		RFrame16Buf buf(Base());
		buf.Set(Host(),iTocOff,iTocExt,EFrameDescriptive16|buf.ERead);
		buf.PushL();
		buf.SeekL(buf.ERead,TStreamPos(window-iTocOff));
		RReadStream stream(&buf);
		MUTABLE_CAST(TInt&,iWindow)=iTocExt;
		stream.ReadL(MUTABLE_CAST(TUint8*,&iBuf[0]),1); // assume half decent read-ahead buffering
		stream.ReadL(MUTABLE_CAST(TUint8*,&iBuf[1]),len-1);
		MUTABLE_CAST(TInt&,iWindow)=window;
		CleanupStack::PopAndDestroy();
		}
	STocEntry e;
	e.handle=aHandle;
	Mem::Copy((TUint8*)&e+KSizeHandleIndex,&iBuf[off-window],KSizeTocEntry); // platform dependency
	if ((e.handle&KMaskHandleClear)!=0||e.handle>=0&&(e.ref<KFrameNonexistent16)||
		e.handle<0&&(e.ref>0||(e.ref&KMaskHandleClear)!=0))
		__LEAVE(KErrCorrupt);
//
	if (TInt(e.handle)!=aHandle)
		__LEAVE(KErrNotFound);
//
	return TInt(e.ref);
	}

void CPermanentStoreToc::PutBaseL(TInt aHandle,TInt aReference)
//
// Update a TOC-base entry
//
	{
	__ASSERT_DEBUG(iTocOff>=0,User::Invariant());

	TInt pos=-KOffsetTocHeader-KSizeTocEntry+KSizeTocEntry*(aHandle&KMaskHandleIndex);
	RFrame16Buf buf(Base());
	buf.Set(Host(),iTocOff,iTocExt,EFrameDescriptive16|buf.EWrite);
	buf.PushL();
	buf.SeekL(buf.EWrite,TStreamPos(pos));
	RWriteStream stream(&buf);
	STocEntry e;
	e.handle=aHandle;
	e.ref=aReference;
	stream.WriteL((TUint8*)&e+KSizeHandleIndex,KSizeTocEntry); // platform dependency
	CleanupStack::PopAndDestroy();
	TInt off=pos+iTocOff-iWindow;
	if (off>=0&&off<KSizeTocBuf)
		Mem::Copy(&iBuf[off],(const TUint8*)&e+KSizeHandleIndex,KSizeTocEntry); // platform dependency
	}

void CPermanentStoreToc::PutDeltaL(TInt aPos, TInt aHandle, TInt aReference)
//
// Update a single stream reference IN-PLACE in the TOC-delta
//
	{
	__ASSERT_DEBUG(HasDelta(),User::Invariant());
	__ASSERT_DEBUG(iOff>=0,User::Invariant());
	RFrame16Buf buf(Base());
	buf.Set(Host(),iOff,iExt,EFrameDescriptive16|buf.EWrite);
	buf.PushL();
	buf.SeekL(buf.EWrite,TStreamPos(-KOffsetTocHeader + KSizeTocDeltaExtra + KSizeTocDeltaEntry*aPos));
	RWriteStream stream(&buf);
	STocEntry e;
	e.handle=aHandle;
	e.ref=aReference;
	stream.WriteL((TUint8*)&e,KSizeTocDeltaEntry); // platform dependency
	CleanupStack::PopAndDestroy();
	}

void CPermanentStoreToc::PutTocL(TInt aTocBase, TPut aCheck)
//
// Update the base TOC link in the TOC-delta
//
	{
	__ASSERT_DEBUG(HasDelta(),User::Invariant());
	__ASSERT_DEBUG(iOff>=0,User::Invariant());
	RFrame16Buf buf(Base());
	buf.Set(Host(),iOff,iExt,EFrameDescriptive16|buf.EWrite|buf.ERead);
	buf.PushL();
	buf.SeekL(buf.EWrite|buf.ERead,TStreamPos(-KOffsetTocHeader));
	RReadStream in(&buf);
	if (aCheck==EWrite || in.ReadInt32L() != aTocBase)
		{
		RWriteStream out(&buf);
		out.WriteInt32L(aTocBase);
		}
	CleanupStack::PopAndDestroy();
	}

// Used prior to PutL to determine exactly where in the file the TOC update will be written.
// This may be used to eliminate the pre-put write of the relocation information in the file
// header in situations where the write is entirely within an atomic file block.
//
// The detailed numbers used to offset into the TOCs are dependant on the TOC formats, as
// implicated by other members of this class
TInt CPermanentStoreToc::RefSpan(TInt aHandle,TInt& aLength)
    {
    __ASSERT_DEBUG(!IsVirtual(),User::Invariant());
    __ASSERT_DEBUG(aHandle>0,User::Invariant());

    if (aHandle == KHandleTocBase)
        {    // locate the TOC-base link
        __ASSERT_DEBUG(HasDelta(),User::Invariant());
        __ASSERT_DEBUG(iOff>=0,User::Invariant());
        aLength = sizeof(TInt32);
        return RFrame16Buf::Position(Base(), iOff - KOffsetTocHeader).Offset();
        }

    TEntry e;
    e.handle=aHandle;
    TInt ix;
    if (iEntries.FindInOrder(e, ix, &TEntry::Compare) == 0)
        {    // locate TOC-delta entry
        TEntry& entry=iEntries[ix];
        if (entry.handle==aHandle)
            {
            __ASSERT_DEBUG(HasDelta(),User::Invariant());
            __ASSERT_DEBUG(iOff>=0,User::Invariant());
            aLength = KSizeTocDeltaEntry;
            return RFrame16Buf::Position(Base(),iOff - KOffsetTocHeader + KSizeTocDeltaExtra + KSizeTocDeltaEntry*ix).Offset();
            }
        }

    // locate the TOC-base entry
    __ASSERT_DEBUG(iTocOff>=0,User::Invariant());
    aLength = KSizeTocEntry;
    return RFrame16Buf::Position(Base(),iTocOff-KOffsetTocHeader-KSizeTocEntry+KSizeTocEntry*(aHandle&KMaskHandleIndex)).Offset();
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////              RPermanentStoreTocIter               ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RPermanentStoreTocIter::RPermanentStoreTocIter(const CPermanentStoreToc& aTable)
	: iTable(aTable), iBuf(aTable.Base()), iDelta(0), iDeltaEnd(0)
	{
	TInt off=aTable.iTocOff;
	__ASSERT_DEBUG(off>=KFrameNonexistent16,User::Invariant());
	TInt ext=off<0?off:aTable.iTocExt;
	iBuf.Set(aTable.Host(),off,ext,EFrameDescriptive16|iBuf.ERead);
	}

void RPermanentStoreTocIter::Release()
	{
	iBuf.Release();
	}

void RPermanentStoreTocIter::ResetL()
	{
	iNext = iIndex = 1;
	if (iBuf.Offset()<0)
		{
		iCount=0;
		return;
		}
//
	RReadStream stream(&iBuf);
	STocHead h;
	stream.ReadL((TUint8*)&h,sizeof(STocHead)); // platform dependency
	if (h.primary<0||(h.primary&~KMaskStreamIdValue)!=0||
		h.avail>0||(h.avail&KMaskHandleClear)!=0||(h.count&~KMaskHandleIndex)!=0)
		__LEAVE(KErrCorrupt);
//
	const CPermanentStoreToc& table=iTable;
	if (table.HasDelta())
		{
		TInt c = table.iEntries.Count();
		if (c)
			{
			iDelta = &table.iEntries[0];
			iDeltaEnd = iDelta + c;
			}
		iIndex = 0;
		}
	iCount = iTable.iCount;
	}

TBool RPermanentStoreTocIter::NextL(TEntry& anEntry)
	{
	TInt i=iIndex;
	__ASSERT_DEBUG(iCount>=0,User::Invariant());
	if (i == 0)
		{
		// report TOC-base as a 'stream'
		anEntry.handle = KHandleTocBase;
		anEntry.ref = iTable.iTocOff;
		iIndex = 1;
		return ETrue;
		}

	__ASSERT_DEBUG(i>0,User::Invariant());
	if (i>iCount)
		return EFalse;
//
	const TEntry* d = iDelta;
	if (d != iDeltaEnd && (d->handle&KMaskHandleIndex) == i)
		{
		anEntry = *d;
		iDelta = d+1;
		iIndex = i+1;
		return ETrue;
		}
//
	RReadStream stream(&iBuf);
	TInt skip = i - iNext;
	if (skip > 0)
		stream.ReadL(KSizeTocEntry * skip);
	STocEntry e;
	e.handle=i;
	stream.ReadL((TUint8*)&e+KSizeHandleIndex,KSizeTocEntry); // platform dependency
	if ((e.handle&KMaskHandleClear)!=0||e.handle>=0&&e.ref<KFrameNonexistent16||
		e.handle<0&&(e.ref>0||(e.ref&KMaskHandleClear)!=0))
		__LEAVE(KErrCorrupt);
//
	iNext = iIndex = i+1;
	anEntry.handle=TInt(e.handle);
	anEntry.ref=TInt(e.ref);
	return ETrue;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////              TPermanentStoreCache               //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const TPermanentStoreCache::TItem* TPermanentStoreCache::At(TInt aHandle) const
	{
	const TItem* item=&iItems[0];
	TInt i=1;
	TInt bit=1;
	while (item->handle!=aHandle)
		{
		__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());
		TInt step=(aHandle&bit)?i+1:i;
		bit<<=1;
		i+=step;
		if (i>EItems)
			return NULL;
//
		item+=step;
		}
	__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());
	return item;
	}

void TPermanentStoreCache::Relocated(TInt aHandle,TInt anOffset)
	{
	const TItem* item=At(aHandle);
	if (item)
		{
		// update the cache item with the new offset
		// As 'extent' may only be a partial extent of a fragmented frame
		// which is no longer fragmented, we cannot simply shift this by the same
		// amount as 'offset'. Simplest to reset this to 'unknown', i.e. 0
		const_cast<TItem*>(item)->offset = anOffset;
		const_cast<TItem*>(item)->extent = 0;
		}
	}

void TPermanentStoreCache::Put(const TItem* anItem,TInt anOffset,TInt anExtent)
	{
	TInt handle=anItem->handle;
	TItem* item=&iItems[0];
	TInt i=1;
	TInt bit=1;
	while (item != anItem)
		{
		__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());
		TInt step=(handle&bit)?i+1:i;
		bit<<=1;
		i+=step;
		__ASSERT_DEBUG(i<=EItems,User::Invariant());
		item+=step;
		}
	__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());

	for(;;)
		{
		__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());
		TInt step=(handle&bit)?i+1:i;
		bit<<=1;
		i+=step;
		if (i>EItems)
			break;
//
		TItem* hole=item;
		item+=step;
		*hole=*item;
		};
	item->handle=handle;
	item->offset=anOffset;
	item->extent=anExtent;
	__ASSERT_DEBUG(At(handle)==item,User::Invariant());
	}

void TPermanentStoreCache::Add(TInt aHandle,TInt anOffset,TInt anExtent)
	{
	TItem* item=&iItems[0];
	TInt i=1;
	TInt bit=1;
	for(;;)
		{
		__ASSERT_DEBUG(item==&iItems[i-1]&&item->handle!=aHandle,User::Invariant());
		TInt step=(aHandle&bit)?i+1:i;
		bit<<=1;
		i+=step;
		if (i>EItems)
			break;
//
		TItem* hole=item;
		item+=step;
		*hole=*item;
		};
	item->handle=aHandle;
	item->offset=anOffset;
	item->extent=anExtent;
	__ASSERT_DEBUG(At(aHandle)==item,User::Invariant());
	}

void TPermanentStoreCache::Remove(TInt aHandle)
	{
	TItem* item=&iItems[0];
	TInt i=1;
	TInt bit=1;
	while (item->handle!=aHandle)
		{
		__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());
		TInt step=(aHandle&bit)?i+1:i;
		bit<<=1;
		i+=step;
		if (i>EItems)
			return;
//
		item+=step;
		}
	TItem* hole=item;
	TInt mask=bit-1;
	while (item!=&iItems[0])
		{
		__ASSERT_DEBUG(item==&iItems[i-1],User::Invariant());
		TInt step=i;
		bit>>=1;
		i>>=1;
		step-=i;
		item-=step;
//
		if (((aHandle^item->handle)&mask)==0)
			{
			*hole=*item;
			hole=item;
			mask=bit-1;
			}
		}
	hole->handle=0;
	__ASSERT_DEBUG(i==1&&At(aHandle)==NULL,User::Invariant());
	}

void TPermanentStoreCache::Invalidate()
	{
	for (TItem* item=&iItems[0],*end=item+EItems;item<end;++item)
		item->handle=0;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////              CPermanentStoreCoord               //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Returns the file system type, according to what the "file write" operation guarantees.
//The return result could be one of:
// - ESimple        - "single byte write" operations are atomic;
// - EBlockAtomic   - "block/sector write" operations are atomic;
// - ETransactional - transactional file system;
CPermanentStoreCoord::TFileQoS CPermanentStoreCoord::FileQoSL()
	{
	//Uncomment, if you want FileQoSL() to return always EBlockAtomic/EBlockAtomic.
	//iFileQos = ETransactional;	
	//iFileQos = EBlockAtomic;	
	//return iFileQos;
	//
	if (iFileQos == EUnknown) //get the file sytem type, if iFileQos is not set yet
		{
		TStreamExchange &se = Host();
		RFileBuf *sb = static_cast<RFileBuf *>(se.HostL());
		RFile &f = sb->File();

		TInt dn;
		TDriveInfo di;
		User::LeaveIfError(f.Drive(dn, di));

		iFileQos = (di.iDriveAtt & KDriveAttTransaction) ? ETransactional : ESimple;
		
		if(iFileQos == ESimple && IsBlockAtomicL(dn))
			{
			iFileQos = EBlockAtomic;	
			}
		}
	return iFileQos;
	}

//The function returns true, if the file system guarantees atomic "block write" operations on aDriveNo.
//It is not the most effective implementation at the moment (creates/closes a file session), 
//probably TDriveInfo::iType can be used in a more effective implementation.
TBool CPermanentStoreCoord::IsBlockAtomicL(TInt aDriveNo) const
	{
	__ASSERT_DEBUG(aDriveNo >= EDriveA && aDriveNo <= EDriveZ, User::Invariant());
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	TVolumeIOParamInfo volInfo;
	TInt err = fs.VolumeIOParam(aDriveNo, volInfo);
	CleanupStack::PopAndDestroy(&fs);
	
	//If VolumeIOParam() succeeds, the media block size is >= 512 bytes and the media block size is power of two - report
	//that the media supports atomic "block write" operations.
	return err == KErrNone && volInfo.iBlockSize >= KDefaultMediaBlockSize && (volInfo.iBlockSize & (volInfo.iBlockSize - 1)) == 0;
	}

TStreamPos CPermanentStoreCoord::LimitL()
	{
	TableL();
	return RFrame16Buf::Position(Base(),iExt);
	}

TStreamId CPermanentStoreCoord::PrimaryL()
	{
	return TStreamId(TableL().Primary());
	}

void CPermanentStoreCoord::ChangedL()
	{
	ConsolidateL().Changed();
	}

TBool CPermanentStoreCoord::CommitL(TStreamId aPrimary)
	{
	__ASSERT_DEBUG(IsTrim(),User::Invariant());
	if (iExtend!=0)
		__LEAVE(KErrInUse);
//
	CPermanentStoreToc& table=ConsolidateL();
	if (!table.IsVirtual())
		return EFalse;
//
	iState|=EClip;
	TInt toc=table.RealizeL(aPrimary.Value(),iExt);
//
	TPermanentStoreHeader header(toc);
	header.SetBackupToc(iToc);
	MStreamBuf* buf=BeginL(header);
	buf->SynchL();
		// it's done, wrap things up
	Host().Share(buf);
	iToc=toc;
	table.Adopt(toc,aPrimary.Value());
	++iGen;
	iState=EBackup;
	iExt=table.Extent();
	return ETrue;
	}

TBool CPermanentStoreCoord::RevertL(TStreamId& aPrimary)
	{
	__ASSERT_ALWAYS(iAccess==0,Panic(EStoreInUse));
		// can't revert things under people's feet
	CPermanentStoreToc* table=iTable;
	iTable=NULL;
	iCache.Invalidate();
	if (table==NULL||!table->IsVirtual())
		{
		__ASSERT_DEBUG(table==NULL||aPrimary==TStreamId(table->Primary()),User::Invariant());
		delete table;
		return EFalse;
		}
//
	aPrimary=TStreamId(table->Primary());
	delete table;
	iState|=EClip;
//
	TStreamExchange& host=Host();
	MStreamBuf* buf=host.HostL();
	host.Release();
	buf->SynchL();
	host.Share(buf);
	return ETrue;
	}

TStreamId CPermanentStoreCoord::ExtendL()
	{
	return TStreamId(ConsolidateL().AllocL(KFrameNonexistent16));
	}

void CPermanentStoreCoord::DeleteL(TStreamId anId)
	{
	TInt handle=anId.Value();
	ConsolidateL().FreeL(handle);
	iCache.Remove(handle);
	}

CPermanentStoreCoord::CPermanentStoreCoord(TStreamPos aBase,TStreamExchange& aHost)
	: iBase(aBase),iHost(&aHost), iFileQos(EUnknown)
	{}

//
// Read and analyse the store header.
// The whole header (14 bytes) is read from the file and:
// - If the dirty bit is set, the backup TOC will be used;
// - If the dirty bit is not set, and the backup TOC ref is not the same as the TOC ref,
//   then it means the the backup TOC ref has not been written successfully, so the TOC ref will be used;
void CPermanentStoreCoord::InternalizeL(RReadStream& aStream)
	{
	if (iTable!=NULL)
		__LEAVE(KErrNotReady);
//
	iState=EClip;
	TPermanentStoreHeader header;
	aStream.ReadL(header.Ptr(),KPermanentStoreHeaderLength);
		// determine where the toc lives
	TInt toc=header.BackupToc();
	if (header.IsDirty())
		{
		iReloc=0;
		iTarget=0;
		}
	else
		{
		TInt handle=header.Handle();
		TInt ref=header.Reference();
		if (handle==0&&toc!=ref)
			{ // toc pointer not backed up, validate as if it was
			toc=ref;
			iState|=EBackup;
			header.SetBackupToc(toc);
			}
		if (!header.IsValid()) // not a permanent store or damaged beyond recognition
			__LEAVE(KErrNotSupported);
	//
		if (toc<0||((handle&~KMaskStreamIdValue)!=0&&handle!=KHandleTocBase)||ref<0||(handle!=0&&ref>=toc+KOffsetTocHeader))
			__LEAVE(KErrCorrupt); // integrity compromised
	//
		iReloc=handle;
		iTarget=ref;
		}
	//
	if (iToc!=0 && iToc!=toc)		// refresh produced a different toc
		__LEAVE(KErrCorrupt);
	iToc=toc;
	}

CPermanentStoreCoord::~CPermanentStoreCoord()
	{
	__ASSERT_ALWAYS(iRefs==0,Panic(EStoreInUse));
	delete iTable;
	}

void CPermanentStoreCoord::CanExtendL()
	{
	__ASSERT_DEBUG(IsTrim(),User::Invariant());
	if (iExtend!=0)
		__LEAVE(KErrInUse);
//
	ConsolidateL();
	}

TInt CPermanentStoreCoord::DoCreateL()
	{
	__ASSERT_DEBUG(IsTrim()&&iReloc==0&&iExtend==0,User::Invariant());
	TInt handle=Table().AllocL();
	iExtend=handle;
	Inc();
	++iAccess;
	return handle;
	}

void CPermanentStoreCoord::DoReplaceL(TInt aHandle)
	{
	__ASSERT_DEBUG(IsTrim()&&iReloc==0&&iExtend==0,User::Invariant());
	TInt off=Table().GetL(aHandle);
	const TItem* item=iCache.At(aHandle);
	__ASSERT_DEBUG(item==NULL||iTable!=NULL&&item->handle==aHandle,User::Invariant());
	if (item==NULL)
		iCache.Add(aHandle,off,Min(off,0));
	iExtend=aHandle;
	Inc();
	++iAccess;
	}

TInt CPermanentStoreCoord::DoOpenL(TInt& anOffset,TInt aHandle)
	{
	const TItem* item=iCache.At(aHandle);
	__ASSERT_DEBUG(item==NULL||iTable!=NULL&&item->handle==aHandle,User::Invariant());
	TInt off;
	TInt ext;
	if (item==NULL)
		{
		off=TableL().AtL(aHandle);
		if (iReloc==aHandle)
			{
			TInt trg=iTarget;
			if (trg==off)
				iReloc=0;
			else
				off=trg;
			}
		ext=Min(off,0); // ensures ext==off for empty streams
		iCache.Add(aHandle,off,ext);
		}
	else
		{
		off=item->offset;
		ext=item->extent;
		}
	Inc();
	++iAccess;
	anOffset=off;
	return ext;
	}

void CPermanentStoreCoord::DoRelease(TInt aHandle,TInt anOffset,TInt anExtent)
	{
	__ASSERT_DEBUG(aHandle!=0,User::Invariant());
	Dec();
	--iAccess;
	if (anExtent==0)
		{ // failed to commit the extending stream
		__ASSERT_DEBUG(aHandle==iExtend&&IsTrim(),User::Invariant());
		iState|=EClip;
		iExtend=0;
		if (aHandle<0)
			Table().Cancel(aHandle);
		}
	else
		{
		const TItem* item=iCache.At(aHandle);
		if (item!=NULL&&item->offset==anOffset&&anExtent>item->extent)
			iCache.Put(item,anOffset,anExtent);
		}
	}

TInt CPermanentStoreCoord::DoCommit(TInt aHandle,TInt anOffset,TInt anExtent)
	{
	__ASSERT_DEBUG(aHandle!=0&&aHandle==iExtend&&(anExtent>=iExt||anOffset==anExtent),User::Invariant());
	aHandle=Table().Set(aHandle,anOffset);
	if (anExtent<0)
		iCache.Remove(aHandle);
	else
		{
		iExt=anExtent;
		const TItem* item=iCache.At(aHandle);
		if (item==NULL)
			iCache.Add(aHandle,anOffset,anExtent);
		else
			iCache.Put(item,anOffset,anExtent);
		}
	iExtend=0;
	return aHandle;
	}

CPermanentStoreToc& CPermanentStoreCoord::TableL()
	{
	CPermanentStoreToc* table=iTable;
	if (table==NULL)
		{
		table=CPermanentStoreToc::NewL(Base(),Host(),iToc,iReloc==KHandleTocBase?iTarget:KFrameNonexistent16);
		iExt=table->Extent();
		iTable=table;
		}
	return *table;
	}

CPermanentStoreToc& CPermanentStoreCoord::ConsolidateL()
	{
	CPermanentStoreToc& table=TableL();
	if (iReloc!=0)
		{
		table.PutL(iReloc,iTarget,CPermanentStoreToc::ETestBeforeWrite);
		iCache.Relocated(iReloc,iTarget);
		iReloc=0;
		}
	return table;
	}

//After stream relocation, the stream entry in the TOC has to be updated with the new stream position.
//If the file system is not transactional or if the file system is "block atomic", but the stream entry is split
//on a block/sector boundary, then the stream handle will be stored in the permanent file store header, in case
//if the TOC entry update fails.
void CPermanentStoreCoord::RelocateL(TInt aHandle,TInt anOffset)
	{
	__ASSERT_DEBUG(!Accessed(),User::Invariant());
	__ASSERT_DEBUG(iReloc==0,User::Invariant());

	TBool updateStoreHeader = ETrue;	
	TFileQoS fileQos = FileQoSL();
	if(fileQos == ETransactional)
		{
		updateStoreHeader = EFalse;	
		}
	else if(fileQos == EBlockAtomic)
		{
		TInt dataLen = 0;
		TInt writePos = iTable->RefSpan(aHandle, dataLen);
		__ASSERT_DEBUG(writePos >= 0 && dataLen > 0, User::Invariant());
		TInt startSectorAddr = writePos & ~(KDefaultMediaBlockSize - 1);
		TInt endSectorAddr = (writePos + dataLen - 1) & ~(KDefaultMediaBlockSize - 1);
		if(startSectorAddr == endSectorAddr)
			{
			updateStoreHeader = EFalse;	
			}
		}
	
	if (updateStoreHeader)
		{
		TPermanentStoreHeader header(iToc,aHandle,anOffset);
		Host().Share(BeginL(header));
		iReloc=aHandle;
		iTarget=anOffset;
		}
	++iGen;
	iTable->PutL(aHandle,anOffset,CPermanentStoreToc::EWrite);
	iCache.Relocated(aHandle,anOffset);
	iReloc=0;
	}

void CPermanentStoreCoord::MoveL(TInt aToc,TInt anExtent)
	{
	__ASSERT_DEBUG(iReloc==0,User::Invariant());
	CPermanentStoreToc& table=Table();
	TPermanentStoreHeader header(aToc);
	header.SetBackupToc(iToc);
	Host().Share(BeginL(header));
		// update data structures but defer the write
	iToc=aToc;
	TInt ext=table.Extent();
	table.Move(aToc,anExtent);
	iState|=EBackup;
	if (iExt==ext)
		{
		iExt=anExtent;
		iState|=EClip;
		}
	}

//
// Starts a pseudo-atomic update of the permanent file store header.
//
// For the effect to be 'atomic', writes need to meet the following requirements:
// 1. When updating n bytes using a single write, bytes 2 through n remain unchanged unless the first byte also changes.
// 2. Changes associated with successive write requests happen in strict sequence.
// 3. Updating a single byte is atomic.
//
// Also, a failure to write to a location shall be reported no later than on the next
// write to that or a different location, or on buffer synchronisation.
//
// The preconditions of the operation are:
// - all stream insert/delete/relocate operations completed, file - updated;
// - the TOC reference in the file store header points to the current (valid) TOC, which does not include the most recent
//   changes, since the last commit;
// - the in-memory backup TOC reference updated and made equal to the file stored TOC reference;
//
// The procedure consists of 3 "file-write" steps:
// -1- write the backup TOC ref (4 bytes) to the permanent file store header.
//     if this operation fails, when the store is reopened, the TOC ref will be used;
// -2- set the dirty bit and write the whole file store header (14 bytes).
//     If this operation fails, but the dirty bit has been successfully set, the backup TOC ref will be used,
//     when the store is reopened;
// -3- clear the dirty bit (1 byte "file write" op). The commit operation has completed successfully;
MStreamBuf* CPermanentStoreCoord::BeginL(TPermanentStoreHeader& aHeader)
	{
	__ASSERT_DEBUG(!aHeader.IsDirty() && aHeader.BackupToc() == Toc() && iReloc == 0, User::Invariant());
	MStreamBuf& buf=*Host().HostL();
	buf.SeekL(buf.EWrite,Base()+KPermanentStoreHeaderOffset);
	TFileQoS fileQos = FileQoSL();
	if (fileQos<EBlockAtomic)
		{
		if (iState&EBackup)
			{ // we've yet to write the backup toc, do that before clobbering the header
			buf.WriteL(aHeader.Ptr(),KPermanentStoreBackupLength);
			buf.SeekL(buf.EWrite,EStreamBeginning);
			buf.SeekL(buf.EWrite,Base()+KPermanentStoreHeaderOffset);
			iState&=~EBackup;
			}
		// first write--dirty flag set in the first byte, backup toc unchanged, new header otherwise
		aHeader.MarkDirty();
		}
	Host().Release(); // from this point onwards any failure results in store shutdown
//
	buf.WriteL(aHeader.Ptr(),KPermanentStoreHeaderLength);
	if (fileQos<EBlockAtomic)
		{
		buf.SeekL(buf.EWrite,EStreamBeginning);
		buf.SeekL(buf.EWrite,Base()+KPermanentStoreHeaderOffset);
		aHeader.SetBackupToc(iToc);
			// second write--single byte write to clear the dirty flag, no change otherwise
		buf.WriteL(aHeader.Ptr(),1);
		}
		// at this point synchronisation is atomic; ie, if successful the change
		// has been recorded, and failure means it will never happen
	return &buf;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////              HPermanentStoreBuf               ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HPermanentStoreBuf* HPermanentStoreBuf::CreateL(CPermanentStoreCoord& aCoord,TStreamId& anId,TInt aMode)
	{
	HPermanentStoreBuf* buf=ExtendLC(aCoord,aMode);
	TInt handle=aCoord.DoCreateL();
	buf->iHandle=handle;
	CleanupStack::Pop();
	anId=TStreamId(handle&KMaskStreamIdValue);
	return buf;
	}

HPermanentStoreBuf* HPermanentStoreBuf::ReplaceL(CPermanentStoreCoord& aCoord,TStreamId anId,TInt aMode)
	{
	HPermanentStoreBuf* buf=ExtendLC(aCoord,aMode);
	TInt handle=anId.Value();
	aCoord.DoReplaceL(handle);
	buf->iHandle=handle;
	CleanupStack::Pop();
	return buf;
	}

HPermanentStoreBuf* HPermanentStoreBuf::OpenL(CPermanentStoreCoord& aCoord,TStreamId anId,TInt aMode)
	{
	HPermanentStoreBuf* buf=NewLC(aCoord);
	TInt handle=anId.Value();
	TInt off=KFrameNonexistent16;
	TInt ext=aCoord.DoOpenL(off,handle);
	buf->iHandle=handle;
	if (ext!=0)
		buf->RFrame16Buf::Set(aCoord.Host(),off,ext,EFrameData16|aMode);
	else
		buf->RFrame16Buf::OpenL(aCoord.Host(),off,EFrameData16|aMode);
	CleanupStack::Pop();
	return buf;
	}

HPermanentStoreBuf::~HPermanentStoreBuf()
	{
	TInt handle=iHandle;
	if (handle!=0)
		Coord().DoRelease(handle,Offset(),Extent());
	RFrame16Buf::DoRelease();
	}

HPermanentStoreBuf* HPermanentStoreBuf::NewLC(CPermanentStoreCoord& aCoord)
	{
	HPermanentStoreBuf* buf=new(ELeave) HPermanentStoreBuf(aCoord);
	buf->PushL();
	return buf;
	}

HPermanentStoreBuf* HPermanentStoreBuf::ExtendLC(CPermanentStoreCoord& aCoord,TInt aMode)
	{
	aCoord.CanExtendL();
	HPermanentStoreBuf* buf=NewLC(aCoord);
	buf->RFrame16Buf::ExtendL(aCoord.Host(),aCoord.iExt,EFrameData16|aMode);
	__ASSERT_DEBUG(TStreamPos(aCoord.Host().SizeL())==buf->Position(buf->Offset()),User::Invariant());
	return buf;
	}

void HPermanentStoreBuf::DoRelease()
	{
	delete this;
	}

void HPermanentStoreBuf::DoSynchL()
	{
	__ASSERT_DEBUG(iHandle!=0,User::Invariant());
	if (IsCommitted())
		return;
//
	CommitL();
	iHandle=Coord().DoCommit(iHandle,Offset(),Extent());
	}

