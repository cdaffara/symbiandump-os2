// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "FileBuf64.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FileBuf64Traces.h"
#endif
#include "SqliteTraceDef.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
This constant is used for initializing the RFileBuf64::iFileSize data member and means that
the iFileSize is not yet initialized with the real file size value. 
(RFileBuf64::iFileSize caches the file size value)
@internalComponent
*/
static const TInt KFileSizeNotSet = -1;

/**
This constant is used as a default initializer for the RFileBuf64::iNextReadFilePos data member,
indicating that the "guessed" file read offset is invalid and should not be used.
@internalComponent
*/
static const TInt KNextReadFilePosNotSet = -1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////        FBUF INVARIANT      ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

#define __FILEBUF64_INVARIANT() Invariant()

#else //_DEBUG

#define __FILEBUF64_INVARIANT() void(0)

#endif//_DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////        RFileBuf64    /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
/**
Initializes RFileBuf64 data members with their default values.

@param aSize Max file buffer size (capacity) in bytes.

@panic Sqlite3 1 In _DEBUG mode - aSize is 0 or negative.
*/
RFileBuf64::RFileBuf64(TInt aSize) :
	iCapacity(aSize),
	iBase(NULL),
	iReadAheadSize(RFileBuf64::KDefaultReadAheadSize)
	{
	SQLITE_TRACE_FBUF(OstTraceExt3(TRACE_INTERNALS, RFILEBUF64_RFILEBUF64, "FBuf;0x%X;RFileBuf64::RFileBuf64;aSize=%d;iReadAheadSize=%d", (TUint)this, aSize, iReadAheadSize));
	__ASSERT_DEBUG(aSize > 0, __SQLITEPANIC(EFBufPanicCapacity));
	}

/**
Initializes the RFileBuf64 object and creates and opens a new file that will be accessed through RFileBuf64 public interface.
If the file already exists, an error is returned.
If the resulting path does not exist, then the operation cannot proceed and the function returns an error code.

@param aFs       The file server session.
@param aFileName The name of the file. Any path components (i.e. drive letter
                 or directory), which are not specified, are taken from
                 the session path.
@param aFileMode The mode in which the file is opened. See TFileMode for details.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

@see TFileMode
@see RFile64::Create()

@panic Sqlite3  7 In _DEBUG mode - Invalid aFs object (null file session handle).
@panic Sqlite3 10 In _DEBUG mode - Invalid file name length (zero file name length).
*/
TInt RFileBuf64::Create(RFs& aFs, const TDesC& aFileName, TUint aFileMode)
	{
	__ASSERT_DEBUG(aFs.Handle() != 0, __SQLITEPANIC(EFBufPanicFsHandle));
	__ASSERT_DEBUG(aFileName.Length() > 0, __SQLITEPANIC(EFBufPanicFileNameLen));
	
	TInt err = DoPreInit();
	if(err == KErrNone)
	    {
	    err = iFile.Create(aFs, aFileName, aFileMode);
	    }
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_CREATE, "FBuf;0x%X;RFileBuf64::Create;aFs.Handle()=0x%X;aFileName=%S;iFile.SubSessionHandle()=0x%X;err=%d", (TUint)this, (TUint)aFs.Handle(), __SQLITEPRNSTR(aFileName), (TUint)iFile.SubSessionHandle(), err));
	DoPostInit(err);
	return err;
	}

/**
Initializes the RFileBuf64 object and opens an existing file that will be accessed through RFileBuf64 public interface.
If the file does not already exist, an error is returned.

@param aFs       The file server session.
@param aFileName The name of the file. Any path components (i.e. drive letter
                 or directory), which are not specified, are taken from
                 the session path.
@param aFileMode The mode in which the file is opened. See TFileMode for details.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

@see TFileMode
@see RFile64::Open()

@panic Sqlite3  7 In _DEBUG mode - Invalid aFs object (null file session handle).
@panic Sqlite3 10 In _DEBUG mode - Invalid file name length (zero file name length).
*/
TInt RFileBuf64::Open(RFs& aFs, const TDesC& aFileName, TUint aFileMode)
	{
	__ASSERT_DEBUG(aFs.Handle() != 0, __SQLITEPANIC(EFBufPanicFsHandle));
	__ASSERT_DEBUG(aFileName.Length() > 0, __SQLITEPANIC(EFBufPanicFileNameLen));
	
    TInt err = DoPreInit();
    if(err == KErrNone)
        {
        err = iFile.Open(aFs, aFileName, aFileMode);
        }
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_OPEN, "FBuf;0x%X;RFileBuf64::Open;aFs.Handle()=0x%X;aFileName=%S;iFile.SubSessionHandle()=0x%X;err=%d", (TUint)this, (TUint)aFs.Handle(), __SQLITEPRNSTR(aFileName), (TUint)iFile.SubSessionHandle(), err));
    DoPostInit(err);
	return err;
	}

