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
// Order-by stage for the cursor data "pipeline"
// 
//

#include "UT_STD.H"
#include "D32COMP.H"

#ifdef _DEBUG
#define _CHECK_ORDERING		// forces a full test of the ordering after sorting
#endif

#define MAX( a, b ) ( (a) > (b) ? (a) : (b) )

/* The key structure.

Each key value is always aligned on a 32-bit boundary to allow word reads and writes
integral values are always atored as 32-bit values just as for the row buffer
Text columns are encoded as follows (trailing padding is omitted from the description)

  Text8 columns			<byte n><n ASCII characters>
  n is the [character] length of the column

  Text16 columns		<short n><n UNICODE characters>
  n is the [character] length of the column

  LongText8 columns		<word n><n ASCII characters>
                 or		<word n|ETrunc><32 ASCII characters><blobId>
  n is [byte] size of the column

  LongText16 columns	<word n><n/2 UNICODE characters>
                  or	<word n|ETrunc><16 UNICODE characters><blobId>
  n is [byte] size of the column

*/


class CDbOrderByStage::HOrdering
	{
private:
	struct TKeyCol
		{
		TDbColNo iOrdinal;
		TUint8 iType;
		TUint8 iOrder;
		TUint8 iLength;
		};
	struct TBlobKey
		{
		enum {ETrunc=(TInt)0x80000000};
		enum {ETruncSize=32};		// store 32 bytes of truncated BLOBs
	public:
		inline TInt Size() const;
	public:
		TInt iSize;
		union {TUint8 iData8[ETruncSize]; TUint16 iData16[ETruncSize>>1];};
		TDbBlobId iId;
		};
public:
	static HOrdering* NewL(CDbTable& aTable,const CDbKey& aKey);
	TAny* EntryL(TAny* aPtr,const RDbTableRow& aRow) const;
	TInt CompareL(const TAny* aLeft,const TAny* aRight) const;
	TInt MaxSize() const;
private:
	inline HOrdering(TInt aCount,TDbTextComparison aComparison,CDbTable& aTable);
	MStreamBuf* BlobLC(TDbBlobId aId,TDbColType aType) const;
	TInt CompareLongText8L(const TBlobKey& aLeft,const TBlobKey& aRight) const;
	TInt CompareLongText16L(const TBlobKey& aLeft,const TBlobKey& aRight) const;
private:
	CDbTable& iTable;
	const TTextOps& iTextOps;
	const TKeyCol* iEndOfKeys;
	TKeyCol iKeys[1];	// one or more keys
	};

// Class HDbOrdering

inline TInt CDbOrderByStage::HOrdering::TBlobKey::Size() const
	{
	TInt size=_FOFF(TBlobKey,iData8[iSize+3]);
	return (size&ETrunc) ? sizeof(TBlobKey) : size&~3;
	}

inline CDbOrderByStage::HOrdering::HOrdering(TInt aCount,TDbTextComparison aComparison,CDbTable& aTable) :
	iTable(aTable),
	iTextOps(TTextOps::Ops(aComparison)),
	iEndOfKeys(&iKeys[aCount])
	{
	}

//
// Construct the ordering based on the key definition (must be valid for the table)
// and the column set provided
//
CDbOrderByStage::HOrdering* CDbOrderByStage::HOrdering::NewL(CDbTable& aTable,const CDbKey& aKey)
	{
	TInt count=aKey.Count();
	__ASSERT(count>0);
	HOrdering* self=new(User::AllocLC(_FOFF(HOrdering,iKeys[count])))
								HOrdering(count,aKey.Comparison(),aTable);
	TKeyCol* pKey=&self->iKeys[0];
	const HDbColumnSet& columns=aTable.Def().Columns();
	for (TInt ii=0;ii<count;++pKey,++ii)
		{
		const TDbKeyCol& key=aKey[ii];
		pKey->iOrder=TUint8(key.iOrder);
		pKey->iOrdinal=columns.ColNoL(key.iName);
		if (pKey->iOrdinal==KDbNullColNo)
			__LEAVE(KErrNotFound);
		const TDbColumnDef& col=columns[pKey->iOrdinal];
		switch (pKey->iType=col.iType)
			{
		default:
			break;
		case EDbColText8:
		case EDbColText16:
			{
			TInt l=col.iMaxLength;
			if (l<256)
				{
				pKey->iLength=TUint8(l);
				break;
				}
			}
			// fall through to argument error
		case EDbColBinary:
		case EDbColLongBinary:
			__LEAVE(KErrArgument);
			}
		}
	CleanupStack::Pop();
	return self;
	}

