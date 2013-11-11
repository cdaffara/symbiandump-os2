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

#include "US_STD.H"
#include "D32COMP.H"

// Class TDbStoreIndexStats::TBound

inline void TDbStoreIndexStats::TBound::Set(TReal64 aBound)
	{iValue=aBound;}

void TDbStoreIndexStats::TBound::Set(const TInt64& aBound)
	{iValue=I64REAL(aBound);}

void TDbStoreIndexStats::TBound::Set(const TText8* aPtr,TInt aLen,const TTextOps& aConv)
	{
	TInt64 v(0u);
	const TText8* const end=aPtr+aLen;
	TInt shift=48;
	do
		{
		if (aPtr==end)
			break;
		TInt64 t(aConv.Fold(*aPtr++));
		t<<=shift;
		v+=t;
		} while ((shift-=8)>=0);
	Set(v);
	}

void TDbStoreIndexStats::TBound::Set(const TText16* aPtr,TInt aLen,const TTextOps& aConv)
	{
	TInt64 v(0u);
	const TText16* const end=aPtr+aLen;
	TInt shift=32;
	do
		{
		if (aPtr==end)
			break;
		TInt64 t(aConv.Fold(*aPtr++));
		t<<=shift;
		v+=t;
		} while ((shift-=16)>=0);
	Set(v);
	}

void TDbStoreIndexStats::TBound::Set(const TDbLookupKey::SColumn& aBound,const TTextOps& aConv)
	{
	switch (aBound.iType)
		{
	default:
		__ASSERT(0);
	case EDbColInt64:
		Set(aBound.iInt64);
		break;
	case EDbColDateTime:
		Set(aBound.iTime().Int64());
		break;
	case EDbColReal64:
		Set(aBound.iReal64);
		break;
	case EDbColText8:
		Set(aBound.iDes8.iPtr,aBound.iDes8.iLength,aConv);
		break;
	case EDbColText16:
		Set(aBound.iDes16.iPtr,aBound.iDes16.iLength,aConv);
		break;
		}
	}

// Class TDbStoreIndexStats

inline TBool TDbStoreIndexStats::NeedsRefresh() const
	{
	return iRefresh<=ERefresh;
	}

inline TInt TDbStoreIndexStats::Cardinality() const
	{
	return iCardinality;
	}

// update the refresh count
inline void TDbStoreIndexStats::Touch()
	{
	TInt r=iRefresh-1;
	if (r>=ERefresh) 
		iRefresh=r;
	}

// an entry is inserted
inline void TDbStoreIndexStats::Inc()
	{
	++iCardinality;
	Touch();
	}

// an entry is removed
inline void TDbStoreIndexStats::Dec()
	{
	--iCardinality;
	Touch();
	}

// contents have changed radically. provoke an immediate refresh
inline void TDbStoreIndexStats::Reset(TInt aCardinality)
	{
	iCardinality=aCardinality;
	iRefresh=ERefresh;
	}

// stats have been refreshed. set for next update
inline void TDbStoreIndexStats::Refresh(TDbStoreIndexStats::TType aType)
	{
	iFlags=(iFlags&~EFlgDiscrete)|aType;
	iRefresh=(iCardinality>>ERefreshFactor)+1;
	}

//
// Internalize the index statistics
// This must handle a stream externalized by builds before 052
//
void TDbStoreIndexStats::InternalizeL(RReadStream& aStream)
	{
	TCardinality c;
	aStream>>c;
	iCardinality=c;
	TUint refresh=aStream.ReadUint32L();
	iFlags=refresh&EFlagsMask;
	iRefresh=TInt(refresh>>ERefreshShift)+EInvalid;
//
// pre-build-052 data would run out here
// if there is no more data, mark the (non-cardinality) stats as invalid
//
	TRAPD(r,aStream>>iLower.iValue>>iUpper.iValue;)
	if (r==KErrEof)
		iRefresh=EInvalid;		// mark as invalid data
	else
		__LEAVE_IF_ERROR(r);	// just an "ordinary" error
	}

//
// Notes: iCardinality cannot exceed 2^29 (due to storage mechanism)
//		thus iRefresh cannot exceed 2^(29-ERefreshFactor)+1
//		this leaves the 5 m.s.b. clear in iRefresh-EInvalid
//		the flags bits are stored there
//
void TDbStoreIndexStats::ExternalizeL(RWriteStream& aStream) const
	{
	__ASSERT((iFlags&~EFlagsMask)==0);
	aStream<<TCardinality(iCardinality);
	aStream<<TUint32((TUint(iRefresh-EInvalid)<<ERefreshShift)|iFlags);
	aStream<<iLower.iValue<<iUpper.iValue;
	}

//
// Reverse the span and bounds
//
TInt TDbStoreIndexStats::ReverseSpan(TUint aInclusion,const TDbLookupKey* aLower,const TDbLookupKey* aUpper,const TTextOps& aConv) const
	{
	__ASSERT(iLower.iValue>iUpper.iValue);
	TDbStoreIndexStats stats(*this);
	stats.iLower=iUpper;
	stats.iUpper=iLower;
	return stats.Span((aInclusion<<1)|(aInclusion>>1),aUpper,aLower,aConv);
	}

