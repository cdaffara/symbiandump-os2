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

// Class TDbCol
inline TDbCol::TDbCol(const TDesC &aName)
	: iName(aName)
	{
	}

/**
TDbCol copy constructor.
@param aSrcCol This TDbCol object will be constructed as an exact copy of aSrcCol object.
*/
inline TDbCol::TDbCol(const TDbCol& aSrcCol) :
	iType(aSrcCol.iType),
	iMaxLength(aSrcCol.iMaxLength),
	iAttributes(aSrcCol.iAttributes),
	iName(static_cast <const TDesC&> (aSrcCol.iName))
	{
	}
	
/**
TDbCol "=" operator.
@param aSrcCol This TDbCol object will be made to be an exact copy of aSrcCol object.
*/
inline TDbCol& TDbCol::operator=(const TDbCol& aSrcCol)
	{
	iType = aSrcCol.iType;
	iMaxLength = aSrcCol.iMaxLength;
	iAttributes = aSrcCol.iAttributes;
	iName = static_cast <const TDesC&> (aSrcCol.iName);
	return *this;
	}

/** Tests if a column is of the Long column type, i.e. one of the EDbColLongXxxx 
types.

@param aType The column type to test.
@return ETrue if the aType is a Long column type, EFalse otherwise. */
inline TBool TDbCol::IsLong(TDbColType aType)
	{
	return aType>=EDbColLongText8;
	}

// Class CDbColSet
/** Returns the number of column definitions in the set.

Note that using a TDbColSetIter is another way to iterate through the contents 
of a CDbColSet.

@return The number of columns in the columns set. */
inline TInt CDbColSet::Count() const
	{
	return iColumns.Count();
	}

/** Removes all of the columns from the column set. */
inline void CDbColSet::Clear()
	{
	iColumns.Reset();
	}

/** Returns a column definition by its ordinal number in the set.

Note that using a TDbColSetIter is another way to iterate through the contents 
of a CDbColSet.

@param aCol The ordinal number of the column in the set. Columns in a column 
set are numbered from 1 to Count(), unlike Symbian OS array classes. Ordinal 
0 is reserved to represent the invalid column number KDbNullColNo.
@return The column definition requested. */
inline const TDbCol& CDbColSet::operator[](TDbColNo aCol) const
	{
	return iColumns[aCol-1];// 1-based column ids 
	}

// Class TDbColSetIter
inline TDbColSetIter::operator TAny* () const
	{
	return CONST_CAST(TDbCol*,iColumn);
	}

/** Dereferences the iterator on the current column definition.

@return A const reference to the current column definition. */
inline const TDbCol& TDbColSetIter::operator*() const
	{
	__ASSERT_DEBUG(iIndex<iArray->Count(),User::Invariant());
	return *iColumn;
	}

/** Gets a member of the currently referenced column definition. This enables the 
use of the following constructs:

if (iter->iType==EDbColText && iter->iMaxLength<50) 

@return A const pointer to the current column definition. */
inline const TDbCol* TDbColSetIter::operator->() const
	{
	__ASSERT_DEBUG(iIndex<iArray->Count(),User::Invariant());
	return iColumn;
	}

/** Returns a column ordinal in the set for the currently referenced column definition.

@return The column ordinal of the current column definition. */
inline TDbColNo TDbColSetIter::Col() const
	{
	__ASSERT_DEBUG(iIndex<iArray->Count(),User::Invariant());
	return iIndex+1;
	}

/** Moves the iterator to the next column in the set post increment operator.

Note that this is implemented in terms of the pre-increment operator, and 
is less efficient.

@param Unused: required for the C++ compiler to resolve the ambiguity with 
the pre-increment operator.
@return A copy of this iterator, referring to the column definition before 
the increment operation is performed. */
inline TDbColSetIter TDbColSetIter::operator++(TInt)
	{
	TDbColSetIter tmp(*this);
	++(*this);
	return tmp;
	}

/**
TDbKeyCol copy constructor.
@param aSrcKeyCol This TDbKeyCol object will be constructed as an exact copy of aSrcKeyCol object.
*/
inline TDbKeyCol::TDbKeyCol(const TDbKeyCol& aSrcKeyCol) :
	iOrder(aSrcKeyCol.iOrder),
	iLength(aSrcKeyCol.iLength),
	iName(static_cast <const TDesC&> (aSrcKeyCol.iName))
	{
	}
	
