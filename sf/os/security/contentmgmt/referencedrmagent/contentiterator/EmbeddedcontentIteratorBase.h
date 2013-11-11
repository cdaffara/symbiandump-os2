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


#ifndef __CAF_EMBEDDEDCONTENTITERATORBASE_H__
#define __CAF_EMBEDDEDCONTENTITERATORBASE_H__

#include <e32base.h>

#include "StreamablePtrArray.h"
#include <caf/virtualpathptr.h>

namespace ContentAccess 
	{
	class CContent;
	class CEmbeddedObject;


	/** Internal class for recursively searching through a particular content file

	Only ever created by CContentIteratorBase, CEmbeddedContetnIterator or itself

	@internalComponent
	@released
	*/
	NONSHARABLE_CLASS(CEmbeddedContentIteratorBase) : public CBase
		{
	public:
		virtual ~CEmbeddedContentIteratorBase();

		CEmbeddedObject& EmbeddedObject();
		TInt Next();

	private:
		// Only ever created by one of these two or itself
		friend class CEmbeddedContentIterator;
		friend class CContentIteratorBase;

		/** Create a content iterator for a particular path in the file system
		@param aContent Already set up to look at the container we are interested in
		@param aRecursive Whether to explore sub-containers under the given path
		@param aMimeType The mime type to search for. Finds content objects of any mime types if this parameter is omitted
		@leave KErrNotFound If no suitable content objects were found inside the given path
		*/
		static CEmbeddedContentIteratorBase* NewL(CContent& aContent, TBool aRecursive, const TDesC8& aMimeType);

		CEmbeddedContentIteratorBase(CContent& aContent, TBool aRecursive, const TDesC8& aMimeType );
		void ConstructL();

		// Construction parameters
		CContent& iContent;
		TBool iRecursive;
		const TDesC8& iMimeType;

		// Index into the list of embedded content objects
		TInt iContentIndex;
		
		// Index into the list of embedded container objects
		TInt iContainerIndex;
		
		// Objects inside the current container within the specificed file
		RStreamablePtrArray<CEmbeddedObject> iEmbeddedContentObjects;

		// Container Objects inside the current container within the specified file
		RStreamablePtrArray<CEmbeddedObject> iEmbeddedContainerObjects;

		// Used for looking inside sub-containers
		CEmbeddedContentIteratorBase* iSubIterator;
		};


	}

#endif