/**
Initializes the RFileBuf64 object and creates and opens a temporary file with unique name that will be accessed through 
RFileBuf64 public interface.

@param aFs       The file server session.
@param aPath     The directory in which the file is created.
@param aFileName On return, contains the full path and file name of the file.
                 The filename is guaranteed to be unique within the directory
                 specified by aPath.
@param aFileMode The mode in which the file is opened. The access mode is
                 automatically set to EFileWrite. See TFileMode for details.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

@see TFileMode
@see RFile64::Temp()

@panic Sqlite3  7 In _DEBUG mode - Invalid aFs object (null file session handle).
*/
TInt RFileBuf64::Temp(RFs& aFs, const TDesC& aPath, TFileName& aFileName, TUint aFileMode)
	{
	__ASSERT_DEBUG(aFs.Handle() != 0, __SQLITEPANIC(EFBufPanicFsHandle));
	
    TInt err = DoPreInit();
    if(err == KErrNone)
        {
        err = iFile.Temp(aFs, aPath, aFileName, aFileMode);
        }
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_TEMP, "FBuf;0x%X;RFileBuf64::Temp;aFs.Handle()=0x%X;aFileName=%S;iFile.SubSessionHandle()=0x%X;err=%d", (TUint)this, (TUint)aFs.Handle(), __SQLITEPRNSTR(aFileName), (TUint)iFile.SubSessionHandle(), err));
    DoPostInit(err);
	return err;
	}

/**
Writes to the file the pending data (if the buffer contains pending data), closes the file and releases
the RFileBuf64 resources. 
RFileBuf64::Flush() should be called before RFileBuf64::Close() to ensure that if there are pending data, they will
be written to the file and if the operation fails, the caller will be notified with an appropriate return error.

@see RFileBuf64::Flush()
*/
void RFileBuf64::Close()
	{
	SQLITE_TRACE_FBUF(OstTraceExt2(TRACE_INTERNALS, RFILEBUF64_CLOSE, "FBuf;0x%X;RFileBuf64::Close;iFile.SubSessionHandle()=0x%X", (TUint)this, (TUint)iFile.SubSessionHandle()));
	if(iFile.SubSessionHandle() != 0 && iBase != 0)
		{
		(void)DoFileWrite2();
		}
	iFile.Close();
	User::Free(iBase);
	iBase = 0;
	}

/**
Calculates and sets optimal read-ahead buffer size.
aBlockSize and aReadRecBufSize values are retrieved by the caller from the file system.

Initialization rules:
Rule 1: If aReadRecBufSize is positive, bigger than the default read-ahead and 
        a power of two then the read-ahead value will be
        initialized with aReadRecBufSize (if aReadRecBufSize is less than the buffer capacity otherwise
        the buffer capacity will be used as a read-ahead value). 
Rule 2: If rule#1 is not applicable then the same checks, as in rule#1, are performed this time for aBlockSize.
        If aBlockSize passes the checks then it will be used as a read-ahead value. 

@param aBlockSize The size of a file block in bytes
@param aReadRecBufSize The recommended buffer size for optimised reading performance

@return The new read-ahead value

@see TVolumeIOParamInfo
*/
TInt RFileBuf64::SetReadAheadSize(TInt aBlockSize, TInt aReadRecBufSize)
	{
	__FILEBUF64_INVARIANT();
	if((aReadRecBufSize & (aReadRecBufSize - 1)) == 0 && aReadRecBufSize > RFileBuf64::KDefaultReadAheadSize)
		{
		iReadAheadSize = aReadRecBufSize > iCapacity ? iCapacity : aReadRecBufSize;
		}
	else if((aBlockSize & (aBlockSize - 1)) == 0 && aBlockSize > RFileBuf64::KDefaultReadAheadSize)
		{
		iReadAheadSize = aBlockSize > iCapacity ? iCapacity : aBlockSize;
		}
	SQLITE_TRACE_FBUF(OstTraceExt4(TRACE_INTERNALS, RFILEBUF64_SETREADAHEADSIZE, "FBuf;0x%X;RFileBuf64::SetReadAheadSize;aBlockSize=%d;aReadRecBufSize=%d;iReadAheadSize=%d", (TUint)this, aBlockSize, aReadRecBufSize, iReadAheadSize));
	__FILEBUF64_INVARIANT();
	return iReadAheadSize;
	}

