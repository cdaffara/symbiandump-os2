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

#include "OldEZGzip.h"

using namespace TOLDEZLIB;

const TUint8 EZGZipFile::ID1 = 31;
const TUint8 EZGZipFile::ID2 = 139;

/**
Constructor
*/
EXPORT_C TEZGZipHeader::TEZGZipHeader() : iId1(EZGZipFile::ID1), iId2(EZGZipFile::ID2), iCompressionMethod(8), iFlags(0), iTime(0), 
	iExtraFlags(0), iOs(255), iXlen(0), iExtra(NULL), iFname(NULL), iComment(NULL), iCrc(0)
	{
	
	}

/**
Destructor
*/
EXPORT_C TEZGZipHeader::~TEZGZipHeader()
	{
	delete iExtra;
	delete iFname;
	delete iComment;
	}

/**
Constructor
*/
EXPORT_C TEZGZipTrailer::TEZGZipTrailer() : iCrc32(0), iSize(0)
	{

	}

/**
Constructor

@param aCrc the CRC to use for archive checking
@param aSize the size of the trailer
*/
EXPORT_C TEZGZipTrailer::TEZGZipTrailer(TInt32 aCrc, TInt32 aSize) : iCrc32(aCrc), iSize(aSize)
	{

	}

//--------------------------------------------------------------------------------------------------------

/**
Read the zip header from the specified zip file into the TEZGZipHeader object

@param aFile the zip file to read from
@param aHeader the target header object
@leave KEZlibErrBadGZipHeader invalid zip header
@leave ... Any of the system wide error codes
*/
EXPORT_C void EZGZipFile::ReadHeaderL(RFile &aFile, TEZGZipHeader &aHeader)
	{
	TInt obligatoryData = sizeof(aHeader.iId1) + sizeof(aHeader.iId2) + sizeof(aHeader.iCompressionMethod) +
		sizeof(aHeader.iFlags) + sizeof(aHeader.iTime) + sizeof(aHeader.iExtraFlags) + sizeof(aHeader.iOs);

	TPtr8 des(&aHeader.iId1,0,obligatoryData);
	TInt err = aFile.Read(des);
	if (err != KErrNone || (des.Size() != obligatoryData))
		User::Leave(KEZlibErrBadGZipHeader);

	if (aHeader.iId1 != ID1 || aHeader.iId2 != ID2)
		User::Leave(KEZlibErrBadGZipHeader);
	
	if (aHeader.iFlags & (1 << EFExtra)) // then the extra bit is set
		{
		des.Set(REINTERPRET_CAST(TUint8 *,&aHeader.iXlen),0,sizeof(aHeader.iXlen));
		err = aFile.Read(des);
		if (err != KErrNone || des.Size() != sizeof(aHeader.iXlen) || aHeader.iXlen < 0)
			User::Leave(KEZlibErrBadGZipHeader);
		
		aHeader.iExtra = HBufC8::NewMaxL(aHeader.iXlen);
		TPtr8 des = aHeader.iExtra->Des();
		err = aFile.Read(des);
		if (err != KErrNone || des.Size() != aHeader.iXlen)
			User::Leave(KEZlibErrBadGZipHeader);
		}
	
	if (aHeader.iFlags & (1 << EFName)) // then read in filename
		ReadStringIntoDescriptorL(aFile,&aHeader.iFname);

	if (aHeader.iFlags & (1 << EFComment)) // then read in comment
		ReadStringIntoDescriptorL(aFile,&aHeader.iComment);
			
	if (aHeader.iFlags & (1 << EFHcrc))
		{
		des.Set(REINTERPRET_CAST(TUint8*,&aHeader.iCrc),0,sizeof(aHeader.iCrc));
		err = aFile.Read(des);
		if (err != KErrNone || des.Size() != sizeof(aHeader.iCrc))
			User::Leave(KEZlibErrBadGZipHeader);
		}
	}