//
// Evaluate the probable proportion of the index set contained within the bounds
//
TInt TDbStoreIndexStats::Span(TUint aInclusion,const TDbLookupKey* aLower,const TDbLookupKey* aUpper,const TTextOps& aConv) const
	{
	if (!IsValid())
		return CDbTable::EUnavailableSpan;	// No valid index data
	if (iCardinality==0)
		return 0;					// no rows at all
//
	if (iLower.iValue>iUpper.iValue)
		return ReverseSpan(aInclusion,aLower,aUpper,aConv);	// descending index
//
	aInclusion&=CDbRecordIndex::EIncludeBoth;
	TBound l(iLower);
	if (aLower)
		{
		TBound bound;
		bound.Set(*aLower->First(),aConv);
		if (bound.iValue>=l.iValue)
			l=bound;
		else
			aInclusion|=CDbRecordIndex::EIncludeLower;
		}
	else
		aInclusion|=CDbRecordIndex::EIncludeLower;
//
	TBound h(iUpper);
	if (aUpper)
		{
		TBound bound;
		bound.Set(*aUpper->First(),aConv);
		if (bound.iValue<=h.iValue)
			h=bound;
		else
			aInclusion|=CDbRecordIndex::EIncludeUpper;
		}
	else
		aInclusion|=CDbRecordIndex::EIncludeUpper;
//
	TRealX restrict(h.iValue);
	restrict.SubEq(l.iValue);		// extended precision--no errors
	TRealX span(iUpper.iValue);
	span.SubEq(iLower.iValue);
	if (iFlags&EFlgDiscrete)
		{
		if (aInclusion==0)
			--restrict;
		else if (aInclusion==CDbRecordIndex::EIncludeBoth)
			++restrict;
		++span;
		}
	else if (restrict.IsZero())		// single value continuous range
		{
		return (span.IsZero() && aInclusion==CDbRecordIndex::EIncludeBoth)
			? CDbTable::EFullIndexSpan : 0;
		}
	if (restrict<=TRealX(0))
		return 0;	// no overlap
	restrict.DivEq(span);
	restrict.MultEq(TInt(CDbTable::EFullIndexSpan));
	return TInt(restrict);
	}


// Class CDbStoreIndex

inline CDbStoreIndex::HKey& CDbStoreIndex::Key() const
	{
	return *iKey;
	}
	
inline const TBtree& CDbStoreIndex::Tree() const
	{
	return iTree;
	}
	
inline TInt CDbStoreIndex::Count() const
	{
	return iStats.Cardinality();
	}


// Class CDbStoreIndex::HKey

NONSHARABLE_CLASS(CDbStoreIndex::HKey) : public MBtreeKey
	{
public:
	static HKey* NewL(const CDbKey& aKey,const HDbColumnSet& aColSet);
//
	inline TInt EntrySize() const;
	virtual TInt KeySize() const;
	virtual TBool IncompleteKey() const;
//
	TInt EntryL(TAny* aPtr,const RDbTableRow& aRow,TDbRecordId aRecord);
	TInt Entry(TAny* aPtr,const TDbLookupKey& aKey);
	inline const TAny* Restriction() const;
	inline void Restrict(const TAny* aRestriction);
//
	void Bound(TDbStoreIndexStats::TBound& aBound,const TAny* aEntry);
	TDbStoreIndexStats::TType KeyType() const;
// from MBtreeKey
	const TAny* Key(const TAny* aRecord) const;
	TInt Compare(const TAny* aLeft,const TAny* aRight) const;
protected:
	HKey() {}
	inline TBool FullComparison() const;
// from MBtreeKey
	void Between(const TAny* aLeft,const TAny* aRight,TBtreePivot& aPivot) const;
private:
	struct SKeyCol
		{
		TDbColNo iOrdinal;
		TInt iSize;
		TUint8 iType;
		TUint8 iOrder;
		};
	const TTextOps* iTextOps;
	TInt iSize;
	const SKeyCol* iEndOfKeys;
	const SKeyCol* iRestrictedEndOfKeys;
	SKeyCol iKeys[1];	// one or more keys
	};

NONSHARABLE_CLASS(CDbStoreIndex::HDupKey) : public CDbStoreIndex::HKey
	{
	friend class CDbStoreIndex::HKey;
private:
	HDupKey() {}
	TInt KeySize() const;
	const TAny* Key(const TAny* aRecord) const;
	TInt Compare(const TAny* aLeft,const TAny* aRight) const;
	TBool IncompleteKey() const;
	};

inline TInt CDbStoreIndex::HKey::EntrySize() const
	{
	return iSize;
	}
	
inline const TAny* CDbStoreIndex::HKey::Restriction() const
	{
	return iRestrictedEndOfKeys;
	}
	
inline void CDbStoreIndex::HKey::Restrict(const TAny* aRestriction)
	{
	__ASSERT(aRestriction==0||(aRestriction>=iKeys&&aRestriction<=iEndOfKeys));
	iRestrictedEndOfKeys=(const SKeyCol*)aRestriction;
	}
	