/**
TDbKeyCol "=" operator.
@param aSrcKeyCol This TDbKeyCol object will be made to be an exact copy of aSrcKeyCol object.
*/
inline TDbKeyCol& TDbKeyCol::operator=(const TDbKeyCol& aSrcKeyCol)
	{
	iOrder = aSrcKeyCol.iOrder;
	iLength = aSrcKeyCol.iLength;
	iName = static_cast <const TDesC&> (aSrcKeyCol.iName);
	return *this;
	}


// Class CDbKey
inline TInt CDbKey::Count() const
	{
	return iKeys.Count();
	}

/** Returns a key column by its position in the key.

@param aCol The position of the column in the key. These are numbered from 
0 to Count()-1.
@return The key column requested. */
inline const TDbKeyCol& CDbKey::operator[](TInt aCol) const
	{
	return iKeys[aCol];
	}

/** Makes the key unique. This ensures that every key value in the index is distinct 
from every other. */
inline void CDbKey::MakeUnique()
	{
	iAttributes|=EUnique;
	}

/** Tests whether the key is unique.

@return ETrue, if the key is unique; EFalse, otherwise. */
inline TBool CDbKey::IsUnique() const
	{
	return iAttributes&EUnique;
	}

/** Tests whether the key is the primary key.

@return ETrue, if the key is unique; EFalse, otherwise. */
inline TBool CDbKey::IsPrimary() const
	{
	return iAttributes&EPrimary;
	}

/** Sets the way in which Text columns are compared for the key. All Text columns 
in the key are compared in the same way.

@param aComparison The comparison type to use. */
inline void CDbKey::SetComparison(TDbTextComparison aComparison)
	{
	iComparison=aComparison;
	}

/** Returns the method used to compare Text columns in this key.

@return The comparison type used for the key. */
inline TDbTextComparison CDbKey::Comparison() const
	{
	return iComparison;
	}

inline void CDbKey::MakePrimary()
	{
	iAttributes|=EPrimary;
	}

// Class TDbQuery
/** Constructs a query object from an SQL string and a text comparison mode.

Note that no copy is made of the descriptor passed; it is stored by reference 
in the query object.

@param aQuery The SQL string as a descriptor.
@param aComparison The type of text comparison to use in evaluation of the 
SQL. If not supplied, normal comparison is used. */
inline TDbQuery::TDbQuery(const TDesC& aQuery,TDbTextComparison aComparison):
	iQuery(aQuery), 
	iComparison(aComparison)
	{
	}

/** Returns the SQL string in the query object.

@return A descriptor containing the SQL string. */
inline const TDesC& TDbQuery::Query() const
	{
	return iQuery;
	}

/** Returns the text comparison mode for the query object.

@return The text comparison mode. */
inline TDbTextComparison TDbQuery::Comparison() const
	{
	return iComparison;
	}

// Class RDbHandleBase
inline RDbHandleBase::RDbHandleBase():
	iObject(0)
	{
	}

// Class RDbRowSet

/** Positions the cursor at the beginning of the rowset. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline void RDbRowSet::BeginningL()
	{
	GotoL(EBeginning);
	}

/** Positions the cursor at the end of the rowset. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline void RDbRowSet::EndL()
	{
	GotoL(EEnd);
	}

/** Positions the cursor on the first row in the rowset. If there are no rows, 
the cursor is positioned at the end.

@return ETrue if the cursor is now at a row, EFalse if it is at the end. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline TBool RDbRowSet::FirstL()
	{
	return GotoL(EFirst);
	}

/** Positions the cursor on the last row in the rowset. If there are no rows, the 
cursor is positioned at the beginning.

@return ETrue if the cursor is now at a row, EFalse if it is at the beginning. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline TBool RDbRowSet::LastL()
	{
	return GotoL(ELast);
	}

/** Moves the cursor to the next row in the rowset. If there are no more rows, 
the cursor is positioned to the end.

If the cursor is at the beginning prior to the function, it is equivalent 
to FirstL().

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline TBool RDbRowSet::NextL()
	{
	return GotoL(ENext);
	}

/** Moves the cursor to the previous row in the rowset. If there are no more rows, 
the cursor is positioned to the beginning.

If the cursor is at the end prior to the function, it is equivalent to LastL().

@return ETrue if the cursor is now at a row, EFalse if it is at the beginning. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline TBool RDbRowSet::PreviousL()
	{
	return GotoL(EPrevious);
	}

/** Tests whether a column has the NULL value.

Columns which have the NULL value can still be extracted with the correct 
accessor function, in which case numerical columns will return a 0 (or equivalent) 
value, and text and binary columns will have a zero length.

@param aCol The column ordinal for the column to test.
@return ETrue if column aCol is NULL, otherwise EFalse. */
inline TBool RDbRowSet::IsColNull(TDbColNo aCol) const
	{
	return ColSize(aCol)==0;
	}

