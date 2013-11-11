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

#ifndef __GZIP_H__
#define __GZIP_H__

#include <ezdecompressor.h>
#include <ezcompressor.h>
#include <ezfilebuffer.h>

/**
The TEZGZipHeader class encapsulates a zip file header, which is written to the start of the zip
file to store various settings of the archive

@publishedAll
@released
*/
class TEZGZipHeader
	{

public:
	IMPORT_C TEZGZipHeader();
	IMPORT_C ~TEZGZipHeader();
	
public:
	TUint8 iId1;
	TUint8 iId2;
	TUint8 iCompressionMethod;
	TUint8 iFlags;
	TInt32 iTime;
	TUint8 iExtraFlags;
	TUint8 iOs;
	TInt16 iXlen;
	HBufC8* iExtra;
	HBufC8* iFname;
	HBufC8* iComment;
	TInt16 iCrc;
	};

/**
The TEZGZipTrailer class encapsulates a zip file trailer, which uses a CRC (cyclic redundancy check) to 
confirm the validity of the unpacked archive

@publishedAll
@released
*/
class TEZGZipTrailer
	{
public:
	IMPORT_C TEZGZipTrailer();
	IMPORT_C TEZGZipTrailer(TInt32 aCrc, TInt32 aSize);
public:
	TInt32 iCrc32;
	TInt32 iSize;
	};

/**
The EZGZipFile class handles writing / reading of headers and trailers to / from zip files

@publishedAll
@released
*/
class EZGZipFile
	{
public:
	
	/** Zip file error codes */	
	enum { 	ENotGZipFile = KEZlibErrNotGZipFile, 					
			EInvalidCompressionMethod = KEZlibErrInvalidCompression,
			EBadGZipHeader= KEZlibErrBadGZipHeader, 				
			EBadGZipTrailer = KEZlibErrBadGZipTrailer, 				
			EBadGZipCrc = KEZlibErrBadGZipCrc };					
	/** Flags to determine which part of the header / trailer is being written */			
	enum { EFText = 0, EFHcrc = 1, EFExtra = 2, EFName = 3, EFComment = 4};

public:
	IMPORT_C static void ReadHeaderL(RFile &aFile, TEZGZipHeader &aHeader);
	IMPORT_C static void WriteHeaderL(RFile &aFile, TEZGZipHeader &aHeader);
	IMPORT_C static void ReadTrailerL(RFile &aFile, TEZGZipTrailer &aTrailer);
	IMPORT_C static void WriteTrailerL(RFile &aFile, TEZGZipTrailer &aTrailer);
	IMPORT_C static void LocateAndReadTrailerL(RFs &aRfs, const TDesC &aFname, TEZGZipTrailer &aTrailer);
	IMPORT_C static TBool IsGzipFile(RFs &aRfs, const TDesC &aFname);
	IMPORT_C static TBool IsGzipFileL(RFs &aRfs, const TDesC &aFname);

private:
	static void ReadStringIntoDescriptorL(RFile &aFile, HBufC8 **aDes);

public:
	/** First of the header ID pair */
	static const TUint8 ID1;
	/** Second of the header ID pair */	
	static const TUint8 ID2;
	};

/**
The CEZFileToGzipBM manages the input and output buffers for compression

@publishedAll
@released
*/
NONSHARABLE_CLASS(CEZFileToGzipBM) : public CEZFileBufferManager
	{
public:

	static CEZFileToGzipBM* NewLC(RFile &aInput, RFile &aOutput, TInt aBufferSize);
	static CEZFileToGzipBM* NewL(RFile &aInput, RFile &aOutput, TInt aBufferSize);

	/**
	Initialise the stream with input and output buffers and starts reading

	@param aZStream the stream to initialise
	*/
	virtual void InitializeL(CEZZStream &aZStream);
	
	/**
	Set the stream's input buffer and starts reading
	
	@param aZStream the steam whose input buffer to set
	*/	
	virtual void NeedInputL(CEZZStream &aZStream);
	
	/**
	Return the CRC - used for checking validity of the archive

	@return the CRC value
	*/	
	TInt32 Crc() const { return iCrc; } ;

private:
	CEZFileToGzipBM(RFile &aInput, RFile &aOutput);
private:
	TInt32 iCrc;
	};

/**
The CEZGzipToFileBM class manages the input and output buffers for de-compression

@publishedAll
@released
*/
NONSHARABLE_CLASS(CEZGzipToFileBM) : public CEZFileBufferManager
	{
public:

	static CEZGzipToFileBM* NewLC(RFile &aInput, RFile &aOutput, TInt aBufferSize);
	static CEZGzipToFileBM* NewL(RFile &aInput, RFile &aOutput, TInt aBufferSize);

	/**
	Finish writing to the stream

	@param aZStream the stream to complete writing to
	*/	
	virtual void FinalizeL(CEZZStream &aZStream);
	
	/**
	Set the stream's output buffer and start writing

	@param aZStream the steam whose output buffer to set
	*/	
	virtual void NeedOutputL(CEZZStream &aZStream);
	
	/**
	Return the CRC - used for checking validity of the archive

	@return the CRC value
	*/	
	TInt32 Crc() const { return iCrc; } ;

private:
	CEZGzipToFileBM(RFile &aInput, RFile &aOutput);
private:
	TInt32 iCrc;
	};


/**
A CEZGZipToFile object allows de-compression of a compressed file.
The name of the compressed file is passed into the constructor along with a target file 
to contain the uncompressed data.  The file is uncompressed by calling the InflateL() function.

@publishedAll
@released
*/
class CEZGZipToFile : public CBase
	{
public:
	~CEZGZipToFile();

	IMPORT_C static CEZGZipToFile* NewLC(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize = 0x8000);
	IMPORT_C static CEZGZipToFile* NewL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize = 0x8000);

	IMPORT_C void ResetL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize = 0x8000);

	IMPORT_C TBool InflateL();

private:
	CEZGZipToFile();
	void ConstructL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize);
	void InitialiseBufManL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize);
private:
	CEZDecompressor *iDecompressor;
	CEZGzipToFileBM *iBufferManager;
	TEZGZipTrailer iTrailer;
	RFile iGZipFile;
	TEZGZipHeader iHeader;
	};

/**
A CEZFileToGZip object allows compression of an uncompressed file to a zip file.
The uncompressed source file is passed into the constructor along with the name of the target
zip file.  The file is compressed by calling the DeflateL() function.

@publishedAll
@released
*/
class CEZFileToGZip : public CBase
	{
public:
	~CEZFileToGZip();

	IMPORT_C static CEZFileToGZip* NewLC(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize = 0x8000);
	IMPORT_C static CEZFileToGZip* NewL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize = 0x8000);

	IMPORT_C void ResetL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize = 0x8000);

	IMPORT_C TBool DeflateL();

private:
	CEZFileToGZip();
	void ConstructL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize);
	void InitialiseBufManL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize);
private:
	CEZCompressor *iCompressor;
	CEZFileToGzipBM *iBufferManager;
	TInt iUncompressedDataSize;
	RFile iGZipFile;
	TEZGZipHeader iHeader;
	};


#endif
