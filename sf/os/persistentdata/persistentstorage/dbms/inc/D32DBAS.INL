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

// Class TTextOps
inline TUint TTextOps::Fold(TUint aChar) const
	{
    return iFold(aChar);
    }

inline TInt TTextOps::Match(const TDesC8& aDes,const TDesC8& aPattern) const
	{
    return iMatch8(aDes,aPattern);
    }

inline TInt TTextOps::Compare(const TText8* aLeftPtr,
                              TInt aLeftLen,
                              const TText8* aRightPtr,
                              TInt aRightLen) const
	{
    return iCompare8(aLeftPtr,aLeftLen,aRightPtr,aRightLen);
    }

inline TInt TTextOps::Match(const TDesC16& aDes,const TDesC16& aPattern) const
	{
    return iMatch16(aDes,aPattern);
    }

inline TInt TTextOps::Compare(const TText16* aLeftPtr,
                              TInt aLeftLen,
                              const TText16* aRightPtr,
                              TInt aRightLen) const
	{
    return iCompare16(aLeftPtr,aLeftLen,aRightPtr,aRightLen);
    }

/**
The method compares aLeftPtr and aRightPtr unicode strings.
Collation level 3 will be used. 
@param aLeftPtr Left string to compare.
@param aLeftLen The length of left string.
@param aRightPtr Right string to compare.
@param aRightLen The length of right string.
This method is used by sorting algorithms when the key field is a unciode string.
@return Positive. if aLeftPtr is greater than aRightPtr.
        Negative. if aLeftPtr is less than aRightPtr.
        Zero, if aLeftPtr is equal to aRightPtr.
*/
inline TInt TTextOps::Order(const TText16* aLeftPtr, TInt aLeftLen, const TText16* aRightPtr, TInt aRightLen) const
	{
	return iOrder16(aLeftPtr, aLeftLen, aRightPtr, aRightLen);
	}

// Class TDbBlob
inline TDbBlob::TDbBlob() :
	iId(KDbNullBlobId), 
    iSize(0)
	{
    }

inline TDbBlob::TDbBlob(TDbBlobId anId,TInt aSize) :
	iId(anId), 
    iSize(aSize)
	{
    }

inline TDbBlob::TDbBlob(const TUint8* aPtr,TInt aSize) :
	iId(KDbNullBlobId), 
    iSize(aSize)
	{
	__ASSERT_DEBUG(aSize>=0&&aSize<=KDbMaxInlineBlobSize,User::Invariant());
	Mem::Copy(iInline,aPtr,aSize);
	}

inline TUint8* TDbBlob::InlineBuffer()
	{
	__ASSERT_DEBUG(IsInline(),User::Invariant());
	return iInline;
	}

inline void TDbBlob::SetId(TDbBlobId anId)
	{
    iId=anId;
    }

inline void TDbBlob::SetSize(TInt aSize)
	{
    iSize=aSize;
    }

inline TBool TDbBlob::IsInline() const
	{
    return iId==KDbNullBlobId;
    }

inline TInt TDbBlob::Size() const
	{
    return iSize;
    }

inline const TUint8* TDbBlob::Data() const
	{
	__ASSERT_DEBUG(IsInline(),User::Invariant());
	return iInline;
	}

inline TPtrC8 TDbBlob::PtrC8() const
	{
	__ASSERT_DEBUG(IsInline(),User::Invariant());
	return TPtrC8(iInline,iSize);
	}

inline TPtrC16 TDbBlob::PtrC16() const
	{
	__ASSERT_DEBUG(IsInline(),User::Invariant());
	return TPtrC16((const TUint16*)iInline,iSize>>1);
	}

inline TDbBlobId TDbBlob::Id() const
	{
	__ASSERT_DEBUG(!IsInline(),User::Invariant());
	return iId;
	}

inline TInt TDbBlob::CellSize() const
	{
    return _FOFF(TDbBlob,iInline[IsInline() ? Size() : 0]);
    }

inline TInt TDbBlob::InlineSize(TInt aSize)
	{
    return _FOFF(TDbBlob,iInline[aSize]);
    }

inline TInt TDbBlob::RefSize()
	{
    return _FOFF(TDbBlob,iInline[0]);
    }

// CLass TDbCell
inline TInt TDbCell::Size(TInt aLength)
	{ 
    return Align4(aLength+sizeof(TInt)); 
    }

inline TInt TDbCell::Size() const
	{
    return Size(iLength);
    }

inline TInt TDbCell::Length() const
	{
    return iLength;
    }

inline const TDbCell* TDbCell::Next() const
	{
    return PtrAdd(this,Size());
    }

inline TDbCell* TDbCell::Next()
	{
    return PtrAdd(this,Size());
    }

inline TAny* TDbCell::Data()
	{
    return iBuf;
    }

inline const TAny* TDbCell::Data() const
	{
    return iBuf;
    }

inline void TDbCell::SetLength(TInt aLength)
	{
    iLength=aLength;
    }

// Class TDbColumnC
inline TDbColumnC::TDbColumnC(const RDbRow& aRow,TDbColNo aCol) :
	iCell(aRow.ColCell(aCol))
	{
    }

inline TInt TDbColumnC::Size() const
	{
    return iCell->Length();
    }

