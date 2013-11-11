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


#include <caf/content.h>
#include <caf/data.h>
#include <zipfile.h>

// ====================================================================
// CZipFile public exported interface
// ====================================================================

/**
Creates a new CZipFile object using the supplied file server session and 
a valid file handle. The caller must have sufficient rights to 
access the content of the zipfile, if encrypted/protected.

@param aFs File server session used for opening the zipfile
@param aFile File handle to be used for accessing the zipfile
@return CZipFile object associated with the zipfile if it succeeded
@leave KZipArchiveError If file cannot be accessed(invalid handle, corrupt file header, etc.)
@leave KZipFileIOError If file cannot be read
@leave KCentralDirectoryTrailerNotFound If zip file header doesn't contain information about files inside the archive
@leave KCentralDirectoryTrailerInvalid If the information about files inside the archive is corrupt
@leave KMultiDiskArchivesNotSupported If zipfile is a multi disk archive
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFile* CZipFile::NewL(RFs& aFs, RFile& aFile)
	{
	TFileName file;
	aFile.Name(file);
	CZipFile* zipFile = new(ELeave) CZipFile(aFs, file);
	CleanupStack::PushL(zipFile);
	zipFile->ConstructL(aFile);
	CleanupStack::Pop(zipFile);
	return zipFile;
	}
	
/**
Creates a new CZipFile object using the supplied file server session and 
file name. The caller must have sufficient capabilities to access the directory. 
The caller must also have sufficient rights to access the content of the 
zipfile, if encrypted/protected.

@param aFs File server session used for opening the zipfile
@param aFileName Name of the zipfile 
@return CZipFile object associated with the zipfile if it succeeded
@leave KZipArchiveError If file cannot be accessed(invalid handle, corrupt file header, etc.)
@leave KZipFileIOError If file cannot be read
@leave KCentralDirectoryTrailerNotFound If zip file header doesn't contain information about files inside the archive
@leave KCentralDirectoryTrailerInvalid If the information about files inside the archive is corrupt
@leave KMultiDiskArchivesNotSupported If zipfile is a multi disk archive. 
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFile* CZipFile::NewL(RFs& aFs, const TDesC& aFileName)
	{
	CZipFile* zipFile = new(ELeave) CZipFile(aFs, aFileName);
	CleanupStack::PushL(zipFile);
	zipFile->ConstructL(aFileName);
	CleanupStack::Pop(zipFile);
	return zipFile;
	}

/**
Destructor
*/
EXPORT_C CZipFile::~CZipFile()
	{
	DeleteMemberPointers();
	}

/** 
@deprecated in 7.0 
*/
EXPORT_C CZipFile::CZipFile(RFs& aFs, const TDesC& aFileName)
	: iFileName(aFileName), iFs(aFs)
	{
	}
 
/** 
@deprecated in 7.0 
*/	
EXPORT_C TInt CZipFile::OpenL(void)
	{
	if (!iMemberPointers)
		{
		ConstructL(iFileName);
		}
	return KErrNone;
	}
	
/** 
@deprecated in 7.0 
*/
EXPORT_C void CZipFile::Close(void)
	{
	DeleteMemberPointers();
	}

