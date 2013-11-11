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

#if !defined(__DIRECTFILESTORE_H__)
#define __DIRECTFILESTORE_H__

#include <pcstore/pcstoredef.h>
#include <pcstore/streamid.h>
#include <pcstore/uid.h>

namespace PCStore
{
class TUidType;
class CStoreWriteStream;
class CStoreReadStream;
class CFileStreamBuf;

const TInt KDirectFileStoreLayoutUidValue=0x10000037; //268435511;
const TUid KDirectFileStoreLayoutUid={KDirectFileStoreLayoutUidValue};

/**
@internalAll

Direct file store.

A direct file store implements a set of the operations to open store, create streams(CStoreWriteStream 
and CStoreReadStream), set root stream id to the store.

Objects can be externalized to streams represented by class CStoreWriteStream. Once the streams 
have been closed, they cannot subsequently be changed, i.e. streams cannot be replaced, deleted, 
extended or changed in any way. 

Existing direct file stores can be opened with this class. Objects can be restored with class 
CStoreReadStream.

Before closing a new store, the root stream id must be set. After opening an existing store, the 
first thing done is to look up the root stream id. The root stream can then be opened and data 
read from the store.

@see CStoreWriteStream
@see CStoreReadStream
*/
class CDirectFileStore
	{
public:
	//Enumerations for the store open mode
	enum TStoreMode 
	{
	EReadStore=0,
	EWriteStore=1
	};

public:
	~CDirectFileStore();

	void SetRoot(TStreamId aId);
	TStreamId Root() const;
	static CDirectFileStore* Open(const char* aFileName);
	static CDirectFileStore* Replace(const char* aFileName);
	static CDirectFileStore* Replace(const char* aFileName, const TUidType& aType);
	CStoreWriteStream* CreateWriteStream(TStreamId& aId);
	CStoreReadStream* CreateReadStream(TStreamId aId);

private:
	CDirectFileStore(const char* aFileName, TStoreMode aMode);
	CStoreWriteStream* CreateIdWriteStream(TStreamId aId);

	void SetType(const TUidType& aType);
	void CheckType();
	void Externalize(CStoreWriteStream& aWs) const;
	void Internalize(CStoreReadStream& aRs);

private:
	TStoreMode iMode;
	TStreamId iRoot;
	CFileStreamBuf* iFileStreamBuf;
	};
}
#endif // !defined(__DIRECTFILESTORE_H__)
