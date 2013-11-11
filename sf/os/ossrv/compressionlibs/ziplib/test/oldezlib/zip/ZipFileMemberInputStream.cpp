// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Revision: 1.1 $
// 
//


#include "oldzipfile.h"
#include "oldzipfilememberinputstream.h"

using namespace TOLDEZIP;

RZipFileMemberReaderStream::RZipFileMemberReaderStream(
    CZipFile& aZipFile,
    TUint32   aDataOffset,
    TUint32   aCompressedSize,
    TUint32   aUncompressedSize,
    TUint32   aCompressionMethod):
    iZipFile(aZipFile),
	iCompressionMethod(aCompressionMethod),
    iCompressedSize(aCompressedSize),
    iUncompressedSize(aUncompressedSize),
	iFileOffset(aDataOffset)
    {    
    }


RZipFileMemberReaderStream* RZipFileMemberReaderStream::NewL(
	CZipFile& aZipFile,
    TUint32   aDataOffset,
    TUint32   aCompressedSize,
    TUint32   aUncompressedSize,
    TUint32   aCompressionMethod)
    {    
	RZipFileMemberReaderStream* me = new(ELeave) RZipFileMemberReaderStream(aZipFile, aDataOffset, aCompressedSize,aUncompressedSize, aCompressionMethod);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;		
    }

/**
Creates input stream to be used for reading the contents of the compressed file.
*/
void RZipFileMemberReaderStream::ConstructL()
	{
	TInt err = inflateInit2(&iStream, -MAX_WBITS);
	if (err == Z_MEM_ERROR)
		{
		User::Leave(KErrNoMemory);
		}
	}

/**
Destructor. All dynamically allocated data structures for this stream are freed.
*/
EXPORT_C RZipFileMemberReaderStream::~RZipFileMemberReaderStream()
{
	inflateEnd(&iStream);
}

