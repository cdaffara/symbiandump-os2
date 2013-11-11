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

#include "UD_STD.H"

// Class RDbRowSet

/** Resets the rowset.

For a Table, this just sets the rowset cursor to the beginning position.

For an SQL view, this discards any evaluated rows and returns the cursor to 
the beginning. The view then requires reevaluation.

The Store Database implementation requires this function to be called in order 
to recover an open rowset object after the database has been rolled back. 
The rowset does not need to be closed in this situation.

If a rowset object requires a reset, then all row functions return or leave 
with KErrNotReady. */
EXPORT_C void RDbRowSet::Reset()
	{
	iCursor->Reset();
	}

/** Closes the rowset and releases any owned resources. It is safe to close a rowset 
object which is not open. */
EXPORT_C void RDbRowSet::Close()
	{
	iCursor.Close();
	}

/** Returns the number of rows available in a rowset.
	
This can take some time to complete, and a parameter can be passed to request 
a quick but not always thorough count of the rows.

For SQL views, the value returned depends on the evaluation window being used 
by the view. If there is an evaluation window this function will always return 
the number of rows available in the window, not the total number which could 
be returned by the query.

@param anAccuracy This specifies whether to ensure that an accurate count 
is returned, or to give up if this value is not readily available. The default 
is to ensure an accurate count.
@return The number of rows available in the rowset, or KDbUndefinedCount if 
EQuick was specified and the count was not available. 

@capability Note For a secure shared database, the caller must satisfy either the read
            or the write access policy for the table.
*/
EXPORT_C TInt RDbRowSet::CountL(TAccuracy aAccuracy) const
	{
	return iCursor->CountL(aAccuracy);
	}

/** Tests whether there are any rows in the rowset. This is often faster than testing 
whether CountL()==0.

@return ETrue if there are no rows available in the rowset, EFalse if there 
are one or more. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TBool RDbRowSet::IsEmptyL() const
	{
	if (AtRow())
		return EFalse;
	TInt count=CountL(EQuick);
	if (count!=KDbUndefinedCount)
		return count==0;
	TDbBookmark mark=Bookmark();
	CDbCursor& cursor=*iCursor;
	TBool hasRow=cursor.GotoL(EFirst);
	cursor.GotoL(mark.iMark);
	return !hasRow;
	}

/** Returns the entire column set for the rowset. This can be used to discover 
column ordinals for named columns in this rowset.

The function leaves with KErrNoMemory if there is not enough memory to carry 
out the operation. 

@return The column set object which describes this rowset structure. The caller 
should delete it when it is no longer required. */
EXPORT_C CDbColSet* RDbRowSet::ColSetL() const
	{
	CDbColSet* cs=CDbColSet::NewLC();
	iCursor->ColumnsL(*cs);
	CleanupStack::Pop();
	return cs;
	}

/** Returns the number of columns which are defined in this rowset.

@return The number of columns which are defined in this rowset. */
EXPORT_C TInt RDbRowSet::ColCount() const
	{
	return iCursor->ColumnCount();
	}

/** Returns the type of a column in the rowset.
	
@param aCol The column ordinal for which the column type is required. Column 
ordinals range from 1 to ColCount().
@return The type of the column aCol. */
EXPORT_C TDbColType RDbRowSet::ColType(TDbColNo aColNo) const
	{
	return iCursor->ColumnType(aColNo);
	}

/** Returns the definition of a column in the rowset.
	
@param aCol The column ordinal for which the column definition is required. 
Column ordinals range from 1 to ColCount().
@return The definition of the column aCol. */
EXPORT_C TDbCol RDbRowSet::ColDef(TDbColNo aColNo) const
	{
	TDbCol col;
	iCursor->ColumnDef(col,aColNo);
	return col;
	}

/** Tests whether the cursor is on a row.

One of the following is true:

the rowset is currently updating a row

the rowset is currently inserting a row

GetL() can be called to retrieve the row

@return ETrue if the cursor is on a valid row; EFalse, otherwise. */
EXPORT_C TBool RDbRowSet::AtRow() const
	{
	return iCursor->AtRow();
	}

/** Tests whether the cursor is at the beginning of the rowset.

@return ETrue if the cursor is at the beginning, otherwise EFalse. */
EXPORT_C TBool RDbRowSet::AtBeginning() const
	{
	return iCursor->AtBeginning();
	}