/**
Reads from the file at the specified position (aFilePos).
If the data to be read is in the buffer, then the data will be taken from the buffer.

@param aFilePos Position of first byte to be read.  This is an offset from
            the start of the file. 
            If aPos is beyond the end of the file, the function returns
            a zero length descriptor.
@param aDes Descriptor into which binary data is read. Any existing contents 
            are overwritten. On return, its length is set to the number of
            bytes read.
            
@return KErrNone if successful, otherwise one of the other system-wide error  codes.

@panic Sqlite3  4 In _DEBUG mode - negative aFilePos value.
See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::Read(TInt64 aFilePos, TDes8& aDes)
	{
	__ASSERT_DEBUG(aFilePos >= 0, __SQLITEPANIC(EFBufPanicFilePos));
	__FILEBUF64_INVARIANT();
	aDes.SetLength(0);
	//1. The output buffer max len is 0
	if(aDes.MaxLength() == 0)
		{
		__FILEBUF64_INVARIANT();
		return KErrNone;	
		}
	//2. Initialize the "iFileSize" if it is not initialized yet
	TInt err = DoFileSize();
	if(err != KErrNone)
		{
		__FILEBUF64_INVARIANT();
		return err;	
		}
	//3. Too big "read" request - read directly from the file
	TInt len = aDes.MaxLength();
	if(len > iCapacity)
		{
		if((aFilePos + len) > iFilePos && aFilePos < (iFilePos + iLength))
			{//Write the pending data if the iDirty flag is set, otherwise preserve the buffer content.
			err = DoFileWrite1(aFilePos);
			}
		if(err == KErrNone)
			{
			err = iFile.Read(aFilePos, aDes);
			SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_READ1, "FBuf;0x%X;RFileBuf64::Read;TooBigRq;iFileSize=%lld;aFilePos=%lld;len=%d;err=%d", (TUint)this, iFileSize, aFilePos, len, err));
			}
		__FILEBUF64_INVARIANT();
		return err;
		}
	//4. The requested data size is smaller than the buffer capacity
	TUint8* outptr = const_cast <TUint8*> (aDes.Ptr());
	while(len > 0 && err == KErrNone && aFilePos < iFileSize)
		{
		//1. If part or all of the data is in the buffer - copy the data to the target location
		if(aFilePos >= iFilePos && aFilePos < (iFilePos + iLength))
			{
			TInt blocklen = Min(len, (iFilePos + iLength - aFilePos));
			outptr = Mem::Copy(outptr, iBase + (aFilePos - iFilePos), blocklen);
			len -= blocklen;
			aFilePos += blocklen;
			}
		//2. Perform a read-ahead operation
		else
			{
			//Write the pending data if the iDirty flag is set, otherwise preserve the buffer content.
			err = DoFileWrite1(aFilePos);
			if(err != KErrNone)
				{
				break;	
				}
			if(iNextReadFilePos != aFilePos)
				{//Guessed read ahead was wrong. Direct "file read" operation
				iNextReadFilePosHits = 0;
				TPtr8 ptr2(outptr, len);
				err = iFile.Read(aFilePos, ptr2);
				SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_READ2, "FBuf;0x%X;RFileBuf64::Read;Read;iFileSize=%lld;aFilePos=%lld;len=%d;err=%d", (TUint)this, iFileSize, aFilePos, len, err));
				if(err == KErrNone)
					{
					iNextReadFilePos = aFilePos + len;
					len -= ptr2.Length();
					}
				break;
				}
			//The guessed from the previous "file read" operation file pos is correct. Start reading-ahead.
			const TInt KMaxReadFilePosHits = 8;//The max read-ahead buffer size can be up to 2^8 times the iReadAheadSize
			if(iNextReadFilePosHits < KMaxReadFilePosHits)
				{
				++iNextReadFilePosHits;
				}
			TInt maxReadAhead = iReadAheadSize * (1 << iNextReadFilePosHits);
			TInt align = (aFilePos + len + maxReadAhead) & (iReadAheadSize - 1);
			TInt readahead = maxReadAhead - align;
			if(readahead < 0)
				{
				// if read-ahead doesn't cross block boundary do it all
				readahead = maxReadAhead;	
				}
			TPtr8 ptr(iBase, Min(iCapacity, (len + readahead)));
			err = iFile.Read(aFilePos, ptr);
			SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_READ3, "FBuf;0x%X;RFileBuf64::Read;ReadAhead;iFileSize=%lld;aFilePos=%lld;len=%d;err=%d", (TUint)this, iFileSize, aFilePos, ptr.MaxLength(), err));
			if(err == KErrNone)
				{
				iFilePos = aFilePos;
				iLength = ptr.Length();	
				iNextReadFilePos = iFilePos + iLength;
				if(iLength == 0)
					{
					break;	
					}
				}
			else
				{
				DoDiscard();	
				}
			}
		}
	aDes.SetLength(aDes.MaxLength() - len);
	__FILEBUF64_INVARIANT();
	return err;
	}
	
/**
Writes to the file at the specified offset (aFilePos) within the file.
If certain conditions are met, the data will be stored in the buffer - no call to the file server.

@param aFilePos The offset from the start of the file at which the first byte is written. 
                If a position beyond the end of the file is specified, then
                the write operation begins at the end of the file.
                If the position has been locked, then the write fails.
            
@param aData The descriptor from which binary data is written. The function writes 
             the entire contents of aData to the file.

@return KErrNone if successful, otherwise one of the other system-wide error  codes.

@panic Sqlite3  4 In _DEBUG mode - negative aFilePos value.
See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::Write(TInt64 aFilePos, const TDesC8& aData)
	{
	__ASSERT_DEBUG(aFilePos >= 0, __SQLITEPANIC(EFBufPanicFilePos));
	__FILEBUF64_INVARIANT();
	if(aData.Length() == 0)
		{
		__FILEBUF64_INVARIANT();
		return KErrNone;	
		}
	TInt err = DoFileSize();
	if(err != KErrNone)
		{
		__FILEBUF64_INVARIANT();
		return err;	
		}
	DoDiscardBufferedReadData();
	const TUint8* data = aData.Ptr();
	for(TInt len = aData.Length(); len > 0 && err == KErrNone;)
		{
		//1. The new write pos is before the buffered file pos
		if(aFilePos < iFilePos)
			{
			//If the new data sticks to/overlapps the old data and there is room in the buffer to move the old data 
			//toward the end, then the new data can be copied at the beginning of the buffer.
			if((aFilePos + len) >= iFilePos && (iFilePos - aFilePos) <= (iCapacity - iLength))
				{
				(void)Mem::Copy(iBase + (iFilePos - aFilePos), iBase, iLength);	//Make room - move the existing data toward the end
				(void)Mem::Copy(iBase, data, len);								//of the buffer. Stick the new data to the old data
				iLength += (iFilePos - aFilePos);
				iFilePos = aFilePos;										//The new file pos is associated with the buffer
				iFileSize = Max(iFileSize, (iFilePos + iLength));
				len = 0;													//No more new data
				iDirty = ETrue;	
				}
			else
			//The "aFilePos" is too far before the "iFilePos". Write the buffer and associate the new pos with the buffer
				{
				err = DoFileWrite2(aFilePos);
				}
			}
		//2. The new write pos is after the associated file pos + the data length.
		else if(aFilePos > (iFilePos + iLength))
			{
			if(aFilePos > iFileSize)											//Beyond the end of the file
				{
				if((iFilePos + iLength) == iFileSize && (aFilePos - iFilePos) < iCapacity)	
					{															//but within the buffer => extend the file with zeros.
					Mem::FillZ(iBase + iLength, aFilePos - iFilePos - iLength);
					iLength = aFilePos - iFilePos;
					iFileSize = Max(iFileSize, (iFilePos + iLength));
					iDirty = ETrue;	
					}
				else									
				//Beyond the end of the file and not in the buffer - set file size.
					{
					err = DoSetFileSize(aFilePos);
					}
				}
			else										
			//Within the file, not in the buffer - write the buffer and associate the new file pos with the buffer
				{
				err = DoFileWrite2(aFilePos);
				}
			}
		//3. The new write pos is in the buffer, but the data length is too big
		//   (For SQLite is OK, otherwise the whole block must be written to the file)
		//4. The new write pos is in the buffer, the data entirely fits in the buffer
		else
			{
			if(iCapacity == iLength)			//The buffer is full. Write the buffer and associate the new file pos
				{
				err = DoFileWrite2(aFilePos);
				}
			if(err == KErrNone)
				{
				TInt amount = Min(len, (iCapacity - (aFilePos - iFilePos)));
				const TUint8* end = Mem::Copy(iBase + (aFilePos - iFilePos), data, amount);
				iLength = Max(iLength, (end - iBase));
				iFileSize = Max(iFileSize, (iFilePos + iLength));
				len -= amount;
				data += amount;
				aFilePos += amount;
				iDirty = ETrue;	
				}
			}
		}
	__FILEBUF64_INVARIANT();
	return err;
	}
	
/**
Gets the current file size.

@param aFileSize On return, the size of the file in bytes.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::Size(TInt64& aFileSize)
	{
	__FILEBUF64_INVARIANT();
	TInt err = DoFileSize();
	if(err == KErrNone)
		{
		aFileSize = iFileSize;
		}
	__FILEBUF64_INVARIANT();
	return err;
	}

/**
Sets the file size.

If the size of the file is reduced, data may be lost from the end of the file.

Note:

1. The current file position remains unchanged unless SetSize() reduces the size 
   of the file in such a way that the current file position is now beyond
   the end of the file. In this case, the current file position is set to
   the end of file. 

2. If the file was not opened for writing, an error is returned.

@param aFileSize The new size of the file, in bytes. This value must not be negative, otherwise the function raises a panic.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

@panic Sqlite3  5 In _DEBUG mode - negative aFileSize value.
See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::SetSize(TInt64 aFileSize)
	{
	__ASSERT_DEBUG(aFileSize >= 0, __SQLITEPANIC(EFBufPanicFileSize));
	__FILEBUF64_INVARIANT();
	return DoSetFileSize(aFileSize);
	}

/**
Locks a region within the file as defined by a range of bytes.

@see RFile64::Lock()

@param aFilePos Position in file from which to lock; this is the  offset from the beginning of the file.
@param aLength  Number of bytes to lock.

@return KErrNone if successful, otherwise one of the other  system-wide error 
        codes.
*/
TInt RFileBuf64::Lock(TInt64 aFilePos, TInt64 aLength) const
	{
	__FILEBUF64_INVARIANT();
	TInt err = iFile.Lock(aFilePos, aLength);
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_LOCK, "FBuf;0x%X;RFileBuf64::Lock;iFileSize=%lld;aFilePos=%lld;aLength=%lld;err=%d", (TUint)this, iFileSize, aFilePos, aLength, err));
	return err;
	}
	