/**
Second phase of construction. Used by Rfile using NewL overload.

@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C void CZipFile::ConstructL(RFile& aFile)
	{
	// Use the full name derived from the session path
	ContentAccess::CContent* content = 
		ContentAccess::CContent::NewL(aFile);
	CleanupStack::PushL(content);
	iData = content->OpenContentL(ContentAccess::EPeek);

	// Parent content object no longer needed because we only need data
	CleanupStack::PopAndDestroy(content);

	// Seek to the end
	TInt length = 0;
	User::LeaveIfError(iData->Seek(ESeekEnd, length));
	iFileLength = length;

	TInt status;
	TUint32 offset;

	if ((status = FindCentralDirectoryTrailer(offset)) != KErrNone) 
		{
		DeleteMemberPointers();
		User::Leave(status);
		}

	if ((status = ReadCentralDirectoryTrailer(offset, iTrailer)) != KErrNone) 
		{
		DeleteMemberPointers();
		User::Leave(status);
		}

	if (iTrailer.iStartDiskNumber != iTrailer.iDiskNumber) 
		{
		DeleteMemberPointers();
		User::Leave(KMultiDiskArchivesNotSupported);
		}

	if ((iTrailer.iOffset + iTrailer.iSize) > iFileLength)
		{
		DeleteMemberPointers();
	    User::Leave(KCentralDirectoryTrailerInvalid);
		}

	if (LoadMemberPointersL() != KErrNone)
		{
		User::Leave(KZipFileIOError);
		}	
	}

/**
Second phase of construction. Used by filename using NewL overload

@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C void CZipFile::ConstructL(const TDesC& aFileName)
	{
	TInt status;
	TUint32 offset;
	
	TRAP(status, OpenFileL(aFileName));
	if (status)
		{
		User::Leave(KZipArchiveError);
		}
	else
	if ((status = FindCentralDirectoryTrailer(offset)) != KErrNone) 
		{
		DeleteMemberPointers();
		User::Leave(status);
		}
	else 
	if ((status = ReadCentralDirectoryTrailer(offset, iTrailer)) != KErrNone) 
		{
		DeleteMemberPointers();
		User::Leave(status);
		}
	else 
	if (iTrailer.iStartDiskNumber != iTrailer.iDiskNumber) 
		{
		DeleteMemberPointers();
		User::Leave(KMultiDiskArchivesNotSupported);
		}
	else
	if ((iTrailer.iOffset > iFileLength) ||
	    ((iTrailer.iOffset + iTrailer.iSize) > iFileLength)) 
		{
		DeleteMemberPointers();
	    User::Leave(KCentralDirectoryTrailerInvalid);
		}
	else
	if (LoadMemberPointersL() != KErrNone)
		{
		User::Leave(KZipFileIOError);
		}	
	}

/**
Gets the size of the compressed data contained in the zip file in bytes
Each CZipFile object has a compressed and uncompressed size. This method will 
return the compressed size of a zip file.

@param aSize On return, the size of the compressed data in bytes
@return KErrNotReady If object hasn't been properly constructed
@return KErrCASizeNotDetermined	If size could not be determined 
@return ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C TInt CZipFile::Size(TInt& aSize) const
{
	TInt err = KErrNotReady;
	if (iData)
		{
		TRAP(err, iData->DataSizeL(aSize));
		}
	return err;
}

/**
Constructs and returns a CZipFileMember object which is used to access 
information about a compressed file contained in the CZipFile archive. 
The name of the file to be searched for in the zipfile is case-sensitive. 

@param aName The name of the file to be searched in the zipfile 
@return the pointer to CZipFileMember object
@return NULL if the file doesn't exist in the zipfile
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFileMember* CZipFile::MemberL(const TDesC& aName)
	{
	TLocalHeader		  header;
	const TMemberPointer* memberPointer;
	HBufC* localName = aName.AllocL();
	TInt loop = 0;
	while (loop < localName->Length())
		{
		if ((*localName)[loop] == '\\')
			{
			(localName->Des())[loop] = '/';
			}
		loop++;
		}
	
	memberPointer = FindMemberPointer(*localName, EFalse);
	if (memberPointer == NULL)
		{
		delete localName;
		return NULL;
		}
	else
	if (ReadLocalHeader(memberPointer->iLocalHeaderOffset, header) != KErrNone)
		{
		delete localName;
		return NULL;
		}
	else
		{
		CleanupStack::PushL(localName);
		CZipFileMember* thisMember = MakeMemberL(*memberPointer, header);
		CleanupStack::PopAndDestroy();		// localName
		return thisMember;
		}
	}

/**
Constructs and returns a CZipFileMember object which is used to access 
information about a compressed file contained in the CZipFile archive. 
The name of the file to be searched for in the zipfile is case-insensitive.

@param aName The name of the file to be searched in the zipfile 
@return A pointer to a member object of zip file
@return NULL If the file doesn't exist in the zipfile
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFileMember* CZipFile::CaseInsensitiveMemberL(const TDesC& aName)
{
	TLocalHeader		  header;
	const TMemberPointer* memberPointer;
	HBufC* localName = aName.AllocL();
	TInt loop=0;
	while (loop < localName->Length())
		{
		if ((*localName)[loop] == '\\')
			{
			(localName->Des())[loop] = '/';
			}
		loop++;
		}
	
	memberPointer = FindMemberPointer(*localName, ETrue);
	if (memberPointer == NULL)
		{
		delete localName;
		return NULL;
		}
	else
	if (ReadLocalHeader(memberPointer->iLocalHeaderOffset, header) != KErrNone)
		{
		delete localName;
		return NULL;
		}
	else
		{
		CleanupStack::PushL(localName);
		CZipFileMember* thisMember = MakeMemberL(*memberPointer, header);
		CleanupStack::PopAndDestroy();
		return thisMember;
		}

	}

/**
Constructs and returns a CZipFileMember object which is used to access 
information about a compressed file contained in the CZipFile archive. 
An exact match for the filename is searched for first. If a match is not found, 
a case-insensitive search is performed. If both filenames exist in the archive, 
the case-sensitive match will be returned.
 
@param aName The name of the file to be searched in the zipfile 
@return A pointer to a member object of zip file
@return NULL If the file doesn't exist in the zipfile
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFileMember* CZipFile::CaseSensitiveOrCaseInsensitiveMemberL(const TDesC& aName)
{
	CZipFileMember* member;
	member = MemberL(aName);
	if (member)
		{
		return member;
		}
	else
		{
		return CaseInsensitiveMemberL(aName);
		}
}

/**
Creates and returns the input stream for a file in the archive. Only files 
compressed with Stored or Deflated compression methods are supported.

@param aMember The compressed file in the archive
@param aStream On return, the stream to be used for reading the contents of the compressed file.  The caller owns this object and is responsible for deleting it.
@return KErrNone if successful
@return KCompressionMethodNotSupported if compression format unsupported 
@return ... Any one of the system-wide error codes for other errors.
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C TInt CZipFile::GetInputStreamL(const CZipFileMember* aMember, RZipFileMemberReaderStream*& aStream)
	{
	TUint32 compressionMethod;
	
	compressionMethod = aMember->iCompressionMethod;
	if ((compressionMethod != EStored) && (compressionMethod != EDeflated)) 
	    {
	    return KCompressionMethodNotSupported;
	    }
	aStream = RZipFileMemberReaderStream::NewL(
								*this,
							   aMember->iDataOffset,
							   aMember->iCompressedSize,
							   aMember->iUncompressedSize,
							   compressionMethod);
	return KErrNone;
	}
	


/**
Gets the iterator used for iterating through the files contained in the ZIP 
file. It is the caller's responsibility to release the iterator when finsihed.
 
@return Pointer to a newly allocated CZipFileMemberIterator object
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFileMemberIterator* CZipFile::GetMembersL()
	{
	return new (ELeave) CZipFileMemberIterator(this);
	}
	


// Implementation

/*
 * Find the 'end of central directory record'. This is at the 'end' of
 * the file, but since it is not a fixed length structure, we have to
 * hunt for it.
 *
 * We try assuming that the variable length section of the record is
 * empty, which usually appears to be the case.
 *
 * If this does not work we resort to 'walking backwards' through the
 * file looking for the signature bytes.
 *
 */
 
