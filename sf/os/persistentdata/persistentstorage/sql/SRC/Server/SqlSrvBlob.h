// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVBLOB_H__
#define __SQLSRVBLOB_H__

#include <s32buf.h>

//Forward declarations
struct sqlite3_blob;

/**
Stream buffer class with no intermediate buffering capabilities.

The primary purpose of this class is:
 - to read/write from/to a blob column using sqlite3_blob_read()/sqlite3_blob_write() SQLite functions;
 - to serve as an IPC buffer for streaming the blob content from/to the client side dll;
 
The class overrides the following virtual MStreamBuf functions:
 - DoRelease();
 - DoSynchL();
 - DoReadL();
 - DoWriteL();
 - DoSeekL();

@internalComponent
*/
NONSHARABLE_CLASS(HBlobBuf) : public MStreamBuf
	{
public: 
	enum TMode 
		{
		EReadOnly,
		EReadWrite
		};

public: 
	static HBlobBuf* NewL(sqlite3* aDb, const TDesC8& aDbName, const TDesC8&  aTableName, const TDesC8&  aColumnName, TInt64 aRowId, TMode aMode);
		
private:
	HBlobBuf();
	void ConstructL(sqlite3* aDb, const TDesC8& aDbName, const TDesC8& aTableName, const TDesC8& aColumnName, TInt64 aRowId, TMode aMode);
	
	virtual void DoRelease();
	virtual void DoSynchL();
	virtual TInt DoReadL(TAny* aPtr, TInt aMaxLength);
	virtual void DoWriteL(const TAny* aPtr, TInt aLength);
	virtual TStreamPos DoSeekL(MStreamBuf::TMark aMark, TStreamLocation aLocation, TInt aOffset);

	sqlite3_blob* BlobHandleL();

private:
	sqlite3_blob*	iBlobHandle;
	TInt 			iBlobSize;
	TInt			iWrPos;
	TInt			iRdPos;
	
	};

#endif//__SQLSRVBLOB_H__
