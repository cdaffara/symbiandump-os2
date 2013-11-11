// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <pcstore/streamid.h>
#include <pcstore/storewritestream.h>
#include <pcstore/storereadstream.h>
#include "pcstoreconst.h"

namespace PCStore
{
/**
Constructs a zero stream id.
*/
TStreamId::TStreamId()
	:iVal(0)
	{}

/**
Constructs a stream id with the specified value.
*/
TStreamId::TStreamId(TUint32 aValue)
	:iVal(aValue)
	{}


/**
Gets the stream ID value encapsulated by this object

@return The stream ID value. 
*/
TUint32 TStreamId::Value() const
	{
	return iVal;
	}

/** 
Internalizes an object of this class from a read stream.

@param aRs The reference to the stream from which the object should be internalized. 
*/
void TStreamId::Internalize(CStoreReadStream& aRs)
	{
	iVal = aRs.ReadUint32();
	}

/** 
Externalizes an object of this class to a write stream.

@param aWs The reference to the stream to which the object should be externalized. 
*/
void TStreamId::Externalize(CStoreWriteStream& aWs) const
	{
	aWs.WriteUint32(iVal);
	}
}