/** Tests whether the cursor is at the end of the rowset.

@return ETrue if the cursor is at the end, otherwise EFalse. */
EXPORT_C TBool RDbRowSet::AtEnd() const
	{
	return iCursor->AtEnd();
	}

/** Moves the cursor to a specified position.

This is invoked by Beginning(), End(), FirstL(), LastL(), NextL() and PreviousL() 
to navigate the cursor. See those functions for descriptions of how the cursor 
behaves given different position specifications.

@param aPosition Specifies the position to move the cursor to.
@return ETrue if the cursor is now at a row, EFalse if it is at the beginning 
or end. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TBool RDbRowSet::GotoL(TPosition aPosition)
	{
	return iCursor->GotoL(aPosition);
	}

/** Gets the bookmark for the current cursor position. Bookmarks cannot be extracted 
when the rowset is updating or inserting a row.

The Store Database implementation allows bookmarks to be extracted for any 
cursor position including the beginning and end.

@return A bookmark which can be used to return to the current position using 
the GotoL() function. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TDbBookmark RDbRowSet::Bookmark() const
	{
	TDbBookmark mark;
	iCursor->Bookmark(mark.iMark);
	return mark;
	}

/** Goes to a previously bookmarked position in a rowset.

The Store Database implements bookmarks which are valid in any rowset based 
on the same table or generated by the same query, and which persist across 
transaction boundaries.

@param aMark The bookmark to return to. This should have been returned by 
a previous call to Bookmark() on this or an equivalent rowset object. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C void RDbRowSet::GotoL(const TDbBookmark& aMark)
	{
	iCursor->GotoL(aMark.iMark);
	}

/** Gets the current row data for access using the column extractor functions. 
The cursor must be positioned at a valid row. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C void RDbRowSet::GetL()
	{
	iCursor->GetL();
	}

/** Inserts a new row into the rowset. All auto-increment columns will be initialised 
with their new values, all other columns will be initialised to NULL values. 
If no client-begun transaction is in progress, this function begins an automatic 
transaction, which is committed by PutL().

After the column values have been set using the SetColL() functions, the row 
can be written to the database using PutL(). 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbRowSet::InsertL()
	{
	iCursor->InsertL(CDbCursor::EClear);
	}

/** Inserts a copy of the current row into the rowset. All auto-increment columns 
will be given a new value (as for InsertL()), the other columns will copy 
their values from the cursor's current row. If no client-begun transaction 
is in progress, this function begins an automatic transaction, which is committed 
by PutL().

After the column values have been modified using the SetColL() functions, 
the row can be written to the database using PutL(). 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbRowSet::InsertCopyL()
	{
	iCursor->InsertL(CDbCursor::ECopy);
	}

/** Prepares the current row for update. If no client-begun transaction is in progress, 
this function begins an automatic transaction, which is committed by PutL().

After the column values have been modified using the SetColL() functions, 
the row can be written back to the database using PutL(). 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbRowSet::UpdateL()
	{
	iCursor->UpdateL();
	}

/** Completes the update or insertion of a row.

First the new row data is validated:

not-null columns are checked to be not NULL

numerical columns are checked to be in range for their type

variable length columns are checked to not exceed their maximum length

unique index keys are checked to ensure uniqueness is not violated

Note that modifying auto-increment columns is not prevented by DBMS.

Following validation the data is written to the database and any affected 
indexes are updated. On successful completion of the write, PutL() will then 
commit any automatic transaction.

The cursor is left positioned on the updated or inserted row — where this 
lies in the rowset is not always well defined. To return to the row which 
was current prior to the update or insertion, a bookmark can be used.

In the Store Database implementation the written row is located in the rowset 
as follows:

Tables without an active index will leave updated rows in the same location 
and append new rows to the end of the rowset.

Tables with an active index place the row according to the active index ordering.

SQL views without an evaluation window will place it according to the rowset 
ordering. The row may subsequently disappear if it does not match the WHERE 
clause of the SQL query.

SQL views with a full evaluation window will leave updated rows in the same 
location and append new rows to the end of the rowset. Re-evaluation may cause 
the row to disappear if it does not match the WHERE clause of the SQL query.

SQL views with a partial evaluation window will leave updated rows in the 
same location, new rows are not added to the window and navigation from the 
new row is undefined. Further navigation and evaluation of the partial window 
will place the rows in the correct location according to the query. 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbRowSet::PutL()
	{
	iCursor->PutL();
	}

/** Deletes the current row in a rowset. The rowset must not currently be updating 
or inserting the row.

The rowset cursor is left positioned at the "hole" left by the deletion of 
the current row. Navigation to the next or previous row will have the same 
effect as if this row had not been deleted. Once the cursor has been moved 
from the "hole" it will disappear from the rowset.

If the client has not begun a transaction, this function will use an automatic 
transaction to update the rowset. 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbRowSet::DeleteL()
	{
	iCursor->DeleteL();
	}

/** Cancels the update or insertion of a row, or recovers the rowset if PutL() 
fails. The cursor will return to the location prior to starting the update 
or insertion. It is also safe to call this function when the rowset object 
is not updating or inserting a row, in which case it does nothing.

In the Store database implementation, if this is called to abort a row update 
or insertion before PutL() is called or during row validation in PutL(), all 
the changes are discarded without requiring a transaction rollback and the 
rowset to be Reset(). */
EXPORT_C void RDbRowSet::Cancel()
	{
	iCursor->Cancel();
	}

