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


#include <assert.h>
#include <pcstore/storeexception.h>
#include "filestreambuf.h"

namespace PCStore
{
/**
Constructs a file stream buffer object with the specified file name and open mode.

Opens the file in specified read/write mode.

@param aFileName The name of the file which the file stream buffer will open and access.
@param aMode The mode in which the file would be opened.
@exception TStoreException::EFileOpenError Error occurs in file opening.
*/
CFileStreamBuf::CFileStreamBuf(const char* aFileName, TFileMode aMode)
	:iFile(NULL), iStreamOpened(false)
	{
	if(aMode == EWriteFile) 
		{
		iFile=fopen(aFileName,"wb");
		}
	else
		{
		iFile=fopen(aFileName,"rb");
		}
	if(iFile == NULL) 
		{
		throw TStoreException(TStoreException::EFileOpenError);
		}
	}

/**
Destructor.
Closes the file.
*/
CFileStreamBuf::~CFileStreamBuf()
	{
	fclose(iFile);
	}

/**
Flags that a stream with the specified stream id is opened.

Checks whether a stream has been opened. If not, moves the file pointer to the position 
which is represented by the given stream id, and sets the stream open flag to TRUE. If a 
stream has been opened, throws an exception.

This function is called by the constructor of CStoreReadStream and CStoreWriteStream
to flag the new stream is opened. 

@param aStreamId The stream id of the open stream.
@exception TStoreException::EStreamExclusionError Another stream is still opened.
@exception TStoreException::EFileSeekError Error occurs when seeking within the file 
due to an incorrect stream id or other reason.
*/
void CFileStreamBuf::StreamOpen(TStreamId aStreamId)
	{
	//check the opened stream
	if(iStreamOpened)
		{
		throw TStoreException(TStoreException::EStreamExclusionError);
		}
	// set pointer to the correct position
	TInt ret = fseek(iFile,aStreamId.Value(),SEEK_SET);  
	if(ret)
		{
		throw TStoreException(TStoreException::EFileSeekError);
		}

	iStreamOpened = true;
	}

/** 
Flags that the opened stream is closed.

This function is called by the destructor of the CStoreReadStream and CStoreWriteStream 
to indicate the stream is being closed.
*/
void CFileStreamBuf::StreamClose()
	{
	iStreamOpened = false;
	}

/**
Gets a new stream id which will be used to create a write stream.

The new stream id is the end position of the store file.

Moves the read/write pointer to the end to the file to get the stream id.

@return The new stream id.
@exception TStoreException::EStreamExclusionError Another stream is still opened.
@exception TStoreException::EFileSeekError Error occurs when the file pointer is moved to 
the end of file.
*/	
TStreamId CFileStreamBuf::GetNewStreamId()
	{
	//check the opened stream
	if(iStreamOpened)
		{
		throw TStoreException(TStoreException::EStreamExclusionError);
		}
	// move pointer to the end of the file
	TInt ret = fseek(iFile,0,SEEK_END);  
	if(ret) 
		{
		throw TStoreException(TStoreException::EFileSeekError);
		}

	TUint size = ftell(iFile);
	TStreamId id = size;
	return id;
	}

/**
Reads the specified length of 8-bit data to the supplied buffer from the file.

Assert aLength must not be negative and aPtr must not be NULL.

@param aPtr The pointer to the buffer to receive the read data.
@param aLength The length of the data to read.
@exception TStoreException::EFileReadError Error occurs in data reading.
*/
void CFileStreamBuf::Read(const TUint8* aPtr, TInt32 aLength)
	{
	assert(aLength >= 0 && aPtr != NULL);
	if (aLength > 0) 
		{
		size_t numRead;
		numRead = fread(reinterpret_cast<void*>(const_cast<TUint8*>(aPtr)),\
			sizeof(TUint8), aLength, iFile);
		if(numRead < static_cast<TUint32>(aLength))
			{
			throw TStoreException(TStoreException::EFileReadError);
			}
		}
	}

/**
Writes the specified length of 8-bit data from the supplied buffer to the file.

Assert aLength must not be negative and aPtr must not be NULL.

@param aPtr The pointer to the buffer holding the written data.
@param aLength The length of the data to write.
@exception TStoreException::EFileWriteError Error occurs in data writing.
*/
void CFileStreamBuf::Write(const TUint8* aPtr, TInt32 aLength)
	{
	assert(aLength >= 0 && aPtr != NULL);	
	if (aLength > 0)
		{
		size_t numWritten ;
		numWritten  = fwrite(reinterpret_cast<void*>(const_cast<TUint8*>(aPtr)),\
			sizeof(TUint8), aLength, iFile);
		if(numWritten < static_cast<TUint32>(aLength))
			{
			throw TStoreException(TStoreException::EFileWriteError);
			}
		}
	}
}