inline TBool CDbStoreIndex::HKey::FullComparison() const
	{
	return iRestrictedEndOfKeys==NULL;
	}

//
// Construct the key based on the key definition (must be valid for the table)
// and the column set provided
//
CDbStoreIndex::HKey* CDbStoreIndex::HKey::NewL(const CDbKey& aKey,const HDbColumnSet& aColumns)
	{
	TInt count=aKey.Count();
	HKey* self=(HKey*)User::AllocLC(_FOFF(HKey,iKeys[count]));
	if (aKey.IsUnique())
		new(self) HKey;
	else
		new(self) HDupKey;
	self->iTextOps=&TTextOps::Ops(aKey.Comparison());
	self->iEndOfKeys=&self->iKeys[count];
	self->iRestrictedEndOfKeys=NULL;
	TInt len=sizeof(TDbRecordId);
	SKeyCol* pKey=&self->iKeys[0];
	for (TInt ii=0;ii<count;++pKey,++ii)
		{
		const TDbKeyCol& key=aKey[ii];
		pKey->iOrder=TUint8(key.iOrder);
		pKey->iOrdinal=aColumns.ColNoL(key.iName);
		if (pKey->iOrdinal==KDbNullColNo)
			__LEAVE(KErrCorrupt);
		const TDbColumnDef& col=aColumns[pKey->iOrdinal];
		pKey->iType=col.iType;
		pKey->iSize=CDbStoreIndexDef::KeySize(key,col);
		len+=Align4(pKey->iSize);
		}
	self->iSize=len;
	if (self->KeySize()>KMaxIndexKeySize)
		__LEAVE(KErrCorrupt);
	CleanupStack::Pop();
	return self;
	}

//
// Construct an entry at aPtr from the record given
//
TInt CDbStoreIndex::HKey::EntryL(TAny* aPtr,const RDbTableRow& aRow,TDbRecordId aRecord)
	{
	TUint8* ptr=(TUint8*)aPtr;
	Mem::FillZ(ptr,iSize);
	*REINTERPRET_CAST(TDbRecordId*,ptr)=aRecord;
	ptr+=sizeof(TDbRecordId);
	const SKeyCol* const end=iEndOfKeys;
	for (const SKeyCol* key=&iKeys[0];key<end;++key)
		{
		const TDbCell* cell=aRow.ColCell(key->iOrdinal);
		TInt size=key->iSize;
		if (cell->Length()!=0)
			{
#ifdef __DOUBLE_WORDS_SWAPPED__
			if (key->iType==EDbColReal64)
				{
				const TUint32* data=(TUint32*)cell->Data();
				((TUint32*)ptr)[0]=data[1];
				((TUint32*)ptr)[1]=data[0];
				}
			else
#endif
			if (TDbCol::IsLong(TDbColType(key->iType)))
				{
				const TDbBlob& blob=*(const TDbBlob*)cell->Data();
				if (blob.IsInline())
					Mem::Copy(ptr,blob.Data(),Min(size,blob.Size()));
				else
					{
					aRow.Table().BlobsL()->ReadLC(blob.Id(),TDbColType(key->iType))->ReadL(ptr,size);
					CleanupStack::PopAndDestroy();	// stream buffer
					}
				}
			else
				Mem::Copy(ptr,cell->Data(),Min(size,cell->Length()));
			}
		ptr+=Align4(size);
		}
	iRestrictedEndOfKeys=NULL;		// use the full key for comparison
	return EntrySize();
	}