//
// Checks for valid Cursor and column ID
//
CDbCursor& RDbRowSet::CheckCol(TDbColNo aCol) const
	{
	CDbCursor& cr=*iCursor;
	__ASSERT_ALWAYS(aCol>0&&aCol<=cr.ColumnCount(),Panic(EDbInvalidColumn));
	return cr;
	}

//
// Checks for valid Cursor, column ID and type
//
TDbColumnC RDbRowSet::ColumnC(TDbColNo aCol,TDbColType aType) const
	{
	CDbCursor& cr=*iCursor;
	TDbColType cType=cr.ColumnType(aCol);
	if (cType!=aType)
		{	// not an exact match
		if (cType>aType)
			Panic(EDbWrongType);		// extraction type is narrower
		else if (!IsIntegral(cType))
			Panic(EDbWrongType);		// type is non-integral
		else if (IsSigned(cType) && IsUnsigned(aType))
			Panic(EDbWrongType);		// cannot get signed column as unsigned
		}
	return cr.ColumnC(aCol);
	}

TDbColumn RDbRowSet::Column(TDbColNo aCol,TDbColType aType)
	{
	CDbCursor& cr=*iCursor;
	__ASSERT_ALWAYS(cr.ColumnType(aCol)==aType,Panic(EDbWrongType));
	return cr.Column(aCol);
	}

/** Gets the size in bytes of a column value. This can be used for all column types, 
including Long columns. NULL columns return a size of 0.

Note:

This may yield unexpected results for small numerical column types as they 
are stored in memory as 32-bit values.

@param aCol The column ordinal of the column to check.
@return The length in bytes of column aCol's value. */
EXPORT_C TInt RDbRowSet::ColSize(TDbColNo aCol) const
	{
	return iCursor->ColumnSize(aCol);
	}

/** Gets the length of a column value. As compared with ColSize(), this returns 
the number of "units" in the column:

NULL columns have a length of 0

non-NULL numerical and date-time columns have a length of 1

for Text columns the length is the character count

for Binary columns the length is the byte count

@param aCol The column ordinal of the column to check.
@return The length in "units" of column aCol's value. */
EXPORT_C TInt RDbRowSet::ColLength(TDbColNo aCol) const
	{
	TInt size=ColSize(aCol);
	switch (iCursor()->ColumnType(aCol))
		{
	case EDbColText8:
	case EDbColLongText8:
	case EDbColBinary:
	case EDbColLongBinary:
		break;
	case EDbColText16:
	case EDbColLongText16:
		if (size>0)
			size>>=1;
		break;
	default:
		if (size)
			size=1;
		break;
		}
	return size;
	}