/**
Unlocks a region within the file as defined by a range of bytes.

@see RFile64::UnLock()

@param aFilePos Position in file from which to unlock; this is the  offset from the beginning of the file.
@param aLength  Number of bytes to unlock.

@return KErrNone if successful, otherwise one of the other  system-wide error codes.
*/
TInt RFileBuf64::UnLock(TInt64 aFilePos, TInt64 aLength) const
	{
	__FILEBUF64_INVARIANT();
	TInt err = iFile.UnLock(aFilePos, aLength);
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_UNLOCK, "FBuf;0x%X;RFileBuf64::UnLock;iFileSize=%lld;aFilePos=%lld;aLength=%lld;err=%d", (TUint)this, iFileSize, aFilePos, aLength, err));
	return err;
	}

/**
Writes the pending data and then flushes the file.

Although RFileBuf64::Close() also flushes internal buffers, it is better
to call RFileBuf64::Flush() before the file is closed. This is because Close() returns no 
error information, so there is no way of telling whether the final data was 
written to the file successfully or not.

@param aFlush The caller should call this function with aResetCachedFileSize parameter set, 
			  when the buffer file is shared between threads.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::Flush(TBool aResetCachedFileSize /*= EFalse*/)
	{
	__FILEBUF64_INVARIANT();
	TInt err = DoFileFlush();
	if(aResetCachedFileSize && err == KErrNone)
		{//Unset the iFileSize to force the file buffers used by the other threads to re-read the file size.
		iFileSize = KFileSizeNotSet;
		}
	return err;
	}

