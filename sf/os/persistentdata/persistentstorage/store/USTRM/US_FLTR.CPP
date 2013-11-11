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

#include "US_STD.H"

EXPORT_C TStreamFilter::TStreamFilter()
	: iHost(NULL),iMode(0)
/** Constructs an empty stream filter object. */
	{}

EXPORT_C void TStreamFilter::EmitL(const TAny* aPtr,TInt aLength)
/** Writes data from the specified memory location directly to the host without 
filtering.

This is useful for sending any final data, when flushing the filter as part 
of DoSynchL().

In debug mode: the filter must be in write mode, otherwise the function raises 
a STORE-Stream 11 panic. 

In debug mode, a host stream must have been set before calling this function, 
otherwise it raises a STORE-Stream 0 panic.

@param aPtr A pointer to the memory location from which data is to be written 
to the host stream.
@param aLength The number of bytes to be written.
@see DoSynchL() */
	{
	__ASSERT_DEBUG(iMode&EWrite,Panic(EStreamCannotWrite));
	__ASSERT_DEBUG(iHost!=NULL,Panic(EStreamNotOpen));
	iHost->WriteL(aPtr,aLength);
	}

EXPORT_C void TStreamFilter::DoRelease()
/** Frees the host stream's resources.

@see MStreamBuf::DoRelease() */
	{
	if (iHost!=NULL&&(iMode&EAttached))
		iHost->Release();
	iHost=NULL;
	}

EXPORT_C void TStreamFilter::DoSynchL()
/** Synchronizes the host's intermediate buffer with its stream, leaving if any 
error occurs.

@see MStreamBuf::DoSynchL() */
	{
	if (iHost!=NULL&&(iMode&EAttached))
		iHost->SynchL();
	}

EXPORT_C TInt TStreamFilter::DoReadL(TAny* aPtr,TInt aMaxLength)
/** Reads data from the host stream through the filter into the specified memory 
location.

In debug mode: the filter must be in read mode, otherwise the function raises 
a STORE-Stream 10 panic.

In debug mode, a host stream must have been set before calling this function, 
otherwise it raises a STORE-Stream 0 panic.

@param aPtr A pointer to the target memory location for the filtered data.
@param aMaxLength The maximum number of bytes to be read. In debug mode: if 
this value is negative then the function raises a STORE-Stream 1 panic; if 
this value is zero, then the function raises a STORE-Stream 3 panic.
@return The number of bytes read. */
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamReadLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamReadNoTransfer));
	__ASSERT_DEBUG(iMode&ERead,Panic(EStreamCannotRead));
	__ASSERT_DEBUG(iHost!=NULL,Panic(EStreamNotOpen));
	TFilterInput input(*this,aPtr,aMaxLength);
	do
		{
		iHost->ReadL(input);
		} while (!(input.Done()||input.Eof()));
	return aMaxLength-input.Left();
	}

EXPORT_C void TStreamFilter::DoWriteL(const TAny* aPtr,TInt aLength)
/** Writes data to the host stream through the filter from the specified memory 
location.

In debug mode: the filter must be in write mode, otherwise the function raises 
a STORE-Stream 11 panic.

In debug mode, a host stream must have been set before calling this function, 
otherwise it raises a STORE-Stream 0 panic.

@param aPtr A pointer to the source memory location.
@param aLength The number of bytes to be written. In debug mode: if this value 
is negative then the function raises a STORE-Stream 1 panic; if this value 
is zero, then the function raises a STORE-Stream 7 panic. */
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	__ASSERT_DEBUG(aLength>0,Panic(EStreamWriteNoTransfer));
	__ASSERT_DEBUG(iMode&EWrite,Panic(EStreamCannotWrite));
	__ASSERT_DEBUG(iHost!=NULL,Panic(EStreamNotOpen));
	TFilterOutput output(*this,aPtr,aLength);
	do
		{
		iHost->WriteL(output);
		} while (!output.Done());
	}

EXPORT_C void TStreamFilter::__DbgChkMode(TInt aMode)
//
// Check mode allows either reading or writing, but not both.
//
	{
	__ASSERT_ALWAYS((aMode&(ERead|EWrite))&&(aMode&(ERead|EWrite))!=(ERead|EWrite),Panic(EStreamModeInvalid));
	}