//
// Construct an entry from a lookup key
//
TInt CDbStoreIndex::HKey::Entry(TAny* aPtr,const TDbLookupKey& aKey)
	{
	TUint8* ptr=(TUint8*)aPtr;
	Mem::FillZ(ptr,iSize);
	ptr+=sizeof(TDbRecordId);
	const TDbLookupKey::SColumn* lkey=aKey.First();
	const TDbLookupKey::SColumn* const lend=lkey+aKey.Count();
	__ASSERT(lkey<lend);
	const SKeyCol* const end=iEndOfKeys;
	const SKeyCol* key=&iKeys[0];
	for (;;)
		{
		TDbColType ltype=lkey->iType;
		TInt size=key->iSize;
		switch (key->iType)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
		case EDbColUint8:
		case EDbColUint16:
		case EDbColUint32:
			if (ltype==EDbColUint32)
				{
				*(TUint32*)ptr=lkey->iUint32;
				break;
				}
			else if (ltype==EDbColInt32)
				{
				if (lkey->iInt32>=0)	// domain check, unsigned value
					{
					*(TUint32*)ptr=lkey->iInt32;
					break;
					}
				}
			else if (ltype==EDbColInt64)
				{
				const TInt64& v=lkey->iInt64;
				if (I64HIGH(v)==0)		// domain check, in unsigned 32-bit range
					{
					*(TUint32*)ptr=I64LOW(v);
					break;
					}
				}
			Panic(EDbWrongType);
			break;
		case EDbColInt8:
		case EDbColInt16:
		case EDbColInt32:
			if (ltype==EDbColInt32)
				{
				*(TInt32*)ptr=lkey->iInt32;
				break;
				}
			else if (ltype==EDbColUint32)
				{
				if (lkey->iUint32<=TUint(KMaxTInt))	// domain check, in signed range
					{
					*(TInt32*)ptr=lkey->iUint32;
					break;
					}
				}
			else if (ltype==EDbColInt64)
				{
				const TInt64& v=lkey->iInt64;
				TInt32 l=I64LOW(v);
				TInt32 h=I64HIGH(v);
				if (h==(l>>31))	// domain check, in signed 32-bit range
					{
					*(TInt32*)ptr=l;
					break;
					}
				}
			Panic(EDbWrongType);
			break;
		case EDbColReal32:
			if (ltype==EDbColReal32)
				{
				*(TReal32*)ptr=lkey->iReal32;
				break;
				}
			else if (ltype==EDbColReal64)
				{	// convert to TReal32, storing +-#inf if reqd.
				TRealX(lkey->iReal64).GetTReal(*(TReal32*)ptr);
				break;
				}
			Panic(EDbWrongType);
			break;
		case EDbColReal64:
#ifdef __DOUBLE_WORDS_SWAPPED__
			if (ltype==EDbColReal64)
				{
				const TUint32* data=(TUint32*)&lkey->iReal64;
				((TUint32*)ptr)[0]=data[1];
				((TUint32*)ptr)[1]=data[0];
				break;
				}
			// drop through
#endif
		case EDbColInt64:
		case EDbColDateTime:
			if (ltype==key->iType)
				Mem::Copy(ptr,&lkey->iInt64,size);	// all at same address
			else
				Panic(EDbWrongType);
			break;
		case EDbColText8:
		case EDbColLongText8:
			if (ltype==EDbColText8)
				Mem::Copy(ptr,lkey->iDes8.iPtr,Min(size,lkey->iDes8.iLength));
			else
				Panic(EDbWrongType);
			break;
		case EDbColText16:
		case EDbColLongText16:
			if (ltype==EDbColText16)
				Mem::Copy(ptr,lkey->iDes16.iPtr,Min(size,lkey->iDes16.iLength<<1));
			else
				Panic(EDbWrongType);
			break;
			}
		++key;
		if (++lkey==lend)
			break;							// end of lookup key
		if (key==end)
			__LEAVE(KErrArgument);		// too many keys
		ptr+=Align4(size);
		}
	iRestrictedEndOfKeys=key;				// use only the keys in the lookup for comparison
	return EntrySize();
	}

void CDbStoreIndex::HKey::Bound(TDbStoreIndexStats::TBound& aBound,const TAny* aEntry)
	{
	aEntry=PtrAdd(aEntry,sizeof(TDbRecordId));	// get to real key data
	switch (iKeys[0].iType)
		{
	default:
		__ASSERT(0);
	case EDbColBit:
	case EDbColUint8:
	case EDbColUint16:
	case EDbColUint32:
		aBound.Set(TInt64(TUint(*STATIC_CAST(const TUint32*,aEntry))));
		break;
	case EDbColInt8:
	case EDbColInt16:
	case EDbColInt32:
		aBound.Set(TInt64(TInt(*STATIC_CAST(const TInt32*,aEntry))));
		break;
	case EDbColInt64:
		aBound.Set(*STATIC_CAST(const TInt64*,aEntry));
		break;
	case EDbColDateTime:
		aBound.Set(STATIC_CAST(const TTime*,aEntry)->Int64());
		break;
	case EDbColReal32:
		aBound.Set(TReal64(*STATIC_CAST(const TReal32*,aEntry)));
		break;
	case EDbColReal64:
#if !defined(__DOUBLE_WORDS_SWAPPED__)
		aBound.Set(*STATIC_CAST(const TReal64*,aEntry));
#else
		{
		TReal64 xKey;
		((TUint32*)&xKey)[0]=STATIC_CAST(const TUint32*,aEntry)[1];
		((TUint32*)&xKey)[1]=STATIC_CAST(const TUint32*,aEntry)[0];
		aBound.Set(xKey);
		}
#endif
		break;
	case EDbColText8:
	case EDbColLongText8:
		aBound.Set(STATIC_CAST(const TUint8*,aEntry),iKeys[0].iSize,*iTextOps);
		break;
	case EDbColText16:
	case EDbColLongText16:
		aBound.Set(STATIC_CAST(const TUint16*,aEntry),iKeys[0].iSize>>1,*iTextOps);
		break;
		}
	}

// Is the index key discrete or continous?
inline TDbStoreIndexStats::TType CDbStoreIndex::HKey::KeyType() const
	{
	return iKeys[0].iType==EDbColReal32 || iKeys[0].iType==EDbColReal64
		? TDbStoreIndexStats::EContinuous : TDbStoreIndexStats::EDiscrete;
	}

TInt CDbStoreIndex::HKey::KeySize() const
	{
	return EntrySize()-sizeof(TDbRecordId);
	}

//
// Report 'true' if the lookup key is not the entire B+tree key
// For a unique index this is if there is a restriction to less than the full key
//
TBool CDbStoreIndex::HKey::IncompleteKey() const
	{
	return iRestrictedEndOfKeys!=0 && iRestrictedEndOfKeys!=iEndOfKeys;
	}

