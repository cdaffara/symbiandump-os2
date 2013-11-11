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

#include "UD_STD.H"

const TInt KPakArrayGranularity=0x80;

// class TDbCol
EXPORT_C TDbCol::TDbCol(const TDesC &aName,TDbColType aType)
	: iType(aType), iAttributes(0), iName(aName)
	{
	iMaxLength=(aType==EDbColText8 || aType==EDbColText16)
		? KDbDefaultTextColLength : KDbUndefinedLength;
	}

EXPORT_C TDbCol::TDbCol(const TDesC &aName,TDbColType aType,TInt aMaxLength)
	: iType(aType), iMaxLength(aMaxLength), iAttributes(0), iName(aName)
/** Constructs a TDbCol with the given name, optional type and optional maximum 
length.

Note: The iAttributes member is initialised to 0.

@param aName The column name.
@param aType The column type.
@param aMaxLength If present, this specifies a limit on how many characters 
may be stored in a Text column, or how many bytes can be stored in a Binary 
column. By default this is set to KDbDefaultTextColLength for Text (but not 
LongText) columns and KDbUndefinedLength for other columns. */
	{}

// class CDbColSet
EXPORT_C CDbColSet::CDbColSet()
: iColumns(KPakArrayGranularity)
/** Constructs an empty column set. */
	{}

EXPORT_C CDbColSet* CDbColSet::NewL()
/** Constructs a new empty column set and returns a pointer to it.

@return A pointer to the column set object. */
	{
	return new(ELeave) CDbColSet;
	}

EXPORT_C CDbColSet* CDbColSet::NewLC()
/** Constructs a new empty column set and returns a pointer to it — placing 
the pointer onto the cleanup stack. This allows the column set object and 
allocated resources to be cleaned up if a subsequent leave occurs.

@return A pointer to the column set object. */
	{
	CDbColSet* cs=NewL();
	CleanupStack::PushL(cs);
	return cs;
	}

EXPORT_C CDbColSet::~CDbColSet()
/** Frees resources owned by the object. */
	{}

EXPORT_C const TDbCol* CDbColSet::Col(const TDesC &aColName) const
/** Returns a named column definition in the set. If such a column does not 
exist in the set NULL is returned.

@param aColName The name of the column to find.
@return A pointer to the column definition found, or NULL. */
	{
	TDbColNo col=ColNo(aColName);
	if (col==KDbNullColNo)
		return NULL;
	else
		return &(*this)[col];
	}

EXPORT_C TDbColNo CDbColSet::ColNo(const TDesC &aColName) const
/** Returns the ordinal for a particular column name in this column set. If 
such a column does not exist in the set the special ordinal KDbNullColNo is 
returned.

This function is particularly important when accessing data through a rowset. 
If the set of columns to be returned by a rowset is not explicitly specified, 
no assumptions should be made about the ordering of the columns returned. 
In such a case, in order to access the column data a column ordinal is required, 
and this can be obtained by using this function on the column set returned 
by RDbRowSet::ColSetL().

@param aColName The name of the column to find.
@return The ordinal number of the column. */
	{
	TInt pos;
	TKeyArrayPak key(_FOFF(TDbCol,iName),ECmpFolded);
	if (iColumns.Find(TDbCol(aColName),key,pos))
		return KDbNullColNo;
	else
		return pos+1;
	}

EXPORT_C CDbColSet& CDbColSet::AddL(const TDbCol& aCol)
/** Adds a column to the column set.

@param aCol The column to add to the set.
@return A reference to this object. */
	{
	iColumns.AppendL(aCol,REINTERPRET_CAST(const TUint8*,aCol.iName.Ptr())+aCol.iName.Size()-REINTERPRET_CAST(const TUint8*,&aCol));
	return *this;
	}

EXPORT_C void CDbColSet::Remove(const TDesC &aColName)
/** Removes the named column from the set.

@param aColName The name of the column to remove from the set. */
	{
	TDbColNo col=ColNo(aColName);
	__ASSERT_ALWAYS(col!=KDbNullColNo,Panic(EDbInvalidColumn));
	iColumns.Delete(col-1);
	}

// Class TDbColSetIter
EXPORT_C TDbColSetIter::TDbColSetIter(const CDbColSet& aColSet)
	: iIndex(-1),iArray(&aColSet.iColumns)