/** Extracts a TInt8 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TInt8 RDbRowSet::ColInt8(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColInt8).Int8();
	} 

/** Extracts a TInt16 or TInt8 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TInt16 RDbRowSet::ColInt16(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColInt16).Int16();
	} 

/** Extracts a TInt32, TInt16 or TInt8 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TInt32 RDbRowSet::ColInt32(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColInt32).Int32();
	} 

/** Extracts a TInt64 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TInt64 RDbRowSet::ColInt64(TDbColNo aCol) const
	{
	CDbCursor& cr=*iCursor;
	TDbColumnC c=cr.ColumnC(aCol);
	TDbColType t=cr.ColumnType(aCol);
	if (t==EDbColInt64)
		return c.Int64();
	if (t>EDbColInt64)
		Panic(EDbWrongType);
	if (IsSigned(t))
		return TInt(c.Int32());
	return TUint(c.Uint32());
	} 

/** Extracts a Uint8 or Bit column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TUint8 RDbRowSet::ColUint8(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColUint8).Uint8();
	} 

/** Extracts a Uint16, Uint8 or Bit column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TUint16 RDbRowSet::ColUint16(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColUint16).Uint16();
	} 

/** Extracts a Uint32, Uint16, Uint8 or Bit column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TUint32 RDbRowSet::ColUint32(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColUint32).Uint32();
	} 

/** Extracts a TReal32 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TReal32 RDbRowSet::ColReal32(TDbColNo aCol) const __SOFTFP
	{
	return ColumnC(aCol,EDbColReal32).Real32();
	} 

/** Extracts a TReal64 column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. */
EXPORT_C TReal64 RDbRowSet::ColReal64(TDbColNo aCol) const __SOFTFP
	{
	return ColumnC(aCol,EDbColReal64).Real64();
	} 

/** Extracts a TTime column value.

@param aCol The column ordinal of the column to extract.
@return The value of column aCol. TTime may be either local time or universal time. 
DBMS doesn't interpret the value of TTime, it is left up to the user to know which has been used.*/
EXPORT_C TTime RDbRowSet::ColTime(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColDateTime).Time();
	} 

/** Extracts any column type, except Long columns, as binary data.
Can handle any type of non-long column

@param aCol The column ordinal of the column to extract.
@return A descriptor of column aCol's value. */
EXPORT_C TPtrC8 RDbRowSet::ColDes8(TDbColNo aCol) const
	{
	CDbCursor& cr=*iCursor;
	__ASSERT_ALWAYS(!IsLong(cr.ColumnType(aCol)),Panic(EDbWrongType));
	return cr.ColumnC(aCol).PtrC8();
	} 

/** Extracts a column as Unicode text.

@param aCol The column ordinal of the column to extract
@return A descriptor of column aCol's value. */
EXPORT_C TPtrC16 RDbRowSet::ColDes16(TDbColNo aCol) const
	{
	return ColumnC(aCol,EDbColText16).PtrC16();
	}

/** Extracts a Text column value. The column type must match the default descriptor 
type to use this extractor, ie. it must be equal to EDbColText.

@param aCol The column ordinal of the column to extract.
@return A descriptor of column aCol's value. */
EXPORT_C TPtrC RDbRowSet::ColDes(TDbColNo aCol) const
	{
	return ColDes16(aCol);
	}