TInt CDbOrderByStage::HOrdering::MaxSize() const
	{
	TInt maxsize=0;
	const TKeyCol* const end=iEndOfKeys;
	const TKeyCol* key=&iKeys[0];
	__ASSERT(key<end);
	do	{
		TInt s;
		switch (key->iType)
			{
		default:
			s=sizeof(TUint32);
			break;
		case EDbColInt64:
			s=sizeof(TInt64);
			break;
		case EDbColDateTime:
			s=sizeof(TTime);
			break;
		case EDbColReal64:
			s=sizeof(TReal64);
			break;
		case EDbColText8:
			s=Align4(key->iLength+1);
			break;
		case EDbColText16:
			s=Align4((key->iLength<<1)+1);
			break;
		case EDbColLongText8:
		case EDbColLongText16:
			s=MAX(__Align8(sizeof(TUint32)+KDbMaxInlineBlobSize),sizeof(TBlobKey));
			break;
			}
		maxsize+=s;
		} while (++key<end);
	return maxsize;
	}

MStreamBuf* CDbOrderByStage::HOrdering::BlobLC(TDbBlobId aId,TDbColType aType) const
	{
	return iTable.BlobsL()->ReadLC(aId,aType);
	}

//
// Construct an entry at aPtr from the record given
// iMaxSize bytes are assumed to be available at aPtr
// return the actual size of the entry
//
TAny* CDbOrderByStage::HOrdering::EntryL(TAny* aPtr,const RDbTableRow& aRow) const
	{
	__ASSERT(Align4(aPtr)==aPtr);
	TUint32* ptr=(TUint32*)aPtr;		// 32-bit words are nice
	const TKeyCol* const end=iEndOfKeys;
	const TKeyCol* key=iKeys;
	__ASSERT(key<end);
	do	{
		const TDbCell* cell=aRow.ColCell(key->iOrdinal);
		TDbColType type=TDbColType(key->iType);
		if (cell->Length()==0)
			{	// null column
			const TUint K64BitColumnMask=(1u<<EDbColInt64)|(1u<<EDbColReal64)|(1u<<EDbColDateTime);
			*ptr++=0;
			if (K64BitColumnMask&(1u<<type))
				*ptr++=0;		// 8-byte column
			continue;
			}
		switch (type)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
		case EDbColUint8:
		case EDbColUint16:
		case EDbColUint32:
		case EDbColInt8:
		case EDbColInt16:
		case EDbColInt32:
		case EDbColReal32:
			*ptr++=*(const TUint32*)cell->Data();
			break;
		case EDbColInt64:
		case EDbColDateTime:
		case EDbColReal64:
			{
			const TUint32* data=(const TUint32*)cell->Data();
			*ptr++=*data++;
			*ptr++=*data;
			}
			break;
		case EDbColText8:
			{
			TInt size=cell->Length();
			*(TUint8*)ptr=TUint8(size);
			ptr=(TUint32*)Align4(Mem::Copy(PtrAdd(ptr,1),cell->Data(),size));
			}
			break;
		case EDbColText16:
			{
			TInt size=cell->Length();
			*(TUint16*)ptr=TUint16(size>>1);
			ptr=(TUint32*)Align4(Mem::Copy(PtrAdd(ptr,2),cell->Data(),size));
			}
			break;
		case EDbColLongText8:
		case EDbColLongText16:
			{
			const TDbBlob& blob=*(const TDbBlob*)cell->Data();
			TInt size=blob.Size();
			if (blob.IsInline())
				{
				*ptr++=size;
				ptr=(TUint32*)Align4(Mem::Copy(ptr,blob.Data(),size));
				}
			else
				{
				TInt rsize=size;
				if (size>TBlobKey::ETruncSize)
					{
					size|=TBlobKey::ETrunc;
					rsize=TBlobKey::ETruncSize;
					}
				*ptr++=size;
				BlobLC(blob.Id(),TDbColType(key->iType))->ReadL(ptr,rsize);
				CleanupStack::PopAndDestroy();
				ptr=Align4(PtrAdd(ptr,rsize));
				if (size&TBlobKey::ETrunc)
					*ptr++=blob.Id();
				}
			}
			break;
			}
		} while (++key<end);
	return ptr;
	}