/** Constructs a column set iterator over a column set. The iterator now 
references the first column in the set.

@param aColSet The column set to iterate over. */
	{
	++(*this);
	}

EXPORT_C TDbColSetIter& TDbColSetIter::operator++()
/** Moves the iterator to the next column in the set -- post increment operator.

Note that this is implemented in terms of the pre-increment operator, and 
is less efficient.

@param Unused: required for the C++ compiler to resolve the ambiguity with 
the pre-increment operator.
@return A copy of this iterator, referring to the column definition before 
the increment operation is performed. */
	{
	__ASSERT(iIndex<iArray->Count());
	iColumn=++iIndex<iArray->Count() ? &(*iArray)[iIndex] : 0;
	return *this;
	}

// class TDbKeyCol
EXPORT_C TDbKeyCol::TDbKeyCol(const TDesC& aName,TOrder anOrder)
	: iOrder(anOrder),iLength(KDbUndefinedLength),iName(aName)
	{}

EXPORT_C TDbKeyCol::TDbKeyCol(const TDesC& aName,TInt aLength,TOrder anOrder)
	: iOrder(anOrder),iLength(aLength),iName(aName)
/** Constructs an object with the given name, ordering and optional truncation 
length.

@param aName The column name.
@param aLength If present, this specifies a limit on how many characters of 
a Text or LongText column are used for the key. This should only be used for 
the last key column in an index. It is required for keys on LongText columns.
@param anOrder The ordering for the key column. By default this is ascending 
order. */
	{}

// class CDbKey

EXPORT_C CDbKey::CDbKey()
	: iKeys(KPakArrayGranularity)
/** Constructs an empty key. It is initialised to non-unique, non-primary and 
normal text comparison. */
	{}

EXPORT_C CDbKey *CDbKey::NewL()
/** Constructs and returns a pointer to a new empty key.

@return A pointer to the key object. */
	{
	return new(ELeave) CDbKey;
	}

EXPORT_C CDbKey *CDbKey::NewLC()
/** Constructs and returns a new empty key and return a pointer to it, leaving 
a pointer to the key object on the cleanup stack. This allows the key object 
and allocated resources to be cleaned up if a subsequent leave occurs. 

@return A pointer to the key object. */
	{
	CDbKey* key=NewL();
	CleanupStack::PushL(key);
	return key;
	}

EXPORT_C CDbKey::~CDbKey()
/** Frees resources owned by the object. */
	{}

EXPORT_C CDbKey& CDbKey::AddL(const TDbKeyCol& aKey)
/** Adds a key column to the end of the key.
	
@param aKeyCol The key column to add to the key.
@return A reference to this object. */
	{
	iKeys.AppendL(aKey,REINTERPRET_CAST(const TUint8*,aKey.iName.Ptr())+aKey.iName.Size()-REINTERPRET_CAST(const TUint8*,&aKey));
	return *this;
	}

EXPORT_C void CDbKey::Remove(const TDesC& aColName)
/** Removes the named column from the key.

@param aColName The name of the column to remove from the key. */
	{
	TInt pos;
	TKeyArrayPak key(_FOFF(TDbKeyCol,iName),ECmpFolded);
	__ASSERT_ALWAYS(!iKeys.Find(TDbKeyCol(aColName),key,pos),Panic(EDbInvalidColumn));
	iKeys.Delete(pos);
	}

EXPORT_C void CDbKey::Clear()
/** Resets the key to its initial empty state. */
	{
	iKeys.Reset();
	iAttributes=0;
	iComparison=EDbCompareNormal;
	}

// Class TDbLookupKey

TDbLookupKey::SColumn& TDbLookupKey::NextKey()
	{
	return iKey[iCount++];
	}

void TDbLookupKey::Add(TInt aKey)
	{
	SColumn& key=NextKey();
	key.iType=EDbColInt32;
	key.iInt32=aKey;
	}

void TDbLookupKey::Add(TUint aKey)
	{
	SColumn& key=NextKey();
	key.iType=EDbColUint32;
	key.iUint32=aKey;
	}

void TDbLookupKey::Add(TInt64 aKey)
	{
	SColumn& key=NextKey();
	key.iType=EDbColInt64;
	key.iInt64()=aKey;
	}

