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


#ifndef __CAF_EMBEDDEDCONTENTITERATOR_H__
#define __CAF_EMBEDDEDCONTENTITERATOR_H__

#include <e32base.h>

#include "StreamablePtrArray.h"
#include <caf/virtualpathptr.h>
#include "ContentIteratorInterface.h"

namespace ContentAccess 
	{
	class CContent;
	class CEmbeddedObject;
	class CEmbeddedContentIteratorBase;

	/** Recursively iterate over all content objects within a file

	Can be used to find the all content objects inside a given file. A mimetype
	can be specified to return only those objects with the correct mime type

	@internalComponent
	@released
	*/
	NONSHARABLE_CLASS(CEmbeddedContentIterator): public MContentIterator
		{	
	public:
		/** Create a content iterator for a particular path in the file system
		@param aVirtualPath The file to search inside
		@param aRecursive Whether to explore sub-containers under the given path
		@param aMimeType The mime type to search for. Finds content objects of any mime types if this parameter is omitted
		@leave KErrNotFound If no suitable content objects were found inside the given path
		*/
		static CEmbeddedContentIterator* NewL(const TVirtualPathPtr& aVirtualPath, TBool aRecursive, const TDesC8& aMimeType = KNullDesC8);

		
		virtual ~CEmbeddedContentIterator();

		// MContentIterator
		virtual const TDesC& FileName() const;
		virtual const TDesC& UniqueId() const;
		virtual const TDesC& Name() const;
		virtual const TDesC8& MimeType() const;
		virtual TInt Next();
		
		
	private:
		CEmbeddedContentIterator();
		void ConstructL(const TVirtualPathPtr& aVirtualPath, TBool aRecursive, const TDesC8& aMimeType);

		// Construction parameters (copies)
		CContent* iContent;  // created from virtual path
		TBool iRecursive;
		HBufC8* iMimeType;
		HBufC* iUri;
			
		// Iterator that actually does the work
		CEmbeddedContentIteratorBase* iSubIterator;
		};

	}

#endif
