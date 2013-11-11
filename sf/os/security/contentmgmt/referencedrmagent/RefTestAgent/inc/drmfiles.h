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

#ifndef __DRMFILES_H__
#define __DRMFILES_H__

#include <f32file.h>

namespace ReferenceTestAgent
	{
	class CDrmFileContainer;
	class CDrmFileContent;
	
	class CDrmFiles : public CBase
		{
	public:
		static CDrmFiles* NewL();
	
		virtual ~CDrmFiles();
		IMPORT_C CDrmFileContainer& CurrentContainer();
		IMPORT_C TInt OpenContainer(const TDesC& aUniqueId);
		IMPORT_C TInt CloseContainer();
		IMPORT_C CDrmFileContent& FindL(const TDesC& aUniqueId);
		
		CDrmFileContainer& RootContainer();
		IMPORT_C CDrmFileContent& FindL(const TDesC8& aCid);
		IMPORT_C CDrmFileContent& Content(TInt aIndex);
		IMPORT_C TInt ContentCount();
	
		// Sets the file object to be used by the "" Unique Id  (ie. the entire file)
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C void SetEntireFileL(RFile64& aFile, RSemaphore& aFileLock, TInt64 aOffset, TInt64 aSize);
#else
		IMPORT_C void SetEntireFileL(RFile& aFile, RSemaphore& aFileLock, TInt aOffset, TInt aSize);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		
		void UpdateContentL();	
	private:
		CDrmFiles();
		void ConstructL();
	
	private:		
		CDrmFileContent* iEntireFile;
		CDrmFileContainer* iRootContainer;
		CDrmFileContainer* iCurrentContainer;
		RPointerArray <CDrmFileContent> iContentArray;
		};
	}

#endif