/**
Write the zip header to the specified file

@param aFile the file to write to
@param aHeader the header object to write to the file
@leave ... Any of the system wide error codes
*/
EXPORT_C void EZGZipFile::WriteHeaderL(RFile &aFile, TEZGZipHeader &aHeader)
	{
	TInt obligatoryData = sizeof(aHeader.iId1) + sizeof(aHeader.iId2) + sizeof(aHeader.iCompressionMethod) +
		sizeof(aHeader.iFlags) + sizeof(aHeader.iTime) + sizeof(aHeader.iExtraFlags) + sizeof(aHeader.iOs);

	TPtrC8 des(&aHeader.iId1,obligatoryData);
	User::LeaveIfError(aFile.Write(des));
	TBuf8<1> null(1);
	null[0] = '\0';

	if (aHeader.iFlags & (1 << EFExtra)) // then the extra bit is set
		{
		des.Set(REINTERPRET_CAST(TUint8 *,&aHeader.iXlen),sizeof(aHeader.iXlen));
		User::LeaveIfError(aFile.Write(des));
		
		User::LeaveIfError(aFile.Write(*aHeader.iExtra));
		}
						
	if (aHeader.iFlags & (1 << EFName)) // then read in filename
		{
		User::LeaveIfError(aFile.Write(*aHeader.iFname));
		User::LeaveIfError(aFile.Write(null));
		}

	if (aHeader.iFlags & (1 << EFComment)) // then read in comment
		{
		User::LeaveIfError(aFile.Write(*aHeader.iComment));
		User::LeaveIfError(aFile.Write(null));
		}
	
	if (aHeader.iFlags & (1 << EFHcrc))
		{
		des.Set(REINTERPRET_CAST(TUint8*,&aHeader.iCrc),sizeof(aHeader.iCrc));
		User::LeaveIfError(aFile.Write(des));
		}
	}

void EZGZipFile::ReadStringIntoDescriptorL(RFile &aFile, HBufC8 **aDes)
	{
	TInt i, err;
	CArrayFixFlat<TUint8> *bytes = new (ELeave) CArrayFixFlat<TUint8>(16);
	CleanupStack::PushL(bytes);
	TBuf8<1> ch;
	while (((err = aFile.Read(ch)) == KErrNone) && ch[0] != '\0')
		bytes->AppendL(*ch.Ptr());
	
	if (err != KErrNone)
		User::Leave(KEZlibErrBadGZipHeader);
	
	*aDes = HBufC8::NewMaxL(bytes->Count());
	
	for (i = 0; i < bytes->Count(); i++)
		(*aDes)->Des()[i] = (*bytes)[i];
	
	CleanupStack::PopAndDestroy(); // delete bytes
	}

/**
Read the zip trailer from the specified zip file into the TEZGZipTrailer object

@param aFile the zip file to read from
@param aTrailer the target trailer object
@leave KEZlibErrBadGZipTrailer invalid zip trailer
@leave ... Any of the system wide error codes
*/
EXPORT_C void EZGZipFile::ReadTrailerL(RFile &aFile, TEZGZipTrailer &aTrailer)
	{
	TPtr8 des(REINTERPRET_CAST(TUint8*,&aTrailer.iCrc32),0,sizeof(TEZGZipTrailer));

	TInt err = aFile.Read(des);
	if (err != KErrNone || des.Size() != sizeof(TEZGZipTrailer))
		User::Leave(KEZlibErrBadGZipTrailer);
	}

/**
Write the zip trailer to the specified file

@param aFile the file to write to
@param aTrailer the trailer object to write to the file
@leave ... Any of the system wide error codes
*/
EXPORT_C void EZGZipFile::WriteTrailerL(RFile &aFile, TEZGZipTrailer &aTrailer)
	{
	TPtrC8 des(REINTERPRET_CAST(TUint8*,&aTrailer.iCrc32),sizeof(TEZGZipTrailer));
	User::LeaveIfError(aFile.Write(des));
	}

