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

#if !defined(__STREAMID_H__)
#define __STREAMID_H__

#include <pcstore/pcstoredef.h>

namespace PCStore
{
class CStoreReadStream;
class CStoreWriteStream;

/**
@internalAll

Provides unique identification for stream within a store.

A store always assigns a new id and constructs and returns an associated TStreamId 
object when a new stream is created.
*/
class TStreamId
	{
public:
	TStreamId();
	TStreamId(TUint32 aValue);
	TUint32 Value() const;
	void Internalize(CStoreReadStream& aRs) ;
	void Externalize(CStoreWriteStream& aWs) const;

private:
	TUint32 iVal;
	};
}
#endif // !defined(__STREAMID_H__)