TInt CDbOrderByStage::HOrdering::CompareLongText8L( const CDbOrderByStage::HOrdering::TBlobKey& aLeft, const CDbOrderByStage::HOrdering::TBlobKey& aRight ) const
	{
	TUint sizel = aLeft.iSize;
	TUint sizer = aRight.iSize;
	TUint s = sizel;
	if ( sizer < s )
		s = sizer;
	if ( s > static_cast<TUint>( TBlobKey::ETruncSize ) && ( ( sizel | sizer ) & static_cast<TUint>( TBlobKey::ETrunc ) ) )
		s = TBlobKey::ETruncSize;
	TInt rr = iTextOps.Compare( aLeft.iData8, s, aRight.iData8, s );
	if ( rr )
		return rr;
//
// matches up to the same-length inlined data...
// now it gets interesting
//
	if ( ( ( sizel | sizer ) & static_cast<TUint>( TBlobKey::ETrunc ) ) == 0 )
		return sizel - sizer;		// neither are truncated
	if ( s == sizel )
		return -1;		// left completely match against truncated right
	if ( s == sizer )
		return 1;		// right completely matched against truncated left

	s = Min( TInt( sizel & ~TBlobKey::ETrunc ), TInt( sizer & ~TBlobKey::ETrunc ) );		// minimum real length
	if ( sizel & static_cast<TUint>( TBlobKey::ETrunc ) )
		{
		MStreamBuf& bufL = *BlobLC( aLeft.iId, EDbColLongText8 );
		if ( sizer & static_cast<TUint>( TBlobKey::ETrunc ) )
			{	// both out-of-line
			rr = Comp::Compare8L( bufL, *BlobLC( aRight.iId, EDbColLongText8 ), s, iTextOps );
			CleanupStack::PopAndDestroy();
			}
		else	// left side only out-of-line
			rr = Comp::Compare8L( bufL, aRight.iData8, s, iTextOps );
		}
	else
		{	// right side only out-of-line
		__ASSERT( sizer & static_cast<TUint>( TBlobKey::ETrunc ) );
		rr = -Comp::Compare8L( *BlobLC( aRight.iId, EDbColLongText8 ), aLeft.iData8, s, iTextOps );
		}
	CleanupStack::PopAndDestroy();
	return rr ? rr : ( sizel & ~TBlobKey::ETrunc ) - ( sizer & ~TBlobKey::ETrunc );
	}