/** Extracts a signed integer column value. The type should fit within a TInt.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
inline TInt RDbRowSet::ColInt(TDbColNo aCol) const
	{
	return ColInt32(aCol);
	}

/** Extracts an unsigned integer column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
inline TUint RDbRowSet::ColUint(TDbColNo aCol) const
	{
	return ColUint32(aCol);
	}

/** Extracts a TReal64 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
inline TReal RDbRowSet::ColReal(TDbColNo aCol) const
	{
	return ColReal64(aCol);
	}

/** Sets a signed integer column value. The type should fit into a TInt.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
inline void RDbRowSet::SetColL(TDbColNo aCol,TInt aValue)
	{
	SetColL(aCol, TInt32(aValue));
	}

/** Sets a signed integer column value. The type should fit into a TInt.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
inline void RDbRowSet::SetColL(TDbColNo aCol,TUint aValue)
	{
	SetColL(aCol,TUint32(aValue));
	}

// Class TDbWindow
/** Constructs this object with a size of ENone. This can be used to request a 
view with no pre-evaluation window. */
inline TDbWindow::TDbWindow():
	iSize(ENone)
	{
	}

/** Constructs this object with a size of EUnlimited. This is used to request a 
completely pre-evaluated view. The constant KDbUnlimitedWindow is an instance 
of such a TDbWindow.

@param The argument is only used to direct the compiler to construct an unlimited 
window. */
inline TDbWindow::TDbWindow(TUnlimited):
	iSize(EUnlimited)
	{
	}

/** Returns the number of rows stored by the view.

@return The number of rows stored by the window. This could be one of the 
special values ENone or EUnlimited. */
inline TInt TDbWindow::Size() const
	{
	return iSize;
	}

/** Returns the preferred position in the window of the current row marker. i.e. 
the position with the forward and backward slots as requested.

@return The preferred position in the window. It is undefined if this is not 
a limited window. */
inline TInt TDbWindow::PreferredPos() const
	{
	return iPreferredPos;
	}

// Class TUnion
template <class T>
inline TUnion<T>::operator const T&() const
	{
	return *(const T*)&iRep[0];
	}

template <class T>
inline const T& TUnion<T>::operator()() const
	{
	return *(const T*)&iRep[0];
	}

template <class T>
inline T& TUnion<T>::operator()()
	{
	return *(T*)&iRep[0];
	}

template <class T>
inline void TUnion<T>::Set(const T& aT)
	{
	new(&iRep[0]) T(aT);
	}

// Class TDbLookupKey
inline TDbLookupKey::TDbLookupKey(): 
	iCount(0)
	{
	}

inline TInt TDbLookupKey::Count() const
	{
	return iCount;
	}

inline const TDbLookupKey::SColumn* TDbLookupKey::First() const
	{
	return &iKey[0];
	}

// Class TDbSeekKey
/** Constructs an empty key value. 

Add() should be called before the key value is used for lookup. */
inline TDbSeekKey::TDbSeekKey(): 
	iMaxKeys(1)
	{
	}

/** Constructs a key value for an TInt8, TInt16 or TInt32 column.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(TInt aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

/** Constructs a key value for a Bit, TUint8, TUint16 or TUint32 column.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(TUint aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

inline TDbSeekKey::TDbSeekKey(TInt64 aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

/** Constructs a key value for a TReal32 column.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(TReal32 aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

/** Construct a key value for a TReal64 column.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(TReal64 aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

/** Constructs a key value for a TDateTime column.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(TTime aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

/** Constructs a key value for a non-Unicode text column.

Note that the seek key does not copy the text data contained by the descriptor. 
This needs to be retained until the seek key is no longer required.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(const TDesC8& aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

/** Constructs a key value for a Unicode text column.

Note that the seek key does not copy the text data contained by the descriptor. 
This needs to be retained until the seek key is no longer required.

@param aKey The key value to lookup. */
inline TDbSeekKey::TDbSeekKey(const TDesC16& aKey): 
	iMaxKeys(1)
	{
	Add(aKey);
	}

