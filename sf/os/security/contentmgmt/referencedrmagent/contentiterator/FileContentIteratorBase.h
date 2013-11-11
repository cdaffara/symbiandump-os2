/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/


#ifndef __FILECONTENTITERATORBASE_H__
#define __FILECONTENTITERATORBASE_H__

#include <e32base.h>

#include "StreamablePtrArray.h"
#include "ContentIteratorInterface.h"
#include <caf/virtualpathptr.h>

class CDir;

namespace ContentAccess 
	{
	class CContent;
	class CManager;
	class CEmbeddedObject;

	/** Internal class used to recursively iterate over all files in a path and then
	iterate into all content objects inside each file

	@internalComponent
	@released
	*/
	NONSHARABLE_CLASS(CFileContentIteratorBase) : public MContentIterator
		{
	public:
		virtual ~CFileContentIteratorBase();
		
		// MContentIterator
		virtual const TDesC& FileName() const;
		virtual const TDesC& UniqueId() const;
		virtual const TDesC& Name() const;
		virtual const TDesC8& MimeType() const;
		virtual TInt Next();
		
	private:
		// Only ever created by CFileContentIterator
		friend class CFileContentIterator;

		/** Create a content iterator for a particular path in the file system
		@param aPath The path to search under
		@param aRecursive Whether to explore sub-directories under the given path
		@param aMimeType The mime type to search for. Finds content objects of any mime types if this parameter is omitted
		@leave KErrNotFound If no suitable content objects were found inside the given path
		*/
		static CFileContentIteratorBase* NewL(CManager &aManager, const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType = KNullDesC8);

		CFileContentIteratorBase(CManager& aManager, const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType);
		void ConstructL();

		// Construction parameters
		CManager& iManager;
		const TDesC& iPath;
		TBool iRecursive;
		const TDesC8& iMimeType;

		MContentIterator* iSubIterator;
		
		// Index into the directory list
		TInt iDirIndex;
		TInt iFileIndex;

		// List of files and directories inside (iPath)
		CDir* iCurrentDirectoryList;
		CDir* iCurrentFileList;
		
		// Used to store the path of the subdirectory we are exploring
		HBufC* iNewPath;
		};
	}

#endif