TInt CDbOrderByStage::HOrdering::CompareLongText16L( const CDbOrderByStage::HOrdering::TBlobKey& aLeft, const CDbOrderByStage::HOrdering::TBlobKey& aRight ) const
	{
	TUint sizeL = aLeft.iSize  & ~TBlobKey::ETrunc; // set truncation bit to 0 to get true size
	TUint sizeR = aRight.iSize & ~TBlobKey::ETrunc;
	TBool truncL = aLeft.iSize  & TBlobKey::ETrunc; // data is truncated if TBlobKey::ETrunc bit is 1
	TBool truncR = aRight.iSize & TBlobKey::ETrunc;
	
	if (!(truncL | truncR)) // neither side is truncated, compare full strings
		{		
		return iTextOps.Order( aLeft.iData16, sizeL>>1, aRight.iData16, sizeR>>1 );
		}
		
	TInt result;
	TUint sizeMin = Min( TInt(sizeL), TInt(sizeR) ); // minimum real length
	if ( truncL )
		{
		MStreamBuf& bufL = *BlobLC( aLeft.iId, EDbColLongText16 );
		if ( truncR )
			{	// both out-of-line
			result = Comp::Compare16L( bufL, *BlobLC( aRight.iId, EDbColLongText16 ), sizeMin, iTextOps );
			CleanupStack::PopAndDestroy();
			}
		else	// left side only out-of-line
			result = Comp::Compare16L( bufL, aRight.iData16, sizeMin, iTextOps );
		}
	else
		{	// right side only out-of-line
		__ASSERT( truncR );
		result = -Comp::Compare16L( *BlobLC( aRight.iId, EDbColLongText16 ), aLeft.iData16, sizeMin, iTextOps );
		}
	CleanupStack::PopAndDestroy();
	return result ? result : ( sizeL ) - ( sizeR );
	}

TInt CDbOrderByStage::HOrdering::CompareL(const TAny* aLeft,const TAny* aRight) const
//
// compare the keys
//
	{
	const TUint8* left=(const TUint8*)aLeft;
	const TUint8* right=(const TUint8*)aRight;
	const TKeyCol* end=iEndOfKeys;
	const TKeyCol* key=&iKeys[0];
	TInt rr;
	for (;;)
		{
		switch (key->iType)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
		case EDbColUint8:
		case EDbColUint16:
		case EDbColUint32:
			rr=Comp::Compare(*(const TUint32*)left,*(const TUint32*)right);
			left+=sizeof(TUint32);
			right+=sizeof(TUint32);
			break;
		case EDbColInt8:
		case EDbColInt16:
		case EDbColInt32:
			rr=Comp::Compare(*(const TInt32*)left,*(const TInt32*)right);
			left+=sizeof(TInt32);
			right+=sizeof(TInt32);
			break;
		case EDbColInt64:
			rr=Comp::Compare(*(const TInt64*)left,*(const TInt64*)right);
			left+=sizeof(TInt64);
			right+=sizeof(TInt64);
			break;
		case EDbColDateTime:
			rr=Comp::Compare(*(const TTime*)left,*(const TTime*)right);
			left+=sizeof(TTime);
			right+=sizeof(TTime);
			break;
		case EDbColReal32:
			rr=Comp::Compare(*(const TReal32*)left,*(const TReal32*)right);
			left+=sizeof(TReal32);
			right+=sizeof(TReal32);
			break;
		case EDbColReal64:
			rr=Comp::Compare(*(const TReal64*)left,*(const TReal64*)right);
			left+=sizeof(TReal64);
			right+=sizeof(TReal64);
			break;
		case EDbColText8:
			{
			TInt sizel=*left++;
			TInt sizer=*right++;
			rr=iTextOps.Compare(left,sizel,right,sizer);
			left=Align4(left+sizel);
			right=Align4(right+sizer);
			}
			break;
		case EDbColText16:
			{
			const TUint16* l16=reinterpret_cast<const TUint16*>(left);
			const TUint16* r16=reinterpret_cast<const TUint16*>(right);
			TInt sizel=*l16++;
			TInt sizer=*r16++;
			rr=iTextOps.Order(l16,sizel,r16,sizer);
			left=Align4(reinterpret_cast<const TUint8*>(l16+sizel));
			right=Align4(reinterpret_cast<const TUint8*>(r16+sizer));
			}
			break;
		case EDbColLongText8:
			{
			const TBlobKey* ltext=(const TBlobKey*)left;
			const TBlobKey* rtext=(const TBlobKey*)right;
			rr=CompareLongText8L(*ltext,*rtext);
			left+=ltext->Size();
			right+=rtext->Size();
			}
			break;
		case EDbColLongText16:
			{
			const TBlobKey* ltext=(const TBlobKey*)left;
			const TBlobKey* rtext=(const TBlobKey*)right;
			rr=CompareLongText16L(*ltext,*rtext);
			left+=ltext->Size();
			right+=rtext->Size();
			}
			break;
			}
		if (rr!=0)
			break;
		if (++key==end)
			break;
		}
	return key->iOrder==TDbKeyCol::EAsc ? rr : -rr;
	}

