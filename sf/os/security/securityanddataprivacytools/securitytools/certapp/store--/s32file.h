#ifndef __S32FILE_H__
#define __S32FILE_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32base.h>
#include <f32file.h>
#include <s32strm.h>
#include <fstream>
#include <string>
#include <memory>
#include <map>

/**
 * @file
 * @internalComponent
 */

#define KPermanentFileStoreLayoutUid TUid::Uid(268435536)
typedef TUint32 TStreamId;
/** The value of the null stream ID. */
const TUint32 KNullStreamIdValue=0;
const TUint32 KMaxStreamIdValue=0xfffffff;
const TUint32 KMaskStreamIdValue=0xfffffff;
const TInt KShiftStreamIdValue=28;

namespace store_private
{
};




class CStreamStore
	{
public:
	virtual ~CStreamStore();
	inline TStreamId ExtendL();
	IMPORT_C void Delete(TStreamId anId);
	IMPORT_C void DeleteL(TStreamId anId);

	virtual void CommitL() = 0;

	//
	// Internal store emulation functions
	//
	virtual store_private::MemStreamBuf *CreateStoreWriteStream() = 0;
	virtual store_private::MemStreamBuf *CreateStoreReadStream(TStreamId aId) const = 0;
	virtual bool Writable() const = 0;
	};

class CPersistentStore : public CStreamStore
	{
public:
	TStreamId Root();
	void SetRootL(TStreamId anId);
protected:
	TStreamId iRoot;
	};

class CFileStore : public CPersistentStore
	{
public:
	IMPORT_C void SetTypeL(const TUidType& aType);
	};


class CPermanentFileStore : public CFileStore
	{
public:
	static CFileStore *ReplaceLC(RFs &aFs, const TDesC &aName, TUint aFileMode);
	static CPermanentFileStore* OpenLC(RFs& aFs, const TDesC& aName, TUint aFileMode);

public:
	//
	// Internal store emulation functions
	//
	virtual store_private::MemStreamBuf *CreateStoreWriteStream();
	virtual store_private::MemStreamBuf *CreateStoreReadStream(TStreamId aId) const;
	virtual bool Writable() const;
private:
	CPermanentFileStore(const TDesC &aName, TUint aFileMode);
	virtual ~CPermanentFileStore();
	virtual void CommitL();

	void ReadInData();
	void ReadStreamData(TStreamId aStreamId, TInt aFrameOffset);
	
	bool iWritable;
	std::auto_ptr<RReadStream> iFileIn;
	std::auto_ptr<RWriteStream> iFileOut;

	TUint32 iLastStreamIdCreated;
	typedef std::map<TUint32, store_private::MemStreamBuf *> MemStreamBufPtrMap;
	MemStreamBufPtrMap iStreams;
	};


class RStoreWriteStream : public RWriteStream
	{
public:
	IMPORT_C TStreamId CreateLC(CStreamStore& aStore);
	};

class RStoreReadStream : public RReadStream
	{
public:
	RStoreReadStream();
	IMPORT_C void OpenLC(const CStreamStore& aStore, TStreamId aId);
	};


#endif
