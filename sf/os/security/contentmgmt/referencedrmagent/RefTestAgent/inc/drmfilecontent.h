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
*
*/


/**
 @file
 @internalAll
 @released
*/

#ifndef __DRMFILECONTENT_H__
#define __DRMFILECONTENT_H__

#include <e32base.h>
#include <f32file.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>

namespace ReferenceTestAgent
	{
	class CVirtualFile;

	class CDrmFileContent : public CBase
		{
	public:
		static CDrmFileContent* NewL(const TDesC8& aCid, const TDesC& aName, const TDesC& aUniqueId, const TDesC& aFileName, ContentAccess::RAttributeSet& aAttributeSet, ContentAccess::RStringAttributeSet& aStringAttributeSet);
		virtual ~CDrmFileContent();
		
		IMPORT_C TInt Read(TDes8& aBuffer, TInt aLength);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C TInt Read(TInt64 aPos, TDes8& aBuffer, TInt aLength);
		IMPORT_C TInt Seek(TSeek aMode, TInt64 &aPos);
		IMPORT_C TInt Size(TInt64& aSize);
#else
		IMPORT_C TInt Read(TInt aPos, TDes8& aBuffer, TInt aLength);
		IMPORT_C TInt Seek(TSeek aMode, TInt &aPos);
		IMPORT_C TInt Size(TInt& aSize);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue);
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		IMPORT_C TInt GetAttributeSet(ContentAccess::RAttributeSet &aAttributeSet); 
		IMPORT_C TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet &aStringAttributeSet);
		IMPORT_C const TDesC8& Cid();
		
		const TDesC& UniqueId();
		const TDesC& Name();
		// Convenience function rather than querying attributes
		IMPORT_C const TDesC8& MimeType();

		// setup parameters for common source file (iVirtualFile)
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C void SetCommonSourceFileL(RFile64& aFile, RSemaphore& aFileLock, TInt64 aOffset, TInt64 aSize);
#else
		IMPORT_C void SetCommonSourceFileL(RFile& aFile, RSemaphore& aFileLock, TInt aOffset, TInt aSize);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	private:
		CDrmFileContent();
		void ConstructL(const TDesC8& aCid, const TDesC& aName, const TDesC& aUniqueId, const TDesC& aFileName, ContentAccess::RAttributeSet& aAttributeSet, ContentAccess::RStringAttributeSet& aStringAttributeSet);
		TInt VirtualFilePresent();
		void SetDefaultAttributeL(TInt aAttribute, TInt aValue, TInt aErrorCode);		
				
	private:
		HBufC8* iCid;
		HBufC* iName;
		HBufC* iUniqueId;
		HBufC* iFileName;
		
		HBufC8* iMimeType;
		
		ContentAccess::RAttributeSet iDefaultAttributeSet;
		ContentAccess::RAttributeSet iAttributeSet;
		ContentAccess::RStringAttributeSet iStringAttributeSet;

		// Used to actually read files
		CVirtualFile* iVirtualFile;
		};	
	}
#endif
