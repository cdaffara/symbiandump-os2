/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* reftestagentarchive.h
*
*/


/**
 @file
 @internalAll
 @released
*/

#ifndef __REFTESTAGENTARCHIVE_H__
#define __REFTESTAGENTARCHIVE_H__

#include <f32file.h>

namespace ReferenceTestAgent
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	_LIT8(KRTAArchiveHeader, "$_SYMRTA_$");
	const TUint KRTAArchiveHeaderSize = 10; 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	class CDrmFiles;
	class CDrmFilesParser;
	class CPartOfFile;
	class CVirtualFile;
	class CRefTestAgentArchive : public CBase
		{
	public:
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C static CRefTestAgentArchive* NewL(RFile64& aFile);
#else
		IMPORT_C static CRefTestAgentArchive* NewL(RFile& aFile);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual ~CRefTestAgentArchive();

		// obtain the mime type of the content in the file
		IMPORT_C const TDesC8& DefaultMimeType();
		
		// return the drm file object used for reading
		IMPORT_C CDrmFiles& DrmFilesL();

		IMPORT_C static TInt CreateArchive(const TDesC& aContentXmlFileName, const TDesC& aRightsXmlFileName, TDes& aOutputFileName, TDes8& aOutputMimeType);
		IMPORT_C static void GetDefaultMimeTypeFromHeaderL(const TDesC8& aBuffer, TDes8& aDefaultMimeType);
		
	private:
		CRefTestAgentArchive();
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		void ConstructL(RFile64& aFile);
#else
		void ConstructL(RFile& aFile);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		
		static void DoCreateArchiveL(const TDesC& aContentXmlFileName, const TDesC& aRightsXmlFileName, const TDesC& aOutputFileName);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		static void WriteHeaderL(CVirtualFile& aVirtualFile, const TInt64& aRightsOffset, const TInt64& aContentOffset);
#else
		static void WriteHeaderL(CVirtualFile& aVirtualFile, TInt aRightsOffset, TInt aContentOffset);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		
	private:		
		CDrmFilesParser* iDrmFilesParser;
		CPartOfFile* iPartOfFile;
		CPartOfFile* iContentXmlFile;
		HBufC8* iMimeType;
		RSemaphore iFileLock;
		
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TInt64	iDrmFileOffset;
		TInt64	iDrmRightsOffset;
		TInt64	iContentOffset;
		TInt64	iFileSize;
		TBool	iIs64BitSize;
#else
		TInt	iDrmFileOffset;
		TInt	iDrmRightsOffset;
		TInt	iContentOffset;
		TInt	iFileSize;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		};
	}

#endif