/** Use this function to set the value of a column to NULL.

@param aCol The column ordinal of the column to set to NULL. 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbRowSet::SetColNullL(TDbColNo aCol)
	{
	iCursor->SetNullL(aCol);
	}

/** Sets a TInt32, TInt16 or TInt8 column value.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,TInt32 aValue)
	{
	CDbCursor& cr=*iCursor;
	TDbColType t=cr.ColumnType(aCol);
	if (t<EDbColInt64)
		{	// set any <= 32-bit integral type
		if (IsSigned(t))
			{
			cr.Column(aCol).SetL(aValue);
			return;
			}
		if (aValue>=0)
			{	// check the domain for unsigned columns
			cr.Column(aCol).SetL(aValue);
			return;
			}
		}
	Panic(EDbWrongType);
	}

/** Sets a TInt64 column value.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,TInt64 aValue)
	{
	CDbCursor& cr=*iCursor;
	TDbColumn c=cr.Column(aCol);
	TDbColType t=cr.ColumnType(aCol);
	if (t==EDbColInt64)
		{		// exact match
		c.SetL(aValue);
		return;
		}
	if (t<EDbColInt64)
		{
		TInt32 l = I64LOW(aValue);
		TInt32 h = I64HIGH(aValue);
		if (IsSigned(t))
			{	// check the domain for signed 32-bit 
			if (h==l>>31)		// sign-extend l gives aValue
				{
				c.SetL(l);
				return;
				}
			}	// invalid type, drop through to panic
		else
			{	// check the domain for unsigned 32-bit 
			if (h==0)
				{				// zero extend l gives aValue
				c.SetL(l);	// in unsigned 32 bit range
				return;
				}
			}	// invalid type, drop through to panic
		}
	Panic(EDbWrongType);
	}

/** Sets a TUint32, TUint16, TUint8 or Bit column value.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,TUint32 aValue)
	{
	CDbCursor& cr=*iCursor;
	TDbColType t=cr.ColumnType(aCol);
	if (t<EDbColInt64)
		{	// set any <= 32-bit integral type
		if (IsUnsigned(t))
			{
			cr.Column(aCol).SetL(aValue);
			return;
			}
		if (aValue<=TUint32(KMaxTInt))
			{	// check the domain for signed columns
			cr.Column(aCol).SetL(aValue);
			return;
			}
		}
	Panic(EDbWrongType);
	}

/** Sets a TReal32 column value.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,TReal32 aValue) __SOFTFP
	{
	Column(aCol,EDbColReal32).SetL(aValue);
	}

/** Sets a TReal64 column value.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,TReal64 aValue) __SOFTFP
	{
	Column(aCol,EDbColReal64).SetL(aValue);
	}

/** Sets a TTime column value.

TTime could be either local time or universal time. 
DBMS doesn't interpret the value of TTime, it is left up to the user to decide which should be used.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,TTime aValue)
	{
	Column(aCol,EDbColDateTime).SetL(aValue);
	}

/** Sets a column value from an 8 bit descriptor. This function can also set the 
value of Long columns.

Usually this is used to set a Text8 or LongText8 column from a non-Unicode 
text descriptor, but can be used for any column type: the data content is 
validated when the row is PutL().

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,const TDesC8 &aValue)
	{
	CDbCursor& c=*iCursor;
	if (IsLong(c.ColumnType(aCol)))
		{
		RDbColWriteStream strm;
		strm.OpenLC(*this,aCol);
		strm.WriteL(aValue);
		strm.CommitL();
		CleanupStack::PopAndDestroy();
		}
	else
		c.Column(aCol).SetL(aValue);
	}

/** Set a column value from Unicode text.

@param aCol The column ordinal of the column to set.
@param aValue The new column value. */
EXPORT_C void RDbRowSet::SetColL(TDbColNo aCol,const TDesC16 &aValue)
	{
	CDbCursor& c=*iCursor;
	if (c.ColumnType(aCol)==EDbColLongText16)
		{
		RDbColWriteStream strm;
		strm.OpenLC(*this,aCol);
		strm.WriteL(aValue);
		strm.CommitL();
		CleanupStack::PopAndDestroy();
		}
	else
		Column(aCol,EDbColText16).SetL(aValue);
	}

/** Searches through a rowset for a row which fulfils an SQL search-condition. 
The search begins from the current position (which must be a valid row) and 
proceeds forwards or backwards through the available rows until it finds a 
match or runs out of rows.

The cursor is positioned to the matching row (or beginning/end of set) on 
return.

This is a brute-force approach to finding a row using an index for key-based 
retrieval on a Table rowset is a much faster but less flexible way of finding 
rows.

@param aDirection Specifies which direction to search after testing the current 
row.
@param aCriteria A query object containing an SQL search-condition to match 
against.
@return If no match is found KErrNotFound is returned. Otherwise the number 
of rows navigated while finding a match, which may be 0 if the current row 
matches. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TInt RDbRowSet::FindL(TDirection aDirection,TDbQuery aCriteria)
	{
	return iCursor->FindL(aDirection,aCriteria);
	}

/** Tests whether the current row in the rowset matches a previously compiled row 
constraint. The rowset must not currently be updating or inserting a row.

@param aConstraint A row constraint object which must have been previously 
opened on this rowset object.
@return ETrue if the current row fulfils the constraint, otherwise EFalse. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TBool RDbRowSet::MatchL(const RDbRowConstraint& aConstraint)
	{
	return iCursor->MatchL(*aConstraint.iConstraint);
	}

// Class RDbRowConstraint

/** Compiles the specified SQL search-condition for matching against rows in the 
specified rowset. The text comparison supplied in aCriteria is used for all 
text columns in the constraint.

@param aView The rowset to which the constraint will apply.
@param aCriteria The SQL string and the text comparison mode for the constraint.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
Specifically:KErrNotFound if a column name in the SQL query does not exist.KErrArgument 
if Invalid or unrecognised SQL syntax was used.KErrGeneral for a column type 
mismatch in a predicate in the SQL query or if a date-literal in the SQL query 
was invalid.KErrOverflow if a number-literal in the SQL query for an integral 
column was too large (did not fit in a 32-bit integral representation). This 
can also be one of the DBMS database error codes. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TInt RDbRowConstraint::Open(const RDbRowSet& aView,TDbQuery aCriteria)
	{
	TRAPD(r,iConstraint=aView.iCursor->ConstraintL(aCriteria));
	return r;
	}

/** Releases the resources used by the constraint before discarding the constraint 
object. */
EXPORT_C void RDbRowConstraint::Close()
	{
	iConstraint.Close();
	}