inline TDbSeekKey::TDbSeekKey(TInt aKeys,TInt): 
	iMaxKeys(aKeys)
	{
	}

// Class TDbSeekMultiKey
/** Constructs an empty multi-column key value. */
template <TInt S>
inline TDbSeekMultiKey<S>::TDbSeekMultiKey(): 
	TDbSeekKey(S,0)
	{
	}

// Class RDbTable
/** Sets the specified index as the active index for this table. The rows will 
be presented in index order, and this index key will be used for lookup by 
the SeekL() function.

If successful, the rowset is reset to the beginning.

@param anIndex The name of the index to activate.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
Specifically:KErrWrite if the table was created with insert-only access.KErrNotFound 
if the index does not exist on the table. This can also be one of the DBMS 
database error codes. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline TInt RDbTable::SetIndex(const TDesC& anIndex)
	{
	return SetIndex(&anIndex);
	}

/** Sets the ordering to be the underlying ordering of the rows — this will 
usually provide faster navigation of the rowset.

@return KErrNone, if successful, otherwise one of the system-wide error codes. 
Specifically:KErrWrite if the table was created with insert-only access. This 
can also be one of the DBMS database error codes. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
inline TInt RDbTable::SetNoIndex()
	{
	return SetIndex(0);
	}

/** Constructs this object by invoking the matching constructor for RWriteStream.

@param anExternalizer Specifies an externaliser */
inline RDbColWriteStream::RDbColWriteStream(const MExternalizer<TStreamRef> &anExternalizer): 
	RWriteStream(anExternalizer)
	{
	}

// Class CDbNames
inline TInt CDbNames::Count() const
	{
	return iList.Count();
	}

inline const TDesC& CDbNames::operator[](TInt anIndex) const
	{
	return iList[anIndex];
	}

// Class RDbDatabase

/**
Creates a table on the database.

@param aName Table name.
@param aColSet A set of column definitions which describe the table structure.

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrAlreadyExists, a table with that name already exists;
        KErrArgument, empty column set, duplicated column name, invalid column length;
        KErrBadName, invalid table name, invalid column name (containing spaces for example);
        KErrNotSupported, unknown column type, unknown column attributes;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema
            access policy for the database.
*/
inline TInt RDbDatabase::CreateTable(const TDesC& aName,const CDbColSet& aColSet)
	{
	return CreateTable(aName,aColSet,NULL);
	}

/**
Creates a table on the database.

@param aName Table name.
@param aColSet A set of column definitions which describe the table structure.
@param aPrimaryKey Primary key definition.

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrAlreadyExists, a table with that name already exists;
        KErrArgument, empty column set, duplicated column name, invalid column length;
        KErrBadName, invalid table name, invalid column name (containing spaces for example);
        KErrNotSupported, unknown column type, unknown column attributes;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema
            access policy for the database.
*/
inline TInt RDbDatabase::CreateTable(const TDesC& aName,const CDbColSet& aColSet,const CDbKey& aPrimaryKey)
	{
	return CreateTable(aName,aColSet,&aPrimaryKey);
	}

// Class RDbIncremental
/** Initiates the execution of a DDL (SQL schema update) statement on the database.

This is the incremental form of RDbDatabase::Execute().

Note that to begin executing a DML (SQL data update) statement incrementally, 
use the RDbUpdate class.

@param aDatabase The database on which the DDL (SQL schema update) statement 
is to execute.
@param aSql The DDL SQL statement to be executed on the database.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to Next() to 
continue the operation.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see RDbDatabase::Execute()
@see RDbUpdate 

@capability Note For a secure shared database, the caller must satisfy:
            - the schema access policy for the database, if the SQL statement is 
			  CREATE/DROP/ALTER; 
            - the write access policy for the table in the SQL, if the SQL statement is 
			  INSERT/UPDATE/DELETE; 
*/
inline TInt RDbIncremental::Execute(RDbDatabase& aDatabase,const TDesC& aSql,TInt& aStep)
	{
	return Execute(aDatabase,aSql,EDbCompareNormal,aStep);
	}

////////////////////////////////////////////////////////////////////////////////////////////
// CDbStrings class

/**
@return The number of elements of the controlled strings array.
*/
inline TInt CDbStrings::Count() const
	{
	return iList.Count();
	}

/**
Allows access to "aIndex" element of the controlled strings array.
@return "aIndex" element of the controlled strings array.
*/
inline const TDesC& CDbStrings::operator[](TInt aIndex) const
	{
	return iList[aIndex];
	}