/**
Gets information about the drive on which this file resides.
 
@param aDriveNumber On return, the drive number.

@param aDriveInfo   On return, contains information describing the drive
                    and the medium mounted on it. The value of TDriveInfo::iType
                    shows whether the drive contains media.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::Drive(TInt& aDriveNumber, TDriveInfo& aDriveInfo) const
	{
	__FILEBUF64_INVARIANT();
	TInt err = iFile.Drive(aDriveNumber, aDriveInfo);
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_DRIVE, "FBuf;0x%X;RFileBuf64::Drive;aDriveNumber=%d;driveAtt=0x%X;mediaAtt=0x%X;err=%d", (TUint)this, aDriveNumber, (TUint)aDriveInfo.iDriveAtt, (TUint)aDriveInfo.iMediaAtt, err));
	return err;
	}

/**
Initializes RFileBuf64 data members with their initial values.   
Allocates memory for the file buffer. 
 
@return KErrNone if successful, 
        KErrNoMemory out of memory;
*/
TInt RFileBuf64::DoPreInit()
    {
    DoDiscard();
    iReadAheadSize = RFileBuf64::KDefaultReadAheadSize;
    iBase = static_cast <TUint8*> (User::Alloc(iCapacity));
    return iBase ? KErrNone : KErrNoMemory; 
    }