// Class RDbColReadStream

/** Opens the column with the specified ordinal in the specified current row in 
the rowset. The row must have previously been read into the rowset using RDbRowSet::GetL().

@param aView The rowset which has the row and column to be read.
@param aCol The column ordinal of the column to be read 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C void RDbColReadStream::OpenL(const RDbRowSet& aView,TDbColNo aCol)
	{
	Attach(aView.ColSourceL(aCol));
	}

/** Opens the column with the specified ordinal in the specified current row in 
the rowset and puts a pointer to the column on the cleanup stack.

The row must have previously been read into the rowset using RDbRowSet::GetL().

@param aView The rowset which has the row and column to be read.
@param aCol The column ordinal of the column to be read. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C void RDbColReadStream::OpenLC(const RDbRowSet& aView,TDbColNo aCol)
	{
	OpenL(aView,aCol);
	PushL();
	}

// Class RDbColWriteStream

/** Opens the column with the specified ordinal in the current row, and in the 
specified rowset, and prepares the column for being written or replaced. The 
rowset must be updating or inserting a row.

@param aView The rowset which has the row and column to be written.
@param aCol The column ordinal of the column to be written. 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbColWriteStream::OpenL(RDbRowSet& aView,TDbColNo aCol)
	{
	Attach(aView.ColSinkL(aCol));
	}

/** Opens the column with the specified ordinal in the current row, and in the 
specified rowset, and prepares the column for being written or replaced, putting 
a cleanup item for this object onto the cleanup stack. The rowset must be 
updating or inserting a row. 

Placing the cleanup object on the cleanup stack allows allocated resources 
to be cleaned up if a subsequent leave occurs.

@param aView The rowset which has the row and column to be written.
@param aCol The column ordinal of the column to be written. 

@capability Note For a secure shared database, the caller must satisfy the write
            access policy for the table.
*/
EXPORT_C void RDbColWriteStream::OpenLC(RDbRowSet& aView,TDbColNo aCol)
	{
	OpenL(aView,aCol);
	PushL();
	}

// Class TDbWindow

/** Constructs this object with the preferred shape. When fully evaluated, the 
view will try to have aForeSlots rows immediately available for navigation 
forwards, and aRearSlots rows immediately available for navigation backwards.

@param aForeSlots The number of rows to evaluate ahead of the current row.
@param aRearSlots The number of rows to evaluate behind the current row. */
EXPORT_C TDbWindow::TDbWindow(TInt aForeSlots,TInt aRearSlots)
	: iSize(aForeSlots+aRearSlots+1), iPreferredPos(aRearSlots)
	{
	__ASSERT_ALWAYS(aForeSlots>=0 && aRearSlots>=0,Panic(EDbInvalidViewWindowParameters));
	}

// Class RDbView

