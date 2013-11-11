// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UT_STD.H"

#pragma BullseyeCoverage off

//
// Panic the process with STORE-Store as the category.
//
GLDEF_C void Panic(TStorePanic aPanic)
	{
	_LIT(KCategory,"STORE-Store");
	User::Panic(KCategory,aPanic);
	}

#pragma BullseyeCoverage on

EXPORT_C void TStreamId::InternalizeL(RReadStream& aStream)
/** Internalises an object of this class from a read stream.

The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

Note that the function has assignment semantics. It replaces the old value 
of the object with a new value read from the read stream.

@param aStream Stream from which the object should be internalised. */
	{
	aStream>>iVal;
	if (iVal>KMaxStreamIdValue)
		__LEAVE(KErrCorrupt);
	}

EXPORT_C void MIncrementalCollector::DoRelease()
/** Implementation of the public Release() function. This signals that client has 
no further need of the object and all necessary clean-up should be done. e.g. 
if the implementation object is allocated on the heap, it could be deleted. */
	{}

EXPORT_C void MIncrementalCollector::DoNextL(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus,TPckgBuf<TInt>& aTotal)
/** Implementation of the public asynchronous NextL() function. The default implementation 
invokes the synchronous form of DoNextL() and then reports the result by signalling 
the request status.

@param aStep The progress value from either the last NextL() increment of 
from ResetL() if the reclamation/compaction was restarted. On return, 
should contain the new progress value, which can be used in subsequent calls 
to NextL(). This must be equal to, or less than, the previous value a 
zero value must be used to indicate that the operation is complete.
@param aStatus A status variable. KErrNone on successful completion, otherwise 
another of the system-wide error codes.
@param aTotal On return, should contain the total amount of free space in the 
store. */
	{
	DoNextL(aStep(),aTotal());
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	}

HDirectStoreBuf* HDirectStoreBuf::CreateL(TStreamExchange& aHost,TStreamId& anId,TInt aMode)
//
// Create a stream buffer for a new 'direct' stream.
//
	{
	TInt size=aHost.SizeL();
	HDirectStoreBuf* buf=NewL(aHost,size,aMode);
	anId=TStreamId(size);
	return buf;
	}

HDirectStoreBuf* HDirectStoreBuf::OpenL(TStreamExchange& aHost,TStreamId anId,TInt aMode)
//
// Create a stream buffer for an existing 'direct' stream.
//
	{
	HDirectStoreBuf* buf=NewL(aHost,anId.Value(),aMode);
	return buf;
	}

HDirectStoreBuf* HDirectStoreBuf::NewL(TStreamExchange& aHost,TInt anOffset,TInt aMode)
	{
	HDirectStoreBuf* buf=new(ELeave) HDirectStoreBuf(anOffset);
	buf->Open(aHost,TStreamPos(anOffset),aMode);
	return buf;
	}

void HDirectStoreBuf::DoRelease()
//
// Finished with this stream buffer.
//
	{
	delete this;
	}

TStreamPos HDirectStoreBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	TInt off=iOff;
	if (aLocation==EStreamBeginning)
		anOffset+=off;
	return RShareBuf::DoSeekL(aMark,aLocation,anOffset)-off;
	}

EXPORT_C void TStreamId::__DbgChkRange(TUint32 aValue)
//
// Check for values out of range.
//
	{
	__ASSERT_ALWAYS(aValue<=KMaxStreamIdValue,Panic(EStoreIdOutOfRange));
	}