//
// For unique keys, key is after record id
//
const TAny* CDbStoreIndex::HKey::Key(const TAny* aRecord) const
	{
	return PtrAdd(aRecord,sizeof(TDbRecordId));
	}

//
// compare the key part of the entry
//
TInt CDbStoreIndex::HKey::Compare(const TAny* aLeft,const TAny* aRight) const
	{
	const SKeyCol* end=iRestrictedEndOfKeys;
	if (end==NULL)
		end=iEndOfKeys;
	const SKeyCol* key=&iKeys[0];
	for (;;)
		{
		TInt size=key->iSize;
		TInt rr;
		switch (key->iType)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
		case EDbColUint8:
		case EDbColUint16:
		case EDbColUint32:
			rr=Comp::Compare(*STATIC_CAST(const TUint32*,aLeft),*STATIC_CAST(const TUint32*,aRight));
			break;
		case EDbColInt8:
		case EDbColInt16:
		case EDbColInt32:
			rr=Comp::Compare(*STATIC_CAST(const TInt32*,aLeft),*STATIC_CAST(const TInt32*,aRight));
			break;
		case EDbColInt64:
			rr=Comp::Compare(*STATIC_CAST(const TInt64*,aLeft),*STATIC_CAST(const TInt64*,aRight));
			break;
		case EDbColDateTime:
			rr=Comp::Compare(*STATIC_CAST(const TTime*,aLeft),*STATIC_CAST(const TTime*,aRight));
			break;
		case EDbColReal32:
			rr=Comp::Compare(*STATIC_CAST(const TReal32*,aLeft),*STATIC_CAST(const TReal32*,aRight));
			break;
		case EDbColReal64:
#if !defined(__DOUBLE_WORDS_SWAPPED__)
			rr=Comp::Compare(*STATIC_CAST(const TReal64*,aLeft),*STATIC_CAST(const TReal64*,aRight));
#else
			TReal64 xLeft;
			((TUint32*)&xLeft)[0]=STATIC_CAST(const TUint32*,aLeft)[1];
			((TUint32*)&xLeft)[1]=STATIC_CAST(const TUint32*,aLeft)[0];
			TReal64 xRight;
			((TUint32*)&xRight)[0]=STATIC_CAST(const TUint32*,aRight)[1];
			((TUint32*)&xRight)[1]=STATIC_CAST(const TUint32*,aRight)[0];
			rr=Comp::Compare(xLeft,xRight);
#endif
			break;
		case EDbColText8:
		case EDbColLongText8:
			rr=iTextOps->Compare(STATIC_CAST(const TUint8*,aLeft),size,STATIC_CAST(const TUint8*,aRight),size);
			break;
		case EDbColText16:
		case EDbColLongText16:
			rr=iTextOps->Order(STATIC_CAST(const TUint16*,aLeft),size>>1,STATIC_CAST(const TUint16*,aRight),size>>1);
			break;
			}
		if (rr!=0)
			return key->iOrder==TDbKeyCol::EAsc ? rr : -rr;
		if (++key==end)
			return rr;
		size=Align4(size);
		aLeft=PtrAdd(aLeft,size);
		aRight=PtrAdd(aRight,size);
		}
	}

//
// No clever stuff yet
//
void CDbStoreIndex::HKey::Between(const TAny* aLeft,const TAny* /*aRight*/,TBtreePivot& aPivot) const
	{
	aPivot.Copy((const TUint8*)aLeft,KeySize());
	}

// Class CDbStoreIndex::HDupKey

TInt CDbStoreIndex::HDupKey::KeySize() const
	{
	return EntrySize();
	}

//
// Report 'true' if the lookup key is not the entire B+tree key
// For a duplicates index this is if there is a restriction (as record id is ingored)
//
TBool CDbStoreIndex::HDupKey::IncompleteKey() const
	{
	return Restriction()!=0;
	}

//
// The key includes the record id
//
const TAny* CDbStoreIndex::HDupKey::Key(const TAny* aRecord) const
	{
	return aRecord;
	}

TInt CDbStoreIndex::HDupKey::Compare(const TAny* aLeft,const TAny* aRight) const
	{
	const TDbRecordId* const left=(const TDbRecordId*)aLeft;
	const TDbRecordId* const right=(const TDbRecordId*)aRight;
	TInt rr=HKey::Compare(left+1,right+1);
	if (rr==0 && FullComparison())
		return Comp::Compare(left->Value(),right->Value());
	return rr;
	}

// Class CDbStoreIndex::CIter

NONSHARABLE_CLASS(CDbStoreIndex::CIter) : public CDbRecordIter
	{
public:
	static CIter* NewL(CDbStoreIndex& aIndex,TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound);
private:
	inline CIter(CDbStoreIndex& aIndex);
	~CIter();
	void ConstructL(TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound);
	TAny* BoundL(const TDbLookupKey& aBound,const TAny*& aRestriction);
//
	inline const CDbStoreIndex& Index() const;
	TBool FindL(TDbRecordId aRecordId,const RDbTableRow& aRow,TBtree::TFind aFind);
	TInt CompareL(const TAny* aBound,const TAny* aRestriction);
	TBool _GotoL(TDbPosition aPosition);
//
	TInt Count() const;
	TDbRecordId CurrentL();
	TBool GotoL(TDbPosition aPosition);
	TBool GotoL(TDbRecordId aRecordId,RDbTableRow& aBuffer);
	TBool SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison);
	TDeleted DoDeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow* aRow);
