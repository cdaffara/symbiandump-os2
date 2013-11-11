// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Written by DanH, Sept 1995
#if !defined(__BACELL_H__)
#define __BACELL_H__

#if !defined(__E32DEF_H_)
#include <e32def.h>
#endif

#if !defined(__S32STRM_H__)
#include <s32strm.h>
#endif

//class RReadStream;
//class RWriteStream;

class TCellRef
/** 
Identifies a cell by row and column number. 
@publishedAll
@released
*/
	{
public:
	inline TCellRef();
	inline TCellRef(TInt aRow,TInt aCol);

	inline TBool operator==(const TCellRef& aCell) const;
	inline TBool operator!=(const TCellRef& aCell) const;

	inline TCellRef& operator+=(const TCellRef& aCell);
	inline TCellRef& operator-=(const TCellRef& aCell);

	IMPORT_C void Offset(TInt aRowOffset,TInt aColOffset);
//
	IMPORT_C void InternalizeL(RReadStream& aStream);
	inline void ExternalizeL(RWriteStream& aStream) const;
public:
	/** The row number. */
	TInt iRow;
	/** The column number. */
	TInt iCol;
	};

/** 
@publishedAll
@released
*/
IMPORT_C TCellRef operator+(const TCellRef& aLeft,const TCellRef& aRight);
IMPORT_C TCellRef operator-(const TCellRef& aLeft,const TCellRef& aRight);

inline TCellRef::TCellRef()
	/** Default constructor.
	
	The object's row and column number are undefined. */
	{}
inline TCellRef::TCellRef(TInt aRow,TInt aCol)
    : iRow(aRow),iCol(aCol)
	/** Constructor taking a row and column number.
	
	@param aRow The row number.
	@param aCol The column number. */
	{}
inline TBool TCellRef::operator==(const TCellRef& aCell) const
	/** Compares this cell with the specified cell for equality.
	
	@param aCell The cell to be compared.
	@return ETrue, if the row numbers are the same and column numbers are 
	the same. */
	{return aCell.iRow==iRow && aCell.iCol==iCol;}
inline TBool TCellRef::operator!=(const TCellRef& aCell) const
	/** Compares this cell with the specified cell for inequality.
	
	@param aCell The cell to be compared.
	@return ETrue, if the row numbers are different or the column numbers are 
	different. */
	{return aCell.iRow!=iRow || aCell.iCol!=iCol;}
inline TCellRef& TCellRef::operator+=(const TCellRef& aCell)
	/** Adds the specified cell to this cell.
	
	Addition is the process of adding the specified cell's row and column 
	numbers to this cell's row and column numbers respectively.
	
	@param aCell The cell to be added.
	@return A reference to this cell object. */
	{Offset(aCell.iRow,aCell.iCol);return *this;}
inline TCellRef& TCellRef::operator-=(const TCellRef& aCell)
	/** Subtracts the specified cell from this cell.
	
	Subtraction is the process of subtracting the specified cell's row and 
	column numbers from this cell's row and column numbers respectively.
	
	@param aCell The cell to be subtracted.
	@return A reference to this cell object. */
	{Offset(-aCell.iRow,-aCell.iCol);return *this;}
inline void TCellRef::ExternalizeL(RWriteStream& aStream) const
	/** Externalises an object of this class to a write stream.
	
	The presence of this function means that the standard templated operator<<() 
	can be used to externalise objects of this class. 
	
	@param aStream Stream to which the object should be externalised. */
	{aStream.WriteInt32L(iRow);aStream.WriteInt32L(iCol);}


class TRangeRef
/** 
Identifies a range of cells by start and end cell references.
The range is inclusive of both the start and end cells.
@see TCellRef
@publishedAll
@released
*/
	{
public:
	inline TRangeRef();
	inline TRangeRef(const TCellRef& aFrom,const TCellRef& aTo);
	inline TRangeRef(TInt aFromRow,TInt aFromCol,TInt aToRow,TInt aToCol);

	IMPORT_C TBool operator==(const TRangeRef& aRange) const;
	inline TBool operator!=(const TRangeRef& aRange) const;

	inline void SetRange(const TCellRef& aFrom,const TCellRef& aTo);
	inline void SetRange(TInt aFromRow,TInt aFromCol,TInt aToRow,TInt aToCol);
	inline TInt NoRows() const;
	inline TInt NoCols() const;
	IMPORT_C TInt NoCells() const;
	IMPORT_C TBool Contains(const TCellRef& aCell) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
public:
	/** The start cell. */
	TCellRef	iFrom;
	/** The end cell. */
	TCellRef	iTo;
public:
	class TIter;	// defined outside TRangeRef class as contains TRangeRef
	};
class TRangeRef::TIter
/**
@publishedAll
@released
*/
	{
public:
	IMPORT_C TIter(const TRangeRef& aRangeRef);
	IMPORT_C TBool operator++();
	inline TBool InRange() const;
public:
	TCellRef	iCurrent;
private:
	TRangeRef	iRange;
	};


inline TRangeRef::TRangeRef()
	/** Default constructor.
	
	The object's start cell and end cell are undefined. */
	{}
inline TRangeRef::TRangeRef(const TCellRef& aFrom,const TCellRef& aTo)
	: iFrom(aFrom),iTo(aTo)
	/** Constructor taking a start and end cell.
	
	@param aFrom The start cell.
	@param aTo The end cell. */
	{}
inline TRangeRef::TRangeRef(TInt aFromRow,TInt aFromCol,TInt aToRow,TInt aToCol)
	: iFrom(aFromRow,aFromCol),iTo(aToRow,aToCol)
	/** Constructor taking a start row and start column number, and an end row and 
	end column number.
	
	@param aFromRow The start row number.
	@param aFromCol The start column number.
	@param aToRow The end row number.
	@param aToCol The end column number. */
	{}
inline void TRangeRef::SetRange(const TCellRef& aFrom,const TCellRef& aTo)
	/** Sets the cell range to the specified cells.
	
	@param aFrom The start cell for the range.
	@param aTo The end cell for the range. */
	{iFrom=aFrom;iTo=aTo;}
inline void TRangeRef::SetRange(TInt aFromRow,TInt aFromCol,TInt aToRow,TInt aToCol)
	/** Sets the cell range to the specified start row and column, and end row 
	and column.
	
	@param aFromRow The start row number.
	@param aFromCol The start column number.
	@param aToRow The end row number.
	@param aToCol The end column number. */
	{iFrom.iRow=aFromRow;iFrom.iCol=aFromCol;iTo.iRow=aToRow;iTo.iCol=aToCol;}
inline TBool TRangeRef::operator!=(const TRangeRef& aRange) const
	/** Compares this cell range with the specified cell range for inequality.
	
	@param aRange The cell to be compared.
	@return ETrue, if the start cells are different or the end cells are 
	different. */
	{return !operator==(aRange);}
inline TInt TRangeRef::NoRows() const
	/** Gets the number of rows represented by the range.
	
	@return The number of rows. */
	{return iTo.iRow - iFrom.iRow + 1;}
inline TInt TRangeRef::NoCols() const
	/** Gets the number of columns represented by the range.
	
	@return The number of columns. */
	{return iTo.iCol - iFrom.iCol + 1;}
inline TBool TRangeRef::TIter::InRange() const
	{return iRange.Contains(iCurrent);}

#endif