//

NONSHARABLE_CLASS(CDbOrderByStage::CKeys) : public CBase
	{
public:		// should be private but VC++ 4.0 whinges
	struct TKey
		{
		TUint32 iId;
		TUint8 iKey[4];		// and then some
		};
private:
	enum {EKeysGranularity=32};
	struct TPage
		{
		TPage* iNext;
		TKey iEntries[1];
		};
	enum {EMinPageSize=0x200,EMinKeyElements=2};
public:
	CKeys(TInt aMaxKeySize);
	~CKeys();
//
	void AddL(TDbRecordId aRecordId,const RDbTableRow& aRow,const HOrdering& aOrder);
	void SortL(const HOrdering& aOrder);
	void GetRecordsL(CArrayFix<TDbRecordId>& aRecords);
#ifdef _CHECK_ORDERING
	void VerifyOrderingL(const HOrdering& aOrder);
#endif
private:
	void SortL(TKey* aData[],TInt aElem,const HOrdering& aOrder);
	TKey& NewKeyL();
	void KeyComplete(TAny* aEnd);
	void Release();
private:
	RPointerArray<TKey> iKeys;
	TInt iMaxKeySize;
	TInt iPageSize;
	TPage* iPages;
	TKey* iNext;
	TKey* iEnd;
	};

// Class CDbOrderByStage::CKeys

CDbOrderByStage::CKeys::CKeys(TInt aMaxKeySize)
	:iKeys(EKeysGranularity),iMaxKeySize(_FOFF(TKey,iKey[aMaxKeySize])),
	 iPageSize(Max(EMinPageSize+iMaxKeySize,iMaxKeySize*EMinKeyElements))
	{}

CDbOrderByStage::CKeys::~CKeys()
	{
	iKeys.Close();
	Release();
	}

void CDbOrderByStage::CKeys::SortL(CDbOrderByStage::CKeys::TKey* aData[],TInt aElem,const HOrdering& aOrder)
//
// Sort the array of row keys
// Uses Heap-sort
//
	{
	__ASSERT(aElem>1);
	TInt heap=aElem>>1;
	--aElem;
	for (;;)
		{
		TKey* key;
		if (heap!=0)
			key=aData[--heap];
		else
			{
			key=aData[aElem];
			aData[aElem]=aData[0];
			if (--aElem==0)
				{
				aData[0]=key;
				break;
				}
			}
		TInt ix=heap;
		TInt parent;
		for (parent=ix;;parent=ix)
			{
			ix=(ix<<1)+1;
			if (ix<=aElem)
				{
				if (ix<aElem && aOrder.CompareL(aData[ix]->iKey,aData[ix+1]->iKey)<0)
					++ix;
				}
			else
				break;
			if (aOrder.CompareL(aData[ix]->iKey,key->iKey)<=0)
				break;
			aData[parent]=aData[ix];
			}
		aData[parent]=key;
		}
	}

void CDbOrderByStage::CKeys::AddL(TDbRecordId aRecordId,const RDbTableRow& aRow,const HOrdering& aOrder)
//
// Create a key for the row and store it
//
	{
	TKey& key=NewKeyL();
	key.iId=aRecordId.Value();
	TAny* end=aOrder.EntryL(&key.iKey[0],aRow);
	__LEAVE_IF_ERROR(iKeys.Append(&key));
	KeyComplete(end);
	}

void CDbOrderByStage::CKeys::SortL(const HOrdering& aOrder)
	{
	TInt elem=iKeys.Count();
	if (elem>1)
		SortL(&iKeys[0],elem,aOrder);
	}

