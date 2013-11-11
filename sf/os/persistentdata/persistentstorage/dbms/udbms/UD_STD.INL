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

inline TBool IsLong( TDbColType aType )
	{ return aType >= EDbColLongText8; }

inline TBool IsIntegral( TDbColType aType )
	{ return aType <= EDbColInt64; }

inline TBool IsSigned(TDbColType aType)
	{ __ASSERT( IsIntegral( aType ) ); return aType&1; }

inline TBool IsUnsigned( TDbColType aType )
	{ return !IsSigned( aType ); }

// Class RDbRowSet
inline MStreamBuf *RDbRowSet::ColSourceL( TDbColNo aCol ) const
	{ return CheckCol( aCol ).ColumnSourceL( aCol ); }

inline MStreamBuf *RDbRowSet::ColSinkL( TDbColNo aCol )
	{ return CheckCol( aCol ).ColumnSinkL( aCol ); }

// class RDbRow
inline TBool RDbRow::Owned() const
	{ return iColumn & static_cast<TUint>( EOwned ); }

inline TInt RDbRow::Column() const
	{ return iColumn & ~EOwned; }

inline void RDbRow::SetCache( TDbCell* aCell, TInt aColumn )
	{ iCell = aCell; iColumn = (iColumn & static_cast<TUint>( EOwned ) ) | aColumn; }