private:
	TBtreePos iPos;
	TUint8 iLowerSeek;
	TUint8 iLowerCheck;
	TUint8 iUpperSeek;
	TUint8 iUpperCheck;
	TAny* iLowerBound;
	const TAny* iLowerRestriction;
	TAny* iUpperBound;
	const TAny* iUpperRestriction;
	};

inline CDbStoreIndex::CIter::CIter(CDbStoreIndex& aIndex) :
	CDbRecordIter(aIndex)
	{
	}
	
inline const CDbStoreIndex& CDbStoreIndex::CIter::Index() const
	{
	return STATIC_CAST(CDbStoreIndex&,Host());
	}

CDbStoreIndex::CIter* CDbStoreIndex::CIter::NewL(CDbStoreIndex& aIndex,TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound)
	{
	CIter* self=new(ELeave) CIter(aIndex);
	CleanupStack::PushL(self);
	self->ConstructL(aInclusion,aLowerBound,aUpperBound);
	CleanupStack::Pop();
	return self;
	}

CDbStoreIndex::CIter::~CIter()
	{
	User::Free(iLowerBound);
	User::Free(iUpperBound);
	}

void CDbStoreIndex::CIter::ConstructL(TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound)
	{
	if (aLowerBound)
		{
		TBtree::TFind seek=TBtree::EGreaterEqual;
		if ((aInclusion&CDbRecordIndex::EIncludeLower)==0)
			{
			seek=TBtree::EGreaterThan;
			iLowerCheck=1;
			}
		iLowerSeek=TUint8(seek);
		iLowerBound=BoundL(*aLowerBound,iLowerRestriction);
		}
	if (aUpperBound)
		{
		TBtree::TFind seek=TBtree::ELessThan;
		if (aInclusion&CDbRecordIndex::EIncludeUpper)
			{
			seek=TBtree::ELessEqual;
			iUpperCheck=1;
			}
		iUpperSeek=TUint8(seek);
		iUpperBound=BoundL(*aUpperBound,iUpperRestriction);
		}
	}

//
// Construct and allocate a key for the boundary value
//
TAny* CDbStoreIndex::CIter::BoundL(const TDbLookupKey& aBound,const TAny*& aRestriction)
	{
	TUint8 entry[KMaxBtreeKeyLength];
	HKey& key=Index().Key();
	TInt size=key.Entry(entry,aBound);
	const TUint8* ekey=(const TUint8*)key.Key(entry);
	size-=ekey-entry;
	TAny* e=User::AllocL(size);
	Mem::Copy(e,ekey,size);
	aRestriction=key.Restriction();
	return e;
	}

//
// Extract the current key and compare it with a boundary key
//
TInt CDbStoreIndex::CIter::CompareL(const TAny* aBound,const TAny* aRestriction)
	{
	TUint8 entry[KMaxBtreeKeyLength];
	HKey& key=Index().Key();
	key.Restrict(aRestriction);
	Index().Tree().ExtractAtL(iPos,entry,key.EntrySize());
	return key.Compare(key.Key(entry),aBound);
	}

//
// return the cardinality of the index
//
TInt CDbStoreIndex::CIter::Count() const
	{
	if (iLowerBound)
		return KDbUndefinedCount;
	if (iUpperBound)
		return KDbUndefinedCount;
	return Index().Count();
	}

//
// return the current record id
//
TDbRecordId CDbStoreIndex::CIter::CurrentL()
	{
	TDbRecordId id;
	Index().Tree().ExtractAtL(iPos,&id,sizeof(id));
	return id;
	}

//
// iterate to the required position, does not test the boundary condition
//
TBool CDbStoreIndex::CIter::_GotoL(TDbPosition aPosition)
	{
	const TBtree& tree=Index().Tree();
	switch (aPosition)
		{
	default:	// all control paths return a value
		__ASSERT(0);
	case EDbNext:
		return tree.NextL(iPos);
	case EDbPrevious:
		return tree.PreviousL(iPos);
	case EDbFirst:
		if (!iLowerBound)
			return tree.FirstL(iPos);
		Index().Key().Restrict(iLowerRestriction);
		return tree.FindL(iPos,iLowerBound,TBtree::TFind(iLowerSeek));
	case EDbLast:
		if (!iUpperBound)
			return tree.LastL(iPos);
		Index().Key().Restrict(iUpperRestriction);
		return tree.FindL(iPos,iUpperBound,TBtree::TFind(iUpperSeek));
		}
	}

