// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UF_STD.H"

EXPORT_C RFileReadStream::RFileReadStream(RFile& aFile,TInt aPos)
/** Constructs the read stream object, associates it with an already opened file, 
and prepares the stream for reading.

@param aFile A reference to the opened file.
@param aPos The offset into the file from where the stream is to be read. Defaults 
to zero. */
	{
	Attach(aFile,aPos);
	}

EXPORT_C TInt RFileReadStream::Open(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a stream and prepares the stream for reading.

The stream will be read from offset zero in the file.

@param aFs Handle to a file server session.
@param aName The full path name of the file.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by by the TFileMode type.
@return KErrNone, if successful; otherwise, one of the other system wide eror 
codes. 
@see TFileMode */
	{
	TInt r=iSource.Open(aFs,aName,aFileMode);
	if (r==KErrNone)
		RReadStream::Attach(&iSource);
	return r;
	}

EXPORT_C void RFileReadStream::Attach(RFile& aFile,TInt aPos)
/** Associates this stream with an already opened file and prepares the stream 
for reading.

@param aFile A reference to the opened file.
@param aPos The offset into the file from where the stream is to be read. Defaults 
to zero. */
	{
	iSource.Attach(aFile,aPos);
	RReadStream::Attach(&iSource);
	}

EXPORT_C RFileWriteStream::RFileWriteStream(RFile& aFile,TInt aPos)
/** Constructs the write stream object, associates it with an already opened file, 
and prepares the stream for writing.

@param aFile A reference to the opened file.
@param aPos The offset into the file where the stream is to be written. Defaults 
to zero. */
	{
	Attach(aFile,aPos);
	}

EXPORT_C TInt RFileWriteStream::Open(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a stream and prepares the stream for writing.

The stream will be written to offset zero in the file.

@param aFs Handle to a file server session.
@param aName The full path name of the file.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by by the TFileMode type.
@return KErrNone, if successful; otherwise, one of the other system wide error 
codes.
@see TFileMode */
	{
	TInt r=iSink.Open(aFs,aName,aFileMode);
	if (r==KErrNone)
		RWriteStream::Attach(&iSink);
	return r;
	}

EXPORT_C TInt RFileWriteStream::Create(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file, associates it with this stream, and prepares the stream 
for writing.

The stream will be written to offset zero in the file.

@param aFs Handle to a file server session.
@param aName The full path name of the new file. A file with this name must 
not already exist. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by by the TFileMode type.
@return KErrNone, if successful; otherwise, one of the other system wide error 
codes.
@see TFileMode */
	{
	TInt r=iSink.Create(aFs,aName,aFileMode);
	if (r==KErrNone)
		RWriteStream::Attach(&iSink);
	return r;
	}

EXPORT_C TInt RFileWriteStream::Replace(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file, associates the file with this stream, and prepares the 
stream for writing.

The file replaces any existing file of the same name.

The stream will be written to offset zero in the file.

@param aFs Handle to a file server session.
@param aName The full path name of the file.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by by the TFileMode type.
@return KErrNone, if successful; otherwise, one of the other system wide error 
codes.
@see TFileMode */
	{
	TInt r=iSink.Replace(aFs,aName,aFileMode);
	if (r==KErrNone)
		RWriteStream::Attach(&iSink);
	return r;
	}

EXPORT_C TInt RFileWriteStream::Temp(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates a temporary file, associates it with this stream, and prepares the 
stream for writing.

The new file is created in the specified path and a unique file name is generated 
by the file server.

Note that the store framework does not delete a temporary file after it is 
closed.

The stream will be written to offset zero in the file.

@param aFs Handle to a file server session.
@param aPath The path where the new file is to be created.
@param aName On return, contains the full path name of the new file.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by by the TFileMode type.
@return KErrNone, if successful; otherwise, one of the other system wide error 
codes.
@see TFileMode */
	{
	TInt r=iSink.Temp(aFs,aPath,aName,aFileMode);
	if (r==KErrNone)
		RWriteStream::Attach(&iSink);
	return r;
	}

EXPORT_C void RFileWriteStream::Attach(RFile& aFile,TInt aPos)
/** Associates this stream with an already opened file and prepares the stream 
for writing.

@param aFile A reference to the opened file.
@param aPos The offset into the file where the stream is to be written. Defaults 
to zero. */
	{
	iSink.Attach(aFile,aPos);
	RWriteStream::Attach(&iSink);
	}