void CDbOrderByStage::CKeys::GetRecordsL(CArrayFix<TDbRecordId>& aRecords)
	{
	TInt elem=iKeys.Count();
	if (elem==0)
		return;
	TKey** const base=&iKeys[0];
	TKey** ptr=base+elem;
	do	{
		--ptr;
		*REINTERPRET_CAST(TDbRecordId*,ptr)=(*ptr)->iId;
		} while (ptr>base);
	Release();		// discard key memory before adding records
	aRecords.InsertL(0,REINTERPRET_CAST(const TDbRecordId*,base),elem);
	iKeys.Reset();
	}

void CDbOrderByStage::CKeys::Release()
//
// Discard all of the allocated pages
//
	{
	TPage* p=iPages;
	while (p)
		{
		TPage* n=p->iNext;
		User::Free(p);
		p=n;
		}
	iPages=0;
	iNext=iEnd=0;
	}

CDbOrderByStage::CKeys::TKey& CDbOrderByStage::CKeys::NewKeyL()
//
// Allocate a key entry for the raw key data
//
	{
	TKey* p=iNext;
	if (PtrAdd(p,iMaxKeySize)>iEnd)
		{	// not enough space for a maximum key
		TPage* page=iPages;
		if (page)
			{	// compress the current page
			__DEBUG(page=(TPage*))User::ReAlloc(page,(TUint8*)iNext-(TUint8*)page);
			__ASSERT(page==iPages);		// if it moves we are dead
			}
		page=(TPage*)User::AllocL(_FOFF(TPage,iEntries)+iPageSize);
		page->iNext=iPages;
		iPages=page;
		p=iNext=&page->iEntries[0];
		iEnd=PtrAdd(p,iPageSize);
		}
	return *p;
	}

void CDbOrderByStage::CKeys::KeyComplete(TAny* aEnd)
//
// Key is complete, prepare for the next one
//
	{
	__ASSERT(aEnd==Align4(aEnd));
	__ASSERT(iNext<=aEnd&&aEnd<=iEnd);
	iNext=STATIC_CAST(TKey*,aEnd);
	}

#ifdef _CHECK_ORDERING
void CDbOrderByStage::CKeys::VerifyOrderingL(const HOrdering& aOrder)
	{
// this performs a full O(N*N) comparison of the record set
	if (iKeys.Count()==0)
		return;
	TKey* const* data=&iKeys[0];
	for (TInt ii=iKeys.Count();--ii>=0;)
		{
		for (TInt jj=iKeys.Count();--jj>=0;)
			{
			TInt rr=aOrder.CompareL(data[ii]->iKey,data[jj]->iKey);
			if (ii==jj)
				__ASSERT_ALWAYS(rr==0,User::Invariant());
			else if (ii<jj)
				__ASSERT_ALWAYS(rr<=0,User::Invariant());
			else
				__ASSERT_ALWAYS(rr>=0,User::Invariant());
			}
		}
	}
#endif

// Class CDbOrderByStage

inline const RDbTableRow& CDbOrderByStage::Row()
	{return iRow;}
inline CDbOrderByStage::CKeys& CDbOrderByStage::Keys()
	{__ASSERT(iKeys);return *iKeys;}
inline const CDbOrderByStage::HOrdering& CDbOrderByStage::Order()
	{__ASSERT(iOrder);return *iOrder;}

CDbOrderByStage::CDbOrderByStage(const RDbTableRow& aRow)
	: CDbBasicWindowStage(KDbUnlimitedWindow),iRow(aRow)
	{
	__ASSERT(iState==EReset);
	}

CDbOrderByStage::~CDbOrderByStage()
	{
	delete iOrder;
	delete iKeys;
	}

void CDbOrderByStage::ConstructL(const CDbKey& aOrderBy)
//
// Build the key structures to support the partial and complete ordering
//
	{
	iOrder=HOrdering::NewL(Row().Table(),aOrderBy);
	}

void CDbOrderByStage::Reset()
//
// Reset the window to initial state
//
	{
	delete iKeys;
	iKeys=0;
	iState=EReset;
	CDbBasicWindowStage::Reset();
	}