void TDbLookupKey::Add(TReal32 aKey) __SOFTFP
	{
	SColumn& key=NextKey();
	key.iType=EDbColReal32;
	key.iReal32=aKey;
	}

void TDbLookupKey::Add(TReal64 aKey) __SOFTFP
	{
	SColumn& key=NextKey();
	key.iType=EDbColReal64;
	key.iReal64=aKey;
	}

void TDbLookupKey::Add(TTime aKey)
	{
	SColumn& key=NextKey();
	key.iType=EDbColDateTime;
	key.iTime()=aKey;
	}

void TDbLookupKey::Add(const TDesC8& aKey)
	{
	SColumn& key=NextKey();
	key.iType=EDbColText8;
	key.iDes8.iPtr=aKey.Ptr();
	key.iDes8.iLength=aKey.Length();
	}

void TDbLookupKey::Add(const TDesC16& aKey)
	{
	SColumn& key=NextKey();
	key.iType=EDbColText16;
	key.iDes16.iPtr=aKey.Ptr();
	key.iDes16.iLength=aKey.Length();
	}

// Class TDbSeekKey

TDbLookupKey& TDbSeekKey::Check()
	{
	__ASSERT_ALWAYS(iKey.Count()<iMaxKeys,Panic(EDbTooManyKeys));
	return iKey;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(TInt aKey)
/** Appends a key value for an TInt8, TInt16 or TInt32 column.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(TUint aKey)
/** Appends a key value for a Bit, TUint8, TUint16 or TUint32 column.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(TInt64 aKey)
/** Appends a key value for an TInt64 column.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(TReal32 aKey) __SOFTFP
/** Appends a key value for a TReal32 column.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(TReal64 aKey) __SOFTFP
/** Appends a key value for a TReal64 column.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(TTime aKey)
/** Appends a key value for a DateTime column.

@param aKey The key value to lookup. TTime may be either local time or universal time. 
DBMS doesn't interpret the value of TTime, it is left up to the user to decide which should be used.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(const TDesC8& aKey)
/** Appends a key value for a non-Unicode text column.

Note that the seek key does not copy the text data contained by the descriptor. 
This needs to be retained until the seek key is no longer required.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

EXPORT_C TDbSeekKey& TDbSeekKey::Add(const TDesC16& aKey)
/** Appends a key value for a Unicode text column.

Note that the seek key does not copy the text data contained by the descriptor. 
This needs to be retained until the seek key is no longer required.

@param aKey The key value to lookup.
@return A reference to this database key value object. */
	{
	Check().Add(aKey);
	return *this;
	}

// Class CDbNames

inline CDbNames::CDbNames()
	: iList(KPakArrayGranularity)
	{}

CDbNames* CDbNames::NewLC()
	{
	CDbNames* self=new(ELeave) CDbNames;
	CleanupStack::PushL(self);
	return self;
	}

CDbNames::~CDbNames()
	{}

EXPORT_C void CDbNames::AddL(const TDesC& aName)
	{
	TDbNameC name(aName);
	iList.AppendL(name,(const TUint8*)name.Ptr()+name.Size()-(const TUint8*)&name);
	}

////////////////////////////////////////////////////////////////////////////////////////////////
// Class CDbStrings

/**
Granularity of CDbStrings array.
@internalComponent
*/
const TInt KPakArrayGranularity2 = KDbMaxStrLen * 2;

/**
@internalComponent
*/
inline CDbStrings::CDbStrings() :
	iList(KPakArrayGranularity2)
	{
	}

/**
Standard phase-one creation method for CDbStrings objects.
@return A pointer to the created CDbStrings object.
@leave KErrNoMemory - Out of memory.
*/
CDbStrings* CDbStrings::NewLC()
	{
	CDbStrings* self = new(ELeave) CDbStrings;
	CleanupStack::PushL(self);
	return self;
	}

/**
@internalComponent
*/
CDbStrings::~CDbStrings()
	{
	}

/**
Adds a string to the array.
@param aStr String to be added to the array.
@leave KErrNoMemory - Out of memory.
@internalComponent
*/
void CDbStrings::AddL(const TDesC& aStr)
	{
	TDbStringC str(aStr);
	iList.AppendL(str, (const TUint8*)str.Ptr() + str.Size() - (const TUint8*)&str);
	}