/**
Find the zip trailer within the specified file, and read it into the TEZGZipTrailer object

@param aRfs file server session
@param aFname the file to read from
@param aTrailer the target trailer object
@leave KEZlibErrBadGZipHeader Invalid zip header
@leave ... Any of the system wide error codes
*/
EXPORT_C void EZGZipFile::LocateAndReadTrailerL(RFs &aRfs, const TDesC &aFname, TEZGZipTrailer &aTrailer)
	{
	TInt fileSize;

	RFile file;

	User::LeaveIfError(file.Open(aRfs,aFname,EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(file);

	TUint8 magic[2];
	TPtr8 des(magic,0,sizeof(TUint8) * 2);
	User::LeaveIfError(file.Read(des));
	if (magic[0] != ID1 || magic[1] != ID2)
		User::Leave(KEZlibErrBadGZipHeader);

	User::LeaveIfError(file.Size(fileSize));
	TInt sizePos = fileSize - (sizeof (TInt32) * 2);
	User::LeaveIfError(file.Seek(ESeekStart,sizePos));

	des.Set(REINTERPRET_CAST(TUint8 *,&aTrailer),0,sizeof(TInt32)*2);
	
	User::LeaveIfError(file.Read(des));

	CleanupStack::PopAndDestroy();
	}

/**
@deprecated Interface is deprecated because it is unsafe as it may leave. It is available for backward compatibility reasons only.
@see EZGZipFile::IsGzipFileL
*/
EXPORT_C TBool EZGZipFile::IsGzipFile(RFs &aRfs, const TDesC &aFname)
	{
	TBool retBool = true;
	TRAPD(errCode, retBool = IsGzipFileL(aRfs, aFname));
	if(errCode != KErrNone)
		{
		retBool = false;
		}
	return retBool;
	}

/**
Determine if the given file is a valid zip file

@param aRfs file server session
@param aFname name of the file to check
@leave ... Any of the system wide error codes
@return ETrue if the file is valid zip file, EFalse otherwise
*/
EXPORT_C TBool EZGZipFile::IsGzipFileL(RFs &aRfs, const TDesC &aFname)
	{
	TUint8 ids[2];
	RFile file;

	User::LeaveIfError(file.Open(aRfs,aFname,EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(file);

	TPtr8 des(ids,0,sizeof(TUint8) * 2);
	
	User::LeaveIfError(file.Read(des));
	CleanupStack::PopAndDestroy();
	return (ids[0] == ID1 && ids[1] == ID2);
	}

//--------------------------------------------------------------------------------------------------------


CEZFileToGzipBM::CEZFileToGzipBM(RFile &aInput, RFile &aOutput) : CEZFileBufferManager(aInput,aOutput)
	{
	iCrc = crc32(iCrc,NULL,0);
	}


CEZFileToGzipBM* CEZFileToGzipBM::NewLC(RFile &aInput, RFile &aOutput, TInt aBufferSize)
	{
	CEZFileToGzipBM *bm = new (ELeave) CEZFileToGzipBM(aInput,aOutput);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	return bm;
	}

CEZFileToGzipBM* CEZFileToGzipBM::NewL(RFile &aInput, RFile &aOutput, TInt aBufferSize)
	{
	CEZFileToGzipBM *bm = new (ELeave) CEZFileToGzipBM(aInput,aOutput);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return bm;
	}

void CEZFileToGzipBM::NeedInputL(CEZZStream &aZStream)
	{
	CEZFileBufferManager::NeedInputL(aZStream);
	iCrc = crc32(iCrc,iInputDescriptor.Ptr(),iInputDescriptor.Size());
	}

void CEZFileToGzipBM::InitializeL(CEZZStream &aZStream)
	{
	CEZFileBufferManager::InitializeL(aZStream);
	iCrc = crc32(iCrc,iInputDescriptor.Ptr(),iInputDescriptor.Size());
	}

//--------------------------------------------------------------------------------------------------------


CEZGzipToFileBM::CEZGzipToFileBM(RFile &aInput, RFile &aOutput) : CEZFileBufferManager(aInput,aOutput)
	{
	iCrc = crc32(iCrc,NULL,0);
	}

CEZGzipToFileBM* CEZGzipToFileBM::NewLC(RFile &aInput, RFile &aOutput, TInt aBufferSize)
	{
	CEZGzipToFileBM *bm = new (ELeave) CEZGzipToFileBM(aInput,aOutput);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	return bm;
	}

CEZGzipToFileBM* CEZGzipToFileBM::NewL(RFile &aInput, RFile &aOutput, TInt aBufferSize)
	{
	CEZGzipToFileBM *bm = new (ELeave) CEZGzipToFileBM(aInput,aOutput);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return bm;
	}

void CEZGzipToFileBM::NeedOutputL(CEZZStream &aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	iCrc = crc32(iCrc,od.Ptr(),od.Size());
	CEZFileBufferManager::NeedOutputL(aZStream);
	}

void CEZGzipToFileBM::FinalizeL(CEZZStream &aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	iCrc = crc32(iCrc,od.Ptr(),od.Size());
	CEZFileBufferManager::FinalizeL(aZStream);
	}


//--------------------------------------------------------------------------------------------------------
CEZGZipToFile::CEZGZipToFile() : iDecompressor(NULL), iBufferManager(NULL)
	{

	}

CEZGZipToFile::~CEZGZipToFile()
	{
	delete iDecompressor;
	delete iBufferManager;
	iGZipFile.Close();
	}

/**
Creates a new CEZGZipToFile object and leaves it on the CleanupStack

@param aRfs open file server session
@param aGzFileName name of the file to be de-compressed
@param aOutput the target file to hold the un-compressed data
@param aBufferSize required size of buffers
@return a pointer to the new CEZGZipToFile object, left on the CleanupStack
*/
EXPORT_C CEZGZipToFile* CEZGZipToFile::NewLC(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize)
	{
	CEZGZipToFile* dec = new (ELeave) CEZGZipToFile;
	CleanupStack::PushL(dec);
	dec->ConstructL(aRfs,aGzFileName,aOutput,aBufferSize);
	return dec;
	}

/**
Creates a new CEZGZipToFile object

@param aRfs open file server session
@param aGzFileName name of the file to be de-compressed
@param aOutput the target file to hold the un-compressed data
@param aBufferSize required size of buffers
@return a pointer to the new CEZGZipToFile object
*/
EXPORT_C CEZGZipToFile* CEZGZipToFile::NewL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize)
	{
	CEZGZipToFile* dec = new (ELeave) CEZGZipToFile;
	CleanupStack::PushL(dec);
	dec->ConstructL(aRfs,aGzFileName,aOutput,aBufferSize);
	CleanupStack::Pop();
	return dec;
	}

/**
Quits the current de-compression operation and restarts with the specified arguments

@param aRfs open file server session
@param aGzFileName name of the file to be de-compressed
@param aOutput the target file to hold the un-compressed data
@param aBufferSize required size of buffers
@leave ... Any of the system wide error codes
*/
EXPORT_C void CEZGZipToFile::ResetL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize)
	{
	delete iBufferManager;
	InitialiseBufManL(aRfs,aGzFileName,aOutput,aBufferSize);
	iDecompressor->ResetL(*iBufferManager);
	}

/**
De-compresses the current zip file in stages.  The function needs to called again until the de-compression 
is finalised, in which case it will return EFalse - for example...

@code
while ( decompressor->InflateL() )
	{
	// No action required
	}
@endcode

@leave KEZlibErrBadGZipCrc Invalid CRC check
@leave ... Any of the system wide error codes
@return ETrue if the de-compression is not complete, and function must be called again
@return EFalse if the de-compression is finalised
*/
EXPORT_C TBool CEZGZipToFile::InflateL()
	{
	TBool keepGoing = iDecompressor->InflateL();

	if (!keepGoing)
		{
		if (iBufferManager->Crc() != iTrailer.iCrc32)
			User::Leave(KEZlibErrBadGZipCrc);
		iGZipFile.Close();
		}

	return keepGoing;
	}

void CEZGZipToFile::InitialiseBufManL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize)
	{
	EZGZipFile::LocateAndReadTrailerL(aRfs,aGzFileName,iTrailer);
	User::LeaveIfError(iGZipFile.Open(aRfs,aGzFileName,EFileStream | EFileRead | EFileShareAny));
	EZGZipFile::ReadHeaderL(iGZipFile,iHeader);
	iBufferManager = CEZGzipToFileBM::NewL(iGZipFile,aOutput,aBufferSize);
	}

void CEZGZipToFile::ConstructL(RFs &aRfs, const TDesC &aGzFileName, RFile &aOutput, TInt aBufferSize)
	{
	InitialiseBufManL(aRfs,aGzFileName,aOutput,aBufferSize);

	// this is a special zlib modification to stop it choking when it can't find the normal zlib stream header.

	iDecompressor = CEZDecompressor::NewL(*iBufferManager,-CEZDecompressor::EMaxWBits);
	}


//--------------------------------------------------------------------------------------------------------


CEZFileToGZip::CEZFileToGZip() : iCompressor(NULL), iBufferManager(NULL)
	{

	}

CEZFileToGZip::~CEZFileToGZip()
	{
	delete iCompressor;
	delete iBufferManager;
	iGZipFile.Close();
	}

/**
Creates a new CEZFileToGZip object and leaves it on the CleanupStack

@param aRfs open file server session
@param aGzFileName the name of the target zip file
@param aInput the file to compress
@param aBufferSize required size of buffers
@return a pointer to the new CEZFileToGZip object, left on the CleanupStack
*/
EXPORT_C CEZFileToGZip* CEZFileToGZip::NewLC(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize)
	{
	CEZFileToGZip* com = new (ELeave) CEZFileToGZip;
	CleanupStack::PushL(com);
	com->ConstructL(aRfs,aGzFileName,aInput,aBufferSize);
	return com;
	}

/**
Creates a new CEZFileToGZip object and leaves it on the CleanupStack

@param aRfs open file server session
@param aGzFileName the name of the target zip file
@param aInput the file to compress
@param aBufferSize required size of buffers
@return a pointer to the new CEZFileToGZip object, left on the CleanupStack
*/
EXPORT_C CEZFileToGZip* CEZFileToGZip::NewL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize)
	{
	CEZFileToGZip* com = new (ELeave) CEZFileToGZip;
	CleanupStack::PushL(com);
	com->ConstructL(aRfs,aGzFileName,aInput,aBufferSize);
	CleanupStack::Pop();
	return com;
	}