inline TBool TDbColumnC::IsNull() const
	{
    return Size()==0;
    }

inline TInt8 TDbColumnC::Int8() const
	{
    return TInt8(*(const TInt32*)iCell->Data());
    }

inline TInt16 TDbColumnC::Int16() const
	{
    return TInt16(*(const TInt32*)iCell->Data());
    }

inline TInt32 TDbColumnC::Int32() const
	{
    return *(const TInt32*)iCell->Data();
    }

inline const TInt64& TDbColumnC::Int64() const
	{
    return *(const TInt64*)iCell->Data();
    }

inline TUint8 TDbColumnC::Uint8() const
	{
    return TUint8(*(const TUint8*)iCell->Data());
    }

inline TUint16 TDbColumnC::Uint16() const
	{
    return TUint16(*(const TUint32*)iCell->Data());
    }

inline TUint32 TDbColumnC::Uint32() const
	{
    return *(const TUint32*)iCell->Data();
    }

inline TReal32 TDbColumnC::Real32() const
	{
    return *(const TReal32*)iCell->Data();
    }

inline const TReal64& TDbColumnC::Real64() const
	{
    return *(const TReal64*)iCell->Data();
    }

inline TPtrC8 TDbColumnC::PtrC8() const
	{
    return TPtrC8((const TUint8*)iCell->Data(),iCell->Length());
    }

inline TPtrC16 TDbColumnC::PtrC16() const
	{
    return TPtrC16((const TUint16*)iCell->Data(),iCell->Length()>>1);
    }

inline const TTime& TDbColumnC::Time() const
	{
    return *(const TTime*)iCell->Data();
    }

inline const TDbBlob& TDbColumnC::Blob() const
	{
    return *(const TDbBlob*)iCell->Data();
    }

// Class TDbColumn
inline TDbColumn::TDbColumn()
#ifdef _DEBUG
	: iRow(0)
#endif
	{
    }

inline TDbColumn::TDbColumn(RDbRow& aRow,TDbColNo aCol) :
	iRow(&aRow), 
    iColumn(aCol)
	{
    }

inline RDbRow& TDbColumn::Row() const
	{
    __ASSERT_DEBUG(iRow,User::Invariant());
    return *iRow;
    }

inline void TDbColumn::SetNull()
	{
	//SetColumnWidthL() can't leave when the second parameter value is 0 ("aWidth" parameter).
	//TRAP_IGNORE() not used because makes the code heavier and SetNull() is a public, exported, inline method.
	//coverity[naming_error]
    Row().SetColumnWidthL(iColumn,0);
    }

inline void TDbColumn::SetL(TInt32 aValue)
	{
    SetL(TUint32(aValue));
    }

inline void TDbColumn::SetL(const TTime& aValue)
	{
    SetL(aValue.Int64());
    }

inline void TDbColumn::SetL(const TDbCell* aCell)
	{
    SetL(aCell->Data(),aCell->Length());
    }

inline void TDbColumn::SetL(const TDbColumnC& aColumn)
	{
    SetL(aColumn.iCell);
    }

// Class TDbColumnC
inline TDbColumnC::TDbColumnC(const TDbColumn& aCol) :
	iCell(aCol.Row().ColCell(aCol.iColumn))
	{
    }

// Class RDbRow
inline RDbRow::RDbRow(TAny* aBuf,TInt aMaxSize)
	{
    Open(aBuf,0,aMaxSize);
    }

inline RDbRow::RDbRow(TAny* aBuf,TInt aSize,TInt aMaxSize)
	{
    Open(aBuf,aSize,aMaxSize);
    }

inline void RDbRow::Open(TAny* aBuf,TInt aMaxSize)
	{
    Open(aBuf,0,aMaxSize);
    }

inline void RDbRow::Reset()
	{
    SetSize(0);
    }

inline TDbCell* RDbRow::First()
	{
    return iFirst;
    }

inline const TDbCell* RDbRow::First() const
	{
    return iFirst;
    }

inline const TDbCell* RDbRow::Last() const
	{
    return iLast;
    }

inline const TDbCell* RDbRow::End() const
	{
    return iEnd;
    }

inline TInt RDbRow::Diff(const TDbCell* aLeft,const TDbCell* aRight)
	{
    return (const TUint8*)aRight-(const TUint8*)aLeft;
    }

inline TInt RDbRow::Size() const
	{
    return Diff(iFirst,iLast);
    }

inline TInt RDbRow::MaxSize() const
	{
    return Diff(iFirst,iEnd);
    }

// Class CDbObject
inline const CDbContext* CDbObject::Context() const
	{
    return iContext;
    }


// Class RDbHandleBase
inline void RDbHandleBase::Set(CDbObject* aObject)
	{
    iObject=aObject;
    }

// template Class RDbHandle
template <class T>
inline T*  RDbHandle<T>::operator=(T* aT)
	{
    Set(aT);return aT;
    }

template <class T>
inline T* RDbHandle<T>::operator->() const
	{
    return &STATIC_CAST(T&,Object());
    }

template <class T>
inline T& RDbHandle<T>::operator*() const
	{
    return STATIC_CAST(T&,Object());
    }

template <class T>
inline T* RDbHandle<T>::operator()() const
	{
    return STATIC_CAST(T*,iObject);
    }
