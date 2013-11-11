// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLBUFITERATOR_H__
#define __SQLBUFITERATOR_H__

#include <sqldb.h>			//TSqlColumnType
#include "SqlBufFlat.h"		//RSqlBufFlat
#include "SqlAssert.h"	

//Forward declarations
class MStreamBuf;

/**
ZeroBlob column type. Used internally.
@internalComponent
*/
enum {ESqlZeroBlob = ESqlBinary + 1};

inline TBool IsSequenceSqlType(TInt aType)
	{
	return aType == ESqlNull || aType == ESqlText || aType == ESqlBinary;
	}

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////  Column type conversion table  /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//|--------------------------------------------------------------------------------|//
//|Column type | ColumnInt() ColumnInt64() ColumnReal() ColumnText() ColumnBinary()|//
//|--------------------------------------------------------------------------------|//
//|Null........|.0...........0.............0.0..........KNullDesC....KNullDesC8....|//
//|Int.........|.Int.........Int64.........Real.........KNullDesC....KNullDesC8....|//
//|Int64.......|.clamp.......Int64.........Real.........KNullDesC....KNullDesC8....|//
//|Real........|.round.......round.........Real.........KNullDesC....KNullDesC8....|//
//|Text........|.0...........0.............0.0..........Text.........KNullDesC8....|//   
//|Binary......|.0...........0.............0.0..........KNullDesC....Binary........|//
//|--------------------------------------------------------------------------------|//
//- "clamp": return KMinTInt or KMaxTInt if the value is outside the range that can //
//  be represented by the type returned by the accessor function.                   //
//- "round": the floating point value will be rounded up to the nearest integer.    //
//  If the result is outside the range that can be represented by the type returned //
//  by the accessor function, then it will be clamped.                              //
//////////////////////////////////////////////////////////////////////////////////////

/**
TSqlBufRIterator iterator gives read-only access to the flat buffer content (RSqlBufFlat class).
For convenience TSqlBufRIterator can represent the flat buffer fields content as one of the basic
SQL types: TInt, TInt64, TReal, 16-bit text, 8-bit binary block.
The text and binary fields may be accessed also as a byte stream.

@see RSqlBufFlat

@internalComponent
*/
class TSqlBufRIterator
	{
public:	
	inline void Set(const RSqlBufFlat& aBuf);
	inline TBool Next();
	inline void MoveTo(TInt aIndex);
	
	inline TBool IsPresent() const;
	inline TInt Type() const;
	inline TInt Size() const;
	
	TInt Int() const;
	TInt64 Int64() const;
	TReal Real() const;
	TPtrC8 Binary() const;
	TPtrC16 Text() const;
	
	MStreamBuf* StreamL() const;

private:
	inline TInt AsInt() const;
	inline TInt64 AsInt64() const;
	inline TReal AsReal() const;
	
private:
	const RSqlBufFlat::TCell*	iCurrent;
	const RSqlBufFlat::TCell*	iBegin;
	const RSqlBufFlat::TCell*	iEnd;
	
	};

/**
TSqlBufWIterator iterator gives write-only access to the flat buffer content (RSqlBufFlat class).
For convenience, with TSqlBufWIterator flat buffer fields can be set as:
TInt, TInt64, TReal, 16-bit text, 8-bit binary block.

@see RSqlBufFlat

@internalComponent
*/
class TSqlBufWIterator
	{
public:
	inline void Set(RSqlBufFlat& aBuf);
	inline TBool Next();
	inline void MoveTo(TInt aIndex);
	
	inline void SetNull();
	inline void SetAsNotPresent(TInt aType, TInt aLength);
	inline TInt SetInt(TInt aValue);
	inline TInt SetInt64(TInt64 aValue);
	inline TInt SetReal(TReal aValue);
	inline TInt SetBinary(const TDesC8& aValue);
	inline TInt SetText(const TDesC16& aValue);
	inline TInt SetZeroBlob(TInt aSize);
	
private:
	RSqlBufFlat*	iBuf;
	TInt			iIndex;
	
	};

#include "SqlBufIterator.inl"

#endif //__SQLBUFITERATOR_H__