TInt CZipFile::FindCentralDirectoryTrailer(TUint32& offset)
{
	TBuf8<KSignatureLength> signature;
		
    if (iFileLength <= KCentralDirectoryTrailerFixedLength) 
		{
    	return KZipArchiveError;
		}
    // Try the obvious place first.Assuming that the comment (variable 
    // length section) is empty,try to find the signature at the offset.
    offset = iFileLength - KCentralDirectoryTrailerFixedLength;
	if (Seek(offset) != KErrNone) 
		{
    	return KZipFileIOError;
		}
	TInt err = iData->Read(signature);

	if ( err != KErrNone) 
		{
		return KZipFileIOError;
		}

	if ((signature[0] == 0x50) && 
		(signature[1] == 0x4b) &&
		(signature[2] == 0x05) &&
		(signature[3] == 0x06)) 
		{
		return KErrNone;
		}
	else 
		{
		// There must be some comments, hence the central directory 
		// record > 22 bytes.
		// This is a slow but fairly obvious way of searching 
		// backwards through the file starting from the offset.
		TUint EndOfTrailerSearch = 0; //Upto beginning of File

		if(iFileLength > KMaxTrailerSearchLength+KCentralDirectoryTrailerFixedLength)
			EndOfTrailerSearch = offset - KMaxTrailerSearchLength; //Upto Last 64K+22 bytes

		while (offset >= EndOfTrailerSearch) 
			{
			if (Seek(offset) != KErrNone)
				{
				return KZipFileIOError;
				}
			if (iData->Read(signature) != KErrNone)
				{
				return KZipFileIOError;
				}
			if ((signature[0] == 0x50) && 
				(signature[1] == 0x4b) &&
				(signature[2] == 0x05) &&
				(signature[3] == 0x06)) 
				{			
				return KErrNone;
				}
			--offset;
			}
		return KCentralDirectoryTrailerNotFound;
		}	
	}
	