/** Prepares the view object for evaluating an SQL select-statement.

Following preparation, the rowset object can always provide schema information, 
but the view may first require evaluation to generate the rowset for navigation.

@param aDatabase The database on which to execute the query.
@param aQuery The SQL query and the text comparison mode for the constraint.
@param anAccess The access specification for the rowset. By default, updatable 
access is given.
@return KErrNone, if successful, otherwise one of the other system-wide error 
codes. Specifically: KErrNotFound if The table does not exist in the database 
or a column name in the SQL query does not exist.KErrNotSupported if a sort-specification 
in the SQL query cannot be provided by an index.KErrArgument if an invalid 
or unrecognised SQL syntax was used.KErrGeneral if there is a column type 
mismatch in a predicate in the SQL query or if a date-literal in the SQL query 
was invalid.KErrOverflow if a number-literal in the SQL query for an integral 
column was too large (did not fit in a 32-bit integral representation). This 
can also be one of the DBMS database error codes.. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TInt RDbView::Prepare(RDbDatabase& aDatabase,const TDbQuery& aQuery,TAccess anAccess)
	{
	return Prepare(aDatabase,aQuery,TDbWindow(),anAccess);
	}

/** Prepares the view object for evaluating an SQL select-statement and specifies 
the evaluation window shape for the rowset.

The function does not specify the access specification for the rowset
updatable access is given.

Following preparation, the rowset object can always provide schema information, 
but the view may first require evaluation to generate the rowset for navigation.

@param aDatabase The database on which to execute the query.
@param aQuery The SQL query and the text comparison mode for the constraint.
@param aWindow The desired evaluation window shape for the rowset. If this 
parameter is omitted, an alternative overload is called e.g. no pre-evaluation 
window is requested.
@return KErrNone, if successful, otherwise one of the other system-wide error 
codes. Specifically: KErrNotFound if The table does not exist in the database 
or a column name in the SQL query does not exist.KErrNotSupported if a sort-specification 
in the SQL query cannot be provided by an index.KErrArgument if an invalid 
or unrecognised SQL syntax was used.KErrGeneral if there is a column type 
mismatch in a predicate in the SQL query or if a date-literal in the SQL query 
was invalid.KErrOverflow if a number-literal in the SQL query for an integral 
column was too large (did not fit in a 32-bit integral representation). This 
can also be one of the DBMS database error codes. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TInt RDbView::Prepare(RDbDatabase& aDatabase,const TDbQuery& aQuery,const TDbWindow& aWindow)
	{
	return Prepare(aDatabase,aQuery,aWindow,EUpdatable);
	}

/** Prepares the view object for evaluating an SQL select-statement, specifies 
the evaluation window shape for the rowset, and sets the access specification 
for the rowset.

Following preparation, the rowset object can always provide schema information, 
but the view may first require evaluation to generate the rowset for navigation.

@param aDatabase The database on which to execute the query.
@param aQuery The SQL query and the text comparison mode for the constraint.
@param aWindow The desired evaluation window shape for the rowset. If this 
parameter is omitted, an alternative overload is called e.g. no pre-evaluation 
window is requested.
@param anAccess The access specification for the rowset. If omitted, updatable 
access is given.
@return KErrNone, if successful, otherwise one of the other system-wide error 
codes. Specifically:KErrNotFound if The table does not exist in the database 
or a column name in the SQL query does not exist.KErrNotSupported if a sort-specification 
in the SQL query cannot be provided by an index.KErrArgument if an invalid 
or unrecognised SQL syntax was used.KErrGeneral if there is a column type 
mismatch in a predicate in the SQL query or if a date-literal in the SQL query 
was invalid.KErrOverflow if a number-literal in the SQL query for an integral 
column was too large (did not fit in a 32-bit integral representation). This 
can also be one of the DBMS database error codes. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TInt RDbView::Prepare(RDbDatabase& aDatabase,const TDbQuery& aQuery,const TDbWindow& aWindow,TAccess anAccess)
	{
	TRAPD(r,iCursor=aDatabase.iDatabase->ViewL(aQuery,aWindow,anAccess));
	return r;
	}

/** Use this function to fully evaluate the view. It is equivalent to:

while (Unevaluated()) { Evaluate(); }

@return KErrNone, if successful, otherwise one of the system wide error codes. */
EXPORT_C TInt RDbView::EvaluateAll()
	{
	TInt r;
	do r=Evaluate(); while (r>0);
	return r;
	}

/** Performs a single step of the view evaluation, and returns when the step is 
complete. To completely evaluate a view in one go, EvaluateAll() should be 
used.

@return 0, evaluation is complete.> 0, more evaluation can be done. < 0, an 
error code, see the class overview for more information. */
EXPORT_C TInt RDbView::Evaluate()
	{
	return iCursor->Evaluate();
	}

///** Performs a single step of the view evaluation, returning immediately and signalling 
//when the step is complete.
//
//This function is most effectively used when the view evaluation is carried 
//out from an active object. 
//
//@param aStatus The request status used to contain completion information for 
//the function. On completion, the status value should be interpreted as follows: 
//0, evaluation is complete.> 0, more evaluation can be done. < 0, an error 
//code, see the class overview for more information. */
//EXPORT_C void RDbView::Evaluate(TRequestStatus& aStatus)
//	{
//	iCursor->Evaluate(aStatus);
//	}