/**
Performs post-initialization of the RFileBuf64 object.   
If aInitErr is not KErrNone, then the buffer memory will be released.

@param aInitErr The result of the performed before the call RFileBuf64 initialization.
*/
void RFileBuf64::DoPostInit(TInt aInitErr)
    {
    if(aInitErr != KErrNone)
        {
        User::Free(iBase);
        iBase = 0;
        }
    }

/**
Discards the content of the RFileBuf64 object returning it to the state as if it has just been created. 
*/
void RFileBuf64::DoDiscard()
	{
	iLength = 0;
	iFilePos = 0;
	iFileSize = KFileSizeNotSet;
	iDirty = EFalse;
	iNextReadFilePos = KNextReadFilePosNotSet;
	iNextReadFilePosHits = 0;
	}

/**
Gets the current file size. 
If iFileSize value is valid, then no call to the file server will be made.
Otherwise the file server will be called and the file size - stored (cached) in iFileSize data member for later use.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::DoFileSize()
	{
	__FILEBUF64_INVARIANT();
	if(iFileSize != KFileSizeNotSet)
		{
		__FILEBUF64_INVARIANT();
		return KErrNone;
		}
	TInt err = iFile.Size(iFileSize);
	SQLITE_TRACE_FBUF(OstTraceExt3(TRACE_INTERNALS, RFILEBUF64_DOFILESIZE, "FBuf;0x%X;RFileBuf64::DoFileSize;iFileSize=%lld;err=%d", (TUint)this, iFileSize, err));
	if(err != KErrNone)
		{
		DoDiscard();
		}
	__FILEBUF64_INVARIANT();
	return err;
	}

/**
Sets the file size.
If the buffer contains pending data, the data will be written to the file 
before the "set file size" operation, if certain conditions are met. 

@param aFileSize The new size of the file, in bytes. This value must not be negative, otherwise the function raises a panic.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

@panic Sqlite3  5 In _DEBUG mode - negative aFileSize value.
See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::DoSetFileSize(TInt64 aFileSize)
	{
	__ASSERT_DEBUG(aFileSize >= 0, __SQLITEPANIC(EFBufPanicFileSize));
	__FILEBUF64_INVARIANT();
	if(aFileSize < iFilePos)
		{
		iDirty = EFalse;
		iLength = 0;	
		}
	//If the new file size is "in" the buffer then change the "iLength"
	else if(aFileSize < (iFilePos + iLength))
		{
		iLength = aFileSize - iFilePos;
		}
	TInt err = iFile.SetSize(aFileSize);
	SQLITE_TRACE_FBUF(OstTraceExt4(TRACE_INTERNALS, RFILEBUF64_DOSETFILESIZE, "FBuf;0x%X;RFileBuf64::DoSetFileSize;iFileSize=%lld;aFileSize=%lld;err=%d", (TUint)this, iFileSize, aFileSize, err));
	if(err != KErrNone)
		{
		DoDiscard();
		}
	else
		{
		iFileSize = aFileSize;
		}
	__FILEBUF64_INVARIANT();
	return err;
	}

/**
Writes the pending data and flushes the file.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for possible panics that may occur when this method is called.

@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::DoFileFlush()
	{
	__FILEBUF64_INVARIANT();
	TInt err = DoFileWrite2();//Write the buffer if the iDirty flag is set. Do not preserve the buffer content and file pos.
	if(err != KErrNone)
		{
		__FILEBUF64_INVARIANT();
		return err;	
		}
	err = iFile.Flush();
	SQLITE_TRACE_FBUF(OstTraceExt3(TRACE_INTERNALS, RFILEBUF64_DOFILEFLUSH, "FBuf;0x%X;RFileBuf64::DoFileFlush;iFileSize=%lld;err=%d", (TUint)this, iFileSize, err));
	if(err != KErrNone)
		{
		DoDiscard();
		}
	iLength = 0;
	__FILEBUF64_INVARIANT();
	return err;
	}

/**
Writes the buffered data to the file if the iLength value is > 0.
If the file write operation extends the file, the iFileSize data member will be initialized with the new file size.
No changes occur in the other data member values.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.

@see RFileBuf64::DoFileWrite1()
@see RFileBuf64::DoFileWrite2()
@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::DoFileWrite()
	{
	__FILEBUF64_INVARIANT();
	if(iLength == 0)
		{
		__FILEBUF64_INVARIANT();
		return KErrNone;	
		}
	TPtrC8 data(iBase, iLength);		
	TInt err = iFile.Write(iFilePos, data);
	SQLITE_TRACE_FBUF(OstTraceExt5(TRACE_INTERNALS, RFILEBUF64_DOFILEWRITE, "FBuf;0x%X;RFileBuf64::DoFileWrite;iFileSize=%lld;iFilePos=%lld;iLength=%d;err=%d", (TUint)this, iFileSize, iFilePos, iLength, err));
	if(err == KErrNone)
		{
		iFileSize = Max(iFileSize, (iFilePos + iLength));
		}
	else
		{
		DoDiscard();
		}
	__FILEBUF64_INVARIANT();
	return err;
	}

/**
Writes the buffered data to the file if the iDirty flag is set.
If the iDirty flag is set and the file write operation was successful, the iFilePos will be initialized with
the aNewFilePos value, the iLength will be set to 0.
This method is called from RFileBuf64::Read(), where:
 - if the buffer contains cached writes (iDirty flag is set), the buffer has to be flushed and iFilePos initialized
   with aNewFilePos - the offset in the file where the next file read operation should start from;
 - if the buffer contains cached reads, then nothing happens, the buffer content will be kept;
The function resets the iDirty flag.

@param aNewFilePos If the buffer is successfully written to the file the iFilePos data member will be initialized with
				   the aNewFilePos value.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.
				   
@panic Sqlite3  4 In _DEBUG mode - negative aNewFilePos value.

@see RFileBuf64::Read()
@see RFileBuf64::DoFileWrite()
@see RFileBuf64::DoFileWrite2()
@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::DoFileWrite1(TInt64 aNewFilePos)
	{
	__ASSERT_DEBUG(aNewFilePos >= 0, __SQLITEPANIC(EFBufPanicFilePos));
	__FILEBUF64_INVARIANT();
	TInt err = KErrNone;
	if(iDirty)
		{
		err = DoFileWrite();
		if(err == KErrNone)	
			{
			iFilePos = aNewFilePos;
			iLength = 0;
			}
		}
	iDirty = EFalse;
	__FILEBUF64_INVARIANT();
	return err;		
	}

/*
Writes the buffered data to the file if the iDirty flag is set.
If the file write operation was successful or if the iDirty flag was not set, the iFilePos will be initialized with
the aNewFilePos value, the iLength will be set to 0.
This method is called from RFileBuf64::Write() an other RFileBuf64 methods (but not from RFileBuf64::Read()), where:
 - if the buffer contains cached writes (iDirty flag is set), the buffer has to be flushed and iFilePos initialized
   with aNewFilePos - the offset in the file for which the write data will be cached in the buffer;
 - if the buffer contains cached reads, then the buffer content will be destroyed, iFilePos initialized with aNewFilePos
   and iLength set to 0;
The function resets the iDirty flag.
The difference between RFileBuf64::DoFileWrite1() and RFileBuf64::DoFileWrite2() is:
 - RFileBuf64::DoFileWrite1() perserves the buffer content if iDirty is not set;
 - RFileBuf64::DoFileWrite2() always destroys the buffer content and initializes iFilePos;

@param aNewFilePos If the buffer is successfully written to the file the iFilePos data member will be initialized with
				   the aNewFilePos value.

@return KErrNone if successful, otherwise one of the other system-wide error codes.

See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.
				   
@panic Sqlite3  4 In _DEBUG mode - negative aNewFilePos value.

@see RFileBuf64::Write()
@see RFileBuf64::DoFileWrite()
@see RFileBuf64::DoFileWrite1()
@see RFileBuf64::Invariant()
*/
TInt RFileBuf64::DoFileWrite2(TInt64 aNewFilePos)
	{
	__ASSERT_DEBUG(aNewFilePos >= 0, __SQLITEPANIC(EFBufPanicFilePos));
	__FILEBUF64_INVARIANT();
	TInt err = KErrNone;
	if(iDirty)
		{
		err = DoFileWrite();
		}
	if(err == KErrNone)	
		{
		iFilePos = aNewFilePos;
		iLength = 0;
		}
	iDirty = EFalse;
	__FILEBUF64_INVARIANT();
	return err;
	}

