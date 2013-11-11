// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UB_STD.H"

typedef union
	{
	const TAny* tany;
	const TInt8* tint8;
	const TInt16* tint16;
	const TInt32* tint32;
	const TInt64* tint64;
	const TUint8* tuint8;
	const TUint16* tuint16;
	const TUint32* tuint32;
	} UPTR;

EXPORT_C const TAny* MBtreeKey::Key(const TAny* anEntry) const
/** Gets the key value for an entry.

@param anEntry Object for which to get the key value
@return Pointer to the key value */
	{
	return anEntry;
	}

EXPORT_C TBtreeKey::TBtreeKey(TInt aLength)
//
// Discriminating key. Does a raw memory comparison on given length
//
	: iKeyOffset(0),iCmpType(ECmpNormal8),iKeyLength(aLength)
	{}

EXPORT_C TBtreeKey::TBtreeKey()
//
// Discriminating key. Does a raw memory comparison on variable length (byte counted)
//
	: iKeyOffset(0),iCmpType(ECmpCollated16+1+ECmpNormal8)
	{}

EXPORT_C TBtreeKey::TBtreeKey(TInt anOffset,TKeyCmpText aType)
//
// Used to specify a variable length text key, the first character is a count of the actual character data that follows
//
	: iKeyOffset(anOffset),iCmpType(ECmpCollated16+aType+1)
	{
	switch (aType)
		{
	case ECmpNormal:
	case ECmpFolded:
	case ECmpCollated:
		Panic(EInvalidKeyComparison);
	default:
		break;
		}
	}

EXPORT_C TBtreeKey::TBtreeKey(TInt anOffset,TKeyCmpText aType,TInt aLength)
//
// Used for fixed length charecter data. the length is the character count, not the byte size
//
	: iKeyOffset(anOffset),iCmpType(aType),iKeyLength(aLength)
	{
	switch (aType)
		{
	case ECmpNormal:
	case ECmpFolded:
	case ECmpCollated:
		Panic(EInvalidKeyComparison);
	default:
		break;
		}
	}

EXPORT_C TBtreeKey::TBtreeKey(TInt anOffset,TKeyCmpNumeric aType)
	: iKeyOffset(anOffset),iCmpType(aType)
	{
	switch (aType)
		{
	case ECmpTInt:
	case ECmpTUint:
		Panic(EInvalidKeyComparison);
	default:
		break;
		}
	}

EXPORT_C const TAny* TBtreeKey::Key(const TAny* anEntry) const
	{
	return PtrAdd(anEntry,iKeyOffset);
	}

EXPORT_C TInt TBtreeKey::Compare(const TAny* aLeft,const TAny* aRight) const
//
// do the right thing
//
	{
	UPTR left;
	left.tany=aLeft;
	UPTR right;
	right.tany=aRight;
	switch (iCmpType)
		{
	case ECmpNormal8:
		return Mem::Compare(left.tuint8,iKeyLength,right.tuint8,iKeyLength);
	case ECmpFolded8:
		return Mem::CompareF(left.tuint8,iKeyLength,right.tuint8,iKeyLength);
	case ECmpCollated8:
		return Mem::CompareC(left.tuint8,iKeyLength,right.tuint8,iKeyLength);
	case ECmpNormal16:
		return Mem::Compare(left.tuint16,iKeyLength,right.tuint16,iKeyLength);
	case ECmpFolded16:
		return Mem::CompareF(left.tuint16,iKeyLength,right.tuint16,iKeyLength);
	case ECmpCollated16:
		return Mem::CompareC(left.tuint16,iKeyLength,right.tuint16,iKeyLength);
	case ECmpCollated16+ECmpNormal8+1:
		return Mem::Compare(left.tuint8+1,*left.tuint8,right.tuint8+1,*right.tuint8);
	case ECmpCollated16+ECmpFolded8+1:
		return Mem::CompareF(left.tuint8+1,*left.tuint8,right.tuint8+1,*right.tuint8);
	case ECmpCollated16+ECmpCollated8+1:
		return Mem::CompareC(left.tuint8+1,*left.tuint8,right.tuint8+1,*right.tuint8);
	case ECmpCollated16+ECmpNormal16+1:
		return Mem::Compare(left.tuint16+1,*left.tuint16,right.tuint16+1,*right.tuint16);
	case ECmpCollated16+ECmpFolded16+1:
		return Mem::CompareF(left.tuint16+1,*left.tuint16,right.tuint16+1,*right.tuint16);
	case ECmpCollated16+ECmpCollated16+1:
		return Mem::CompareC(left.tuint16+1,*left.tuint16,right.tuint16+1,*right.tuint16);
	case ECmpTInt8:
		return *left.tint8-*right.tint8;
	case ECmpTUint8:
		return TInt(*left.tuint8)-TInt(*right.tuint8);
	case ECmpTInt16:
		return *left.tint16-*right.tint16;
	case ECmpTUint16:
		return TInt(*left.tuint16)-TInt(*right.tuint16);
	case ECmpTInt32:
		if (*left.tint32<*right.tint32) 
			return -1;
		if (*left.tint32>*right.tint32) 
			return 1;
		break;	   
	case ECmpTUint32:
		if (*left.tuint32<*right.tuint32)
			return -1;
		if (*left.tuint32>*right.tuint32)
			return 1;
		break;
	case ECmpTInt64:
		if (*left.tint64<*right.tint64) 
			return -1;
		if (*left.tint64>*right.tint64) 
			return 1;
		break;	   
	default:
		break;
		}
	return 0;
	}

EXPORT_C void TBtreeKey::Between(const TAny* aLeft,const TAny*  /*aRight*/,TBtreePivot& aPivot) const
	{
//#pragma message( __FILE__ " : 'TBtreeKey::Between()' whizzy pivot generation not implemented" )
	UPTR left;
	left.tany=aLeft;
//	UPTR right=(UPTR)aRight;
	switch (iCmpType)
		{
	case ECmpNormal8:
	case ECmpFolded8:
	case ECmpCollated8:
		aPivot.Copy(left.tuint8,iKeyLength);
		break;
	case ECmpNormal16:
	case ECmpFolded16:
	case ECmpCollated16:
		aPivot.Copy(left.tuint8,iKeyLength<<1);
		break;
	case ECmpCollated16+ECmpNormal8+1:
	case ECmpCollated16+ECmpFolded8+1:
	case ECmpCollated16+ECmpCollated8+1:
		aPivot.Copy(left.tuint8,1+*left.tuint8);	// include length count
		break;
	case ECmpCollated16+ECmpNormal16+1:
	case ECmpCollated16+ECmpFolded16+1:
	case ECmpCollated16+ECmpCollated16+1:
		aPivot.Copy(left.tuint8,(1+*left.tuint16)<<1);	// include length count
		break;
	case ECmpTInt8:
		aPivot.Copy(left.tuint8,sizeof(TInt8));
		break;
	case ECmpTUint8:
		aPivot.Copy(left.tuint8,sizeof(TUint8));
		break;
	case ECmpTInt16:
		aPivot.Copy(left.tuint8,sizeof(TInt16));
		break;
	case ECmpTUint16:
		aPivot.Copy(left.tuint8,sizeof(TUint16));
		break;
	case ECmpTInt32:
		aPivot.Copy(left.tuint8,sizeof(TInt16));
		break;
	case ECmpTUint32:
		aPivot.Copy(left.tuint8,sizeof(TUint32));
		break;
	case ECmpTInt64:
		aPivot.Copy(left.tuint8,sizeof(TInt64));
		break;
	default:
		break;
		}
	}

