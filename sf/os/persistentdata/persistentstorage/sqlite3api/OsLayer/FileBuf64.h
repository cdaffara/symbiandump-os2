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
#ifndef FILEBUF64_H
#define FILEBUF64_H

#include <f32file.h>
#include <f32file64.h>

/**
The RFileBuf64 class provides buffered file read/write operations on a single RFile64 object.
RFileBuf64::Read() and RFileBuf64::Write() methods may boost the performance of the read/write file operations up to 30% 
if compared to their RFile64 equivalents. Especially this is true in the case of a set of sequential read or write
operations when the file offset of operation N+1 is the end file offset of operation N plus one byte.

The RFileBuf64 public methods declarations match the declarations of the most often used RFile64 public methods.
That makes the source code migration from RFile64 to RFileBuf64 easier (or from RFileBuf64 to RFile64).

The RFileBuf64 capabilities are similar to those of the RFileBuf class, except the fact that RFileBuf
uses 32-bit file offsets, RFileBuf64 uses 64-bit file offsets and RFileBuf64 provides optimised read-ahead operations,
crafted especially for the case when RFileBuf64 is used with a page based database management system (like SQLite).

Usage notes:
@code
	- an object of RFileBuf64 type must be defined first, specifying the max size (capacity) of the buffer as a parameter
	  of the constructor:
	  
	  	RFileBuf64 fbuf(<N>);//<N> is the buffer capacity in bytes
	  	
	- the second step is to initialize the just defined RFileBuf64 object by calling one of the "resource acquisition"
	  methods: RFileBuf64::Create(), RFileBuf64::Open() or RFileBuf64::Temp().
	  
	  In details, to create a file and access it through a RFileBuf64 object:
	  
	  	RFs fs;
	  	TInt err = fs.Connect();
        //check the error
	  	...
	  	RFileBuf64 fbuf(<N>);									//<N> is the buffer capacity in bytes
	  	err = fbuf.Create(fs, <file name>, <file mode>);
	  	//check the error
	  	
	  To open an existing file and access it through a RFileBuf64 object:

	  	RFs fs;
        TInt err = fs.Connect();
        //check the error
	  	...
	  	RFileBuf64 fbuf(<N>);									//<N> is the buffer capacity in bytes
	  	err = fbuf.Open(fs, <file name>, <file mode>);
	  	//check the error
	  
	  To create a temporary file and access it through a RFileBuf64 object:

	  	RFs fs;
        TInt err = fs.Connect();
        //check the error
	  	...
	  	RFileBuf64 fbuf(<N>);									//<N> is the buffer capacity in bytes
	  	err = fbuf.Temp(fs, <path>, <file name>, <file mode>);
	  	//check the error
	  
	- if the RFileBuf64 object is initialised successfully, now the public RFileBuf64 methods can be called to perform
	  requested operations on the file:
	  
	  	err = fbuf.Write(<file pos>, <data>);
	  	//check the error
	  	....
	  	err = fbuf.Read(<file pos>, <buf>);
	  	//check the error
	  	....
	  Note: do not forget to call RFileBuf64::Flush() at the moment when you want to ensure that the file data
	  	    (possibly buffered) is really written to the file.

	- The final step is to close the RFileBuf64 object and the corresponding file thus realising the used resouces:
	
		fbuf.Close();
	  
@endcode

Implementation notes: the current RFileBuf64 implementation is optimised for use by the SQLite OS porting layer.
	After investigation of SQLite file read/write operations it was found that buffering of
	two or more logical file writes into a single physical file write has a positive impact (as expected) on the performance 
	of the database write operations. But the picture is quite different for the file read operations. The database data is
	organised in pages with fixed size. After a database is created and set of insert/update/delete operations is performed 
	on it, after a while the database pages (identified by their numbers) are not sequential in the database file and using
	a read-ahead buffer with fixed size makes no sense because for each "page read" request of N bytes, the RFileBuf64 object
	will read up to K bytes, K >= N, where K is the read-ahead value in bytes. Since the "read page" requests in general are 
	not sequential (relatively to the page numbers), obviously the read-ahead data is wasted and the "read page" performance
	is negatively impacted. This observation is true in general except two cases:
		- sequential scan of a database table;
		- reading of a BLOB column;
	In these two cases it is likely that the table/blob data occupies pages with sequential numbers located in a continuous
	file area. Then if a read-ahead buffer is used that will have a positive impact on the "read page" performance, because
	the number of the read IPC calls to the file server will be reduced.
	In order to satisfy those two orthogonal requirements, the RFileBuf64 implementation uses a "read file offset prediction"
	algorithm:
		- The file buffer object is created with 0 read-ahead value;
		- After each "file read" operation the buffer implementation "guesses" what might be the file offset of the
		  next file read operation (it is possible because the database data is organised in pages with fixed size and
		  generally all "file read" requests are for reading a database page) and stores the value in one of its data members;
		- If the file offset of the next file read operation matches the "guessed" offset, the read-ahead value is changed from
		  0 to 1024 bytes (1024 bytes is the default database page size);
		- Every next match of the "guessed" file offset value doubles the read-ahead value. But the max read-ahead value is
		  capped by the capacity of the buffer;
		- If the file offset of the next file read operation does not match the "guessed" file offset, then the read-ahead
		  value is set back to 0;
	Shortly, depending of the nature of the file read requests, the RFileBuf64 object will dynamically change the read-ahead
	value thus minimising the amount of the wasted read data and improving the "file read" performance in general.

@see RFile64
@see RFileBuf

@internalComponent
*/
class RFileBuf64
	{
	enum {KDefaultReadAheadSize = 1024};//Default size in bytes of the read-ahead buffer
	
public:
	RFileBuf64(TInt aSize);

	TInt Create(RFs& aFs, const TDesC& aFileName, TUint aFileMode);
	TInt Open(RFs& aFs, const TDesC& aFileName, TUint aFileMode);
	TInt Temp(RFs& aFs, const TDesC& aPath, TFileName& aFileName, TUint aFileMode);
	void Close();
	TInt SetReadAheadSize(TInt aBlockSize, TInt aReadRecBufSize);

	TInt Read(TInt64 aFilePos, TDes8& aDes);
	TInt Write(TInt64 aFilePos, const TDesC8& aData);

	TInt Size(TInt64& aFileSize);
	TInt SetSize(TInt64 aFileSize);
	TInt Lock(TInt64 aFilePos, TInt64 aLength) const;
	TInt UnLock(TInt64 aFilePos, TInt64 aLength) const;
	TInt Flush(TBool aResetCachedFileSize = EFalse);

	TInt Drive(TInt& aDriveNumber, TDriveInfo& aDriveInfo) const;

private:
	void Invariant() const;
    TInt DoPreInit();
    void DoPostInit(TInt aInitErr);
	void DoDiscard();
	TInt DoFileSize();
	TInt DoSetFileSize(TInt64 aFileSize);
	TInt DoFileFlush();
	TInt DoFileWrite();
	TInt DoFileWrite1(TInt64 aNewFilePos);
	TInt DoFileWrite2(TInt64 aNewFilePos = 0LL);
	void DoDiscardBufferedReadData();
	
private:
	//Buffer related
	const TInt	iCapacity;				//The buffer size. Indicates how much data can be put in.
	TUint8*		iBase;					//Pointer to the beginning of the buffer.
	TInt		iLength;				//The length of the data currently held in the buffer.
	//File related
	TInt64		iFilePos;				//The file position associated with the beginning of the buffer.
	TInt64		iFileSize;				//The file size.
	RFile64		iFile;					//The file object.
	//Read-ahead related
	TBool		iDirty;					//The buffer contains pending data to be written to the file
	TInt64		iNextReadFilePos;		//The guessed file position of the next "file read" operation
	TInt		iNextReadFilePosHits;	//How many times the guessed file position of the "file read" operation was correct
	TInt		iReadAheadSize;

	//Profiler related
#ifdef _SQLPROFILER
public:
	TInt		iFileReadCount;		//The number of the non-buffered file reads (RFile64::Read() calls).
	TInt64		iFileReadAmount;	//The amount of the data read from the file.
	TInt		iFileWriteCount;	//The number of the non-buffered file writes (RFile64::Write() calls).
	TInt64		iFileWriteAmount;	//The amount of the data written to the file.
	TInt		iFileSizeCount;		//The number of non-buffered RFile64::Size() calls.
	TInt		iFileSetSizeCount;	//The number of non-buffered RFile64::SetSize() calls.
	TInt		iFileFlushCount;	//The number of RFile64::Flush() calls.
#endif
	
	};

#endif//FILEBUF64_H
