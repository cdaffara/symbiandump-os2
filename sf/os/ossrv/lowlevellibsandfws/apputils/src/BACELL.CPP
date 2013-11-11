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

#include <e32std.h>
#include <bacell.h>

EXPORT_C TCellRef operator+(const TCellRef& aLeft,const TCellRef& aRight)
	{
	return TCellRef(aLeft.iRow+aRight.iRow,aLeft.iCol+aRight.iCol);
	}

EXPORT_C TCellRef operator-(const TCellRef& aLeft,const TCellRef& aRight)
	{
	return TCellRef(aLeft.iRow-aRight.iRow,aLeft.iCol-aRight.iCol);
	}

EXPORT_C void TCellRef::Offset(TInt aRowOffset,TInt aColOffset)
/** Adds the specified row and column numbers to this cell's row and column values.

@param aRowOffset The row number to be added. The value may be positive, zero 
or negative.
@param aColOffset The column number to be added. The value may be positive, 
zero or negative. */
	{
	iRow += aRowOffset;
	iCol += aColOffset;
	}

EXPORT_C void TCellRef::InternalizeL(RReadStream& aStream)
/** Internalises an object of this class from a read stream.

The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

Note that the function has assignment semantics. It replaces the old value 
of the object with a new value read from the read stream. 

@param aStream Stream from which the object is to be internalised. */
	{
	iRow = aStream.ReadInt32L();
	iCol = aStream.ReadInt32L();
	}

// class TRangeRef

EXPORT_C TBool TRangeRef::operator==(const TRangeRef& aRange) const
/** Compares this cell range with the specified cell range for equality.

@param aRange The cell range to be compared.
@return ETrue, if the start cells and the end cells are the same. */
	{
	return aRange.iFrom==iFrom && aRange.iTo==iTo;
	}

EXPORT_C TInt TRangeRef::NoCells() const
/** Gets the number of cells represented by the range.

@return The number of cells. */
	{
	return NoRows()*NoCols();
	}

EXPORT_C TBool TRangeRef::Contains(const TCellRef& aCell) const
/** Tests whether the specified cell is contained within the range.

A range includes its outer perimeter.

@param aCell The cell to be tested.
@return ETrue, if the specified cell is contained within the range; 
EFalse, otherwise. */
	{
	TInt row=aCell.iRow;
	TInt col=aCell.iCol;
	return row>=iFrom.iRow && row<=iTo.iRow && col>=iFrom.iCol && col<=iTo.iCol;
	}

EXPORT_C void TRangeRef::InternalizeL(RReadStream& aStream)
/** Internalises an object of this class from a read stream.

The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

Note that the function has assignment semantics. It replaces the old value 
of the object with a new value read from the read stream. 

@param aStream Stream from which the object is to be internalised. */
	{
	aStream>>iFrom>>iTo;
	}

EXPORT_C void TRangeRef::ExternalizeL(RWriteStream& aStream) const
/** Externalises an object of this class to a write stream.

The presence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class. 

@param aStream Stream to which the object should be externalised. */
	{
	aStream<<iFrom<<iTo; 
	}

// TRangeRef::TIter

EXPORT_C TRangeRef::TIter::TIter(const TRangeRef& aRange)
	: iCurrent(aRange.iFrom),iRange(aRange)
    {}

EXPORT_C TBool TRangeRef::TIter::operator++()
	{
	if (iCurrent.iCol<iRange.iTo.iCol)
		{
		++iCurrent.iCol;
		}
	else
		{
		++iCurrent.iRow;
		iCurrent.iCol = iRange.iFrom.iCol;
		}
	return iCurrent.iRow<=iRange.iFrom.iRow;
	}


/* End of $Workfile:   bacell.cpp  $*/