/**
Reads data from the stream buffer into the specified descriptor.
On return, contains the data read from the stream buffer

@param aDes The target descriptor for the data read from the stream buffer
@param aLength The maximum number of bytes to be read
@return KErrNone If all bytes read successfully.
@return	KErrCorrupt If reading fails.
@return KErrEof If end of file is reached.
@return ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C TInt RZipFileMemberReaderStream::Read(TDes16& aDes, TInt aLength)
{
	TUint32 numBytesRead = 0;
	TInt err = Read(CONST_CAST(TByte*,(const TByte*)aDes.Ptr()), 2*aLength, &numBytesRead);
	if (err != KErrNone)
	{
		aDes.SetLength( (err==KErrEof) ? numBytesRead>>2 : 0 );
		return err;
	}
	
	aDes.SetLength(numBytesRead>>2);
	return KErrNone;
}

TInt RZipFileMemberReaderStream::Read(void)
	{
	TByte b;
	TUint32 numRead = 0;
	
	if (Read(&b, 1, &numRead) == 1)
		{
		return b;
		}
	else
		{
		return -1;
		}
	}
	
TInt RZipFileMemberReaderStream::Read(TDes8& aDes, TInt aLength)
	{
	TUint32 numBytesRead = 0;
	TInt err = Read(CONST_CAST(TByte*,aDes.Ptr()), aLength, &numBytesRead);
	if (err != KErrNone)
	{
		aDes.SetLength( (err==KErrEof) ? numBytesRead : 0 );
		return err;
	}

	aDes.SetLength(numBytesRead);
	return KErrNone;
	}

void RZipFileMemberReaderStream::ReadL(TDes16& aDes, TInt aLength)
	{
	User::LeaveIfError(Read(aDes, aLength));
	}

void RZipFileMemberReaderStream::Release()
{}

void RZipFileMemberReaderStream::Close()
{}


TInt RZipFileMemberReaderStream::Read(TByte* aBytes, TUint32 aLength, TUint32* aRetLength)
	{
	if (iCompressionMethod == CZipArchive::EDeflated)
		{
		return GetBytes(aBytes, aLength, aRetLength);
		}
	else
		{
		return GetStoredBytes(aBytes, aLength, aRetLength);
		}
	}
	
TInt RZipFileMemberReaderStream::GetBytes(TByte* aBytes, TUint32 aLength, TUint32* aRetLength)
	{
	TUint32 bytesLeftToRead;
	TInt	status;

	iBytesLength = 0;

	// Be careful not to confuse compressed bytes and uncompressed bytes.
	// The length passed in is in uncompressed bytes, compressed bytes
	// are mainly used in the GetCompressedBytes() function called.
	// iBytesLength refers to the number of bytes already read.
	// If the request is to read past the end of the file
	// we should return KErrNone on the first instance, i.e. return all bytes
	// read successfully. On the second instance return KErrEof, i.e. we have 
	// already read all the bytes when another request comes in, so return KErrEof.
	// This follows the rules for reading an uncompressed file within this component
	// and this is also the way that RFile::ReadL() does it.

	if (aLength > iUncompressedSize)
		{
		aLength = iUncompressedSize; // no point trying to read more than we have
		}

	bytesLeftToRead = aLength;

	while (bytesLeftToRead > 0) 
		{
		if (iStream.avail_in == 0)
			{
			if (GetCompressedBytes() != KErrNone)
				{
				return KErrCorrupt;
				}
			}

		// The decompression will be done in the user provided memory.
		iStream.next_out = &aBytes[iBytesLength];
		iStream.avail_out = aLength - iBytesLength;
		status = inflate(&iStream, Z_SYNC_FLUSH);

		switch (status)
			{
			case Z_OK:
				iBytesLength = aLength - iStream.avail_out;
				break;
				
			case Z_STREAM_END:	//EOF
				if (iBytesLength == aLength - iStream.avail_out)
					{
					*aRetLength = iBytesLength;
					return KErrEof;
					}
				else
					{
					iBytesLength = aLength - iStream.avail_out;
					break;
					}
			case Z_MEM_ERROR:
				return KErrNoMemory;
				
			default:
				return KErrCorrupt;

			}
		bytesLeftToRead = aLength - iBytesLength;
		}

	*aRetLength = iBytesLength;
	return KErrNone;
	}

TInt RZipFileMemberReaderStream::GetCompressedBytes(void)
	{
	if (iOffset < iCompressedSize)
		{
		TUint32	nBytesLeft;
		TUint32 nBytesToRead;
		
		nBytesLeft = iCompressedSize - iOffset;
		nBytesToRead = (nBytesLeft > sizeof(iCompressedBytes)) ? sizeof(iCompressedBytes) : nBytesLeft;
		if (iZipFile.Seek(iFileOffset) != KErrNone)
			{
			return KErrCorrupt; 
			}
		else
		if (iZipFile.Read(iCompressedBytes, nBytesToRead) != KErrNone)
			{
			return KErrCorrupt; 
			}
		iFileOffset += nBytesToRead;
		iOffset += nBytesToRead;
		iStream.next_in = iCompressedBytes;
		iStream.avail_in = nBytesToRead;
		return KErrNone;
		}
	else
	if (iDone == EFalse)
		{
		iCompressedBytes[0] = 0;
		iStream.avail_in = 1;
		iStream.next_in = iCompressedBytes;
		iDone = ETrue;
		return KErrNone;
		}
	else
		{
		return KErrCorrupt; 
		}
	}
	
TInt RZipFileMemberReaderStream::GetStoredBytes(TByte* aBytes, TUint32 aLength, TUint32* aRetLength)
	{
	TInt status;
	
	if (aLength > iUncompressedSize)
		{
		aLength = iUncompressedSize; // no point trying to read more than we have
		}
	if (aLength == 0) // empty file like a directory
		{
		return KErrNone;
		}
	if (iOffset == iCompressedSize) // end of zip item.
		{
		return KErrEof; 
		}
	if ((iOffset + aLength) > iCompressedSize)
		{
		aLength = iCompressedSize - iOffset; // adjust read to what is left
		if ( aLength <= 0 )
			{
			return KErrCorrupt; 
			}
		}
	if (iZipFile.Seek(iFileOffset) != KErrNone)
		{
		return KErrCorrupt; 
		}
	status = iZipFile.Read(aBytes, aLength);
	if (status == KErrNone)
		{
		iFileOffset += aLength;
		iOffset += aLength;
		*aRetLength = aLength;
		}
	return status;
	}