//
// iterate to the required position and check that it is in bounds
//
TBool CDbStoreIndex::CIter::GotoL(TDbPosition aPosition)
	{
	TBool r=_GotoL(aPosition);
	if (r)
		{
		if (aPosition==EDbFirst || aPosition==EDbNext)
			{
			if (iUpperBound)
				return CompareL(iUpperBound,iUpperRestriction)-iUpperCheck<0;
			}
		else
			{
			if (iLowerBound)
				return CompareL(iLowerBound,iLowerRestriction)-iLowerCheck>=0;
			}
		}
	return r;
	}

//
// Construct the Btree key for the row and lookup
//
TBool CDbStoreIndex::CIter::FindL(TDbRecordId aRecordId,const RDbTableRow& aRow,TBtree::TFind aFind)
	{
	TUint8 entry[KMaxBtreeKeyLength];
	HKey& key=Index().Key();
	key.EntryL(entry,aRow,aRecordId);
	return Index().Tree().FindL(iPos,key.Key(entry),aFind);
	}

//
// Go directly to a row
//
TBool CDbStoreIndex::CIter::GotoL(TDbRecordId aRecordId,RDbTableRow& aRow)
	{
	aRow.ReadL(aRecordId);
	return FindL(aRecordId,aRow,TBtree::EEqualTo);
	}

//
// Do a keyed lookup in the index
//
TBool CDbStoreIndex::CIter::SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison)
	{
	TUint8 entry[KMaxBtreeKeyLength];
	HKey& key=Index().Key();
	key.Entry(entry,aKey);
	const TAny* ekey=key.Key(entry);
	TBtree::TFind find;
	switch (aComparison)
		{
	default:
		__ASSERT(0);
	case RDbTable::ELessThan:
		find=TBtree::ELessThan;
		break;
	case RDbTable::ELessEqual:
		find=TBtree::ELessEqual;
		break;
	case RDbTable::EEqualTo:
		if (key.IncompleteKey())
			{
			// The B+tree search code cannot correctly do a == search when the
			// comparison key is not complete. Instead we do a >= search and then
			// check the returned entry does match
			//
			if (!Index().Tree().FindL(iPos,ekey,TBtree::EGreaterEqual))
				return EFalse;	// off the end
			return CompareL(ekey,key.Restriction())==0;
			}
		find=TBtree::EEqualTo;
		break;
	case RDbTable::EGreaterEqual:
		find=TBtree::EGreaterEqual;
		break;
	case RDbTable::EGreaterThan:
		find=TBtree::EGreaterThan;
		break;
		}
	return Index().Tree().FindL(iPos,ekey,find);
	}

//
// Set the iterator following a record deletion
//
CDbStoreIndex::CIter::TDeleted CDbStoreIndex::CIter::DoDeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow* aRow)
	{
	if (aRow==0)
		return ENotSupported;
	return FindL(aRecordId,*aRow,aPosition==EDbNext ? TBtree::EGreaterEqual : TBtree::ELessEqual) ? EAtRow : ENoRow;
	}

// Class CDbStoreIndex

CDbStoreIndex::CDbStoreIndex(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef) :
	iDatabase(aDatabase), 
	iTree(EBtreeFast), 
	iStats(MUTABLE_CAST(TDbStoreIndexStats&,aDef.iStats))
	{
	}

CDbStoreIndex::~CDbStoreIndex()
	{
	delete iKey;
	}

//
// Create the persistent representation of the index in the store
//
TStreamId CDbStoreIndex::CreateL(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef)
	{
	MUTABLE_CAST(TDbStoreIndexStats&,aDef.iStats).Reset();
	RStoreWriteStream strm;
	TStreamId id=strm.CreateLC(aDatabase.Store());
	strm<<KEmptyBtreeToken<<aDef.iStats;
	strm.CommitL();
	CleanupStack::PopAndDestroy();
	return id;
	}

//
// Check the index for damage (without constructing the object)
//
TBool CDbStoreIndex::IsDamagedL(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef)
	{
	RStoreReadStream strm;
	strm.OpenLC(aDatabase.Store(),aDef.TokenId());
	TBtreeToken token;
	strm>>token;
	CleanupStack::PopAndDestroy();
	return token.IsBroken();
	}

//
// Create a StoreIndex object
//
CDbStoreIndex* CDbStoreIndex::NewL(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef,const CDbTableDef& aTable)
	{
	CDbStoreIndex* self=new(ELeave) CDbStoreIndex(aDatabase,aDef);
	CleanupStack::PushL(self);
	self->iTokenId=aDef.TokenId();
	HKey* key=self->iKey=HKey::NewL(aDef.Key(),aTable.Columns());
	self->iLeafOrg.SetEntrySize(key->EntrySize());
	self->iIndexOrg.SetEntrySize(key->KeySize());
	self->iTree.Connect(&aDatabase.PagePoolL(),key,&self->iLeafOrg,&self->iIndexOrg);
	CleanupStack::Pop();
	return self;
	}

//
// restore from the Store
//
TBool CDbStoreIndex::RestoreL()
	{
	RStoreReadStream strm;
	strm.OpenLC(iDatabase.Store(),iTokenId);
	TBtreeToken token;
	strm>>token>>iStats;
	CleanupStack::PopAndDestroy();
	iTree.Set(token,EBtreeFast);
	return iTree.IsBroken();
	}

