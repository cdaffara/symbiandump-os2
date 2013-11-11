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
//


#ifndef _ZIP_FILE_MEMBER_INPUT_STREAM_H_
#define _ZIP_FILE_MEMBER_INPUT_STREAM_H_

#include <zlib.h>
#include <localtypes.h>
#include <s32file.h>

class CZipFile;

/**
A RZipFileMemberReaderStream represents a input stream for compressed files in the archive. 
This is a friend class to CZipFile. In order to create a input stream for a file in the archive,
CZipFile will call its member function GetInputStreamL() by passing a pointer to 
RZipFileMemberReaderStream and the function will return the input stream for the file
Note: This class is actually a C class as it derives from CBase

@publishedAll
@released
*/
class RZipFileMemberReaderStream : public CBase
	{
friend class CZipFile;

public:
	
	IMPORT_C ~RZipFileMemberReaderStream();
	IMPORT_C TInt Read(TDes16& aDes, TInt aLength);
	
	/**
	Overload version of the RZipFileMemberReaderStream read method.
	On return, contains the data read from the stream buffer.
	
	@param aDes The target descriptor for the data read from the stream buffer
	@param aLength The maximum number of bytes to be read
	@return KErrNone If all bytes read successfully.
	@return	KErrCorrupt If reading fails.
	@return KErrEof If end of file is reached.
	@return ... Any one of the system-wide error codes for other errors.
	*/	
	virtual TInt Read(TDes8& aDes, TInt aLength);
	
	/**
	Overload version of the RZipFileMemberReaderStream readL method.
	On return, contains the data read from the stream buffer.
	
	@param aDes The target descriptor for the data read from the stream buffer
	@param aLength The maximum number of bytes to be read
	@leave KErrNone If all bytes read successfully.
	@leave KErrCorrupt If reading fails.
	@leave KErrEof If end of file is reached.
	@leave ... Any one of the system-wide error codes for other errors.
	*/
	virtual void ReadL(TDes16& aDes, TInt aLength);


	void Release();
	void Close();

private:
	RZipFileMemberReaderStream(CZipFile&, TUint32, TUint32, TUint32, TUint32);
	
	static RZipFileMemberReaderStream* NewL(CZipFile&, TUint32, TUint32, TUint32, TUint32);
	void RZipFileMemberReaderStream::ConstructL();
	
	virtual TInt Read(void);
	virtual TInt Read(TByte*, TUint32, TUint32*);

	TInt GetBytes(TByte*, TUint32, TUint32*);
	TInt GetCompressedBytes(void);
	TInt GetStoredBytes(TByte*, TUint32, TUint32*);
	
	void ReadL(TDes8&);
	void ReadL(TDes16&);
	void ReadL(TDes8&, TChar);
	void ReadL(TDes16&, TChar);
	void ReadL(TUint8*, TInt);
	void ReadL(TUint16*, TInt);
	void ReadL(RWriteStream&);
	void ReadL(RWriteStream&, TInt);
	void ReadL(TInt);

	TInt8 ReadInt8L();
	TInt16 ReadInt16L();
	TInt32 ReadInt32L();
	TUint8 ReadUint8L();
	TUint16 ReadUint16L();
	TUint32 ReadUint32L();

	TReal32 ReadReal32L() __SOFTFP;
	TReal64 ReadReal64L() __SOFTFP;

	void PushL();
	void Pop();
	void Attach(MStreamBuf*);
	void Detach();
	MStreamBuf* Source();


private:

	enum
		{
		kCompressedBytesSize = 1024
		};
		
private:
	
	CZipFile& 	iZipFile;
	
	/** The method for compressing file*/ 
	TUint32   	iCompressionMethod;
	
	z_stream  	iStream;
	
	/** The size of compressed file */
	TUint32     iCompressedSize;
	
	/** The size of uncompressed file */
	TUint32     iUncompressedSize;
	
	TUint32     iFileOffset;
	TUint32     iOffset;
	TBool       iDone;
	
	/** The number of bytes already read in uncompressed bytes*/
	TUint32     iBytesLength;
	
	/** To store compressed bytes read*/
	TByte       iCompressedBytes[kCompressedBytesSize];
	
	/** Not used in implementation*/
	TUint32     iCompressedBytesOffset;
	};

	
#endif /* !_ZIP_FILE_MEMBER_INPUT_STREAM_H_ */