/**
This function discards the buffer content if the buffer contains cached read data.
The function is called from RFileBuf64::Write(), because if the buffer contains cached read data,
they cannot be mixed with the cached write data.
Reason: for example the buffer contains 8Kb cached read data from file offset 0.
        The data write request is 10 bytes at offset 4000. The write data will be cached,
        because the buffer contains data from from this file area: [0..8192].
        The iDirty flag will be set. Later when RFileBuf64::Flush() is called, the whole
        8Kb buffer will be written. There is nothing wrong with that, the file content will be consistent.
        But from performance point of view: 8Kb written vs. 10 bytes written - that may badly impact the performance.

@see RFileBuf64::Write()

See RFileBuf64::Invariant() for other possible panics that may occur when this method is called.
*/
void RFileBuf64::DoDiscardBufferedReadData()
	{
	__FILEBUF64_INVARIANT();
	if(!iDirty && iLength > 0)
		{
		iLength = 0;
		iFilePos = 0;
		iNextReadFilePos = KNextReadFilePosNotSet;
		iNextReadFilePosHits = 0;
		}
	__FILEBUF64_INVARIANT();
	}

#ifdef _DEBUG

/**
RFileBuf64 invariant. Called in _DEBUG mode at the beginning and before the end of every RFileBuf64 method
(except the init/destroy methods).

@panic Sqlite3  11 In _DEBUG mode - null "this" pointer.
@panic Sqlite3   1 In _DEBUG mode - negative iCapacity value.
@panic Sqlite3   2 In _DEBUG mode - the buffer pointer is null (possible the buffer is not allocated or already destroyed).
@panic Sqlite3   3 In _DEBUG mode - invalid iLength value (negative or bigger than iCapacity).
@panic Sqlite3   4 In _DEBUG mode - negative iFilePos value.
@panic Sqlite3   5 In _DEBUG mode - set but negative iFileSize value.
@panic Sqlite3   6 In _DEBUG mode - null file handle (the RFile64 object is not created or already destroyed).
@panic Sqlite3  13 In _DEBUG mode - set but negative iNextReadFilePos value.
@panic Sqlite3  14 In _DEBUG mode - negative iNextReadFilePosHits value.
@panic Sqlite3  15 In _DEBUG mode - iReadAheadSize is negative or is not power of two.
*/
void RFileBuf64::Invariant() const
	{
	__ASSERT_DEBUG(this != 0, __SQLITEPANIC(EFBufPanicNullThis));
	__ASSERT_DEBUG(iCapacity > 0, __SQLITEPANIC(EFBufPanicCapacity));
	__ASSERT_DEBUG(iBase != 0, __SQLITEPANIC(EFBufPanicNullBuf));
	__ASSERT_DEBUG(iLength >= 0 && iLength <= iCapacity, __SQLITEPANIC(EFBufPanicBufLen));
	__ASSERT_DEBUG(iFilePos >= 0, __SQLITEPANIC(EFBufPanicFilePos));
	__ASSERT_DEBUG(iFileSize == KFileSizeNotSet || iFileSize >= 0, __SQLITEPANIC(EFBufPanicFileSize));
	__ASSERT_DEBUG(iFile.SubSessionHandle() != 0, __SQLITEPANIC(EFBufPanicFileHandle));
	__ASSERT_DEBUG(iNextReadFilePos == KNextReadFilePosNotSet || iNextReadFilePos >= 0, __SQLITEPANIC(EFBufPanicNextReadFilePos));
	__ASSERT_DEBUG(iNextReadFilePosHits >= 0, __SQLITEPANIC(EFBufPanicNextReadFilePosHits));
	__ASSERT_DEBUG(iReadAheadSize > 0 && (iReadAheadSize & (iReadAheadSize - 1)) == 0, __SQLITEPANIC(EFBufPanicFileBlockSize));
	}
	
#endif