/** Tests whether any more evaluation can be done to a view.

@return ETrue, if the view can be further evaluated; EFalse, if evaluation will 
have no effect. */
EXPORT_C TBool RDbView::Unevaluated() const
	{
	return iCursor->Unevaluated();
	}

// Class RDbTable

/** Opens the named table object on a database with the specified access.

If successful, the rowset is positioned to the beginning.

@param aDatabase The database on which to open the table.
@param aName The name of the table to open.
@param anAccess The access specification for the rowset, the default is updatable 
access.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
Specifically:KErrNotFound if the table does not exist in the database.KErrAccessDenied 
if an incremental operation is in progress. This can also be one of the DBMS 
database error codes. 

@capability Note For a secure shared database, the caller must satisfy either the read
            or the write access policy for the table.
*/
EXPORT_C TInt RDbTable::Open(RDbDatabase &aDatabase,const TDesC &aName,TAccess anAccess)
	{
	TRAPD(r,iCursor=aDatabase.iDatabase->TableL(aName,anAccess));
	return r;
	}

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
EXPORT_C TInt RDbTable::SetIndex(const TDesC* anIndex)
	{
	TRAPD(r,iCursor->SetIndexL(anIndex));
	return r;
	}

/** Finds a row in a table based on a key in the active index.

This function cannot be called while the rowset is currently updating or inserting 
a row. The currently active index on the table must have a key definition 
which matches the types in the key value.

Less columns can be added to the key than are present in the index definition: 
the keys will only be compared up to the columns present further columns 
in the index are not considered.

If successful the cursor is positioned to the row which was found, otherwise 
the cursor is left on an invalid row.

The underlying Store database can leave with KErrWrite, if the table was created 
with insert-only access.

The function can also leave with one of the DBMS database error codes.

@param aKey The key value to lookup in the index.
@param aComparison The comparison operation for the lookup, the default is 
to look for an exact match (EEqualTo).
@return ETrue if a row which compares correctly with the key exists, EFalse if 
not. 

@capability Note For a secure shared database, the caller must satisfy the read
            access policy for the table.
*/
EXPORT_C TBool RDbTable::SeekL(const TDbSeekKey& aKey,TComparison aComparison)
	{
	__ASSERT_ALWAYS(aKey.iKey.Count()>0,Panic(EDbNoColumnsInSeekKey));
	return iCursor->SeekL(aKey.iKey,aComparison);
	}

// Class CDbCursor

//
// Default implementation in terms of ColumnDef. Might be inefficient
//
EXPORT_C void CDbCursor::ColumnsL(CDbColSet& aColSet)
	{
	TDbCol col;
	TDbColNo max=ColumnCount();
	for (TDbColNo ii=0;++ii<=max;)
		{
		ColumnDef(col,ii);
		aColSet.AddL(col);
		}
	}

//
// Default implementation in terms of navigation
// Faster counting may be possible
//
EXPORT_C TInt CDbCursor::CountL(RDbRowSet::TAccuracy)
	{
	TDbBookmark::TMark mark;
	Bookmark(mark);
	GotoL(RDbRowSet::EBeginning);
	TInt count=0;
	while (GotoL(RDbRowSet::ENext))
		++count;
	GotoL(mark);
	return count;
	}

//
//  Default implementation in terms of constraints
//
EXPORT_C TInt CDbCursor::FindL(RDbRowSet::TDirection aDirection,const TDbQuery& aCriteria)
	{
	CDbRowConstraint* constraint=ConstraintL(aCriteria);
	constraint->PushL();
	RDbRowSet::TPosition next=aDirection==RDbRowSet::EForwards ? RDbRowSet::ENext : RDbRowSet::EPrevious;
	TInt skip=0;
	do
		{
		if (MatchL(*constraint))
			{
			CleanupStack::PopAndDestroy();	// constraint
			return skip;
			}
		++skip;
		} while (GotoL(next));
	CleanupStack::PopAndDestroy();
	return KErrNotFound;
	}

TInt CDbCursor::Evaluate()
	{
	TRAPD(r,r=EvaluateL());
	return r;
	}

CDbRowConstraint* CDbCursor::ConstraintL(const TDbQuery& aQuery)
	{
	return AttachContext(this,OpenConstraintL(aQuery));
	}

//
// Reserved for future development
//
EXPORT_C void CDbCursor::Reserved_1()
	{
	}

//
// Reserved for future development
//
EXPORT_C void CDbCursor::Reserved_2()
	{
	}