/**
Quits the current compression operation and restarts with the specified arguments

@param aRfs open file server session
@param aGzFileName the name of the target zip file
@param aInput the file to compress
@param aBufferSize required size of buffers
@leave ... Any of the system wide error codes
*/
EXPORT_C void CEZFileToGZip::ResetL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize)
	{
	delete iBufferManager;
	InitialiseBufManL(aRfs,aGzFileName,aInput,aBufferSize);
	iCompressor->ResetL(*iBufferManager);
	}

/**
Compresses the current file in stages.  The function needs to called again until the compression 
is finalised, in which case it will return EFalse - for example...

@code
while ( compressor->DeflateL() )
	{
	// No action required
	}
@endcode

@leave ... Any of the system wide error codes
@return ETrue if the compression is not complete, and function must be called again
@return EFalse if the compression is finalised
*/
EXPORT_C TBool CEZFileToGZip::DeflateL()
	{
	TBool keepgoing = iCompressor->DeflateL();
	if (!keepgoing)
		{
		TEZGZipTrailer trailer(iBufferManager->Crc(), iUncompressedDataSize);
		EZGZipFile::WriteTrailerL(iGZipFile, trailer);
		iGZipFile.Close();
		}

	return keepgoing;
	}

void CEZFileToGZip::InitialiseBufManL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize)
	{
	User::LeaveIfError(aInput.Size(iUncompressedDataSize));
	TInt err;

	err = iGZipFile.Create(aRfs, aGzFileName,EFileStream | EFileWrite | EFileShareExclusive);
	if (err == KErrAlreadyExists)
		User::LeaveIfError(iGZipFile.Open(aRfs,aGzFileName,EFileStream | EFileWrite | EFileShareExclusive));
	else 
		User::LeaveIfError(err);
	
	EZGZipFile::WriteHeaderL(iGZipFile,iHeader);
	iBufferManager = CEZFileToGzipBM::NewL(aInput,iGZipFile,aBufferSize);
	}

void CEZFileToGZip::ConstructL(RFs &aRfs, const TDesC &aGzFileName, RFile &aInput, TInt aBufferSize)
	{
	InitialiseBufManL(aRfs,aGzFileName,aInput,aBufferSize);

	// this is a special zlib modification to stop zlib writing out its normal zlib stream header.

	iCompressor = CEZCompressor::NewL(*iBufferManager,CEZCompressor::EDefaultCompression,-CEZCompressor::EMaxWBits);
	}
