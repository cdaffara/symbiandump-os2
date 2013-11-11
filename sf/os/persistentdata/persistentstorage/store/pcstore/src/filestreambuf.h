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

#if !defined(__FILESTREAMBUF_H__)
#define __FILESTREAMBUF_H__

#include <stdio.h>
#include <pcstore/streamid.h>

namespace PCStore
{
class CDirectFileStore;

/**
@internalComponent
 
A file stream buffer that provides I/O interfaces for streamed data.

This file stream buffer: 
. is unbuffered: all data passed to it will immediately be written to the corresponding file
  or returned to its caller.

. provides read or write capability: it opens the file in read or write mode. After it is 
  instantiated, only read or write function can be called, depending on the file open mode. 
  Exception will be thrown upon an unmatching call.

. is unseekable. It only moves the file pointer to the position represented by the stream id, 
  which is passed to its constructor. In the subsequent read or write operations, the pointer 
  always goes forward.

For each file store, one object of this class is created in the constructor of CDirectFileStore. 
CDirectFileStore will pass the reference of this object to the stream it creates. In order to 
ensure there is only one stream opened in any moment, streams will call the StreamOpen and 
StreamClose methods respectively in their constructor and destructor to signal their open 
and close.

@see CDirectFileStore 
@see CStoreWriteStream
@see CStoreReadStream
*/
class CFileStreamBuf
	{
public:
	//Enumerations for the file open mode
	enum TFileMode 
	{
	EReadFile=0,
	EWriteFile=1
	};

public:
	CFileStreamBuf(const char* aFileName, TFileMode aMode);
	~CFileStreamBuf();
	void StreamOpen(TStreamId aStreamId);
	void StreamClose();
	TStreamId GetNewStreamId();
	void Read(const TUint8* aPtr, TInt32 aLength);
	void Write(const TUint8* aPtr, TInt32 aLength);

private:
	CFileStreamBuf(const CFileStreamBuf& aBuf); 
	CFileStreamBuf& operator=(const CFileStreamBuf& aBuf); 

private:
	FILE* iFile;
	TBool iStreamOpened;
	};
}
#endif // !defined(__FILESTREAMBUF_H__)
