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


#ifndef _ZIP_ARCHIVE_H_
#define _ZIP_ARCHIVE_H_

#include <localtypes.h>

/**
This class represents a zip archive

@publishedAll
@released
*/
class CZipArchive: public CBase
	{
public:

    enum 
		{	
		KZipArchiveError 					= -256,
	    KCentralDirectoryTrailerNotFound	= KZipArchiveError - 1,
	    KCentralDirectoryTrailerInvalid		= KZipArchiveError - 3,
	    KCompressionMethodNotSupported		= KZipArchiveError - 4,
	    KLocalHeaderSignatureInvalid		= KZipArchiveError - 5,
	    KMultiDiskArchivesNotSupported		= KZipArchiveError - 6,
		KMemberNotFound						= KZipArchiveError - 7,

		KZipArchiveMinError					= KZipArchiveError - 8

		};
    
    enum TCompressionMethod 
		{
		
		EStored 		= 0,
		EShrunk 		= 1,
		EReducedCFOne	= 2,
		EReducedCFTwo	= 3,
		EReducedCFThree	= 4,
		EReducedCFFour	= 5,
		EImploded		= 6,
		EReservedOne	= 7,
		EDeflated		= 8,
		EReservedTwo	= 9,
		EPKWAREImploded	= 10
		
		};

protected:

    enum // Constants 
		{
    
        KCentralDirectorySignature  		= 0x06054b50,
    	KCentralDirectoryHeaderSignature	= 0x02014b50,
    	KLocalHeaderSignature   			= 0x04034b50
		};
	
	enum // Lengths 
		{
	
		KCentralDirectoryTrailerFixedLength = 22,
		KLocalHeaderFixedLength             = 30,
		KCentralDirectoryHeaderFixedLength	= 46,
		KSignatureLength                    =  4,
		KMaxTrailerSearchLength             = 65536
		};
	
	enum // Offsets 
		{
	
		KCentralFileHeaderFileNameLengthOffset   = 28,
		KCentralFileHeaderExtraFieldLengthOffset = 30,
		KCentralFileHeaderFileNameOffset         = 46
		};
	
	/**
	Represents the archive's central directory trailer - the central directory contains information
	about files in the arhive

	@publishedAll
	@released
	*/
    struct TCentralDirectoryTrailer 
		{
        TUint32 iSignature;
		TUint16 iDiskNumber;
		TUint16 iStartDiskNumber;
		TUint16 iLocalEntryCount;
		TUint16 iTotalEntryCount;
		TUint32 iSize;
		TUint32 iOffset;
		TUint16 iCommentLength;
		// comment -- variable length
		};

	/**
	Represents the archive's central directory header - the central directory contains information
	about files in the arhive

	@publishedAll
	@released
	*/
	struct TCentralDirectoryHeader 
		{
		TUint32 iSignature;
		TUint16 iMadeBy;
		TUint16 iRequired;
		TUint16 iFlags;
		TUint16 iCompressionMethod;
		TUint16 iLastModifiedFileTime;
		TUint16 iLastModifiedFileDate;
		TUint32 iCRC32;
		TUint32 iCompressedSize;
		TUint32 iUncompressedSize;
		TUint16 iFileNameLength;
		TUint16 iExtraFieldLength;
		TUint16 iFileCommentLength;
		TUint16 iDiskNumberStart;
		TUint16 iInternalFileAttributes;
		TUint32 iExternalFileAttributes;
		TUint32 iLocalHeaderOffset;
		// file name    -- variable length 
		// extra field  -- variable length
		// file comment -- variable length
		};

	/**
	Represents the archive's local header

	@publishedAll
	@released
	*/
	struct TLocalHeader 
		{
		TUint32 iSignature;
		TUint16 iVersionNeeded;
		TUint16 iFlags;
		TUint16 iCompressionMethod;
		TUint16 iLastModifiedFileTime;
		TUint16 iLastModifiedFileDate;
		TUint32 iCRC32;
		TUint32 iCompressedSize;
		TUint32 iUncompressedSize;
		TUint16 iFileNameLength;
		TUint16 iExtraFieldLength;
		// file name    -- variable length 
		// extra field  -- variable length
		};
	};

#endif /* !_ZIP_ARCHIVE_H_ */