TInt CZipFile::ReadCentralDirectoryTrailer(TUint32 offset, struct TCentralDirectoryTrailer& r )
{
    // Skip the signature		    
    if (Seek(offset + KSignatureLength) != KErrNone) 
    	{
    	return KZipFileIOError;
    	} 
	else
	if (Read(r.iDiskNumber) != KErrNone) 
		{
		return KZipFileIOError;
		}
	else
	if (Read(r.iStartDiskNumber)!= KErrNone) 
		{
		return KZipFileIOError;
		}
	else
	if (Read(r.iLocalEntryCount) != KErrNone) 
		{
		return KZipFileIOError;
		}
	else
	if (Read(r.iTotalEntryCount) != KErrNone) 
		{
		return KZipFileIOError;
		}
	else
	if (Read(r.iSize) != KErrNone) 
		{
		return KZipFileIOError;
		}
	else
	if (Read(r.iOffset) != KErrNone) 
		{
		return KZipFileIOError;
		}
	else
		{
		return KErrNone;
		}
}

TInt CZipFile::LoadMemberPointersL(void)
	{
	TCentralDirectoryHeader	header;
	TUint32					i;
	TUint32					memberPointerCount;
		
	if (Seek(iTrailer.iOffset) != KErrNone)
		{
		return KZipFileIOError;
		}
	memberPointerCount = iTrailer.iTotalEntryCount;
	iMemberPointers = new (ELeave) TMemberPointer[memberPointerCount];
	for (i = 0; i < memberPointerCount; i++)
		{
		iMemberPointers[i].iName = NULL;
		}
	CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(converter);
	TInt converterState = CCnvCharacterSetConverter::KStateDefault;
	for (i = 0; i < memberPointerCount; i++)
		{
		if (ReadCentralDirectoryHeaderL(header, iMemberPointers[i], converter, converterState) != KErrNone)
			{
			return KZipFileError;
			}
		}
	CleanupStack::PopAndDestroy(converter);
	return KErrNone;
	}
	
static void ConvertFileNameToUnicodeL(
						TDes16& aUnicode,
						const TDesC8& aForeign,
						const TUint16& aMadeBy,
						CCnvCharacterSetConverter* aConverter,
						TInt aConverterState,
						RFs aFs)