TBool CDbOrderByStage::ReadL(TInt& aWork,TDbPosition aNext)
//
// Read some more record keys
//
	{
	TDbRecordId id(KDbNullRecordIdValue);
	TGoto r;
	while ((r=CDbDataStage::GotoL(aWork,aNext,id))==ESuccess)
		{
		CDbDataStage::ReadRowL(id);
		Keys().AddL(id,Row(),Order());
		aNext=EDbNext;
		}
	switch (r)
		{
	default:
		__ASSERT(0);
	case ESynchFailure:
		__LEAVE(KErrNotReady);
	case EExhausted:
		return ETrue;
	case ENoRow:
		return EFalse;
		}
	}

TBool CDbOrderByStage::DoEvaluateL(TInt& aWork)
//
// Build the key collection, and finally sort it
//
	{
	TState s=iState;
	iState=EFailed;
	switch (s)
		{
	default:
		__ASSERT(0);
	case EFailed:
		__LEAVE(KErrNotReady);
	case EReset:
		__ASSERT(!iKeys);
		iKeys=new(ELeave) CKeys(Order().MaxSize());
		// drop through to EEvaluate
	case EEvaluate:
		if (ReadL(aWork,s==EReset ? EDbFirst : EDbNext))
			{
			iState=EEvaluate;
			return ETrue;
			}
		// drop through to ESort
	case ESort:
		Keys().SortL(Order());
#ifdef _CHECK_ORDERING
		Keys().VerifyOrderingL(Order());
#endif
		// drop through to EAdd
	case EAdd:
		Keys().GetRecordsL(iRecords);
		delete iKeys;
		iKeys=0;
		// drop through to EComplete
	case EComplete:
		iState=EComplete;
		return EFalse;
		}
	}

TBool CDbOrderByStage::Unevaluated()
//
// Return whether it is worth Evaluating
//
	{
	if (iState==EComplete)
		return CDbDataStage::Unevaluated();
	return iState!=EFailed;
	}


// Class CDbReorderWindowStage

CDbReorderWindowStage::CDbReorderWindowStage()
	: CDbBasicWindowStage(KDbUnlimitedWindow),iRows(EGranularity)
	{
	__ASSERT(iState==EReset);
	}

CDbReorderWindowStage::~CDbReorderWindowStage()
	{
	iRows.Close();
	}

void CDbReorderWindowStage::Reset()
//
// Reset the window to initial state
//
	{
	iRows.Reset();
	iState=EReset;
	CDbBasicWindowStage::Reset();
	}

TBool CDbReorderWindowStage::ReadL(TInt& aWork,TDbPosition aNext)
//
// Read some more row ids
//
	{
	TDbRecordId id(KDbNullRecordIdValue);
	TGoto r;
	while ((r=CDbDataStage::GotoL(aWork,aNext,id))==ESuccess)
		{
		__LEAVE_IF_ERROR(iRows.Append(id.Value()));
		aNext=EDbNext;
		}
	switch (r)
		{
	default:
		__ASSERT(0);
	case ESynchFailure:
		__LEAVE(KErrNotReady);
	case EExhausted:
		return ETrue;
	case ENoRow:
		return EFalse;
		}
	}

TBool CDbReorderWindowStage::DoEvaluateL(TInt& aWork)
//
// Build the key collection, and finally sort it
//
	{
	TState s=iState;
	iState=EFailed;
	switch (s)
		{
	default:
		__ASSERT(0);
	case EFailed:
		__LEAVE(KErrNotReady);
	case EReset:
	case EEvaluate:
		if (ReadL(aWork,s==EReset ? EDbFirst : EDbNext))
			{
			iState=EEvaluate;
			return ETrue;
			}
		if (iRows.Count())
			{
			iRows.Sort();
			iRecords.AppendL((const TDbRecordId*)&iRows[0],iRows.Count());
			iRows.Reset();
			}
	case EComplete:
		iState=EComplete;
		return EFalse;
		}
	}

TBool CDbReorderWindowStage::Unevaluated()
//
// Return whether it is worth Evaluating
//
	{
	if (iState==EComplete)
		return CDbDataStage::Unevaluated();
	return iState!=EFailed;
	}

