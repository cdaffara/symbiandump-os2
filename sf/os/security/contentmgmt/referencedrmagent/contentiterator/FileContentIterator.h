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


#ifndef __CAF_FILECONTENTITERATOR_H__
#define __CAF_FILECONTENTITERATOR_H__

#include <e32base.h>

#include "StreamablePtrArray.h"
#include <caf/virtualpathptr.h>
#include "ContentIteratorInterface.h"

namespace ContentAccess 
	{
	class CManager;
	class CFileContentIteratorBase;

	/** Recursively iterates over all files and then into each file to determine the content
	objects inside 

	@internalComponent
	@released
	*/
	NONSHARABLE_CLASS(CFileContentIterator): public MContentIterator
		{
	public:
		/** Create a content iterator for a particular path in the file system
		@param aPath The path to search under
		@param aRecursive Whether to explore sub-directories under the given path
		@param aMimeType The mime type to search for. Finds content objects of any mime types if this parameter is omitted
		@leave KErrNotFound If no suitable content objects were found inside the given path
		*/
		static CFileContentIterator* NewL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType);

		virtual ~CFileContentIterator();

		// MContentIterator
		virtual const TDesC& FileName() const;
		virtual const TDesC& UniqueId() const;
		virtual const TDesC& Name() const;
		virtual const TDesC8& MimeType() const; 
		virtual TInt Next();
		

	private:
		CFileContentIterator();
		void ConstructL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType);

		// Manager for doing GetDir()
		CManager* iManager;

		// Construction Parameters
		HBufC* iPath;
		TBool iRecursive;
		HBufC8* iMimeType;

		CFileContentIteratorBase* iSubIterator;
		};

	}

#endif