// Have to decide whether filename encoding is CP850 or CP1252. According to tec support
// at WinZip, if 'madeby' is set to DOS(0) then the encoding is CP850 and if it's set to
// NTFS (11) then it's CP1252.  However, if the MS Compressed Folders program was used
// to zip, then madeby is always set to NTFS and the encoding is always CP850 - the exact
// opposite. Because of this confusion, I have written a very basic decision algorithm
// based on the premise that filenames are likely to use alphabet-style extended
// characters (rather than box edges or punctuation etc.)
	{
	TInt len = aForeign.Length();
	TInt ANSIpoints = 0;
	TInt OEMpoints = 0;
	for (TInt i=0; i<len; i++)
		{
		if (aForeign[i] >= 128 && aForeign[i] <= 165)
			OEMpoints ++;
		if (aForeign[i] >= 192 && aForeign[i] <= 255)
			ANSIpoints ++;
		}
	if (ANSIpoints == OEMpoints)
		{
		if (aMadeBy>>8) //only interested in the upper byte
			ANSIpoints ++;
		else
			OEMpoints ++;
		}
	TInt unconvertibleChars = 0;

	CCnvCharacterSetConverter::TAvailability availabilty = CCnvCharacterSetConverter::EAvailable;
	if (ANSIpoints > OEMpoints)
		{
		// It's probably ANSI (CP1252)
		availabilty = aConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252,aFs);
		aConverter->ConvertToUnicode(aUnicode, aForeign, aConverterState, unconvertibleChars);
		
		}
	if (OEMpoints > ANSIpoints || unconvertibleChars)
		{
		// It's definitely OEM (CP850)
 		availabilty = aConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCP850,aFs);
		if(availabilty != CCnvCharacterSetConverter::EAvailable )
			{
			//if cp850 plugin is not available, use cp1252
			availabilty = aConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, aFs);
			if(availabilty != CCnvCharacterSetConverter::EAvailable)
				User::Leave (KErrNotSupported);
			}
		
		aConverter->ConvertToUnicode(aUnicode, aForeign, aConverterState);
		
		}
	}

TInt CZipFile::ReadCentralDirectoryHeaderL(
		 		   TCentralDirectoryHeader& aHeader, 
		 		   TMemberPointer&          aMemberPointer,
				   CCnvCharacterSetConverter* aConverter,
				   TInt aConverterState)
/*
As this function might be called many times and the request will 
eventually be translated to calls to server to read the data, 
so performance is the major issue. Try to minimize calls to server.
Read data in a chunk rather than member-by-member.
*/
	{
	TByte tmpHeader[KCentralDirectoryHeaderFixedLength];
 
	if (Read(tmpHeader,KCentralDirectoryHeaderFixedLength) != KErrNone)
		{
		return KZipFileIOError;
		}

	Mem::Copy(&aHeader.iSignature, &tmpHeader[0], 4);

	if (aHeader.iSignature != KCentralDirectoryHeaderSignature)
		{
		return KZipFileIOError;
		}

	Mem::Copy(&aHeader.iMadeBy, &tmpHeader[4], 2);
	Mem::Copy(&aHeader.iRequired, &tmpHeader[6], 2);
	Mem::Copy(&aHeader.iFlags, &tmpHeader[8], 2);
	Mem::Copy(&aHeader.iCompressionMethod, &tmpHeader[10], 2);
	Mem::Copy(&aHeader.iLastModifiedFileTime, &tmpHeader[12], 2);
	Mem::Copy(&aHeader.iLastModifiedFileDate, &tmpHeader[14], 2);
	Mem::Copy(&aHeader.iCRC32, &tmpHeader[16], 4);
	Mem::Copy(&aHeader.iCompressedSize, &tmpHeader[20], 4);
	Mem::Copy(&aHeader.iUncompressedSize, &tmpHeader[24], 4);
	Mem::Copy(&aHeader.iFileNameLength, &tmpHeader[28], 2);
	Mem::Copy(&aHeader.iExtraFieldLength, &tmpHeader[30], 2);
	Mem::Copy(&aHeader.iFileCommentLength, &tmpHeader[32], 2);
	Mem::Copy(&aHeader.iDiskNumberStart, &tmpHeader[34], 2);
	Mem::Copy(&aHeader.iInternalFileAttributes, &tmpHeader[36], 2);
	Mem::Copy(&aHeader.iExternalFileAttributes, &tmpHeader[38], 4);
	Mem::Copy(&aHeader.iLocalHeaderOffset, &tmpHeader[42], 4);

	aMemberPointer.iCRC32             = aHeader.iCRC32;
	aMemberPointer.iCompressedSize    = aHeader.iCompressedSize;
	aMemberPointer.iUncompressedSize  = aHeader.iUncompressedSize;
	aMemberPointer.iLocalHeaderOffset = aHeader.iLocalHeaderOffset;
    aMemberPointer.iName = new(ELeave) TFileName;
	
	TBuf8<KMaxFileName> input;
	if (iData->Read(input, aHeader.iFileNameLength) != KErrNone)
		{
		return KZipFileIOError;
		}
	ConvertFileNameToUnicodeL(*aMemberPointer.iName, input, aHeader.iMadeBy, aConverter, aConverterState, iFs);
	
    // Ignore the remaining fields
	TInt pos;

	pos = aHeader.iExtraFieldLength;	
	if (pos != 0)
		{
		// Don't pass aHeader.iExtraFieldLength in place of pos
		// as the below function will update the content of that variable.
		// In this case, the function is used to ignore the data
		// by just moving the current file pointer location.
		if (iData->Seek(ESeekCurrent, pos) != KErrNone) 
			{
			return KZipFileIOError;
			}
		}

	pos = aHeader.iFileCommentLength;
	if (pos != 0)
		{
		// Don't pass aHeader.iFileCommentLength in place of pos
		// as the below function will update the content of that variable.
		// In this case, the function is used to ignore the data
		// by just moving the current file pointer location.
		if (iData->Seek(ESeekCurrent, pos) != KErrNone) 
			{
			return KZipFileIOError;
			}
		}

  	return  KErrNone;
}