//
// Update the index statistics from the index
//
void CDbStoreIndex::RefreshStatsL()
	{
	HKey& key=Key();
	TBtreePos pos;
	if (iTree.FirstL(pos))
		{
		TUint8 entry[KMaxBtreeKeyLength];
		Tree().ExtractAtL(pos,entry,key.EntrySize());
		key.Bound(iStats.iLower,entry);
		iTree.LastL(pos);
		Tree().ExtractAtL(pos,entry,key.EntrySize());
		key.Bound(iStats.iUpper,entry);
		}
	iStats.Refresh(key.KeyType());
	}

//
// Framework member: synchronise the persistent data
//
void CDbStoreIndex::SynchL()
	{
	if (iStats.NeedsRefresh())
		RefreshStatsL();
	RStoreWriteStream strm;
	strm.ReplaceLC(iDatabase.Store(),iTokenId);
	strm<<iTree.Token()<<iStats;
	strm.CommitL();
	CleanupStack::PopAndDestroy();
	}

//
// Framework member: mark the persistent token dirty
//
void CDbStoreIndex::AboutToModifyL()
	{
	iDatabase.MarkL();
	if (!iTree.IsEmpty())	// empty btrees are unbreakable
		{
		TBtreeToken token=iTree.Token();
		token.Touch();		// mark persistent data as broken
		RStoreWriteStream strm;
		strm.OpenLC(iDatabase.Store(),iTokenId);
		strm<<token;
		strm.CommitL();
		CleanupStack::PopAndDestroy();
		}
	}

//
// Try to find a record in the index
// return ENoMatch if no key match, EEntryMatch if entire entry is present
// EKeyMatch if only key matches (only for unique indexes)
//
CDbStoreIndex::TFind CDbStoreIndex::FindL(TDbRecordId aRecordId,const RDbTableRow& aRow)
	{
	__ASSERT((!iTree.IsEmpty())==(Count()!=0));
	TUint8 entry[KMaxBtreeKeyLength];
	iKey->EntryL(entry,aRow,aRecordId);
	TBtreePos pos;
	if (!iTree.FindL(pos,iKey->Key(entry)))
		return ENoMatch;
	TDbRecordId id;
	iTree.ExtractAtL(pos,&id,sizeof(id));
	return id==aRecordId ? EEntryMatch : EKeyMatch;
	}

//
// Add the row to the index
// return True if insertion was good, false if duplicate found
//
TBool CDbStoreIndex::DoInsertL(TDbRecordId aRecordId,const RDbTableRow& aRow)
	{
	__ASSERT((!iTree.IsEmpty())==(Count()!=0));
	TUint8 entry[KMaxBtreeKeyLength];
	TInt len=iKey->EntryL(entry,aRow,aRecordId);
	TBtreePos pos;
	TBool insert=iTree.InsertL(pos,entry,len);
	if (insert)
		iStats.Inc();
	return insert;
	}

//
// Remove row from index
//
void CDbStoreIndex::DoDeleteL(TDbRecordId aRecordId,const RDbTableRow& aRow)
	{
	__ASSERT((!iTree.IsEmpty())==(Count()!=0));
	TUint8 entry[KMaxBtreeKeyLength];
	iKey->EntryL(entry,aRow,aRecordId);
	__DEBUG(TInt dbgchk =)iTree.DeleteL(iKey->Key(entry));
	__ASSERT(dbgchk);
	iStats.Dec();
	}

//
// Provide an iterator for the index ordering
//
CDbRecordIter* CDbStoreIndex::IteratorL(TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound)
	{
	return CIter::NewL(*this,aInclusion,aLowerBound,aUpperBound);
	}

//
// repair the tree from the sequence set after reclamation of the page pool
//
void CDbStoreIndex::RepairL()
	{
	if (!iTree.IsEmpty())	// empty trees are unbreakable
		{
		TouchL();
		iTree.MarkBroken();
		iStats.Reset(iTree.RepairL());
		SynchL();
		}
	}

//
// Throw away the index data, also used prior to a recovering rebuild
//
void CDbStoreIndex::DiscardL()
	{
	TouchL();
	iTree.ClearL();
	iStats.Reset();
	MarkIntact();
	}

//
// Throw away the token
//
void CDbStoreIndex::DestroyL()
	{
	iDatabase.Store().DeleteL(iTokenId);
	}

// Class CDbStoreIndex::CDiscarder

CDbStoreIndex::CDiscarder::CDiscarder()
	{}

CDbStoreIndex::CDiscarder::~CDiscarder()
	{
	delete iIndex;
	}

TInt CDbStoreIndex::CDiscarder::Open(CDbStoreIndex* anIndex)
	{
	__ASSERT(!iIndex);
	iIndex=anIndex;
	return 1;
	}

//
// Do the single step of index discard
//
TInt CDbStoreIndex::CDiscarder::StepL(TInt)
	{
	__ASSERT(iIndex);
	CDbStoreIndex& index=*iIndex;
	index.DiscardL();
	index.DestroyL();
	return 0;
	}
