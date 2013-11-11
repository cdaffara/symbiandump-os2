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


#ifndef _ZIP_FILE_H_
#define _ZIP_FILE_H_

#include <e32std.h>
#include <s32file.h>
#include <charconv.h>

#include <localtypes.h>
#include <ziparchive.h>
#include <zipfilememberinputstream.h>
#include <zipfilememberiterator.h>
#include <zipfilemember.h>

// Forward Class Declaration(s)

class CFileInputStream;
class CZipFileMemberIterator;
class CZipFileMember;
namespace ContentAccess
	{
	class CData;
	}

// End of Forward Class Declaration(s)


/** 
A CZipFile represents a ZIP archive contained in a single file. 
Multi file zip archives are not supported.

@publishedAll
@released
*/
class CZipFile : public CZipArchive
	{
	friend class RZipFileMemberReaderStream;
	friend class CZipFileMemberIterator;

	public:
		/**	ZipFile error enumeration.
		*/
		enum
			{
			/** Cannot read file directory in the archive file*/
			KZipFileError 		= KZipArchiveMinError - 1,
			
			/** File not found error. It is not used in current implemenation*/
			KZipFileNotFound	= KZipFileError - 1,
			
			/** File IO error.Any error occurs during a archive file is readed. 
			For examples, any error about reading number of disk,
			an offset of signature, or the content of file is non-readable.
			*/
			KZipFileIOError 	= KZipFileError - 2
			};

	public:
		IMPORT_C static CZipFile* NewL(RFs& aFs, RFile& aFile);	
		IMPORT_C static CZipFile* NewL(RFs& aFs, const TDesC& aFileName);
		IMPORT_C CZipFile(RFs& aFs, const TDesC& aFileName);
		IMPORT_C TInt OpenL(void);
		IMPORT_C void Close(void);
		IMPORT_C virtual ~CZipFile();

	protected:
		IMPORT_C void ConstructL(const TDesC& aFileName);
		
		IMPORT_C void ConstructL(RFile& aFile);

	public:
		IMPORT_C TInt Size(TInt& aSize) const;
    
		IMPORT_C CZipFileMember* MemberL(const TDesC& aName);
		IMPORT_C CZipFileMember* CaseInsensitiveMemberL(const TDesC& aName);
		IMPORT_C CZipFileMember* CaseSensitiveOrCaseInsensitiveMemberL(const TDesC& aName);
		IMPORT_C TInt GetInputStreamL(const CZipFileMember* aMember, RZipFileMemberReaderStream*& aStream);

  		IMPORT_C CZipFileMemberIterator* GetMembersL(void);
	
    
protected:

	/** 
	Internal representation of a compressed file in a zipfile
	@publishedAll
	@released
	*/
	struct TMemberPointer
		{
		/** the name of a compressed file*/
		TFileName*  iName;
		
		/** Not used in current implementation*/
		TUint32		iCentralHeaderOffset;
		
		/** The local offset of header in a compressed file*/
		TUint32	iLocalHeaderOffset;
		
		/** CRC with 32 bits length in a compressed file*/
		TUint32 iCRC32;
		
		/** The size of compressed file */	
		TUint32	iCompressedSize;
		
		/** The size of file without compressed*/
		TUint32	iUncompressedSize;
		};
		
protected:
	TInt FindCentralDirectoryTrailer(TUint32& offset);
    TInt ReadCentralDirectoryTrailer(TUint32 offset, TCentralDirectoryTrailer&);
    TInt ReadCentralDirectoryHeaderL(TCentralDirectoryHeader&, TMemberPointer&,
    	CCnvCharacterSetConverter* aConverter, TInt aConverterState);

	TInt ReadLocalHeader(TUint32, TLocalHeader&);
	
	const TMemberPointer* FindMemberPointer(const TDesC&, TBool);
	
    TInt LoadMemberPointersL(void);
    
	RZipFileMemberReaderStream*   MakeInputStreamL(TUint32, TUint32, TUint32, TUint32);
    CZipFileMember* MakeMemberL(TInt);
    CZipFileMember* MakeMemberL(const TMemberPointer&, const TLocalHeader&);
    
    // File I/O Support
    void OpenFileL(const TDesC& aFileName);
  
    TInt Read(TByte*, TUint32);
    TInt Read(TUint16&);
    TInt Read(TUint32&);
    
    TInt Seek(TInt);

	// Misc
	void DeleteMemberPointers(void);

private:
    const TDesC&				iFileName;
    TUint32 iFileLength;
	
	/** The central directory of ZIP File Trailer*/
	TCentralDirectoryTrailer iTrailer;
	
	/** The pointer to a a compressed file in a zipfile*/
	TMemberPointer*	iMemberPointers;
	
	RFs& iFs;
	
	/** The pointer to CData object created from a zip file*/
	ContentAccess::CData* iData;
	};

#endif /* !_ZIP_FILE_H_ */