TInt CZipFile::ReadLocalHeader(TUint32 aOffset, TLocalHeader& aHeader)
/*
As this function might be called many times and the request will 
eventually be translated to calls to server to read the data, 
so performance is the major issue. Try to minimize calls to server.
Read data in a chunk rather than member-by-member.
*/
	{
	TByte tmpHeader[KLocalHeaderFixedLength];

	if (Seek(aOffset) != KErrNone)
		{
		return KZipFileIOError;
		}
	if (Read(tmpHeader,KLocalHeaderFixedLength) != KErrNone)
		{
		return KZipFileIOError;
		}
	Mem::Copy(&aHeader.iSignature, &tmpHeader[0], 4);

	if (aHeader.iSignature != KLocalHeaderSignature)
		{
		return KLocalHeaderSignatureInvalid;
		}

	Mem::Copy(&aHeader.iVersionNeeded, &tmpHeader[4], 2);
	Mem::Copy(&aHeader.iFlags, &tmpHeader[6], 2);
	Mem::Copy(&aHeader.iCompressionMethod, &tmpHeader[8], 2);
	Mem::Copy(&aHeader.iLastModifiedFileTime, &tmpHeader[10], 2);
	Mem::Copy(&aHeader.iLastModifiedFileDate, &tmpHeader[12], 2);
	Mem::Copy(&aHeader.iCRC32, &tmpHeader[14], 4);
	Mem::Copy(&aHeader.iCompressedSize, &tmpHeader[18], 4);
	Mem::Copy(&aHeader.iUncompressedSize, &tmpHeader[22], 4);
	Mem::Copy(&aHeader.iFileNameLength, &tmpHeader[26], 2);
	Mem::Copy(&aHeader.iExtraFieldLength, &tmpHeader[28], 2);

	return  KErrNone;
	}
	
const CZipFile::TMemberPointer* CZipFile::FindMemberPointer(const TDesC& aName, TBool aCaseInsensitive)
	{
	for (TUint32 i = 0; i < iTrailer.iTotalEntryCount; i++)
		{
		if (aCaseInsensitive && (!aName.CompareF(*iMemberPointers[i].iName)))
			{
			return iMemberPointers + i;
			}
		else if (aName == *iMemberPointers[i].iName)
			{
			return iMemberPointers + i;
			}
		}
	return NULL;
	}

RZipFileMemberReaderStream* CZipFile::MakeInputStreamL(
							TUint32 aDataOffset, 
							TUint32 aCompressedSize, 
							TUint32 aUncompressedSize, 
							TUint32 aCompressionMethod)
	{
	return RZipFileMemberReaderStream::NewL(
						    *this,
							aDataOffset,
							aCompressedSize,
							aUncompressedSize, 
							aCompressionMethod);
	}

CZipFileMember* CZipFile::MakeMemberL(TInt aMemberIndex)
	{
	TLocalHeader    header;
	TMemberPointer*	memberPointer;
	
	if (aMemberIndex >= iTrailer.iTotalEntryCount)
		{
		return NULL;
		}
	memberPointer = iMemberPointers + aMemberIndex;
	if (ReadLocalHeader(memberPointer->iLocalHeaderOffset, header) != KErrNone)
		{
		return NULL;
		}
	else
		{
		return MakeMemberL(*memberPointer, header);
		}
	}
	
CZipFileMember* CZipFile::MakeMemberL(
                              const TMemberPointer& aMemberPointer, 
			                  const TLocalHeader&   aHeader)
	{
	CZipFileMember* member;
	
	member = new (ELeave) CZipFileMember;
	CleanupStack::PushL(member);
	member->iCRC32 = aMemberPointer.iCRC32;
	member->iCompressedSize = aMemberPointer.iCompressedSize;
	member->iCompressionMethod = aHeader.iCompressionMethod;
	member->iName = new (ELeave) TFileName(*aMemberPointer.iName);
	TInt loop=0;
	while (loop < member->iName->Length())
		{
		if ((*member->iName)[loop] == '/')
			{
			(*member->iName)[loop] = '\\';
			}
		loop++;
		}
	member->iUncompressedSize = aMemberPointer.iUncompressedSize;
	member->iDataOffset = aMemberPointer.iLocalHeaderOffset + 
		                  KLocalHeaderFixedLength + 
						  aHeader.iFileNameLength + 
						  aHeader.iExtraFieldLength;
	CleanupStack::Pop();
	return member;
	}

void CZipFile::DeleteMemberPointers()
	{
	if (iMemberPointers)
		{
		for (TUint32 i = 0; i < iTrailer.iTotalEntryCount; i++)
			{
			delete iMemberPointers[i].iName;
			}
		delete[] iMemberPointers;
		iMemberPointers = 0;
		}

	delete iData;
	iData = NULL;
	return;
	}

void CZipFile::OpenFileL(const TDesC& aFileName)
	{
	// We need to look at the session path of the filesystem passed
	// in to derive the fullpath of the file to open
	HBufC* sessionPath = HBufC::NewLC(KMaxFileName);
	TPtr ptr(sessionPath->Des());
	User::LeaveIfError(iFs.SessionPath(ptr));
	TParse parse;
	User::LeaveIfError(parse.Set(aFileName, sessionPath, NULL));
	
	// Use the full name derived from the session path
	ContentAccess::CContent* content = 
		ContentAccess::CContent::NewL(parse.FullName());
	CleanupStack::PushL(content);
	iData = content->OpenContentL(ContentAccess::EPeek);

	// Parent content object no longer needed because we only need data
	CleanupStack::PopAndDestroy(content);

	// Seek to the end
	TInt length = 0;
	User::LeaveIfError(iData->Seek(ESeekEnd, length));
	iFileLength = length;
	CleanupStack::PopAndDestroy(sessionPath);
	}

TInt CZipFile::Read(TUint16& aUs)
	{
	TPckgBuf<TUint16> temp(aUs);
	
	if (iData->Read(temp) != KErrNone)
		{
		return KZipFileIOError;
		}
	
	aUs=temp();
	return KErrNone;
	}
	
TInt CZipFile::Read(TUint32& aUl)
	{
	TPckgBuf<TUint32> temp;
	
	if (iData->Read(temp) != KErrNone)
		{
		return KZipFileIOError;
		}
	aUl=temp();
	return KErrNone;
	}

TInt CZipFile::Read(TByte* aBytes, TUint32 aLength)

	{
	TPtr8 ptr(aBytes, aLength);
	if(iData->Read(ptr, aLength))
		{
		return KZipFileIOError;
		}
	else
		{
		return KErrNone;
		}
	}
	
TInt CZipFile::Seek(TInt aOffset)
	{
	if (iData->Seek(ESeekStart, aOffset) < 0) 
		{
		return KZipFileIOError;
		}
	else
		{
		return KErrNone;
		}
	}

